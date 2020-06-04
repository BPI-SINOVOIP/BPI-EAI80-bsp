/**
 * @file edgui_test_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_img.h"

#if EDGUI_USE_IMG && EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/
EDGUI_IMG_DECLARE(img_flower_icon)

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
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create images to test their functionalities
 */
void edgui_test_img_1(void)
{
    /*Create an image object from a varibale*/
    edgui_obj_t * img1 = edgui_img_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_img_set_src(img1, &img_flower_icon);
    edgui_obj_set_pos(img1, 10, 10);

    /*Copy the previous image and set a redish style*/
    static edgui_style_t style;
    edgui_style_copy(&style, &edgui_style_plain);
    style.image.color = EDGUI_COLOR_RED;
    style.image.intense = EDGUI_OPA_70;

    edgui_obj_t * img2 = edgui_img_create(edgui_disp_get_scr_act(NULL), img1);
    edgui_img_set_style(img2, EDGUI_IMG_STYLE_MAIN, &style);
    edgui_obj_align(img2, img1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Copy the previous image and test the mosaic feature*/
    edgui_obj_t * img3 = edgui_img_create(edgui_disp_get_scr_act(NULL), img2);
    edgui_obj_set_size(img3, 100, 100);
    edgui_obj_align(img3, img2, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Test symbol drawing*/
    edgui_obj_t * img4 = edgui_img_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_img_set_src(img4, EDGUI_SYMBOL_SETTINGS EDGUI_SYMBOL_OK);
    edgui_obj_align(img4, img3, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_IMG && EDGUI_USE_TESTS*/
