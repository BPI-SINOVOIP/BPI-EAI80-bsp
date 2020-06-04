/**
 * @file edgui_test_btn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_btn.h"

#if EDGUI_USE_BTN && EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void btn_event_cb(edgui_obj_t * btn, edgui_event_t event);

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
 * Create buttons to test their functionalities
 */
void edgui_test_btn_1(void)
{
    /* Create a button which looks well */
    edgui_obj_t * btn1 = edgui_btn_create(edgui_disp_get_scr_act(NULL), NULL);

    /* Create a default button manually set to toggled state*/
    edgui_obj_t * btn2 = edgui_btn_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(btn2, btn1, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 20);
    edgui_btn_set_state(btn2, EDGUI_BTN_STATE_TGL_REL);

    /* Create a button which can be toggled */
    edgui_obj_t * btn3 = edgui_btn_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(btn3, btn2, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 20);
    edgui_btn_set_toggle(btn3, true);

    /* Test actions:
     * Press: increase width, Release: decrease width, Long press: delete */
    edgui_obj_t * btn4 = edgui_btn_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(btn4, btn1, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);
    edgui_obj_set_event_cb(btn4, btn_event_cb);

    /* Test styles and copy. Same as 'btn4' but different styles */
    static edgui_style_t style_rel;
    edgui_style_copy(&style_rel, &edgui_style_pretty);
    style_rel.body.main_color = EDGUI_COLOR_ORANGE;
    style_rel.body.grad_color = EDGUI_COLOR_BLACK;
    style_rel.body.border.color = EDGUI_COLOR_RED;
    style_rel.body.shadow.color = EDGUI_COLOR_MAROON;
    style_rel.body.shadow.width = 10;

    static edgui_style_t style_pr;
    edgui_style_copy(&style_pr,  &edgui_style_pretty);
    style_pr.body.opa = EDGUI_OPA_TRANSP;
    style_pr.body.border.color = EDGUI_COLOR_RED;
    style_pr.body.border.width = 4;

    /*Skip 'TGL_PR' (leave unchanged)*/

    static edgui_style_t style_tpr;
    edgui_style_copy(&style_tpr, &edgui_style_pretty);
    style_tpr.body.opa = EDGUI_OPA_TRANSP;
    style_tpr.body.border.color = EDGUI_COLOR_RED;
    style_tpr.body.border.width = 4;

    static edgui_style_t style_ina;
    edgui_style_copy(&style_ina, &edgui_style_pretty);
    style_ina.body.main_color = EDGUI_COLOR_SILVER;
    style_ina.body.grad_color = EDGUI_COLOR_GRAY;
    style_ina.body.border.color = EDGUI_COLOR_RED;

    /*Create styled button*/
    edgui_obj_t * btn5 = edgui_btn_create(edgui_scr_act(), btn4);
    edgui_obj_align(btn5, btn4, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 20);
    edgui_btn_set_style(btn5, EDGUI_BTN_STYLE_REL, &style_rel);
    edgui_btn_set_style(btn5, EDGUI_BTN_STYLE_PR, &style_pr);
    edgui_btn_set_style(btn5, EDGUI_BTN_STYLE_TGL_PR, &style_tpr);
    edgui_btn_set_style(btn5, EDGUI_BTN_STYLE_INA, &style_ina);
    edgui_btn_set_toggle(btn5, true);

    /* Test style copy and inactive state*/
    edgui_obj_t * btn6 = edgui_btn_create(edgui_scr_act(), btn5);
    edgui_obj_align(btn6, btn5, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 20);
    edgui_btn_set_state(btn6, EDGUI_BTN_STATE_INA);

    /*Test horizontal fit and default layout (CENTER)*/
    edgui_obj_t * btn7 = edgui_btn_create(edgui_scr_act(), NULL);
    edgui_btn_set_fit2(btn7, EDGUI_FIT_TIGHT, EDGUI_FIT_NONE);
    edgui_obj_t * label = edgui_label_create(btn7, NULL);
    edgui_label_set_text(label, "A quite long text");
    label = edgui_label_create(btn7, NULL);
    edgui_label_set_text(label, "Short text");
    edgui_obj_align(btn7, btn4, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void btn_event_cb(edgui_obj_t * btn, edgui_event_t event)
{
    if(event == EDGUI_EVENT_PRESSED) {
        edgui_obj_set_width(btn, edgui_obj_get_width(btn) + (10));
    }
    else if(event == EDGUI_EVENT_RELEASED) {
        edgui_obj_set_width(btn, edgui_obj_get_width(btn) - (10));
    }
    else if(event == EDGUI_EVENT_LONG_PRESSED) {
        edgui_obj_del(btn);
    }
}


#endif /*EDGUI_USE_BTN && EDGUI_USE_TESTS*/
