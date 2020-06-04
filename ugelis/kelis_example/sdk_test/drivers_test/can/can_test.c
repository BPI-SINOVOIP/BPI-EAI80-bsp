/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       adc demo source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_hal_gpio.h>
#include <gm_hal_sqi.h>
#include <gm_common.h>
#include <misc/printk.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>
#include <ztest.h>
#include <can.h>


#define CAN_SEND_TIMEOUT 1000000
#define TIME_NO_WAIT  0x00000000

static void PrintRxMesage(struct can_msg *msg)
{
    int n = 0;
    TC_PRINT("msg id :%d\n", msg->id);
    TC_PRINT("msg dlc :%d\n", msg->dlc);
    TC_PRINT("msg id :%d\n", msg->id);
    for (n = 0; n < 8; n++)
    {
        TC_PRINT("data[%d] :%d\n", n, msg->data[n]);
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
                TC_PRINT("Err CAN_GetMessage\n");
                return ;
            }
            PrintRxMesage(&msg);
        }
    }
}

int can_tx_demo(void)
{
    uint8_t ret = 0;
    int i = 0;
    int n = 0;
    struct device *dev;
    dev = device_get_binding(DEV_CAN0_NAME);
    if (!dev)
    {
        TC_PRINT("Main Err: device_get_binding\n");
        return TC_FAIL;
    }

    can_set_callback(dev, callback);
    struct can_msg msg;

    for (i = 0; i < 15; i++)
    {
        msg.id = 0x01;
        msg.dlc = 8;
        msg.info = ext_msg;
        msg.type = can_data_frame;
        for (n = 0; n < 8; n++)
        {
            if (n % 2)
            {
                msg.data[n] = 0x5;
            }
            else
            {
                msg.data[n] = 0xa;
            }
        }
        ret = can_send(dev, &msg, CAN_SEND_TIMEOUT);
        if (ret != TC_PASS)
        {
            return TC_FAIL;
        }
        HAL_DelayMs(100);

    }
    return ret;
}


void can_test(void)
{
    TC_PRINT("================CAN Test Start=================\n\n\n");
    zassert_true(can_tx_demo() == TC_PASS, NULL);

    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);

    TC_PRINT("================CAN Test  End==================\n\n\n");
}
