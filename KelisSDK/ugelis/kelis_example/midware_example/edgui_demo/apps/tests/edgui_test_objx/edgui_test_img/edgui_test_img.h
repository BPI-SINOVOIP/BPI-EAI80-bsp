/**
 * @file edgui_test_img.h
 *
 */

#ifndef EDGUI_TEST_IMG_H
#define EDGUI_TEST_IMG_H

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


#if EDGUI_USE_IMG && EDGUI_USE_TESTS

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
 * Create images to test their functionalities
 */
void edgui_test_img_1(void);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_IMG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_USE_IMG && EDGUI_USE_TESTS*/
