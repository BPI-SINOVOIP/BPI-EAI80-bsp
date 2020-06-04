/**
 * @file symon.h
 *
 */

#ifndef SYSMON_H
#define SYSMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui.h"
#include "edgui_pri_conf.h"
#else
#include <edgui/edgui.h>
#include <edgui_pri_conf.h>
#endif
#if EDGUI_USE_SYSMON

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
 * Initialize the system monitor
 */
void sysmon_create(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_SYSMON*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SYSMON_H */
