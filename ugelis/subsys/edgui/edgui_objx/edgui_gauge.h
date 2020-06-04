/**
 * @file edgui_gauge.h
 *
 */

#ifndef EDGUI_GAUGE_H
#define EDGUI_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui_conf.h"
#else
#include "../edgui/edgui_conf.h"
#endif

#if EDGUI_USE_GAUGE != 0

/*Testing of dependencies*/
#if EDGUI_USE_LMETER == 0
#error "edgui_gauge: edgui_lmeter is required. Enable it in edgui_conf.h (EDGUI_USE_LMETER  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_lmeter.h"
#include "edgui_label.h"
#include "edgui_line.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of gauge*/
typedef struct
{
    edgui_lmeter_ext_t lmeter; /*Ext. of ancestor*/
    /*New data for this type */
    int16_t * values;                 /*Array of the set values (for needles) */
    const edgui_color_t * needle_colors; /*Color of the needles (edgui_color_t my_colors[needle_num])*/
    uint8_t needle_count;             /*Number of needles*/
    uint8_t label_count;              /*Number of labels on the scale*/
} edgui_gauge_ext_t;

/*Styles*/
enum {
    EDGUI_GAUGE_STYLE_MAIN,
};
typedef uint8_t edgui_gauge_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a gauge objects
 * @param par pointer to an object, it will be the parent of the new gauge
 * @param copy pointer to a gauge object, if not NULL then the new object will be copied from it
 * @return pointer to the created gauge
 */
edgui_obj_t * edgui_gauge_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of needles
 * @param gauge pointer to gauge object
 * @param needle_cnt new count of needles
 * @param colors an array of colors for needles (with 'num' elements)
 */
void edgui_gauge_set_needle_count(edgui_obj_t * gauge, uint8_t needle_cnt, const edgui_color_t colors[]);

/**
 * Set the value of a needle
 * @param gauge pointer to a gauge
 * @param needle_id the id of the needle
 * @param value the new value
 */
void edgui_gauge_set_value(edgui_obj_t * gauge, uint8_t needle_id, int16_t value);

/**
 * Set minimum and the maximum values of a gauge
 * @param gauge pointer to he gauge object
 * @param min minimum value
 * @param max maximum value
 */
static __inline void edgui_gauge_set_range(edgui_obj_t * gauge, int16_t min, int16_t max)
{
    edgui_lmeter_set_range(gauge, min, max);
}

/**
 * Set a critical value on the scale. After this value 'line.color' scale lines will be drawn
 * @param gauge pointer to a gauge object
 * @param value the critical value
 */
static __inline void edgui_gauge_set_critical_value(edgui_obj_t * gauge, int16_t value)
{
    edgui_lmeter_set_value(gauge, value);
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
void edgui_gauge_set_scale(edgui_obj_t * gauge, uint16_t angle, uint8_t line_cnt, uint8_t label_cnt);

/**
 * Set the styles of a gauge
 * @param gauge pointer to a gauge object
 * @param type which style should be set (can be only `EDGUI_GAUGE_STYLE_MAIN`)
 * @param style set the style of the gauge
 *  */
static __inline void edgui_gauge_set_style(edgui_obj_t * gauge, edgui_gauge_style_t type, edgui_style_t * style)
{
    (void)type; /*Unused*/
    edgui_obj_set_style(gauge, style);
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
int16_t edgui_gauge_get_value(const edgui_obj_t * gauge, uint8_t needle);

/**
 * Get the count of needles on a gauge
 * @param gauge pointer to gauge
 * @return count of needles
 */
uint8_t edgui_gauge_get_needle_count(const edgui_obj_t * gauge);

/**
 * Get the minimum value of a gauge
 * @param gauge pointer to a gauge object
 * @return the minimum value of the gauge
 */
static __inline int16_t edgui_gauge_get_min_value(const edgui_obj_t * lmeter)
{
    return edgui_lmeter_get_min_value(lmeter);
}

/**
 * Get the maximum value of a gauge
 * @param gauge pointer to a gauge object
 * @return the maximum value of the gauge
 */
static __inline int16_t edgui_gauge_get_max_value(const edgui_obj_t * lmeter)
{
    return edgui_lmeter_get_max_value(lmeter);
}

/**
 * Get a critical value on the scale.
 * @param gauge pointer to a gauge object
 * @return the critical value
 */
static __inline int16_t edgui_gauge_get_critical_value(const edgui_obj_t * gauge)
{
    return edgui_lmeter_get_value(gauge);
}

/**
 * Set the number of labels (and the thicker lines too)
 * @param gauge pointer to a gauge object
 * @return count of labels
 */
uint8_t edgui_gauge_get_label_count(const edgui_obj_t * gauge);

/**
 * Get the scale number of a gauge
 * @param gauge pointer to a gauge object
 * @return number of the scale units
 */
static __inline uint8_t edgui_gauge_get_line_count(const edgui_obj_t * gauge)
{
    return edgui_lmeter_get_line_count(gauge);
}

/**
 * Get the scale angle of a gauge
 * @param gauge pointer to a gauge object
 * @return angle of the scale
 */
static __inline uint16_t edgui_gauge_get_scale_angle(const edgui_obj_t * gauge)
{
    return edgui_lmeter_get_scale_angle(gauge);
}

/**
 * Get the style of a gauge
 * @param gauge pointer to a gauge object
 * @param type which style should be get (can be only `EDGUI_GAUGE_STYLE_MAIN`)
 * @return pointer to the gauge's style
 */
static __inline const edgui_style_t * edgui_gauge_get_style(const edgui_obj_t * gauge, edgui_gauge_style_t type)
{
    (void)type; /*Unused*/
    return edgui_obj_get_style(gauge);
}

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_GAUGE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_GAUGE_H*/
