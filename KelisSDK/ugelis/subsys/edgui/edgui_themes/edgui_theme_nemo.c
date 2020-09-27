/**
 * @file edgui_theme_nemo.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_theme.h"

#if EDGUI_USE_THEME_NEMO

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

static uint16_t _hue;
static edgui_font_t * _font;
static edgui_font_t * _font;
static edgui_font_t * _font;

static edgui_theme_t theme;
static edgui_style_t def;
static edgui_style_t bg;
static edgui_style_t scr;
static edgui_style_t panel; /*General fancy background (e.g. to chart or ta)*/
static edgui_style_t sb;
static edgui_style_t btn_rel, btn_pr, btn_trel, btn_tpr, btn_ina;

#if EDGUI_USE_BAR
static edgui_style_t bar_bg, bar_indic;
#endif

#if EDGUI_USE_SLIDER
static edgui_style_t slider_knob;
#endif

#if EDGUI_USE_LMETER
static edgui_style_t lmeter_bg;
#endif

#if EDGUI_USE_DDLIST
static edgui_style_t ddlist_bg, ddlist_sel;
#endif

#if EDGUI_USE_BTNM
static edgui_style_t btnm_bg, btnm_rel, btnm_pr, btnm_trel, btnm_ina;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    /*Default*/
    edgui_style_copy(&def, &edgui_style_plain);
    def.body.opa = EDGUI_OPA_COVER;
    def.glass    = 0;

    def.body.main_color     = edgui_color_hex3(0x222);
    def.body.grad_color     = edgui_color_hex3(0x222);
    def.body.radius         = 0;
    def.body.padding.left   = EDGUI_DPI / 8;
    def.body.padding.right  = EDGUI_DPI / 8;
    def.body.padding.top    = EDGUI_DPI / 8;
    def.body.padding.bottom = EDGUI_DPI / 8;
    def.body.padding.inner  = EDGUI_DPI / 8;
    def.body.border.color   = EDGUI_COLOR_SILVER;
    def.body.border.width   = 1;
    def.body.border.opa     = EDGUI_OPA_COVER;
    def.body.shadow.color   = EDGUI_COLOR_SILVER;
    def.body.shadow.width   = 0;
    def.body.shadow.type    = EDGUI_SHADOW_FULL;

    def.text.color        = edgui_color_hex3(0xDDD);
    def.text.font         = _font;
    def.text.letter_space = 1;
    def.text.line_space   = 2;

    def.image.color   = edgui_color_hex3(0xDDD);
    def.image.intense = EDGUI_OPA_TRANSP;

    def.line.color = edgui_color_hex3(0xDDD);
    def.line.width = 1;

    /*Background*/
    edgui_style_copy(&bg, &def);
    bg.body.main_color   = edgui_color_hex3(0x005);
    bg.body.grad_color   = edgui_color_hex3(0x045);
    bg.body.border.width = 2;
    bg.body.border.color = edgui_color_hex3(0x666);
    bg.body.shadow.color = EDGUI_COLOR_SILVER;

    edgui_style_copy(&scr, &bg);
    scr.body.padding.bottom = 0;
    scr.body.padding.top    = 0;
    scr.body.padding.left   = 0;
    scr.body.padding.right  = 0;

    /*Panel*/
    edgui_style_copy(&panel, &def);
    panel.body.radius       = EDGUI_DPI / 10;
    panel.body.main_color   = edgui_color_hex3(0x500);
    panel.body.grad_color   = edgui_color_hex3(0x505);
    panel.body.border.color = edgui_color_hex3(0xccc);
    panel.body.border.width = 2;
    panel.body.border.opa   = EDGUI_OPA_60;
    panel.text.color        = edgui_color_hsv_to_rgb(_hue, 8, 96);
    panel.line.color        = edgui_color_hsv_to_rgb(_hue, 20, 70);

    /*Scrollbar*/
    edgui_style_copy(&sb, &def);
    sb.body.opa            = EDGUI_OPA_50;
    sb.body.radius         = EDGUI_RADIUS_CIRCLE;
    sb.body.border.color   = EDGUI_COLOR_SILVER;
    sb.body.border.opa     = EDGUI_OPA_40;
    sb.body.border.width   = 1;
    sb.body.main_color     = edgui_color_hsv_to_rgb(_hue, 33, 92);
    sb.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 33, 92);
    sb.body.padding.left   = 1;
    sb.body.padding.right  = 1;
    sb.body.padding.top    = 1;
    sb.body.padding.bottom = 1;
    sb.body.padding.inner  = EDGUI_DPI / 15; /*Scrollbar width*/

    theme.style.bg    = &bg;
    theme.style.scr   = &scr;
    theme.style.panel = &panel;
}

static void btn_init(void)
{
#if EDGUI_USE_BTN != 0
    edgui_style_copy(&btn_rel, &def);
    btn_rel.glass               = 0;
    btn_rel.body.opa            = EDGUI_OPA_TRANSP;
    btn_rel.body.radius         = EDGUI_RADIUS_CIRCLE;
    btn_rel.body.border.width   = 2;
    btn_rel.body.border.color   = edgui_color_hsv_to_rgb(_hue, 70, 90);
    btn_rel.body.border.opa     = EDGUI_OPA_80;
    btn_rel.body.padding.left   = EDGUI_DPI / 4;
    btn_rel.body.padding.right  = EDGUI_DPI / 4;
    btn_rel.body.padding.top    = EDGUI_DPI / 6;
    btn_rel.body.padding.bottom = EDGUI_DPI / 6;
    btn_rel.body.padding.inner  = EDGUI_DPI / 10;
    btn_rel.text.color          = edgui_color_hsv_to_rgb(_hue, 8, 96);
    btn_rel.text.font           = _font;

    edgui_style_copy(&btn_pr, &btn_rel);
    btn_pr.body.opa        = EDGUI_OPA_COVER;
    btn_pr.body.main_color = edgui_color_hsv_to_rgb(_hue, 50, 50);
    btn_pr.body.grad_color = edgui_color_hsv_to_rgb(_hue, 50, 50);
    btn_pr.body.border.opa = EDGUI_OPA_60;
    btn_pr.text.font       = _font;
    btn_pr.text.color      = edgui_color_hsv_to_rgb(_hue, 10, 100);

    edgui_style_copy(&btn_trel, &btn_pr);
    btn_trel.body.opa          = EDGUI_OPA_COVER;
    btn_trel.body.main_color   = edgui_color_hsv_to_rgb(_hue, 50, 60);
    btn_trel.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 50, 60);
    btn_trel.body.border.opa   = EDGUI_OPA_60;
    btn_trel.body.border.color = edgui_color_hsv_to_rgb(_hue, 80, 90);
    btn_trel.text.font         = _font;
    btn_trel.text.color        = edgui_color_hsv_to_rgb(_hue, 0, 100);

    edgui_style_copy(&btn_tpr, &btn_trel);
    btn_tpr.body.opa          = EDGUI_OPA_COVER;
    btn_tpr.body.main_color   = edgui_color_hsv_to_rgb(_hue, 50, 50);
    btn_tpr.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 50, 50);
    btn_tpr.body.border.opa   = EDGUI_OPA_60;
    btn_tpr.body.border.color = edgui_color_hsv_to_rgb(_hue, 80, 70);
    btn_tpr.text.font         = _font;
    btn_tpr.text.color        = edgui_color_hsv_to_rgb(_hue, 10, 90);

    edgui_style_copy(&btn_ina, &btn_rel);
    btn_ina.body.border.opa   = EDGUI_OPA_60;
    btn_ina.body.border.color = edgui_color_hsv_to_rgb(_hue, 10, 50);
    btn_ina.text.font         = _font;
    btn_ina.text.color        = edgui_color_hsv_to_rgb(_hue, 10, 90);

    theme.style.btn.rel     = &btn_rel;
    theme.style.btn.pr      = &btn_pr;
    theme.style.btn.tgl_rel = &btn_trel;
    theme.style.btn.tgl_pr  = &btn_tpr;
    theme.style.btn.ina     = &btn_ina;
#endif
}

static void label_init(void)
{
#if EDGUI_USE_LABEL != 0
    static edgui_style_t label_prim, label_sec, label_hint;

    edgui_style_copy(&label_prim, &def);
    label_prim.text.font  = _font;
    label_prim.text.color = edgui_color_hsv_to_rgb(_hue, 5, 96);

    edgui_style_copy(&label_sec, &label_prim);
    label_sec.text.color = edgui_color_hsv_to_rgb(_hue, 40, 85);

    edgui_style_copy(&label_hint, &label_prim);
    label_hint.text.color = edgui_color_hsv_to_rgb(_hue, 20, 70);

    theme.style.label.prim = &label_prim;
    theme.style.label.sec  = &label_sec;
    theme.style.label.hint = &label_hint;
#endif
}

static void bar_init(void)
{
#if EDGUI_USE_BAR
    edgui_style_copy(&bar_bg, &def);
    bar_bg.body.opa            = EDGUI_OPA_30;
    bar_bg.body.radius         = EDGUI_RADIUS_CIRCLE;
    bar_bg.body.main_color     = EDGUI_COLOR_WHITE;
    bar_bg.body.grad_color     = EDGUI_COLOR_SILVER;
    bar_bg.body.border.width   = 2;
    bar_bg.body.border.color   = EDGUI_COLOR_SILVER;
    bar_bg.body.border.opa     = EDGUI_OPA_20;
    bar_bg.body.padding.left   = 0;
    bar_bg.body.padding.right  = 0;
    bar_bg.body.padding.top    = EDGUI_DPI / 10;
    bar_bg.body.padding.bottom = EDGUI_DPI / 10;
    bar_bg.body.padding.inner  = 0;

    edgui_style_copy(&bar_indic, &def);
    bar_indic.body.radius         = EDGUI_RADIUS_CIRCLE;
    bar_indic.body.border.width   = 2;
    bar_indic.body.border.color   = EDGUI_COLOR_SILVER;
    bar_indic.body.border.opa     = EDGUI_OPA_70;
    bar_indic.body.padding.left   = 0;
    bar_indic.body.padding.right  = 0;
    bar_indic.body.padding.top    = 0;
    bar_indic.body.padding.bottom = 0;
    bar_indic.body.shadow.width   = EDGUI_DPI / 20;
    bar_indic.body.shadow.color   = edgui_color_hsv_to_rgb(_hue, 20, 90);
    bar_indic.body.main_color     = edgui_color_hsv_to_rgb(_hue, 40, 80);
    bar_indic.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 40, 80);

    theme.style.bar.bg    = &bar_bg;
    theme.style.bar.indic = &bar_indic;
#endif
}

static void img_init(void)
{
#if EDGUI_USE_IMG != 0
    static edgui_style_t img_light, img_dark;
    edgui_style_copy(&img_light, &def);
    img_light.image.color   = edgui_color_hsv_to_rgb(_hue, 15, 85);
    img_light.image.intense = EDGUI_OPA_80;

    edgui_style_copy(&img_dark, &def);
    img_light.image.color   = edgui_color_hsv_to_rgb(_hue, 85, 65);
    img_light.image.intense = EDGUI_OPA_80;

    theme.style.img.light = &img_light;
    theme.style.img.dark  = &img_dark;
#endif
}

static void line_init(void)
{
#if EDGUI_USE_LINE != 0
    static edgui_style_t line_decor;
    edgui_style_copy(&line_decor, &def);
    line_decor.line.color = edgui_color_hsv_to_rgb(_hue, 50, 50);
    line_decor.line.width = 1;

    theme.style.line.decor = &line_decor;
#endif
}

static void led_init(void)
{
#if EDGUI_USE_LED != 0
    static edgui_style_t led;
    edgui_style_copy(&led, &edgui_style_pretty_color);
    led.body.shadow.width = EDGUI_DPI / 10;
    led.body.radius       = EDGUI_RADIUS_CIRCLE;
    led.body.border.width = EDGUI_DPI / 30;
    led.body.border.opa   = EDGUI_OPA_30;
    led.body.main_color   = edgui_color_hsv_to_rgb(_hue, 100, 100);
    led.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 100, 40);
    led.body.border.color = edgui_color_hsv_to_rgb(_hue, 60, 60);
    led.body.shadow.color = edgui_color_hsv_to_rgb(_hue, 100, 100);

    theme.style.led = &led;
#endif
}

static void slider_init(void)
{
#if EDGUI_USE_SLIDER != 0
    edgui_style_copy(&slider_knob, &def);
    slider_knob.body.opa          = EDGUI_OPA_60;
    slider_knob.body.radius       = EDGUI_RADIUS_CIRCLE;
    slider_knob.body.main_color   = EDGUI_COLOR_PURPLE;
    slider_knob.body.grad_color   = EDGUI_COLOR_SILVER;
    slider_knob.body.border.width = 2;
    slider_knob.body.border.color = EDGUI_COLOR_ORANGE;
    slider_knob.body.border.opa   = EDGUI_OPA_50;

    theme.style.slider.bg    = &bar_bg;
    theme.style.slider.indic = &bar_indic;
    theme.style.slider.knob  = &slider_knob;
#endif
}

static void sw_init(void)
{
#if EDGUI_USE_SW != 0
    static edgui_style_t sw_bg, sw_indic, sw_knob;
    edgui_style_copy(&sw_bg, &bar_bg);
    sw_bg.body.opa            = EDGUI_OPA_COVER;
    sw_bg.body.padding.left   = -2;
    sw_bg.body.padding.right  = -2;
    sw_bg.body.padding.top    = -2;
    sw_bg.body.padding.bottom = -2;
    sw_bg.body.main_color     = edgui_color_hex3(0x666);
    sw_bg.body.grad_color     = edgui_color_hex3(0x999);
    sw_bg.body.border.width   = 2;
    sw_bg.body.border.opa     = EDGUI_OPA_50;

    edgui_style_copy(&sw_indic, &bar_indic);
    sw_indic.body.shadow.width   = EDGUI_DPI / 20;
    sw_indic.body.padding.left   = 0;
    sw_indic.body.padding.right  = 0;
    sw_indic.body.padding.top    = 0;
    sw_indic.body.padding.bottom = 0;

    edgui_style_copy(&sw_knob, &slider_knob);
    sw_knob.body.opa = EDGUI_OPA_80;

    theme.style.sw.bg       = &sw_bg;
    theme.style.sw.indic    = &sw_indic;
    theme.style.sw.knob_off = &sw_knob;
    theme.style.sw.knob_on  = &sw_knob;
#endif
}

static void lmeter_init(void)
{
#if EDGUI_USE_LMETER != 0
    edgui_style_copy(&lmeter_bg, &def);
    lmeter_bg.body.main_color   = edgui_color_hsv_to_rgb(_hue, 10, 70);
    lmeter_bg.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 80, 80);
    lmeter_bg.body.padding.left = EDGUI_DPI / 8; /*Scale line length*/
    lmeter_bg.line.color        = edgui_color_hex3(0x500);
    lmeter_bg.line.width        = 2;

    theme.style.lmeter = &lmeter_bg;

#endif
}

static void gauge_init(void)
{
#if EDGUI_USE_GAUGE != 0
    static edgui_style_t gauge_bg;
    edgui_style_copy(&gauge_bg, &def);
    gauge_bg.body.main_color     = edgui_color_hsv_to_rgb(_hue, 20, 100);
    gauge_bg.body.grad_color     = gauge_bg.body.main_color;
    gauge_bg.body.padding.left   = EDGUI_DPI / 16; /*Scale line length*/
    gauge_bg.body.padding.right  = EDGUI_DPI / 16; /*Scale line length*/
    gauge_bg.body.padding.top    = EDGUI_DPI / 20; /*Needle center size*/
    gauge_bg.body.padding.bottom = EDGUI_DPI / 20; /*Needle center size*/
    gauge_bg.body.padding.inner  = EDGUI_DPI / 12; /*Label - scale distance*/
    gauge_bg.body.border.color   = edgui_color_hex3(0x500);
    gauge_bg.line.color          = edgui_color_hsv_to_rgb(_hue, 80, 75);
    gauge_bg.line.width          = 2;
    gauge_bg.text.color          = edgui_color_hsv_to_rgb(_hue, 10, 90);
    gauge_bg.text.font           = _font;

    theme.style.gauge = &gauge_bg;
#endif
}

static void arc_init(void)
{
#if EDGUI_USE_ARC != 0

    static edgui_style_t arc;
    edgui_style_copy(&arc, &def);
    arc.line.width   = 10;
    arc.line.color   = edgui_color_hsv_to_rgb(_hue, 70, 90);
    arc.line.rounded = 1;

    /*For preloader*/
    arc.body.border.width = 0;

    theme.style.arc = &arc;
#endif
}

static void preload_init(void)
{
#if EDGUI_USE_PRELOAD != 0

    theme.style.preload = theme.style.arc;
#endif
}

static void chart_init(void)
{
#if EDGUI_USE_CHART
    theme.style.chart = &panel;
#endif
}

static void calendar_init(void)
{
#if EDGUI_USE_CALENDAR != 0
    static edgui_style_t ina_days;
    edgui_style_copy(&ina_days, &def);
    ina_days.text.color = edgui_color_hsv_to_rgb(_hue, 0, 50);

    static edgui_style_t high_days;
    edgui_style_copy(&high_days, &def);
    high_days.text.color = edgui_color_hsv_to_rgb(_hue, 50, 90);

    static edgui_style_t week_box;
    edgui_style_copy(&week_box, &def);
    week_box.body.opa            = EDGUI_OPA_TRANSP;
    week_box.body.border.color   = theme.style.panel->body.border.color;
    week_box.body.padding.top    = EDGUI_DPI / 20;
    week_box.body.padding.bottom = EDGUI_DPI / 20;

    static edgui_style_t today_box;
    edgui_style_copy(&today_box, &def);
    today_box.body.main_color     = EDGUI_COLOR_WHITE;
    today_box.body.grad_color     = EDGUI_COLOR_WHITE;
    today_box.body.padding.top    = EDGUI_DPI / 20;
    today_box.body.padding.bottom = EDGUI_DPI / 20;
    today_box.body.radius         = 0;

    theme.style.calendar.bg               = theme.style.panel;
    theme.style.calendar.header           = theme.style.label.prim;
    theme.style.calendar.inactive_days    = theme.style.label.hint;
    theme.style.calendar.highlighted_days = theme.style.label.sec;
    theme.style.calendar.week_box         = &week_box;
    theme.style.calendar.today_box        = &week_box;
    theme.style.calendar.header_pr        = theme.style.label.prim;
#endif
}

static void cb_init(void)
{
#if EDGUI_USE_CB != 0
    static edgui_style_t cb_bg, cb_rel, cb_pr, cb_trel, cb_tpr, cb_ina;
    edgui_style_copy(&cb_rel, &bg);
    cb_rel.body.radius       = EDGUI_DPI / 20;
    cb_rel.body.border.width = 1;
    cb_rel.body.border.color = EDGUI_COLOR_ORANGE;
    cb_rel.body.main_color   = EDGUI_COLOR_PURPLE;
    cb_rel.body.grad_color   = EDGUI_COLOR_SILVER;

    edgui_style_copy(&cb_bg, &bg);
    cb_bg.body.opa            = EDGUI_OPA_TRANSP;
    cb_bg.body.border.width   = 0;
    cb_bg.body.padding.inner  = EDGUI_DPI / 8;
    cb_bg.body.padding.left   = 0;
    cb_bg.body.padding.right  = 0;
    cb_bg.body.padding.top    = 0;
    cb_bg.body.padding.bottom = 0;
    cb_bg.text.font           = _font;

    edgui_style_copy(&cb_pr, &cb_rel);
    cb_pr.body.main_color = edgui_color_hsv_to_rgb(_hue, 10, 90);
    cb_pr.body.main_color = edgui_color_hsv_to_rgb(_hue, 10, 82);

    edgui_style_copy(&cb_trel, &cb_rel);
    cb_trel.body.border.width = 4;
    cb_trel.body.border.color = EDGUI_COLOR_WHITE;
    cb_trel.body.border.opa   = EDGUI_OPA_60;
    cb_trel.body.main_color   = edgui_color_hsv_to_rgb(_hue, 50, 82);
    cb_trel.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 50, 62);

    edgui_style_copy(&cb_tpr, &cb_trel);
    cb_tpr.body.border.color = EDGUI_COLOR_SILVER;
    cb_tpr.body.border.opa   = EDGUI_OPA_70;
    cb_tpr.body.main_color   = edgui_color_hsv_to_rgb(_hue, 50, 72);
    cb_tpr.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 50, 52);

    edgui_style_copy(&cb_ina, &cb_trel);
    cb_ina.body.border.width = 1;
    cb_ina.body.border.color = EDGUI_COLOR_GRAY;
    cb_ina.body.main_color   = EDGUI_COLOR_PURPLE;
    cb_ina.body.grad_color   = EDGUI_COLOR_SILVER;

    theme.style.cb.bg          = &cb_bg;
    theme.style.cb.box.rel     = &cb_rel;
    theme.style.cb.box.pr      = &cb_pr;
    theme.style.cb.box.tgl_rel = &cb_trel;
    theme.style.cb.box.tgl_pr  = &cb_tpr;
    theme.style.cb.box.ina     = &cb_ina;
#endif
}

static void btnm_init(void)
{
#if EDGUI_USE_BTNM
    edgui_style_copy(&btnm_bg, &edgui_style_transp_tight);
    btnm_bg.body.border.width = 1;
    btnm_bg.body.border.color = edgui_color_hsv_to_rgb(_hue, 60, 80);
    btnm_bg.body.border.opa   = EDGUI_OPA_COVER;
    btnm_bg.body.radius       = EDGUI_DPI / 8;

    edgui_style_copy(&btnm_rel, &edgui_style_plain);
    btnm_rel.body.opa    = EDGUI_OPA_TRANSP;
    btnm_rel.body.radius = EDGUI_DPI / 8;
    btnm_rel.text.color  = edgui_color_hsv_to_rgb(_hue, 60, 80);
    btnm_rel.text.font   = _font;

    edgui_style_copy(&btnm_pr, &edgui_style_plain);
    btnm_pr.body.main_color = edgui_color_hsv_to_rgb(_hue, 40, 70);
    btnm_pr.body.grad_color = edgui_color_hsv_to_rgb(_hue, 40, 70);
    btnm_pr.body.radius     = EDGUI_DPI / 8;
    btnm_pr.text.color      = edgui_color_hsv_to_rgb(_hue, 40, 40);
    btnm_pr.text.font       = _font;

    edgui_style_copy(&btnm_trel, &btnm_rel);
    btnm_trel.body.border.color = edgui_color_hsv_to_rgb(_hue, 80, 80);
    btnm_trel.body.border.width = 3;

    edgui_style_copy(&btnm_ina, &btnm_rel);
    btnm_ina.text.color = edgui_color_hsv_to_rgb(_hue, 10, 60);

    theme.style.btnm.bg          = &btnm_bg;
    theme.style.btnm.btn.rel     = &btnm_rel;
    theme.style.btnm.btn.pr      = &btnm_pr;
    theme.style.btnm.btn.tgl_rel = &btnm_trel;
    theme.style.btnm.btn.tgl_pr  = &btnm_pr;
    theme.style.btnm.btn.ina     = &btnm_ina;
#endif
}

static void kb_init(void)
{
#if EDGUI_USE_KB
    theme.style.kb.bg          = &btnm_bg;
    theme.style.kb.btn.rel     = &btnm_rel;
    theme.style.kb.btn.pr      = &btnm_pr;
    theme.style.kb.btn.tgl_rel = &btnm_trel;
    theme.style.kb.btn.tgl_pr  = &btnm_pr;
    theme.style.kb.btn.ina     = &btnm_ina;
#endif
}

static void mbox_init(void)
{
#if EDGUI_USE_MBOX
    static edgui_style_t mbox_bg;
    edgui_style_copy(&mbox_bg, &panel);
    mbox_bg.body.shadow.width = EDGUI_DPI / 12;

    theme.style.mbox.bg      = &mbox_bg;
    theme.style.mbox.btn.bg  = &edgui_style_transp;
    theme.style.mbox.btn.rel = &btn_trel;
    theme.style.mbox.btn.pr  = &btn_tpr;
#endif
}

static void page_init(void)
{
#if EDGUI_USE_PAGE
    theme.style.page.bg   = &panel;
    theme.style.page.scrl = &edgui_style_transp_fit;
    theme.style.page.sb   = &sb;
#endif
}

static void ta_init(void)
{
#if EDGUI_USE_TA
    theme.style.ta.area    = &panel;
    theme.style.ta.oneline = &panel;
    theme.style.ta.cursor  = NULL;
    theme.style.ta.sb      = &sb;
#endif
}

static void spinbox_init(void)
{
#if EDGUI_USE_SPINBOX
    theme.style.spinbox.bg     = &panel;
    theme.style.spinbox.cursor = theme.style.ta.cursor;
    theme.style.spinbox.sb     = theme.style.ta.sb;
#endif
}

static void list_init(void)
{
#if EDGUI_USE_LIST != 0
    static edgui_style_t list_bg, list_rel, list_pr, list_trel, list_tpr, list_ina;
    edgui_style_copy(&list_rel, &def);
    list_rel.body.opa          = EDGUI_OPA_TRANSP;
    list_rel.body.border.width = 1;
    list_rel.body.border.color = edgui_color_hsv_to_rgb(_hue, 50, 85);
    list_rel.body.border.opa   = EDGUI_OPA_COVER;
    list_rel.text.color        = edgui_color_hsv_to_rgb(_hue, 10, 94);
    list_rel.text.font         = _font;

    edgui_style_copy(&list_pr, &list_rel);
    list_pr.body.opa        = EDGUI_OPA_TRANSP;
    list_pr.body.opa        = EDGUI_OPA_COVER;
    list_pr.body.main_color = edgui_color_hsv_to_rgb(_hue, 34, 41);
    list_pr.body.grad_color = edgui_color_hsv_to_rgb(_hue, 34, 41);
    list_pr.text.color      = edgui_color_hsv_to_rgb(_hue, 7, 96);

    edgui_style_copy(&list_trel, &list_rel);
    edgui_style_copy(&list_tpr, &list_pr);
    edgui_style_copy(&list_ina, &def);

    edgui_style_copy(&list_bg, &list_rel);
    list_bg.body.padding.left   = 0;
    list_bg.body.padding.right  = 0;
    list_bg.body.padding.top    = 0;
    list_bg.body.padding.bottom = 0;

    theme.style.list.sb          = &sb;
    theme.style.list.bg          = &list_bg;
    theme.style.list.scrl        = &edgui_style_transp_tight;
    theme.style.list.btn.rel     = &list_rel;
    theme.style.list.btn.pr      = &list_pr;
    theme.style.list.btn.tgl_rel = &list_trel;
    theme.style.list.btn.tgl_pr  = &list_tpr;
    theme.style.list.btn.ina     = &list_ina;
#endif
}

static void ddlist_init(void)
{
#if EDGUI_USE_DDLIST != 0
    edgui_style_copy(&ddlist_bg, &panel);
    ddlist_bg.text.line_space     = EDGUI_DPI / 8;
    ddlist_bg.body.padding.left   = EDGUI_DPI / 6;
    ddlist_bg.body.padding.right  = EDGUI_DPI / 6;
    ddlist_bg.body.padding.top    = EDGUI_DPI / 6;
    ddlist_bg.body.padding.bottom = EDGUI_DPI / 6;

    edgui_style_copy(&ddlist_sel, &panel);
    ddlist_sel.body.main_color = edgui_color_hsv_to_rgb(_hue, 45, 70);
    ddlist_sel.body.grad_color = edgui_color_hsv_to_rgb(_hue, 45, 70);
    ddlist_sel.body.opa        = EDGUI_OPA_COVER;
    ddlist_sel.body.radius     = 0;

    theme.style.ddlist.bg  = &ddlist_bg;
    theme.style.ddlist.sel = &ddlist_sel;
    theme.style.ddlist.sb  = &sb;
#endif
}

static void roller_init(void)
{
#if EDGUI_USE_ROLLER != 0
    static edgui_style_t roller_bg, roller_sel;
    edgui_style_copy(&roller_bg, &ddlist_bg);
    roller_bg.text.line_space   = EDGUI_DPI / 6;
    roller_bg.body.radius       = EDGUI_DPI / 20;
    roller_bg.body.main_color   = edgui_color_hex3(0x500);
    roller_bg.body.grad_color   = edgui_color_hex3(0x005);
    roller_bg.body.border.opa   = EDGUI_OPA_30;
    roller_bg.text.opa          = EDGUI_OPA_70;
    roller_bg.text.color        = edgui_color_hsv_to_rgb(_hue, 20, 70);
    roller_bg.body.shadow.width = 0;

    edgui_style_copy(&roller_sel, &panel);
    roller_sel.body.opa    = EDGUI_OPA_TRANSP;
    roller_sel.body.radius = 0;
    roller_sel.text.opa    = EDGUI_OPA_COVER;
    roller_sel.text.color  = edgui_color_hsv_to_rgb(_hue, 70, 95);

    theme.style.roller.bg  = &roller_bg;
    theme.style.roller.sel = &roller_sel;
#endif
}

static void tabview_init(void)
{
#if EDGUI_USE_TABVIEW != 0
    static edgui_style_t tab_rel, tab_pr, tab_trel, tab_tpr, tab_indic;
    edgui_style_copy(&tab_rel, &def);
    tab_rel.body.main_color     = edgui_color_hex3(0x500);
    tab_rel.body.grad_color     = edgui_color_hex3(0x005);
    tab_rel.body.padding.left   = 0;
    tab_rel.body.padding.right  = 0;
    tab_rel.body.padding.top    = EDGUI_DPI / 6;
    tab_rel.body.padding.bottom = EDGUI_DPI / 6;
    tab_rel.body.padding.inner  = 0;
    tab_rel.body.border.width   = 1;
    tab_rel.body.border.color   = EDGUI_COLOR_SILVER;
    tab_rel.body.border.opa     = EDGUI_OPA_40;
    tab_rel.text.color          = edgui_color_hex3(0xDDD);
    tab_rel.text.font           = _font;

    edgui_style_copy(&tab_pr, &tab_rel);
    tab_pr.body.main_color = edgui_color_hex3(0x005);
    tab_pr.body.grad_color = edgui_color_hex3(0x500);

    edgui_style_copy(&tab_trel, &def);
    tab_trel.body.opa            = EDGUI_OPA_TRANSP;
    tab_trel.body.padding.left   = 0;
    tab_trel.body.padding.right  = 0;
    tab_trel.body.padding.top    = EDGUI_DPI / 6;
    tab_trel.body.padding.bottom = EDGUI_DPI / 6;
    tab_trel.body.padding.inner  = 0;
    tab_trel.body.border.width   = 1;
    tab_trel.body.border.color   = EDGUI_COLOR_SILVER;
    tab_trel.body.border.opa     = EDGUI_OPA_40;
    tab_trel.text.color          = edgui_color_hsv_to_rgb(_hue, 10, 94);
    tab_trel.text.font           = _font;

    edgui_style_copy(&tab_tpr, &def);
    tab_tpr.body.main_color     = EDGUI_COLOR_GRAY;
    tab_tpr.body.grad_color     = EDGUI_COLOR_GRAY;
    tab_tpr.body.padding.left   = 0;
    tab_tpr.body.padding.right  = 0;
    tab_tpr.body.padding.top    = EDGUI_DPI / 6;
    tab_tpr.body.padding.bottom = EDGUI_DPI / 6;
    tab_tpr.body.padding.inner  = 0;
    tab_tpr.body.border.width   = 1;
    tab_tpr.body.border.color   = EDGUI_COLOR_SILVER;
    tab_tpr.body.border.opa     = EDGUI_OPA_40;
    tab_tpr.text.color          = edgui_color_hsv_to_rgb(_hue, 10, 94);
    tab_tpr.text.font           = _font;

    edgui_style_copy(&tab_indic, &def);
    tab_indic.body.border.width  = 0;
    tab_indic.body.main_color    = edgui_color_hsv_to_rgb(_hue, 80, 87);
    tab_indic.body.grad_color    = edgui_color_hsv_to_rgb(_hue, 80, 87);
    tab_indic.body.padding.inner = EDGUI_DPI / 10; /*Indicator height*/

    theme.style.tabview.bg          = &bg;
    theme.style.tabview.indic       = &tab_indic;
    theme.style.tabview.btn.bg      = &edgui_style_transp_tight;
    theme.style.tabview.btn.rel     = &tab_rel;
    theme.style.tabview.btn.pr      = &tab_pr;
    theme.style.tabview.btn.tgl_rel = &tab_trel;
    theme.style.tabview.btn.tgl_pr  = &tab_tpr;
#endif
}

static void tileview_init(void)
{
#if EDGUI_USE_TILEVIEW != 0
    theme.style.tileview.bg   = &edgui_style_transp_tight;
    theme.style.tileview.scrl = &edgui_style_transp_tight;
    theme.style.tileview.sb   = theme.style.page.sb;
#endif
}

static void table_init(void)
{
#if EDGUI_USE_TABLE != 0
    static edgui_style_t cell;
    edgui_style_copy(&cell, &panel);
    cell.body.radius       = 0;
    cell.body.border.width = 1;

    theme.style.table.bg   = &edgui_style_transp_tight;
    theme.style.table.cell = &cell;
#endif
}

static void win_init(void)
{
#if EDGUI_USE_WIN != 0
    static edgui_style_t win_header;

    edgui_style_copy(&win_header, &panel);
    win_header.body.radius         = 0;
    win_header.body.padding.left   = EDGUI_DPI / 12;
    win_header.body.padding.right  = EDGUI_DPI / 12;
    win_header.body.padding.top    = EDGUI_DPI / 20;
    win_header.body.padding.bottom = EDGUI_DPI / 20;
    win_header.body.border.opa     = panel.body.border.opa;
    win_header.body.border.width   = panel.body.border.width;
    win_header.body.border.color   = edgui_color_hsv_to_rgb(_hue, 20, 80);
    win_header.text.color          = edgui_color_hsv_to_rgb(_hue, 5, 100);

    theme.style.win.bg      = &bg;
    theme.style.win.sb      = &sb;
    theme.style.win.header  = &win_header;
    theme.style.win.content = &edgui_style_transp;
    theme.style.win.btn.rel = &btn_rel;
    theme.style.win.btn.pr  = &btn_pr;
#endif
}

#if EDGUI_USE_GROUP

static void style_mod(edgui_group_t * group, edgui_style_t * style)
{
    (void)group; /*Unused*/
#if EDGUI_COLOR_DEPTH != 1
    style->body.border.width = 2;
    style->body.border.color = EDGUI_COLOR_SILVER;
    style->body.border.opa   = EDGUI_OPA_70;
    style->body.shadow.width = EDGUI_DPI / 20;
    style->body.shadow.color = edgui_color_hsv_to_rgb(_hue, 20, 90);
    style->body.main_color   = edgui_color_hsv_to_rgb(_hue, 40, 80);
    style->body.grad_color   = edgui_color_hsv_to_rgb(_hue, 40, 80);
#else
    style->body.border.opa   = EDGUI_OPA_COVER;
    style->body.border.color = EDGUI_COLOR_BLACK;
    style->body.border.width = 2;
#endif
}

static void style_mod_edit(edgui_group_t * group, edgui_style_t * style)
{
    (void)group; /*Unused*/
#if EDGUI_COLOR_DEPTH != 1
    /*Make the style to be a little bit orange*/
    style->body.border.opa   = EDGUI_OPA_COVER;
    style->body.border.color = EDGUI_COLOR_GREEN;

    /*If not empty or has border then emphasis the border*/
    if(style->body.opa != EDGUI_OPA_TRANSP || style->body.border.width != 0) style->body.border.width = EDGUI_DPI / 20;

    style->body.main_color   = edgui_color_mix(style->body.main_color, EDGUI_COLOR_GREEN, EDGUI_OPA_70);
    style->body.grad_color   = edgui_color_mix(style->body.grad_color, EDGUI_COLOR_GREEN, EDGUI_OPA_70);
    style->body.shadow.color = edgui_color_mix(style->body.shadow.color, EDGUI_COLOR_GREEN, EDGUI_OPA_60);

    style->text.color = edgui_color_mix(style->text.color, EDGUI_COLOR_GREEN, EDGUI_OPA_70);
#else
    style->body.border.opa   = EDGUI_OPA_COVER;
    style->body.border.color = EDGUI_COLOR_BLACK;
    style->body.border.width = 3;
#endif
}

#endif /*EDGUI_USE_GROUP*/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the nemo theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
edgui_theme_t * edgui_theme_nemo_init(uint16_t hue, edgui_font_t * font)
{
    if(font == NULL) font = EDGUI_FONT_DEFAULT;

    _hue  = hue;
    _font = font;

    /*For backward compatibility initialize all theme elements with a default style */
    uint16_t i;
    edgui_style_t ** style_p = (edgui_style_t **)&theme.style;
    for(i = 0; i < EDGUI_THEME_STYLE_COUNT; i++) {
        *style_p = &def;
        style_p++;
    }

    basic_init();
    btn_init();
    label_init();
    bar_init();
    img_init();
    line_init();
    led_init();
    slider_init();
    sw_init();
    lmeter_init();
    gauge_init();
    arc_init();
    preload_init();
    chart_init();
    calendar_init();
    cb_init();
    btnm_init();
    kb_init();
    mbox_init();
    page_init();
    ta_init();
    spinbox_init();
    list_init();
    ddlist_init();
    roller_init();
    tabview_init();
    tileview_init();
    table_init();
    win_init();

#if EDGUI_USE_GROUP
    theme.group.style_mod_xcb      = style_mod;
    theme.group.style_mod_edit_xcb = style_mod_edit;
#endif

    return &theme;
}

/**
 * Get a pointer to the theme
 * @return pointer to the theme
 */
edgui_theme_t * edgui_theme_get_nemo(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
