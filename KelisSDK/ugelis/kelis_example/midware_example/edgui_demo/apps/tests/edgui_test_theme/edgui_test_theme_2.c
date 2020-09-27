/**
 * @file edgui_test_theme_2.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_theme_2.h"

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
static void header_create(void);
static void sb_create(void);
static void content_create(void);
static void theme_select_event_handler(edgui_obj_t * roller, edgui_event_t event);
static void hue_select_event_cb(edgui_obj_t * roller, edgui_event_t event);
static void init_all_themes(uint16_t hue);
static void bar_set_value(edgui_obj_t * bar, int16_t value);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_obj_t * header;
static edgui_obj_t * sb;
static edgui_obj_t * content;
static edgui_theme_t * th_act;

static const char * th_options =
{

#if EDGUI_USE_THEME_NIGHT
        "Night"
#endif

#if EDGUI_USE_THEME_MATERIAL
        "\nMaterial"
#endif

#if EDGUI_USE_THEME_ALIEN
        "\nAlien"
#endif

#if EDGUI_USE_THEME_ZEN
        "\nZen"
#endif

#if EDGUI_USE_THEME_NEMO
        "\nNemo"
#endif

#if EDGUI_USE_THEME_MONO
        "\nMono"
#endif

#if EDGUI_USE_THEME_DEFAULT
        "\nDefault"
#endif
        ""
};

static edgui_theme_t * themes[8];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Test run time theme change
 */
void edgui_test_theme_2(void)
{
    /* By doing this, we hide the first (empty) option. */
    if(th_options[0] == '\n')
	    th_options++;

    init_all_themes(0);
    th_act = themes[0];
    if(th_act == NULL) {
    	EDGUI_LOG_WARN("edgui_test_theme_2: no theme is enabled. Check edgui_conf.h");
    	return;
    }


    edgui_theme_set_current(th_act);

    edgui_obj_t * scr = edgui_obj_create(NULL, NULL);
    edgui_disp_load_scr(scr);

    header_create();
    sb_create();
    content_create();


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void header_create(void)
{
    header = edgui_cont_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_width(header, edgui_disp_get_hor_res(NULL));

    edgui_obj_t * sym = edgui_label_create(header, NULL);
    edgui_label_set_text(sym, EDGUI_SYMBOL_GPS EDGUI_SYMBOL_WIFI EDGUI_SYMBOL_BLUETOOTH EDGUI_SYMBOL_VOLUME_MAX);
    edgui_obj_align(sym, NULL, EDGUI_ALIGN_IN_RIGHT_MID, -EDGUI_DPI/10, 0);

    edgui_obj_t * clock = edgui_label_create(header, NULL);
    edgui_label_set_text(clock, "14:21");
    edgui_obj_align(clock, NULL, EDGUI_ALIGN_IN_LEFT_MID, EDGUI_DPI/10, 0);

    edgui_cont_set_fit2(header, EDGUI_FIT_NONE, EDGUI_FIT_TIGHT);   /*Let the height set automatically*/
    edgui_obj_set_pos(header, 0, 0);

}

static void sb_create(void)
{
    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    sb = edgui_page_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_page_set_scrl_layout(sb, EDGUI_LAYOUT_COL_M);
    edgui_page_set_style(sb, EDGUI_PAGE_STYLE_BG, &edgui_style_transp_tight);
    edgui_page_set_style(sb, EDGUI_PAGE_STYLE_SCRL, &edgui_style_transp);

    edgui_obj_t * th_label = edgui_label_create(sb, NULL);
    edgui_label_set_text(th_label, "Theme");

    edgui_obj_t * th_roller = edgui_roller_create(sb, NULL);
    edgui_roller_set_options(th_roller, th_options, true);
    edgui_obj_set_event_cb(th_roller, theme_select_event_handler);

    edgui_obj_t * hue_label = edgui_label_create(sb, NULL);
    edgui_label_set_text(hue_label, "\nColor");

    edgui_obj_t * hue_roller = edgui_roller_create(sb, NULL);
    edgui_roller_set_options(hue_roller, "0\n30\n60\n90\n120\n150\n180\n210\n240\n270\n300\n330", true);
    edgui_obj_set_event_cb(hue_roller, hue_select_event_cb);

    if(hres > vres) {
        edgui_obj_set_height(sb, vres - edgui_obj_get_height(header));
        edgui_cont_set_fit2(sb, EDGUI_FIT_TIGHT, EDGUI_FIT_NONE);
        edgui_obj_align(sb, header, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
        edgui_page_set_sb_mode(sb, EDGUI_SB_MODE_DRAG);
    } else {
        edgui_obj_set_size(sb, hres, vres / 2 - edgui_obj_get_height(header));
        edgui_obj_align(sb, header, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
        edgui_page_set_sb_mode(sb, EDGUI_SB_MODE_AUTO);
    }
}

static void content_create(void)
{
    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    content = edgui_page_create(edgui_disp_get_scr_act(NULL), NULL);

    if(hres > vres) {
        edgui_obj_set_size(content, hres - edgui_obj_get_width(sb), vres - edgui_obj_get_height(header));
        edgui_obj_set_pos(content,  edgui_obj_get_width(sb), edgui_obj_get_height(header));
    } else {
        edgui_obj_set_size(content, hres , vres / 2);
        edgui_obj_set_pos(content,  0, vres / 2);
    }

    edgui_page_set_scrl_layout(content, EDGUI_LAYOUT_PRETTY);
    edgui_page_set_scrl_fit2(content, EDGUI_FIT_FLOOD, EDGUI_FIT_TIGHT);

    edgui_coord_t max_w = edgui_page_get_fit_width(content);


    /*Button*/
    edgui_obj_t * btn = edgui_btn_create(content, NULL);
    edgui_btn_set_ink_in_time(btn, 200);
    edgui_btn_set_ink_wait_time(btn, 100);
    edgui_btn_set_ink_out_time(btn, 500);
    edgui_obj_t * label = edgui_label_create(btn, NULL);
    edgui_label_set_text(label, "Button");

    /*Switch*/
    edgui_obj_t * sw = edgui_sw_create(content, NULL);
    edgui_sw_set_anim_time(sw, 250);

    /*Check box*/
    edgui_cb_create(content, NULL);

    /*Bar*/
    edgui_obj_t * bar = edgui_bar_create(content, NULL);
    edgui_obj_set_width(bar, EDGUI_MATH_MIN(max_w, 3 * EDGUI_DPI / 2));
#if EDGUI_USE_ANIMATION
    edgui_anim_t a;
    a.var = bar;
    a.start = 0;
    a.end = 100;
    a.exec_cb = (edgui_anim_exec_xcb_t)bar_set_value;
    a.path_cb = edgui_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = 0;
    a.time = 1000;
    a.playback = 1;
    a.playback_pause = 100;
    a.repeat = 1;
    a.repeat_pause = 100;
    edgui_anim_create(&a);
#endif

    /*Slider*/
    edgui_obj_t * slider = edgui_slider_create(content, NULL);
    edgui_obj_set_width(slider, EDGUI_MATH_MIN(max_w, 3 * EDGUI_DPI / 2));
    edgui_slider_set_value(slider, 30, false);

    /*Roller*/
    static const char * days = "Monday\nTuesday\nWednesday\nThursday\nFriday\nSaturday\nSunday";
    edgui_obj_t * roller = edgui_roller_create(content, NULL);
    edgui_roller_set_options(roller, days, false);

    /*Drop down list*/
    static const char * nums = "One\nTwo\nThree\nFour";
    edgui_obj_t * ddlist = edgui_ddlist_create(content, NULL);
    edgui_ddlist_set_options(ddlist, nums);

    /*Line meter*/
    edgui_obj_t * lmeter = edgui_lmeter_create(content, NULL);
    edgui_obj_set_click(lmeter, false);
#if EDGUI_USE_ANIMATION
    a.var = lmeter;
    a.start = 0;
    a.end = 100;
    a.exec_cb = (edgui_anim_exec_xcb_t)edgui_lmeter_set_value;
    a.path_cb = edgui_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = 0;
    a.time = 1000;
    a.playback = 1;
    a.playback_pause = 100;
    a.repeat = 1;
    a.repeat_pause = 100;
    edgui_anim_create(&a);
#endif

    /*Gauge*/
    edgui_obj_t * gauge = edgui_gauge_create(content, NULL);
    edgui_gauge_set_value(gauge, 0, 47);
    edgui_obj_set_size(gauge, EDGUI_MATH_MIN(max_w, EDGUI_DPI * 3 / 2), EDGUI_MATH_MIN(max_w, EDGUI_DPI * 3 / 2));
    edgui_obj_set_click(gauge, false);

    /*Text area*/
    edgui_obj_t * ta = edgui_ta_create(content, NULL);
    edgui_obj_set_width(ta, EDGUI_MATH_MIN(max_w, EDGUI_DPI * 3 / 2));
    edgui_ta_set_one_line(ta, true);
    edgui_ta_set_text(ta, "Type...");

    /*Keyboard*/
    edgui_obj_t * kb = edgui_kb_create(content, NULL);
    edgui_obj_set_width(kb, max_w - EDGUI_DPI / 4);
    edgui_kb_set_ta(kb, ta);

    edgui_obj_t * mbox = edgui_mbox_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_drag(mbox, true);
    edgui_mbox_set_text(mbox, "Choose a theme and a color on the left!");

    static const char * mbox_btns[] = {"Ok", ""};
    edgui_mbox_add_btns(mbox, mbox_btns);

    edgui_obj_align(mbox, NULL, EDGUI_ALIGN_CENTER, 0, 0);

}

static void theme_select_event_handler(edgui_obj_t * roller, edgui_event_t event)
{
    if(event == EDGUI_EVENT_VALUE_CHANGED) {
        edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
        edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

        uint16_t opt = edgui_roller_get_selected(roller);
        th_act = themes[opt];
        edgui_theme_set_current(th_act);

        edgui_obj_align(header, NULL, EDGUI_ALIGN_IN_TOP_MID, 0, 0);
        edgui_obj_align(sb, header, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    if(hres > vres) {
        edgui_obj_set_size(content, hres - edgui_obj_get_width(sb), vres - edgui_obj_get_height(header));
        edgui_obj_set_pos(content,  edgui_obj_get_width(sb), edgui_obj_get_height(header));
    } else {
        edgui_obj_set_size(content, hres , vres / 2);
        edgui_obj_set_pos(content,  0, vres / 2);
    }

        edgui_page_focus(sb, roller, EDGUI_ANIM_ON);
    }
}


static void hue_select_event_cb(edgui_obj_t * roller, edgui_event_t event)
{

    if(event == EDGUI_EVENT_VALUE_CHANGED) {
        uint16_t hue = edgui_roller_get_selected(roller) * 30;

        init_all_themes(hue);

        edgui_theme_set_current(th_act);

        edgui_page_focus(sb, roller, EDGUI_ANIM_ON);
    }
}


static void init_all_themes(uint16_t hue)
{
    /* NOTE: This must be adjusted if more themes are added. */
    int i = 0;
#if EDGUI_USE_THEME_NIGHT
    themes[i++] = edgui_theme_night_init(hue, NULL);
#endif

#if EDGUI_USE_THEME_MATERIAL
    themes[i++] = edgui_theme_material_init(hue, NULL);
#endif

#if EDGUI_USE_THEME_ALIEN
    themes[i++] = edgui_theme_alien_init(hue, NULL);
#endif

#if EDGUI_USE_THEME_ZEN
    themes[i++] = edgui_theme_zen_init(hue, NULL);
#endif

#if EDGUI_USE_THEME_NEMO
    themes[i++] = edgui_theme_nemo_init(hue, NULL);
#endif

#if EDGUI_USE_THEME_MONO
    themes[i++] = edgui_theme_mono_init(hue, NULL);
#endif

#if EDGUI_USE_THEME_DEFAULT
    themes[i++] = edgui_theme_default_init(hue, NULL);
#endif
}

static void bar_set_value(edgui_obj_t * bar, int16_t value)
{
    edgui_bar_set_value(bar, value, EDGUI_ANIM_OFF);
}

#endif /*EDGUI_USE_TESTS*/
