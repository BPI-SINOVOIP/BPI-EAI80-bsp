/**
 * @file edgui_test_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_label.h"

#if EDGUI_USE_LABEL && EDGUI_USE_TESTS

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
 * Create labels with dynamic, static and array texts
 */
void edgui_test_label_1(void)
{
    /* Default object*/
    edgui_obj_t * label1 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);

    /* Set label text to "I'm testing\nthe labels" */
    edgui_obj_t * label2 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_label_set_text(label2, "I'm testing\nthe labels");
    edgui_obj_align(label2, label1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /* Set a static array as text and modify a letter later (Goal is "STATic text")*/
    static char label_static_text[] =  {"static text"};
    edgui_obj_t * label3 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_label_set_static_text(label3, label_static_text);
    edgui_obj_align(label3, label2, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    label_static_text[0] = 'S';         /*Randomly modify letters*/
    label_static_text[1] = 'T';
    label_static_text[2] = 'A';
    label_static_text[3] = 'T';
    edgui_label_set_text(label3, NULL);    /*Refresh after modification*/

    /* Set text from array*/
    char array_text[3]; /*Not static to see the text will remain after this variable is destroyed*/
    array_text[0] = 'a';
    array_text[1] = 'b';
    array_text[2] = 'c';    /*Not need to be '\0' terminated*/
    edgui_obj_t * label4 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_label_set_array_text(label4, array_text, sizeof(array_text));
    edgui_obj_align(label4, label3, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);

    /* Copy 'label2' (dynamic) and set style and background*/
    edgui_obj_t * label5 = edgui_label_create(edgui_disp_get_scr_act(NULL), label2);
    edgui_obj_set_style(label5, &edgui_style_pretty_color);
    edgui_obj_align(label5, label2, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    edgui_label_set_body_draw(label5, true);

    /* Copy 'label3' (static) and set style and background*/
    edgui_obj_t * label6 = edgui_label_create(edgui_disp_get_scr_act(NULL), label3);
    edgui_obj_set_style(label6, &edgui_style_pretty_color);
    edgui_obj_align(label6, label3, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    edgui_label_set_body_draw(label6, true);

    /* Copy 'label4' (array) and set style and background*/
    edgui_obj_t * label7 = edgui_label_create(edgui_disp_get_scr_act(NULL), label4);
    edgui_obj_set_style(label7, &edgui_style_pretty_color);
    edgui_obj_align(label7, label4, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    edgui_label_set_body_draw(label7, true);
}

/**
 * Test label long modes
 */
void edgui_test_label_2(void)
{
    /* Test EDGUI_LABEL_LONG_EXPAND (default)
     * GOAL: A label with a long line*/
    edgui_obj_t * label1 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_style(label1, &edgui_style_plain_color);    /*Set a background to clearly see the label size*/
    edgui_label_set_body_draw(label1, true);
    edgui_label_set_text(label1, "This is a very long line which is not broken.");
    edgui_label_set_long_mode(label1, EDGUI_LABEL_LONG_EXPAND);

    /* EDGUI_LABEL_LONG_BERAK (set width and test line break)
     * GOAL: the words are wrapped into multiple lines */
    edgui_obj_t * label2 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_label_set_body_draw(label2, true);
    edgui_obj_set_style(label2, &edgui_style_plain_color);
    edgui_label_set_text(label2, "This is a long line and a VeryVeryLongWordToWrap.\n"
                      "A new line and a lot of spaces:                        . Can you see them?");
    edgui_label_set_long_mode(label2, EDGUI_LABEL_LONG_BREAK);
    edgui_obj_align(label2, label1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    edgui_obj_set_width(label2, 100);

    /* EDGUI_LABEL_LONG_ROLL (set size and test rolling)
     * GOAL: the text is rolled in both directions*/
    edgui_obj_t * label3 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_label_set_body_draw(label3, true);
    edgui_obj_set_style(label3, &edgui_style_plain_color);
    edgui_label_set_text(label3, "Long line to roll!");
    edgui_label_set_long_mode(label3, EDGUI_LABEL_LONG_SROLL);
    edgui_obj_align(label3, label2, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    edgui_obj_set_size(label3, 100, 50);

    /* EDGUI_LABEL_LONG_ROLL (set size and test rolling)
     * GOAL: the text is rolled circularly*/
    edgui_obj_t * label4 = edgui_label_create(edgui_scr_act(), label3);
    edgui_obj_set_style(label4, &edgui_style_plain_color);
    edgui_label_set_text(label4, "Long line to roll circularly!");
//    edgui_label_set_body_draw(label4, true);
    edgui_label_set_long_mode(label4, EDGUI_LABEL_LONG_SROLL_CIRC);
    edgui_obj_align(label4, label3, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /* EDGUI_LABEL_LONG_DOTS (set size and a long text)
     * GOAL: see dots at the end of the size */
    edgui_obj_t * label5 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_style(label5, &edgui_style_plain_color);
    edgui_label_set_body_draw(label5, true);
    edgui_label_set_long_mode(label5, EDGUI_LABEL_LONG_DOT);
    edgui_obj_set_size(label5, 100, 60);
    edgui_label_set_text(label5, "Dots: aáeéiíoóuúAÁEÉIÍOÓUÚ");
    edgui_obj_align(label5, label4, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Restore dots*/
    edgui_obj_t * label6 = edgui_label_create(edgui_disp_get_scr_act(NULL), label5);
    edgui_label_set_long_mode(label6, EDGUI_LABEL_LONG_EXPAND);
    edgui_obj_align(label6, label5, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
}


/**
 * Test text insert and cut
 */
void edgui_test_label_3(void)
{
    /*Test inserting*/
    edgui_obj_t * label1 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(label1, 10, 10);
    edgui_label_set_text(label1, "Test insert");
    edgui_label_ins_text(label1, 4, " the");
    edgui_label_ins_text(label1, 0, "I will ");
    edgui_label_ins_text(label1, EDGUI_LABEL_POS_LAST, " feature");

    edgui_label_ins_text(label1, 7, "(UTF-8: aÁoÓ) ");

    edgui_obj_t * label2 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(label2, label1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    edgui_label_set_text(label2, "Characters to delete: abcd aÁ uÚ üŰ");
    edgui_label_cut_text(label2, 4, 5);
    edgui_label_cut_text(label2, 21, 3);
}

/**
 * Test mixed features
 */
void edgui_test_label_4(void)
{
    /* Create a label with '\r', '\n', '\r\n' and '\n\r' line breaks
     * GOAL: The text in 5 lines without empty lines*/
    edgui_obj_t * label1 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_label_set_text(label1, "Line1\n"
                      "Line2\r"
                      "Line3\r\n"
                      "Line4");

    /* Test recoloring
     * GOAL: the word "red" is red*/
    edgui_obj_t * label3 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_label_set_text(label3, "This is a #ff0000 red# word");
    edgui_label_set_recolor(label3, true);
    edgui_obj_align(label3, label1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /* Test UTF-8 support with EDGUI_LABEL_LONG_BREAK, new lines and recolor
     * GOAL: the word "red" is red*/
    edgui_obj_t * label4 = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_label_set_text(label4, "Normal ASCII\n"
                      "UTF-8 letters:áÁééőŐöÖúÚűŰ\n"
                      "Recolor UTF-8: #ff0000 öŐ##00ff00 üŰ##0000ff éÉ#");
    edgui_label_set_recolor(label4, true);
    edgui_label_set_long_mode(label4, EDGUI_LABEL_LONG_BREAK);
    edgui_obj_set_width(label4, 100);
    edgui_obj_align(label4, label3, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_LABEL && EDGUI_USE_TESTS*/
