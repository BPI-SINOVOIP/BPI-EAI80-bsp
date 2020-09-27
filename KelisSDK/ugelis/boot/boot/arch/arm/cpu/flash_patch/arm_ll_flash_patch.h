/*
 * Copyright (c) 2017 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */



#ifndef ARM_LL_FLASH_PATCH_H_
#define ARM_LL_FLASH_PATCH_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <gm_common.h>

/* memory mapping structur for FPB */
typedef struct{
    __IO uint32_t CTRL;                         /*!< FPB Control Registers  */
    __IO uint32_t REMAP;                        /*!< FPB Control Registers  */
    __IO uint32_t COMP0;                        /*!< FPB Control Registers  */
    __IO uint32_t COMP1;                        /*!< FPB Control Registers  */
    __IO uint32_t COMP2;                        /*!< FPB Control Registers  */
    __IO uint32_t COMP3;                        /*!< FPB Control Registers  */
    __IO uint32_t COMP4;                        /*!< FPB Control Registers  */
    __IO uint32_t COMP5;                        /*!< FPB Control Registers  */
    __IO uint32_t COMP6;                        /*!< FPB Control Registers  */
    __IO uint32_t COMP7;                        /*!< FPB Control Registers  */

} FPB_Device_T;

#define FPB_BASE                                (0xE0002000)                             /*!< FPB Base Address */
#define FPB_Dev                                 ((FPB_Device_T *)FPB_BASE)      /*!< FPB Device   */

#ifdef __cplusplus
}
#endif
#endif

