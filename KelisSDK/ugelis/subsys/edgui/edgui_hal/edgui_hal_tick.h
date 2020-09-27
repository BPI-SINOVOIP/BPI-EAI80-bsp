/**
 * @file edgui_hal_tick.h
 * Provide access to the system tick with 1 millisecond resolution
 */

#ifndef EDGUI_HAL_TICK_H
#define EDGUI_HAL_TICK_H

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
#include <stdint.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/
#ifndef EDGUI_ATTRIBUTE_TICK_INC
#define EDGUI_ATTRIBUTE_TICK_INC
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

//! @cond Doxygen_Suppress

/**
 * You have to call this function periodically
 * @param tick_period the call period of this function in milliseconds
 */
EDGUI_ATTRIBUTE_TICK_INC void edgui_tick_inc(uint32_t tick_period);

//! @endcond

/**
 * Get the elapsed milliseconds since start up
 * @return the elapsed milliseconds
 */
uint32_t edgui_tick_get(void);

/**
 * Get the elapsed milliseconds since a previous time stamp
 * @param prev_tick a previous time stamp (return value of systick_get() )
 * @return the elapsed milliseconds since 'prev_tick'
 */
uint32_t edgui_tick_elaps(uint32_t prev_tick);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_HAL_TICK_H*/
