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
//#define  F6721B_EVB_176PIN 1
//#define  LQFP176_DEMO_BOARD

#define  F6721B_EVB_QFN80PIN 1

/**
 *feature for your project
 */
#ifdef F6721B_EVB_QFN80PIN
#define CONFIG_KERNEL_MEM_RESVER_SIZE   0
#define CAMERA_IMAGE_BUFFER_LENGTH      0
#endif


#define CONFIG_CONSOLE      1

#define CONFIG_SFLASH       1
#define CONFIG_SDRAM_BOOT   1
#define CONFIG_POLL

/* sai config*/
#define RECORDER_SAI        1
#define CONFIG_SAIA_INT     1
#define CONFIG_SAIB_INT     1
#define CONFIG_SAIA_DMA     0
#define CONFIG_SAIB_DMA     0
#if RECORDER_SAI
#define DMA_PINGPANG_MODE   1
#endif

#define CONFIG_SHELL
#define CONFIG_OV5640
#define CONFIG_DISPLAY

//#define ATK7                1   //7'  (800x480) touch screen
#define ATK43               1   //4.3'(480x272) touch screen

#if ATK7
#define CONFIG_LCD800x480
#define CONFIG_FT5426
#endif

#if ATK43
#define CONFIG_GT9147
#endif

#ifdef __cplusplus
}
#endif

#endif /* _PRJ_CONFIG_H_ */
