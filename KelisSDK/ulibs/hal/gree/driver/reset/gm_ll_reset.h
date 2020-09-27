/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_reset.h
 *
 * @author        chris.xie
 *
 * @version       1.0.0
 *
 * @date          2016/07/04
 *
 * @brief         Reset driver
 *
 * @note
 *    2019/04/17, chris.xie, V1.0.0
 *        Initial version.
 */

#ifndef _GM_LL_RESET_H_
#define _GM_LL_RESET_H_

#include <gm_common.h>
#include <gm_soc.h>


#ifdef __cplusplus
extern "C" {
#endif

/*Reset one Periphera(IPSWRST0)*/

#define LL_RESET_IPSWRST0_MODULE_CLEAR(DEVICE, VALUE)      (IO_BIT_CLEAR((DEVICE)->IPSWRST[0], VALUE))
#define LL_RESET_IPSWRST0_MODULE(DEVICE, VALUE)      (IO_BIT_SET((DEVICE)->IPSWRST[0], VALUE))

#define LL_RESET_BKD_SW_RSTJ_MODULE_CLEAR(DEVICE, VALUE)       (IO_BIT_CLEAR((DEVICE)->RTCIOCTRL, VALUE))
#define LL_RESET_BKD_SW_RSTJ_MODULE(DEVICE, VALUE)       (IO_BIT_SET((DEVICE)->RTCIOCTRL, VALUE))

/*Reset one Periphera(IPSWRST1)*/
#define LL_RESET_IPSWRST1_MODULE(DEVICE, VALUE)      (IO_BIT_SET((DEVICE)->IPSWRST[1], VALUE))

/*Reset one Periphera(IPSWRST2)*/
#define LL_RESET_IPSWRST2_MODULE(DEVICE, VALUE)      (IO_BIT_SET((DEVICE)->IPSWRST[2], VALUE))

/*BKD Reset status*/
#define LL_RESET_BKD(DEVICE, VALUE)             (IO_BIT_GET((DEVICE)->SRS, VALUE))

/*VIN Reset Clear status*/
#define LL_RESET_VIN_CLEAR_STATUS(DEVICE, VALUE)    (IO_BIT_GET((DEVICE)->IPSWRST[0], VALUE))

#ifdef __cplusplus
}
#endif

#endif /*_GM_LL_RESET_H_*/



