/**
 * @file edgui_tileview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_tileview.h"
#if EDGUI_USE_TILEVIEW != 0

#include <stdbool.h>
#include "edgui_cont.h"
#include "../edgui_themes/edgui_theme.h"

/*********************
 *      DEFINES
 *********************/
#if EDGUI_USE_ANIMATION
#ifndef EDGUI_TILEVIEW_DEF_ANIM_TIME
#define EDGUI_TILEVIEW_DEF_ANIM_TIME 300 /*Animation time loading a tile [ms] (0: no animation)  */
#endif
#else
#undef EDGUI_TILEVIEW_DEF_ANIM_TIME
#define EDGUI_TILEVIEW_DEF_ANIM_TIME 0 /*No animations*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_tileview_signal(edgui_obj_t * tileview, edgui_signal_t sign, void * param);
static edgui_res_t edgui_tileview_scrl_signal(edgui_obj_t * scrl, edgui_signal_t sign, void * param);
static void tileview_scrl_event_cb(edgui_obj_t * scrl, edgui_event_t event);
static void drag_end_handler(edgui_obj_t * tileview);
static bool set_valid_drag_dirs(edgui_obj_t * tileview);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;
static edgui_signal_cb_t ancestor_scrl_signal;
static edgui_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a tileview object
 * @param par pointer to an object, it will be the parent of the new tileview
 * @param copy pointer to a tileview object, if not NULL then the new object will be copied from it
 * @return pointer to the created tileview
 */
edgui_obj_t * edgui_tileview_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("tileview create started");

    /*Create the ancestor of tileview*/
    edgui_obj_t * new_tileview = edgui_page_create(par, copy);
    edgui_mem_assert(new_tileview);
    if(new_tileview == NULL) return NULL;

    /*Allocate the tileview type specific extended data*/
    edgui_tileview_ext_t * ext = edgui_obj_allocate_ext_attr(new_tileview, sizeof(edgui_tileview_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_tileview);
    if(ancestor_scrl_signal == NULL) ancestor_scrl_signal = edgui_obj_get_signal_cb(edgui_page_get_scrl(new_tileview));
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_tileview);

        /*Initialize the allocated 'ext' */
#if EDGUI_USE_ANIMATION
    ext->anim_time = EDGUI_TILEVIEW_DEF_ANIM_TIME;
#endif
    ext->act_id.x      = 0;
    ext->act_id.y      = 0;
    ext->valid_pos     = NULL;
    ext->valid_pos_cnt = 0;

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_tileview, edgui_tileview_signal);
    edgui_obj_set_signal_cb(edgui_page_get_scrl(new_tileview), edgui_tileview_scrl_signal);

    /*Init the new tileview*/
    if(copy == NULL) {
        /* Set a size which fits into the parent.
         * Don't use `par` directly because if the tileview is created on a page it is moved to the
         * scrollable so the parent has changed */
        edgui_obj_set_size(new_tileview, edgui_obj_get_width_fit(edgui_obj_get_parent(new_tileview)),
                        edgui_obj_get_height_fit(edgui_obj_get_parent(new_tileview)));

        edgui_obj_set_drag_throw(edgui_page_get_scrl(new_tileview), false);
        edgui_page_set_scrl_fit(new_tileview, EDGUI_FIT_TIGHT);
        edgui_obj_set_event_cb(ext->page.scrl, tileview_scrl_event_cb);
        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_page_set_style(new_tileview, EDGUI_PAGE_STYLE_BG, th->style.tileview.bg);
            edgui_page_set_style(new_tileview, EDGUI_PAGE_STYLE_SCRL, th->style.tileview.scrl);
            edgui_page_set_style(new_tileview, EDGUI_PAGE_STYLE_SB, th->style.tileview.sb);
        } else {
            edgui_page_set_style(new_tileview, EDGUI_PAGE_STYLE_BG, &edgui_style_transp_tight);
            edgui_page_set_style(new_tileview, EDGUI_PAGE_STYLE_SCRL, &edgui_style_transp_tight);
        }
    }
    /*Copy an existing tileview*/
    else {
        edgui_tileview_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->act_id.x                = copy_ext->act_id.x;
        ext->act_id.y                = copy_ext->act_id.y;
        ext->valid_pos               = copy_ext->valid_pos;
        ext->valid_pos_cnt           = copy_ext->valid_pos_cnt;
#if EDGUI_USE_ANIMATION
        ext->anim_time = copy_ext->anim_time;
#endif

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_tileview);
    }

    EDGUI_LOG_INFO("tileview created");

    return new_tileview;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Register an object on the tileview. The register object will able to slide the tileview
 * @param tileview pointer to a Tileview object
 * @param element pointer to an object
 */
void edgui_tileview_add_element(edgui_obj_t * tileview, edgui_obj_t * element)
{
    /* Let the objects event to propagate to the scrollable part of the tileview.
     * It is required the handle dargging of the tileview with the element.*/
    element->parent_event = 1;
    edgui_obj_set_drag_parent(element, true);

    /* When adding a new element the coordinates may shift.
     * For example y=1 can become y=1 if an element is added to the top.
     * So be sure the current tile is correctly shown*/
    edgui_tileview_ext_t * ext = edgui_obj_get_ext_attr(tileview);
    edgui_tileview_set_tile_act(tileview, ext->act_id.x, ext->act_id.y, false);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the valid position's indices. The scrolling will be possible only to these positions.
 * @param tileview pointer to a Tileview object
 * @param valid_pos array width the indices. E.g. `edgui_point_t p[] = {{0,0}, {1,0}, {1,1}`. Only the
 * pointer is saved so can't be a local variable.
 * @param valid_pos_cnt numner of elements in `valid_pos` array
 */
void edgui_tileview_set_valid_positions(edgui_obj_t * tileview, const edgui_point_t * valid_pos, uint16_t valid_pos_cnt)
{
    edgui_tileview_ext_t * ext = edgui_obj_get_ext_attr(tileview);
    ext->valid_pos          = valid_pos;
    ext->valid_pos_cnt      = valid_pos_cnt;

    /*If valid pos. is selected do nothing*/
    uint16_t i;
    for(i = 0; i < valid_pos_cnt; i++) {
        if(valid_pos->x == ext->act_id.x && valid_pos->y == ext->act_id.y) {
            return;
        }
    }

    /*Set a valid position if now an invalid is selected*/
    if(valid_pos_cnt > 0) {
        edgui_tileview_set_tile_act(tileview, valid_pos[0].x, valid_pos[0].y, EDGUI_ANIM_OFF);
    }
}

/**
 * Set the tile to be shown
 * @param tileview pointer to a tileview object
 * @param x column id (0, 1, 2...)
 * @param y line id (0, 1, 2...)
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 */
void edgui_tileview_set_tile_act(edgui_obj_t * tileview, edgui_coord_t x, edgui_coord_t y, edgui_anim_enable_t anim)
{
#if EDGUI_USE_ANIMATION == 0
    anim = EDGUI_ANIM_OFF;
#endif

    edgui_tileview_ext_t * ext = edgui_obj_get_ext_attr(tileview);

    uint32_t tile_id;
    bool valid = false;
    for(tile_id = 0; tile_id < ext->valid_pos_cnt; tile_id++) {
        if(ext->valid_pos[tile_id].x == x && ext->valid_pos[tile_id].y == y) {
            valid = true;
        }
    }

    if(valid == false) return; /*Don't load not valid tiles*/

    ext->act_id.x = x;
    ext->act_id.y = y;

    edgui_coord_t x_coord = -x * edgui_obj_get_width(tileview);
    edgui_coord_t y_coord = -y * edgui_obj_get_height(tileview);
    edgui_obj_t * scrl    = edgui_page_get_scrl(tileview);
    if(anim) {
#if EDGUI_USE_ANIMATION
        edgui_coord_t x_act = edgui_obj_get_x(scrl);
        edgui_coord_t y_act = edgui_obj_get_y(scrl);

        edgui_anim_t a;
        a.var            = scrl;
        a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_obj_set_x;
        a.path_cb        = edgui_anim_path_linear;
        a.ready_cb       = NULL;
        a.act_time       = 0;
        a.time           = ext->anim_time;
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;

        if(x_coord != x_act) {
            a.start = x_act;
            a.end   = x_coord;
            edgui_anim_create(&a);
        }

        if(y_coord != y_act) {
            a.start   = y_act;
            a.end     = y_coord;
            a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
            edgui_anim_create(&a);
        }
#endif
    } else {
        edgui_obj_set_pos(scrl, x_coord, y_coord);
    }

    edgui_res_t res = EDGUI_RES_OK;
    res          = edgui_event_send(tileview, EDGUI_EVENT_VALUE_CHANGED, &tile_id);
    if(res != EDGUI_RES_OK) return; /*Prevent the tile loading*/
}

/**
 * Set a style of a tileview.
 * @param tileview pointer to tileview object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_tileview_set_style(edgui_obj_t * tileview, edgui_tileview_style_t type, const edgui_style_t * style)
{

    switch(type) {
        case EDGUI_TILEVIEW_STYLE_MAIN: edgui_obj_set_style(tileview, style); break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" functions come here
 */

/**
 * Get style of a tileview.
 * @param tileview pointer to tileview object
 * @param type which style should be get
 * @return style pointer to the style
 */
const edgui_style_t * edgui_tileview_get_style(const edgui_obj_t * tileview, edgui_tileview_style_t type)
{
    const edgui_style_t * style = NULL;
    switch(type) {
        case EDGUI_TILEVIEW_STYLE_MAIN: style = edgui_obj_get_style(tileview); break;
        default: style = NULL;
    }

    return style;
}

/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the tileview
 * @param tileview pointer to a tileview object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_tileview_signal(edgui_obj_t * tileview, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(tileview, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_tileview";
    }

    return res;
}

/**
 * Signal function of the tileview scrollable
 * @param tileview pointer to the scrollable part of the tileview object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_tileview_scrl_signal(edgui_obj_t * scrl, edgui_signal_t sign, void * param)
{

    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_scrl_signal(scrl, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_obj_t * tileview         = edgui_obj_get_parent(scrl);
    const edgui_style_t * style_bg = edgui_tileview_get_style(tileview, EDGUI_TILEVIEW_STYLE_MAIN);

    /*Apply constraint on moving of the tileview*/
    if(sign == EDGUI_SIGNAL_CORD_CHG) {
        edgui_indev_t * indev = edgui_indev_get_act();
        if(indev) {
            edgui_tileview_ext_t * ext = edgui_obj_get_ext_attr(tileview);

            /*Set horizontal drag constraint if no vertical constraint an dragged to valid x
             * direction */
            if(ext->drag_ver == 0 &&
               ((ext->drag_right_en && indev->proc.types.pointer.drag_sum.x <= -EDGUI_INDEV_DEF_DRAG_LIMIT) ||
                (ext->drag_left_en && indev->proc.types.pointer.drag_sum.x >= EDGUI_INDEV_DEF_DRAG_LIMIT))) {
                ext->drag_hor = 1;
            }
            /*Set vertical drag constraint if no horizontal constraint an dragged to valid y
             * direction */
            if(ext->drag_hor == 0 &&
               ((ext->drag_bottom_en && indev->proc.types.pointer.drag_sum.y <= -EDGUI_INDEV_DEF_DRAG_LIMIT) ||
                (ext->drag_top_en && indev->proc.types.pointer.drag_sum.y >= EDGUI_INDEV_DEF_DRAG_LIMIT))) {
                ext->drag_ver = 1;
            }

#if EDGUI_USE_ANIMATION
            if(ext->drag_hor) {
                ext->page.edge_flash.top_ip    = 0;
                ext->page.edge_flash.bottom_ip = 0;
            }

            if(ext->drag_ver) {
                ext->page.edge_flash.right_ip = 0;
                ext->page.edge_flash.left_ip  = 0;
            }
#endif

            edgui_coord_t x = edgui_obj_get_x(scrl);
            edgui_coord_t y = edgui_obj_get_y(scrl);
            edgui_coord_t h = edgui_obj_get_height(tileview);
            edgui_coord_t w = edgui_obj_get_width(tileview);
            if(ext->drag_top_en == 0) {
                if(y > -(ext->act_id.y * h) && indev->proc.types.pointer.vect.y > 0 && ext->drag_hor == 0) {
#if EDGUI_USE_ANIMATION
                    if(ext->page.edge_flash.enabled && ext->page.edge_flash.left_ip == 0 &&
                       ext->page.edge_flash.right_ip == 0 && ext->page.edge_flash.top_ip == 0 &&
                       ext->page.edge_flash.bottom_ip == 0) {
                        ext->page.edge_flash.top_ip = 1;
                        edgui_page_start_edge_flash(tileview);
                    }
#endif

                    edgui_obj_set_y(scrl, -ext->act_id.y * h + style_bg->body.padding.top);
                }
            }
            if(ext->drag_bottom_en == 0 && indev->proc.types.pointer.vect.y < 0 && ext->drag_hor == 0) {
                if(y < -(ext->act_id.y * h)) {
#if EDGUI_USE_ANIMATION
                    if(ext->page.edge_flash.enabled && ext->page.edge_flash.left_ip == 0 &&
                       ext->page.edge_flash.right_ip == 0 && ext->page.edge_flash.top_ip == 0 &&
                       ext->page.edge_flash.bottom_ip == 0) {
                        ext->page.edge_flash.bottom_ip = 1;
                        edgui_page_start_edge_flash(tileview);
                    }
#endif
                }

                edgui_obj_set_y(scrl, -ext->act_id.y * h + style_bg->body.padding.top);
            }
            if(ext->drag_left_en == 0) {
                if(x > -(ext->act_id.x * w) && indev->proc.types.pointer.vect.x > 0 && ext->drag_ver == 0) {
#if EDGUI_USE_ANIMATION
                    if(ext->page.edge_flash.enabled && ext->page.edge_flash.left_ip == 0 &&
                       ext->page.edge_flash.right_ip == 0 && ext->page.edge_flash.top_ip == 0 &&
                       ext->page.edge_flash.bottom_ip == 0) {
                        ext->page.edge_flash.left_ip = 1;
                        edgui_page_start_edge_flash(tileview);
                    }
#endif

                    edgui_obj_set_x(scrl, -ext->act_id.x * w + style_bg->body.padding.left);
                }
            }
            if(ext->drag_right_en == 0 && indev->proc.types.pointer.vect.x < 0 && ext->drag_ver == 0) {
                if(x < -(ext->act_id.x * w)) {
#if EDGUI_USE_ANIMATION
                    if(ext->page.edge_flash.enabled && ext->page.edge_flash.left_ip == 0 &&
                       ext->page.edge_flash.right_ip == 0 && ext->page.edge_flash.top_ip == 0 &&
                       ext->page.edge_flash.bottom_ip == 0) {
                        ext->page.edge_flash.right_ip = 1;
                        edgui_page_start_edge_flash(tileview);
                    }
#endif
                }

                edgui_obj_set_x(scrl, -ext->act_id.x * w + style_bg->body.padding.top);
            }

            /*Apply the drag constraints*/
            if(ext->drag_ver == 0)
                edgui_obj_set_y(scrl, -ext->act_id.y * edgui_obj_get_height(tileview) + style_bg->body.padding.top);
            if(ext->drag_hor == 0)
                edgui_obj_set_x(scrl, -ext->act_id.x * edgui_obj_get_width(tileview) + style_bg->body.padding.left);
        }
    }
    return res;
}

static void tileview_scrl_event_cb(edgui_obj_t * scrl, edgui_event_t event)
{
    edgui_obj_t * tileview = edgui_obj_get_parent(scrl);

    /*Initialize some variables on PRESS*/
    if(event == EDGUI_EVENT_PRESSED) {
        edgui_tileview_ext_t * ext = edgui_obj_get_ext_attr(tileview);
        ext->drag_hor           = 0;
        ext->drag_ver           = 0;
        set_valid_drag_dirs(tileview);
    }
    /*Animate the tabview to the correct location on RELEASE*/
    else if(event == EDGUI_EVENT_PRESS_LOST || event == EDGUI_EVENT_RELEASED) {
        /* If the element was dragged and it moved the tileview finish the drag manually to
         * let the tileview to finish the move.*/
        edgui_indev_t * indev      = edgui_indev_get_act();
        edgui_tileview_ext_t * ext = edgui_obj_get_ext_attr(tileview);
        if(edgui_indev_is_dragging(indev) && (ext->drag_hor || ext->drag_ver)) {
            indev->proc.types.pointer.drag_in_prog = 0;
        }

        drag_end_handler(tileview);
    }
}

/**
 * Called when the user releases an element of the tileview after dragging it.
 * @param tileview pointer to a tileview object
 */
static void drag_end_handler(edgui_obj_t * tileview)
{
    edgui_tileview_ext_t * ext = edgui_obj_get_ext_attr(tileview);
    edgui_indev_t * indev      = edgui_indev_get_act();
    edgui_point_t point_act;
    edgui_indev_get_point(indev, &point_act);
    edgui_obj_t * scrl = edgui_page_get_scrl(tileview);
    edgui_point_t p;

    p.x = -(scrl->coords.x1 - edgui_obj_get_width(tileview) / 2);
    p.y = -(scrl->coords.y1 - edgui_obj_get_height(tileview) / 2);

    /*From the drag vector (drag throw) predict the end position*/
    if(ext->drag_hor) {
        edgui_point_t vect;
        edgui_indev_get_vect(indev, &vect);
        edgui_coord_t predict = 0;

        while(vect.x != 0) {
            predict += vect.x;
            vect.x = vect.x * (100 - EDGUI_INDEV_DEF_DRAG_THROW) / 100;
        }

        p.x -= predict;
    } else if(ext->drag_ver) {
        edgui_point_t vect;
        edgui_indev_get_vect(indev, &vect);
        edgui_coord_t predict = 0;

        while(vect.y != 0) {
            predict += vect.y;
            vect.y = vect.y * (100 - EDGUI_INDEV_DEF_DRAG_THROW) / 100;
        }

        p.y -= predict;
    }

    /*Get the index of the tile*/
    p.x = p.x / edgui_obj_get_width(tileview);
    p.y = p.y / edgui_obj_get_height(tileview);

    /*Max +- move*/
    edgui_coord_t x_move = p.x - ext->act_id.x;
    edgui_coord_t y_move = p.y - ext->act_id.y;
    if(x_move < -1) x_move = -1;
    if(x_move > 1) x_move = 1;
    if(y_move < -1) y_move = -1;
    if(y_move > 1) y_move = 1;

    /*Set the new tile*/
    edgui_tileview_set_tile_act(tileview, ext->act_id.x + x_move, ext->act_id.y + y_move, true);
}

static bool set_valid_drag_dirs(edgui_obj_t * tileview)
{

    edgui_tileview_ext_t * ext = edgui_obj_get_ext_attr(tileview);
    if(ext->valid_pos == NULL) return false;

    ext->drag_bottom_en = 0;
    ext->drag_top_en    = 0;
    ext->drag_left_en   = 0;
    ext->drag_right_en  = 0;

    uint16_t i;
    for(i = 0; i < ext->valid_pos_cnt; i++) {
        if(ext->valid_pos[i].x == ext->act_id.x && ext->valid_pos[i].y == ext->act_id.y - 1) ext->drag_top_en = 1;
        if(ext->valid_pos[i].x == ext->act_id.x && ext->valid_pos[i].y == ext->act_id.y + 1) ext->drag_bottom_en = 1;
        if(ext->valid_pos[i].x == ext->act_id.x - 1 && ext->valid_pos[i].y == ext->act_id.y) ext->drag_left_en = 1;
        if(ext->valid_pos[i].x == ext->act_id.x + 1 && ext->valid_pos[i].y == ext->act_id.y) ext->drag_right_en = 1;
    }

    return true;
}

#endif
