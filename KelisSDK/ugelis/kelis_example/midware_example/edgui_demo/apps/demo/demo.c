/**
 * @file demo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "demo.h"
#if EDGUI_USE_DEMO

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void write_create(edgui_obj_t *parent);
static void text_area_event_handler(edgui_obj_t *text_area, edgui_event_t event);
static void keyboard_event_cb(edgui_obj_t *keyboard, edgui_event_t event);
#if EDGUI_USE_ANIMATION
    static void kb_hide_anim_end(edgui_anim_t *a);
#endif
static void list_create(edgui_obj_t *parent);
static void chart_create(edgui_obj_t *parent);
static void slider_event_handler(edgui_obj_t *slider, edgui_event_t event);
static void list_btn_event_handler(edgui_obj_t *slider, edgui_event_t event);
#if EDGUI_DEMO_SLIDE_SHOW
    static void tab_switcher(edgui_task_t *task);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_obj_t *chart;
static edgui_obj_t *ta;
static edgui_obj_t *kb;

static edgui_style_t style_kb;
static edgui_style_t style_kb_rel;
static edgui_style_t style_kb_pr;

#if EDGUI_DEMO_WALLPAPER
    EDGUI_IMG_DECLARE(img_bubble_pattern)
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a demo application
 */
void demo_create(void)
{
    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

#if EDGUI_DEMO_WALLPAPER
    edgui_obj_t *wp = edgui_img_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_img_set_src(wp, &img_bubble_pattern);
    edgui_obj_set_width(wp, hres * 4);
    edgui_obj_set_protect(wp, EDGUI_PROTECT_POS);
#endif

    static edgui_style_t style_tv_btn_bg;
    edgui_style_copy(&style_tv_btn_bg, &edgui_style_plain);
    style_tv_btn_bg.body.main_color = edgui_color_hex(0x487fb7);
    style_tv_btn_bg.body.grad_color = edgui_color_hex(0x487fb7);
    style_tv_btn_bg.body.padding.top = 0;
    style_tv_btn_bg.body.padding.bottom = 0;

    static edgui_style_t style_tv_btn_rel;
    edgui_style_copy(&style_tv_btn_rel, &edgui_style_btn_rel);
    style_tv_btn_rel.body.opa = EDGUI_OPA_TRANSP;
    style_tv_btn_rel.body.border.width = 0;

    static edgui_style_t style_tv_btn_pr;
    edgui_style_copy(&style_tv_btn_pr, &edgui_style_btn_pr);
    style_tv_btn_pr.body.radius = 0;
    style_tv_btn_pr.body.opa = EDGUI_OPA_50;
    style_tv_btn_pr.body.main_color = EDGUI_COLOR_WHITE;
    style_tv_btn_pr.body.grad_color = EDGUI_COLOR_WHITE;
    style_tv_btn_pr.body.border.width = 0;
    style_tv_btn_pr.text.color = EDGUI_COLOR_GRAY;

    edgui_obj_t *tv = edgui_tabview_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(tv, hres, vres);

#if EDGUI_DEMO_WALLPAPER
    edgui_obj_set_parent(wp, ((edgui_tabview_ext_t *) tv->ext_attr)->content);
    edgui_obj_set_pos(wp, 0, -5);
#endif

    edgui_obj_t *tab1 = edgui_tabview_add_tab(tv, "Write");
    edgui_obj_t *tab2 = edgui_tabview_add_tab(tv, "List");
    edgui_obj_t *tab3 = edgui_tabview_add_tab(tv, "Chart");

#if EDGUI_DEMO_WALLPAPER == 0
    /*Blue bg instead of wallpaper*/
    edgui_tabview_set_style(tv, EDGUI_TABVIEW_STYLE_BG, &style_tv_btn_bg);
#endif
    edgui_tabview_set_style(tv, EDGUI_TABVIEW_STYLE_BTN_BG, &style_tv_btn_bg);
    edgui_tabview_set_style(tv, EDGUI_TABVIEW_STYLE_INDIC, &edgui_style_plain);
    edgui_tabview_set_style(tv, EDGUI_TABVIEW_STYLE_BTN_REL, &style_tv_btn_rel);
    edgui_tabview_set_style(tv, EDGUI_TABVIEW_STYLE_BTN_PR, &style_tv_btn_pr);
    edgui_tabview_set_style(tv, EDGUI_TABVIEW_STYLE_BTN_TGL_REL, &style_tv_btn_rel);
    edgui_tabview_set_style(tv, EDGUI_TABVIEW_STYLE_BTN_TGL_PR, &style_tv_btn_pr);

    write_create(tab1);
    list_create(tab2);
    chart_create(tab3);

#if EDGUI_DEMO_SLIDE_SHOW
    // edgui_task_create(tab_switcher, 30000, EDGUI_TASK_PRIO_MID, tv);
#endif
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void write_create(edgui_obj_t *parent)
{
    edgui_page_set_style(parent, EDGUI_PAGE_STYLE_BG, &edgui_style_transp_fit);
    edgui_page_set_style(parent, EDGUI_PAGE_STYLE_SCRL, &edgui_style_transp_fit);

    edgui_page_set_sb_mode(parent, EDGUI_SB_MODE_OFF);

    static edgui_style_t style_ta;
    edgui_style_copy(&style_ta, &edgui_style_pretty);
    style_ta.body.opa = EDGUI_OPA_30;
    style_ta.body.radius = 0;
    style_ta.text.color = edgui_color_hex3(0x222);

    ta = edgui_ta_create(parent, NULL);
    edgui_obj_set_size(ta, edgui_page_get_scrl_width(parent), edgui_obj_get_height(parent) / 2);
    edgui_ta_set_style(ta, EDGUI_TA_STYLE_BG, &style_ta);
    edgui_ta_set_text(ta, "");
    edgui_obj_set_event_cb(ta, text_area_event_handler);
    edgui_style_copy(&style_kb, &edgui_style_plain);
    edgui_ta_set_text_sel(ta, true);

    style_kb.body.opa = EDGUI_OPA_70;
    style_kb.body.main_color = edgui_color_hex3(0x333);
    style_kb.body.grad_color = edgui_color_hex3(0x333);
    style_kb.body.padding.left = 0;
    style_kb.body.padding.right = 0;
    style_kb.body.padding.top = 0;
    style_kb.body.padding.bottom = 0;
    style_kb.body.padding.inner = 0;

    edgui_style_copy(&style_kb_rel, &edgui_style_plain);
    style_kb_rel.body.opa = EDGUI_OPA_TRANSP;
    style_kb_rel.body.radius = 0;
    style_kb_rel.body.border.width = 1;
    style_kb_rel.body.border.color = EDGUI_COLOR_SILVER;
    style_kb_rel.body.border.opa = EDGUI_OPA_50;
    style_kb_rel.body.main_color = edgui_color_hex3(0x333);    /*Recommended if EDGUI_VDB_SIZE == 0 and bpp > 1 fonts are used*/
    style_kb_rel.body.grad_color = edgui_color_hex3(0x333);
    style_kb_rel.text.color = EDGUI_COLOR_WHITE;

    edgui_style_copy(&style_kb_pr, &edgui_style_plain);
    style_kb_pr.body.radius = 0;
    style_kb_pr.body.opa = EDGUI_OPA_50;
    style_kb_pr.body.main_color = EDGUI_COLOR_WHITE;
    style_kb_pr.body.grad_color = EDGUI_COLOR_WHITE;
    style_kb_pr.body.border.width = 1;
    style_kb_pr.body.border.color = EDGUI_COLOR_SILVER;

}

static void text_area_event_handler(edgui_obj_t *text_area, edgui_event_t event)
{
    (void) text_area;    /*Unused*/

    /*Text area is on the scrollable part of the page but we need the page itself*/
    edgui_obj_t *parent = edgui_obj_get_parent(edgui_obj_get_parent(ta));

    if (event == EDGUI_EVENT_CLICKED)
    {
        if (kb == NULL)
        {
            kb = edgui_kb_create(parent, NULL);
            edgui_obj_set_size(kb, edgui_obj_get_width_fit(parent), edgui_obj_get_height_fit(parent) / 2);
            edgui_obj_align(kb, ta, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 0);
            edgui_kb_set_ta(kb, ta);
            edgui_kb_set_style(kb, EDGUI_KB_STYLE_BG, &style_kb);
            edgui_kb_set_style(kb, EDGUI_KB_STYLE_BTN_REL, &style_kb_rel);
            edgui_kb_set_style(kb, EDGUI_KB_STYLE_BTN_PR, &style_kb_pr);
            edgui_obj_set_event_cb(kb, keyboard_event_cb);

#if EDGUI_USE_ANIMATION
            edgui_anim_t a;
            a.var = kb;
            a.start = EDGUI_VER_RES;
            a.end = edgui_obj_get_y(kb);
            a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
            a.path_cb = edgui_anim_path_linear;
            a.ready_cb = NULL;
            a.act_time = 0;
            a.time = 50;
            a.playback = 0;
            a.playback_pause = 0;
            a.repeat = 0;
            a.repeat_pause = 0;
            edgui_anim_create(&a);
#endif
        }
    }

}

/**
 * Called when the close or ok button is pressed on the keyboard
 * @param keyboard pointer to the keyboard
 * @return
 */
static void keyboard_event_cb(edgui_obj_t *keyboard, edgui_event_t event)
{
    (void) keyboard;    /*Unused*/

    edgui_kb_def_event_cb(kb, event);

    if (event == EDGUI_EVENT_APPLY || event == EDGUI_EVENT_CANCEL)
    {
#if EDGUI_USE_ANIMATION
        edgui_anim_t a;
        a.var = kb;
        a.start = edgui_obj_get_y(kb);
        a.end = EDGUI_VER_RES;
        a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
        a.path_cb = edgui_anim_path_linear;
        a.ready_cb = kb_hide_anim_end;
        a.act_time = 0;
        a.time = 100;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        edgui_anim_create(&a);
#else
        edgui_obj_del(kb);
        kb = NULL;
#endif
    }
}

static void list_create(edgui_obj_t *parent)
{
    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);

    edgui_page_set_style(parent, EDGUI_PAGE_STYLE_BG, &edgui_style_transp_fit);
    edgui_page_set_style(parent, EDGUI_PAGE_STYLE_SCRL, &edgui_style_transp_fit);

    edgui_page_set_sb_mode(parent, EDGUI_SB_MODE_OFF);

    /*Create styles for the buttons*/
    static edgui_style_t style_btn_rel;
    static edgui_style_t style_btn_pr;
    edgui_style_copy(&style_btn_rel, &edgui_style_btn_rel);
    style_btn_rel.body.main_color = edgui_color_hex3(0x333);
    style_btn_rel.body.grad_color = EDGUI_COLOR_BLACK;
    style_btn_rel.body.border.color = EDGUI_COLOR_SILVER;
    style_btn_rel.body.border.width = 1;
    style_btn_rel.body.border.opa = EDGUI_OPA_50;
    style_btn_rel.body.radius = 0;

    edgui_style_copy(&style_btn_pr, &style_btn_rel);
    style_btn_pr.body.main_color = edgui_color_make(0x55, 0x96, 0xd8);
    style_btn_pr.body.grad_color = edgui_color_make(0x37, 0x62, 0x90);
    style_btn_pr.text.color = edgui_color_make(0xbb, 0xd5, 0xf1);

    edgui_obj_t *list = edgui_list_create(parent, NULL);
    edgui_obj_set_height(list, 2 * edgui_obj_get_height(parent) / 3);
    edgui_list_set_style(list, EDGUI_LIST_STYLE_BG, &edgui_style_transp_tight);
    edgui_list_set_style(list, EDGUI_LIST_STYLE_SCRL, &edgui_style_transp_tight);
    edgui_list_set_style(list, EDGUI_LIST_STYLE_BTN_REL, &style_btn_rel);
    edgui_list_set_style(list, EDGUI_LIST_STYLE_BTN_PR, &style_btn_pr);
    edgui_obj_align(list, NULL, EDGUI_ALIGN_IN_TOP_MID, 0, EDGUI_DPI / 4);

    edgui_obj_t *list_btn;
    list_btn = edgui_list_add_btn(list, EDGUI_SYMBOL_FILE, "New");
    edgui_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = edgui_list_add_btn(list, EDGUI_SYMBOL_DIRECTORY, "Open");
    edgui_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = edgui_list_add_btn(list, EDGUI_SYMBOL_TRASH, "Delete");
    edgui_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = edgui_list_add_btn(list, EDGUI_SYMBOL_EDIT, "Edit");
    edgui_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = edgui_list_add_btn(list, EDGUI_SYMBOL_SAVE, "Save");
    edgui_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = edgui_list_add_btn(list, EDGUI_SYMBOL_WIFI, "WiFi");
    edgui_obj_set_event_cb(list_btn, list_btn_event_handler);

    list_btn = edgui_list_add_btn(list, EDGUI_SYMBOL_GPS, "GPS");
    edgui_obj_set_event_cb(list_btn, list_btn_event_handler);

    edgui_obj_t *mbox = edgui_mbox_create(parent, NULL);
    edgui_mbox_set_text(mbox, "Click a button to copy its text to the Text area ");
    edgui_obj_set_width(mbox, hres - EDGUI_DPI);
    static const char *mbox_btns[] = {"Got it", ""};
    edgui_mbox_add_btns(mbox, mbox_btns);    /*The default action is close*/
    edgui_obj_align(mbox, parent, EDGUI_ALIGN_IN_TOP_MID, 0, EDGUI_DPI / 2);
}

#if EDGUI_USE_ANIMATION
static void kb_hide_anim_end(edgui_anim_t *a)
{
    edgui_obj_del(a->var);
    kb = NULL;
}
#endif

static void chart_create(edgui_obj_t *parent)
{

    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    edgui_page_set_style(parent, EDGUI_PAGE_STYLE_BG, &edgui_style_transp_fit);
    edgui_page_set_style(parent, EDGUI_PAGE_STYLE_SCRL, &edgui_style_transp_fit);

    edgui_page_set_scrl_height(parent, edgui_obj_get_height(parent));
    edgui_page_set_sb_mode(parent, EDGUI_SB_MODE_OFF);

    static edgui_style_t style_chart;
    edgui_style_copy(&style_chart, &edgui_style_pretty);
    style_chart.body.opa = EDGUI_OPA_60;
    style_chart.body.radius = 0;
    style_chart.line.color = EDGUI_COLOR_GRAY;

    chart = edgui_chart_create(parent, NULL);
    edgui_obj_set_size(chart, 2 * edgui_obj_get_width(parent) / 3, edgui_obj_get_height(parent) / 2);
    edgui_obj_align(chart, NULL,  EDGUI_ALIGN_IN_TOP_MID, 0, EDGUI_DPI / 4);
    edgui_chart_set_type(chart, EDGUI_CHART_TYPE_COLUMN);
    edgui_chart_set_style(chart, EDGUI_CHART_STYLE_MAIN, &style_chart);
    edgui_chart_set_series_opa(chart, EDGUI_OPA_70);
    edgui_chart_series_t *ser1;
    ser1 = edgui_chart_add_series(chart, EDGUI_COLOR_RED);
    edgui_chart_set_next(chart, ser1, 40);
    edgui_chart_set_next(chart, ser1, 30);
    edgui_chart_set_next(chart, ser1, 47);
    edgui_chart_set_next(chart, ser1, 59);
    edgui_chart_set_next(chart, ser1, 59);
    edgui_chart_set_next(chart, ser1, 31);
    edgui_chart_set_next(chart, ser1, 55);
    edgui_chart_set_next(chart, ser1, 70);
    edgui_chart_set_next(chart, ser1, 82);
    edgui_chart_set_next(chart, ser1, 91);

    /*Create a bar, an indicator and a knob style*/
    static edgui_style_t style_bar;
    static edgui_style_t style_indic;
    static edgui_style_t style_knob;

    edgui_style_copy(&style_bar, &edgui_style_pretty);
    style_bar.body.main_color =  EDGUI_COLOR_BLACK;
    style_bar.body.grad_color =  EDGUI_COLOR_GRAY;
    style_bar.body.radius = EDGUI_RADIUS_CIRCLE;
    style_bar.body.border.color = EDGUI_COLOR_WHITE;
    style_bar.body.opa = EDGUI_OPA_60;
    style_bar.body.padding.left = 0;
    style_bar.body.padding.right = 0;
    style_bar.body.padding.top = EDGUI_DPI / 10;
    style_bar.body.padding.bottom = EDGUI_DPI / 10;

    edgui_style_copy(&style_indic, &edgui_style_pretty);
    style_indic.body.grad_color =  EDGUI_COLOR_MAROON;
    style_indic.body.main_color =  EDGUI_COLOR_RED;
    style_indic.body.radius = EDGUI_RADIUS_CIRCLE;
    style_indic.body.shadow.width = EDGUI_DPI / 10;
    style_indic.body.shadow.color = EDGUI_COLOR_RED;
    style_indic.body.padding.left = EDGUI_DPI / 30;
    style_indic.body.padding.right = EDGUI_DPI / 30;
    style_indic.body.padding.top = EDGUI_DPI / 30;
    style_indic.body.padding.bottom = EDGUI_DPI / 30;

    edgui_style_copy(&style_knob, &edgui_style_pretty);
    style_knob.body.radius = EDGUI_RADIUS_CIRCLE;
    style_knob.body.opa = EDGUI_OPA_70;

    /*Create a second slider*/
    edgui_obj_t *slider = edgui_slider_create(parent, NULL);
    edgui_slider_set_style(slider, EDGUI_SLIDER_STYLE_BG, &style_bar);
    edgui_slider_set_style(slider, EDGUI_SLIDER_STYLE_INDIC, &style_indic);
    edgui_slider_set_style(slider, EDGUI_SLIDER_STYLE_KNOB, &style_knob);
    edgui_obj_set_size(slider, edgui_obj_get_width(chart), EDGUI_DPI / 3);
    edgui_obj_align(slider, chart, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, (vres - chart->coords.y2 - edgui_obj_get_height(slider)) / 2); /*Align to below the chart*/
    edgui_obj_set_event_cb(slider, slider_event_handler);
    edgui_slider_set_range(slider, 10, 1000);
    edgui_slider_set_value(slider, 700, false);
    slider_event_handler(slider, EDGUI_EVENT_VALUE_CHANGED);          /*Simulate a user value set the refresh the chart*/
}

/**
 * Called when a new value on the slider on the Chart tab is set
 * @param slider pointer to the slider
 * @return EDGUI_RES_OK because the slider is not deleted in the function
 */
static void slider_event_handler(edgui_obj_t *slider, edgui_event_t event)
{

    if (event == EDGUI_EVENT_VALUE_CHANGED)
    {
        int16_t v = edgui_slider_get_value(slider);
        v = 1000 * 100 / v; /*Convert to range modify values linearly*/
        edgui_chart_set_range(chart, 0, v);
    }
}

/**
 * Called when a a list button is clicked on the List tab
 * @param btn pointer to a list button
 * @return EDGUI_RES_OK because the button is not deleted in the function
 */
static void list_btn_event_handler(edgui_obj_t *btn, edgui_event_t event)
{

    if (event == EDGUI_EVENT_SHORT_CLICKED)
    {
        edgui_ta_add_char(ta, '\n');
        edgui_ta_add_text(ta, edgui_list_get_btn_text(btn));
    }
}

#if EDGUI_DEMO_SLIDE_SHOW
/**
 * Called periodically (edgui_task) to switch to the next tab
 */
static void tab_switcher(edgui_task_t *task)
{
    static uint8_t tab = 0;
    edgui_obj_t *tv = task->user_data;
    tab++;
    if (tab >= 3)
    {
        tab = 0;
    }
    edgui_tabview_set_tab_act(tv, tab, true);
}
#endif


#endif  /*EDGUI_USE_DEMO*/
