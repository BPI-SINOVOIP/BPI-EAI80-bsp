/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_power.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2018/04/03
 *
 * @brief         CMSIS Peripheral Access Layer for GM6721
 *
 * @note
 *        porting to ugelis system.
 */
#ifndef _GM_LL_POWER_H_
#define _GM_LL_POWER_H_
#include <gm_common.h>
#include <gm_soc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*name to makesure*/
#define LL_CPU_ENTER_DEEP_SLEEP(En)  do{                                   \
                                            if(En){                        \
                                                SCB->SCR |= (1UL << 2);   \
                                            }else{                         \
                                                SCB->SCR &= ~(1UL << 2);  \
                                            }                              \
                                     }while(0)

#define LL_CPU_WFI()                __WFI();


#define LL_POWER_CONFIG_XPA0_WKEN(pDev, val)                 IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_IO_WKEN_MASK,\
														        	       (val)<< SYS_POWER_IO_WKEN_POS)
#define LL_POWER_CONFIG_XPA0_WKEDG(pDev, val)                IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_IO_WKEDG_MASK,\
												 			              (val)<< SYS_POWER_IO_WKEDG_POS)


#define LL_POWER_PMU_ENTER_STOP1_MODE(pDev)                  IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_POWER_MODE_EN_MASK,\
												 			              (0x00)<< SYS_POWER_POWER_MODE_EN_POS)



#define LL_POWER_PMU_ENTER_STOP2_MODE(pDev)                  IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_POWER_MODE_EN_MASK,\
																		 (0x01)<< SYS_POWER_POWER_MODE_EN_POS)

#define LL_POWER_PMU_ENTER_STANDBY_MODE(pDev)                IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_POWER_MODE_EN_MASK,\
																		 (0x02)<< SYS_POWER_POWER_MODE_EN_POS)

#define LL_POWER_PMU_ENTER_SHUTDOWN_MODE(pDev)               IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_POWER_MODE_EN_MASK,\
																		(0x03)<< SYS_POWER_POWER_MODE_EN_POS)


#define LL_POWER_CONFIG_PES_WKEN(pDev, val)                  IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_PES_WKEN_MASK,\
												 			              (val)<< SYS_POWER_PES_WKEN_POS)

#define LL_POWER_SEL_PES_WKEDG(pDev, val)                    IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_PES_WKEDG_SEL_MASK,\
										    	 	    	              (val)<< SYS_POWER_PES_WKEDG_SEL_POS)

#define LL_POWER_CONFIG_WR_UNLOCK(pDev, val)                 IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_BKP_WR_UNLOCK_MASK,\
									    		    		              (val)<< SYS_POWER_BKP_WR_UNLOCK_POS)

#define LL_POWER_CONFIG_ISO_HOLD_EN(pDev, val)               IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_ISO_HOLD_EN_MASK,\
												 			              (val)<< SYS_POWER_ISO_HOLD_EN_POS)

#define LL_POWER_CONFIG_MEM_RET_EN(pDev, val)                IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_MEM_RET_EN_MASK,\
												 			              (val)<< SYS_POWER_MEM_RET_EN_POS)

#define LL_POWER_CONFIG_LDO_HW_TRIM(pDev, val)               IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_LDO_HW_TRIM_MASK,\
                                                                          (val)<< SYS_POWER_LDO_HW_TRIM_POS)

#define LL_POWER_CONFIG_POWER_MODE_EN(pDev, val)             IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_POWER_MODE_EN_MASK,\
												 			             (val)<< SYS_POWER_POWER_MODE_EN_POS)

#define LL_POWER_CONFIG_POWER_PMUENABLE(pDev, val)           IO_BITMASK_SET(pDev->CFGPM,SYS_POWER_POWER_PMUENABLE_MASK,\
												 			             (val)<< SYS_POWER_POWER_PMUENABLE_POS)


#define LL_POWER_BACKUP_REG_UNLOCK(pDev)                     IO_BIT_SET(pDev->CFGPM, SYS_POWER_BKP_WR_UNLOCK_MASK)

#define LL_POWER_BACKUP_REG_LOCK(pDev)                       IO_BIT_CLEAR(pDev->CFGPM, SYS_POWER_BKP_WR_UNLOCK_MASK)

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_POWER_H_ */
