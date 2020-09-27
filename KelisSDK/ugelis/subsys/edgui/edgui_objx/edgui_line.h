/**
 * @file edgui_line.h
 *
 */

#ifndef EDGUI_LINE_H
#define EDGUI_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui_conf.h"
#else
#include "../edgui/edgui_conf.h"
#endif

#if EDGUI_USE_LINE != 0

#include "../edgui_core/edgui_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of line*/
typedef struct
{
    /*Inherited from 'base_obj' so no inherited ext.*/ /*Ext. of ancestor*/
    const edgui_point_t * point_array;                    /*Pointer to an array with the points of the line*/
    uint16_t point_num;                                /*Number of points in 'point_array' */
    uint8_t auto_size : 1;                             /*1: set obj. width to x max and obj. height to y max */
    uint8_t y_inv : 1;                                 /*1: y == 0 will be on the bottom*/
} edgui_line_ext_t;

/*Styles*/
enum {
    EDGUI_LINE_STYLE_MAIN,
};
typedef uint8_t edgui_line_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a line objects
 * @param par pointer to an object, it will be the parent of the new line
 * @return pointer to the created line
 */
edgui_obj_t * edgui_line_create(edgui_obj_t * par, const edgui_obj_t * copy);

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
void edgui_line_set_points(edgui_obj_t * line, const edgui_point_t point_a[], uint16_t point_num);

/**
 * Enable (or disable) the auto-size option. The size of the object will fit to its points.
 * (set width to x max and height to y max)
 * @param line pointer to a line object
 * @param en true: auto size is enabled, false: auto size is disabled
 */
void edgui_line_set_auto_size(edgui_obj_t * line, bool en);

/**
 * Enable (or disable) the y coordinate inversion.
 * If enabled then y will be subtracted from the height of the object,
 * therefore the y=0 coordinate will be on the bottom.
 * @param line pointer to a line object
 * @param en true: enable the y inversion, false:disable the y inversion
 */
void edgui_line_set_y_invert(edgui_obj_t * line, bool en);

#define edgui_line_set_y_inv                                                                                              \
    edgui_line_set_y_invert /*The name was inconsistent. In v.6.0 only `edgui_line_set_y_invert`will                         \
                            work */

/**
 * Set the style of a line
 * @param line pointer to a line object
 * @param type which style should be set (can be only `EDGUI_LINE_STYLE_MAIN`)
 * @param style pointer to a style
 */
static __inline void edgui_line_set_style(edgui_obj_t * line, edgui_line_style_t type, const edgui_style_t * style)
{
    (void)type; /*Unused*/
    edgui_obj_set_style(line, style);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the auto size attribute
 * @param line pointer to a line object
 * @return true: auto size is enabled, false: disabled
 */
bool edgui_line_get_auto_size(const edgui_obj_t * line);

/**
 * Get the y inversion attribute
 * @param line pointer to a line object
 * @return true: y inversion is enabled, false: disabled
 */
bool edgui_line_get_y_invert(const edgui_obj_t * line);

/**
 * Get the style of an line object
 * @param line pointer to an line object
 * @param type which style should be get (can be only `EDGUI_LINE_STYLE_MAIN`)
 * @return pointer to the line's style
 */
static __inline const edgui_style_t * edgui_line_get_style(const edgui_obj_t * line, edgui_line_style_t type)
{
    (void)type; /*Unused*/
    return edgui_obj_get_style(line);
}

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_LINE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_LINE_H*/
