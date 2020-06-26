/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          pes_demo.c
 *
 * @author        Christal.Xu
 *
 * @version       1.0.0
 *
 * @date          2019/04/20
 *
 * @brief         PES Demo Calls PES Driver
 *
 * @note
 *    2019/04/20, Christal.Xu, V1.0.0
 *        Initial version.
 */

/*NOTE:
You need to choose two GPIO, one as slave with LED, another as master with pin
When pes and GPIO are configured, you need to change the level of master IO from low to high
and the led of slave IO will changed from on to off
The level and LED change depend on the PES configuration
*/

#include <gm_hal_pes.h>
#include <gm_hal_gpio.h>

#define IO_TEST_CHANNLE     PES_EVENT_CHANNEL1  //channel set

#define MASTER_IO_BANK      GPIOB
#define SLAVE_IO_BANK       GPIOA
#define MASTER_IO_PINNUM    IO_PIN_PORT_8
#define SLAVE_IO_PINNUM     IO_PIN_PORT_1

#define DBG_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printk
#endif

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

int main(void)
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
        ;
    }
}
