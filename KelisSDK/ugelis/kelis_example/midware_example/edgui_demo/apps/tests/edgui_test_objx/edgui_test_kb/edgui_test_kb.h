/**
 * @file edgui_test_kb.h
 *
 */

#ifndef EDGUI_TEST_KB_H
#define EDGUI_TEST_KB_H

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


#if EDGUI_USE_KB && EDGUI_USE_TESTS

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
 * Create a default object and test the basic functions
 */
void edgui_test_kb_1(void);

/**
 * Create a styles keyboard
 */
void edgui_test_kb_2(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_KB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_USE_KB && EDGUI_USE_TESTS*/
