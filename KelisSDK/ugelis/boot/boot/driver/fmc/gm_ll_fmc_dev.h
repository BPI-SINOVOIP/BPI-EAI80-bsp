
#ifndef _GM_LL_FMC_DEV_H_
#define _GM_LL_FMC_DEV_H_
#include <gm_common.h>

#ifdef __cplusplus
extern "C" {
#endif
#if 0
typedef struct
{
  __IO uint32_t BTCR[8];    /*!< NOR/PSRAM chip-select control register(BCR) and chip-select timing register(BTR), Address offset: 0x00-1C */
} FMC_Bank1_TypeDef; 

/** 
  * @brief Flexible Memory Controller Bank1E
  */
  
typedef struct
{
  __IO uint32_t BWTR[7];    /*!< NOR/PSRAM write timing registers, Address offset: 0x104-0x11C */
} FMC_Bank1E_TypeDef;

#define FMC_Bank1           ((FMC_Bank1_TypeDef *) FMC_Bank1_R_BASE)
#define FMC_Bank1E          ((FMC_Bank1E_TypeDef *) FMC_Bank1E_R_BASE)

#define FMC_R_BASE            ((uint32_t)0xA0000000) /*!< todo, FMC registers base address */

#define FMC_Bank1_R_BASE      (FMC_R_BASE + 0x0000) //todo
#define FMC_Bank1E_R_BASE     (FMC_R_BASE + 0x0104) //todo
#endif


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_FMC_DEV_H_ */
