/**
*
* Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
*
* @file        config.h
*
* @author      Richard.Liu
*
* @version     v1.0.0
*
* @date        2019/01/25
*
* @brief       config source file
*
* @note
*              2019/01/25, Richard.Liu, v1.0.0
*                  Initial version.
*
**/
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



#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
