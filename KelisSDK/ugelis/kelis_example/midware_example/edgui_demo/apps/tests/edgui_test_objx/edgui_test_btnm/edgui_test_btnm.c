/**
 * @file edgui_test_btnm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>  /*For printf in the action*/

#include "edgui_test_btnm.h"

#if  EDGUI_USE_BTNM && EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void btnm_event_cb(edgui_obj_t * btnm, edgui_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * btnm_map[] = {"One line", "\n", "\212", "\242Ina", "\204üŰöŐ", "\221éÉ", "\n", "\214", "\202Left", ""};
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create button matrixes to test their functionalities
 */
void edgui_test_btnm_1(void)
{
    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    /* Default object
     * GOAL: A button matrix with default buttons */
    edgui_obj_t * btnm1 = edgui_btnm_create(edgui_disp_get_scr_act(NULL), NULL);

    /* Test map, size and position. Also try some features.
     * GOAL: A button matrix with default buttons.  */
    static edgui_style_t rel;
    edgui_style_copy(&rel, &edgui_style_btn_tgl_rel);
    rel.body.main_color = EDGUI_COLOR_RED;
    rel.body.grad_color = EDGUI_COLOR_BLACK;
    rel.text.color = EDGUI_COLOR_YELLOW;

    static edgui_style_t pr;
    edgui_style_copy(&pr, &edgui_style_btn_tgl_rel);
    pr.body.main_color = EDGUI_COLOR_ORANGE;
    pr.body.grad_color = EDGUI_COLOR_BLACK;
    pr.text.color = EDGUI_COLOR_WHITE;


    edgui_obj_t * btnm2 = edgui_btnm_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_btnm_set_map(btnm2, btnm_map);
    edgui_obj_set_size(btnm2, hres / 2, vres / 3);
    edgui_obj_align(btnm2, btnm1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    edgui_btnm_set_btn_ctrl(btnm2, 2, EDGUI_BTNM_CTRL_TGL_STATE);
    edgui_obj_set_event_cb(btnm2, btnm_event_cb);
    edgui_btnm_set_style(btnm2, EDGUI_BTNM_STYLE_BTN_REL, &rel);
    edgui_btnm_set_style(btnm2, EDGUI_BTNM_STYLE_BTN_PR, &pr);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void btnm_event_cb(edgui_obj_t * btnm, edgui_event_t event)
{
    (void) btnm; /*Unused*/

    if(event != EDGUI_EVENT_CLICKED) return;



#if EDGUI_EX_PRINTF
    const char * txt = edgui_btnm_get_active_btn_text(btnm);
    if(txt) {
        printf("Key pressed: %s\n", txt);
    }
#endif
}

#endif /* EDGUI_USE_BTNM && EDGUI_USE_TESTS*/
