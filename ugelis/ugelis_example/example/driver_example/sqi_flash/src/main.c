/**
 *
 * Copyright (C) 2018 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          main.c
 *
 * @author        Steven.lee
 *
 * @version       1.1.0
 *
 * @date          2019/11/27
 *
 * @brief         sqi demo source file
 *
 * @note
 *    2019/11/27, Steven.lee, V1.0.0
 *        Initial version.
 */

#include <ugelis.h>
#include <device.h>
#include <gm_hal_gpio.h>
#include <gm_hal_sqi.h>
#include <gm_common.h>
#include <misc/printk.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>

#include <flash.h>
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
#ifdef BPI-ALL
#define FLASH_TOTALSIZE_LEN      (0x1000000)
#else
#define FLASH_TOTALSIZE_LEN      (0x0010000)
#endif
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
        printf("********SFlash Init Error*******\n");

        return -1;
    }

    ret = HAL_SQI_Print_Device(&gSQIHandle);
    if (ret != RET_OK)
    {
        printf("********SFlash Init Error*******\n");

        return -1;
    }

    printf("********SFlash es easer chip Begin*******\n");

    ret  = HAL_SQI_Erase(&gSQIHandle, FLASH_TEST_OFFSET, (FLASH_TOTALSIZE_LEN - FLASH_TEST_OFFSET));
    if (ret != RET_OK)
    {
        printf("********SFlash easer Error*******\n");

        return -1;
    }

    for (i = 0; i < sector_count; i++)
    {
        memset(flash_to_buff, 0x11, FLASH_SECTOR_SIZE);

        ret  = HAL_SQI_Read(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            printf("********SFlash write Error*******\n");

            return -1;
        }

        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE) == 0)
        {
            printf("The Flash earse chip OK sector = %d !\n", i);
        }
        else
        {
            printf("The Flash earse chip ERROR sector = %d !\n", i);

            return -1;
        }
    }

    printf("********SFlash es easer sector Begin*******\n");
    for (i = 0; i < sector_count; i++)
    {
        ret  = HAL_SQI_Erase(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            printf("********SFlash easer sector Error*******\n");

            return -1;
        }

        memset(flash_to_buff, 0x00, FLASH_SECTOR_SIZE);

        ret  = HAL_SQI_Read(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            printf("********SFlash read Error*******\n");

            return -1;
        }

        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE) == 0)
        {
            printf("The Flash earse chip OK sector = %d !\n", i);
        }
        else
        {
            printf("The Flash earse chip ERROR sector = %d !\n", i);

            return -1;
        }
    }

    printf("The Flash earse chip all OK !\n");

    return 0;
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


    printf("********SFlash es Test Begin*******\n");
    ret  = HAL_SQI_Init(&gSQIHandle, pSQIDev, &gSQIConfig);
    if (ret != RET_OK)
    {
        printf("********SFlash Init Error*******\n");

        return -1;
    }

    ret = HAL_SQI_Print_Device(&gSQIHandle);
    if (ret != RET_OK)
    {
        printf("********SFlash Init Error*******\n");

        return -1;
    }

    ret  = HAL_SQI_Erase(&gSQIHandle, FLASH_TEST_OFFSET, (FLASH_TOTALSIZE_LEN - FLASH_TEST_OFFSET));
    if (ret != RET_OK)
    {
        printf("********SFlash easer Error*******\n");

        return -1;
    }

    for (i = 0; i < 100; i++)
    {
        ret  = HAL_SQI_Write(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)buff_to_flash, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            printf("********SFlash write Error*******\n");

            return -1;
        }
        printf("1. ********SFlash Read & Write test Begin*******\n");
        memset(flash_to_buff, 0x00, FLASH_SECTOR_SIZE);
        ret  = HAL_SQI_Read(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            printf("********SFlash write Error*******\n");

            return -1;
        }
        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE) == 0)
        {
            printf("The Flash Write & Read Data OK sector = %d !\n", i);
        }
        else
        {
            printf("The Flash Write & Read Data ERROR sector = %d !\n", i);

            return -1;
        }
    }

    printf("The Flash Write & Read Data all OK !\n");

    return 0;
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


    printf("********SFlash es Test Begin*******\n");
    ret  = HAL_SQI_Init(&gSQIHandle, pSQIDev, &gSQIConfig);
    if (ret != RET_OK)
    {
        printf("********SFlash Init Error*******\n");

        return -1;
    }

    ret = HAL_SQI_Print_Device(&gSQIHandle);
    if (ret != RET_OK)
    {
        printf("********SFlash Init Error*******\n");

        return -1;
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

    printf("1. ********SFlash Read & WriteAll test Begin*******\n");

    for (i = 99; i < sector_count - 3; i++)
    {
        ret  = HAL_SQI_WriteAll(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)buff_to_flash, FLASH_SECTOR_SIZE * 2);
        if (ret != RET_OK)
        {
            printf("********SFlash write Error*******\n");

            return -1;
        }

        memset(flash_to_buff, 0x00, FLASH_SECTOR_SIZE);
        ret  = HAL_SQI_Read(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE * 2);
        if (ret != RET_OK)
        {
            printf("********SFlash write Error*******\n");

            return -1;
        }

        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE * 2) == 0)
        {
            printf("The Flash WriteAll & Read Data OK sector = %d !\n", i);
        }
        else
        {
            printf("The Flash WriteAll & Read Data ERROR sector = %d !\n", i);

            return -1;
        }
    }

    printf("The Flash WriteAll & Read Data all OK !\n");

    return 0;
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

    printf("********SFlash es Test Begin*******\n");
    ret  = HAL_SQI_Init(&gSQIHandle, pSQIDev, &gSQIConfig);
    if (ret != RET_OK)
    {
        printf("********SFlash Init Error*******\n");

        return -1;
    }

    ret = HAL_SQI_Print_Device(&gSQIHandle);
    if (ret != RET_OK)
    {
        printf("********SFlash Init Error*******\n");

        return -1;
    }

    ret  = HAL_SQI_Erase(&gSQIHandle, FLASH_TEST_OFFSET, (FLASH_TOTALSIZE_LEN - FLASH_TEST_OFFSET));
    if (ret != RET_OK)
    {
        printf("********SFlash easer Error*******\n");

        return -1;
    }

    for (i = 0; i < 100; i++)
    {
        ret  = HAL_SQI_Write(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)buff_to_flash, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            printf("********SFlash write Error*******\n");

            return -1;
        }
        printf("1. ********SFlash Read & Write test Begin*******\n");
        memset(flash_to_buff, 0x00, FLASH_SECTOR_SIZE);
        ret  = HAL_SQI_Read(&gSQIHandle, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            printf("********SFlash read Error*******\n");

            return -1;
        }
        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE) == 0)
        {
            printf("The Flash Write & Read Data OK sector = %d !\n", i);
        }
        else
        {
            printf("The Flash Write & Read Data ERROR sector = %d !\n", i);

            return -1;
        }
    }

    printf("The Flash Write & Read Data all OK !\n");

    return 0;
}

int sqi_flash_driver_write_read_demo(void)
{
    int ret;
    int i;
    uint32_t    value_t;
    struct device *dev;
    uint32_t    sector_offset;
    sector_offset = FLASH_TEST_OFFSET / FLASH_SECTOR_SIZE;
    ret = -1;
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
    printf("********SFlash ugelis driver Test Begin*******\n");
    dev = device_get_binding(DEV_FLASH_NAME);
    if (!dev)
    {
        printk("Main Err: device_get_binding\n");
        return ret;
    }
    ret = flash_erase(dev, FLASH_TEST_OFFSET, FLASH_TEST_ERASE_SIZE);
    if (ret != RET_OK)
    {
        printf("********SFlash Erase Error*******\n");

        return -1;
    }
    for (i = 0; i < 100; i++)
    {
        ret  = flash_write(dev, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)buff_to_flash, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            printf("********SFlash write Error*******\n");

            return -1;
        }

        memset(flash_to_buff, 0x00, FLASH_SECTOR_SIZE);
        ret  = flash_read(dev, (i + sector_offset) * FLASH_SECTOR_SIZE, (void *)flash_to_buff, FLASH_SECTOR_SIZE);
        if (ret != RET_OK)
        {
            printf("********SFlash read Error*******\n");
            return -1;
        }
        if (arraycmp(buff_to_flash, flash_to_buff, FLASH_SECTOR_SIZE) == 0)
        {
            printf("The Flash Write & Read Data OK sector = %d !\n", i);
        }
        else
        {
            printf("The Flash Write & Read Data ERROR sector = %d !\n", i);

            return -1;
        }
    }

    printf("The Flash Write & Read Data all OK !\n");
    return ret;

}

void main(void)
{
    printf("================SFlash Test Start=================\n\n\n");
    printf("================SFlash erase boot1=================\n\n\n");
    sqi_flash_earse_demo();
#if 0
    sqi_flash_write_read_demo();
    sqi_flash_writeall_read_demo();
    sqi_flash_dma_write_read_demo();
    sqi_flash_driver_write_read_demo();
#endif
    printf("================SFlash Test  End==================\n\n\n");
}

