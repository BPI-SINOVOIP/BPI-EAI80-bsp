/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>

#define CONFIG_SRAM_32BIT_RW               (1)
#define CONFIG_SRAM_16BIT_RW               (1)
#define CONFIG_SRAM_8BIT_RW                (1)
#define CONFIG_SRAM_RAND_32BIT_ONE_RW      (0)
#define CONFIG_SRAM_RAND_32BIT_PAGE_RW     (0)
#define CONFIG_SRAM_RAND_8BIT_ONE_RW       (0)
#define CONFIG_SRAM_RAND_8BIT_PAGE_RW      (0)

//gm6721 sram 386k size 0x2001_FFFF
//#define SRAM_ADDR_END   0x200607FF
#define SRAM_ADDR_END   0x2001FFFF

#define STF_SW_RSTJ     14
#define KDP_SW_RSTJ     16
#define IMP_SW_RSTJ     17
#define STF_CLK_EN      14
#define KDPSRAM_CLK_EN  15
#define KDP_CLK_EN      16
#define IMP_CLK_EN      17

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

uint32_t WriteAddr = 0;
uint32_t ReadAddr  = 0;
uint8_t  w_buf     = 0xff;
uint8_t  r_buf     = 0;

int sram_test(void)
{
    printk("*** SRAM enter......\n");
    *(volatile uint32_t *)0x400002e0 |= 0x4;
    *(volatile uint32_t *)0x40000328 |= 0x10;
    *(volatile uint32_t *)0x40000098 |= 0x400;

    /*
        init kdp310 sram
    */
    *(volatile uint32_t *)(0x1FFFF0B0) &= ~(1 << 18); //kdp on
    *(volatile uint32_t *)(0x40000070) |= 1 << STF_SW_RSTJ | 1 << 15 | 1 << KDP_SW_RSTJ | 1 << IMP_SW_RSTJ;
    *(volatile uint32_t *)(0x40000080) |= 1 << STF_CLK_EN | 1 << KDPSRAM_CLK_EN | 1 << KDP_CLK_EN | 1 << IMP_CLK_EN;

    while (1)
    {
#if CONFIG_SRAM_32BIT_RW
        printk("*** CONFIG_SRAM_32BIT_RW......\n");
        for (WriteAddr = 0x20000000; WriteAddr < SRAM_ADDR_END;)
        {
            *(uint32_t *)(WriteAddr) = 0xa5a5a5a5;
            WriteAddr = WriteAddr + 4;
            //*(uint32_t*)(WriteAddr)= 0x5a5a5a5a;
            //WriteAddr = WriteAddr + 4;
        }


        for (WriteAddr = 0x20000000; WriteAddr < SRAM_ADDR_END;)
        {
            if (*(uint32_t *)(WriteAddr) != 0xa5a5a5a5)
            {
                return 1;
                printk("*** SRAM test error, 32 bit ......\n");
            }
            WriteAddr = WriteAddr + 4;
        }
#endif

#if CONFIG_SRAM_32BIT_RW
        printk("*** SRAM test 32bit write, read......\n");
        for (WriteAddr = 0x20000000; WriteAddr < SRAM_ADDR_END;)
        {
            *(uint32_t *)(WriteAddr) = WriteAddr;
            WriteAddr = WriteAddr + 4;
        }



        for (WriteAddr = 0x20000000; WriteAddr < SRAM_ADDR_END;)
        {
            //if ((*(uint32_t*)(WriteAddr) != WriteAddr) && (*(uint32_t*)(WriteAddr) != WriteAddr + 0x200))
            if ((*(uint32_t *)(WriteAddr) != WriteAddr))
            {
                return 2;
                printk("*** SRAM test error, 32 bit ......\n");
            }
            WriteAddr = WriteAddr + 4;
        }
#endif
        //}
        //test 16bit write, read
#if CONFIG_SRAM_16BIT_RW
        printk("*** SRAM test 16bit write, read......\n");
        for (WriteAddr = 0x20000000; WriteAddr < SRAM_ADDR_END;)
        {
            *(uint16_t *)(WriteAddr) = 0xff00;
            WriteAddr = WriteAddr + 2;
        }


        for (WriteAddr = 0x20000000; WriteAddr < SRAM_ADDR_END;)
        {
            if (*(uint16_t *)(WriteAddr) != 0xff00)
            {
                return 3;
                printk("*** SRAM test error, 16 bit ......\n");
            }
            WriteAddr = WriteAddr + 2;
        }
#endif

        //test 0x5a, a5
#if CONFIG_SRAM_8BIT_RW
        printk("*** SRAM test 8bit write, read......\n");
        for (WriteAddr = 0x20000000; WriteAddr < SRAM_ADDR_END; WriteAddr++)
        {
            *(uint8_t *)(WriteAddr) = 0x0f;
        }



        for (WriteAddr = 0x20000000; WriteAddr < SRAM_ADDR_END; WriteAddr++)
        {
            if (*(uint8_t *)(WriteAddr) != 0x0f)
            {
                return 4;
                printk("*** SRAM test error, 8 bit ......\n");
            }
        }
        printk("*** SRAM 5 memory_test end......\n");
#endif
    }


#if CONFIG_SRAM_32BIT_128M_RW
    printk("*** SRAM test 128M 8bit write, read......\n");
    for (WriteAddr = 0x20000000; WriteAddr < 0xC8000000; WriteAddr++)
    {
        *(uint8_t *)(WriteAddr) = 0x5A;
    }

    for (WriteAddr = 0x20000000; WriteAddr < 0xC8000000; WriteAddr++)
    {
        *(uint8_t *)(WriteAddr) = 0xA5;
        for (ReadAddr = 0x20000000; ReadAddr < 0xC8000000; ReadAddr++)
        {
            if (ReadAddr != WriteAddr)
            {
                if (*(uint8_t *)(ReadAddr) != 0x5A)
                {
                    return 5;
                    printk("*** SRAM test error, 8 bit ......\n");
                }
            }
        }
        *(uint8_t *)(WriteAddr) = 0x5A;
    }
    printk("*** SRAM test 128M 8bit, test end......\n");
#endif

#if CONFIG_SRAM_32BIT_32M_RW
    printk("*** SRAM test 128M 8bit write, read......\n");
    for (WriteAddr = 0x20000000; WriteAddr < SRAM_ADDR_END; WriteAddr++)
    {
        *(uint8_t *)(WriteAddr) = 0x5A;
    }

    for (WriteAddr = 0x20000000; WriteAddr < SRAM_ADDR_END; WriteAddr++)
    {
        *(uint8_t *)(WriteAddr) = 0xA5;
        for (ReadAddr = 0x20000000; ReadAddr < SRAM_ADDR_END; ReadAddr++)
        {
            if (ReadAddr != WriteAddr)
            {
                if (*(uint8_t *)(ReadAddr) != 0x5A)
                {
                    return 6;
                    printk("*** SRAM test error, 8 bit ......\n");
                }
            }
        }
        *(uint8_t *)(WriteAddr) = 0x5A;
    }
    printk("*** SRAM test 128M 8bit, test end......\n");
#endif
    //}
}

void main(void)
{
    int ret = 0;
    ret = sram_test();
    if (ret != 0)
    {
        printk("error, SRAM test failed!!\n");
    }
    return;
}
