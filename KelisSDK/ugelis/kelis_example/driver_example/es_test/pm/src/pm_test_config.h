/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */



#ifndef _PM_TEST_CONFIG_H_
#define _PM_TEST_CONFIG_H_



#ifdef __cplusplus
extern "C" {
#endif


//#define PM_NORMAL_TEST   1
#define PM_SWITECH_TEST  1



#ifdef PM_SWITECH_TEST
#define PM_SWITCH_GO_ON_FUN_NUM 3
#define PM_SWITCH_RESET_FUN_NUM 2
#define PM_SWITCH_FUN_NUM (PM_SWITCH_GO_ON_FUN_NUM+PM_SWITCH_RESET_FUN_NUM)
#define HOSC_ENABLE 1
//  #define LOCS_ENABLE 1
#endif


#ifdef PM_NORMAL_TEST

/*sleep mode: cpu not rest*/
//#define CONFIG_TEST_HIRC_SLEEP_MODE_DAM_WAKEUP                      1
//#define CONFIG_TEST_KDPPLL_SLEEP_MODE_DAM_WAKEUP                    1
//#define CONFIG_TEST_HIRC_SLEEP_MODE_RTC_WAKEUP                      1
//#define CONFIG_TEST_KDPPLL_SLEEP_MODE_RTC_WAKEUP                    1


/*stop1 mode: cpu not rest*/
//#define CONFIG_TEST_HIRC_STOP1_MODE_RTC_WAKEUP                       1
//#define CONFIG_TEST_KDPPLL_STOP1_MODE_RTC_WAKEUP                     1


/*stop2 mode: cpu not rest : wkpin:xpA0*/

//#define CONFIG_TEST_HIRC_STOP2_MODE_WAKPIN_WAKEUP                    1
//#define CONFIG_TEST_KDPPLL_STOP2_MODE_WAKPIN_WAKEUP                  1

//#define CONFIG_TEST_HIRC_STOP2_MODE_RTC_WAKEUP                       1
//#define CONFIG_TEST_KDPPLL_STOP2_MODE_RTC_WAKEUP                     1

//#define CONFIG_TEST_HIRC_STOP2_MODE_PES_WAKEUP                         1
//#define CONFIG_TEST_KDPPLL_STOP2_MODE_PES_WAKEUP                     1

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*standby mode:cpu rest: wkpin:xpA0*/
//#define CONFIG_TEST_STANDBY_MODE_WAKPIN_WAKEUP                        1
//#define CONFIG_TEST_STANDBY_MODE_RTC_WAKEUP                           1
//#define CONFIG_TEST_STANDBY_MODE_IWDG_WAKEUP                          1
//#define CONFIG_TEST_STANDBY_MODE_XRSTJ_WAKEUP                         1       // resume status err:0x41


/*shutdown mode: cpu rest : wkpin:xpA7 wkpin:xpA8*/


//#define CONFIG_TEST_SHUTDOWN_MODE_A7_WAKEUP                           1    //resume status err:  0x51
//#define CONFIG_TEST_SHUTDOWN_MODE_A8_WAKEUP                           1   //resume status err:  0x51
//#define CONFIG_TEST_SHUTDOWN_MODE_RTC_WAKEUP                          1
//#define CONFIG_TEST_SHUTDOWN_MODE_IWDG_WAKEUP                         1
//#define CONFIG_TEST_SHUTDOWN_MODE_XRSTJ_WAKEUP                        1          //resume status err  0x41

/*exter clk config*/
//#define HOSC_ENABLE 1
#define LOCS_ENABLE 1


#endif











#ifdef __cplusplus
}
#endif

#endif





