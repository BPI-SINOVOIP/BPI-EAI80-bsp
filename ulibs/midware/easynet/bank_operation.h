#ifndef __BANK_OPERATION_H__
#define __BANK_OPERATION_H__

#include "cross_kdp.h"
#include "kdp.h"

typedef enum
{
    RELU, LEAKY, LOGISTIC, LINEAR
} ACTIVATION;

void put_cmd(unsigned char *cmd_addr_start, int start, int length);
void kdp_print_op(int code);
void kdp_print_bank_op(int code);
int bank_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param, kdp_processor_t *processor);

#endif
