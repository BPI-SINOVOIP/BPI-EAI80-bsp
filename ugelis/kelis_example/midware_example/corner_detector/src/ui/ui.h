#ifndef __UI_H__
#define __UI_H__
#include <kernel.h>
#include "draw.h"
//#include "font.h"

#define MEM_RESERVE_START   (CONFIG_SDRAM_BASE_ADDRESS + (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024)

#define MEM_IDTLIST_BASE    (MEM_RESERVE_START)
#define MEM_IDTLIST_SIZE    (2*1024)

#define MEM_UI_BASE         (MEM_IDTLIST_BASE + MEM_IDTLIST_SIZE)
#define MEM_UI_SIZE         (256*SIZE_1K)

#define MEM_CAM_BASE        (MEM_UI_BASE + MEM_UI_SIZE)//(0x10020000)
#define MEM_CAM_SIZE        (CAM_BUF_SIZE)

#define LCD_UI_W            160
#define LCD_UI_H            240

#endif
