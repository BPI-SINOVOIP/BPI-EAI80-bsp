/**
 * @file edgui_draw.h
 *
 */

#ifndef EDGUI_DRAW_H
#define EDGUI_DRAW_H

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

#include "../edgui_core/edgui_style.h"
#include "../edgui_misc/edgui_txt.h"
#include "edgui_img_decoder.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Give a buffer with the given to use during drawing.
 * Be careful to not use the buffer while other processes are using it.
 * @param size the required size
 */
void * edgui_draw_get_buf(uint32_t size);

/**
 * Free the draw buffer
 */
void edgui_draw_free_buf(void);

#if EDGUI_ANTIALIAS

/**
 * Get the opacity of a pixel based it's position in a line segment
 * @param seg segment length
 * @param px_id position of  of a pixel which opacity should be get [0..seg-1]
 * @param base_opa the base opacity
 * @return the opacity of the given pixel
 */
edgui_opa_t edgui_draw_aa_get_opa(edgui_coord_t seg, edgui_coord_t px_id, edgui_opa_t base_opa);

/**
 * Add a vertical  anti-aliasing segment (pixels with decreasing opacity)
 * @param x start point x coordinate
 * @param y start point y coordinate
 * @param length length of segment (negative value to start from 0 opacity)
 * @param mask draw only in this area
 * @param color color of pixels
 * @param opa maximum opacity
 */
void edgui_draw_aa_ver_seg(edgui_coord_t x, edgui_coord_t y, edgui_coord_t length, const edgui_area_t * mask, edgui_color_t color,
                        edgui_opa_t opa);

/**
 * Add a horizontal anti-aliasing segment (pixels with decreasing opacity)
 * @param x start point x coordinate
 * @param y start point y coordinate
 * @param length length of segment (negative value to start from 0 opacity)
 * @param mask draw only in this area
 * @param color color of pixels
 * @param opa maximum opacity
 */
void edgui_draw_aa_hor_seg(edgui_coord_t x, edgui_coord_t y, edgui_coord_t length, const edgui_area_t * mask, edgui_color_t color,
                        edgui_opa_t opa);
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 *********************/
#include "edgui_draw_basic.h"
#include "edgui_draw_rect.h"
#include "edgui_draw_label.h"
#include "edgui_draw_img.h"
#include "edgui_draw_line.h"
#include "edgui_draw_triangle.h"
#include "edgui_draw_arc.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_DRAW_H*/
