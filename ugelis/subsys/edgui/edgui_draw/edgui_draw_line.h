/**
 * @file edgui_draw_line.h
 *
 */

#ifndef EDGUI_DRAW_LINE_H
#define EDGUI_DRAW_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

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
 * Draw a line
 * @param point1 first point of the line
 * @param point2 second point of the line
 * @param mask the line will be drawn only on this area
 * @param style pointer to a line's style
 * @param opa_scale scale down all opacities by the factor
 */
void edgui_draw_line(const edgui_point_t * point1, const edgui_point_t * point2, const edgui_area_t * mask,
                  const edgui_style_t * style, edgui_opa_t opa_scale);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_DRAW_LINE_H*/
