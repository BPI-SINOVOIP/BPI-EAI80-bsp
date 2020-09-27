/**
 * @file edgui_tutorial_animation.h
 *
 */

#ifndef EDGUI_TUTORIAL_ANIMATION_H
#define EDGUI_TUTORIAL_ANIMATION_H

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

#if EDGUI_USE_TUTORIALS && EDGUI_USE_ANIMATION


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
 * Crate some objects an animate them
 */
void edgui_tutorial_animations(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TUTORIAL_ANTMATION_H*/
