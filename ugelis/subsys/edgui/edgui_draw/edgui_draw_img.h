/**
 * @file edgui_draw_img.h
 *
 */

#ifndef EDGUI_DRAW_IMG_H
#define EDGUI_DRAW_IMG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "edgui_draw.h"
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
 * Draw an image
 * @param coords the coordinates of the image
 * @param mask the image will be drawn only in this area
 * @param src pointer to a edgui_color_t array which contains the pixels of the image
 * @param style style of the image
 * @param opa_scale scale down all opacities by the factor
 */
void edgui_draw_img(const edgui_area_t * coords, const edgui_area_t * mask, const void * src, const edgui_style_t * style,
                 edgui_opa_t opa_scale);

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'edgui_img_t' variable (image stored internally and compiled into the code)
 *  - a path to a file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. EDGUI_SYMBOL_CLOSE)
 * @return type of the image source EDGUI_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKNOWN
 */
edgui_img_src_t edgui_img_src_get_type(const void * src);

/**
 * Get the color of an image's pixel
 * @param dsc an image descriptor
 * @param x x coordinate of the point to get
 * @param y x coordinate of the point to get
 * @param style style of the image. In case of `EDGUI_IMG_CF_ALPHA_1/2/4/8` `style->image.color` shows
 * the color. Can be `NULL` but for `ALPHA` images black will be returned. In other cases it is not
 * used.
 * @return color of the point
 */
edgui_color_t edgui_img_buf_get_px_color(edgui_img_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y, const edgui_style_t * style);
/**
 * Get the alpha value of an image's pixel
 * @param dsc pointer to an image descriptor
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @return alpha value of the point
 */
edgui_opa_t edgui_img_buf_get_px_alpha(edgui_img_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y);

/**
 * Set the color of a pixel of an image. The alpha channel won't be affected.
 * @param dsc pointer to an image descriptor
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @param c color of the point
 */
void edgui_img_buf_set_px_color(edgui_img_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y, edgui_color_t c);

/**
 * Set the alpha value of a pixel of an image. The color won't be affected
 * @param dsc pointer to an image descriptor
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @param opa the desired opacity
 */
void edgui_img_buf_set_px_alpha(edgui_img_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y, edgui_opa_t opa);

/**
 * Set the palette color of an indexed image. Valid only for `EDGUI_IMG_CF_INDEXED1/2/4/8`
 * @param dsc pointer to an image descriptor
 * @param id the palette color to set:
 *   - for `EDGUI_IMG_CF_INDEXED1`: 0..1
 *   - for `EDGUI_IMG_CF_INDEXED2`: 0..3
 *   - for `EDGUI_IMG_CF_INDEXED4`: 0..15
 *   - for `EDGUI_IMG_CF_INDEXED8`: 0..255
 * @param c the color to set
 */
void edgui_img_buf_set_palette(edgui_img_dsc_t * dsc, uint8_t id, edgui_color_t c);

/**
 * Get the pixel size of a color format in bits
 * @param cf a color format (`EDGUI_IMG_CF_...`)
 * @return the pixel size in bits
 */
uint8_t edgui_img_color_format_get_px_size(edgui_img_cf_t cf);

/**
 * Check if a color format is chroma keyed or not
 * @param cf a color format (`EDGUI_IMG_CF_...`)
 * @return true: chroma keyed; false: not chroma keyed
 */
bool edgui_img_color_format_is_chroma_keyed(edgui_img_cf_t cf);

/**
 * Check if a color format has alpha channel or not
 * @param cf a color format (`EDGUI_IMG_CF_...`)
 * @return true: has alpha channel; false: doesn't have alpha channel
 */
bool edgui_img_color_format_has_alpha(edgui_img_cf_t cf);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEMPL_H*/
