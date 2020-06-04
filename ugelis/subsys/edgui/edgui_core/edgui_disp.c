/**
 * @file edgui_disp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_disp.h"
#include "../edgui_misc/edgui_math.h"

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
 * Return with a pointer to the active screen
 * @param disp pointer to display which active screen should be get. (NULL to use the default
 * screen)
 * @return pointer to the active screen object (loaded by 'edgui_scr_load()')
 */
edgui_obj_t * edgui_disp_get_scr_act(edgui_disp_t * disp)
{
    if(!disp) disp = edgui_disp_get_default();
    if(!disp) {
        EDGUI_LOG_WARN("edgui_scr_act: no display registered to get its top layer");
        return NULL;
    }

    return disp->act_scr;
}

/**
 * Make a screen active
 * @param scr pointer to a screen
 */
void edgui_disp_load_scr(edgui_obj_t * scr)
{
    edgui_disp_t * d = edgui_obj_get_disp(scr);

    d->act_scr = scr;

    edgui_obj_invalidate(scr);
}

/**
 * Return with the top layer. (Same on every screen and it is above the normal screen layer)
 * @param disp pointer to display which top layer should be get. (NULL to use the default screen)
 * @return pointer to the top layer object  (transparent screen sized edgui_obj)
 */
edgui_obj_t * edgui_disp_get_layer_top(edgui_disp_t * disp)
{
    if(!disp) disp = edgui_disp_get_default();
    if(!disp) {
        EDGUI_LOG_WARN("edgui_layer_top: no display registered to get its top layer");
        return NULL;
    }

    return disp->top_layer;
}

/**
 * Return with the sys. layer. (Same on every screen and it is above the normal screen and the top
 * layer)
 * @param disp pointer to display which sys. layer  should be get. (NULL to use the default screen)
 * @return pointer to the sys layer object  (transparent screen sized edgui_obj)
 */
edgui_obj_t * edgui_disp_get_layer_sys(edgui_disp_t * disp)
{
    if(!disp) disp = edgui_disp_get_default();
    if(!disp) {
        EDGUI_LOG_WARN("edgui_layer_sys: no display registered to get its top layer");
        return NULL;
    }

    return disp->sys_layer;
}

/**
 * Assign a screen to a display.
 * @param disp pointer to a display where to assign the screen
 * @param scr pointer to a screen object to assign
 */
void edgui_disp_assign_screen(edgui_disp_t * disp, edgui_obj_t * scr)
{
    if(edgui_obj_get_parent(scr) != NULL) {
        EDGUI_LOG_WARN("edgui_disp_assign_screen: try to assign a non-screen object");
        return;
    }

    edgui_disp_t * old_disp = edgui_obj_get_disp(scr);

    if(old_disp == disp) return;

    edgui_ll_chg_list(&old_disp->scr_ll, &disp->scr_ll, scr, true);
}

/**
 * Get a pointer to the screen refresher task to
 * modify its parameters with `edgui_task_...` functions.
 * @param disp pointer to a display
 * @return pointer to the display refresher task. (NULL on error)
 */
edgui_task_t * edgui_disp_get_refr_task(edgui_disp_t * disp)
{
    if(!disp) disp = edgui_disp_get_default();
    if(!disp) {
        EDGUI_LOG_WARN("edgui_disp_get_refr_task: no display registered");
        return NULL;
    }

    return disp->refr_task;
}

/**
 * Get elapsed time since last user activity on a display (e.g. click)
 * @param disp pointer to an display (NULL to get the overall smallest inactivity)
 * @return elapsed ticks (milliseconds) since the last activity
 */
uint32_t edgui_disp_get_inactive_time(const edgui_disp_t * disp)
{
    if(!disp) disp = edgui_disp_get_default();
    if(!disp) {
        EDGUI_LOG_WARN("edgui_disp_get_inactive_time: no display registered");
        return 0;
    }

    if(disp) return edgui_tick_elaps(disp->last_activity_time);

    edgui_disp_t * d;
    uint32_t t = UINT32_MAX;
    d          = edgui_disp_get_next(NULL);
    while(d) {
        t = EDGUI_MATH_MIN(t, edgui_tick_elaps(d->last_activity_time));
        d = edgui_disp_get_next(d);
    }

    return t;
}

/**
 * Manually trigger an activity on a display
 * @param disp pointer to an display (NULL to use the default display)
 */
void edgui_disp_trig_activity(edgui_disp_t * disp)
{
    if(!disp) disp = edgui_disp_get_default();
    if(!disp) {
        EDGUI_LOG_WARN("edgui_disp_trig_activity: no display registered");
        return;
    }

    disp->last_activity_time = edgui_tick_get();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
