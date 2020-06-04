/**
*
* Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
*
* @file        gm_hal_dac.c
*
* @author      Richard.Liu
*
* @version     v1.0.0
*
* @date        2019/01/24
*
* @brief       DAC source file
*
* @note
*              2019/01/24, Richard.Liu, v1.0.0
*                  Initial version.
*
**/

#include <gm_hal_dac.h>
#include <gm_hal_dma.h>
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>

unsigned int HAL_DAC_GetDataOut(DAC_Handle_T *pHandle)
{
    return pHandle->device->DAC_DO;
}

RET_CODE HAL_DAC_Triger(DAC_Handle_T *pHandle, DAC_Config_T *pConfig)
{
#if defined(CHIP_GM6721)
    DAC_ComDevice_T *pDACCOM = DAC_COM_DEV;
#endif
    DAC_Device_T *pDAC = pHandle->device;

    if (pConfig->triggerMode == DAC_SINGLE_SOFT_TRIG)
    {
        DAC_SOFT_TRIG(pDAC);
    }
    else if (pConfig->triggerMode == DAC_HARDWARE_TRIG)
    {
        DAC_HARD_TRIG(pDAC);
    }
#if defined(CHIP_GM6721)
    else if (pConfig->triggerMode == DAC_DUAL_SOFTWARE_TRIG)
    {
        DAC_DUAL_SOFT_TRIG(pDACCOM);
    }
#endif
    return RET_OK;
}

/**
  * @brief  The following  function are used to reset DAC peripheral clock.
  * @note  software has to reset peripheral  before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_DAC_SW_Reset(void)
{
    HAL_Reset_Module(RESET_DAC_SW_RSTJ);
}

/**
  * @brief  The following two functions are used to enable/disable peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application
  *         software has to enable clock before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_DAC_CLK_ENABLE(void)
{
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_DAC);
}

void HAL_DAC_CLK_DISABLE(void)
{
    HAL_CLOCK_PeripheralClockDisable0(SYS_ENCLK0_DAC);
}

RET_CODE HAL_DAC_Init(DAC_Handle_T *pHandle, DAC_Device_T *pDAC, DAC_Config_T *pConfig)

{
    RET_CODE ret = RET_OK;
#if defined(CHIP_GM6721)
    DAC_ComDevice_T *pDACCOM = DAC_COM_DEV;
#endif

    if (!(pHandle && pDAC && pConfig))
    {
        ret = RET_INVPARAM;
        goto out;
    }

    pHandle->device = pDAC;

    DAC_CLK_SEL(LIRC_CLOCK);
    HAL_DAC_CLK_ENABLE();
    HAL_DAC_SW_Reset();

    DAC_DISABLE(pHandle->device);

    if (pConfig->waveType == DAS_WAVE_NOISE_EN)
    {
        DAC_SET_WAVE_TYPE(pDAC, pConfig->waveType);
    }
    else if (pConfig->waveType == DAS_WAVE_TRIANGLE_EN)
    {
        DAC_SET_WAVE_TYPE(pDAC, pConfig->waveType);
    }
    else
    {
        DAC_SET_WAVE_TYPE(pDAC, DAC_WAVE_DISABLE);
    }

    DAC_SET_DATA_MODE(pDAC, pConfig->resolution);

    DAC_SET_MS_AM(pDAC, pConfig->msAM);

    if (pConfig->dacSel == DAC0_SEL || pConfig->dacSel == DAC1_SEL)
    {
        DAC_SET_DATAHOLD_REG(pDAC, pConfig->dacComConfig.dacHoldData);
    }
#if defined(CHIP_GM6721)
    else if (pConfig->dacSel == DAC_DUAL && (pConfig->triggerMode == DAC_DUAL_SOFTWARE_TRIG))
    {
        DAC0_SET_DOUL_HOLD_DATA(pDACCOM, pConfig->dacComConfig.dacDAC0DualHoldData);
        DAC1_SET_DOUL_HOLD_DATA(pDACCOM, pConfig->dacComConfig.dacDAC1DualHoldData);
    }
#endif
    else
    {
        return RET_ERROR;
    }

    if (pConfig->samholdEn == ENABLE)
    {
        DAC_SET_SAMPLE_HOLD_SAME_TIME(pDAC, pConfig->dacComConfig.dacHoldSampTime);
        DAC_SET_SAMPLE_HOLD_HOLD_TIME(pDAC, pConfig->dacComConfig.dacHoldTime);
        DAC_SET_SAMPLE_HOLD_REFRESH_TIME(pDAC, pConfig->dacComConfig.dacHoldRefTime);
    }

    if (pConfig->dacEn == ENABLE)
    {
        DAC_ENABLE(pDAC);
    }
out:
    return ret;
}

RET_CODE HAL_DAC_UnInit(DAC_Handle_T *pHandle, DAC_Config_T *pConfig)

{
    RET_CODE ret = RET_OK;
#if defined(CHIP_GM6721)
    DAC_ComDevice_T *pDACCOM = DAC_COM_DEV;
#endif
    DAC_Device_T *pDAC;

    pDAC = pHandle->device;

    if (!(pHandle && pDAC && pConfig))
    {
        ret = RET_INVPARAM;
        goto out;
    }

    DAC_DISABLE(pHandle->device);

    DAC_SET_WAVE_TYPE(pDAC, DAC_WAVE_DISABLE);

    DAC_SET_DATA_MODE(pDAC, 0);

    DAC_SET_MS_AM(pDAC, 0);

    if (pConfig->dacSel == DAC0_SEL || pConfig->dacSel == DAC1_SEL)
    {
        DAC_SET_DATAHOLD_REG(pDAC, 0);
    }
#if defined(CHIP_GM6721)
    else if (pConfig->dacSel == DAC_DUAL && (pConfig->triggerMode == DAC_DUAL_SOFTWARE_TRIG))
    {
        DAC0_SET_DOUL_HOLD_DATA(pDACCOM, 0);
        DAC1_SET_DOUL_HOLD_DATA(pDACCOM, 0);
    }
#endif
    else
    {
        return RET_ERROR;
    }

    if (pConfig->samholdEn == ENABLE)
    {
        DAC_SET_SAMPLE_HOLD_SAME_TIME(pDAC, pConfig->dacComConfig.dacHoldSampTime);
        DAC_SET_SAMPLE_HOLD_HOLD_TIME(pDAC, pConfig->dacComConfig.dacHoldTime);
        DAC_SET_SAMPLE_HOLD_REFRESH_TIME(pDAC, pConfig->dacComConfig.dacHoldRefTime);
    }

    HAL_DAC_CLK_DISABLE();

out:
    return ret;
}



#ifdef DEBUG_DAC_FPGA
RET_CODE HAL_DAC_Result_Check(int data_out, int expect_data_out, int convert_mode)
{
    float   convert_result;
    float   expect_result;
    int error = 0;
    int data_mask   = 0xFFF, data_mask_10 = 0x3FF, data_mask_8 = 0xFF;
    float vref  = 4096, vref_10 = 1024, vref_8 = 256;


    convert_result  = ((data_out + 1) / vref) * 5;

    if (convert_mode == 12)
    {
        expect_result   = (((expect_data_out & data_mask) + 1) / vref) * 5;
    }
    else if (convert_mode == 10)
    {
        expect_result   = (((expect_data_out & data_mask_10) + 1) / vref_10) * 5;
    }
    else if (convert_mode == 8)
    {
        expect_result   = (((expect_data_out & data_mask_8) + 1) / vref_8) * 5;
    }

    if (convert_result != expect_result)
    {
        PR_DEBUG("**Error : convert result != expect result\n");
        error += 1;
    }
    else
    {
        PR_DEBUG("**Success : convert result = expect result\n");
    }

    PR_DEBUG("  ======expect_result   = %f ======\n", expect_result);
    PR_DEBUG("  ======convert_result  = %f ======\n\n", convert_result);

    return  error;
}

RET_CODE HAL_DAC_Noise_Result(int data_hold, int mask, int shift_num)
{
    int i = 0;
    int shift_value = 0xAAA;
    int shift_value_12  = 0;
    int shift_value_all = 0;
    int shift_value_0   = 0;
    int shift_value_1   = 0;
    int shift_value_4   = 0;
    int shift_value_6   = 0;
    int data_out    = 0;

    if (shift_num == 0)
    {
        data_out    = data_hold + (shift_value & mask);
    }
    else
    {
        for (i = 0 ; i < shift_num ; i++)
        {
            shift_value_all = shift_value > 0 ? 0 : 1;
            shift_value_0   = shift_value & (1 << 0);
            shift_value_1   = (shift_value & (1 << 1)) >> 1;
            shift_value_4   = (shift_value & (1 << 4)) >> 4;
            shift_value_6   = (shift_value & (1 << 6)) >> 6;
            shift_value_12  = shift_value_all ^ shift_value_0 ^ shift_value_1 ^ shift_value_4 ^ shift_value_6;

            shift_value = ((shift_value >> 1) & 0xFFF) | (shift_value_12 << 11);

            data_out    = (data_hold + (shift_value & mask)) & 0xFFF;
        }
    }
    return data_out;
}
#endif
