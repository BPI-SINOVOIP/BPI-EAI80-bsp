/**
 * @file edgui_preload.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_preload.h"
#if EDGUI_USE_PRELOAD != 0

#include "../edgui_misc/edgui_math.h"
#include "../edgui_draw/edgui_draw_rect.h"
#include "../edgui_draw/edgui_draw_arc.h"
#include "../edgui_themes/edgui_theme.h"

/*********************
 *      DEFINES
 *********************/
#ifndef EDGUI_PRELOAD_DEF_ARC_LENGTH
#define EDGUI_PRELOAD_DEF_ARC_LENGTH 60 /*[deg]*/
#endif

#ifndef EDGUI_PRELOAD_DEF_SPIN_TIME
#define EDGUI_PRELOAD_DEF_SPIN_TIME 1000 /*[ms]*/
#endif

#ifndef EDGUI_PRELOAD_DEF_ANIM
#define EDGUI_PRELOAD_DEF_ANIM EDGUI_PRELOAD_TYPE_SPINNING_ARC /*animation type*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_preload_design(edgui_obj_t * preload, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_preload_signal(edgui_obj_t * preload, edgui_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;
static edgui_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a pre loader object
 * @param par pointer to an object, it will be the parent of the new pre loader
 * @param copy pointer to a pre loader object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created pre loader
 */
edgui_obj_t * edgui_preload_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("preload create started");

    /*Create the ancestor of pre loader*/
    edgui_obj_t * new_preload = edgui_arc_create(par, copy);
    edgui_mem_assert(new_preload);
    if(new_preload == NULL) return NULL;

    /*Allocate the pre loader type specific extended data*/
    edgui_preload_ext_t * ext = edgui_obj_allocate_ext_attr(new_preload, sizeof(edgui_preload_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_preload);
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_preload);

    /*Initialize the allocated 'ext' */
    ext->arc_length = EDGUI_PRELOAD_DEF_ARC_LENGTH;
    ext->anim_type  = EDGUI_PRELOAD_DEF_ANIM;
    ext->anim_dir   = EDGUI_PRELOAD_DIR_FORWARD;

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_preload, edgui_preload_signal);
    edgui_obj_set_design_cb(new_preload, edgui_preload_design);

    /*Init the new pre loader pre loader*/
    if(copy == NULL) {
        edgui_obj_set_size(new_preload, EDGUI_DPI / 2, EDGUI_DPI / 2);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_preload_set_style(new_preload, EDGUI_PRELOAD_STYLE_MAIN, th->style.preload);
        } else {
            edgui_obj_set_style(new_preload, &edgui_style_pretty_color);
        }

        ext->time = EDGUI_PRELOAD_DEF_SPIN_TIME;

    }
    /*Copy an existing pre loader*/
    else {
        edgui_preload_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->arc_length             = copy_ext->arc_length;
        ext->time                   = copy_ext->time;
        ext->anim_dir               = copy_ext->anim_dir;
        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_preload);
    }

    edgui_preload_set_type(new_preload, ext->anim_type);

    EDGUI_LOG_INFO("preload created");

    return new_preload;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Set the length of the spinning  arc in degrees
 * @param preload pointer to a preload object
 * @param deg length of the arc
 */
void edgui_preload_set_arc_length(edgui_obj_t * preload, edgui_anim_value_t deg)
{
    edgui_preload_ext_t * ext = edgui_obj_get_ext_attr(preload);

    ext->arc_length = deg;
}

/**
 * Set the spin time of the arc
 * @param preload pointer to a preload object
 * @param time time of one round in milliseconds
 */
void edgui_preload_set_spin_time(edgui_obj_t * preload, uint16_t time)
{
    edgui_preload_ext_t * ext = edgui_obj_get_ext_attr(preload);

    ext->time = time;
    edgui_preload_set_type(preload, ext->anim_type);
}
/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void edgui_preload_set_style(edgui_obj_t * preload, edgui_preload_style_t type, const edgui_style_t * style)
{
    switch(type) {
        case EDGUI_PRELOAD_STYLE_MAIN: edgui_arc_set_style(preload, EDGUI_ARC_STYLE_MAIN, style); break;
    }
}

/**
 * Set the animation type of a preloadeer.
 * @param preload pointer to pre loader object
 * @param type animation type of the preload
 *  */
void edgui_preload_set_type(edgui_obj_t * preload, edgui_preload_type_t type)
{
    edgui_preload_ext_t * ext = edgui_obj_get_ext_attr(preload);

    /*delete previous animation*/
    edgui_anim_del(preload, NULL);
    switch(type) {
        case EDGUI_PRELOAD_TYPE_FILLSPIN_ARC: {
            ext->anim_type = EDGUI_PRELOAD_TYPE_FILLSPIN_ARC;
            edgui_anim_t a;
            a.var = preload;
            if(ext->anim_dir == EDGUI_PRELOAD_DIR_FORWARD) {
                /* Clockwise */
                a.start = 360;
                a.end   = 0;
            } else {
                a.start = 0;
                a.end   = 360;
            }
            a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_preload_spinner_anim;
            a.path_cb        = edgui_anim_path_ease_in_out;
            a.ready_cb       = NULL;
            a.act_time       = 0;
            a.time           = ext->time;
            a.playback       = 0;
            a.playback_pause = 0;
            a.repeat         = 1;
            a.repeat_pause   = 0;
            edgui_anim_create(&a);

            edgui_anim_t b;
            b.var = preload;
            if(ext->anim_dir == EDGUI_PRELOAD_DIR_FORWARD) {
                /* Clockwise */
                b.start = 360 - ext->arc_length;
                b.end   = ext->arc_length;
            } else {
                b.start = ext->arc_length;
                b.end   = 360 - ext->arc_length;
            }
            b.exec_cb        = (edgui_anim_exec_xcb_t)edgui_preload_set_arc_length;
            b.path_cb        = edgui_anim_path_ease_in_out;
            b.ready_cb       = NULL;
            b.act_time       = 0;
            b.time           = ext->time;
            b.playback       = 1;
            b.playback_pause = 0;
            b.repeat         = 1;
            b.repeat_pause   = 0;
            edgui_anim_create(&b);
            break;
        }
        case EDGUI_PRELOAD_TYPE_SPINNING_ARC:
        default: {
            ext->anim_type = EDGUI_PRELOAD_TYPE_SPINNING_ARC;
            edgui_anim_t a;
            a.var = preload;
            if(ext->anim_dir == EDGUI_PRELOAD_DIR_FORWARD) {
                /* Clockwise */
                a.start = 360;
                a.end   = 0;
            } else {
                a.start = 0;
                a.end   = 360;
            }
            a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_preload_spinner_anim;
            a.path_cb        = edgui_anim_path_ease_in_out;
            a.ready_cb       = NULL;
            a.act_time       = 0;
            a.time           = ext->time;
            a.playback       = 0;
            a.playback_pause = 0;
            a.repeat         = 1;
            a.repeat_pause   = 0;
            edgui_anim_create(&a);
            break;
        }
    }
}

void edgui_preload_set_dir(edgui_obj_t * preload, edgui_preload_dir_t dir)
{
    edgui_preload_ext_t * ext = edgui_obj_get_ext_attr(preload);

    ext->anim_dir = dir;
    edgui_preload_set_type(preload, ext->anim_type);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the arc length [degree] of the a pre loader
 * @param preload pointer to a pre loader object
 */
edgui_anim_value_t edgui_preload_get_arc_length(const edgui_obj_t * preload)
{
    edgui_preload_ext_t * ext = edgui_obj_get_ext_attr(preload);
    return ext->arc_length;
}

/**
 * Get the spin time of the arc
 * @param preload pointer to a pre loader object [milliseconds]
 */
uint16_t edgui_preload_get_spin_time(const edgui_obj_t * preload)
{
    edgui_preload_ext_t * ext = edgui_obj_get_ext_attr(preload);
    return ext->time;
}

/**
 * Get style of a pre loader.
 * @param preload pointer to pre loader object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const edgui_style_t * edgui_preload_get_style(const edgui_obj_t * preload, edgui_preload_style_t type)
{
    const edgui_style_t * style = NULL;

    switch(type) {
        case EDGUI_PRELOAD_STYLE_MAIN: style = edgui_arc_get_style(preload, EDGUI_ARC_STYLE_MAIN); break;
        default: style = NULL; break;
    }

    return style;
}

/**
 * Get the animation type of a preloadeer.
 * @param preload pointer to pre loader object
 * @return animation type
 *  */
edgui_preload_type_t edgui_preload_get_type(edgui_obj_t * preload)
{
    edgui_preload_ext_t * ext = edgui_obj_get_ext_attr(preload);
    return ext->anim_type;
}

edgui_preload_dir_t edgui_preload_get_dir(edgui_obj_t * preload)
{
    edgui_preload_ext_t * ext = edgui_obj_get_ext_attr(preload);
    return ext->anim_dir;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Animator function  (exec_cb) to rotate the arc of spinner.
 * @param ptr pointer to preloader
 * @param val the current desired value [0..360]
 */
void edgui_preload_spinner_anim(void * ptr, edgui_anim_value_t val)
{
    edgui_obj_t * preload     = ptr;
    edgui_preload_ext_t * ext = edgui_obj_get_ext_attr(preload);

    int16_t angle_start = val - ext->arc_length / 2 + 180;
    int16_t angle_end   = angle_start + ext->arc_length;

    angle_start = angle_start % 360;
    angle_end   = angle_end % 360;

    edgui_arc_set_angles(preload, angle_start, angle_end);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the pre loaders
 * @param preload pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_preload_design(edgui_obj_t * preload, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == EDGUI_DESIGN_DRAW_MAIN) {

        /*Draw a circle as background*/
        const edgui_style_t * style = edgui_arc_get_style(preload, EDGUI_ARC_STYLE_MAIN);
        if(style->body.border.width > 0) {
            edgui_coord_t r = (EDGUI_MATH_MIN(edgui_obj_get_width(preload), edgui_obj_get_height(preload))) / 2;
            r -= EDGUI_MATH_MIN(style->body.padding.left, style->body.padding.top);

            edgui_coord_t x = preload->coords.x1 + edgui_obj_get_width(preload) / 2;
            edgui_coord_t y = preload->coords.y1 + edgui_obj_get_height(preload) / 2;

            edgui_style_t bg_style;
            edgui_style_copy(&bg_style, &edgui_style_plain);
            bg_style.body.opa          = EDGUI_OPA_TRANSP;
            bg_style.body.radius       = EDGUI_RADIUS_CIRCLE;
            bg_style.body.border.color = style->body.border.color;
            bg_style.body.border.width = style->body.border.width;

            edgui_area_t bg_area;
            bg_area.x1 = x - r;
            bg_area.y1 = y - r;
            bg_area.x2 = x + r;
            bg_area.y2 = y + r;

            edgui_draw_rect(&bg_area, mask, &bg_style, edgui_obj_get_opa_scale(preload));
        }
        /*Draw the arc above the background circle */
        ancestor_design(preload, mask, mode);
    }
    /*Post draw when the children are drawn*/
    else if(mode == EDGUI_DESIGN_DRAW_POST) {
    }

    return true;
}

/**
 * Signal function of the pre loader
 * @param preload pointer to a pre loader object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_preload_signal(edgui_obj_t * preload, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(preload, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_preload";
    }

    return res;
}

#endif
