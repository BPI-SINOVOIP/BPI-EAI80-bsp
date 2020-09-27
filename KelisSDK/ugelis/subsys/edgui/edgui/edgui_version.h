/**
 * @file edgui_version.h
 *
 */

#ifndef EDGUI_VERSION_H
#define EDGUI_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
/*Current version of edgui*/
#define EDGUI_VERSION_MAJOR   6
#define EDGUI_VERSION_MINOR   0
#define EDGUI_VERSION_PATCH   2
#define EDGUI_VERSION_INFO    ""


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/
/** Gives 1 if the x.y.z version is supported in the current version
 * Usage:
 *
 * - Require v6
 * #if EDGUI_VERSION_CHECK(6,0,0)
 *   new_func_in_v6();
 * #endif
 *
 *
 * - Require at least v5.3
 * #if EDGUI_VERSION_CHECK(5,3,0)
 *   new_feature_from_v5_3();
 * #endif
 *
 *
 * - Require v5.3.2 bugfixes
 * #if EDGUI_VERSION_CHECK(5,3,2)
 *   bugfix_in_v5_3_2();
 * #endif
 *
 * */
#define EDGUI_VERSION_CHECK(x,y,z) (x == EDGUI_VERSION_MAJOR && (y < EDGUI_VERSION_MINOR || (y == EDGUI_VERSION_MINOR && z <= EDGUI_VERSION_PATCH)))


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_VERSION_H*/
