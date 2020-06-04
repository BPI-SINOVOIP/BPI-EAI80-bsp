/**
 * @file hal_indev.c
 *
 * @description Input device HAL interface
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../edgui_hal/edgui_hal_indev.h"
#include "../edgui_core/edgui_indev.h"
#include "../edgui_misc/edgui_mem.h"
#include "../edgui_misc/edgui_gc.h"
#include "edgui_hal_disp.h"

#if defined(EDGUI_GC_INCLUDE)
#include EDGUI_GC_INCLUDE
#endif /* EDGUI_ENABLE_GC */

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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize an input device driver with default values.
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
void edgui_indev_drv_init(edgui_indev_drv_t * driver)
{
    memset(driver, 0, sizeof(edgui_indev_drv_t));

    driver->type                = EDGUI_INDEV_TYPE_NONE;
    driver->drag_limit          = EDGUI_INDEV_DEF_DRAG_LIMIT;
    driver->drag_throw          = EDGUI_INDEV_DEF_DRAG_THROW;
    driver->long_press_time     = EDGUI_INDEV_DEF_LONG_PRESS_TIME;
    driver->long_press_rep_time = EDGUI_INDEV_DEF_LONG_PRESS_REP_TIME;
}

/**
 * Register an initialized input device driver.
 * @param driver pointer to an initialized 'edgui_indev_drv_t' variable (can be local variable)
 * @return pointer to the new input device or NULL on error
 */
edgui_indev_t * edgui_indev_drv_register(edgui_indev_drv_t * driver)
{

    if(driver->disp == NULL) driver->disp = edgui_disp_get_default();

    if(driver->disp == NULL) {
        EDGUI_LOG_WARN("edgui_indev_drv_register: no display registered hence can't attache the indev to "
                    "a display");
        return NULL;
    }

    edgui_indev_t * indev = edgui_ll_ins_head(&EDGUI_GC_ROOT(_edgui_indev_ll));
    if(!indev) {
        edgui_mem_assert(indev);
        return NULL;
    }

    memset(indev, 0, sizeof(edgui_indev_t));
    memcpy(&indev->driver, driver, sizeof(edgui_indev_drv_t));

    indev->proc.reset_query = 1;
    indev->cursor           = NULL;
    indev->group            = NULL;
    indev->btn_points       = NULL;

    indev->driver.read_task = edgui_task_create(edgui_indev_read_task, EDGUI_INDEV_DEF_READ_PERIOD, EDGUI_TASK_PRIO_MID, indev);

    return indev;
}

/**
 * Update the driver in run time.
 * @param indev pointer to a input device. (return value of `edgui_indev_drv_register`)
 * @param new_drv pointer to the new driver
 */
void edgui_indev_drv_update(edgui_indev_t * indev, edgui_indev_drv_t * new_drv)
{
    memcpy(&indev->driver, new_drv, sizeof(edgui_indev_drv_t));
}

/**
 * Get the next input device.
 * @param indev pointer to the current input device. NULL to initialize.
 * @return the next input devise or NULL if no more. Give the first input device when the parameter
 * is NULL
 */
edgui_indev_t * edgui_indev_get_next(edgui_indev_t * indev)
{
    if(indev == NULL)
        return edgui_ll_get_head(&EDGUI_GC_ROOT(_edgui_indev_ll));
    else
        return edgui_ll_get_next(&EDGUI_GC_ROOT(_edgui_indev_ll), indev);
}

/**
 * Read data from an input device.
 * @param indev pointer to an input device
 * @param data input device will write its data here
 * @return false: no more data; true: there more data to read (buffered)
 */
bool edgui_indev_read(edgui_indev_t * indev, edgui_indev_data_t * data)
{
    bool cont = false;

    memset(data, 0, sizeof(edgui_indev_data_t));

    /* For touchpad sometimes users don't the last pressed coordinate on release.
     * So be sure a coordinates are initialized to the last point */
    if(indev->driver.type == EDGUI_INDEV_TYPE_POINTER) {
        data->point.x = indev->proc.types.pointer.act_point.x;
        data->point.y = indev->proc.types.pointer.act_point.y;
    }
    /*Similarly set at least the last key in case of the  the user doesn't set it  on release*/
    else if(indev->driver.type == EDGUI_INDEV_TYPE_KEYPAD) {
        data->key = indev->proc.types.keypad.last_key;
    }

    if(indev->driver.read_cb) {
        EDGUI_LOG_TRACE("idnev read started");
        cont = indev->driver.read_cb(&indev->driver, data);
        EDGUI_LOG_TRACE("idnev read finished");
    } else {
        EDGUI_LOG_WARN("indev function registered");
    }

    return cont;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
