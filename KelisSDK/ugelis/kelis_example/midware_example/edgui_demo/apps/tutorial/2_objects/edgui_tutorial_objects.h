/**
 * @file edgui_tutorial_objects.h
 *
 */

#ifndef EDGUI_TUTORIAL_OBJECTS_H
#define EX_OBJECTS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui.h"
#include "edgui_ex_conf.h"
#else
#include <edgui/edgui.h>
#include <edgui/edgui_conf.h>
#endif
	
#include "edgui_prj_conf.h"

#if EDGUI_USE_TUTORIALS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void edgui_tutorial_objects(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TUTORIAL_OBJECTS_H*/
