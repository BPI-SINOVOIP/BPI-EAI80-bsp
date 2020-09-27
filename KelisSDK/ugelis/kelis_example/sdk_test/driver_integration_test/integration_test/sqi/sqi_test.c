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

static SQI_Device_T *pSQIDev = (SQI_Device_T *)EFLASH_CTRL_BASE;
static struct SQI_Handle  gSQIHandle;
static struct SQI_Config  gSQIConfig;
#define FLASH_TEST_ERASE_SIZE    0x500000
#define FLASH_TEST_REGION_OFFSET 0xff000
#define FLASH_SECTOR_SIZE        (4096)
#define FLASH_TOTALSIZE_LEN      (0x1000000)
#define FLASH_TEST_OFFSET        (0x0000000)
uint8_t buff_to_flash[FLASH_SECTOR_SIZE * 3];//Sector,page,block32,block all use this buffer,not will change
uint8_t flash_to_buff[FLASH_SECTOR_SIZE * 3];//Sector,page,block32,block all use this buffer

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;


static int arraycmp(uint8_t *src, uint8_t *dst, uint32_t size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        if (src[i] != dst[i])
        {
            return -(i + 1);
        }
    }

    return 0;
}

int sqi_flash_earse_demo(void)
{
    uint32_t    i;
    uint32_t    value_t;
    RET_CODE    ret;
    uint32_t    sector_count;
    uint32_t    sector_offset;

    sector_offset = FLASH_TEST_OFFSET / FLASH_SECTOR_SIZE;
    sector_count = (FLASH_TOTALSIZE_LEN - FLASH_TEST_OFFSET) / FLASH_SECTOR_SIZE;

    memset(buff_to_flash, 0x00, FLASH_SECTOR_SIZE);

    value_t = 0;
    for (i = 0; i < FLASH_SECTOR_SIZE; ++i)
    {
        buff_to_flash[i] = 0xff;
    }


    ret  = HAL_SQI_Init(&gSQIHandle, pSQIDev, &gSQIConfig);
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash Init Error*******\n");

        return TC_FAIL;
    }

    ret = HAL_SQI_Print_Device(&gSQIHandle);
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash Init Error*******\n");

        return TC_FAIL;
    }

    TC_PRINT("********SFlash es easer chip Begin*******\n");

    ret  = HAL_SQI_Erase(&gSQIHandle, FLASH_TEST_OFFSET, (FLASH_TOTALSIZE_LEN - FLASH_TEST_OFFSET));
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash easer Error*******\n");

        return TC_FAIL;
    }

    for (i = 0; i < sector_count; i++)
    {
        memset(flash_to_buff, 0x11, FLASH_SECTOR_SIZE);

        ret  = HAL_SQI_Read(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            TC_PRINT("********SFlash write Error*******\n");

            return TC_FAIL;
        }

        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE) != 0)
        {
            TC_PRINT("The Flash earse chip ERROR sector = %d !\n", i);
            return TC_FAIL;
        }
    }

    TC_PRINT("********SFlash es easer sector Begin*******\n");
    for (i = 0; i < sector_count; i++)
    {
        ret  = HAL_SQI_Erase(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            TC_PRINT("********SFlash easer sector Error*******\n");

            return TC_FAIL;
        }

        memset(flash_to_buff, 0x00, FLASH_SECTOR_SIZE);

        ret  = HAL_SQI_Read(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            TC_PRINT("********SFlash read Error*******\n");

            return TC_FAIL;
        }

        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE) != 0)
        {
            TC_PRINT("The Flash earse chip ERROR sector = %d !\n", i);
            return TC_FAIL;
        }
    }

    TC_PRINT("The Flash earse chip all OK !\n");

    return TC_PASS;
}

int sqi_flash_write_read_demo(void)
{
    uint32_t    i;
    uint32_t    value_t;
    RET_CODE    ret;
    uint32_t    sector_count;
    uint32_t    sector_offset;

    sector_offset = FLASH_TEST_OFFSET / FLASH_SECTOR_SIZE;
    sector_count = (FLASH_TOTALSIZE_LEN - FLASH_TEST_OFFSET) / FLASH_SECTOR_SIZE;

    memset(buff_to_flash, 0x00, FLASH_SECTOR_SIZE);

    value_t = 0;
    for (i = 0; i < FLASH_SECTOR_SIZE; ++i)
    {
        buff_to_flash[i] = value_t;
        ++value_t;
        if (value_t % FLASH_SECTOR_SIZE == 0)
        {
            value_t = 0;
        }
    }


    TC_PRINT("********SFlash es Test Begin*******\n");
    ret  = HAL_SQI_Init(&gSQIHandle, pSQIDev, &gSQIConfig);
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash Init Error*******\n");

        return TC_FAIL;
    }

    ret = HAL_SQI_Print_Device(&gSQIHandle);
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash Init Error*******\n");

        return TC_FAIL;
    }

    ret  = HAL_SQI_Erase(&gSQIHandle, FLASH_TEST_OFFSET, (FLASH_TOTALSIZE_LEN - FLASH_TEST_OFFSET));
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash easer Error*******\n");

        return TC_FAIL;
    }

    for (i = 0; i < 100; i++)
    {
        ret  = HAL_SQI_Write(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)buff_to_flash, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            TC_PRINT("********SFlash write Error*******\n");

            return TC_FAIL;
        }
        memset(flash_to_buff, 0x00, FLASH_SECTOR_SIZE);
        ret  = HAL_SQI_Read(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            TC_PRINT("********SFlash write Error*******\n");

            return TC_FAIL;
        }
        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE) != 0)
        {
            TC_PRINT("The Flash Write & Read Data ERROR sector = %d !\n", i);
            return TC_FAIL;
        }
    }

    TC_PRINT("The Flash Write & Read Data all OK !\n");

    return TC_PASS;
}

int sqi_flash_writeall_read_demo(void)
{
    uint32_t    i;
    uint32_t    value_t;
    RET_CODE    ret;
    uint32_t    sector_count;
    uint32_t    sector_offset;

    sector_offset = FLASH_TEST_OFFSET / FLASH_SECTOR_SIZE;
    sector_count = (FLASH_TOTALSIZE_LEN - FLASH_TEST_OFFSET) / FLASH_SECTOR_SIZE;

    memset(buff_to_flash, 0x00, FLASH_SECTOR_SIZE);

    value_t = 0;
    for (i = 0; i < FLASH_SECTOR_SIZE; ++i)
    {
        buff_to_flash[i] = value_t;
        ++value_t;
        if (value_t % FLASH_SECTOR_SIZE == 0)
        {
            value_t = 0;
        }
    }


    TC_PRINT("********SFlash es Test Begin*******\n");
    ret  = HAL_SQI_Init(&gSQIHandle, pSQIDev, &gSQIConfig);
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash Init Error*******\n");

        return TC_FAIL;
    }

    ret = HAL_SQI_Print_Device(&gSQIHandle);
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash Init Error*******\n");

        return TC_FAIL;
    }

    value_t = 1;
    for (i = 0; i < FLASH_SECTOR_SIZE * 2; ++i)
    {
        buff_to_flash[i] = value_t;
        value_t += 2;
        if (value_t % (FLASH_SECTOR_SIZE * 2) == 0)
        {
            value_t = 1;
        }
    }


    for (i = 99; i < sector_count - 3; i++)
    {
        ret  = HAL_SQI_WriteAll(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)buff_to_flash, FLASH_SECTOR_SIZE * 2);
        if (ret != RET_OK)
        {
            TC_PRINT("********SFlash write Error*******\n");

            return TC_FAIL;
        }

        memset(flash_to_buff, 0x00, FLASH_SECTOR_SIZE);
        ret  = HAL_SQI_Read(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE * 2);
        if (ret != RET_OK)
        {
            TC_PRINT("********SFlash write Error*******\n");

            return TC_FAIL;
        }

        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE * 2) != 0)
        {
            TC_PRINT("The Flash WriteAll & Read Data ERROR sector = %d !\n", i);
            return TC_FAIL;
        }
    }

    TC_PRINT("The Flash WriteAll & Read Data all OK !\n");

    return TC_PASS;
}

int sqi_flash_dma_write_read_demo(void)
{
    uint32_t    i;
    uint32_t    value_t;
    RET_CODE    ret;
    uint32_t    sector_offset;
    sector_offset = FLASH_TEST_OFFSET / FLASH_SECTOR_SIZE;

    memset(buff_to_flash, 0x00, FLASH_SECTOR_SIZE);

    value_t = 0;
    for (i = 0; i < FLASH_SECTOR_SIZE; ++i)
    {
        buff_to_flash[i] = value_t;
        ++value_t;
        if (value_t % FLASH_SECTOR_SIZE == 0)
        {
            value_t = 0;
        }
    }


    TC_PRINT("********SFlash es Test Begin*******\n");
    gSQIConfig.bit_mode = SQI_4BIT_MODE;
    gSQIConfig.read_mode = SQI_READ_BY_DMA;
    ret  = HAL_SQI_Init(&gSQIHandle, pSQIDev, &gSQIConfig);
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash Init Error*******\n");

        return TC_FAIL;
    }

    ret = HAL_SQI_Print_Device(&gSQIHandle);
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash Init Error*******\n");

        return TC_FAIL;
    }

    ret  = HAL_SQI_Erase(&gSQIHandle, FLASH_TEST_OFFSET, FLASH_TEST_ERASE_SIZE);
    if (ret != RET_OK)
    {
        TC_PRINT("********SFlash erase Error*******\n");

        return TC_FAIL;
    }

    for (i = 0; i < 100; i++)
    {
        ret  = HAL_SQI_Write(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)buff_to_flash, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            TC_PRINT("********SFlash write Error*******\n");

            return TC_FAIL;
        }
        memset(flash_to_buff, 0x00, FLASH_SECTOR_SIZE);
        ret  = HAL_SQI_Read(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            TC_PRINT("********SFlash read Error*******\n");

            return TC_FAIL;
        }
        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE) != 0)
        {
            TC_PRINT("The Flash Write & Read Data ERROR sector = %d !\n", i);
            return TC_FAIL;
        }
    }

    TC_PRINT("The Flash Write & Read Data all OK !\n");

    return TC_PASS;
}


void sqi_test(void)
{
    TC_PRINT("================SFlash Test Start=================\n\n\n");
    TC_PRINT("TEST1\n");
    zassert_true(sqi_flash_earse_demo() == TC_PASS, NULL);
    TC_PRINT("TEST2\n");
    zassert_true(sqi_flash_writeall_read_demo() == TC_PASS, NULL);
    TC_PRINT("TEST3\n");
    zassert_true(sqi_flash_write_read_demo() == TC_PASS, NULL);
    TC_PRINT("TEST4\n");
    zassert_true(sqi_flash_dma_write_read_demo() == TC_PASS, NULL);
    TC_PRINT("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);

    TC_PRINT("================SFlash Test  End==================\n\n\n");
}
