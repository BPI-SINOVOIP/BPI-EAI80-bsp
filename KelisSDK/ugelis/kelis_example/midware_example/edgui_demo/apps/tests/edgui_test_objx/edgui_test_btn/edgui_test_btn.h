/**
 * @file edgui_test_btn.h
 *
 */

#ifndef EDGUI_TEST_BTN_H
#define EDGUI_TEST_BTN_H

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


#if EDGUI_USE_BTN && EDGUI_USE_TESTS

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
 * Create buttons to test their functionalities
 */
void edgui_test_btn_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_BTN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_USE_BTN && EDGUI_USE_TESTS*/
