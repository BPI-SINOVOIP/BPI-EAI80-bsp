/**
 * @file edgui_tutorial_antialiasing.h
 *
 */

/*
 * --------------------------------------------------------------
 * Learn how to make the drawings smoother with anti aliasing
 * --------------------------------------------------------------
 *
 * You have several options to make your GUI smoother:
 *
 * 1. ANTI-ALIASED DRAWING
 *  By setting EDGUI_ANTIALAIS  1 in edgui_conf.h the library will draw
 *  smooth lines and curves (radius of rectangles). It has very low
 *  resource requirement because only the required pixels are calculated
 *  on the edges.
 *
 * 2. HIGHER BPP FONTS
 *  You enable can built-in fonts in edgui_conf.h.
 *  By setting for example EDGUI_FONT_DEJAVU_20  4 the font will describe one pixel
 *  with 4 bits meaning 16 values for one pixel. It will result smoother letters.
 *  The possible values are 1, 2, 4 or 8.  Not that the size of the font is increasing
 *  by increasing the bpp.
 *  With the font converter tool you can also create your own fonts with the desired bpp:
 *  https://littlevgl.com/ttf-font-to-c-array
 *
 * 3. PIXEL LEVEL OPACITY ON IMAGES
 *  In the font converter you can enable 'Transparency: Alpha byte' which
 *  will add an alpha byte the every pixel. It ensures smooth edges and holes on images.
 *  Check the Image converter here: https://littlevgl.com/image-to-c-array
 *
 * Try the example by changing the following settings in edgui_conf.h:
 * - EDGUI_ANTIALAIS 0 or 1
 * - EDGUI_FONT_DEJAVU_...     1 or 2 or 4 or 8
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_tutorial_antialiasing.h"
#if EDGUI_USE_TUTORIALS

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
EDGUI_IMG_DECLARE(apple_icon_chroma)
EDGUI_IMG_DECLARE(apple_icon_alpha)

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create object to see how they change from the anti aliasing
 */
void edgui_tutorial_antialiasing(void)
{
    edgui_obj_t * scr = edgui_disp_get_scr_act(NULL);     /*Get the current screen*/

    edgui_obj_t * label;

    static edgui_style_t style1;
    edgui_style_copy(&style1, &edgui_style_btn_rel);
    style1.body.radius = 20;
    style1.body.border.width = 4;

    edgui_obj_t * btn1;
    btn1 = edgui_btn_create(scr, NULL);
    edgui_obj_set_pos(btn1, 10, 10);
    edgui_obj_set_size(btn1, 100, 60);
    edgui_btn_set_style(btn1, EDGUI_BTN_STYLE_REL, &style1);

    label = edgui_label_create(btn1, NULL);
    edgui_label_set_text(label, "Button");

    /*Crate an image which is NOT automatically upscaled to compensate the anti aliasing*/
    edgui_obj_t * img_normal = edgui_img_create(scr, NULL);
    edgui_img_set_src(img_normal, &apple_icon_chroma);
    edgui_obj_align(img_normal, btn1, EDGUI_ALIGN_OUT_RIGHT_TOP, 10, 0);

    /*Crate an image which is automatically upscaled to compensate the anti aliasing*/
    edgui_obj_t * img_alpha_byte = edgui_img_create(scr, img_normal); /*Crate an image object*/
    edgui_img_set_src(img_alpha_byte, &apple_icon_alpha);
    edgui_obj_align(img_alpha_byte, img_normal, EDGUI_ALIGN_OUT_RIGHT_TOP, 10, 0);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/
