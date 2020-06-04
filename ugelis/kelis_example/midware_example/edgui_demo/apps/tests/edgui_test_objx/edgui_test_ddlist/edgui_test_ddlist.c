/**
 * @file edgui_test_ddlist.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "edgui_test_ddlist.h"

#if EDGUI_EX_PRINTF
#include <stdio.h>
#endif


#if EDGUI_USE_DDLIST && EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void event_handler(edgui_obj_t * ddlist, edgui_event_t event);

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
 * Create drop down lists to test their functionalities
 */
void edgui_test_ddlist_1(void)
{
    /* Create a default object*/
    edgui_obj_t * ddlist1 = edgui_ddlist_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(ddlist1, 10, 10);

    /* Create a drop down list with a lot of options, fix height and anim time.
     * Open it by default without animation and assign an action*/
    edgui_obj_t * ddlist2 = edgui_ddlist_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(ddlist2, ddlist1, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);
    edgui_ddlist_set_options(ddlist2, "First\nSecond\nThird\nForth\nFifth\nSixth");
    edgui_ddlist_set_fix_height(ddlist2, EDGUI_DPI);
    edgui_ddlist_set_selected(ddlist2, 2);
    edgui_ddlist_set_anim_time(ddlist2, 100);
    edgui_ddlist_open(ddlist2, false);
    edgui_ddlist_set_fix_width(ddlist2, EDGUI_DPI * 2);
    edgui_obj_set_event_cb(ddlist2, event_handler);

    /*Copy the previous drop down list and modify its style*/
    static edgui_style_t ddlist3_style;
    edgui_style_copy(&ddlist3_style, &edgui_style_pretty);
    ddlist3_style.body.main_color = EDGUI_COLOR_GRAY;
    ddlist3_style.body.grad_color = EDGUI_COLOR_BLACK;
    ddlist3_style.body.padding.left = 20;
    ddlist3_style.body.padding.right = 20;
    ddlist3_style.body.padding.top = 30;
    ddlist3_style.body.padding.bottom = 30;

    ddlist3_style.text.color = EDGUI_COLOR_RED;
    ddlist3_style.text.letter_space = 5;
    ddlist3_style.text.line_space = 15;

    edgui_obj_t * ddlist3 = edgui_ddlist_create(edgui_disp_get_scr_act(NULL), ddlist2);
    edgui_obj_align(ddlist3, ddlist2, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    edgui_ddlist_set_style(ddlist3, EDGUI_DDLIST_STYLE_BG, &ddlist3_style);
    edgui_ddlist_set_style(ddlist3, EDGUI_DDLIST_STYLE_SEL, &edgui_style_plain_color);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void event_handler(edgui_obj_t * ddlist, edgui_event_t event)
{
    if(event == EDGUI_EVENT_VALUE_CHANGED) {

#if EDGUI_EX_PRINTF
    char buf[64];
    edgui_ddlist_get_selected_str(ddlist, buf, sizeof(buf));
    printf("New option selected on a drop down list: %s\n", buf);
#endif
    }
}

#endif /*EDGUI_USE_DDLIST && EDGUI_USE_TESTS*/

