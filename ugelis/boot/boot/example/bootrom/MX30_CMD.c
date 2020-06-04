/*
 * COPYRIGHT (c) 2010-2016 MACRONIX INTERNATIONAL CO., LTD
 * MX30-series Parallel Flash Low Level Driver (LLD) Sample Code
 *
 * Command functions source code.
 * This sample code provides a reference, not recommand for direct using.
 *
 * $Id: MX30_CMD.c,v 1.13 2015/10/16 06:29:53 mxclldb1 Exp $
 */

#include "MX30_CMD.h"
#include <gm_hal_dma.h>

static DMA_Handle_T g_nf_dma_handle;

/* void function to avoid compile error */

#define DMA_DEV ((DMA_Device_T*)0x40020000)
RET_CODE HAL_DMA_Init(DMA_Device_T *pDMA, DMA_Config_T *pConfig, DMA_Handle_T *hDMA)
{}
RET_CODE HAL_DMA_Start(DMA_Handle_T *hDMA, uint32_t srcAddress, uint32_t destAddress, uint32_t size)
{}
RET_CODE HAL_DMA_Abort(DMA_Handle_T *hDMA)
{}
void k_sleep(uint32_t s)
{}




/*
 * Function:     Reset_OP
 * Arguments:    None
 * Return Value: Flash_Success
 * Description:  The reset command FFh resets the read/program/erase
 *               operation and clear the status register to be E0h
 */
ReturnMsg Reset_OP( void )   //Colin>>Modified
{
    /* Send reset command */
    SendCommand( 0xFF );

    /*Send read state command*/
	SendCommand( 0x70 );
	
    /* Wait reset finish */
    //WaitTime( FLASH_TIMEOUT_VALUE );

	/* Wait Flash 0xE0,0xE0 is showed in spec */
    WaitFlashReady(NF_RESET_FLAG);
    return Flash_Success;
}

/*
 * Function:     Read_ID_OP
 * Arguments:    IdBuf -> The data buffer to store ID
 * Return Value: Flash_Busy, Flash_Success
 * Description:  The Read ID command read the manufacturer ID, device ID, 3rd and 4th ID code.
 */
ReturnMsg Read_ID_OP( uint8_t *IdBuf )  //Colin>>Checked
{
    uint8_t i;

    /* Check flash is busy or not */
    if( CheckStatus( SR6_ReadyBusy ) != READY ) return Flash_Busy;

    /* Send Read ID command */
    SendCommand( 0x90 );

    /* Send one-byte address */
    SendByteAddr( 0x00 );

    #ifdef READ_ID_5BYTE
        /*Read 5-byte ID */
        for(i=0; i<5; i++)
        {
            IdBuf[i] = VAL_NF_PIO_DR;
        }
    #else
        /* Read 4-byte ID */
        for(i=0; i<4; i=i+1)
        {
            IdBuf[i] = VAL_NF_PIO_DR;
        }
    #endif

    return Flash_Success;
}

/*
 * Function:     Status_Read_OP
 * Arguments:    StatusReg -> store status value
 * Return Value: Flash_Success
 * Description:  The status read command outputs the device status
 */
ReturnMsg Status_Read_OP( uint8_t *StatusReg )  //Colin>>Modify OK
{
    /* Send status read command */
    SendCommand( 0x70 );

    /* Read status value */
    *StatusReg = VAL_NF_PIO_DR;

    return Flash_Success;
}

/*
 * Function:     Page_Read_OP
 * Arguments:    Address -> flash address
 *               DataBuf -> data buffer to store data
 *               Length  -> the number of byte(word) to read
 * Return Value: Flash_Busy, Flash_Success
 * Description:  Read page data in sequence.
 *               Note: User can only read data in the same
 *                     page.
 */
ReturnMsg Page_Read_OP( uint32_t Address, uint8_t * DataBuf, uint32_t Length )
{
    uint32_t i;

    /* Check flash is busy or not */
    if( CheckStatus( SR6_ReadyBusy ) != READY ) return Flash_Busy;

    /* Send page read command */
    SendCommand( 0x00 );

    /* Send flash address */
    SendAddr( Address );

    /* Send page read confirmed command */
    SendCommand( 0x30 );

    /* Wait flash ready and read data in a page */
    WaitFlashReady(NF_RESET_FLAG);
	/*Send Command Again*/
	SendCommand( 0x00 );
	k_sleep(1);
	
    for( i=0; i<Length; i=i+1 ){
        DataBuf[i] = FlashRead();
    }
	
    return Flash_Success;
}



static void NFlash_DMACallback(DMA_Handle_T * hDMA)
{
	HAL_DMA_Abort(hDMA);
}

static void NF_DMAConfig(uint8_t dmaRead, DMA_Handle_T *pDMAHandle)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_ERROR;


    //__ASSERT((pDMAHandle != NULL), "");

#if 0//Evan Debug
    ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_NAND, &reqResult);            
    if (ret != RET_OK)
    {
        //fail, need return fail status
        return;
    }
#endif

    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
    dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
    dmaConfig.width = DMA_WIDTH_32BIT;
    dmaConfig.burst = 0;
    dmaConfig.priority = DMA_PRIORITY_HIGH;
    dmaConfig.alter = DMA_PRIMARY;
    dmaConfig.callback = NFlash_DMACallback;
		
    if (dmaRead) 
	{
        dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
        dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
    } 
	else 
	{
        dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
        dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
    }

    HAL_DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);
}



ReturnMsg Page_Read_OP_DMA_DMA( uint32_t Address, uint8_t * DataBuf, uint32_t Length )
{
	uint8_t nf_state;

	/* Check flash is busy or not */
	if( CheckStatus( SR6_ReadyBusy ) != READY ) return Flash_Busy;

	/* Send page read command */
	SendCommand( 0x00 );

	/* Send flash address */
	SendAddr( Address );

	/* Send page read confirmed command */
	SendCommand( 0x30 );

	/* Wait flash ready and read data in a page */
	WaitFlashReady(NF_RESET_FLAG);
	/*Send Command Again*/
	SendCommand( 0x00 );

    k_sleep(1);

	NF_DMAConfig(1,&g_nf_dma_handle);
	
	/*Set DMA Control Register*/
    HAL_DMA_Start(&g_nf_dma_handle,NF_DMA_DR, (uint32_t)DataBuf, Length/4);
	
#ifdef NF_FLASH_READ_DMA_DMA
		VAL_NF_DMA_CFG = 0x82; //Set FW_REDU_LEN:4	  DMA_LEN:2   
		VAL_NF_DMA_CTRL = 0x05;//DMA_BURST_LVL:0X00 DMA_MODE:1 --PIO  DMA_DIR:0  DMA_EN:1
#endif

	/* Check DMA_RD_FSH_FLAG = 1 */
	do{
		nf_state = VAL_NF_SR2;
	}
	while((nf_state & (0x02)) != 0x02);
	
	/*	 Clear NF_DMA_FLAG = 0, NF_ECC_FLAG = 0 	*/
	VAL_NF_SR0 = 0;
	
	return Flash_Success;
}
   
/*
 * Function:     Page_Program_OP
 * Arguments:    Address -> flash address
 *               DataBuf -> provide data to program
 *               Length  -> the number of byte(word) to program
 * Return Value: Flash_Busy, Flash_Success, Flash_ProgramFailed,
 *               Flash_OperationTimeOut
 * Description:  Program data in one page.
 */
ReturnMsg Page_Program_OP( uint32_t Address, uint8_t * DataBuf, uint32_t Length )
{
    uint32_t i;
    uint8_t status;

    /* Check flash is busy or not */
    if( CheckStatus( SR6_ReadyBusy ) != READY ) return Flash_Busy;

    /* Send page program command */
    SendCommand( 0x80 );

    /* Send flash address */
    SendAddr( Address );

    /* Send data to program */

    for( i=0; i<Length; i=i+1 ){
   	 	FlashWrite( DataBuf[i] );
    }


    /* Send page program confirm command */
    SendCommand( 0x10 );

    /* Wait page program finish */
    WaitFlashReady(SR6_ReadyBusy);
    /* Check program result */
    Status_Read_OP( &status );
    if( (status & SR0_ChipStatus ) == SR0_ChipStatus )
        return Flash_ProgramFailed;
    else
        return Flash_Success;

}

ReturnMsg Page_Program_OP_DMA_DMA( uint32_t Address, uint8_t * DataBuf, uint32_t Length )
{
    uint8_t status;
	uint8_t nf_state;

    /* Check flash is busy or not */
    if( CheckStatus( SR6_ReadyBusy ) != READY ) return Flash_Busy;

    /* Send page program command */
    VAL_NF_CR     =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0 	
	
    /* Send commmand data */
    VAL_NF_PIO_DR =    0x80;

    /* Send flash address */
    VAL_NF_CR     =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0   	
	SendAddr( Address );
	
	VAL_NF_CR     =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0 	
    VAL_NF_CLE     =   0x0;

	NF_DMAConfig(0,&g_nf_dma_handle);

#if 0
	HAL_DMA_Start(&g_nf_dma_handle,DataBuf,NF_DMA_DR,Length);
#else
	HAL_DMA_Start(&g_nf_dma_handle,(uint32_t)DataBuf,NF_DMA_DR,Length/4);
#endif
    #ifdef NF_FLASH_WRITE_DMA_DMA
		VAL_NF_DMA_CFG = 0x82; //Set FW_REDU_LEN:4	  DMA_LEN:2   
		VAL_NF_DMA_CTRL = 0x07;//BURST_ACCESS_LVL:0 DMA_MODE:1 --PIO  DMA_DIR:1  DMA_EN:1
	#endif
	k_sleep(1);
    /* Check DMA_WR_FSH_FLAG = 1 */
	do{
		nf_state = VAL_NF_SR2;
	}
	while((nf_state & (0x01)) != 0x01);

	/*	 Check	NF_DMA_FLAG = 1	*/
	do{
			nf_state = VAL_NF_SR0;
	}
	while((nf_state & (0x80)) != 0x80);
		
    /*	 Clear	NF_DMA_FLAG  */
	VAL_NF_SR0 = 0x00;//VAL_NF_SR0 & (0x7F);

    /* Send page program confirm command */
    SendCommand( 0x10 );
	k_sleep(1);

    /* Wait page program finish */
    WaitFlashReady(NF_RESET_FLAG);
	
    /* Check program result */
    Status_Read_OP( &status );
    if( (status & SR0_ChipStatus ) == SR0_ChipStatus )
        return Flash_ProgramFailed;
    else
        return Flash_Success;

}


/*
 * Function:     Block_Erase_OP
 * Arguments:    Address -> flash address
 * Return Value: Flash_Busy, Flash_EraseFailed,
 *               Flash_Success, Flash_OperationTimeOut
 * Description:  Erase data in a block
 */
ReturnMsg Block_Erase_OP( uint32_t Address )//Colin>>checked
{
    uint8_t status;

    /* Check flash is busy or not */
    if( CheckStatus( SR6_ReadyBusy ) != READY ) return Flash_Busy;

    /* Send block erase command */
    SendCommand( 0x60 );

    /* Send block address (2 or 3 bytes) */
#if ( FLASH_SIZE <= 0x8000000 )
       SendByteAddr( (Address >> BYTE2_OFFSET) & BYTE_MASK );
       SendByteAddr( (Address >> BYTE3_OFFSET) & BYTE_MASK );
#else
       SendByteAddr( (Address >> BYTE2_OFFSET) & BYTE_MASK );
       SendByteAddr( (Address >> BYTE3_OFFSET) & BYTE_MASK );
       SendByteAddr( (Address >> BYTE4_OFFSET) & BYTE_MASK );
#endif

    /* Send random read confirmed command */
    SendCommand( 0xD0 );

    /* Wait block erase finish */
    WaitFlashReady(SR6_ReadyBusy);
    /* Check erase result */
    Status_Read_OP( &status );
    if( (status & SR0_ChipStatus ) == SR0_ChipStatus )
        return Flash_EraseFailed;
    else
        return Flash_Success;
   
}

/*
 * Utility Function
 */
/*
 * Function:       WaitTime
 * Arguments:      TimeValue -> The time to wait in us
 * Return Value:   READY, TIMEOUT
 * Description:    Wait flash device for a specfic time
 */
void WaitTime( uint32_t TimeValue )  //Colin>>Modified 
{   
    //printf("Error: If printf this WaitTime(),because we needn't use\n");
    FlashInfo flash_info;
    flash_info.Tus = TimeValue;
    Set_Timer( &flash_info );
    Wait_Timer( &flash_info );
}


/*
 * Function:       WaitFlashReady
 * Arguments:      None
 * Return Value:   READY
 * Description:    Wait flash device until time-out.
 *                 Check SR6_ReadyBusy of status register.
 */
void WaitFlashReady( uint8_t check_value )   //Colin>>Modified
{
    uint8_t flag;
	flag = CheckStatus(check_value);
	while( !flag)
	{
		flag = CheckStatus(check_value);
	}
}


/*
 * Function:     CheckStatus
 * Arguments:    CheckFlag -> the status bit to check
 * Return Value: READY, BUSY
 * Description:  Check status register bit 7 ~ bit 0
 */
BOOL CheckStatus( uint8_t CheckFlag )  //Colin>>checked
{
    uint8_t status;
    Status_Read_OP( &status );
    if( (status & CheckFlag) == CheckFlag )
        return TRUE;
    else
        return FALSE;
}



#if 0 //def Timer_8051

#else
void Set_Timer( FlashInfo *fptr  )
{
    fptr->Timeout_Value = (fptr->Tus)/COUNT_ONE_MICROSECOND;
    fptr->Timer_counter = 0;
}

BOOL Check_Timer( FlashInfo *fptr  )
{
    if( fptr->Timer_counter < fptr->Timeout_Value ){
        fptr->Timer_counter = fptr->Timer_counter + 1;
        return TIMENOTOUT;
    }else{
        return TIMEOUT;
    }
}

void Wait_Timer( FlashInfo *fptr  )
{
    /* Wait timer until time-out */
    while( fptr->Timer_counter < fptr->Timeout_Value ){
        fptr->Timer_counter = fptr->Timer_counter + 1;
    }
}

#endif  /* End Timer_8051 */
/*
 * Basic function
 */

/*
 * Function:     InitFlash
 * Arguments:    None.
 * Return Value: None.
 * Description:  Initial MX30 flash device
 */
void InitFlash( void )
{
	VAL_NF_MR		  = 0x01;    
	VAL_NF_RD_TIM	  = 0x24;   
	VAL_NF_WR_TIM     = 0x22; 
	VAL_NF_EDO_RB_TIM = 0x04;   
	VAL_NF_DMA_CTRL	  = 0x00;   
	VAL_NF_ECC_SR	  = 0x04;   
	VAL_NF_SR0		  = 0x00;
}

/*
 * Function:     SendCommand
 * Arguments:    CMD_code -> The MX30 flash command code
 * Return Value: None.
 * Description:  Send flash command
 */
void SendCommand( uint8_t CMD_code )//Colin>>Modified OK
{

    /* Enable command latch signal */
    //CLE_HIGH();
	VAL_NF_CR     =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0 	
	
    /* Send commmand data */
    VAL_NF_PIO_DR =    CMD_code;

    /* Disable command latch signal */
    VAL_NF_CLE     =   0x0;
}

/*
 * Function:     SendByteAddr
 * Arguments:    Byte_addr -> one byte address
 * Return Value: None.
 * Description:  Send one byte address
 */
void SendByteAddr( uint8_t Byte_addr )//Colin>>Modified OK
{
    /* Enable address latch signal */
	VAL_NF_CR	  =   0x00000100;	 //WP:0, CLE:0, ALE:1, CEJ:0	

    /* Send address data */
    VAL_NF_PIO_DR =	Byte_addr;

    /* Disable address latch signal */
    VAL_NF_ALE	=	0x0 ;

}

/*
 * Function:     SendAddr
 * Arguments:    Address -> 4(5) byte address
 * Return Value: None.
 * Description:  Send 4(5) byte address
 */
void SendAddr( uint32_t Address )//Colin>>Modified OK
{
	uint32_t page_addr = 0;
	
    /* Enable address latch signal */
    VAL_NF_CR     =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0     
         

    /* Send four byte address data */     
	page_addr = Address/PAGE_SIZE;
#if (FLASH_SIZE <= 0x8000000)
    VAL_NF_PIO_DR = 0;//Column 1;//( (Address >> BYTE0_OFFSET) & BYTE_MASK );
    VAL_NF_PIO_DR = 0;//Column 2;//( (Address >> BYTE1_OFFSET) & BYTE1_MASK );
    VAL_NF_PIO_DR = ( (page_addr >> BYTE0_OFFSET) & BYTE_MASK );
    VAL_NF_PIO_DR = ( (page_addr >> BYTE1_OFFSET) & BYTE_MASK );
#else

    VAL_NF_PIO_DR = 0;//Column 1;//( (Address >> BYTE0_OFFSET) & BYTE_MASK );
    VAL_NF_PIO_DR = 0;//Column 2;//( (Address >> BYTE1_OFFSET) & BYTE1_MASK );
    VAL_NF_PIO_DR = ( (page_addr >> BYTE0_OFFSET) & BYTE_MASK );
    VAL_NF_PIO_DR = ( (page_addr >> BYTE1_OFFSET) & BYTE_MASK );
    VAL_NF_PIO_DR = ( (page_addr >> BYTE2_OFFSET) & BYTE_MASK );

#endif
    /* Disable address latch signal */
     VAL_NF_ALE    =   0x0 ;
}

/*
 * Function:     FlashWrite
 * Arguments:    Value -> data to write
 * Return Value: None.
 * Description:  Write a data to flash
 */
void FlashWrite( uint8_t Value )
{
	VAL_NF_PIO_DR = Value;
}

/*
 * Function:     FlashRead
 * Arguments:    None
 * Return Value: Read data
 * Description:  Read one byte data from flash
 */
uint8_t FlashRead( void )
{
    uint8_t ret;
    ret = VAL_NF_PIO_DR;
	return ret;
}

/*
 * Function:     FlashReadDmaPio
 * Arguments:    None
 * Return Value: Read data by DMA PIO Mode
 * Description:  Read a DWORD data from flash
 */

uint32_t FlashReadDmaPio( void )
{
    uint32_t ret;
    ret = VAL_NF_DMA_DR;
	return ret;
}
