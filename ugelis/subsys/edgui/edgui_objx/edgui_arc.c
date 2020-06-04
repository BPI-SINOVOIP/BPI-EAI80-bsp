/**
 * @file edgui_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_arc.h"
#if EDGUI_USE_ARC != 0

#include "../edgui_misc/edgui_math.h"
#include "../edgui_draw/edgui_draw_arc.h"
#include "../edgui_themes/edgui_theme.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_arc_design(edgui_obj_t * arc, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_arc_signal(edgui_obj_t * arc, edgui_signal_t sign, void * param);

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
 * Create a arc object
 * @param par pointer to an object, it will be the parent of the new arc
 * @param copy pointer to a arc object, if not NULL then the new object will be copied from it
 * @return pointer to the created arc
 */
edgui_obj_t * edgui_arc_create(edgui_obj_t * par, const edgui_obj_t * copy)
{

    EDGUI_LOG_TRACE("arc create started");

    /*Create the ancestor of arc*/
    edgui_obj_t * new_arc = edgui_obj_create(par, copy);
    edgui_mem_assert(new_arc);
    if(new_arc == NULL) return NULL;

    /*Allocate the arc type specific extended data*/
    edgui_arc_ext_t * ext = edgui_obj_allocate_ext_attr(new_arc, sizeof(edgui_arc_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_arc);
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_arc);

    /*Initialize the allocated 'ext' */
    ext->angle_start = 45;
    ext->angle_end   = 315;

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_arc, edgui_arc_signal);
    edgui_obj_set_design_cb(new_arc, edgui_arc_design);

    /*Init the new arc arc*/
    if(copy == NULL) {
        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_arc_set_style(new_arc, EDGUI_ARC_STYLE_MAIN, th->style.arc);
        } else {
            edgui_arc_set_style(new_arc, EDGUI_ARC_STYLE_MAIN, &edgui_style_plain_color);
        }

    }
    /*Copy an existing arc*/
    else {
        edgui_arc_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->angle_start        = copy_ext->angle_start;
        ext->angle_end          = copy_ext->angle_end;

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_arc);
    }

    EDGUI_LOG_INFO("arc created");

    return new_arc;
}

/*======================
 * Add/remove functions
 *=====================*/

/*
 * New object specific "add" or "remove" functions come here
 */

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the start and end angles of an arc. 0 deg: bottom, 90 deg: right etc.
 * @param arc pointer to an arc object
 * @param start the start angle [0..360]
 * @param end the end angle [0..360]
 */
void edgui_arc_set_angles(edgui_obj_t * arc, uint16_t start, uint16_t end)
{
    edgui_arc_ext_t * ext = edgui_obj_get_ext_attr(arc);

    if(start > 360) start = 360;
    if(end > 360) end = 360;

    ext->angle_start = start;
    ext->angle_end   = end;

    edgui_obj_invalidate(arc);
}

/**
 * Set a style of a arc.
 * @param arc pointer to arc object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void edgui_arc_set_style(edgui_obj_t * arc, edgui_arc_style_t type, const edgui_style_t * style)
{
    switch(type) {
        case EDGUI_ARC_STYLE_MAIN: edgui_obj_set_style(arc, style); break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the start angle of an arc.
 * @param arc pointer to an arc object
 * @return the start angle [0..360]
 */
uint16_t edgui_arc_get_angle_start(edgui_obj_t * arc)
{
    edgui_arc_ext_t * ext = edgui_obj_get_ext_attr(arc);

    return ext->angle_start;
}

/**
 * Get the end angle of an arc.
 * @param arc pointer to an arc object
 * @return the end angle [0..360]
 */
uint16_t edgui_arc_get_angle_end(edgui_obj_t * arc)
{
    edgui_arc_ext_t * ext = edgui_obj_get_ext_attr(arc);

    return ext->angle_end;
}

/**
 * Get style of a arc.
 * @param arc pointer to arc object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const edgui_style_t * edgui_arc_get_style(const edgui_obj_t * arc, edgui_arc_style_t type)
{
    const edgui_style_t * style = NULL;

    switch(type) {
        case EDGUI_ARC_STYLE_MAIN: style = edgui_obj_get_style(arc); break;
        default: style = NULL; break;
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
 * Handle the drawing related tasks of the arcs
 * @param arc pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_arc_design(edgui_obj_t * arc, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        edgui_arc_ext_t * ext       = edgui_obj_get_ext_attr(arc);
        const edgui_style_t * style = edgui_arc_get_style(arc, EDGUI_ARC_STYLE_MAIN);

        edgui_coord_t r       = (EDGUI_MATH_MIN(edgui_obj_get_width(arc), edgui_obj_get_height(arc))) / 2;
        edgui_coord_t x       = arc->coords.x1 + edgui_obj_get_width(arc) / 2;
        edgui_coord_t y       = arc->coords.y1 + edgui_obj_get_height(arc) / 2;
        edgui_opa_t opa_scale = edgui_obj_get_opa_scale(arc);
        edgui_draw_arc(x, y, r, mask, ext->angle_start, ext->angle_end, style, opa_scale);

        /*Draw circle on the ends if enabled */
        if(style->line.rounded) {
            edgui_coord_t thick_half = style->line.width / 2;
            edgui_coord_t cir_x      = ((r - thick_half) * edgui_trigo_sin(ext->angle_start) >> EDGUI_TRIGO_SHIFT);
            edgui_coord_t cir_y      = ((r - thick_half) * edgui_trigo_sin(ext->angle_start + 90) >> EDGUI_TRIGO_SHIFT);

            edgui_style_t cir_style;
            edgui_style_copy(&cir_style, &edgui_style_plain);
            cir_style.body.grad_color = style->line.color;
            cir_style.body.main_color = cir_style.body.grad_color;
            cir_style.body.radius     = EDGUI_RADIUS_CIRCLE;
            edgui_area_t cir_area;
            cir_area.x1 = cir_x + x - thick_half;
            cir_area.y1 = cir_y + y - thick_half;
            cir_area.x2 = cir_x + x + thick_half;
            cir_area.y2 = cir_y + y + thick_half;

            edgui_draw_rect(&cir_area, mask, &cir_style, opa_scale);

            cir_x = ((r - thick_half) * edgui_trigo_sin(ext->angle_end) >> EDGUI_TRIGO_SHIFT);
            cir_y = ((r - thick_half) * edgui_trigo_sin(ext->angle_end + 90) >> EDGUI_TRIGO_SHIFT);

            cir_area.x1 = cir_x + x - thick_half;
            cir_area.y1 = cir_y + y - thick_half;
            cir_area.x2 = cir_x + x + thick_half;
            cir_area.y2 = cir_y + y + thick_half;

            edgui_draw_rect(&cir_area, mask, &cir_style, opa_scale);
        }

    }
    /*Post draw when the children are drawn*/
    else if(mode == EDGUI_DESIGN_DRAW_POST) {
    }

    return true;
}

/**
 * Signal function of the arc
 * @param arc pointer to a arc object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_arc_signal(edgui_obj_t * arc, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(arc, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_arc";
    }

    return res;
}

#endif
