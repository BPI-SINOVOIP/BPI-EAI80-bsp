#ifndef __CAMERA_H__
#define __CAMERA_H__

#define CAMERA_BUF_BASE ((CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0x65400 + 0xC0000000)//(0x10480000)
int camera_config(dcmi_frame_irq_callback_t cb);

#endif

