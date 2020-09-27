/**
 * @file edgui_examples.h
 *
 */

#ifndef EDGUI_EXAMPLES_H
#define EDGUI_EXAMPLES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl/edgui.h"

/*********************
 *      DEFINES
 *********************/
/*Test  edgui version*/
#define EDGUI_EXAMPLES_EDGUI_REQ_MAJOR  6
#define EDGUI_EXAMPLES_EDGUI_REQ_MINOR  0
#define EDGUI_EXAMPLES_EDGUI_REQ_PATCH  0

#if EDGUI_EXAMPLES_EDGUI_REQ_MAJOR != EDGUI_VERSION_MAJOR
#error "edgui_examples: Wrong edgui major version"
#endif

#if EDGUI_EXAMPLES_EDGUI_REQ_MINOR > EDGUI_VERSION_MINOR
#error "edgui_examples: Wrong edgui minor version"
#endif

#if EDGUI_EXAMPLES_EDGUI_REQ_PATCH > EDGUI_VERSION_PATCH
#error "edgui_examples: Wrong edgui bug fix version"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_EXAMPLES_H*/
