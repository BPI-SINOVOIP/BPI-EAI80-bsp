/**
 * @file edgui_test_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>  /*For printf in the action*/
#include "edgui_test_sw.h"

#if EDGUI_USE_SW && EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void event_handler(edgui_obj_t * sw, edgui_event_t event);

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
 * Create switches to test their functionalities
 */
void edgui_test_sw_1(void)
{
    /* Default object */
    edgui_obj_t * sw1 = edgui_sw_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(sw1, 10, 10);
    edgui_obj_set_event_cb(sw1, event_handler);

    static edgui_style_t bg;
    static edgui_style_t indic;

    edgui_style_copy(&bg, &edgui_style_pretty);
    bg.body.padding.left = -5;
    bg.body.padding.right = -5;
    bg.body.padding.top = -5;
    bg.body.padding.bottom = -5;

    edgui_style_copy(&indic, &edgui_style_pretty_color);
    indic.body.padding.left = 8;
    indic.body.padding.right = 8;
    indic.body.padding.top  = 8;
    indic.body.padding.bottom = 8;

    edgui_obj_t * sw2 = edgui_sw_create(edgui_disp_get_scr_act(NULL), sw1);
    edgui_sw_set_style(sw2, EDGUI_SW_STYLE_BG, &bg);
    edgui_sw_set_style(sw2, EDGUI_SW_STYLE_INDIC, &indic);
    edgui_sw_set_style(sw2, EDGUI_SW_STYLE_KNOB_OFF, &edgui_style_btn_pr);
    edgui_sw_set_style(sw2, EDGUI_SW_STYLE_KNOB_ON, &edgui_style_btn_tgl_pr);

    edgui_sw_on(sw2, EDGUI_ANIM_OFF);
    edgui_obj_align(sw2, sw1, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);

    edgui_obj_t * sw3 = edgui_sw_create(edgui_disp_get_scr_act(NULL), sw2);
    edgui_obj_align(sw3, sw2, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void event_handler(edgui_obj_t * sw, edgui_event_t event)
{

    if(event == EDGUI_EVENT_VALUE_CHANGED) {
#if EDGUI_EX_PRINTF
        printf("Switch state: %d\n", edgui_sw_get_state(sw));
#endif
    }
}


#endif /*EDGUI_USE_SW && EDGUI_USE_TESTS*/
