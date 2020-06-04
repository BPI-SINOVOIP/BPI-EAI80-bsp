/**
 * @file edgui_test_task.h
 *
 */

#ifndef EDGUI_TEST_TASK_H
#define EDGUI_TEST_TASK_H

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
 * Test the scheduling with various periods and priorities.
 */
void edgui_test_task_1(void);

/**
 * Create a lot of short task and see their order. They should be executed according to their priority
 */
void edgui_test_task_2(void);

/**
 * Change the priority later
 */
void edgui_test_task_3(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEST_TASK_H*/
