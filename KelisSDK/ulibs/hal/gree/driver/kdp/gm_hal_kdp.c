/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_hal_kdp.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       kdp source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <gm_common.h>
#include <gm_soc.h>
#include <gm_hal_kdp.h>

#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF
#endif

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

void HAL_CNN_Start()
{
    SYSKDP->CTRL |= 1 << 0;
}

/*
bram_hfwd_rw_en: block ram halfword write/read enable, when this bit is enable, kdp sram will be modify by halfword, only the chosen halfword of 32bit data is effective, the select halfword reference to bram_HL_sel
0: disable
1:enable
*/
void HAL_CNN_Bram_hfwd_rw_en(char enable)
{
    if (enable == 1)
    {
        SYSKDP->CTRL |= 1 << 5;
    }
    else
    {
        SYSKDP->CTRL &= ~(1 << 5);
    }
}


/*
bram_HL_sel: block ram halfword data select
0:select high 16bit
1:select low 16bit
*/
void HAL_CNN_Bram_HL_sel(CNN_SEL_CONFIG sel)
{
    if (sel == SEL_HIGT_16BIT)
    {
        SYSKDP->CTRL &= ~(1 << 6);
    }
    else
    {
        SYSKDP->CTRL |= 1 << 6;
    }
}

void HAL_CNN_Wait()
{
    volatile int ready;

    ready = SYSKDP->STATUS;
    while (ready != 1)
    {
        ready = SYSKDP->STATUS;
    }
}

int HAL_DMA_Send(unsigned int pkt_length, unsigned int base_address)
{
    HAL_StreamFiFo_init((uint32_t *)base_address, pkt_length, AHB_WRAP4, UNLEN16, NORMAL_MODE);
    return 0;
}

void HAL_StreamFiFo_init(uint32_t *src, uint32_t len, uint8_t burst_type, uint8_t burst_unlen, uint8_t continue_mode)
{
    uint32_t level = 0;
    STRFIFO->CTRL = 0;

    if (burst_type == AHB_WRAP4)
    {
        level = 4 - 1;
    }
    else if (burst_type == AHB_WRAP8)
    {
        level = 8 - 1;
    }
    else if (burst_type == AHB_WRAP16)
    {
        level = 16 - 1;
    }
    else if (burst_type == AHB_SINGLE)
    {
        level = 1 - 1;
    }
    else if (burst_type == AHB_INCR)
    {
        switch (burst_unlen)
        {
            case UNLEN4    :
                level = 4   - 1;
                break;
            case UNLEN8    :
                level = 8   - 1;
                break;
            case UNLEN16   :
                level = 16  - 1;
                break;
            case UNLEN32   :
                level = 32  - 1;
                break;
            case UNLEN64   :
                level = 64  - 1;
                break;
            case UNLEN128  :
                level = 128 - 1;
                break;
            case UNLEN256  :
                level = 256 - 1;
                break;
            case UNLEN512  :
                level = 512 - 1;
                break;
            default :
                break;
        }
    }

    STRFIFO->SRC  = (uint32_t) src;
    if (continue_mode == CONTINUE_MODE)
    {
        STRFIFO->DATACNT = 0x00ffffff;
    }
    else
    {
        STRFIFO->DATACNT = len;
    }
    STRFIFO->CTRL = (1 << 31)             | //[31]   done_ie
                    (1 << 30)             | //[30]   over_ie
                    (1 << 29)             | //[29]   under_ie
                    (0 << 28)             | //[28]   full_ie
                    ((1024 - level) << 8) | //[17:8] dma_level
                    (burst_unlen << 5)    | //[7:5]  burst unlen : 2->len 16
                    (burst_type << 2)     | //[4:2]  burst type : 1->INCR
                    (1 << 1)              | //[1]    internal fifo enable
                    (1 << 0)              ; //[0]    STRFIFO enable


}

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
int HAL_IMP_Trans(int src_addr, int src_offset, int drc_addr, int drc_offset, int image_w, int image_h, int rd16_wr32_en, int rd32_wr16_en)
{
    unsigned int  tmp, i, b, c;
    unsigned int  imp_base;
    unsigned int  imp_cr_addr;
    unsigned int  imp_bpc_addr      ;
    unsigned int  imp_isr_addr      ;
    unsigned int  imp_fgmar_addr        ;
    unsigned int  imp_fgor_addr ;
    unsigned int  imp_bgor_addr     ;
    unsigned int  imp_bgmar_addr    ;
    unsigned int  imp_fgpfccr_addr      ;
    unsigned int  imp_fgclor_addr   ;
    unsigned int  imp_bgpfccr_addr;
    unsigned int  imp_bgclor_addr       ;
    unsigned int  imp_opfccr_addr    ;
    unsigned int  imp_ocolr_addr  ;
    unsigned int  imp_omar_addr   ;
    unsigned int  imp_oor_addr   ;
    unsigned int  imp_nlr_addr   ;
    unsigned int  imp_lwr_addr   ;
    unsigned int  imp_amtcr_addr   ;
    unsigned int  imp_ip_ver_addr  ;

    imp_base = 0x40024000;
    imp_cr_addr         = imp_base + 0x00;
    imp_isr_addr            = imp_base + 0x04;
    imp_fgmar_addr          = imp_base + 0x0C;
    imp_fgor_addr           = imp_base + 0x10;
    imp_bgor_addr           = imp_base + 0x18;
    imp_bgmar_addr          = imp_base + 0x14;
    imp_fgpfccr_addr            = imp_base + 0x1C;
    imp_fgclor_addr         = imp_base + 0x20;
    imp_bgpfccr_addr            = imp_base + 0x24;
    imp_bgclor_addr         = imp_base + 0x28;
    imp_opfccr_addr          = imp_base + 0x34;
    imp_ocolr_addr        = imp_base + 0x38;
    imp_omar_addr         = imp_base + 0x3C;
    imp_oor_addr         = imp_base + 0x40;
    imp_nlr_addr         = imp_base + 0x44;
    imp_lwr_addr         = imp_base + 0x48;
    imp_amtcr_addr         = imp_base + 0x4c;
    //release clock and reset
    HW32_REG(0x40000080) = 0xFFFFFFFF;
    HW32_REG(0x40000070) = 0xFFFFFFFF;
    //config fg memory addr
    HW32_REG(imp_fgmar_addr) = src_addr;
    //config fg offset = 0
    HW32_REG(imp_fgor_addr) = src_offset;
    //config fg color mode = argb8888
    //    HW32_REG(imp_fgpfccr_addr)= 0x0;

    if (rd16_wr32_en)
    {
        HW32_REG(imp_fgpfccr_addr) = 0x2;
    }
    else if (rd32_wr16_en)
    {
        HW32_REG(imp_fgpfccr_addr) = 0x0;
    }
    //config out memory addr

    if (rd16_wr32_en)
    {
        HW32_REG(imp_opfccr_addr) = 0;
    }
    else if (rd32_wr16_en)
    {
        HW32_REG(imp_opfccr_addr) = 2;
    }

    //config out memory addr
    HW32_REG(imp_omar_addr) = drc_addr;
    //config out memory offset 0
    HW32_REG(imp_oor_addr) = drc_offset;
    //config out pl/nl
    HW32_REG(imp_nlr_addr) = (image_w << 16) + image_h;
    //    HW32_REG(imp_cr_addr)=0x00001;
    HW32_REG(imp_cr_addr) = (rd16_wr32_en << 20) + (rd32_wr16_en << 21) + (1 << 16) + 1;
    while (HW32_REG(imp_cr_addr) & 1 != 0);
    return 1;
}

