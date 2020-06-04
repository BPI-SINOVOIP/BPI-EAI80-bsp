/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_otp.c
 *
 * @author        william.wang
 *
 * @version       1.0.0
 *
 * @date          2018/04/20
 *
 * @brief         the hal layer of wwdg driver.
 *
 * @note
 *    2018/04/20    william.wang     V1.0
 *        Update For GM6721       V1.0.0
 */
#include<gm_hal_otp.h>
#include<gm_irq.h>
#include"gm_ll_otp.h"
#include"gm_ll_otp_memio.h"

#define CONFIG_OTP_HAL_INTERRUPT           (0)

static void HAL_OTP_IRQHandler(OTP_Handle_T *hOTP)
{
	
}

/**
 * @brief  Init OTP Device Config Timing.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  pConfig: 	OTP Config.
 * @retval HAL status
 */
void HAL_OTP_ConfigTiming(OTP_Handle_T *pOTPHandle,OTP_TimingConfig_T *pOTPTiming)
{
	LL_OTP_SetTimingData(pOTPHandle->mDevice,pOTPTiming->mTPW,pOTPTiming->mTPWI,
											pOTPTiming->mTCS,pOTPTiming->mTPPS,
											pOTPTiming->mTPPH,pOTPTiming->mTPPR);
}

/**
 * @brief  OTP Device Program Operation Wait To Finished Done.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval HAL status
 */
static void HAL_OTP_WaitProg_done(OTP_Handle_T *pOTPHandle)
{
    int 			j = 0;
    uint32_t    	status;

    HAL_DelayUs(5);

    while(1)
    {	
    	status = LL_OTP_GetStatus(pOTPHandle->mDevice);
        if((status & OTP_STS_PROG_BSY_BUSY) == 0)
        {
        	break;
        }       
        else if(j==200)
        {
            break;
        }
        
        j++;
        //printk ("\n   ===> OTP macro program data is going\n" );    
        HAL_DelayUs(100);
    }
}

/**
 * @brief  Init OTP Device to buffer.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  pOTPDevice: 	OTP Device.
 * @param  pConfig: 	OTP Config.
 * @retval HAL status
 */
RET_CODE HAL_OTP_Init(OTP_Handle_T *pOTPHandle,OTP_Device_T *pOTPDevice,OTP_MEMIO_T *pOTPMemIO,OTP_Config_T *pConfig)
{
    RET_CODE ret = RET_OK;

    if ((pOTPHandle && pOTPDevice && pConfig) == NULL )
    {
        ret = RET_INVPARAM;
        
        goto out;
    }

    pOTPHandle->mDevice = pOTPDevice;
    pOTPHandle->mMemIO = pOTPMemIO;

    //LL_OTP_SetTimingData(pOTPDevice,pConfig->mTimgConfig.mTPW,pConfig->mTimgConfig.mTPWI,pConfig->mTimgConfig.mTCS,pConfig->mTimgConfig.mTPPS,pConfig->mTimgConfig.mTPPH,pConfig->mTimgConfig.mTPPR);

    LL_OTP_Disable(pOTPDevice);
    /* Enabel nvic OTP irq and register irq callback */
    pOTPHandle->mOTPCallback = pConfig->mOTPCallback;
    /* Register UART IR number and callback */
#if CONFIG_OTP_HAL_INTERRUPT
    HAL_RequestIrq(OTP_IRQn, OTP_IRQ_PRIORITY, HAL_OTP_IRQHandler, pOTPHandle);
    HAL_NVIC_EnableIRQ(OTP_IRQn);
#endif

out:
    return ret;
}

/**
 * @brief  Read Buffer from OTP Device to buffer.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  address: 	OTP Device Offset address.
 * @param  buffer: 		The dst memory Buffer.
 * @param  size:  		read data size(DWORD).
 * @retval HAL status
 */
RET_CODE HAL_OTP_ReadBuffer(OTP_Handle_T *pOTPHandle,uint32_t address,void *buffer,uint32_t size)
{
	RET_CODE ret = RET_OK;
	uint32_t  *getbuffer;
	uint32_t  getaddr;

    if ((address + size) > MAX_OTP_SIZE)
    {
        ret = RET_INVPARAM;
        
        goto out;
    }
    
    OTP_WORKMODE_T   mode;
    
    mode = LL_OTP_GetWorkMode(pOTPHandle->mDevice);
    if(mode != OTP_WORKMODE_NORMAL)
    {
    	LL_OTP_Disable(pOTPHandle->mDevice);
    	LL_OTP_SetWorkMode(pOTPHandle->mDevice,OTP_WORKMODE_NORMAL);
    }
    
    getbuffer = (uint32_t *)buffer;
    getaddr   = address;
    
    for(int i  = 0;i < size;i++)
    {
    	*getbuffer = LL_OTP_GetData(pOTPHandle->mMemIO,getaddr);
    	getbuffer++;
    	getaddr += 4;
    }
    
out:
    return ret;
}

/**
 * @brief  Write Buffer Data to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  address: 	OTP Device Offset address.
 * @param  buffer: 		The source memory Buffer.
 * @param  size:  		write data size(DWORD).
 * @retval HAL status
 */
RET_CODE HAL_OTP_WriteBuffer(OTP_Handle_T *pOTPHandle, uint32_t address,void *buffer,uint32_t size)
{
	RET_CODE ret = RET_OK;
	uint32_t  *getbuffer;
	uint32_t  getaddr;

    if ((address + size) > MAX_OTP_SIZE)
    {
        ret = RET_INVPARAM;
        
        return ret;
    }
    
    OTP_WORKMODE_T   mode;
    
    mode = LL_OTP_GetWorkMode(pOTPHandle->mDevice);
    if(mode != OTP_WM_MODE_PGM)
    {
    	LL_OTP_Disable(pOTPHandle->mDevice);
    	LL_OTP_SetWorkMode(pOTPHandle->mDevice,OTP_WM_MODE_PGM);
    }
    
    getbuffer = (uint32_t *)buffer;
    getaddr   = address;
    
    for(int i  = 0;i < size;i++)
    {
    	LL_OTP_SetAddress(pOTPHandle->mDevice,getaddr);
    	LL_OTP_SetData(pOTPHandle->mDevice,(uint32_t)(*getbuffer));
    	
    	
    	LL_OTP_SetCellMode(pOTPHandle->mDevice,OTP_CELLMODE_MAIN);
    	LL_OTP_Write_Start(pOTPHandle->mDevice);
    	HAL_OTP_WaitProg_done(pOTPHandle);
    	
    	LL_OTP_SetCellMode(pOTPHandle->mDevice,OTP_CELLMODE_REDUNDANCY);
    	LL_OTP_Write_Start(pOTPHandle->mDevice);
    	HAL_OTP_WaitProg_done(pOTPHandle);
    	
    	getaddr += 4;
    	getbuffer++;
    }
    
    return RET_OK;
}

/**
 * @brief  Write data to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  address: 	OTP Device Offset address.
 * @param  data: 		The data will be writed.
 * @retval HAL status
 */
RET_CODE HAL_OTP_WriteData(OTP_Handle_T *pOTPHandle, uint32_t address,uint32_t data)
{
	RET_CODE ret = RET_OK;

    if (address > MAX_OTP_SIZE)
    {
        ret = RET_INVPARAM;
        
        goto out;
    }
    
    OTP_WORKMODE_T   mode;
    
    mode = LL_OTP_GetWorkMode(pOTPHandle->mDevice);
    if(mode != OTP_WM_MODE_PGM)
    {
    	LL_OTP_Disable(pOTPHandle->mDevice);
    	LL_OTP_SetWorkMode(pOTPHandle->mDevice,OTP_WM_MODE_PGM);
    }
    
    LL_OTP_SetAddress(pOTPHandle->mDevice,address);
    LL_OTP_SetData(pOTPHandle->mDevice,data);
    	  	
	LL_OTP_SetCellMode(pOTPHandle->mDevice,OTP_CELLMODE_MAIN);
	LL_OTP_Write_Start(pOTPHandle->mDevice);
	HAL_OTP_WaitProg_done(pOTPHandle);
	
	LL_OTP_SetCellMode(pOTPHandle->mDevice,OTP_CELLMODE_REDUNDANCY);
	LL_OTP_Write_Start(pOTPHandle->mDevice);
	HAL_OTP_WaitProg_done(pOTPHandle);
    
out:
    return ret;
}

/**
 * @brief  Read data from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  address: 	OTP Device Offset address.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_ReadData(OTP_Handle_T *pOTPHandle, uint32_t address)
{
	uint32_t ret = (uint32_t)RET_INVPARAM;
	
	if (address > MAX_OTP_SIZE)
    {
        ret = RET_INVPARAM;
        
        goto out;
    }

    OTP_WORKMODE_T   mode;
    
    mode = LL_OTP_GetWorkMode(pOTPHandle->mDevice);
    if(mode != OTP_WORKMODE_NORMAL)
    {
    	LL_OTP_Disable(pOTPHandle->mDevice);
    	LL_OTP_SetWorkMode(pOTPHandle->mDevice,OTP_WORKMODE_NORMAL);
    }
    
    ret = LL_OTP_GetData(pOTPHandle->mMemIO,address);
    
out:
    return ret;
}

/**
 * @brief  Set Write Protection Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The wp1 will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetWP1(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,WP1_OFFSET,mask);
}

/**
 * @brief  Get Write Protection Mask Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetWP1(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,WP1_OFFSET);
}

/**
 * @brief  Set Write Protection2 Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The wp2 will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetWP2(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,WP2_OFFSET,mask);
}


/**
 * @brief  Get Write Protection2 Mask Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetWP2(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,WP2_OFFSET);
}

/**
 * @brief  Set Read Protection Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The rp mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetRP(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,RP_OFFSET,mask);
}

/**
 * @brief  Get Read Protection Mask Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetRP(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,RP_OFFSET);
}

/**
 * @brief  Set Write Protection Selection Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The wp selection will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetWPSel(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,WP_SEL_OFFSET,mask);
}


/**
 * @brief  Get Write Protection Selection Mask Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetWPSel(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,WP_SEL_OFFSET);
}

/**
 * @brief  Set System Config0 Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The system config0 mask will be setted.
 * @retval HAL status
 */
 RET_CODE HAL_OTP_SetSysCfg0(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,SYS_CFG0_OFFSET,mask);
}


/**
 * @brief  Get System Config0 Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg0(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,SYS_CFG0_OFFSET);
}

/**
 * @brief  Set System Config1(MFC) Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The system config1(MFC) will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg1MFC(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,SYS_CFG1MFC_OFFSET,mask);
}

/**
 * @brief  Get System Config1(MFC) Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg1MFC(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,SYS_CFG1MFC_OFFSET);
}

/**
 * @brief  Set System Config1(CSTM) Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The system config1(CSTM) mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg1CSTM(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,SYS_CFG1CSTM_OFFSET,mask);
}

/**
 * @brief  Get System Config1(CSTM) Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg1CSTM(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,SYS_CFG1CSTM_OFFSET);
}


/**
 * @brief  Set System Config1 Selection Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The system config1 selection will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg1Sel(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,SYS_CFG1SEL_OFFSET,mask);
}

/**
 * @brief  Get System Config1 Selection Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg1Sel(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,SYS_CFG1SEL_OFFSET);
}

/**
 * @brief  Set System Config2 mask to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The system config2 mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg2(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,SYS_CFG0_OFFSET,mask);
}

/**
 * @brief  Get System Config2 Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
 
uint32_t HAL_OTP_GetSysCfg2(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,SYS_CFG0_OFFSET);
}


/**
 * @brief  Set System Config3 mask to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The system config3 mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg3(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,SYS_CFG0_OFFSET,mask);
}

/**
 * @brief  Get System Config3 Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg3(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,SYS_CFG0_OFFSET);
}


/**
 * @brief  Set System Config4 mask to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The system config4 mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg4(OTP_Handle_T *pOTPHandle,uint32_t mask)
{
	return HAL_OTP_WriteData(pOTPHandle,SYS_CFG0_OFFSET,mask);
}

/**
 * @brief  Get System Config4 Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg4(OTP_Handle_T *pOTPHandle)
{
	return HAL_OTP_ReadData(pOTPHandle,SYS_CFG0_OFFSET);
}


/**
 * @brief  Set chipid to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The chipid array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetChipID(OTP_Handle_T *pOTPHandle,OTP_ARRAY8_T *chiparray)
{
	if (chiparray == NULL )
    {
        return RET_INVPARAM;
    }
    
	HAL_OTP_WriteData(pOTPHandle,CHIPID1_OFFSET,chiparray->array[0]);
	HAL_OTP_WriteData(pOTPHandle,CHIPID1_OFFSET + 0x04,chiparray->array[1]);
	HAL_OTP_WriteData(pOTPHandle,CHIPID1_OFFSET + 0x08,chiparray->array[2]);
	HAL_OTP_WriteData(pOTPHandle,CHIPID1_OFFSET + 0x0C,chiparray->array[3]);
	HAL_OTP_WriteData(pOTPHandle,CHIPID2_OFFSET,chiparray->array[4]);
	HAL_OTP_WriteData(pOTPHandle,CHIPID2_OFFSET + 0x04,chiparray->array[5]);
	HAL_OTP_WriteData(pOTPHandle,CHIPID2_OFFSET + 0x08,chiparray->array[6]);
	HAL_OTP_WriteData(pOTPHandle,CHIPID2_OFFSET + 0x0C,chiparray->array[7]);

	return RET_OK;
}

/**
 * @brief  Get CHIPID from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  chiparray:  return chipid array(8 Dword).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetChipID(OTP_Handle_T *pOTPHandle,OTP_ARRAY8_T *chiparray)
{
	if (chiparray == NULL )
    {
        return RET_INVPARAM;
    }
    
    chiparray->array[0] = HAL_OTP_ReadData(pOTPHandle,CHIPID1_OFFSET);
    chiparray->array[1] = HAL_OTP_ReadData(pOTPHandle,CHIPID1_OFFSET + 0x04);
    chiparray->array[2] = HAL_OTP_ReadData(pOTPHandle,CHIPID1_OFFSET + 0x08);
    chiparray->array[3] = HAL_OTP_ReadData(pOTPHandle,CHIPID1_OFFSET + 0x0C);
    chiparray->array[4] = HAL_OTP_ReadData(pOTPHandle,CHIPID2_OFFSET);
    chiparray->array[5] = HAL_OTP_ReadData(pOTPHandle,CHIPID2_OFFSET + 0x04);
    chiparray->array[6] = HAL_OTP_ReadData(pOTPHandle,CHIPID2_OFFSET + 0x08);
    chiparray->array[7] = HAL_OTP_ReadData(pOTPHandle,CHIPID2_OFFSET + 0x0C);
       
	return RET_OK;
}


/**
 * @brief  Set 128/256-bit hardware unique key to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The huk array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetHUK(OTP_Handle_T *pOTPHandle,OTP_ARRAY8_T *hukarray)
{
	if (hukarray == NULL )
    {
        return RET_INVPARAM;
    }
    
	HAL_OTP_WriteData(pOTPHandle,HUK_OFFSET,hukarray->array[0]);
	HAL_OTP_WriteData(pOTPHandle,HUK_OFFSET + 0x04,hukarray->array[1]);
	HAL_OTP_WriteData(pOTPHandle,HUK_OFFSET + 0x08,hukarray->array[2]);
	HAL_OTP_WriteData(pOTPHandle,HUK_OFFSET + 0x0C,hukarray->array[3]);
	HAL_OTP_WriteData(pOTPHandle,HUK_OFFSET + 0x10,hukarray->array[4]);
	HAL_OTP_WriteData(pOTPHandle,HUK_OFFSET + 0x14,hukarray->array[5]);
	HAL_OTP_WriteData(pOTPHandle,HUK_OFFSET + 0x18,hukarray->array[6]);
	HAL_OTP_WriteData(pOTPHandle,HUK_OFFSET + 0x1C,hukarray->array[7]);

	return RET_OK;
}

/**
 * @brief  Get 128/256-bit hardware unique key from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  hukarray:  return huk array(8 Dword).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetHUK(OTP_Handle_T *pOTPHandle,OTP_ARRAY8_T *hukarray)
{
	if (hukarray == NULL )
    {
        return RET_INVPARAM;
    }
    
    hukarray->array[0] = HAL_OTP_ReadData(pOTPHandle,HUK_OFFSET);
    hukarray->array[1] = HAL_OTP_ReadData(pOTPHandle,HUK_OFFSET + 0x04);
    hukarray->array[2] = HAL_OTP_ReadData(pOTPHandle,HUK_OFFSET + 0x08);
    hukarray->array[3] = HAL_OTP_ReadData(pOTPHandle,HUK_OFFSET + 0x0C);
    hukarray->array[4] = HAL_OTP_ReadData(pOTPHandle,HUK_OFFSET + 0x10);
    hukarray->array[5] = HAL_OTP_ReadData(pOTPHandle,HUK_OFFSET + 0x14);
    hukarray->array[6] = HAL_OTP_ReadData(pOTPHandle,HUK_OFFSET + 0x18);
    hukarray->array[7] = HAL_OTP_ReadData(pOTPHandle,HUK_OFFSET + 0x1C);
       
	return RET_OK;
}

/**
 * @brief  Set 256-bits root of trust public key to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The rotpk array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetROTPK(OTP_Handle_T *pOTPHandle,OTP_ARRAY8_T *rotpkarray)
{
	if (rotpkarray == NULL )
    {
        return RET_INVPARAM;
    }
    
	HAL_OTP_WriteData(pOTPHandle,ROTPK_OFFSET,rotpkarray->array[0]);
	HAL_OTP_WriteData(pOTPHandle,ROTPK_OFFSET + 0x04,rotpkarray->array[1]);
	HAL_OTP_WriteData(pOTPHandle,ROTPK_OFFSET + 0x08,rotpkarray->array[2]);
	HAL_OTP_WriteData(pOTPHandle,ROTPK_OFFSET + 0x0C,rotpkarray->array[3]);
	HAL_OTP_WriteData(pOTPHandle,ROTPK_OFFSET + 0x10,rotpkarray->array[4]);
	HAL_OTP_WriteData(pOTPHandle,ROTPK_OFFSET + 0x14,rotpkarray->array[5]);
	HAL_OTP_WriteData(pOTPHandle,ROTPK_OFFSET + 0x18,rotpkarray->array[6]);
	HAL_OTP_WriteData(pOTPHandle,ROTPK_OFFSET + 0x1C,rotpkarray->array[7]);

	return RET_OK;
}

/**
 * @brief  Get 256-bits root of trust public key from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  rotpkarray:  return rotpk array(8 Dword).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetROTPK(OTP_Handle_T *pOTPHandle,OTP_ARRAY8_T *rotpkarray)
{
	if (rotpkarray == NULL )
    {
        return RET_INVPARAM;
    }
    
    rotpkarray->array[0] = HAL_OTP_ReadData(pOTPHandle,ROTPK_OFFSET);
    rotpkarray->array[1] = HAL_OTP_ReadData(pOTPHandle,ROTPK_OFFSET + 0x04);
    rotpkarray->array[2] = HAL_OTP_ReadData(pOTPHandle,ROTPK_OFFSET + 0x08);
    rotpkarray->array[3] = HAL_OTP_ReadData(pOTPHandle,ROTPK_OFFSET + 0x0C);
    rotpkarray->array[4] = HAL_OTP_ReadData(pOTPHandle,ROTPK_OFFSET + 0x10);
    rotpkarray->array[5] = HAL_OTP_ReadData(pOTPHandle,ROTPK_OFFSET + 0x14);
    rotpkarray->array[6] = HAL_OTP_ReadData(pOTPHandle,ROTPK_OFFSET + 0x18);
    rotpkarray->array[7] = HAL_OTP_ReadData(pOTPHandle,ROTPK_OFFSET + 0x1C);
       
	return RET_OK;
}

/**
 * @brief  Set A 256-bit Endorsement Key to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The ek array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetEK(OTP_Handle_T *pOTPHandle,OTP_ARRAY8_T *ekarray)
{
	if (ekarray == NULL )
    {
        return RET_INVPARAM;
    }
    
	HAL_OTP_WriteData(pOTPHandle,EK_OFFSET,ekarray->array[0]);
	HAL_OTP_WriteData(pOTPHandle,EK_OFFSET + 0x04,ekarray->array[1]);
	HAL_OTP_WriteData(pOTPHandle,EK_OFFSET + 0x08,ekarray->array[2]);
	HAL_OTP_WriteData(pOTPHandle,EK_OFFSET + 0x0C,ekarray->array[3]);
	HAL_OTP_WriteData(pOTPHandle,EK_OFFSET + 0x10,ekarray->array[4]);
	HAL_OTP_WriteData(pOTPHandle,EK_OFFSET + 0x14,ekarray->array[5]);
	HAL_OTP_WriteData(pOTPHandle,EK_OFFSET + 0x18,ekarray->array[6]);
	HAL_OTP_WriteData(pOTPHandle,EK_OFFSET + 0x1C,ekarray->array[7]);

	return RET_OK;
}

/**
 * @brief  Get A 256-bit Endorsement Key from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  ekarray:  return ekpk array(8 Dword).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetEK(OTP_Handle_T *pOTPHandle,OTP_ARRAY8_T *ekarray)
{
	if (ekarray == NULL )
    {
        return RET_INVPARAM;
    }
    
    ekarray->array[0] = HAL_OTP_ReadData(pOTPHandle,EK_OFFSET);
    ekarray->array[1] = HAL_OTP_ReadData(pOTPHandle,EK_OFFSET + 0x04);
    ekarray->array[2] = HAL_OTP_ReadData(pOTPHandle,EK_OFFSET + 0x08);
    ekarray->array[3] = HAL_OTP_ReadData(pOTPHandle,EK_OFFSET + 0x0C);
    ekarray->array[4] = HAL_OTP_ReadData(pOTPHandle,EK_OFFSET + 0x10);
    ekarray->array[5] = HAL_OTP_ReadData(pOTPHandle,EK_OFFSET + 0x14);
    ekarray->array[6] = HAL_OTP_ReadData(pOTPHandle,EK_OFFSET + 0x18);
    ekarray->array[7] = HAL_OTP_ReadData(pOTPHandle,EK_OFFSET + 0x1C);
       
	return RET_OK;
}


/**
 * @brief  Set 128/256-bit Secret Symmetric Key to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The ssk array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSSK(OTP_Handle_T *pOTPHandle,OTP_ARRAY8_T *sskarray)
{
	if (sskarray == NULL )
    {
        return RET_INVPARAM;
    }
    
	HAL_OTP_WriteData(pOTPHandle,SSK_OFFSET,sskarray->array[0]);
	HAL_OTP_WriteData(pOTPHandle,SSK_OFFSET + 0x04,sskarray->array[1]);
	HAL_OTP_WriteData(pOTPHandle,SSK_OFFSET + 0x08,sskarray->array[2]);
	HAL_OTP_WriteData(pOTPHandle,SSK_OFFSET + 0x0C,sskarray->array[3]);
	HAL_OTP_WriteData(pOTPHandle,SSK_OFFSET + 0x10,sskarray->array[4]);
	HAL_OTP_WriteData(pOTPHandle,SSK_OFFSET + 0x14,sskarray->array[5]);
	HAL_OTP_WriteData(pOTPHandle,SSK_OFFSET + 0x18,sskarray->array[6]);
	HAL_OTP_WriteData(pOTPHandle,SSK_OFFSET + 0x1C,sskarray->array[7]);

	return RET_OK;
}

/**
 * @brief  Get 128/256-bit Secret Symmetric Key from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  sskarray:  return ssk array(8 Dword).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetSSK(OTP_Handle_T *pOTPHandle,OTP_ARRAY8_T *sskarray)
{
	if (sskarray == NULL )
    {
        return RET_INVPARAM;
    }
    
    sskarray->array[0] = HAL_OTP_ReadData(pOTPHandle,SSK_OFFSET);
    sskarray->array[1] = HAL_OTP_ReadData(pOTPHandle,SSK_OFFSET + 0x04);
    sskarray->array[2] = HAL_OTP_ReadData(pOTPHandle,SSK_OFFSET + 0x08);
    sskarray->array[3] = HAL_OTP_ReadData(pOTPHandle,SSK_OFFSET + 0x0C);
    sskarray->array[4] = HAL_OTP_ReadData(pOTPHandle,SSK_OFFSET + 0x10);
    sskarray->array[5] = HAL_OTP_ReadData(pOTPHandle,SSK_OFFSET + 0x14);
    sskarray->array[6] = HAL_OTP_ReadData(pOTPHandle,SSK_OFFSET + 0x18);
    sskarray->array[7] = HAL_OTP_ReadData(pOTPHandle,SSK_OFFSET + 0x1C);
       
	return RET_OK;
}


/**
 * @brief  Set Trim data for analog configuration to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The trim array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetTrim(OTP_Handle_T *pOTPHandle,OTP_ARRAY4_T *trimarray)
{
	if (trimarray == NULL )
    {
        return RET_INVPARAM;
    }
    
	HAL_OTP_WriteData(pOTPHandle,TRIM_OFFSET,trimarray->array[0]);
	HAL_OTP_WriteData(pOTPHandle,TRIM_OFFSET + 0x04,trimarray->array[1]);
	HAL_OTP_WriteData(pOTPHandle,TRIM_OFFSET + 0x08,trimarray->array[2]);
	HAL_OTP_WriteData(pOTPHandle,TRIM_OFFSET + 0x0C,trimarray->array[3]);

	return RET_OK;
}

/**
 * @brief  Get Trim data for analog configuration from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  trimarray:  return trim array(4 Dwords).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetTrim(OTP_Handle_T *pOTPHandle,OTP_ARRAY4_T *trimarray)
{
	if (trimarray == NULL )
    {
        return RET_INVPARAM;
    }
    
    trimarray->array[0] = HAL_OTP_ReadData(pOTPHandle,TRIM_OFFSET);
    trimarray->array[1] = HAL_OTP_ReadData(pOTPHandle,TRIM_OFFSET + 0x04);
    trimarray->array[2] = HAL_OTP_ReadData(pOTPHandle,TRIM_OFFSET + 0x08);
    trimarray->array[3] = HAL_OTP_ReadData(pOTPHandle,TRIM_OFFSET + 0x0C);
       
	return RET_OK;
}

/**
 * @brief  Set firmware version to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The fwver array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetFWVer(OTP_Handle_T *pOTPHandle,OTP_ARRAY4_T *fwverarray)
{
	if (fwverarray == NULL )
    {
        return RET_INVPARAM;
    }
    
	HAL_OTP_WriteData(pOTPHandle,FW_VER_OFFSET,fwverarray->array[0]);
	HAL_OTP_WriteData(pOTPHandle,FW_VER_OFFSET + 0x04,fwverarray->array[1]);
	HAL_OTP_WriteData(pOTPHandle,FW_VER_OFFSET + 0x08,fwverarray->array[2]);
	HAL_OTP_WriteData(pOTPHandle,FW_VER_OFFSET + 0x0C,fwverarray->array[3]);

	return RET_OK;
}

/**
 * @brief  Get firmware version from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  fwverarray:  return fwver array(4 Dwords).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetFWVer(OTP_Handle_T *pOTPHandle,OTP_ARRAY4_T *fwverarray)
{
	if (fwverarray == NULL )
    {
        return RET_INVPARAM;
    }
    
    fwverarray->array[0] = HAL_OTP_ReadData(pOTPHandle,FW_VER_OFFSET);
    fwverarray->array[1] = HAL_OTP_ReadData(pOTPHandle,FW_VER_OFFSET + 0x04);
    fwverarray->array[2] = HAL_OTP_ReadData(pOTPHandle,FW_VER_OFFSET + 0x08);
    fwverarray->array[3] = HAL_OTP_ReadData(pOTPHandle,FW_VER_OFFSET + 0x0C);
       
	return RET_OK;
}

/**
 * @brief  Set DebugPort Password to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask: 		The dbgpw array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetDBGPW(OTP_Handle_T *pOTPHandle,OTP_ARRAY4_T *dbgpwarray)
{
	if (dbgpwarray == NULL )
    {
        return RET_INVPARAM;
    }
    
	HAL_OTP_WriteData(pOTPHandle,DBG_PW_OFFSET,dbgpwarray->array[0]);
	HAL_OTP_WriteData(pOTPHandle,DBG_PW_OFFSET + 0x04,dbgpwarray->array[1]);
	HAL_OTP_WriteData(pOTPHandle,DBG_PW_OFFSET + 0x08,dbgpwarray->array[2]);
	HAL_OTP_WriteData(pOTPHandle,DBG_PW_OFFSET + 0x0C,dbgpwarray->array[3]);

	return RET_OK;
}

/**
 * @brief  Get DebugPort Password from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  dbgpwarray:  return dbgpw array(4 Dwords).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetDBGPW(OTP_Handle_T *pOTPHandle,OTP_ARRAY4_T *dbgpwarray)
{
	if (dbgpwarray == NULL )
    {
        return RET_INVPARAM;
    }
    
    dbgpwarray->array[0] = HAL_OTP_ReadData(pOTPHandle,DBG_PW_OFFSET);
    dbgpwarray->array[1] = HAL_OTP_ReadData(pOTPHandle,DBG_PW_OFFSET + 0x04);
    dbgpwarray->array[2] = HAL_OTP_ReadData(pOTPHandle,DBG_PW_OFFSET + 0x08);
    dbgpwarray->array[3] = HAL_OTP_ReadData(pOTPHandle,DBG_PW_OFFSET + 0x0C);
       
	return RET_OK;
}





