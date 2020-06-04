#ifndef __PROCESSOR_COMMON_H__
#define __PROCESSOR_COMMON_H__
//#include "ai_common.h"
#include <stdint.h>

__asm void *FastFirstPutDataOptimize(void *bank_addr, int32_t len_col, void *row_addr, void *row_next_addr);
__asm void *FastShortPutDataOptimize(void *bank_addr, int32_t len_col, void *row_addr, void *row_next_addr);
__asm void *FastMaxPool_2X2_S1(void *bank_addr, int32_t num_row, int32_t num_col);
__asm void *FastMaxPool_2X2_S2(void *bank_addr, int32_t num_row, int32_t num_col, void *bank_out_add);
#endif
