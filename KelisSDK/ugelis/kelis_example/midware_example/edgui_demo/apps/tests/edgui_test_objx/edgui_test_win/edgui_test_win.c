/**
 * @file edgui_test_win.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_win.h"

#if EDGUI_USE_WIN && EDGUI_USE_TESTS

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
 * Create windows to test their functionalities
 */
void edgui_test_win_1(void)
{
    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    edgui_obj_t * win1 = edgui_win_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(win1, hres / 2 - EDGUI_DPI / 20, vres / 2 - EDGUI_DPI / 20);

    edgui_obj_t * win2 = edgui_win_create(edgui_disp_get_scr_act(NULL), win1);
    edgui_obj_align(win2, NULL, EDGUI_ALIGN_IN_TOP_RIGHT, 0, 0);
    edgui_win_set_title(win2, "Random title");
    edgui_win_add_btn(win2, EDGUI_SYMBOL_CLOSE);
    edgui_win_add_btn(win2, EDGUI_SYMBOL_OK);
    edgui_win_add_btn(win2, EDGUI_SYMBOL_EDIT);

    edgui_obj_t * label = edgui_label_create(win2, NULL);
    edgui_obj_set_pos(label, 10, 10);
    edgui_label_set_text(label, "Long\n\n\ntext\n\n\nto\n\n\nsee\n\n\nthe\n\n\nscrollbars");


    static edgui_style_t header;
    edgui_style_copy(&header, &edgui_style_plain);
    header.body.main_color = EDGUI_COLOR_RED;
    header.body.grad_color = EDGUI_COLOR_MAROON;
    header.body.padding.inner = 0;
    header.text.color = EDGUI_COLOR_WHITE;

    edgui_obj_t * win3 = edgui_win_create(edgui_disp_get_scr_act(NULL), win2);
    edgui_obj_align(win3, NULL, EDGUI_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    edgui_win_set_style(win3, EDGUI_WIN_STYLE_HEADER, &header);
    edgui_win_set_style(win3, EDGUI_WIN_STYLE_BTN_REL, &edgui_style_transp);
    edgui_win_set_style(win3, EDGUI_WIN_STYLE_BG, &edgui_style_plain_color);
    edgui_win_set_btn_size(win3, EDGUI_DPI / 3);

    label = edgui_label_create(win3, NULL);
    edgui_obj_set_pos(label, 10, 10);
    edgui_label_set_text(label, "Styled window\n\nThe content background has\ndifferent color");

    edgui_obj_t * win4 = edgui_win_create(edgui_disp_get_scr_act(NULL), win3);
    edgui_obj_align(win4, NULL, EDGUI_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_WIN && EDGUI_USE_TESTS*/
