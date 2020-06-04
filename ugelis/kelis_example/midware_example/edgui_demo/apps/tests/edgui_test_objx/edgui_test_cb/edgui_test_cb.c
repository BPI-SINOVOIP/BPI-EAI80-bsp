/**
 * @file edgui_test_cb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_cb.h"

#if EDGUI_USE_CB && EDGUI_USE_TESTS

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
 * Create check boxes to test their functionalities
 */
void edgui_test_cb_1(void)
{
    /* Create a default object*/
    edgui_obj_t * cb1 = edgui_cb_create(edgui_disp_get_scr_act(NULL), NULL);

    /*Create an other check box and set its text*/
    edgui_obj_t * cb2 = edgui_cb_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_cb_set_text(cb2, "UTF8-text: üŰ öŐ íÍ");
    edgui_obj_align(cb2, cb1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    /*Create styles for the bullets*/
    static edgui_style_t cb3_styles[_EDGUI_BTN_STATE_NUM];
    edgui_style_copy(&cb3_styles[EDGUI_BTN_STATE_REL], &edgui_style_plain);
    cb3_styles[EDGUI_BTN_STATE_REL].body.radius = EDGUI_DPI / 20;
    cb3_styles[EDGUI_BTN_STATE_REL].body.border.width = 1;
    cb3_styles[EDGUI_BTN_STATE_REL].body.border.color = EDGUI_COLOR_GRAY;
    cb3_styles[EDGUI_BTN_STATE_REL].body.main_color = EDGUI_COLOR_WHITE;
    cb3_styles[EDGUI_BTN_STATE_REL].body.grad_color = EDGUI_COLOR_SILVER;

    edgui_style_copy(&cb3_styles[EDGUI_BTN_STATE_PR], &cb3_styles[EDGUI_BTN_STATE_REL]);
    cb3_styles[EDGUI_BTN_STATE_PR].body.main_color = EDGUI_COLOR_SILVER;
    cb3_styles[EDGUI_BTN_STATE_PR].body.grad_color = EDGUI_COLOR_GRAY;

    edgui_style_copy(&cb3_styles[EDGUI_BTN_STATE_TGL_REL], &cb3_styles[EDGUI_BTN_STATE_REL]);
    cb3_styles[EDGUI_BTN_STATE_TGL_REL].body.border.width = 4;
    cb3_styles[EDGUI_BTN_STATE_TGL_REL].body.border.color = EDGUI_COLOR_WHITE;
    cb3_styles[EDGUI_BTN_STATE_TGL_REL].body.border.opa = EDGUI_OPA_70;
    cb3_styles[EDGUI_BTN_STATE_TGL_REL].body.main_color = EDGUI_COLOR_GRAY;
    cb3_styles[EDGUI_BTN_STATE_TGL_REL].body.grad_color = EDGUI_COLOR_BLACK;

    edgui_style_copy(&cb3_styles[EDGUI_BTN_STATE_TGL_PR], &cb3_styles[EDGUI_BTN_STATE_TGL_REL]);
    cb3_styles[EDGUI_BTN_STATE_TGL_PR].body.border.color = EDGUI_COLOR_SILVER;
    cb3_styles[EDGUI_BTN_STATE_TGL_PR].body.border.opa = EDGUI_OPA_70;
    cb3_styles[EDGUI_BTN_STATE_TGL_PR].body.main_color = EDGUI_COLOR_GRAY;
    cb3_styles[EDGUI_BTN_STATE_TGL_PR].body.grad_color = EDGUI_COLOR_BLACK;

    edgui_style_copy(&cb3_styles[EDGUI_BTN_STATE_INA], &cb3_styles[EDGUI_BTN_STATE_TGL_REL]);
    cb3_styles[EDGUI_BTN_STATE_INA].body.border.width = 1;
    cb3_styles[EDGUI_BTN_STATE_INA].body.border.color = EDGUI_COLOR_GRAY;
    cb3_styles[EDGUI_BTN_STATE_INA].body.main_color = EDGUI_COLOR_SILVER;
    cb3_styles[EDGUI_BTN_STATE_INA].body.grad_color = EDGUI_COLOR_SILVER;


    /*Copy the previous check box and apply the new styles*/
    edgui_obj_t * cb3 = edgui_cb_create(edgui_disp_get_scr_act(NULL), cb2);
    edgui_cb_set_style(cb3, EDGUI_CB_STYLE_BOX_REL, &cb3_styles[EDGUI_BTN_STATE_REL]);
    edgui_cb_set_style(cb3, EDGUI_CB_STYLE_BOX_PR, &cb3_styles[EDGUI_BTN_STATE_PR]);
    edgui_cb_set_style(cb3, EDGUI_CB_STYLE_BOX_TGL_REL, &cb3_styles[EDGUI_BTN_STATE_TGL_REL]);
    edgui_cb_set_style(cb3, EDGUI_CB_STYLE_BOX_TGL_PR, &cb3_styles[EDGUI_BTN_STATE_TGL_PR]);
    edgui_cb_set_style(cb3, EDGUI_CB_STYLE_BOX_INA, &cb3_styles[EDGUI_BTN_STATE_INA]);
    edgui_obj_align(cb3, cb2, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    /*Copy the previous check box and set it to INACTIVE*/
    edgui_obj_t * cb4 = edgui_cb_create(edgui_disp_get_scr_act(NULL), cb3);
    edgui_obj_align(cb4, cb3, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    edgui_btn_set_state(cb4, EDGUI_BTN_STATE_INA);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_CB && EDGUI_USE_TESTS*/
