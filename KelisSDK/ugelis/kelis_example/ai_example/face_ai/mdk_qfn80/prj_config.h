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

//#define  GM6721_EVB_QFN40PIN
//#define  GM6721_EVB_QFN80PIN
//#define  GM6721_EVB_144PIN
//#define  GM6721_EVB_176PIN

/**
 *feature for your project
 */
#define CONFIG_CONSOLE    1

#define CONFIG_SFLASH     1
#define CONFIG_SDRAM_BOOT  1
#define CONFIG_POLL

/*
faceai memory optimize
*/
#define CONFIG_KERNEL_MEM_RESVER_SIZE 0
#define __SIZE__K(x)                        (x * 1024)
#define  CAMERA_IMAGE_BUFFER_LENGTH        __SIZE__K(0)


/*Subsystem: USB Host Stack Configuration*/
//#define CONFIG_GM_USB_HOST_STACK
//#define CONFIG_USB_MASS_STORAGE
#define CONFIG_SHELL
//#define CONFIG_CONSOLE
//#define CONFIG_UART_CONSOLE
#define CONFIG_OPENMV
#define CONFIG_GPIO
#define CONFIG_PINMUX
#define CONFIG_OV5640
#define CONFIG_DISPLAY
//#define QFN80_SORTING_BOARD

#define CONFIG_GT9147

#ifdef __cplusplus
}
#endif

#endif /* _PRJ_CONFIG_H_ */
