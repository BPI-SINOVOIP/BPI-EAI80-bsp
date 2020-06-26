/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __DT_BINDING_GM_MEM_H
#define __DT_BINDING_GM_MEM_H

#define __SIZE_K(x) (x * 1024)

#if (defined(CONFIG_SOC_GM6721)||defined(CONFIG_SOC_F6721B))
    #define DT_SRAM_SIZE            __SIZE_K(128)     /**
    * 128K For Normal System
    * 256K For AI Module(KDP310)
    */
#else
    #error "Flash, RAM, and CCM sizes not defined for this chip"
#endif

#endif /* __DT_BINDING_GM_MEM_H */
