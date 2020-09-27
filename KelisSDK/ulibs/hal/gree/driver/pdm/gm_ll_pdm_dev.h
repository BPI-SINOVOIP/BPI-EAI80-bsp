/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_ll_pdm_dev.h
 *
 * @author      Robert_hu
 *
 * @version     v1.0.0
 *
 * @date        2019/11/01
 *
 * @brief       PDM Source file
 *
 * @note
 *              2019/11/01, Robert_hu, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef _GM_LL_PDM_DEV_H_
#define _GM_LL_PDM_DEV_H_

/*gm include files*/
#include <gm_soc.h>
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/* PDM register layout type definition */
typedef struct
{
    __IO uint32_t CFG1;                             /* PDM xConfiguration register 1 : 0x0 */
    __IO uint32_t CFG2;                             /* PDM xConfiguration register 2 : 0x4 */
    __IO uint32_t RESV1;                            /* PDM xreserver register 1 :        0x8 */
    __IO uint32_t SLOT;                             /* PDM Slot register:offset:           0x0c */
    __IO uint32_t IM;                               /* PDM xIM register : offset           0x10 */
    __IO uint32_t ST;                               /* PDM Status register:offset:        0x14 */
    __IO uint32_t CFR;                              /* PDM Clear flag register:offset:    0x18 */
    __IO uint32_t DR;                               /* PDM data register:offset:            0x1c */
    __IO uint32_t CCR;                              /* PDM Clock Control register:offset: 0x20 */
    __IO uint32_t PDM;                              /* PDM PDM:offset:                         0x24 */
    __IO uint32_t RESV3[53];                        /* PDM xreserver register 3 :        0x28~0xf8 */
    __IO uint32_t VERSION;                          /* PDM version register:offset:        0xfc */

} PDM_Device_T;
#define PDM_BASE                           (0x4003f000)
#define PDMA_BASE                          (PDM_BASE)
#define PDMB_BASE                          (PDM_BASE+0x400)
#define PDMC_BASE                          (PDM_BASE+0x800)
#define PDMD_BASE                          (PDM_BASE+0xC00)


#define PDMA_DEV                                    ((PDM_Device_T *)PDMA_BASE) /* PDMA device pointer  0x4003f000 */
#define PDMB_DEV                                    ((PDM_Device_T *)PDMB_BASE) /* PDMB device pointer  0x4003f400 */
#define PDMC_DEV                                    ((PDM_Device_T *)PDMC_BASE) /* PDMC device pointer  0x4003f800 */
#define PDMD_DEV                                    ((PDM_Device_T *)PDMD_BASE) /* PDMD device pointer  0x4003fC00 */


#ifdef __cplusplus
}
#endif

#endif /*  #ifndef _GM_LL_PDM_DEV_H_ */
