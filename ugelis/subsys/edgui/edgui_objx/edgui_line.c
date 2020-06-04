/**
 * @file edgui_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_line.h"

#if EDGUI_USE_LINE != 0
#include "../edgui_draw/edgui_draw.h"
#include "../edgui_misc/edgui_math.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_line_design(edgui_obj_t * line, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_line_signal(edgui_obj_t * line, edgui_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a line objects
 * @param par pointer to an object, it will be the parent of the new line
 * @return pointer to the created line
 */
edgui_obj_t * edgui_line_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("line create started");

    /*Create a basic object*/
    edgui_obj_t * new_line = edgui_obj_create(par, copy);
    edgui_mem_assert(new_line);
    if(new_line == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_line);

    /*Extend the basic object to line object*/
    edgui_line_ext_t * ext = edgui_obj_allocate_ext_attr(new_line, sizeof(edgui_line_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->point_num   = 0;
    ext->point_array = NULL;
    ext->auto_size   = 1;
    ext->y_inv       = 0;

    edgui_obj_set_design_cb(new_line, edgui_line_design);
    edgui_obj_set_signal_cb(new_line, edgui_line_signal);

    /*Init the new line*/
    if(copy == NULL) {
        edgui_obj_set_size(new_line, EDGUI_DPI,
                        EDGUI_DPI);          /*Auto size is enables, but set default size until no points are added*/
        edgui_obj_set_style(new_line, NULL); /*Inherit parent's style*/
        edgui_obj_set_click(new_line, false);
    }
    /*Copy an existing object*/
    else {
        edgui_line_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        edgui_line_set_auto_size(new_line, edgui_line_get_auto_size(copy));
        edgui_line_set_y_invert(new_line, edgui_line_get_y_invert(copy));
        edgui_line_set_auto_size(new_line, edgui_line_get_auto_size(copy));
        edgui_line_set_points(new_line, copy_ext->point_array, copy_ext->point_num);
        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_line);
    }

    EDGUI_LOG_INFO("line created");

    return new_line;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set an array of points. The line object will connect these points.
 * @param line pointer to a line object
 * @param point_a an array of points. Only the address is saved,
 * so the array can NOT be a local variable which will be destroyed
 * @param point_num number of points in 'point_a'
 */
void edgui_line_set_points(edgui_obj_t * line, const edgui_point_t point_a[], uint16_t point_num)
{
    edgui_line_ext_t * ext = edgui_obj_get_ext_attr(line);
    ext->point_array    = point_a;
    ext->point_num      = point_num;

    if(point_num > 0 && ext->auto_size != 0) {
        uint16_t i;
        edgui_coord_t xmax = EDGUI_COORD_MIN;
        edgui_coord_t ymax = EDGUI_COORD_MIN;
        for(i = 0; i < point_num; i++) {
            xmax = EDGUI_MATH_MAX(point_a[i].x, xmax);
            ymax = EDGUI_MATH_MAX(point_a[i].y, ymax);
        }

        const edgui_style_t * style = edgui_line_get_style(line, EDGUI_LINE_STYLE_MAIN);
        edgui_obj_set_size(line, xmax + style->line.width, ymax + style->line.width);
    }

    edgui_obj_invalidate(line);
}

/**
 * Enable (or disable) the auto-size option. The size of the object will fit to its points.
 * (set width to x max and height to y max)
 * @param line pointer to a line object
 * @param en true: auto size is enabled, false: auto size is disabled
 */
void edgui_line_set_auto_size(edgui_obj_t * line, bool en)
{
    edgui_line_ext_t * ext = edgui_obj_get_ext_attr(line);
    if(ext->auto_size == en) return;

    ext->auto_size = en == false ? 0 : 1;

    /*Refresh the object*/
    if(en) edgui_line_set_points(line, ext->point_array, ext->point_num);
}

/**
 * Enable (or disable) the y coordinate inversion.
 * If enabled then y will be subtracted from the height of the object,
 * therefore the y=0 coordinate will be on the bottom.
 * @param line pointer to a line object
 * @param en true: enable the y inversion, false:disable the y inversion
 */
void edgui_line_set_y_invert(edgui_obj_t * line, bool en)
{
    edgui_line_ext_t * ext = edgui_obj_get_ext_attr(line);
    if(ext->y_inv == en) return;

    ext->y_inv = en == false ? 0 : 1;

    edgui_obj_invalidate(line);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the auto size attribute
 * @param line pointer to a line object
 * @return true: auto size is enabled, false: disabled
 */
bool edgui_line_get_auto_size(const edgui_obj_t * line)
{
    edgui_line_ext_t * ext = edgui_obj_get_ext_attr(line);

    return ext->auto_size == 0 ? false : true;
}

/**
 * Get the y inversion attribute
 * @param line pointer to a line object
 * @return true: y inversion is enabled, false: disabled
 */
bool edgui_line_get_y_invert(const edgui_obj_t * line)
{
    edgui_line_ext_t * ext = edgui_obj_get_ext_attr(line);

    return ext->y_inv == 0 ? false : true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the lines
 * @param line pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_line_design(edgui_obj_t * line, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    /*A line never covers an area*/
    if(mode == EDGUI_DESIGN_COVER_CHK)
        return false;
    else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        edgui_line_ext_t * ext = edgui_obj_get_ext_attr(line);

        if(ext->point_num == 0 || ext->point_array == NULL) return false;

        const edgui_style_t * style = edgui_obj_get_style(line);
        edgui_opa_t opa_scale       = edgui_obj_get_opa_scale(line);
        edgui_area_t area;
        edgui_obj_get_coords(line, &area);
        edgui_coord_t x_ofs = area.x1;
        edgui_coord_t y_ofs = area.y1;
        edgui_point_t p1;
        edgui_point_t p2;
        edgui_coord_t h = edgui_obj_get_height(line);
        uint16_t i;

        edgui_style_t circle_style_tmp; /*If rounded...*/
        edgui_style_copy(&circle_style_tmp, style);
        circle_style_tmp.body.radius     = EDGUI_RADIUS_CIRCLE;
        circle_style_tmp.body.main_color = style->line.color;
        circle_style_tmp.body.grad_color = style->line.color;
        circle_style_tmp.body.opa        = style->line.opa;
        edgui_area_t circle_area;

        /*Read all points and draw the lines*/
        for(i = 0; i < ext->point_num - 1; i++) {

            p1.x = ext->point_array[i].x + x_ofs;
            p2.x = ext->point_array[i + 1].x + x_ofs;

            if(ext->y_inv == 0) {
                p1.y = ext->point_array[i].y + y_ofs;
                p2.y = ext->point_array[i + 1].y + y_ofs;
            } else {
                p1.y = h - ext->point_array[i].y + y_ofs;
                p2.y = h - ext->point_array[i + 1].y + y_ofs;
            }
            edgui_draw_line(&p1, &p2, mask, style, opa_scale);

            /*Draw circle on the joints if enabled*/
            if(style->line.rounded) {
                circle_area.x1 = p1.x - ((style->line.width - 1) >> 1) - ((style->line.width - 1) & 0x1);
                circle_area.y1 = p1.y - ((style->line.width - 1) >> 1) - ((style->line.width - 1) & 0x1);
                circle_area.x2 = p1.x + ((style->line.width - 1) >> 1);
                circle_area.y2 = p1.y + ((style->line.width - 1) >> 1);
                edgui_draw_rect(&circle_area, mask, &circle_style_tmp, opa_scale);
            }
        }

        /*Draw circle on the last point too if enabled*/
        if(style->line.rounded) {
            circle_area.x1 = p2.x - ((style->line.width - 1) >> 1) - ((style->line.width - 1) & 0x1);
            circle_area.y1 = p2.y - ((style->line.width - 1) >> 1) - ((style->line.width - 1) & 0x1);
            circle_area.x2 = p2.x + ((style->line.width - 1) >> 1);
            circle_area.y2 = p2.y + ((style->line.width - 1) >> 1);
            edgui_draw_rect(&circle_area, mask, &circle_style_tmp, opa_scale);
        }
    }
    return true;
}

/**
 * Signal function of the line
 * @param line pointer to a line object
 * @param sign a signal type from edgui_signal_t enum
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_line_signal(edgui_obj_t * line, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(line, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_line";
    } else if(sign == EDGUI_SIGNAL_REFR_EXT_DRAW_PAD) {
        const edgui_style_t * style = edgui_line_get_style(line, EDGUI_LINE_STYLE_MAIN);
        if(line->ext_draw_pad < style->line.width) line->ext_draw_pad = style->line.width;
    }

    return res;
}
#endif
