/**
 * @file edgui_test_ta.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_ta.h"

#if EDGUI_USE_TA && EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void nav_btn_event_handler(edgui_obj_t * btn, edgui_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_obj_t * ta2_1;
static edgui_obj_t * ta2_2;
static edgui_obj_t * ta2_3;
static edgui_obj_t * ta2_4;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create text areas to test their basic functionalities
 */
void edgui_test_ta_1(void)
{
    /* Create a text area which looks well */
    edgui_obj_t * ta1 = edgui_ta_create(edgui_disp_get_scr_act(NULL), NULL);

    /*A text area with the following text:
     * Next long test text and testing the automatic       (line break here)
     * and manual line break feature too.
     *(The cursor should be at the end of text)
     *
     *
     * If UTF-8 is enabled these characters are added at the beginning:
     * "űŰöÖ "
     * The cursor should be after these letters*/

    edgui_obj_t * ta2 = edgui_ta_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_ta_set_text(ta2, "New text");
    edgui_ta_set_cursor_pos(ta2, 4);
    edgui_ta_add_text(ta2, "test ");
    edgui_ta_set_cursor_pos(ta2, 4);
    edgui_ta_add_char(ta2, 'l');
    edgui_ta_add_char(ta2, 'o');
    edgui_ta_add_char(ta2, 'n');
    edgui_ta_add_char(ta2, 'g');
    edgui_ta_add_char(ta2, ' ');
    edgui_ta_set_cursor_pos(ta2, EDGUI_TA_CURSOR_LAST);
    edgui_ta_add_text(ta2, " and testing the automatic\n"
                   "and manual line break feature too.");

    edgui_ta_set_cursor_pos(ta2, 0);
    edgui_ta_add_text(ta2, "á");
    edgui_ta_add_text(ta2, "Á");
    edgui_ta_add_text(ta2, "ü");
    edgui_ta_add_text(ta2, "Ü");
    edgui_ta_add_char(ta2, ' ');

    edgui_obj_align(ta2, ta1, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /*Test password mode and one line*/
    edgui_obj_t * ta3 = edgui_ta_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_ta_set_pwd_mode(ta3, true);
    edgui_ta_set_one_line(ta3, true);

    edgui_obj_align(ta3, ta2, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);
    edgui_ta_set_text(ta3, "a A");
    edgui_ta_set_cursor_pos(ta3, 1);
    edgui_ta_add_char(ta3, 'b');
    edgui_ta_add_text(ta3, "é");
    edgui_ta_set_cursor_pos(ta3, 5);

    edgui_ta_add_char(ta3, 'B');
    edgui_ta_add_text(ta3, "É");

    /*Get the password text and set in a new text area*/
    edgui_obj_t * ta4 = edgui_ta_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_ta_set_one_line(ta4, true);
    edgui_ta_set_text(ta4, edgui_ta_get_text(ta3));
    edgui_obj_align(ta4, ta3, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);

}

/**
 * Test cursor modes
 */
void edgui_test_ta_2(void)
{

    static edgui_style_t bg;
    static edgui_style_t sb;
    static edgui_style_t cur;

    edgui_style_copy(&bg, &edgui_style_pretty);
    edgui_style_copy(&sb, &edgui_style_pretty);
    edgui_style_copy(&cur, &edgui_style_pretty);


    bg.body.main_color = EDGUI_COLOR_BLACK;
    bg.body.padding.left = 10;
    bg.body.padding.right = 10;
    bg.body.padding.top = 10;
    bg.body.padding.bottom = 10;
    bg.text.color = EDGUI_COLOR_BLUE;
    bg.text.letter_space = 4;
    bg.text.line_space = 10;

    sb.body.padding.left = 3;
    sb.body.padding.right = 3;
    sb.body.padding.inner = 10;
    sb.body.main_color = EDGUI_COLOR_WHITE;
    sb.body.grad_color = EDGUI_COLOR_WHITE;
    sb.body.opa = EDGUI_OPA_70;

    cur.body.padding.left = 2;
    cur.body.padding.right = 2;
    cur.body.padding.top = 4;
    cur.body.padding.top = 4;
    cur.body.main_color = EDGUI_COLOR_RED;
    cur.body.grad_color = EDGUI_COLOR_YELLOW;
    cur.body.border.color = EDGUI_COLOR_ORANGE;
    cur.body.opa = EDGUI_OPA_70;
    cur.text.color = EDGUI_COLOR_WHITE;
    cur.line.width = 4;


    ta2_1 = edgui_ta_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_ta_set_style(ta2_1, EDGUI_TA_STYLE_BG, &bg);
    edgui_ta_set_style(ta2_1, EDGUI_TA_STYLE_SB, &sb);
    edgui_ta_set_style(ta2_1, EDGUI_TA_STYLE_CURSOR, &cur);
    edgui_ta_set_cursor_type(ta2_1, EDGUI_CURSOR_LINE);
    edgui_ta_set_text(ta2_1, "Some UTF-8 characters "
                   "áÁabcöÖABC\n"
                   "\n"
                   "Í\n"
                   "W\n"
                   "abc");

    ta2_2 = edgui_ta_create(edgui_disp_get_scr_act(NULL), ta2_1);
    edgui_obj_align(ta2_2, ta2_1, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);
    edgui_ta_set_cursor_type(ta2_2, EDGUI_CURSOR_BLOCK);

    ta2_3 = edgui_ta_create(edgui_disp_get_scr_act(NULL), ta2_1);
    edgui_obj_align(ta2_3, ta2_2, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);
    edgui_ta_set_cursor_type(ta2_3, EDGUI_CURSOR_OUTLINE);

    ta2_4 = edgui_ta_create(edgui_disp_get_scr_act(NULL), ta2_1);
    edgui_obj_align(ta2_4, ta2_3, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);
    edgui_ta_set_cursor_type(ta2_4, EDGUI_CURSOR_UNDERLINE);

    edgui_obj_t * btn = edgui_btn_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(btn, ta2_1, EDGUI_ALIGN_OUT_RIGHT_MID, 10, 0);
    edgui_obj_set_event_cb(btn, nav_btn_event_handler);
    edgui_obj_t * label = edgui_label_create(btn, NULL);
    edgui_label_set_text(label, "Up");

    btn = edgui_btn_create(edgui_disp_get_scr_act(NULL), btn);
    edgui_obj_align(btn, btn, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);
    label = edgui_label_create(btn, NULL);
    edgui_label_set_text(label, "Down");

    btn = edgui_btn_create(edgui_disp_get_scr_act(NULL), btn);
    edgui_obj_align(btn, btn, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);
    label = edgui_label_create(btn, NULL);
    edgui_label_set_text(label, "Left");

    btn = edgui_btn_create(edgui_disp_get_scr_act(NULL), btn);
    edgui_obj_align(btn, btn, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);
    label = edgui_label_create(btn, NULL);
    edgui_label_set_text(label, "Right");

    btn = edgui_btn_create(edgui_disp_get_scr_act(NULL), btn);
    edgui_obj_align(btn, btn, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 10);
    label = edgui_label_create(btn, NULL);
    edgui_label_set_text(label, "Del");



}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void nav_btn_event_handler(edgui_obj_t * btn, edgui_event_t event)
{
    if(event != EDGUI_EVENT_SHORT_CLICKED) return;


    edgui_obj_t * label = edgui_obj_get_child(btn, NULL);
    const char * txt = edgui_label_get_text(label);

    if(strcmp(txt, "Up") == 0) {
        edgui_ta_cursor_up(ta2_1);
        edgui_ta_cursor_up(ta2_2);
        edgui_ta_cursor_up(ta2_3);
        edgui_ta_cursor_up(ta2_4);
    }
    else if(strcmp(txt, "Down") == 0) {
        edgui_ta_cursor_down(ta2_1);
        edgui_ta_cursor_down(ta2_2);
        edgui_ta_cursor_down(ta2_3);
        edgui_ta_cursor_down(ta2_4);
    }
    else if(strcmp(txt, "Left") == 0) {
        edgui_ta_cursor_left(ta2_1);
        edgui_ta_cursor_left(ta2_2);
        edgui_ta_cursor_left(ta2_3);
        edgui_ta_cursor_left(ta2_4);
    }
    else if(strcmp(txt, "Right") == 0) {
        edgui_ta_cursor_right(ta2_1);
        edgui_ta_cursor_right(ta2_2);
        edgui_ta_cursor_right(ta2_3);
        edgui_ta_cursor_right(ta2_4);
    }
}
#endif /*EDGUI_USE_TA && EDGUI_USE_TESTS*/
