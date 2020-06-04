/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <gm_hal_uart.h>
#include <gm_common.h>
#include <pinmux.h>

#ifdef RABBIT_NO_OS_SYSTEM
    #include <systick.h>
#endif

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

int debug_cnt = 0;
int debug_cnt_max = 10000;

#define printf(...)        do {} while (0)
//#define GM_SDRAM_DEBGU          1
#define GM_SDRAM_ERR              1
#ifdef GM_SDRAM_DEBGU
    #define sdram_pr_debug       printk
#else
    #define sdram_pr_debug(...)  do{}while(0)
#endif
#ifdef GM_SDRAM_ERR
    #define sdram_pr_err         printk
#else
    #define sdram_pr_err(...)    do{}while(0)
#endif

//#define  SDR_32B_MODE
#ifdef  SDR_32B_MODE
    #define SDRAM_DQ_Mode      0x0     //0->32bits mode, 1->16bits mode, define in top.v first
#else
    #define SDRAM_DQ_Mode      0x1     //0->32bits mode, 1->16bits mode, define in top.v first
#endif


#ifdef SDRAM_8M_PM
    #define  sdram_d0_8m_4b   1
#endif
#ifdef SDRAM_16M_PM
    #define  sdram_d0_16m_4b  1
#endif
#ifdef SDRAM_32M_PM
    #define  sdram_d0_32m_4b  1
#endif
#ifdef SDRAM_64M_PM
    #define  sdram_d0_64m_4b  1
#endif
//#define  sdram_d0_4m_4b   1
//#define  sdram_d0_8m_4b   1
//#define  sdram_d0_16m_4b  1
//#define  sdram_d0_32m_4b  1
//#define  sdram_d0_64m_4b  1

//#define  sdram_d1_4m_4b   1
//#define  sdram_d1_8m_4b   1
//#define  sdram_d1_16m_4b  1
//#define  sdram_d1_32m_4b  1
//#define  sdram_d1_64m_4b  1

//for sdram initial
#ifdef SDR_32B_MODE
    #define       SDRAM_DQ_Mode             0x0     //0->32bits mode, 1->16bits mode, define in top.v first
#else
    #define       SDRAM_DQ_Mode             0x1     //0->32bits mode, 1->16bits mode, define in top.v first
#endif
#define       SDRAM_Cas_Latency         0x3     // Tcl  time    default is 3clk 2 -> 2clk

#ifdef SDR_200MHz
    #define       SDRAM_Ras_ActTime         0x5     // Tras time    9T
    #define       SDRAM_Ras_RefCyc          0xa     // Trfc time    15 clks
    #define       SDRAM_CtoC_dly            0x1     // Tc2c time    default is 0 0-> 1Tclk, 1-> 2Tclk
    #define       SDRAM_RtoR_dly            0x1     // Trrd delay   0-> 2cycles, 1->3cycles
    #define       SDRAM_RtoC_dly            0x3     // Trcd delay   0 -> 2clk 3 -> 5 clks
    #define       SDRAM_Ras_PreTime         0x3     // Trp  delay   0 -> 2clk 3 -> 5 clks
    #define       SDRAM_TurnAround_Mode     0x0     //SDRAM Controller Turn around 0 -> 2 cycle 1 -> 1 cycle
#else
    #ifdef SDR_160MHz
        #define       SDRAM_Ras_ActTime         0x3     // Tras time    default is 7 clks
        #define       SDRAM_Ras_RefCyc          0x7     // Trfc time    12 clks
        #define       SDRAM_CtoC_dly            0x0     // Tc2c time    default is 0 0-> 1Tclk, 1-> 2Tclk
        #define       SDRAM_RtoR_dly            0x1     // Trrd delay   0-> 2cycles, 1->3cycles
        #define       SDRAM_RtoC_dly            0x2     // Trcd delay   0 -> 2clk 3 -> 5 clks
        #define       SDRAM_Ras_PreTime         0x2     // Trp  delay   0 -> 2clk 3 -> 5 clks
        #define       SDRAM_TurnAround_Mode     0x0     //SDRAM Controller Turn around 0 -> 2 cycle 1 -> 1 cycle
    #else
        #define       SDRAM_Ras_ActTime         0x3     // Tras time    default is 7 clks
        #define       SDRAM_Ras_RefCyc          0x5     // Trfc time    default is 10 clks
        #define       SDRAM_CtoC_dly            0x0     // Tc2c time    default is 0 0-> 1Tclk, 1-> 2Tclk
        #define       SDRAM_RtoR_dly            0x1     // Trrd delay   0-> 2cycles, 1->3cycles
        #define       SDRAM_RtoC_dly            0x1     // Trcd delay   0 -> 2clk 3 -> 5 clks
        #define       SDRAM_Ras_PreTime         0x1     // Trp  delay   0 -> 2clk 3 -> 5 clks
        #define       SDRAM_TurnAround_Mode     0x0     //SDRAM Controller Turn around 0 -> 2 cycle 1 -> 1 cycle
    #endif
#endif

#define       SDRAM_OE_Post_En          0x0     //SDRAM OE post 1-> enable 0->disable(default)
#define       SDRAM_OE_Pre_En           0x0     //SDRAM OE pre  1-> enable 0->disable(default)
#define       SDRAM_Burst_Length        0x2+SDRAM_DQ_Mode   //3'b010->4 bl, 3'b011->8bl
#define       SDRAM_Dimm1_En            0x0
#define       SDRAM_Dimm_Bank_Map       0x1
#define       SDRAM_Dimm_Map            0x4     //for device set
#define       SDRAM_Conti_Setting       0x0     //SDRAM Controller Main Arbt Conti Mode 1 cycle
//SDRAM Controller SubB Arbt Conti Mode 1 cycle
//SDRAM Controller SubA Arbt Conti Mode 1 cycle
#define       SDRAM_CPU_Req_Prio        0x1     //CPU priority set 1 High priority
#define       SDRAM_Main_Arbt_Mode      0x0     //SDRAM Controller Main Arbt Priority Mode
#define       SDRAM_SubA_Arbt_Mode      0x0     //SDRAM Controller Sub Arbt Priority Mode
#define       SDRAM_SubB_Arbt_Mode      0x0     //SDRAM Controller Sub-B Arbt Priority Mode
//--------FOR LOAD  PARAMTER
#define       SDRAM_Row_Map             0x4         // 2 banks, 8 bit column addr   3'h0 for load
// 4 banks, 8 bit column addr   3'h4
// 4 banks, 9 bit column addr   3'h5
// 4 banks, 10 bit column addr  3'h6
#define       SDRAM_Row1_Base           0x01000000      //d1_2m_2b              32'h0040_0000;
//d1_4m_4b  12/8    32'h0100_0000;
//d1_8m_4b  12/9    32'h0200_0000;
//d1_16m_4b 12/10   32'h0400_0000;
#define       SDRAM_WRDLock_Timer_En    0x0     //Write Dead Lock Timer: 1-> Enable, 0-> Disable
#define       SDRAM_WRDLock_Exit_En     0x0     //Write Dead Auto-Exit: 1-> Enable, 0-> Disable

#ifndef FPGA
    #define       SDRAM_Refresh_Time        0x0d
#else
    #define       SDRAM_Refresh_Time        0x05
#endif
#define       SDRAM_Clk_Tree_Dly        0x9
#define       SDRAM_Interleave          0x0     // SDRAM Linear Mode
#define       SDRAM_Read_Dly            0x0     // 5'h4;
#define       SDRAM_CTRL_REG            0x40039280

#define MultiBitCfg(reg,bit_vld,start_bit,val) (HW32_REG(reg) =(HW32_REG(reg)& ~(bit_vld<<start_bit)) | (val<<start_bit))
//SDRAM End

void SDRAM_Wait_Idle(void);
void DIMM_Set_Map_Mode(void);
void SDRAM_Set_TimeParam(void);
void SDRAM_NOP(void);
void SDRAM_PrechargeAll(void);
void SDRAM_Mode_Set(void);
void SDRAM_Refresh(void);
void SDRAM_Set_DIMM_Map(unsigned int dimm1_en, unsigned int dimm0_banks, unsigned int dimm0_map, unsigned int dimm1_banks, unsigned int dimm1_map);
void SDRAM_Set_Refresh_Timer(unsigned int ref_init);
void dram_init(void);
void SDRAM_Enable(void);
void sdram_pinmux_configure(void);
void enable_pll_200Mhz(void);
void enable_pll_100Mhz(void);
void enable_pll_133Mhz(void);

void SDRAM_Wait_Idle(void)
{
    unsigned int flag;
    unsigned int temp_mem;
    printf("*** Waiting SDRAM idle........\n");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
    flag = 0x1;
    while (flag)
    {
        temp_mem = HW32_REG(SDRAM_CTRL_REG);

        if ((temp_mem & 0x7) == 0x0)
        {
            flag = 0;
        }
        else
        {
            flag = 1;
        }
    }
}


void DIMM_Set_Map_Mode(void)
{

    unsigned int    dimm0_banks;
    unsigned int    dimm0_map;
    unsigned int    dimm1_en;
    unsigned int    dimm1_banks;
    unsigned int    dimm1_map;

    unsigned int    sdram_row0_map;
    unsigned int    sdram_row1_base;
    unsigned int    sdram_row1_map;
    unsigned int    SDRAM_DIMM1_BASE;

    /****************************************************************
    ----           SDRAM Address Split Mode [3:0].             ------
    ----           [3:2]: Row Address Width:                   ------
    ----           00 ->11 bits                                ------
    ----           01 ->12 bits (Default)                      ------
    ----           10 ->13bits                                 ------
    ----           11 ->Reserved                               ------
    ----           [1:0]: Column Address Width                 ------
    ----           00 -> 8 bits (Default)                      ------
    ----           01 -> 9 bits                                ------
    ----           10 -> 10 bits                               ------
    ----           11 ->Reserved                               ------
    *****************************************************************/

#ifdef sdram_d0_64m_4b  // 13/10 --> Split mode[3:0]='b10_10
    dimm0_banks                     =       0x1;
    dimm0_map                       =       0xa;
#else
#ifdef sdram_d0_32m_4b  // 13/09  --> Split mode[3:0]='b10_01
    dimm0_banks                     =       0x1;
    dimm0_map                       =       0x9;
#else
#ifdef sdram_d0_16m_4b  // 12/9  --> Split mode[3:0]='b01_01
    dimm0_banks                     =       0x1;
    dimm0_map                       =       0x5;
#else
#ifdef sdram_d0_8m_4b   // 12/8  --> Split mode[3:0]='b01_00
    dimm0_banks                     =       0x1;
    dimm0_map                       =       0x4;
#else
#ifdef sdram_d0_4m_4b   // 11/8  --> Split mode[3:0]='b00_00
    dimm0_banks                     =       0x1;
    dimm0_map                       =       0x0;
#else
    dimm0_banks                     =       0x0;
    dimm0_map                       =       0x0;
#endif
#endif
#endif
#endif
#endif

#ifdef sdram_d1_64m_4b  // 12/10
    dimm1_en                        =       0x1;
    dimm1_banks                     =       0x1;
    dimm1_map                       =       0x6;
#else
#ifdef sdram_d1_32m_4b  // 12/10
    dimm1_en                        =       0x1;
    dimm1_banks                     =       0x1;
    dimm1_map                       =       0x6;
#else
#ifdef sdram_d1_16m_4b  // 12/10
    dimm1_en                        =       0x1;
    dimm1_banks                     =       0x1;
    dimm1_map                       =       0x6;
#else
#ifdef sdram_d1_8m_4b   // 12/9
    dimm1_en                        =       0x1;
    dimm1_banks                     =       0x1;
    dimm1_map                       =       0x5;
#else
#ifdef sdram_d1_4m_4b   // 12/8
    dimm1_en                        =       0x1;
    dimm1_banks                     =       0x1;
    dimm1_map                       =       0x4;
#else
#ifdef sdram_d1_2m_2b
    dimm1_en                        =       0x1;
    dimm1_banks                     =       0x0;
    dimm1_map                       =       0x0;
#else
    dimm1_en                        =       0x0;
    dimm1_banks                     =       0x0;
    dimm1_map                       =       0x0;
#endif  //`ifdef sdram_d1_2m_2b
#endif  //`ifdef sdram_d1_4m_4b // 12/8
#endif  //`ifdef sdram_d1_8m_4b // 12/9
#endif  //`ifdef sdram_d1_16m_4b// 12/10
#endif
#endif

    SDRAM_Set_DIMM_Map(dimm1_en, dimm0_banks, dimm0_map, dimm1_banks, dimm1_map);

}//end DIMM_Set_Map_Mode


void    SDRAM_Set_DIMM_Map(unsigned int dimm1_en, unsigned int dimm0_banks, unsigned int dimm0_map, unsigned int dimm1_banks, unsigned int dimm1_map)
{
    unsigned int      dimm1_active  = dimm1_en;
    unsigned int      dimm_banks    = dimm0_banks;
    unsigned int      map_mode      = dimm0_map;
    //unsigned int    dimm1_banks;
    unsigned int      map1_mode     = dimm1_map;
    unsigned int      temp;

    printf("Set SDRAM Address Mapping Split Mode:\n");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
    if (dimm1_active)
    {
        printf("External SDRAM DIMM1 is actived\n");
    }
    else
    {
        printf("External SDRAM DIMM1 is unactived\n");
    }

    if (dimm_banks)
    {
        printf("The SDRAM DIMM0 is four banks\n");
    }
    else
    {
        printf("The SDRAM DIMM0 is two banks\n");

    }

    switch (map_mode & 0x3)
    {
        case 0x0:
        {
            printf("The SDRAM DIMM0 Column Address Width is 8-bits\n");
            for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
            {
                __NOP;
                __NOP;
            }
        }
        break;
        case 0x1:
            printf("The SDRAM DIMM0 Column Address Width is 9-bits\n");
            for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
            {
                __NOP;
                __NOP;
            }
            break;
        case 0x2:
            printf("The SDRAM DIMM0 Column Address Width is 10-bits\n");
            for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
            {
                __NOP;
                __NOP;
            }
            break;
        default:
            printf("DRAM_rsim_Error...............\n");
            for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
            {
                __NOP;
                __NOP;
            }
            break;
    }

    switch (map_mode & 0xc)
    {
        case 0x0:
            printf("The SDRAM DIMM0 Row Address Width is 11-bits\n");
            for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
            {
                __NOP;
                __NOP;
            }
            break;
        case 0x4:
            printf("The SDRAM DIMM0 Row Address Width is 12-bits\n");
            for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
            {
                __NOP;
                __NOP;
            }
            break;
        case 0x8:
            printf("The SDRAM DIMM0 Row Address Width is 13-bits\n");
            for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
            {
                __NOP;
                __NOP;
            }
            break;
        default:
            printf("DRAM_rsim_Error...............\n");
            for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
            {
                __NOP;
                __NOP;
            }
            break;
    }

    if (dimm1_active)  //for GM6721 dimm1_map is meaningless
    {
        if (dimm1_banks)
        {
            printf("The SDRAM DIMM1 is four banks\n");

        }

        else
        {
            printf("The SDRAM DIMM1 is two banks\n");

        }
        switch (map1_mode & 0x3)
        {
            case 0x0:
                printf("The SDRAM DIMM1 Column Address Width is 8-bits\n");
                for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
                {
                    __NOP;
                    __NOP;
                }
                break;
            case 0x1:
                printf("The SDRAM DIMM1 Column Address Width is 9-bits\n");
                for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
                {
                    __NOP;
                    __NOP;
                }
                break;
            case 0x2:
                printf("The SDRAM DIMM1 Column Address Width is 10-bits\n");
                for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
                {
                    __NOP;
                    __NOP;
                }
                break;
            default:
                printf("DRAM_rsim_Error...............\n");
                for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
                {
                    __NOP;
                    __NOP;
                }
                break;
        }

        switch (map1_mode & 0xc)
        {
            case 0x0:
                printf("The SDRAM DIMM1 Row Address Width is 11-bits\n");
                for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
                {
                    __NOP;
                    __NOP;
                }
                break;
            case 0x4:
                printf("The SDRAM DIMM1 Row Address Width is 12-bits\n");
                for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
                {
                    __NOP;
                    __NOP;
                }
                break;
            default:
                printf("DRAM_rsim_Error...............\n");
                for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
                {
                    __NOP;
                    __NOP;
                }
                break;
        }
    }

    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG) = (temp & 0x0000ffff) | ((SDRAM_TurnAround_Mode << 31) | (SDRAM_OE_Post_En << 30) |
                               (SDRAM_OE_Pre_En << 29) | (SDRAM_DQ_Mode << 28) | (dimm1_active << 27) |
                               (0x0 << 26) | (dimm1_banks << 25) | (dimm_banks << 24) |
                               (map1_mode << 20) | (map_mode << 16)
                                                     );
}

void    SDRAM_Set_TimeParam()
{
    unsigned int temp;

    printf("*** SDRAM set timing parameter......\n");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }

    temp = HW32_REG(SDRAM_CTRL_REG + 0x4);
    HW32_REG(SDRAM_CTRL_REG + 0x4)    = (temp & 0xffff0000) | ((SDRAM_Ras_ActTime << 12) | (SDRAM_Ras_RefCyc << 8) |
                                        (SDRAM_CtoC_dly << 7) | (SDRAM_RtoR_dly << 6) |
                                        (SDRAM_RtoC_dly << 2) | (SDRAM_Ras_PreTime));
}
void SDRAM_NOP()
{
    unsigned int temp;
    SDRAM_Wait_Idle();
    printf("*** Send NOP Command to SDRAMs ......\n");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
    temp = HW32_REG(SDRAM_CTRL_REG + 0x0);
    HW32_REG(SDRAM_CTRL_REG + 0x0) = (temp & 0xffffff00) | 0x0;
}

void    SDRAM_PrechargeAll()
{
    unsigned int temp;
    SDRAM_Wait_Idle();
    printf("*** Precharging All Banks of SDRAMs ......\n");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
    temp = HW32_REG(SDRAM_CTRL_REG + 0x0);
    HW32_REG(SDRAM_CTRL_REG + 0x0) = (temp & 0xffffff00) | 0x2;
}

void SDRAM_Mode_Set()
{
    unsigned int temp;
    SDRAM_Wait_Idle();
    printf("*** Setting SDRAM mode ......\n");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
    temp = HW32_REG(SDRAM_CTRL_REG + 0x4);
    HW32_REG(SDRAM_CTRL_REG + 0x4) = (temp & 0xff00ffff) | ((SDRAM_Cas_Latency << 20) | (SDRAM_Interleave << 19) | (SDRAM_Burst_Length << 16)) ;
    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG)         = (temp & 0xffffff00) | 0x1;
}

void SDRAM_Refresh()
{
    unsigned int temp;

    SDRAM_Wait_Idle();
    printf("*** SDRAM auto refreshing ......\n");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG) = ((temp & 0xffffff00) | 0x3);
}

void SDRAM_Ref_Disable(void)
{
    SDRAM_Wait_Idle();
    printf("*** SDRAM Disable Auto-Refresh Timer......\n");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
    SDRAM_Set_Refresh_Timer(0x0);
    SDRAM_Wait_Idle();
}

void SDRAM_Set_Refresh_Timer(unsigned int ref_init)
{
    unsigned int    temp;
    printf("*** SDRAM set refresh timer......\n");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG) = ((temp & 0xffff00ff) | (ref_init << 8));
}
void SDRAM_Enable(void)
{
    unsigned int    temp;
    temp = HW32_REG(0x40000070);
    HW32_REG(0x40000070) = (temp | (0x80));
    temp = HW32_REG(0x40000080);
    HW32_REG(0x40000080) = (temp | (0x80));
}


void sdram_pinmux_configure()
{
    HW32_REG(0x40000298) = 0xffffffff;
    HW32_REG(0x4000029c) = 0xffffffff;
    HW32_REG(0x400002a0) = 0xffffffff;
    HW32_REG(0x400002a4) = 0xffffffff;
    HW32_REG(0x400002a8) = 0xffffffff;
    HW32_REG(0x400002ac) = 0xffffffff;
    HW32_REG(0x400002b0) = 0xffffffff;
    HW32_REG(0x400002b4) = 0xffffffff;
    HW32_REG(0x400002b8) = 0xffffffff;
    HW32_REG(0x400002bc) = 0xffffffff;
    HW32_REG(0x400002c0) = 0xffffffff;

    //SDRAM inteface
    HW32_REG(0x400002C4) |= 1 << 0;
    HW32_REG(0x400002F0) |= 5 << 0;     //SDRAM_D[16]           XPA0
    HW32_REG(0x400002C4) |= 1 << 2;
    HW32_REG(0x400002F0) |= 5 << 4;     //SDRAM_D[17]           XPA1
    HW32_REG(0x400002C4) |= 1 << 4;
    HW32_REG(0x400002F0) |= 5 << 8;     //SDRAM_D[18]           XPA2
    HW32_REG(0x400002C4) |= 1 << 6;
    HW32_REG(0x400002F0) |= 5 << 12;    //SDRAM_D[19]           XPA3
    HW32_REG(0x400002C4) |= 1 << 8;
    HW32_REG(0x400002F0) |= 5 << 16;    //SDRAM_D[20]           XPA4
    HW32_REG(0x400002C4) |= 1 << 10;
    HW32_REG(0x400002F0) |= 5 << 20;    //SDRAM_D[21]           XPA5
    HW32_REG(0x400002C4) |= 1 << 12;
    HW32_REG(0x400002F0) |= 5 << 24;    //SDRAM_D[22]           XPA6
    HW32_REG(0x400002C4) |= 1 << 14;
    HW32_REG(0x400002F0) |= 5 << 28;    //SDRAM_D[23]           XPA7
    HW32_REG(0x400002C4) |= 1 << 16;
    HW32_REG(0x400002F4) |= 5 << 0;     //SDRAM_A[3]            XPA8
    HW32_REG(0x400002C4) |= 1 << 18;
    HW32_REG(0x400002F4) |= 5 << 4;     //SDRAM_A[2]            XPA9
    HW32_REG(0x400002C4) |= 1 << 20;
    HW32_REG(0x400002F4) |= 5 << 8;     //SDRAM_A[1]            XPA10
    HW32_REG(0x400002C4) |= 1 << 22;
    HW32_REG(0x400002F4) |= 5 << 12;    //SDRAM_A[0]            XPA11
    HW32_REG(0x400002C4) |= 1 << 24;
    HW32_REG(0x400002F4) |= 5 << 16;    //SDRAM_A[10]           XPA12
    HW32_REG(0x400002C4) |= 1 << 26;
    HW32_REG(0x400002F4) |= 5 << 20;    //SDRAM_BA[1]           XPA13
    HW32_REG(0x400002C4) |= 1 << 28;
    HW32_REG(0x400002F4) |= 5 << 24;    //SDRAM_BA[0]           XPA14
    HW32_REG(0x400002C4) |= 1 << 30;
    HW32_REG(0x400002F4) |= 5 << 28;    //SDRAM_SDNE[1]         XPA15
    HW32_REG(0x400002C8) |= 1 << 0;
    HW32_REG(0x400002F8) |= 5 << 0;     //SDRAM_SDNE[0]         XPB0
    HW32_REG(0x400002C8) |= 1 << 2;
    HW32_REG(0x400002F8) |= 5 << 4;     //SDRAM_SDNRAS          XPB1
    HW32_REG(0x400002C8) |= 1 << 4;
    HW32_REG(0x400002F8) |= 5 << 8;     //SDRAM_SDNCAS          XPB2
    HW32_REG(0x400002C8) |= 1 << 6;
    HW32_REG(0x400002F8) |= 5 << 12;    //SDRAM_SDNWE           XPB3
    HW32_REG(0x400002C8) |= 1 << 8;
    HW32_REG(0x400002F8) |= 5 << 16;    //SDRAM_NBL[0]          XPB4
    HW32_REG(0x400002C8) |= 1 << 12;
    HW32_REG(0x400002F8) |= 5 << 24;    //SDRAM_NBL[2]          XPB6
    HW32_REG(0x400002E4) |= 1 << 8 ;
    HW32_REG(0x40000330) |= 5 << 16;    //SDRAM_NBL[3]          XPI4
    HW32_REG(0x400002E4) |= 1 << 10;
    HW32_REG(0x40000330) |= 5 << 20;    //SDRAM_SDCKE[1]        XPI5
    HW32_REG(0x400002E4) |= 1 << 12;
    HW32_REG(0x40000330) |= 5 << 24;    //SDRAM_NBL[1]          XPI6
    HW32_REG(0x400002E4) |= 1 << 14;
    HW32_REG(0x40000330) |= 5 << 28;    //SDRAM_SDCLK           XPI7
    HW32_REG(0x400002E4) |= 1 << 16;
    HW32_REG(0x40000334) |= 5 << 0;     //SDRAM_SDCKE[0]        XPI8
    HW32_REG(0x400002E4) |= 1 << 18;
    HW32_REG(0x40000334) |= 5 << 4;     //SDRAM_A[12]           XPI9
    HW32_REG(0x400002E4) |= 1 << 20;
    HW32_REG(0x40000334) |= 5 << 8;     //SDRAM_A[11]           XPI10
    HW32_REG(0x400002E4) |= 1 << 22;
    HW32_REG(0x40000334) |= 5 << 12;    //SDRAM_A[9]            XPI11
    HW32_REG(0x400002E4) |= 1 << 24;
    HW32_REG(0x40000334) |= 5 << 16;    //SDRAM_A[8]            XPI12
    HW32_REG(0x400002E4) |= 1 << 26;
    HW32_REG(0x40000334) |= 5 << 20;    //SDRAM_A[7]            XPI13
    HW32_REG(0x400002E4) |= 1 << 28;
    HW32_REG(0x40000334) |= 5 << 24;    //SDRAM_A[6]            XPI14
    HW32_REG(0x400002E4) |= 1 << 30;
    HW32_REG(0x40000334) |= 5 << 28;    //SDRAM_A[5]            XPI15
    HW32_REG(0x400002E8) |= 1 << 0;
    HW32_REG(0x40000338) |= 5 << 0;     //SDRAM_A[4]            XPJ0
    HW32_REG(0x400002E8) |= 1 << 2;
    HW32_REG(0x40000338) |= 5 << 4;     //SDRAM_D[24]           XPJ1
    HW32_REG(0x400002E8) |= 1 << 4;
    HW32_REG(0x40000338) |= 5 << 8;     //SDRAM_D[25]           XPJ2
    HW32_REG(0x400002E8) |= 1 << 6;
    HW32_REG(0x40000338) |= 5 << 12;    //SDRAM_D[26]           XPJ3
    HW32_REG(0x400002E8) |= 1 << 8;
    HW32_REG(0x40000338) |= 5 << 16;    //SDRAM_D[27]           XPJ4
    HW32_REG(0x400002E8) |= 1 << 10;
    HW32_REG(0x40000338) |= 5 << 20;    //SDRAM_D[28]           XPJ5
    HW32_REG(0x400002E8) |= 1 << 12;
    HW32_REG(0x40000338) |= 5 << 24;    //SDRAM_D[29]           XPJ6
    HW32_REG(0x400002E8) |= 1 << 14;
    HW32_REG(0x40000338) |= 5 << 28;    //SDRAM_D[30]           XPJ7
    HW32_REG(0x400002E8) |= 1 << 16;
    HW32_REG(0x4000033C) |= 5 << 0;     //SDRAM_D[31]           XPJ8
    HW32_REG(0x400002E8) |= 1 << 18;
    HW32_REG(0x4000033C) |= 5 << 4;     //SDRAM_D[8]            XPJ9
    HW32_REG(0x400002E8) |= 1 << 20;
    HW32_REG(0x4000033C) |= 5 << 8;     //SDRAM_D[9]            XPJ10
    HW32_REG(0x400002E8) |= 1 << 22;
    HW32_REG(0x4000033C) |= 5 << 12;    //SDRAM_D[10]           XPJ11
    HW32_REG(0x400002E8) |= 1 << 24;
    HW32_REG(0x4000033C) |= 5 << 16;    //SDRAM_D[11]           XPJ12
    HW32_REG(0x400002E8) |= 1 << 26;
    HW32_REG(0x4000033C) |= 5 << 20;    //SDRAM_D[12]           XPJ13
    HW32_REG(0x400002E8) |= 1 << 28;
    HW32_REG(0x4000033C) |= 5 << 24;    //SDRAM_D[14]           XPJ14
    HW32_REG(0x400002E8) |= 1 << 30;
    HW32_REG(0x4000033C) |= 5 << 28;    //SDRAM_D[13]           XPJ15
    HW32_REG(0x400002EC) |= 1 << 0;
    HW32_REG(0x40000340) |= 5 << 0;     //SDRAM_D[15]           XPK0
    HW32_REG(0x400002EC) |= 1 << 2;
    HW32_REG(0x40000340) |= 5 << 4;     //SDRAM_D[0]            XPK1
    HW32_REG(0x400002EC) |= 1 << 4;
    HW32_REG(0x40000340) |= 5 << 8;     //SDRAM_D[2]            XPK2
    HW32_REG(0x400002EC) |= 1 << 6;
    HW32_REG(0x40000340) |= 5 << 12;    //SDRAM_D[1]            XPK3
    HW32_REG(0x400002EC) |= 1 << 8;
    HW32_REG(0x40000340) |= 5 << 16;    //SDRAM_D[3]            XPK4
    HW32_REG(0x400002EC) |= 1 << 10;
    HW32_REG(0x40000340) |= 5 << 20;    //SDRAM_D[4]            XPK5
    HW32_REG(0x400002EC) |= 1 << 12;
    HW32_REG(0x40000340) |= 5 << 24;    //SDRAM_D[5]            XPK6
    HW32_REG(0x400002EC) |= 1 << 14;
    HW32_REG(0x40000340) |= 5 << 28;    //SDRAM_D[6]            XPK7
    HW32_REG(0x400002EC) |= 1 << 16;
    HW32_REG(0x40000344) |= 5 << 0;     //SDRAM_D[7]            XPK8
}

void enable_pll_200Mhz(void)
{
    HW32_REG(0x40000180) = 1;  //enable PLL
    do {;}
    while ((HW32_REG(0x400001A4) & 1 << 4) == 0);
    HW32_REG(0x40000090) |= 3;
    printf("\n Switch clock to 200MHz");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
}

void enable_pll_150Mhz(void)
{
    HW32_REG(0x40000188) = 1;  //enable KDP PLL
    do {;}
    while ((HW32_REG(0x400001A4) & 1 << 5) == 0);
    HW32_REG(0x40000090) |= 2;
    printf("\n Switch clock to 150MHz");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
}

void enable_pll_100Mhz(void)
{
    HW32_REG(0x40000180) = 1;  //enable PLL
    do {;}
    while ((HW32_REG(0x400001A4) & 1 << 4) == 0);
    HW32_REG(0x40000090) |= 3;
    HW32_REG(0x40000090) |= 1 << 4;
    printf("\n Switch clock to 100MHz");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
}

void enable_pll_133Mhz(void)
{
    HW32_REG(0x40000188) = 1;  //enable PLL KDP
    do {;}
    while ((HW32_REG(0x400001A4) & 1 << 5) == 0);
    HW32_REG(0x40000090) |= 4;
    printf("\n Switch clock to 133MHz");
    for (debug_cnt = 0; debug_cnt < debug_cnt_max; debug_cnt++)
    {
        __NOP;
        __NOP;
    }
}

#if 1
void dram_cfg_restore(void)
{
    int err_code = 0;
    int t_i = 0;
    int i;
    SDRAM_Enable();  // release SDR clock & reset
    for (i = 0; i < 10; i++)
    {
        __NOP;    // Add delay for SDR clock stable
    }

    HW32_REG(0x40039280 + 0x20)    = 0x0; // set CKE=0
    sdram_pinmux_configure();

    DIMM_Set_Map_Mode();
    SDRAM_Set_TimeParam();
    SDRAM_NOP();
    SDRAM_NOP();

    HW32_REG(0x40039280 + 0x20)    = 0x1;
    HW32_REG(0x40039280) &= 0xfffffffc;
    HW32_REG(0x40039280) &= 0xfffffffc;
    for (i = 0; i < 10; i++)
    {
        __NOP;
        __NOP;
    }

    HW32_REG(0x40039280) |= 0x3;
    SDRAM_Mode_Set();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Wait_Idle();
    SDRAM_Set_Refresh_Timer(SDRAM_Refresh_Time);
    SDRAM_Wait_Idle();
}
#endif

void gm6721b_ddr_init(void)
{
    int err_code = 0;
    int t_i = 0;
    int i;
    sdram_pinmux_configure();
    SDRAM_Enable();
    SDRAM_Ref_Disable();
    DIMM_Set_Map_Mode();
    SDRAM_Set_TimeParam();
    SDRAM_NOP();
    SDRAM_NOP();
    SDRAM_NOP();
    SDRAM_PrechargeAll();
    SDRAM_Mode_Set();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Refresh();
    SDRAM_Wait_Idle();
    SDRAM_Set_Refresh_Timer(SDRAM_Refresh_Time);
    SDRAM_Wait_Idle();
}

void Enter_SDRAM_SelfRefresh(void)
{
    uint32_t i = 0;
    printk("\n Intend to enter self-refresh mode");
    HW32_REG(0x40039280 + 0x20)    |= (1 << 8);
    SDRAM_Refresh();

    for (i = 0; i < 1000; i++)
    {
        __NOP;
        __NOP;
        __NOP;
        __NOP;
        __NOP;
        __NOP;
    }
}

