/**
 * @file edgui_gauge.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_gauge.h"
#if EDGUI_USE_GAUGE != 0

#include "../edgui_draw/edgui_draw.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_txt.h"
#include "../edgui_misc/edgui_math.h"
#include "../edgui_misc/edgui_utils.h"
#include <stdio.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define EDGUI_GAUGE_DEF_NEEDLE_COLOR EDGUI_COLOR_RED
#define EDGUI_GAUGE_DEF_LABEL_COUNT 6
#define EDGUI_GAUGE_DEF_LINE_COUNT 21 /*Should be: ((label_cnt - 1) * internal_lines) + 1*/
#define EDGUI_GAUGE_DEF_ANGLE 220
#define EDGUI_GAUGE_INTERPOLATE_SHIFT 5 /*Interpolate the needle drawing between to degrees*/
#define EDGUI_GAUGE_INTERPOLATE_MASK 0x1F

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_gauge_design(edgui_obj_t * gauge, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_gauge_signal(edgui_obj_t * gauge, edgui_signal_t sign, void * param);
static void edgui_gauge_draw_scale(edgui_obj_t * gauge, const edgui_area_t * mask);
static void edgui_gauge_draw_needle(edgui_obj_t * gauge, const edgui_area_t * mask);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_design_cb_t ancestor_design;
static edgui_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a gauge objects
 * @param par pointer to an object, it will be the parent of the new gauge
 * @param copy pointer to a gauge object, if not NULL then the new object will be copied from it
 * @return pointer to the created gauge
 */
edgui_obj_t * edgui_gauge_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("gauge create started");

    /*Create the ancestor gauge*/
    edgui_obj_t * new_gauge = edgui_lmeter_create(par, copy);
    edgui_mem_assert(new_gauge);
    if(new_gauge == NULL) return NULL;

    /*Allocate the gauge type specific extended data*/
    edgui_gauge_ext_t * ext = edgui_obj_allocate_ext_attr(new_gauge, sizeof(edgui_gauge_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */
    ext->needle_count  = 0;
    ext->values        = NULL;
    ext->needle_colors = NULL;
    ext->label_count   = EDGUI_GAUGE_DEF_LABEL_COUNT;
    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_gauge);
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_gauge);

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_gauge, edgui_gauge_signal);
    edgui_obj_set_design_cb(new_gauge, edgui_gauge_design);

    /*Init the new gauge gauge*/
    if(copy == NULL) {
        edgui_gauge_set_scale(new_gauge, EDGUI_GAUGE_DEF_ANGLE, EDGUI_GAUGE_DEF_LINE_COUNT, EDGUI_GAUGE_DEF_LABEL_COUNT);
        edgui_gauge_set_needle_count(new_gauge, 1, NULL);
        edgui_gauge_set_critical_value(new_gauge, 80);
        edgui_obj_set_size(new_gauge, 2 * EDGUI_DPI, 2 * EDGUI_DPI);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_gauge_set_style(new_gauge, EDGUI_GAUGE_STYLE_MAIN, th->style.gauge);
        } else {
            edgui_gauge_set_style(new_gauge, EDGUI_GAUGE_STYLE_MAIN, &edgui_style_pretty_color);
        }
    }
    /*Copy an existing gauge*/
    else {
        edgui_gauge_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        edgui_gauge_set_needle_count(new_gauge, copy_ext->needle_count, copy_ext->needle_colors);

        uint8_t i;
        for(i = 0; i < ext->needle_count; i++) {
            ext->values[i] = copy_ext->values[i];
        }
        ext->label_count = copy_ext->label_count;
        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_gauge);
    }

    EDGUI_LOG_INFO("gauge created");

    return new_gauge;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of needles
 * @param gauge pointer to gauge object
 * @param needle_cnt new count of needles
 * @param colors an array of colors for needles (with 'num' elements)
 */
void edgui_gauge_set_needle_count(edgui_obj_t * gauge, uint8_t needle_cnt, const edgui_color_t colors[])
{
    edgui_gauge_ext_t * ext = edgui_obj_get_ext_attr(gauge);

    if(ext->needle_count != needle_cnt) {
        if(ext->values != NULL) {
            edgui_mem_free(ext->values);
            ext->values = NULL;
        }

        ext->values = edgui_mem_realloc(ext->values, needle_cnt * sizeof(int16_t));
        edgui_mem_assert(ext->values);
        if(ext->values == NULL) return;

        int16_t min = edgui_gauge_get_min_value(gauge);
        uint8_t n;
        for(n = ext->needle_count; n < needle_cnt; n++) {
            ext->values[n] = min;
        }

        ext->needle_count = needle_cnt;
    }

    ext->needle_colors = colors;
    edgui_obj_invalidate(gauge);
}

/**
 * Set the value of a needle
 * @param gauge pointer to a gauge
 * @param needle_id the id of the needle
 * @param value the new value
 */
void edgui_gauge_set_value(edgui_obj_t * gauge, uint8_t needle_id, int16_t value)
{
    edgui_gauge_ext_t * ext = edgui_obj_get_ext_attr(gauge);

    if(needle_id >= ext->needle_count) return;
    if(ext->values[needle_id] == value) return;

    int16_t min = edgui_gauge_get_min_value(gauge);
    int16_t max = edgui_gauge_get_max_value(gauge);

    if(value > max)
        value = max;
    else if(value < min)
        value = min;

    ext->values[needle_id] = value;

    edgui_obj_invalidate(gauge);
}

/**
 * Set the scale settings of a gauge
 * @param gauge pointer to a gauge object
 * @param angle angle of the scale (0..360)
 * @param line_cnt count of scale lines.
 * The get a given "subdivision" lines between label, `line_cnt` = (sub_div + 1) * (label_cnt - 1) +
 * 1
 * @param label_cnt count of scale labels.
 */
void edgui_gauge_set_scale(edgui_obj_t * gauge, uint16_t angle, uint8_t line_cnt, uint8_t label_cnt)
{
    edgui_lmeter_set_scale(gauge, angle, line_cnt);

    edgui_gauge_ext_t * ext = edgui_obj_get_ext_attr(gauge);
    ext->label_count     = label_cnt;
    edgui_obj_invalidate(gauge);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a needle
 * @param gauge pointer to gauge object
 * @param needle the id of the needle
 * @return the value of the needle [min,max]
 */
int16_t edgui_gauge_get_value(const edgui_obj_t * gauge, uint8_t needle)
{
    edgui_gauge_ext_t * ext = edgui_obj_get_ext_attr(gauge);
    int16_t min          = edgui_gauge_get_min_value(gauge);

    if(needle >= ext->needle_count) return min;

    return ext->values[needle];
}

/**
 * Get the count of needles on a gauge
 * @param gauge pointer to gauge
 * @return count of needles
 */
uint8_t edgui_gauge_get_needle_count(const edgui_obj_t * gauge)
{
    edgui_gauge_ext_t * ext = edgui_obj_get_ext_attr(gauge);
    return ext->needle_count;
}

/**
 * Set the number of labels (and the thicker lines too)
 * @param gauge pointer to a gauge object
 * @return count of labels
 */
uint8_t edgui_gauge_get_label_count(const edgui_obj_t * gauge)
{
    edgui_gauge_ext_t * ext = edgui_obj_get_ext_attr(gauge);
    return ext->label_count;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the gauges
 * @param gauge pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_gauge_design(edgui_obj_t * gauge, const edgui_area_t * mask, edgui_design_mode_t mode)
{

    /*Return false if the object is not covers the mask_p area*/
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == EDGUI_DESIGN_DRAW_MAIN) {

        /* Store the real pointer because of 'edgui_group'
         * If the object is in focus 'edgui_obj_get_style()' will give a pointer to tmp style
         * and to the real object style. It is important because of style change tricks below*/
        const edgui_style_t * style_ori_p = gauge->style_p;
        const edgui_style_t * style       = edgui_obj_get_style(gauge);
        edgui_gauge_ext_t * ext           = edgui_obj_get_ext_attr(gauge);

        edgui_gauge_draw_scale(gauge, mask);

        /*Draw the ancestor line meter with max value to show the rainbow like line colors*/
        uint16_t line_cnt_tmp = ext->lmeter.line_cnt;
        ancestor_design(gauge, mask, mode); /*To draw lines*/

        /*Temporally modify the line meter to draw longer lines where labels are*/
        edgui_style_t style_tmp;
        edgui_style_copy(&style_tmp, style);
        ext->lmeter.line_cnt         = ext->label_count;                 /*Only to labels*/
        style_tmp.body.padding.left  = style_tmp.body.padding.left * 2;  /*Longer lines*/
        style_tmp.body.padding.right = style_tmp.body.padding.right * 2; /*Longer lines*/
        gauge->style_p               = &style_tmp;

        ancestor_design(gauge, mask, mode); /*To draw lines*/

        ext->lmeter.line_cnt = line_cnt_tmp; /*Restore the parameters*/
        gauge->style_p       = style_ori_p;  /*Restore the ORIGINAL style pointer*/

        edgui_gauge_draw_needle(gauge, mask);

    }
    /*Post draw when the children are drawn*/
    else if(mode == EDGUI_DESIGN_DRAW_POST) {
        ancestor_design(gauge, mask, mode);
    }

    return true;
}

/**
 * Signal function of the gauge
 * @param gauge pointer to a gauge object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_gauge_signal(edgui_obj_t * gauge, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(gauge, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_gauge_ext_t * ext = edgui_obj_get_ext_attr(gauge);
    if(sign == EDGUI_SIGNAL_CLEANUP) {
        edgui_mem_free(ext->values);
        ext->values = NULL;
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_gauge";
    }

    return res;
}

/**
 * Draw the scale on a gauge
 * @param gauge pointer to gauge object
 * @param mask mask of drawing
 */
static void edgui_gauge_draw_scale(edgui_obj_t * gauge, const edgui_area_t * mask)
{
    char scale_txt[16];

    edgui_gauge_ext_t * ext     = edgui_obj_get_ext_attr(gauge);
    const edgui_style_t * style = edgui_obj_get_style(gauge);
    edgui_opa_t opa_scale       = edgui_obj_get_opa_scale(gauge);
    edgui_coord_t r             = edgui_obj_get_width(gauge) / 2 - (3 * style->body.padding.left) - style->body.padding.inner;
    edgui_coord_t x_ofs         = edgui_obj_get_width(gauge) / 2 + gauge->coords.x1;
    edgui_coord_t y_ofs         = edgui_obj_get_height(gauge) / 2 + gauge->coords.y1;
    int16_t scale_angle      = edgui_lmeter_get_scale_angle(gauge);
    uint16_t label_num       = ext->label_count;
    int16_t angle_ofs        = 90 + (360 - scale_angle) / 2;
    int16_t min              = edgui_gauge_get_min_value(gauge);
    int16_t max              = edgui_gauge_get_max_value(gauge);

    uint8_t i;
    for(i = 0; i < label_num; i++) {
        /*Calculate the position a scale label*/
        int16_t angle = (i * scale_angle) / (label_num - 1) + angle_ofs;

        edgui_coord_t y = (int32_t)((int32_t)edgui_trigo_sin(angle) * r) / EDGUI_TRIGO_SIN_MAX;
        y += y_ofs;

        edgui_coord_t x = (int32_t)((int32_t)edgui_trigo_sin(angle + 90) * r) / EDGUI_TRIGO_SIN_MAX;
        x += x_ofs;

        int16_t scale_act = (int32_t)((int32_t)(max - min) * i) / (label_num - 1);
        scale_act += min;
        edgui_utils_num_to_str(scale_act, scale_txt);

        edgui_area_t label_cord;
        edgui_point_t label_size;
        edgui_txt_get_size(&label_size, scale_txt, style->text.font, style->text.letter_space, style->text.line_space,
                        EDGUI_COORD_MAX, EDGUI_TXT_FLAG_NONE);

        /*Draw the label*/
        label_cord.x1 = x - label_size.x / 2;
        label_cord.y1 = y - label_size.y / 2;
        label_cord.x2 = label_cord.x1 + label_size.x;
        label_cord.y2 = label_cord.y1 + label_size.y;

        edgui_draw_label(&label_cord, mask, style, opa_scale, scale_txt, EDGUI_TXT_FLAG_NONE, NULL, -1, -1, NULL);
    }
}
/**
 * Draw the needles of a gauge
 * @param gauge pointer to gauge object
 * @param mask mask of drawing
 */
static void edgui_gauge_draw_needle(edgui_obj_t * gauge, const edgui_area_t * mask)
{
    edgui_style_t style_needle;
    edgui_gauge_ext_t * ext     = edgui_obj_get_ext_attr(gauge);
    const edgui_style_t * style = edgui_gauge_get_style(gauge, EDGUI_GAUGE_STYLE_MAIN);
    edgui_opa_t opa_scale       = edgui_obj_get_opa_scale(gauge);

    edgui_coord_t r      = edgui_obj_get_width(gauge) / 2 - style->body.padding.left;
    edgui_coord_t x_ofs  = edgui_obj_get_width(gauge) / 2 + gauge->coords.x1;
    edgui_coord_t y_ofs  = edgui_obj_get_height(gauge) / 2 + gauge->coords.y1;
    uint16_t angle    = edgui_lmeter_get_scale_angle(gauge);
    int16_t angle_ofs = 90 + (360 - angle) / 2;
    int16_t min       = edgui_gauge_get_min_value(gauge);
    int16_t max       = edgui_gauge_get_max_value(gauge);
    edgui_point_t p_mid;
    edgui_point_t p_end;
    edgui_point_t p_end_low;
    edgui_point_t p_end_high;
    uint8_t i;

    edgui_style_copy(&style_needle, style);

    p_mid.x = x_ofs;
    p_mid.y = y_ofs;
    for(i = 0; i < ext->needle_count; i++) {
        /*Calculate the end point of a needle*/
        int16_t needle_angle =
            (ext->values[i] - min) * angle * (1 << EDGUI_GAUGE_INTERPOLATE_SHIFT) / (max - min);

        int16_t needle_angle_low  = (needle_angle >> EDGUI_GAUGE_INTERPOLATE_SHIFT) + angle_ofs;
        int16_t needle_angle_high = needle_angle_low + 1;

        p_end_low.y = (edgui_trigo_sin(needle_angle_low) * r) / EDGUI_TRIGO_SIN_MAX + y_ofs;
        p_end_low.x = (edgui_trigo_sin(needle_angle_low + 90) * r) / EDGUI_TRIGO_SIN_MAX + x_ofs;

        p_end_high.y = (edgui_trigo_sin(needle_angle_high) * r) / EDGUI_TRIGO_SIN_MAX + y_ofs;
        p_end_high.x = (edgui_trigo_sin(needle_angle_high + 90) * r) / EDGUI_TRIGO_SIN_MAX + x_ofs;

        uint16_t rem  = needle_angle & ((1 << EDGUI_GAUGE_INTERPOLATE_SHIFT) - 1);
        int16_t x_mod = ((EDGUI_MATH_ABS(p_end_high.x - p_end_low.x)) * rem) >> EDGUI_GAUGE_INTERPOLATE_SHIFT;
        int16_t y_mod = ((EDGUI_MATH_ABS(p_end_high.y - p_end_low.y)) * rem) >> EDGUI_GAUGE_INTERPOLATE_SHIFT;

        if(p_end_high.x < p_end_low.x) x_mod = -x_mod;
        if(p_end_high.y < p_end_low.y) y_mod = -y_mod;

        p_end.x = p_end_low.x + x_mod;
        p_end.y = p_end_low.y + y_mod;

        /*Draw the needle with the corresponding color*/
        if(ext->needle_colors == NULL)
            style_needle.line.color = EDGUI_GAUGE_DEF_NEEDLE_COLOR;
        else
            style_needle.line.color = ext->needle_colors[i];

        edgui_draw_line(&p_mid, &p_end, mask, &style_needle, opa_scale);
    }

    /*Draw the needle middle area*/
    edgui_style_t style_neddle_mid;
    edgui_style_copy(&style_neddle_mid, &edgui_style_plain);
    style_neddle_mid.body.main_color = style->body.border.color;
    style_neddle_mid.body.grad_color = style->body.border.color;
    style_neddle_mid.body.radius     = EDGUI_RADIUS_CIRCLE;

    edgui_area_t nm_cord;
    nm_cord.x1 = x_ofs - style->body.radius;
    nm_cord.y1 = y_ofs - style->body.radius;
    nm_cord.x2 = x_ofs + style->body.radius;
    nm_cord.y2 = y_ofs + style->body.radius;

    edgui_draw_rect(&nm_cord, mask, &style_neddle_mid, edgui_obj_get_opa_scale(gauge));
}

#endif
