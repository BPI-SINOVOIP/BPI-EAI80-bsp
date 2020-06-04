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
//#define  F6721B_EVB_144PIN 1
//#define  F6721B_EVB_176PIN 1
//#define  F6721B_EVB_QFN40PIN 1
#define  F6721B_EVB_QFN80PIN 1

/**
 *feature for your project
 */
#define CONFIG_CONSOLE    1

#define CONFIG_SFLASH     1

#define CONFIG_SDRAM_BOOT  1

/*Kernel Configurtion*/
#define CONFIG_POLL


/*Subsystem: Console Shell*/
#define CONFIG_SHELL

#define CONFIG_DISK_ACCESS
#define CONFIG_DISK_ACCESS_SFLASH
#define CONFIG_FILE_SYSTEM

/*Driver Configuration*/
//#define CONFIG_OV5640

#define CONFIG_OV7725
#define CONFIG_DISPLAY

#ifdef __cplusplus
}
#endif

#endif /* _PRJ_CONFIG_H_ */
