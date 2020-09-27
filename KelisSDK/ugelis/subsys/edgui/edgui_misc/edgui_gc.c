/**
 * @file edgui_gc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "edgui_gc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
#if(!defined(EDGUI_ENABLE_GC)) || EDGUI_ENABLE_GC == 0
EDGUI_ROOTS
#endif /* EDGUI_ENABLE_GC */
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/
