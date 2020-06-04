/**
 * @file edgui_refr.h
 *
 */

#ifndef EDGUI_REFR_H
#define EDGUI_REFR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "edgui_obj.h"
#include <stdbool.h>

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
 * Initialize the screen refresh subsystem
 */
void edgui_refr_init(void);

/**
 * Redraw the invalidated areas now.
 * Normally the redrawing is periodically executed in `edgui_task_handler` but a long blocking process
 * can prevent the call of `edgui_task_handler`. In this case if the the GUI is updated in the process
 * (e.g. progress bar) this function can be called when the screen should be updated.
 * @param disp pointer to display to refresh. NULL to refresh all displays.
 */
void edgui_refr_now(edgui_disp_t * disp);

/**
 * Invalidate an area on display to redraw it
 * @param area_p pointer to area which should be invalidated (NULL: delete the invalidated areas)
 * @param disp pointer to display where the area should be invalidated (NULL can be used if there is
 * only one display)
 */
void edgui_inv_area(edgui_disp_t * disp, const edgui_area_t * area_p);

/**
 * Get the display which is being refreshed
 * @return the display being refreshed
 */
edgui_disp_t * edgui_refr_get_disp_refreshing(void);

/**
 * Set the display which is being refreshed.
 * It shouldn1t be used directly by the user.
 * It can be used to trick the drawing functions about there is an active display.
 * @param the display being refreshed
 */
void edgui_refr_set_disp_refreshing(edgui_disp_t * disp);

/**
 * Called periodically to handle the refreshing
 * @param task pointer to the task itself
 */
void edgui_disp_refr_task(edgui_task_t * task);

/**********************
 *   STATIC FUNCTIONS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_REFR_H*/
