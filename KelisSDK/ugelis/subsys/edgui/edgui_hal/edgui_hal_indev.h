/**
 * @file edgui_hal_indev.h
 *
 * @description Input Device HAL interface layer header file
 *
 */

#ifndef EDGUI_HAL_INDEV_H
#define EDGUI_HAL_INDEV_H

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

#include <stdbool.h>
#include <stdint.h>
#include "../edgui_misc/edgui_area.h"
#include "../edgui_misc/edgui_task.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _edgui_obj_t;
struct _disp_t;
struct _edgui_indev_t;
struct _edgui_indev_drv_t;

/** Possible input device types*/
enum {
    EDGUI_INDEV_TYPE_NONE,    /**< Uninitialized state*/
    EDGUI_INDEV_TYPE_POINTER, /**< Touch pad, mouse, external button*/
    EDGUI_INDEV_TYPE_KEYPAD,  /**< Keypad or keyboard*/
    EDGUI_INDEV_TYPE_BUTTON,  /**< External (hardware button) which is assigned to a specific point of the
                              screen*/
    EDGUI_INDEV_TYPE_ENCODER, /**< Encoder with only Left, Right turn and a Button*/
};
typedef uint8_t edgui_indev_type_t;

/** States for input devices*/
enum { EDGUI_INDEV_STATE_REL = 0, EDGUI_INDEV_STATE_PR };
typedef uint8_t edgui_indev_state_t;

/** Data structure passed to an input driver to fill */
typedef struct
{
    edgui_point_t point; /**< For EDGUI_INDEV_TYPE_POINTER the currently pressed point*/
    uint32_t key;     /**< For EDGUI_INDEV_TYPE_KEYPAD the currently pressed key*/
    uint32_t btn_id;  /**< For EDGUI_INDEV_TYPE_BUTTON the currently pressed button*/
    int16_t enc_diff; /**< For EDGUI_INDEV_TYPE_ENCODER number of steps since the previous read*/

    edgui_indev_state_t state; /**< EDGUI_INDEV_STATE_REL or EDGUI_INDEV_STATE_PR*/
} edgui_indev_data_t;

/** Initialized by the user and registered by 'edgui_indev_add()'*/
typedef struct _edgui_indev_drv_t
{

    /**< Input device type*/
    edgui_indev_type_t type;

    /**< Function pointer to read input device data.
     * Return 'true' if there is more data to be read (buffered).
     * Most drivers can safely return 'false' */
    bool (*read_cb)(struct _edgui_indev_drv_t * indev_drv, edgui_indev_data_t * data);

    /** Called when an action happened on the input device.
     * The second parameter is the event from `edgui_event_t`*/
    void (*feedback_cb)(struct _edgui_indev_drv_t *, uint8_t);

#if EDGUI_USE_USER_DATA
    edgui_indev_drv_user_data_t user_data;
#endif

    /**< Pointer to the assigned display*/
    struct _disp_t * disp;

    /**< Task to read the periodically read the input device*/
    edgui_task_t * read_task;

    /**< Number of pixels to slide before actually drag the object*/
    uint8_t drag_limit;

    /**< Drag throw slow-down in [%]. Greater value means faster slow-down */
    uint8_t drag_throw;

    /**< Long press time in milliseconds*/
    uint16_t long_press_time;

    /**< Repeated trigger period in long press [ms] */
    uint16_t long_press_rep_time;
} edgui_indev_drv_t;

/** Run time data of input devices
 * Internally used by the library, you should not need to touch it.
 */
typedef struct _edgui_indev_proc_t
{
    edgui_indev_state_t state; /**< Current state of the input device. */
    union
    {
        struct
        { /*Pointer and button data*/
            edgui_point_t act_point; /**< Current point of input device. */
            edgui_point_t last_point; /**< Last point of input device. */
            edgui_point_t vect; /**< Difference between `act_point` and `last_point`. */
            edgui_point_t drag_sum; /*Count the dragged pixels to check EDGUI_INDEV_DEF_DRAG_LIMIT*/
            edgui_point_t drag_throw_vect;
            struct _edgui_obj_t * act_obj;      /*The object being pressed*/
            struct _edgui_obj_t * last_obj;     /*The last obejct which was pressed (used by dragthrow and
                                                other post-release event)*/
            struct _edgui_obj_t * last_pressed; /*The lastly pressed object*/

            /*Flags*/
            uint8_t drag_limit_out : 1;
            uint8_t drag_in_prog : 1;
        } pointer;
        struct
        { /*Keypad data*/
            edgui_indev_state_t last_state;
            uint32_t last_key;
        } keypad;
    } types;

    uint32_t pr_timestamp;         /**< Pressed time stamp*/
    uint32_t longpr_rep_timestamp; /**< Long press repeat time stamp*/

    /*Flags*/
    uint8_t long_pr_sent : 1;
    uint8_t reset_query : 1;
    uint8_t disabled : 1;
    uint8_t wait_until_release : 1;
} edgui_indev_proc_t;

struct _edgui_obj_t;
struct _edgui_group_t;

/** The main input device descriptor with driver, runtime data ('proc') and some additional
 * information*/
typedef struct _edgui_indev_t
{
    edgui_indev_drv_t driver;
    edgui_indev_proc_t proc;
    struct _edgui_obj_t * cursor;     /**< Cursor for EDGUI_INPUT_TYPE_POINTER*/
    struct _edgui_group_t * group;    /**< Keypad destination group*/
    const edgui_point_t * btn_points; /**< Array points assigned to the button ()screen will be pressed
                                      here by the buttons*/
} edgui_indev_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize an input device driver with default values.
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
void edgui_indev_drv_init(edgui_indev_drv_t * driver);

/**
 * Register an initialized input device driver.
 * @param driver pointer to an initialized 'edgui_indev_drv_t' variable (can be local variable)
 * @return pointer to the new input device or NULL on error
 */
edgui_indev_t * edgui_indev_drv_register(edgui_indev_drv_t * driver);

/**
 * Update the driver in run time.
 * @param indev pointer to a input device. (return value of `edgui_indev_drv_register`)
 * @param new_drv pointer to the new driver
 */
void edgui_indev_drv_update(edgui_indev_t * indev, edgui_indev_drv_t * new_drv);

/**
 * Get the next input device.
 * @param indev pointer to the current input device. NULL to initialize.
 * @return the next input devise or NULL if no more. Give the first input device when the parameter
 * is NULL
 */
edgui_indev_t * edgui_indev_get_next(edgui_indev_t * indev);

/**
 * Read data from an input device.
 * @param indev pointer to an input device
 * @param data input device will write its data here
 * @return false: no more data; true: there more data to read (buffered)
 */
bool edgui_indev_read(edgui_indev_t * indev, edgui_indev_data_t * data);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
