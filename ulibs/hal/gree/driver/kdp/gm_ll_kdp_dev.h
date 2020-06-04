/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file		gm_ll_kdp_dev.h
 *
 * @author		Richard.Liu
 *
 * @version		v1.0.0
 *
 * @date		2019/01/24
 *
 * @brief		KDP Source file
 *
 * @note
 *				2019/01/24, Richard.Liu, v1.0.0
 *					Initial version.
 *
 **/

#ifndef _GM_LL_KDP_DEV_H_
#define _GM_LL_KDP_DEV_H_

/*gm include files*/
#include <gm_soc.h>
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif


#define KDP_CTRL					0x46000000
#define KDP_STATUS					0x46000004
#define CORE_BASEADDR				0x46000000

#define BRAM_OUTPUT_A_BASEARDDR     0x20020000
#define BRAM_OUTPUT_B_BASEARDDR     0x20030000

#define BRAM_OUTPUT_C_BASEARDDR     0x20040000
#define BRAM_OUTPUT_D_BASEARDDR     0x20050000

#define BRAM_CMD_BASEARDDR			0x20060000
#define BRAM_INPUT_BASEADDR			0x20020000 //Input Bank: BankA, BankB; Output Bank: BankC BankD; Bank Address:0x4000,0000; BankB address: 0x4002,0000;


#if 1
/** KDP - Register Layout Typedef */
typedef struct {
    __IO uint32_t CTRL;            /**< Offset: 0x0, read: input sampled at pin, write: to data output */
    __IO uint32_t STATUS;           /**< Offset: 0x4, read: input sampled at pin, write: to data output */
} KDP_Device_T;

#define  SYSKDP                               ((KDP_Device_T *)(KDP_CTRL))

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

typedef struct
{
    __IO uint32_t CTRL     ;
    __IO uint32_t STATUE   ;
    __IO uint32_t SRC      ;
    __IO uint32_t DATACNT  ;
    __IO uint32_t DATD     ;
    __IO uint32_t VERSION  ;
}STRFIFO_Device_T;

//* Base addresses of STRFIFO Peripheral Control Registers */
#define STRFIFO_BASE                               (0x46001000U)

/* I2C Device Pointer */
#define STRFIFO                                    ((STRFIFO_Device_T *)STRFIFO_BASE)


#define   AHB_SINGLE    0  //3'b000
#define   AHB_INCR      1  //3'b001
#define   AHB_WRAP4     2  //3'b010
#define   AHB_WRAP8     4  //3'b100
#define   AHB_WRAP16    6  //3'b110


#define   UNLEN4        0  //3'b000
#define   UNLEN8        1  //3'b001
#define   UNLEN16       2  //3'b010
#define   UNLEN32       3  //3'b011
#define   UNLEN64       4  //3'b100
#define   UNLEN128      5  //3'b101
#define   UNLEN256      6  //3'b110
#define   UNLEN512      7  //3'b111

#define   NORMAL_MODE   0
#define   CONTINUE_MODE 1


void streamfifo_init(uint32_t *src, uint32_t len, uint8_t burst_type, uint8_t burst_unlen, uint8_t continue_mode);

#endif 

#if 0

typedef struct {
    __IO uint32_t IO_BPS_REG[6];             /**< IO Debounced bypass control Regsiters: 0x220~0x234 */
    __IO uint32_t IO_OD_REG[6];              /**< IO Pad Open Drain Controller Register : 0x238~0x24C */
    __IO uint32_t IO_IE_REG[6];              /**< IO Input Enable Control Regsiters: 0x250~0x264 */
    __IO uint32_t IO_PU_REG[6];              /**< IO Pull Up Control Regsiters: 0x268~0x27C */
    __IO uint32_t IO_PD_REG[6];              /**< IO Pull DownControl Regsiters: 0x7C~0x90 */
    __IO uint32_t IO_DS_REG[11];             /**< IO Driven Strength Control Regsiters: 0x298~0x2C0*/
    __IO uint32_t IO_MODE_REG[11];           /**< IO Mode Control Regsiters:0x2C4~0x2EC*/
    __IO uint32_t IO_AF_REG[22];             /**< IO Alternate Function Control Regsiters:0x2F0~0x344*/

} SYSIO_Device_T;

#define  SYSIO                               ((SYSIO_Device_T *)(0x40000220u))


typedef struct {
    __IO uint32_t DB_CLK_DIV_CTRL;			/**<  sourced by DB_SRC_CLK divide Register : 0x400000BC */
	__IO uint32_t DB_CLK_SRC_CTRL;			/**<  group IO debounce clock source setting Register : 0x400000C0 */
} SYS_DB_CLK_Device_T;

#define  SYSDBCLKREG                       ((SYS_DB_CLK_Device_T *)(0x400000BCu))


typedef struct {
    __IO uint32_t IP_RST_CTRL[3];
	__IO uint32_t RES;
    __IO uint32_t CLK_EN_CTRL[3];              /**< Clock enable register Register : 0x80~0x88 */
	__IO uint32_t RES1;
	__IO uint32_t CLK_SRC_CTRL[11];            /* System clock control register and  Clock source control register: 0x90~0xB8*/

} SYS_RST_CLK_Device_T;

#define  SYSRSTCLKREG                       ((SYS_RST_CLK_Device_T *)(0x40000070u))

#endif

#ifdef __cplusplus
}
#endif

#endif
