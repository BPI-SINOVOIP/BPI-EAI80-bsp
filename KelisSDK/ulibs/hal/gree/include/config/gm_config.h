#ifndef _GM_CONFIG_H_
#define _GM_CONFIG_H_

#include "config.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * #######################################################
 *       for RABBIT_NO_OS_SYSTEM config
 * #######################################################
 */

#ifdef RABBIT_NO_OS_SYSTEM

    #include <gm_soc.h>
    /*config run addres space*/
    #ifdef CONFIG_SDRAM
        /*code:sdram data:sdram*/
        #define CONFIG_SDRAM_BOOT               1
        #define CONFIG_GM_HAL_CACHE             1
    #endif

    #ifdef CONFIG_FLASH_BOOT
        /*code:flash data:sram */
        #define  CONFIG_XIP
    #endif

#endif

/**
 * #######################################################
 *       end    RABBIT_NO_OS_SYSTEM config
 * #######################################################
 */


/**
 * #######################################################
 *       for ugelis config
 * #######################################################
 */
#if (defined(CONFIG_UGELIS))
    /*config chip id*/
    #if defined(CONFIG_SOC_GM6721)
        #define CHIP_GM6721
    #elif  defined(CONFIG_SOC_F6721B)
        #define CHIP_F6721B
    #else
        #error "soc chip not support"
    #endif

#endif

/**
 * #######################################################
 *      end ugelis config
 * #######################################################
 */


/**
* #######################################################
*       for ugelis and kgelis config
* #######################################################
*/
#if (defined(__USE_KEIL_) || defined(CONFIG_UGELIS))

    /*config clk*/
    #define SYS_CLOCK                           CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC
    #define CONFIG_SYS_CLOCK_FROM_INTERNAL      1

    /**
     *for specil user want to operate device reg direct
     *for example for fpga test.maybe user want to rw
     *reg directly for qucikly test.
     */
    #ifdef  CONFIG_USE_DEVICE_LL_DIRECT
        #define USE_DEVICE_LL_DIRECT 1
    #endif

    /*hal requeset irq*/
    #ifdef CONFIG_USE_HAL_TEST
        #define USE_HAL_TEST                    1
    #endif

#endif

/**
 * #######################################################
 *       end    for ugelis and kgelis config
 * #######################################################
 */


/**
* #######################################################
*       for ugelis and kgelis and kabbit config
* #######################################################
*/
#ifdef CONFIG_SDRAM_BOOT
    #define CONFIG_GM_HAL_DCACHE                1
#endif
/**
 * #######################################################
 *       end ugelis and kgelis and kabbit config
 * #######################################################
 */


#ifdef __cplusplus
}
#endif

#endif /* _GM_CONFIG_H_ */



















































