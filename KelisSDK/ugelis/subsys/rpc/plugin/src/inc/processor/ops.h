/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		ops.h
 *
 * @author		Wen.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/03/20
 *
 * @brief		Operator Header For easynet
 *
 * @note		
 *				2019/03/20, Wen.Liu, v1.0.0
 *					Initial version.
 *			
 **/

#ifndef __OPS_H__
#define __OPS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Hardware Operators on Abstract Layer 
 */
 
typedef enum 
{
    OP_HW, 
    OP_WAIT, 
    OP_MAXPOOL,
    OP_AVGPOOL,
    OP_CONNECTED,
    OP_WRITEOUT,
    OP_DATAFETCH,    
    OP_INPUT,
    OP_SWITCH_INOUT,
    OP_LAYER,          /*< This is layer indicator for debugging step by step */
    OP_BUFFER,
    OP_SHORTCUT,   
    OP_ROUTE,  
    OP_ACTIVATION,
} OP_TYPE;

/* sync word for each operator */
#define SYNC_WORD 0x78515255
/**
 *  Abstract Operator Parameter, hw specific paramters are specified and processed by hardware layer 
 */
struct easynet_ops_param
{
    uint32_t sync;
    uint32_t op;
    uint32_t len;
};


/**
 *  Abstract Device Running Operators 
 */
struct easynet_dev
{
    void *param;
    int (*process)(struct easynet_dev *dev, struct easynet_ops_param *);
    //uint32_t (*compile)(struct easynet_dev *dev, network *net, void *bin, uint32_t max_len);
    void (*load)(struct easynet_dev *dev, void *bin, uint32_t len, void *input);
    uint32_t cur_addr;
    uint32_t len;
	unsigned char *input;
    void *context;  
    
};

/**
 * @brief Process abstract operator stream
 *
 * This function process all operators by calling low layer routine of computing device.
 *
 * @param  dev             handle of computing device
 *                    
 * @retval                 None
 */
struct easynet_dev *easynet_ops_init(void); 
int  easynet_ops_process(struct easynet_dev *dev);
void easynet_ops_load(struct easynet_dev *dev, void *bin, uint32_t len, void *input);

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _OPS_H_ */
