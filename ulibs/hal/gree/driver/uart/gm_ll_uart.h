/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart_dev.h
 *
 * @author        TerryLee
 *
 * @version       1.0.0
 *
 * @date          2018/04/03
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *        porting to ugelis system.
 */
#ifndef _GM_LL_UART_H_
#define _GM_LL_UART_H_

#include <gm_common.h>
#include <gm_soc.h>
#include "gm_ll_uart_dev.h"


#ifdef __cplusplus
extern "C" {
#endif


/**************************** UART Data register bitfiled *********************************/
#define UART_DR_RXBIT9_POS                   ((uint32_t) 8)                                   /* UART buffer for 9th bit data received pos */
#define UART_DR_RXBIT9                       ((uint32_t) ((uint32_t)1 << UART_DR_RXBIT9_POS)) /* UART buffer for 9th bit data received filed */
#define UART_DR_TXBIT9_POS                   ((uint32_t) 8)                                   /* UART buffer for 9th bit data transmitted pos */
#define UART_DR_TXBIT9                       ((uint32_t) ((uint32_t)1 << UART_DR_TXBIT9_POS)) /* UART buffer for 9th bit data transmitted filed */
#define UART_DR_BTBR_POS                     ((uint32_t) 7)                                   /* UART buffer for 9th bit data transmitted pos */
#define UART_DR_BTBR                         ((uint32_t) ((uint32_t)0xFF << UART_DR_BTBR_POS))/* UART buffer for 9th bit data transmitted filed */

/**************************** UART Baud Rate register bitfiled *********************************/
#define UART_TC_SCE_POS                      ((uint32_t) 28)                                   /* UART sample times configurable function pos */
#define UART_TC_SCE                          ((uint32_t) ((uint32_t)1 << UART_TC_SCE_POS))     /* UART sample times configurable function filed */
#define UART_TC_SRAL_POS                     ((uint32_t) 20)                                   /* UART added bits sample length pos */
#define UART_TC_SRAL                         ((uint32_t) ((uint32_t)0x3F << UART_TC_SRAL_POS)) /* UART added bits sample length filed */
#define UART_TC_STV_POS                      ((uint32_t) 16)                                   /* UART sample times value pos */
#define UART_TC_STV                          ((uint32_t) ((uint32_t)0xF << UART_TC_STV_POS))   /* UART sample times value filed */
#define UART_TC_BD_POS                       ((uint32_t) 0)                                    /* UART baud rate Divisor pos */
#define UART_TC_BD                           ((uint32_t) ((uint32_t)0xFFFF << UART_TC_BD_POS)) /* UART baud rate Divisor filed */
#define UART_TC_SCLK_FIELD(x)                (((uint32_t)(x))&0x0000FFFF)

/**************************** UART C1 register bitfiled *********************************/
#define UART_C1_DAT_POS                      ((uint32_t) 27)                                   /* UART enable assertion time pos */
#define UART_C1_DAT                          ((uint32_t) ((uint32_t)0x1F << UART_C1_DAT_POS))  /* UART enable assertion time filed */
#define UART_C1_DDT_POS                      ((uint32_t) 22)                                   /* UART de assertion time pos */
#define UART_C1_DDT                          ((uint32_t) ((uint32_t)0x1F << UART_C1_DDT_POS))  /* UART de assertion time filed */
#define UART_C1_DSV_POS                      ((uint32_t) 21)                                   /* UART de value written by software pos */
#define UART_C1_DSV                          ((uint32_t) ((uint32_t)1 << UART_C1_DSV_POS))     /* UART de value written by software filed */
#define UART_C1_DSS_POS                      ((uint32_t) 20)                                   /* UART de value control by software pos */
#define UART_C1_DSS                          ((uint32_t) ((uint32_t)1 << UART_C1_DSS_POS))     /* UART de value control by software filed */
#define UART_C1_DPS_POS                      ((uint32_t) 19)                                   /* UART driver enable polarity selection pos */
#define UART_C1_DPS                          ((uint32_t) ((uint32_t)1 << UART_C1_DPS_POS))     /* UART driver enable polarity selection filed */
#define UART_C1_DME_POS                      ((uint32_t) 18)                                   /* UART driver enable mode pos */
#define UART_C1_DME                          ((uint32_t) ((uint32_t)1 << UART_C1_DME_POS))     /* UART driver enable mode filed */
#define UART_C1_HDE_POS                      ((uint32_t) 16)                                   /* UART half-duplex mode pos */
#define UART_C1_HDE                          ((uint32_t) ((uint32_t)1 << UART_C1_RHCE_POS))    /* UART half-duplex mode filed */
#define UART_C1_RHCE_POS                     ((uint32_t) 15)                                   /* UART rts hardware flow control enable pos */
#define UART_C1_RHCE                         ((uint32_t) ((uint32_t)1 << UART_C1_CHCE_POS))    /* UART rts hardware flow control enable filed */
#define UART_C1_CHCE_POS                     ((uint32_t) 14)                                   /* UART cts hardware flow control enable pos */
#define UART_C1_CHCE                         ((uint32_t) ((uint32_t)1 << UART_C1_CHCE_POS))    /* UART cts hardware flow control enable filed */
#define UART_C1_ILE_POS                      ((uint32_t) 12)                                   /* UART irda loop back function enable pos */
#define UART_C1_ILE                          ((uint32_t) ((uint32_t)1 << UART_C1_ILE_POS))     /* UART irda loop back function enable filed */
#define UART_C1_ITIE_POS                     ((uint32_t) 11)                                   /* UART invert the irda tx enable pos */
#define UART_C1_ITIE                         ((uint32_t) ((uint32_t)1 << UART_C1_ITIE_POS))    /* UART invert the irda tx enable filed */
#define UART_C1_IRE_POS                      ((uint32_t) 10)                                   /* UART irda function enable pos */
#define UART_C1_IRE                          ((uint32_t) ((uint32_t)1 << UART_C1_IRE_POS))     /* UART irda function enable filed */
#define UART_C1_AGE_POS                      ((uint32_t) 8)                                    /* UART anti-glitch function enable pos */
#define UART_C1_AGE                          ((uint32_t) ((uint32_t)1 << UART_C1_AGE_POS))     /* UART anti-glitch function enable filed */
#define UART_C1_SGT_POS                      ((uint32_t) 4)                                    /* UART glitch threshold for start bit pos */
#define UART_C1_SGT                          ((uint32_t) ((uint32_t)0x7 << UART_C1_SGT_POS))   /* UART glitch threshold for start bit filed */
#define UART_C1_SGTST_POS                    ((uint32_t) 0)                                    /* UART glitch threshold for signal bit pos */
#define UART_C1_SGTSI                        ((uint32_t) ((uint32_t)0xF << UART_C1_SGTST_POS)) /* UART glitch threshold for signal bit filed */

/**************************** UART C2 register bitfiled *********************************/
#define UART_C2_TSLE_POS                     ((uint32_t) 30)                                   /* UART tx stop Length value enable pos */
#define UART_C2_TSLE                         ((uint32_t) ((uint32_t)0x1 << UART_C2_TSLE_POS))  /* UART tx stop Length value enable filed */
#define UART_C2_TSL_POS                      ((uint32_t) 24)                                   /* UART tx stop Length value enable pos */
#define UART_C2_TSL                          ((uint32_t) ((uint32_t)0x3F << UART_C2_TSL_POS))  /* UART tx stop Length value enable filed */
#define UART_C2_T9BDE_POS                    ((uint32_t) 17)                                   /* UART transmit 9 bit data function enable pos */
#define UART_C2_T9BDE                        ((uint32_t) ((uint32_t)0x1 << UART_C2_T9BDE_POS)) /* UART transmit 9 bit data function enable filed */
#define UART_C2_R9BDE_POS                    ((uint32_t) 16)                                   /* UART transmit 9 bit data function enable pos */
#define UART_C2_R9BDE                        ((uint32_t) ((uint32_t)0x1 << UART_C2_R9BDE_POS)) /* UART transmit 9 bit data function enable filed */
#define UART_C2_MS_POS                       ((uint32_t) 12)                                   /* UART most significant bit first pos */
#define UART_C2_MS                           ((uint32_t) ((uint32_t)0x1 << UART_C2_MS_POS))    /* UART most significant bit first filed */
#define UART_C2_ITE_POS                      ((uint32_t) 11)                                   /* UART most significant bit first pos */
#define UART_C2_ITE                          ((uint32_t) ((uint32_t)0x1 << UART_C2_ITE_POS))   /* UART most significant bit first filed */
#define UART_C2_IRE_POS                      ((uint32_t) 10)                                   /* UART invert rx input function pos */
#define UART_C2_IRE                          ((uint32_t) ((uint32_t)0x1 << UART_C2_IRE_POS))   /* UART invert rx input function filed */
#define UART_C2_FTE_POS                      ((uint32_t) 9)                                    /* UART forbid transmitter function pos */
#define UART_C2_FTE                          ((uint32_t) ((uint32_t)0x1 << UART_C2_FTE_POS))   /* UART forbid transmitter function filed */
#define UART_C2_FRE_POS                      ((uint32_t) 8)                                    /* UART forbid receiver function pos */
#define UART_C2_FRE                          ((uint32_t) ((uint32_t)0x1 << UART_C2_FRE_POS))   /* UART forbid receiver function filed */
#define UART_C2_TFR_POS                      ((uint32_t) 2)                                    /* UART tx fifo reset pos */
#define UART_C2_TFR                          ((uint32_t) ((uint32_t)0x1 << UART_C2_TFR_POS))   /* UART tx fifo reset filed */
#define UART_C2_RFR_POS                      ((uint32_t) 1)                                    /* UART rx fifo reset pos */
#define UART_C2_RFR                          ((uint32_t) ((uint32_t)0x1 << UART_C2_RFR_POS))   /* UART rx fifo reset filed */
#define UART_C2_FE_POS                       ((uint32_t) 0)                                    /* UART fifo function enable pos */
#define UART_C2_FE                           ((uint32_t) ((uint32_t)0x1 << UART_C2_FE_POS))    /* UART fifo function enable filed */

/**************************** UART C3 register bitfiled *********************************/
#define UART_C3_ADD7_4_POS                   ((uint32_t) 28)                                   /* UART address of the UART node pos */
#define UART_C3_ADD7_4                       ((uint32_t) ((uint32_t)0xF << UART_C3_ADD7_4_POS))/* UART address of the UART node filed */
#define UART_C3_ADD3_0_POS                   ((uint32_t) 24)                                   /* UART address of the UART node pos */
#define UART_C3_ADD3_0                       ((uint32_t) ((uint32_t)0xF << UART_C3_ADD3_0_POS))/* UART address of the UART node filed */
#define UART_C3_LME_POS                      ((uint32_t) 20)                                   /* UART loop mode enable pos */
#define UART_C3_LME                          ((uint32_t) ((uint32_t)0x1 << UART_C3_LME_POS))   /* UART loop mode enable filed */
#define UART_C3_DOV_POS                      ((uint32_t) 19)                                   /* UART output value of data carrier detect pos */
#define UART_C3_DOV                          ((uint32_t) ((uint32_t)0x1 << UART_C3_DOV_POS))   /* UART output value of data carrier detect filed */
#define UART_C3_ROV_POS                      ((uint32_t) 18)                                   /* UART output value of ring indicator pos */
#define UART_C3_ROV                          ((uint32_t) ((uint32_t)0x1 << UART_C3_ROV_POS))   /* UART output value of ring indicator filed */
#define UART_C3_DTROV_POS                    ((uint32_t) 17)                                   /* UART output value of data terminal ready pos */
#define UART_C3_DTROV                        ((uint32_t) ((uint32_t)0x1 << UART_C3_DTROV_POS)) /* UART output value of data terminal ready pos */
#define UART_C3_RTSOV_POS                    ((uint32_t) 16)                                   /* UART output value of RTS (Request to Send) pos */
#define UART_C3_RTSOV                        ((uint32_t) ((uint32_t)0x1 << UART_C3_RTSOV_POS)) /* UART output value of RTS (Request to Send) pos */
#define UART_C3_ADDM_POS                     ((uint32_t) 11)                                   /* UART 7bit address detection 4bit address detection pos */
#define UART_C3_ADDM                         ((uint32_t) ((uint32_t)0x1 << UART_C3_ADDM_POS))  /* UART 7bit address detection 4bit address detection filed */
#define UART_C3_MR_POS                       ((uint32_t) 10)                                   /* UART mute mode request pos */
#define UART_C3_MR                           ((uint32_t) ((uint32_t)0x1 << UART_C3_MR_POS))    /* UART mute mode request filed */
#define UART_C3_WS_POS                       ((uint32_t) 9)                                    /* UART receiver wakeup method pos */
#define UART_C3_WS                           ((uint32_t) ((uint32_t)0x1 << UART_C3_WS_POS))    /* UART receiver wakeup method filed */
#define UART_C3_MME_POS                      ((uint32_t) 8)                                    /* UART mute mode enable pos */
#define UART_C3_MME                          ((uint32_t) ((uint32_t)0x1 << UART_C3_MME_POS))   /* UART mute mode enable filed */
#define UART_C3_TBE_POS                      ((uint32_t) 6)                                    /* UART transmit break enable pos */
#define UART_C3_TBE                          ((uint32_t) ((uint32_t)0x1 << UART_C3_TBE_POS))   /* UART transmit break enable filed */
#define UART_C3_SE_POS                       ((uint32_t) 5)                                    /* UART stick parity bit enable pos */
#define UART_C3_SE                           ((uint32_t) ((uint32_t)0x1 << UART_C3_SE_POS))    /* UART stick parity bit enable filed */
#define UART_C3_EPS_POS                      ((uint32_t) 4)                                    /* UART even parity selection pos */
#define UART_C3_EPS                          ((uint32_t) ((uint32_t)0x1 << UART_C3_EPS_POS))   /* UART even parity selection filed */
#define UART_C3_PE_POS                       ((uint32_t) 3)                                    /* UART even parity selection pos */
#define UART_C3_PE                           ((uint32_t) ((uint32_t)0x1 << UART_C3_PE_POS))    /* UART even parity selection filed */
#define UART_C3_SS_POS                       ((uint32_t) 2)                                    /* UART stop bit select pos */
#define UART_C3_SS                           ((uint32_t) ((uint32_t)0x1 << UART_C3_SS_POS))    /* UART stop bit select filed */
#define UART_C3_WLS_POS                      ((uint32_t) 0)                                    /* UART stop bit select pos */
#define UART_C3_WLS                          ((uint32_t) ((uint32_t)0x3 << UART_C3_WLS_POS))   /* UART stop bit select filed */

/**************************** UART line status register bitfiled *********************************/
#define UART_LS_RS_POS                       ((uint32_t) 24)                                   /* UART receiver wakeup from mute mode pos */
#define UART_LS_RS                           ((uint32_t) ((uint32_t)0x1 << UART_LS_RS_POS))    /* UART receiver wakeup from mute mode filed */
#define UART_LS_DIV_POS                      ((uint32_t) 23)                                   /* UART the value input signal data carrier detect pos */
#define UART_LS_DIV                          ((uint32_t) ((uint32_t)0x1 << UART_LS_DIV_POS))   /* UART the value input signal data carrier detect filed */
#define UART_LS_RIV_POS                      ((uint32_t) 22)                                   /* UART the value input signal ring indicator pos */
#define UART_LS_RIV                          ((uint32_t) ((uint32_t)0x1 << UART_LS_RIV_POS))   /* UART the value input signal ring indicator filed */
#define UART_LS_DSRIV_POS                    ((uint32_t) 21)                                   /* UART the value input signal dsr pos */
#define UART_LS_DSRIV                        ((uint32_t) ((uint32_t)0x1 << UART_LS_DSRIV_POS)) /* UART the value input signal dsr filed */
#define UART_LS_CIV_POS                      ((uint32_t) 20)                                   /* UART the value input signal cts pos */
#define UART_LS_CIV                          ((uint32_t) ((uint32_t)0x1 << UART_LS_CIV_POS))   /* UART the value input signal cts filed */
#define UART_LS_DCS_POS                      ((uint32_t) 19)                                   /* UART data carrier detect input change status pos */
#define UART_LS_DCS                          ((uint32_t) ((uint32_t)0x1 << UART_LS_DCS_POS))   /* UART data carrier detect input change status filed */
#define UART_LS_RPOS_POS                     ((uint32_t) 18)                                   /* UART ring indicator input pos edge status pos */
#define UART_LS_RPOS                         ((uint32_t) ((uint32_t)0x1 << UART_LS_RPOS_POS))  /* UART ring indicator input pos edge status filed */
#define UART_LS_DCHS_POS                     ((uint32_t) 17)                                   /* UART data set ready input change status pos */
#define UART_LS_DCHS                         ((uint32_t) ((uint32_t)0x1 << UART_LS_DCHS_POS))  /* UART data Set ready input change status filed */
#define UART_LS_CCS_POS                      ((uint32_t) 16)                                   /* UART Clear to send input change status pos */
#define UART_LS_CCS                          ((uint32_t) ((uint32_t)0x1 << UART_LS_CCS_POS))   /* UART Clear to send input change status filed */
#define UART_LS_TES_POS                      ((uint32_t) 7)                                    /* UART transmitter empty status pos */
#define UART_LS_TXE                          ((uint32_t) ((uint32_t)0x1 << UART_LS_TES_POS))   /* UART transmitter empty status filed */
#define UART_LS_THES_POS                     ((uint32_t) 6)                                    /* UART transmitter holding register empty status pos */
#define UART_LS_TXHE                         ((uint32_t) ((uint32_t)0x1 << UART_LS_THES_POS))  /* UART transmitter holding register empty status filed */
#define UART_LS_DRS_POS                      ((uint32_t) 5)                                    /* UART data ready status pos */
#define UART_LS_RXDA                         ((uint32_t) ((uint32_t)0x1 << UART_LS_DRS_POS))   /* UART data ready status filed */
#define UART_LS_RFES_POS                     ((uint32_t) 4)                                    /* UART data error in rx fifo status pos */
#define UART_LS_RFES                         ((uint32_t) ((uint32_t)0x1 << UART_LS_RFES_POS))  /* UART data error in rx fifo status filed */
#define UART_LS_RBS_POS                      ((uint32_t) 3)                                    /* UART receive break status pos */
#define UART_LS_RBS                          ((uint32_t) ((uint32_t)0x1 << UART_LS_RBS_POS))   /* UART receive break status filed */
#define UART_LS_RFS_POS                      ((uint32_t) 2)                                    /* UART receive frame error status pos */
#define UART_LS_RFS                          ((uint32_t) ((uint32_t)0x1 << UART_LS_RFS_POS))   /* UART receive frame error status filed */
#define UART_LS_RPS_POS                      ((uint32_t) 1)                                    /* UART receive parity error status pos */
#define UART_LS_RPS                          ((uint32_t) ((uint32_t)0x1 << UART_LS_RPS_POS))   /* UART receive parity error status filed */
#define UART_LS_ROS_POS                      ((uint32_t) 0)                                    /* UART receive overrun error status pos */
#define UART_LS_ROS                          ((uint32_t) ((uint32_t)0x1 << UART_LS_ROS_POS))   /* UART receive overrun error status filed */
#define UART_LS_RXES                         (UART_LS_ROS | UART_LS_RPS | UART_LS_RFS | UART_LS_RFES) /* UART receives error character */

/**************************** UART trigger register bitfiled *********************************/
#define UART_TH_RFC_POS                       ((uint32_t) 24)                                     /* UART rx fifread out while reading operation occurs pos */
#define UART_TH_RFC                           ((uint32_t) ((uint32_t)0x1F << UART_TH_RFC_POS))    /* UART rx firead out while reading operation occurs filed */
#define UART_TH_RTL_POS                       ((uint32_t) 16)                                     /* UART rx fifread out while reading operation occurs pos */
#define UART_TH_RTL                           ((uint32_t) ((uint32_t)0x1F << UART_TH_RTL_POS))    /* UART rx fifread out while reading operation occurs filed */
#define UART_TH_TFC_POS                       ((uint32_t) 8)                                      /* UART tx fiead out while reading operation occurs pos */
#define UART_TH_TFC                           ((uint32_t) ((uint32_t)0x1F << UART_TH_TFC_POS))    /* UART tx fifoead out while reading operation occurs filed */
#define UART_TH_TTL_POS                       ((uint32_t) 0)                                      /* UART tx fifo trigger level pos */
#define UART_TH_TTL                           ((uint32_t) ((uint32_t)0x1F << UART_TH_TTL_POS))    /* UART tx fifo trigger level filed */

/************************** UART interrupt enable register bitfiled *******************************/
#define UART_IE_CMIE_POS                      ((uint32_t) 19)                                     /* UART Character match interrupt enable pos */
#define UART_IE_CMIE                          ((uint32_t) ((uint32_t)0x1 << UART_IE_CMIE_POS))    /* UART Character match interrupt enable filed */
#define UART_IE_IIE_POS                       ((uint32_t) 18)                                     /* UART  idle interrupt enable pos */
#define UART_IE_IIE                           ((uint32_t) ((uint32_t)0x1 << UART_IE_IIE_POS))     /* UART idle interrupt enable filed */
#define UART_IE_RFTRIE_POS                    ((uint32_t) 17)                                     /* UART rx fifo trigger interrupt enable pos */
#define UART_IE_RFT                           ((uint32_t) ((uint32_t)0x1 << UART_IE_RFTRIE_POS))  /* UART rx fifo trigger interrupt enable filed */
#define UART_IE_TFTIE_POS                     ((uint32_t) 16)                                     /* UART tx fifo trigger interrupt enable pos */
#define UART_IE_TFT                           ((uint32_t) ((uint32_t)0x1 << UART_IE_TFTIE_POS))   /* UART rx fifo trigger interrupt enable filed */
#define UART_IE_CSIE_POS                      ((uint32_t) 11)                                     /* UART enable cd status interrupt pos */
#define UART_IE_CSIE                          ((uint32_t) ((uint32_t)0x1 << UART_IE_CSIE_POS))    /* UART enable cd status interrupt filed */
#define UART_IE_RSIE_POS                      ((uint32_t) 10)                                     /* UART enable ri status interrupt pos */
#define UART_IE_RSIE                          ((uint32_t) ((uint32_t)0x1 << UART_IE_RSIE_POS))    /* UART enable ri status interrupt filed */
#define UART_IE_DSIE_POS                      ((uint32_t) 9)                                      /* UART enable dst status interrupt pos */
#define UART_IE_DSIE                          ((uint32_t) ((uint32_t)0x1 << UART_IE_DSIE_POS))    /* UART enable dst status interrupt filed */
#define UART_IE_CSI_POS                       ((uint32_t) 8)                                      /* UART enable cts status interrupt pos */
#define UART_IE_CSI                           ((uint32_t) ((uint32_t)0x1 << UART_IE_CSI_POS))     /* UART enable cts status interrupt filed */
#define UART_IE_TEE_POS                       ((uint32_t) 7)                                      /* UART transmitter empty interrupt enable pos */
#define UART_IE_TEE                           ((uint32_t) ((uint32_t)0x1 << UART_IE_TEE_POS))     /* UART transmitter empty interrupt enable filed */
#define UART_IE_THEIE_POS                     ((uint32_t) 6)                                      /* UART transmitter holding register empty interrupt enable pos */
#define UART_IE_TXHE                          ((uint32_t) ((uint32_t)0x1 << UART_IE_THEIE_POS))   /* UART transmitter holding register empty interrupt enable filed */
#define UART_IE_RXDA_POS                      ((uint32_t) 5)                                      /* UART receive data available interrupt enable pos */
#define UART_IE_RXDA                          ((uint32_t) ((uint32_t)0x1 << UART_IE_RXDA_POS))    /* UART receive data available interrupt enable filed */
#define UART_IE_RFTIE_POS                     ((uint32_t) 4)                                      /* UART receive timerout interrupt enable pos */
#define UART_IE_RFTIE                         ((uint32_t) ((uint32_t)0x1 << UART_IE_RFTIE_POS))   /* UART receive timerout interrupt enable filed */
#define UART_IE_RBIE_POS                      ((uint32_t) 3)                                      /* UART receive break interrupt enable pos */
#define UART_IE_RBIE                          ((uint32_t) ((uint32_t)0x1 << UART_IE_RBIE_POS))    /* UART receive break interrupt enable filed */
#define UART_IE_RFIE_POS                      ((uint32_t) 2)                                      /* UART receive framing error interrupt enable pos */
#define UART_IE_RFIE                          ((uint32_t) ((uint32_t)0x1 << UART_IE_RFIE_POS))    /* UART receive framing error interrupt enable filed */
#define UART_IE_RPIE_POS                      ((uint32_t) 1)                                      /* UART receive parity error interrupt enable pos */
#define UART_IE_RPIE                          ((uint32_t) ((uint32_t)0x1 << UART_IE_RPIE_POS))    /* UART receive parity error interrupt enable filed */
#define UART_IE_ROIE_POS                      ((uint32_t) 0)                                      /* UART receive overrun error interrupt enable pos */
#define UART_IE_ROIE                          ((uint32_t) ((uint32_t)0x1 << UART_IE_ROIE_POS))    /* UART receive overrun error interrupt enable filed */
#define UART_IE_OPFB                          ((uint32_t)0x0000000F)                              /* Receive Line Error Interrupt Mask */

/************************** UART interrupt status register bitfiled *******************************/
#define UART_IS_CMF_POS                      ((uint32_t) 19)                                       /* UART character match flag pos */
#define UART_IS_CMF                          ((uint32_t) ((uint32_t)0x1 << UART_IS_CMF_POS))       /* UART character match flag filed */
#define UART_IS_IDLE_POS                     ((uint32_t) 18)                                       /* UART idle line detected pos */
#define UART_IS_IDLE                         ((uint32_t) ((uint32_t)0x1 << UART_IS_IDLE_POS))      /* UART idle line detected filed */
#define UART_IS_RFTI_POS                     ((uint32_t) 17)                                       /* UART rx fifo trigger interrupt status pos */
#define UART_IS_RFT                         ((uint32_t) ((uint32_t)0x1 << UART_IS_RFTI_POS))       /* UART rx fifo trigger interrupt status filed */
#define UART_IS_TFTI_POS                     ((uint32_t) 16)                                       /* UART tx fifo trigger interrupt status pos */
#define UART_IS_TFT                         ((uint32_t) ((uint32_t)0x1 << UART_IS_TFTI_POS))       /* UART tx fifo trigger interrupt status filed */
#define UART_IS_CDSI_POS                     ((uint32_t) 11)                                       /* UART cd status change interrupt status pos */
#define UART_IS_CDSI                         ((uint32_t) ((uint32_t)0x1 << UART_IS_CDSI_POS))      /* UART cd status change interrupt status filed */
#define UART_IS_RSI_POS                      ((uint32_t) 10)                                       /* UART ri status change interrupt status pos */
#define UART_IS_RSI                          ((uint32_t) ((uint32_t)0x1 << UART_IS_RSI_POS))       /* UART ri status change interrupt status filed */
#define UART_IS_DSI_POS                      ((uint32_t) 9)                                        /* UART dsr status change interrupt pos */
#define UART_IS_DSI                          ((uint32_t) ((uint32_t)0x1 << UART_IS_DSI_POS))       /* UART dsr status change interrupt filed */
#define UART_IS_CSI_POS                      ((uint32_t) 8)                                        /* UART cts status change interrupt pos */
#define UART_IS_CSI                          ((uint32_t) ((uint32_t)0x1 << UART_IS_CSI_POS))       /* UART cts status change interrupt filed */
#define UART_IS_TEI_POS                      ((uint32_t) 7)                                        /* UART transmitter empty interrupt pos */
#define UART_IS_TXE                          ((uint32_t) ((uint32_t)0x1 << UART_IS_TEI_POS))       /* UART transmitter empty interrupt filed */
#define UART_IS_THEI_POS                     ((uint32_t) 6)                                        /* UART transmitter holding register empty interrupt pos */
#define UART_IS_TXHE                         ((uint32_t) ((uint32_t)0x1 << UART_IS_TEI_POS))       /* UART transmitter holding register empty interrupt filed */
#define UART_IS_RDAI_POS                     ((uint32_t) 5)                                        /* UART receive data available interrupt pos */
#define UART_IS_RXDA                         ((uint32_t) ((uint32_t)0x1 << UART_IS_RDAI_POS))      /* UART receive data available interrupt filed */
#define UART_IS_RFTOI_POS                    ((uint32_t) 4)                                        /* UART rx fifo timeout interrupt pos */
#define UART_IS_RFTO                         ((uint32_t) ((uint32_t)0x1 << UART_IS_RFTOI_POS))     /* UART rx fifo timeout interrupt filed */
#define UART_IS_RBI_POS                      ((uint32_t) 3)                                        /* UART rceive break interrupt status pos */
#define UART_IS_RBI                          ((uint32_t) ((uint32_t)0x1 << UART_IS_RBI_POS))       /* UART receive break interrupt status filed */
#define UART_IS_RFI_POS                      ((uint32_t) 2)                                        /* UART receive Framing Error interrupt status  pos */
#define UART_IS_RFI                          ((uint32_t) ((uint32_t)0x1 << UART_IS_RFI_POS))       /* UART receive Framing Error interrupt status filed */
#define UART_IS_RPI_POS                      ((uint32_t) 1)                                        /* UART receive parity error interrupt status pos */
#define UART_IS_RPI                          ((uint32_t) ((uint32_t)0x1 << UART_IS_RPI_POS))       /* UART receive parity error interrupt status filed */
#define UART_IS_ROI_POS                      ((uint32_t) 0)                                        /* UART receive overrun error interrupt status pos */
#define UART_IS_ROI                          ((uint32_t) ((uint32_t)0x1 << UART_IS_ROI_POS))       /* UART receive overrun error interrupt status filed */
#define  UART_IS_OPFB                        ((uint32_t)0x0000000F)                                /* Receive Line Error Interrupt Status */

/************************** UART dma control register bitfiled *******************************/
#define UART_DMA_TDL_POS                     ((uint32_t) 24)                                       /* UART transmit data level pos */
#define UART_DMA_TDL                         ((uint32_t) ((uint32_t)0x1F << UART_DMA_TDL_POS))     /* UART transmit data level filed */
#define UART_DMA_TDE_POS                     ((uint32_t) 16)                                       /* UART DMA enable transmitter pos */
#define UART_DMA_TDE                         ((uint32_t) ((uint32_t)0x1 << UART_DMA_TDE_POS))      /* UART DMA enable transmitter filed */
#define UART_DMA_RDL_POS                     ((uint32_t) 8)                                        /* UART receive data level pos */
#define UART_DMA_RDL                         ((uint32_t) ((uint32_t)0x1F << UART_DMA_RDL_POS))     /* UART receive data level filed */
#define UART_DMA_RDE_POS                     ((uint32_t) 0)                                        /* UART DMA enable receiver pos */
#define UART_DMA_RDE                         ((uint32_t) ((uint32_t)0x1 << UART_DMA_RDE_POS))      /* UART DMA enable receiver filed */


#define UART0                                ((UART_Device_T *)(UART0_BASE))    /* UART0 reg base */
#define UART1                                ((UART_Device_T *)(UART1_BASE))    /* UART1 reg base */
#define UART2                                ((UART_Device_T *)(UART2_BASE))    /* UART2 reg base */
#define UART3                                ((UART_Device_T *)(UART3_BASE))    /* UART3 reg base */
#define UART4                                ((UART_Device_T *)(UART4_BASE))    /* UART4 reg base */
#define UART5                                ((UART_Device_T *)(UART5_BASE))    /* UART5 reg base */
#define UART6                                ((UART_Device_T *)(UART6_BASE))    /* UART6 reg base */
#define UART7                                ((UART_Device_T *)(UART7_BASE))    /* UART7 reg base */


#define LL_UART_TX_DISABLE(pUART)                   IO_BIT_SET(pUART->C2, UART_C2_FTE)
#define LL_UART_RX_DISABLE(pUART)                   IO_BIT_SET(pUART->C2, UART_C2_FRE)
#define LL_UART_TX_ENABLE(pUART)                    IO_BIT_CLEAR(pUART->C2, UART_C2_FTE)
#define LL_UART_RX_ENABLE(pUART)                    IO_BIT_CLEAR(pUART->C2, UART_C2_FRE)
#define LL_UART_TX_FIFO_RESET_ENABLE(pUART)         IO_BIT_SET(pUART->C2, UART_C2_TFR)
#define LL_UART_RX_FIFO_RESET_ENABLE(pUART)         IO_BIT_SET(pUART->C2, UART_C2_RFR)
#define LL_UART_FIFO_RESET_ENABLE(pUART)            IO_BIT_SET(pUART->C2, UART_C2_FE)
#define LL_UART_FIFO_RESET_DISABEL(pUART)           IO_BIT_CLEAR(pUART->C2, UART_C2_FE)

#define LL_UART_SET_RTS(pUART, val)                 IO_BITMASK_SET(pUART->C3, UART_C3_RTSOV, (val << UART_C3_RTSOV_POS))
#define LL_UART_SET_DTR(pUART, val)                 IO_BITMASK_SET(pUART->C3, UART_C3_DTROV, (val << UART_C3_DTROV_POS))
#define LL_UART_SET_STOPBIT(pUART, val)             IO_BITMASK_SET(pUART->C3, UART_C3_SS, (val << UART_C3_SS_POS))
#define LL_UART_SET_DATA_BIT(pUART, val)            IO_BITMASK_SET(pUART->C3, UART_C3_WLS, val << UART_C3_WLS_POS)
#define LL_UART_PARITY_ENABLE(pUART)                IO_BIT_SET(pUART->C3, UART_C3_PE)
#define LL_UART_SET_EVEN_PARITY(pUART)              IO_BIT_SET(pUART->C3, UART_C3_EPS)
#define LL_UART_PARITY_DISABLE(pUART)               IO_BIT_CLEAR(pUART->C3, UART_C3_PE)
#define LL_UART_SET_ODD_PARITY(pUART)               IO_BIT_CLEAR(pUART->C3, UART_C3_EPS)

#define LL_UART_TX_DMA_ENABLE(pUART)                IO_BIT_SET(pUART->DMA, UART_DMA_TDE)
#define LL_UART_RX_DMA_ENABLE(pUART)                IO_BIT_SET(pUART->DMA, UART_DMA_RDE)
#define LL_UART_TX_DMA_DISABLE(pUART)               IO_BIT_CLEAR(pUART->DMA, UART_DMA_TDE)
#define LL_UART_RX_DMA_DISABLE(pUART)               IO_BIT_CLEAR(pUART->DMA, UART_DMA_RDE)
#define LL_UART_TX_DMA_TDL(pUART, val)              IO_BITMASK_SET(pUART->DMA, UART_DMA_TDL, (val << UART_DMA_TDL_POS))
#define LL_UART_RX_DMA_RDL(pUART, val)              IO_BITMASK_SET(pUART->DMA, UART_DMA_RDL, (val << UART_DMA_RDL_POS))

#define LL_UART_SET_BAUDRATE_DIV(pUART, val)        IO_BITMASK_SET(pUART->TC, UART_TC_BD, (UART_TC_SCLK_FIELD(val) << UART_TC_BD_POS))
#define LL_UART_INTERRUPTES_DISABLE(pUART, mask)    IO_BIT_CLEAR(pUART->IE, mask)
#define LL_UART_GetIsrFlag(pUART, mask)             IO_BIT_GET(pUART->IS, mask)
#define LL_UART_ClearIsrFlag(pUART, mask)           IO_BIT_SET(pUART->IS, mask)

#define LL_UART_SET_TXFIFO_WATERMARK(pUART, val)    IO_BITMASK_SET(pUART->TH, UART_TH_TTL, (val << UART_TH_TTL_POS))
#define LL_UART_SET_RXFIFO_WATERMARK(pUART, val)    IO_BITMASK_SET(pUART->TH, UART_TH_RTL, (val << UART_TH_RTL_POS))
#define LL_UART_GET_TXFIFO_WAIT_NUM(pUART)          ((pUART->TH & UART_TH_TFC) >> UART_TH_TFC_POS)
#define LL_UART_GET_RXFIFO_WAIT_NUM(pUART)          ((pUART->TH & UART_TH_RFC) >> UART_TH_RFC_POS)
#define LL_UART_Is_C2_FE(pUART)                     (pUART->C2 & UART_C2_FE)
#define LL_UART_RESET_STATUS(pUART)                 (pUART->LS = 0)


//about TX config
__STATIC_INLINE void LL_UART_Enable_Tx_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IE, UART_IE_TXHE);
}

__STATIC_INLINE void LL_UART_Disable_Tx_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_CLEAR(pUART->IE, UART_IE_TXHE);
}

__STATIC_INLINE int LL_UART_Is_IE_TXHE(UART_Device_T *pUART)
{
    return !!(pUART->IE & UART_IE_TXHE);
}

__STATIC_INLINE int LL_UART_Is_IS_TXHE(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_TXHE);
}

__STATIC_INLINE void LL_UART_Clear_IS_TXHE(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IS, UART_IS_TXHE);
}

__STATIC_INLINE int LL_UART_Is_LS_TXHE(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_TXHE);
}

__STATIC_INLINE int LL_UART_Is_LS_TXE(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_TXE);
}

__STATIC_INLINE int LL_UART_Is_IS_TXE(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_TXE);
}

//about RX config
__STATIC_INLINE void LL_UART_Enable_Rx_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IE, (UART_IE_RXDA | UART_IE_RFTIE));
}

__STATIC_INLINE void LL_UART_Disable_Rx_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_CLEAR(pUART->IE, (UART_IE_RXDA | UART_IE_RFTIE));
}

__STATIC_INLINE int LL_UART_Is_IE_RXDA(UART_Device_T *pUART)
{
    return !!(pUART->IE & UART_IE_RXDA);
}

__STATIC_INLINE int LL_UART_Is_IS_RXDA(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_RXDA);
}

__STATIC_INLINE int LL_UART_Is_LS_RXDA(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RXDA);
}

//about Error config
__STATIC_INLINE void LL_UART_Enable_RxErr_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IE, UART_IE_OPFB);
}

__STATIC_INLINE void LL_UART_Disable_RxErr_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_CLEAR(pUART->IE, UART_IE_OPFB);
}

__STATIC_INLINE int LL_UART_Is_IE_OPFB(UART_Device_T *pUART)
{
    return !!(pUART->IE & UART_IE_OPFB);
}

__STATIC_INLINE void LL_UART_Clear_IS_OPFB(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IS, UART_IS_OPFB);
}

__STATIC_INLINE int LL_UART_Is_IS_OPFB(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_OPFB);
}

__STATIC_INLINE int LL_UART_Is_LS_ROS(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_ROS);
}

__STATIC_INLINE int LL_UART_Is_LS_RPS(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RPS);
}

__STATIC_INLINE int LL_UART_Is_LS_RFS(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RFS);
}

__STATIC_INLINE int LL_UART_Is_LS_RBS(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RBS);
}

__STATIC_INLINE int LL_UART_Is_LS_RFES(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RFES);
}

__STATIC_INLINE int LL_UART_Is_LS_RXES(UART_Device_T *pUART)
{
    return !!(pUART->LS & UART_LS_RXES);
}

//about TX Fifo config
__STATIC_INLINE void LL_UART_Enable_TxFifo_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IE, UART_IE_TFT);
}

__STATIC_INLINE void LL_UART_Disable_TxFifo_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_CLEAR(pUART->IE, UART_IE_TFT);
}

__STATIC_INLINE int LL_UART_Is_IE_TFT(UART_Device_T *pUART)
{
    return !!(pUART->IE & UART_IE_TFT);
}

__STATIC_INLINE void LL_UART_Clear_IS_TFT(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IS, UART_IS_TFT);
}

__STATIC_INLINE int LL_UART_Is_IS_TFT(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_TFT);
}

//about RX Fifo config
__STATIC_INLINE void LL_UART_Enable_RxFifo_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IE, UART_IE_RFT);
}

__STATIC_INLINE void LL_UART_Disable_RxFifo_Interrupts(UART_Device_T *pUART)
{
    IO_BIT_CLEAR(pUART->IE, UART_IE_RFT);
}

__STATIC_INLINE void LL_UART_Clear_IS_RFT(UART_Device_T *pUART)
{
    IO_BIT_SET(pUART->IS, UART_IS_RFT);
}

__STATIC_INLINE int LL_UART_Is_IS_RFT(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_RFT);
}

__STATIC_INLINE int LL_UART_Is_IS_RFTO(UART_Device_T *pUART)
{
    return !!(pUART->IS & UART_IS_RFTO);
}

//get char and put char
__STATIC_INLINE uint16_t LL_UART_GetChar(UART_Device_T *pUART)
{
    return ((uint16_t)(pUART->DR & 0x1FF));
}

__STATIC_INLINE void LL_UART_PutChar(UART_Device_T *pUART, uint16_t val)
{
    pUART->DR = val;
}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_LL_UART_H_ */

