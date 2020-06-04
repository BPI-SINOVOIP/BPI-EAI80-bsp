/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          basic_types.h
 *
 * @author        wen.liu
 *
 * @version       1.0.0
 *
 * @date          2015/08/21
 *
 * @brief         Basic types definition for all source files.
 *
 * @note
 *    2015/08/21, wen.liu, V1.0.0
 *        Initial version.
 */
#ifndef __BASIC_TYPES_H__
#define __BASIC_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup BOOLEAN_VALUE
 * Specify boolean values commonly used
 * @{
 */
#ifdef  FALSE
#undef  FALSE
#endif
#define FALSE   (0)

#ifdef  TRUE
#undef  TRUE
#endif
#define TRUE    (1)

#ifdef  NULL
#undef  NULL
#endif
#define NULL    (0)

#ifdef  ON
#undef  ON
#endif
#define ON      (1)

#ifdef  OFF
#undef  OFF
#endif
#define OFF     (0)

#undef  ENABLE
#define ENABLE  (1)

#undef  DISABLE
#define DISABLE  (0)
/**
 * @}
 */

typedef unsigned long           ulong_t;

typedef volatile char           vint8_t;  /*  8 bits */
typedef volatile short int      vint16_t; /* 16 bits */
typedef volatile int            vint32_t; /* 32 bits */

typedef volatile unsigned char          vuint8_t;  /*  8 bits */
typedef volatile unsigned short int     vuint16_t; /* 16 bits */
typedef volatile unsigned long int      vuint32_t; /* 32 bits */


typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;


/**
 * @defgroup RET_CODE
 * Specify general return value
 * @{
 */
typedef int     RET_CODE; /* 32 bits */
#define RET_OK       0x00  /**< Function call returns succesfully  */
#define RET_ERROR    0x01  /**< Function fails because of some error condition  */
#define RET_BUSY     0x02  /**< Function fails because device is busy */
#define RET_TIMEOUT  0x03  /**< Function fails because transaction is not over within specified time */
#define RET_INVPARAM 0x04  /**< Function fails because parameter is invalid */
#define RET_ONGOING  0x05  /**< Non-blocking function returns if transaction is on progress and not finished */
/**
 * @}
 */

/**
 * @defgroup TIMEOUT_PARAMETER
 * Specify general timeout parameter of blocking and non-blocking function
 * @{
 */
#define TIMEOUT_NO_WAIT      0x00000000 /**< Transaction is over after function returns, it returns RET_ERROR if transaction is not finished. */
#define TIMEOUT_WAIT_FOREVER 0xFFFFFFFF /**< Function is about to block till transaction is over */
#define TIMEOUT_NO_BLOCKING  0xFFFFFFFE /**< Function is never blocked, user will be notified by callback after transaction is over */
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif  /* __BASIC_TYPES_H__ */

