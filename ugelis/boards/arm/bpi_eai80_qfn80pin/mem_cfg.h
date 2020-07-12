/*
 * Copyright (c) 2016 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MEM_CFG_H
#define __MEM_CFG_H
#ifdef __cplusplus
extern "C" {
#endif

#if (defined(CONFIG_SDRAM_BOOT) || defined(CONFIG_SLAVE_SDRAM))

#include <ugelis.h>

#define __SIZE_K(x)                        (x * 1024)

/**
 * brief:resver memcfg for power manage for save contex we need
 */
#define MEM_PM_RESVER_SIZE              __SIZE_K(1)
#define MEM_PM_SAVE_START               (CONFIG_SDRAM_BASE_ADDRESS)
#define MEM_PM_SAVE_END                 (MEM_PM_SAVE_START+MEM_PM_RESVER_SIZE)

#if(MEM_PM_SAVE_END > (CONFIG_SDRAM_BASE_ADDRESS + __SIZE_K(CONFIG_IMAGE_OFFSET_SIZE)))
#error "###MEM_PM_SAVE_END > (CONFIG_SDRAM_BASE_ADDRESS + __SIZE_K(CONFIG_IMAGE_OFFSET_SIZE))####"
#endif


/**
 * brief:resver memcfg
 * the resver spavce memory is for kernel or some specil driver to use.
 * application please not use those resver memory.
 */
#define  MEM_RESVER_SIZE                   __SIZE_K(CONFIG_KERNEL_MEM_RESVER_SIZE)
#define  MEM_RESVER_START                  (CONFIG_SDRAM_BASE_ADDRESS + __SIZE_K(CONFIG_SDRAM_SIZE) - MEM_RESVER_SIZE)

/*resever memory for demo module0*/
#define  DEMO_MODULE0_KERNEL_USE_START     (MEM_RESVER_START)
#define  DEMO_MODULE0_KERNEL_USE_LENGTH    (0)
#define  DEMO_MODULE0_KERNEL_USE_END       (DEMO_MODULE0_KERNEL_USE_START+ DEMO_MODULE0_KERNEL_USE_LENGTH)
/*resever memory for demo module1*/
#define  DEMO_MODULE1_KERNEL_USE_START     (DEMO_MODULE0_KERNEL_USE_END)
#define  DEMO_MODULE1_KERNEL_USE_LENGTH    (0)
#define  DEMO_MODULE1_KERNEL_USE_END       (DEMO_MODULE1_KERNEL_USE_START+ DEMO_MODULE1_KERNEL_USE_LENGTH)

/*resever memory for camera image buffer*/
#define  CAMERA_IMAGE_BUFFER_START         (DEMO_MODULE1_KERNEL_USE_END)

#ifndef CONFIG_CAMERA_IMAGE_BUFFER_LENGTH
#define  CONFIG_CAMERA_IMAGE_BUFFER_LENGTH        __SIZE_K(1024)
#endif
#define  CAMERA_IMAGE_BUFFER_END           (CAMERA_IMAGE_BUFFER_START+ CAMERA_IMAGE_BUFFER_LENGTH)

/*resever memory for slave*/
#define  SLAVE_MEM_START                    (CAMERA_IMAGE_BUFFER_END)
#define  SLAVE_MEM_LENGTH                   __SIZE_K(CONFIG_SLAVE_MEM_RESVER_SIZE)
#define  SLAVE_MEM_END                      (SLAVE_MEM_START+ SLAVE_MEM_LENGTH)

/*add other modules resver memory here*/
#define  MEM_RESVER_END                    SLAVE_MEM_END

#if(MEM_RESVER_END > (CONFIG_SDRAM_BASE_ADDRESS + __SIZE_K(CONFIG_SDRAM_SIZE)))
#error "###MEM_RESVER_END > (CONFIG_SDRAM_BASE_ADDRESS + CONFIG_SDRAM_SIZE)####"
#endif

#endif

#ifdef __cplusplus
}
#endif


#endif /* __MEM_CFG_H */
