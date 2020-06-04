/**
 * @file edgui_mem.h
 *
 */

#ifndef EDGUI_MEM_H
#define EDGUI_MEM_H

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
#include <stddef.h>
#include "edgui_log.h"

/*********************
 *      DEFINES
 *********************/
// Check windows
#ifdef __WIN64
#define EDGUI_MEM_ENV64
#endif

// Check GCC
#ifdef __GNUC__
#if defined(__x86_64__) || defined(__ppc64__)
#define EDGUI_MEM_ENV64
#endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Heap information structure.
 */
typedef struct
{
    uint32_t total_size; /**< Total heap size */
    uint32_t free_cnt;
    uint32_t free_size; /**< Size of available memory */
    uint32_t free_biggest_size;
    uint32_t used_cnt;
    uint8_t used_pct; /**< Percentage used */
    uint8_t frag_pct; /**< Amount of fragmentation */
} edgui_mem_monitor_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initiaize the dyn_mem module (work memory and other variables)
 */
void edgui_mem_init(void);

/**
 * Allocate a memory dynamically
 * @param size size of the memory to allocate in bytes
 * @return pointer to the allocated memory
 */
void * edgui_mem_alloc(uint32_t size);

/**
 * Free an allocated data
 * @param data pointer to an allocated memory
 */
void edgui_mem_free(const void * data);

/**
 * Reallocate a memory with a new size. The old content will be kept.
 * @param data pointer to an allocated memory.
 * Its content will be copied to the new memory block and freed
 * @param new_size the desired new size in byte
 * @return pointer to the new memory
 */
void * edgui_mem_realloc(void * data_p, uint32_t new_size);

/**
 * Join the adjacent free memory blocks
 */
void edgui_mem_defrag(void);

/**
 * Give information about the work memory of dynamic allocation
 * @param mon_p pointer to a dm_mon_p variable,
 *              the result of the analysis will be stored here
 */
void edgui_mem_monitor(edgui_mem_monitor_t * mon_p);

/**
 * Give the size of an allocated memory
 * @param data pointer to an allocated memory
 * @return the size of data memory in bytes
 */
uint32_t edgui_mem_get_size(const void * data);

/**********************
 *      MACROS
 **********************/

/**
 * Halt on NULL pointer
 * p pointer to a memory
 */
#if EDGUI_USE_LOG == 0
#define edgui_mem_assert(p)                                                                                               \
    {                                                                                                                  \
        if(p == NULL)                                                                                                  \
            while(1)                                                                                                   \
                ;                                                                                                      \
    }
#else
#define edgui_mem_assert(p)                                                                                               \
    {                                                                                                                  \
        if(p == NULL) {                                                                                                \
            EDGUI_LOG_ERROR("Out of memory!");                                                                            \
            while(1)                                                                                                   \
                ;                                                                                                      \
        }                                                                                                              \
    }
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_MEM_H*/
