/**
 * @file edgui_test_tabview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_tabview.h"

#if EDGUI_USE_TABVIEW && EDGUI_USE_TESTS

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
 * Create tab views to test their functionalities
 */
void edgui_test_tabview_1(void)
{
    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    /* Default object. It will be an empty tab view*/
    edgui_obj_t * tv1 = edgui_tabview_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_tabview_add_tab(tv1, "First");
    edgui_tabview_add_tab(tv1, "Second");
    edgui_obj_set_size(tv1, hres / 2 - 10, vres / 2 - 10);

    /*Copy the first tabview and add some texts*/
    edgui_obj_t * tv2 = edgui_tabview_create(edgui_disp_get_scr_act(NULL), tv1);
    edgui_obj_align(tv2, NULL, EDGUI_ALIGN_IN_TOP_RIGHT, 0, 0);

    edgui_obj_t * tab = edgui_tabview_get_tab(tv2, 0);
    edgui_obj_t * label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = edgui_tabview_get_tab(tv2, 1);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "This is the second tab");


    /*Create styles*/
    static edgui_style_t bg;
    static edgui_style_t sb;
    static edgui_style_t btns_bg;
    static edgui_style_t tab_bg;
    static edgui_style_t rel;
    static edgui_style_t pr;
    static edgui_style_t tgl_rel;
    static edgui_style_t tgl_pr;
    static edgui_style_t indic;

    edgui_style_copy(&btns_bg, &edgui_style_plain_color);
    edgui_style_copy(&tab_bg, &edgui_style_pretty_color);
    edgui_style_copy(&bg, &edgui_style_pretty_color);
    edgui_style_copy(&sb, &edgui_style_pretty);
    edgui_style_copy(&btns_bg, &edgui_style_transp_fit);
    edgui_style_copy(&rel, &edgui_style_plain);
    edgui_style_copy(&pr, &edgui_style_plain);
    edgui_style_copy(&tgl_rel, &edgui_style_plain);
    edgui_style_copy(&tgl_pr, &edgui_style_plain);
    edgui_style_copy(&indic, &edgui_style_plain);

    rel.body.main_color = EDGUI_COLOR_SILVER;
    pr.body.main_color = EDGUI_COLOR_GRAY;
    tgl_rel.body.main_color = EDGUI_COLOR_RED;
    tgl_pr.body.main_color = EDGUI_COLOR_MAROON;
    indic.body.main_color = EDGUI_COLOR_ORANGE;
    indic.body.grad_color = EDGUI_COLOR_ORANGE;
    indic.body.padding.inner = EDGUI_DPI / 16;
    tab_bg.body.main_color = EDGUI_COLOR_SILVER;
    tab_bg.body.grad_color = EDGUI_COLOR_GREEN;
    tab_bg.text.color = EDGUI_COLOR_YELLOW;

    /*Apply the styles*/
    edgui_obj_t * tv3 = edgui_tabview_create(edgui_disp_get_scr_act(NULL), tv2);
    edgui_obj_align(tv3, NULL, EDGUI_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    edgui_tabview_set_style(tv3, EDGUI_TABVIEW_STYLE_BG, &bg);
    edgui_tabview_set_style(tv3, EDGUI_TABVIEW_STYLE_BTN_BG, &btns_bg);
    edgui_tabview_set_style(tv3, EDGUI_TABVIEW_STYLE_BTN_REL, &rel);
    edgui_tabview_set_style(tv3, EDGUI_TABVIEW_STYLE_BTN_PR, &pr);
    edgui_tabview_set_style(tv3, EDGUI_TABVIEW_STYLE_BTN_TGL_REL, &tgl_rel);
    edgui_tabview_set_style(tv3, EDGUI_TABVIEW_STYLE_BTN_TGL_PR, &tgl_pr);
    edgui_tabview_set_style(tv3, EDGUI_TABVIEW_STYLE_INDIC, &indic);

    tab = edgui_tabview_get_tab(tv3, 0);
    edgui_page_set_style(tab, EDGUI_PAGE_STYLE_BG, &tab_bg);
    edgui_page_set_style(tab, EDGUI_PAGE_STYLE_SB, &sb);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = edgui_tabview_get_tab(tv3, 1);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "This is the second tab");

    /*Copy the styles tab view*/
    edgui_obj_t * tv4 = edgui_tabview_create(edgui_disp_get_scr_act(NULL), tv3);
    edgui_obj_align(tv4, NULL, EDGUI_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
}

void edgui_test_tabview_2(void)
{
    edgui_obj_t * tab;
    edgui_obj_t * label;

    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    // Tabview 1
    edgui_obj_t * tv1 = edgui_tabview_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(tv1, hres / 2 - 10, vres / 2 - 10);
    edgui_obj_align(tv1, NULL, EDGUI_ALIGN_IN_TOP_LEFT, 0, 0);
    edgui_tabview_set_btns_pos(tv1, EDGUI_TABVIEW_BTNS_POS_TOP);

    edgui_tabview_add_tab(tv1, "111");
    edgui_tabview_add_tab(tv1, "222");
    edgui_tabview_add_tab(tv1, "333");
    edgui_tabview_add_tab(tv1, "444");

    tab = edgui_tabview_get_tab(tv1, 0);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "1111This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = edgui_tabview_get_tab(tv1, 1);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "2222");

    tab = edgui_tabview_get_tab(tv1, 2);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "3333");

    tab = edgui_tabview_get_tab(tv1, 3);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "444");

    // Tabview 2
    edgui_obj_t * tv2 = edgui_tabview_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(tv2, hres / 2 - 10, vres / 2 - 10);
    edgui_obj_align(tv2, NULL, EDGUI_ALIGN_IN_TOP_RIGHT, 0, 0);
    edgui_tabview_set_btns_pos(tv2, EDGUI_TABVIEW_BTNS_POS_BOTTOM);

    edgui_tabview_add_tab(tv2, "111");
    edgui_tabview_add_tab(tv2, "222");
    edgui_tabview_add_tab(tv2, "333");
    edgui_tabview_add_tab(tv2, "444");        

    tab = edgui_tabview_get_tab(tv2, 0);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "1111This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = edgui_tabview_get_tab(tv2, 1);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "2222");

    tab = edgui_tabview_get_tab(tv2, 2);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "3333");

    tab = edgui_tabview_get_tab(tv2, 3);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "444");

    // Tabview 3
    edgui_obj_t * tv3 = edgui_tabview_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(tv3, hres / 2 - 10, vres / 2 - 10);
    edgui_obj_align(tv3, NULL, EDGUI_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    edgui_tabview_set_btns_pos(tv3, EDGUI_TABVIEW_BTNS_POS_LEFT);

    edgui_tabview_add_tab(tv3, "111");
    edgui_tabview_add_tab(tv3, "222");
    edgui_tabview_add_tab(tv3, "333");
    edgui_tabview_add_tab(tv3, "444");        

    tab = edgui_tabview_get_tab(tv3, 0);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "1111This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = edgui_tabview_get_tab(tv3, 1);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "2222");

    tab = edgui_tabview_get_tab(tv3, 2);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "3333");

    tab = edgui_tabview_get_tab(tv3, 3);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "444");

    // Tabview 4
    edgui_obj_t * tv4 = edgui_tabview_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(tv4, hres / 2 - 10, vres / 2 - 10);
    edgui_obj_align(tv4, NULL, EDGUI_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    edgui_tabview_set_btns_pos(tv4, EDGUI_TABVIEW_BTNS_POS_RIGHT);

    edgui_tabview_add_tab(tv4, "111");
    edgui_tabview_add_tab(tv4, "222");
    edgui_tabview_add_tab(tv4, "333");
    edgui_tabview_add_tab(tv4, "444");        

    tab = edgui_tabview_get_tab(tv4, 0);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "1111This is\n\n\nA long text\n\n\ntext\n\n\non the\n\n\nsecond\n\n\ntab\n\n\nto see\n\n\nthe scrolling");

    tab = edgui_tabview_get_tab(tv4, 1);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "2222");

    tab = edgui_tabview_get_tab(tv4, 2);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "3333");

    tab = edgui_tabview_get_tab(tv4, 3);
    label = edgui_label_create(tab, NULL);
    edgui_label_set_text(label, "444");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TABVIEW && EDGUI_USE_TESTS*/
