/**
 * @file edgui_port_disp_templ.h
 *
 */
#ifndef EDGUI_PORT_DISP_TEMPL_H
#define EDGUI_PORT_DISP_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <edgui.h>
#include <display/fb_display.h>
#include <gpio.h>

/*********************
 *      DEFINES
 *********************/

#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)

#define MEM_RESERVE_START   (CONFIG_SDRAM_BASE_ADDRESS + (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024)

#define MEM_IDTLIST_BASE    (MEM_RESERVE_START)
#define MEM_IDTLIST_SIZE    (2*1024)

#define MEM_UI_BASE         (MEM_IDTLIST_BASE + MEM_IDTLIST_SIZE)
#define MEM_UI_SIZE         (EDGUI_HOR_RES_MAX * EDGUI_VER_RES_MAX * 2)

#define MEM_DRAW_BASE       (MEM_UI_BASE + MEM_UI_SIZE)
#define MEM_DRAW_SIZE       (EDGUI_HOR_RES_MAX * EDGUI_VER_RES_MAX * 2)
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

#endif /*EDGUI_PORT_DISP_H*/
