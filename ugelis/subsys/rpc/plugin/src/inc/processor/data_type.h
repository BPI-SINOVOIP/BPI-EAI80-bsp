/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        data_type.h
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2019/04/03
 *
 * @brief       DateType Header For easynet
 *
 * @note
 *              2019/04/03, Angrad.Yang, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#include <stdint.h>

#ifdef FIX_MATH
    typedef int64_t data_t;
    typedef int16_t dat_t;
    #define FLT_MAX 0x7FFF
#else
    typedef float data_t;
    typedef float dat_t;
    #define FLT_MAX 3.402823466e+38F
#endif

/** Misc. Defines */

#ifdef	FALSE
#undef	FALSE
#endif
#define FALSE	(0)

#ifdef	TRUE
#undef	TRUE
#endif
#define	TRUE	(1)

#ifdef	NULL
#undef	NULL
#endif
#define NULL	(0)

/**
 * @defgroup RET_CODE  
 * Specify general return value  
 * @{
 */
typedef int		RET_CODE; /* 32 bits */
#define RET_SUCCESS  0x00  /**< Function call returns succesfully  */
#define RET_ERROR    0x01  /**< Function fails because of some error condition  */
#define RET_BUSY     0x02  /**< Function fails because device is busy */
#define RET_TIMEOUT  0x03  /**< Function fails because transaction is not over within specified time */
#define RET_INVPARAM 0x04  /**< Function fails because parameter is invalid */
#define RET_ONGOING  0x05  /**< Non-blocking function returns if transaction is on progress and not finished */ 
#define RET_OVER     0x06  /**< The whole processing is finished now */
/**
 * @}
 */

#endif
