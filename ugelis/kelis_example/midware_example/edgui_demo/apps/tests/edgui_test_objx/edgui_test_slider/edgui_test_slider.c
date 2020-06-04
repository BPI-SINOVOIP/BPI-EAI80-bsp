/**
 * @file edgui_test_slider.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_slider.h"

#if EDGUI_USE_SLIDER && EDGUI_USE_TESTS

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
 * Create sliders to test their functionalities
 */
void edgui_test_slider_1(void)
{
    /* Create a default object*/
    edgui_obj_t * slider1 = edgui_slider_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(slider1, 10, 10);

    /* Modify size and position, range and set to 75 % */
    edgui_obj_t * slider2 = edgui_slider_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(slider2, 150, 50);
    edgui_obj_align(slider2, slider1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    edgui_slider_set_range(slider2, -50, 50);
    edgui_slider_set_value(slider2, 25, false);

    /* Copy 'slider2' but set its size to be vertical (indicator at 75%)*/
    edgui_obj_t * slider3 = edgui_slider_create(edgui_disp_get_scr_act(NULL), slider2);
    edgui_obj_set_size(slider3, 50, 150);
    edgui_obj_align(slider3, slider2, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);


    /* Copy 'slider2' and set new style for it
     * (like 'slider2' on its left but dark bg, thin red indicator with big light)*/
    static edgui_style_t slider_bg;
    edgui_style_copy(&slider_bg, &edgui_style_pretty);
    slider_bg.body.main_color = EDGUI_COLOR_BLACK;

    static edgui_style_t slider_indic;
    edgui_style_copy(&slider_indic, &edgui_style_pretty);
    slider_indic.body.main_color = EDGUI_COLOR_RED;
    slider_indic.body.grad_color = EDGUI_COLOR_MAROON;
    slider_indic.body.shadow.color = EDGUI_COLOR_RED;
    slider_indic.body.shadow.width = 20;
    slider_indic.body.padding.left = 0;
    slider_indic.body.padding.right = 0;
    slider_indic.body.padding.top = 0;
    slider_indic.body.padding.bottom = 0;

    static edgui_style_t slider_knob;
    edgui_style_copy(&slider_knob, &edgui_style_pretty);
    slider_knob.body.radius = EDGUI_RADIUS_CIRCLE;
    slider_knob.body.border.color = EDGUI_COLOR_BLUE;
    slider_knob.body.opa = EDGUI_OPA_TRANSP;

    edgui_obj_t * slider4 = edgui_slider_create(edgui_disp_get_scr_act(NULL), slider2);
    edgui_obj_align(slider4, slider2, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);
    edgui_slider_set_style(slider4, EDGUI_SLIDER_STYLE_BG, &slider_bg);
    edgui_slider_set_style(slider4, EDGUI_SLIDER_STYLE_INDIC, &slider_indic);
    edgui_slider_set_style(slider4, EDGUI_SLIDER_STYLE_KNOB, &slider_knob);

    /* Copy 'slider4' but set its size to be vertical*/
    edgui_obj_t * slider5 = edgui_slider_create(edgui_disp_get_scr_act(NULL), slider4);
    edgui_obj_set_size(slider5, 50, 150);
    edgui_obj_align(slider5, slider4, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_SLIDER && EDGUI_USE_TESTS*/
