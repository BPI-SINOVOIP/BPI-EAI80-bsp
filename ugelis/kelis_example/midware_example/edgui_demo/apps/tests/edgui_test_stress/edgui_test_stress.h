/**
 * @file edgui_test_stress.h
 *
 */

#ifndef EDGUI_TEST_STRESS_H
#define EDGUI_TEST_STRESS_H

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
 * Create and delete a lot of objects and animations.
 */
void edgui_test_stress_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEST_STRESS_H*/
