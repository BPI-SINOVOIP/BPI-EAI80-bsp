/**
 * @file edgui_test_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_line.h"

#if EDGUI_USE_LINE && EDGUI_USE_TESTS

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
 * Create lines to test their functionalities
 */
void edgui_test_line_1(void)
{
    static const edgui_point_t p[] = {{5, 5}, {60, 5}, {120, 65}};

    /* Create a default object*/
    edgui_obj_t * line1 = edgui_line_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(line1, 10, 10);
    edgui_line_set_points(line1, p, 3);

    /*Test y invert*/
    edgui_obj_t * line2 = edgui_line_create(edgui_disp_get_scr_act(NULL), line1);
    edgui_line_set_y_invert(line2, true);
    edgui_obj_align(line2, line1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    /*Test styling*/
    static edgui_style_t style1;
    edgui_style_copy(&style1, &edgui_style_plain);

    style1.line.color = EDGUI_COLOR_RED;
    style1.line.width = 20;

    edgui_obj_t * line3 = edgui_line_create(edgui_disp_get_scr_act(NULL), line2);
    edgui_line_set_style(line3, EDGUI_LINE_STYLE_MAIN, &style1);
    edgui_obj_align(line3, line1, EDGUI_ALIGN_OUT_RIGHT_TOP, 5, 0);
    edgui_obj_set_hidden(line3, false);

    /*Test rounding*/
    static edgui_style_t style2;
    edgui_style_copy(&style2, &style1);
    style2.line.rounded = 1;

    edgui_obj_t * line4 = edgui_line_create(edgui_disp_get_scr_act(NULL), line3);
    edgui_line_set_style(line4, EDGUI_LINE_STYLE_MAIN, &style2);
    edgui_obj_align(line4, line3, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    edgui_obj_set_hidden(line4, false);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_LINE && EDGUI_USE_TESTS*/
