/**
 * @file edgui_spinbox.h
 *
 */

#ifndef EDGUI_SPINBOX_H
#define EDGUI_SPINBOX_H

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

#if EDGUI_USE_SPINBOX != 0

/*Testing of dependencies*/
#if EDGUI_USE_TA == 0
#error "edgui_spinbox: edgui_ta is required. Enable it in edgui_conf.h (EDGUI_USE_TA  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "../edgui_objx/edgui_ta.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_SPINBOX_MAX_DIGIT_COUNT 16

/**********************
 *      TYPEDEFS
 **********************/

/*Data of spinbox*/
typedef struct
{
    edgui_ta_ext_t ta; /*Ext. of ancestor*/
    /*New data for this type */
    int32_t value;
    int32_t range_max;
    int32_t range_min;
    int32_t step;
    uint16_t digit_count : 4;
    uint16_t dec_point_pos : 4; /*if 0, there is no separator and the number is an integer*/
    uint16_t digit_padding_left : 4;
} edgui_spinbox_ext_t;

/*Styles*/
enum {
    EDGUI_SPINBOX_STYLE_BG,
    EDGUI_SPINBOX_STYLE_SB,
    EDGUI_SPINBOX_STYLE_CURSOR,
};
typedef uint8_t edgui_spinbox_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a spinbox objects
 * @param par pointer to an object, it will be the parent of the new spinbox
 * @param copy pointer to a spinbox object, if not NULL then the new object will be copied from it
 * @return pointer to the created spinbox
 */
edgui_obj_t * edgui_spinbox_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a style of a spinbox.
 * @param templ pointer to template object
 * @param type which style should be set
 * @param style pointer to a style
 */
static __inline void edgui_spinbox_set_style(edgui_obj_t * spinbox, edgui_spinbox_style_t type, edgui_style_t * style)
{
    edgui_ta_set_style(spinbox, type, style);
}

/**
 * Set spinbox value
 * @param spinbox pointer to spinbox
 * @param i value to be set
 */
void edgui_spinbox_set_value(edgui_obj_t * spinbox, int32_t i);

/**
 * Set spinbox digit format (digit count and decimal format)
 * @param spinbox pointer to spinbox
 * @param digit_count number of digit excluding the decimal separator and the sign
 * @param separator_position number of digit before the decimal point. If 0, decimal point is not
 * shown
 */
void edgui_spinbox_set_digit_format(edgui_obj_t * spinbox, uint8_t digit_count, uint8_t separator_position);

/**
 * Set spinbox step
 * @param spinbox pointer to spinbox
 * @param step steps on increment/decrement
 */
void edgui_spinbox_set_step(edgui_obj_t * spinbox, uint32_t step);

/**
 * Set spinbox value range
 * @param spinbox pointer to spinbox
 * @param range_min maximum value, inclusive
 * @param range_max minimum value, inclusive
 */
void edgui_spinbox_set_range(edgui_obj_t * spinbox, int32_t range_min, int32_t range_max);

/**
 * Set spinbox left padding in digits count (added between sign and first digit)
 * @param spinbox pointer to spinbox
 * @param cb Callback function called on value change event
 */
void edgui_spinbox_set_padding_left(edgui_obj_t * spinbox, uint8_t padding);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get style of a spinbox.
 * @param templ pointer to template object
 * @param type which style should be get
 * @return style pointer to the style
 */
static __inline const edgui_style_t * edgui_spinbox_get_style(edgui_obj_t * spinbox, edgui_spinbox_style_t type)
{
    return edgui_ta_get_style(spinbox, type);
}

/**
 * Get the spinbox numeral value (user has to convert to float according to its digit format)
 * @param spinbox pointer to spinbox
 * @return value integer value of the spinbox
 */
int32_t edgui_spinbox_get_value(edgui_obj_t * spinbox);

/*=====================
 * Other functions
 *====================*/

/**
 * Select next lower digit for edition by dividing the step by 10
 * @param spinbox pointer to spinbox
 */
void edgui_spinbox_step_next(edgui_obj_t * spinbox);

/**
 * Select next higher digit for edition by multiplying the step by 10
 * @param spinbox pointer to spinbox
 */
void edgui_spinbox_step_prev(edgui_obj_t * spinbox);

/**
 * Increment spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void edgui_spinbox_increment(edgui_obj_t * spinbox);

/**
 * Decrement spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void edgui_spinbox_decrement(edgui_obj_t * spinbox);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_SPINBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_SPINBOX_H*/
