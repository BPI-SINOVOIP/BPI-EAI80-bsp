#ifndef _PM_SYSTEM_H_
#define _PM_SYSTEM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))

#define POWER_MODE_STOP1         0
#define POWER_MODE_STOP2         1
#define POWER_MODE_STANDBY       2
#define POWER_MODE_SHUTDOWM      3

#define readb(addr)     (*((volatile unsigned char  *)(addr)))
#define readw(addr)     (*((volatile unsigned short *)(addr)))
#define readl(addr)     (*((volatile unsigned long  *)(addr)))
#define writeb(v, addr) (*((volatile unsigned char  *)(addr)) = (unsigned char)(v))
#define writew(v, addr) (*((volatile unsigned short *)(addr)) = (unsigned short)(v))
#define writel(v, addr) (*((volatile unsigned long  *)(addr)) = (unsigned long)(v))
#define SYSTEM_BASE              (0x40000000)
#define PWR_MOD_REG              (SYSTEM_BASE + 0x18 )

void goto_sleep_mode(void);
void goto_stop1_mode(void);
void goto_stop2_mode(void);
void goto_standby_mode(void);
void goto_shut_down_mode(void);

#ifdef __cplusplus
}
#endif
#endif