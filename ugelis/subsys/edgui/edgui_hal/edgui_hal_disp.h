/**
 * @file edgui_hal_disp.h
 *
 * @description Display Driver HAL interface header file
 *
 */

#ifndef EDGUI_HAL_DISP_H
#define EDGUI_HAL_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include "edgui_hal.h"
#include "../edgui_misc/edgui_color.h"
#include "../edgui_misc/edgui_area.h"
#include "../edgui_misc/edgui_ll.h"
#include "../edgui_misc/edgui_task.h"

/*********************
 *      DEFINES
 *********************/
#ifndef EDGUI_INV_BUF_SIZE
#define EDGUI_INV_BUF_SIZE 32 /*Buffer size for invalid areas */
#endif

#ifndef EDGUI_ATTRIBUTE_FLUSH_READY
#define EDGUI_ATTRIBUTE_FLUSH_READY
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _disp_t;
struct _disp_drv_t;

/**
 * Structure for holding display buffer information.
 */
typedef struct
{
    void * buf1; /**< First display buffer. */
    void * buf2; /**< Second display buffer. */

    /*Internal, used by the library*/
    void * buf_act;
    uint32_t size; /*In pixel count*/
    edgui_area_t area;
    volatile uint32_t flushing : 1;
} edgui_disp_buf_t;

/**
 * Display Driver structure to be registered by HAL
 */
typedef struct _disp_drv_t
{

    edgui_coord_t hor_res; /**< Horizontal resolution. */
    edgui_coord_t ver_res; /**< Vertical resolution. */

    /** Pointer to a buffer initialized with `edgui_disp_buf_init()`.
     * edgui will use this buffer(s) to draw the screens contents */
    edgui_disp_buf_t * buffer;

#if EDGUI_ANTIALIAS
    uint32_t antialiasing : 1; /**< 1: antialiasing is enabled on this display. */
#endif
    uint32_t rotated : 1; /**< 1: turn the display by 90 degree. @warning Does not update coordinates for you!*/

#if EDGUI_COLOR_SCREEN_TRANSP
    /**Handle if the the screen doesn't have a solid (opa == EDGUI_OPA_COVER) background.
     * Use only if required because it's slower.*/
    uint32_t screen_transp : 1;
#endif

    /** MANDATORY: Write the internal buffer (VDB) to the display. 'edgui_disp_flush_ready()' has to be
     * called when finished */
    void (*flush_cb)(struct _disp_drv_t * disp_drv, const edgui_area_t * area, edgui_color_t * color_p);

    /** OPTIONAL: Extend the invalidated areas to match with the display drivers requirements
     * E.g. round `y` to, 8, 16 ..) on a monochrome display*/
    void (*rounder_cb)(struct _disp_drv_t * disp_drv, edgui_area_t * area);

    /** OPTIONAL: Set a pixel in a buffer according to the special requirements of the display
     * Can be used for color format not supported in LittelvGL. E.g. 2 bit -> 4 gray scales
     * @note Much slower then drawing with supported color formats. */
    void (*set_px_cb)(struct _disp_drv_t * disp_drv, uint8_t * buf, edgui_coord_t buf_w, edgui_coord_t x, edgui_coord_t y,
                      edgui_color_t color, edgui_opa_t opa);

    /** OPTIONAL: Called after every refresh cycle to tell the rendering and flushing time + the
     * number of flushed pixels */
    void (*monitor_cb)(struct _disp_drv_t * disp_drv, uint32_t time, uint32_t px);

#if EDGUI_USE_GPU
    /** OPTIONAL: Blend two memories using opacity (GPU only)*/
    void (*gpu_blend_cb)(struct _disp_drv_t * disp_drv, edgui_color_t * dest, const edgui_color_t * src, uint32_t length,
                         edgui_opa_t opa);

    /** OPTIONAL: Fill a memory with a color (GPU only)*/
    void (*gpu_fill_cb)(struct _disp_drv_t * disp_drv, edgui_color_t * dest_buf, edgui_coord_t dest_width,
                        const edgui_area_t * fill_area, edgui_color_t color);
#endif

    /** On CHROMA_KEYED images this color will be transparent.
     * `EDGUI_COLOR_TRANSP` by default. (edgui_conf.h)*/
    edgui_color_t color_chroma_key;

#if EDGUI_USE_USER_DATA
    edgui_disp_drv_user_data_t user_data; /**< Custom display driver user data */
#endif

} edgui_disp_drv_t;

struct _edgui_obj_t;

/**
 * Display structure.
 * ::edgui_disp_drv_t is the first member of the structure.
 */
typedef struct _disp_t
{
    /**< Driver to the display*/
    edgui_disp_drv_t driver;

    /**< A task which periodically checks the dirty areas and refreshes them*/
    edgui_task_t * refr_task;

    /** Screens of the display*/
    edgui_ll_t scr_ll;
    struct _edgui_obj_t * act_scr; /**< Currently active screen on this display */
    struct _edgui_obj_t * top_layer; /**< @see edgui_disp_get_layer_top */
    struct _edgui_obj_t * sys_layer; /**< @see edgui_disp_get_layer_sys */

    /** Invalidated (marked to redraw) areas*/
    edgui_area_t inv_areas[EDGUI_INV_BUF_SIZE];
    uint8_t inv_area_joined[EDGUI_INV_BUF_SIZE];
    uint32_t inv_p : 10;

    /*Miscellaneous data*/
    uint32_t last_activity_time; /**< Last time there was activity on this display */
} edgui_disp_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a display driver with default values.
 * It is used to have known values in the fields and not junk in memory.
 * After it you can safely set only the fields you need.
 * @param driver pointer to driver variable to initialize
 */
void edgui_disp_drv_init(edgui_disp_drv_t * driver);

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
void edgui_disp_buf_init(edgui_disp_buf_t * disp_buf, void * buf1, void * buf2, uint32_t size_in_px_cnt);

/**
 * Register an initialized display driver.
 * Automatically set the first display as active.
 * @param driver pointer to an initialized 'edgui_disp_drv_t' variable (can be local variable)
 * @return pointer to the new display or NULL on error
 */
edgui_disp_t * edgui_disp_drv_register(edgui_disp_drv_t * driver);

/**
 * Update the driver in run time.
 * @param disp pointer to a display. (return value of `edgui_disp_drv_register`)
 * @param new_drv pointer to the new driver
 */
void edgui_disp_drv_update(edgui_disp_t * disp, edgui_disp_drv_t * new_drv);

/**
 * Remove a display
 * @param disp pointer to display
 */
void edgui_disp_remove(edgui_disp_t * disp);

/**
 * Set a default screen. The new screens will be created on it by default.
 * @param disp pointer to a display
 */
void edgui_disp_set_default(edgui_disp_t * disp);

/**
 * Get the default display
 * @return pointer to the default display
 */
edgui_disp_t * edgui_disp_get_default(void);

/**
 * Get the horizontal resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the horizontal resolution of the display
 */
edgui_coord_t edgui_disp_get_hor_res(edgui_disp_t * disp);

/**
 * Get the vertical resolution of a display
 * @param disp pointer to a display (NULL to use the default display)
 * @return the vertical resolution of the display
 */
edgui_coord_t edgui_disp_get_ver_res(edgui_disp_t * disp);

/**
 * Get if anti-aliasing is enabled for a display or not
 * @param disp pointer to a display (NULL to use the default display)
 * @return true: anti-aliasing is enabled; false: disabled
 */
bool edgui_disp_get_antialiasing(edgui_disp_t * disp);

//! @cond Doxygen_Suppress

/**
 * Call in the display driver's `flush_cb` function when the flushing is finished
 * @param disp_drv pointer to display driver in `flush_cb` where this function is called
 */
EDGUI_ATTRIBUTE_FLUSH_READY void edgui_disp_flush_ready(edgui_disp_drv_t * disp_drv);

//! @endcond

/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
edgui_disp_t * edgui_disp_get_next(edgui_disp_t * disp);

/**
 * Get the internal buffer of a display
 * @param disp pointer to a display
 * @return pointer to the internal buffers
 */
edgui_disp_buf_t * edgui_disp_get_buf(edgui_disp_t * disp);

/**
 * Get the number of areas in the buffer
 * @return number of invalid areas
 */
uint16_t edgui_disp_get_inv_buf_size(edgui_disp_t * disp);

/**
 * Pop (delete) the last 'num' invalidated areas from the buffer
 * @param num number of areas to delete
 */
void edgui_disp_pop_from_inv_buf(edgui_disp_t * disp, uint16_t num);

/**
 * Check the driver configuration if it's double buffered (both `buf1` and `buf2` are set)
 * @param disp pointer to to display to check
 * @return true: double buffered; false: not double buffered
 */
bool edgui_disp_is_double_buf(edgui_disp_t * disp);

/**
 * Check the driver configuration if it's TRUE double buffered (both `buf1` and `buf2` are set and
 * `size` is screen sized)
 * @param disp pointer to to display to check
 * @return true: double buffered; false: not double buffered
 */
bool edgui_disp_is_true_double_buf(edgui_disp_t * disp);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
