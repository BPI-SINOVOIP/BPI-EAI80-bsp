/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/*====================================================================
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file main.c
 *
 * @version 1.0.0
 *
 * @date 2018/12/10
 *
 *

 **********************************************************************/
#include <ugelis.h>
#include <gm_hal_can.h>
#include <gm_hal_uart.h>
#include <gm_hal_gpio.h>
#include <misc/printk.h>
#include <pinmux.h>



#define CAN_BASE_ADDR      CAN


#define SELT_TX_RX_TEST         1
//#define RX_NORMAL_TEST          1
//#define TX_NORMAL_TEST          1
//#define RX_FILTER_TEST          1
//#define RX_ADDRESS_FIFO_TEST    1


/******************************************************************************
* Constants and macros
******************************************************************************/

#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF
#endif
#ifdef ERR_PRINTF
    #undef ERR_PRINTF
#endif
#ifdef WARNING_PRINTF
    #undef WARNING_PRINTF
#endif

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifdef DEBUG_PRINTF
    #define PR_DEBUG       printk
#else
    #define PR_DEBUG(...)  do{}while(0);
#endif


#ifdef ERR_PRINTF
    #define PR_ERR       printk
#else
    #define PR_ERR(...)  do{}while(0);
#endif


#ifdef WARNING_PRINTF
    #define PR_WARNINR       printk
#else
    #define PR_WARNINR(...)  do{}while(0);
#endif



#define TIME                 1000000 /* timeout = 1s */


/******************************************************************************
* Local types
******************************************************************************/
static CAN_Handle_T s_Handle;


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


/******************************************************************************
* Local functions
******************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/
typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

/*use CAN0,XPI3 is CAN_RX,XPI4 is CAN_TX*/
static int can0_pinmux_config()
{
    int ret = 0;
    struct device *porti = NULL;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_CANPinMux[2] =
    {
        {PINMUX_GPIO_PIN_3, GPIOI, GPIOI3_AF7_CAN1_RX},
        {PINMUX_GPIO_PIN_4, GPIOI, GPIOI4_AF7_CAN1_TX},
    };

    porti = device_get_binding(CONFIG_PINMUX_GM_PORTI_NAME);
    if (porti == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_CANPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[0].alternate;
    ret = pinmux_pin_set(porti, s_CANPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_CANPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_CANPinMux[1].alternate;
    ret = pinmux_pin_set(porti, s_CANPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}

static void PrintRxMesage(CAN_FrameInfor_T *pFrameInfor)
{
    uint32_t i = 0;

    PR_DEBUG("\nRxFrameInfor ID :0x%x\n", pFrameInfor->id);

    if (pFrameInfor->frameFormat == CAN_STAND_FRAME)
    {
        PR_DEBUG("RxFrameInfor is STANDFRAME\n");
        PR_DEBUG("RxFrameInfor R0:%d\n", pFrameInfor->r0);
    }
    else
    {
        PR_DEBUG("RxFrameInfor is EXTEND FRAME\n");
        PR_DEBUG("RxFrameInfor R0:%d\n", pFrameInfor->r0);
        PR_DEBUG("RxFrameInfor R1:%d\n", pFrameInfor->r1);
    }
    if (pFrameInfor->frameType == CAN_DATA_FRAME)
    {
        PR_DEBUG("RxFrameInfor is DATA FRAME\n");
    }
    else if (pFrameInfor->frameType == CAN_REMOTE_FRAME)
    {
        PR_DEBUG("RxFrameInfor is REMOTE FRAME\n");
        return ;
    }
    else if (pFrameInfor->frameType == CAN_SELF_DATA_FRAME)
    {
        PR_DEBUG("RxFrameInfor is SELF DATA FRAME\n");
    }
    else
    {
        PR_DEBUG("RxFrameInfor is SELF CAN_SELF REMOTE FRAME\n");
        return ;
    }

    PR_DEBUG("RxFrameInfor.leng:%d\n", pFrameInfor->dataLength);

    for (i = 0; i < pFrameInfor->dataLength ; i++)
    {
        PR_DEBUG("DATA%d:0x%x\n", i, pFrameInfor->data[i]);
    }

}





static void CAN_Callback(CAN_Handle_T *hCAN, CAN_Event_T event, unsigned long param)
{
    uint8_t i = 0;
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
                PR_ERR("Err CAN_GetMessage\n");
                return ;
            }
            PrintRxMesage(pRxFrameInfor);
        }
    }
}







#ifdef RX_FILTER_TEST
static void CAN_FilterInit(CAN_FilterConfig_T *pFilter)
{
    /*change different msk to test*/

    pFilter->filterCode[0] = 0x0fffffff;
    pFilter->filterMask[0] = 0xe0000000;

    pFilter->filterCode[1] = 0x17ffffff;
    pFilter->filterMask[1] = 0xe0000000;

    pFilter->filterCode[2] = 0x1bffffff;
    pFilter->filterMask[2] = 0xfbffffff;

    pFilter->filterCode[3] = 0x1dffffff;
    pFilter->filterMask[3] = 0xe0000000;

    pFilter->filterCode[4] = 0x1effffff;
    pFilter->filterMask[4] = 0xe0000000;

    pFilter->filterCode[5] = 0x1f7fffff;
    pFilter->filterMask[5] = 0xe0000000;

    pFilter->filterCode[6] = 0x1fbfffff;
    pFilter->filterMask[6] = 0xe0000000;

    pFilter->filterCode[7] = 0x1fdfffff;
    pFilter->filterMask[7] = 0xe0000000;

    pFilter->filterCode[8] = 0x1fefffff;
    pFilter->filterMask[8] = 0xe0000000;

    pFilter->filterCode[9] = 0x1ff7ffff;
    pFilter->filterMask[9] = 0xe0000000;

    pFilter->filterCode[10] = 0x1ffbffff;
    pFilter->filterMask[10] = 0xe0000000;

    pFilter->filterCode[11] = 0x1ffdffff;
    pFilter->filterMask[11] = 0xe0000000;

    pFilter->filterCode[12] = 0x1ffeffff;
    pFilter->filterMask[12] = 0xe0000000;

    pFilter->filterCode[13] = 0x1fff7fff;
    pFilter->filterMask[13] = 0xe0000000;

    pFilter->filterCode[14] = 0x1fffbfff;
    pFilter->filterMask[14] = 0xe0000000;

    pFilter->filterCode[15] = 0x1fffdfff;
    pFilter->filterMask[15] = 0xe0000000;

}
#else
static void CAN_FilterInit(CAN_FilterConfig_T *pFilter)
{
    uint8_t i = 0 ;

    for (i = 0; i < MAX_FILTER_NUM; i++)
    {
        pFilter->filterCode[i] = s_filterCode[i];
        pFilter->filterMask[i] = s_filterMask[i];

    }
}

#endif


#ifdef RX_ADDRESS_FIFO_TEST
void CAN_SetRxAddress(uint8_t address)
{
    /*set to init mode*/
    LL_SET_INIT_MODE(CAN);
    /*set address : 0-15*/
    LL_CAN_SET_RX_ADDRESS(CAN, (address));
    LL_SET_WORK_MODE(CAN, CAN_STAND_MODE);
}
#endif





RET_CODE rx_normal_test()
{

    RET_CODE ret = RET_OK;
    CAN_Config_T config;

#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_Init();
    DebugConsoleInit(19200);
#endif
    /*CAN_RX:XPI3  CAN_TX:XPI4*/
    can0_pinmux_config();

    if (ret != RET_OK)
    {
        PR_ERR("%s:Err CAN_ConfigPinmux\n", __FUNCTION__);
        goto out;
    }




    PR_DEBUG("Can Receive Message Test Start\n");


    memset(&config, 0, sizeof(CAN_Config_T));
    config.errWarnThreshold = 120;
    config.workMode = CAN_STAND_MODE;


    CAN_FilterInit(&config.filter);



    config.baudRateAttribute.prescaler    = 48000000;
    config.baudRateAttribute.baudRate     = 20000;        /* 20kbps */
    config.baudRateAttribute.sample       = CAN_SAM_3;
    config.baudRateAttribute.sjw          = CAN_SJW_1TQ;
    config.baudRateAttribute.bs1          = CAN_TSEG_7TQ;
    config.baudRateAttribute.bs2          = CAN_TSEG_4TQ;


    config.callback = CAN_Callback;

    ret = HAL_CAN_Init(CAN, &config, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:CAN_Init\n", __FUNCTION__);
        goto out;
    }


    while (1)
    {
        /*deadloop to wait message at callback function*/
        k_sleep(1);
    }


out:
    return ret;

}





RET_CODE tx_normal_test()
{
    RET_CODE ret = RET_OK;
    CAN_Config_T config;
    CAN_FrameInfor_T txFrameInfor;
    uint8_t i = 0;

#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_Init();
    DebugConsoleInit(19200);
#endif
    /*CAN_RX:XPI3  CAN_TX:XPI4*/
    can0_pinmux_config();

    if (ret != RET_OK)
    {
        PR_ERR("%s:Err CAN_ConfigPinmux\n", __FUNCTION__);
        goto out;
    }



    PR_DEBUG("Can Demo Test Start\n");

    memset(&config, 0, sizeof(CAN_Config_T));
    config.errWarnThreshold   = 120;
    config.workMode           = CAN_STAND_MODE;

    CAN_FilterInit(&config.filter);

    /* config baudRateSetting */
    config.baudRateAttribute.prescaler    = 48000000;
    config.baudRateAttribute.baudRate     = 20000;        /* 20kbps */
    config.baudRateAttribute.sample       = CAN_SAM_3;
    config.baudRateAttribute.sjw          = CAN_SJW_1TQ;
    config.baudRateAttribute.bs1          = CAN_TSEG_7TQ;
    config.baudRateAttribute.bs2          = CAN_TSEG_4TQ;

    config.callback = NULL;

    ret = HAL_CAN_Init(CAN, &config, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:CAN Init\n", __FUNCTION__);
        goto out;
    }

    /* config Tx Message And Sent To CAN NetWork */
    memset(&txFrameInfor, 0, sizeof(CAN_FrameInfor_T));
    do
    {
        for (i = 0; i < 8; i++)
        {
            txFrameInfor.data[i] = i;
        }
        txFrameInfor.dataLength  = 8;
        txFrameInfor.frameFormat = CAN_EXTEND_FRAME;
        txFrameInfor.frameType   = CAN_DATA_FRAME;
        txFrameInfor.id          = 0x01;

        ret = HAL_CAN_TransferMessage(&s_Handle, &txFrameInfor, TIME);
        if (ret != RET_OK)
        {
            PR_ERR("Err:%s:CAN TransferMessage\n", __FUNCTION__);
            goto out;
        }

        k_sleep(1);
    } while (1);

out:
    return ret;
}







RET_CODE self_tx_rx_test()
{
    RET_CODE ret = RET_OK;
    CAN_Config_T config;
    CAN_FrameInfor_T txFrameInfor;
    uint8_t i = 0;

#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_Init();
    DebugConsoleInit(19200);
#endif

    /*CAN_RX:XPI3  CAN_TX:XPI4*/
    can0_pinmux_config();

    if (ret != RET_OK)
    {
        PR_ERR("%s:Err CAN_ConfigPinmux\n", __FUNCTION__);
        goto out;
    }

    PR_DEBUG("Can Demo Test Start\n");

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

    ret = HAL_CAN_Init(CAN, &config, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:CAN_Init\n", __FUNCTION__);
        goto out;
    }

    /* config TX message and sent to CAN bus network */
    memset(&txFrameInfor, 0, sizeof(CAN_FrameInfor_T));
    do
    {
        for (i = 0; i < 8; i++)
        {
            txFrameInfor.data[i] = 1;
        }
        txFrameInfor.dataLength  = 8;
        txFrameInfor.frameFormat = CAN_EXTEND_FRAME;
        txFrameInfor.frameType   = CAN_SELF_DATA_FRAME;
        txFrameInfor.id          = 0x01;

        ret = HAL_CAN_TransferMessage(&s_Handle, &txFrameInfor, TIME);
        if (ret != RET_OK)
        {
            PR_ERR("Err:%s:CAN_TransferMessage\n", __FUNCTION__);
            goto out;
        }

        k_sleep(1);
    } while (1);

out:
    return ret;
}




/*to test filter by changge the param in CAN_FilterInit function*/
RET_CODE rx_filter_test()
{
    uint8_t i = 0;
    CAN_Config_T config;
    CAN_FrameInfor_T txFrameInfor;
    RET_CODE ret = RET_OK;
#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_Init();
    DebugConsoleInit(19200);
#endif
    /*CAN_RX:XPI3  CAN_TX:XPI4*/
    can0_pinmux_config();

    PR_DEBUG("%s\n", __FUNCTION__);

    memset(&config, 0, sizeof(CAN_Config_T));

    /*for this test,CAN_FilterInit function is different*/
    CAN_FilterInit(&config.filter);

    config.errWarnThreshold = 120;
    config.workMode = CAN_STAND_MODE;
    /* config baudRateSetting */
    config.baudRateAttribute.prescaler    = 48000000;    /* 48MHZ */
    config.baudRateAttribute.baudRate     = 20000;       /* 20kbps */
    config.baudRateAttribute.sample       = CAN_SAM_3;
    config.baudRateAttribute.sjw          = CAN_SJW_1TQ;
    config.baudRateAttribute.bs1          = CAN_TSEG_7TQ;
    config.baudRateAttribute.bs2          = CAN_TSEG_4TQ;

    config.callback = CAN_Callback;

    ret = HAL_CAN_Init(CAN, &config, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:CAN_Init\n", __FUNCTION__);
        return ret;
    }


    while (1);
}




RET_CODE rx_address_fifo_test()
{
    uint8_t i = 0;
    CAN_Config_T config;
    CAN_FrameInfor_T txFrameInfor;
    RET_CODE ret = RET_OK;
    int address;

#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_Init();
    DebugConsoleInit(19200);
#endif
    /*CAN_RX:XPI3  CAN_TX:XPI4*/
    can0_pinmux_config();

    PR_DEBUG("%s\n", __FUNCTION__);

    memset(&config, 0, sizeof(CAN_Config_T));

    CAN_FilterInit(&config.filter);

    config.errWarnThreshold = 120;
    config.workMode = CAN_STAND_MODE;
    /* config baudRateSetting */
    config.baudRateAttribute.prescaler    = 48000000;    /* 48MHZ */
    config.baudRateAttribute.baudRate     = 20000;       /* 20kbps */
    config.baudRateAttribute.sample       = CAN_SAM_3;
    config.baudRateAttribute.sjw          = CAN_SJW_1TQ;
    config.baudRateAttribute.bs1          = CAN_TSEG_7TQ;
    config.baudRateAttribute.bs2          = CAN_TSEG_4TQ;

    config.callback = CAN_Callback;

    ret = HAL_CAN_Init(CAN, &config, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:CAN_Init\n", __FUNCTION__);
        return ret;
    }

    /*max 15*/
    address = 31;
    CAN_SetRxAddress(4 * address);
    PR_DEBUG("start rx:0X%08X\n", LL_CAN_GET_RX_ADDRESS(CAN));

    while (1)
    {
        /* config Tx Message And Sent To CAN NetWork */
        memset(&txFrameInfor, 0, sizeof(CAN_FrameInfor_T));

        /*##########################################################################*/
        /* extern data frame :4dw*/
        PR_DEBUG("\nextern data frame :4dw\n");
        PR_DEBUG("rxaddress:0X%08X\n", LL_CAN_GET_RX_ADDRESS(CAN));
        k_sleep(1000);
        txFrameInfor.data[0] = 0x11;
        txFrameInfor.data[1] = 0x22;
        txFrameInfor.data[2] = 0x33;
        txFrameInfor.data[3] = 0x44;
        txFrameInfor.data[4] = 0x55;
        txFrameInfor.data[5] = 0x66;
        txFrameInfor.data[6] = 0x77;
        txFrameInfor.data[7] = 0x88;

        txFrameInfor.dataLength  = 8;
        txFrameInfor.frameFormat = CAN_EXTEND_FRAME;
        txFrameInfor.frameType   = CAN_SELF_DATA_FRAME;
        txFrameInfor.id++;

        ret = HAL_CAN_TransferMessage(&s_Handle, &txFrameInfor, TIME);
        if (ret != RET_OK)
        {
            PR_ERR("Err:%s:CAN_TransferMessage\n", __FUNCTION__);
            return ret;
        }

        /*##########################################################################*/
        /* stand remote  :2dw*/
        PR_DEBUG("\nstand remote  :2dw\n");
        PR_DEBUG("rxaddress:0X%08X\n", LL_CAN_GET_RX_ADDRESS(CAN));
        k_sleep(1000);
        memset(&txFrameInfor, 0, sizeof(CAN_FrameInfor_T));
        txFrameInfor.dataLength  = 0;
        txFrameInfor.frameFormat = CAN_STAND_FRAME;
        txFrameInfor.frameType   = CAN_SELF_REMOTE_FRAME;
        txFrameInfor.id++;

        ret = HAL_CAN_TransferMessage(&s_Handle, &txFrameInfor, TIME);
        if (ret != RET_OK)
        {
            PR_ERR("Err:%s:CAN_TransferMessage\n", __FUNCTION__);
            return ret;
        }

        /*##########################################################################*/
        /* extren data frame : 3 dw*/
        PR_DEBUG("\nextren data frame : 3 dw\n");
        PR_DEBUG("rxaddress:0X%08X\n", LL_CAN_GET_RX_ADDRESS(CAN));
        k_sleep(1000);
        memset(&txFrameInfor, 0, sizeof(CAN_FrameInfor_T));
        txFrameInfor.data[0]     = 0x88;
        txFrameInfor.data[1]     = 0x77;
        txFrameInfor.data[2]     = 0x66;

        txFrameInfor.dataLength  = 3;
        txFrameInfor.frameFormat = CAN_EXTEND_FRAME;
        txFrameInfor.frameType   = CAN_SELF_DATA_FRAME;
        txFrameInfor.id++;
        ret = HAL_CAN_TransferMessage(&s_Handle, &txFrameInfor, TIME);
        if (ret != RET_OK)
        {
            PR_ERR("Err:%s:CAN_TransferMessage\n", __FUNCTION__);
            return ret;
        }
        /*##########################################################################*/
        /* extern remode : 2dw */
        PR_DEBUG("\nextren remode : 2dw\n");
        PR_DEBUG("rxaddress:0X%08X\n", LL_CAN_GET_RX_ADDRESS(CAN));
        k_sleep(1000);
        memset(&txFrameInfor, 0, sizeof(CAN_FrameInfor_T));
        txFrameInfor.dataLength  = 0;
        txFrameInfor.frameFormat = CAN_EXTEND_FRAME;
        txFrameInfor.frameType   = CAN_SELF_REMOTE_FRAME;
        txFrameInfor.id++;

        ret = HAL_CAN_TransferMessage(&s_Handle, &txFrameInfor, TIME);
        if (ret != RET_OK)
        {
            PR_ERR("Err:%s:CAN_TransferMessage\n", __FUNCTION__);
            return ret;
        }

        /*##########################################################################*/
        /* stand data frame : 3dw*/
        PR_DEBUG("\nstand data frame : 3dw\n");
        PR_DEBUG("rxaddress:0X%08X\n", LL_CAN_GET_RX_ADDRESS(CAN));
        k_sleep(1000);
        memset(&txFrameInfor, 0, sizeof(CAN_FrameInfor_T));
        txFrameInfor.data[0] = 0x11;
        txFrameInfor.data[1] = 0x22;

        txFrameInfor.dataLength  = 2;
        txFrameInfor.frameFormat = CAN_STAND_FRAME;
        txFrameInfor.frameType   = CAN_SELF_DATA_FRAME;
        txFrameInfor.id++;

        ret = HAL_CAN_TransferMessage(&s_Handle, &txFrameInfor, TIME);
        if (ret != RET_OK)
        {
            PR_ERR("Err:%s:CAN_TransferMessage\n", __FUNCTION__);
            return ret;
        }
    }
}




RET_CODE main()
{

    RET_CODE ret = RET_OK;
#ifdef RX_NORMAL_TEST
    ret = rx_normal_test();
#endif

#ifdef TX_NORMAL_TEST
    ret = tx_normal_test();
#endif


#ifdef SELT_TX_RX_TEST
    ret = self_tx_rx_test();
#endif


#ifdef RX_FILTER_TEST
    ret = rx_filter_test();
#endif


#ifdef RX_ADDRESS_FIFO_TEST
    ret = rx_address_fifo_test();
#endif



#ifdef CODE_TEST
    ret = can_code_test();
#endif


    while (5)
    {
        PR_ERR("Err:NO test define!\n");
        k_sleep(1);

    }

    return ret;



}

