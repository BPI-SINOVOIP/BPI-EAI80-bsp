/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//#include <ugelis.h>
//#include <misc/printk.h>

#include "stdio.h"

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
    #define sdram_pr_err(...)   do{}while(0)
#endif


//#define						sdram_d0_16m_4b			1
#define						sdram_d0_32m_4b			1

#define SDRAM_MEM_START_ADDR 0xC0000000
/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile unsigned int  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile unsigned short int *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile unsigned char  *)(ADDRESS)))

//for sdram initial
#define       SDRAM_DQ_Mode           	0x1 	//0->32bits mode, 1->16bits mode, define in top.v first
#define       SDRAM_Cas_Latency        	0x3  	// Tcl  time    default is 3clk 2 -> 2clk
#define       SDRAM_Ras_ActTime      	0x3   	// Tras time    default is 7 clks
#define       SDRAM_Ras_RefCyc       	0x5   	// Trfc time    default is 10 clks
#define       SDRAM_CtoC_dly         	0x0   	// Tc2c time    default is 0 0-> 1Tclk, 1-> 2Tclk
#define       SDRAM_RtoR_dly         	0x1   	// Trrd delay   0-> 2cycles, 1->3cycles
#define       SDRAM_RtoC_dly         	0x1   	// Trcd delay   0 -> 2clk 3 -> 5 clks
#define       SDRAM_Ras_PreTime      	0x1   	// Trp  delay   0 -> 2clk 3 -> 5 clks
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
#define       SDRAM_Row_Map           	0x4   		// 2 banks, 8 bit column addr   3'h0 for load
//#define       SDRAM_Row_Map           	0x5   		// 2 banks, 8 bit column addr   3'h0 for load
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

void SDRAM_Wait_Idle(void);
void DIMM_Set_Map_Mode(void);
void SDRAM_Set_TimeParam(void);
void SDRAM_NOP(void);
void SDRAM_PrechargeAll(void);
void SDRAM_Mode_Set(void);
void SDRAM_Refresh(void	);
void SDRAM_Set_DIMM_Map(unsigned int dimm1_en,unsigned int dimm0_banks,unsigned int dimm0_map,unsigned int dimm1_banks,unsigned int dimm1_map);
void SDRAM_Set_Refresh_Timer(unsigned int ref_init);
void dram_init(void);

void SDRAM_Wait_Idle(void) {
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


void DIMM_Set_Map_Mode(void){
    unsigned int 	dimm0_banks;
    unsigned int	dimm0_map;
    unsigned int    dimm1_en;
    unsigned int    dimm1_banks;
    unsigned int    dimm1_map;
    unsigned int 	sdram_row0_map;
    unsigned int  	sdram_row1_base;
    unsigned int 	sdram_row1_map;
    unsigned int 	SDRAM_DIMM1_BASE;

#ifdef sdram_d0_32m_4b  // 12/10
    sdram_row0_map          =       0x6;   // 4 banks, 10 bit column addr
    sdram_row1_base         =       0x04000000;
    if(SDRAM_DQ_Mode)
            SDRAM_DIMM1_BASE        =       0x02000000;

    else
            SDRAM_DIMM1_BASE        =       0x04000000;

    dimm0_banks                     =       0x1;
    //dimm0_map                     =       0x6;
    dimm0_map                       =       0x9;

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
    dimm0_map                       =       0x5;

#else
#ifdef sdram_d0_4m_4b   // 12/8
    sdram_row0_map          =       0x4;   // 4 banks, 8 bit column addr
    sdram_row1_base         =       0x01000000;
    if(SDRAM_DQ_Mode)
            SDRAM_DIMM1_BASE        =       0x00800000;

    else
            SDRAM_DIMM1_BASE        =       0x0100_0000;

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

//sdram_row0_map  =       SDRAM_Row_Map   ;                       // 2 banks, 8 bit column addr
//sdram_row1_base =       SDRAM_Row1_Base ;
//dimm0_banks     =       SDRAM_Dimm_Bank_Map     ;
//dimm0_map       =       SDRAM_Dimm_Map          ;
//dimm1_en        =       SDRAM_Dimm1_En          ;

    SDRAM_Set_DIMM_Map(dimm1_en,dimm0_banks,dimm0_map,dimm1_banks,dimm1_map);
}//end DIMM_Set_Map_Mode


void    SDRAM_Set_DIMM_Map(unsigned int dimm1_en,unsigned int dimm0_banks,unsigned int dimm0_map,unsigned int dimm1_banks,unsigned int dimm1_map)
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

void    SDRAM_Set_TimeParam(){
    unsigned int temp;

    sdram_pr_debug("*** SDRAM set timing parameter......\n");

	temp = HW32_REG(SDRAM_CTRL_REG+0x4);
	HW32_REG(SDRAM_CTRL_REG+0x4)	= (temp&0xffff0000) | ((SDRAM_Ras_ActTime<<12)|(SDRAM_Ras_RefCyc<<8)|
														(SDRAM_CtoC_dly<<7) | (SDRAM_RtoR_dly<<6) |
														(SDRAM_RtoC_dly<<2) | (SDRAM_Ras_PreTime));
}
void SDRAM_NOP()
{
    unsigned int temp;
    SDRAM_Wait_Idle();
    sdram_pr_debug ("*** Send NOP Command to SDRAMs ......\n");
    temp = HW32_REG(SDRAM_CTRL_REG+0x0);
    HW32_REG(SDRAM_CTRL_REG+0x0) = (temp&0xffffff00) | 0x0;
}

void    SDRAM_PrechargeAll() {
    unsigned int temp;

    SDRAM_Wait_Idle();
    sdram_pr_debug ("*** Precharging All Banks of SDRAMs ......\n");
    temp = HW32_REG(SDRAM_CTRL_REG+0x0);
    HW32_REG(SDRAM_CTRL_REG+0x0) = (temp&0xffffff00) | 0x2;
    //cpu2mem_cfgwr(4'h0,4'b1110,32'h2);
}

void SDRAM_Mode_Set() {
    unsigned int temp;
    SDRAM_Wait_Idle();
    sdram_pr_debug ("*** Setting SDRAM mode ......\n");
    temp = HW32_REG(SDRAM_CTRL_REG+0x4);
    HW32_REG(SDRAM_CTRL_REG+0x4) = (temp&0xff00ffff) | ((SDRAM_Cas_Latency<<20)|(SDRAM_Interleave<<19)|(SDRAM_Burst_Length<<16)) ;
    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG)		 = (temp&0xffffff00)|0x1;
}

void SDRAM_Refresh() {
    unsigned int temp;

    SDRAM_Wait_Idle();
    sdram_pr_debug ("*** SDRAM auto refreshing ......\n");
    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG) = ((temp&0xffffff00) | 0x3);

    //cpu2mem_cfgwr(4'h0,4'b1110,32'h3);
}

void SDRAM_Ref_Disable(void) {
    SDRAM_Wait_Idle();
    sdram_pr_debug ("*** SDRAM Disable Auto-Refresh Timer......\n");
    SDRAM_Set_Refresh_Timer(0x0);
    SDRAM_Wait_Idle();
}

void SDRAM_Set_Refresh_Timer(unsigned int ref_init)
{
    unsigned int 	temp;
    sdram_pr_debug("*** SDRAM set refresh timer......\n");
    temp = HW32_REG(SDRAM_CTRL_REG);
    HW32_REG(SDRAM_CTRL_REG) = ((temp&0xffff00FF) | (ref_init<<8));
    //cpu2mem_cfgwr(4'h0,4'b1101,{16'h0,ref_init,8'h0});
}

void dram_init (void)
{
    /*reference count:only init for one time*/
    static int referenceCount = 0;
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

}
