/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_sqi_host.c
 *
 * @author       steven.lee
 *
 * @version       1.0.0
 *
 * @date          2019/11/28
 *
 * @brief
 *
 * @note
 *    2019/11/28, steven.lee, V1.0.0
 *        Initial version.
 */
#include <gm_hal_sqi.h>
#include "gm_hal_sqi_host.h"
#ifdef RABBIT_NO_OS_SYSTEM
    #include <systick.h>
#else
    #include <ugelis.h>
#endif


#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))
#define HW32_REG(ADDRESS)   (*((volatile uint32_t  *)(ADDRESS)))



/*delay function*/
void HAL_SQI_Time_Delayms(uint32_t ms)
{
#ifdef RABBIT_NO_OS_SYSTEM
    SysTick_DelayMS(ms);
#else
    k_sleep(ms);
#endif

}



/*=================================primary function==================================================*/
/**
 * @brief  Set SQI Flash config register.
 * @param  pSQIDev:  SQI Device.
 * @param  rValue:   register value.
 * @retval HAL status
 */

static RET_CODE   HAL_SQI_SET_CFG_REGISTER(SQI_Device_T *pSQIDev, uint32_t rValue)
{
    if (NULL == pSQIDev)
    {
        return RET_ERROR;
    }

    if ((rValue & SQI_HOLDJ_EN))
    {
        LL_SQI_HOLDJ_ENABLE(pSQIDev);
    }
    else
    {
        LL_SQI_HOLDJ_DISABLE(pSQIDev);
    }

    if ((rValue & SQI_WJ_EN))
    {
        LL_SQI_WJ_ENABLE(pSQIDev);
    }
    else
    {
        LL_SQI_WJ_DISABLE(pSQIDev);
    }

    if ((rValue & SQI_FAST_RW_CNT_EN))
    {
        LL_SQI_FAST_RW_CNT_ENABLE(pSQIDev);
    }
    else
    {
        LL_SQI_FAST_RW_CNT_DISABLE(pSQIDev);
    }
    if ((rValue & SQI_CONTINUE_WRITE_EN))
    {
        LL_SQI_CONTINUE_WRITE_ENABLE(pSQIDev);
    }
    else
    {
        LL_SQI_CONTINUE_WRITE_DISABLE(pSQIDev);
    }

    if ((rValue & SQI_CONTINUE_READ_EN))
    {
        LL_SQI_CONTINUE_READ_ENABLE(pSQIDev);
    }
    else
    {
        LL_SQI_CONTINUE_READ_DISABLE(pSQIDev);
    }

    if ((rValue & SQI_SET_CODE_HIT))
    {
        LL_SQI_SET_CODE_HIT(pSQIDev);
    }
    else
    {
        LL_SQI_DISABLE_CODE_HIT(pSQIDev);
    }

    if ((rValue & SQI_SET_ADDR_HIT))
    {
        LL_SQI_SET_ADDR_HIT(pSQIDev);
    }
    else
    {
        LL_SQI_DISABLE_ADDR_HIT(pSQIDev);
    }

    if ((rValue & SQI_SET_DMY_HIT))
    {
        LL_SQI_SET_DMY_HIT(pSQIDev);
    }
    else
    {
        LL_SQI_DISABLE_DMY_HIT(pSQIDev);
    }

    if ((rValue & SQI_SET_DATA_HIT))
    {
        LL_SQI_SET_DATA_HIT(pSQIDev);
    }
    else
    {
        LL_SQI_DISABLE_DATA_HIT(pSQIDev);
    }
    LL_SQI_COMMAND_CLEAR(pSQIDev, 0xff);
    LL_SQI_COMMAND_SET(pSQIDev, rValue & SQI_INS_SET);

    return RET_OK;
}

/**
 * @brief  SQI Flash read status operation register.
 * @param  pSQIDev:  SQI Device.
 * @param  regno:    register no.
 * @retval HAL status
 */

__STATIC_INLINE uint8_t HAL_SQI_Read_Status(SQI_Device_T *pSQIDev, MAX_READ_STATUS_REG_T regno)
{
    uint8_t reg = 0;
    uint8_t command = 0;

    switch (regno)
    {
        case NORR_RDSR:
            command = NOR_RDSR_CMD;
            break;
        case MX25R_RDSCUR:
            command = MX25R_RDSCUR_CMD;
            break;
        case MX25R_RDCR:
            command = MX25R_RDCR_CMD;
            break;
        case GD25R_RDSRM:
            command = GD25R_RDSRM_CMD;
            break;
        case GD25R_RDSRH:
            command = GD25R_RDSRH_CMD;
            break;
        case WB25R_RDSRM:
            command = WB25R_RDSRM_CMD;
            break;
        case WB25R_RDSRH:
            command = WB25R_RDSRH_CMD;
            break;
        case MT25R_RDNOVCFGRG:
            command = MT25R_RDNOVCFGRG_CMD;
            break;
        default:
            command = NOR_RDSR_CMD;
            break;
    }
    LL_SQI_SET_RW_IO_MODE(pSQIDev, SQI_MOD0);

    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | command);

    reg = HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE);//trig to send out the command and value

    return reg;
}






/**
 * @brief  SQI Flash wait flash idle.
 * @param  pSQIDev:  SQI Device.
 * @retval HAL status
 */

void HAL_SQI_Wait_Idle(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;

    rbuf   = HAL_SQI_Read_Status(pSQIDev, NORR_RDSR);
    while (rbuf & NOR_WIP_ENABLE)
    {
        rbuf = HAL_SQI_Read_Status(pSQIDev, NORR_RDSR);
    }
}

/**
 * @brief  SQI Flash write enable operation.
 * @param  pSQIDev:  SQI Device.
 * @retval HAL status
 */

void HAL_SQI_Write_Enable(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;
    LL_SQI_SET_RW_IO_MODE(pSQIDev, SQI_MOD0);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | NOR_WREN_CMD);
    HW32_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x0;
    rbuf   = HAL_SQI_Read_Status(pSQIDev, NORR_RDSR);
    while (!(rbuf & NOR_WRITE_LAUTCH_ENABLE))
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | NOR_WREN_CMD);
        HW32_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x0;
        rbuf   = HAL_SQI_Read_Status(pSQIDev, NORR_RDSR);
    }

}



/**
 * @brief  Set SQI Flash config register.
 * @param  pSQIDev:  SQI Device.
 * @param  regno:  write register number.
 * @param  rValue:   register value.
 * @retval HAL status
 */
static void  HAL_SQI_Write_Status(SQI_Device_T *pSQIDev, MAX_WRITE_STATUS_REG_T regno, uint8_t regv)
{
    uint8_t command = NOR_WRSR_CMD;//0x01

    switch (regno)
    {
        case NORR_WRSR:
            command = NOR_WRSR_CMD;
            break;
        case MX25R_WRSCUR:
            command = MX25R_WRSCUR_CMD;
            break;
        case GD25R_WRSRM:
            command = GD25R_WRSRM_CMD;
            break;
        case GD25R_WRSRH:
            command = GD25R_WRSRH_CMD;
            break;
        case WB25R_WRSRM:
            command = WB25R_WRSRM_CMD;
            break;
        case WB25R_WRSRH:
            command = WB25R_WRSRH_CMD;
            break;
        case MT25R_WRNOVCFGRG:
            command = MT25R_WRNOVCFGRG_CMD;
            break;
        default:
            command = NOR_WRSR_CMD;
            break;
    }
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_Wait_Idle(pSQIDev);

    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | command);

    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = regv; //trig to send out the command and value
}


/*=================================MX25primary function==================================================*/

static RET_CODE HAL_SQI_MX25_4LINE_Disable(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;
    int     timeout;

    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | NOR_WRSR_CMD);
    LL_SQI_FAST_RW_CNT_SET(pSQIDev, 2);
    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x00;
    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x00;
    HAL_SQI_Wait_Idle(pSQIDev);
    rbuf   = HAL_SQI_Read_Status(pSQIDev, NORR_RDSR);
    timeout = SQI_TIMEOUT;
    while (((rbuf & MX25R_QUAD_ENABLE)) && (timeout > 0))
    {
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_Write_Enable(pSQIDev);
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | NOR_WRSR_CMD);
        LL_SQI_FAST_RW_CNT_SET(pSQIDev, 2);
        HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x00;
        HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x00;
        HAL_SQI_Wait_Idle(pSQIDev);
        rbuf = HAL_SQI_Read_Status(pSQIDev, NORR_RDSR);
        timeout--;
        HAL_SQI_Time_Delayms(10);
    }
    if (timeout == 0)
    {
        return RET_TIMEOUT;
    }
    return RET_OK;

}


/**
 * @brief  Set SQI Flash enable 4line mode
 * @param  pSQIDev:  SQI Device.
 */

static RET_CODE HAL_SQI_MX25_4LINE_Enable(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;
    int     timeout;
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | NOR_WRSR_CMD);
    LL_SQI_FAST_RW_CNT_SET(pSQIDev, 2);
    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x40;
    HAL_SQI_Wait_Idle(pSQIDev);
    rbuf   = HAL_SQI_Read_Status(pSQIDev, NORR_RDSR);
    timeout = SQI_TIMEOUT;
    while ((!(rbuf & MX25R_QUAD_ENABLE)) && (timeout > 0))
    {
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_Write_Enable(pSQIDev);
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | NOR_WRSR_CMD);
        LL_SQI_FAST_RW_CNT_SET(pSQIDev, 2);
        HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x40;
        HAL_SQI_Wait_Idle(pSQIDev);
        rbuf = HAL_SQI_Read_Status(pSQIDev, NORR_RDSR);
        timeout--;
        HAL_SQI_Time_Delayms(10);
    }
    if (timeout == 0)
    {
        return RET_TIMEOUT;
    }
    return RET_OK;

}

RET_CODE HAL_SQI_MX25_4LineWrite_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size)
{
    RET_CODE ret;
    ret = HAL_SQI_MX25_4LINE_Enable(pSQIDev);
    if (ret != RET_OK)
    {
        return ret;
    }
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    LL_SQI_SET_RW_IO_MODE(pSQIDev, SQI_MOD3);
    if (flash_size > SFLASH_16M_SIZE)
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT  | SQI_SET_DATA_HIT | MX25R_4PP4B_CMD);
    }
    else
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT  | SQI_SET_DATA_HIT | MX25R_4PP_CMD);
    }
    LL_SQI_FAST_RW_CNT_SET(pSQIDev, count);
    return RET_OK;

}


RET_CODE HAL_SQI_MX25_4LineRead_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size)
{
    RET_CODE ret;
    ret = HAL_SQI_MX25_4LINE_Enable(pSQIDev);
    if (ret != RET_OK)
    {
        return ret;
    }
    LL_SQI_SET_RW_IO_MODE(pSQIDev, SQI_MOD5);
    LL_SQI_FAST_RW_CNT_SET(pSQIDev, count);
    if (flash_size > SFLASH_16M_SIZE)
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_READ_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DMY_HIT | SQI_SET_DATA_HIT | MX25R_QREAD4B_CMD);
    }
    else
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_READ_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DMY_HIT | SQI_SET_DATA_HIT | MX25R_QREAD_CMD);
    }

    return RET_OK;

}

RET_CODE HAL_SQI_MX25_Close_4Line(SQI_Device_T *pSQIDev)
{
    if (pSQIDev)
    {
        return HAL_SQI_MX25_4LINE_Disable(pSQIDev);
    }
    else
    {
        return RET_ERROR;
    }
}

/**
 * @brief  Set SQI Flash enable 4line mode
 * @param  pSQIDev:  SQI Device.
 * @retuen  the result
 */

RET_CODE HAL_SQI_MX25_4Address_Enable(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;
    int     timeout;

    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | NOR_EN4B_CMD);
    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0; //trig to send out the command and value
    HAL_SQI_Wait_Idle(pSQIDev);
    rbuf   = HAL_SQI_Read_Status(pSQIDev, MX25R_RDCR);
    timeout = SQI_TIMEOUT;
    while ((!(rbuf & MX25R_4ADDRESS_ENABLE)) && (timeout > 0))
    {
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_Write_Enable(pSQIDev);
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | NOR_EN4B_CMD);
        HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0; //trig to send out the command and value
        HAL_SQI_Wait_Idle(pSQIDev);
        rbuf = HAL_SQI_Read_Status(pSQIDev, MX25R_RDCR);
        timeout--;
        HAL_SQI_Time_Delayms(10);
    }
    if (timeout == 0)
    {
        return RET_TIMEOUT;
    }

    LL_SQI_SET_ADDR_BYTE_NUM(pSQIDev, 1);
    return RET_OK;
}




/*=================================GD25 primary function==================================================*/


__STATIC_INLINE RET_CODE HAL_SQI_GD25_4LINE_Disable(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;
    int     timeout;

    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | GD25R_WRSRM_CMD);
    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x00;
    HAL_SQI_Wait_Idle(pSQIDev);
    rbuf   = HAL_SQI_Read_Status(pSQIDev, GD25R_RDSRM);
    timeout = SQI_TIMEOUT;
    while (((rbuf & GD25_QUAD_ENABLE)) && (timeout > 0))
    {
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_Write_Enable(pSQIDev);
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | GD25R_WRSRM_CMD);
        HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x00;
        HAL_SQI_Wait_Idle(pSQIDev);
        rbuf = HAL_SQI_Read_Status(pSQIDev, GD25R_RDSRM);
        timeout--;
        HAL_SQI_Time_Delayms(10);
    }
    if (timeout == 0)
    {
        return RET_TIMEOUT;
    }
    return RET_OK;

}


/**
 * @brief  Set SQI Flash enable 4line mode
 * @param  pSQIDev:  SQI Device.
 */

__STATIC_INLINE RET_CODE HAL_SQI_GD25_4LINE_Enable(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;
    int     timeout;

    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | GD25R_WRSRM_CMD);
    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x02;
    HAL_SQI_Wait_Idle(pSQIDev);
    rbuf   = HAL_SQI_Read_Status(pSQIDev, GD25R_RDSRM);
    timeout = SQI_TIMEOUT;
    while ((!(rbuf & GD25_QUAD_ENABLE)) && (timeout > 0))
    {
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_Write_Enable(pSQIDev);
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_SET_CFG_REGISTER(pSQIDev,  SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | GD25R_WRSRM_CMD);
        HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x02;
        HAL_SQI_Wait_Idle(pSQIDev);
        rbuf = HAL_SQI_Read_Status(pSQIDev, GD25R_RDSRM);
        timeout--;
        HAL_SQI_Time_Delayms(10);
    }
    if (timeout == 0)
    {
        return RET_TIMEOUT;
    }
    return RET_OK;

}



RET_CODE HAL_SQI_GD25_4LineWrite_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size)
{
    RET_CODE ret;
    ret = HAL_SQI_GD25_4LINE_Enable(pSQIDev);
    if (ret != RET_OK)
    {
        return ret;
    }
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    LL_SQI_SET_RW_IO_MODE(pSQIDev, SQI_MOD5);
    if (flash_size > SFLASH_16M_SIZE)
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT  | SQI_SET_DATA_HIT | GD25R_4PP4B_CMD);
    }
    else
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT  | SQI_SET_DATA_HIT | GD25R_4PP_CMD);
    }

    LL_SQI_FAST_RW_CNT_SET(pSQIDev, count);
    return RET_OK;
}


RET_CODE HAL_SQI_GD25_4LineRead_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size)
{
    RET_CODE ret;

    ret = HAL_SQI_GD25_4LINE_Enable(pSQIDev);
    if (ret != RET_OK)
    {
        return ret;
    }
    LL_SQI_SET_RW_IO_MODE(pSQIDev, SQI_MOD5);
    LL_SQI_FAST_RW_CNT_SET(pSQIDev, count);
    if (flash_size > SFLASH_16M_SIZE)
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_READ_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DMY_HIT | SQI_SET_DATA_HIT | GD25R_QREAD4B_CMD);
    }
    else
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_READ_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DMY_HIT | SQI_SET_DATA_HIT | GD25R_QREAD_CMD);
    }

    return RET_OK;

}

RET_CODE HAL_SQI_GD25_Close_4Line(SQI_Device_T *pSQIDev)
{
    if (pSQIDev)
    {
        return HAL_SQI_GD25_4LINE_Disable(pSQIDev);
    }
    else
    {
        return RET_ERROR;
    }
}

/**
 * @brief  Set SQI Flash enable 4line mode
 * @param  pSQIDev:  SQI Device.
 * @retuen  the result
 */

RET_CODE HAL_SQI_GD25_4Address_Enable(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;
    int     timeout;

    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | NOR_EN4B_CMD);
    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0; //trig to send out the command and value
    HAL_SQI_Wait_Idle(pSQIDev);
    rbuf   = HAL_SQI_Read_Status(pSQIDev, GD25R_RDSRM);
    timeout = SQI_TIMEOUT;
    while ((!(rbuf & GD25_4ADDRESS_ENABLE)) && (timeout > 0))
    {
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_Write_Enable(pSQIDev);
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | NOR_EN4B_CMD);
        HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0; //trig to send out the command and value
        HAL_SQI_Wait_Idle(pSQIDev);
        rbuf = HAL_SQI_Read_Status(pSQIDev, GD25R_RDSRM);
        timeout--;
        HAL_SQI_Time_Delayms(10);
    }
    if (timeout == 0)
    {
        return RET_TIMEOUT;
    }
    LL_SQI_SET_ADDR_BYTE_NUM(pSQIDev, 1);
    return RET_OK;
}

/*=================================MT25 primary function==================================================*/

RET_CODE HAL_SQI_MT25_4LineWrite_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size)
{
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    LL_SQI_SET_RW_IO_MODE(pSQIDev, SQI_MOD3);
    if (flash_size > SFLASH_16M_SIZE)
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT  | SQI_SET_DATA_HIT | MT25R_4PP4B_CMD);
    }
    else
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT  | SQI_SET_DATA_HIT | MT25R_4PP_CMD);
    }

    LL_SQI_FAST_RW_CNT_SET(pSQIDev, count);
    return RET_OK;
}
RET_CODE HAL_SQI_MT25_4LineRead_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size)
{
    HAL_SQI_Wait_Idle(pSQIDev);
    LL_SQI_SET_RW_IO_MODE(pSQIDev, SQI_MOD5);
    LL_SQI_FAST_RW_CNT_SET(pSQIDev, count);
    if (flash_size > SFLASH_16M_SIZE)
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_READ_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DMY_HIT | SQI_SET_DATA_HIT | MT25R_QREAD4B_CMD);
    }
    else
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_READ_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DMY_HIT | SQI_SET_DATA_HIT | MT25R_QREAD_CMD);
    }

    return RET_OK;
}


/**
 * @brief  Set SQI Flash enable 4line mode
 * @param  pSQIDev:  SQI Device.
 * @retuen  the result
 */
RET_CODE HAL_SQI_MT25_4Address_Enable(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;
    int     timeout;

    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | NOR_EN4B_CMD);
    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0; //trig to send out the command and value
    HAL_SQI_Wait_Idle(pSQIDev);
    rbuf   = HAL_SQI_Read_Status(pSQIDev, MT25R_RDNOVCFGRG);
    timeout = SQI_TIMEOUT;
    while ((!(rbuf & MT25_4ADDRESS_ENABLE)) && (timeout > 0))
    {
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_Write_Enable(pSQIDev);
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | NOR_EN4B_CMD);
        HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0; //trig to send out the command and value
        HAL_SQI_Wait_Idle(pSQIDev);
        rbuf = HAL_SQI_Read_Status(pSQIDev, MT25R_RDNOVCFGRG);
        timeout--;
        HAL_SQI_Time_Delayms(10);
    }
    if (timeout == 0)
    {
        return RET_TIMEOUT;
    }
    LL_SQI_SET_ADDR_BYTE_NUM(pSQIDev, 1);
    return RET_OK;

}
/*=================================W25 primary function==================================================*/


/**
 * @brief  Set SQI Flash enable 4line mode
 * @param  pSQIDev:  SQI Device.
 */

__STATIC_INLINE RET_CODE HAL_SQI_W25_4LINE_Enable(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;
    int     timeout;

    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | WB25R_WRSRM_CMD);
    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x02;
    HAL_SQI_Wait_Idle(pSQIDev);
    rbuf   = HAL_SQI_Read_Status(pSQIDev, WB25R_RDSRM);
    timeout = SQI_TIMEOUT;
    while ((!(rbuf & WB25_QUAD_ENABLE)) && (timeout > 0))
    {
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_Write_Enable(pSQIDev);
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | WB25R_WRSRM_CMD);
        HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0x02;
        HAL_SQI_Wait_Idle(pSQIDev);
        rbuf = HAL_SQI_Read_Status(pSQIDev, WB25R_RDSRM);
        timeout--;
        HAL_SQI_Time_Delayms(10);
    }
    if (timeout == 0)
    {
        return RET_TIMEOUT;
    }
    return RET_OK;

}



RET_CODE HAL_SQI_W25_4LineWrite_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size)
{
    RET_CODE ret;
    ret = HAL_SQI_W25_4LINE_Enable(pSQIDev);
    if (ret != RET_OK)
    {
        return ret;
    }
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    LL_SQI_SET_RW_IO_MODE(pSQIDev, SQI_MOD5);
    if (flash_size > SFLASH_16M_SIZE)
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT  | SQI_SET_DATA_HIT | WB25R_4PP4B_CMD);
    }
    else
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT  | SQI_SET_DATA_HIT | WB25R_4PP_CMD);
    }

    LL_SQI_FAST_RW_CNT_SET(pSQIDev, count);
    return RET_OK;

}


RET_CODE HAL_SQI_W25_4LineRead_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size)
{
    RET_CODE ret;
    ret = HAL_SQI_W25_4LINE_Enable(pSQIDev);
    if (ret != RET_OK)
    {
        return ret;
    }
    LL_SQI_SET_RW_IO_MODE(pSQIDev, SQI_MOD5);
    LL_SQI_FAST_RW_CNT_SET(pSQIDev, count);
    if (flash_size > SFLASH_16M_SIZE)
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_READ_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DMY_HIT | SQI_SET_DATA_HIT | WB25R_QREAD4B_CMD);
    }
    else
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_READ_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DMY_HIT | SQI_SET_DATA_HIT | WB25R_QREAD_CMD);
    }

    return RET_OK;
}

/**
 * @brief  Set SQI Flash enable 4line mode
 * @param  pSQIDev:  SQI Device.
 * @retuen  the result
 */

RET_CODE HAL_SQI_W25_4Address_Enable(SQI_Device_T *pSQIDev)
{
    uint8_t rbuf;
    int     timeout;

    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | NOR_EN4B_CMD);
    HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0; //trig to send out the command and value
    HAL_SQI_Wait_Idle(pSQIDev);
    rbuf   = HAL_SQI_Read_Status(pSQIDev, WB25R_RDSRH);
    timeout = SQI_TIMEOUT;
    while ((!(rbuf & WB25_4ADDRESS_ENABLE)) && (timeout > 0))
    {
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_Write_Enable(pSQIDev);
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | NOR_EN4B_CMD);
        HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE) = 0; //trig to send out the command and value
        HAL_SQI_Wait_Idle(pSQIDev);
        rbuf = HAL_SQI_Read_Status(pSQIDev, WB25R_RDSRH);
        timeout--;
        HAL_SQI_Time_Delayms(10);
    }
    if (timeout == 0)
    {
        return RET_TIMEOUT;
    }
    LL_SQI_SET_ADDR_BYTE_NUM(pSQIDev, 1);
    return RET_OK;
}


/*=================================public primary function==================================================*/



/**
 * @brief  SQI Flash write enable operation.
 * @param  pSQIDev:  SQI Device.
 * @param  count:  data size
 * @retval HAL status
 */

void HAL_SQI_NormalWrite_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size)
{

    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_Wait_Idle(pSQIDev);
    if (flash_size > SFLASH_16M_SIZE)
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DATA_HIT | NOR_WRITE4B_CMD);
    }
    else
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_WRITE_EN | SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DATA_HIT | NOR_WRITE_CMD);
    }

    LL_SQI_FAST_RW_CNT_SET(pSQIDev, count);

}


/**
 * @brief  SQI Flash read enable operation.
 * @param  pSQIDev:  SQI Device.
 * @retval HAL status
 */

void HAL_SQI_NormalRead_Enable(SQI_Device_T *pSQIDev, uint32_t flash_size)
{

    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_Wait_Idle(pSQIDev);
    if (flash_size > SFLASH_16M_SIZE)
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DATA_HIT | NOR_READ4B_CMD);
    }
    else
    {
        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | SQI_SET_DATA_HIT | NOR_READ_CMD);
    }

}


/**
 * @brief  clear flash register s0~s7
 * @param  pSQIDev:  SQI Device.
  * @retval SFlash ID
 */


void HAL_SQI_Clear_SR(SQI_Device_T *pSQIDev)
{
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Enable(pSQIDev);
    HAL_SQI_Wait_Idle(pSQIDev);
    HAL_SQI_Write_Status(pSQIDev, NORR_WRSR, 0x00);
    HAL_SQI_Wait_Idle(pSQIDev);
}





/**
 * @brief  read ID
 * @param  pSQIDev:  SQI Device.
  * @retval SFlash ID
 */
uint32_t HAL_SQI_ReadID(SQI_Device_T *pSQIDev)
{
    uint32_t id = 0;
    int       i;
    i = 0;
    HAL_SQI_Wait_Idle(pSQIDev);
    LL_SQI_FAST_RW_CNT_SET(pSQIDev, 4);
    HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_FAST_RW_CNT_EN | SQI_CONTINUE_READ_EN | SQI_SET_CODE_HIT | SQI_SET_DATA_HIT | NOR_READ_ID_CMD);
    for (i = 0; i < 4; i++)
    {
        id  |= HW8_REG(CONFIG_SQI_SFLASH_MMAP_BASE);
        if (i < 3)
        {
            id = id << 8;
        }
    }
    return id;
}

/**
 * @brief  SQI Flash erase operation.
 * @param  pSQIDev:  SQI Device.
 * @param  offset:  the address of flash.
 * @param  size:  erase size.
 * @retval HAL status
 */
RET_CODE HAL_SQI_Erase_Internal(SQI_Device_T *pSQIDev, uint32_t offset, uint32_t size, uint32_t flash_size)
{

    uint8_t     erase_opcode;
    uint32_t    count  = 1;
    uint32_t    i = 0;
    /*1. WREN instruction must execute to set the Write Enable Latch (WEL) bit
        before sending the Sector Erase (SE)*/
    if (flash_size > SFLASH_16M_SIZE)
    {
        erase_opcode = NOR_SE4B_CMD;
    }
    else
    {
        switch (size)
        {
            case SFLASH_SECTOR_LEN:
                erase_opcode = NOR_SE_CMD;
                break;
            default:
                erase_opcode = NOR_SE_CMD;
                break;
        }
    }

    if (size % SFLASH_SECTOR_LEN == 0)
    {
        count = size / SFLASH_SECTOR_LEN;
    }
    else
    {
        count = size / SFLASH_SECTOR_LEN + 1;
    }


    for (i = 0; i < count; i++)
    {
        HAL_SQI_Wait_Idle(pSQIDev);
        HAL_SQI_Write_Enable(pSQIDev);
        HAL_SQI_Wait_Idle(pSQIDev);

        HAL_SQI_SET_CFG_REGISTER(pSQIDev, SQI_SET_CODE_HIT | SQI_SET_ADDR_HIT | erase_opcode);

        HW32_REG(CONFIG_SQI_SFLASH_MMAP_BASE + offset + i * SFLASH_SECTOR_LEN) = 0x0;

        HAL_SQI_Wait_Idle(pSQIDev);
    }

    return RET_OK;

}



/**
 * @brief  config div
 * @param  hSQI:  SQI Handle.
  * @retval result
 */
RET_CODE HAL_SQI_Div_Config(SQI_Device_T *pSQIDev, uint32_t div)
{
    if (pSQIDev)
    {
        LL_SQI_SET_CLK_DIV(pSQIDev, div);
    }
    else
    {
        return RET_ERROR;
    }

    return RET_OK;
}








