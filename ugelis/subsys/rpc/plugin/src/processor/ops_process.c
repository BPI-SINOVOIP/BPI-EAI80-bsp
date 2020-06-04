/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		ops_process.c
 *
 * @author		Wen.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/03/19
 *
 * @brief		Process operators stream 
 *
 * @note		
 *				2019/03/19, Wen.Liu, v1.0.0
 *              Initial version.
 *			
 **/

#include "data_type.h"
#include <stdlib.h>
#include "network.h"
#include "ops.h"
#include "processor.h"

int easynet_ops_process(struct easynet_dev *dev)
{
    int ret;
    struct easynet_ops_param *param = (struct easynet_ops_param *)(dev->param + dev->cur_addr);// + sizeof(struct easynet_ops_param);
    
    /* Check sync word */
    if (param->sync != SYNC_WORD)
    {
        return RET_ERROR;
    } 

    
    /* Process one software command/operator or one group of hardware operators */
    ret = dev->process(dev, param);
    
    /* Continue polling if it is wait command and HW IP is busy on processing last command */
    if (param->op == OP_WAIT && ret == RET_BUSY)
    {
        return RET_BUSY; 
    }
    
    /* Next operator */
    dev->cur_addr += param->len;

	param = (struct easynet_ops_param *)(dev->param + dev->cur_addr);
    /* If it is last operator */
    //if (dev->cur_addr >= dev->len)
    if (param->len == 0)
    {
        return RET_OVER;
    }
    
    return RET_ONGOING;        
}

void easynet_ops_load(struct easynet_dev *dev, void *bin, uint32_t len, void *input)
{
    dev->cur_addr = 0;
    dev->param = (struct easynet_ops_param *)bin;
    dev->len = len;
    
    dev->load(dev, bin, len, input);
}
