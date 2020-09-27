/*
 * Config the features of startup demo programs.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include<kernel.h>

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

// GCC < 4.8.2 (Before BSP400 gcc version)
// Check the GCC version for toolchain builtin macro compatible issue.


#define LOOP_COUNT              2000000 // loop count for the benchmark


#define CPU_MHz                 (CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC/1000000)


#define malloc                  k_malloc

#endif // __CONFIG_H__
