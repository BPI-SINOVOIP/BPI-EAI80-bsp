/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file        config.h
 *
 * @author      wen.liu
 *
 * @version     1.1.0
 *
 * @date        2016/06/13
 *
 * @brief       Configuration file of this project.
 *
 * @note
 *    2015/08/21, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


/*cortex m4 enabel fpu as default*/


#define RABBIT_NO_OS_SYSTEM
#define printk  printf

/*dram run code and data*/
//#define CONFIG_SDRAM

#define DEBUG_PRINTF

/*clk config*/
#define SYS_CLOCK                           200000000
/*clk chose source*/
#define CONFIG_SYS_CLOCK_FROM_INTERNAL      1

/*for temp, it will remove when keil pack is ok*/
#undef CHIP_GM6721
#define CHIP_F6721B                         1

//#define SDRAM_8M_PM
//#define SDRAM_16M_PM
#define SDRAM_32M_PM
//#define SDRAM_64M_PM

//#define SDRAM_HASH_TEST

#define SDR_200MHz

#if 0
#ifdef SDRAM_8M_PM
#define HASH_ADDR_START    (0xc0000400)
#define HASH_SIZE       (1024*1024*8-1024)
#endif

#ifdef SDRAM_16M_PM
#define HASH_ADDR_START    (0xc0000400)
#define HASH_SIZE       (1024*1024*16-1024)
#endif

#ifdef SDRAM_32M_PM
#define HASH_ADDR_START    (0xc0000400)
#define HASH_SIZE       (1024*1024*32-1024)
#endif

#ifdef SDRAM_64M_PM
#define HASH_ADDR_START    (0xc0000400)
#define HASH_SIZE       (1024*1024*64-1024)
#endif
#endif



/**
 * Add your project's Macro here
 */



#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
