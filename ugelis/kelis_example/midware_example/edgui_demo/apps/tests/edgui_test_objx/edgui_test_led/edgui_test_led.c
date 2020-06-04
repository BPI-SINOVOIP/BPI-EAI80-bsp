/**
 * @file edgui_test_led.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_led.h"

#if EDGUI_USE_LED && EDGUI_USE_TESTS

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
 * Create LEDs to test their functionalities
 */
void edgui_test_led_1(void)
{
    /* Create a default object*/
    edgui_obj_t * led1 = edgui_led_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(led1, 20, 20);

    /*Create styles LED*/
    static edgui_style_t style;
    edgui_style_copy(&style, &edgui_style_pretty_color);
    style.body.shadow.width = 10;
    style.body.radius = EDGUI_RADIUS_CIRCLE;
    style.body.border.width = 3;
    style.body.border.opa = EDGUI_OPA_30;
    style.body.main_color = edgui_color_make(0xb5, 0x0f, 0x04);
    style.body.grad_color = edgui_color_make(0x50, 0x07, 0x02);
    style.body.border.color = edgui_color_make(0xfa, 0x0f, 0x00);
    style.body.shadow.color = edgui_color_make(0xb5, 0x0f, 0x04);

    edgui_obj_t * led2 = edgui_led_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_led_set_style(led2, EDGUI_LED_STYLE_MAIN, &style);
    edgui_obj_align(led2, led1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Turned ON LED*/
    edgui_obj_t * led_on = edgui_led_create(edgui_disp_get_scr_act(NULL), led2);
    edgui_obj_align(led_on, led2, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    edgui_led_on(led_on);

    /*Tuned OFF LED*/
    edgui_obj_t * led_off = edgui_led_create(edgui_disp_get_scr_act(NULL), led_on);
    edgui_obj_align(led_off, led_on, EDGUI_ALIGN_OUT_RIGHT_MID, 10, 0);
    edgui_led_off(led_off);

    /*Arbitrary brightness*/
    edgui_obj_t * led_x = edgui_led_create(edgui_disp_get_scr_act(NULL), led_off);
    edgui_obj_align(led_x, led_off, EDGUI_ALIGN_OUT_RIGHT_MID, 10, 0);
    edgui_led_set_bright(led_x, 170);


}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_LED && EDGUI_USE_TESTS*/
