/**
 * @file edgui_test_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_arc.h"
#if EDGUI_USE_ARC && EDGUI_USE_TESTS

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
 * Create arcs to test their functionalities
 */
void edgui_test_arc_1(void)
{
    /* Create a default object*/
    edgui_obj_t * arc1 = edgui_arc_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(arc1, 10, 10);

    /* Modify size, position and angles*/
    edgui_obj_t * arc2 = edgui_arc_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(arc2, 100, 100);
    edgui_obj_align(arc2, arc1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    edgui_arc_set_angles(arc2, 0, 250);

    /* Copy 'arc2' and set a new style for it */
    static edgui_style_t style1;
    edgui_style_copy(&style1, &edgui_style_plain);
    style1.line.color = EDGUI_COLOR_RED;
    style1.line.width = 8;
    edgui_obj_t * arc3 = edgui_arc_create(edgui_disp_get_scr_act(NULL), arc2);
    edgui_obj_set_style(arc3, &style1);
    edgui_obj_align(arc3, arc2, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_ARC && EDGUI_USE_TESTS*/
