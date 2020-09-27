#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "ai_common.h"
#ifdef IMG_RECT
    #define CAMERA_BUF_BASE ((CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0x35800 + 0xC0000000)//reserve 0xCA800 for camera buffer
#else
    #define CAMERA_BUF_BASE ((CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0x65400 + 0xC0000000)//(0x10480000)
#endif
int camera_config(dcmi_frame_irq_callback_t cb);

#endif

