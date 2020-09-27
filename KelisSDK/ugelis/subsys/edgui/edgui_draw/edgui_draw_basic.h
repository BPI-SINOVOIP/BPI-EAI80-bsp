/**
 * @file edgui_draw_basic.h
 *
 */

#ifndef EDGUI_DRAW_BASIC_H
#define EDGUI_DRAW_BASIC_H

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

#include "../edgui_font/edgui_font.h"
#include "../edgui_misc/edgui_color.h"
#include "../edgui_misc/edgui_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void edgui_draw_px(edgui_coord_t x, edgui_coord_t y, const edgui_area_t * mask_p, edgui_color_t color, edgui_opa_t opa);
/**
 * Fill an area in the Virtual Display Buffer
 * @param cords_p coordinates of the area to fill
 * @param mask_p fill only o this mask
 * @param color fill color
 * @param opa opacity of the area (0..255)
 */
void edgui_draw_fill(const edgui_area_t * cords_p, const edgui_area_t * mask_p, edgui_color_t color, edgui_opa_t opa);

/**
 * Draw a letter in the Virtual Display Buffer
 * @param pos_p left-top coordinate of the latter
 * @param mask_p the letter will be drawn only on this area
 * @param font_p pointer to font
 * @param letter a letter to draw
 * @param color color of letter
 * @param opa opacity of letter (0..255)
 */
void edgui_draw_letter(const edgui_point_t * pos_p, const edgui_area_t * mask_p, const edgui_font_t * font_p, uint32_t letter,
                    edgui_color_t color, edgui_opa_t opa);

/**
 * Draw a color map to the display (image)
 * @param cords_p coordinates the color map
 * @param mask_p the map will drawn only on this area  (truncated to VDB area)
 * @param map_p pointer to a edgui_color_t array
 * @param opa opacity of the map
 * @param chroma_keyed true: enable transparency of EDGUI_IMG_EDGUI_COLOR_TRANSP color pixels
 * @param alpha_byte true: extra alpha byte is inserted for every pixel
 * @param recolor mix the pixels with this color
 * @param recolor_opa the intense of recoloring
 */
void edgui_draw_map(const edgui_area_t * cords_p, const edgui_area_t * mask_p, const uint8_t * map_p, edgui_opa_t opa,
                 bool chroma_key, bool alpha_byte, edgui_color_t recolor, edgui_opa_t recolor_opa);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_DRAW_BASIC_H*/
