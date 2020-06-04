/**
 * @file edgui_tutorial_keyboard.h
 *
 */

#ifndef EDGUI_TUTORIAL_KEYBOARD_H
#define EDGUI_TUTORIAL_KEYBOARD_H

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


#if EDGUI_USE_TUTORIALS && EDGUI_USE_GROUP

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
 * Create a simple GUI to demonstrate encoder control capability
 * kp_indev optinonally pass a keypad input device to control the object (NULL if unused)
 */
void edgui_tutorial_keyboard(edgui_indev_t * kp_indev);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TUTORIAL_KEYBOARD_H*/
