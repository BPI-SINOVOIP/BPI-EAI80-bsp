#ifndef __STORAGE_H__
#define __STORAGE_H__
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>
void ai_init_storage(void);
int ai_load_ops_data(unsigned int ops_offset, unsigned int ops_len, unsigned int img_offset, unsigned int img_len, unsigned char *ops_data, unsigned char *image);
int aimodule_init_device(void);
#endif
