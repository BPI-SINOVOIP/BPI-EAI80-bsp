/**
 * @file edgui_test_object.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_obj.h"
#include "../../edgui_examples.h"  /*Just to include somewhere to test 'edgui_example' version*/
#if EDGUI_USE_TESTS

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
 * Create base objects to test their functionalities
 */
void edgui_test_object_1(void)
{
    /* Create a default object and set EDGUI_STYLE_PRETTY_COLOR style */
    edgui_obj_t * obj1 = edgui_obj_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_style(obj1, &edgui_style_plain_color);


    /*Create an object and set a user create style for it*/
    static edgui_style_t style_obj2;
    edgui_style_copy(&style_obj2, &edgui_style_pretty);
    style_obj2.body.main_color = EDGUI_COLOR_RED;
    style_obj2.body.grad_color = EDGUI_COLOR_BLACK;
    style_obj2.body.radius = 0;
    style_obj2.body.border.color = EDGUI_COLOR_WHITE;
    style_obj2.body.border.width = 4;
    style_obj2.body.opa = EDGUI_OPA_50;
    style_obj2.body.shadow.width = 10;

    edgui_obj_t * obj2 = edgui_obj_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(obj2, 30, 30);
    edgui_obj_align(obj2, obj1, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);
    edgui_obj_set_style(obj2, &style_obj2);

    /*Test drag, drag_parent, drag throw and copy*/
    edgui_obj_t * obj3_parent = edgui_obj_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(obj3_parent, obj2, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);
    edgui_obj_set_style(obj3_parent, &edgui_style_pretty);
    edgui_obj_set_drag(obj3_parent, true);
    edgui_obj_set_drag_throw(obj3_parent, true);

    edgui_obj_t * obj3 = edgui_obj_create(obj3_parent, obj2);
    edgui_obj_align(obj3, NULL, EDGUI_ALIGN_CENTER, 0, 0);
    edgui_obj_set_click(obj3, true);
    edgui_obj_set_drag_parent(obj3, true);

    /*Create a parent and 3 objects on it. Hide the parent but move 2 children to the screen*/

    edgui_obj_t * obj4_parent = edgui_obj_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(obj4_parent, edgui_obj_get_x(obj1) + 10, edgui_obj_get_y(obj1) + edgui_obj_get_height(obj1) + 20);
    edgui_obj_set_style(obj4_parent, &edgui_style_pretty_color);
    edgui_obj_set_hidden(obj4_parent, true); /*Hide this and all children objects*/

    edgui_obj_t * obj4_1 = edgui_obj_create(obj4_parent, obj2);
    edgui_obj_set_pos(obj4_1, 10, 10);

    edgui_obj_t * obj4_2 = edgui_obj_create(obj4_parent, obj2);
    edgui_obj_set_pos(obj4_2, 20, 20);

    edgui_obj_t * obj4_3 = edgui_obj_create(obj4_parent, obj2);
    edgui_obj_set_pos(obj4_3, 30, 30);

    /*Move two children to the screen (now they will be visible)*/
    edgui_obj_set_parent(obj4_2, edgui_disp_get_scr_act(NULL));
    edgui_obj_align(obj4_2, obj4_parent, EDGUI_ALIGN_OUT_RIGHT_MID, 10, 0);

    edgui_obj_set_parent(obj4_3, edgui_disp_get_scr_act(NULL));
    edgui_obj_align(obj4_3, obj4_parent, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 5);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TESTS*/
