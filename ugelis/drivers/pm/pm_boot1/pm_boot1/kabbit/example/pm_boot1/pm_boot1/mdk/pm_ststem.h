#ifndef _PM_SYSTEM_H_
#define _PM_SYSTEM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)

#define POWER_MODE_STOP1         0
#define POWER_MODE_STOP2         1
#define POWER_MODE_STANDBY       2
#define POWER_MODE_SHUTDOWM      3



#ifdef __cplusplus
}
#endif
#endif