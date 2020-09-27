//#include <gm_soc.h>
#include <kernel.h>
#include <gm_hal_nf.h>
#include "gm_ll_nf.h"
#include "gm_hal_nf_cmd.h"



#define NF_FLASH_WRITE_DMA_PIO    1
#define NF_FLASH_WRITE_DMA_DMA    1
#define NF_FLASH_READ_DMA_DMA     1
#define READ_ID_5BYTE             1

void nf_clearflag()
{
	(*(volatile uint32_t*)(0x70000054)) &= !(0x000000ff);
	(*(volatile uint32_t*)(0x70000050)) &= !(0x000000ff);
}



static DMA_Handle_T g_nf_dma_handle;
FlashReturnMsg HAL_NFlash_Init(NFlash_Device_T *pNFlashDev)
{
	if(NULL == pNFlashDev)
	{
		return Flash_Error;
	}
	LL_NF_MR_CLEAR(pNFlashDev);
	LL_NF_CTRL_ENABLE(pNFlashDev);
	IO_BITMASK_SET(pNFlashDev->RDTIMR,BITS8_MASK,0x24);
	IO_BITMASK_SET(pNFlashDev->WRTIMR,BITS8_MASK,0X22);
	IO_BITMASK_SET(pNFlashDev->EDORDTIMR,BITS8_MASK,0X04);
	IO_BITMASK_SET(pNFlashDev->DMACR,BITS16_MASK,0x0000);
	IO_BITMASK_SET(pNFlashDev->ECCSR,BITS8_MASK,0X04);
	IO_BITMASK_SET(pNFlashDev->SR,BITS8_MASK,0X00);

	return Flash_Success;
}

/** 
* @brief  Send  Command Code to NF controller
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param  cmd_code          The command code will be sent
* 
* @retval void 
*/
static void HAL_NFlash_Send_Cmd( NFlash_Device_T *pNFlashDev,uint8_t cmd_code )
{
    /* Enable command latch signal */
    LL_NF_CR_CLEAR(pNFlashDev);
    LL_NF_CLE_ENABLE(pNFlashDev); //WP:0, CLE:1, ALE:0, CEJ:0 	
    
    /* Send commmand data */
    LL_NF_DIN_SET(pNFlashDev,cmd_code);
	k_sleep(1);
	
    /* Disable command latch signal */
    LL_NF_CLE_DISABLE(pNFlashDev);
}

/** 
* @brief  Send One Byte Address to NF controller
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param  byte_addr          The One Byte Address will be sent
* 
* @retval void
*/
static void HAL_NFlash_Send_Byte_Addr( NFlash_Device_T *pNFlashDev,uint8_t byte_addr )//Colin>>Modified OK
{
    /* Enable address latch signal */
    LL_NF_CR_CLEAR(pNFlashDev);
    LL_NF_ALE_ENABLE(pNFlashDev);//WP:0, CLE:0, ALE:1, CEJ:0	
	
    /* Send address data */
    LL_NF_DIN_SET(pNFlashDev,byte_addr);

    /* Disable address latch signal */
    LL_NF_ALE_DISABLE(pNFlashDev);
}
/** 
* @brief   Send  4(5) bytes Address to NF controller
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param  addr         The One Byte Address will be sent
* 
* @retval void 
*/

static void HAL_NFlash_Send_Addr( NFlash_Device_T *pNFlashDev,uint32_t addr )//Colin>>Modified OK
{
	uint32_t row_addr = 0;
	uint32_t col_addr = 0;

    /* Enable address latch signal */
    LL_NF_CR_CLEAR(pNFlashDev);
    LL_NF_ALE_ENABLE(pNFlashDev);//WP:0, CLE:0, ALE:1, CEJ:0	
		 

	/* Send four or five bytes address data */
	col_addr = addr%PAGE_SIZE;
	row_addr = addr/PAGE_SIZE;
	
#if (FLASH_SIZE <= 0x8000000)
	LL_NF_DIN_SET(pNFlashDev,(col_addr >> BYTE0_OFFSET) & BYTE_MASK);
	LL_NF_DIN_SET(pNFlashDev,(col_addr >> BYTE1_OFFSET) & BYTE_MASK);
	LL_NF_DIN_SET(pNFlashDev,(row_addr >> BYTE0_OFFSET) & BYTE_MASK);
	LL_NF_DIN_SET(pNFlashDev,(row_addr >> BYTE1_OFFSET) & BYTE_MASK);
	k_sleep(1);
//	LL_NF_DIN_SET(pNFlashDev,(row_addr >> BYTE2_OFFSET) & BYTE_MASK);
#else
	LL_NF_DIN_SET(pNFlashDev,(col_addr >> BYTE0_OFFSET) & BYTE_MASK);
	LL_NF_DIN_SET(pNFlashDev,(col_addr >> BYTE1_OFFSET) & BYTE_MASK);
	LL_NF_DIN_SET(pNFlashDev,(row_addr >> BYTE0_OFFSET) & BYTE_MASK);
	LL_NF_DIN_SET(pNFlashDev,(row_addr >> BYTE1_OFFSET) & BYTE_MASK);
	LL_NF_DIN_SET(pNFlashDev,(row_addr >> BYTE2_OFFSET) & BYTE_MASK);
	k_sleep(1);
#endif

	/* Disable address latch signal */
	LL_NF_CLE_ENABLE(pNFlashDev);	//damon add
	k_sleep(1);
	LL_NF_CLE_DISABLE(pNFlashDev);	//damon add
	LL_NF_ALE_DISABLE(pNFlashDev);
}


/** 
* @brief   Get the device status by using status read cmd
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param  status_reg          The value of the device status 
* 
* @retval void 
*/

static void HAL_NFlash_Status_Read( NFlash_Device_T * pNFlashDev,uint8_t *status_reg)
{
    /* Send status read command */
    HAL_NFlash_Send_Cmd(pNFlashDev,NF_STATUS_RD_CMD);

    /* Read status value */
    *status_reg = (uint8_t)LL_NF_DIN_GET(pNFlashDev);
}

/** 
* @brief    Check status register bit 7 ~ bit 0
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param  check_flag          The status bit to check
* 
* @retval BOOL 
*/
static BOOL HAL_NFlash_Check_Status( NFlash_Device_T * pNFlashDev,uint8_t check_flag)  //Colin>>checked
{
    uint8_t status;
    HAL_NFlash_Status_Read(pNFlashDev,&status);
    if( (status & check_flag) == check_flag )
        return TRUE;
    else
        return FALSE;
}

/** 
* @brief     Wait Flash Ready before operating the Nand Flash
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param  check_value          The status bit to be checked
*
* @retval void 
*/
static void HAL_NFlash_Wait_Flash_Ready( NFlash_Device_T * pNFlashDev,uint8_t check_value)
{
	uint8_t flag;

	flag = HAL_NFlash_Check_Status(pNFlashDev,check_value);
	while( !flag)
	{
		flag = HAL_NFlash_Check_Status(pNFlashDev,check_value);
	}
}

/** 
* @brief     Write one byte data to the Nand Flash
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param  byte_data          The one byte data will be writen to Flash
* 
* @retval void 
*/
static void HAL_NFlash_Byte_Write( NFlash_Device_T * pNFlashDev,uint8_t byte_data )
{
	LL_NF_DIN_SET(pNFlashDev,byte_data);
}

/** 
* @brief     Read one byte data from the Nand Flash
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @retval uint8_t 
*/
static uint8_t HAL_NFlash_Byte_Read( NFlash_Device_T * pNFlashDev )
{
    uint8_t ret;
    ret = (uint8_t)(pNFlashDev->DR);
	return ret;
}

/** 
* @brief     Write DWORD data to the Nand Flash
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param  data          The one byte data will be writen to Flash
* 
* @retval void 
*/

static void HAL_NFlash_Write_Dma_PIO( NFlash_Device_T * pNFlashDev,uint32_t data )
{
	LL_NF_DMA_DIN_SET(pNFlashDev,data);
}


/** 
* @brief     Read DWORD data from the Nand Flash
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @retval uint32_t 
*/
static uint32_t HAL_NFlash_Read_Dma_PIO( NFlash_Device_T * pNFlashDev )
{
    uint32_t ret;
    ret = pNFlashDev->DMADR;
	return ret;
}


//The codes below may change the position//Colin

/** 
* @brief     Resets the read/program/erase operation and clear the status register to be 0xE0 or 0xA0
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @retval FlashReturnMsg 
*/

FlashReturnMsg HAL_NFlash_Reset( NFlash_Device_T * pNFlashDev ) 
{
	if(NULL == pNFlashDev)
	{
		return Flash_Error;
	}
    /* Send reset command */
    HAL_NFlash_Send_Cmd(pNFlashDev,NF_RESET_CMD);

    /*Send read state command*/
	HAL_NFlash_Send_Cmd(pNFlashDev,NF_STATUS_RD_CMD);
	
    /* Wait reset finish */
    //WaitTime( FLASH_TIMEOUT_VALUE );

	/* Wait Flash 0xE0 or 0xA0, showed in spec */
    HAL_NFlash_Wait_Flash_Ready(pNFlashDev,NF_RESET_FLAG);
    return Flash_Success;
}

/** 
* @brief     Resets the read/program/erase operation and clear the status register to be 0xE0 or 0xA0
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param  id_buf                The buffer is used to store ID
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Read_ID( NFlash_Device_T * pNFlashDev, uint8_t *id_buf )
{
    uint8_t i;

	if(NULL == pNFlashDev)
	{
		return Flash_Error;
	}

    /* Check flash is busy or not */
    if(HAL_NFlash_Check_Status( pNFlashDev,SR6_READY_BUSY) != NF_READY )
    {
		return Flash_Busy;
    }

    /* Send Read ID command */
    HAL_NFlash_Send_Cmd(pNFlashDev,NF_ID_READ_CMD);

    /* Send one-byte address */
    HAL_NFlash_Send_Byte_Addr(pNFlashDev,0x00);
	k_sleep(1);

    #ifdef READ_ID_5BYTE
        /*Read 5-byte ID */
        for(i=0; i<5; i++)
        {
            id_buf[i] = HAL_NFlash_Byte_Read(pNFlashDev);
        }
    #else
        /* Read 4-byte ID */
        for(i=0; i<4; i=i+1)
        {
            id_buf[i] = HAL_NFlash_Byte_Read(pNFlashDev);
        }
    #endif

    return Flash_Success;
}

/** 
* @brief     Config the Read Page Command before  Read  data from Nand Flash
*               Note: User can only read data in the same
*                        page.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @retval FlashReturnMsg 
*/

static FlashReturnMsg HAL_NFlash_Page_Read_CMD( NFlash_Device_T * pNFlashDev,uint32_t addr)
{
    if(HAL_NFlash_Check_Status( pNFlashDev,SR6_READY_BUSY) != NF_READY ) 
	{
		return Flash_Busy;
    }

    /* Send page read command */
    HAL_NFlash_Send_Cmd( pNFlashDev,0x00);

    /* Send flash address */
    HAL_NFlash_Send_Addr(pNFlashDev,addr);
	

    /* Send page read confirmed command */
    HAL_NFlash_Send_Cmd( pNFlashDev,NF_PAGE_RD_CMD);
	

    /* Wait flash ready and read data in a page */
//	HAL_NFlash_Wait_Flash_Ready(pNFlashDev, 0xc0);
    HAL_NFlash_Wait_Flash_Ready(pNFlashDev, NF_RESET_FLAG);
	
	/*Send Command Again*/
    HAL_NFlash_Send_Cmd( pNFlashDev,0x00);
    
	k_sleep(1);

	return Flash_Success;

}

/** 
* @brief     Read page data without ECC by CPU PIO mode 
*               Note: User can only read data in the same
*                        page.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to store data
*
* @param   length              the data length will be read
*
* @retval FlashReturnMsg 
*/

FlashReturnMsg HAL_NFlash_Page_Read( NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length )
{
    uint32_t i;
	uint32_t page_remain_len;

	/*disable DMA*/
	LL_NF_DMA_MODE_DISABLE(pNFlashDev);
 
    if((NULL == pNFlashDev) || (HAL_NFlash_Page_Read_CMD(pNFlashDev,addr) != Flash_Success))
    {
    	return Flash_Error;
    }

	page_remain_len = PAGE_SIZE - (addr%PAGE_SIZE);

	if (length > page_remain_len)
	{
		length = page_remain_len;
		//printf("Warning!!! %s(): The length is too large to read");
	}

	/*Begin to Read Page Data From */
    for( i=0; i<length; i=i+1 ){
        data_buf[i] = HAL_NFlash_Byte_Read(pNFlashDev);
    }
	
    return Flash_Success;
}

/** 
* @brief     Read page data with ECC by DMA PIO mode 
*               Note: User can only read data in the same
*                        page.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to store data
*
* @param   length              the data length will be read
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Page_Read_DMA_PIO(NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length )
{
    uint32_t i,j;
	uint32_t length_leave = length;
	uint8_t nf_state;
	uint32_t temp_data;
	uint32_t sect_num;

	/*Set DMA Control Register*/
	//#ifdef NF_FLASH_READ_DMA_PIO

		LL_NF_DMA_MODE_DISABLE(pNFlashDev);//Set DMA_EN:0
		nf_clearflag();

#if 0		//damon1
		if((NULL == pNFlashDev) || (HAL_NFlash_Page_Read_CMD(pNFlashDev,addr) != Flash_Success))
    {
    	return Flash_Error;
    }

#endif		//damon1
		
	    LL_NF_FW_REDU_LEN_SET(pNFlashDev,4);//Set FW_REDU_LEN:4    
	    LL_NF_DMA_LEN_SET(pNFlashDev,2);//Set DMA_LEN:2   

		//damon add
		if((NULL == pNFlashDev) || (HAL_NFlash_Page_Read_CMD(pNFlashDev,addr) != Flash_Success))
			{
				return Flash_Error;
			}
		//damon add

		//damon delete
		//LL_NF_DMA_MODE_SET(pNFlashDev,0);//Set DMA_MODE:0 --PIO  
		LL_NF_DMA_DIR_SET(pNFlashDev,0);//Set DMA_DIR:0 
		LL_NF_DMA_MODE_ENABLE(pNFlashDev);// Set DMA_EN:1
	//#endif
	
    sect_num = 0;
    while(length_leave != 0)
	{
		do{
			nf_state = (uint8_t)((pNFlashDev->SR & BITS16_MASK)>>BYTE1_OFFSET);
		}
		while((nf_state & (0x01)) != (0x01));
        
		for( i=0; i<SECT_SIZE_ECC_MODE/sizeof(uint32_t); ++i )
		{
            temp_data = HAL_NFlash_Read_Dma_PIO(pNFlashDev);
			j = 4 * i;
		    data_buf[sect_num * SECT_SIZE_ECC_MODE + j] = temp_data & BYTE_MASK;
			data_buf[sect_num * SECT_SIZE_ECC_MODE + j+1] = (temp_data >> BYTE1_OFFSET) & BYTE_MASK;
		 	data_buf[sect_num * SECT_SIZE_ECC_MODE + j+2] = (temp_data >> BYTE2_OFFSET) & BYTE_MASK;
		 	data_buf[sect_num * SECT_SIZE_ECC_MODE + j+3] = (temp_data >> BYTE3_OFFSET) & BYTE_MASK;
		}

	    length_leave = length_leave - SECT_SIZE_ECC_MODE;
	    ++sect_num;
	}
    /*   Check  NF_DMA_FLAG = 1? NF_ECC_FLAG = 1?   */
	do{
		nf_state = (uint8_t)(pNFlashDev->SR & BITS8_MASK);
	}
	while((nf_state & (0xA0)) != 0xA0);
    
    /*   Clear NF_DMA_FLAG = 0, NF_ECC_FLAG = 0     */
	LL_NF_DMA_FLG_CLEAR(pNFlashDev);
	LL_NF_ECC_FLAG_CLEAR(pNFlashDev);
    
    return Flash_Success;
}



/** 
* @brief     Read page data with ECC  odd check by DMA PIO mode 
*               Note: User can only read data in the same
*                        page.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to store data
*
* @param   length              the data length will be read
*
* @retval FlashReturnMsg 
*/

FlashReturnMsg HAL_NFlash_Page_Read_DMA_PIO_ODD( NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length )
{
    uint32_t i,j;
	uint32_t length_leave = length;
	uint8_t nf_state;
	uint32_t temp_data;
	uint32_t sect_num;

    if((NULL == pNFlashDev) || (HAL_NFlash_Page_Read_CMD(pNFlashDev,addr) != Flash_Success))
    {
    	return Flash_Error;
    }
	
	/*Set DMA Control Register*/
		
	//#ifdef NF_FLASH_READ_DMA_PIO
		LL_NF_FW_REDU_LEN_SET(pNFlashDev,4);//Set FW_REDU_LEN:4    
		LL_NF_DMA_LEN_SET(pNFlashDev,4);//Set DMA_LEN:4  
		
		LL_NF_DMA_MODE_SET(pNFlashDev,0);//Set DMA_MODE:0 --PIO  
		LL_NF_DMA_DIR_SET(pNFlashDev,0);//Set DMA_DIR:0 
		LL_NF_DMA_MODE_ENABLE(pNFlashDev);// Set DMA_EN:1
	//#endif

	
    sect_num = 0;
    while(length_leave != 0)
	{
		do{
			nf_state = (uint8_t)((pNFlashDev->SR & BITS16_MASK)>>BYTE1_OFFSET);
		}
		while((nf_state & (0x01)) != (0x01));
        
		for( i=0; i<SECT_SIZE_UNECC_MODE/sizeof(uint32_t); ++i )
		{
            temp_data = HAL_NFlash_Read_Dma_PIO(pNFlashDev);
			j = 4 * i;
		    data_buf[sect_num * SECT_SIZE_UNECC_MODE + j] = temp_data & BYTE_MASK;
			data_buf[sect_num * SECT_SIZE_UNECC_MODE + j+1] = (temp_data >> BYTE1_OFFSET) & BYTE_MASK;
		 	data_buf[sect_num * SECT_SIZE_UNECC_MODE + j+2] = (temp_data >> BYTE2_OFFSET) & BYTE_MASK;
		 	data_buf[sect_num * SECT_SIZE_UNECC_MODE + j+3] = (temp_data >> BYTE3_OFFSET) & BYTE_MASK;
       }

	   length_leave = length_leave - SECT_SIZE_UNECC_MODE;
	   ++sect_num;
	}    

	/*   Check  NF_DMA_FLAG = 1? NF_ECC_FLAG = 1?   */
	do{
		nf_state = (uint8_t)(pNFlashDev->SR & BITS8_MASK);
	}
	while((nf_state & (0xA0)) != 0xA0);
    
    /*   Clear NF_DMA_FLAG = 0, NF_ECC_FLAG = 0     */
	LL_NF_DMA_FLG_CLEAR(pNFlashDev);
	LL_NF_ECC_FLAG_CLEAR(pNFlashDev);
	
    
    return Flash_Success;
}

static void HAL_NFlash_DMACallback(DMA_Handle_T * hDMA)
{
	HAL_DMA_Abort(hDMA);
}


static void HAL_NFlash_DMAConfig(uint8_t dmaRead, DMA_Handle_T *pDMAHandle)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_ERROR;


    __ASSERT((pDMAHandle != NULL), "");

    ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_NAND, &reqResult);            
    if (ret != RET_OK)
    {
        //fail, need return fail status
        return;
    }

    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
    dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
    dmaConfig.width = DMA_WIDTH_32BIT;
    dmaConfig.burst = 0;/*change 0 to 1*/
    dmaConfig.priority = DMA_PRIORITY_HIGH;/*change HIGH to LOW*/
    dmaConfig.alter = DMA_PRIMARY;
    dmaConfig.callback = HAL_NFlash_DMACallback;
		
    if (dmaRead) 
	{
        dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
        dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
		dmaConfig.burst = 1;
    } 
	else 
	{
        dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
        dmaConfig.increment = 0x02;/*changge DMA_SRC_INC_DST_KEEP  to 0x02*/
    }

    HAL_DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);
}

/** 
* @brief     Read page data with ECC  DMA hardware mode 
*               Note: User can only read data in the same
*                        page.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to store data
*
* @param   length              the data length will be read
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Page_Read_DMA_DMA(NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length )
{
	uint8_t nf_state;
    if((NULL == pNFlashDev) || (HAL_NFlash_Page_Read_CMD(pNFlashDev,addr) != Flash_Success))
    {
    	return Flash_Error;
    }
	
	

	
#ifdef NF_FLASH_READ_DMA_DMA
	    LL_NF_FW_REDU_LEN_SET(pNFlashDev,4);//Set FW_REDU_LEN:4    
	    LL_NF_DMA_LEN_SET(pNFlashDev,2);//Set DMA_LEN:2   
		LL_NF_DMA_MODE_SET(pNFlashDev,1);//Set DMA_MODE:1 --DMA  
		LL_NF_DMA_DIR_SET(pNFlashDev,0);//Set DMA_DIR:0 
		LL_NF_DMA_MODE_ENABLE(pNFlashDev);// Set DMA_EN:1
#endif

	HAL_NFlash_DMAConfig(1,&g_nf_dma_handle);
	
	/*Set DMA Control Register*/
    HAL_DMA_Start(&g_nf_dma_handle,&(pNFlashDev->DMADR),data_buf, length/4);

	/* Check DMA_RD_FSH_FLAG = 1 
      * Wait Reading  data finish  by DMA handshake mode
      */

    do{
		
		}
	while(!((*(volatile uint32_t*)(0x40020FF0))&0x02));

	/*   Clear NF_DMA_FLAG = 0, NF_ECC_FLAG = 0     */
	LL_NF_DMA_FLG_CLEAR(pNFlashDev);
	LL_NF_ECC_FLAG_CLEAR(pNFlashDev);
    
	return Flash_Success;
}


/** 
* @brief      Cache Read Seq/Rand function
 *               Note: User needs to execute  HAL_NFlash_Cache_Seq_Read_Another_OP()
 *                     or Cache_Read_Random_OP() to finish this command.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Cache_Seq_Rand_Read_Begin( NFlash_Device_T * pNFlashDev,uint32_t addr )//Colin>>Checked
{

	if(NULL == pNFlashDev)
	{
		return Flash_Error;
	}

    /* Check the address  is valid or invalid*/
    if (addr & PAGE_MASK)
    {
        return Flash_AddrInvalid;
    }
    /* Check flash is busy or not */
    if(HAL_NFlash_Check_Status( pNFlashDev,SR6_READY_BUSY) != NF_READY ) 
	{
		return Flash_Busy;
    }

    /* Send cache read command */
    HAL_NFlash_Send_Cmd(pNFlashDev,0x00);
    
    /* Send flash address (4 byte) */
	HAL_NFlash_Send_Addr(pNFlashDev,addr);

    /* Send cache read confirmed command */
    HAL_NFlash_Send_Cmd(pNFlashDev,0x30);

    /* Wait flash ready */
   // WaitTime( FIRST_BYTE_LATENCY_tR );//Colin>>This may have some problem
    k_sleep(25);
    return Flash_Success;

}

/** 
* @brief      read another cache buffer
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   data_buf          data buffer to store data
*
* @param   length              the data length will be read
*
* @param   last_page          Indicate the last page or not
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Cache_Seq_Read_Another_OP(NFlash_Device_T * pNFlashDev,uint8_t* data_buf, uint32_t length, BOOL last_page)
{
    uint32_t i;
	
	if(NULL == pNFlashDev)
	{
		return Flash_Error;
	}

    /* Send cache read command */
    if (last_page)	
        HAL_NFlash_Send_Cmd(pNFlashDev,0x3F);
    else	
        HAL_NFlash_Send_Cmd(pNFlashDev,0x31);

    /* Wait flash ready and read data in a cache buffer */
    //WaitTime( tRCBSY );//Colin>>This may have some problem
	k_sleep(25);
    for(i=0; i<length; i++)
        data_buf[i] = HAL_NFlash_Byte_Read(pNFlashDev);

    return Flash_Success;
}

/** 
* @brief     Config the Write Page Command before  Write  data to Nand Flash
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @retval FlashReturnMsg 
*/
static FlashReturnMsg HAL_NFlash_Page_Program_Cmd( NFlash_Device_T * pNFlashDev,uint32_t addr)
{
    /* Check flash is busy or not */
    if(HAL_NFlash_Check_Status( pNFlashDev,SR6_READY_BUSY) != NF_READY ) 
	{
		return Flash_Busy;
    }
	
    /* Send page program command */
    HAL_NFlash_Send_Cmd(pNFlashDev,NF_PAGE_WR_CMD);
	
    /* Send flash address */
	HAL_NFlash_Send_Addr(pNFlashDev,addr);
	
	return Flash_Success;
}

 
/** 
* @brief     Program data in one page.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to be writen
*
* @param   length              the data length will be writen
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Page_Program(NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length )
{
    uint32_t i;
    uint8_t status;

    if((NULL == pNFlashDev) || (HAL_NFlash_Page_Program_Cmd(pNFlashDev,addr)!= Flash_Success))
    {
    	return Flash_Error;
    }

    /* Send data to program */
    for( i=0; i<length; i=i+1 ){
   	 	HAL_NFlash_Byte_Write(pNFlashDev, data_buf[i] );
    }


    /* Send page program confirm command */
    HAL_NFlash_Send_Cmd(pNFlashDev,NF_PAGE_WR_CONFIRM_CMD);

    /* Wait page program finish */
	HAL_NFlash_Wait_Flash_Ready(pNFlashDev,SR6_READY_BUSY);
    /* Check program result */
    HAL_NFlash_Status_Read(pNFlashDev,&status );
    if( (status & SR0_CHIP_STATUS ) == SR0_CHIP_STATUS )
        return Flash_ProgramFailed;
    else
        return Flash_Success;
}

/** 
* @brief     Program data in one page by DMA PIO mode.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to be writen
*
* @param   length              the data length will be writen
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Page_Program_DMA_PIO(NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length )
{
    uint32_t i;
    uint8_t status;
	uint32_t length_leave = length;
	uint8_t nf_state;
    uint32_t temp_data;
	uint32_t sect_num;

	LL_NF_DMA_MODE_DISABLE(pNFlashDev);
	nf_clearflag();
/*
    if((NULL == pNFlashDev) || (HAL_NFlash_Page_Program_Cmd(pNFlashDev,addr)!= Flash_Success))
    {
    	return Flash_Error;
    }
*/	
    #ifdef NF_FLASH_WRITE_DMA_PIO
	    LL_NF_FW_REDU_LEN_SET(pNFlashDev,4);//Set FW_REDU_LEN:4    
	    LL_NF_DMA_LEN_SET(pNFlashDev,2);//Set DMA_LEN:2   

		//damon add begin
		if((NULL == pNFlashDev) || (HAL_NFlash_Page_Program_Cmd(pNFlashDev,addr)!= Flash_Success))
			{
				return Flash_Error;
			}
		//damon add end

		LL_NF_DMA_MODE_SET(pNFlashDev,0);//Set DMA_MODE:0 --PIO                //damon delete  
		LL_NF_DMA_DIR_SET(pNFlashDev,1);//Set DMA_DIR:1 
		LL_NF_DMA_MODE_ENABLE(pNFlashDev);// Set DMA_EN:1
	#endif
	k_sleep(1);
    sect_num = 0;

	while(length_leave != 0)
	{


		do{
				nf_state = (uint8_t)((pNFlashDev->SR & BITS16_MASK)>>BYTE1_OFFSET);
		  }
		while((nf_state & (0x02)) == 0x02);


			
        /* Send data to program */
		for( i=0; i<SECT_SIZE_ECC_MODE; i = i+4){
			temp_data = (uint32_t)data_buf[sect_num * SECT_SIZE_ECC_MODE + i+3] << BYTE3_OFFSET;
			temp_data = temp_data | ((uint32_t)data_buf[sect_num * SECT_SIZE_ECC_MODE +i+2] << BYTE2_OFFSET);
			temp_data = temp_data | ((uint32_t)data_buf[sect_num * SECT_SIZE_ECC_MODE +i+1] << BYTE1_OFFSET);
			temp_data = temp_data | ((uint32_t)data_buf[sect_num * SECT_SIZE_ECC_MODE +i]);
			HAL_NFlash_Write_Dma_PIO(pNFlashDev,temp_data);
			k_sleep(1);
	   }
	   length_leave = length_leave - SECT_SIZE_ECC_MODE;	
	   ++sect_num;
	}
	/*	 Check	NF_DMA_FLAG = 1	*/
	do{
		nf_state = (uint8_t)(pNFlashDev->SR & BITS8_MASK);
	}
	while((nf_state & (0x80)) != 0x80);
		
    /*	 Clear	NF_DMA_FLAG  */
	LL_NF_DMA_FLG_CLEAR(pNFlashDev);

    /* Send page program confirm command */
    HAL_NFlash_Send_Cmd(pNFlashDev, 0x10 );
	k_sleep(1);

    /* Wait page program finish */
    HAL_NFlash_Wait_Flash_Ready(pNFlashDev,NF_RESET_FLAG);
    /* Check program result */
    HAL_NFlash_Status_Read( pNFlashDev,&status );
    if( (status & SR0_CHIP_STATUS ) == SR0_CHIP_STATUS )
        return Flash_ProgramFailed;
    else
        return Flash_Success;

}

/** 
* @brief     Program data in one page by DMA PIO ODD mode.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to be writen
*
* @param   length              the data length will be writen
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Page_Program_DMA_PIO_ODD(NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length )
{
    uint32_t i;
    uint8_t status;
	uint32_t lengthLeave = length;
	uint8_t nf_state;
    uint32_t temp_data;
	uint32_t sect_num;
    if((NULL == pNFlashDev) || (HAL_NFlash_Page_Program_Cmd(pNFlashDev,addr)!= Flash_Success))
    {
    	return Flash_Error;
    }

    #ifdef NF_FLASH_WRITE_DMA_PIO
	    LL_NF_FW_REDU_LEN_SET(pNFlashDev,4);//Set FW_REDU_LEN:4    
	    LL_NF_DMA_LEN_SET(pNFlashDev,2);//Set DMA_LEN:2   

		LL_NF_DMA_MODE_SET(pNFlashDev,0);//Set DMA_MODE:0 --PIO  
		LL_NF_DMA_DIR_SET(pNFlashDev,1);//Set DMA_DIR:1 
		LL_NF_DMA_MODE_ENABLE(pNFlashDev);// Set DMA_EN:1
    #endif

	k_sleep(1);
    sect_num = 0;

	//Status_Read_OP( &status );
	//printf("status 0x%x\n",status);
	while(lengthLeave != 0)
	{
		do{
			nf_state = (uint8_t)((pNFlashDev->SR & BITS16_MASK)>>BYTE1_OFFSET);
		}
		while((nf_state & (0x02)) == 0x02);
			
        /* Send data to program */
		for( i=0; i<SECT_SIZE_UNECC_MODE; i = i+4){
			//FlashWrite( DataBuf[i]);
			temp_data = (uint32_t)data_buf[sect_num * SECT_SIZE_UNECC_MODE + i+3] << BYTE3_OFFSET;
			temp_data = temp_data | ((uint32_t)data_buf[sect_num * SECT_SIZE_UNECC_MODE +i+2] << BYTE2_OFFSET);
			temp_data = temp_data | ((uint32_t)data_buf[sect_num * SECT_SIZE_UNECC_MODE +i+1] << BYTE1_OFFSET);
			temp_data = temp_data | ((uint32_t)data_buf[sect_num * SECT_SIZE_UNECC_MODE +i]);
			LL_NF_DMA_DIN_SET(pNFlashDev,temp_data);
	   }
	   lengthLeave = lengthLeave - SECT_SIZE_UNECC_MODE;	
	   ++sect_num;
	}
	
	/*	 Check	NF_DMA_FLAG = 1 */
	do{
		nf_state = (uint8_t)(pNFlashDev->SR & BITS8_MASK);
	}
	while((nf_state & (0x80)) != 0x80);
		
	/*	 Clear	NF_DMA_FLAG  */
	LL_NF_DMA_FLG_CLEAR(pNFlashDev);

	/* Send page program confirm command */
	HAL_NFlash_Send_Cmd(pNFlashDev, 0x10 );
	k_sleep(1);

	/* Wait page program finish */
	HAL_NFlash_Wait_Flash_Ready(pNFlashDev,NF_RESET_FLAG);
	/* Check program result */
	HAL_NFlash_Status_Read( pNFlashDev,&status );
	if( (status & SR0_CHIP_STATUS ) == SR0_CHIP_STATUS )
		return Flash_ProgramFailed;
	else
		return Flash_Success;
}

/** 
* @brief     Program data in one page by DMA hardware handshake mode.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to be writen
*
* @param   length              the data length will be writen
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Page_Program_DMA_DMA(NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length )
{
    uint8_t status;
	uint8_t nf_state;


	LL_NF_DMA_MODE_DISABLE(pNFlashDev);

    if((NULL == pNFlashDev) || (HAL_NFlash_Page_Program_Cmd(pNFlashDev,addr)!= Flash_Success))
    {
    	return Flash_Error;
    }



    #ifdef NF_FLASH_WRITE_DMA_DMA
		LL_NF_FW_REDU_LEN_SET(pNFlashDev,4);//Set FW_REDU_LEN:4    
		LL_NF_DMA_LEN_SET(pNFlashDev,2);//Set DMA_LEN:2   
	
		LL_NF_DMA_MODE_SET(pNFlashDev,1);//Set DMA_MODE:1 --DMA  
		LL_NF_DMA_DIR_SET(pNFlashDev,1);//Set DMA_DIR:1 
		LL_NF_DMA_MODE_ENABLE(pNFlashDev);// Set DMA_EN:1
	#endif

	HAL_NFlash_DMAConfig(0,&g_nf_dma_handle);

    #if 0
		HAL_DMA_Start(&g_nf_dma_handle,data_buf,&(pNFlashDev->DMADR),length);
    #else
		//uint32_t tempBuf[16];
	HAL_DMA_Start(&g_nf_dma_handle,data_buf,&(pNFlashDev->DMADR),length/4);
	//HAL_DMA_Start(&g_nf_dma_handle,&(pNFlashDev->DMADR),data_buf,length/4);
    #endif

	
	k_sleep(1);
    /* Check DMA_WR_FSH_FLAG = 1 */
	do{
		nf_state = (uint8_t)((pNFlashDev->SR & BITS24_MASK)>>BYTE2_OFFSET);
	}
	while((nf_state & (0x01)) != 0x01);
	//NFlash_DMACallback(&g_nf_dma_handle);


	/*	 Check	NF_DMA_FLAG = 1	*/
do{
		nf_state = (uint8_t)(pNFlashDev->SR & BITS8_MASK);
	}
	while((nf_state & (0x80)) != 0x80);
		
	/*	 Clear	NF_DMA_FLAG  */
	LL_NF_DMA_FLG_CLEAR(pNFlashDev);
	
	/* Send page program confirm command */
	HAL_NFlash_Send_Cmd(pNFlashDev, 0x10 );
	k_sleep(1);

	/* Wait page program finish */
	HAL_NFlash_Wait_Flash_Ready(pNFlashDev,NF_RESET_FLAG);
	/* Check program result */
	HAL_NFlash_Status_Read( pNFlashDev,&status );
	if( (status & SR0_CHIP_STATUS ) == SR0_CHIP_STATUS )
		return Flash_ProgramFailed;
	else
		return Flash_Success;
}

/** 
* @brief     Cache program can enhances the program performance
*               by using the cache buffer. The last page needs
*               confirmed with 0x10 commad to end cache program.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to be writen
*
* @param   length              the data length will be writen
*
* @param   last_page         Indicate the last page or not   0: False, 1: True
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Cache_Program(NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length,BOOL last_page )
{
    uint32_t i;

    if(NULL == pNFlashDev)
    {
    	return Flash_Error;
    }

	/* Send page program command */
	HAL_NFlash_Send_Cmd(pNFlashDev,NF_PAGE_WR_CMD);
	
	/* Send flash address */
	HAL_NFlash_Send_Addr(pNFlashDev,addr);

    /* Send data to program */
    for( i=0; i<length; i=i+1 )
	{
		HAL_NFlash_Byte_Write(pNFlashDev,data_buf[i]);
    }

    /* Send page program confirm command */
    if( last_page )
        HAL_NFlash_Send_Cmd( pNFlashDev,0x10 );    // program last page
    else
        HAL_NFlash_Send_Cmd( pNFlashDev,0x15 );    // continue cache program

    HAL_NFlash_Wait_Flash_Ready(pNFlashDev,NF_RESET_FLAG);
    return Flash_Success;
}

/** 
* @brief     Program data randomly in one page.
*               Note: User needs to execute Page_Random_Program_Input_OP() (if need)
*                     and Page_Random_Program_End_OP() to finish this command.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to be writen
*
* @param   length              the data length will be writen
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Page_Random_Program_Begin(NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length )
{
    if((NULL == pNFlashDev) || (HAL_NFlash_Page_Program_Cmd(pNFlashDev,addr)!= Flash_Success))
	{
	    return Flash_Error;
	}
	
    k_sleep(1);
    /* Send data to program */
	
	//we used this function not to send data but just to change data
	#if 0
	uint32_t i;
    for( i=0; i<Length; i=i+1 ){
       FlashWrite( DataBuf[i] );
    }
    #endif
	
    return Flash_Success;

}

/** 
* @brief     Input data during page random program operation.
*               Note: User needs to execute Page_Random_Program_Begin_OP()
*                     command first before Page_Random_Program_Input_OP().
*                     And end with Page_Random_Program_End_OP().
*                     The number of Page_Random_Program_Input_OP()
*                     is not limited.
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @param   data_buf          data buffer to be writen
*
* @param   length              the data length will be writen
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Page_Random_Program_Input(NFlash_Device_T * pNFlashDev,uint32_t addr, uint8_t * data_buf, uint32_t length )
{
    uint32_t i;
	if(NULL == pNFlashDev)
	{
		return Flash_Error;
	}

	LL_NF_DMA_MODE_DISABLE(pNFlashDev);  

    /* Send page data input command */
    HAL_NFlash_Send_Cmd(pNFlashDev, 0x80 );/*changge 85 to 80*/

    /* Send page address (2 byte) */
 //   HAL_NFlash_Send_Byte_Addr(pNFlashDev, (addr>> BYTE0_OFFSET) & BYTE_MASK );
 //   HAL_NFlash_Send_Byte_Addr(pNFlashDev, (addr >> BYTE1_OFFSET) & BYTE1_MASK );

#if 1
	HAL_NFlash_Send_Addr(pNFlashDev,addr);   /*add*/

//	 k_sleep(1);

     HAL_NFlash_Send_Cmd(pNFlashDev, 0x85 ); /*add*/
	 LL_NF_CR_CLEAR(pNFlashDev);
     LL_NF_ALE_ENABLE(pNFlashDev);//WP:0, CLE:0, ALE:1, CEJ:0	
	 LL_NF_DIN_SET(pNFlashDev,(0 >> BYTE0_OFFSET) & BYTE_MASK);
	 LL_NF_DIN_SET(pNFlashDev,(0 >> BYTE1_OFFSET) & BYTE_MASK);
	 LL_NF_CLE_ENABLE(pNFlashDev);
	 LL_NF_CR_CLEAR(pNFlashDev);

	 
	 LL_NF_DMA_MODE_DISABLE(pNFlashDev);
//	 k_sleep(1);

	 
	 (*(volatile uint32_t*)(0x70000020)) = 0x0; 
#endif
    /* Send data to program */
   /* for( i=0; i<length; i=i+1 )
	{
		HAL_NFlash_Byte_Write(pNFlashDev,data_buf[i]);
    }
*/
    return Flash_Success;
}

/** 
* @brief      Start page program after data input is finished.
*                Note: User needs to execute Page_Random_Program_Begin_OP()
*                     and Page_Random_Program_Input_OP() (if need) commands
*                     before Page_Program_Confirm_OP().
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Page_Random_Program_End( NFlash_Device_T * pNFlashDev )
{
	if(NULL == pNFlashDev)
	{
		return Flash_Error;
	}

    /* Send page program confirm command */
    HAL_NFlash_Send_Cmd(pNFlashDev, 0x10 );
	k_sleep(1);
	
    /* Wait page program finish */
    HAL_NFlash_Wait_Flash_Ready(pNFlashDev,SR6_READY_BUSY);
    return Flash_Success;
}

/** 
* @brief     Erase data in a block
* 
* @param  pNFlashDev       The Nand FLash Device
* 
* @param   addr                flash address
*
* @retval FlashReturnMsg 
*/
FlashReturnMsg HAL_NFlash_Block_Erase(NFlash_Device_T * pNFlashDev, uint32_t addr )
{
    uint8_t status;
	if(NULL == pNFlashDev)
	{
		return Flash_Error;
	}

    /* Check flash is busy or not */
	if(HAL_NFlash_Check_Status( pNFlashDev,SR6_READY_BUSY) != NF_READY ) 
   	{
	    return Flash_Busy;
    }
    
    /* Send block erase command */
    HAL_NFlash_Send_Cmd(pNFlashDev,0x60 );

    /* Send block address (2 or 3 bytes) */
#if ( FLASH_SIZE <= 0x8000000 )
       HAL_NFlash_Send_Byte_Addr(pNFlashDev, (addr>> BYTE2_OFFSET) & BYTE_MASK );
       HAL_NFlash_Send_Byte_Addr(pNFlashDev, (addr >> BYTE3_OFFSET) & BYTE_MASK );
#else
       HAL_NFlash_Send_Byte_Addr(pNFlashDev, (addr >> BYTE2_OFFSET) & BYTE_MASK );
       HAL_NFlash_Send_Byte_Addr(pNFlashDev, (addr >> BYTE3_OFFSET) & BYTE_MASK );
       HAL_NFlash_Send_Byte_Addr(pNFlashDev, (addr >> BYTE4_OFFSET) & BYTE_MASK );
#endif

    /* Send random read confirmed command */
    HAL_NFlash_Send_Cmd(pNFlashDev, 0xD0 );

    /* Wait block erase finish */
    HAL_NFlash_Wait_Flash_Ready(pNFlashDev,SR6_READY_BUSY);
    /* Check erase result */
    HAL_NFlash_Status_Read(pNFlashDev, &status );
	if( (status & SR0_CHIP_STATUS ) == SR0_CHIP_STATUS )
		return Flash_EraseFailed;
	else
		return Flash_Success;
}

