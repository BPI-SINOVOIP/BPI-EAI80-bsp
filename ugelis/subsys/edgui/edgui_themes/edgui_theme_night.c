/**
 * @file edgui_theme_night.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_theme.h"

#if EDGUI_USE_THEME_NIGHT

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
static edgui_theme_t theme;
static edgui_style_t def;

/*Static style definitions*/
static edgui_style_t scr, bg, sb, panel;
static edgui_style_t prim, sec, hint;

/*Saved input parameters*/
static uint16_t _hue;
static edgui_font_t * _font;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void basic_init(void)
{
    edgui_style_copy(&def, &edgui_style_pretty); /*Initialize the default style*/
    def.text.font = _font;

    edgui_style_copy(&bg, &edgui_style_plain);
    bg.body.main_color = edgui_color_hsv_to_rgb(_hue, 11, 30);
    bg.body.grad_color = edgui_color_hsv_to_rgb(_hue, 11, 30);
    bg.text.color      = edgui_color_hsv_to_rgb(_hue, 5, 95);
    bg.text.font       = _font;
    bg.image.color     = edgui_color_hsv_to_rgb(_hue, 5, 95);

    edgui_style_copy(&scr, &bg);
    scr.body.padding.bottom = 0;
    scr.body.padding.top    = 0;
    scr.body.padding.left   = 0;
    scr.body.padding.right  = 0;

    edgui_style_copy(&sb, &def);
    sb.body.main_color     = edgui_color_hsv_to_rgb(_hue, 30, 60);
    sb.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 30, 60);
    sb.body.border.width   = 0;
    sb.body.padding.inner  = EDGUI_DPI / 20;
    sb.body.padding.left   = 0;
    sb.body.padding.right  = 0;
    sb.body.padding.top    = 0;
    sb.body.padding.bottom = 0;
    sb.body.radius         = EDGUI_DPI / 30;
    sb.body.opa            = EDGUI_OPA_COVER;

    edgui_style_copy(&panel, &bg);
    panel.body.main_color     = edgui_color_hsv_to_rgb(_hue, 11, 18);
    panel.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 11, 18);
    panel.body.radius         = EDGUI_DPI / 20;
    panel.body.border.color   = edgui_color_hsv_to_rgb(_hue, 10, 25);
    panel.body.border.width   = 1;
    panel.body.border.opa     = EDGUI_OPA_COVER;
    panel.body.padding.left   = EDGUI_DPI / 10;
    panel.body.padding.right  = EDGUI_DPI / 10;
    panel.body.padding.top    = EDGUI_DPI / 10;
    panel.body.padding.bottom = EDGUI_DPI / 10;
    panel.line.color          = edgui_color_hsv_to_rgb(_hue, 20, 40);
    panel.line.width          = 1;

    theme.style.scr   = &scr;
    theme.style.bg    = &bg;
    theme.style.panel = &def;
}

static void cont_init(void)
{
#if EDGUI_USE_CONT != 0

    theme.style.cont = &panel;
#endif
}
static void btn_init(void)
{
#if EDGUI_USE_BTN != 0
	static edgui_style_t btn_rel, btn_pr, btn_tgl_rel, btn_tgl_pr, btn_ina;

    edgui_style_copy(&btn_rel, &def);
    btn_rel.body.main_color     = edgui_color_hsv_to_rgb(_hue, 10, 40);
    btn_rel.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 10, 20);
    btn_rel.body.border.color   = edgui_color_hex3(0x111);
    btn_rel.body.border.width   = 1;
    btn_rel.body.border.opa     = EDGUI_OPA_70;
    btn_rel.body.padding.left   = EDGUI_DPI / 4;
    btn_rel.body.padding.right  = EDGUI_DPI / 4;
    btn_rel.body.padding.top    = EDGUI_DPI / 8;
    btn_rel.body.padding.bottom = EDGUI_DPI / 8;
    btn_rel.body.shadow.type    = EDGUI_SHADOW_BOTTOM;
    btn_rel.body.shadow.color   = edgui_color_hex3(0x111);
    btn_rel.body.shadow.width   = EDGUI_DPI / 30;
    btn_rel.text.color          = edgui_color_hex3(0xeee);
    btn_rel.image.color         = edgui_color_hex3(0xeee);

    edgui_style_copy(&btn_pr, &btn_rel);
    btn_pr.body.main_color = edgui_color_hsv_to_rgb(_hue, 10, 30);
    btn_pr.body.grad_color = edgui_color_hsv_to_rgb(_hue, 10, 10);

    edgui_style_copy(&btn_tgl_rel, &btn_rel);
    btn_tgl_rel.body.main_color   = edgui_color_hsv_to_rgb(_hue, 10, 20);
    btn_tgl_rel.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 10, 40);
    btn_tgl_rel.body.shadow.width = EDGUI_DPI / 40;
    btn_tgl_rel.text.color        = edgui_color_hex3(0xddd);
    btn_tgl_rel.image.color       = edgui_color_hex3(0xddd);

    edgui_style_copy(&btn_tgl_pr, &btn_rel);
    btn_tgl_pr.body.main_color   = edgui_color_hsv_to_rgb(_hue, 10, 10);
    btn_tgl_pr.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 10, 30);
    btn_tgl_pr.body.shadow.width = EDGUI_DPI / 30;
    btn_tgl_pr.text.color        = edgui_color_hex3(0xddd);
    btn_tgl_pr.image.color       = edgui_color_hex3(0xddd);

    edgui_style_copy(&btn_ina, &btn_rel);
    btn_ina.body.main_color   = edgui_color_hsv_to_rgb(_hue, 10, 20);
    btn_ina.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 10, 20);
    btn_ina.body.shadow.width = 0;
    btn_ina.text.color        = edgui_color_hex3(0xaaa);
    btn_ina.image.color       = edgui_color_hex3(0xaaa);

    theme.style.btn.rel     = &btn_rel;
    theme.style.btn.pr      = &btn_pr;
    theme.style.btn.tgl_rel = &btn_tgl_rel;
    theme.style.btn.tgl_pr  = &btn_tgl_pr;
    theme.style.btn.ina     = &btn_ina;
#endif
}

static void label_init(void)
{
#if EDGUI_USE_LABEL != 0

    edgui_style_copy(&prim, &bg);
    prim.text.color = edgui_color_hsv_to_rgb(_hue, 5, 95);

    edgui_style_copy(&sec, &bg);
    sec.text.color = edgui_color_hsv_to_rgb(_hue, 15, 65);

    edgui_style_copy(&hint, &bg);
    hint.text.color = edgui_color_hsv_to_rgb(_hue, 20, 55);

    theme.style.label.prim = &prim;
    theme.style.label.sec  = &sec;
    theme.style.label.hint = &hint;
#endif
}

static void line_init(void)
{
#if EDGUI_USE_LINE != 0

    theme.style.line.decor = &def;
#endif
}

static void led_init(void)
{
#if EDGUI_USE_LED != 0
    static edgui_style_t led;
    edgui_style_copy(&led, &def);
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

static void img_init(void)
{
#if EDGUI_USE_IMG != 0

    theme.style.img.light = &def;
    theme.style.img.dark  = &def;
#endif
}

static void bar_init(void)
{
#if EDGUI_USE_BAR
	static edgui_style_t bar_bg, bar_indic;
    edgui_style_copy(&bar_bg, &panel);
    bar_bg.body.padding.left   = EDGUI_DPI / 16;
    bar_bg.body.padding.right  = EDGUI_DPI / 16;
    bar_bg.body.padding.top    = EDGUI_DPI / 16;
    bar_bg.body.padding.bottom = EDGUI_DPI / 16;
    bar_bg.body.radius         = EDGUI_RADIUS_CIRCLE;

    edgui_style_copy(&bar_indic, &def);
    bar_indic.body.main_color     = edgui_color_hsv_to_rgb(_hue, 80, 70);
    bar_indic.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 80, 70);
    bar_indic.body.border.color   = edgui_color_hsv_to_rgb(_hue, 20, 15);
    bar_indic.body.border.width   = 1;
    bar_indic.body.border.opa     = EDGUI_OPA_COVER;
    bar_indic.body.radius         = EDGUI_RADIUS_CIRCLE;
    bar_indic.body.padding.left   = 0;
    bar_indic.body.padding.right  = 0;
    bar_indic.body.padding.top    = 0;
    bar_indic.body.padding.bottom = 0;

    theme.style.bar.bg    = &bar_bg;
    theme.style.bar.indic = &bar_indic;
#endif
}

static void slider_init(void)
{
#if EDGUI_USE_SLIDER != 0
	static edgui_style_t slider_knob;
    edgui_style_copy(&slider_knob, theme.style.btn.rel);
    slider_knob.body.radius = EDGUI_RADIUS_CIRCLE;

    theme.style.slider.bg    = theme.style.bar.bg;
    theme.style.slider.indic = theme.style.bar.indic;
    theme.style.slider.knob  = &slider_knob;
#endif
}

static void sw_init(void)
{
#if EDGUI_USE_SW != 0

    theme.style.sw.bg       = theme.style.bar.bg;
    theme.style.sw.indic    = theme.style.bar.indic;
    theme.style.sw.knob_off = theme.style.slider.knob;
    theme.style.sw.knob_on  = theme.style.slider.knob;
#endif
}

static void lmeter_init(void)
{
#if EDGUI_USE_LMETER != 0
	static edgui_style_t lmeter_bg;
    edgui_style_copy(&lmeter_bg, &def);
    lmeter_bg.body.main_color    = edgui_color_hsv_to_rgb(_hue, 10, 70);
    lmeter_bg.body.grad_color    = edgui_color_hsv_to_rgb(_hue, 95, 90);
    lmeter_bg.body.padding.left  = EDGUI_DPI / 10; /*Scale line length*/
    lmeter_bg.body.padding.inner = EDGUI_DPI / 10; /*Text padding*/
    lmeter_bg.body.border.color  = edgui_color_hex3(0x333);
    lmeter_bg.line.color         = edgui_color_hex3(0x555);
    lmeter_bg.line.width         = 1;
    lmeter_bg.text.color         = edgui_color_hex3(0xddd);

    theme.style.lmeter = &lmeter_bg;
#endif
}

static void gauge_init(void)
{
#if EDGUI_USE_GAUGE != 0
    static edgui_style_t gauge_bg;
    edgui_style_copy(&gauge_bg, &def);
    gauge_bg.body.main_color = edgui_color_hsv_to_rgb(_hue, 10, 70);
    gauge_bg.body.grad_color = gauge_bg.body.main_color;
    gauge_bg.line.color      = edgui_color_hsv_to_rgb(_hue, 80, 75);
    gauge_bg.line.width      = 1;
    gauge_bg.text.color      = edgui_color_hex3(0xddd);

    theme.style.gauge = &gauge_bg;
#endif
}

static void arc_init(void)
{
#if EDGUI_USE_ARC != 0

    static edgui_style_t arc;
    edgui_style_copy(&arc, &def);
    arc.line.width   = 8;
    arc.line.color   = edgui_color_hsv_to_rgb(_hue, 80, 70);
    arc.line.rounded = 1;

    /*For preloader*/
    arc.body.border.width   = 7;
    arc.body.border.color   = edgui_color_hsv_to_rgb(_hue, 11, 48);
    arc.body.padding.left   = 1;
    arc.body.padding.right  = 1;
    arc.body.padding.top    = 1;
    arc.body.padding.bottom = 1;

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
#if EDGUI_USE_CALENDAR
    static edgui_style_t cal_bg;
    edgui_style_copy(&cal_bg, &bg);
    cal_bg.body.main_color     = edgui_color_hsv_to_rgb(_hue, 10, 40);
    cal_bg.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 10, 40);
    cal_bg.body.border.color   = edgui_color_hex3(0x333);
    cal_bg.body.border.width   = 1;
    cal_bg.body.radius         = EDGUI_DPI / 20;
    cal_bg.body.padding.left   = EDGUI_DPI / 10;
    cal_bg.body.padding.right  = EDGUI_DPI / 10;
    cal_bg.body.padding.top    = EDGUI_DPI / 10;
    cal_bg.body.padding.bottom = EDGUI_DPI / 10;

    static edgui_style_t cal_header;
    edgui_style_copy(&cal_header, &bg);
    cal_header.body.main_color     = edgui_color_hsv_to_rgb(_hue, 10, 20);
    cal_header.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 10, 20);
    cal_header.body.radius         = 0;
    cal_header.body.border.width   = 1;
    cal_header.body.border.color   = edgui_color_hex3(0x333);
    cal_header.body.padding.left   = EDGUI_DPI / 10;
    cal_header.body.padding.right  = EDGUI_DPI / 10;
    cal_header.body.padding.top    = EDGUI_DPI / 10;
    cal_header.body.padding.bottom = EDGUI_DPI / 10;

    static edgui_style_t week_box;
    edgui_style_copy(&week_box, &panel);
    week_box.body.main_color     = edgui_color_hsv_to_rgb(_hue, 30, 45);
    week_box.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 30, 45);
    week_box.body.radius         = EDGUI_DPI / 20;
    week_box.body.border.width   = 1;
    week_box.body.padding.left   = EDGUI_DPI / 20;
    week_box.body.padding.right  = EDGUI_DPI / 20;
    week_box.body.padding.top    = EDGUI_DPI / 25;
    week_box.body.padding.bottom = EDGUI_DPI / 25;

    static edgui_style_t today_box;
    edgui_style_copy(&today_box, &week_box);
    today_box.body.main_color     = edgui_color_hsv_to_rgb(_hue, 80, 70);
    today_box.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 80, 70);
    today_box.body.radius         = EDGUI_DPI / 20;
    today_box.body.padding.left   = EDGUI_DPI / 14;
    today_box.body.padding.right  = EDGUI_DPI / 14;
    today_box.body.padding.top    = EDGUI_DPI / 14;
    today_box.body.padding.bottom = EDGUI_DPI / 14;

    static edgui_style_t highlighted_days;
    edgui_style_copy(&highlighted_days, &bg);
    highlighted_days.text.color = edgui_color_hsv_to_rgb(_hue, 40, 80);

    static edgui_style_t ina_days;
    edgui_style_copy(&ina_days, &bg);
    ina_days.text.color = edgui_color_hsv_to_rgb(_hue, 0, 60);

    theme.style.calendar.bg               = &cal_bg;
    theme.style.calendar.header           = &cal_header;
    theme.style.calendar.week_box         = &week_box;
    theme.style.calendar.today_box        = &today_box;
    theme.style.calendar.highlighted_days = &highlighted_days;
    theme.style.calendar.day_names        = &cal_bg;
    theme.style.calendar.inactive_days    = &ina_days;
#endif
}

static void cb_init(void)
{
#if EDGUI_USE_CB != 0

    static edgui_style_t rel, pr, tgl_rel, tgl_pr, ina;

    edgui_style_copy(&rel, &def);
    rel.body.radius       = EDGUI_DPI / 20;
    rel.body.main_color   = edgui_color_hsv_to_rgb(_hue, 10, 95);
    rel.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 10, 95);
    rel.body.border.color = edgui_color_hsv_to_rgb(_hue, 10, 50);
    rel.body.border.width = 2;
    ;

    edgui_style_copy(&pr, &rel);
    pr.body.main_color   = edgui_color_hsv_to_rgb(_hue, 10, 80);
    pr.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 10, 80);
    pr.body.border.color = edgui_color_hsv_to_rgb(_hue, 10, 20);
    pr.body.border.width = 1;
    ;

    edgui_style_copy(&tgl_rel, &rel);
    tgl_rel.body.main_color   = edgui_color_hsv_to_rgb(_hue, 80, 90);
    tgl_rel.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 80, 90);
    tgl_rel.body.border.color = edgui_color_hsv_to_rgb(_hue, 80, 50);

    edgui_style_copy(&tgl_pr, &tgl_rel);
    tgl_pr.body.main_color   = edgui_color_hsv_to_rgb(_hue, 80, 70);
    tgl_pr.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 80, 70);
    tgl_pr.body.border.color = edgui_color_hsv_to_rgb(_hue, 80, 30);
    tgl_pr.body.border.width = 1;
    ;

    edgui_style_copy(&ina, &rel);
    ina.body.main_color   = edgui_color_hex3(0x777);
    ina.body.grad_color   = edgui_color_hex3(0x777);
    ina.body.border.width = 0;

    theme.style.cb.bg          = &edgui_style_transp;
    theme.style.cb.box.rel     = &rel;
    theme.style.cb.box.pr      = &pr;
    theme.style.cb.box.tgl_rel = &tgl_rel;
    theme.style.cb.box.tgl_pr  = &tgl_pr;
    theme.style.cb.box.ina     = &def;
#endif
}

static void btnm_init(void)
{
#if EDGUI_USE_BTNM
    static edgui_style_t btnm_bg, rel, pr, tgl_rel, tgl_pr, ina;

    edgui_style_copy(&btnm_bg, theme.style.btn.rel);
    btnm_bg.body.padding.left   = 2;
    btnm_bg.body.padding.right  = 2;
    btnm_bg.body.padding.top    = 2;
    btnm_bg.body.padding.bottom = 2;
    btnm_bg.body.padding.inner  = 0;
    btnm_bg.body.border.width   = 1;

    edgui_style_copy(&rel, theme.style.btn.rel);
    rel.body.border.part  = EDGUI_BORDER_FULL | EDGUI_BORDER_INTERNAL;
    rel.body.border.width = 1;
    rel.body.radius       = 2;

    edgui_style_copy(&pr, theme.style.btn.pr);
    pr.body.border.part  = rel.body.border.part;
    pr.body.border.width = rel.body.border.width;
    pr.body.radius       = rel.body.radius;

    edgui_style_copy(&tgl_rel, theme.style.btn.tgl_rel);
    tgl_rel.body.border.part  = rel.body.border.part;
    tgl_rel.body.border.width = rel.body.border.width;
    tgl_rel.body.radius       = rel.body.radius;

    edgui_style_copy(&tgl_pr, theme.style.btn.pr);
    tgl_pr.body.border.part  = rel.body.border.part;
    tgl_pr.body.border.width = rel.body.border.width;
    tgl_pr.body.radius       = rel.body.radius;

    edgui_style_copy(&ina, theme.style.btn.ina);
    ina.body.border.part  = rel.body.border.part;
    ina.body.border.width = rel.body.border.width;
    ina.body.radius       = rel.body.radius;

    theme.style.btnm.bg          = &btnm_bg;
    theme.style.btnm.btn.rel     = &rel;
    theme.style.btnm.btn.pr      = &pr;
    theme.style.btnm.btn.tgl_rel = &tgl_rel;
    theme.style.btnm.btn.tgl_pr  = &tgl_pr;
    theme.style.btnm.btn.ina     = &ina;
#endif
}

static void kb_init(void)
{
#if EDGUI_USE_KB
    theme.style.kb.bg          = &bg;
    theme.style.kb.btn.rel     = theme.style.btn.rel;
    theme.style.kb.btn.pr      = theme.style.btn.pr;
    theme.style.kb.btn.tgl_rel = theme.style.btn.tgl_rel;
    theme.style.kb.btn.tgl_pr  = theme.style.btn.tgl_pr;
    theme.style.kb.btn.ina     = theme.style.btn.ina;
#endif
}

static void mbox_init(void)
{
#if EDGUI_USE_MBOX
    static edgui_style_t mbox_bg;
    edgui_style_copy(&mbox_bg, &bg);
    mbox_bg.body.main_color   = edgui_color_hsv_to_rgb(_hue, 30, 30);
    mbox_bg.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 30, 30);
    mbox_bg.body.border.color = edgui_color_hsv_to_rgb(_hue, 11, 20);
    mbox_bg.body.border.width = 1;
    mbox_bg.body.shadow.width = EDGUI_DPI / 10;
    mbox_bg.body.shadow.color = edgui_color_hex3(0x222);
    mbox_bg.body.radius       = EDGUI_DPI / 20;
    theme.style.mbox.bg       = &mbox_bg;
    theme.style.mbox.btn.bg   = &edgui_style_transp;
    theme.style.mbox.btn.rel  = theme.style.btn.rel;
    theme.style.mbox.btn.pr   = theme.style.btn.pr;
#endif
}

static void page_init(void)
{
#if EDGUI_USE_PAGE

    static edgui_style_t page_scrl;
    edgui_style_copy(&page_scrl, &bg);
    page_scrl.body.main_color   = edgui_color_hsv_to_rgb(_hue, 10, 40);
    page_scrl.body.grad_color   = edgui_color_hsv_to_rgb(_hue, 10, 40);
    page_scrl.body.border.color = edgui_color_hex3(0x333);
    page_scrl.body.border.width = 1;
    page_scrl.body.radius       = EDGUI_DPI / 20;

    theme.style.page.bg   = &panel;
    theme.style.page.scrl = &page_scrl;
    theme.style.page.sb   = &sb;
#endif
}

static void ta_init(void)
{
#if EDGUI_USE_TA
    theme.style.ta.area    = &panel;
    theme.style.ta.oneline = &panel;
    theme.style.ta.cursor  = NULL;
    theme.style.ta.sb      = &def;
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
    static edgui_style_t list_bg, list_btn_rel, list_btn_pr, list_btn_tgl_rel, list_btn_tgl_pr;

    edgui_style_copy(&list_bg, &panel);
    list_bg.body.padding.top    = 0;
    list_bg.body.padding.bottom = 0;
    list_bg.body.padding.left   = 0;
    list_bg.body.padding.right  = 0;
    list_bg.body.padding.inner  = 0;

    edgui_style_copy(&list_btn_rel, &bg);
    list_btn_rel.body.opa            = EDGUI_OPA_TRANSP;
    list_btn_rel.body.border.part    = EDGUI_BORDER_BOTTOM;
    list_btn_rel.body.border.color   = edgui_color_hsv_to_rgb(_hue, 10, 5);
    list_btn_rel.body.border.width   = 1;
    list_btn_rel.body.radius         = EDGUI_DPI / 10;
    list_btn_rel.text.color          = edgui_color_hsv_to_rgb(_hue, 5, 80);
    list_btn_rel.image.color         = edgui_color_hsv_to_rgb(_hue, 5, 80);
    list_btn_rel.body.padding.top    = EDGUI_DPI / 6;
    list_btn_rel.body.padding.bottom = EDGUI_DPI / 6;
    list_btn_rel.body.padding.left   = EDGUI_DPI / 8;
    list_btn_rel.body.padding.right  = EDGUI_DPI / 8;

    edgui_style_copy(&list_btn_pr, theme.style.btn.pr);
    list_btn_pr.body.main_color     = edgui_color_hsv_to_rgb(_hue, 10, 5);
    list_btn_pr.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 10, 5);
    list_btn_pr.body.border.color   = edgui_color_hsv_to_rgb(_hue, 10, 5);
    list_btn_pr.body.border.width   = 0;
    list_btn_pr.body.padding.top    = EDGUI_DPI / 6;
    list_btn_pr.body.padding.bottom = EDGUI_DPI / 6;
    list_btn_pr.body.padding.left   = EDGUI_DPI / 8;
    list_btn_pr.body.padding.right  = EDGUI_DPI / 8;
    list_btn_pr.text.color          = edgui_color_hsv_to_rgb(_hue, 5, 80);
    list_btn_pr.image.color         = edgui_color_hsv_to_rgb(_hue, 5, 80);

    edgui_style_copy(&list_btn_tgl_rel, &list_btn_rel);
    list_btn_tgl_rel.body.opa        = EDGUI_OPA_COVER;
    list_btn_tgl_rel.body.main_color = edgui_color_hsv_to_rgb(_hue, 80, 70);
    list_btn_tgl_rel.body.grad_color = edgui_color_hsv_to_rgb(_hue, 80, 70);
    list_btn_tgl_rel.body.border.color   = edgui_color_hsv_to_rgb(_hue, 60, 40);
    list_btn_tgl_rel.body.radius         = list_bg.body.radius;

    edgui_style_copy(&list_btn_tgl_pr, &list_btn_tgl_rel);
    list_btn_tgl_pr.body.main_color = edgui_color_hsv_to_rgb(_hue, 80, 60);
    list_btn_tgl_pr.body.grad_color = edgui_color_hsv_to_rgb(_hue, 80, 60);

    theme.style.list.sb          = &sb;
    theme.style.list.bg          = &list_bg;
    theme.style.list.scrl        = &edgui_style_transp_tight;
    theme.style.list.btn.rel     = &list_btn_rel;
    theme.style.list.btn.pr      = &list_btn_pr;
    theme.style.list.btn.tgl_rel = &list_btn_tgl_rel;
    theme.style.list.btn.tgl_pr  = &list_btn_tgl_pr;
    theme.style.list.btn.ina     = &def;
#endif
}

static void ddlist_init(void)
{
#if EDGUI_USE_DDLIST != 0
	static edgui_style_t ddlist_bg, ddlist_sel;
    edgui_style_copy(&ddlist_bg, theme.style.btn.rel);
    ddlist_bg.text.line_space     = EDGUI_DPI / 8;
    ddlist_bg.body.padding.top    = EDGUI_DPI / 8;
    ddlist_bg.body.padding.bottom = EDGUI_DPI / 8;
    ddlist_bg.body.padding.left   = EDGUI_DPI / 8;
    ddlist_bg.body.padding.right  = EDGUI_DPI / 8;
    ddlist_bg.body.radius         = EDGUI_DPI / 30;

    edgui_style_copy(&ddlist_sel, theme.style.btn.rel);
    ddlist_sel.body.main_color = edgui_color_hsv_to_rgb(_hue, 20, 50);
    ddlist_sel.body.grad_color = edgui_color_hsv_to_rgb(_hue, 20, 50);
    ddlist_sel.body.radius     = 0;

    theme.style.ddlist.bg  = &ddlist_bg;
    theme.style.ddlist.sel = &ddlist_sel;
    theme.style.ddlist.sb  = &def;
#endif
}

static void roller_init(void)
{
#if EDGUI_USE_ROLLER != 0
    static edgui_style_t roller_bg;

    edgui_style_copy(&roller_bg, theme.style.ddlist.bg);
    roller_bg.body.main_color = edgui_color_hsv_to_rgb(_hue, 10, 20);
    roller_bg.body.grad_color = edgui_color_hsv_to_rgb(_hue, 10, 40);
    roller_bg.text.color      = edgui_color_hsv_to_rgb(_hue, 5, 70);
    roller_bg.text.opa        = EDGUI_OPA_60;

    theme.style.roller.bg  = &roller_bg;
    theme.style.roller.sel = theme.style.ddlist.sel;
#endif
}

static void tabview_init(void)
{
#if EDGUI_USE_TABVIEW != 0
    theme.style.tabview.bg          = &bg;
    theme.style.tabview.indic       = &edgui_style_transp;
    theme.style.tabview.btn.bg      = &edgui_style_transp;
    theme.style.tabview.btn.rel     = theme.style.btn.rel;
    theme.style.tabview.btn.pr      = theme.style.btn.pr;
    theme.style.tabview.btn.tgl_rel = theme.style.btn.tgl_rel;
    theme.style.tabview.btn.tgl_pr  = theme.style.btn.tgl_pr;
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
    cell.body.radius         = 0;
    cell.body.border.width   = 1;
    cell.body.padding.left   = EDGUI_DPI / 12;
    cell.body.padding.right  = EDGUI_DPI / 12;
    cell.body.padding.top    = EDGUI_DPI / 12;
    cell.body.padding.bottom = EDGUI_DPI / 12;

    theme.style.table.bg   = &edgui_style_transp_tight;
    theme.style.table.cell = &cell;
#endif
}

static void win_init(void)
{
#if EDGUI_USE_WIN != 0
    static edgui_style_t win_bg;
    edgui_style_copy(&win_bg, &bg);
    win_bg.body.border.color = edgui_color_hex3(0x333);
    win_bg.body.border.width = 1;

    static edgui_style_t win_header;
    edgui_style_copy(&win_header, &win_bg);
    win_header.body.main_color     = edgui_color_hsv_to_rgb(_hue, 10, 20);
    win_header.body.grad_color     = edgui_color_hsv_to_rgb(_hue, 10, 20);
    win_header.body.radius         = 0;
    win_header.body.padding.left   = 0;
    win_header.body.padding.right  = 0;
    win_header.body.padding.top    = 0;
    win_header.body.padding.bottom = 0;

    static edgui_style_t win_btn_pr;
    edgui_style_copy(&win_btn_pr, &def);
    win_btn_pr.body.main_color = edgui_color_hsv_to_rgb(_hue, 10, 10);
    win_btn_pr.body.grad_color = edgui_color_hsv_to_rgb(_hue, 10, 10);
    win_btn_pr.text.color      = edgui_color_hex3(0xaaa);
    win_btn_pr.image.color     = edgui_color_hex3(0xaaa);

    theme.style.win.bg      = &win_bg;
    theme.style.win.sb      = &sb;
    theme.style.win.header  = &win_header;
    theme.style.win.content = &edgui_style_transp;
    theme.style.win.btn.rel = &edgui_style_transp;
    theme.style.win.btn.pr  = &win_btn_pr;
#endif
}

#if EDGUI_USE_GROUP

static void style_mod(edgui_group_t * group, edgui_style_t * style)
{
    (void)group; /*Unused*/
#if EDGUI_COLOR_DEPTH != 1
    /*Make the style to be a little bit orange*/
    style->body.border.opa   = EDGUI_OPA_COVER;
    style->body.border.color = edgui_color_hsv_to_rgb(_hue, 80, 70);

    /*If not empty or has border then emphasis the border*/
    if(style->body.opa != EDGUI_OPA_TRANSP || style->body.border.width != 0) style->body.border.width = EDGUI_DPI / 20;
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
 * Initialize the night theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
edgui_theme_t * edgui_theme_night_init(uint16_t hue, edgui_font_t * font)
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
    cont_init();
    btn_init();
    label_init();
    img_init();
    line_init();
    led_init();
    bar_init();
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
edgui_theme_t * edgui_theme_get_night(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
