
#ifndef _PRJ_CONFIG_H_
#define _PRJ_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *platform select
 *#########################################
 *!!!you must be sure only open one
 *!!!!plafrom marco and close the others
 *########################################
 */
#define  F6721B_EVB_144PIN 1
//#define  F6721B_EVB_176PIN 1
//#define  F6721B_EVB_QFN40PIN 1
//#define  F6721B_EVB_QFN80PIN 1

//#define  GM6721_EVB_QFN40PIN
//#define  GM6721_EVB_QFN80PIN
//#define  GM6721_EVB_144PIN
//#define  GM6721_EVB_176PIN

#define  CHIP_F6721B

#define CONFIG_ENTROPY
#define CONFIG_CONSOLE
//#define CONFIG_IWDG_GM_DEVICE_NAME "IWDG"
//#define CONFIG_IWDG_GM_PRESCALER      64
//#define CONFIG_IWDG_GM_RELOAD_COUNTER 500
#ifdef __cplusplus
}
#endif

#endif /* _PRJ_CONFIG_H_ */
