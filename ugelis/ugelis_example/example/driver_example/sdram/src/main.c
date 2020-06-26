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
 * @brief       Sample code for adc test case.
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <stdio.h>
#include <gm_hal_gpio.h>
#include <gm_hal_uart.h>
//#include <gm_hal_adc.h>
#include <gm_common.h>
//#include <gm_hal_fmc.h>
#include <gm_hal_sdram.h>
#include "stdio.h"

#define CONFIG_SDRAM_32BIT_RW               (1)
#define CONFIG_SDRAM_16BIT_RW               (1)
#define CONFIG_SDRAM_8BIT_RW                (1)
#define CONFIG_SDRAM_RAND_32BIT_ONE_RW      (0)
#define CONFIG_SDRAM_RAND_32BIT_PAGE_RW     (0)
#define CONFIG_SDRAM_RAND_8BIT_ONE_RW       (0)
#define CONFIG_SDRAM_RAND_8BIT_PAGE_RW      (0)
#define CONFIG_SDRAM_32BIT_128M_RW          (0)
#define CONFIG_SDRAM_32BIT_32M_RW           (0)
#define CONFIG_SDRAM_SELFR_EFRESH           (0)
#define CONFIG_SDRAM_32BIT_32M_RW_CHECKLOOP (0)

#define SDRAM_MEM_START_ADDR 0xC0000000
/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

uint32_t WriteAddr = 0;
uint32_t ReadAddr  = 0;
uint8_t w_buf = 0xff;
uint8_t r_buf = 0;

//static atomic_val_t _rand32_counter;

#define _RAND32_INC 1000000013
#define SDRAM_ADDR_END 0xC1000000
/**
 *
 * @brief Get a 32 bit random number
 *
 * The non-random number generator returns values that are based off the
 * target's clock counter, which means that successive calls will return
 * different values.
 *
 * @return a 32-bit number
 */

uint32_t rand_data = 0;
uint32_t rand_addr = 0;
uint32_t rand_data0 = 0;
uint32_t rand_addr0 = 0;
uint32_t rand_addr1 = 0;
uint32_t rand_addr2 = 0;
int sdram_test(void)
{
    ////printf("*** SDRAM enter......\n");
    SDRAM_HandleTypeDef hsdram;
    hsdram.size  = SDRAM_16M;
    hsdram.width = SDRAM_16BIT;
    HAL_SDRAM_Init(&hsdram);
    ////printf("*** CONFIG_SDRAM_32BIT_RW......\n");
    HW32_REG(0x40039290) |= (0xA) << 12; //1-15

    while (1)
    {
#if CONFIG_SDRAM_32BIT_32M_RW_CHECKLOOP
        while (1)
        {
            //printf("*** CONFIG_SDRAM_32BIT_32M_RW_CHECKLOOP ......\n");
            for (WriteAddr = SDRAM_ADDR_END; WriteAddr > 0xC0000000; WriteAddr--)
            {
                *(uint8_t *)(WriteAddr) = 0xF0;
            }

            for (WriteAddr = SDRAM_ADDR_END; WriteAddr > 0xC0000000; WriteAddr--)
            {
                *(uint8_t *)(WriteAddr) = 0xA5;
                for (ReadAddr = SDRAM_ADDR_END; ReadAddr > 0xC0000000; ReadAddr--)
                {
                    if (ReadAddr != WriteAddr)
                    {
                        if (*(uint8_t *)(ReadAddr) != 0xF0)
                        {
                            ////printf("*** SDRAM test error, 8 bit ......\n");
                            return -1;
                        }
                    }
                }
                *(uint8_t *)(WriteAddr) = 0xF0;
            }
            //printf("*** CONFIG_SDRAM_32BIT_32M_RW_CHECKLOOP test finish once......\n");
        }
#endif
#if CONFIG_SDRAM_32BIT_RW
        //printf("*** CONFIG_SDRAM_32BIT_RW......\n");
        for (WriteAddr = 0xC0000000; WriteAddr < SDRAM_ADDR_END;)
        {
            *(uint32_t *)(WriteAddr) = 0xa5a5a5a5;
            WriteAddr = WriteAddr + 4;
        }
        for (WriteAddr = 0xC0000000; WriteAddr < SDRAM_ADDR_END;)
        {
            if (*(uint32_t *)(WriteAddr) != 0xa5a5a5a5)
            {
                return 1;
                ////printf("*** SDRAM test error, 32 bit ......\n");
            }
            WriteAddr = WriteAddr + 4;
        }
#endif

#if CONFIG_SDRAM_32BIT_RW
        //printf("*** SDRAM test 32bit write, read......\n");
        for (WriteAddr = 0xC0000000; WriteAddr < SDRAM_ADDR_END;)
        {
            *(uint32_t *)(WriteAddr) = WriteAddr;
            WriteAddr = WriteAddr + 4;
        }
        for (WriteAddr = 0xC0000000; WriteAddr < SDRAM_ADDR_END;)
        {
            if ((*(uint32_t *)(WriteAddr) != WriteAddr))
            {
                return 2;
                ////printf("*** SDRAM test error, 32 bit ......\n");
            }
            WriteAddr = WriteAddr + 4;
        }
#endif
        //test 16bit write, read
#if CONFIG_SDRAM_16BIT_RW
        //printf("*** SDRAM test 16bit write, read......\n");
        for (WriteAddr = 0xC0000000; WriteAddr < SDRAM_ADDR_END;)
        {
            *(uint16_t *)(WriteAddr) = 0xff00;
            WriteAddr = WriteAddr + 2;
        }
        for (WriteAddr = 0xC0000000; WriteAddr < SDRAM_ADDR_END;)
        {
            if (*(uint16_t *)(WriteAddr) != 0xff00)
            {
                return 3;
                ////printf("*** SDRAM test error, 16 bit ......\n");
            }
            WriteAddr = WriteAddr + 2;
        }
#endif

        //test 0x5a, a5
#if CONFIG_SDRAM_8BIT_RW
        //printf("*** SDRAM test 8bit write, read......\n");
        for (WriteAddr = 0xC0000000; WriteAddr < SDRAM_ADDR_END; WriteAddr++)
        {
            *(uint8_t *)(WriteAddr) = 0x0f;
        }
        for (WriteAddr = 0xC0000000; WriteAddr < SDRAM_ADDR_END; WriteAddr++)
        {
            if (*(uint8_t *)(WriteAddr) != 0x0f)
            {
                return 4;
                //printf("*** SDRAM test error, 8 bit ......\n");
            }
        }
        //printf("*** SDRAM 5 memory_test end......\n");
#endif
    }

#if CONFIG_SDRAM_32BIT_128M_RW
    //printf("*** SDRAM test 128M 8bit write, read......\n");
    for (WriteAddr = 0xC0000000; WriteAddr < 0xC8000000; WriteAddr++)
    {
        *(uint8_t *)(WriteAddr) = 0x5A;
    }

    for (WriteAddr = 0xC0000000; WriteAddr < 0xC8000000; WriteAddr++)
    {
        *(uint8_t *)(WriteAddr) = 0xA5;
        for (ReadAddr = 0xC0000000; ReadAddr < 0xC8000000; ReadAddr++)
        {
            if (ReadAddr != WriteAddr)
            {
                if (*(uint8_t *)(ReadAddr) != 0x5A)
                {
                    return 5;
                    //printf("*** SDRAM test error, 8 bit ......\n");
                }
            }
        }
        *(uint8_t *)(WriteAddr) = 0x5A;
    }
    ////printf("*** SDRAM test 128M 8bit, test end......\n");
#endif

#if CONFIG_SDRAM_32BIT_32M_RW
    //printf("*** SDRAM test 128M 8bit write, read......\n");
    for (WriteAddr = 0xC0000000; WriteAddr < SDRAM_ADDR_END; WriteAddr++)
    {
        *(uint8_t *)(WriteAddr) = 0x5A;
    }

    for (WriteAddr = 0xC0000000; WriteAddr < SDRAM_ADDR_END; WriteAddr++)
    {
        *(uint8_t *)(WriteAddr) = 0xA5;
        for (ReadAddr = 0xC0000000; ReadAddr < SDRAM_ADDR_END; ReadAddr++)
        {
            if (ReadAddr != WriteAddr)
            {
                if (*(uint8_t *)(ReadAddr) != 0x5A)
                {
                    return 6;
                    ////printf("*** SDRAM test error, 8 bit ......\n");
                }
            }
        }
        *(uint8_t *)(WriteAddr) = 0x5A;
    }
    ////printf("*** SDRAM test 128M 8bit, test end......\n");
#endif
    //}

#if CONFIG_SDRAM_SELFR_EFRESH
    //sdram_refresh_test();
#endif

}

int main(void)
{
    int ret = 0;
    ret = sdram_test();
    if (ret != 0)
    {
        printf("error, sdram test failed!!\n");
    }
    return ret;
}

