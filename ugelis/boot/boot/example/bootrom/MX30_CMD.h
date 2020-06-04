/*
 * COPYRIGHT (c) 2010-2016 MACRONIX INTERNATIONAL CO., LTD
 * MX30-series Parallel Flash Low Level Driver (LLD) Sample Code
 *
 * Function prototype and related definitions.
 * This sample code provides a reference, not recommand for directing using.
 *
 * $Id: MX30_CMD.h,v 1.11 2015/10/16 06:30:52 mxclldb1 Exp $
 */

#ifndef __MX30_CMD_H__
#define __MX30_CMD_H__
//#include <ugelis.h>

typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;

#include "MX30_DEF.h"


#define PAGE_SIZE 2048//except Spare size
#define SECT_SIZE 1024//512

/* Return Message */
typedef enum{
    Flash_Success,
    Flash_Busy,
    Flash_OperationTimeOut,
    Flash_ProgramFailed,
    Flash_EraseFailed,
    Flash_ReadIDFailed,
    Flash_CmdInvalid,
    Flash_DataInvalid,
    Flash_AddrInvalid
}ReturnMsg;

/* Two Plane Cache Read Mode */
typedef enum{
   Cache_SeqRead_Mode,
   Cache_RandRead_Mode,
   LastPage_Mode
}ReadMode;

/* Flash Parameter Structure */
struct fps{
    uint8_t *Array;
    uint32_t Address;
    uint32_t Length;
};

typedef struct fps FlashParameter;

/* Flash Information */
struct fi{
    /* Timer Variable */
    uint32_t  Tus;        // time-out length in us
#if 0//def Timer_8051
    uint32_t  TEXT0;      // extend timer
#else
    uint32_t  Timeout_Value;
    uint32_t  Timer_counter;
#endif
};

typedef struct fi FlashInfo;

/* Basic Function */
void InitFlash( void );
void SendCommand( uint8_t CMD_code );
void SendByteAddr( uint8_t Byte_addr );
void SendAddr( uint32_t Address );
void SendAddrTrad( uint32_t Address, BOOL LastPlane );
void FlashWrite( uint8_t Value );
uint8_t FlashRead( void );


/* Utility Function */
void WaitTime( uint32_t TimeValue );
void WaitFlashReady( uint8_t check_value );
BOOL CheckStatus( uint8_t CheckFlag );
void Set_Timer( FlashInfo *fptr );
BOOL Check_Timer( FlashInfo *fptr );
void Wait_Timer( FlashInfo *fptr );
ReturnMsg Page_Read_OP_DMA_DMA( uint32_t Address, uint8_t * DataBuf, uint32_t Length );
ReturnMsg Page_Program_OP_DMA_DMA( uint32_t Address, uint8_t * DataBuf, uint32_t Length );
/* Flash Command */
ReturnMsg Reset_OP( void );
ReturnMsg Read_ID_OP( uint8_t *ID_BUF );
ReturnMsg Status_Read_OP( uint8_t *StatusReg );

ReturnMsg Block_Erase_OP( uint32_t Address );



#endif  /* __MX30_CMD_H__ */
