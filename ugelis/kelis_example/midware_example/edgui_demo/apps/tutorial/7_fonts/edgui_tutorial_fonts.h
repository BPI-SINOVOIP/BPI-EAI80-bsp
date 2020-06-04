/**
 * @file edgui_tutorial_fonts.h
 *
 */

#ifndef EDGUI_TUTORIAL_FONTS_H
#define EDGUI_TUTORIAL_FONTS_H

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

void edgui_tutorial_fonts(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TUTORIAL_FONTS_H*/
