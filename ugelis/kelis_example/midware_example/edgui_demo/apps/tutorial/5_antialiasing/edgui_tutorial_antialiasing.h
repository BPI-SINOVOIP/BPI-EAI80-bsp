/**
 * @file edgui_tutorial_antialiasing.h
 *
 */

#ifndef EDGUI_TUTORIAL_ANTIALIASING_H
#define EDGUI_TUTORIAL_ANTIALIASING_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui.h"
#include "edgui_conf.h"
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

/**
 * Create object to see how they change from the anti aliasing
 * Modify EDGUI_ANTIALIAS and EDGUI_FONT_ANTIALIAS to see what is changing
 */
void edgui_tutorial_antialiasing(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TUTORIAL_ANTIALIASING_H*/
