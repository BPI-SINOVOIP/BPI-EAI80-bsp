/**
 * @file edgui_lmeter.h
 *
 */

#ifndef EDGUI_LMETER_H
#define EDGUI_LMETER_H

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

#if EDGUI_USE_LMETER != 0

#include "../edgui_core/edgui_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of line meter*/
typedef struct
{
    /*No inherited ext.*/ /*Ext. of ancestor*/
    /*New data for this type */
    uint16_t scale_angle; /*Angle of the scale in deg. (0..360)*/
    uint8_t line_cnt;     /*Count of lines */
    int16_t cur_value;
    int16_t min_value;
    int16_t max_value;
} edgui_lmeter_ext_t;

/*Styles*/
enum {
    EDGUI_LMETER_STYLE_MAIN,
};
typedef uint8_t edgui_lmeter_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a line meter objects
 * @param par pointer to an object, it will be the parent of the new line meter
 * @param copy pointer to a line meter object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created line meter
 */
edgui_obj_t * edgui_lmeter_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the line meter
 * @param lmeter pointer to a line meter object
 * @param value new value
 */
void edgui_lmeter_set_value(edgui_obj_t * lmeter, int16_t value);

/**
 * Set minimum and the maximum values of a line meter
 * @param lmeter pointer to he line meter object
 * @param min minimum value
 * @param max maximum value
 */
void edgui_lmeter_set_range(edgui_obj_t * lmeter, int16_t min, int16_t max);

/**
 * Set the scale settings of a line meter
 * @param lmeter pointer to a line meter object
 * @param angle angle of the scale (0..360)
 * @param line_cnt number of lines
 */
void edgui_lmeter_set_scale(edgui_obj_t * lmeter, uint16_t angle, uint8_t line_cnt);

/**
 * Set the styles of a line meter
 * @param lmeter pointer to a line meter object
 * @param type which style should be set (can be only `EDGUI_LMETER_STYLE_MAIN`)
 * @param style set the style of the line meter
 */
static __inline void edgui_lmeter_set_style(edgui_obj_t * lmeter, edgui_lmeter_style_t type, edgui_style_t * style)
{
    (void)type; /*Unused*/
    edgui_obj_set_style(lmeter, style);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the value of the line meter
 */
int16_t edgui_lmeter_get_value(const edgui_obj_t * lmeter);

/**
 * Get the minimum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the minimum value of the line meter
 */
int16_t edgui_lmeter_get_min_value(const edgui_obj_t * lmeter);

/**
 * Get the maximum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the maximum value of the line meter
 */
int16_t edgui_lmeter_get_max_value(const edgui_obj_t * lmeter);

/**
 * Get the scale number of a line meter
 * @param lmeter pointer to a line meter object
 * @return number of the scale units
 */
uint8_t edgui_lmeter_get_line_count(const edgui_obj_t * lmeter);

/**
 * Get the scale angle of a line meter
 * @param lmeter pointer to a line meter object
 * @return angle of the scale
 */
uint16_t edgui_lmeter_get_scale_angle(const edgui_obj_t * lmeter);

/**
 * Get the style of a line meter
 * @param lmeter pointer to a line meter object
 * @param type which style should be get (can be only `EDGUI_LMETER_STYLE_MAIN`)
 * @return pointer to the line meter's style
 */
static __inline const edgui_style_t * edgui_lmeter_get_style(const edgui_obj_t * lmeter, edgui_lmeter_style_t type)
{
    (void)type; /*Unused*/
    return edgui_obj_get_style(lmeter);
}

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_LMETER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_LMETER_H*/
