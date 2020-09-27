/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Christal.Xu
 *
 * @version     v1.0.0
 *
 * @date        2019/12/27
 *
 * @brief       pes test demo source file
 *
 * @note
 *              2019/12/27, Christal.Xu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_hal_gpio.h>
#include <gm_hal_pes.h>
#include <gm_common.h>
#include <misc/printk.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>
#include <ztest.h>

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printk
#endif

#define IO_TEST_CHANNLE     PES_EVENT_CHANNEL1  //channel set
#define IO_PRE_CHANNLE      PES_EVENT_CHANNEL0
#define IO_NEXT_CHANNLE     PES_EVENT_CHANNEL2

#define MASTER_IO_BANK      GPIOB
#define SLAVE_IO_BANK       GPIOA
#define MASTER_IO_PINNUM    IO_PIN_PORT_8
#define SLAVE_IO_PINNUM     IO_PIN_PORT_1

/*config IO as PES slave, GPIO bank and pin num according to different channel
    This IO can receive PES event, and light up LED
*/
static int PES_IOasSlaveIOCfg(GPIO_Device_T *bank, uint32_t pinnum)
{
    RET_CODE ret = RET_OK;
    pinnum = 1 << pinnum;

    ret = HAL_PES_IOPinmuxConfig(bank, pinnum, IO_SLAVE);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return ret;
    }
    return ret;
}

/*config another IO as PES Master, GPIO bank and pin num according to different channel
    level of this IO need to be changed, so that this IO can trigger PES
*/
static int PES_IOasMasterIOCfg(GPIO_Device_T *bank, uint32_t pinnum)
{
    RET_CODE ret = RET_OK;
    pinnum = 1 << pinnum;

    ret = HAL_PES_IOPinmuxConfig(bank, pinnum, IO_MASTER);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return ret;
    }
    return ret;
}

static RET_CODE PES_PreChannelOutputCfg(PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;
    PES_ChannelConfig_T chCfg;
    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));

    chCfg.masterSrcConfig = PES_MASTER_PESOUT;
    chCfg.masterEvtSrcConfig.pesChannelOutput = PES_OUTPUT_EVENT_FROM_CHANNEL0;
    chCfg.detectMode = PES_DET_DISABLE;
    chCfg.channelOutputInverted = FALSE;
    chCfg.cascadeConfig.andNextChannelEnable = FALSE;
    chCfg.cascadeConfig.orpreChannelEnable = FALSE;
    chCfg.syncMode = PES_ASYNC;

    ret = HAL_PES_ConfigEventChannel(&chCfg, chIndex);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return ret;
    }
    return ret;
}

static RET_CODE PES_NextChannelOutputCfg(PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;
    PES_ChannelConfig_T chCfg;
    /*master rtc : config receive master event config */
    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));

    chCfg.masterSrcConfig = PES_MASTER_PESOUT;
    chCfg.masterEvtSrcConfig.pesChannelOutput = PES_OUTPUT_EVENT_FROM_CHANNEL0;
    chCfg.detectMode = PES_DET_DISABLE;
    chCfg.channelOutputInverted = TRUE;
    chCfg.cascadeConfig.andNextChannelEnable = FALSE;
    chCfg.cascadeConfig.orpreChannelEnable = FALSE;
    chCfg.syncMode = PES_ASYNC;

    ret = HAL_PES_ConfigEventChannel(&chCfg, chIndex);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return ret;
    }
    return ret;
}

static RET_CODE PES_TestChannelOutputCfg(PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;
    PES_ChannelConfig_T chCfg;
    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));

    chCfg.masterSrcConfig = PES_MASTER_PESOUT;
    chCfg.masterEvtSrcConfig.pesChannelOutput = PES_OUTPUT_EVENT_FROM_CHANNEL0;
    chCfg.detectMode = PES_DET_DISABLE;
    chCfg.channelOutputInverted = FALSE;
    chCfg.cascadeConfig.andNextChannelEnable = FALSE;
    chCfg.cascadeConfig.orpreChannelEnable = TRUE;
    chCfg.syncMode = PES_ASYNC;

    ret = HAL_PES_ConfigEventChannel(&chCfg, chIndex);
    if (ret != RET_OK)
    {
        TC_PRINT("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
        return ret;
    }
    return ret;
}


/*goal:test IO as master trigger PES
* default: LED will be extinguished when the master IO is given a raising edge
*(when level changes from low to high)
*/
int test_pes()

{
    RET_CODE ret = RET_OK;
    TC_PRINT("Testcase: %s\n", __func__);

    PES_IOasSlaveIOCfg(SLAVE_IO_BANK, SLAVE_IO_PINNUM);
    if (ret != RET_OK)
    {
        TC_PRINT("********PES slave io config Error*******\n");

        return TC_FAIL;
    }

    PES_IOasMasterIOCfg(MASTER_IO_BANK, MASTER_IO_PINNUM);
    if (ret != RET_OK)
    {
        TC_PRINT("********PES master io config Error*******\n");

        return TC_FAIL;
    }

    PES_PreChannelOutputCfg(IO_PRE_CHANNLE);
    if (ret != RET_OK)
    {
        TC_PRINT("********PES pre channel config Error*******\n");

        return TC_FAIL;
    }

    PES_TestChannelOutputCfg(IO_TEST_CHANNLE);
    if (ret != RET_OK)
    {
        TC_PRINT("********PES test channel config Error*******\n");

        return TC_FAIL;
    }

    PES_NextChannelOutputCfg(IO_NEXT_CHANNLE);
    if (ret != RET_OK)
    {
        TC_PRINT("********PES next channel config Error*******\n");

        return TC_FAIL;
    }

    HAL_PES_ConfigIEandStatusReg(TRUE, IO_TEST_CHANNLE);
    if (ret != RET_OK)
    {
        TC_PRINT("********PES ie and statusReg config Error*******\n");

        return TC_FAIL;
    }

    HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, IO_PRE_CHANNLE);
    if (ret != RET_OK)
    {
        TC_PRINT("********PES software trigger Error*******\n");

        return TC_FAIL;
    }

    HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, IO_NEXT_CHANNLE);
    if (ret != RET_OK)
    {
        TC_PRINT("********PES software trigger Error*******\n");

        return TC_FAIL;
    }

    ret = HAL_PES_GetIrqStatus(IO_TEST_CHANNLE);

    if (ret = TRUE)
    {
        TC_PRINT("Testcase %s passed!\n", __func__);
        return TC_PASS;
    }
    TC_PRINT("pes interrupt failed\n");
    return TC_FAIL;
}

void pes_test(void)
{
    TC_PRINT("================PES Test Start=================\n\n\n");
    TC_PRINT("TEST1\n");
    zassert_true(test_pes() == TC_PASS, NULL);
    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);
    TC_PRINT("================PES Test End==================\n\n\n");
}
