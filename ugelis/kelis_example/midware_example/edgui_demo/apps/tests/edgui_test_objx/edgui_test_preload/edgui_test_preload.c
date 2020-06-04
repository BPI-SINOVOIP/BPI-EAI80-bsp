/**
 * @file edgui_test_preload.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_preload.h"
#if EDGUI_USE_PRELOAD && EDGUI_USE_TESTS

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
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create preloads to test their functionalities
 */
void edgui_test_preload_1(void)
{
    /* Create a default object. It will look strange with the default style*/
    edgui_obj_t * preload1 = edgui_preload_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(preload1, 10, 10);

    /* Create and apply a style*/
    static edgui_style_t style1;
    edgui_style_copy(&style1, &edgui_style_plain);
    style1.line.color = EDGUI_COLOR_RED;
    style1.line.width = 8;
    style1.line.rounded = 1;
    style1.body.border.width = 2;
    style1.body.border.color = EDGUI_COLOR_MAROON;
    style1.body.padding.left = 3;
    style1.body.padding.right = 3;
    style1.body.padding.top = 3;
    style1.body.padding.bottom = 3;
    edgui_obj_t * preload2 = edgui_preload_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(preload2, 60, 60);
    edgui_preload_set_style(preload2, EDGUI_PRELOAD_STYLE_MAIN, &style1);
    edgui_obj_align(preload2, preload1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);


    edgui_obj_t * preload3 = edgui_preload_create(edgui_disp_get_scr_act(NULL), preload2);
    edgui_preload_set_arc_length(preload3, 90);
    edgui_preload_set_spin_time(preload3, 5000);
    edgui_obj_align(preload3, preload2, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
//
//    /* Copy 'preload2' and set a new style for it */
//    static edgui_style_t style1;
//    edgui_style_copy(&style1, &edgui_style_plain);
//    style1.line.color = EDGUI_COLOR_RED;
//    style1.line.width = 8;
//    edgui_obj_t * preload3 = edgui_preload_create(edgui_scr_act(NULL), preload2);
//    edgui_obj_set_style(preload3, &style1);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_PRELOAD && EDGUI_USE_TESTS*/
