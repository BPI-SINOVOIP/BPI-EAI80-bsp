/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <dma.h>
#include <device.h>
#include <watchdog.h>
#include <gpio.h>
#include "board.h"
#include <misc/printk.h>
#include <uart.h>
#include <gm_hal_uart.h>
#include "pl230_udma.h"


//uint8_t ReservedArray[512] __attribute__((at(0x20000400)));
//uint8_t ReservedArray[768] __attribute__((at(0x200003C0)));

#if 0
    PL230_UDMA_Control_T   UDMA_Control  __attribute__((at(0x20000400)));
#else
    /* Pointers to DMA reserved ram area */
    PL230_UDMA_Control_T   UDMA_Control __attribute__((aligned(512)));
#endif

void UDMA_init(uint8_t ch, uint8_t sel, uint8_t high, uint8_t alter, uint32_t *src_end, uint32_t *dst_end, int len, uint8_t src_tr_width, uint8_t mem2mem, uint8_t no_inc, uint8_t inten, uint8_t burst_len, uint8_t scatter_gather)
{
    uint8_t burst_en;

    ch  &= 0xf;
    len &= 0xffffff; //the max len

    if (burst_len == NO_BURST)
    {
        burst_en = 0;
    }
    else
    {
        burst_en = 1;
    }


    if (alter)
    {
        //Alternate control data struct config
        UDMA_Control.Alter_CH[ch].SRC_END = (uint32_t) src_end;

        UDMA_Control.Alter_CH[ch].DES_END = (uint32_t) dst_end;

        UDMA_Control.Alter_CH[ch].CTRL = ((len - 1) << 8)     | //[31:8] n_minus_1
                                         (src_tr_width << 6)  | //[7:6] src size
                                         (burst_len << 4)     | //[5:4] R_power
                                         (burst_en << 3)      | //[3] burst
                                         (0x0 << 0);           //[2:0] trans

        if (scatter_gather)
        {
            UDMA_Control.Alter_CH[ch].CTRL |= (5 << 0);    //Memory scatter-gather using the alternate data structure
        }
        else if (mem2mem)
        {
            UDMA_Control.Alter_CH[ch].CTRL |= (2 << 0);    //auto request
        }
        else
        {
            UDMA_Control.Alter_CH[ch].CTRL |= (1 << 0);    //base trans
        }

    }
    else
    {
        //Primary control data struct config
        UDMA_Control.Prim_CH[ch].SRC_END = (uint32_t) src_end;

        UDMA_Control.Prim_CH[ch].DES_END = (uint32_t) dst_end;

        UDMA_Control.Prim_CH[ch].CTRL = ((len - 1) << 8)     | //[31:8] n_minus_1
                                        (src_tr_width << 6)  | //[7:6] src size
                                        (burst_len << 4)     | //[5:4] R_power
                                        (burst_en << 3)      | //[3] burst
                                        (0 << 0);             //[2:0] base trans

        if (scatter_gather)
        {
            UDMA_Control.Prim_CH[ch].CTRL |= (4 << 0);    //Memory scatter-gather using the primary data structure
        }
        else if (mem2mem)
        {
            UDMA_Control.Prim_CH[ch].CTRL |= (2 << 0);    //auto request
        }
        else
        {
            UDMA_Control.Prim_CH[ch].CTRL |= (1 << 0);    //base trans
        }
    }


    //Primary control data struct base address
    UDMA->CTRL_BASE_PTR = (uint32_t)(&(UDMA_Control.Prim_CH[0]));

    //Alternate control data struct base address
    UDMA->ALT_CTRL_BASE_PTR = (uint32_t)(&(UDMA_Control.Alter_CH[0]));

    if (mem2mem)   //memory to memory
    {
        //Channel request mask set : disable the channel req
        UDMA->CHNL_REQ_MASK_SET = (1 << ch);
    }
    else
    {
        //Channel request mask clear : enable the channel req
        UDMA->CHNL_REQ_MASK_CLR = (1 << ch);
    }

    //use the Alter data struct or Primary data struct
    if (alter)
    {
        UDMA->CHNL_PRI_ALT_SET = (1 << ch);
    }
    else
    {
        UDMA->CHNL_PRI_ALT_CLR = (1 << ch);
    }

    //set the high priority of the channel
    if (high)
    {
        UDMA->CHNL_PRIORITY_SET = (1 << ch);
    }
    else
    {
        UDMA->CHNL_PRIORITY_CLR = (1 << ch);
    }

    //Bus error clear
    UDMA->ERR_CLR = (1 << 0);

    //master handshake select
    if (ch < 8)
    {
        sel &= 0xf;
        UDMA->CH_CFG0 &= ~(0xf << (ch * 4));
        UDMA->CH_CFG0 |= (sel << (ch * 4));
    }
    else
    {
        sel &= 0xf;
        UDMA->CH_CFG1 &= ~(0xf << ((ch - 8) * 4));
        UDMA->CH_CFG1 |= (sel << ((ch - 8) * 4));
    }

    //config the src/dst address change or not
    UDMA->NO_INC &= ~(3 << (ch * 2));
    UDMA->NO_INC |= (no_inc << (ch * 2));

    if (inten)
    {
        UDMA->IE |= (1 << ch);
    }
    else
    {
        UDMA->IE &= ~(1 << ch);
    }

    //enable the channel
    UDMA->CHNL_ENABLE_SET = (1 << ch);

    //enable udma
    UDMA->DMA_CFG |= (1 << 0);

    if (mem2mem)   //memory to memory
    {
        UDMA->CHNL_SW_REQUEST |= (1 << ch);
    }
}


void control_struct_init(PL230_UDMA_CH_T *p, uint32_t *src_end, uint32_t *dst_end, int len, uint8_t src_tr_width, uint8_t mem2mem, uint8_t burst_len)
{
    uint8_t burst_en;

    len &= 0xffffff; //the max len

    if (burst_len == NO_BURST)
    {
        burst_en = 0;
    }
    else
    {
        burst_en = 1;
    }

    //Alternate control data struct config
    p->SRC_END = (uint32_t) src_end;

    p->DES_END = (uint32_t) dst_end;

    p->CTRL = ((len - 1) << 8)     | //[31:8] n_minus_1
              (src_tr_width << 6)  | //[7:6] src size
              (burst_len << 4)     | //[5:4] R_power
              (burst_en << 3)      | //[3] burst
              (0x0 << 0);           //[2:0] trans

    //if(mem2mem)
    //  p->CTRL |= (2<<0); //auto request
    //else
    //  p->CTRL |= (1<<0); //base trans

    p->CTRL |= (5 << 0); //Memory scatter-gather using the alternate data structure
}



void wait_udma_done(uint8_t ch, uint8_t alter)
{
#if 0
    if (alter)
    {
        while ((UDMA_Control.Alter_CH[ch].CTRL & 0x7)); //wait CTRL[2:0] to be 0
    }
    else
    {
        while ((UDMA_Control.Prim_CH[ch].CTRL & 0x7)); //wait CTRL[2:0] to be 0
    }
#else
    while (!(UDMA->INTFLAG & (1 << ch))); //wait done flag
    UDMA->INTFLAG &= ~(1 << ch);        //clear done flag
#endif
    printf("chnel %d is done\n", ch);
}

typedef struct
{
    __IO uint32_t CTRL     ;
    __IO uint32_t STATUE   ;
    __IO uint32_t RESERVED0;
    __IO uint32_t RESERVED1;
    __IO uint32_t DATD     ;
    __IO uint32_t VERSION  ;
} STRFIFO_Device_T;

//* Base addresses of STRFIFO Peripheral Control Registers */
#define STRFIFO_BASE                               (0x46001000U)

/* I2C Device Pointer */
#define STRFIFO                                    ((STRFIFO_Device_T *)STRFIFO_BASE)

#define HW32_REG(ADDRESS)  (*((volatile unsigned long  *)(ADDRESS)))


PL230_UDMA_CH_T ctrl_arry[8];

__IO uint32_t txmem[128];
__IO uint32_t rxmem[128];

__IO uint32_t txmem0[32];
__IO uint32_t rxmem0[32];

__IO uint32_t txmem1[32];
__IO uint32_t rxmem1[32];

__IO uint32_t txmem2[32];
__IO uint32_t rxmem2[32];

__IO uint32_t txmem3[32];
__IO uint32_t rxmem3[32];



void udma_reset_release(void)
{
    HW32_REG(0x40000070) |= (1 << 2);
}

void udma_clk_enable(void)
{
    HW32_REG(0x40000080) |= (1 << 2);
}


//int main(void)
int memory_scatter_gather_test(void)
{
    int i, len;

    printf("pl230_udma test start ! \n");

    for (i = 0; i < 32; i++)
    {
        txmem[i]  = 0x12345678 + i;
        txmem0[i] = 0xaaaaaaaa + i;
        txmem1[i] = 0xbbbbbbbb + i;
        txmem2[i] = 0xcccccccc + i;
        txmem3[i] = 0xdddddddd + i;
    }

    //STRFIFO->CTRL = (125<<8) |   //dma_level
    //                (1<<1)   |   //internal fifo enable
    //                (1<<0)   ;   //STRFIFO enable

    udma_clk_enable();
    udma_reset_release();

    len = 6;
    control_struct_init(&ctrl_arry[0], &txmem0[len - 1], &rxmem0[len - 1], len, DMA_WORD_MODE, DMA_NON_MEM2MEM, BURST4);

    len = 5;
    control_struct_init(&ctrl_arry[1], &txmem1[len - 1], &rxmem1[len - 1], len, DMA_WORD_MODE, DMA_NON_MEM2MEM, BURST4);

    len = 8;
    control_struct_init(&ctrl_arry[2], &txmem2[len - 1], &rxmem2[len - 1], len, DMA_WORD_MODE, DMA_NON_MEM2MEM, BURST4);

    len = 9;
    control_struct_init(&ctrl_arry[3], &txmem3[len - 1], &rxmem3[len - 1], len, DMA_WORD_MODE, DMA_NON_MEM2MEM, BURST4);
    ctrl_arry[3].CTRL &= ~(7 << 0);
    ctrl_arry[3].CTRL |= (2 << 0); //auto request
    //ctrl_arry[3].CTRL |= (1<<0); //base trans


    len = 4 * 4;
    //
    // memory to memory
    //
    //UDMA_init(DMA_CH11, DMA_DEV7, LOW_PRIORITY, PRIMARY, &txmem[len-1], &rxmem[len-1], 4*8, DMA_WORD_MODE, DMA_MEM2MEM, DST_NO_CHANGE, INT_ENABLE, BURST4);
    UDMA_init(DMA_CH11, DMA_DEV7, LOW_PRIORITY, PRIMARY, &(ctrl_arry[3].RESERVED), &(UDMA_Control.Alter_CH[DMA_CH11].RESERVED), len, DMA_WORD_MODE, DMA_MEM2MEM, ALL_CHANGE, INT_ENABLE, BURST4, SCATTER_GATHER);


    //
    // memory to device
    //
    //UDMA_init(DMA_CH11, DMA_DEV4, LOW_PRIORITY, PRIMARY, &txmem[len-1], &(STRFIFO->DATD), 115, DMA_WORD_MODE, DMA_NON_MEM2MEM, DST_NO_CHANGE, INT_ENABLE, BURST4);
    //UDMA_init(DMA_CH11, DMA_DEV4, LOW_PRIORITY, ALTER, &txmem[len-1], &(STRFIFO->DATD), 115, DMA_WORD_MODE, DMA_NON_MEM2MEM, DST_NO_CHANGE, INT_ENABLE, BURST4);

    //
    // device to memory
    //
    //UDMA_init(DMA_CH11, DMA_DEV4, LOW_PRIORITY, PRIMARY, &(STRFIFO->DATD), &txmem[len-1], 115, DMA_WORD_MODE, DMA_NON_MEM2MEM, SRC_NO_CHANGE, INT_ENABLE, BURST4);


    /* Evan: make sure the irq is not set by the driver layer, or wait_udma_done() will never success! */
    wait_udma_done(DMA_CH11, PRIMARY);
    //wait_udma_done(DMA_CH11, ALTER);

#if 1
    len = 6;
    for (i = 0; i < len; i++)
    {
        if (txmem0[i] == rxmem0[i])
        {
            printf("PASS : i=%x, txmem0=%x, rxmem0=%x\n", i, txmem0[i], rxmem0[i]);
        }
        else
        {
            printf("ERROR : i=%x, txmem0=%x, rxmem0%x\n", i, txmem0[i], rxmem0[i]);
        }
    }
    //----------------------------------------------------------------------------
    len = 5;
    for (i = 0; i < len; i++)
    {
        if (txmem1[i] == rxmem1[i])
        {
            printf("PASS : i=%x, txmem1=%x, rxmem1=%x\n", i, txmem1[i], rxmem1[i]);
        }
        else
        {
            printf("ERROR : i=%x, txmem1=%x, rxmem1%x\n", i, txmem1[i], rxmem1[i]);
        }
    }
    //----------------------------------------------------------------------------
    len = 8;
    for (i = 0; i < len; i++)
    {
        if (txmem2[i] == rxmem2[i])
        {
            printf("PASS : i=%x, txmem2=%x, rxmem2=%x\n", i, txmem2[i], rxmem2[i]);
        }
        else
        {
            printf("ERROR : i=%x, txmem2=%x, rxmem2%x\n", i, txmem2[i], rxmem2[i]);
        }
    }
    //----------------------------------------------------------------------------
    len = 9;
    for (i = 0; i < len; i++)
    {
        if (txmem3[i] == rxmem3[i])
        {
            printf("PASS : i=%x, txmem3=%x, rxmem3=%x\n", i, txmem3[i], rxmem3[i]);
        }
        else
        {
            printf("ERROR : i=%x, txmem3=%x, rxmem3%x\n", i, txmem3[i], rxmem3[i]);
        }
    }
#endif

    printf("pl230_udma test end ! \n");

    //UartEndSimulation(); /* Simulation stop */

    return 0;
}

void main(void)
{
    int ret = -1;//debug
    //int ret = TC_FAIL;

    ret |= memory_scatter_gather_test();

    return;
}

