/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_sdram.c
 *
 * @author        hongze
 *
 * @version       1.0.0
 *
 * @date          2018/03/19
 *
 * @brief        sdram APIs for application.
 *
 * @note
 *    2018/03/19, hongze, V1.0.0
 *        Initial version.
 */
#include "stdio.h"
#include <gm_common.h>
#include "gm_ll_sdram.h"
#include <gm_hal_sdram.h>
//#define GM_SDRAM_DEBGU          1
//#define GM_SDRAM_ERR              1
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

uint32_t SDR_32B_MODE  = 0;
uint32_t sdram_mode_0  = 0;
uint32_t sdram_mode_1  = 0;
uint32_t SDRAM_DQ_Mode = 0;
uint32_t SDRAM_Burst_Length = 0x2;
/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

#define       SDRAM_Ras_RefCyc         0x7
#define       SDRAM_RtoC_dly           0x2
#define       SDRAM_Ras_PreTime        0x2
//for sdram initial
#define       SDRAM_Cas_Latency         0x3  	// Tcl  time    default is 3clk 2 -> 2clk
#define       SDRAM_Ras_ActTime      	0x3   	// Tras time    default is 7 clks
#define       SDRAM_CtoC_dly         	0x0   	// Tc2c time    default is 0 0-> 1Tclk, 1-> 2Tclk
#define       SDRAM_RtoR_dly         	0x1   	// Trrd delay   0-> 2cycles, 1->3cycles
#define       SDRAM_TurnAround_Mode   	0x0   	//SDRAM Controller Turn around 0 -> 2 cycle 1 -> 1 cycle
#define       SDRAM_OE_Post_En        	0x0   	//SDRAM OE post 1-> enable 0->disable(default)
#define       SDRAM_OE_Pre_En         	0x0   	//SDRAM OE pre  1-> enable 0->disable(default)
//#define       SDRAM_Burst_Length      	0x2+SDRAM_DQ_Mode   //3'b010->4 bl, 3'b011->8bl
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
#define       SDRAM_Interleave        	0x0
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

static void SDRAM_Wait_Idle(void) 
{
    unsigned int flag;
    unsigned int temp_mem;
    sdram_pr_debug("*** Waiting SDRAM idle........\n");

    flag = 0x1;
    while (flag){
        temp_mem = HW32_REG(SDRAM_CTRL_REG);

        if ((temp_mem&0x7) == 0x0)
        	flag = 0;
        else
        	flag = 1;
    }
}

static void DIMM_Set_Map_Mode(void)
{
    unsigned int    dimm0_banks;
    unsigned int    dimm0_map;
    unsigned int    dimm1_en;
    unsigned int    dimm1_banks;
    unsigned int    dimm1_map;
    //unsigned int    sdram_row0_map;
    //unsigned int    sdram_row1_base;
    //unsigned int    sdram_row1_map;
    //unsigned int    SDRAM_DIMM1_BASE;

    if(sdram_mode_0 == SDRAM_D0_64M_4B){
        //sdram_row0_map          =       0x6;   // 4 banks, 10 bit column addr
        //sdram_row1_base         =       0x04000000;
        /*
        if(SDRAM_DQ_Mode)
            //SDRAM_DIMM1_BASE        =       0x02000000;
        else
            //SDRAM_DIMM1_BASE        =       0x04000000;
        */
        dimm0_banks                     =       0x1;
        dimm0_map                       =       0xa;
    }
    else if(sdram_mode_0 == SDRAM_D0_32M_4B){
        //sdram_row0_map          =       0x6;   // 4 banks, 10 bit column addr
        //sdram_row1_base         =       0x04000000;
        /*
        if(SDRAM_DQ_Mode)
            //SDRAM_DIMM1_BASE        =       0x02000000;
        else
            //SDRAM_DIMM1_BASE        =       0x04000000;
        */
        dimm0_banks                     =       0x1;
        dimm0_map                       =       0x9;
    }
    else if(sdram_mode_0 == SDRAM_D0_16M_4B){
        //sdram_row0_map          =       0x6;   // 4 banks, 10 bit column addr
        //sdram_row1_base         =       0x04000000;
        /*
        if(SDRAM_DQ_Mode)
            //SDRAM_DIMM1_BASE        =       0x02000000;
        else
            //SDRAM_DIMM1_BASE        =       0x04000000;
        */
        dimm0_banks                     =       0x1;
        dimm0_map                       =       0x5;
    }
    else if(sdram_mode_0 == SDRAM_D0_8M_4B){
        //sdram_row0_map          =       0x5;   // 4 banks, 9 bit column addr
        //sdram_row1_base         =       0x02000000;
        /*
        if(SDRAM_DQ_Mode)
            //SDRAM_DIMM1_BASE        =       0x01000000;
        else
            //SDRAM_DIMM1_BASE        =       0x02000000;
        */
        dimm0_banks                     =       0x1;
        dimm0_map                       =       0x4;//5 is bug
    }
    else if(sdram_mode_0 == SDRAM_D0_4M_4B){
        //sdram_row0_map          =       0x4;   // 4 banks, 8 bit column addr
        //sdram_row1_base         =       0x01000000;
        /*
        if(SDRAM_DQ_Mode)
            //SDRAM_DIMM1_BASE        =       0x00800000;
        else
            //SDRAM_DIMM1_BASE        =       0x01000000;
        */
        dimm0_banks                     =       0x1;
        dimm0_map                       =       0x4;
    }
    else{
        //sdram_row0_map          =       0x0;   // 2 banks, 8 bit column addr
        //sdram_row1_base         =       0x00400000;
        /*
        if(SDRAM_DQ_Mode)
            //SDRAM_DIMM1_BASE        =       0x00200000;
        else
            //SDRAM_DIMM1_BASE        =       0x00400000;
        */
        dimm0_banks                     =       0x0;
        dimm0_map                       =       0x0;
    }

    if(sdram_mode_1 == SDRAM_D1_64M_4B){
        dimm1_en                        =       0x1;
        //sdram_row1_map                  =       0x6;   // 4 banks, 10 bit column addr
        dimm1_banks                     =       0x1;
        dimm1_map                       =       0x6;
    }
    else if(sdram_mode_1 == SDRAM_D1_32M_4B){
        dimm1_en                        =       0x1;
        //sdram_row1_map                  =       0x6;   // 4 banks, 10 bit column addr
        dimm1_banks                     =       0x1;
        dimm1_map                       =       0x6;
    }
    else if(sdram_mode_1 == SDRAM_D1_16M_4B){
        dimm1_en                        =       0x1;
        //sdram_row1_map                  =       0x6;   // 4 banks, 10 bit column addr
        dimm1_banks                     =       0x1;
        dimm1_map                       =       0x6;
    }
    else if(sdram_mode_1 == SDRAM_D1_8M_4B){
        dimm1_en                        =       0x1;
        //sdram_row1_map                  =       0x5;   // 4 banks, 9 bit column addr
        dimm1_banks                     =       0x1;
        dimm1_map                       =       0x5;
    }
    else if(sdram_mode_1 == SDRAM_D1_4M_4B){
        dimm1_en                        =       0x1;
        //sdram_row1_map                  =       0x4;   // 4 banks, 8 bit column addr
        dimm1_banks                     =       0x1;
        dimm1_map                       =       0x4;
    }
    else if(sdram_mode_1 == SDRAM_D1_2M_4B){
        dimm1_en                        =       0x1;
        //sdram_row1_map                  =       0x0;   // 2 banks, 8 bit column addr
        dimm1_banks                     =       0x0;
        dimm1_map                       =       0x0;
    }
    else{
        dimm1_en                        =       0x0;
        //sdram_row1_map                  =       0x0;   // 2 banks, 8 bit column addr
        dimm1_banks                     =       0x0;
        dimm1_map                       =       0x0;
    }
    SDRAM_Set_DIMM_Map(dimm1_en,dimm0_banks,dimm0_map,dimm1_banks,dimm1_map);
}//end DIMM_Set_Map_Mode


static void SDRAM_Set_DIMM_Map(unsigned int dimm1_en,unsigned int dimm0_banks,unsigned int dimm0_map,unsigned int dimm1_banks,unsigned int dimm1_map)
{
    unsigned int      dimm1_active 	= dimm1_en;
    unsigned int      dimm_banks	= dimm0_banks;
    unsigned int      map_mode 		= dimm0_map;
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

#if defined(CHIP_GM6721)
void gm6721_sdram_pinmux_config()
{
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
	 *(unsigned int*)(0x40000298) |= 0xffffffff;
	 *(unsigned int*)(0x4000029c) |= 0xffffffff;
	 *(unsigned int*)(0x400002a0) |= 0xffffffff;
	 *(unsigned int*)(0x400002a4) |= 0xffffffff;
	 *(unsigned int*)(0x400002a8) |= 0xffffffff;
	 *(unsigned int*)(0x400002ac) |= 0xffffffff;
	 *(unsigned int*)(0x400002b0) |= 0xffffffff;
	 *(unsigned int*)(0x400002b4) |= 0xffffffff;
	 *(unsigned int*)(0x400002b8) |= 0xffffffff;
	 *(unsigned int*)(0x400002bc) |= 0xffffffff;
	 *(unsigned int*)(0x400002c0) |= 0xffffffff;
}
#endif

#if defined(CHIP_F6721B)
void f6721_sdram_pinmux_config()
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

	////printf("\n------sdram pinmux configure1------ \n");
	//SDRAM inteface
	HW32_REG(0x400002C4) |= 1<<0;   HW32_REG(0x400002F0) |= 5<<0;       //SDRAM_D[16]		    XPA0 	
	HW32_REG(0x400002C4) |= 1<<2;   HW32_REG(0x400002F0) |= 5<<4;       //SDRAM_D[17]		    XPA1 	
	HW32_REG(0x400002C4) |= 1<<4;   HW32_REG(0x400002F0) |= 5<<8;       //SDRAM_D[18]		    XPA2 	
	HW32_REG(0x400002C4) |= 1<<6;   HW32_REG(0x400002F0) |= 5<<12;      //SDRAM_D[19]		    XPA3 	
	HW32_REG(0x400002C4) |= 1<<8;   HW32_REG(0x400002F0) |= 5<<16;      //SDRAM_D[20]		    XPA4	
	HW32_REG(0x400002C4) |= 1<<10;  HW32_REG(0x400002F0) |= 5<<20;      //SDRAM_D[21]		    XPA5 	
	HW32_REG(0x400002C4) |= 1<<12;  HW32_REG(0x400002F0) |= 5<<24;      //SDRAM_D[22]		    XPA6 	
	HW32_REG(0x400002C4) |= 1<<14;  HW32_REG(0x400002F0) |= 5<<28;      //SDRAM_D[23]		    XPA7 	
	HW32_REG(0x400002C4) |= 1<<16;  HW32_REG(0x400002F4) |= 5<<0;       //SDRAM_A[3]		    XPA8 	
	HW32_REG(0x400002C4) |= 1<<18;  HW32_REG(0x400002F4) |= 5<<4;       //SDRAM_A[2]		    XPA9 	
	HW32_REG(0x400002C4) |= 1<<20;  HW32_REG(0x400002F4) |= 5<<8;       //SDRAM_A[1]		    XPA10	
	HW32_REG(0x400002C4) |= 1<<22;  HW32_REG(0x400002F4) |= 5<<12;      //SDRAM_A[0]		    XPA11	
	HW32_REG(0x400002C4) |= 1<<24;  HW32_REG(0x400002F4) |= 5<<16;      //SDRAM_A[10]		    XPA12	
	HW32_REG(0x400002C4) |= 1<<26;  HW32_REG(0x400002F4) |= 5<<20;      //SDRAM_BA[1]		    XPA13	
	HW32_REG(0x400002C4) |= 1<<28;  HW32_REG(0x400002F4) |= 5<<24;      //SDRAM_BA[0]		    XPA14	
	HW32_REG(0x400002C4) |= 1<<30;  HW32_REG(0x400002F4) |= 5<<28;      //SDRAM_SDNE[1]		    XPA15	
	HW32_REG(0x400002C8) |= 1<<0;   HW32_REG(0x400002F8) |= 5<<0;       //SDRAM_SDNE[0]		    XPB0 	
	HW32_REG(0x400002C8) |= 1<<2;   HW32_REG(0x400002F8) |= 5<<4;       //SDRAM_SDNRAS		    XPB1 	
	HW32_REG(0x400002C8) |= 1<<4;   HW32_REG(0x400002F8) |= 5<<8;       //SDRAM_SDNCAS		    XPB2 	
	HW32_REG(0x400002C8) |= 1<<6;   HW32_REG(0x400002F8) |= 5<<12;      //SDRAM_SDNWE		    XPB3 	
	HW32_REG(0x400002C8) |= 1<<8;   HW32_REG(0x400002F8) |= 5<<16;      //SDRAM_NBL[0]		    XPB4 	
	HW32_REG(0x400002C8) |= 1<<12;  HW32_REG(0x400002F8) |= 5<<24;      //SDRAM_NBL[2]		    XPB6 	
	HW32_REG(0x400002E4) |= 1<<8 ;  HW32_REG(0x40000330) |= 5<<16;      //SDRAM_NBL[3]		    XPI4 	
	HW32_REG(0x400002E4) |= 1<<10;  HW32_REG(0x40000330) |= 5<<20;      //SDRAM_SDCKE[1]		XPI5 	
	HW32_REG(0x400002E4) |= 1<<12;  HW32_REG(0x40000330) |= 5<<24;      //SDRAM_NBL[1]		    XPI6 	
	HW32_REG(0x400002E4) |= 1<<14;  HW32_REG(0x40000330) |= 5<<28;      //SDRAM_SDCLK		    XPI7 	
	HW32_REG(0x400002E4) |= 1<<16;  HW32_REG(0x40000334) |= 5<<0;       //SDRAM_SDCKE[0]		XPI8 	
	HW32_REG(0x400002E4) |= 1<<18;  HW32_REG(0x40000334) |= 5<<4;       //SDRAM_A[12]		    XPI9 	
	HW32_REG(0x400002E4) |= 1<<20;  HW32_REG(0x40000334) |= 5<<8;       //SDRAM_A[11]		    XPI10	
	HW32_REG(0x400002E4) |= 1<<22;  HW32_REG(0x40000334) |= 5<<12;      //SDRAM_A[9]		    XPI11	
	HW32_REG(0x400002E4) |= 1<<24;  HW32_REG(0x40000334) |= 5<<16;      //SDRAM_A[8]		    XPI12	
	HW32_REG(0x400002E4) |= 1<<26;  HW32_REG(0x40000334) |= 5<<20;      //SDRAM_A[7]		    XPI13	
	HW32_REG(0x400002E4) |= 1<<28;  HW32_REG(0x40000334) |= 5<<24;      //SDRAM_A[6]		    XPI14	
	HW32_REG(0x400002E4) |= 1<<30;  HW32_REG(0x40000334) |= 5<<28;      //SDRAM_A[5]		    XPI15	
	HW32_REG(0x400002E8) |= 1<<0;   HW32_REG(0x40000338) |= 5<<0;       //SDRAM_A[4]		    XPJ0 	
	HW32_REG(0x400002E8) |= 1<<2;   HW32_REG(0x40000338) |= 5<<4;       //SDRAM_D[24]		    XPJ1 	
	HW32_REG(0x400002E8) |= 1<<4;   HW32_REG(0x40000338) |= 5<<8;       //SDRAM_D[25]		    XPJ2 	
	HW32_REG(0x400002E8) |= 1<<6;   HW32_REG(0x40000338) |= 5<<12;      //SDRAM_D[26]		    XPJ3	
	HW32_REG(0x400002E8) |= 1<<8;   HW32_REG(0x40000338) |= 5<<16;      //SDRAM_D[27]		    XPJ4	
	HW32_REG(0x400002E8) |= 1<<10;  HW32_REG(0x40000338) |= 5<<20;      //SDRAM_D[28]		    XPJ5	
	HW32_REG(0x400002E8) |= 1<<12;  HW32_REG(0x40000338) |= 5<<24;      //SDRAM_D[29]		    XPJ6	
	HW32_REG(0x400002E8) |= 1<<14;  HW32_REG(0x40000338) |= 5<<28;      //SDRAM_D[30]		    XPJ7	
	HW32_REG(0x400002E8) |= 1<<16;  HW32_REG(0x4000033C) |= 5<<0;       //SDRAM_D[31]		    XPJ8	
	HW32_REG(0x400002E8) |= 1<<18;  HW32_REG(0x4000033C) |= 5<<4;       //SDRAM_D[8]		    XPJ9	
	HW32_REG(0x400002E8) |= 1<<20;  HW32_REG(0x4000033C) |= 5<<8;       //SDRAM_D[9]		    XPJ10	
	HW32_REG(0x400002E8) |= 1<<22;  HW32_REG(0x4000033C) |= 5<<12;      //SDRAM_D[10]		    XPJ11	
	HW32_REG(0x400002E8) |= 1<<24;  HW32_REG(0x4000033C) |= 5<<16;      //SDRAM_D[11]		    XPJ12	
	HW32_REG(0x400002E8) |= 1<<26;  HW32_REG(0x4000033C) |= 5<<20;      //SDRAM_D[12]		    XPJ13	
	HW32_REG(0x400002E8) |= 1<<28;  HW32_REG(0x4000033C) |= 5<<24;      //SDRAM_D[14]		    XPJ14	
	HW32_REG(0x400002E8) |= 1<<30;  HW32_REG(0x4000033C) |= 5<<28;      //SDRAM_D[13]		    XPJ15	
	HW32_REG(0x400002EC) |= 1<<0;   HW32_REG(0x40000340) |= 5<<0;       //SDRAM_D[15]		    XPK0	
	HW32_REG(0x400002EC) |= 1<<2;   HW32_REG(0x40000340) |= 5<<4;       //SDRAM_D[0]		    XPK1	
	HW32_REG(0x400002EC) |= 1<<4;   HW32_REG(0x40000340) |= 5<<8;       //SDRAM_D[2]		    XPK2	
	HW32_REG(0x400002EC) |= 1<<6;   HW32_REG(0x40000340) |= 5<<12;      //SDRAM_D[1]		    XPK3	
	HW32_REG(0x400002EC) |= 1<<8;   HW32_REG(0x40000340) |= 5<<16;      //SDRAM_D[3]		    XPK4 	
	HW32_REG(0x400002EC) |= 1<<10;  HW32_REG(0x40000340) |= 5<<20;      //SDRAM_D[4]		    XPK5	
	HW32_REG(0x400002EC) |= 1<<12;  HW32_REG(0x40000340) |= 5<<24;      //SDRAM_D[5]		    XPK6	
	HW32_REG(0x400002EC) |= 1<<14;  HW32_REG(0x40000340) |= 5<<28;      //SDRAM_D[6]		    XPK7	
	HW32_REG(0x400002EC) |= 1<<16;  HW32_REG(0x40000344) |= 5<<0;       //SDRAM_D[7]		    XPK8	
}
#endif
HAL_StatusTypeDef HAL_SDRAM_Init(SDRAM_HandleTypeDef *hsdram)
{
    /*reference count:only init for one time*/
    static int referenceCount = 0;
    sdram_mode_0 = 0;
    sdram_mode_1 = 0;
#if defined(CHIP_GM6721)
	gm6721_sdram_pinmux_config();
#endif
#if defined(CHIP_F6721B)
	f6721_sdram_pinmux_config();
#endif
    if(hsdram->width == SDRAM_16BIT)
    {
        SDRAM_DQ_Mode = 1;
        //just use d0
        sdram_mode_1 = 0;
        if(hsdram->size == SDRAM_4M)
        {
            sdram_mode_0 = SDRAM_D0_4M_4B;
        }else if(hsdram->size == SDRAM_8M)
        {
            sdram_mode_0 = SDRAM_D0_8M_4B;
        }else if(hsdram->size == SDRAM_16M)
        {
            sdram_mode_0 = SDRAM_D0_16M_4B;
        }else if(hsdram->size == SDRAM_32M)
        {
            sdram_mode_0 = SDRAM_D0_32M_4B;
        }else if(hsdram->size == SDRAM_64M)
        {
            sdram_mode_0 = SDRAM_D0_64M_4B;
        }
        
    }else if(hsdram->width == SDRAM_32BIT){
        SDRAM_DQ_Mode = 0;
        //use d0, d1
        if(hsdram->size == SDRAM_4M)
        {
            sdram_mode_0 = SDRAM_D0_2M_4B;
            sdram_mode_1 = SDRAM_D1_2M_4B;
        }else if(hsdram->size == SDRAM_8M)
        {
            sdram_mode_0 = SDRAM_D0_4M_4B;
            sdram_mode_1 = SDRAM_D0_4M_4B;
        }else if(hsdram->size == SDRAM_16M)
        {
            sdram_mode_0 = SDRAM_D0_8M_4B;
            sdram_mode_1 = SDRAM_D0_8M_4B;
        }else if(hsdram->size == SDRAM_32M)
        {
            sdram_mode_0 = SDRAM_D0_16M_4B;
            sdram_mode_1 = SDRAM_D0_16M_4B;
        }else if(hsdram->size == SDRAM_64M)
        {
            sdram_mode_0 = SDRAM_D0_32M_4B;
            sdram_mode_1 = SDRAM_D0_32M_4B;
        }
        
    }
    SDRAM_Burst_Length = 0x2+SDRAM_DQ_Mode;
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
#if defined(CHIP_GM6721)
    HW32_REG(0x40039290) |= (0xA)<<12;
#endif
    return HAL_OK;
}
