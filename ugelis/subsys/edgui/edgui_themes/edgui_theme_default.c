/**
 * @file edgui_theme_default.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_theme.h"

#if EDGUI_USE_THEME_DEFAULT

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
static edgui_style_t scr;

/*Static style definitions*/
static edgui_style_t sb;
static edgui_style_t plain_bordered;
static edgui_style_t label_prim;
static edgui_style_t label_sec;
static edgui_style_t label_hint;

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

    edgui_style_copy(&scr, &def);
    scr.body.padding.bottom = 0;
    scr.body.padding.top    = 0;
    scr.body.padding.left   = 0;
    scr.body.padding.right  = 0;

    edgui_style_copy(&sb, &edgui_style_pretty_color);
    sb.body.grad_color     = sb.body.main_color;
    sb.body.padding.right  = sb.body.padding.right / 2; /*Make closer to the edges*/
    sb.body.padding.bottom = sb.body.padding.bottom / 2;

    edgui_style_copy(&plain_bordered, &edgui_style_plain);
    plain_bordered.body.border.width = 2;
    plain_bordered.body.border.color = edgui_color_hex3(0xbbb);

    theme.style.bg    = &edgui_style_plain;
    theme.style.scr   = &scr;
    theme.style.panel = &edgui_style_pretty;
}

static void btn_init(void)
{
#if EDGUI_USE_BTN != 0
    theme.style.btn.rel     = &edgui_style_btn_rel;
    theme.style.btn.pr      = &edgui_style_btn_pr;
    theme.style.btn.tgl_rel = &edgui_style_btn_tgl_rel;
    theme.style.btn.tgl_pr  = &edgui_style_btn_tgl_pr;
    theme.style.btn.ina     = &edgui_style_btn_ina;
#endif
}

static void label_init(void)
{
#if EDGUI_USE_LABEL != 0

    edgui_style_copy(&label_prim, &edgui_style_plain);
    edgui_style_copy(&label_sec, &edgui_style_plain);
    edgui_style_copy(&label_hint, &edgui_style_plain);

    label_prim.text.color = edgui_color_hex3(0x111);
    label_sec.text.color  = edgui_color_hex3(0x888);
    label_hint.text.color = edgui_color_hex3(0xaaa);

    theme.style.label.prim = &label_prim;
    theme.style.label.sec  = &label_sec;
    theme.style.label.hint = &label_hint;
#endif
}

static void img_init(void)
{
#if EDGUI_USE_IMG != 0

    theme.style.img.light = &def;
    theme.style.img.dark  = &def;
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

    edgui_style_copy(&led, &edgui_style_pretty_color);
    led.body.shadow.width = EDGUI_DPI / 10;
    led.body.radius       = EDGUI_RADIUS_CIRCLE;
    led.body.border.width = EDGUI_DPI / 30;
    led.body.border.opa   = EDGUI_OPA_30;
    led.body.shadow.color = led.body.main_color;

    theme.style.led = &led;
#endif
}

static void bar_init(void)
{
#if EDGUI_USE_BAR

    theme.style.bar.bg    = &edgui_style_pretty;
    theme.style.bar.indic = &edgui_style_pretty_color;
#endif
}

static void slider_init(void)
{
#if EDGUI_USE_SLIDER != 0
	static edgui_style_t slider_bg;
    edgui_style_copy(&slider_bg, &edgui_style_pretty);
    slider_bg.body.padding.left   = EDGUI_DPI / 20;
    slider_bg.body.padding.right  = EDGUI_DPI / 20;
    slider_bg.body.padding.top    = EDGUI_DPI / 20;
    slider_bg.body.padding.bottom = EDGUI_DPI / 20;

    theme.style.slider.bg    = &slider_bg;
    theme.style.slider.indic = &edgui_style_pretty_color;
    theme.style.slider.knob  = &edgui_style_pretty;
#endif
}

static void sw_init(void)
{
#if EDGUI_USE_SW != 0
	static edgui_style_t sw_bg;
    edgui_style_copy(&sw_bg, &edgui_style_pretty);
    sw_bg.body.padding.left   = 3;
    sw_bg.body.padding.right  = 3;
    sw_bg.body.padding.top    = 3;
    sw_bg.body.padding.bottom = 3;

    theme.style.sw.bg       = &sw_bg;
    theme.style.sw.indic    = &edgui_style_pretty_color;
    theme.style.sw.knob_off = &edgui_style_pretty;
    theme.style.sw.knob_on  = &edgui_style_pretty;
#endif
}

static void lmeter_init(void)
{
#if EDGUI_USE_LMETER != 0
	static edgui_style_t lmeter;
    edgui_style_copy(&lmeter, &edgui_style_pretty_color);
    lmeter.line.color      = edgui_color_hex3(0xddd);
    lmeter.line.width      = 2;
    lmeter.body.main_color = edgui_color_mix(lmeter.body.main_color, EDGUI_COLOR_WHITE, EDGUI_OPA_50);
    lmeter.body.grad_color = edgui_color_mix(lmeter.body.grad_color, EDGUI_COLOR_BLACK, EDGUI_OPA_50);

    theme.style.lmeter = &lmeter;
#endif
}

static void gauge_init(void)
{
#if EDGUI_USE_GAUGE != 0
    static edgui_style_t gauge;
    edgui_style_copy(&gauge, theme.style.lmeter);
    gauge.line.color      = theme.style.lmeter->body.grad_color;
    gauge.line.width      = 2;
    gauge.body.main_color = edgui_color_hex3(0x888);
    gauge.body.grad_color = theme.style.lmeter->body.main_color;
    gauge.text.color      = edgui_color_hex3(0x888);

    theme.style.gauge = &gauge;
#endif
}

static void chart_init(void)
{
#if EDGUI_USE_CHART

    theme.style.chart = &edgui_style_pretty;
#endif
}

static void cb_init(void)
{
#if EDGUI_USE_CB != 0

    theme.style.cb.bg          = &edgui_style_transp;
    theme.style.cb.box.rel     = &edgui_style_pretty;
    theme.style.cb.box.pr      = &edgui_style_btn_pr;
    theme.style.cb.box.tgl_rel = &edgui_style_btn_tgl_rel;
    theme.style.cb.box.tgl_pr  = &edgui_style_btn_tgl_pr;
    theme.style.cb.box.ina     = &edgui_style_btn_ina;
#endif
}

static void btnm_init(void)
{
#if EDGUI_USE_BTNM

    theme.style.btnm.bg          = &edgui_style_pretty;
    theme.style.btnm.btn.rel     = &edgui_style_btn_rel;
    theme.style.btnm.btn.pr      = &edgui_style_btn_pr;
    theme.style.btnm.btn.tgl_rel = &edgui_style_btn_tgl_rel;
    theme.style.btnm.btn.tgl_pr  = &edgui_style_btn_tgl_pr;
    theme.style.btnm.btn.ina     = &edgui_style_btn_ina;
#endif
}

static void kb_init(void)
{
#if EDGUI_USE_KB

    theme.style.kb.bg          = &edgui_style_pretty;
    theme.style.kb.btn.rel     = &edgui_style_btn_rel;
    theme.style.kb.btn.pr      = &edgui_style_btn_pr;
    theme.style.kb.btn.tgl_rel = &edgui_style_btn_tgl_rel;
    theme.style.kb.btn.tgl_pr  = &edgui_style_btn_tgl_pr;
    theme.style.kb.btn.ina     = &edgui_style_btn_ina;
#endif
}

static void mbox_init(void)
{
#if EDGUI_USE_MBOX

    theme.style.mbox.bg      = &edgui_style_pretty;
    theme.style.mbox.btn.bg  = &edgui_style_transp;
    theme.style.mbox.btn.rel = &edgui_style_btn_rel;
    theme.style.mbox.btn.pr  = &edgui_style_btn_tgl_pr;
#endif
}

static void page_init(void)
{
#if EDGUI_USE_PAGE

    theme.style.page.bg   = &edgui_style_pretty;
    theme.style.page.scrl = &edgui_style_transp_tight;
    theme.style.page.sb   = &sb;
#endif
}

static void ta_init(void)
{
#if EDGUI_USE_TA

    theme.style.ta.area    = &edgui_style_pretty;
    theme.style.ta.oneline = &edgui_style_pretty;
    theme.style.ta.cursor  = NULL;
    theme.style.ta.sb      = &sb;
#endif
}

static void list_init(void)
{
#if EDGUI_USE_LIST != 0

    theme.style.list.bg          = &edgui_style_pretty;
    theme.style.list.scrl        = &edgui_style_transp_fit;
    theme.style.list.sb          = &sb;
    theme.style.list.btn.rel     = &edgui_style_btn_rel;
    theme.style.list.btn.pr      = &edgui_style_btn_pr;
    theme.style.list.btn.tgl_rel = &edgui_style_btn_tgl_rel;
    theme.style.list.btn.tgl_pr  = &edgui_style_btn_tgl_pr;
    theme.style.list.btn.ina     = &edgui_style_btn_ina;
#endif
}

static void ddlist_init(void)
{
#if EDGUI_USE_DDLIST != 0

    theme.style.ddlist.bg  = &edgui_style_pretty;
    theme.style.ddlist.sel = &edgui_style_plain_color;
    theme.style.ddlist.sb  = &sb;
#endif
}

static void roller_init(void)
{
#if EDGUI_USE_ROLLER != 0

    theme.style.roller.bg  = &edgui_style_pretty;
    theme.style.roller.sel = &edgui_style_plain_color;
#endif
}

static void tabview_init(void)
{
#if EDGUI_USE_TABVIEW != 0

    theme.style.tabview.bg          = &plain_bordered;
    theme.style.tabview.indic       = &edgui_style_plain_color;
    theme.style.tabview.btn.bg      = &edgui_style_transp;
    theme.style.tabview.btn.rel     = &edgui_style_btn_rel;
    theme.style.tabview.btn.pr      = &edgui_style_btn_pr;
    theme.style.tabview.btn.tgl_rel = &edgui_style_btn_tgl_rel;
    theme.style.tabview.btn.tgl_pr  = &edgui_style_btn_tgl_pr;
#endif
}

static void table_init(void)
{
#if EDGUI_USE_TABLE != 0
    theme.style.table.bg   = &edgui_style_transp_tight;
    theme.style.table.cell = &edgui_style_plain;
#endif
}

static void win_init(void)
{
#if EDGUI_USE_WIN != 0

    theme.style.win.bg      = &plain_bordered;
    theme.style.win.sb      = &sb;
    theme.style.win.header  = &edgui_style_plain_color;
    theme.style.win.content = &edgui_style_transp;
    theme.style.win.btn.rel = &edgui_style_btn_rel;
    theme.style.win.btn.pr  = &edgui_style_btn_pr;
#endif
}

#if EDGUI_USE_GROUP

static void style_mod(edgui_group_t * group, edgui_style_t * style)
{
    (void)group; /*Unused*/
#if EDGUI_COLOR_DEPTH != 1
    /*Make the style to be a little bit orange*/
    style->body.border.opa   = EDGUI_OPA_COVER;
    style->body.border.color = EDGUI_COLOR_ORANGE;

    /*If not empty or has border then emphasis the border*/
    if(style->body.opa != EDGUI_OPA_TRANSP || style->body.border.width != 0) style->body.border.width = EDGUI_DPI / 20;

    style->body.main_color   = edgui_color_mix(style->body.main_color, EDGUI_COLOR_ORANGE, EDGUI_OPA_70);
    style->body.grad_color   = edgui_color_mix(style->body.grad_color, EDGUI_COLOR_ORANGE, EDGUI_OPA_70);
    style->body.shadow.color = edgui_color_mix(style->body.shadow.color, EDGUI_COLOR_ORANGE, EDGUI_OPA_60);

    style->text.color = edgui_color_mix(style->text.color, EDGUI_COLOR_ORANGE, EDGUI_OPA_70);
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
 * Initialize the default theme
 * @param hue [0..360] hue value from HSV color space to define the theme's base color
 * @param font pointer to a font (NULL to use the default)
 * @return pointer to the initialized theme
 */
edgui_theme_t * edgui_theme_default_init(uint16_t hue, edgui_font_t * font)
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
    img_init();
    line_init();
    led_init();
    bar_init();
    slider_init();
    sw_init();
    lmeter_init();
    gauge_init();
    chart_init();
    cb_init();
    btnm_init();
    kb_init();
    mbox_init();
    page_init();
    ta_init();
    list_init();
    ddlist_init();
    roller_init();
    tabview_init();
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
edgui_theme_t * edgui_theme_get_default(void)
{
    return &theme;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
