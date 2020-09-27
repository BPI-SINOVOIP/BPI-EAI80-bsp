/**
 * @file edgui_tutorial_fonts.h
 *
 */

/*
 * --------------------------------------------------------------
 * How to add new fonts (even Unicode) to the graphic library
 * --------------------------------------------------------------
 *
 * BUILT-IN FONTS
 * - There are several built in font in the library which can be enabled in edgui_conf.h
 * - For example: edgui_font_roboto_16,

 * USING A FONT
 * - Just set a font in style.text.font
 * - For example: style.text.font = &edgui_font_roboto_16
 *
 * SYMBOLS
 * - There are symbols as well which stored as fonts.
 * - To reference a symbol use the defines EDGUI_SYMBOL_... (EDGUI_SYMBOL_FILE, EDGUI_SYMBOL_OK, etc see edgui_symbol_def.h)
 *
 * BIT-PER-PIXEL
 * - The fonts can describe a pixel with 1, 2 or 4 bit. The higher value results smoother letters
 * but larger foot memory foot print.
 *
 * ADDING NEW FONTS
 *  - You can generate your own fonts using the online font converter tool:
 *    https://littlevgl.com/ttf-font-to-c-array
 *  - All information is provided on the website
 *
 */



/*********************
 *      INCLUDES
 *********************/
#include "edgui_tutorial_fonts.h"
#if EDGUI_USE_TUTORIALS

#include "edgui/edgui.h"

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
EDGUI_FONT_DECLARE(arial_20)        /*Declare a font*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Concat two font and create label with Unicode characters
 */
void edgui_tutorial_fonts(void)
{
    /*Create a style and use the new font*/
    static edgui_style_t style1;
    edgui_style_copy(&style1, &edgui_style_plain);
    style1.text.font = &arial_20; /*Set the base font whcih is concatenated with the others*/

    /*Create a label and set new text*/
    edgui_obj_t * label = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(label, 10, 10);
    edgui_label_set_style(label, EDGUI_LABEL_STYLE_MAIN, &style1);
    edgui_label_set_text(label, "Hello\nпривет\n∞∑");      /*Use ASCII and Cyrillic letters together*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/
