/**
 * @file systick.c
 * Provide access to the system tick with 1 millisecond resolution
 */

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui_conf.h"
#else
#include "../edgui/edgui_conf.h"
#endif

#include "edgui_hal_tick.h"
#include <stddef.h>

#if EDGUI_TICK_CUSTOM == 1
#include EDGUI_TICK_CUSTOM_INCLUDE
#endif

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
static uint32_t sys_time = 0;
static volatile uint8_t tick_irq_flag;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * You have to call this function periodically
 * @param tick_period the call period of this function in milliseconds
 */
EDGUI_ATTRIBUTE_TICK_INC void edgui_tick_inc(uint32_t tick_period)
{
    tick_irq_flag = 0;
    sys_time += tick_period;
}

/**
 * Get the elapsed milliseconds since start up
 * @return the elapsed milliseconds
 */
uint32_t edgui_tick_get(void)
{
#if EDGUI_TICK_CUSTOM == 0
    uint32_t result;
    do {
        tick_irq_flag = 1;
        result        = sys_time;
    } while(!tick_irq_flag); /*'edgui_tick_inc()' clears this flag which can be in an interrupt.
                                Continue until make a non interrupted cycle */

    return result;
#else
    return EDGUI_TICK_CUSTOM_SYS_TIME_EXPR;
#endif
}

/**
 * Get the elapsed milliseconds since a previous time stamp
 * @param prev_tick a previous time stamp (return value of systick_get() )
 * @return the elapsed milliseconds since 'prev_tick'
 */
uint32_t edgui_tick_elaps(uint32_t prev_tick)
{
    uint32_t act_time = edgui_tick_get();

    /*If there is no overflow in sys_time simple subtract*/
    if(act_time >= prev_tick) {
        prev_tick = act_time - prev_tick;
    } else {
        prev_tick = UINT32_MAX - prev_tick + 1;
        prev_tick += act_time;
    }

    return prev_tick;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
