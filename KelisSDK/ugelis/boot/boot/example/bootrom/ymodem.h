/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file            ymodem.h
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date           2016/07/19 
 *    
 * @brief           This file implements the Ymodem protocol to send/receive files.
 * 
 * @note
 *    2016/07/19, terry.wu, V1.0.0
 *        Initial version.
 */

#ifndef _YMODEM_H_
#define _YMODEM_H_

/** Ymodem protocol definition */
#define SOH                     ((uint8_t)0x01)  /** indicate 128 byte data packet */
#define STX                     ((uint8_t)0x02)  /** indicate 1024 byte data packet */
#define EOT                     ((uint8_t)0x04)  /** end of transmission */
#define ACK                     ((uint8_t)0x06)  /** acknowledge */
#define NAK                     ((uint8_t)0x15)  /** negative acknowledge */
#define CAN                     ((uint8_t)0x18)  /** two of these in succession aborts transfer */
#define C                       ((uint8_t)0x43)  /** request to send data  */
#define NEGATIVE_BYTE           ((uint8_t)0xFF)

/************ Ymodem packet in memory *************************************
 * |   0       |  1     |  2         |  3     |  4         | ... | n+4     | n+5  | n+6  | 
 * |----------------------------------------------------------------------|
 * | unused    | start  | number     | !num   | data[0] | ... | data[n] | crc0 | crc1   |
 * |----------------------------------------------------------------------|
 */
#define PACKET_DATA_INDEX       ((uint8_t)4)
#define PACKET_START_INDEX      ((uint8_t)1)
#define PACKET_NUMBER_INDEX     ((uint8_t)2)
#define PACKET_CNUMBER_INDEX    ((uint8_t)3)

#define PACKET_HEADER_SIZE      ((uint8_t)3)
#define PACKET_TRAILER_SIZE     ((uint8_t)2)
#define PACKET_SIZE             ((uint8_t)128)
#define PACKET_1K_SIZE          ((uint32_t)1024)

#endif
