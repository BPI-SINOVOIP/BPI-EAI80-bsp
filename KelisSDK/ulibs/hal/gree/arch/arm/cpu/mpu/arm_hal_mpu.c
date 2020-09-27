/*
 * Copyright (c) 2017 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <gm_common.h>
#include <gm_soc.h>
#include <arm_hal_mpu.h>

#define ARM_MPU_DEV     ((MPU_Type *)(SCS_BASE +  0x0D90UL))



/* ARM MPU Enabled state */
static uint8_t mpuEnabled;
static uint8_t lock = HAL_UNLOCK;


static RET_CODE HAL_MPU_RegionSeting(uint32_t index, uint32_t regionAddr,
			                                      uint32_t regionAttr);

RET_CODE HAL_MPU_GetRegionsNum(void)
{
    RET_CODE ret = RET_OK;
    uint32_t type = 0;

    ret = HAL_TryLock(&lock);
    if(ret){
        return ret;
    }

	type = ARM_MPU_DEV->TYPE;

    HAL_UnLock(&lock);

	type = (type & 0xFF00) >> 8;

	return (uint8_t)type;
}



/* ARM Core MPU Driver API Implementation for ARM MPU */

/**
 * @brief enable the MPU
 */
RET_CODE HAL_MPU_Enable(void)
{

    RET_CODE ret = RET_OK;

    ret = HAL_TryLock(&lock);
    if(ret){
        goto out;
    }

	if (mpuEnabled == 0) {
		/* Enable MPU */

        ARM_MPU_DEV->CTRL = MPU_CTRL_ENABLE_Msk | MPU_CTRL_HFNMIENA_Msk|MPU_CTRL_PRIVDEFENA_Msk;
		mpuEnabled = 1;
	}

    HAL_UnLock(&lock);
out:
    return ret;
}



/**
 * @brief disable the MPU
 */
RET_CODE HAL_MPU_Disable(void)
{
    RET_CODE ret = RET_OK;

    ret = HAL_TryLock(&lock);
    if(ret){
        goto out;
    }

	if (mpuEnabled == 1) {
		/* Disable MPU */
		ARM_MPU_DEV->CTRL = 0;

		mpuEnabled = 0;
	}

    HAL_UnLock(&lock);
out:
    return ret;
}


static RET_CODE HAL_MPU_RegionSeting(uint32_t index, uint32_t regionAddr,
			 uint32_t regionAttr)
{
    RET_CODE ret = RET_OK;

    ret = HAL_TryLock(&lock);
    if(ret){
        goto out;
    }

    /* Select the region you want to access */
    ARM_MPU_DEV->RNR = index;
    /* Configure the region,disable sub region */
    ARM_MPU_DEV->RBAR = (regionAddr & MPU_RBAR_ADDR_Msk)
                         | MPU_RBAR_VALID_Msk | index ;
    ARM_MPU_DEV->RASR = regionAttr | MPU_RASR_ENABLE_Msk;

    HAL_UnLock(&lock);
out:
    return ret;

}


RET_CODE HAL_MPU_SingleRegionConfig(uint32_t index, uint32_t regionAddr,
			 uint32_t regionAttr)
{
    RET_CODE ret = RET_OK;

    ret = HAL_TryLock(&lock);
    if(ret){
        goto out;
    }

	/* Disable MPU */
	HAL_MPU_Disable();

    HAL_MPU_RegionSeting(index, regionAddr , regionAttr);

	/* Enable MPU */
	HAL_MPU_Enable();

    HAL_UnLock(&lock);

out:
    return ret;
}


RET_CODE HAL_MPU_MutiRegionConfig(MPU_CONFIG_T * pConfig)
{
	uint32_t index;

    RET_CODE ret = RET_OK;

	/* ARM MPU supports up to 16 Regions */
	if (pConfig->numRegions > HAL_MPU_GetRegionsNum()) {
		return RET_INVPARAM;
	}

	/* Disable MPU */
	ret = HAL_MPU_Disable();
    if(ret){
        return ret;
    }
	/* Configure regions */
	for (index = 0; index < pConfig->numRegions; index++) {
        ret = HAL_MPU_RegionSeting(index,
                                   pConfig->mpuRegions[index].base,
                                   pConfig->mpuRegions[index].attr);
        if(ret){
            return ret;
        }
	}

	/* Enable MPU */
	ret = HAL_MPU_Enable();

    return ret;

}


