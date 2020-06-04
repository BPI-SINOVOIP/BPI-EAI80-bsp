/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          flash.c
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/06/27
 *
 * @brief         Implementation of Flash Driver.
 *
 * @note
 *    2016/06/27, wizephen.wang, V1.0.0
 *        Initial version.
 *
 */

#include <drv/dev/flash.h>

static RET_CODE Flash_WaitforOperationDone(int32_t timeout)
{
    uint32_t status = 0;

    while((FLASH->ST & FLASH_ST_BSY) && (timeout--))
    {
        SysTick_DelayUS(1);
	}

	if (timeout <= 0)
	{
	    return RET_TIMEOUT;
	}

    /* Unlock Flash Controller Registers */
    FLASH->KEY = FLASH_UNLOCK_KEY_FOR_MANIPULATION;

    status = FLASH->ST;

    /* Write 1 to clear each finish flag, respectively */
    if (status & FLASH_ST_OMF || status & FLASH_ST_OEF || status & FLASH_ST_OPF)
    {
        if (status & FLASH_ST_OMF)
        {
            FLASH->ST = FLASH_ST_OMF;
        }

        if (status & FLASH_ST_OEF)
        {
            FLASH->ST = FLASH_ST_OEF;
        }

        if (status & FLASH_ST_OPF)
        {
            FLASH->ST = FLASH_ST_OPF;
        }
    }

    /* Leave them for application detection and clearing */
    if (status & FLASH_ST_PVE || status & FLASH_ST_PPE ||
        status & FLASH_ST_ASE || status & FLASH_ST_EPE )
    {
        return RET_ERROR;
    }

    return RET_OK;
}

RET_CODE Flash_Program2Address(uint32_t targetAddress, uint32_t wrData, FLASH_ProgWidth_T width)
{
    RET_CODE retval = RET_ERROR;
    FLASH_Param_T param;

#if defined(CHIP_GM6601)
    if(width == FLASH_PROGRAM_DATA_WIDTH_16BIT)
    {
        if(targetAddress & 0x01)
        {
            goto FuncExit;
        }
    }
    else
    {
        if(targetAddress & 0x03)
        {
            goto FuncExit;
        }
    }
#endif

#if defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    if(targetAddress & 0x03)
    {
        goto FuncExit;
    }
#endif

    Flash_GetParam(&param);

    /* Unlock before manipulating flash controller register */
    FLASH->KEY = FLASH_UNLOCK_KEY_FOR_MANIPULATION;

    /* Clear all error flags */
    FLASH->ST = 0xFFFFFFFF;

    /* Write the physical address in flash */
    FLASH->ADDR = targetAddress | param.basePhyAddress;
    FLASH->DT   = wrData;

    /* Write program command */
    FLASH->CMD  = FLASH_PROGRAM_CMD;

#if defined(CHIP_GM6601)
    /* Set program mode */
    if(width == FLASH_PROGRAM_DATA_WIDTH_16BIT)
    {
        FLASH->CTRL |= FLASH_CTRL_PGMD;
    }
    else
    {
        FLASH->CTRL &= ~FLASH_CTRL_PGMD;
    }
#endif

#if defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    /* Only 32Bit Programming is allowed */
    FLASH->CTRL &= ~FLASH_CTRL_PGMD;
#endif

#if defined(CHIP_GM6621)
    FLASH->CTRL &= ~0x03;
    FLASH->CTRL |= width;
#endif

    /* Lock flash controller when writing or erasing flash is going on */
    IO_BITS_SET(FLASH->CMD, (FLASH_CMD_TRG | FLASH_CMD_LCK));
    retval = RET_OK;
	goto FuncExit;
FuncExit:
    return retval;
}

static void Flash_Erase_All(void)
{
    /* Unlock before manipulating flash controller register */
    FLASH->KEY = FLASH_UNLOCK_KEY_FOR_MANIPULATION;

    /* Clear all error flags */
    FLASH->ST = 0xFFFFFFFF;

    /* Write mass erase command */
    FLASH->CMD  = FLASH_MASSERASE_CMD;

    /* Lock flash controller when writing or erasing flash is going on */
    IO_BITS_SET(FLASH->CMD, (FLASH_CMD_TRG | FLASH_CMD_LCK));
}

static RET_CODE Flash_Erase_Page(uint32_t pageAddress)
{
    uint32_t phyPageAddr = 0;
    FLASH_Param_T param;
    Flash_GetParam(&param);

    ASSERT((pageAddress | param.basePhyAddress) < (FLASH_TOTAL_SIZE | param.basePhyAddress));

    phyPageAddr = pageAddress | param.basePhyAddress;

    /* Unlock before manipulating flash controller registers */
    FLASH->KEY = FLASH_UNLOCK_KEY_FOR_MANIPULATION;

    /* clear all error flags */
    FLASH->ST   = 0xFFFFFFFF;
    FLASH->ADDR = phyPageAddr;

    /* Write page erase command */
    FLASH->CMD  = FLASH_PAGEERASE_CMD;

    /* Lock flash controller when writing or erasing flash is going on */
    IO_BITS_SET(FLASH->CMD, (FLASH_CMD_TRG | FLASH_CMD_LCK));

    return RET_OK;
}

RET_CODE Flash_Init(void)
{
    return RET_OK;
}

RET_CODE Flash_DeInit (void)
{
    return RET_OK;
}

RET_CODE Flash_Erase(FLASH_EraseType_T eraseType, uint32_t pageAddress)
{
    RET_CODE retval = RET_ERROR;

    if (pageAddress & (FLASH_PAGE_SIZE - 1))
    {
        goto FuncExit;
    }

    switch (eraseType)
    {
        case ERASE_TYPE_MASS:
            UNUSED(pageAddress);
            /* Mass erase */
            Flash_Erase_All();
            /* Wait mass-erase complete */
            retval = Flash_WaitforOperationDone(FLASH_MASSERASE_MAXTIME);
            break;

        case ERASE_TYPE_PAGE:
            /* Page erase*/
            retval = Flash_Erase_Page(pageAddress);
            if(retval != RET_OK)
            {
                break;
            }
            retval = Flash_WaitforOperationDone(FLASH_PAGEERASE_MAXTIME);
            break;
    }

FuncExit:
    return retval;
}

RET_CODE Flash_Write(uint32_t address, uint8_t *pData, uint16_t count)
{
    RET_CODE retval = RET_OK;
    uint16_t progWdCnt = 0;
    uint32_t datatmp   = 0;
    uint8_t *pDataTmp = pData;

    FLASH_Param_T param;
    Flash_GetParam(&param);

    ASSERT((address | param.basePhyAddress) + count <= (FLASH_TOTAL_SIZE | param.basePhyAddress));

    /* Logical address*/
    address &= ~(param.basePhyAddress);

    progWdCnt = count >> 2;
    while (progWdCnt)
    {
        datatmp = *pDataTmp | *(pDataTmp + 1) << 8 | *(pDataTmp + 2) << 16 | *(pDataTmp + 3) << 24;

        /* Program data to target address */
        retval  = Flash_Program2Address(address, datatmp, FLASH_PROGRAM_DATA_WIDTH_32BIT);

        if (retval != RET_OK)
        {
            goto FuncExit;
        }

        /* Wait flash programming complete */
        retval = Flash_WaitforOperationDone(FLASH_PROGRAM_MAXTIME);

        if (retval != RET_OK)
        {
            goto FuncExit;
        }

        pDataTmp += 4;
        progWdCnt --;
        address += 4;
    }

    if(count & 0x03)
    {
        switch(count & 0x03)
        {
            case 1:
                datatmp = 0xFFFFFF00 | *(pData + count - 1);
                break;
            case 2:
                datatmp = 0xFFFF0000 | *(pData + count - 2) | *(pData + count - 1) << 8;
                break;
            case 3:
                datatmp = 0xFF000000 | *(pData + count - 3) | *(pData + count - 2) << 8 | *(pData + count - 1) << 16;
                break;
        }

        retval  = Flash_Program2Address(address, datatmp, FLASH_PROGRAM_DATA_WIDTH_32BIT);
        if (retval != RET_OK)
        {
            goto FuncExit;
        }

        retval = Flash_WaitforOperationDone(FLASH_PROGRAM_MAXTIME);
    }

FuncExit:
    return retval;
}

RET_CODE Flash_GetParam(FLASH_Param_T *pParam)
{
    ASSERT(pParam != NULL);

#if defined (CHIP_GM6601) || defined (CHIP_GM6621) || defined (CHIP_GM6611) ||\
    defined (CHIP_GM6601B) || defined(CHIP_GM6611B)
    pParam->pagesPerFlash  = FLASH_TOTAL_PAGES;
    pParam->sizePerPage    = FLASH_PAGE_SIZE;
    pParam->sizePerFlash   = FLASH_TOTAL_SIZE;
    pParam->baseLgcAddress = FLASH_LOGICAL_BASE_ADDRESS;
    pParam->basePhyAddress = FLASH_START_PHYSICAL_ADDRESS;
#endif

    return RET_OK;
}

Page_LockType_T Flash_PageIsLocked(uint32_t pageAddress)
{
    uint32_t page = 0;
    FLASH_Param_T param;
    Flash_GetParam(&param);

    ASSERT((pageAddress | param.basePhyAddress) < (FLASH_TOTAL_SIZE | param.basePhyAddress));
    /* Logical page address*/
    pageAddress &= ~(param.basePhyAddress);

    page = pageAddress / FLASH_LOCK_DENSITY;
    if((FLASH->PRT) & (uint32_t)(1 << page))
    {
        return PAGE_IS_LOCKED;
    }

    return PAGE_IS_UNLOCKED;
}

RET_CODE Flash_Read(uint32_t address, uint8_t *pData, uint16_t count)
{
    FLASH_Param_T param;
    Flash_GetParam(&param);

    ASSERT(pData != NULL);
    ASSERT((address | param.basePhyAddress) + count < (FLASH_TOTAL_SIZE | param.basePhyAddress));

    address = address | param.basePhyAddress;
    memcpy(pData, (uint8_t *)address, count);

    return RET_OK;
}
