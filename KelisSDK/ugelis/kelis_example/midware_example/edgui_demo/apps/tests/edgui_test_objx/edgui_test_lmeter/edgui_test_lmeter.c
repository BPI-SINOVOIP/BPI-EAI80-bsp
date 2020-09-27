/**
 * @file edgui_test_lmeter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>

#include "edgui_test_lmeter.h"

#if EDGUI_USE_LMETER && EDGUI_USE_TESTS

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
 * Create line meters to test their functionalities
 */
void edgui_test_lmeter_1(void)
{
    /* Create a default object*/
    edgui_obj_t * lmeter1 = edgui_lmeter_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(lmeter1, 10, 10);
    edgui_lmeter_set_value(lmeter1, 60);

    /*Copy the previous line meter and set smaller size for it*/
    edgui_obj_t * lmeter2 = edgui_lmeter_create(edgui_disp_get_scr_act(NULL), lmeter1);
    edgui_obj_set_size(lmeter2, EDGUI_DPI / 2,  EDGUI_DPI / 2);
    edgui_obj_align(lmeter2, lmeter1, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /*Create a styled line meter*/
    static edgui_style_t style3;
    edgui_style_copy(&style3, &edgui_style_pretty);
    style3.body.main_color = EDGUI_COLOR_GREEN;
    style3.body.grad_color = EDGUI_COLOR_RED;
    style3.body.padding.left = 4;
    style3.body.border.color = EDGUI_COLOR_GRAY;      /*Means the needle middle*/
    style3.line.width = 2;
    style3.line.color = EDGUI_COLOR_SILVER;

    edgui_obj_t * lmeter3 = edgui_lmeter_create(edgui_disp_get_scr_act(NULL), lmeter1);
    edgui_obj_align(lmeter3, lmeter1, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);
    edgui_obj_set_style(lmeter3, &style3);
    edgui_lmeter_set_scale(lmeter3, 270, 41);
    edgui_lmeter_set_range(lmeter3, -100, 100);
    edgui_lmeter_set_value(lmeter3, 50);

    /*Copy the modified 'lmeter3' and set a smaller size for it*/
    edgui_obj_t * lmeter4 = edgui_lmeter_create(edgui_disp_get_scr_act(NULL), lmeter3);
    edgui_obj_set_size(lmeter4, 60, 60);
    edgui_obj_align(lmeter4, lmeter3, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_LMETER && EDGUI_USE_TESTS*/
