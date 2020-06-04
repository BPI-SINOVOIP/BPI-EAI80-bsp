/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


//gpio c 8 cd

#include <ugelis.h>
#include <misc/printk.h>
#include <gm_soc.h>
#include <gm_hal_mmc.h>
#include <gm_hal_gpio.h>
#include <pinmux.h>

#define     MMC_TEST_DEBUG 1
#define     MMC_TEST_ERR   1

#ifdef MMC_TEST_DEBUG
    #define mmc_test_pr_debug       printk
#else
    #define mmc_test_pr_debug(...)  do{}while(0);
#endif

#ifdef MMC_TEST_ERR
    #define mmc_test_pr_err         printk
#else
    #define mmc_test_pr_err(...)    do{}while(0);
#endif

#define TEST_BLOCK_NUM              (30)

/*host basic test*/
#define TESA_CASE_1                 1
#define RD                          10
#define WR                          10



typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

static int MMC_ConfigPinmux()
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    struct device *portg = NULL;
    struct device *portf = NULL;
    Device_Pinmux_T s_mmcPinMux[6] =
    {
        {PINMUX_GPIO_PIN_0, GPIOG, GPIOG0_AF10_SDMMC_CMD},
        {PINMUX_GPIO_PIN_1, GPIOG, GPIOG1_AF10_SDMMC_D2},
        {PINMUX_GPIO_PIN_2, GPIOG, GPIOG2_AF10_SDMMC_D3},

        {PINMUX_GPIO_PIN_13, GPIOF, GPIOF13_AF10_SDMMC_CLK},
        {PINMUX_GPIO_PIN_14, GPIOF, GPIOF14_AF10_SDMMC_D0},
        {PINMUX_GPIO_PIN_15, GPIOF, GPIOF15_AF10_SDMMC_D1},
    };

    portg = device_get_binding(CONFIG_PINMUX_GM_PORTG_NAME);
    if (portg == NULL)
    {
        return -1;
    }

    portf = device_get_binding(CONFIG_PINMUX_GM_PORTF_NAME);
    if (portf == NULL)
    {
        return -1;
    }

    pin_cfg.pin = BIT(s_mmcPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[0].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = BIT(s_mmcPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[1].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = BIT(s_mmcPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[2].alternate;
    ret = pinmux_pin_set(portg, s_mmcPinMux[2].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = BIT(s_mmcPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[3].alternate;
    ret = pinmux_pin_set(portf, s_mmcPinMux[3].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_mmcPinMux[4].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[4].alternate;
    ret = pinmux_pin_set(portf, s_mmcPinMux[4].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }


    pin_cfg.pin = BIT(s_mmcPinMux[5].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = s_mmcPinMux[5].alternate;
    ret = pinmux_pin_set(portf, s_mmcPinMux[5].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}







/*travel all the card setor test*/
//#define TESA_CASE_2                 1


/*singal test*/
//#define  TESA_CASE_3                1
#ifdef   TESA_CASE_3
    #undef  TEST_BLOCK_NUM
    #define TEST_BLOCK_NUM          1
#endif


/*earse test*/
//#define  TESA_CASE_4                1


#define TEST_BLOCK_START            (0)
#define TEST_DATA                   (0xa5)
#define TEST_DATA_SIZE              (512*TEST_BLOCK_NUM)

static uint8_t gbuf[TEST_DATA_SIZE] = {0};
MMC_Handle_T ghMMC;

/************************************** TESA_CASE_1 || TESA_CASE_3*********************************************/
#if (defined(TESA_CASE_1) || defined(TESA_CASE_3))
int main(void)
{
    RET_CODE ret = RET_OK;
    uint32_t blk_cnt = TEST_BLOCK_NUM;
    uint32_t blk_start = TEST_BLOCK_START;
    static uint32_t cnt = 0;
    uint8_t *buf = gbuf;
    uint32_t i = 0;
    MMC_Config_T config;
    MMC_Handle_T *hMMC = &ghMMC;

    MMC_ConfigPinmux();
#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_Init();
    DebugConsoleInit(19200);
#endif

    mmc_test_pr_debug("test TESA_CASE_1 || TESA_CASE_3\n");

    /*init mmc*/
    config.busWitch      = MMC_BUS_WIDTH_4;
    config.type            = MMC_SD;
    config.removable   = TRUE;



    mmc_test_pr_debug("##########Try MMC Init ##############\n");

    ret = HAL_MMC_Init(hMMC, MMC0, &config);
    if (ret != RET_OK)
    {
        mmc_test_pr_err("Err:%s:HAL_MMC_Init\n", __FUNCTION__);
        return ret;
    }

    mmc_test_pr_debug("##########MMC Init Ok##############\n");

    while (1)
    {
#ifdef  WR
        memset(buf, TEST_DATA, TEST_DATA_SIZE);
        /*write mmc*/
        blk_cnt = HAL_MMC_Write(hMMC, blk_start, TEST_BLOCK_NUM, buf);
        if (blk_cnt != TEST_BLOCK_NUM)
        {
            mmc_test_pr_err("Err:%s:HAL_MMC_Write\n", __FUNCTION__);
            return RET_ERROR;
        }
#endif

#ifdef RD
        /*clear buf*/
        memset(buf, 0, TEST_DATA_SIZE);
        /*read mmc*/
        blk_cnt = HAL_MMC_Read(hMMC, blk_start, TEST_BLOCK_NUM, buf);
        if (blk_cnt != TEST_BLOCK_NUM)
        {
            mmc_test_pr_err("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
            return RET_ERROR;
        }
#endif
        /*checdata is ok*/
        for (i = 0 ; i < TEST_DATA_SIZE ; i++)
        {
            if (buf[i] != TEST_DATA)
            {
                mmc_test_pr_err("Check Err: sector:%d buf[%d]:0x%08x\n", (i / 512), i, buf[i]);
                return RET_ERROR;
            }
        }
        mmc_test_pr_debug("MMC TEST PASS:CNT:%u\n", cnt++);

    }
}

#endif



/************************************** TESA_CASE_2 || TESA_CASE_3*********************************************/

#if (defined(TESA_CASE_2) || defined(TESA_CASE_3))

int main(void)
{
    RET_CODE ret = RET_OK;
    uint32_t blk_cnt = TEST_BLOCK_NUM;
    uint32_t blk_start = TEST_BLOCK_START;
    static uint32_t cnt = 0;
    uint8_t *buf = gbuf;
    uint32_t i = 0;
    MMC_Config_T config;
    MMC_Handle_T *hMMC = &ghMMC;
    uint32_t totalSector = 0;

    MMC_ConfigPinmux();
#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_Init();
    DebugConsoleInit(19200);
#endif

    mmc_test_pr_debug("test TESA_CASE_2 || TESA_CASE_3\n");

    /*init mmc*/
    config.busWitch     = MMC_BUS_WIDTH_1;
    config.type         = MMC_SD;
    config.removable    = TRUE;



    ret = HAL_MMC_Init(hMMC, MMC0, &config);
    if (ret != RET_OK)
    {
        mmc_test_pr_err("Err:%s:HAL_MMC_Init\n", __FUNCTION__);
        return ret;
    }

    mmc_test_pr_debug("##########MMC Init Ok##############\n");
    /*get sdcard cap*/
    totalSector = HAL_MMC_GetCapcity(hMMC);

    mmc_test_pr_debug("total sectors :%u\n", totalSector);


    while (1)
    {

        memset(buf, TEST_DATA, TEST_DATA_SIZE);
        /*write mmc*/
        blk_cnt = HAL_MMC_Write(hMMC, blk_start, TEST_BLOCK_NUM, buf);
        if (blk_cnt != TEST_BLOCK_NUM)
        {
            mmc_test_pr_err("Err:%s:HAL_MMC_Write\n", __FUNCTION__);
            return RET_ERROR;
        }

        /*clear buf*/
        memset(buf, 0, TEST_DATA_SIZE);
        /*read mmc*/

        blk_cnt = HAL_MMC_Read(hMMC, blk_start, TEST_BLOCK_NUM, buf);
        if (blk_cnt != TEST_BLOCK_NUM)
        {
            mmc_test_pr_err("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
            return RET_ERROR;
        }

        /*checdata is ok*/
        for (i = 0 ; i < TEST_DATA_SIZE ; i++)
        {
            if (buf[i] != TEST_DATA)
            {
                mmc_test_pr_err("Check Err: sector:%d buf[%d]:0x%08x\n", (i / 512), i, buf[i]);
                return RET_ERROR;
            }
        }

        mmc_test_pr_debug("MMC TEST PASS: blk_start:%u CNT:%u\n", blk_start, cnt++);
        blk_start += TEST_BLOCK_NUM;
        if (blk_start >= totalSector)
        {
            mmc_test_pr_debug("ALL Sector:%u PASS\n", blk_start);
        }

    }
}

#endif



/************************************** TESA_CASE_4*********************************************/

#ifdef TESA_CASE_4

int main(void)
{
    RET_CODE ret = RET_OK;
    uint32_t blk_cnt = TEST_BLOCK_NUM;
    uint32_t blk_start = TEST_BLOCK_START;
    static uint32_t cnt = 0;
    uint8_t *buf = gbuf;
    uint32_t i = 0;
    MMC_Config_T config;
    uint64_t totalSector = 0;
    MMC_Handle_T *hMMC = &ghMMC;

    MMC_ConfigPinmux();
#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_Init();
    DebugConsoleInit(19200);
#endif
    printk("test TESA_CASE_4 \n");

    /*init mmc*/
    config.busWitch     = MMC_BUS_WIDTH_4;
    config.type         = MMC_SD;
    config.removable    = TRUE;
    config.callback     = NULL;



    ret = HAL_MMC_Init(hMMC, MMC0, &config);
    if (ret != RET_OK)
    {
        mmc_test_pr_err("Err:%s:HAL_MMC_Init\n", __FUNCTION__);
        return ret;
    }

    mmc_test_pr_debug("##########MMC Init OK##############\n");
    totalSector = HAL_MMC_GetCapcity(hMMC);

    while (1)
    {
        /*write data*/
        memset(buf, TEST_DATA, TEST_DATA_SIZE);
        /*write mmc*/
        blk_cnt = HAL_MMC_Write(hMMC, blk_start, TEST_BLOCK_NUM, buf);
        if (blk_cnt != TEST_BLOCK_NUM)
        {
            mmc_test_pr_err("Err:%s:HAL_MMC_Write\n", __FUNCTION__);
            return RET_ERROR;
        }

        /*read data*/
        memset(buf, 0, TEST_DATA_SIZE);
        blk_cnt = HAL_MMC_Read(hMMC, blk_start, TEST_BLOCK_NUM, buf);
        if (blk_cnt != TEST_BLOCK_NUM)
        {
            mmc_test_pr_err("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
            return RET_ERROR;
        }

        /*checdata is ok*/
        for (i = 0 ; i < TEST_DATA_SIZE ; i++)
        {
            if (buf[i] != TEST_DATA)
            {
                mmc_test_pr_err("Check Err: sector:%d buf[%d]:0x%08x\n", (i / 512), i, buf[i]);
                return RET_ERROR;
            }
        }

        /*easer secotr*/
        blk_cnt = HAL_MMC_Erase(hMMC, blk_start, TEST_BLOCK_NUM);
        if (blk_cnt != TEST_BLOCK_NUM)
        {
            mmc_test_pr_err("Err:%s:HAL_MMC_Erase blk_cnt:%u\n", __FUNCTION__, blk_cnt);
            return RET_ERROR;
        }

        /*when earse finish and then read data*/
        blk_cnt = HAL_MMC_Read(hMMC, blk_start, TEST_BLOCK_NUM, buf);
        if (blk_cnt != TEST_BLOCK_NUM)
        {
            mmc_test_pr_err("Err:%s:HAL_MMC_Read\n", __FUNCTION__);
            return RET_ERROR;
        }
        /*check erase data is ok*/
        for (i = 0 ; i < TEST_DATA_SIZE ; i++)
        {
            if (buf[i] == TEST_DATA_SIZE)
            {
                mmc_test_pr_err("Check Err: sector:%d buf[%d]:0x%08x\n", (i / 512), i, buf[i]);
                return RET_ERROR;
            }
        }
        mmc_test_pr_debug("MMC TEST PASS:CNT:%u\n", cnt++);
        blk_start += TEST_BLOCK_NUM;
        if (blk_start >= totalSector)
        {
            mmc_test_pr_debug("ALL Sector:%u PASS\n", blk_start);
        }


    }

}

#endif

