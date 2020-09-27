/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/util.h>
#include "board.h"
#include <misc/printk.h>
#include <gm_soc.h>
#include <gm_hal_rtc.h>
#include <gm_hal_dma.h>
#include <watchdog.h>
#include <gm_hal_gpio.h>
#include <pinmux.h>
#include <device.h>
#include <gpio.h>

#include "pm_system.h"
#include <gm_hal_clock.h>
#include "pm_wakeup_src.h"



typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;




/*dma marco*/
//#define BLOCK_SIZE                              ((1024*1024)/4)
#define BLOCK_SIZE                              ((1024*1024*2)/4)

#define RX_BUFF_SIZE                            (BLOCK_SIZE*4)



/*irq generate flag*/
unsigned int IntTaken = 0x123;

/*dma define */
static DMA_Handle_T s_dmaHandle;
static volatile  uint32_t s_dmaComplete = 0;
static volatile  uint32_t s_dmaError = 0;
static char src_data[RX_BUFF_SIZE] = { 0 };
static char dst_data[RX_BUFF_SIZE] = { 0 };

#ifdef PM_SWITECH_TEST
    /*rand val*/
    int (*pFunPmGoOn[PM_SWITCH_FUN_NUM])(FCLK_SourceType_T);
    int (*pFunPmRest[PM_SWITCH_FUN_NUM])();
#endif


static void clk_hirc_trim();
void clk_swith_internal_hirc_8M();
static void clk_swith_pll_200M();
static void clk_swith_pllkdp_150M();
static void pm_plls_hosc_losc_en_config(uint8_t en);
static RET_CODE pm_check_sysclk_src_status();
static RET_CODE  pm_check_irq_generate_wakeup();
static void dmaCallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param);
static int pm_dma_config();
static void pm_dma_start();
static RET_CODE pm_check_dma_process_result();
static RET_CODE pm_xrstj_config();
static RET_CODE pm_iwdg_config();
static void rtcCallback(int type);
static RET_CODE pm_rtc_config();
static RET_CODE pm_wkpin_config(WAKE_PIN_TYPE_T pin);
static void pm_pes_config();
static void pm_enter_shutdown();
static void pm_enter_standby();
static void  pm_enter_sleep_mode(void);
static void pm_enter_stop1_mode();
static void pm_enter_stop2_mode();

















/***********************clk imple*************************************/
static void clk_hirc_trim()
{
    /*todo*/
}

/*switch hirc*/
void clk_swith_internal_hirc_8M()
{
    CLOCK_SysClockConfig_T SysClockConfigStructure;

    clk_hirc_trim();

    SysClockConfigStructure.sysClkPrescaler = CLOCK_DIVIDED_1;
    SysClockConfigStructure.sysClkSource    =  SYSCLOCK_SOURCE_HIRC8M;
    HAL_CLOCK_SYSCLKConfig(&SysClockConfigStructure);
}

/*open pll and switch , pll source is hirc*/
static void clk_swith_pll_200M()
{
    CLOCK_SysClockConfig_T SysClockConfigStructure;

    clk_hirc_trim();

    SysClockConfigStructure.sysClkPrescaler =  CLOCK_DIVIDED_1;
    SysClockConfigStructure.sysClkSource    =  SYSCLOCK_SOURCE_PLL;

    HAL_CLOCK_SYSCLKConfig(&SysClockConfigStructure);
}

static void clk_swith_pllkdp_150M()
{
    CLOCK_SysClockConfig_T SysClockConfigStructure;

    clk_hirc_trim();

    SysClockConfigStructure.sysClkPrescaler =  CLOCK_DIVIDED_1;
    SysClockConfigStructure.sysClkSource    =  SYSCLOCK_SOURCE_PLLKDP200;

    HAL_CLOCK_SYSCLKConfig(&SysClockConfigStructure);
}

static void pm_plls_hosc_losc_en_config(uint8_t en)
{
    /* plls and hosc and losc enable */

    HAL_CLOCK_PLLcmd(en);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE));
    HAL_CLOCK_PLLI2Scmd(en);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE));
    HAL_CLOCK_PLLKDPcmd(en);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE));
    HAL_CLOCK_PLLSAIcmd(en);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE));

    /*open extern clk*/
#ifdef   HOSC_ENABLE
    HAL_CLOCK_HOSCcmd(en);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE));
#endif
#ifdef   LOCS_ENABLE
    HAL_CLOCK_LOSCcmd(en);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_LOSC_STABLE));
#endif
}




static RET_CODE pm_check_sysclk_src_status_same_as_before(FCLK_SourceType_T type)
{
    uint32_t data = 0;
    RET_CODE ret = RET_OK ;

    data = get_fclk_source();
    if (type == FCLK_SOURCE_HIRC)
    {
        if (data != 0)
        {
            PR_ERR("!!Err:Resume clk is not hirc  Srcdata:%d\n", data);
            ret = RET_ERROR;
        }
    }
    else
    {
        if (data != 2)
        {
            PR_ERR("!!Err:Resume clk is not kdpdpll 150Mhz  Srcdata:%d\n", data);
            ret = RET_ERROR;
        }

    }

    return ret;
}




static RET_CODE pm_check_sysclk_src_status()
{
    uint32_t data = 0;
    RET_CODE ret = RET_OK;

    data = get_fclk_source();
    if (data != 0x00)
    {
        PR_ERR("Error!!!, get_fclk_source = 0x%x\n", data);
        ret = RET_ERROR;
        goto out;
    }

#ifdef   HOSC_ENABLE
    if (get_hosc_enable_bit() == 0x01)
    {
        PR_ERR("Error!!!, HOSC ON\n");
        ret = RET_ERROR;
        goto out;
    }
#endif
#ifdef   LOCS_ENABLE
    if (get_losc_enable_bit() == 0x00)
    {
        PR_ERR("Error!!!, LOSC OFF\n");
        ret = RET_ERROR;
        goto out;
    }
#endif
    if (get_lirc_enable_bit() == 0x00)
    {
        PR_ERR("Error!!!, LIRC OFF\n");
        ret = RET_ERROR;
        goto out;
    }

    if (get_pll_enable_bit() == 0x01)
    {
        PR_ERR("Error!!!, PLL ON\n");
        ret = RET_ERROR;
        goto out;
    }
    if (get_pllkdp_enable_bit() == 0x01)
    {
        PR_ERR("Error!!!, PLLKDP ON\n");
        ret = RET_ERROR;
        goto out;
    }
    if (get_plli2s_enable_bit() == 0x01)
    {
        PR_ERR("Error!!!, PLLI2S ON\n");
        ret = RET_ERROR;
        goto out;
    }
    if (get_pllsai_enable_bit() == 0x01)
    {
        PR_ERR("Error!!!, PLLSAI ON\n");
        ret = RET_ERROR;
        goto out;
    }
out:
    return ret;

}


/***********************clk imp**************************************/








/***********************irq generate test****************************/

static RET_CODE  pm_check_irq_generate_wakeup()
{
    RET_CODE ret = RET_OK;
    if (IntTaken != 0x321)
    {
        PR_ERR("Error!!!, IntTaken = 0x%x\n", IntTaken);
        return RET_ERROR;
    }

    return ret;
}

/***********************irq generate test****************************/








/***********************dma as wakeup src config imp******************************/

static void dmaCallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        /* DMA transaction completes */
        s_dmaComplete = 1;
    }
    else if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        /* Error occurs during DMA transaction */
        s_dmaError = 1;
    }
    IntTaken = 0x321;
    PR_DEBUG("\nEnter dmaCallback()\n");
}


/*m to m*/
static int pm_dma_config()
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_ERROR;
    DMA_Handle_T *pDMAHandle = &s_dmaHandle;
    uint32_t i = 0;

    memset(src_data, RX_BUFF_SIZE, 0);
    memset(dst_data, RX_BUFF_SIZE, 0);
    for (i = 0; i < RX_BUFF_SIZE; i++)
    {
        src_data[i] = i;
    }

    ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_M2M, &reqResult);
    if (ret != RET_OK)
    {
        return ret;
    }

    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx   = reqResult.channelIdx;
    dmaConfig.deviceIdx    = reqResult.deviceIdx;
    dmaConfig.direction    = DMA_MEMORY_TO_MEMORY;
    dmaConfig.increment    = DMA_SRC_INC_DST_INC;
    dmaConfig.width        = DMA_WIDTH_32BIT;

    dmaConfig.priority     = DMA_PRIORITY_LOW;
    dmaConfig.burst        = 0;
    dmaConfig.alter        = DMA_PRIMARY;
    dmaConfig.callback     = dmaCallback;

    HAL_DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);

}


static void pm_dma_start()
{
    HAL_DMA_Start(&s_dmaHandle, (uint32_t)src_data, (uint32_t)dst_data, BLOCK_SIZE);
}

static RET_CODE pm_check_dma_process_result()
{

    RET_CODE ret = RET_OK;
    uint32_t compare_size = 0;
    /* Wait till either DMA completes or error occures */
    while ((s_dmaComplete == 0) && (s_dmaError == 0)) ;

    compare_size = RX_BUFF_SIZE;

    if (s_dmaComplete == 1)
    {
        if (memcmp(src_data, dst_data, compare_size) != 0)
        {
            PR_ERR("ERROR: dma data error\n");
            return RET_ERROR;
        }
    }

    HAL_DMA_Abort(&s_dmaHandle);
    return ret;

}

/***********************dma as wakeup src config imp******************************/



/***********************xrstj as wakeup src config imp******************************/
static RET_CODE pm_xrstj_config()
{
    RET_CODE ret = RET_OK;

    return ret;
}
/***********************xrstj as wakeup src config imp******************************/





/***********************iwdg as wakeup src config imp******************************/

static RET_CODE pm_iwdg_config()
{
    RET_CODE ret = RET_OK;

    struct wdt_config wr_cfg;
    struct device *wdt_dev;

    wdt_dev = device_get_binding("IWDG");
    if (!wdt_dev)
    {
        printk("Cann't get GM-IWDG Device\n");
    }

    /*5s later reset*/
    wr_cfg.timeout = 5000;
    wr_cfg.mode = WDT_MODE_RESET;
    wr_cfg.clkdiv = IWDG_CLKDIV_FACTOR_256;
    wr_cfg.interrupt_fn = NULL;

    wdt_set_config(wdt_dev, &wr_cfg);
    wdt_get_config(wdt_dev, &wr_cfg);

    wdt_enable(wdt_dev);

    return ret;
}
/***********************iwdg as wakeup src config imp******************************/







/***********************rtc as wakeup src config imp******************************/

static void rtcCallback(int type)
{
    type = type;
    IntTaken = 0x321;
    PR_DEBUG("\n####Enter rtcCallback()######\n");

}


static RET_CODE pm_rtc_config()
{
    RET_CODE ret = RET_OK;

    UTCTime_T utcTime;
    HAL_RTC_Config_T rtcConfig;

#define YEAR    2017
#define MON     5
#define DAY     11
#define HOUR    14
#define MINUTE  7
#define SEC     0

    SysTick_Enable();

    rtcConfig.alarmCallback = rtcCallback;
    rtcConfig.clock         = HAL_RTC_CLOCK_LIRC;

    HAL_RTC_Init(&rtcConfig);

    /* RTC works now */
    HAL_RTC_Start();

    HAL_RTC_SetDateTime(YEAR, MON, DAY, HOUR, MINUTE, SEC);

    memset(&utcTime, 0, sizeof(UTCTime_T));
    utcTime.year = YEAR;
    utcTime.mon  = MON;
    utcTime.day  = DAY;
    utcTime.hour = HOUR;
    utcTime.min  = MINUTE;
    utcTime.sec  = SEC + 3; /* ensure sec < 60 */

    HAL_RTC_SetAlarm(&utcTime);


    SysTick_Disable();

    return ret;

}

/***********************rtc as wakeup src config imp******************************/




/***********************wkpin as wakeup src config imp******************************/


//gpio input
/*default level*/
/* xpa0 :0*/
/* xpa7 : */
/* xpa8 :*/



#define XPA0_INPUT_GPIO_PIN   0
#define XPA7_INPUT_GPIO_PIN   7
#define XPA8_INPUT_GPIO_PIN   8

static void wkpin_pinmux_cfg(WAKE_PIN_TYPE_T type)
{

    int val ;
    int i = 0;
    struct device *dev;
    struct device *gpiob;
    GPIO_PinConfig_T GPIO_InitStruct;

    dev = device_get_binding("GPIOA");
    gpiob = dev;
    if (type == WKPIN_XPA0)
    {
        gpio_pin_configure(dev, XPA0_INPUT_GPIO_PIN, GPIO_MODE_INPUT);
        gpio_pin_read(dev, XPA0_INPUT_GPIO_PIN, &val);
        printk("XPA0 val = [%d]\n", val);
    }
    else if (type == WKPIN_XPA7)
    {
        gpio_pin_configure(dev, XPA7_INPUT_GPIO_PIN, GPIO_MODE_INPUT);
        gpio_pin_read(dev, XPA7_INPUT_GPIO_PIN, &val);
        printk("XPA7 val = [%d]\n", val);

    }
    else if (type == WKPIN_XPA8)
    {
        gpio_pin_configure(dev, XPA8_INPUT_GPIO_PIN, GPIO_MODE_INPUT);
        gpio_pin_read(dev, XPA8_INPUT_GPIO_PIN, &val);
        printk("XPA8 val = [%d]\n", val);
    }

}



//The valid wake up is falling edge
void wkup_pin_config_A0_f(void)
{
    u32 data;

    data = readl(PWR_MOD_REG);
    data = data | (0x01 << 19) | (0x01 << 18);
    writel(data, PWR_MOD_REG);
    writel(0x01 << 26, RST_STATUS_REG);
}

//The valid wake up is rising edge
void wkup_pin_config_A0_r(void)
{
    u32 data;

    data = readl(PWR_MOD_REG);
    data = data | (0x01 << 19);
    writel(data, PWR_MOD_REG);
    writel(0x01 << 26, RST_STATUS_REG);
}






static RET_CODE pm_wkpin_config(WAKE_PIN_TYPE_T type)
{
    RET_CODE ret = RET_OK;
    u32 data;

    wkpin_pinmux_cfg(type);

    if (type == WKPIN_XPA0)
    {
        wkup_pin_config_A0_r();

    }
    else if (type == WKPIN_XPA7)
    {
        lirc_enable();
        while (lirc_lock_flag() == 0x00);
        backup_reg_unlock();
        writel(0x1ACCE551, RTC_BASE + 0x40);//unlock register


        writel(0x01, RTC_BASE + 0x58);  //bit[0] enable; bit[1] edge(0:rise; 1:fall)
        writel(0x101, RTC_BASE + 0x58);//bit[8] clear pending bit



    }
    else
    {
        lirc_enable();
        while (lirc_lock_flag() == 0x00);
        backup_reg_unlock();
        writel(0x1ACCE551, RTC_BASE + 0x40);//unlock register

        writel(0x18, RTC_BASE + 0x58);  //bit[3] enable; bit[4] edge(0:rise; 1:fall)
        writel(0x218, RTC_BASE + 0x58); //bit[9] clear pending bit


    }

    return ret;

}


static void pm_pmu_irq_Hander()
{

    IntTaken = 0x321;
    printk("\npm_pmu_irq_Hander\n");

    if (readl(RST_STATUS_REG) & (IO_WK_PENDING))
    {
        printk("\nEnter PMU Handler:IO_WK_PENDING\n");
        reset_pend_clear(IO_WK_PENDING);
    }
    if (readl(RST_STATUS_REG) & (PES_WK_PENDING))
    {
        printk("\nEnter PMU Handler:PES_WK_PENDING\n");
        reset_pend_clear(PES_WK_PENDING);
    }
    if (readl(RST_STATUS_REG) & (HOSC_LOSS_PENDING))
    {
        printk("\nEnter PMU Handler:HOSC_LOSS_PENDING\n");
        reset_pend_clear(PES_WK_PENDING);
    }

}



static void pm_pmu_irq_config()
{

    HAL_RequestIrq(FMC_IRQn, 3, pm_pmu_irq_Hander, 0);

    HAL_IRQ_ENABLE(FMC_IRQn);

}


/***********************pes as wakeup src config imp******************************/

void pes_gpio_config_A0(void)
{
    u32 data;

    data = readl(PWR_MOD_REG);
    /*fall edge to wkup*/
    data = data | (0x01 << 17);
    writel(data, PWR_MOD_REG);
    writel(0x03 | (0x01 << 31) | (0x01 << 16), PES_BASE + 0x20);
    writel(0x01 << 25, RST_STATUS_REG);
    \



}


static void pm_pes_config()
{
    pes_gpio_config_A0();
}

/***********************pes as wakeup src config imp******************************/



/***********************enter power mode imp******************************/



static void pm_enter_shutdown()
{
    pm_pmu_irq_config();
    goto_shut_down_mode();

}

static void pm_enter_standby()
{
    pm_pmu_irq_config();
    goto_standby_mode();

}

static void  pm_enter_sleep_mode(void)
{
    goto_sleep_mode();

}

static void pm_enter_stop1_mode()
{


    goto_stop1_mode();
}


static void pm_enter_stop2_mode()
{
    pm_pmu_irq_config();

    goto_stop2_mode();
}



/***********************enter power mode imp******************************/

/***********************sleep mode imp***********************************/
RET_CODE  pm_sleepmode_dma_wakeup(FCLK_SourceType_T type)
{
    RET_CODE ret = RET_OK;
    uint32_t data;

    /*switch 8M hirc clk*/
    if (type == FCLK_SOURCE_HIRC)
    {
        clk_swith_internal_hirc_8M();
    }
    else
    {
        clk_swith_pllkdp_150M();
    }

    /* conifg LDO to low power mode */

    pm_dma_config();
    pm_dma_start();

    PR_DEBUG("##pm_enter_sleep_mode: wakeup will go on##\n");
    /*enter sleep mode*/
    pm_enter_sleep_mode();

    PR_DEBUG("resume from sleep mode test  \n");

    /*check result*/
    ret = pm_check_irq_generate_wakeup();
    if (ret != RET_OK)
    {
        PR_ERR("Error!!!,pm_check_irq_generate_wakeup not generate\n");
        goto out;
    }

    ret = pm_check_sysclk_src_status_same_as_before(type);
    if (ret)
    {
        goto out;
    }

    ret = pm_check_dma_process_result();
    if (ret != RET_OK)
    {
        PR_ERR("pm_check_dma_process_result fail\n");
    }

out:
    return ret;

}




RET_CODE  pm_sleepmode_rtc_wakeup(FCLK_SourceType_T type)
{
    RET_CODE ret = RET_OK;
    uint32_t data;



    /*switch 8M hirc clk*/
    if (type == FCLK_SOURCE_HIRC)
    {
        clk_swith_internal_hirc_8M();
    }
    else
    {
        clk_swith_pllkdp_150M();
    }

    /* config dma wakeupo source */
    pm_rtc_config();

    PR_DEBUG("##sys enter rtc sleep mode: wakeup go on##\n");


    /*enter sleep mode*/
    pm_enter_sleep_mode();

    PR_DEBUG("resume rtc from sleep mode test  \n");

    /*check irq result*/
    ret = pm_check_irq_generate_wakeup();
    if (ret != RET_OK)
    {
        PR_ERR("pm_check_irq_generate_wakeup not generate\n");
    }
    /*check resume clk*/

    ret = pm_check_sysclk_src_status_same_as_before(type);

    return ret;

}
/***********************sleep mode imp******************************/




/***********************stop1 mode imp******************************/
RET_CODE  pm_stop1_rtc_wakeup(FCLK_SourceType_T type)
{
    RET_CODE ret = RET_OK;
    /*switch 8M hirc clk*/
    if (type == FCLK_SOURCE_HIRC)
    {
        clk_swith_internal_hirc_8M();
    }
    else
    {
        clk_swith_pllkdp_150M();
    }
    /* config dma wakeupo source */
    pm_rtc_config();

    PR_DEBUG("##sys rtc enter stop1: wakeup go on##\n");
    /*enter stop1  mode*/
    pm_enter_stop1_mode();

    PR_DEBUG("resume rtc from stop1 mode test  \n");

    ret = pm_check_sysclk_src_status_same_as_before(type);
    if (ret)
    {
        goto out;
    }


    /*check result*/
    ret = pm_check_irq_generate_wakeup();
    if (ret != RET_OK)
    {
        PR_ERR("pm_check_irq_generate_wakeup not generate\n");
    }

out:
    return ret;

}

/***********************stop1 mode imp******************************/




/***********************stop2 mode imp******************************/
RET_CODE  pm_stop2_wkpin_wakeup(FCLK_SourceType_T type)
{
    RET_CODE ret = RET_OK;
    /*switch 8M hirc clk*/
    if (type == FCLK_SOURCE_HIRC)
    {
        clk_swith_internal_hirc_8M();
    }
    else
    {
        clk_swith_pllkdp_150M();
    }

    /* config dma wakeupo source */
    pm_wkpin_config(WKPIN_XPA0);

    /* plls and hosc and losc enable */
    pm_plls_hosc_losc_en_config(ENABLE);

    PR_DEBUG("##sys xpa0 enter stop2: wakeup go on##\n");
    /*enter stop2  mode*/
    pm_enter_stop2_mode();

    PR_DEBUG("resume xpa0 from sleep mode test  \n");
    PR_DEBUG("###system clock is HIRC after wake up and HOSC and PLLs clocks are disable##\n");

    /*check result*/
    ret = pm_check_sysclk_src_status();
    if (ret != RET_OK)
    {
        PR_ERR("pm_check_sysclk_src_status:Fail\n");
    }



    return ret;

}


RET_CODE  pm_stop2_wkpin_uninterrupt_wakeup(FCLK_SourceType_T type)
{
    RET_CODE ret = RET_OK;
    /*switch 8M hirc clk*/
    if (type == FCLK_SOURCE_HIRC)
    {
        clk_swith_internal_hirc_8M();
    }
    else
    {
        clk_swith_pllkdp_150M();
    }

    /* config dma wakeupo source */
    pm_wkpin_uninterrupt_config(WKPIN_XPA0);

    /* plls and hosc and losc enable */
    pm_plls_hosc_losc_en_config(ENABLE);

    PR_DEBUG("##sys enter stop2: wakeup go on##\n");
    /*enter stop2  mode*/
    pm_enter_stop2_mode();

    PR_DEBUG("resume from sleep mode test  \n");
    PR_DEBUG("###system clock is HIRC after wake up and HOSC and PLLs clocks are disable##\n");

    /*check result*/
    ret = pm_check_sysclk_src_status();
    if (ret != RET_OK)
    {
        PR_ERR("pm_check_sysclk_src_status:Fail\n");
    }

    return ret;

}


RET_CODE  pm_stop2_rtc_wakeup(FCLK_SourceType_T type)
{
    RET_CODE ret = RET_OK;
    /*switch 8M hirc clk*/


    if (type == FCLK_SOURCE_HIRC)
    {
        clk_swith_internal_hirc_8M();
    }
    else
    {
        clk_swith_pllkdp_150M();
    }

    /* config dma wakeupo source */
    pm_rtc_config();

    /* plls and hosc and losc enable */
    pm_plls_hosc_losc_en_config(ENABLE);

    PR_DEBUG("##sys rtc enter stop2: wakeup go on##\n");
    /*enter stop2  mode*/
    pm_enter_stop2_mode();

    PR_DEBUG("resume rtc from sleep mode test  \n");
    PR_DEBUG("###system clock is HIRC after wake up and HOSC and PLLs clocks are disable##\n");

    /*check result*/
    ret = pm_check_sysclk_src_status();
    if (ret != RET_OK)
    {
        PR_ERR("pm_check_sysclk_src_status:Fail\n");
    }

    return ret;

}


RET_CODE  pm_stop2_pes_wakeup(FCLK_SourceType_T type)
{
    RET_CODE ret = RET_OK;
    /*switch 8M hirc clk*/
    if (type == FCLK_SOURCE_HIRC)
    {
        clk_swith_internal_hirc_8M();
    }
    else
    {
        clk_swith_pllkdp_150M();
    }
    /* config dma wakeupo source */
    pm_pes_config();

    /* plls and hosc and losc enable */
    pm_plls_hosc_losc_en_config(ENABLE);

    PR_DEBUG("##sys enter stop2: wakeup go on##\n");
    /*enter stop2  mode*/
    pm_enter_stop2_mode();

    PR_DEBUG("resume from sleep mode test  \n");
    PR_DEBUG("###system clock is HIRC after wake up and HOSC and PLLs clocks are disable##\n");

    /*check result*/
    ret = pm_check_sysclk_src_status();
    if (ret != RET_OK)
    {
        PR_ERR("pm_check_sysclk_src_status:Fail\n");
    }

    return ret;

}

/***********************stop2 mode imp******************************/



/***********************standby mode imp******************************/

RET_CODE pm_standby_wkpin_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    data = readl(RST_STATUS_REG);

    if ((data & RESET_BY_POR) == RESET_BY_POR)
    {

        writel(RESET_BY_POR | RESET_BY_IWDG, RST_STATUS_REG);


        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);
        /* config xpa0 wkpin*/
        pm_wkpin_config(WKPIN_XPA0);


        PR_DEBUG("##sys XPA0 enter standby: wakeup will reboot##\n");
        /* enter stanby*/
        pm_enter_standby();

    }
    else if ((data & RESET_BY_STANDBY) == RESET_BY_STANDBY)
    {
        writel(RESET_BY_STANDBY | FROM_STANDBY_MODE, RST_STATUS_REG);
        PR_DEBUG("Reboot:XPA0 pm_standby_wkpin_wakeup\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}


RET_CODE pm_standby_wkpin_untinterrupt_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    data = readl(RST_STATUS_REG);

    if ((data & RESET_BY_POR) == RESET_BY_POR)
    {

        writel(RESET_BY_POR | RESET_BY_IWDG, RST_STATUS_REG);

        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);
        /* clk change to pllkdp 200M*/
        clk_swith_pllkdp_150M();
        /* config xpa0 wkpin*/
        pm_wkpin_uninterrupt_config(WKPIN_XPA0);

        PR_DEBUG("##sys enter standby: wakeup will reboot##\n");
        /* enter stanby*/
        pm_enter_standby();

    }
    else if ((data & RESET_BY_STANDBY) == RESET_BY_STANDBY)
    {
        writel(RESET_BY_STANDBY | FROM_STANDBY_MODE, RST_STATUS_REG);
        PR_DEBUG("Reboot:pm_standby_wkpin_wakeup\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}




uint8_t rest_pm_mode()
{
    uint32_t reset_status;

    reset_status = readl(RST_STATUS_REG);

    if ((reset_status & RESET_BY_STANDBY) || (reset_status & FROM_SHUT_DOWN_MODE))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}





RET_CODE pm_standby_rtc_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    SysTick_Enable();

    data = readl(RST_STATUS_REG);


    if ((data & RESET_BY_STANDBY) != RESET_BY_STANDBY)
    {
        writel(RESET_BY_POR | RESET_BY_IWDG, RST_STATUS_REG);
        /**/
        ldo_hw_trim_enable();
        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);

        /* rtc*/
        pm_rtc_config();

        PR_DEBUG("##sys rtc enter standby: wakeup will reboot##\n");
        /* enter stanby*/
        pm_enter_standby();


    }
    else if ((data & RESET_BY_STANDBY) == RESET_BY_STANDBY)
    {
        writel(RESET_BY_STANDBY | FROM_STANDBY_MODE, RST_STATUS_REG);
        PR_DEBUG("Reboot:rtc pm_standby_rtc_wakeup\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}


RET_CODE pm_standby_iwdg_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    data = readl(RST_STATUS_REG);

    if ((data & RESET_BY_POR) == RESET_BY_POR)
    {

        writel(RESET_BY_POR | RESET_BY_IWDG, RST_STATUS_REG);

        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);

        /* iwdg config wakeup*/
        pm_iwdg_config();

        PR_DEBUG("##sys enter iwdg standby: wakeup will reboot##\n");
        /* enter stanby*/
        pm_enter_standby();

    }
    else if ((data & FROM_STANDBY_MODE) == FROM_STANDBY_MODE)
    {
        writel(FROM_STANDBY_MODE | RESET_BY_IWDG, RST_STATUS_REG);
        PR_DEBUG("Reboot:iwdg pm_standby_iwdg_wakeup\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}


RET_CODE pm_standby_xrstj_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    data = readl(RST_STATUS_REG);

    if ((data & RESET_BY_POR) == RESET_BY_POR)
    {

        writel(RESET_BY_POR, RST_STATUS_REG);

        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);
        PR_DEBUG("##sys reset key enter standby: wakeup will reboot##\n");
        /* enter stanby*/
        pm_enter_standby();

    }
    else if ((data & (FROM_STANDBY_MODE)) == (FROM_STANDBY_MODE))
    {
        writel(FROM_STANDBY_MODE, RST_STATUS_REG);
        PR_DEBUG("Reboot:reset key pm_standby_xrstj_wakeup\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}


/***********************standby mode imp******************************/



/***********************shutdown mode imp******************************/


RET_CODE pm_shutdown_wkpinA7_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    data = readl(RST_STATUS_REG);

    if (((data & FROM_SHUT_DOWN_MODE) != FROM_SHUT_DOWN_MODE) && ((data & RESET_BY_STANDBY) != RESET_BY_STANDBY))
    {

        writel(RESET_BY_POR | RESET_BY_IWDG, RST_STATUS_REG);

        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);
        /* Xp config wakeup*/
        pm_wkpin_config(WKPIN_XPA7);

        PR_DEBUG("##sys enter shutdown: XPA7 wakeup will reboot##\n");
        /* enter shutdown mode*/
        pm_enter_shutdown();

    }
    else
    {
        writel(RESET_BY_STANDBY | FROM_SHUT_DOWN_MODE, RST_STATUS_REG);
        PR_DEBUG("Reboot:XPA7 pm_standby_wkpin_wakeup\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}

RET_CODE pm_shutdown_wkpinA7_uninterrupt_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    data = readl(RST_STATUS_REG);

    if ((data & FROM_SHUT_DOWN_MODE) != FROM_SHUT_DOWN_MODE)
    {

        writel(RESET_BY_POR | RESET_BY_IWDG, RST_STATUS_REG);

        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);
        /* clk change to pll 200M*/
        clk_swith_pllkdp_150M();
        /* Xp config wakeup*/
        pm_wkpin_uninterrupt_config(WKPIN_XPA7);

        PR_DEBUG("##sys enter shutdown: wakeup will reboot##\n");
        /* enter shutdown mode*/
        pm_enter_shutdown();

    }
    else
    {
        writel(RESET_BY_STANDBY | FROM_SHUT_DOWN_MODE, RST_STATUS_REG);
        PR_DEBUG("Reboot:pm_standby_wkpin_wakeup\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}


RET_CODE pm_shutdown_wkpinA8_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    data = readl(RST_STATUS_REG);

    if (((data & FROM_SHUT_DOWN_MODE) != FROM_SHUT_DOWN_MODE) && ((data & RESET_BY_STANDBY) != RESET_BY_STANDBY))
    {
        writel(RESET_BY_POR | RESET_BY_IWDG, RST_STATUS_REG);

        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);

        /* Xp config wakeup*/
        pm_wkpin_config(WKPIN_XPA8);

        PR_DEBUG("##sys enter shutdown: XPA8 wakeup will reboot##\n");
        /* enter shutdown mode*/
        pm_enter_shutdown();

    }
    else
    {
        writel(RESET_BY_STANDBY | FROM_SHUT_DOWN_MODE, RST_STATUS_REG);
        PR_DEBUG("Reboot:XPA8 pm_standby_wkpin_wakeup\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}


RET_CODE pm_shutdown_rtc_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    data = readl(RST_STATUS_REG);

    if (((data & FROM_SHUT_DOWN_MODE) != FROM_SHUT_DOWN_MODE) && ((data & RESET_BY_STANDBY) != RESET_BY_STANDBY))
    {


        writel(RESET_BY_POR | RESET_BY_IWDG, RST_STATUS_REG);

        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);

        /* Xp config wakeup*/
        pm_rtc_config();

        PR_DEBUG("##sys rtc enter shutdown: wakeup will reboot##\n");
        /* enter shutdown mode*/
        pm_enter_shutdown();

    }
    else
    {
        writel(RESET_BY_STANDBY | FROM_SHUT_DOWN_MODE, RST_STATUS_REG);
        PR_DEBUG("Reboot:rtc pm_rtc_shutdown\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}

RET_CODE pm_shutdown_iwdg_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    data = readl(RST_STATUS_REG);

    if (((data & FROM_SHUT_DOWN_MODE) != FROM_SHUT_DOWN_MODE) && ((data & RESET_BY_STANDBY) != RESET_BY_STANDBY))
    {

        writel(RESET_BY_POR | RESET_BY_IWDG, RST_STATUS_REG);

        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);
        /* iwdg config wakeup*/
        pm_iwdg_config();

        PR_DEBUG("##sys iwdg enter shutdown: wakeup will reboot##\n");
        /* enter shutdown mode*/
        pm_enter_shutdown();

    }
    else
    {
        writel(RESET_BY_STANDBY | FROM_SHUT_DOWN_MODE, RST_STATUS_REG);
        PR_DEBUG("Reboot:iwdg pm_shutdown_iwdg_wakeup\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}


RET_CODE pm_shutdown_xrstj_wakeup()
{
    u32 data;
    RET_CODE ret = RET_OK;

    data = readl(RST_STATUS_REG);

    if (((data & FROM_SHUT_DOWN_MODE) != FROM_SHUT_DOWN_MODE) && ((data & RESET_BY_STANDBY) != RESET_BY_STANDBY))
    {

        writel(RESET_BY_POR | RESET_BY_IWDG, RST_STATUS_REG);

        /* plls and hosc and losc enable */
        pm_plls_hosc_losc_en_config(ENABLE);

        PR_DEBUG("##sys reset key enter shutdown: wakeup will reboot##\n");
        /* enter shutdown mode*/
        pm_enter_shutdown();

    }
    else
    {
        writel(RESET_BY_STANDBY | FROM_SHUT_DOWN_MODE, RST_STATUS_REG);
        PR_DEBUG("Reboot:rest key pm_standby_wkpin_wakeup\n");
        /*check result*/
        ret = pm_check_sysclk_src_status();
        if (ret != RET_OK)
        {
            PR_ERR("pm_check_sysclk_src_status:Fail\n");
        }

    }

    return ret;

}




/***********************shutdown mode imp******************************/

/***********************get rand val*****************************/

#ifdef PM_SWITECH_TEST

static void pm_test_swich_func_install()
{
    pFunPmGoOn[0] = pm_sleepmode_rtc_wakeup;
    pFunPmGoOn[1] = pm_stop1_rtc_wakeup;
    pFunPmGoOn[2] = pm_stop2_rtc_wakeup;
    pFunPmRest[0] = pm_standby_rtc_wakeup;
    pFunPmRest[1] = pm_shutdown_rtc_wakeup;
}



void pm_switch_test()
{
    RET_CODE ret = RET_OK;

    uint32_t  reset_status;

    FCLK_SourceType_T  rand_goon_clk_Index;
    bool     fun_goon_flag = 0;
    uint32_t rand = 0;

    int (*pGoONFun)(FCLK_SourceType_T) = NULL;
    int (*pRestFun)() = NULL;

    pm_test_swich_func_install();
    while (1)
    {

        SysTick_Enable();
        extern u32_t sys_rand32_get(void);
        rand = sys_rand32_get();
        rand = rand % PM_SWITCH_FUN_NUM ;
        PR_DEBUG("#########get_rand_val:rand:%d#######\n", rand);

        if (rand >= 3)
        {
            /*wkup rest: run standby or shutdown*/
            fun_goon_flag = FALSE;
        }
        else
        {
            /*wkup goon: sleep stop1 stop2*/
            fun_goon_flag = TRUE;
            /*Run HIRC OR PLL*/
            rand_goon_clk_Index = rand % 2;
        }
        SysTick_Disable();
        reset_status = readl(RST_STATUS_REG);
        if (reset_status & FROM_STANDBY_MODE)
        {
            /*rest form standby,do check*/
            ret = pm_standby_rtc_wakeup();
            if (ret)
            {
                goto out;
            }
        }
        else if ((reset_status == 0x51))  /*shutdown bug*/
        {
            /*rest form shutdown,do check*/
            ret = pm_shutdown_rtc_wakeup();
            if (ret)
            {
                goto out;
            }
        }

        /*do fun swtich test*/
        if (fun_goon_flag == TRUE)
        {
            pGoONFun = pFunPmGoOn[rand];
            ret = pGoONFun(rand_goon_clk_Index);
        }
        else
        {
            pRestFun = pFunPmRest[rand - PM_SWITCH_GO_ON_FUN_NUM];
            ret = pRestFun();
        }

out:
        if (ret == RET_OK)
        {
            PR_DEBUG("######################PASS <-##-> ################\n");
        }
        else
        {
            PR_DEBUG("#####################Fail <-##-> #################\n");
            do {} while (1);
        }
    }

}

#endif









