/**
 * @file edgui_test_bar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_bar.h"
#if EDGUI_USE_BAR && EDGUI_USE_TESTS

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
 * Create bars to test their functionalities
 */
void edgui_test_bar_1(void)
{
    /* Create a default object*/
    edgui_obj_t * bar1 = edgui_bar_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(bar1, 10, 10);
    edgui_bar_set_value(bar1, 40, false);

    /* Modify size and position, range and set to 75 % */
    edgui_obj_t * bar2 = edgui_bar_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(bar2, 200, 50);
    edgui_obj_align(bar2, bar1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    edgui_bar_set_range(bar2, -50, 50);
    edgui_bar_set_value(bar2, 25, false);

    /* Copy 'bar2' but set its size to be vertical (indicator at 75%)*/
    edgui_obj_t * bar3 = edgui_bar_create(edgui_disp_get_scr_act(NULL), bar2);
    edgui_obj_set_size(bar3, 50, 200);
    edgui_obj_align(bar3, bar2, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);


    /* Copy 'bar2' and set new style for it
     * (like 'bar2' on its left but dark bg, thin red indicator with big light)*/
    static edgui_style_t bar_bg;
    edgui_style_copy(&bar_bg, &edgui_style_pretty);
    bar_bg.body.main_color = EDGUI_COLOR_BLACK;

    static edgui_style_t bar_indic;
    edgui_style_copy(&bar_indic, &edgui_style_pretty);
    bar_indic.body.main_color = EDGUI_COLOR_RED;
    bar_indic.body.grad_color = EDGUI_COLOR_MAROON;
    bar_indic.body.shadow.color = EDGUI_COLOR_RED;
    bar_indic.body.shadow.width = 20;
    bar_indic.body.padding.top = 10;       /*Set the padding around the indicator*/
    bar_indic.body.padding.bottom = 3;
    bar_indic.body.padding.left = 3;
    bar_indic.body.padding.right = 10;

    edgui_obj_t * bar4 = edgui_bar_create(edgui_disp_get_scr_act(NULL), bar2);
    edgui_obj_align(bar4, bar2, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);
    edgui_bar_set_style(bar4, EDGUI_BAR_STYLE_BG, &bar_bg);
    edgui_bar_set_style(bar4, EDGUI_BAR_STYLE_INDIC, &bar_indic);

    /* Copy 'bar4' but set its size to be vertical*/
    edgui_obj_t * bar5 = edgui_bar_create(edgui_disp_get_scr_act(NULL), bar4);
    edgui_obj_set_size(bar5, 50, 200);
    edgui_obj_align(bar5, bar4, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_BAR && EDGUI_USE_TESTS*/
