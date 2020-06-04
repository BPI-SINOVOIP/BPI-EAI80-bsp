/**
 * @file edgui_types.h
 *
 */

#ifndef EDGUI_TYPES_H
#define EDGUI_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * edgui error codes.
 */
enum {
    EDGUI_RES_INV = 0, /*Typically indicates that the object is deleted (become invalid) in the action
                       function or an operation was failed*/
    EDGUI_RES_OK,      /*The object is valid (no deleted) after the action*/
};
typedef uint8_t edgui_res_t;

typedef unsigned long int edgui_uintptr_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TYPES_H*/
