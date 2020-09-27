/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//#include <ugelis.h>
//#include <misc/printk.h>

#include "stdio.h"
#include <gm_common.h>

//#define GM_SDRAM_DEBGU          1
#define GM_SDRAM_ERR              1
#ifdef GM_SDRAM_DEBGU
    #define sdram_pr_debug       printf
#else
    #define sdram_pr_debug(...)  do{}while(0)
#endif
#ifdef GM_SDRAM_ERR
    #define sdram_pr_err        printf
#else
    #define sdram_pr_err(...)    do{}while(0)
#endif

//#define  SDR_32B_MODE
#ifdef  SDR_32B_MODE
#define SDRAM_DQ_Mode      0x0     //0->32bits mode, 1->16bits mode, define in top.v first
#else 
#define SDRAM_DQ_Mode      0x1     //0->32bits mode, 1->16bits mode, define in top.v first
#endif

//#define  sdram_d0_4m_4b   1
//#define  sdram_d0_8m_4b   1
//#define  sdram_d0_16m_4b  1
#define  sdram_d0_32m_4b  1
//#define  sdram_d0_64m_4b  1

//#define  sdram_d1_4m_4b   1
//#define  sdram_d1_8m_4b   1
//#define  sdram_d1_16m_4b  1
//#define  sdram_d1_32m_4b  1
//#define  sdram_d1_64m_4b  1

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

#if 1
#define        SDRAM_Ras_RefCyc         0x7
#define        SDRAM_RtoC_dly           0x2
#define        SDRAM_Ras_PreTime        0x2
#else
#define        SDRAM_Ras_RefCyc         0x5
#define        SDRAM_RtoC_dly           0x1
#define        SDRAM_Ras_PreTime        0x1
#endif

//for sdram initial
//#define     SDRAM_DQ_Mode             0x1     //0->32bits mode, 1->16bits mode, define in top.v first
#define       SDRAM_Cas_Latency         0x3  	// Tcl  time    default is 3clk 2 -> 2clk
#define       SDRAM_Ras_ActTime      	0x3   	// Tras time    default is 7 clks
#define       SDRAM_CtoC_dly         	0x0   	// Tc2c time    default is 0 0-> 1Tclk, 1-> 2Tclk
#define       SDRAM_RtoR_dly         	0x1   	// Trrd delay   0-> 2cycles, 1->3cycles
#define       SDRAM_TurnAround_Mode   	0x0   	//SDRAM Controller Turn around 0 -> 2 cycle 1 -> 1 cycle
#define       SDRAM_OE_Post_En        	0x0   	//SDRAM OE post 1-> enable 0->disable(default)
#define       SDRAM_OE_Pre_En         	0x0   	//SDRAM OE pre  1-> enable 0->disable(default)
#define       SDRAM_Burst_Length      	0x2+SDRAM_DQ_Mode   //3'b010->4 bl, 3'b011->8bl
#define       SDRAM_Dimm1_En          	0x0
#define       SDRAM_Dimm_Bank_Map     	0x1
#define       SDRAM_Dimm_Map          	0x4   	//for device set
#define       SDRAM_Conti_Setting     	0x0  	//SDRAM Controller Main Arbt Conti Mode 1 cycle
                                                //SDRAM Controller SubB Arbt Conti Mode 1 cycle
                                                //SDRAM Controller SubA Arbt Conti Mode 1 cycle
#define       SDRAM_CPU_Req_Prio      	0x1   	//CPU priority set 1 High priority
#define       SDRAM_Main_Arbt_Mode    	0x0   	//SDRAM Controller Main Arbt Priority Mode
#define       SDRAM_SubA_Arbt_Mode    	0x0   	//SDRAM Controller Sub Arbt Priority Mode
#define       SDRAM_SubB_Arbt_Mode    	0x0   	//SDRAM Controller Sub-B Arbt Priority Mode
//--------FOR LOAD  PARAMTER
#define       SDRAM_Row_Map           	0x4     // 2 banks, 8 bit column addr   3'h0 for load
//#define       SDRAM_Row_Map           0x5   	// 2 banks, 8 bit column addr   3'h0 for load
                                                // 4 banks, 8 bit column addr   3'h4
                                                // 4 banks, 9 bit column addr   3'h5
                                                // 4 banks, 10 bit column addr  3'h6

#define       SDRAM_Row1_Base         	0x01000000  	//d1_2m_2b              32'h0040_0000;
                                                          //d1_4m_4b  12/8    32'h0100_0000;
                                                          //d1_8m_4b  12/9    32'h0200_0000;
                                                          //d1_16m_4b 12/10   32'h0400_0000;
#define       SDRAM_WRDLock_Timer_En  	0x0   	//Write Dead Lock Timer: 1-> Enable, 0-> Disable
#define       SDRAM_WRDLock_Exit_En   	0x0   	//Write Dead Auto-Exit: 1-> Enable, 0-> Disable

//#define       SDRAM_Refresh_Time      	0x0d
#define       SDRAM_Refresh_Time      	0x05
#define       SDRAM_Clk_Tree_Dly      	0x0
#define       SDRAM_Interleave        	0x0   	// SDRAM Linear Mode
#define       SDRAM_Read_Dly          	0x0   	// 5'h4;
#define 	  SDRAM_CTRL_REG		  	0x40039280

static void SDRAM_Wait_Idle(void);
static void DIMM_Set_Map_Mode(void);
static void SDRAM_Set_TimeParam(void);
static void SDRAM_NOP(void);
static void SDRAM_PrechargeAll(void);
static void SDRAM_Mode_Set(void);
static void SDRAM_Refresh(void	);
static void SDRAM_Set_DIMM_Map(unsigned int dimm1_en,unsigned int dimm0_banks,unsigned int dimm0_map,unsigned int dimm1_banks,unsigned int dimm1_map);
static void SDRAM_Set_Refresh_Timer(unsigned int ref_init);
void gm6721_sdram_init(void);

static void SDRAM_Wait_Idle(void) {
    unsigned int flag;
    unsigned int temp_mem;
    sdram_pr_debug("*** Waiting SDRAM idle........\n");

    flag = 0x1;
    while (flag)    {
            temp_mem = HW32_REG(SDRAM_CTRL_REG);

            if ((temp_mem&0x7) == 0x0)
            	flag = 0;
            else
            	flag = 1;
            }
}


static void DIMM_Set_Map_Mode(void){
    unsigned int 	dimm0_banks;
    unsigned int	dimm0_map;
    unsigned int    dimm1_en;
    unsigned int    dimm1_banks;
    unsigned int    dimm1_map;
    unsigned int 	sdram_row0_map;
    unsigned int  	sdram_row1_base;
    unsigned int 	sdram_row1_map;
    unsigned int 	SDRAM_DIMM1_BASE;

#ifdef sdram_d0_64m_4b  // 13/10
        sdram_row0_map          =       0x6;   // 4 banks, 10 bit column addr
        sdram_row1_base         =       0x04000000;
        if(SDRAM_DQ_Mode)
                SDRAM_DIMM1_BASE        =       0x02000000;
        
        else
                SDRAM_DIMM1_BASE        =       0x04000000;
        
        dimm0_banks                     =       0x1;
        dimm0_map                       =       0xa;
#else 
#ifdef sdram_d0_32m_4b  // 12/10
    sdram_row0_map          =       0x6;   // 4 banks, 10 bit column addr
    sdram_row1_base         =       0x04000000;
    if(SDRAM_DQ_Mode)
            SDRAM_DIMM1_BASE        =       0x02000000;

    else
            SDRAM_DIMM1_BASE        =       0x04000000;

    dimm0_banks                     =       0x1;
    //dimm0_map                     =       0x6;
    dimm0_map                       =       0x9;//hongze debug
    //dimm0_map                       =       0x5;//hongze debug

#else
#ifdef sdram_d0_16m_4b  // 12/10
    sdram_row0_map          =       0x6;   // 4 banks, 10 bit column addr
    sdram_row1_base         =       0x04000000;
    if(SDRAM_DQ_Mode)
            SDRAM_DIMM1_BASE        =       0x02000000;

    else
            SDRAM_DIMM1_BASE        =       0x04000000;

    dimm0_banks                     =       0x1;
    /*dimm0_map                     =       0x6;*/
    dimm0_map                       =       0x5;

#else
#ifdef sdram_d0_8m_4b   // 12/9
    sdram_row0_map          =       0x5;   // 4 banks, 9 bit column addr
    sdram_row1_base         =       0x02000000;
    if(SDRAM_DQ_Mode)
            SDRAM_DIMM1_BASE        =       0x01000000;

    else
            SDRAM_DIMM1_BASE        =       0x02000000;

    dimm0_banks                     =       0x1;
    dimm0_map                       =       0x4;//5 is bug

#else
#ifdef sdram_d0_4m_4b   // 12/8
    sdram_row0_map          =       0x4;   // 4 banks, 8 bit column addr
    sdram_row1_base         =       0x01000000;
    if(SDRAM_DQ_Mode)
            SDRAM_DIMM1_BASE        =       0x00800000;

    else
            SDRAM_DIMM1_BASE        =       0x01000000;

    dimm0_banks                     =       0x1;
    dimm0_map                       =       0x4;

#else
    sdram_row0_map          =       0x0;   // 2 banks, 8 bit column addr
    sdram_row1_base         =       0x00400000;
    if(SDRAM_DQ_Mode)
            SDRAM_DIMM1_BASE        =       0x00200000;

    else
            SDRAM_DIMM1_BASE        =       0x00400000;

    dimm0_banks                     =       0x0;
    dimm0_map                       =       0x0;

#endif
#endif
#endif
#endif
#endif

#ifdef sdram_d1_64m_4b  // 12/10
        dimm1_en                        =       0x1;
        sdram_row1_map                  =       0x6;   // 4 banks, 10 bit column addr
        dimm1_banks                     =       0x1;
        dimm1_map                       =       0x6;

#else
#ifdef sdram_d1_32m_4b  // 12/10
        dimm1_en                        =       0x1;
        sdram_row1_map                  =       0x6;   // 4 banks, 10 bit column addr
        dimm1_banks                     =       0x1;
        dimm1_map                       =       0x6;

#else
#ifdef sdram_d1_16m_4b  // 12/10
    dimm1_en                        =       0x1;
    sdram_row1_map                  =       0x6;   // 4 banks, 10 bit column addr
    dimm1_banks                     =       0x1;
    dimm1_map                       =       0x6;

#else
#ifdef sdram_d1_8m_4b   // 12/9
    dimm1_en                        =       0x1;
    sdram_row1_map                  =       0x5;   // 4 banks, 9 bit column addr
    dimm1_banks                     =       0x1;
    dimm1_map                       =       0x5;

#else
#ifdef sdram_d1_4m_4b   // 12/8
    dimm1_en                        =       0x1;
    sdram_row1_map                  =       0x4;   // 4 banks, 8 bit column addr
    dimm1_banks                     =       0x1;
    dimm1_map                       =       0x4;

#else
#ifdef sdram_d1_2m_2b
    dimm1_en                        =       0x1;
    sdram_row1_map                  =       0x0;   // 2 banks, 8 bit column addr
    dimm1_banks                     =       0x0;
    dimm1_map                       =       0x0;
#else
    dimm1_en                        =       0x0;
    sdram_row1_map                  =       0x0;   // 2 banks, 8 bit column addr
    dimm1_banks                     =       0x0;
    dimm1_map                       =       0x0;
#endif  //`ifdef sdram_d1_2m_2b
#endif  //`ifdef sdram_d1_4m_4b // 12/8
#endif  //`ifdef sdram_d1_8m_4b // 12/9
#endif  //`ifdef sdram_d1_16m_4b// 12/10
#endif
#endif
//sdram_row0_map  =       SDRAM_Row_Map   ;                       // 2 banks, 8 bit column addr
//sdram_row1_base =       SDRAM_Row1_Base ;
//dimm0_banks     =       SDRAM_Dimm_Bank_Map     ;
//dimm0_map       =       SDRAM_Dimm_Map          ;
//dimm1_en        =       SDRAM_Dimm1_En          ;

    SDRAM_Set_DIMM_Map(dimm1_en,dimm0_banks,dimm0_map,dimm1_banks,dimm1_map);
}//end DIMM_Set_Map_Mode


static void SDRAM_Set_DIMM_Map(unsigned int dimm1_en,unsigned int dimm0_banks,unsigned int dimm0_map,unsigned int dimm1_banks,unsigned int dimm1_map)
{
    unsigned int      dimm1_active 	= dimm1_en;
    unsigned int      dimm_banks	= dimm0_banks;
    unsigned int      map_mode 		= dimm0_map;
    //unsigned int 	  dimm1_banks;
    unsigned int      map1_mode		= dimm1_map;
    unsigned int 	  temp;

    sdram_pr_debug("Set SDRAM Address Mapping Split Mode:\n");
    if(dimm1_active)
        sdram_pr_debug("External SDRAM DIMM1 is actived\n");
    else
        sdram_pr_debug("External SDRAM DIMM1 is unactived\n");

    if(dimm_banks)
        sdram_pr_debug("The SDRAM DIMM0 is four banks\n");
    else
        sdram_pr_debug("The SDRAM DIMM0 is two banks\n");

    switch(map_mode&0x3) {
    case 0x0:
        sdram_pr_debug("The SDRAM DIMM0 Column Address Width is 8-bits\n");
        break;
    case 0x1:
        sdram_pr_debug("The SDRAM DIMM0 Column Address Width is 9-bits\n");
        break;
    case 0x2:
        sdram_pr_debug("The SDRAM DIMM0 Column Address Width is 10-bits\n");
        break;
    default:
        sdram_pr_err("DRAM_rsim_Error...............\n");
        break;
    }

    switch (map_mode&0xc) {
    case 0x0:
        sdram_pr_debug("The SDRAM DIMM0 Row Address Width is 11-bits\n");
        break;
    case 0x4:
        sdram_pr_debug("The SDRAM DIMM0 Row Address Width is 12-bits\n");
        break;
    case 0x8:
        sdram_pr_debug("The SDRAM DIMM0 Row Address Width is 13-bits\n");
        break;
    default:
        sdram_pr_err("DRAM_rsim_Error...............\n");
        break;
    }

    if(dimm1_active) {
        if(dimm1_banks)
            sdram_pr_debug("The SDRAM DIMM1 is four banks\n");
        else
            sdram_pr_debug("The SDRAM DIMM1 is two banks\n");

        switch (map1_mode&0x3) {
        case 0x0:
            sdram_pr_debug("The SDRAM DIMM1 Column Address Width is 8-bits\n");
            break;
        case 0x1:
            sdram_pr_debug("The SDRAM DIMM1 Column Address Width is 9-bits\n");
            break;
        case 0x2:
            sdram_pr_debug("The SDRAM DIMM1 Column Address Width is 10-bits\n");
            break;
        default:
            sdram_pr_err("DRAM_rsim_Error...............\n");
            break;
        }

        switch (map1_mode&0xc) {
        case 0x0:
                sdram_pr_debug("The SDRAM DIMM1 Row Address Width is 11-bits\n");
                break;
        case 0x4:
                sdram_pr_debug("The SDRAM DIMM1 Row Address Width is 12-bits\n");
                break;
        default:
                sdram_pr_err("DRAM_rsim_Error...............\n");
                break;
        }
    }
    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG)= (temp&0x0000ffff)|((SDRAM_TurnAround_Mode<<31)|(SDRAM_OE_Post_En<<30)|
    		(SDRAM_OE_Pre_En<<29)|(SDRAM_DQ_Mode<<28)|(dimm1_active<<27)|
    		(0x0<<26)|(dimm1_banks<<25)|(dimm_banks<<24)|
    		(map1_mode<<20)|(map_mode<<16)
    		);
}

static void SDRAM_Set_TimeParam(){
    unsigned int temp;
    sdram_pr_debug("*** SDRAM set timing parameter......\n");
	temp = HW32_REG(SDRAM_CTRL_REG+0x4);
	HW32_REG(SDRAM_CTRL_REG+0x4)	= (temp&0xffff0000) | ((SDRAM_Ras_ActTime<<12)|(SDRAM_Ras_RefCyc<<8)|
														(SDRAM_CtoC_dly<<7) | (SDRAM_RtoR_dly<<6) |
														(SDRAM_RtoC_dly<<2) | (SDRAM_Ras_PreTime));
}
static void SDRAM_NOP()
{
    unsigned int temp;
    SDRAM_Wait_Idle();
    sdram_pr_debug ("*** Send NOP Command to SDRAMs ......\n");
    temp = HW32_REG(SDRAM_CTRL_REG+0x0);
    HW32_REG(SDRAM_CTRL_REG+0x0) = (temp&0xffffff00) | 0x0;
}

static void SDRAM_PrechargeAll() {
    unsigned int temp;
    SDRAM_Wait_Idle();
    sdram_pr_debug ("*** Precharging All Banks of SDRAMs ......\n");
    temp = HW32_REG(SDRAM_CTRL_REG+0x0);
    HW32_REG(SDRAM_CTRL_REG+0x0) = (temp&0xffffff00) | 0x2;
}

static void SDRAM_Mode_Set() {
    unsigned int temp;
    SDRAM_Wait_Idle();
    sdram_pr_debug ("*** Setting SDRAM mode ......\n");
    temp = HW32_REG(SDRAM_CTRL_REG+0x4);
    HW32_REG(SDRAM_CTRL_REG+0x4) = (temp&0xff00ffff) | ((SDRAM_Cas_Latency<<20)|(SDRAM_Interleave<<19)|(SDRAM_Burst_Length<<16)) ;
    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG)		 = (temp&0xffffff00)|0x1;
}

static void SDRAM_Refresh() {
    unsigned int temp;
    SDRAM_Wait_Idle();
    sdram_pr_debug ("*** SDRAM auto refreshing ......\n");
    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG) = ((temp&0xffffff00) | 0x3);
}

static void SDRAM_Ref_Disable(void) {
    SDRAM_Wait_Idle();
    sdram_pr_debug ("*** SDRAM Disable Auto-Refresh Timer......\n");
    SDRAM_Set_Refresh_Timer(0x0);
    SDRAM_Wait_Idle();
}

static void SDRAM_Set_Refresh_Timer(unsigned int ref_init)
{
    unsigned int 	temp;
    sdram_pr_debug("*** SDRAM set refresh timer......\n");
    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG) = ((temp&0xffff00FF) | (ref_init<<8));
}

#if 1
void gm6721_pinmux_config()
{
     *(unsigned int*)(0x1ffff0a8) |= 0xfffffba7;
     *(unsigned int*)(0x1ffff0ac) |= 0xfff5ffff;
     *(unsigned int*)(0x1ffff0b0) |= 0xff3fffff;
     
     //*(unsigned int*)(0x1ffff0a8) |= 0xfffffbef;
     //*(unsigned int*)(0x1ffff0ac) |= 0xff71ffff;
     //*(unsigned int*)(0x1ffff0b0) |= 0xff3fffff;
 #if 1 
     *(unsigned int*)(0x400002DC) |= 0x01000000;
     *(unsigned int*)(0x40000324) |= 0x000f0000;
     *(unsigned int*)(0x400002E8) |= 0x44044555;
     *(unsigned int*)(0x40000338) |= 0xf0ffffff;
     *(unsigned int*)(0x4000033C) |= 0xf0f000f0;
     *(unsigned int*)(0x400002EC) |= 0x00015544;
     *(unsigned int*)(0x40000340) |= 0xfffff0f0;
     *(unsigned int*)(0x400002E4) |= 0x54444400;
     *(unsigned int*)(0x40000330) |= 0xf0f00000;
     *(unsigned int*)(0x40000334) |= 0xfff0f0f0;
     *(unsigned int*)(0x40000344) |= 0x0000000f;
     *(unsigned int*)(0x400002C4) |= 0x00140000;
     *(unsigned int*)(0x400002F4) |= 0x00000ff0;
     *(unsigned int*)(0x400002D0) |= 0x55455554;
     *(unsigned int*)(0x40000308) |= 0xfffffff0;
     *(unsigned int*)(0x4000030C) |= 0xfffff0ff;
     *(unsigned int*)(0x400002D4) |= 0x44451111;
     *(unsigned int*)(0x40000310) |= 0x0f0f0f0f;
     *(unsigned int*)(0x40000314) |= 0xf0f0f0ff;
     *(unsigned int*)(0x400002CC) |= 0x44445450;
	 *(unsigned int*)(0x40000304) |= 0xf0f0f0f0;
	 *(unsigned int*)(0x40000300) |= 0xfff0ff00;
 #else
HW32_REG(0x400002DC) |= 1<<24;      HW32_REG(0x40000324) |= 0xf<<16; //SDRAM_SDCLK         XPG12   
            HW32_REG(0x400002E8) |= 1<<10;      HW32_REG(0x40000338) |= 0xf<<20; //SDRAM_D[0]          XPJ5    
            HW32_REG(0x400002E8) |= 1<<14;      HW32_REG(0x40000338) |= 0xf<<28; //SDRAM_D[1]          XPJ7    
            HW32_REG(0x400002E8) |= 1<<18;      HW32_REG(0x4000033C) |= 0xf<<4 ; //SDRAM_D[2]          XPJ9    
            HW32_REG(0x400002E8) |= 1<<26;      HW32_REG(0x4000033C) |= 0xf<<20; //SDRAM_D[3]          XPJ13   
            HW32_REG(0x400002E8) |= 1<<30;      HW32_REG(0x4000033C) |= 0xf<<28; //SDRAM_D[4]          XPJ15   
            HW32_REG(0x400002EC) |= 1<<2 ;      HW32_REG(0x40000340) |= 0xf<<4 ; //SDRAM_D[5]          XPK1    
            HW32_REG(0x400002EC) |= 1<<6 ;      HW32_REG(0x40000340) |= 0xf<<12; //SDRAM_D[6]          XPK3    
            HW32_REG(0x400002EC) |= 1<<10;      HW32_REG(0x40000340) |= 0xf<<20; //SDRAM_D[7]          XPK5    
            HW32_REG(0x400002E4) |= 1<<10;      HW32_REG(0x40000330) |= 0xf<<20; //SDRAM_D[8]          XPI5    
            HW32_REG(0x400002E4) |= 1<<14;      HW32_REG(0x40000330) |= 0xf<<28; //SDRAM_D[9]          XPI7    
            HW32_REG(0x400002E4) |= 1<<18;      HW32_REG(0x40000334) |= 0xf<<4 ; //SDRAM_D[10]         XPI9    
            HW32_REG(0x400002E4) |= 1<<22;      HW32_REG(0x40000334) |= 0xf<<12; //SDRAM_D[11]         XPI11   
            HW32_REG(0x400002E4) |= 1<<26;      HW32_REG(0x40000334) |= 0xf<<20; //SDRAM_D[12]         XPI13   
            HW32_REG(0x400002E4) |= 1<<30;      HW32_REG(0x40000334) |= 0xf<<28; //SDRAM_D[13]         XPI15   
            HW32_REG(0x400002E8) |= 1<<2 ;      HW32_REG(0x40000338) |= 0xf<<4 ; //SDRAM_D[14]         XPJ1    
            HW32_REG(0x400002E8) |= 1<<6 ;      HW32_REG(0x40000338) |= 0xf<<12; //SDRAM_D[15]         XPJ3    
            HW32_REG(0x400002E4) |= 1<<28;      HW32_REG(0x40000334) |= 0xf<<24; //SDRAM_D[16]         XPI14   
            HW32_REG(0x400002E8) |= 1<<0 ;      HW32_REG(0x40000338) |= 0xf<<0 ; //SDRAM_D[17]         XPJ0    
            HW32_REG(0x400002E8) |= 1<<4 ;      HW32_REG(0x40000338) |= 0xf<<8 ; //SDRAM_D[18]         XPJ2    
            HW32_REG(0x400002E8) |= 1<<8 ;      HW32_REG(0x40000338) |= 0xf<<16; //SDRAM_D[19]         XPJ4    
            HW32_REG(0x400002EC) |= 1<<8 ;      HW32_REG(0x40000340) |= 0xf<<16; //SDRAM_D[20]         XPK4    
            HW32_REG(0x400002EC) |= 1<<12;      HW32_REG(0x40000340) |= 0xf<<24; //SDRAM_D[21]         XPK6    
            HW32_REG(0x400002EC) |= 1<<14;      HW32_REG(0x40000340) |= 0xf<<28; //SDRAM_D[22]         XPK7    
            HW32_REG(0x400002EC) |= 1<<16;      HW32_REG(0x40000344) |= 0xf<<0 ; //SDRAM_D[23]         XPK8    
            HW32_REG(0x400002C4) |= 1<<18;      HW32_REG(0x400002F4) |= 0xf<<4 ; //SDRAM_D[24]         XPA9    
            HW32_REG(0x400002C4) |= 1<<20;      HW32_REG(0x400002F4) |= 0xf<<8 ; //SDRAM_D[25]         XPA10   
            HW32_REG(0x400002D0) |= 1<<4 ;      HW32_REG(0x40000308) |= 0xf<<8 ; //SDRAM_D[26]         XPD2    
            HW32_REG(0x400002D0) |= 1<<8 ;      HW32_REG(0x40000308) |= 0xf<<16; //SDRAM_D[27]         XPD4    
            HW32_REG(0x400002D0) |= 1<<12;      HW32_REG(0x40000308) |= 0xf<<24; //SDRAM_D[28]         XPD6    
            HW32_REG(0x400002D0) |= 1<<16;      HW32_REG(0x4000030C) |= 0xf<<0 ; //SDRAM_D[29]         XPD8    
            HW32_REG(0x400002D0) |= 1<<24;      HW32_REG(0x4000030C) |= 0xf<<16; //SDRAM_D[30]         XPD12   
            HW32_REG(0x400002D0) |= 1<<28;      HW32_REG(0x4000030C) |= 0xf<<24; //SDRAM_D[31]         XPD14   
            HW32_REG(0x400002D0) |= 1<<14;      HW32_REG(0x40000308) |= 0xf<<28; //SDRAM_A[0]          XPD7    
            HW32_REG(0x400002D0) |= 1<<18;      HW32_REG(0x4000030C) |= 0xf<<4 ; //SDRAM_A[1]          XPD9    
            HW32_REG(0x400002D0) |= 1<<22;      HW32_REG(0x4000030C) |= 0xf<<12; //SDRAM_A[2]          XPD11   
            HW32_REG(0x400002D0) |= 1<<26;      HW32_REG(0x4000030C) |= 0xf<<20; //SDRAM_A[3]          XPD13   
            HW32_REG(0x400002D0) |= 1<<30;      HW32_REG(0x4000030C) |= 0xf<<28; //SDRAM_A[4]          XPD15   
            HW32_REG(0x400002D4) |= 1<<4 ;      HW32_REG(0x40000310) |= 0xf<<8 ; //SDRAM_A[5]          XPE2    
            HW32_REG(0x400002D4) |= 1<<8 ;      HW32_REG(0x40000310) |= 0xf<<16; //SDRAM_A[6]          XPE4    
            HW32_REG(0x400002D4) |= 1<<12;      HW32_REG(0x40000310) |= 0xf<<24; //SDRAM_A[7]          XPE6    
            HW32_REG(0x400002D4) |= 1<<16;      HW32_REG(0x40000314) |= 0xf<<0 ; //SDRAM_A[8]          XPE8    
            HW32_REG(0x400002D4) |= 1<<18;      HW32_REG(0x40000314) |= 0xf<<4 ; //SDRAM_A[9]          XPE9    
            HW32_REG(0x400002D0) |= 1<<10;      HW32_REG(0x40000308) |= 0xf<<20; //SDRAM_A[10]         XPD5    
            HW32_REG(0x400002D4) |= 1<<22;      HW32_REG(0x40000314) |= 0xf<<12; //SDRAM_A[11]         XPE11   
            HW32_REG(0x400002D4) |= 1<<0 ;      HW32_REG(0x40000310) |= 0xf<<0 ; //SDRAM_A[12]         XPE0    
            HW32_REG(0x400002CC) |= 1<<18;      HW32_REG(0x40000304) |= 0xf<<4 ; //SDRAM_SDNWE         XPC9    
            HW32_REG(0x400002CC) |= 1<<30;      HW32_REG(0x40000304) |= 0xf<<28; //SDRAM_SDNE[0]       XPC15   
            HW32_REG(0x400002CC) |= 1<<4 ;      HW32_REG(0x40000300) |= 0xf<<8 ; //SDRAM_SDNE[1]       XPC2    
            HW32_REG(0x400002D4) |= 1<<26;      HW32_REG(0x40000314) |= 0xf<<20; //SDRAM_SDCKE[0]      XPE13   
            HW32_REG(0x400002CC) |= 1<<6 ;      HW32_REG(0x40000300) |= 0xf<<12; //SDRAM_SDCKE[1]      XPC3    
            HW32_REG(0x400002CC) |= 1<<26;      HW32_REG(0x40000304) |= 0xf<<20; //SDRAM_SDNRAS        XPC13   
            HW32_REG(0x400002CC) |= 1<<14;      HW32_REG(0x40000300) |= 0xf<<28; //SDRAM_NBL[0]        XPC7    
            HW32_REG(0x400002D4) |= 1<<30;      HW32_REG(0x40000314) |= 0xf<<28; //SDRAM_NBL[1]        XPE15   
            HW32_REG(0x400002CC) |= 1<<10;      HW32_REG(0x40000300) |= 0xf<<20; //SDRAM_NBL[2]        XPC5    
            HW32_REG(0x400002CC) |= 1<<12;      HW32_REG(0x40000300) |= 0xf<<24; //SDRAM_NBL[3]        XPC6    
            HW32_REG(0x400002CC) |= 1<<22;      HW32_REG(0x40000304) |= 0xf<<12; //SDRAM_SDNCAS        XPC11   
            HW32_REG(0x400002D0) |= 1<<2 ;      HW32_REG(0x40000308) |= 0xf<<4 ; //SDRAM_BA[0]         XPD1    
            HW32_REG(0x400002D0) |= 1<<6 ;      HW32_REG(0x40000308) |= 0xf<<12; //SDRAM_BA[1]         XPD3
 #endif
    
 
	 *(unsigned int*)(0x40000298) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
	 *(unsigned int*)(0x4000029c) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
	 *(unsigned int*)(0x400002a0) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
	 *(unsigned int*)(0x400002a4) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
	 *(unsigned int*)(0x400002a8) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
	 *(unsigned int*)(0x400002ac) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
	 *(unsigned int*)(0x400002b0) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
	 *(unsigned int*)(0x400002b4) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
	 *(unsigned int*)(0x400002b8) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
	 *(unsigned int*)(0x400002bc) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
	 *(unsigned int*)(0x400002c0) |= (0xf<<0)|(0xf<<4)|(0xf<<8)|(0xf<<12)|(0xf<<16)|(0xf<<20)|(0xf<<24)|(0xf<<28);
}
#endif

void gm6721_sdram_init (void)
{
    /*reference count:only init for one time*/
    static int referenceCount = 0;
	gm6721_pinmux_config();
    if(!referenceCount)
    {
        sdram_pr_debug(" - DRAM Init Begin\n");
        //release the sai_a reset	
        HW32_REG(0x40000070) |= (1<<7);	
        //release the sai_b reset	
        HW32_REG(0x40000080) |= (1<<7);
    
        SDRAM_Ref_Disable();
        DIMM_Set_Map_Mode();
        SDRAM_Set_TimeParam();
        SDRAM_NOP();
        SDRAM_NOP();
        SDRAM_NOP();
        SDRAM_PrechargeAll();
        SDRAM_Mode_Set();
        SDRAM_Set_Refresh_Timer(0x05);
        SDRAM_Refresh();SDRAM_Refresh();SDRAM_Refresh();SDRAM_Refresh();
        SDRAM_Refresh();SDRAM_Refresh();SDRAM_Refresh();SDRAM_Refresh();
        SDRAM_Wait_Idle();
        referenceCount++;
    }
    HW32_REG(0x40039290) |= (0xA)<<12;
}