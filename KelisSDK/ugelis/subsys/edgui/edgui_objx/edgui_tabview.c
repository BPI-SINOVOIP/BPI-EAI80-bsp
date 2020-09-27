/**
 * @file edgui_tab.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_tabview.h"
#if EDGUI_USE_TABVIEW != 0

#include "edgui_btnm.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_anim.h"
#include "../edgui_core/edgui_disp.h"

/*********************
 *      DEFINES
 *********************/
#if EDGUI_USE_ANIMATION
#ifndef EDGUI_TABVIEW_DEF_ANIM_TIME
#define EDGUI_TABVIEW_DEF_ANIM_TIME 300 /*Animation time of focusing to the a list element [ms] (0: no animation)  */
#endif
#else
#undef EDGUI_TABVIEW_DEF_ANIM_TIME
#define EDGUI_TABVIEW_DEF_ANIM_TIME 0 /*No animations*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_tabview_signal(edgui_obj_t * tabview, edgui_signal_t sign, void * param);
static edgui_res_t tabpage_signal(edgui_obj_t * tab_page, edgui_signal_t sign, void * param);
static edgui_res_t tabpage_scrl_signal(edgui_obj_t * tab_scrl, edgui_signal_t sign, void * param);

static void tabpage_pressed_handler(edgui_obj_t * tabview, edgui_obj_t * tabpage);
static void tabpage_pressing_handler(edgui_obj_t * tabview, edgui_obj_t * tabpage);
static void tabpage_press_lost_handler(edgui_obj_t * tabview, edgui_obj_t * tabpage);
static void tab_btnm_event_cb(edgui_obj_t * tab_btnm, edgui_event_t event);
static void tabview_realign(edgui_obj_t * tabview);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;
static edgui_signal_cb_t page_signal;
static edgui_signal_cb_t page_scrl_signal;
static const char * tab_def[] = {""};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a Tab view object
 * @param par pointer to an object, it will be the parent of the new tab
 * @param copy pointer to a tab object, if not NULL then the new object will be copied from it
 * @return pointer to the created tab
 */
edgui_obj_t * edgui_tabview_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("tab view create started");

    /*Create the ancestor of tab*/
    edgui_obj_t * new_tabview = edgui_obj_create(par, copy);
    edgui_mem_assert(new_tabview);
    if(new_tabview == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_tabview);

    /*Allocate the tab type specific extended data*/
    edgui_tabview_ext_t * ext = edgui_obj_allocate_ext_attr(new_tabview, sizeof(edgui_tabview_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */
    ext->drag_hor     = 0;
    ext->draging      = 0;
    ext->scroll_ver   = 0;
    ext->slide_enable = 1;
    ext->tab_cur      = 0;
    ext->point_last.x = 0;
    ext->point_last.y = 0;
    ext->content      = NULL;
    ext->indic        = NULL;
    ext->btns         = NULL;
    ext->btns_pos     = EDGUI_TABVIEW_BTNS_POS_TOP;
#if EDGUI_USE_ANIMATION
    ext->anim_time = EDGUI_TABVIEW_DEF_ANIM_TIME;
#endif
    ext->btns_hide = 0;

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_tabview, edgui_tabview_signal);

    /*Init the new tab tab*/
    if(copy == NULL) {
        ext->tab_name_ptr = edgui_mem_alloc(sizeof(char *));
        edgui_mem_assert(ext->tab_name_ptr);
        if(ext->tab_name_ptr == NULL) return NULL;
        ext->tab_name_ptr[0] = "";
        ext->tab_cnt         = 0;

        /* Set a size which fits into the parent.
         * Don't use `par` directly because if the tabview is created on a page it is moved to the
         * scrollable so the parent has changed */
        edgui_obj_set_size(new_tabview, edgui_obj_get_width_fit(edgui_obj_get_parent(new_tabview)),
                        edgui_obj_get_height_fit(edgui_obj_get_parent(new_tabview)));

        ext->content = edgui_cont_create(new_tabview, NULL);
        ext->btns    = edgui_btnm_create(new_tabview, NULL);
        ext->indic   = edgui_obj_create(ext->btns, NULL);

        edgui_obj_set_height(ext->btns, 3 * EDGUI_DPI / 4);
        edgui_btnm_set_map(ext->btns, tab_def);
        edgui_obj_set_event_cb(ext->btns, tab_btnm_event_cb);

        edgui_obj_set_width(ext->indic, EDGUI_DPI);
        edgui_obj_align(ext->indic, ext->btns, EDGUI_ALIGN_IN_BOTTOM_LEFT, 0, 0);
        edgui_obj_set_click(ext->indic, false);

        edgui_cont_set_fit2(ext->content, EDGUI_FIT_TIGHT, EDGUI_FIT_NONE);
        edgui_cont_set_layout(ext->content, EDGUI_LAYOUT_ROW_T);
        edgui_cont_set_style(ext->content, EDGUI_CONT_STYLE_MAIN, &edgui_style_transp_tight);
        edgui_obj_set_height(ext->content, edgui_obj_get_height(new_tabview) - edgui_obj_get_height(ext->btns));
        edgui_obj_align(ext->content, ext->btns, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_tabview_set_style(new_tabview, EDGUI_TABVIEW_STYLE_BG, th->style.tabview.bg);
            edgui_tabview_set_style(new_tabview, EDGUI_TABVIEW_STYLE_INDIC, th->style.tabview.indic);
            edgui_tabview_set_style(new_tabview, EDGUI_TABVIEW_STYLE_BTN_BG, th->style.tabview.btn.bg);
            edgui_tabview_set_style(new_tabview, EDGUI_TABVIEW_STYLE_BTN_REL, th->style.tabview.btn.rel);
            edgui_tabview_set_style(new_tabview, EDGUI_TABVIEW_STYLE_BTN_PR, th->style.tabview.btn.pr);
            edgui_tabview_set_style(new_tabview, EDGUI_TABVIEW_STYLE_BTN_TGL_REL, th->style.tabview.btn.tgl_rel);
            edgui_tabview_set_style(new_tabview, EDGUI_TABVIEW_STYLE_BTN_TGL_PR, th->style.tabview.btn.tgl_pr);
        } else {
            edgui_tabview_set_style(new_tabview, EDGUI_TABVIEW_STYLE_BG, &edgui_style_plain);
            edgui_tabview_set_style(new_tabview, EDGUI_TABVIEW_STYLE_BTN_BG, &edgui_style_transp);
            edgui_tabview_set_style(new_tabview, EDGUI_TABVIEW_STYLE_INDIC, &edgui_style_plain_color);
        }
    }
    /*Copy an existing tab view*/
    else {
        edgui_tabview_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->point_last.x           = 0;
        ext->point_last.y           = 0;
        ext->btns                   = edgui_btnm_create(new_tabview, copy_ext->btns);
        ext->indic                  = edgui_obj_create(ext->btns, copy_ext->indic);
        ext->content                = edgui_cont_create(new_tabview, copy_ext->content);
#if EDGUI_USE_ANIMATION
        ext->anim_time = copy_ext->anim_time;
#endif

        ext->tab_name_ptr = edgui_mem_alloc(sizeof(char *));
        edgui_mem_assert(ext->tab_name_ptr);
        if(ext->tab_name_ptr == NULL) return NULL;
        ext->tab_name_ptr[0] = "";
        edgui_btnm_set_map(ext->btns, ext->tab_name_ptr);

        uint16_t i;
        edgui_obj_t * new_tab;
        edgui_obj_t * copy_tab;
        for(i = 0; i < copy_ext->tab_cnt; i++) {
            new_tab  = edgui_tabview_add_tab(new_tabview, copy_ext->tab_name_ptr[i]);
            copy_tab = edgui_tabview_get_tab(copy, i);
            edgui_page_set_style(new_tab, EDGUI_PAGE_STYLE_BG, edgui_page_get_style(copy_tab, EDGUI_PAGE_STYLE_BG));
            edgui_page_set_style(new_tab, EDGUI_PAGE_STYLE_SCRL, edgui_page_get_style(copy_tab, EDGUI_PAGE_STYLE_SCRL));
            edgui_page_set_style(new_tab, EDGUI_PAGE_STYLE_SB, edgui_page_get_style(copy_tab, EDGUI_PAGE_STYLE_SB));
        }

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_tabview);
    }

    EDGUI_LOG_INFO("tab view created");

    return new_tabview;
}

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void edgui_tabview_clean(edgui_obj_t * obj)
{
    edgui_obj_t * scrl = edgui_page_get_scrl(obj);
    edgui_obj_clean(scrl);
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a new tab with the given name
 * @param tabview pointer to Tab view object where to ass the new tab
 * @param name the text on the tab button
 * @return pointer to the created page object (edgui_page). You can create your content here
 */
edgui_obj_t * edgui_tabview_add_tab(edgui_obj_t * tabview, const char * name)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);

    /*Create the container page*/
    edgui_obj_t * h = edgui_page_create(ext->content, NULL);
    edgui_obj_set_size(h, edgui_obj_get_width(tabview), edgui_obj_get_height(ext->content));
    edgui_page_set_sb_mode(h, EDGUI_SB_MODE_AUTO);
    edgui_page_set_style(h, EDGUI_PAGE_STYLE_BG, &edgui_style_transp);
    edgui_page_set_style(h, EDGUI_PAGE_STYLE_SCRL, &edgui_style_transp);

    if(page_signal == NULL) page_signal = edgui_obj_get_signal_cb(h);
    if(page_scrl_signal == NULL) page_scrl_signal = edgui_obj_get_signal_cb(edgui_page_get_scrl(h));
    edgui_obj_set_signal_cb(h, tabpage_signal);
    edgui_obj_set_signal_cb(edgui_page_get_scrl(h), tabpage_scrl_signal);

    /*Extend the button matrix map with the new name*/
    char * name_dm;
    name_dm = edgui_mem_alloc(strlen(name) + 1); /*+1 for the the closing '\0' */
    edgui_mem_assert(name_dm);
    if(name_dm == NULL) return NULL;
    strcpy(name_dm, name);

    ext->tab_cnt++;

    switch(ext->btns_pos) {
        case EDGUI_TABVIEW_BTNS_POS_TOP:
        case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
            ext->tab_name_ptr = edgui_mem_realloc(ext->tab_name_ptr, sizeof(char *) * (ext->tab_cnt + 1));
            break;
        case EDGUI_TABVIEW_BTNS_POS_LEFT:
        case EDGUI_TABVIEW_BTNS_POS_RIGHT:
            ext->tab_name_ptr = edgui_mem_realloc(ext->tab_name_ptr, sizeof(char *) * (ext->tab_cnt * 2));
            break;
    }

    edgui_mem_assert(ext->tab_name_ptr);
    if(ext->tab_name_ptr == NULL) return NULL;

    /* FIXME: It is not possible yet to switch tab button position from/to top/bottom from/to left/right at runtime.
     * Method: clean extra \n when switch from EDGUI_TABVIEW_BTNS_POS_LEFT or EDGUI_TABVIEW_BTNS_POS_RIGHT
     * to EDGUI_TABVIEW_BTNS_POS_TOP or EDGUI_TABVIEW_BTNS_POS_BOTTOM.
     */
    switch(ext->btns_pos) {
        case EDGUI_TABVIEW_BTNS_POS_TOP:
        case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
            ext->tab_name_ptr[ext->tab_cnt - 1] = name_dm;
            ext->tab_name_ptr[ext->tab_cnt]     = "";
            break;
        case EDGUI_TABVIEW_BTNS_POS_LEFT:
        case EDGUI_TABVIEW_BTNS_POS_RIGHT:
            if(ext->tab_cnt == 1) {
                ext->tab_name_ptr[0] = name_dm;
                ext->tab_name_ptr[1] = "";
            } else {
                ext->tab_name_ptr[ext->tab_cnt * 2 - 3] = "\n";
                ext->tab_name_ptr[ext->tab_cnt * 2 - 2] = name_dm;
                ext->tab_name_ptr[ext->tab_cnt * 2 - 1] = "";
            }
            break;
    }

    /* The button matrix's map still points to the old `tab_name_ptr` which might be freed by
     * `edgui_mem_realloc`. So make its current map invalid*/
    edgui_btnm_ext_t * btnm_ext = edgui_obj_get_ext_attr(ext->btns);
    btnm_ext->map_p          = NULL;

    edgui_btnm_set_map(ext->btns, ext->tab_name_ptr);
    edgui_btnm_set_btn_ctrl(ext->btns, ext->tab_cur, EDGUI_BTNM_CTRL_NO_REPEAT);

    /*Modify the indicator size*/
    const edgui_style_t * style_tabs = edgui_obj_get_style(ext->btns);
    edgui_coord_t indic_size;
    edgui_coord_t max_h, btn_h, act_y;

    switch(ext->btns_pos) {
        case EDGUI_TABVIEW_BTNS_POS_TOP:
        case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
            indic_size = (edgui_obj_get_width(tabview) - style_tabs->body.padding.inner * (ext->tab_cnt - 1) -
                          style_tabs->body.padding.left - style_tabs->body.padding.right) /
                         ext->tab_cnt;
            edgui_obj_set_width(ext->indic, indic_size);
            edgui_obj_set_x(ext->indic, indic_size * ext->tab_cur + style_tabs->body.padding.inner * ext->tab_cur +
                                         style_tabs->body.padding.left);
            break;
        case EDGUI_TABVIEW_BTNS_POS_LEFT:
        case EDGUI_TABVIEW_BTNS_POS_RIGHT:
            max_h = edgui_obj_get_height(ext->btns) - style_tabs->body.padding.top - style_tabs->body.padding.bottom;
            btn_h = max_h - ((ext->tab_cnt - 1) * style_tabs->body.padding.inner);
            btn_h = btn_h / ext->tab_cnt;
            btn_h--; /*-1 because e.g. height = 100 means 101 pixels (0..100)*/
            act_y = style_tabs->body.padding.top + ext->tab_cur * (btn_h + style_tabs->body.padding.inner);

            edgui_obj_set_height(ext->indic, btn_h);
            edgui_obj_set_y(ext->indic, act_y);
            break;
    }

    /*Set the first btn as active*/
    if(ext->tab_cnt == 1) {
        ext->tab_cur = 0;
    }

    tabview_realign(tabview); /*Set the size of the pages, tab buttons and indicator*/

    edgui_tabview_set_tab_act(tabview, ext->tab_cur, false);

    return h;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new tab
 * @param tabview pointer to Tab view object
 * @param id index of a tab to load
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 */
void edgui_tabview_set_tab_act(edgui_obj_t * tabview, uint16_t id, edgui_anim_enable_t anim)
{
#if EDGUI_USE_ANIMATION == 0
    anim = EDGUI_ANIM_OFF;
#endif
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);

    const edgui_style_t * style = edgui_obj_get_style(ext->content);

    if(id >= ext->tab_cnt) id = ext->tab_cnt - 1;

    edgui_btnm_clear_btn_ctrl(ext->btns, ext->tab_cur, EDGUI_BTNM_CTRL_TGL_STATE);

    ext->tab_cur = id;

    edgui_coord_t cont_x;

    switch(ext->btns_pos) {
        case EDGUI_TABVIEW_BTNS_POS_TOP:
        case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
            cont_x = -(edgui_obj_get_width(tabview) * id + style->body.padding.inner * id + style->body.padding.left);
            break;
        case EDGUI_TABVIEW_BTNS_POS_LEFT:
            cont_x = -((edgui_obj_get_width(tabview) - edgui_obj_get_width(ext->btns)) * id + style->body.padding.inner * id +
                       style->body.padding.left) +
                     edgui_obj_get_width(ext->btns);
            break;
        case EDGUI_TABVIEW_BTNS_POS_RIGHT:
            cont_x = -((edgui_obj_get_width(tabview) - edgui_obj_get_width(ext->btns)) * id + style->body.padding.inner * id +
                       style->body.padding.left);
            break;
    }

    if(anim == EDGUI_ANIM_OFF || edgui_tabview_get_anim_time(tabview) == 0) {
        edgui_obj_set_x(ext->content, cont_x);
    }
#if EDGUI_USE_ANIMATION
    else {
        edgui_anim_t a;
        a.var            = ext->content;
        a.start          = edgui_obj_get_x(ext->content);
        a.end            = cont_x;
        a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_obj_set_x;
        a.path_cb        = edgui_anim_path_linear;
        a.ready_cb       = NULL;
        a.act_time       = 0;
        a.time           = ext->anim_time;
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        edgui_anim_create(&a);
    }
#endif

    /*Move the indicator*/
    const edgui_style_t * tabs_style = edgui_obj_get_style(ext->btns);
    edgui_coord_t indic_size;
    edgui_coord_t indic_pos;

    switch(ext->btns_pos) {
        case EDGUI_TABVIEW_BTNS_POS_TOP:
        case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
            indic_size = edgui_obj_get_width(ext->indic);
            indic_pos  = indic_size * id + tabs_style->body.padding.inner * id + tabs_style->body.padding.left;
            break;
        case EDGUI_TABVIEW_BTNS_POS_LEFT:
        case EDGUI_TABVIEW_BTNS_POS_RIGHT:
            indic_size = edgui_obj_get_height(ext->indic);
            indic_pos  = tabs_style->body.padding.top + id * (indic_size + tabs_style->body.padding.inner);
            break;
    }

#if EDGUI_USE_ANIMATION
    if(anim == EDGUI_ANIM_OFF || ext->anim_time == 0)
#endif
    {
        switch(ext->btns_pos) {
            case EDGUI_TABVIEW_BTNS_POS_TOP:
            case EDGUI_TABVIEW_BTNS_POS_BOTTOM: edgui_obj_set_x(ext->indic, indic_pos); break;
            case EDGUI_TABVIEW_BTNS_POS_LEFT:
            case EDGUI_TABVIEW_BTNS_POS_RIGHT: edgui_obj_set_y(ext->indic, indic_pos); break;
        }
    }
#if EDGUI_USE_ANIMATION
    else {
        edgui_anim_t a;
        a.var = ext->indic;

        switch(ext->btns_pos) {
            case EDGUI_TABVIEW_BTNS_POS_TOP:
            case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
                a.start   = edgui_obj_get_x(ext->indic);
                a.end     = indic_pos;
                a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_x;
                break;
            case EDGUI_TABVIEW_BTNS_POS_LEFT:
            case EDGUI_TABVIEW_BTNS_POS_RIGHT:
                a.start   = edgui_obj_get_y(ext->indic);
                a.end     = indic_pos;
                a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
                break;
        }

        a.path_cb        = edgui_anim_path_linear;
        a.ready_cb       = NULL;
        a.act_time       = 0;
        a.time           = ext->anim_time;
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        edgui_anim_create(&a);
    }
#endif

    edgui_btnm_set_btn_ctrl(ext->btns, ext->tab_cur, EDGUI_BTNM_CTRL_TGL_STATE);
}

/**
 * Enable horizontal sliding with touch pad
 * @param tabview pointer to Tab view object
 * @param en true: enable sliding; false: disable sliding
 */
void edgui_tabview_set_sliding(edgui_obj_t * tabview, bool en)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    ext->slide_enable      = en == false ? 0 : 1;
}

/**
 * Set the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @param anim_time_ms time of animation in milliseconds
 */
void edgui_tabview_set_anim_time(edgui_obj_t * tabview, uint16_t anim_time)
{
#if EDGUI_USE_ANIMATION
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    ext->anim_time         = anim_time;
#else
    (void)tabview;
    (void)anim_time;
#endif
}

/**
 * Set the style of a tab view
 * @param tabview pointer to a tan view object
 * @param type which style should be set
 * @param style pointer to the new style
 */
void edgui_tabview_set_style(edgui_obj_t * tabview, edgui_tabview_style_t type, const edgui_style_t * style)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);

    switch(type) {
        case EDGUI_TABVIEW_STYLE_BG: edgui_obj_set_style(tabview, style); break;
        case EDGUI_TABVIEW_STYLE_BTN_BG:
            edgui_btnm_set_style(ext->btns, EDGUI_BTNM_STYLE_BG, style);
            tabview_realign(tabview);
            break;
        case EDGUI_TABVIEW_STYLE_BTN_REL:
            edgui_btnm_set_style(ext->btns, EDGUI_BTNM_STYLE_BTN_REL, style);
            tabview_realign(tabview);
            break;
        case EDGUI_TABVIEW_STYLE_BTN_PR: edgui_btnm_set_style(ext->btns, EDGUI_BTNM_STYLE_BTN_PR, style); break;
        case EDGUI_TABVIEW_STYLE_BTN_TGL_REL: edgui_btnm_set_style(ext->btns, EDGUI_BTNM_STYLE_BTN_TGL_REL, style); break;
        case EDGUI_TABVIEW_STYLE_BTN_TGL_PR: edgui_btnm_set_style(ext->btns, EDGUI_BTNM_STYLE_BTN_TGL_PR, style); break;
        case EDGUI_TABVIEW_STYLE_INDIC:
            edgui_obj_set_style(ext->indic, style);

            switch(ext->btns_pos) {
                case EDGUI_TABVIEW_BTNS_POS_TOP:
                case EDGUI_TABVIEW_BTNS_POS_BOTTOM: edgui_obj_set_height(ext->indic, style->body.padding.inner); break;
                case EDGUI_TABVIEW_BTNS_POS_LEFT:
                case EDGUI_TABVIEW_BTNS_POS_RIGHT: edgui_obj_set_width(ext->indic, style->body.padding.inner); break;
            }

            tabview_realign(tabview);
            break;
    }
}

/**
 * Set the position of tab select buttons
 * @param tabview pointer to a tan view object
 * @param btns_pos which button position
 */
void edgui_tabview_set_btns_pos(edgui_obj_t * tabview, edgui_tabview_btns_pos_t btns_pos)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);

    ext->btns_pos = btns_pos;
    tabview_realign(tabview);
}

/**
 * Set whether tab buttons are hidden
 * @param tabview pointer to a tab view object
 * @param en whether tab buttons are hidden
 */
void edgui_tabview_set_btns_hidden(edgui_obj_t * tabview, bool en)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);

    ext->btns_hide = en;
    tabview_realign(tabview);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the index of the currently active tab
 * @param tabview pointer to Tab view object
 * @return the active btn index
 */
uint16_t edgui_tabview_get_tab_act(const edgui_obj_t * tabview)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    return ext->tab_cur;
}

/**
 * Get the number of tabs
 * @param tabview pointer to Tab view object
 * @return btn count
 */
uint16_t edgui_tabview_get_tab_count(const edgui_obj_t * tabview)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    return ext->tab_cnt;
}

/**
 * Get the page (content area) of a tab
 * @param tabview pointer to Tab view object
 * @param id index of the btn (>= 0)
 * @return pointer to page (edgui_page) object
 */
edgui_obj_t * edgui_tabview_get_tab(const edgui_obj_t * tabview, uint16_t id)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    uint16_t i             = 0;
    edgui_obj_t * page        = edgui_obj_get_child_back(ext->content, NULL);

    while(page != NULL && i != id) {
        i++;
        page = edgui_obj_get_child_back(ext->content, page);
    }

    if(i == id) return page;

    return NULL;
}

/**
 * Get horizontal sliding is enabled or not
 * @param tabview pointer to Tab view object
 * @return true: enable sliding; false: disable sliding
 */
bool edgui_tabview_get_sliding(const edgui_obj_t * tabview)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    return ext->slide_enable ? true : false;
}

/**
 * Get the animation time of tab view when a new tab is loaded
 * @param tabview pointer to Tab view object
 * @return time of animation in milliseconds
 */
uint16_t edgui_tabview_get_anim_time(const edgui_obj_t * tabview)
{
#if EDGUI_USE_ANIMATION
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    return ext->anim_time;
#else
    (void)tabview;
    return 0;
#endif
}

/**
 * Get a style of a tab view
 * @param tabview pointer to a ab view object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_tabview_get_style(const edgui_obj_t * tabview, edgui_tabview_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_tabview_ext_t * ext   = edgui_obj_get_ext_attr(tabview);

    switch(type) {
        case EDGUI_TABVIEW_STYLE_BG: style = edgui_obj_get_style(tabview); break;
        case EDGUI_TABVIEW_STYLE_BTN_BG: style = edgui_btnm_get_style(ext->btns, EDGUI_BTNM_STYLE_BG); break;
        case EDGUI_TABVIEW_STYLE_BTN_REL: style = edgui_btnm_get_style(ext->btns, EDGUI_BTNM_STYLE_BTN_REL); break;
        case EDGUI_TABVIEW_STYLE_BTN_PR: style = edgui_btnm_get_style(ext->btns, EDGUI_BTNM_STYLE_BTN_PR); break;
        case EDGUI_TABVIEW_STYLE_BTN_TGL_REL: style = edgui_btnm_get_style(ext->btns, EDGUI_BTNM_STYLE_BTN_TGL_REL); break;
        case EDGUI_TABVIEW_STYLE_BTN_TGL_PR: style = edgui_btnm_get_style(ext->btns, EDGUI_BTNM_STYLE_BTN_TGL_PR); break;
        default: style = NULL; break;
    }

    return style;
}

/**
 * Get position of tab select buttons
 * @param tabview pointer to a ab view object
 */
edgui_tabview_btns_pos_t edgui_tabview_get_btns_pos(const edgui_obj_t * tabview)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    return ext->btns_pos;
}

/**
 * Get whether tab buttons are hidden
 * @param tabview pointer to a tab view object
 * @return whether tab buttons are hidden
 */
bool edgui_tabview_get_btns_hidden(const edgui_obj_t * tabview)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);

    return ext->btns_hide;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the Tab view
 * @param tabview pointer to a Tab view object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_tabview_signal(edgui_obj_t * tabview, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(tabview, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    if(sign == EDGUI_SIGNAL_CLEANUP) {
        uint8_t i;
        for(i = 0; ext->tab_name_ptr[i][0] != '\0'; i++) edgui_mem_free(ext->tab_name_ptr[i]);

        edgui_mem_free(ext->tab_name_ptr);
        ext->tab_name_ptr = NULL;
        ext->btns         = NULL; /*These objects were children so they are already invalid*/
        ext->content      = NULL;
    } else if(sign == EDGUI_SIGNAL_CORD_CHG) {
        if(ext->content != NULL && (edgui_obj_get_width(tabview) != edgui_area_get_width(param) ||
                                    edgui_obj_get_height(tabview) != edgui_area_get_height(param))) {
            tabview_realign(tabview);
        }
    } else if(sign == EDGUI_SIGNAL_RELEASED) {
#if EDGUI_USE_GROUP
        /*If released by a KEYPAD or ENCODER then really the tab buttons should be released.
         * So simulate a CLICK on the tab buttons*/
        edgui_indev_t * indev         = edgui_indev_get_act();
        edgui_indev_type_t indev_type = edgui_indev_get_type(indev);
        if(indev_type == EDGUI_INDEV_TYPE_KEYPAD ||
           (indev_type == EDGUI_INDEV_TYPE_ENCODER && edgui_group_get_editing(edgui_obj_get_group(tabview)))) {
            edgui_event_send(ext->btns, EDGUI_EVENT_CLICKED, edgui_event_get_data());
        }
#endif
    } else if(sign == EDGUI_SIGNAL_FOCUS || sign == EDGUI_SIGNAL_DEFOCUS || sign == EDGUI_SIGNAL_CONTROL) {
        /* The button matrix is not in a group (the tab view is in it) but it should handle the
         * group signals. So propagate the related signals to the button matrix manually*/
        if(ext->btns) {
            ext->btns->signal_cb(ext->btns, sign, param);
        }

        if(sign == EDGUI_SIGNAL_FOCUS) {
            edgui_indev_type_t indev_type = edgui_indev_get_type(edgui_indev_get_act());
            /*With ENCODER select the first button only in edit mode*/
            if(indev_type == EDGUI_INDEV_TYPE_ENCODER) {
#if EDGUI_USE_GROUP
                edgui_group_t * g = edgui_obj_get_group(tabview);
                if(edgui_group_get_editing(g)) {
                    edgui_btnm_ext_t * btnm_ext = edgui_obj_get_ext_attr(ext->btns);
                    btnm_ext->btn_id_pr      = 0;
                    edgui_obj_invalidate(ext->btns);
                }
#endif
            } else {
                edgui_btnm_ext_t * btnm_ext = edgui_obj_get_ext_attr(ext->btns);
                btnm_ext->btn_id_pr      = 0;
                edgui_obj_invalidate(ext->btns);
            }
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
        buf->type[i] = "edgui_tabview";
    }

    return res;
}

/**
 * Signal function of a tab's page
 * @param tab pointer to a tab page object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t tabpage_signal(edgui_obj_t * tab_page, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = page_signal(tab_page, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_obj_t * cont    = edgui_obj_get_parent(tab_page);
    edgui_obj_t * tabview = edgui_obj_get_parent(cont);

    if(edgui_tabview_get_sliding(tabview) == false) return res;

    if(sign == EDGUI_SIGNAL_PRESSED) {
        tabpage_pressed_handler(tabview, tab_page);
    } else if(sign == EDGUI_SIGNAL_PRESSING) {
        tabpage_pressing_handler(tabview, tab_page);
    } else if(sign == EDGUI_SIGNAL_RELEASED || sign == EDGUI_SIGNAL_PRESS_LOST) {
        tabpage_press_lost_handler(tabview, tab_page);
    }

    return res;
}
/**
 * Signal function of the tab page's scrollable object
 * @param tab_scrl pointer to a tab page's scrollable object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t tabpage_scrl_signal(edgui_obj_t * tab_scrl, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = page_scrl_signal(tab_scrl, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_obj_t * tab_page = edgui_obj_get_parent(tab_scrl);
    edgui_obj_t * cont     = edgui_obj_get_parent(tab_page);
    edgui_obj_t * tabview  = edgui_obj_get_parent(cont);

    if(edgui_tabview_get_sliding(tabview) == false) return res;

    if(sign == EDGUI_SIGNAL_PRESSED) {
        tabpage_pressed_handler(tabview, tab_page);
    } else if(sign == EDGUI_SIGNAL_PRESSING) {
        tabpage_pressing_handler(tabview, tab_page);
    } else if(sign == EDGUI_SIGNAL_RELEASED || sign == EDGUI_SIGNAL_PRESS_LOST) {
        tabpage_press_lost_handler(tabview, tab_page);
    }

    return res;
}

/**
 * Called when a tab's page or scrollable object is pressed
 * @param tabview pointer to the btn view object
 * @param tabpage pointer to the page of a btn
 */
static void tabpage_pressed_handler(edgui_obj_t * tabview, edgui_obj_t * tabpage)
{
    (void)tabpage;

    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    edgui_indev_t * indev     = edgui_indev_get_act();
    edgui_indev_get_point(indev, &ext->point_last);
}

/**
 * Called when a tab's page or scrollable object is being pressed
 * @param tabview pointer to the btn view object
 * @param tabpage pointer to the page of a btn
 */
static void tabpage_pressing_handler(edgui_obj_t * tabview, edgui_obj_t * tabpage)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    edgui_indev_t * indev     = edgui_indev_get_act();
    edgui_point_t point_act;
    edgui_indev_get_point(indev, &point_act);
    edgui_coord_t x_diff = point_act.x - ext->point_last.x;
    edgui_coord_t y_diff = point_act.y - ext->point_last.y;

    if(!ext->scroll_ver && (x_diff >= EDGUI_INDEV_DEF_DRAG_LIMIT || x_diff <= -EDGUI_INDEV_DEF_DRAG_LIMIT)) {
        ext->draging = 1;
        /*Check if the page is on the edge */
        if((edgui_page_on_edge(tabpage, EDGUI_PAGE_EDGE_LEFT) && x_diff > 0) ||
           (edgui_page_on_edge(tabpage, EDGUI_PAGE_EDGE_RIGHT) && x_diff < 0)) {
            if(ext->drag_hor == 0) {
                ext->point_last.x = point_act.x;
                ext->point_last.y = point_act.y;
            }
            ext->drag_hor = 1;
            edgui_obj_set_drag(edgui_page_get_scrl(tabpage), false);

        } else if(ext->drag_hor == 0) {
            ext->drag_hor = 0;
        }
    } else if(y_diff >= EDGUI_INDEV_DEF_DRAG_LIMIT || y_diff <= -EDGUI_INDEV_DEF_DRAG_LIMIT) {
        ext->drag_hor   = 0;
        ext->draging    = 1;
        ext->scroll_ver = 1;
    } else
        ext->draging = 0;

    if(ext->drag_hor) {
        edgui_obj_set_x(ext->content, edgui_obj_get_x(ext->content) + point_act.x - ext->point_last.x);
        ext->point_last.x = point_act.x;
        ext->point_last.y = point_act.y;

        /*Move the indicator*/
        const edgui_style_t * tabs_style = edgui_obj_get_style(ext->btns);
        edgui_coord_t indic_size;
        edgui_coord_t p;
        edgui_coord_t indic_y;
        const edgui_style_t * indic_style;

        switch(ext->btns_pos) {
            case EDGUI_TABVIEW_BTNS_POS_TOP:
            case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
                indic_size  = edgui_obj_get_width(ext->indic);
                indic_style = edgui_obj_get_style(ext->indic);
                p = ((tabpage->coords.x1 - tabview->coords.x1) * (indic_size + tabs_style->body.padding.inner)) /
                    edgui_obj_get_width(tabview);

                edgui_obj_set_x(ext->indic, indic_size * ext->tab_cur + tabs_style->body.padding.inner * ext->tab_cur +
                                             indic_style->body.padding.left - p);
                break;
            case EDGUI_TABVIEW_BTNS_POS_LEFT:
            case EDGUI_TABVIEW_BTNS_POS_RIGHT:
                indic_size = edgui_obj_get_height(ext->indic);
                indic_y = tabs_style->body.padding.top + ext->tab_cur * (indic_size + tabs_style->body.padding.inner);
                edgui_obj_set_y(ext->indic, indic_y);
                break;
        }
    }
}

/**
 * Called when a tab's page or scrollable object is released or the press is lost
 * @param tabview pointer to the btn view object
 * @param tabpage pointer to the page of a btn
 */
static void tabpage_press_lost_handler(edgui_obj_t * tabview, edgui_obj_t * tabpage)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);
    ext->drag_hor          = 0;
    ext->draging           = 0;
    ext->scroll_ver        = 0;

    edgui_obj_set_drag(edgui_page_get_scrl(tabpage), true);

    edgui_indev_t * indev = edgui_indev_get_act();
    edgui_point_t point_act;
    edgui_indev_get_point(indev, &point_act);
    edgui_point_t vect;
    edgui_indev_get_vect(indev, &vect);
    edgui_coord_t x_predict = 0;

    while(vect.x != 0) {
        x_predict += vect.x;
        vect.x = vect.x * (100 - EDGUI_INDEV_DEF_DRAG_THROW) / 100;
    }

    edgui_coord_t page_x1  = tabpage->coords.x1 - tabview->coords.x1 + x_predict;
    edgui_coord_t page_x2  = page_x1 + edgui_obj_get_width(tabpage);
    edgui_coord_t treshold = edgui_obj_get_width(tabview) / 2;

    uint16_t tab_cur = ext->tab_cur;
    if(page_x1 > treshold) {
        if(tab_cur != 0) tab_cur--;
    } else if(page_x2 < treshold) {
        if(tab_cur < ext->tab_cnt - 1) tab_cur++;
    }

    uint32_t id_prev = edgui_tabview_get_tab_act(tabview);
    edgui_tabview_set_tab_act(tabview, tab_cur, EDGUI_ANIM_ON);
    uint32_t id_new = edgui_tabview_get_tab_act(tabview);

    edgui_res_t res = EDGUI_RES_OK;
    if(id_prev != id_new) res = edgui_event_send(tabview, EDGUI_EVENT_VALUE_CHANGED, &id_new);

    if(res != EDGUI_RES_OK) return;
}

/**
 * Called when a tab button is clicked
 * @param tab_btnm pointer to the tab's button matrix object
 * @param event type of the event
 */
static void tab_btnm_event_cb(edgui_obj_t * tab_btnm, edgui_event_t event)
{
    if(event != EDGUI_EVENT_CLICKED) return;

    uint16_t btn_id = edgui_btnm_get_active_btn(tab_btnm);
    if(btn_id == EDGUI_BTNM_BTN_NONE) return;

    edgui_btnm_clear_btn_ctrl_all(tab_btnm, EDGUI_BTNM_CTRL_TGL_STATE);
    edgui_btnm_set_btn_ctrl(tab_btnm, btn_id, EDGUI_BTNM_CTRL_TGL_STATE);

    edgui_obj_t * tabview = edgui_obj_get_parent(tab_btnm);

    uint32_t id_prev = edgui_tabview_get_tab_act(tabview);
    edgui_tabview_set_tab_act(tabview, btn_id, EDGUI_ANIM_ON);
    uint32_t id_new = edgui_tabview_get_tab_act(tabview);

    edgui_res_t res = EDGUI_RES_OK;
    if(id_prev != id_new) res = edgui_event_send(tabview, EDGUI_EVENT_VALUE_CHANGED, &id_new);

    if(res != EDGUI_RES_OK) return;
}

/**
 * Realign and resize the elements of Tab view
 * @param tabview pointer to a Tab view object
 */
static void tabview_realign(edgui_obj_t * tabview)
{
    edgui_tabview_ext_t * ext = edgui_obj_get_ext_attr(tabview);

    edgui_obj_set_width(ext->btns, edgui_obj_get_width(tabview));

    if(ext->btns_hide) {
        edgui_obj_set_hidden(ext->btns, true);
        edgui_obj_set_hidden(ext->indic, true);
        edgui_obj_set_height(ext->content, edgui_obj_get_height(tabview));
        edgui_obj_align(ext->content, NULL, EDGUI_ALIGN_IN_TOP_LEFT, 0, 0);
    } else if(ext->tab_cnt != 0) {
        edgui_obj_set_hidden(ext->btns, false);
        edgui_obj_set_hidden(ext->indic, false);

        const edgui_style_t * style_btn_bg  = edgui_tabview_get_style(tabview, EDGUI_TABVIEW_STYLE_BTN_BG);
        const edgui_style_t * style_btn_rel = edgui_tabview_get_style(tabview, EDGUI_TABVIEW_STYLE_BTN_REL);

        /*Set the indicator width/height*/
        edgui_coord_t indic_size;
        edgui_coord_t max_h;

        switch(ext->btns_pos) {
            case EDGUI_TABVIEW_BTNS_POS_TOP:
            case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
                indic_size = (edgui_obj_get_width(tabview) - style_btn_bg->body.padding.inner * (ext->tab_cnt - 1) -
                              style_btn_bg->body.padding.left - style_btn_bg->body.padding.right) /
                             ext->tab_cnt;
                edgui_obj_set_width(ext->indic, indic_size);
                break;
            case EDGUI_TABVIEW_BTNS_POS_LEFT:
            case EDGUI_TABVIEW_BTNS_POS_RIGHT:
                edgui_obj_set_height(ext->btns, edgui_obj_get_height(tabview));

                max_h =
                    edgui_obj_get_height(ext->btns) - style_btn_bg->body.padding.top - style_btn_bg->body.padding.bottom;
                indic_size = max_h - ((ext->tab_cnt - 1) * style_btn_bg->body.padding.inner);
                indic_size = indic_size / ext->tab_cnt;
                indic_size--; /*-1 because e.g. height = 100 means 101 pixels (0..100)*/
                edgui_obj_set_height(ext->indic, indic_size);
                break;
        }

        /*Set the tabs height/width*/
        edgui_coord_t btns_size;

        switch(ext->btns_pos) {
            case EDGUI_TABVIEW_BTNS_POS_TOP:
            case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
                btns_size = edgui_font_get_line_height(style_btn_rel->text.font) + style_btn_rel->body.padding.top +
                            style_btn_rel->body.padding.bottom + style_btn_bg->body.padding.top +
                            style_btn_bg->body.padding.bottom;
                edgui_obj_set_height(ext->btns, btns_size);
                break;
            case EDGUI_TABVIEW_BTNS_POS_LEFT:
            case EDGUI_TABVIEW_BTNS_POS_RIGHT:
                btns_size = edgui_font_get_glyph_width(style_btn_rel->text.font, 'A', '\0') +
                            style_btn_rel->body.padding.left + style_btn_rel->body.padding.right +
                            style_btn_bg->body.padding.left + style_btn_bg->body.padding.right;
                edgui_obj_set_width(ext->btns, btns_size);
                break;
        }

        switch(ext->btns_pos) {
            case EDGUI_TABVIEW_BTNS_POS_TOP:
            case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
                edgui_obj_set_height(ext->content, edgui_obj_get_height(tabview) - edgui_obj_get_height(ext->btns));
                break;
            case EDGUI_TABVIEW_BTNS_POS_LEFT:
            case EDGUI_TABVIEW_BTNS_POS_RIGHT: edgui_obj_set_height(ext->content, edgui_obj_get_height(tabview)); break;
        }

        switch(ext->btns_pos) {
            case EDGUI_TABVIEW_BTNS_POS_TOP:
                edgui_obj_align(ext->btns, NULL, EDGUI_ALIGN_IN_TOP_LEFT, 0, 0);
                edgui_obj_align(ext->content, ext->btns, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
                edgui_obj_align(ext->indic, ext->btns, EDGUI_ALIGN_IN_BOTTOM_LEFT, 0, 0);

                edgui_cont_set_fit2(ext->content, EDGUI_FIT_TIGHT, EDGUI_FIT_NONE);
                edgui_cont_set_layout(ext->content, EDGUI_LAYOUT_ROW_T);
                edgui_obj_set_height(ext->content, edgui_obj_get_height(tabview) - edgui_obj_get_height(ext->btns));
                break;
            case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
                edgui_obj_align(ext->content, NULL, EDGUI_ALIGN_IN_TOP_LEFT, 0, 0);
                edgui_obj_align(ext->btns, ext->content, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
                edgui_obj_align(ext->indic, ext->btns, EDGUI_ALIGN_IN_TOP_LEFT, 0, 0);

                edgui_cont_set_fit2(ext->content, EDGUI_FIT_TIGHT, EDGUI_FIT_NONE);
                edgui_cont_set_layout(ext->content, EDGUI_LAYOUT_ROW_T);
                edgui_obj_set_height(ext->content, edgui_obj_get_height(tabview) - edgui_obj_get_height(ext->btns));
                break;
            case EDGUI_TABVIEW_BTNS_POS_LEFT:
                edgui_obj_align(ext->btns, NULL, EDGUI_ALIGN_IN_TOP_LEFT, 0, 0);
                edgui_obj_align(ext->content, tabview, EDGUI_ALIGN_IN_TOP_LEFT, edgui_obj_get_width(ext->btns), 0);
                edgui_obj_align(ext->indic, ext->btns, EDGUI_ALIGN_IN_TOP_RIGHT, 0, 0);

                edgui_cont_set_fit2(ext->content, EDGUI_FIT_TIGHT, EDGUI_FIT_NONE);
                edgui_cont_set_layout(ext->content, EDGUI_LAYOUT_ROW_T);
                edgui_obj_set_width(ext->content, edgui_obj_get_width(tabview) - edgui_obj_get_width(ext->btns));

                edgui_obj_set_height(ext->btns, edgui_obj_get_height(tabview));
                edgui_obj_set_width(ext->indic, style_btn_bg->body.padding.inner);
                break;
            case EDGUI_TABVIEW_BTNS_POS_RIGHT:
                edgui_obj_align(ext->btns, NULL, EDGUI_ALIGN_IN_TOP_RIGHT, 0, 0);
                edgui_obj_align(ext->content, tabview, EDGUI_ALIGN_IN_TOP_LEFT, 0, 0);
                edgui_obj_align(ext->indic, ext->btns, EDGUI_ALIGN_IN_TOP_LEFT, 0, 0);

                edgui_cont_set_fit2(ext->content, EDGUI_FIT_TIGHT, EDGUI_FIT_NONE);
                edgui_cont_set_layout(ext->content, EDGUI_LAYOUT_ROW_T);
                edgui_obj_set_width(ext->content, edgui_obj_get_width(tabview) - edgui_obj_get_width(ext->btns));

                edgui_obj_set_height(ext->btns, edgui_obj_get_height(tabview));
                edgui_obj_set_width(ext->indic, style_btn_bg->body.padding.inner);
                break;
        }
    }

    edgui_obj_t * pages = edgui_obj_get_child(ext->content, NULL);
    while(pages != NULL) {
        if(edgui_obj_get_signal_cb(pages) == tabpage_signal) { /*Be sure adjust only the pages (user can other things)*/
            switch(ext->btns_pos) {
                case EDGUI_TABVIEW_BTNS_POS_TOP:
                case EDGUI_TABVIEW_BTNS_POS_BOTTOM:
                    edgui_obj_set_size(pages, edgui_obj_get_width(tabview), edgui_obj_get_height(ext->content));
                    break;
                case EDGUI_TABVIEW_BTNS_POS_LEFT:
                case EDGUI_TABVIEW_BTNS_POS_RIGHT:
                    edgui_obj_set_size(pages, edgui_obj_get_width(tabview) - edgui_obj_get_width(ext->btns),
                                    edgui_obj_get_height(ext->content));
                    break;
            }
        }
        pages = edgui_obj_get_child(ext->content, pages);
    }

    if(!ext->btns_hide) {
        switch(ext->btns_pos) {
            case EDGUI_TABVIEW_BTNS_POS_TOP: edgui_obj_align(ext->indic, ext->btns, EDGUI_ALIGN_IN_BOTTOM_LEFT, 0, 0); break;
            case EDGUI_TABVIEW_BTNS_POS_BOTTOM: edgui_obj_align(ext->indic, ext->btns, EDGUI_ALIGN_IN_TOP_LEFT, 0, 0); break;
            case EDGUI_TABVIEW_BTNS_POS_LEFT: edgui_obj_align(ext->indic, ext->btns, EDGUI_ALIGN_IN_TOP_RIGHT, 0, 0); break;
            case EDGUI_TABVIEW_BTNS_POS_RIGHT: edgui_obj_align(ext->indic, ext->btns, EDGUI_ALIGN_IN_TOP_LEFT, 0, 0); break;
        }
    }

    edgui_tabview_set_tab_act(tabview, ext->tab_cur, EDGUI_ANIM_OFF);
}
#endif
