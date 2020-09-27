/**
 * @file edgui_test_slider.h
 *
 */

#ifndef EDGUI_TEST_SLIDER_H
#define EDGUI_TEST_SLIDER_H

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


#if EDGUI_USE_SLIDER && EDGUI_USE_TESTS

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
 * Create sliders to test their functionalities
 */
void edgui_test_slider_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_SLIDER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_USE_SLIDER && EDGUI_USE_TESTS*/
