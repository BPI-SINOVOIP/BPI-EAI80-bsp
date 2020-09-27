/**
 * @file edgui_test_roller.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_roller.h"

#if EDGUI_USE_ROLLER && EDGUI_USE_TESTS

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
 * Create rollers to test their functionalities
 */
void edgui_test_roller_1(void)
{
    /* Default object*/
    edgui_obj_t * roller1 = edgui_roller_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(roller1, 10, 10);

    static edgui_style_t bg;
    edgui_style_copy(&bg, &edgui_style_pretty);
    bg.body.main_color = EDGUI_COLOR_SILVER;
    bg.body.grad_color = EDGUI_COLOR_WHITE;
    bg.body.shadow.width = 5;
    bg.text.line_space = 10;
    bg.text.opa = EDGUI_OPA_60;

    edgui_obj_t * roller2 = edgui_roller_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(roller2, roller1, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    edgui_roller_set_anim_time(roller2, 500);
    edgui_roller_set_style(roller2, EDGUI_ROLLER_STYLE_BG, &bg);
    edgui_roller_set_style(roller2, EDGUI_ROLLER_STYLE_SEL, &edgui_style_plain);
    edgui_roller_set_selected(roller2, 4, true);
    edgui_roller_set_options(roller2, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", true);
    edgui_roller_set_visible_row_count(roller2, 3);

    edgui_obj_t * roller3 = edgui_roller_create(edgui_disp_get_scr_act(NULL), roller2);
    edgui_obj_align(roller3, roller2, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    edgui_roller_set_fix_width(roller3, EDGUI_DPI);
    edgui_roller_set_align(roller3, EDGUI_LABEL_ALIGN_RIGHT);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_ROLLER && EDGUI_USE_TESTS*/
