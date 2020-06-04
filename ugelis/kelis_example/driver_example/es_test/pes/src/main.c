/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>

#include <device.h>
#include <gm_hal_pes.h>
#include <gm_hal_gpio.h>

//#include <gm_gpio.h>

#include <misc/printk.h>
#include "gm_hal_uart.h"


/******************************************************************************
* Global variables
******************************************************************************/
#define FPGA_PES_TEST_CASE1             (0) //CPU as master and IO as slave
#define FPGA_PES_TEST_CASE2             (0) //AND'ed with next channel
#define FPGA_PES_TEST_CASE3             (0) //OR'ed with previous channel
#define FPGA_PES_TEST_CASE4             (1) //one IO as master and another IO as slave

#define IO_TEST_CHANNLE     PES_EVENT_CHANNEL1  //channel set
#define IO_PRE_CHANNLE      PES_EVENT_CHANNEL0
#define IO_NEXT_CHANNLE     PES_EVENT_CHANNEL2

#define MASTER_IO_BANK      GPIOB
#define SLAVE_IO_BANK       GPIOA
#define MASTER_IO_PINNUM    IO_PIN_PORT_8
#define SLAVE_IO_PINNUM     IO_PIN_PORT_1

#ifndef ERR_PRINTF
    #define PR_DEBUG(...)        do{}while(0)
#else
    #define PR_DEBUG             printk
#endif


#if FPGA_PES_TEST_CASE1

/*config pes slave IO, different channel need to config different IO according to the end of PES spec*/
static int PES_IOasSlaveIOCfg(GPIO_Device_T *bank, uint32_t pinnum)
{
    RET_CODE ret = RET_OK;
    pinnum = 1 << pinnum;
    ret = HAL_PES_IOPinmuxConfig(bank, pinnum, IO_SLAVE);

    return ret;
}

/*goal:test software event function and slave IO
*PES software event to slave io, and slave io can light led up
*/
int hal_pes_test1()
{
    RET_CODE ret = RET_OK;

    /* choose one channel to test*/
    PES_ChannelIndex_T chIndex = IO_TEST_CHANNLE;
    PES_ChannelConfig_T chCfg;

    PR_DEBUG("PES software event test start\n ");

    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));
    /*config another PES channel as master, but the default event source is PES CH0, so it means no events*/
    chCfg.masterSrcConfig = PES_MASTER_PESOUT;
    chCfg.syncMode = PES_ASYNC;
    ret = HAL_PES_ConfigEventChannel(&chCfg, chIndex);
    PES_IOasSlaveIOCfg(SLAVE_IO_BANK, SLAVE_IO_PINNUM);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err:HAL_PES_ConfigEventChannel\n", __FUNCTION__);
        return ret;
    }
    while (1)
    {
        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, chIndex);
        PR_DEBUG("LED expect to be lit up!");
        k_sleep(1000);
        HAL_PES_GenerateSWTrigerEvent(PES_HIGH_LEVEL, chIndex);
        PR_DEBUG("LED expect to be extinguished!");
        k_sleep(1000);
    }
    return ret;
}

#endif



#if FPGA_PES_TEST_CASE2

static RET_CODE PES_TestChannelOutputCfg(PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;
    PES_ChannelConfig_T chCfg;
    /*master rtc : config receive master event config */
    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));
    chCfg.masterSrcConfig = PES_MASTER_PESOUT;
    chCfg.cascadeConfig.andNextChannelEnable = TRUE;
    chCfg.syncMode = PES_ASYNC;
    ret = HAL_PES_ConfigEventChannel(&chCfg, chIndex);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err:HAL_PES_ConfigEventChannel\n", __FUNCTION__);
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
    chCfg.syncMode = PES_ASYNC;
    ret = HAL_PES_ConfigEventChannel(&chCfg, chIndex);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err:HAL_PES_ConfigEventChannel\n", __FUNCTION__);
        return ret;
    }
    return ret;
}

/*goal:test channel cascade ANDNEXT function
* test channel output will 'ANDed with its next channel
*/
int hal_pes_test2()
{
    RET_CODE ret = RET_OK;

    PES_ChannelIndex_T testChIndex = IO_TEST_CHANNLE;
    PES_ChannelIndex_T nextChIndex = IO_NEXT_CHANNLE;

    PR_DEBUG("PES Cascade Output & With Next Channel PES Out\n");

    ret = PES_TestChannelOutputCfg(testChIndex);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err:PES_TestChannelOutputCfg\n", __FUNCTION__);
        goto out;
    }

    ret = PES_NextChannelOutputCfg(nextChIndex);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err:PES_NextChannelOutputCfg\n", __FUNCTION__);
        goto out;
    }
    while (1)
    {

        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, testChIndex);
        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, nextChIndex);
        PR_DEBUG("Expecet Level is PES_LOW_LEVEL\n");
        k_sleep(5000);

        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, testChIndex);
        HAL_PES_GenerateSWTrigerEvent(PES_HIGH_LEVEL, nextChIndex);
        PR_DEBUG("Expecet Level is PES_LOW_LEVEL\n");

        k_sleep(5000);

        HAL_PES_GenerateSWTrigerEvent(PES_HIGH_LEVEL, testChIndex);
        HAL_PES_GenerateSWTrigerEvent(PES_HIGH_LEVEL, nextChIndex);
        PR_DEBUG("Expecet Level is PES_HIGH_LEVEL\n");
        k_sleep(5000);

        HAL_PES_GenerateSWTrigerEvent(PES_HIGH_LEVEL, testChIndex);
        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, nextChIndex);
        PR_DEBUG("Expecet Level is PES_LOW_LEVEL\n");
        k_sleep(5000);
    }
out:
    return ret;
}

#endif



#if FPGA_PES_TEST_CASE3

static RET_CODE PES_TestChannelOutputCfg(PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;
    PES_ChannelConfig_T chCfg;
    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));
    chCfg.masterSrcConfig = PES_MASTER_PESOUT;
    chCfg.cascadeConfig.orpreChannelEnable = TRUE;
    chCfg.syncMode = PES_ASYNC;
    ret = HAL_PES_ConfigEventChannel(&chCfg, chIndex);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err:HAL_PES_ConfigEventChannel\n", __FUNCTION__);
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
    chCfg.syncMode = PES_ASYNC;
    ret = HAL_PES_ConfigEventChannel(&chCfg, chIndex);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err:HAL_PES_ConfigEventChannel\n", __FUNCTION__);
        return ret;
    }

    return ret;
}

/*goal:test channel cascade ORPRE function
* test channel output will 'ORed with its previous channel
*/
int hal_pes_test3()
{
    RET_CODE ret = RET_OK;

    PES_ChannelIndex_T testChIndex = IO_TEST_CHANNLE;
    PES_ChannelIndex_T preChIndex  = IO_PRE_CHANNLE;

    printf("FPGA_PES_TEST_CASE3\n ");
    printf("PES Cascade Output & With Next Channel PES Out\n");

    ret = PES_TestChannelOutputCfg(testChIndex);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err:HAL_PES_TestChannelOutputCfg\n", __FUNCTION__);
        goto out;
    }

    ret = PES_PreChannelOutputCfg(preChIndex);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err:HAL_PES_NextChannelOutputCfg\n", __FUNCTION__);
        goto out;
    }

    while (1)
    {

        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, testChIndex);
        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, preChIndex);
        PR_DEBUG("\nExpecet Level is PES_LOW_LEVEL\n");
        k_sleep(5000);

        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, testChIndex);
        HAL_PES_GenerateSWTrigerEvent(PES_HIGH_LEVEL, preChIndex);
        PR_DEBUG("\nExpecet Level is PES_HIGH_LEVEL\n");
        k_sleep(5000);

        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, testChIndex);
        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, preChIndex);
        PR_DEBUG("\nExpecet Level is PES_LOW_LEVEL\n");
        k_sleep(5000);

        HAL_PES_GenerateSWTrigerEvent(PES_HIGH_LEVEL, testChIndex);
        HAL_PES_GenerateSWTrigerEvent(PES_HIGH_LEVEL, preChIndex);
        PR_DEBUG("\nExpecet Level is PES_HIGH_LEVEL\n");
        k_sleep(5000);

        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, testChIndex);
        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, preChIndex);
        PR_DEBUG("\nExpecet Level is PES_LOW_LEVEL\n");
        k_sleep(5000);

        HAL_PES_GenerateSWTrigerEvent(PES_HIGH_LEVEL, testChIndex);
        HAL_PES_GenerateSWTrigerEvent(PES_LOW_LEVEL, preChIndex);
        PR_DEBUG("\nExpecet Level is PES_HIGH_LEVEL\n");
        k_sleep(5000);
    }
out:
    return ret;
}

#endif


#if FPGA_PES_TEST_CASE4

/*config IO as PES slave, GPIO bank and pin num according to different channel
    This IO can receive PES event, and light up LED
*/
static int PES_IOasSlaveIOCfg(GPIO_Device_T *bank, uint32_t pinnum)
{
    RET_CODE ret = RET_OK;
    pinnum = 1 << pinnum;
    ret = HAL_PES_IOPinmuxConfig(bank, pinnum, IO_SLAVE);

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

    return ret;
}

static int PES_Cfg()
{
    RET_CODE ret = RET_OK;
    PES_ChannelConfig_T chCfg;
    PES_ChannelIndex_T chIndex;

    chIndex = IO_TEST_CHANNLE;

    chCfg.syncMode = PES_ASYNC;
    chCfg.masterSrcConfig = PES_MASTER_IO;  //select IO as master
    chCfg.masterEvtSrcConfig.ioPinOutput = MASTER_IO_PINNUM;
    chCfg.cascadeConfig.andNextChannelEnable = FALSE;
    chCfg.cascadeConfig.orpreChannelEnable = FALSE;
    chCfg.channelOutputInverted = FALSE;
    chCfg.detectMode = PES_RISING_EDGE;

    ret = HAL_PES_ConfigEventChannel(&chCfg, chIndex);
    HAL_PES_ConfigIEandStatusReg(TRUE, chIndex);

    return ret;
}

/*goal:test IO as master trigger PES
* default: LED will be extinguished when the master IO is given a raising edge
*(when level changes from low to high)
*/
int hal_pes_test4()

{
    RET_CODE ret = RET_OK;
    PR_DEBUG("(PES example start)\n");
    ret = PES_IOasSlaveIOCfg(SLAVE_IO_BANK, SLAVE_IO_PINNUM);
    if (ret)
    {
        PR_DEBUG("(config master IO failed)\n");
        return FALSE;
    }
    PES_IOasMasterIOCfg(MASTER_IO_BANK, MASTER_IO_PINNUM);
    if (ret)
    {
        PR_DEBUG("(config slave IO failed)\n");
        return FALSE;
    }
    PES_Cfg();
    PR_DEBUG("(need to change the master IO level)\n");
    while (1)
    {

    }
}

#endif



void hal_pes_test()
{
#if FPGA_PES_TEST_CASE1
    hal_pes_test1();
#endif

#if FPGA_PES_TEST_CASE2
    hal_pes_test2();
#endif

#if FPGA_PES_TEST_CASE3
    hal_pes_test3();
#endif

#if FPGA_PES_TEST_CASE4
    hal_pes_test4();
#endif

}

/* PES Requires a callback, there is no interrupt enable / disable. */
void pes_callback(struct device *dev)
{
    printf("pes fired\n");
}

void main(void)
{
    hal_pes_test();
}

