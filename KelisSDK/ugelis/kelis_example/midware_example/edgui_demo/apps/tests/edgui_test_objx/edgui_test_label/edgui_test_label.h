/**
 * @file edgui_test_label.h
 *
 */

#ifndef EDGUI_TEST_LABEL_H
#define EDGUI_TEST_LABEL_H

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


#if EDGUI_USE_LABEL && EDGUI_USE_TESTS

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
 * Create labels with dynamic, static and array texts
 */
void edgui_test_label_1(void);

/**
 * Test label long modes
 */
void edgui_test_label_2(void);

/**
 * Test text insert and cut
 */
void edgui_test_label_3(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_LABEL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_USE_LABEL && EDGUI_USE_TESTS*/
