/**
 * @file edgui_font.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "edgui_font.h"
#include "../edgui_misc/edgui_utils.h"
#include "../edgui_misc/edgui_log.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Return with the bitmap of a font.
 * @param font_p pointer to a font
 * @param letter an UNICODE character code
 * @return  pointer to the bitmap of the letter
 */
const uint8_t * edgui_font_get_glyph_bitmap(const edgui_font_t * font_p, uint32_t letter)
{
    return font_p->get_glyph_bitmap(font_p, letter);
}

/**
 * Get the descriptor of a glyph
 * @param font_p pointer to font
 * @param dsc_out store the result descriptor here
 * @param letter an UNICODE letter code
 * @return true: descriptor is successfully loaded into `dsc_out`.
 *         false: the letter was not found, no data is loaded to `dsc_out`
 */
bool edgui_font_get_glyph_dsc(const edgui_font_t * font_p, edgui_font_glyph_dsc_t * dsc_out, uint32_t letter, uint32_t letter_next)
{
    return font_p->get_glyph_dsc(font_p, dsc_out, letter, letter_next);
}

/**
 * Get the width of a glyph with kerning
 * @param font pointer to a font
 * @param letter an UNICODE letter
 * @param letter_next the next letter after `letter`. Used for kerning
 * @return the width of the glyph
 */
uint16_t edgui_font_get_glyph_width(const edgui_font_t * font, uint32_t letter, uint32_t letter_next)
{
    edgui_font_glyph_dsc_t g;
    bool ret;
    ret = edgui_font_get_glyph_dsc(font, &g, letter, letter_next);
    if(ret) return g.adv_w;
    else return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
