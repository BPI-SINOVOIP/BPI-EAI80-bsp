/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          sai_dev.h
 *
 * @author        wen.liu
 *
 * @version       1.0.0
 *
 * @date          2016/06/13
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/02/25, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _GM_LL_SAI_H_
#define _GM_LL_SAI_H_
#include <gm_common.h>
#include<gm_soc.h>
#include"gm_ll_sai_dev.h"

#ifdef __cplusplus
extern "C" {
#endif


#define SAI1_BASE             (APB2PERIPH_BASE + 0x5800U)// todo
#define SAI1_Block_A_BASE     (SAI1_BASE + 0x004U)// todo
#define SAI1_Block_B_BASE     (SAI1_BASE + 0x024U)// todo

#define SAI1                ((SAI_TypeDef *) SAI1_BASE)
#define SAI1_Block_A        ((SAI_Block_TypeDef *)SAI1_Block_A_BASE)
#define SAI1_Block_B        ((SAI_Block_TypeDef *)SAI1_Block_B_BASE)

/******************************************************************************/
/*                                                                            */
/*                          Serial Audio Interface                            */
/*                                                                            */
/******************************************************************************/

/*******************  Bit definition for SAI_xCR1 register  *******************/
#define SAI_xCR1_MODE_Pos          (0U)                                        
#define SAI_xCR1_MODE_Msk          (0x3U << SAI_xCR1_MODE_Pos)                 /*!< 0x00000003 */
#define SAI_xCR1_MODE              SAI_xCR1_MODE_Msk                           /*!<MODE[1:0] bits (Audio Block Mode)           */
#define SAI_xCR1_MODE_0            (0x1U << SAI_xCR1_MODE_Pos)                 /*!< 0x00000001 */
#define SAI_xCR1_MODE_1            (0x2U << SAI_xCR1_MODE_Pos)                 /*!< 0x00000002 */

#define SAI_xCR1_PRTCFG_Pos        (2U)                                        
#define SAI_xCR1_PRTCFG_Msk        (0x3U << SAI_xCR1_PRTCFG_Pos)               /*!< 0x0000000C */
#define SAI_xCR1_PRTCFG            SAI_xCR1_PRTCFG_Msk                         /*!<PRTCFG[1:0] bits (Protocol Configuration)   */
#define SAI_xCR1_PRTCFG_0          (0x1U << SAI_xCR1_PRTCFG_Pos)               /*!< 0x00000004 */
#define SAI_xCR1_PRTCFG_1          (0x2U << SAI_xCR1_PRTCFG_Pos)               /*!< 0x00000008 */

#define SAI_xCR1_DS_Pos            (5U)                                        
#define SAI_xCR1_DS_Msk            (0x7U << SAI_xCR1_DS_Pos)                   /*!< 0x000000E0 */
#define SAI_xCR1_DS                SAI_xCR1_DS_Msk                             /*!<DS[1:0] bits (Data Size) */
#define SAI_xCR1_DS_0              (0x1U << SAI_xCR1_DS_Pos)                   /*!< 0x00000020 */
#define SAI_xCR1_DS_1              (0x2U << SAI_xCR1_DS_Pos)                   /*!< 0x00000040 */
#define SAI_xCR1_DS_2              (0x4U << SAI_xCR1_DS_Pos)                   /*!< 0x00000080 */

#define SAI_xCR1_LSBFIRST_Pos      (8U)                                        
#define SAI_xCR1_LSBFIRST_Msk      (0x1U << SAI_xCR1_LSBFIRST_Pos)             /*!< 0x00000100 */
#define SAI_xCR1_LSBFIRST          SAI_xCR1_LSBFIRST_Msk                       /*!<LSB First Configuration  */
#define SAI_xCR1_CKSTR_Pos         (9U)                                        
#define SAI_xCR1_CKSTR_Msk         (0x1U << SAI_xCR1_CKSTR_Pos)                /*!< 0x00000200 */
#define SAI_xCR1_CKSTR             SAI_xCR1_CKSTR_Msk                          /*!<ClocK STRobing edge      */

#define SAI_xCR1_SYNCEN_Pos        (10U)                                       
#define SAI_xCR1_SYNCEN_Msk        (0x3U << SAI_xCR1_SYNCEN_Pos)               /*!< 0x00000C00 */
#define SAI_xCR1_SYNCEN            SAI_xCR1_SYNCEN_Msk                         /*!<SYNCEN[1:0](SYNChronization ENable) */
#define SAI_xCR1_SYNCEN_0          (0x1U << SAI_xCR1_SYNCEN_Pos)               /*!< 0x00000400 */
#define SAI_xCR1_SYNCEN_1          (0x2U << SAI_xCR1_SYNCEN_Pos)               /*!< 0x00000800 */

#define SAI_xCR1_MONO_Pos          (12U)                                       
#define SAI_xCR1_MONO_Msk          (0x1U << SAI_xCR1_MONO_Pos)                 /*!< 0x00001000 */
#define SAI_xCR1_MONO              SAI_xCR1_MONO_Msk                           /*!<Mono mode                  */
#define SAI_xCR1_OUTDRIV_Pos       (13U)                                       
#define SAI_xCR1_OUTDRIV_Msk       (0x1U << SAI_xCR1_OUTDRIV_Pos)              /*!< 0x00002000 */
#define SAI_xCR1_OUTDRIV           SAI_xCR1_OUTDRIV_Msk                        /*!<Output Drive               */
#define SAI_xCR1_SAIEN_Pos         (16U)                                       
#define SAI_xCR1_SAIEN_Msk         (0x1U << SAI_xCR1_SAIEN_Pos)                /*!< 0x00010000 */
#define SAI_xCR1_SAIEN             SAI_xCR1_SAIEN_Msk                          /*!<Audio Block enable         */
#define SAI_xCR1_DMAEN_Pos         (17U)                                       
#define SAI_xCR1_DMAEN_Msk         (0x1U << SAI_xCR1_DMAEN_Pos)                /*!< 0x00020000 */
#define SAI_xCR1_DMAEN             SAI_xCR1_DMAEN_Msk                          /*!<DMA enable                 */

#define SAI_xCR1_FSSOEN_Pos         (18U)                                       
#define SAI_xCR1_FSSOEN_Msk         (0x1U << SAI_xCR1_FSSOEN_Pos)                /*!< 0x00080000 */
#define SAI_xCR1_FSSOEN             SAI_xCR1_FSSOEN_Msk                          /*!<No Divider Configuration   */


#define SAI_xCR1_BCKSOEN_Pos         (19U)                                       
#define SAI_xCR1_BCKSOEN_Msk         (0x1U << SAI_xCR1_BCKSOEN_Pos)                /*!< 0x00080000 */
#define SAI_xCR1_BCKSOEN             SAI_xCR1_BCKSOEN_Msk                          /*!<No Divider Configuration   */

#define SAI_xCR1_MCKSOEN_Pos         (20U)                                       
#define SAI_xCR1_MCKSOEN_Msk         (0x1U << SAI_xCR1_MCKSOEN_Pos)                /*!< 0x00080000 */
#define SAI_xCR1_MCKSOEN             SAI_xCR1_MCKSOEN_Msk                          /*!<No Divider Configuration   */

#if 0
#define SAI_xCR1_NODIV_Pos         (19U)                                       
#define SAI_xCR1_NODIV_Msk         (0x1U << SAI_xCR1_NODIV_Pos)                /*!< 0x00080000 */
#define SAI_xCR1_NODIV             SAI_xCR1_NODIV_Msk                          /*!<No Divider Configuration   */

#define SAI_xCR1_MCKDIV_Pos        (20U)                                       
#define SAI_xCR1_MCKDIV_Msk        (0xFU << SAI_xCR1_MCKDIV_Pos)               /*!< 0x00F00000 */
#define SAI_xCR1_MCKDIV            SAI_xCR1_MCKDIV_Msk                         /*!<MCKDIV[3:0] (Master ClocK Divider)  */
#define SAI_xCR1_MCKDIV_0          (0x1U << SAI_xCR1_MCKDIV_Pos)               /*!< 0x00100000 */
#define SAI_xCR1_MCKDIV_1          (0x2U << SAI_xCR1_MCKDIV_Pos)               /*!< 0x00200000 */
#define SAI_xCR1_MCKDIV_2          (0x4U << SAI_xCR1_MCKDIV_Pos)               /*!< 0x00400000 */
#define SAI_xCR1_MCKDIV_3          (0x8U << SAI_xCR1_MCKDIV_Pos)               /*!< 0x00800000 */

#endif
/*******************  Bit definition for SAI_xCR2 register  *******************/
#define SAI_xCR2_FTH_Pos           (0U)                                        
#define SAI_xCR2_FTH_Msk           (0x7U << SAI_xCR2_FTH_Pos)                  /*!< 0x00000007 */
#define SAI_xCR2_FTH               SAI_xCR2_FTH_Msk                            /*!<FTH[2:0](Fifo THreshold)  */
#define SAI_xCR2_FTH_0             (0x1U << SAI_xCR2_FTH_Pos)                  /*!< 0x00000001 */
#define SAI_xCR2_FTH_1             (0x2U << SAI_xCR2_FTH_Pos)                  /*!< 0x00000002 */
#define SAI_xCR2_FTH_2             (0x4U << SAI_xCR2_FTH_Pos)                  /*!< 0x00000004 */

#define SAI_xCR2_FFLUSH_Pos        (3U)                                        
#define SAI_xCR2_FFLUSH_Msk        (0x1U << SAI_xCR2_FFLUSH_Pos)               /*!< 0x00000008 */
#define SAI_xCR2_FFLUSH            SAI_xCR2_FFLUSH_Msk                         /*!<Fifo FLUSH                       */
#define SAI_xCR2_TRIS_Pos          (4U)                                        
#define SAI_xCR2_TRIS_Msk          (0x1U << SAI_xCR2_TRIS_Pos)                 /*!< 0x00000010 */
#define SAI_xCR2_TRIS              SAI_xCR2_TRIS_Msk                           /*!<TRIState Management on data line */
#define SAI_xCR2_MUTE_Pos          (5U)                                        
#define SAI_xCR2_MUTE_Msk          (0x1U << SAI_xCR2_MUTE_Pos)                 /*!< 0x00000020 */
#define SAI_xCR2_MUTE              SAI_xCR2_MUTE_Msk                           /*!<Mute mode                        */
#define SAI_xCR2_MUTEVAL_Pos       (6U)                                        
#define SAI_xCR2_MUTEVAL_Msk       (0x1U << SAI_xCR2_MUTEVAL_Pos)              /*!< 0x00000040 */
#define SAI_xCR2_MUTEVAL           SAI_xCR2_MUTEVAL_Msk                        /*!<Muate value                      */

#define SAI_xCR2_MUTECNT_Pos       (7U)                                        
#define SAI_xCR2_MUTECNT_Msk       (0x3FU << SAI_xCR2_MUTECNT_Pos)             /*!< 0x00001F80 */
#define SAI_xCR2_MUTECNT           SAI_xCR2_MUTECNT_Msk                        /*!<MUTECNT[5:0] (MUTE counter) */
#define SAI_xCR2_MUTECNT_0         (0x01U << SAI_xCR2_MUTECNT_Pos)             /*!< 0x00000080 */
#define SAI_xCR2_MUTECNT_1         (0x02U << SAI_xCR2_MUTECNT_Pos)             /*!< 0x00000100 */
#define SAI_xCR2_MUTECNT_2         (0x04U << SAI_xCR2_MUTECNT_Pos)             /*!< 0x00000200 */
#define SAI_xCR2_MUTECNT_3         (0x08U << SAI_xCR2_MUTECNT_Pos)             /*!< 0x00000400 */
#define SAI_xCR2_MUTECNT_4         (0x10U << SAI_xCR2_MUTECNT_Pos)             /*!< 0x00000800 */
#define SAI_xCR2_MUTECNT_5         (0x20U << SAI_xCR2_MUTECNT_Pos)             /*!< 0x00001000 */

#define SAI_xCR2_CPL_Pos           (13U)                                       
#define SAI_xCR2_CPL_Msk           (0x1U << SAI_xCR2_CPL_Pos)                  /*!< 0x00002000 */
#define SAI_xCR2_CPL               SAI_xCR2_CPL_Msk                            /*!< Complement Bit             */

#define SAI_xCR2_COMP_Pos          (14U)                                       
#define SAI_xCR2_COMP_Msk          (0x3U << SAI_xCR2_COMP_Pos)                 /*!< 0x0000C000 */
#define SAI_xCR2_COMP              SAI_xCR2_COMP_Msk                           /*!<COMP[1:0] (Companding mode) */
#define SAI_xCR2_COMP_0            (0x1U << SAI_xCR2_COMP_Pos)                 /*!< 0x00004000 */
#define SAI_xCR2_COMP_1            (0x2U << SAI_xCR2_COMP_Pos)                 /*!< 0x00008000 */

/******************  Bit definition for SAI_xFRCR register  *******************/
#define SAI_xFRCR_FRL_Pos          (0U)                                        
#define SAI_xFRCR_FRL_Msk          (0xFFU << SAI_xFRCR_FRL_Pos)                /*!< 0x000000FF */
#define SAI_xFRCR_FRL              SAI_xFRCR_FRL_Msk                           /*!<FRL[1:0](Frame length)  */
#define SAI_xFRCR_FRL_0            (0x01U << SAI_xFRCR_FRL_Pos)                /*!< 0x00000001 */
#define SAI_xFRCR_FRL_1            (0x02U << SAI_xFRCR_FRL_Pos)                /*!< 0x00000002 */
#define SAI_xFRCR_FRL_2            (0x04U << SAI_xFRCR_FRL_Pos)                /*!< 0x00000004 */
#define SAI_xFRCR_FRL_3            (0x08U << SAI_xFRCR_FRL_Pos)                /*!< 0x00000008 */
#define SAI_xFRCR_FRL_4            (0x10U << SAI_xFRCR_FRL_Pos)                /*!< 0x00000010 */
#define SAI_xFRCR_FRL_5            (0x20U << SAI_xFRCR_FRL_Pos)                /*!< 0x00000020 */
#define SAI_xFRCR_FRL_6            (0x40U << SAI_xFRCR_FRL_Pos)                /*!< 0x00000040 */
#define SAI_xFRCR_FRL_7            (0x80U << SAI_xFRCR_FRL_Pos)                /*!< 0x00000080 */

#define SAI_xFRCR_FSALL_Pos        (8U)                                        
#define SAI_xFRCR_FSALL_Msk        (0x7FU << SAI_xFRCR_FSALL_Pos)              /*!< 0x00007F00 */
#define SAI_xFRCR_FSALL            SAI_xFRCR_FSALL_Msk                         /*!<FRL[1:0] (Frame synchronization active level length)  */
#define SAI_xFRCR_FSALL_0          (0x01U << SAI_xFRCR_FSALL_Pos)              /*!< 0x00000100 */
#define SAI_xFRCR_FSALL_1          (0x02U << SAI_xFRCR_FSALL_Pos)              /*!< 0x00000200 */
#define SAI_xFRCR_FSALL_2          (0x04U << SAI_xFRCR_FSALL_Pos)              /*!< 0x00000400 */
#define SAI_xFRCR_FSALL_3          (0x08U << SAI_xFRCR_FSALL_Pos)              /*!< 0x00000800 */
#define SAI_xFRCR_FSALL_4          (0x10U << SAI_xFRCR_FSALL_Pos)              /*!< 0x00001000 */
#define SAI_xFRCR_FSALL_5          (0x20U << SAI_xFRCR_FSALL_Pos)              /*!< 0x00002000 */
#define SAI_xFRCR_FSALL_6          (0x40U << SAI_xFRCR_FSALL_Pos)              /*!< 0x00004000 */

#define SAI_xFRCR_FSDEF_Pos        (16U)                                       
#define SAI_xFRCR_FSDEF_Msk        (0x1U << SAI_xFRCR_FSDEF_Pos)               /*!< 0x00010000 */
#define SAI_xFRCR_FSDEF            SAI_xFRCR_FSDEF_Msk                         /*!< Frame Synchronization Definition */
#define SAI_xFRCR_FSPOL_Pos        (17U)                                       
#define SAI_xFRCR_FSPOL_Msk        (0x1U << SAI_xFRCR_FSPOL_Pos)               /*!< 0x00020000 */
#define SAI_xFRCR_FSPOL            SAI_xFRCR_FSPOL_Msk                         /*!<Frame Synchronization POLarity    */
#define SAI_xFRCR_FSOFF_Pos        (18U)                                       
#define SAI_xFRCR_FSOFF_Msk        (0x1U << SAI_xFRCR_FSOFF_Pos)               /*!< 0x00040000 */
#define SAI_xFRCR_FSOFF            SAI_xFRCR_FSOFF_Msk                         /*!<Frame Synchronization OFFset      */
/* Legacy defines */
#define  SAI_xFRCR_FSPO                   SAI_xFRCR_FSPOL

/******************  Bit definition for SAI_xSLOTR register  *******************/
#define SAI_xSLOTR_FBOFF_Pos       (0U)                                        
#define SAI_xSLOTR_FBOFF_Msk       (0x1FU << SAI_xSLOTR_FBOFF_Pos)             /*!< 0x0000001F */
#define SAI_xSLOTR_FBOFF           SAI_xSLOTR_FBOFF_Msk                        /*!<FRL[4:0](First Bit Offset)  */
#define SAI_xSLOTR_FBOFF_0         (0x01U << SAI_xSLOTR_FBOFF_Pos)             /*!< 0x00000001 */
#define SAI_xSLOTR_FBOFF_1         (0x02U << SAI_xSLOTR_FBOFF_Pos)             /*!< 0x00000002 */
#define SAI_xSLOTR_FBOFF_2         (0x04U << SAI_xSLOTR_FBOFF_Pos)             /*!< 0x00000004 */
#define SAI_xSLOTR_FBOFF_3         (0x08U << SAI_xSLOTR_FBOFF_Pos)             /*!< 0x00000008 */
#define SAI_xSLOTR_FBOFF_4         (0x10U << SAI_xSLOTR_FBOFF_Pos)             /*!< 0x00000010 */

#define SAI_xSLOTR_SLOTSZ_Pos      (6U)                                        
#define SAI_xSLOTR_SLOTSZ_Msk      (0x3U << SAI_xSLOTR_SLOTSZ_Pos)             /*!< 0x000000C0 */
#define SAI_xSLOTR_SLOTSZ          SAI_xSLOTR_SLOTSZ_Msk                       /*!<SLOTSZ[1:0] (Slot size)  */
#define SAI_xSLOTR_SLOTSZ_0        (0x1U << SAI_xSLOTR_SLOTSZ_Pos)             /*!< 0x00000040 */
#define SAI_xSLOTR_SLOTSZ_1        (0x2U << SAI_xSLOTR_SLOTSZ_Pos)             /*!< 0x00000080 */

#define SAI_xSLOTR_NBSLOT_Pos      (8U)                                        
#define SAI_xSLOTR_NBSLOT_Msk      (0xFU << SAI_xSLOTR_NBSLOT_Pos)             /*!< 0x00000F00 */
#define SAI_xSLOTR_NBSLOT          SAI_xSLOTR_NBSLOT_Msk                       /*!<NBSLOT[3:0] (Number of Slot in audio Frame)  */
#define SAI_xSLOTR_NBSLOT_0        (0x1U << SAI_xSLOTR_NBSLOT_Pos)             /*!< 0x00000100 */
#define SAI_xSLOTR_NBSLOT_1        (0x2U << SAI_xSLOTR_NBSLOT_Pos)             /*!< 0x00000200 */
#define SAI_xSLOTR_NBSLOT_2        (0x4U << SAI_xSLOTR_NBSLOT_Pos)             /*!< 0x00000400 */
#define SAI_xSLOTR_NBSLOT_3        (0x8U << SAI_xSLOTR_NBSLOT_Pos)             /*!< 0x00000800 */

#define SAI_xSLOTR_SLOTEN_Pos      (16U)                                       
#define SAI_xSLOTR_SLOTEN_Msk      (0xFFFFU << SAI_xSLOTR_SLOTEN_Pos)          /*!< 0xFFFF0000 */
#define SAI_xSLOTR_SLOTEN          SAI_xSLOTR_SLOTEN_Msk                       /*!<SLOTEN[15:0] (Slot Enable)  */

/*******************  Bit definition for SAI_xIMR register  *******************/
#define SAI_xIMR_OVRUDRIE_Pos      (0U)                                        
#define SAI_xIMR_OVRUDRIE_Msk      (0x1U << SAI_xIMR_OVRUDRIE_Pos)             /*!< 0x00000001 */
#define SAI_xIMR_OVRUDRIE          SAI_xIMR_OVRUDRIE_Msk                       /*!<Overrun underrun interrupt enable                              */
#define SAI_xIMR_MUTEDETIE_Pos     (1U)                                        
#define SAI_xIMR_MUTEDETIE_Msk     (0x1U << SAI_xIMR_MUTEDETIE_Pos)            /*!< 0x00000002 */
#define SAI_xIMR_MUTEDETIE         SAI_xIMR_MUTEDETIE_Msk                      /*!<Mute detection interrupt enable                                */
#define SAI_xIMR_WCKCFGIE_Pos      (2U)                                        
#define SAI_xIMR_WCKCFGIE_Msk      (0x1U << SAI_xIMR_WCKCFGIE_Pos)             /*!< 0x00000004 */
#define SAI_xIMR_WCKCFGIE          SAI_xIMR_WCKCFGIE_Msk                       /*!<Wrong Clock Configuration interrupt enable                     */
#define SAI_xIMR_FREQIE_Pos        (3U)                                        
#define SAI_xIMR_FREQIE_Msk        (0x1U << SAI_xIMR_FREQIE_Pos)               /*!< 0x00000008 */
#define SAI_xIMR_FREQIE            SAI_xIMR_FREQIE_Msk                         /*!<FIFO request interrupt enable                                  */
#define SAI_xIMR_CNRDYIE_Pos       (4U)                                        
#define SAI_xIMR_CNRDYIE_Msk       (0x1U << SAI_xIMR_CNRDYIE_Pos)              /*!< 0x00000010 */
#define SAI_xIMR_CNRDYIE           SAI_xIMR_CNRDYIE_Msk                        /*!<Codec not ready interrupt enable                               */
#define SAI_xIMR_AFSDETIE_Pos      (5U)                                        
#define SAI_xIMR_AFSDETIE_Msk      (0x1U << SAI_xIMR_AFSDETIE_Pos)             /*!< 0x00000020 */
#define SAI_xIMR_AFSDETIE          SAI_xIMR_AFSDETIE_Msk                       /*!<Anticipated frame synchronization detection interrupt enable   */
#define SAI_xIMR_LFSDETIE_Pos      (6U)                                        
#define SAI_xIMR_LFSDETIE_Msk      (0x1U << SAI_xIMR_LFSDETIE_Pos)             /*!< 0x00000040 */
#define SAI_xIMR_LFSDETIE          SAI_xIMR_LFSDETIE_Msk                       /*!<Late frame synchronization detection interrupt enable          */

/********************  Bit definition for SAI_xSR register  *******************/
#define SAI_xSR_OVRUDR_Pos         (0U)                                        
#define SAI_xSR_OVRUDR_Msk         (0x1U << SAI_xSR_OVRUDR_Pos)                /*!< 0x00000001 */
#define SAI_xSR_OVRUDR             SAI_xSR_OVRUDR_Msk                          /*!<Overrun underrun                               */
#define SAI_xSR_MUTEDET_Pos        (1U)                                        
#define SAI_xSR_MUTEDET_Msk        (0x1U << SAI_xSR_MUTEDET_Pos)               /*!< 0x00000002 */
#define SAI_xSR_MUTEDET            SAI_xSR_MUTEDET_Msk                         /*!<Mute detection                                 */
#define SAI_xSR_WCKCFG_Pos         (2U)                                        
#define SAI_xSR_WCKCFG_Msk         (0x1U << SAI_xSR_WCKCFG_Pos)                /*!< 0x00000004 */
#define SAI_xSR_WCKCFG             SAI_xSR_WCKCFG_Msk                          /*!<Wrong Clock Configuration                      */
#define SAI_xSR_FREQ_Pos           (3U)                                        
#define SAI_xSR_FREQ_Msk           (0x1U << SAI_xSR_FREQ_Pos)                  /*!< 0x00000008 */
#define SAI_xSR_FREQ               SAI_xSR_FREQ_Msk                            /*!<FIFO request                                   */
#define SAI_xSR_CNRDY_Pos          (4U)                                        
#define SAI_xSR_CNRDY_Msk          (0x1U << SAI_xSR_CNRDY_Pos)                 /*!< 0x00000010 */
#define SAI_xSR_CNRDY              SAI_xSR_CNRDY_Msk                           /*!<Codec not ready                                */
#define SAI_xSR_AFSDET_Pos         (5U)                                        
#define SAI_xSR_AFSDET_Msk         (0x1U << SAI_xSR_AFSDET_Pos)                /*!< 0x00000020 */
#define SAI_xSR_AFSDET             SAI_xSR_AFSDET_Msk                          /*!<Anticipated frame synchronization detection    */
#define SAI_xSR_LFSDET_Pos         (6U)                                        
#define SAI_xSR_LFSDET_Msk         (0x1U << SAI_xSR_LFSDET_Pos)                /*!< 0x00000040 */
#define SAI_xSR_LFSDET             SAI_xSR_LFSDET_Msk                          /*!<Late frame synchronization detection           */

#define SAI_xSR_FLVL_Pos           (16U)                                       
#define SAI_xSR_FLVL_Msk           (0x7U << SAI_xSR_FLVL_Pos)                  /*!< 0x00070000 */
#define SAI_xSR_FLVL               SAI_xSR_FLVL_Msk                            /*!<FLVL[2:0] (FIFO Level Threshold)               */
#define SAI_xSR_FLVL_0             (0x1U << SAI_xSR_FLVL_Pos)                  /*!< 0x00010000 */
#define SAI_xSR_FLVL_1             (0x2U << SAI_xSR_FLVL_Pos)                  /*!< 0x00020000 */
#define SAI_xSR_FLVL_2             (0x4U << SAI_xSR_FLVL_Pos)                  /*!< 0x00040000 */

/******************  Bit definition for SAI_xCLRFR register  ******************/
#define SAI_xCLRFR_COVRUDR_Pos     (0U)                                        
#define SAI_xCLRFR_COVRUDR_Msk     (0x1U << SAI_xCLRFR_COVRUDR_Pos)            /*!< 0x00000001 */
#define SAI_xCLRFR_COVRUDR         SAI_xCLRFR_COVRUDR_Msk                      /*!<Clear Overrun underrun                               */
#define SAI_xCLRFR_CMUTEDET_Pos    (1U)                                        
#define SAI_xCLRFR_CMUTEDET_Msk    (0x1U << SAI_xCLRFR_CMUTEDET_Pos)           /*!< 0x00000002 */
#define SAI_xCLRFR_CMUTEDET        SAI_xCLRFR_CMUTEDET_Msk                     /*!<Clear Mute detection                                 */
#define SAI_xCLRFR_CWCKCFG_Pos     (2U)                                        
#define SAI_xCLRFR_CWCKCFG_Msk     (0x1U << SAI_xCLRFR_CWCKCFG_Pos)            /*!< 0x00000004 */
#define SAI_xCLRFR_CWCKCFG         SAI_xCLRFR_CWCKCFG_Msk                      /*!<Clear Wrong Clock Configuration                      */
#define SAI_xCLRFR_CFREQ_Pos       (3U)                                        
#define SAI_xCLRFR_CFREQ_Msk       (0x1U << SAI_xCLRFR_CFREQ_Pos)              /*!< 0x00000008 */
#define SAI_xCLRFR_CFREQ           SAI_xCLRFR_CFREQ_Msk                        /*!<Clear FIFO request                                   */
#define SAI_xCLRFR_CCNRDY_Pos      (4U)                                        
#define SAI_xCLRFR_CCNRDY_Msk      (0x1U << SAI_xCLRFR_CCNRDY_Pos)             /*!< 0x00000010 */
#define SAI_xCLRFR_CCNRDY          SAI_xCLRFR_CCNRDY_Msk                       /*!<Clear Codec not ready                                */
#define SAI_xCLRFR_CAFSDET_Pos     (5U)                                        
#define SAI_xCLRFR_CAFSDET_Msk     (0x1U << SAI_xCLRFR_CAFSDET_Pos)            /*!< 0x00000020 */
#define SAI_xCLRFR_CAFSDET         SAI_xCLRFR_CAFSDET_Msk                      /*!<Clear Anticipated frame synchronization detection    */
#define SAI_xCLRFR_CLFSDET_Pos     (6U)                                        
#define SAI_xCLRFR_CLFSDET_Msk     (0x1U << SAI_xCLRFR_CLFSDET_Pos)            /*!< 0x00000040 */
#define SAI_xCLRFR_CLFSDET         SAI_xCLRFR_CLFSDET_Msk                      /*!<Clear Late frame synchronization detection           */

/******************  Bit definition for SAI_xDR register  ******************/
#define SAI_xDR_DATA_Pos           (0U)                                        
#define SAI_xDR_DATA_Msk           (0xFFFFFFFFU << SAI_xDR_DATA_Pos)           /*!< 0xFFFFFFFF */
#define SAI_xDR_DATA               SAI_xDR_DATA_Msk

/******************  Bit definition for SAI_xClock register  ******************/
#define SAI_xCKCTL_BCKDIV_Pos                      ((uint32_t) 0)                                    /* UART stop bit select pos */
#define SAI_xCKCTL_BCKDIV                          ((uint32_t) ((uint32_t)0xFF << SAI_xCKCTL_BCKDIV_Pos))   /* UART stop bit select filed */

#define SAI_xCKCTL_MCKDIV_Pos                      ((uint32_t) 8)                                    /* UART stop bit select pos */
#define SAI_xCKCTL_MCKDIV                          ((uint32_t) ((uint32_t)0x7F << SAI_xCKCTL_MCKDIV_Pos))   /* UART stop bit select filed */

#define SAI_xCKCTL_MCKNODIV_Pos         (17U)                                       
#define SAI_xCKCTL_MCKNODIV_Msk         (0x1U << SAI_xCKCTL_MCKNODIV_Pos)                /*!< 0x00020000 */
#define SAI_xCKCTL_MCKNODIV             SAI_xCKCTL_MCKNODIV_Msk                          /*!<DMA enable       

/******************  Bit definition for SAI version register  ******************/
#define SAI_SAIVER_VN_Pos                      ((uint32_t) 0)                                    /* UART stop bit select pos */
#define SAI_SAIVER_VN                          ((uint32_t) ((uint32_t)0xFFFFFFFF << SAI_SAIVER_VN_Pos))   /* UART stop bit select filed */

/**
 * @brief Set rts
 *
 * @param  pUART           pUART devive Point
 * @param  val             Rts value
 *
 * @retval void
 */
#define LL_SAI_SET_BCKDIV_BIT(pSAI, val)            IO_BITMASK_SET(pSAI->CLKCR, SAI_xCKCTL_BCKDIV, val << SAI_xCKCTL_BCKDIV_Pos); 

#define LL_SAI_SET_MCKNODIV_BIT(pSAI, val)          IO_BITMASK_SET(pSAI->CLKCR, SAI_xCKCTL_MCKNODIV, val << SAI_xCKCTL_MCKNODIV_Pos); 

/**
 * @brief Set rts
 *
 * @param  pUART           pUART devive Point
 * @param  val             Rts value
 *
 * @retval void
 */
#define LL_SAI_SET_MCKDIV_BIT(pSAI, val)            IO_BITMASK_SET(pSAI->CLKCR, SAI_xCKCTL_MCKDIV, val << SAI_xCKCTL_MCKDIV_Pos); 

/**
 * @brief Set rts
 *
 * @param  pUART           pUART devive Point
 * @param  val             Rts value
 *
 * @retval void
 */
#define LL_SAI_SET_VN_BIT(pSAI, val)            IO_BITMASK_SET(pSAI->VER, SAI_SAIVER_VN, val << SAI_SAIVER_VN_Pos); 

/**
 * @brief Set rts
 *
 * @param  pUART           pUART devive Point
 * @param  val             Rts value
 *
 * @retval void
 */
#define LL_SAI_FSSOEN_BIT(pSAI, val)            IO_BITMASK_SET(pSAI->CR1, SAI_xCR1_FSSOEN, val << SAI_xCR1_FSSOEN_Pos);

/**
 * @brief Set rts
 *
 * @param  pUART           pUART devive Point
 * @param  val             Rts value
 *
 * @retval void
 */
#define LL_SAI_BCKSOEN_BIT(pSAI, val)            IO_BITMASK_SET(pSAI->CR1, SAI_xCR1_BCKSOEN, val << SAI_xCR1_BCKSOEN_Pos); 

/**
 * @brief Set rts
 *
 * @param  pUART           pUART devive Point
 * @param  val             Rts value
 *
 * @retval void
 */
#define LL_SAI_MCKSOEN_BIT(pSAI, val)            IO_BITMASK_SET(pSAI->CR1, SAI_xCR1_MCKSOEN, val << SAI_xCR1_MCKSOEN_Pos); 

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_SAI_H_ */
