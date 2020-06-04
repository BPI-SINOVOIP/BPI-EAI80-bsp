/**
 * @file edgui_test_page.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_page.h"

#if EDGUI_USE_PAGE && EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void event_handler(edgui_obj_t * page, edgui_event_t event);

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
 * Create pages to test their basic functionalities
 */
void edgui_test_page_1(void)
{
    /*Create a page which should look well*/
    edgui_obj_t * page1 = edgui_page_create(edgui_disp_get_scr_act(NULL), NULL);

    /*Resize the page*/
    edgui_obj_t * page2 = edgui_page_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(page2, EDGUI_DPI, EDGUI_DPI * 2);
    edgui_obj_align(page2, page1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    /*Add some text to text the scrolling*/
    edgui_obj_t * page3 = edgui_page_create(edgui_disp_get_scr_act(NULL), page2);
    edgui_obj_set_size(page3, EDGUI_DPI, EDGUI_DPI * 2);
    edgui_obj_align(page3, page2, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);

    edgui_obj_t * label = edgui_label_create(page3, NULL);
    edgui_label_set_text(label, "First line of a text\n"
                      "Second line of a text\n"
                      "Third line of a text\n"
                      "Forth line of a text\n"
                      "Fifth line of a text\n"
                      "Sixth line of a text\n"
                      "Seventh line of a text\n"
                      "Eight line of a text\n"
                      "Ninth line of a text\n"
                      "Tenth line of a text\n");

    /*Enable horizontal fit to set scrolling in both directions*/
    edgui_obj_t * page4 = edgui_page_create(edgui_disp_get_scr_act(NULL), page3);
    edgui_obj_align(page4, page3, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    label = edgui_label_create(page4, label);

}

/**
 * Test styling, scrollbar modes, layout and action
 */
void edgui_test_page_2(void)
{
    static edgui_style_t bg;
    static edgui_style_t scrl;
    static edgui_style_t sb;

    edgui_style_copy(&bg, &edgui_style_pretty);
    edgui_style_copy(&scrl, &edgui_style_pretty);
    edgui_style_copy(&sb, &edgui_style_pretty);

    bg.body.main_color = EDGUI_COLOR_SILVER;
    bg.body.grad_color = EDGUI_COLOR_GRAY;
    bg.body.padding.left = 5;
    bg.body.padding.right = 5;
    bg.body.padding.top = 20;
    bg.body.padding.bottom = 20;

    scrl.body.main_color = EDGUI_COLOR_BLUE;
    scrl.body.grad_color = EDGUI_COLOR_NAVY;
    scrl.body.padding.left = 3;
    scrl.body.padding.right = 3;
    scrl.body.padding.top = 3;
    scrl.body.padding.bottom = 3;
    scrl.body.shadow.width = 15;
    scrl.text.color = EDGUI_COLOR_SILVER;

    sb.body.padding.right = -10;    /*Out of the page*/
    sb.body.padding.bottom = 10;
    sb.body.padding.inner = 10;
    sb.body.main_color = EDGUI_COLOR_WHITE;
    sb.body.grad_color = EDGUI_COLOR_WHITE;
    sb.body.opa = EDGUI_OPA_70;

    /* Create a page with new style, layout, fit, action and scrollbar OFF*/
    edgui_obj_t * page1 = edgui_page_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(page1, EDGUI_DPI, EDGUI_DPI * 2);
    edgui_page_set_scrl_layout(page1, EDGUI_LAYOUT_COL_L);
    edgui_page_set_sb_mode(page1, EDGUI_SB_MODE_OFF);
    edgui_obj_set_event_cb(page1, event_handler);
    edgui_page_set_style(page1, EDGUI_PAGE_STYLE_BG, &bg);
    edgui_page_set_style(page1, EDGUI_PAGE_STYLE_SCRL, &scrl);
    edgui_page_set_style(page1, EDGUI_PAGE_STYLE_SB, &sb);

    edgui_obj_t * label = edgui_label_create(page1, NULL);
    edgui_label_set_text(label, "First line of a text\n"
                      "Second line of a text\n"
                      "Third line of a text\n"
                      "Forth line of a text\n"
                      "Fifth line of a text\n");

    /*Copy 'page1' and set scrollbar ON*/
    edgui_obj_t * page2 = edgui_page_create(edgui_disp_get_scr_act(NULL), page1);
    edgui_obj_align(page2, page1, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    label = edgui_label_create(page2, label);
    edgui_page_set_sb_mode(page2, EDGUI_SB_MODE_ON);

    /*Copy 'page1' and set scrollbar AUTO*/
    edgui_obj_t * page3 = edgui_page_create(edgui_disp_get_scr_act(NULL), page1);
    edgui_obj_align(page3, page2, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    label = edgui_label_create(page3, label);
    edgui_page_set_sb_mode(page3, EDGUI_SB_MODE_AUTO);

    /*Copy 'page1' and set scrollbar DRAG*/
    edgui_obj_t * page4 = edgui_page_create(edgui_disp_get_scr_act(NULL), page1);
    edgui_obj_align(page4, page3, EDGUI_ALIGN_OUT_RIGHT_TOP, 20, 0);
    label = edgui_label_create(page4, label);
    edgui_page_set_sb_mode(page4, EDGUI_SB_MODE_DRAG);


}


/**********************
 *   STATIC FUNCTIONS
 **********************/


static void event_handler(edgui_obj_t * page, edgui_event_t event)
{
    if(event == EDGUI_EVENT_SHORT_CLICKED) {
        edgui_obj_t * label = edgui_label_create(page, NULL);
        edgui_label_set_text(label, "First line of a text\n"
                "Second line of a text\n"
                "Third line of a text\n"
                "Forth line of a text\n"
                "Fifth line of a text\n");
    }
}


#endif /*EDGUI_USE_PAGE && EDGUI_USE_TESTS*/
