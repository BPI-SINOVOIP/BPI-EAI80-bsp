/**
 * @file edgui_test_theme_1.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_theme_1.h"

#if EDGUI_USE_TESTS
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_tab1(edgui_obj_t * parent);
static void create_tab2(edgui_obj_t * parent);
static void create_tab3(edgui_obj_t * parent);

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
 * Create a test screen with a lot objects and apply the given theme on them
 * @param th pointer to a theme
 */
void edgui_test_theme_1(edgui_theme_t * th)
{
    edgui_theme_set_current(th);
    th = edgui_theme_get_current();    /*If `EDGUI_THEME_LIVE_UPDATE  1` `th` is not used directly so get the real theme after set*/
    edgui_obj_t * scr = edgui_cont_create(NULL, NULL);
    edgui_disp_load_scr(scr);

    edgui_obj_t * tv = edgui_tabview_create(scr, NULL);
    edgui_obj_set_size(tv, edgui_disp_get_hor_res(NULL), edgui_disp_get_ver_res(NULL));
    edgui_obj_t * tab1 = edgui_tabview_add_tab(tv, "Tab 1");
    edgui_obj_t * tab2 = edgui_tabview_add_tab(tv, "Tab 2");
    edgui_obj_t * tab3 = edgui_tabview_add_tab(tv, "Tab 3");

    create_tab1(tab1);
    create_tab2(tab2);
    create_tab3(tab3);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void create_tab1(edgui_obj_t * parent)
{
    edgui_page_set_scrl_layout(parent, EDGUI_LAYOUT_PRETTY);

    edgui_theme_t * th = edgui_theme_get_current();

    static edgui_style_t h_style;
    edgui_style_copy(&h_style, &edgui_style_transp);
    h_style.body.padding.inner = EDGUI_DPI / 10;
    h_style.body.padding.left = EDGUI_DPI / 4;
    h_style.body.padding.right = EDGUI_DPI / 4;
    h_style.body.padding.top = EDGUI_DPI / 10;
    h_style.body.padding.bottom = EDGUI_DPI / 10;

    edgui_obj_t * h = edgui_cont_create(parent, NULL);
    edgui_obj_set_style(h, &h_style);
    edgui_obj_set_click(h, false);
    edgui_cont_set_fit(h, EDGUI_FIT_TIGHT);
    edgui_cont_set_layout(h, EDGUI_LAYOUT_COL_M);

    edgui_obj_t * btn = edgui_btn_create(h, NULL);
    edgui_btn_set_fit(btn, EDGUI_FIT_TIGHT);
    edgui_btn_set_toggle(btn, true);
    edgui_obj_t * btn_label = edgui_label_create(btn, NULL);
    edgui_label_set_text(btn_label, "Button");

    btn = edgui_btn_create(h, btn);
    edgui_btn_toggle(btn);
    btn_label = edgui_label_create(btn, NULL);
    edgui_label_set_text(btn_label, "Toggled");

    btn = edgui_btn_create(h, btn);
    edgui_btn_set_state(btn, EDGUI_BTN_STATE_INA);
    btn_label = edgui_label_create(btn, NULL);
    edgui_label_set_text(btn_label, "Inactive");

    edgui_obj_t * label = edgui_label_create(h, NULL);
    edgui_label_set_text(label, "Primary");
    edgui_obj_set_style(label, th->style.label.prim);

    label = edgui_label_create(h, NULL);
    edgui_label_set_text(label, "Secondary");
    edgui_obj_set_style(label, th->style.label.sec);

    label = edgui_label_create(h, NULL);
    edgui_label_set_text(label, "Hint");
    edgui_obj_set_style(label, th->style.label.hint);

    static const char * btnm_str[] = {"1", "2", "3", EDGUI_SYMBOL_OK, EDGUI_SYMBOL_CLOSE, ""};
    edgui_obj_t * btnm = edgui_btnm_create(h, NULL);
    edgui_obj_set_size(btnm, edgui_disp_get_hor_res(NULL) / 4, 2 * EDGUI_DPI / 3);
    edgui_btnm_set_map(btnm, btnm_str);
    edgui_btnm_set_btn_ctrl_all(btnm, EDGUI_BTNM_CTRL_TGL_ENABLE);
    edgui_btnm_set_one_toggle(btnm, true);

    edgui_obj_t * table = edgui_table_create(h, NULL);
    edgui_table_set_col_cnt(table, 3);
    edgui_table_set_row_cnt(table, 4);
    edgui_table_set_col_width(table, 0, EDGUI_DPI / 3);
    edgui_table_set_col_width(table, 1, EDGUI_DPI / 2);
    edgui_table_set_col_width(table, 2, EDGUI_DPI / 2);
    edgui_table_set_cell_merge_right(table, 0, 0, true);
    edgui_table_set_cell_merge_right(table, 0, 1, true);

    edgui_table_set_cell_value(table, 0, 0, "Table");
    edgui_table_set_cell_align(table, 0, 0, EDGUI_LABEL_ALIGN_CENTER);

    edgui_table_set_cell_value(table, 1, 0, "1");
    edgui_table_set_cell_value(table, 1, 1, "13");
    edgui_table_set_cell_align(table, 1, 1, EDGUI_LABEL_ALIGN_RIGHT);
    edgui_table_set_cell_value(table, 1, 2, "ms");

    edgui_table_set_cell_value(table, 2, 0, "2");
    edgui_table_set_cell_value(table, 2, 1, "46");
    edgui_table_set_cell_align(table, 2, 1, EDGUI_LABEL_ALIGN_RIGHT);
    edgui_table_set_cell_value(table, 2, 2, "ms");

    edgui_table_set_cell_value(table, 3, 0, "3");
    edgui_table_set_cell_value(table, 3, 1, "61");
    edgui_table_set_cell_align(table, 3, 1, EDGUI_LABEL_ALIGN_RIGHT);
    edgui_table_set_cell_value(table, 3, 2, "ms");

    h = edgui_cont_create(parent, h);

    edgui_obj_t * sw_h = edgui_cont_create(h, NULL);
    edgui_cont_set_style(sw_h, EDGUI_CONT_STYLE_MAIN, &edgui_style_transp);
    edgui_cont_set_fit2(sw_h, EDGUI_FIT_NONE, EDGUI_FIT_TIGHT);
    edgui_obj_set_width(sw_h, EDGUI_HOR_RES / 4);
    edgui_cont_set_layout(sw_h, EDGUI_LAYOUT_PRETTY);

    edgui_obj_t * sw = edgui_sw_create(sw_h, NULL);
    edgui_sw_set_anim_time(sw, 250);

    sw = edgui_sw_create(sw_h, sw);
    edgui_sw_on(sw, EDGUI_ANIM_OFF);


    edgui_obj_t * bar = edgui_bar_create(h, NULL);
    edgui_bar_set_value(bar, 70, false);

    edgui_obj_t * slider = edgui_slider_create(h, NULL);
    edgui_bar_set_value(slider, 70, false);

    edgui_obj_t * line = edgui_line_create(h, NULL);
    static edgui_point_t line_p[2];
    line_p[0].x = 0;
    line_p[0].y = 0;
    line_p[1].x = edgui_disp_get_hor_res(NULL) / 5;
    line_p[1].y = 0;

    edgui_line_set_points(line, line_p, 2);
    edgui_line_set_style(line, EDGUI_LINE_STYLE_MAIN, th->style.line.decor);

    edgui_obj_t * cb = edgui_cb_create(h, NULL);

    cb = edgui_cb_create(h, cb);
    edgui_btn_set_state(cb, EDGUI_BTN_STATE_TGL_REL);

    edgui_obj_t * ddlist = edgui_ddlist_create(h, NULL);
    edgui_ddlist_set_fix_width(ddlist, edgui_obj_get_width(ddlist) + EDGUI_DPI / 2);   /*Make space for the arrow*/
    edgui_ddlist_set_draw_arrow(ddlist, true);

    h = edgui_cont_create(parent, h);

    edgui_obj_t * list = edgui_list_create(h, NULL);
    edgui_obj_set_size(list, edgui_disp_get_hor_res(NULL) / 4, edgui_disp_get_ver_res(NULL) / 2);
    edgui_obj_t * list_btn;
    list_btn = edgui_list_add_btn(list, EDGUI_SYMBOL_GPS,  "GPS");
    edgui_btn_set_toggle(list_btn, true);

    edgui_list_add_btn(list, EDGUI_SYMBOL_WIFI, "WiFi");
    edgui_list_add_btn(list, EDGUI_SYMBOL_GPS, "GPS");
    edgui_list_add_btn(list, EDGUI_SYMBOL_AUDIO, "Audio");
    edgui_list_add_btn(list, EDGUI_SYMBOL_VIDEO, "Video");
    edgui_list_add_btn(list, EDGUI_SYMBOL_CALL, "Call");
    edgui_list_add_btn(list, EDGUI_SYMBOL_BELL, "Bell");
    edgui_list_add_btn(list, EDGUI_SYMBOL_FILE, "File");
    edgui_list_add_btn(list, EDGUI_SYMBOL_EDIT, "Edit");
    edgui_list_add_btn(list, EDGUI_SYMBOL_CUT,  "Cut");
    edgui_list_add_btn(list, EDGUI_SYMBOL_COPY, "Copy");

    edgui_obj_t * roller = edgui_roller_create(h, NULL);
    edgui_roller_set_options(roller, "Monday\nTuesday\nWednesday\nThursday\nFriday\nSaturday\nSunday", true);
    edgui_roller_set_selected(roller, 1, false);
    edgui_roller_set_visible_row_count(roller, 3);


}

static void create_tab2(edgui_obj_t * parent)
{
    edgui_coord_t w = edgui_page_get_scrl_width(parent);

    edgui_obj_t * chart = edgui_chart_create(parent, NULL);
    edgui_chart_set_type(chart, EDGUI_CHART_TYPE_AREA);
    edgui_obj_set_size(chart, w / 3, edgui_disp_get_ver_res(NULL) / 3);
    edgui_obj_set_pos(chart, EDGUI_DPI / 10, EDGUI_DPI / 10);
    edgui_chart_series_t * s1 = edgui_chart_add_series(chart, EDGUI_COLOR_RED);
    edgui_chart_set_next(chart, s1, 30);
    edgui_chart_set_next(chart, s1, 20);
    edgui_chart_set_next(chart, s1, 10);
    edgui_chart_set_next(chart, s1, 12);
    edgui_chart_set_next(chart, s1, 20);
    edgui_chart_set_next(chart, s1, 27);
    edgui_chart_set_next(chart, s1, 35);
    edgui_chart_set_next(chart, s1, 55);
    edgui_chart_set_next(chart, s1, 70);
    edgui_chart_set_next(chart, s1, 75);


    edgui_obj_t * gauge = edgui_gauge_create(parent, NULL);
    edgui_gauge_set_value(gauge, 0, 40);
    edgui_obj_set_size(gauge, w / 4, w / 4);
    edgui_obj_align(gauge, chart, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, EDGUI_DPI / 4);


    edgui_obj_t * arc = edgui_arc_create(parent, NULL);
    edgui_obj_align(arc, gauge, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, EDGUI_DPI / 8);

    edgui_obj_t * ta = edgui_ta_create(parent, NULL);
    edgui_obj_set_size(ta, w / 3, edgui_disp_get_ver_res(NULL) / 4);
    edgui_obj_align(ta, NULL, EDGUI_ALIGN_IN_TOP_RIGHT, -EDGUI_DPI / 10, EDGUI_DPI / 10);
    edgui_ta_set_cursor_type(ta, EDGUI_CURSOR_BLOCK);

    edgui_obj_t * kb = edgui_kb_create(parent, NULL);
    edgui_obj_set_size(kb, 2 * w / 3, edgui_disp_get_ver_res(NULL) / 3);
    edgui_obj_align(kb, ta, EDGUI_ALIGN_OUT_BOTTOM_RIGHT, 0, EDGUI_DPI);
    edgui_kb_set_ta(kb, ta);

#if EDGUI_USE_ANIMATION
    edgui_obj_t * loader = edgui_preload_create(parent, NULL);
    edgui_obj_align(loader, NULL, EDGUI_ALIGN_CENTER, 0, - EDGUI_DPI);
#endif
}


static void create_tab3(edgui_obj_t * parent)
{
    /*Create a Window*/
    edgui_obj_t * win = edgui_win_create(parent, NULL);
    edgui_obj_t * win_btn = edgui_win_add_btn(win, EDGUI_SYMBOL_CLOSE);
    edgui_obj_set_event_cb(win_btn, edgui_win_close_event_cb);
    edgui_win_add_btn(win, EDGUI_SYMBOL_DOWN);
    edgui_obj_set_size(win, edgui_disp_get_hor_res(NULL) / 2, edgui_disp_get_ver_res(NULL) / 2);
    edgui_obj_set_pos(win, EDGUI_DPI / 20, EDGUI_DPI / 20);
    edgui_obj_set_top(win, true);


    /*Create a Label in the Window*/
    edgui_obj_t * label = edgui_label_create(win, NULL);
    edgui_label_set_text(label, "Label in the window");

    /*Create a  Line meter in the Window*/
    edgui_obj_t * lmeter = edgui_lmeter_create(win, NULL);
    edgui_obj_align(lmeter, label, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, EDGUI_DPI / 2);
    edgui_lmeter_set_value(lmeter, 70);

    /*Create a 2 LEDs in the Window*/
    edgui_obj_t * led1 = edgui_led_create(win, NULL);
    edgui_obj_align(led1, lmeter, EDGUI_ALIGN_OUT_RIGHT_MID, EDGUI_DPI / 2, 0);
    edgui_led_on(led1);

    edgui_obj_t * led2 = edgui_led_create(win, NULL);
    edgui_obj_align(led2, led1, EDGUI_ALIGN_OUT_RIGHT_MID, EDGUI_DPI / 2, 0);
    edgui_led_off(led2);

    /*Create a Page*/
    edgui_obj_t * page = edgui_page_create(parent, NULL);
    edgui_obj_set_size(page, edgui_disp_get_hor_res(NULL) / 3, edgui_disp_get_ver_res(NULL) / 2);
    edgui_obj_set_top(page, true);
    edgui_obj_align(page, win, EDGUI_ALIGN_IN_TOP_RIGHT,  EDGUI_DPI, EDGUI_DPI);

    label = edgui_label_create(page, NULL);
    edgui_label_set_text(label, "Lorem ipsum dolor sit amet, repudiare voluptatibus pri cu.\n"
                      "Ei mundi pertinax posidonium eum, cum tempor maiorum at,\n"
                      "mea fuisset assentior ad. Usu cu suas civibus iudicabit.\n"
                      "Eum eu congue tempor facilisi. Tale hinc unum te vim.\n"
                      "Te cum populo animal eruditi, labitur inciderint at nec.\n\n"
                      "Eius corpora et quo. Everti voluptaria instructior est id,\n"
                      "vel in falli primis. Mea ei porro essent admodum,\n"
                      "his ei malis quodsi, te quis aeterno his.\n"
                      "Qui tritani recusabo reprehendunt ne,\n"
                      "per duis explicari at. Simul mediocritatem mei et.");

    /*Create a Calendar*/
    edgui_obj_t * cal = edgui_calendar_create(parent, NULL);
    edgui_obj_set_size(cal, 5 * EDGUI_DPI / 2, 5 * EDGUI_DPI / 2);
    edgui_obj_align(cal, page, EDGUI_ALIGN_OUT_RIGHT_TOP, -EDGUI_DPI / 2, EDGUI_DPI / 3);
    edgui_obj_set_top(cal, true);

    static edgui_calendar_date_t highlighted_days[2];
    highlighted_days[0].day = 5;
    highlighted_days[0].month = 5;
    highlighted_days[0].year = 2018;

    highlighted_days[1].day = 8;
    highlighted_days[1].month = 5;
    highlighted_days[1].year = 2018;

    edgui_calendar_set_highlighted_dates(cal, highlighted_days, 2);
    edgui_calendar_set_today_date(cal, &highlighted_days[0]);
    edgui_calendar_set_showed_date(cal, &highlighted_days[0]);

    /*Create a Message box*/
    static const char * mbox_btn_map[] = {" ", "Got it!", " ", ""};
    edgui_obj_t * mbox = edgui_mbox_create(parent, NULL);
    edgui_mbox_set_text(mbox, "Click on the window or the page to bring it to the foreground");
    edgui_mbox_add_btns(mbox, mbox_btn_map);
    edgui_btnm_set_btn_ctrl(edgui_mbox_get_btnm(mbox), 0, EDGUI_BTNM_CTRL_HIDDEN);
    edgui_btnm_set_btn_width(edgui_mbox_get_btnm(mbox), 1, 7);
    edgui_btnm_set_btn_ctrl(edgui_mbox_get_btnm(mbox), 2, EDGUI_BTNM_CTRL_HIDDEN);
    edgui_obj_set_top(mbox, true);


}

#endif /*EDGUI_USE_TESTS*/
