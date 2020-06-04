/**
 * @file edgui_test_mbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_mbox.h"
#if EDGUI_USE_MBOX && EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void mbox_event_cb(edgui_obj_t * mbox, edgui_event_t event);

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
 * Create message boxes to test their functionalities
 */
void edgui_test_mbox_1(void)
{
    /* Default object */
    edgui_obj_t * mbox1 = edgui_mbox_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(mbox1, 10, 10);

    /*Add buttons and modify text*/
    static const char * btns2[] = {"Ok", "Cancel", ""};
    edgui_obj_t * mbox2 = edgui_mbox_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_mbox_add_btns(mbox2, btns2);
    edgui_mbox_set_text(mbox2, "Message");
    edgui_obj_set_width(mbox2, edgui_disp_get_hor_res(NULL) / 2);
    edgui_obj_align(mbox2, mbox1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    /*Add styles*/
    static edgui_style_t bg;
    static edgui_style_t btn_bg;
    edgui_style_copy(&bg, &edgui_style_pretty);
    edgui_style_copy(&btn_bg, &edgui_style_pretty);
    bg.body.padding.left = 20;
    bg.body.padding.right = 20;
    bg.body.padding.top = 20;
    bg.body.padding.bottom = 20;
    bg.body.padding.inner = 20;
    bg.body.main_color = EDGUI_COLOR_BLACK;
    bg.body.grad_color = EDGUI_COLOR_MAROON;
    bg.text.color = EDGUI_COLOR_WHITE;

    btn_bg.body.padding.left = 10;
    btn_bg.body.padding.right = 10;
    btn_bg.body.padding.top = 5;
    btn_bg.body.padding.bottom = 5;
    btn_bg.body.padding.inner = 40;
    btn_bg.body.opa = EDGUI_OPA_TRANSP;
    btn_bg.body.border.color = EDGUI_COLOR_WHITE;
    btn_bg.text.color = EDGUI_COLOR_WHITE;

    static edgui_style_t btn_rel;
    edgui_style_copy(&btn_rel, &edgui_style_btn_rel);
    btn_rel.body.opa = EDGUI_OPA_TRANSP;
    btn_rel.body.border.color = EDGUI_COLOR_WHITE;

    edgui_obj_t * mbox3 = edgui_mbox_create(edgui_disp_get_scr_act(NULL), mbox2);
    edgui_mbox_set_style(mbox3, EDGUI_MBOX_STYLE_BTN_REL, &btn_rel);
    edgui_mbox_set_style(mbox3, EDGUI_MBOX_STYLE_BTN_BG,  &btn_bg);
    edgui_mbox_set_style(mbox3, EDGUI_MBOX_STYLE_BG, &bg);
    edgui_obj_align(mbox3, mbox1, EDGUI_ALIGN_OUT_RIGHT_TOP, 10, 0);
    edgui_obj_set_event_cb(mbox3, mbox_event_cb);

    /*Copy with styles and set button width*/
    edgui_obj_t * mbox4 = edgui_mbox_create(edgui_disp_get_scr_act(NULL), mbox3);
    edgui_mbox_set_text(mbox4, "A quite long message text which is\n"
                     "manually broken into multiple lines");

    static const char * btns3[] = {"Ok", "Cancel", "Third", ""};
    edgui_mbox_add_btns(mbox4, btns3);
    edgui_obj_set_event_cb(mbox4, mbox_event_cb);
    edgui_obj_align(mbox4, mbox3, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void mbox_event_cb(edgui_obj_t * mbox, edgui_event_t event)
{
    if(event != EDGUI_EVENT_CLICKED) return;

    const char * btn_txt = edgui_mbox_get_active_btn_text(mbox);
    if(btn_txt) {
        edgui_mbox_set_text(mbox, btn_txt);
    }
}

#endif /*EDGUI_USE_MBOX && EDGUI_USE_TESTS*/
