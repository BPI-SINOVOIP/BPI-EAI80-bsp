
#ifndef _GM_LL_FMC_H_
#define _GM_LL_FMC_H_
#include <gm_common.h>
#include <gm_soc.h>
#include "gm_ll_fmc_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                                                                            */
/*                          Flexible Memory Controller                        */
/*                                                                            */
/******************************************************************************/
/******************  Bit definition for FMC_BCR1 register  *******************/
#define FMC_BCR1_MBKEN_Pos          (0U)                                       
#define FMC_BCR1_MBKEN_Msk          (0x1U << FMC_BCR1_MBKEN_Pos)               /*!< 0x00000001 */
#define FMC_BCR1_MBKEN              FMC_BCR1_MBKEN_Msk                         /*!<Memory bank enable bit                 */
#define FMC_BCR1_MUXEN_Pos          (1U)                                       
#define FMC_BCR1_MUXEN_Msk          (0x1U << FMC_BCR1_MUXEN_Pos)               /*!< 0x00000002 */
#define FMC_BCR1_MUXEN              FMC_BCR1_MUXEN_Msk                         /*!<Address/data multiplexing enable bit   */

#define FMC_BCR1_MTYP_Pos           (2U)                                       
#define FMC_BCR1_MTYP_Msk           (0x3U << FMC_BCR1_MTYP_Pos)                /*!< 0x0000000C */
#define FMC_BCR1_MTYP               FMC_BCR1_MTYP_Msk                          /*!<MTYP[1:0] bits (Memory type)           */
#define FMC_BCR1_MTYP_0             (0x1U << FMC_BCR1_MTYP_Pos)                /*!< 0x00000004 */
#define FMC_BCR1_MTYP_1             (0x2U << FMC_BCR1_MTYP_Pos)                /*!< 0x00000008 */

#define FMC_BCR1_MWID_Pos           (4U)                                       
#define FMC_BCR1_MWID_Msk           (0x3U << FMC_BCR1_MWID_Pos)                /*!< 0x00000030 */
#define FMC_BCR1_MWID               FMC_BCR1_MWID_Msk                          /*!<MWID[1:0] bits (Memory data bus width) */
#define FMC_BCR1_MWID_0             (0x1U << FMC_BCR1_MWID_Pos)                /*!< 0x00000010 */
#define FMC_BCR1_MWID_1             (0x2U << FMC_BCR1_MWID_Pos)                /*!< 0x00000020 */

#define FMC_BCR1_FACCEN_Pos         (6U)                                       
#define FMC_BCR1_FACCEN_Msk         (0x1U << FMC_BCR1_FACCEN_Pos)              /*!< 0x00000040 */
#define FMC_BCR1_FACCEN             FMC_BCR1_FACCEN_Msk                        /*!<Flash access enable        */
#define FMC_BCR1_BURSTEN_Pos        (8U)                                       
#define FMC_BCR1_BURSTEN_Msk        (0x1U << FMC_BCR1_BURSTEN_Pos)             /*!< 0x00000100 */
#define FMC_BCR1_BURSTEN            FMC_BCR1_BURSTEN_Msk                       /*!<Burst enable bit           */
#define FMC_BCR1_WAITPOL_Pos        (9U)                                       
#define FMC_BCR1_WAITPOL_Msk        (0x1U << FMC_BCR1_WAITPOL_Pos)             /*!< 0x00000200 */
#define FMC_BCR1_WAITPOL            FMC_BCR1_WAITPOL_Msk                       /*!<Wait signal polarity bit   */
#define FMC_BCR1_WRAPMOD_Pos        (10U)                                      
#define FMC_BCR1_WRAPMOD_Msk        (0x1U << FMC_BCR1_WRAPMOD_Pos)             /*!< 0x00000400 */
#define FMC_BCR1_WRAPMOD            FMC_BCR1_WRAPMOD_Msk                       /*!<Wrapped burst mode support */
#define FMC_BCR1_WAITCFG_Pos        (11U)                                      
#define FMC_BCR1_WAITCFG_Msk        (0x1U << FMC_BCR1_WAITCFG_Pos)             /*!< 0x00000800 */
#define FMC_BCR1_WAITCFG            FMC_BCR1_WAITCFG_Msk                       /*!<Wait timing configuration  */
#define FMC_BCR1_WREN_Pos           (12U)                                      
#define FMC_BCR1_WREN_Msk           (0x1U << FMC_BCR1_WREN_Pos)                /*!< 0x00001000 */
#define FMC_BCR1_WREN               FMC_BCR1_WREN_Msk                          /*!<Write enable bit           */
#define FMC_BCR1_WAITEN_Pos         (13U)                                      
#define FMC_BCR1_WAITEN_Msk         (0x1U << FMC_BCR1_WAITEN_Pos)              /*!< 0x00002000 */
#define FMC_BCR1_WAITEN             FMC_BCR1_WAITEN_Msk                        /*!<Wait enable bit            */
#define FMC_BCR1_EXTMOD_Pos         (14U)                                      
#define FMC_BCR1_EXTMOD_Msk         (0x1U << FMC_BCR1_EXTMOD_Pos)              /*!< 0x00004000 */
#define FMC_BCR1_EXTMOD             FMC_BCR1_EXTMOD_Msk                        /*!<Extended mode enable       */
#define FMC_BCR1_ASYNCWAIT_Pos      (15U)                                      
#define FMC_BCR1_ASYNCWAIT_Msk      (0x1U << FMC_BCR1_ASYNCWAIT_Pos)           /*!< 0x00008000 */
#define FMC_BCR1_ASYNCWAIT          FMC_BCR1_ASYNCWAIT_Msk                     /*!<Asynchronous wait          */
#define FMC_BCR1_CPSIZE_Pos         (16U)                                      
#define FMC_BCR1_CPSIZE_Msk         (0x7U << FMC_BCR1_CPSIZE_Pos)              /*!< 0x00070000 */
#define FMC_BCR1_CPSIZE             FMC_BCR1_CPSIZE_Msk                        /*!<CRAM page size             */
#define FMC_BCR1_CPSIZE_0           (0x1U << FMC_BCR1_CPSIZE_Pos)              /*!< 0x00010000 */
#define FMC_BCR1_CPSIZE_1           (0x2U << FMC_BCR1_CPSIZE_Pos)              /*!< 0x00020000 */
#define FMC_BCR1_CPSIZE_2           (0x4U << FMC_BCR1_CPSIZE_Pos)              /*!< 0x00040000 */
#define FMC_BCR1_CBURSTRW_Pos       (19U)                                      
#define FMC_BCR1_CBURSTRW_Msk       (0x1U << FMC_BCR1_CBURSTRW_Pos)            /*!< 0x00080000 */
#define FMC_BCR1_CBURSTRW           FMC_BCR1_CBURSTRW_Msk                      /*!<Write burst enable         */
#define FMC_BCR1_CCLKEN_Pos         (20U)                                      
#define FMC_BCR1_CCLKEN_Msk         (0x1U << FMC_BCR1_CCLKEN_Pos)              /*!< 0x00100000 */
#define FMC_BCR1_CCLKEN             FMC_BCR1_CCLKEN_Msk                        /*!<Continous clock enable     */

/******************  Bit definition for FMC_BCR2 register  *******************/
#define FMC_BCR2_MBKEN_Pos          (0U)                                       
#define FMC_BCR2_MBKEN_Msk          (0x1U << FMC_BCR2_MBKEN_Pos)               /*!< 0x00000001 */
#define FMC_BCR2_MBKEN              FMC_BCR2_MBKEN_Msk                         /*!<Memory bank enable bit                 */
#define FMC_BCR2_MUXEN_Pos          (1U)                                       
#define FMC_BCR2_MUXEN_Msk          (0x1U << FMC_BCR2_MUXEN_Pos)               /*!< 0x00000002 */
#define FMC_BCR2_MUXEN              FMC_BCR2_MUXEN_Msk                         /*!<Address/data multiplexing enable bit   */

#define FMC_BCR2_MTYP_Pos           (2U)                                       
#define FMC_BCR2_MTYP_Msk           (0x3U << FMC_BCR2_MTYP_Pos)                /*!< 0x0000000C */
#define FMC_BCR2_MTYP               FMC_BCR2_MTYP_Msk                          /*!<MTYP[1:0] bits (Memory type)           */
#define FMC_BCR2_MTYP_0             (0x1U << FMC_BCR2_MTYP_Pos)                /*!< 0x00000004 */
#define FMC_BCR2_MTYP_1             (0x2U << FMC_BCR2_MTYP_Pos)                /*!< 0x00000008 */

#define FMC_BCR2_MWID_Pos           (4U)                                       
#define FMC_BCR2_MWID_Msk           (0x3U << FMC_BCR2_MWID_Pos)                /*!< 0x00000030 */
#define FMC_BCR2_MWID               FMC_BCR2_MWID_Msk                          /*!<MWID[1:0] bits (Memory data bus width) */
#define FMC_BCR2_MWID_0             (0x1U << FMC_BCR2_MWID_Pos)                /*!< 0x00000010 */
#define FMC_BCR2_MWID_1             (0x2U << FMC_BCR2_MWID_Pos)                /*!< 0x00000020 */

#define FMC_BCR2_FACCEN_Pos         (6U)                                       
#define FMC_BCR2_FACCEN_Msk         (0x1U << FMC_BCR2_FACCEN_Pos)              /*!< 0x00000040 */
#define FMC_BCR2_FACCEN             FMC_BCR2_FACCEN_Msk                        /*!<Flash access enable        */
#define FMC_BCR2_BURSTEN_Pos        (8U)                                       
#define FMC_BCR2_BURSTEN_Msk        (0x1U << FMC_BCR2_BURSTEN_Pos)             /*!< 0x00000100 */
#define FMC_BCR2_BURSTEN            FMC_BCR2_BURSTEN_Msk                       /*!<Burst enable bit           */
#define FMC_BCR2_WAITPOL_Pos        (9U)                                       
#define FMC_BCR2_WAITPOL_Msk        (0x1U << FMC_BCR2_WAITPOL_Pos)             /*!< 0x00000200 */
#define FMC_BCR2_WAITPOL            FMC_BCR2_WAITPOL_Msk                       /*!<Wait signal polarity bit   */
#define FMC_BCR2_WRAPMOD_Pos        (10U)                                      
#define FMC_BCR2_WRAPMOD_Msk        (0x1U << FMC_BCR2_WRAPMOD_Pos)             /*!< 0x00000400 */
#define FMC_BCR2_WRAPMOD            FMC_BCR2_WRAPMOD_Msk                       /*!<Wrapped burst mode support */
#define FMC_BCR2_WAITCFG_Pos        (11U)                                      
#define FMC_BCR2_WAITCFG_Msk        (0x1U << FMC_BCR2_WAITCFG_Pos)             /*!< 0x00000800 */
#define FMC_BCR2_WAITCFG            FMC_BCR2_WAITCFG_Msk                       /*!<Wait timing configuration  */
#define FMC_BCR2_WREN_Pos           (12U)                                      
#define FMC_BCR2_WREN_Msk           (0x1U << FMC_BCR2_WREN_Pos)                /*!< 0x00001000 */
#define FMC_BCR2_WREN               FMC_BCR2_WREN_Msk                          /*!<Write enable bit           */
#define FMC_BCR2_WAITEN_Pos         (13U)                                      
#define FMC_BCR2_WAITEN_Msk         (0x1U << FMC_BCR2_WAITEN_Pos)              /*!< 0x00002000 */
#define FMC_BCR2_WAITEN             FMC_BCR2_WAITEN_Msk                        /*!<Wait enable bit            */
#define FMC_BCR2_EXTMOD_Pos         (14U)                                      
#define FMC_BCR2_EXTMOD_Msk         (0x1U << FMC_BCR2_EXTMOD_Pos)              /*!< 0x00004000 */
#define FMC_BCR2_EXTMOD             FMC_BCR2_EXTMOD_Msk                        /*!<Extended mode enable       */
#define FMC_BCR2_ASYNCWAIT_Pos      (15U)                                      
#define FMC_BCR2_ASYNCWAIT_Msk      (0x1U << FMC_BCR2_ASYNCWAIT_Pos)           /*!< 0x00008000 */
#define FMC_BCR2_ASYNCWAIT          FMC_BCR2_ASYNCWAIT_Msk                     /*!<Asynchronous wait          */
#define FMC_BCR2_CPSIZE_Pos         (16U)                                      
#define FMC_BCR2_CPSIZE_Msk         (0x7U << FMC_BCR2_CPSIZE_Pos)              /*!< 0x00070000 */
#define FMC_BCR2_CPSIZE             FMC_BCR2_CPSIZE_Msk                        /*!<CRAM page size */
#define FMC_BCR2_CPSIZE_0           (0x1U << FMC_BCR2_CPSIZE_Pos)              /*!< 0x00010000 */
#define FMC_BCR2_CPSIZE_1           (0x2U << FMC_BCR2_CPSIZE_Pos)              /*!< 0x00020000 */
#define FMC_BCR2_CPSIZE_2           (0x4U << FMC_BCR2_CPSIZE_Pos)              /*!< 0x00040000 */
#define FMC_BCR2_CBURSTRW_Pos       (19U)                                      
#define FMC_BCR2_CBURSTRW_Msk       (0x1U << FMC_BCR2_CBURSTRW_Pos)            /*!< 0x00080000 */
#define FMC_BCR2_CBURSTRW           FMC_BCR2_CBURSTRW_Msk                      /*!<Write burst enable         */

/******************  Bit definition for FMC_BCR3 register  *******************/
#define FMC_BCR3_MBKEN_Pos          (0U)                                       
#define FMC_BCR3_MBKEN_Msk          (0x1U << FMC_BCR3_MBKEN_Pos)               /*!< 0x00000001 */
#define FMC_BCR3_MBKEN              FMC_BCR3_MBKEN_Msk                         /*!<Memory bank enable bit                 */
#define FMC_BCR3_MUXEN_Pos          (1U)                                       
#define FMC_BCR3_MUXEN_Msk          (0x1U << FMC_BCR3_MUXEN_Pos)               /*!< 0x00000002 */
#define FMC_BCR3_MUXEN              FMC_BCR3_MUXEN_Msk                         /*!<Address/data multiplexing enable bit   */

#define FMC_BCR3_MTYP_Pos           (2U)                                       
#define FMC_BCR3_MTYP_Msk           (0x3U << FMC_BCR3_MTYP_Pos)                /*!< 0x0000000C */
#define FMC_BCR3_MTYP               FMC_BCR3_MTYP_Msk                          /*!<MTYP[1:0] bits (Memory type)           */
#define FMC_BCR3_MTYP_0             (0x1U << FMC_BCR3_MTYP_Pos)                /*!< 0x00000004 */
#define FMC_BCR3_MTYP_1             (0x2U << FMC_BCR3_MTYP_Pos)                /*!< 0x00000008 */

#define FMC_BCR3_MWID_Pos           (4U)                                       
#define FMC_BCR3_MWID_Msk           (0x3U << FMC_BCR3_MWID_Pos)                /*!< 0x00000030 */
#define FMC_BCR3_MWID               FMC_BCR3_MWID_Msk                          /*!<MWID[1:0] bits (Memory data bus width) */
#define FMC_BCR3_MWID_0             (0x1U << FMC_BCR3_MWID_Pos)                /*!< 0x00000010 */
#define FMC_BCR3_MWID_1             (0x2U << FMC_BCR3_MWID_Pos)                /*!< 0x00000020 */

#define FMC_BCR3_FACCEN_Pos         (6U)                                       
#define FMC_BCR3_FACCEN_Msk         (0x1U << FMC_BCR3_FACCEN_Pos)              /*!< 0x00000040 */
#define FMC_BCR3_FACCEN             FMC_BCR3_FACCEN_Msk                        /*!<Flash access enable        */
#define FMC_BCR3_BURSTEN_Pos        (8U)                                       
#define FMC_BCR3_BURSTEN_Msk        (0x1U << FMC_BCR3_BURSTEN_Pos)             /*!< 0x00000100 */
#define FMC_BCR3_BURSTEN            FMC_BCR3_BURSTEN_Msk                       /*!<Burst enable bit           */
#define FMC_BCR3_WAITPOL_Pos        (9U)                                       
#define FMC_BCR3_WAITPOL_Msk        (0x1U << FMC_BCR3_WAITPOL_Pos)             /*!< 0x00000200 */
#define FMC_BCR3_WAITPOL            FMC_BCR3_WAITPOL_Msk                       /*!<Wait signal polarity bit   */
#define FMC_BCR3_WRAPMOD_Pos        (10U)                                      
#define FMC_BCR3_WRAPMOD_Msk        (0x1U << FMC_BCR3_WRAPMOD_Pos)             /*!< 0x00000400 */
#define FMC_BCR3_WRAPMOD            FMC_BCR3_WRAPMOD_Msk                       /*!<Wrapped burst mode support */
#define FMC_BCR3_WAITCFG_Pos        (11U)                                      
#define FMC_BCR3_WAITCFG_Msk        (0x1U << FMC_BCR3_WAITCFG_Pos)             /*!< 0x00000800 */
#define FMC_BCR3_WAITCFG            FMC_BCR3_WAITCFG_Msk                       /*!<Wait timing configuration  */
#define FMC_BCR3_WREN_Pos           (12U)                                      
#define FMC_BCR3_WREN_Msk           (0x1U << FMC_BCR3_WREN_Pos)                /*!< 0x00001000 */
#define FMC_BCR3_WREN               FMC_BCR3_WREN_Msk                          /*!<Write enable bit           */
#define FMC_BCR3_WAITEN_Pos         (13U)                                      
#define FMC_BCR3_WAITEN_Msk         (0x1U << FMC_BCR3_WAITEN_Pos)              /*!< 0x00002000 */
#define FMC_BCR3_WAITEN             FMC_BCR3_WAITEN_Msk                        /*!<Wait enable bit            */
#define FMC_BCR3_EXTMOD_Pos         (14U)                                      
#define FMC_BCR3_EXTMOD_Msk         (0x1U << FMC_BCR3_EXTMOD_Pos)              /*!< 0x00004000 */
#define FMC_BCR3_EXTMOD             FMC_BCR3_EXTMOD_Msk                        /*!<Extended mode enable       */
#define FMC_BCR3_ASYNCWAIT_Pos      (15U)                                      
#define FMC_BCR3_ASYNCWAIT_Msk      (0x1U << FMC_BCR3_ASYNCWAIT_Pos)           /*!< 0x00008000 */
#define FMC_BCR3_ASYNCWAIT          FMC_BCR3_ASYNCWAIT_Msk                     /*!<Asynchronous wait          */
#define FMC_BCR3_CPSIZE_Pos         (16U)                                      
#define FMC_BCR3_CPSIZE_Msk         (0x7U << FMC_BCR3_CPSIZE_Pos)              /*!< 0x00070000 */
#define FMC_BCR3_CPSIZE             FMC_BCR3_CPSIZE_Msk                        /*!<CRAM page size */
#define FMC_BCR3_CPSIZE_0           (0x1U << FMC_BCR3_CPSIZE_Pos)              /*!< 0x00010000 */
#define FMC_BCR3_CPSIZE_1           (0x2U << FMC_BCR3_CPSIZE_Pos)              /*!< 0x00020000 */
#define FMC_BCR3_CPSIZE_2           (0x4U << FMC_BCR3_CPSIZE_Pos)              /*!< 0x00040000 */
#define FMC_BCR3_CBURSTRW_Pos       (19U)                                      
#define FMC_BCR3_CBURSTRW_Msk       (0x1U << FMC_BCR3_CBURSTRW_Pos)            /*!< 0x00080000 */
#define FMC_BCR3_CBURSTRW           FMC_BCR3_CBURSTRW_Msk                      /*!<Write burst enable         */

/******************  Bit definition for FMC_BCR4 register  *******************/
#define FMC_BCR4_MBKEN_Pos          (0U)                                       
#define FMC_BCR4_MBKEN_Msk          (0x1U << FMC_BCR4_MBKEN_Pos)               /*!< 0x00000001 */
#define FMC_BCR4_MBKEN              FMC_BCR4_MBKEN_Msk                         /*!<Memory bank enable bit                 */
#define FMC_BCR4_MUXEN_Pos          (1U)                                       
#define FMC_BCR4_MUXEN_Msk          (0x1U << FMC_BCR4_MUXEN_Pos)               /*!< 0x00000002 */
#define FMC_BCR4_MUXEN              FMC_BCR4_MUXEN_Msk                         /*!<Address/data multiplexing enable bit   */

#define FMC_BCR4_MTYP_Pos           (2U)                                       
#define FMC_BCR4_MTYP_Msk           (0x3U << FMC_BCR4_MTYP_Pos)                /*!< 0x0000000C */
#define FMC_BCR4_MTYP               FMC_BCR4_MTYP_Msk                          /*!<MTYP[1:0] bits (Memory type)           */
#define FMC_BCR4_MTYP_0             (0x1U << FMC_BCR4_MTYP_Pos)                /*!< 0x00000004 */
#define FMC_BCR4_MTYP_1             (0x2U << FMC_BCR4_MTYP_Pos)                /*!< 0x00000008 */

#define FMC_BCR4_MWID_Pos           (4U)                                       
#define FMC_BCR4_MWID_Msk           (0x3U << FMC_BCR4_MWID_Pos)                /*!< 0x00000030 */
#define FMC_BCR4_MWID               FMC_BCR4_MWID_Msk                          /*!<MWID[1:0] bits (Memory data bus width) */
#define FMC_BCR4_MWID_0             (0x1U << FMC_BCR4_MWID_Pos)                /*!< 0x00000010 */
#define FMC_BCR4_MWID_1             (0x2U << FMC_BCR4_MWID_Pos)                /*!< 0x00000020 */

#define FMC_BCR4_FACCEN_Pos         (6U)                                       
#define FMC_BCR4_FACCEN_Msk         (0x1U << FMC_BCR4_FACCEN_Pos)              /*!< 0x00000040 */
#define FMC_BCR4_FACCEN             FMC_BCR4_FACCEN_Msk                        /*!<Flash access enable        */
#define FMC_BCR4_BURSTEN_Pos        (8U)                                       
#define FMC_BCR4_BURSTEN_Msk        (0x1U << FMC_BCR4_BURSTEN_Pos)             /*!< 0x00000100 */
#define FMC_BCR4_BURSTEN            FMC_BCR4_BURSTEN_Msk                       /*!<Burst enable bit           */
#define FMC_BCR4_WAITPOL_Pos        (9U)                                       
#define FMC_BCR4_WAITPOL_Msk        (0x1U << FMC_BCR4_WAITPOL_Pos)             /*!< 0x00000200 */
#define FMC_BCR4_WAITPOL            FMC_BCR4_WAITPOL_Msk                       /*!<Wait signal polarity bit   */
#define FMC_BCR4_WRAPMOD_Pos        (10U)                                      
#define FMC_BCR4_WRAPMOD_Msk        (0x1U << FMC_BCR4_WRAPMOD_Pos)             /*!< 0x00000400 */
#define FMC_BCR4_WRAPMOD            FMC_BCR4_WRAPMOD_Msk                       /*!<Wrapped burst mode support */
#define FMC_BCR4_WAITCFG_Pos        (11U)                                      
#define FMC_BCR4_WAITCFG_Msk        (0x1U << FMC_BCR4_WAITCFG_Pos)             /*!< 0x00000800 */
#define FMC_BCR4_WAITCFG            FMC_BCR4_WAITCFG_Msk                       /*!<Wait timing configuration  */
#define FMC_BCR4_WREN_Pos           (12U)                                      
#define FMC_BCR4_WREN_Msk           (0x1U << FMC_BCR4_WREN_Pos)                /*!< 0x00001000 */
#define FMC_BCR4_WREN               FMC_BCR4_WREN_Msk                          /*!<Write enable bit           */
#define FMC_BCR4_WAITEN_Pos         (13U)                                      
#define FMC_BCR4_WAITEN_Msk         (0x1U << FMC_BCR4_WAITEN_Pos)              /*!< 0x00002000 */
#define FMC_BCR4_WAITEN             FMC_BCR4_WAITEN_Msk                        /*!<Wait enable bit            */
#define FMC_BCR4_EXTMOD_Pos         (14U)                                      
#define FMC_BCR4_EXTMOD_Msk         (0x1U << FMC_BCR4_EXTMOD_Pos)              /*!< 0x00004000 */
#define FMC_BCR4_EXTMOD             FMC_BCR4_EXTMOD_Msk                        /*!<Extended mode enable       */
#define FMC_BCR4_ASYNCWAIT_Pos      (15U)                                      
#define FMC_BCR4_ASYNCWAIT_Msk      (0x1U << FMC_BCR4_ASYNCWAIT_Pos)           /*!< 0x00008000 */
#define FMC_BCR4_ASYNCWAIT          FMC_BCR4_ASYNCWAIT_Msk                     /*!<Asynchronous wait          */
#define FMC_BCR4_CPSIZE_Pos         (16U)                                      
#define FMC_BCR4_CPSIZE_Msk         (0x7U << FMC_BCR4_CPSIZE_Pos)              /*!< 0x00070000 */
#define FMC_BCR4_CPSIZE             FMC_BCR4_CPSIZE_Msk                        /*!<CRAM page size */
#define FMC_BCR4_CPSIZE_0           (0x1U << FMC_BCR4_CPSIZE_Pos)              /*!< 0x00010000 */
#define FMC_BCR4_CPSIZE_1           (0x2U << FMC_BCR4_CPSIZE_Pos)              /*!< 0x00020000 */
#define FMC_BCR4_CPSIZE_2           (0x4U << FMC_BCR4_CPSIZE_Pos)              /*!< 0x00040000 */
#define FMC_BCR4_CBURSTRW_Pos       (19U)                                      
#define FMC_BCR4_CBURSTRW_Msk       (0x1U << FMC_BCR4_CBURSTRW_Pos)            /*!< 0x00080000 */
#define FMC_BCR4_CBURSTRW           FMC_BCR4_CBURSTRW_Msk                      /*!<Write burst enable         */

/******************  Bit definition for FMC_BTR1 register  ******************/
#define FMC_BTR1_ADDSET_Pos         (0U)                                       
#define FMC_BTR1_ADDSET_Msk         (0xFU << FMC_BTR1_ADDSET_Pos)              /*!< 0x0000000F */
#define FMC_BTR1_ADDSET             FMC_BTR1_ADDSET_Msk                        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define FMC_BTR1_ADDSET_0           (0x1U << FMC_BTR1_ADDSET_Pos)              /*!< 0x00000001 */
#define FMC_BTR1_ADDSET_1           (0x2U << FMC_BTR1_ADDSET_Pos)              /*!< 0x00000002 */
#define FMC_BTR1_ADDSET_2           (0x4U << FMC_BTR1_ADDSET_Pos)              /*!< 0x00000004 */
#define FMC_BTR1_ADDSET_3           (0x8U << FMC_BTR1_ADDSET_Pos)              /*!< 0x00000008 */

#define FMC_BTR1_ADDHLD_Pos         (4U)                                       
#define FMC_BTR1_ADDHLD_Msk         (0xFU << FMC_BTR1_ADDHLD_Pos)              /*!< 0x000000F0 */
#define FMC_BTR1_ADDHLD             FMC_BTR1_ADDHLD_Msk                        /*!<ADDHLD[3:0] bits (Address-hold phase duration)  */
#define FMC_BTR1_ADDHLD_0           (0x1U << FMC_BTR1_ADDHLD_Pos)              /*!< 0x00000010 */
#define FMC_BTR1_ADDHLD_1           (0x2U << FMC_BTR1_ADDHLD_Pos)              /*!< 0x00000020 */
#define FMC_BTR1_ADDHLD_2           (0x4U << FMC_BTR1_ADDHLD_Pos)              /*!< 0x00000040 */
#define FMC_BTR1_ADDHLD_3           (0x8U << FMC_BTR1_ADDHLD_Pos)              /*!< 0x00000080 */

#define FMC_BTR1_DATAST_Pos         (8U)                                       
#define FMC_BTR1_DATAST_Msk         (0xFFU << FMC_BTR1_DATAST_Pos)             /*!< 0x0000FF00 */
#define FMC_BTR1_DATAST             FMC_BTR1_DATAST_Msk                        /*!<DATAST [3:0] bits (Data-phase duration) */
#define FMC_BTR1_DATAST_0           (0x01U << FMC_BTR1_DATAST_Pos)             /*!< 0x00000100 */
#define FMC_BTR1_DATAST_1           (0x02U << FMC_BTR1_DATAST_Pos)             /*!< 0x00000200 */
#define FMC_BTR1_DATAST_2           (0x04U << FMC_BTR1_DATAST_Pos)             /*!< 0x00000400 */
#define FMC_BTR1_DATAST_3           (0x08U << FMC_BTR1_DATAST_Pos)             /*!< 0x00000800 */
#define FMC_BTR1_DATAST_4           (0x10U << FMC_BTR1_DATAST_Pos)             /*!< 0x00001000 */
#define FMC_BTR1_DATAST_5           (0x20U << FMC_BTR1_DATAST_Pos)             /*!< 0x00002000 */
#define FMC_BTR1_DATAST_6           (0x40U << FMC_BTR1_DATAST_Pos)             /*!< 0x00004000 */
#define FMC_BTR1_DATAST_7           (0x80U << FMC_BTR1_DATAST_Pos)             /*!< 0x00008000 */

#define FMC_BTR1_BUSTURN_Pos        (16U)                                      
#define FMC_BTR1_BUSTURN_Msk        (0xFU << FMC_BTR1_BUSTURN_Pos)             /*!< 0x000F0000 */
#define FMC_BTR1_BUSTURN            FMC_BTR1_BUSTURN_Msk                       /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define FMC_BTR1_BUSTURN_0          (0x1U << FMC_BTR1_BUSTURN_Pos)             /*!< 0x00010000 */
#define FMC_BTR1_BUSTURN_1          (0x2U << FMC_BTR1_BUSTURN_Pos)             /*!< 0x00020000 */
#define FMC_BTR1_BUSTURN_2          (0x4U << FMC_BTR1_BUSTURN_Pos)             /*!< 0x00040000 */
#define FMC_BTR1_BUSTURN_3          (0x8U << FMC_BTR1_BUSTURN_Pos)             /*!< 0x00080000 */

#define FMC_BTR1_CLKDIV_Pos         (20U)                                      
#define FMC_BTR1_CLKDIV_Msk         (0xFU << FMC_BTR1_CLKDIV_Pos)              /*!< 0x00F00000 */
#define FMC_BTR1_CLKDIV             FMC_BTR1_CLKDIV_Msk                        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define FMC_BTR1_CLKDIV_0           (0x1U << FMC_BTR1_CLKDIV_Pos)              /*!< 0x00100000 */
#define FMC_BTR1_CLKDIV_1           (0x2U << FMC_BTR1_CLKDIV_Pos)              /*!< 0x00200000 */
#define FMC_BTR1_CLKDIV_2           (0x4U << FMC_BTR1_CLKDIV_Pos)              /*!< 0x00400000 */
#define FMC_BTR1_CLKDIV_3           (0x8U << FMC_BTR1_CLKDIV_Pos)              /*!< 0x00800000 */

#define FMC_BTR1_DATLAT_Pos         (24U)                                      
#define FMC_BTR1_DATLAT_Msk         (0xFU << FMC_BTR1_DATLAT_Pos)              /*!< 0x0F000000 */
#define FMC_BTR1_DATLAT             FMC_BTR1_DATLAT_Msk                        /*!<DATLA[3:0] bits (Data latency) */
#define FMC_BTR1_DATLAT_0           (0x1U << FMC_BTR1_DATLAT_Pos)              /*!< 0x01000000 */
#define FMC_BTR1_DATLAT_1           (0x2U << FMC_BTR1_DATLAT_Pos)              /*!< 0x02000000 */
#define FMC_BTR1_DATLAT_2           (0x4U << FMC_BTR1_DATLAT_Pos)              /*!< 0x04000000 */
#define FMC_BTR1_DATLAT_3           (0x8U << FMC_BTR1_DATLAT_Pos)              /*!< 0x08000000 */

#define FMC_BTR1_ACCMOD_Pos         (28U)                                      
#define FMC_BTR1_ACCMOD_Msk         (0x3U << FMC_BTR1_ACCMOD_Pos)              /*!< 0x30000000 */
#define FMC_BTR1_ACCMOD             FMC_BTR1_ACCMOD_Msk                        /*!<ACCMOD[1:0] bits (Access mode) */
#define FMC_BTR1_ACCMOD_0           (0x1U << FMC_BTR1_ACCMOD_Pos)              /*!< 0x10000000 */
#define FMC_BTR1_ACCMOD_1           (0x2U << FMC_BTR1_ACCMOD_Pos)              /*!< 0x20000000 */

/******************  Bit definition for FMC_BTR2 register  *******************/
#define FMC_BTR2_ADDSET_Pos         (0U)                                       
#define FMC_BTR2_ADDSET_Msk         (0xFU << FMC_BTR2_ADDSET_Pos)              /*!< 0x0000000F */
#define FMC_BTR2_ADDSET             FMC_BTR2_ADDSET_Msk                        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define FMC_BTR2_ADDSET_0           (0x1U << FMC_BTR2_ADDSET_Pos)              /*!< 0x00000001 */
#define FMC_BTR2_ADDSET_1           (0x2U << FMC_BTR2_ADDSET_Pos)              /*!< 0x00000002 */
#define FMC_BTR2_ADDSET_2           (0x4U << FMC_BTR2_ADDSET_Pos)              /*!< 0x00000004 */
#define FMC_BTR2_ADDSET_3           (0x8U << FMC_BTR2_ADDSET_Pos)              /*!< 0x00000008 */

#define FMC_BTR2_ADDHLD_Pos         (4U)                                       
#define FMC_BTR2_ADDHLD_Msk         (0xFU << FMC_BTR2_ADDHLD_Pos)              /*!< 0x000000F0 */
#define FMC_BTR2_ADDHLD             FMC_BTR2_ADDHLD_Msk                        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define FMC_BTR2_ADDHLD_0           (0x1U << FMC_BTR2_ADDHLD_Pos)              /*!< 0x00000010 */
#define FMC_BTR2_ADDHLD_1           (0x2U << FMC_BTR2_ADDHLD_Pos)              /*!< 0x00000020 */
#define FMC_BTR2_ADDHLD_2           (0x4U << FMC_BTR2_ADDHLD_Pos)              /*!< 0x00000040 */
#define FMC_BTR2_ADDHLD_3           (0x8U << FMC_BTR2_ADDHLD_Pos)              /*!< 0x00000080 */

#define FMC_BTR2_DATAST_Pos         (8U)                                       
#define FMC_BTR2_DATAST_Msk         (0xFFU << FMC_BTR2_DATAST_Pos)             /*!< 0x0000FF00 */
#define FMC_BTR2_DATAST             FMC_BTR2_DATAST_Msk                        /*!<DATAST [3:0] bits (Data-phase duration) */
#define FMC_BTR2_DATAST_0           (0x01U << FMC_BTR2_DATAST_Pos)             /*!< 0x00000100 */
#define FMC_BTR2_DATAST_1           (0x02U << FMC_BTR2_DATAST_Pos)             /*!< 0x00000200 */
#define FMC_BTR2_DATAST_2           (0x04U << FMC_BTR2_DATAST_Pos)             /*!< 0x00000400 */
#define FMC_BTR2_DATAST_3           (0x08U << FMC_BTR2_DATAST_Pos)             /*!< 0x00000800 */
#define FMC_BTR2_DATAST_4           (0x10U << FMC_BTR2_DATAST_Pos)             /*!< 0x00001000 */
#define FMC_BTR2_DATAST_5           (0x20U << FMC_BTR2_DATAST_Pos)             /*!< 0x00002000 */
#define FMC_BTR2_DATAST_6           (0x40U << FMC_BTR2_DATAST_Pos)             /*!< 0x00004000 */
#define FMC_BTR2_DATAST_7           (0x80U << FMC_BTR2_DATAST_Pos)             /*!< 0x00008000 */

#define FMC_BTR2_BUSTURN_Pos        (16U)                                      
#define FMC_BTR2_BUSTURN_Msk        (0xFU << FMC_BTR2_BUSTURN_Pos)             /*!< 0x000F0000 */
#define FMC_BTR2_BUSTURN            FMC_BTR2_BUSTURN_Msk                       /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define FMC_BTR2_BUSTURN_0          (0x1U << FMC_BTR2_BUSTURN_Pos)             /*!< 0x00010000 */
#define FMC_BTR2_BUSTURN_1          (0x2U << FMC_BTR2_BUSTURN_Pos)             /*!< 0x00020000 */
#define FMC_BTR2_BUSTURN_2          (0x4U << FMC_BTR2_BUSTURN_Pos)             /*!< 0x00040000 */
#define FMC_BTR2_BUSTURN_3          (0x8U << FMC_BTR2_BUSTURN_Pos)             /*!< 0x00080000 */

#define FMC_BTR2_CLKDIV_Pos         (20U)                                      
#define FMC_BTR2_CLKDIV_Msk         (0xFU << FMC_BTR2_CLKDIV_Pos)              /*!< 0x00F00000 */
#define FMC_BTR2_CLKDIV             FMC_BTR2_CLKDIV_Msk                        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define FMC_BTR2_CLKDIV_0           (0x1U << FMC_BTR2_CLKDIV_Pos)              /*!< 0x00100000 */
#define FMC_BTR2_CLKDIV_1           (0x2U << FMC_BTR2_CLKDIV_Pos)              /*!< 0x00200000 */
#define FMC_BTR2_CLKDIV_2           (0x4U << FMC_BTR2_CLKDIV_Pos)              /*!< 0x00400000 */
#define FMC_BTR2_CLKDIV_3           (0x8U << FMC_BTR2_CLKDIV_Pos)              /*!< 0x00800000 */

#define FMC_BTR2_DATLAT_Pos         (24U)                                      
#define FMC_BTR2_DATLAT_Msk         (0xFU << FMC_BTR2_DATLAT_Pos)              /*!< 0x0F000000 */
#define FMC_BTR2_DATLAT             FMC_BTR2_DATLAT_Msk                        /*!<DATLA[3:0] bits (Data latency) */
#define FMC_BTR2_DATLAT_0           (0x1U << FMC_BTR2_DATLAT_Pos)              /*!< 0x01000000 */
#define FMC_BTR2_DATLAT_1           (0x2U << FMC_BTR2_DATLAT_Pos)              /*!< 0x02000000 */
#define FMC_BTR2_DATLAT_2           (0x4U << FMC_BTR2_DATLAT_Pos)              /*!< 0x04000000 */
#define FMC_BTR2_DATLAT_3           (0x8U << FMC_BTR2_DATLAT_Pos)              /*!< 0x08000000 */

#define FMC_BTR2_ACCMOD_Pos         (28U)                                      
#define FMC_BTR2_ACCMOD_Msk         (0x3U << FMC_BTR2_ACCMOD_Pos)              /*!< 0x30000000 */
#define FMC_BTR2_ACCMOD             FMC_BTR2_ACCMOD_Msk                        /*!<ACCMOD[1:0] bits (Access mode) */
#define FMC_BTR2_ACCMOD_0           (0x1U << FMC_BTR2_ACCMOD_Pos)              /*!< 0x10000000 */
#define FMC_BTR2_ACCMOD_1           (0x2U << FMC_BTR2_ACCMOD_Pos)              /*!< 0x20000000 */

/*******************  Bit definition for FMC_BTR3 register  *******************/
#define FMC_BTR3_ADDSET_Pos         (0U)                                       
#define FMC_BTR3_ADDSET_Msk         (0xFU << FMC_BTR3_ADDSET_Pos)              /*!< 0x0000000F */
#define FMC_BTR3_ADDSET             FMC_BTR3_ADDSET_Msk                        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define FMC_BTR3_ADDSET_0           (0x1U << FMC_BTR3_ADDSET_Pos)              /*!< 0x00000001 */
#define FMC_BTR3_ADDSET_1           (0x2U << FMC_BTR3_ADDSET_Pos)              /*!< 0x00000002 */
#define FMC_BTR3_ADDSET_2           (0x4U << FMC_BTR3_ADDSET_Pos)              /*!< 0x00000004 */
#define FMC_BTR3_ADDSET_3           (0x8U << FMC_BTR3_ADDSET_Pos)              /*!< 0x00000008 */

#define FMC_BTR3_ADDHLD_Pos         (4U)                                       
#define FMC_BTR3_ADDHLD_Msk         (0xFU << FMC_BTR3_ADDHLD_Pos)              /*!< 0x000000F0 */
#define FMC_BTR3_ADDHLD             FMC_BTR3_ADDHLD_Msk                        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define FMC_BTR3_ADDHLD_0           (0x1U << FMC_BTR3_ADDHLD_Pos)              /*!< 0x00000010 */
#define FMC_BTR3_ADDHLD_1           (0x2U << FMC_BTR3_ADDHLD_Pos)              /*!< 0x00000020 */
#define FMC_BTR3_ADDHLD_2           (0x4U << FMC_BTR3_ADDHLD_Pos)              /*!< 0x00000040 */
#define FMC_BTR3_ADDHLD_3           (0x8U << FMC_BTR3_ADDHLD_Pos)              /*!< 0x00000080 */

#define FMC_BTR3_DATAST_Pos         (8U)                                       
#define FMC_BTR3_DATAST_Msk         (0xFFU << FMC_BTR3_DATAST_Pos)             /*!< 0x0000FF00 */
#define FMC_BTR3_DATAST             FMC_BTR3_DATAST_Msk                        /*!<DATAST [3:0] bits (Data-phase duration) */
#define FMC_BTR3_DATAST_0           (0x01U << FMC_BTR3_DATAST_Pos)             /*!< 0x00000100 */
#define FMC_BTR3_DATAST_1           (0x02U << FMC_BTR3_DATAST_Pos)             /*!< 0x00000200 */
#define FMC_BTR3_DATAST_2           (0x04U << FMC_BTR3_DATAST_Pos)             /*!< 0x00000400 */
#define FMC_BTR3_DATAST_3           (0x08U << FMC_BTR3_DATAST_Pos)             /*!< 0x00000800 */
#define FMC_BTR3_DATAST_4           (0x10U << FMC_BTR3_DATAST_Pos)             /*!< 0x00001000 */
#define FMC_BTR3_DATAST_5           (0x20U << FMC_BTR3_DATAST_Pos)             /*!< 0x00002000 */
#define FMC_BTR3_DATAST_6           (0x40U << FMC_BTR3_DATAST_Pos)             /*!< 0x00004000 */
#define FMC_BTR3_DATAST_7           (0x80U << FMC_BTR3_DATAST_Pos)             /*!< 0x00008000 */

#define FMC_BTR3_BUSTURN_Pos        (16U)                                      
#define FMC_BTR3_BUSTURN_Msk        (0xFU << FMC_BTR3_BUSTURN_Pos)             /*!< 0x000F0000 */
#define FMC_BTR3_BUSTURN            FMC_BTR3_BUSTURN_Msk                       /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define FMC_BTR3_BUSTURN_0          (0x1U << FMC_BTR3_BUSTURN_Pos)             /*!< 0x00010000 */
#define FMC_BTR3_BUSTURN_1          (0x2U << FMC_BTR3_BUSTURN_Pos)             /*!< 0x00020000 */
#define FMC_BTR3_BUSTURN_2          (0x4U << FMC_BTR3_BUSTURN_Pos)             /*!< 0x00040000 */
#define FMC_BTR3_BUSTURN_3          (0x8U << FMC_BTR3_BUSTURN_Pos)             /*!< 0x00080000 */

#define FMC_BTR3_CLKDIV_Pos         (20U)                                      
#define FMC_BTR3_CLKDIV_Msk         (0xFU << FMC_BTR3_CLKDIV_Pos)              /*!< 0x00F00000 */
#define FMC_BTR3_CLKDIV             FMC_BTR3_CLKDIV_Msk                        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define FMC_BTR3_CLKDIV_0           (0x1U << FMC_BTR3_CLKDIV_Pos)              /*!< 0x00100000 */
#define FMC_BTR3_CLKDIV_1           (0x2U << FMC_BTR3_CLKDIV_Pos)              /*!< 0x00200000 */
#define FMC_BTR3_CLKDIV_2           (0x4U << FMC_BTR3_CLKDIV_Pos)              /*!< 0x00400000 */
#define FMC_BTR3_CLKDIV_3           (0x8U << FMC_BTR3_CLKDIV_Pos)              /*!< 0x00800000 */

#define FMC_BTR3_DATLAT_Pos         (24U)                                      
#define FMC_BTR3_DATLAT_Msk         (0xFU << FMC_BTR3_DATLAT_Pos)              /*!< 0x0F000000 */
#define FMC_BTR3_DATLAT             FMC_BTR3_DATLAT_Msk                        /*!<DATLA[3:0] bits (Data latency) */
#define FMC_BTR3_DATLAT_0           (0x1U << FMC_BTR3_DATLAT_Pos)              /*!< 0x01000000 */
#define FMC_BTR3_DATLAT_1           (0x2U << FMC_BTR3_DATLAT_Pos)              /*!< 0x02000000 */
#define FMC_BTR3_DATLAT_2           (0x4U << FMC_BTR3_DATLAT_Pos)              /*!< 0x04000000 */
#define FMC_BTR3_DATLAT_3           (0x8U << FMC_BTR3_DATLAT_Pos)              /*!< 0x08000000 */

#define FMC_BTR3_ACCMOD_Pos         (28U)                                      
#define FMC_BTR3_ACCMOD_Msk         (0x3U << FMC_BTR3_ACCMOD_Pos)              /*!< 0x30000000 */
#define FMC_BTR3_ACCMOD             FMC_BTR3_ACCMOD_Msk                        /*!<ACCMOD[1:0] bits (Access mode) */
#define FMC_BTR3_ACCMOD_0           (0x1U << FMC_BTR3_ACCMOD_Pos)              /*!< 0x10000000 */
#define FMC_BTR3_ACCMOD_1           (0x2U << FMC_BTR3_ACCMOD_Pos)              /*!< 0x20000000 */

/******************  Bit definition for FMC_BTR4 register  *******************/
#define FMC_BTR4_ADDSET_Pos         (0U)                                       
#define FMC_BTR4_ADDSET_Msk         (0xFU << FMC_BTR4_ADDSET_Pos)              /*!< 0x0000000F */
#define FMC_BTR4_ADDSET             FMC_BTR4_ADDSET_Msk                        /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define FMC_BTR4_ADDSET_0           (0x1U << FMC_BTR4_ADDSET_Pos)              /*!< 0x00000001 */
#define FMC_BTR4_ADDSET_1           (0x2U << FMC_BTR4_ADDSET_Pos)              /*!< 0x00000002 */
#define FMC_BTR4_ADDSET_2           (0x4U << FMC_BTR4_ADDSET_Pos)              /*!< 0x00000004 */
#define FMC_BTR4_ADDSET_3           (0x8U << FMC_BTR4_ADDSET_Pos)              /*!< 0x00000008 */

#define FMC_BTR4_ADDHLD_Pos         (4U)                                       
#define FMC_BTR4_ADDHLD_Msk         (0xFU << FMC_BTR4_ADDHLD_Pos)              /*!< 0x000000F0 */
#define FMC_BTR4_ADDHLD             FMC_BTR4_ADDHLD_Msk                        /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define FMC_BTR4_ADDHLD_0           (0x1U << FMC_BTR4_ADDHLD_Pos)              /*!< 0x00000010 */
#define FMC_BTR4_ADDHLD_1           (0x2U << FMC_BTR4_ADDHLD_Pos)              /*!< 0x00000020 */
#define FMC_BTR4_ADDHLD_2           (0x4U << FMC_BTR4_ADDHLD_Pos)              /*!< 0x00000040 */
#define FMC_BTR4_ADDHLD_3           (0x8U << FMC_BTR4_ADDHLD_Pos)              /*!< 0x00000080 */

#define FMC_BTR4_DATAST_Pos         (8U)                                       
#define FMC_BTR4_DATAST_Msk         (0xFFU << FMC_BTR4_DATAST_Pos)             /*!< 0x0000FF00 */
#define FMC_BTR4_DATAST             FMC_BTR4_DATAST_Msk                        /*!<DATAST [3:0] bits (Data-phase duration) */
#define FMC_BTR4_DATAST_0           (0x01U << FMC_BTR4_DATAST_Pos)             /*!< 0x00000100 */
#define FMC_BTR4_DATAST_1           (0x02U << FMC_BTR4_DATAST_Pos)             /*!< 0x00000200 */
#define FMC_BTR4_DATAST_2           (0x04U << FMC_BTR4_DATAST_Pos)             /*!< 0x00000400 */
#define FMC_BTR4_DATAST_3           (0x08U << FMC_BTR4_DATAST_Pos)             /*!< 0x00000800 */
#define FMC_BTR4_DATAST_4           (0x10U << FMC_BTR4_DATAST_Pos)             /*!< 0x00001000 */
#define FMC_BTR4_DATAST_5           (0x20U << FMC_BTR4_DATAST_Pos)             /*!< 0x00002000 */
#define FMC_BTR4_DATAST_6           (0x40U << FMC_BTR4_DATAST_Pos)             /*!< 0x00004000 */
#define FMC_BTR4_DATAST_7           (0x80U << FMC_BTR4_DATAST_Pos)             /*!< 0x00008000 */

#define FMC_BTR4_BUSTURN_Pos        (16U)                                      
#define FMC_BTR4_BUSTURN_Msk        (0xFU << FMC_BTR4_BUSTURN_Pos)             /*!< 0x000F0000 */
#define FMC_BTR4_BUSTURN            FMC_BTR4_BUSTURN_Msk                       /*!<BUSTURN[3:0] bits (Bus turnaround phase duration) */
#define FMC_BTR4_BUSTURN_0          (0x1U << FMC_BTR4_BUSTURN_Pos)             /*!< 0x00010000 */
#define FMC_BTR4_BUSTURN_1          (0x2U << FMC_BTR4_BUSTURN_Pos)             /*!< 0x00020000 */
#define FMC_BTR4_BUSTURN_2          (0x4U << FMC_BTR4_BUSTURN_Pos)             /*!< 0x00040000 */
#define FMC_BTR4_BUSTURN_3          (0x8U << FMC_BTR4_BUSTURN_Pos)             /*!< 0x00080000 */

#define FMC_BTR4_CLKDIV_Pos         (20U)                                      
#define FMC_BTR4_CLKDIV_Msk         (0xFU << FMC_BTR4_CLKDIV_Pos)              /*!< 0x00F00000 */
#define FMC_BTR4_CLKDIV             FMC_BTR4_CLKDIV_Msk                        /*!<CLKDIV[3:0] bits (Clock divide ratio) */
#define FMC_BTR4_CLKDIV_0           (0x1U << FMC_BTR4_CLKDIV_Pos)              /*!< 0x00100000 */
#define FMC_BTR4_CLKDIV_1           (0x2U << FMC_BTR4_CLKDIV_Pos)              /*!< 0x00200000 */
#define FMC_BTR4_CLKDIV_2           (0x4U << FMC_BTR4_CLKDIV_Pos)              /*!< 0x00400000 */
#define FMC_BTR4_CLKDIV_3           (0x8U << FMC_BTR4_CLKDIV_Pos)              /*!< 0x00800000 */

#define FMC_BTR4_DATLAT_Pos         (24U)                                      
#define FMC_BTR4_DATLAT_Msk         (0xFU << FMC_BTR4_DATLAT_Pos)              /*!< 0x0F000000 */
#define FMC_BTR4_DATLAT             FMC_BTR4_DATLAT_Msk                        /*!<DATLA[3:0] bits (Data latency) */
#define FMC_BTR4_DATLAT_0           (0x1U << FMC_BTR4_DATLAT_Pos)              /*!< 0x01000000 */
#define FMC_BTR4_DATLAT_1           (0x2U << FMC_BTR4_DATLAT_Pos)              /*!< 0x02000000 */
#define FMC_BTR4_DATLAT_2           (0x4U << FMC_BTR4_DATLAT_Pos)              /*!< 0x04000000 */
#define FMC_BTR4_DATLAT_3           (0x8U << FMC_BTR4_DATLAT_Pos)              /*!< 0x08000000 */

#define FMC_BTR4_ACCMOD_Pos         (28U)                                      
#define FMC_BTR4_ACCMOD_Msk         (0x3U << FMC_BTR4_ACCMOD_Pos)              /*!< 0x30000000 */
#define FMC_BTR4_ACCMOD             FMC_BTR4_ACCMOD_Msk                        /*!<ACCMOD[1:0] bits (Access mode) */
#define FMC_BTR4_ACCMOD_0           (0x1U << FMC_BTR4_ACCMOD_Pos)              /*!< 0x10000000 */
#define FMC_BTR4_ACCMOD_1           (0x2U << FMC_BTR4_ACCMOD_Pos)              /*!< 0x20000000 */

/******************  Bit definition for FMC_BWTR1 register  ******************/
#define FMC_BWTR1_ADDSET_Pos        (0U)                                       
#define FMC_BWTR1_ADDSET_Msk        (0xFU << FMC_BWTR1_ADDSET_Pos)             /*!< 0x0000000F */
#define FMC_BWTR1_ADDSET            FMC_BWTR1_ADDSET_Msk                       /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define FMC_BWTR1_ADDSET_0          (0x1U << FMC_BWTR1_ADDSET_Pos)             /*!< 0x00000001 */
#define FMC_BWTR1_ADDSET_1          (0x2U << FMC_BWTR1_ADDSET_Pos)             /*!< 0x00000002 */
#define FMC_BWTR1_ADDSET_2          (0x4U << FMC_BWTR1_ADDSET_Pos)             /*!< 0x00000004 */
#define FMC_BWTR1_ADDSET_3          (0x8U << FMC_BWTR1_ADDSET_Pos)             /*!< 0x00000008 */

#define FMC_BWTR1_ADDHLD_Pos        (4U)                                       
#define FMC_BWTR1_ADDHLD_Msk        (0xFU << FMC_BWTR1_ADDHLD_Pos)             /*!< 0x000000F0 */
#define FMC_BWTR1_ADDHLD            FMC_BWTR1_ADDHLD_Msk                       /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define FMC_BWTR1_ADDHLD_0          (0x1U << FMC_BWTR1_ADDHLD_Pos)             /*!< 0x00000010 */
#define FMC_BWTR1_ADDHLD_1          (0x2U << FMC_BWTR1_ADDHLD_Pos)             /*!< 0x00000020 */
#define FMC_BWTR1_ADDHLD_2          (0x4U << FMC_BWTR1_ADDHLD_Pos)             /*!< 0x00000040 */
#define FMC_BWTR1_ADDHLD_3          (0x8U << FMC_BWTR1_ADDHLD_Pos)             /*!< 0x00000080 */

#define FMC_BWTR1_DATAST_Pos        (8U)                                       
#define FMC_BWTR1_DATAST_Msk        (0xFFU << FMC_BWTR1_DATAST_Pos)            /*!< 0x0000FF00 */
#define FMC_BWTR1_DATAST            FMC_BWTR1_DATAST_Msk                       /*!<DATAST [3:0] bits (Data-phase duration) */
#define FMC_BWTR1_DATAST_0          (0x01U << FMC_BWTR1_DATAST_Pos)            /*!< 0x00000100 */
#define FMC_BWTR1_DATAST_1          (0x02U << FMC_BWTR1_DATAST_Pos)            /*!< 0x00000200 */
#define FMC_BWTR1_DATAST_2          (0x04U << FMC_BWTR1_DATAST_Pos)            /*!< 0x00000400 */
#define FMC_BWTR1_DATAST_3          (0x08U << FMC_BWTR1_DATAST_Pos)            /*!< 0x00000800 */
#define FMC_BWTR1_DATAST_4          (0x10U << FMC_BWTR1_DATAST_Pos)            /*!< 0x00001000 */
#define FMC_BWTR1_DATAST_5          (0x20U << FMC_BWTR1_DATAST_Pos)            /*!< 0x00002000 */
#define FMC_BWTR1_DATAST_6          (0x40U << FMC_BWTR1_DATAST_Pos)            /*!< 0x00004000 */
#define FMC_BWTR1_DATAST_7          (0x80U << FMC_BWTR1_DATAST_Pos)            /*!< 0x00008000 */

#define FMC_BWTR1_BUSTURN_Pos       (16U)                                      
#define FMC_BWTR1_BUSTURN_Msk       (0xFU << FMC_BWTR1_BUSTURN_Pos)            /*!< 0x000F0000 */
#define FMC_BWTR1_BUSTURN           FMC_BWTR1_BUSTURN_Msk                      /*!<BUSTURN[3:0] bits (Bus turnaround duration) */
#define FMC_BWTR1_BUSTURN_0         (0x1U << FMC_BWTR1_BUSTURN_Pos)            /*!< 0x00010000 */
#define FMC_BWTR1_BUSTURN_1         (0x2U << FMC_BWTR1_BUSTURN_Pos)            /*!< 0x00020000 */
#define FMC_BWTR1_BUSTURN_2         (0x4U << FMC_BWTR1_BUSTURN_Pos)            /*!< 0x00040000 */
#define FMC_BWTR1_BUSTURN_3         (0x8U << FMC_BWTR1_BUSTURN_Pos)            /*!< 0x00080000 */

#define FMC_BWTR1_ACCMOD_Pos        (28U)                                      
#define FMC_BWTR1_ACCMOD_Msk        (0x3U << FMC_BWTR1_ACCMOD_Pos)             /*!< 0x30000000 */
#define FMC_BWTR1_ACCMOD            FMC_BWTR1_ACCMOD_Msk                       /*!<ACCMOD[1:0] bits (Access mode) */
#define FMC_BWTR1_ACCMOD_0          (0x1U << FMC_BWTR1_ACCMOD_Pos)             /*!< 0x10000000 */
#define FMC_BWTR1_ACCMOD_1          (0x2U << FMC_BWTR1_ACCMOD_Pos)             /*!< 0x20000000 */

/******************  Bit definition for FMC_BWTR2 register  ******************/
#define FMC_BWTR2_ADDSET_Pos        (0U)                                       
#define FMC_BWTR2_ADDSET_Msk        (0xFU << FMC_BWTR2_ADDSET_Pos)             /*!< 0x0000000F */
#define FMC_BWTR2_ADDSET            FMC_BWTR2_ADDSET_Msk                       /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define FMC_BWTR2_ADDSET_0          (0x1U << FMC_BWTR2_ADDSET_Pos)             /*!< 0x00000001 */
#define FMC_BWTR2_ADDSET_1          (0x2U << FMC_BWTR2_ADDSET_Pos)             /*!< 0x00000002 */
#define FMC_BWTR2_ADDSET_2          (0x4U << FMC_BWTR2_ADDSET_Pos)             /*!< 0x00000004 */
#define FMC_BWTR2_ADDSET_3          (0x8U << FMC_BWTR2_ADDSET_Pos)             /*!< 0x00000008 */

#define FMC_BWTR2_ADDHLD_Pos        (4U)                                       
#define FMC_BWTR2_ADDHLD_Msk        (0xFU << FMC_BWTR2_ADDHLD_Pos)             /*!< 0x000000F0 */
#define FMC_BWTR2_ADDHLD            FMC_BWTR2_ADDHLD_Msk                       /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define FMC_BWTR2_ADDHLD_0          (0x1U << FMC_BWTR2_ADDHLD_Pos)             /*!< 0x00000010 */
#define FMC_BWTR2_ADDHLD_1          (0x2U << FMC_BWTR2_ADDHLD_Pos)             /*!< 0x00000020 */
#define FMC_BWTR2_ADDHLD_2          (0x4U << FMC_BWTR2_ADDHLD_Pos)             /*!< 0x00000040 */
#define FMC_BWTR2_ADDHLD_3          (0x8U << FMC_BWTR2_ADDHLD_Pos)             /*!< 0x00000080 */

#define FMC_BWTR2_DATAST_Pos        (8U)                                       
#define FMC_BWTR2_DATAST_Msk        (0xFFU << FMC_BWTR2_DATAST_Pos)            /*!< 0x0000FF00 */
#define FMC_BWTR2_DATAST            FMC_BWTR2_DATAST_Msk                       /*!<DATAST [3:0] bits (Data-phase duration) */
#define FMC_BWTR2_DATAST_0          (0x01U << FMC_BWTR2_DATAST_Pos)            /*!< 0x00000100 */
#define FMC_BWTR2_DATAST_1          (0x02U << FMC_BWTR2_DATAST_Pos)            /*!< 0x00000200 */
#define FMC_BWTR2_DATAST_2          (0x04U << FMC_BWTR2_DATAST_Pos)            /*!< 0x00000400 */
#define FMC_BWTR2_DATAST_3          (0x08U << FMC_BWTR2_DATAST_Pos)            /*!< 0x00000800 */
#define FMC_BWTR2_DATAST_4          (0x10U << FMC_BWTR2_DATAST_Pos)            /*!< 0x00001000 */
#define FMC_BWTR2_DATAST_5          (0x20U << FMC_BWTR2_DATAST_Pos)            /*!< 0x00002000 */
#define FMC_BWTR2_DATAST_6          (0x40U << FMC_BWTR2_DATAST_Pos)            /*!< 0x00004000 */
#define FMC_BWTR2_DATAST_7          (0x80U << FMC_BWTR2_DATAST_Pos)            /*!< 0x00008000 */

#define FMC_BWTR2_BUSTURN_Pos       (16U)                                      
#define FMC_BWTR2_BUSTURN_Msk       (0xFU << FMC_BWTR2_BUSTURN_Pos)            /*!< 0x000F0000 */
#define FMC_BWTR2_BUSTURN           FMC_BWTR2_BUSTURN_Msk                      /*!<BUSTURN[3:0] bits (Bus turnaround duration) */
#define FMC_BWTR2_BUSTURN_0         (0x1U << FMC_BWTR2_BUSTURN_Pos)            /*!< 0x00010000 */
#define FMC_BWTR2_BUSTURN_1         (0x2U << FMC_BWTR2_BUSTURN_Pos)            /*!< 0x00020000 */
#define FMC_BWTR2_BUSTURN_2         (0x4U << FMC_BWTR2_BUSTURN_Pos)            /*!< 0x00040000 */
#define FMC_BWTR2_BUSTURN_3         (0x8U << FMC_BWTR2_BUSTURN_Pos)            /*!< 0x00080000 */

#define FMC_BWTR2_ACCMOD_Pos        (28U)                                      
#define FMC_BWTR2_ACCMOD_Msk        (0x3U << FMC_BWTR2_ACCMOD_Pos)             /*!< 0x30000000 */
#define FMC_BWTR2_ACCMOD            FMC_BWTR2_ACCMOD_Msk                       /*!<ACCMOD[1:0] bits (Access mode) */
#define FMC_BWTR2_ACCMOD_0          (0x1U << FMC_BWTR2_ACCMOD_Pos)             /*!< 0x10000000 */
#define FMC_BWTR2_ACCMOD_1          (0x2U << FMC_BWTR2_ACCMOD_Pos)             /*!< 0x20000000 */

/******************  Bit definition for FMC_BWTR3 register  ******************/
#define FMC_BWTR3_ADDSET_Pos        (0U)                                       
#define FMC_BWTR3_ADDSET_Msk        (0xFU << FMC_BWTR3_ADDSET_Pos)             /*!< 0x0000000F */
#define FMC_BWTR3_ADDSET            FMC_BWTR3_ADDSET_Msk                       /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define FMC_BWTR3_ADDSET_0          (0x1U << FMC_BWTR3_ADDSET_Pos)             /*!< 0x00000001 */
#define FMC_BWTR3_ADDSET_1          (0x2U << FMC_BWTR3_ADDSET_Pos)             /*!< 0x00000002 */
#define FMC_BWTR3_ADDSET_2          (0x4U << FMC_BWTR3_ADDSET_Pos)             /*!< 0x00000004 */
#define FMC_BWTR3_ADDSET_3          (0x8U << FMC_BWTR3_ADDSET_Pos)             /*!< 0x00000008 */

#define FMC_BWTR3_ADDHLD_Pos        (4U)                                       
#define FMC_BWTR3_ADDHLD_Msk        (0xFU << FMC_BWTR3_ADDHLD_Pos)             /*!< 0x000000F0 */
#define FMC_BWTR3_ADDHLD            FMC_BWTR3_ADDHLD_Msk                       /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define FMC_BWTR3_ADDHLD_0          (0x1U << FMC_BWTR3_ADDHLD_Pos)             /*!< 0x00000010 */
#define FMC_BWTR3_ADDHLD_1          (0x2U << FMC_BWTR3_ADDHLD_Pos)             /*!< 0x00000020 */
#define FMC_BWTR3_ADDHLD_2          (0x4U << FMC_BWTR3_ADDHLD_Pos)             /*!< 0x00000040 */
#define FMC_BWTR3_ADDHLD_3          (0x8U << FMC_BWTR3_ADDHLD_Pos)             /*!< 0x00000080 */

#define FMC_BWTR3_DATAST_Pos        (8U)                                       
#define FMC_BWTR3_DATAST_Msk        (0xFFU << FMC_BWTR3_DATAST_Pos)            /*!< 0x0000FF00 */
#define FMC_BWTR3_DATAST            FMC_BWTR3_DATAST_Msk                       /*!<DATAST [3:0] bits (Data-phase duration) */
#define FMC_BWTR3_DATAST_0          (0x01U << FMC_BWTR3_DATAST_Pos)            /*!< 0x00000100 */
#define FMC_BWTR3_DATAST_1          (0x02U << FMC_BWTR3_DATAST_Pos)            /*!< 0x00000200 */
#define FMC_BWTR3_DATAST_2          (0x04U << FMC_BWTR3_DATAST_Pos)            /*!< 0x00000400 */
#define FMC_BWTR3_DATAST_3          (0x08U << FMC_BWTR3_DATAST_Pos)            /*!< 0x00000800 */
#define FMC_BWTR3_DATAST_4          (0x10U << FMC_BWTR3_DATAST_Pos)            /*!< 0x00001000 */
#define FMC_BWTR3_DATAST_5          (0x20U << FMC_BWTR3_DATAST_Pos)            /*!< 0x00002000 */
#define FMC_BWTR3_DATAST_6          (0x40U << FMC_BWTR3_DATAST_Pos)            /*!< 0x00004000 */
#define FMC_BWTR3_DATAST_7          (0x80U << FMC_BWTR3_DATAST_Pos)            /*!< 0x00008000 */

#define FMC_BWTR3_BUSTURN_Pos       (16U)                                      
#define FMC_BWTR3_BUSTURN_Msk       (0xFU << FMC_BWTR3_BUSTURN_Pos)            /*!< 0x000F0000 */
#define FMC_BWTR3_BUSTURN           FMC_BWTR3_BUSTURN_Msk                      /*!<BUSTURN[3:0] bits (Bus turnaround duration) */
#define FMC_BWTR3_BUSTURN_0         (0x1U << FMC_BWTR3_BUSTURN_Pos)            /*!< 0x00010000 */
#define FMC_BWTR3_BUSTURN_1         (0x2U << FMC_BWTR3_BUSTURN_Pos)            /*!< 0x00020000 */
#define FMC_BWTR3_BUSTURN_2         (0x4U << FMC_BWTR3_BUSTURN_Pos)            /*!< 0x00040000 */
#define FMC_BWTR3_BUSTURN_3         (0x8U << FMC_BWTR3_BUSTURN_Pos)            /*!< 0x00080000 */

#define FMC_BWTR3_ACCMOD_Pos        (28U)                                      
#define FMC_BWTR3_ACCMOD_Msk        (0x3U << FMC_BWTR3_ACCMOD_Pos)             /*!< 0x30000000 */
#define FMC_BWTR3_ACCMOD            FMC_BWTR3_ACCMOD_Msk                       /*!<ACCMOD[1:0] bits (Access mode) */
#define FMC_BWTR3_ACCMOD_0          (0x1U << FMC_BWTR3_ACCMOD_Pos)             /*!< 0x10000000 */
#define FMC_BWTR3_ACCMOD_1          (0x2U << FMC_BWTR3_ACCMOD_Pos)             /*!< 0x20000000 */

/******************  Bit definition for FMC_BWTR4 register  ******************/
#define FMC_BWTR4_ADDSET_Pos        (0U)                                       
#define FMC_BWTR4_ADDSET_Msk        (0xFU << FMC_BWTR4_ADDSET_Pos)             /*!< 0x0000000F */
#define FMC_BWTR4_ADDSET            FMC_BWTR4_ADDSET_Msk                       /*!<ADDSET[3:0] bits (Address setup phase duration) */
#define FMC_BWTR4_ADDSET_0          (0x1U << FMC_BWTR4_ADDSET_Pos)             /*!< 0x00000001 */
#define FMC_BWTR4_ADDSET_1          (0x2U << FMC_BWTR4_ADDSET_Pos)             /*!< 0x00000002 */
#define FMC_BWTR4_ADDSET_2          (0x4U << FMC_BWTR4_ADDSET_Pos)             /*!< 0x00000004 */
#define FMC_BWTR4_ADDSET_3          (0x8U << FMC_BWTR4_ADDSET_Pos)             /*!< 0x00000008 */

#define FMC_BWTR4_ADDHLD_Pos        (4U)                                       
#define FMC_BWTR4_ADDHLD_Msk        (0xFU << FMC_BWTR4_ADDHLD_Pos)             /*!< 0x000000F0 */
#define FMC_BWTR4_ADDHLD            FMC_BWTR4_ADDHLD_Msk                       /*!<ADDHLD[3:0] bits (Address-hold phase duration) */
#define FMC_BWTR4_ADDHLD_0          (0x1U << FMC_BWTR4_ADDHLD_Pos)             /*!< 0x00000010 */
#define FMC_BWTR4_ADDHLD_1          (0x2U << FMC_BWTR4_ADDHLD_Pos)             /*!< 0x00000020 */
#define FMC_BWTR4_ADDHLD_2          (0x4U << FMC_BWTR4_ADDHLD_Pos)             /*!< 0x00000040 */
#define FMC_BWTR4_ADDHLD_3          (0x8U << FMC_BWTR4_ADDHLD_Pos)             /*!< 0x00000080 */

#define FMC_BWTR4_DATAST_Pos        (8U)                                       
#define FMC_BWTR4_DATAST_Msk        (0xFFU << FMC_BWTR4_DATAST_Pos)            /*!< 0x0000FF00 */
#define FMC_BWTR4_DATAST            FMC_BWTR4_DATAST_Msk                       /*!<DATAST [3:0] bits (Data-phase duration) */
#define FMC_BWTR4_DATAST_0          (0x01U << FMC_BWTR4_DATAST_Pos)            /*!< 0x00000100 */
#define FMC_BWTR4_DATAST_1          (0x02U << FMC_BWTR4_DATAST_Pos)            /*!< 0x00000200 */
#define FMC_BWTR4_DATAST_2          (0x04U << FMC_BWTR4_DATAST_Pos)            /*!< 0x00000400 */
#define FMC_BWTR4_DATAST_3          (0x08U << FMC_BWTR4_DATAST_Pos)            /*!< 0x00000800 */
#define FMC_BWTR4_DATAST_4          (0x10U << FMC_BWTR4_DATAST_Pos)            /*!< 0x00001000 */
#define FMC_BWTR4_DATAST_5          (0x20U << FMC_BWTR4_DATAST_Pos)            /*!< 0x00002000 */
#define FMC_BWTR4_DATAST_6          (0x40U << FMC_BWTR4_DATAST_Pos)            /*!< 0x00004000 */
#define FMC_BWTR4_DATAST_7          (0x80U << FMC_BWTR4_DATAST_Pos)            /*!< 0x00008000 */

#define FMC_BWTR4_BUSTURN_Pos       (16U)                                      
#define FMC_BWTR4_BUSTURN_Msk       (0xFU << FMC_BWTR4_BUSTURN_Pos)            /*!< 0x000F0000 */
#define FMC_BWTR4_BUSTURN           FMC_BWTR4_BUSTURN_Msk                      /*!<BUSTURN[3:0] bits (Bus turnaround duration) */
#define FMC_BWTR4_BUSTURN_0         (0x1U << FMC_BWTR4_BUSTURN_Pos)            /*!< 0x00010000 */
#define FMC_BWTR4_BUSTURN_1         (0x2U << FMC_BWTR4_BUSTURN_Pos)            /*!< 0x00020000 */
#define FMC_BWTR4_BUSTURN_2         (0x4U << FMC_BWTR4_BUSTURN_Pos)            /*!< 0x00040000 */
#define FMC_BWTR4_BUSTURN_3         (0x8U << FMC_BWTR4_BUSTURN_Pos)            /*!< 0x00080000 */

#define FMC_BWTR4_ACCMOD_Pos        (28U)                                      
#define FMC_BWTR4_ACCMOD_Msk        (0x3U << FMC_BWTR4_ACCMOD_Pos)             /*!< 0x30000000 */
#define FMC_BWTR4_ACCMOD            FMC_BWTR4_ACCMOD_Msk                       /*!<ACCMOD[1:0] bits (Access mode) */
#define FMC_BWTR4_ACCMOD_0          (0x1U << FMC_BWTR4_ACCMOD_Pos)             /*!< 0x10000000 */
#define FMC_BWTR4_ACCMOD_1          (0x2U << FMC_BWTR4_ACCMOD_Pos)             /*!< 0x20000000 */

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_FMC_H_ */
