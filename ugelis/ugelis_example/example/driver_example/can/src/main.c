/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <ugelis.h>
#include <misc/printk.h>
#include <can.h>


#define CAN_SEND_TIMEOUT 1000000
#define TIME_NO_WAIT  0x00000000


#define CAN_DEBUG     1
#define CAN_ERR       1

#define DEBUG()  __asm("BKPT")


#ifndef CAN_DEBUG
    #define can_debug(...)      do {} while (0)
#else
    #define can_debug           printk
#endif

#ifndef CAN_ERR
    #define can_err(...)        do {} while (0)
#else
    #define can_err             printk
#endif


#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))

void patch_config(void) /*FIX HIRC UART PRINT ISSUE*/
{
    HW32_REG(0x1ffff0a8) = 0xfffffbab;
    HW32_REG(0x1ffff0ac) = 0xff88ffff;
    HW32_REG(0x1ffff0b0) = 0xff3fffff;
}

static void PrintRxMesage(struct can_msg *msg)
{
    int n = 0;
    can_debug("msg id :%d\n", msg->id);
    can_debug("msg dlc :%d\n", msg->dlc);
    can_debug("msg id :%d\n", msg->id);
    for (n = 0; n < 8; n++)
    {
        can_debug("data[%d] :%d\n", n, msg->data[n]);
    }

}







static void callback(enum can_evt event, s32_t param)
{
    uint8_t i = 0;
    struct can_msg msg;
    struct device *dev;
    dev = device_get_binding(DEV_CAN0_NAME);
    if (event == can_event_receive_success)
    {
        for (i = 0; i < param ; i++)
        {
            /**         * enter success rx callback means data is ready,so         * we can set timeout to 0         */
            if (can_get_msg(dev, &msg, TIME_NO_WAIT) != 0)
            {
                can_err("Err CAN_GetMessage\n");
                return ;
            }
            PrintRxMesage(&msg);
        }
    }





}



int main()
{
    patch_config();
    uint8_t ret = 0;
    int i = 0;
    struct device *dev;
    dev = device_get_binding(DEV_CAN0_NAME);
    if (!dev)
    {
        printk("Main Err: device_get_binding\n");
        return ret;
    }

    can_set_callback(dev, callback);
    struct can_msg msg;

    while (1)
    {
        msg.id = 0x01;
        msg.dlc = 8;
        msg.info = ext_msg;
        msg.type = can_data_frame;
        for (i = 0; i < 8; i++)
        {
            msg.data[i] = i;
        }

        can_send(dev, &msg, CAN_SEND_TIMEOUT);
        k_sleep(100);

    }

}





