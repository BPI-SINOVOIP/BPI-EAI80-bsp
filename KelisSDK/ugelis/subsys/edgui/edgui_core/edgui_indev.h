/**
 * @file edgui_indev.h
 *
 */

#ifndef EDGUI_INDEV_H
#define EDGUI_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "edgui_obj.h"
#include "../edgui_hal/edgui_hal_indev.h"
#include "../edgui_core/edgui_group.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the display input device subsystem
 */
void edgui_indev_init(void);

/**
 * Called periodically to read the input devices
 * @param task pointer to the task itself
 */
void edgui_indev_read_task(edgui_task_t * task);

/**
 * Get the currently processed input device. Can be used in action functions too.
 * @return pointer to the currently processed input device or NULL if no input device processing
 * right now
 */
edgui_indev_t * edgui_indev_get_act(void);

/**
 * Get the type of an input device
 * @param indev pointer to an input device
 * @return the type of the input device from `edgui_hal_indev_type_t` (`EDGUI_INDEV_TYPE_...`)
 */
edgui_indev_type_t edgui_indev_get_type(const edgui_indev_t * indev);

/**
 * Reset one or all input devices
 * @param indev pointer to an input device to reset or NULL to reset all of them
 */
void edgui_indev_reset(edgui_indev_t * indev);

/**
 * Reset the long press state of an input device
 * @param indev_proc pointer to an input device
 */
void edgui_indev_reset_long_press(edgui_indev_t * indev);

/**
 * Enable or disable an input devices
 * @param indev pointer to an input device
 * @param en true: enable; false: disable
 */
void edgui_indev_enable(edgui_indev_t * indev, bool en);

/**
 * Set a cursor for a pointer input device (for EDGUI_INPUT_TYPE_POINTER and EDGUI_INPUT_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @param cur_obj pointer to an object to be used as cursor
 */
void edgui_indev_set_cursor(edgui_indev_t * indev, edgui_obj_t * cur_obj);

#if EDGUI_USE_GROUP
/**
 * Set a destination group for a keypad input device (for EDGUI_INDEV_TYPE_KEYPAD)
 * @param indev pointer to an input device
 * @param group point to a group
 */
void edgui_indev_set_group(edgui_indev_t * indev, edgui_group_t * group);
#endif

/**
 * Set the an array of points for EDGUI_INDEV_TYPE_BUTTON.
 * These points will be assigned to the buttons to press a specific point on the screen
 * @param indev pointer to an input device
 * @param group point to a group
 */
void edgui_indev_set_button_points(edgui_indev_t * indev, const edgui_point_t * points);

/**
 * Get the last point of an input device (for EDGUI_INDEV_TYPE_POINTER and EDGUI_INDEV_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @param point pointer to a point to store the result
 */
void edgui_indev_get_point(const edgui_indev_t * indev, edgui_point_t * point);

/**
 * Get the last pressed key of an input device (for EDGUI_INDEV_TYPE_KEYPAD)
 * @param indev pointer to an input device
 * @return the last pressed key (0 on error)
 */
uint32_t edgui_indev_get_key(const edgui_indev_t * indev);

/**
 * Check if there is dragging with an input device or not (for EDGUI_INDEV_TYPE_POINTER and
 * EDGUI_INDEV_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @return true: drag is in progress
 */
bool edgui_indev_is_dragging(const edgui_indev_t * indev);

/**
 * Get the vector of dragging of an input device (for EDGUI_INDEV_TYPE_POINTER and
 * EDGUI_INDEV_TYPE_BUTTON)
 * @param indev pointer to an input device
 * @param point pointer to a point to store the vector
 */
void edgui_indev_get_vect(const edgui_indev_t * indev, edgui_point_t * point);

/**
 * Do nothing until the next release
 * @param indev pointer to an input device
 */
void edgui_indev_wait_release(edgui_indev_t * indev);

/**
 * Get a pointer to the indev read task to
 * modify its parameters with `edgui_task_...` functions.
 * @param indev pointer to an inout device
 * @return pointer to the indev read refresher task. (NULL on error)
 */
edgui_task_t * edgui_indev_get_read_task(edgui_disp_t * indev);

/**
 * Gets a pointer to the currently active object in indev proc functions.
 * NULL if no object is currently being handled or if groups aren't used.
 * @return pointer to currently active object
 */
edgui_obj_t * edgui_indev_get_obj_act(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_INDEV_H*/
