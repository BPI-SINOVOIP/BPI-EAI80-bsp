/**
 * @file edgui_log.h
 *
 */

#ifndef EDGUI_LOG_H
#define EDGUI_LOG_H

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

/*********************
 *      DEFINES
 *********************/

/*Possible log level. For compatibility declare it independently from `EDGUI_USE_LOG`*/

#define EDGUI_LOG_LEVEL_TRACE 0 /**< A lot of logs to give detailed information*/
#define EDGUI_LOG_LEVEL_INFO 1  /**< Log important events*/
#define EDGUI_LOG_LEVEL_WARN 2  /**< Log if something unwanted happened but didn't caused problem*/
#define EDGUI_LOG_LEVEL_ERROR 3 /**< Only critical issue, when the system may fail*/
#define EDGUI_LOG_LEVEL_NONE 4 /**< Do not log anything*/
#define _EDGUI_LOG_LEVEL_NUM 5 /**< Number of log levels */

typedef int8_t edgui_log_level_t;

#if EDGUI_USE_LOG
/**********************
 *      TYPEDEFS
 **********************/

/**
 * Log print function. Receives "Log Level", "File path", "Line number" and "Description".
 */
typedef void (*edgui_log_print_g_cb_t)(edgui_log_level_t level, const char *, uint32_t, const char *);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register custom print/write function to call when a log is added.
 * It can format its "File path", "Line number" and "Description" as required
 * and send the formatted log message to a consol or serial port.
 * @param print_cb a function pointer to print a log
 */
void edgui_log_register_print_cb(edgui_log_print_g_cb_t print_cb);

/**
 * Add a log
 * @param level the level of log. (From `edgui_log_level_t` enum)
 * @param file name of the file when the log added
 * @param line line number in the source code where the log added
 * @param dsc description of the log
 */
void edgui_log_add(edgui_log_level_t level, const char * file, int line, const char * dsc);

/**********************
 *      MACROS
 **********************/

#if EDGUI_LOG_LEVEL <= EDGUI_LOG_LEVEL_TRACE
#define EDGUI_LOG_TRACE(dsc) edgui_log_add(EDGUI_LOG_LEVEL_TRACE, __FILE__, __LINE__, dsc);
#else
#define EDGUI_LOG_TRACE(dsc)                                                                                              \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#endif

#if EDGUI_LOG_LEVEL <= EDGUI_LOG_LEVEL_INFO
#define EDGUI_LOG_INFO(dsc) edgui_log_add(EDGUI_LOG_LEVEL_INFO, __FILE__, __LINE__, dsc);
#else
#define EDGUI_LOG_INFO(dsc)                                                                                               \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#endif

#if EDGUI_LOG_LEVEL <= EDGUI_LOG_LEVEL_WARN
#define EDGUI_LOG_WARN(dsc) edgui_log_add(EDGUI_LOG_LEVEL_WARN, __FILE__, __LINE__, dsc);
#else
#define EDGUI_LOG_WARN(dsc)                                                                                               \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#endif

#if EDGUI_LOG_LEVEL <= EDGUI_LOG_LEVEL_ERROR
#define EDGUI_LOG_ERROR(dsc) edgui_log_add(EDGUI_LOG_LEVEL_ERROR, __FILE__, __LINE__, dsc);
#else
#define EDGUI_LOG_ERROR(dsc)                                                                                              \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#endif

#else /*EDGUI_USE_LOG*/

/*Do nothing if `EDGUI_USE_LOG  0`*/
#define edgui_log_add(level, file, line, dsc)                                                                             \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#define EDGUI_LOG_TRACE(dsc)                                                                                              \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#define EDGUI_LOG_INFO(dsc)                                                                                               \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#define EDGUI_LOG_WARN(dsc)                                                                                               \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#define EDGUI_LOG_ERROR(dsc)                                                                                              \
    {                                                                                                                  \
        ;                                                                                                              \
    }
#endif /*EDGUI_USE_LOG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_LOG_H*/
