/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <misc/printk.h>
#include <ztest.h>
#include <device.h>
#include "can_test.h"
#include "board.h"
#include <gm_hal_can.h>
//#include <gm_hal_clock.h>
#include <pinmux.h>
#include <gm_hal_gpio.h>

#define CAN_BASE_ADDR      CAN

#define TIME                 1000000 /* timeout = 1s */

static volatile uint32_t test_step = 0;
uint8_t send_data[8] = {0};

/******************************************************************************
* Local types
******************************************************************************/
static CAN_Handle_T s_Handle;

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;



static int can0_pinmux_config()
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_CANPinMux[3] =
    {
        {GPIO_PIN_14, GPIOH, GPIOH14_AF1_CAN0_RX},
        {GPIO_PIN_15, GPIOH, GPIOH15_AF1_CAN0_TX},
        {GPIO_PIN_0, GPIOI, GPIOI0_AF1_CAN0_DBG},
    };

    pin_cfg.pin = s_CANPinMux[0].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[0].alternate;
    ret = HAL_GPIO_Init(s_CANPinMux[0].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_CANPinMux[1].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[1].alternate;
    ret = HAL_GPIO_Init(s_CANPinMux[1].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = s_CANPinMux[2].pin;
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[2].alternate;
    ret = HAL_GPIO_Init(s_CANPinMux[2].device, &pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}


static uint32_t s_filterCode[MAX_FILTER_NUM] =
{
    0x0fffffff, 0x17ffffff, 0x1bffffff, 0x1dffffff,
    0x1effffff, 0x1f7fffff, 0x1fbfffff, 0x1fefffff,
    0x1ff7ffff, 0x1ffbffff, 0x1ffdffff, 0x1ffeffff,
    0x1fff7fff, 0x1fffbfff, 0x1fffdfff, 0xabcddecb
};
static uint32_t s_filterMask[MAX_FILTER_NUM] =
{
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};

static void PrintRxMesage(CAN_FrameInfor_T *pFrameInfor)
{
    uint32_t i = 0;

    TC_PRINT("\nRxFrameInfor ID :0x%x\n", pFrameInfor->id);

    if (pFrameInfor->frameFormat == CAN_STAND_FRAME)
    {
        TC_PRINT("RxFrameInfor is STANDFRAME\n");
        TC_PRINT("RxFrameInfor R0:%d\n", pFrameInfor->r0);
        test_step++;
    }
    else
    {
        TC_PRINT("RxFrameInfor is EXTEND FRAME\n");
        TC_PRINT("RxFrameInfor R0:%d\n", pFrameInfor->r0);
        TC_PRINT("RxFrameInfor R1:%d\n", pFrameInfor->r1);
        test_step++;
    }
    if (pFrameInfor->frameType == CAN_DATA_FRAME)
    {
        TC_PRINT("RxFrameInfor is DATA FRAME\n");
    }
    else if (pFrameInfor->frameType == CAN_REMOTE_FRAME)
    {
        TC_PRINT("RxFrameInfor is REMOTE FRAME\n");
        return ;
    }
    else if (pFrameInfor->frameType == CAN_SELF_DATA_FRAME)
    {
        TC_PRINT("RxFrameInfor is SELF DATA FRAME\n");
    }
    else
    {
        TC_PRINT("RxFrameInfor is SELF CAN_SELF REMOTE FRAME\n");
        return ;
    }

    TC_PRINT("RxFrameInfor.leng:%d\n", pFrameInfor->dataLength);

    for (i = 0; i < pFrameInfor->dataLength ; i++)
    {


        TC_PRINT("DATA%d:0x%x\n", i, pFrameInfor->data[i]);
    }

}


static void CAN_Callback(CAN_Handle_T *hCAN, CAN_Event_T event, unsigned long param)
{
    uint8_t i = 0;
    uint8_t j = 0;
    CAN_FrameInfor_T *pRxFrameInfor = NULL;

    if (event == CAN_EVENT_RECEIVE_SUCCESS)
    {
        for (i = 0; i < param ; i++)
        {
            /**
             * enter success rx callback means data is ready,so
             * we can set timeout to 0
             */
            if (HAL_CAN_GetMessage(hCAN, &pRxFrameInfor, TIMEOUT_NO_WAIT) != RET_OK)
            {
                TC_PRINT("Err CAN_GetMessage\n");
                return TC_FAIL;
            }

            for (j = 0; j < pRxFrameInfor->dataLength ; j++)
            {
                if (pRxFrameInfor->data[i] != send_data[i])
                {
                    TC_PRINT("Err send data is not same to recevice data\n");
                    return TC_FAIL;
                }
            }

            PrintRxMesage(pRxFrameInfor);
        }
    }
}


static void CAN_FilterInit(CAN_FilterConfig_T *pFilter)
{
    uint8_t i = 0 ;

    for (i = 0; i < MAX_FILTER_NUM; i++)
    {
        pFilter->filterCode[i] = s_filterCode[i];
        pFilter->filterMask[i] = s_filterMask[i];

    }
}


/*CAN self transfer and recevice message test*/

RET_CODE self_tx_rx_test()
{
    TC_PRINT("CAN SELF_RX_TX test begin!\n");
    k_sleep(1);
    RET_CODE ret = RET_OK;
    CAN_Config_T config;
    CAN_FrameInfor_T txFrameInfor;
    uint8_t i = 0;
    can0_pinmux_config();
    k_sleep(1);

    if (ret != RET_OK)
    {
        TC_PRINT("%s:Err CAN_ConfigPinmux\n", __FUNCTION__);
        goto out;
    }


    memset(&config, 0, sizeof(CAN_Config_T));
    config.errWarnThreshold   = 120;
    config.workMode           = CAN_SELFTEST_MODE;

    CAN_FilterInit(&config.filter);

    /* config baudRateSetting */
    config.baudRateAttribute.prescaler    = 48000000;
    config.baudRateAttribute.baudRate     = 20000;        /* 20kbps */
    config.baudRateAttribute.sample       = CAN_SAM_1;
    config.baudRateAttribute.sjw          = CAN_SJW_1TQ;
    config.baudRateAttribute.bs1          = CAN_TSEG_7TQ;
    config.baudRateAttribute.bs2          = CAN_TSEG_4TQ;

    config.callback = CAN_Callback;
    k_sleep(1);
    ret = HAL_CAN_Init(CAN, &config, &s_Handle);
    if (ret != RET_OK)
    {
        TC_PRINT("%s:Err:CAN_Init\n", __FUNCTION__);
        goto out;
    }
    k_sleep(1);

    /* config TX message and sent to CAN bus network */
    memset(&txFrameInfor, 0, sizeof(CAN_FrameInfor_T));
    send_data[0] = 0xaa;
    send_data[1] = 0x55;
    send_data[2] = 0x5a;
    send_data[3] = 0xa5;
    send_data[4] = 0xff;
    send_data[5] = 0x00;
    send_data[6] = 0xc0;
    send_data[7] = 0x0b;

    do
    {

        for (i = 0; i < 8; i++)
        {
            txFrameInfor.data[i] = send_data[i];
        }

        txFrameInfor.dataLength  = 8;
        txFrameInfor.frameFormat = CAN_EXTEND_FRAME;
        txFrameInfor.frameType   = CAN_SELF_DATA_FRAME;
        txFrameInfor.id          = 0x01;

        TC_PRINT("Start send message!\n");

        ret = HAL_CAN_TransferMessage(&s_Handle, &txFrameInfor, TIME);
        if (ret != RET_OK)
        {
            TC_PRINT("Err:%s:CAN_TransferMessage\n", __FUNCTION__);
            goto out;
        }


    } while (test_step == 0);
    TC_PRINT("CAN SELF_RX_TX test PASS!\n");
    return TC_PASS;


out:
    return TC_FAIL;
}


void can_test(void)
{
    TC_PRINT("================HAL CAN Test Start=================\n\n\n");
    zassert_true(self_tx_rx_test() == TC_PASS, NULL);
    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);
    TC_PRINT("================HAL CAN Test  End==================\n\n\n");
}

