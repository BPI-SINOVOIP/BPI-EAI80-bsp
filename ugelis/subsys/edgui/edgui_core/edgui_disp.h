/**
 * @file edgui_disp.h
 *
 */

#ifndef EDGUI_DISP_H
#define EDGUI_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../edgui_hal/edgui_hal.h"
#include "edgui_obj.h"

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
 * Return with a pointer to the active screen
 * @param disp pointer to display which active screen should be get. (NULL to use the default
 * screen)
 * @return pointer to the active screen object (loaded by 'edgui_scr_load()')
 */
edgui_obj_t * edgui_disp_get_scr_act(edgui_disp_t * disp);

/**
 * Make a screen active
 * @param scr pointer to a screen
 */
void edgui_disp_load_scr(edgui_obj_t * scr);

/**
 * Return with the top layer. (Same on every screen and it is above the normal screen layer)
 * @param disp pointer to display which top layer should be get. (NULL to use the default screen)
 * @return pointer to the top layer object  (transparent screen sized edgui_obj)
 */
edgui_obj_t * edgui_disp_get_layer_top(edgui_disp_t * disp);

/**
 * Return with the sys. layer. (Same on every screen and it is above the normal screen and the top
 * layer)
 * @param disp pointer to display which sys. layer  should be get. (NULL to use the default screen)
 * @return pointer to the sys layer object  (transparent screen sized edgui_obj)
 */
edgui_obj_t * edgui_disp_get_layer_sys(edgui_disp_t * disp);

/**
 * Assign a screen to a display.
 * @param disp pointer to a display where to assign the screen
 * @param scr pointer to a screen object to assign
 */
void edgui_disp_assign_screen(edgui_disp_t * disp, edgui_obj_t * scr);

/**
 * Get a pointer to the screen refresher task to
 * modify its parameters with `edgui_task_...` functions.
 * @param disp pointer to a display
 * @return pointer to the display refresher task. (NULL on error)
 */
edgui_task_t * edgui_disp_get_refr_task(edgui_disp_t * disp);

/**
 * Get elapsed time since last user activity on a display (e.g. click)
 * @param disp pointer to an display (NULL to get the overall smallest inactivity)
 * @return elapsed ticks (milliseconds) since the last activity
 */
uint32_t edgui_disp_get_inactive_time(const edgui_disp_t * disp);

/**
 * Manually trigger an activity on a display
 * @param disp pointer to an display (NULL to use the default display)
 */
void edgui_disp_trig_activity(edgui_disp_t * disp);

/*------------------------------------------------
 * To improve backward compatibility
 * Recommended only if you have one display
 *------------------------------------------------*/

/**
 * Get the active screen of the default display
 * @return pointer to the active screen
 */
static __inline edgui_obj_t * edgui_scr_act(void)
{
    return edgui_disp_get_scr_act(edgui_disp_get_default());
}

/**
 * Get the top layer  of the default display
 * @return pointer to the top layer
 */
static __inline edgui_obj_t * edgui_layer_top(void)
{
    return edgui_disp_get_layer_top(edgui_disp_get_default());
}

/**
 * Get the active screen of the deafult display
 * @return  pointer to the sys layer
 */
static __inline edgui_obj_t * edgui_layer_sys(void)
{
    return edgui_disp_get_layer_sys(edgui_disp_get_default());
}

static __inline void edgui_scr_load(edgui_obj_t * scr)
{
    edgui_disp_load_scr(scr);
}

/**********************
 *      MACROS
 **********************/

/*------------------------------------------------
 * To improve backward compatibility
 * Recommended only if you have one display
 *------------------------------------------------*/

#ifndef EDGUI_HOR_RES
/**
 * The horizontal resolution of the currently active display.
 */
#define EDGUI_HOR_RES edgui_disp_get_hor_res(edgui_disp_get_default())
#endif

#ifndef EDGUI_VER_RES
/**
 * The vertical resolution of the currently active display.
 */
#define EDGUI_VER_RES edgui_disp_get_ver_res(edgui_disp_get_default())
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEMPL_H*/
