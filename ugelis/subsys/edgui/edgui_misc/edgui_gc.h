/**
 * @file edgui_gc.h
 *
 */

#ifndef EDGUI_GC_H
#define EDGUI_GC_H

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
#include "edgui_mem.h"
#include "edgui_ll.h"
#include "../edgui_draw/edgui_img_cache.h"

/*********************
 *      DEFINES
 *********************/

#define EDGUI_GC_ROOTS(prefix)                                                                                            \
    prefix edgui_ll_t _edgui_task_ll;  /*Linked list to store the edgui_tasks*/                                                 \
    prefix edgui_ll_t _edgui_disp_ll;  /*Linked list of screens*/                                                            \
    prefix edgui_ll_t _edgui_indev_ll; /*Linked list of screens*/                                                            \
    prefix edgui_ll_t _edgui_drv_ll;                                                                                         \
    prefix edgui_ll_t _edgui_file_ll;                                                                                        \
    prefix edgui_ll_t _edgui_anim_ll;                                                                                        \
    prefix edgui_ll_t _edgui_group_ll;                                                                                       \
    prefix edgui_ll_t _edgui_img_defoder_ll;                                                                                 \
    prefix edgui_img_cache_entry_t * _edgui_img_cache_array;                                                                 \
    prefix void * _edgui_task_act;                                                                                        \
    prefix void * _edgui_draw_buf; 

#define EDGUI_NO_PREFIX
#define EDGUI_ROOTS EDGUI_GC_ROOTS(EDGUI_NO_PREFIX)

#if EDGUI_ENABLE_GC == 1
#if EDGUI_MEM_CUSTOM != 1
#error "GC requires CUSTOM_MEM"
#endif /* EDGUI_MEM_CUSTOM */
#else  /* EDGUI_ENABLE_GC */
#define EDGUI_GC_ROOT(x) x
EDGUI_GC_ROOTS(extern)
#endif /* EDGUI_ENABLE_GC */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_GC_H*/
