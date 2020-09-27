/**
 * @file edgui_draw_triangle.h
 *
 */

#ifndef EDGUI_DRAW_TRIANGLE_H
#define EDGUI_DRAW_TRIANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "edgui_draw.h"

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
 *
 * @param points pointer to an array with 3 points
 * @param mask the triangle will be drawn only in this mask
 * @param style style for of the triangle
 * @param opa_scale scale down all opacities by the factor (0..255)
 */
void edgui_draw_triangle(const edgui_point_t * points, const edgui_area_t * mask, const edgui_style_t * style, edgui_opa_t opa_scale);

/**
 * Draw a polygon from triangles. Only convex polygons are supported
 * @param points an array of points
 * @param point_cnt number of points
 * @param mask polygon will be drawn only in this mask
 * @param style style of the polygon
 * @param opa_scale scale down all opacities by the factor (0..255)
 */
void edgui_draw_polygon(const edgui_point_t * points, uint32_t point_cnt, const edgui_area_t * mask, const edgui_style_t * style,
                     edgui_opa_t opa_scale);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_DRAW_TRIANGLE_H*/
