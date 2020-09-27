/**
 * @file edgui_test_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "edgui_test_kb.h"

#if EDGUI_USE_KB && EDGUI_USE_TESTS

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
 * Create a default object and test the basic functions
 */
void edgui_test_kb_1(void)
{

    edgui_obj_t * ta = edgui_ta_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(ta, NULL, EDGUI_ALIGN_IN_TOP_MID, 0, 30);

    /* Default object*/
    edgui_obj_t * kb1 = edgui_kb_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(kb1, NULL, EDGUI_ALIGN_IN_BOTTOM_MID, 0, 0);
    edgui_kb_set_ta(kb1, ta);
}

/**
 * Create a styles keyboard
 */
void edgui_test_kb_2(void)
{

    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    edgui_obj_t * ta = edgui_ta_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(ta, NULL, EDGUI_ALIGN_IN_TOP_MID, 0, 30);

    /* Default object*/
    edgui_obj_t * kb1 = edgui_kb_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(kb1, hres / 2, vres / 4);
    edgui_obj_align(kb1, NULL, EDGUI_ALIGN_IN_BOTTOM_MID, 0, 0);
    edgui_kb_set_ta(kb1, ta);

    static edgui_style_t bg;
    static edgui_style_t rel;
    static edgui_style_t pr;

    edgui_style_copy(&bg, &edgui_style_plain_color);
    bg.body.main_color = EDGUI_COLOR_NAVY;
    bg.body.grad_color = EDGUI_COLOR_NAVY;
    bg.body.padding.left = 0;
    bg.body.padding.right = 0;
    bg.body.padding.top = 10;
    bg.body.padding.bottom = 10;
    bg.body.padding.inner = 0;

    edgui_style_copy(&rel, &edgui_style_plain);
    rel.body.border.width = 1;
    rel.body.main_color = EDGUI_COLOR_WHITE;
    rel.body.grad_color = EDGUI_COLOR_SILVER;
    rel.body.grad_color = EDGUI_COLOR_SILVER;
    rel.text.color = EDGUI_COLOR_NAVY;
    edgui_style_copy(&pr, &edgui_style_plain_color);

    edgui_kb_set_style(kb1, EDGUI_KB_STYLE_BG, &bg);
    edgui_kb_set_style(kb1, EDGUI_KB_STYLE_BTN_REL, &rel);
    edgui_kb_set_style(kb1, EDGUI_KB_STYLE_BTN_PR, &pr);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_KB && EDGUI_USE_TESTS*/
