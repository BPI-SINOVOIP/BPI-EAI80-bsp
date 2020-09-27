
/**
 * @file edgui_port_indev_templ.h
 *
 */

#ifndef EDGUI_PORT_INDEV_H
#define EDGUI_PORT_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <edgui.h>
#include <display/touch.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern const uint32_t POINT_COLOR_TBL[5];
extern Touch_Screen_t touchScreen;
/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_PORT_INDEV_H*/
