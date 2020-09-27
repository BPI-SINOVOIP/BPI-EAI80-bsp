/**
 * @file edgui_led.h
 *
 */

#ifndef EDGUI_LED_H
#define EDGUI_LED_H

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

#if EDGUI_USE_LED != 0

#include "../edgui_core/edgui_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of led*/
typedef struct
{
    /*No inherited ext.*/
    /*New data for this type */
    uint8_t bright; /*Current brightness of the LED (0..255)*/
} edgui_led_ext_t;

/*Styles*/
enum {
    EDGUI_LED_STYLE_MAIN,
};
typedef uint8_t edgui_led_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a led objects
 * @param par pointer to an object, it will be the parent of the new led
 * @param copy pointer to a led object, if not NULL then the new object will be copied from it
 * @return pointer to the created led
 */
edgui_obj_t * edgui_led_create(edgui_obj_t * par, const edgui_obj_t * copy);

/**
 * Set the brightness of a LED object
 * @param led pointer to a LED object
 * @param bright 0 (max. dark) ... 255 (max. light)
 */
void edgui_led_set_bright(edgui_obj_t * led, uint8_t bright);

/**
 * Light on a LED
 * @param led pointer to a LED object
 */
void edgui_led_on(edgui_obj_t * led);

/**
 * Light off a LED
 * @param led pointer to a LED object
 */
void edgui_led_off(edgui_obj_t * led);

/**
 * Toggle the state of a LED
 * @param led pointer to a LED object
 */
void edgui_led_toggle(edgui_obj_t * led);

/**
 * Set the style of a led
 * @param led pointer to a led object
 * @param type which style should be set (can be only `EDGUI_LED_STYLE_MAIN`)
 * @param style pointer to a style
 */
static __inline void edgui_led_set_style(edgui_obj_t * led, edgui_led_style_t type, const edgui_style_t * style)
{
    (void)type; /*Unused*/
    edgui_obj_set_style(led, style);
}

/**
 * Get the brightness of a LEd object
 * @param led pointer to LED object
 * @return bright 0 (max. dark) ... 255 (max. light)
 */
uint8_t edgui_led_get_bright(const edgui_obj_t * led);

/**
 * Get the style of an led object
 * @param led pointer to an led object
 * @param type which style should be get (can be only `EDGUI_CHART_STYLE_MAIN`)
 * @return pointer to the led's style
 */
static __inline const edgui_style_t * edgui_led_get_style(const edgui_obj_t * led, edgui_led_style_t type)
{
    (void)type; /*Unused*/
    return edgui_obj_get_style(led);
}

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_LED*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_LED_H*/
