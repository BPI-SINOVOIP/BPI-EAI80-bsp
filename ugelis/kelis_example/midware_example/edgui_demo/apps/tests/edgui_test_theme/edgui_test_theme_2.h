/**
 * @file edgui_test_theme_2.h
 *
 */

#ifndef EDGUI_TEST_THEME_2_H
#define EDGUI_TEST_THEME_2_H

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

	
#include "edgui_prj_conf.h"

#if EDGUI_USE_TESTS

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
 * Test run time theme change
 */
void edgui_test_theme_2(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEST_THEME_2_H*/
