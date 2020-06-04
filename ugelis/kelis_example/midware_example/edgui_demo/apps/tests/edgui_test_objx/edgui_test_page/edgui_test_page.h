/**
 * @file edgui_test_page.h
 *
 */

#ifndef EDGUI_TEST_PAGE_H
#define EDGUI_TEST_PAGE_H

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


#if EDGUI_USE_PAGE && EDGUI_USE_TESTS

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
 * Create pages to test their basic functionalities
 */
void edgui_test_page_1(void);

/**
 * Test styling, scrollbar modes, layout and action
 */
void edgui_test_page_2(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_PAGE && EDGUI_USE_TESTS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEST_PAGE_H*/
