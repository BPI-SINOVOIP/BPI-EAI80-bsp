/**
 * @file edgui_test_tabview.h
 *
 */

#ifndef EDGUI_TEST_TABVIEW_H
#define EDGUI_TEST_TABVIEW_H

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


#if EDGUI_USE_TABVIEW && EDGUI_USE_TESTS

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
 * Create tab views to test their functionalities
 */
void edgui_test_tabview_1(void);
void edgui_test_tabview_2(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TABVIEW && EDGUI_USE_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEST_TABVIEW_H*/
