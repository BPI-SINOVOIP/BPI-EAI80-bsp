/**
 * @file edgui_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_obj.h"
#include "../edgui_misc/edgui_mem.h"
#include "../edgui_misc/edgui_anim.h"

/*********************
 *      DEFINES
 *********************/
#define STYLE_MIX_MAX 256
#define STYLE_MIX_SHIFT 8 /*log2(STYLE_MIX_MAX)*/

#define VAL_PROP(v1, v2, r) v1 + (((v2 - v1) * r) >> STYLE_MIX_SHIFT)
#define STYLE_ATTR_MIX(attr, r)                                                                                        \
    if(start->attr != end->attr) {                                                                                     \
        res->attr = VAL_PROP(start->attr, end->attr, r);                                                               \
    } else {                                                                                                           \
        res->attr = start->attr;                                                                                       \
    }

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if EDGUI_USE_ANIMATION
static void style_animator(edgui_style_anim_dsc_t * dsc, edgui_anim_value_t val);
static void style_animation_common_end_cb(edgui_anim_t * a);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
edgui_style_t edgui_style_scr;
edgui_style_t edgui_style_transp;
edgui_style_t edgui_style_transp_fit;
edgui_style_t edgui_style_transp_tight;
edgui_style_t edgui_style_plain;
edgui_style_t edgui_style_plain_color;
edgui_style_t edgui_style_pretty;
edgui_style_t edgui_style_pretty_color;
edgui_style_t edgui_style_btn_rel;
edgui_style_t edgui_style_btn_pr;
edgui_style_t edgui_style_btn_tgl_rel;
edgui_style_t edgui_style_btn_tgl_pr;
edgui_style_t edgui_style_btn_ina;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 *  Init the basic styles
 */
void edgui_style_init(void)
{
    /* Not White/Black/Gray colors are created by HSV model with
     * HUE = 210*/

    /*Screen style*/
    edgui_style_scr.glass               = 0;
    edgui_style_scr.body.opa            = EDGUI_OPA_COVER;
    edgui_style_scr.body.main_color     = EDGUI_COLOR_WHITE;
    edgui_style_scr.body.grad_color     = EDGUI_COLOR_WHITE;
    edgui_style_scr.body.radius         = 0;
    edgui_style_scr.body.padding.left   = 0;
    edgui_style_scr.body.padding.right  = 0;
    edgui_style_scr.body.padding.top    = 0;
    edgui_style_scr.body.padding.bottom = 0;
    edgui_style_scr.body.padding.inner  = EDGUI_DPI / 20;

    edgui_style_scr.body.border.color = EDGUI_COLOR_BLACK;
    edgui_style_scr.body.border.opa   = EDGUI_OPA_COVER;
    edgui_style_scr.body.border.width = 0;
    edgui_style_scr.body.border.part  = EDGUI_BORDER_FULL;

    edgui_style_scr.body.shadow.color = EDGUI_COLOR_GRAY;
    edgui_style_scr.body.shadow.type  = EDGUI_SHADOW_FULL;
    edgui_style_scr.body.shadow.width = 0;

    edgui_style_scr.text.opa          = EDGUI_OPA_COVER;
    edgui_style_scr.text.color        = edgui_color_make(0x30, 0x30, 0x30);
    edgui_style_scr.text.sel_color    = edgui_color_make(0x55, 0x96, 0xd8);
    edgui_style_scr.text.font         = EDGUI_FONT_DEFAULT;
    edgui_style_scr.text.letter_space = 0;
    edgui_style_scr.text.line_space   = 2;

    edgui_style_scr.image.opa     = EDGUI_OPA_COVER;
    edgui_style_scr.image.color   = edgui_color_make(0x20, 0x20, 0x20);
    edgui_style_scr.image.intense = EDGUI_OPA_TRANSP;

    edgui_style_scr.line.opa     = EDGUI_OPA_COVER;
    edgui_style_scr.line.color   = edgui_color_make(0x20, 0x20, 0x20);
    edgui_style_scr.line.width   = 2;
    edgui_style_scr.line.rounded = 0;

    /*Plain style (by default near the same as the screen style)*/
    edgui_style_copy(&edgui_style_plain, &edgui_style_scr);
    edgui_style_plain.body.padding.left   = EDGUI_DPI / 20;
    edgui_style_plain.body.padding.right  = EDGUI_DPI / 20;
    edgui_style_plain.body.padding.top    = EDGUI_DPI / 20;
    edgui_style_plain.body.padding.bottom = EDGUI_DPI / 20;

    /*Plain color style*/
    edgui_style_copy(&edgui_style_plain_color, &edgui_style_plain);
    edgui_style_plain_color.text.color      = edgui_color_make(0xf0, 0xf0, 0xf0);
    edgui_style_plain_color.image.color     = edgui_color_make(0xf0, 0xf0, 0xf0);
    edgui_style_plain_color.line.color      = edgui_color_make(0xf0, 0xf0, 0xf0);
    edgui_style_plain_color.body.main_color = edgui_color_make(0x55, 0x96, 0xd8);
    edgui_style_plain_color.body.grad_color = edgui_style_plain_color.body.main_color;

    /*Pretty style */
    edgui_style_copy(&edgui_style_pretty, &edgui_style_plain);
    edgui_style_pretty.text.color        = edgui_color_make(0x20, 0x20, 0x20);
    edgui_style_pretty.image.color       = edgui_color_make(0x20, 0x20, 0x20);
    edgui_style_pretty.line.color        = edgui_color_make(0x20, 0x20, 0x20);
    edgui_style_pretty.body.main_color   = EDGUI_COLOR_WHITE;
    edgui_style_pretty.body.grad_color   = EDGUI_COLOR_SILVER;
    edgui_style_pretty.body.radius       = EDGUI_DPI / 15;
    edgui_style_pretty.body.border.color = edgui_color_make(0x40, 0x40, 0x40);
    edgui_style_pretty.body.border.width = EDGUI_DPI / 50 >= 1 ? EDGUI_DPI / 50 : 1;
    edgui_style_pretty.body.border.opa   = EDGUI_OPA_30;

    /*Pretty color style*/
    edgui_style_copy(&edgui_style_pretty_color, &edgui_style_pretty);
    edgui_style_pretty_color.text.color        = edgui_color_make(0xe0, 0xe0, 0xe0);
    edgui_style_pretty_color.image.color       = edgui_color_make(0xe0, 0xe0, 0xe0);
    edgui_style_pretty_color.line.color        = edgui_color_make(0xc0, 0xc0, 0xc0);
    edgui_style_pretty_color.body.main_color   = edgui_color_make(0x6b, 0x9a, 0xc7);
    edgui_style_pretty_color.body.grad_color   = edgui_color_make(0x2b, 0x59, 0x8b);
    edgui_style_pretty_color.body.border.color = edgui_color_make(0x15, 0x2c, 0x42);

    /*Transparent style*/
    edgui_style_copy(&edgui_style_transp, &edgui_style_plain);
    edgui_style_transp.glass             = 1;
    edgui_style_transp.body.border.width = 0;
    edgui_style_transp.body.opa          = EDGUI_OPA_TRANSP;

    /*Transparent fitting size*/
    edgui_style_copy(&edgui_style_transp_fit, &edgui_style_transp);
    edgui_style_transp_fit.body.padding.left   = 0;
    edgui_style_transp_fit.body.padding.right  = 0;
    edgui_style_transp_fit.body.padding.top    = 0;
    edgui_style_transp_fit.body.padding.bottom = 0;

    /*Transparent tight style*/
    edgui_style_copy(&edgui_style_transp_tight, &edgui_style_transp_fit);
    edgui_style_transp_tight.body.padding.inner = 0;

    /*Button released style*/
    edgui_style_copy(&edgui_style_btn_rel, &edgui_style_plain);
    edgui_style_btn_rel.body.main_color     = edgui_color_make(0x76, 0xa2, 0xd0);
    edgui_style_btn_rel.body.grad_color     = edgui_color_make(0x19, 0x3a, 0x5d);
    edgui_style_btn_rel.body.radius         = EDGUI_DPI / 15;
    edgui_style_btn_rel.body.padding.left   = EDGUI_DPI / 4;
    edgui_style_btn_rel.body.padding.right  = EDGUI_DPI / 4;
    edgui_style_btn_rel.body.padding.top    = EDGUI_DPI / 6;
    edgui_style_btn_rel.body.padding.bottom = EDGUI_DPI / 6;
    edgui_style_btn_rel.body.padding.inner  = EDGUI_DPI / 10;
    edgui_style_btn_rel.body.border.color   = edgui_color_make(0x0b, 0x19, 0x28);
    edgui_style_btn_rel.body.border.width   = EDGUI_DPI / 50 >= 1 ? EDGUI_DPI / 50 : 1;
    edgui_style_btn_rel.body.border.opa     = EDGUI_OPA_70;
    edgui_style_btn_rel.body.shadow.color   = EDGUI_COLOR_GRAY;
    edgui_style_btn_rel.body.shadow.width   = 0;
    edgui_style_btn_rel.text.color          = edgui_color_make(0xff, 0xff, 0xff);
    edgui_style_btn_rel.image.color         = edgui_color_make(0xff, 0xff, 0xff);

    /*Button pressed style*/
    edgui_style_copy(&edgui_style_btn_pr, &edgui_style_btn_rel);
    edgui_style_btn_pr.body.main_color = edgui_color_make(0x33, 0x62, 0x94);
    edgui_style_btn_pr.body.grad_color = edgui_color_make(0x10, 0x26, 0x3c);
    edgui_style_btn_pr.text.color      = edgui_color_make(0xa4, 0xb5, 0xc6);
    edgui_style_btn_pr.image.color     = edgui_color_make(0xa4, 0xb5, 0xc6);
    edgui_style_btn_pr.line.color      = edgui_color_make(0xa4, 0xb5, 0xc6);

    /*Button toggle released style*/
    edgui_style_copy(&edgui_style_btn_tgl_rel, &edgui_style_btn_rel);
    edgui_style_btn_tgl_rel.body.main_color   = edgui_color_make(0x0a, 0x11, 0x22);
    edgui_style_btn_tgl_rel.body.grad_color   = edgui_color_make(0x37, 0x62, 0x90);
    edgui_style_btn_tgl_rel.body.border.color = edgui_color_make(0x01, 0x07, 0x0d);
    edgui_style_btn_tgl_rel.text.color        = edgui_color_make(0xc8, 0xdd, 0xf4);
    edgui_style_btn_tgl_rel.image.color       = edgui_color_make(0xc8, 0xdd, 0xf4);
    edgui_style_btn_tgl_rel.line.color        = edgui_color_make(0xc8, 0xdd, 0xf4);

    /*Button toggle pressed style*/
    edgui_style_copy(&edgui_style_btn_tgl_pr, &edgui_style_btn_tgl_rel);
    edgui_style_btn_tgl_pr.body.main_color = edgui_color_make(0x02, 0x14, 0x27);
    edgui_style_btn_tgl_pr.body.grad_color = edgui_color_make(0x2b, 0x4c, 0x70);
    edgui_style_btn_tgl_pr.text.color      = edgui_color_make(0xa4, 0xb5, 0xc6);
    edgui_style_btn_tgl_pr.image.color     = edgui_color_make(0xa4, 0xb5, 0xc6);
    edgui_style_btn_tgl_pr.line.color      = edgui_color_make(0xa4, 0xb5, 0xc6);

    /*Button inactive style*/
    edgui_style_copy(&edgui_style_btn_ina, &edgui_style_btn_rel);
    edgui_style_btn_ina.body.main_color   = edgui_color_make(0xd8, 0xd8, 0xd8);
    edgui_style_btn_ina.body.grad_color   = edgui_color_make(0xd8, 0xd8, 0xd8);
    edgui_style_btn_ina.body.border.color = edgui_color_make(0x90, 0x90, 0x90);
    edgui_style_btn_ina.text.color        = edgui_color_make(0x70, 0x70, 0x70);
    edgui_style_btn_ina.image.color       = edgui_color_make(0x70, 0x70, 0x70);
    edgui_style_btn_ina.line.color        = edgui_color_make(0x70, 0x70, 0x70);
}

/**
 * Copy a style to an other
 * @param dest pointer to the destination style
 * @param src pointer to the source style
 */
void edgui_style_copy(edgui_style_t * dest, const edgui_style_t * src)
{
    memcpy(dest, src, sizeof(edgui_style_t));
}

/**
 * Mix two styles according to a given ratio
 * @param start start style
 * @param end end style
 * @param res store the result style here
 * @param ratio the ratio of mix [0..256]; 0: `start` style; 256: `end` style
 */
void edgui_style_mix(const edgui_style_t * start, const edgui_style_t * end, edgui_style_t * res, uint16_t ratio)
{
    STYLE_ATTR_MIX(body.opa, ratio);
    STYLE_ATTR_MIX(body.radius, ratio);
    STYLE_ATTR_MIX(body.border.width, ratio);
    STYLE_ATTR_MIX(body.border.opa, ratio);
    STYLE_ATTR_MIX(body.shadow.width, ratio);
    STYLE_ATTR_MIX(body.padding.left, ratio);
    STYLE_ATTR_MIX(body.padding.right, ratio);
    STYLE_ATTR_MIX(body.padding.top, ratio);
    STYLE_ATTR_MIX(body.padding.bottom, ratio);
    STYLE_ATTR_MIX(body.padding.inner, ratio);
    STYLE_ATTR_MIX(text.line_space, ratio);
    STYLE_ATTR_MIX(text.letter_space, ratio);
    STYLE_ATTR_MIX(text.opa, ratio);
    STYLE_ATTR_MIX(line.width, ratio);
    STYLE_ATTR_MIX(line.opa, ratio);
    STYLE_ATTR_MIX(image.intense, ratio);
    STYLE_ATTR_MIX(image.opa, ratio);

    edgui_opa_t opa = ratio == STYLE_MIX_MAX ? EDGUI_OPA_COVER : ratio;

    res->body.main_color   = edgui_color_mix(end->body.main_color, start->body.main_color, opa);
    res->body.grad_color   = edgui_color_mix(end->body.grad_color, start->body.grad_color, opa);
    res->body.border.color = edgui_color_mix(end->body.border.color, start->body.border.color, opa);
    res->body.shadow.color = edgui_color_mix(end->body.shadow.color, start->body.shadow.color, opa);
    res->text.color        = edgui_color_mix(end->text.color, start->text.color, opa);
    res->image.color       = edgui_color_mix(end->image.color, start->image.color, opa);
    res->line.color        = edgui_color_mix(end->line.color, start->line.color, opa);

    if(ratio < (STYLE_MIX_MAX >> 1)) {
        res->body.border.part = start->body.border.part;
        res->glass            = start->glass;
        res->text.font        = start->text.font;
        res->body.shadow.type = start->body.shadow.type;
        res->line.rounded     = start->line.rounded;
    } else {
        res->body.border.part = end->body.border.part;
        res->glass            = end->glass;
        res->text.font        = end->text.font;
        res->body.shadow.type = end->body.shadow.type;
        res->line.rounded     = end->line.rounded;
    }
}

#if EDGUI_USE_ANIMATION

void edgui_style_anim_init(edgui_anim_t * a)
{
    edgui_anim_init(a);
    a->start    = 0;
    a->end      = STYLE_MIX_MAX;
    a->exec_cb  = (edgui_anim_exec_xcb_t)style_animator;
    a->path_cb  = edgui_anim_path_linear;
    a->ready_cb = style_animation_common_end_cb;

    edgui_style_anim_dsc_t * dsc;
    dsc = edgui_mem_alloc(sizeof(edgui_style_anim_dsc_t));
    edgui_mem_assert(dsc);
    if(dsc == NULL) return;
    dsc->ready_cb   = NULL;
    dsc->style_anim = NULL;
    edgui_style_copy(&dsc->style_start, &edgui_style_plain);
    edgui_style_copy(&dsc->style_end, &edgui_style_plain);

    a->var = (void *)dsc;
}

void edgui_style_anim_set_styles(edgui_anim_t * a, edgui_style_t * to_anim, const edgui_style_t * start, const edgui_style_t * end)
{

    edgui_style_anim_dsc_t * dsc = a->var;
    dsc->style_anim           = to_anim;
    memcpy(&dsc->style_start, start, sizeof(edgui_style_t));
    memcpy(&dsc->style_end, end, sizeof(edgui_style_t));
    memcpy(dsc->style_anim, start, sizeof(edgui_style_t));
}
#endif
/**********************
 *   STATIC FUNCTIONS
 **********************/
#if EDGUI_USE_ANIMATION
/**
 * Used by the style animations to set the values of a style according to start and end style.
 * @param dsc the 'animated variable' set by edgui_style_anim_create()
 * @param val the current state of the animation between 0 and EDGUI_ANIM_RESOLUTION
 */
static void style_animator(edgui_style_anim_dsc_t * dsc, edgui_anim_value_t val)
{
    const edgui_style_t * start = &dsc->style_start;
    const edgui_style_t * end   = &dsc->style_end;
    edgui_style_t * act         = dsc->style_anim;

    edgui_style_mix(start, end, act, val);

    edgui_obj_report_style_mod(dsc->style_anim);
}

/**
 * Called when a style animation is ready
 * It called the user defined call back and free the allocated memories
 * @param a pointer to the animation
 */
static void style_animation_common_end_cb(edgui_anim_t * a)
{

    (void)a;                            /*Unused*/
    edgui_style_anim_dsc_t * dsc = a->var; /*To avoid casting*/

    if(dsc->ready_cb) dsc->ready_cb(a);

    edgui_mem_free(dsc);
}

#endif
