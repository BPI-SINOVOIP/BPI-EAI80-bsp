
/**
 * @file hal_disp.c
 *
 * @description HAL layer for display driver
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stddef.h>
#include "edgui_hal.h"
#include "../edgui_misc/edgui_mem.h"
#include "../edgui_core/edgui_obj.h"
#include "../edgui_core/edgui_refr.h"
#include "../edgui_misc/edgui_gc.h"

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
static edgui_disp_t * disp_def;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize a display driver with default values.
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
void edgui_disp_drv_init(edgui_disp_drv_t * driver)
{
    memset(driver, 0, sizeof(edgui_disp_drv_t));

    driver->flush_cb         = NULL;
    driver->hor_res          = EDGUI_HOR_RES_MAX;
    driver->ver_res          = EDGUI_VER_RES_MAX;
    driver->buffer           = NULL;
    driver->rotated          = 0;
    driver->color_chroma_key = EDGUI_COLOR_TRANSP;

#if EDGUI_ANTIALIAS
    driver->antialiasing = true;
#endif

#if EDGUI_COLOR_SCREEN_TRANSP
    driver->screen_transp = 1;
#endif

#if EDGUI_USE_GPU
    driver->gpu_blend_cb = NULL;
    driver->gpu_fill_cb  = NULL;
#endif

#if EDGUI_USE_USER_DATA
    driver->user_data = NULL;
#endif

    driver->set_px_cb = NULL;
}

/**
 * Initialize a display buffer
 * @param disp_buf pointer `edgui_disp_buf_t` variable to initialize
 * @param buf1 A buffer to be used by edgui to draw the image.
 *             Always has to specified and can't be NULL.
 *             Can be an array allocated by the user. E.g. `static edgui_color_t disp_buf1[1024 * 10]`
 *             Or a memory address e.g. in external SRAM
 * @param buf2 Optionally specify a second buffer to make image rendering and image flushing
 *             (sending to the display) parallel.
 *             In the `disp_drv->flush` you should use DMA or similar hardware to send
 *             the image to the display in the background.
 *             It lets edgui to render next frame into the other buffer while previous is being
 * sent. Set to `NULL` if unused.
 * @param size_in_px_cnt size of the `buf1` and `buf2` in pixel count.
 */
void edgui_disp_buf_init(edgui_disp_buf_t * disp_buf, void * buf1, void * buf2, uint32_t size_in_px_cnt)
{
    memset(disp_buf, 0, sizeof(edgui_disp_buf_t));

    disp_buf->buf1    = buf1;
    disp_buf->buf2    = buf2;
    disp_buf->buf_act = disp_buf->buf1;
    disp_buf->size    = size_in_px_cnt;
}

/**
 * Register an initialized display driver.
 * Automatically set the first display as active.
 * @param driver pointer to an initialized 'edgui_disp_drv_t' variable (can be local variable)
 * @return pointer to the new display or NULL on error
 */
edgui_disp_t * edgui_disp_drv_register(edgui_disp_drv_t * driver)
{
    edgui_disp_t * disp = edgui_ll_ins_head(&EDGUI_GC_ROOT(_edgui_disp_ll));
    if(!disp) {
        edgui_mem_assert(disp);
        return NULL;
    }

    memcpy(&disp->driver, driver, sizeof(edgui_disp_drv_t));
    memset(&disp->inv_area_joined, 0, sizeof(disp->inv_area_joined));
    memset(&disp->inv_areas, 0, sizeof(disp->inv_areas));
    edgui_ll_init(&disp->scr_ll, sizeof(edgui_obj_t));

    if(disp_def == NULL) disp_def = disp;

    edgui_disp_t * disp_def_tmp = disp_def;
    disp_def                 = disp; /*Temporarily change the default screen to create the default screens on the
                                        new display*/

    disp->inv_p = 0;

    disp->act_scr   = edgui_obj_create(NULL, NULL); /*Create a default screen on the display*/
    disp->top_layer = edgui_obj_create(NULL, NULL); /*Create top layer on the display*/
    disp->sys_layer = edgui_obj_create(NULL, NULL); /*Create sys layer on the display*/
    edgui_obj_set_style(disp->top_layer, &edgui_style_transp);
    edgui_obj_set_style(disp->sys_layer, &edgui_style_transp);

    edgui_obj_invalidate(disp->act_scr);

    disp_def = disp_def_tmp; /*Revert the default display*/

    /*Create a refresh task*/
    disp->refr_task = edgui_task_create(edgui_disp_refr_task, EDGUI_DISP_DEF_REFR_PERIOD, EDGUI_TASK_PRIO_MID, disp);
    edgui_mem_assert(disp->refr_task);
    if(disp->refr_task == NULL) return NULL;

    edgui_task_ready(disp->refr_task); /*Be sure the screen will be refreshed immediately on start up*/

    return disp;
}

/**
 * Update the driver in run time.
 * @param disp pointer to a display. (return value of `edgui_disp_drv_register`)
 * @param new_drv pointer to the new driver
 */
void edgui_disp_drv_update(edgui_disp_t * disp, edgui_disp_drv_t * new_drv)
{
    memcpy(&disp->driver, new_drv, sizeof(edgui_disp_drv_t));

    edgui_obj_t * scr;
    EDGUI_LL_READ(disp->scr_ll, scr)
    {
        edgui_obj_set_size(scr, edgui_disp_get_hor_res(disp), edgui_disp_get_ver_res(disp));
    }
}

/**
 * Remove a display
 * @param disp pointer to display
 */
void edgui_disp_remove(edgui_disp_t * disp)
{
    bool was_default = false;
    if(disp == edgui_disp_get_default()) was_default = true;

    /*Detach the input devices */
    edgui_indev_t * indev;
    indev = edgui_indev_get_next(NULL);
    while(indev) {
        if(indev->driver.disp == disp) {
            indev->driver.disp = NULL;
        }
        indev = edgui_indev_get_next(indev);
    }

    edgui_ll_rem(&EDGUI_GC_ROOT(_edgui_disp_ll), disp);
    edgui_mem_free(disp);

    if(was_default) edgui_disp_set_default(edgui_ll_get_head(&EDGUI_GC_ROOT(_edgui_disp_ll)));
}

/**
 * Set a default screen. The new screens will be created on it by default.
 * @param disp pointer to a display
 */
void edgui_disp_set_default(edgui_disp_t * disp)
{
    disp_def = disp;
}

/**
 * Get the default display
 * @return pointer to the default display
 */
edgui_disp_t * edgui_disp_get_default(void)
{
    return disp_def;
}

/**
 * Get the horizontal resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the horizontal resolution of the display
 */
edgui_coord_t edgui_disp_get_hor_res(edgui_disp_t * disp)
{
    if(disp == NULL) disp = edgui_disp_get_default();

    if(disp == NULL)
        return EDGUI_HOR_RES_MAX;
    else
        return disp->driver.rotated == 0 ? disp->driver.hor_res : disp->driver.ver_res;
}

/**
 * Get the vertical resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the vertical resolution of the display
 */
edgui_coord_t edgui_disp_get_ver_res(edgui_disp_t * disp)
{
    if(disp == NULL) disp = edgui_disp_get_default();

    if(disp == NULL)
        return EDGUI_VER_RES_MAX;
    else
        return disp->driver.rotated == 0 ? disp->driver.ver_res : disp->driver.hor_res;
}

/**
 * Get if anti-aliasing is enabled for a display or not
 * @param disp pointer to a display (NULL to use the default display)
 * @return true: anti-aliasing is enabled; false: disabled
 */
bool edgui_disp_get_antialiasing(edgui_disp_t * disp)
{
#if EDGUI_ANTIALIAS == 0
    return false;
#else
    if(disp == NULL) disp = edgui_disp_get_default();
    if(disp == NULL) return false;

    return disp->driver.antialiasing ? true : false;
#endif
}

/**
 * Call in the display driver's `flush_cb` function when the flushing is finished
 * @param disp_drv pointer to display driver in `flush_cb` where this function is called
 */
EDGUI_ATTRIBUTE_FLUSH_READY void edgui_disp_flush_ready(edgui_disp_drv_t * disp_drv)
{
    disp_drv->buffer->flushing = 0;

    /*If the screen is transparent initialize it when the flushing is ready*/
#if EDGUI_COLOR_SCREEN_TRANSP
    if(disp_drv->screen_transp) {
        memset(disp_drv->buffer->buf_act, 0x00, disp_drv->buffer->size * sizeof(edgui_color32_t));
    }
#endif
}

/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
edgui_disp_t * edgui_disp_get_next(edgui_disp_t * disp)
{
    if(disp == NULL)
        return edgui_ll_get_head(&EDGUI_GC_ROOT(_edgui_disp_ll));
    else
        return edgui_ll_get_next(&EDGUI_GC_ROOT(_edgui_disp_ll), disp);
}

/**
 * Get the internal buffer of a display
 * @param disp pointer to a display
 * @return pointer to the internal buffers
 */
edgui_disp_buf_t * edgui_disp_get_buf(edgui_disp_t * disp)
{
    return disp->driver.buffer;
}

/**
 * Get the number of areas in the buffer
 * @return number of invalid areas
 */
uint16_t edgui_disp_get_inv_buf_size(edgui_disp_t * disp)
{
    return disp->inv_p;
}

/**
 * Pop (delete) the last 'num' invalidated areas from the buffer
 * @param num number of areas to delete
 */
void edgui_disp_pop_from_inv_buf(edgui_disp_t * disp, uint16_t num)
{

    if(disp->inv_p < num)
        disp->inv_p = 0;
    else
        disp->inv_p -= num;
}

/**
 * Check the driver configuration if it's double buffered (both `buf1` and `buf2` are set)
 * @param disp pointer to to display to check
 * @return true: double buffered; false: not double buffered
 */
bool edgui_disp_is_double_buf(edgui_disp_t * disp)
{
    if(disp->driver.buffer->buf1 && disp->driver.buffer->buf2)
        return true;
    else
        return false;
}

/**
 * Check the driver configuration if it's TRUE double buffered (both `buf1` and `buf2` are set and
 * `size` is screen sized)
 * @param disp pointer to to display to check
 * @return true: double buffered; false: not double buffered
 */
bool edgui_disp_is_true_double_buf(edgui_disp_t * disp)
{
    uint32_t scr_size = disp->driver.hor_res * disp->driver.ver_res;

    if(edgui_disp_is_double_buf(disp) && disp->driver.buffer->size == scr_size) {
        return true;
    } else {
        return false;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
