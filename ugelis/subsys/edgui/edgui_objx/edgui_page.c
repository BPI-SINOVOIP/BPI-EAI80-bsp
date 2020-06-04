/**
 * @file edgui_page.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../edgui_objx/edgui_page.h"
#if EDGUI_USE_PAGE != 0

#include "../edgui_core/edgui_group.h"
#include "../edgui_draw/edgui_draw.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_core/edgui_refr.h"
#include "../edgui_misc/edgui_anim.h"
#include "../edgui_misc/edgui_math.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_PAGE_SB_MIN_SIZE (EDGUI_DPI / 8)

/*[ms] Scroll anim time on `edgui_page_scroll_up/down/left/rigth`*/
#define EDGUI_PAGE_SCROLL_ANIM_TIME 200

#define EDGUI_PAGE_END_FLASH_SIZE (EDGUI_DPI / 4)
#define EDGUI_PAGE_END_ANIM_TIME 300
#define EDGUI_PAGE_END_ANIM_WAIT_TIME 300

#if EDGUI_USE_ANIMATION == 0
#undef EDGUI_PAGE_DEF_ANIM_TIME
#define EDGUI_PAGE_DEF_ANIM_TIME 0 /*No animation*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void edgui_page_sb_refresh(edgui_obj_t * page);
static bool edgui_page_design(edgui_obj_t * page, const edgui_area_t * mask, edgui_design_mode_t mode);
static bool edgui_scrl_design(edgui_obj_t * scrl, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_page_signal(edgui_obj_t * page, edgui_signal_t sign, void * param);
static edgui_res_t edgui_page_scrollable_signal(edgui_obj_t * scrl, edgui_signal_t sign, void * param);
static void scrl_def_event_cb(edgui_obj_t * scrl, edgui_event_t event);
#if EDGUI_USE_ANIMATION
static void edge_flash_anim(void * page, edgui_anim_value_t v);
static void edge_flash_anim_end(edgui_anim_t * a);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_design_cb_t ancestor_design;
static edgui_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a page objects
 * @param par pointer to an object, it will be the parent of the new page
 * @param copy pointer to a page object, if not NULL then the new object will be copied from it
 * @return pointer to the created page
 */
edgui_obj_t * edgui_page_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("page create started");

    /*Create the ancestor object*/
    edgui_obj_t * new_page = edgui_cont_create(par, copy);
    edgui_mem_assert(new_page);
    if(new_page == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_page);
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_page);

    /*Allocate the object type specific extended data*/
    edgui_page_ext_t * ext = edgui_obj_allocate_ext_attr(new_page, sizeof(edgui_page_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->scrl        = NULL;
    ext->sb.hor_draw = 0;
    ext->sb.ver_draw = 0;
    ext->sb.style    = &edgui_style_pretty;
    ext->sb.mode     = EDGUI_SB_MODE_AUTO;
#if EDGUI_USE_ANIMATION
    ext->edge_flash.enabled   = 0;
    ext->edge_flash.bottom_ip = 0;
    ext->edge_flash.top_ip    = 0;
    ext->edge_flash.left_ip   = 0;
    ext->edge_flash.right_ip  = 0;
    ext->edge_flash.state     = 0;
    ext->edge_flash.style     = &edgui_style_plain_color;
    ext->anim_time            = EDGUI_PAGE_DEF_ANIM_TIME;
#endif
    ext->scroll_prop    = 0;
    ext->scroll_prop_ip = 0;

    /*Init the new page object*/
    if(copy == NULL) {
        ext->scrl = edgui_cont_create(new_page, NULL);
        edgui_obj_set_signal_cb(ext->scrl, edgui_page_scrollable_signal);
        edgui_obj_set_design_cb(ext->scrl, edgui_scrl_design);
        edgui_obj_set_drag(ext->scrl, true);
        edgui_obj_set_drag_throw(ext->scrl, true);
        edgui_obj_set_protect(ext->scrl, EDGUI_PROTECT_PARENT | EDGUI_PROTECT_PRESS_LOST);
        edgui_cont_set_fit4(ext->scrl, EDGUI_FIT_FILL, EDGUI_FIT_FILL, EDGUI_FIT_FILL, EDGUI_FIT_FILL);
        edgui_obj_set_event_cb(ext->scrl, scrl_def_event_cb); /*Propagate some event to the background
                                                              object by default for convenience */

        /* Add the signal function only if 'scrolling' is created
         * because everything has to be ready before any signal is received*/
        edgui_obj_set_signal_cb(new_page, edgui_page_signal);
        edgui_obj_set_design_cb(new_page, edgui_page_design);

        edgui_page_set_sb_mode(new_page, ext->sb.mode);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            if(par == NULL) { /*Different styles if it is screen*/
                edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_BG, th->style.bg);
                edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_SCRL, &edgui_style_transp);
            } else {
                edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_BG, th->style.page.bg);
                edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_SCRL, th->style.page.scrl);
            }
            edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_SB, th->style.page.sb);
        } else {
            edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_BG, &edgui_style_pretty_color);
            edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_SCRL, &edgui_style_pretty);
            edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_SB, &edgui_style_pretty_color);
        }

    } else {
        edgui_page_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->scrl                = edgui_cont_create(new_page, copy_ext->scrl);
        edgui_obj_set_signal_cb(ext->scrl, edgui_page_scrollable_signal);

        edgui_page_set_sb_mode(new_page, copy_ext->sb.mode);

        edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_BG, edgui_page_get_style(copy, EDGUI_PAGE_STYLE_BG));
        edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_SCRL, edgui_page_get_style(copy, EDGUI_PAGE_STYLE_SCRL));
        edgui_page_set_style(new_page, EDGUI_PAGE_STYLE_SB, edgui_page_get_style(copy, EDGUI_PAGE_STYLE_SB));

        /* Add the signal function only if 'scrolling' is created
         * because everything has to be ready before any signal is received*/
        edgui_obj_set_signal_cb(new_page, edgui_page_signal);
        edgui_obj_set_design_cb(new_page, edgui_page_design);

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_page);
    }

    edgui_page_sb_refresh(new_page);

    EDGUI_LOG_INFO("page created");

    return new_page;
}

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void edgui_page_clean(edgui_obj_t * obj)
{
    edgui_obj_t * scrl = edgui_page_get_scrl(obj);
    edgui_obj_clean(scrl);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @param sb_mode the new mode from 'edgui_page_sb.mode_t' enum
 */
void edgui_page_set_sb_mode(edgui_obj_t * page, edgui_sb_mode_t sb_mode)
{
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);
    if(ext->sb.mode == sb_mode) return;

    if(sb_mode == EDGUI_SB_MODE_HIDE)
        ext->sb.mode |= EDGUI_SB_MODE_HIDE; /*Set the hidden flag*/
    else if(sb_mode == EDGUI_SB_MODE_UNHIDE)
        ext->sb.mode &= (~EDGUI_SB_MODE_HIDE); /*Clear the hidden flag*/
    else {
        if(ext->sb.mode & EDGUI_SB_MODE_HIDE) sb_mode |= EDGUI_SB_MODE_HIDE;
        ext->sb.mode = sb_mode;
    }

    ext->sb.hor_draw = 0;
    ext->sb.ver_draw = 0;

    edgui_page_sb_refresh(page);
    edgui_obj_invalidate(page);
}

/**
 * Set the animation time for the page
 * @param page pointer to a page object
 * @param anim_time animation time in milliseconds
 */
void edgui_page_set_anim_time(edgui_obj_t * page, uint16_t anim_time)
{
#if EDGUI_USE_ANIMATION
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);
    ext->anim_time      = anim_time;
#else
    (void)page;      /*Unused*/
    (void)anim_time; /*Unused*/
#endif
}

/**
 * Enable the scroll propagation feature. If enabled then the page will move its parent if there is
 * no more space to scroll.
 * @param page pointer to a Page
 * @param en true or false to enable/disable scroll propagation
 */
void edgui_page_set_scroll_propagation(edgui_obj_t * page, bool en)
{
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);
    ext->scroll_prop    = en ? 1 : 0;
}

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param page pointer to a Page
 * @param en true or false to enable/disable end flash
 */
void edgui_page_set_edge_flash(edgui_obj_t * page, bool en)
{
#if EDGUI_USE_ANIMATION
    edgui_page_ext_t * ext     = edgui_obj_get_ext_attr(page);
    ext->edge_flash.enabled = en ? 1 : 0;
#else
    (void)page;
    (void)en;
#endif
}

/**
 * Set a style of a page
 * @param page pointer to a page object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void edgui_page_set_style(edgui_obj_t * page, edgui_page_style_t type, const edgui_style_t * style)
{
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);

    switch(type) {
        case EDGUI_PAGE_STYLE_BG: edgui_obj_set_style(page, style); break;
        case EDGUI_PAGE_STYLE_SCRL: edgui_obj_set_style(ext->scrl, style); break;
        case EDGUI_PAGE_STYLE_SB:
            ext->sb.style = style;
            edgui_area_set_height(&ext->sb.hor_area, ext->sb.style->body.padding.inner);
            edgui_area_set_width(&ext->sb.ver_area, ext->sb.style->body.padding.inner);
            edgui_page_sb_refresh(page);
            edgui_obj_refresh_ext_draw_pad(page);
            edgui_obj_invalidate(page);
            break;
#if EDGUI_USE_ANIMATION
        case EDGUI_PAGE_STYLE_EDGE_FLASH: ext->edge_flash.style = style; break;
#endif
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the scrollable object of a page
 * @param page pointer to a page object
 * @return pointer to a container which is the scrollable part of the page
 */
edgui_obj_t * edgui_page_get_scrl(const edgui_obj_t * page)
{
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);

    return ext->scrl;
}

/**
 * Get the animation time
 * @param page pointer to a page object
 * @return the animation time in milliseconds
 */
uint16_t edgui_page_get_anim_time(const edgui_obj_t * page)
{
#if EDGUI_USE_ANIMATION
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);
    return ext->anim_time;
#else
    (void)page; /*Unused*/
    return 0;
#endif
}

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @return the mode from 'edgui_page_sb.mode_t' enum
 */
edgui_sb_mode_t edgui_page_get_sb_mode(const edgui_obj_t * page)
{
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);
    return ext->sb.mode;
}

/**
 * Get the scroll propagation property
 * @param page pointer to a Page
 * @return true or false
 */
bool edgui_page_get_scroll_propagation(edgui_obj_t * page)
{
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);
    return ext->scroll_prop == 0 ? false : true;
}

/**
 * Get the edge flash effect property.
 * @param page pointer to a Page
 * return true or false
 */
bool edgui_page_get_edge_flash(edgui_obj_t * page)
{
#if EDGUI_USE_ANIMATION
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);
    return ext->edge_flash.enabled == 0 ? false : true;
#else
    (void)page;
    return false;
#endif
}

/**
 * Get that width which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the width which still fits into the page
 */
edgui_coord_t edgui_page_get_fit_width(edgui_obj_t * page)
{
    const edgui_style_t * bg_style   = edgui_page_get_style(page, EDGUI_PAGE_STYLE_BG);
    const edgui_style_t * scrl_style = edgui_page_get_style(page, EDGUI_PAGE_STYLE_SCRL);

    return edgui_obj_get_width(page) - bg_style->body.padding.left - bg_style->body.padding.right -
           scrl_style->body.padding.left - scrl_style->body.padding.right;
}

/**
 * Get that height which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the height which still fits into the page
 */
edgui_coord_t edgui_page_get_fit_height(edgui_obj_t * page)
{
    const edgui_style_t * bg_style   = edgui_page_get_style(page, EDGUI_PAGE_STYLE_BG);
    const edgui_style_t * scrl_style = edgui_page_get_style(page, EDGUI_PAGE_STYLE_SCRL);

    return edgui_obj_get_height(page) - bg_style->body.padding.top - bg_style->body.padding.bottom -
           scrl_style->body.padding.top - scrl_style->body.padding.bottom;
}

/**
 * Get a style of a page
 * @param page pointer to page object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
const edgui_style_t * edgui_page_get_style(const edgui_obj_t * page, edgui_page_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_page_ext_t * ext      = edgui_obj_get_ext_attr(page);

    switch(type) {
        case EDGUI_PAGE_STYLE_BG: style = edgui_obj_get_style(page); break;
        case EDGUI_PAGE_STYLE_SCRL: style = edgui_obj_get_style(ext->scrl); break;
        case EDGUI_PAGE_STYLE_SB: style = ext->sb.style; break;
#if EDGUI_USE_ANIMATION
        case EDGUI_PAGE_STYLE_EDGE_FLASH: style = ext->edge_flash.style; break;
#endif
        default: style = NULL; break;
    }

    return style;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Find whether the page has been scrolled to a certain edge.
 * @param page Page object
 * @param edge Edge to check
 * @return true if the page is on the specified edge
 */
bool edgui_page_on_edge(edgui_obj_t * page, edgui_page_edge_t edge)
{
    const edgui_style_t * page_style = edgui_obj_get_style(page);
    edgui_obj_t * scrl               = edgui_page_get_scrl(page);
    edgui_area_t page_coords;
    edgui_area_t scrl_coords;

    edgui_obj_get_coords(scrl, &scrl_coords);
    edgui_obj_get_coords(page, &page_coords);

    if((edge & EDGUI_PAGE_EDGE_TOP) && scrl_coords.y1 == page_coords.y1 + page_style->body.padding.top) return true;
    if((edge & EDGUI_PAGE_EDGE_BOTTOM) && scrl_coords.y2 == page_coords.y2 - page_style->body.padding.bottom) return true;
    if((edge & EDGUI_PAGE_EDGE_LEFT) && scrl_coords.x1 == page_coords.x1 + page_style->body.padding.left) return true;
    if((edge & EDGUI_PAGE_EDGE_RIGHT) && scrl_coords.x2 == page_coords.x2 - page_style->body.padding.right) return true;

    return false;
}

/**
 * Glue the object to the page. After it the page can be moved (dragged) with this object too.
 * @param obj pointer to an object on a page
 * @param glue true: enable glue, false: disable glue
 */
void edgui_page_glue_obj(edgui_obj_t * obj, bool glue)
{
    edgui_obj_set_drag_parent(obj, glue);
    edgui_obj_set_drag(obj, glue);
}

/**
 * Focus on an object. It ensures that the object will be visible on the page.
 * @param page pointer to a page object
 * @param obj pointer to an object to focus (must be on the page)
 * @param anim_en EDGUI_ANIM_ON to focus with animation; EDGUI_ANIM_OFF to focus without animation
 */
void edgui_page_focus(edgui_obj_t * page, const edgui_obj_t * obj, edgui_anim_enable_t anim_en)
{
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);

#if EDGUI_USE_ANIMATION
    /* Be sure there is no position changing animation in progress
     * because it can overide the current changes*/
    edgui_anim_del(page, (edgui_anim_exec_xcb_t)edgui_obj_set_x);
    edgui_anim_del(page, (edgui_anim_exec_xcb_t)edgui_obj_set_y);
    edgui_anim_del(ext->scrl, (edgui_anim_exec_xcb_t)edgui_obj_set_x);
    edgui_anim_del(ext->scrl, (edgui_anim_exec_xcb_t)edgui_obj_set_y);
#endif

    const edgui_style_t * style      = edgui_page_get_style(page, EDGUI_PAGE_STYLE_BG);
    const edgui_style_t * style_scrl = edgui_page_get_style(page, EDGUI_PAGE_STYLE_SCRL);

    /*If obj is higher then the page focus where the "error" is smaller*/
    edgui_coord_t obj_y      = obj->coords.y1 - ext->scrl->coords.y1;
    edgui_coord_t obj_h      = edgui_obj_get_height(obj);
    edgui_coord_t scrlable_y = edgui_obj_get_y(ext->scrl);
    edgui_coord_t page_h     = edgui_obj_get_height(page);

    edgui_coord_t top_err = -(scrlable_y + obj_y);
    edgui_coord_t bot_err = scrlable_y + obj_y + obj_h - page_h;

    /*Out of the page on the top*/
    if((obj_h <= page_h && top_err > 0) || (obj_h > page_h && top_err < bot_err)) {
        /*Calculate a new position and let some space above*/
        scrlable_y = -(obj_y - style_scrl->body.padding.top - style->body.padding.top);
        scrlable_y += style_scrl->body.padding.top;
    }
    /*Out of the page on the bottom*/
    else if((obj_h <= page_h && bot_err > 0) || (obj_h > page_h && top_err >= bot_err)) {
        /*Calculate a new position and let some space below*/
        scrlable_y = -(obj_y + style_scrl->body.padding.bottom + style->body.padding.bottom);
        scrlable_y -= style_scrl->body.padding.bottom;
        scrlable_y += page_h - obj_h;
    }

    /*If obj is wider then the page focus where the "error" is smaller*/
    edgui_coord_t obj_x      = obj->coords.x1 - ext->scrl->coords.x1;
    edgui_coord_t obj_w      = edgui_obj_get_width(obj);
    edgui_coord_t scrlable_x = edgui_obj_get_x(ext->scrl);
    edgui_coord_t page_w     = edgui_obj_get_width(page);

    edgui_coord_t left_err  = -(scrlable_x + obj_x);
    edgui_coord_t right_err = scrlable_x + obj_x + obj_w - page_w;

    /*Out of the page on the left*/
    if((obj_w <= page_w && left_err > 0) || (obj_w > page_w && left_err < right_err)) {
        /*Calculate a new position and let some space above*/
        scrlable_x = -(obj_x - style_scrl->body.padding.left - style->body.padding.left);
        scrlable_x += style_scrl->body.padding.left;
    }
    /*Out of the page on the rigth*/
    else if((obj_w <= page_w && right_err > 0) || (obj_w > page_w && left_err >= right_err)) {
        /*Calculate a new position and let some space below*/
        scrlable_x = -(obj_x + style_scrl->body.padding.right + style->body.padding.right);
        scrlable_x -= style_scrl->body.padding.right;
        scrlable_x += page_w - obj_w;
    }

    if(anim_en == EDGUI_ANIM_OFF || edgui_page_get_anim_time(page) == 0) {
        edgui_obj_set_y(ext->scrl, scrlable_y);
        edgui_obj_set_x(ext->scrl, scrlable_x);
    } else {
#if EDGUI_USE_ANIMATION
        edgui_anim_t a;
        a.act_time = 0;
        a.start    = edgui_obj_get_y(ext->scrl);
        a.end      = scrlable_y;
        a.time     = edgui_page_get_anim_time(page);
        a.ready_cb = NULL;
        a.playback = 0;
        a.repeat   = 0;
        a.var      = ext->scrl;
        a.path_cb  = edgui_anim_path_linear;
        a.exec_cb  = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
        edgui_anim_create(&a);

        a.start   = edgui_obj_get_x(ext->scrl);
        a.end     = scrlable_x;
        a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_x;
        edgui_anim_create(&a);
#endif
    }
}

/**
 * Scroll the page horizontally
 * @param page pointer to a page object
 * @param dist the distance to scroll (< 0: scroll right; > 0 scroll left)
 */
void edgui_page_scroll_hor(edgui_obj_t * page, edgui_coord_t dist)
{
    edgui_obj_t * scrl = edgui_page_get_scrl(page);

#if EDGUI_USE_ANIMATION
    edgui_anim_t a;
    a.var            = scrl;
    a.start          = edgui_obj_get_x(scrl);
    a.end            = a.start + dist;
    a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_obj_set_x;
    a.path_cb        = edgui_anim_path_linear;
    a.ready_cb       = NULL;
    a.act_time       = 0;
    a.time           = EDGUI_PAGE_SCROLL_ANIM_TIME;
    a.playback       = 0;
    a.playback_pause = 0;
    a.repeat         = 0;
    a.repeat_pause   = 0;
    edgui_anim_create(&a);
#else
    edgui_obj_set_x(scrl, edgui_obj_get_x(scrl) + dist);
#endif
}

/**
 * Scroll the page vertically
 * @param page pointer to a page object
 * @param dist the distance to scroll (< 0: scroll down; > 0 scroll up)
 */
void edgui_page_scroll_ver(edgui_obj_t * page, edgui_coord_t dist)
{
    edgui_obj_t * scrl = edgui_page_get_scrl(page);

#if EDGUI_USE_ANIMATION
    edgui_anim_t a;
    a.var            = scrl;
    a.start          = edgui_obj_get_y(scrl);
    a.end            = a.start + dist;
    a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
    a.path_cb        = edgui_anim_path_linear;
    a.ready_cb       = NULL;
    a.act_time       = 0;
    a.time           = EDGUI_PAGE_SCROLL_ANIM_TIME;
    a.playback       = 0;
    a.playback_pause = 0;
    a.repeat         = 0;
    a.repeat_pause   = 0;
    edgui_anim_create(&a);
#else
    edgui_obj_set_y(scrl, edgui_obj_get_y(scrl) + dist);
#endif
}

/**
 * Not intended to use directly by the user but by other object types internally.
 * Start an edge flash animation. Exactly one `ext->edge_flash.xxx_ip` should be set
 * @param page
 */
void edgui_page_start_edge_flash(edgui_obj_t * page)
{
#if EDGUI_USE_ANIMATION
    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);
    if(ext->edge_flash.enabled) {
        edgui_anim_t a;
        a.var            = page;
        a.start          = 0;
        a.end            = EDGUI_PAGE_END_FLASH_SIZE;
        a.exec_cb        = (edgui_anim_exec_xcb_t)edge_flash_anim;
        a.path_cb        = edgui_anim_path_linear;
        a.ready_cb       = edge_flash_anim_end;
        a.act_time       = 0;
        a.time           = EDGUI_PAGE_END_ANIM_TIME;
        a.playback       = 1;
        a.playback_pause = EDGUI_PAGE_END_ANIM_WAIT_TIME;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        edgui_anim_create(&a);
    }
#else
    (void)page; /*Unused*/
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the pages
 * @param page pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_page_design(edgui_obj_t * page, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return ancestor_design(page, mask, mode);
    }
    /*Cache page bg style for temporary modification*/
    const edgui_style_t * style = edgui_page_get_style(page, EDGUI_PAGE_STYLE_BG);
    edgui_style_t style_tmp;
    edgui_style_copy(&style_tmp, style);

    if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        /*Draw without border*/
        style_tmp.body.border.width = 0;
        edgui_draw_rect(&page->coords, mask, &style_tmp, edgui_obj_get_opa_scale(page));

    } else if(mode == EDGUI_DESIGN_DRAW_POST) {
        /*Draw only a border*/
        style_tmp.body.shadow.width = 0;
        style_tmp.body.opa          = EDGUI_OPA_TRANSP;
        edgui_draw_rect(&page->coords, mask, &style_tmp, edgui_obj_get_opa_scale(page));

        edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);

        /*Draw the scrollbars*/
        edgui_area_t sb_area;
        if(ext->sb.hor_draw && (ext->sb.mode & EDGUI_SB_MODE_HIDE) == 0) {
            /*Convert the relative coordinates to absolute*/
            edgui_area_copy(&sb_area, &ext->sb.hor_area);
            sb_area.x1 += page->coords.x1;
            sb_area.y1 += page->coords.y1;
            sb_area.x2 += page->coords.x1;
            sb_area.y2 += page->coords.y1;
            edgui_draw_rect(&sb_area, mask, ext->sb.style, edgui_obj_get_opa_scale(page));
        }

        if(ext->sb.ver_draw && (ext->sb.mode & EDGUI_SB_MODE_HIDE) == 0) {
            /*Convert the relative coordinates to absolute*/
            edgui_area_copy(&sb_area, &ext->sb.ver_area);
            sb_area.x1 += page->coords.x1;
            sb_area.y1 += page->coords.y1;
            sb_area.x2 += page->coords.x1;
            sb_area.y2 += page->coords.y1;
            edgui_draw_rect(&sb_area, mask, ext->sb.style, edgui_obj_get_opa_scale(page));
        }

#if EDGUI_USE_ANIMATION
        {
            edgui_coord_t page_w = edgui_obj_get_width(page);
            edgui_coord_t page_h = edgui_obj_get_height(page);

            edgui_area_t flash_area;

            if(ext->edge_flash.top_ip) {
                flash_area.x1 = page->coords.x1 - page_w;
                flash_area.x2 = page->coords.x2 + page_w;
                flash_area.y1 = page->coords.y1 - 3 * page_w + ext->edge_flash.state;
                flash_area.y2 = page->coords.y1 + ext->edge_flash.state;
            } else if(ext->edge_flash.bottom_ip) {
                flash_area.x1 = page->coords.x1 - page_w;
                flash_area.x2 = page->coords.x2 + page_w;
                flash_area.y1 = page->coords.y2 - ext->edge_flash.state;
                flash_area.y2 = page->coords.y2 + 3 * page_w - ext->edge_flash.state;
            } else if(ext->edge_flash.right_ip) {
                flash_area.x1 = page->coords.x2 - ext->edge_flash.state;
                flash_area.x2 = page->coords.x2 + 3 * page_h - ext->edge_flash.state;
                flash_area.y1 = page->coords.y1 - page_h;
                flash_area.y2 = page->coords.y2 + page_h;
            } else if(ext->edge_flash.left_ip) {
                flash_area.x1 = page->coords.x1 - 3 * page_h + ext->edge_flash.state;
                flash_area.x2 = page->coords.x1 + ext->edge_flash.state;
                flash_area.y1 = page->coords.y1 - page_h;
                flash_area.y2 = page->coords.y2 + page_h;
            }

            if(ext->edge_flash.left_ip || ext->edge_flash.right_ip || ext->edge_flash.top_ip ||
               ext->edge_flash.bottom_ip) {
                edgui_style_t flash_style;
                edgui_style_copy(&flash_style, ext->edge_flash.style);
                flash_style.body.radius = EDGUI_RADIUS_CIRCLE;
                uint32_t opa            = (flash_style.body.opa * ext->edge_flash.state) / EDGUI_PAGE_END_FLASH_SIZE;
                flash_style.body.opa    = opa;
                edgui_draw_rect(&flash_area, mask, &flash_style, edgui_obj_get_opa_scale(page));
            }
        }
#endif
    }

    return true;
}

/**
 * Handle the drawing related tasks of the scrollable object
 * @param scrl pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_scrl_design(edgui_obj_t * scrl, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return ancestor_design(scrl, mask, mode);
    } else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
#if EDGUI_USE_GROUP
        /* If the page is focused in a group and
         * the background object is not visible (transparent)
         * then "activate" the style of the scrollable*/
        const edgui_style_t * style_scrl_ori = edgui_obj_get_style(scrl);
        edgui_obj_t * page                   = edgui_obj_get_parent(scrl);
        const edgui_style_t * style_page     = edgui_obj_get_style(page);
        edgui_group_t * g                    = edgui_obj_get_group(page);
        if((style_page->body.opa == EDGUI_OPA_TRANSP) &&
           style_page->body.border.width == 0) { /*Is the background visible?*/
            if(edgui_group_get_focused(g) == page) {
                edgui_style_t * style_mod;
                style_mod = edgui_group_mod_style(g, style_scrl_ori);
                /*If still not visible modify the style a littel bit*/
                if((style_mod->body.opa == EDGUI_OPA_TRANSP) && style_mod->body.border.width == 0) {
                    style_mod->body.opa          = EDGUI_OPA_50;
                    style_mod->body.border.width = 1;
                    style_mod                    = edgui_group_mod_style(g, style_mod);
                }

                scrl->style_p = style_mod; /*Temporally change the style to the activated */
            }
        }
#endif
        ancestor_design(scrl, mask, mode);

#if EDGUI_USE_GROUP
        scrl->style_p = style_scrl_ori; /*Revert the style*/
#endif
    } else if(mode == EDGUI_DESIGN_DRAW_POST) {
        ancestor_design(scrl, mask, mode);
    }

    return true;
}

/**
 * Signal function of the page
 * @param page pointer to a page object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_page_signal(edgui_obj_t * page, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(page, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_page_ext_t * ext = edgui_obj_get_ext_attr(page);
    edgui_obj_t * child;
    if(sign == EDGUI_SIGNAL_CHILD_CHG) { /*Automatically move children to the scrollable object*/
        const edgui_style_t * style = edgui_page_get_style(page, EDGUI_PAGE_STYLE_SCRL);
        edgui_fit_t fit_left        = edgui_page_get_scrl_fit_left(page);
        edgui_fit_t fit_top         = edgui_page_get_scrl_fit_top(page);
        child                    = edgui_obj_get_child(page, NULL);
        while(child != NULL) {
            if(edgui_obj_is_protected(child, EDGUI_PROTECT_PARENT) == false) {
                edgui_obj_t * tmp = child;
                child          = edgui_obj_get_child(page, child); /*Get the next child before move this*/

                /* Reposition the child to take padding into account (Only if it's on (0;0) now)
                 * It's required to keep new the object on the same coordinate if FIT is enabled.*/
                if((tmp->coords.x1 == page->coords.x1) && (fit_left == EDGUI_FIT_TIGHT || fit_left == EDGUI_FIT_FILL)) {
                    tmp->coords.x1 += style->body.padding.left;
                    tmp->coords.x2 += style->body.padding.left;
                }
                if((tmp->coords.y1 == page->coords.y1) && (fit_top == EDGUI_FIT_TIGHT || fit_top == EDGUI_FIT_FILL)) {
                    tmp->coords.y1 += style->body.padding.top;
                    tmp->coords.y2 += style->body.padding.top;
                }
                edgui_obj_set_parent(tmp, ext->scrl);
            } else {
                child = edgui_obj_get_child(page, child);
            }
        }
    } else if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        ext->scrl->signal_cb(ext->scrl, EDGUI_SIGNAL_CORD_CHG, &ext->scrl->coords);

        /*The scrollbars are important only if they are visible now*/
        if(ext->sb.hor_draw || ext->sb.ver_draw) edgui_page_sb_refresh(page);

        /*Refresh the ext. size because the scrollbars might be positioned out of the page*/
        edgui_obj_refresh_ext_draw_pad(page);
    } else if(sign == EDGUI_SIGNAL_CORD_CHG) {
        /*Refresh the scrollbar and notify the scrl if the size is changed*/
        if(ext->scrl != NULL && (edgui_obj_get_width(page) != edgui_area_get_width(param) ||
                                 edgui_obj_get_height(page) != edgui_area_get_height(param))) {
            /*If no hor_fit enabled set the scrollable's width to the page's width*/
            ext->scrl->signal_cb(ext->scrl, EDGUI_SIGNAL_CORD_CHG, &ext->scrl->coords);

            /*The scrollbars are important only if they are visible now*/
            if(ext->sb.hor_draw || ext->sb.ver_draw) edgui_page_sb_refresh(page);
        }
    } else if(sign == EDGUI_SIGNAL_REFR_EXT_DRAW_PAD) {
        /*Ensure ext. size for the scrollbars if they are out of the page*/
        if(page->ext_draw_pad < (-ext->sb.style->body.padding.right))
            page->ext_draw_pad = -ext->sb.style->body.padding.right;
        if(page->ext_draw_pad < (-ext->sb.style->body.padding.bottom))
            page->ext_draw_pad = -ext->sb.style->body.padding.bottom;
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        uint32_t c = *((uint32_t *)param);

        if(c == EDGUI_KEY_DOWN) {
            edgui_page_scroll_ver(page, -edgui_obj_get_height(page) / 4);
        } else if(c == EDGUI_KEY_UP) {
            edgui_page_scroll_ver(page, edgui_obj_get_height(page) / 4);
        } else if(c == EDGUI_KEY_RIGHT) {
            /*If the page can't be scrolled horizontally because it's not wide enough then scroll it
             * vertically*/
            if(edgui_page_get_scrl_width(page) <= edgui_obj_get_width(page))
                edgui_page_scroll_ver(page, -edgui_obj_get_height(page) / 4);
            else
                edgui_page_scroll_hor(page, -edgui_obj_get_width(page) / 4);
        } else if(c == EDGUI_KEY_LEFT) {
            /*If the page can't be scrolled horizontally because it's not wide enough then scroll it
             * vertically*/
            if(edgui_page_get_scrl_width(page) <= edgui_obj_get_width(page))
                edgui_page_scroll_ver(page, edgui_obj_get_height(page) / 4);
            else
                edgui_page_scroll_hor(page, edgui_obj_get_width(page) / 4);
        }
    } else if(sign == EDGUI_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = true;
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_page";
    }

    return res;
}

/**
 * Signal function of the scrollable part of a page
 * @param scrl pointer to the scrollable object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_page_scrollable_signal(edgui_obj_t * scrl, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(scrl, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_obj_t * page               = edgui_obj_get_parent(scrl);
    const edgui_style_t * page_style = edgui_obj_get_style(page);
    edgui_page_ext_t * page_ext      = edgui_obj_get_ext_attr(page);

    if(sign == EDGUI_SIGNAL_CORD_CHG) {
        /*Limit the position of the scrollable object to be always visible
         * (Do not let its edge inner then its parent respective edge)*/
        edgui_coord_t new_x = edgui_obj_get_x(scrl);
        edgui_coord_t new_y = edgui_obj_get_y(scrl);
        bool refr_x      = false;
        bool refr_y      = false;
        edgui_area_t page_coords;
        edgui_area_t scrl_coords;
        edgui_obj_get_coords(scrl, &scrl_coords);
        edgui_obj_get_coords(page, &page_coords);

        edgui_area_t * ori_coords = (edgui_area_t *)param;
        edgui_coord_t diff_x      = scrl->coords.x1 - ori_coords->x1;
        edgui_coord_t diff_y      = scrl->coords.y1 - ori_coords->y1;
        edgui_coord_t hpad        = page_style->body.padding.left + page_style->body.padding.right;
        edgui_coord_t vpad        = page_style->body.padding.top + page_style->body.padding.bottom;
        edgui_obj_t * page_parent = edgui_obj_get_parent(page);

        edgui_indev_t * indev = edgui_indev_get_act();
        edgui_point_t drag_vect;
        edgui_indev_get_vect(indev, &drag_vect);

        /* Start the scroll propagation if there is drag vector on the indev, but the drag is not
         * started yet and the scrollable is in a corner. It will enable the scroll propagation only
         * when a new scroll begins and not when the scrollable is already being scrolled.*/
        if(page_ext->scroll_prop && page_ext->scroll_prop_ip == 0 && edgui_indev_is_dragging(indev) == false) {
            if(((drag_vect.y > 0 && scrl_coords.y1 == page_coords.y1 + page_style->body.padding.top) ||
                (drag_vect.y < 0 && scrl_coords.y2 == page_coords.y2 - page_style->body.padding.bottom)) &&
               ((drag_vect.x > 0 && scrl_coords.x1 == page_coords.x1 + page_style->body.padding.left) ||
                (drag_vect.x < 0 && scrl_coords.x2 == page_coords.x2 - page_style->body.padding.right))) {

                if(edgui_obj_get_parent(page_parent) != NULL) { /*Do not propagate the scroll to a screen*/
                    page_ext->scroll_prop_ip = 1;
                }
            }
        }

        /*scrollable width smaller then page width? -> align to left*/
        if(edgui_area_get_width(&scrl_coords) + hpad <= edgui_area_get_width(&page_coords)) {
            if(scrl_coords.x1 != page_coords.x1 + page_style->body.padding.left) {
                new_x  = page_style->body.padding.left;
                refr_x = true;
            }
        } else {
            /*If the scroll propagation is in progress revert the original coordinates (don't let
             * the page scroll)*/
            if(page_ext->scroll_prop_ip) {
                if(drag_vect.x == diff_x) { /*`scrl` is bouncing: drag pos. it somewhere and here it
                                               is reverted. Handle only the pos. because of drag*/
                    new_x  = ori_coords->x1 - page_coords.x1;
                    refr_x = true;
                }
            }
            /*The edges of the scrollable can not be in the page (minus hpad) */
            else if(scrl_coords.x2 < page_coords.x2 - page_style->body.padding.right) {
                new_x = edgui_area_get_width(&page_coords) - edgui_area_get_width(&scrl_coords) -
                        page_style->body.padding.right; /* Right align */
                refr_x = true;
#if EDGUI_USE_ANIMATION
                if(page_ext->edge_flash.enabled && page_ext->edge_flash.left_ip == 0 &&
                   page_ext->edge_flash.right_ip == 0 && page_ext->edge_flash.top_ip == 0 &&
                   page_ext->edge_flash.bottom_ip == 0) {
                    edgui_page_start_edge_flash(page);
                    page_ext->edge_flash.right_ip = 1;
                }
#endif
            } else if(scrl_coords.x1 > page_coords.x1 + page_style->body.padding.left) {
                new_x  = page_style->body.padding.left; /*Left align*/
                refr_x = true;
#if EDGUI_USE_ANIMATION
                if(page_ext->edge_flash.enabled && page_ext->edge_flash.left_ip == 0 &&
                   page_ext->edge_flash.right_ip == 0 && page_ext->edge_flash.top_ip == 0 &&
                   page_ext->edge_flash.bottom_ip == 0) {
                    edgui_page_start_edge_flash(page);
                    page_ext->edge_flash.left_ip = 1;
                }
#endif
            }
        }

        /*scrollable height smaller then page height? -> align to top*/
        if(edgui_area_get_height(&scrl_coords) + vpad <= edgui_area_get_height(&page_coords)) {
            if(scrl_coords.y1 != page_coords.y1 + page_style->body.padding.top) {
                new_y  = page_style->body.padding.top;
                refr_y = true;
            }
        } else {
            /*If the scroll propagation is in progress revert the original coordinates (don't let
             * the page scroll)*/
            if(page_ext->scroll_prop_ip) {
                if(drag_vect.y == diff_y) { /*`scrl` is bouncing: drag pos. it somewhere and here it
                                               is reverted. Handle only the pos. because of drag*/
                    new_y  = ori_coords->y1 - page_coords.y1;
                    refr_y = true;
                }
            }
            /*The edges of the scrollable can not be in the page (minus vpad) */
            else if(scrl_coords.y2 < page_coords.y2 - page_style->body.padding.bottom) {
                new_y = edgui_area_get_height(&page_coords) - edgui_area_get_height(&scrl_coords) -
                        page_style->body.padding.bottom; /* Bottom align */
                refr_y = true;
#if EDGUI_USE_ANIMATION
                if(page_ext->edge_flash.enabled && page_ext->edge_flash.left_ip == 0 &&
                   page_ext->edge_flash.right_ip == 0 && page_ext->edge_flash.top_ip == 0 &&
                   page_ext->edge_flash.bottom_ip == 0) {
                    edgui_page_start_edge_flash(page);
                    page_ext->edge_flash.bottom_ip = 1;
                }
#endif
            } else if(scrl_coords.y1 > page_coords.y1 + page_style->body.padding.top) {
                new_y  = page_style->body.padding.top; /*Top align*/
                refr_y = true;
#if EDGUI_USE_ANIMATION
                if(page_ext->edge_flash.enabled && page_ext->edge_flash.left_ip == 0 &&
                   page_ext->edge_flash.right_ip == 0 && page_ext->edge_flash.top_ip == 0 &&
                   page_ext->edge_flash.bottom_ip == 0) {
                    edgui_page_start_edge_flash(page);
                    page_ext->edge_flash.top_ip = 1;
                }
#endif
            }
        }

        if(refr_x || refr_y) {
            edgui_obj_set_pos(scrl, new_x, new_y);

            if(page_ext->scroll_prop_ip) {
                if(refr_y) edgui_obj_set_y(page_parent, edgui_obj_get_y(page_parent) + diff_y);
                if(refr_x) edgui_obj_set_x(page_parent, edgui_obj_get_x(page_parent) + diff_x);
            }
        }

        edgui_page_sb_refresh(page);
    } else if(sign == EDGUI_SIGNAL_DRAG_END) {

        /*Scroll propagation is finished on drag end*/
        page_ext->scroll_prop_ip = 0;

        /*Hide scrollbars if required*/
        if(page_ext->sb.mode == EDGUI_SB_MODE_DRAG) {
            edgui_disp_t * disp = edgui_obj_get_disp(page);
            edgui_area_t sb_area_tmp;
            if(page_ext->sb.hor_draw) {
                edgui_area_copy(&sb_area_tmp, &page_ext->sb.hor_area);
                sb_area_tmp.x1 += page->coords.x1;
                sb_area_tmp.y1 += page->coords.y1;
                sb_area_tmp.x2 += page->coords.x1;
                sb_area_tmp.y2 += page->coords.y1;
                edgui_inv_area(disp, &sb_area_tmp);
                page_ext->sb.hor_draw = 0;
            }
            if(page_ext->sb.ver_draw) {
                edgui_area_copy(&sb_area_tmp, &page_ext->sb.ver_area);
                sb_area_tmp.x1 += page->coords.x1;
                sb_area_tmp.y1 += page->coords.y1;
                sb_area_tmp.x2 += page->coords.x1;
                sb_area_tmp.y2 += page->coords.y1;
                edgui_inv_area(disp, &sb_area_tmp);
                page_ext->sb.ver_draw = 0;
            }
        }
    }

    return res;
}

/**
 * Propagate the input device related event of the scrollable to the parent page background
 * It is used by default if the scrollable's event is not specified
 * @param scrl pointer to the page's scrollable object
 * @param event type of the event
 * @param data data of the event
 */
static void scrl_def_event_cb(edgui_obj_t * scrl, edgui_event_t event)
{
    edgui_obj_t * page = edgui_obj_get_parent(scrl);

    /*clang-format off*/
    if(event == EDGUI_EVENT_PRESSED || event == EDGUI_EVENT_PRESSING || event == EDGUI_EVENT_PRESS_LOST ||
       event == EDGUI_EVENT_RELEASED || event == EDGUI_EVENT_SHORT_CLICKED || event == EDGUI_EVENT_CLICKED ||
       event == EDGUI_EVENT_LONG_PRESSED || event == EDGUI_EVENT_LONG_PRESSED_REPEAT ||
       event == EDGUI_EVENT_DRAG_BEGIN || event == EDGUI_EVENT_DRAG_END || event == EDGUI_EVENT_DRAG_THROW_BEGIN) {
        edgui_event_send(page, event, edgui_event_get_data());
    }
    /*clang-format on*/
}

/**
 * Refresh the position and size of the scroll bars.
 * @param page pointer to a page object
 */
static void edgui_page_sb_refresh(edgui_obj_t * page)
{
    edgui_page_ext_t * ext      = edgui_obj_get_ext_attr(page);
    const edgui_style_t * style = edgui_obj_get_style(page);
    edgui_obj_t * scrl          = ext->scrl;
    edgui_coord_t size_tmp;
    edgui_coord_t scrl_w = edgui_obj_get_width(scrl);
    edgui_coord_t scrl_h = edgui_obj_get_height(scrl);
    edgui_coord_t obj_w  = edgui_obj_get_width(page);
    edgui_coord_t obj_h  = edgui_obj_get_height(page);

    /*Always let 'scrollbar width' padding above, under, left and right to the scrollbars
     * else:
     * - horizontal and vertical scrollbars can overlap on the corners
     * - if the page has radius the scrollbar can be out of the radius  */
    edgui_coord_t sb_hor_pad = EDGUI_MATH_MAX(ext->sb.style->body.padding.inner, style->body.padding.right);
    edgui_coord_t sb_ver_pad = EDGUI_MATH_MAX(ext->sb.style->body.padding.inner, style->body.padding.bottom);

    if(ext->sb.mode == EDGUI_SB_MODE_OFF) return;

    if(ext->sb.mode == EDGUI_SB_MODE_ON) {
        ext->sb.hor_draw = 1;
        ext->sb.ver_draw = 1;
    }

    /*Invalidate the current (old) scrollbar areas*/
    edgui_disp_t * disp = edgui_obj_get_disp(page);
    edgui_area_t sb_area_tmp;
    if(ext->sb.hor_draw != 0) {
        edgui_area_copy(&sb_area_tmp, &ext->sb.hor_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        edgui_inv_area(disp, &sb_area_tmp);
    }
    if(ext->sb.ver_draw != 0) {
        edgui_area_copy(&sb_area_tmp, &ext->sb.ver_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        edgui_inv_area(disp, &sb_area_tmp);
    }

    if(ext->sb.mode == EDGUI_SB_MODE_DRAG && edgui_indev_is_dragging(edgui_indev_get_act()) == false) {
        ext->sb.hor_draw = 0;
        ext->sb.ver_draw = 0;
        return;
    }

    /*Full sized horizontal scrollbar*/
    if(scrl_w <= obj_w - style->body.padding.left - style->body.padding.right) {
        edgui_area_set_width(&ext->sb.hor_area, obj_w - 2 * sb_hor_pad);
        edgui_area_set_pos(&ext->sb.hor_area, sb_hor_pad,
                        obj_h - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.bottom);
        if(ext->sb.mode == EDGUI_SB_MODE_AUTO || ext->sb.mode == EDGUI_SB_MODE_DRAG) ext->sb.hor_draw = 0;
    }
    /*Smaller horizontal scrollbar*/
    else {
        size_tmp =
            (obj_w * (obj_w - (2 * sb_hor_pad))) / (scrl_w + style->body.padding.left + style->body.padding.right);
        if(size_tmp < EDGUI_PAGE_SB_MIN_SIZE) size_tmp = EDGUI_PAGE_SB_MIN_SIZE;
        edgui_area_set_width(&ext->sb.hor_area, size_tmp);

        edgui_area_set_pos(&ext->sb.hor_area,
                        sb_hor_pad +
                            (-(edgui_obj_get_x(scrl) - style->body.padding.left) * (obj_w - size_tmp - 2 * sb_hor_pad)) /
                                (scrl_w + style->body.padding.left + style->body.padding.right - obj_w),
                        obj_h - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.bottom);

        if(ext->sb.mode == EDGUI_SB_MODE_AUTO || ext->sb.mode == EDGUI_SB_MODE_DRAG) ext->sb.hor_draw = 1;
    }

    /*Full sized vertical scroll bar*/
    if(scrl_h <= obj_h - style->body.padding.top - style->body.padding.bottom) {
        edgui_area_set_height(&ext->sb.ver_area, obj_h - 2 * sb_ver_pad);
        edgui_area_set_pos(&ext->sb.ver_area,
                        obj_w - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.right, sb_ver_pad);
        if(ext->sb.mode == EDGUI_SB_MODE_AUTO || ext->sb.mode == EDGUI_SB_MODE_DRAG) ext->sb.ver_draw = 0;
    }
    /*Smaller vertical scroll bar*/
    else {
        size_tmp =
            (obj_h * (obj_h - (2 * sb_ver_pad))) / (scrl_h + style->body.padding.top + style->body.padding.bottom);
        if(size_tmp < EDGUI_PAGE_SB_MIN_SIZE) size_tmp = EDGUI_PAGE_SB_MIN_SIZE;
        edgui_area_set_height(&ext->sb.ver_area, size_tmp);

        edgui_area_set_pos(&ext->sb.ver_area,
                        obj_w - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.right,
                        sb_ver_pad + (-(edgui_obj_get_y(scrl) - ext->sb.style->body.padding.bottom) *
                                      (obj_h - size_tmp - 2 * sb_ver_pad)) /
                                         (scrl_h + style->body.padding.top + style->body.padding.bottom - obj_h));

        if(ext->sb.mode == EDGUI_SB_MODE_AUTO || ext->sb.mode == EDGUI_SB_MODE_DRAG) ext->sb.ver_draw = 1;
    }

    /*Invalidate the new scrollbar areas*/
    if(ext->sb.hor_draw != 0) {
        edgui_area_copy(&sb_area_tmp, &ext->sb.hor_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        edgui_inv_area(disp, &sb_area_tmp);
    }
    if(ext->sb.ver_draw != 0) {
        edgui_area_copy(&sb_area_tmp, &ext->sb.ver_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        edgui_inv_area(disp, &sb_area_tmp);
    }
}

#if EDGUI_USE_ANIMATION
static void edge_flash_anim(void * page, edgui_anim_value_t v)
{
    edgui_page_ext_t * ext   = edgui_obj_get_ext_attr(page);
    ext->edge_flash.state = v;
    edgui_obj_invalidate(page);
}

static void edge_flash_anim_end(edgui_anim_t * a)
{
    edgui_page_ext_t * ext       = edgui_obj_get_ext_attr(a->var);
    ext->edge_flash.top_ip    = 0;
    ext->edge_flash.bottom_ip = 0;
    ext->edge_flash.left_ip   = 0;
    ext->edge_flash.right_ip  = 0;
    edgui_obj_invalidate(a->var);
}
#endif

#endif
