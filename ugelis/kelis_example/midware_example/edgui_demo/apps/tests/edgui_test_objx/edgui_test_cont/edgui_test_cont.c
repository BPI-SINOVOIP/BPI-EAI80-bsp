/**
 * @file edgui_test_cont.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_cont.h"

#if EDGUI_USE_CONT && EDGUI_USE_TESTS

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
 * Create containers to test their  basic functionalities
 */
void edgui_test_cont_1(void)
{
    /* Create a default object*/
    edgui_obj_t * cont1 = edgui_cont_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(cont1, 10, 10);
    edgui_cont_set_style(cont1, EDGUI_CONT_STYLE_MAIN, &edgui_style_pretty);

    /*Test fit wit adding two labels*/
    edgui_obj_t * cont2 = edgui_cont_create(edgui_disp_get_scr_act(NULL), cont1);
    edgui_cont_set_fit(cont2, EDGUI_FIT_TIGHT);

    edgui_obj_t * obj2_1 = edgui_label_create(cont2, NULL);
    edgui_label_set_text(obj2_1, "Short");

    edgui_obj_t * obj2_2 = edgui_label_create(cont2, NULL);
    edgui_label_set_text(obj2_2, "A longer text");
    edgui_obj_set_pos(obj2_2, 80, 30);

    edgui_obj_align(cont2, cont1, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);

    /*Test layout and fit togother*/
    edgui_obj_t * cont3 = edgui_cont_create(edgui_disp_get_scr_act(NULL), cont2);
    edgui_label_create(cont3, obj2_1);
    edgui_label_create(cont3, obj2_2);
    edgui_cont_set_layout(cont3, EDGUI_LAYOUT_COL_L);

    edgui_obj_align(cont3, cont2, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 10);


    /*Set a new style with new padding*/
    static edgui_style_t cont4_style;
    edgui_style_copy(&cont4_style, &edgui_style_pretty_color);
    cont4_style.body.padding.left = 20;
    cont4_style.body.padding.right = 20;
    cont4_style.body.padding.top = 40;
    cont4_style.body.padding.bottom = 40;
    cont4_style.body.padding.inner = 1;

    edgui_obj_t * cont4 = edgui_cont_create(edgui_disp_get_scr_act(NULL), cont3);
    edgui_label_create(cont4, obj2_1);
    edgui_label_create(cont4, obj2_2);
    edgui_cont_set_style(cont4, EDGUI_CONT_STYLE_MAIN, &cont4_style);

    edgui_obj_align(cont4, cont3, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

}

/**
 * Test nested style inheritance on padding update
 */
void edgui_test_cont_2(void)
{

    /*Create a new style with big paddings*/
    static edgui_style_t cont4_style;
    edgui_style_copy(&cont4_style, &edgui_style_pretty_color);
    cont4_style.body.padding.left = 10;
    cont4_style.body.padding.right = 10;
    cont4_style.body.padding.top = 20;
    cont4_style.body.padding.bottom = 20;
    cont4_style.body.padding.inner = 1;

    /* Create a main container*/
    edgui_obj_t * cont_main = edgui_cont_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(cont_main, 50, 50);
    edgui_cont_set_style(cont_main, EDGUI_CONT_STYLE_MAIN, &edgui_style_pretty);
    edgui_cont_set_fit(cont_main, EDGUI_FIT_TIGHT);
    edgui_cont_set_layout(cont_main, EDGUI_LAYOUT_ROW_M);

    /*Create two containers on the main* with two-two labels*/
    edgui_obj_t * cont_sub1 = edgui_cont_create(cont_main, NULL);
    edgui_cont_set_style(cont_sub1, EDGUI_CONT_STYLE_MAIN, NULL);      /*Inherit style from parent*/
    edgui_cont_set_fit(cont_sub1, EDGUI_FIT_TIGHT);
    edgui_cont_set_layout(cont_sub1, EDGUI_LAYOUT_COL_M);

    edgui_obj_t * obj1_1 = edgui_label_create(cont_sub1, NULL);
    edgui_label_set_text(obj1_1, "Short");

    edgui_obj_t * obj1_2 = edgui_label_create(cont_sub1, NULL);
    edgui_label_set_text(obj1_2, "A long text");

    edgui_obj_t * cont_sub2 = edgui_cont_create(cont_main, cont_sub1);
    edgui_label_create(cont_sub2, obj1_1);
    edgui_label_create(cont_sub2, obj1_2);

    /*Set the new style*/
    edgui_cont_set_style(cont_main, EDGUI_CONT_STYLE_MAIN, &cont4_style);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_CONT && EDGUI_USE_TESTS*/
