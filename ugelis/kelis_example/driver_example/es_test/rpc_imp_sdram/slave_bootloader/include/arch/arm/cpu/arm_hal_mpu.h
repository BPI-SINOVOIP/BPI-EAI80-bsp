/*
 * Copyright (c) 2017 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ARM_HAL_MPU_H_
#define _ARM_HAL_MPU_H_

#ifdef __cplusplus
extern "C" {
#endif

/* eXecute Never */
#define MPU_INSTRUCTION_EXEC    (0x1 << 28)

/* Privileged No Access, Unprivileged No Access */
#define MPU_DATA_P_NA_U_NA      (0x0 << 24)

/* Privileged Read Write, Unprivileged No Access */
#define MPU_DATA_P_RW_U_NA      (0x1 << 24)

/* Privileged Read Write, Unprivileged Read Only */
#define MPU_DATA_P_RW_U_RO      (0x2 << 24)

/* Privileged Read Write, Unprivileged Read Write */
#define MPU_DATA_P_RW_U_RW      (0x3 << 24)


/* do kown ; see sepc*/
#define MPU_DATA_NO_RESULT      (0x4 << 24)


/* Privileged Read Only, Unprivileged No Access */
#define MPU_DATA_P_RO_U_NA      (0x5 << 24)
/* Privileged Read Only, Unprivileged Read Only */
#define MPU_DATA_P_RO_U_RO      (0x6 << 24)
/* Privileged Read Only, Unprivileged Read Only */
#define MPU_DATA_RO             (0x7 << 24)


#define MPU_REGION_32B      (0x04 << 1)
#define MPU_REGION_64B      (0x05 << 1)
#define MPU_REGION_128B     (0x06 << 1)
#define MPU_REGION_256B     (0x07 << 1)
#define MPU_REGION_512B     (0x08 << 1)
#define MPU_REGION_1K       (0x09 << 1)
#define MPU_REGION_2K       (0x0A << 1)
#define MPU_REGION_4K       (0x0B << 1)
#define MPU_REGION_8K       (0x0C << 1)
#define MPU_REGION_16K      (0x0D << 1)
#define MPU_REGION_32K      (0x0E << 1)
#define MPU_REGION_64K      (0x0F << 1)
#define MPU_REGION_128K     (0x10 << 1)
#define MPU_REGION_256K     (0x11 << 1)
#define MPU_REGION_512K     (0x12 << 1)
#define MPU_REGION_1M       (0x13 << 1)
#define MPU_REGION_2M       (0x14 << 1)
#define MPU_REGION_4M       (0x15 << 1)
#define MPU_REGION_8M       (0x16 << 1)
#define MPU_REGION_16M      (0x17 << 1)
#define MPU_REGION_32M      (0x18 << 1)
#define MPU_REGION_64M      (0x19 << 1)
#define MPU_REGION_128M     (0x1A << 1)
#define MPU_REGION_256M     (0x1B << 1)
#define MPU_REGION_512M     (0x1C << 1)
#define MPU_REGION_1G       (0x1D << 1)
#define MPU_REGION_2G       (0x1E << 1)
#define MPU_REGION_4G       (0x1F << 1)



#define MPU_REGION_ENTRY(_name, _base, _attr) \
    {\
        .name = _name, \
                .base = _base, \
                        .attr = _attr, \
    }

typedef struct
{
    /* Region Base Address */
    uint32_t base;
    /* Region Name */
    const char name[10];
    /* Region Attributes */
    uint32_t attr;
} MPU_REGION_T;


/* MPU configuration data structure */
typedef struct
{
    /* Number of regions */
    uint32_t numRegions;
    /* Regions */
    MPU_REGION_T *mpuRegions;

} MPU_CONFIG_T;




extern RET_CODE HAL_MPU_Enable(void);
extern RET_CODE HAL_MPU_Disable(void);
extern RET_CODE HAL_MPU_GetRegionsNum(void);

extern RET_CODE HAL_MPU_MutiRegionConfig(MPU_CONFIG_T *pConfig);
extern RET_CODE HAL_MPU_SingleRegionConfig(uint32_t index, uint32_t regionAddr,
        uint32_t regionAttr);


#ifdef __cplusplus
}
#endif
#endif /* _ARM_MPU_H_ */
