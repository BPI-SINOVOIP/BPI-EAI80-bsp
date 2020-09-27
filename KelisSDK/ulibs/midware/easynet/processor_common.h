#ifndef __PROCESSOR_COMMON_H__
#define __PROCESSOR_COMMON_H__
//#include "ai_common.h"
#include <stdint.h>

#include "processor.h"


struct param_fcm
{
    int32_t len_col;
    int32_t valid_len_col;
    int32_t valid_len_row;
};

void kdp_hw_processor(uint32_t weight_len, unsigned char *weight_addr_start, uint32_t weight_offset,
                      unsigned char *cmd_addr_start, uint32_t cmd_offset, uint32_t cmd_len);

__asm void *FastFirstPutDataOptimize(void *bank_addr, int32_t len_col, void *row_addr, void *row_next_addr);
__asm void *FastShortPutDataOptimize(void *bank_addr, int32_t len_col, void *row_addr, void *row_next_addr);
__asm void *FastMaxPool_2X2_S1(void *bank_addr, int32_t num_row, int32_t num_col);
__asm void *FastMaxPool_2X2_S2(void *bank_addr, int32_t num_row, int32_t num_col, void *bank_out_add);
#endif
