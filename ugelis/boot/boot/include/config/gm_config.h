
#ifndef _GM_CONFIG_H_
#define _GM_CONFIG_H_

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*HAL CONFIG FILE*/
//#if defined(CONFIG_SOC_GM6721)
//    #define CHIP_GM6721
//#else
//    #error "define gm chip not match"
//#endif

//#define SYS_CLOCK    CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC

/*
 * Include the cpu specific header file
*/


#if (defined(CHIP_GM6611) || defined(CHIP_GM6721))

#include <gm_soc.h>

#else
 #error "define gm chip not match"
#endif


/**
 *for specil user want to operate device reg direct
 *for example for fpga test.maybe user want to rw
 *reg directly for qucikly test.
 */
#ifdef  CONFIG_USE_DEVICE_LL_DIRECT
    #define USE_DEVICE_LL_DIRECT 1
#endif

/**
 *when use to impliment start.s weak hander irq,
 *for exampler use keil as mcu develop
 */
//#define IRQ_HANDER_WEAK 1


/*add for more hal config,for default seting*/
/*tofor hal config */

#ifdef __cplusplus
}
#endif

#endif /* _GM_CONFIG_H_ */



















































