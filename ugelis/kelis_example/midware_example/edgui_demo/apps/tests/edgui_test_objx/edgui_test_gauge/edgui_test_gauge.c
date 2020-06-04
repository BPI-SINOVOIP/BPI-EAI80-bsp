/**
 * @file edgui_test_gauge.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_gauge.h"

#if EDGUI_USE_GAUGE && EDGUI_USE_TESTS

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
 * Create gauges to test their functionalities
 */
void edgui_test_gauge_1(void)
{
    /* Create a default object*/
    edgui_obj_t * gauge1 = edgui_gauge_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(gauge1, 10, 10);
    edgui_gauge_set_value(gauge1, 0, 75);

    /*Copy the previous gauge and set smaller size for it*/
    edgui_obj_t * gauge2 = edgui_gauge_create(edgui_disp_get_scr_act(NULL), gauge1);
    edgui_obj_set_size(gauge2, 2 * edgui_obj_get_width(gauge1) / 3,  2 * edgui_obj_get_height(gauge1) / 3);
    edgui_obj_align(gauge2, gauge1, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /*Copy the first gauge add more needles and set new style*/
    static edgui_color_t needle_colors[3];
    needle_colors[0] = EDGUI_COLOR_BLUE;
    needle_colors[1] = EDGUI_COLOR_PURPLE;
    needle_colors[2] = EDGUI_COLOR_TEAL;

    /*Create a styled gauge*/
    static edgui_style_t style3;
    edgui_style_copy(&style3, &edgui_style_pretty);
    style3.body.main_color = EDGUI_COLOR_GREEN;
    style3.body.grad_color = EDGUI_COLOR_RED;
    style3.body.padding.left = 6;
    style3.body.padding.inner = 10;
    style3.body.padding.top = 8;
    style3.body.border.color = EDGUI_COLOR_GRAY;
    style3.line.width = 2;

    edgui_obj_t * gauge3 = edgui_gauge_create(edgui_disp_get_scr_act(NULL), gauge1);
    edgui_obj_align(gauge3, gauge1, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);
    edgui_obj_set_style(gauge3, &style3);
    edgui_gauge_set_scale(gauge3, 270, 41, 5);
    edgui_gauge_set_needle_count(gauge3, 3, needle_colors);
    edgui_gauge_set_value(gauge3, 0, 20);
    edgui_gauge_set_value(gauge3, 1, 40);
    edgui_gauge_set_value(gauge3, 2, 60);

    /*Copy the modified 'gauge3' and set a smaller size for it*/
    edgui_obj_t * gauge4 = edgui_gauge_create(edgui_disp_get_scr_act(NULL), gauge3);
    edgui_obj_set_size(gauge4, 100, 100);
    edgui_obj_align(gauge4, gauge3, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_GAUGE && EDGUI_USE_TESTS*/

