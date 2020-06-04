/**
 * @file edgui_test_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_list.h"

#if EDGUI_USE_LIST && EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void list_move_event_handler(edgui_obj_t * btn, edgui_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_obj_t * list1;
static edgui_obj_t * list2;
static edgui_obj_t * list3;
static edgui_obj_t * list4;

static edgui_obj_t * btn_up;
static edgui_obj_t * btn_down;

EDGUI_IMG_DECLARE(img_flower_icon) /*Comes from edgui_test_img*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create lists to test their functionalities
 */
void edgui_test_list_1(void)
{
    /* Default object. It will be an empty list*/
    list1 = edgui_list_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(list1, 10, 10);

    list2 = edgui_list_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(list2, list1, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    edgui_list_add_btn(list2, EDGUI_SYMBOL_FILE, "File");
    edgui_list_add_btn(list2, EDGUI_SYMBOL_DIRECTORY, "Directory");
    edgui_list_add_btn(list2, &img_flower_icon, "Image icon");
    edgui_obj_set_width(list2, 100);

    list3 = edgui_list_create(edgui_disp_get_scr_act(NULL), list2);
    edgui_obj_align(list3, list2, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    edgui_list_add_btn(list3, NULL, "No icon");
    edgui_list_add_btn(list3, EDGUI_SYMBOL_CLOSE, "");
    edgui_list_add_btn(list3, EDGUI_SYMBOL_UP, "Up");
    edgui_list_add_btn(list3, EDGUI_SYMBOL_DOWN, "Down");

    static edgui_style_t sb;
    static edgui_style_t bg;
    edgui_style_copy(&sb, &edgui_style_pretty_color);
    edgui_style_copy(&bg, &edgui_style_pretty_color);
    sb.body.padding.right = -10;
    sb.body.padding.inner = 10;

    bg.body.padding.left = 20;
    bg.body.padding.right = 20;

    list4 = edgui_list_create(edgui_disp_get_scr_act(NULL), list3);
    edgui_list_set_style(list4, EDGUI_LIST_STYLE_BG, &bg);
    edgui_list_set_style(list4, EDGUI_LIST_STYLE_SB, &sb);
    edgui_obj_align(list4, list3, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    edgui_obj_set_width(list4, 200);

    /*Add list up/down buttons*/
    btn_up = edgui_btn_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(btn_up, list1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    edgui_obj_set_event_cb(btn_up, list_move_event_handler);
    edgui_obj_t * label = edgui_label_create(btn_up, NULL);
    edgui_label_set_text(label, EDGUI_SYMBOL_UP);

    btn_down = edgui_btn_create(edgui_disp_get_scr_act(NULL), btn_up);
    edgui_obj_align(btn_down, btn_up, EDGUI_ALIGN_OUT_RIGHT_MID, 10, 0);
    label = edgui_label_create(btn_down, NULL);
    edgui_label_set_text(label, EDGUI_SYMBOL_DOWN);

}



/**********************
 *   STATIC FUNCTIONS
 **********************/

static void list_move_event_handler(edgui_obj_t * btn, edgui_event_t event)
{
    if(event != EDGUI_EVENT_SHORT_CLICKED) return;

    if(btn == btn_up) {
        edgui_list_up(list1);
        edgui_list_up(list2);
        edgui_list_up(list3);
        edgui_list_up(list4);
    } else if(btn == btn_down) {
        edgui_list_down(list1);
        edgui_list_down(list2);
        edgui_list_down(list3);
        edgui_list_down(list4);
    }
}

#endif /*EDGUI_USE_LIST && EDGUI_USE_TESTS*/
