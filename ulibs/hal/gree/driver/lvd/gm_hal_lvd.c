/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_lvd.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.2
 *
 * @date          2019/02/28
 *
 * @brief        LVD APIs for application.
 *
 * @note
 *    2016/08/22, mark.lee,     V1.0.0      Initial version.
 *    2017/05/19, Angrad.Yang,  V1.0.1      Modified LVD_Init to enable that VD1 and VD2 can work together.
 *    2019/02/28, Angrad.Yang,  V1.0.2      Modification for GM6721.
 *
 */
#include <gm_soc.h>
#include <gm_hal_lvd.h>
#include <gm_irq.h>

#include "gm_ll_lvd.h"


static LVD_Int_Callback hal_lvd_user_cb = NULL;

void HAL_LVD_IRQHandler(void)
{
    /* only vd2 has interrupt */
    if (IO_BITS_GET(LVD->VD2SR, HAL_LVD_VD2_FLAG))
    {
        HAL_LVD_ClearInterruptFlag(LVD_VOLDT_2);
        if (hal_lvd_user_cb)
        {
            hal_lvd_user_cb(2);
        }
    }
}

int HAL_LVD_GetVDXFlag(int source)
{
    if (LVD_VOLDT_1 == source)
    {
        return IO_BITS_GET(LVD->VD1SR, HAL_LVD_VD1_OUT);
    }
    else if (LVD_VOLDT_2 == source)
    {
        return IO_BITS_GET(LVD->VD2SR, HAL_LVD_VD2_OUT);
    }
    else if (LVD_VOLDT_3 == source)
    {
        return IO_BITS_GET(LVD->VD3SR, HAL_LVD_VD3_OUT);
    }
    return 0;
}



//VD1, VD2:     Dedicated for VCCIO detection
//VD3:          Dedicated for VDDCORE detection

//VD1EN and VD3EN are set to "1" by default.
//VD2EN is cleared to "0" immediately after a power-on reset or a reset by an external reset input is released.

//VD1 and VD3 support reset chipset signal generation when supply voltage is lower than selected level.
//VD2 supports interrupt generation when supply voltage is lower than selected level.
void HAL_LVD_Init(const LVD_Config_T *pConfig1, const LVD_Config_T *pConfig2,
                  const LVD_Config_T *pConfig3)
{
    uint32_t ctrl1, ctrl2;
    uint32_t mask1, mask2;
    uint32_t ctrl3;
    uint32_t mask3;
    ctrl1 = ctrl2 = 0;
    mask1 = mask2 = 0;
    ctrl3 = 0;
    mask3 = 0;

    hal_lvd_user_cb = pConfig2->int_call;

    if (pConfig1)
    {
        ctrl1 |= HAL_LVD_VD1_LEVEL_FIELD(pConfig1->detectionLevel);
        ctrl1 |= ((pConfig1->enable) ? HAL_LVD_VD1_EN : 0);
        mask1 |=  HAL_LVD_VD1_LEVEL | HAL_LVD_VD1_EN;
    }
    if (pConfig2)
    {
        ctrl2 |= HAL_LVD_VD2_THR_FIELD(pConfig2->threshold);
        ctrl2 |= HAL_LVD_VD2_LEVEL_FIELD(pConfig2->detectionLevel);
        ctrl2 |= ((pConfig2->enable) ? HAL_LVD_VD2_EN : 0);
        mask2 |= HAL_LVD_VD2_THR | HAL_LVD_VD2_LEVEL | HAL_LVD_VD2_EN;
    }

    if (pConfig1)
    {
        IO_BITMASK_SET(LVD->VD1CR, mask1, ctrl1);
    }
    if (pConfig2)
    {
        IO_BITMASK_SET(LVD->VD2CR, mask2, ctrl2);
    }

    if (pConfig3)
    {
        ctrl3 |= HAL_LVD_VD3_LEVEL_FIELD(pConfig3->detectionLevel);
        ctrl3 |= ((pConfig3->enable) ? HAL_LVD_VD3_EN : 0);
        mask3 |= HAL_LVD_VD3_LEVEL | HAL_LVD_VD3_EN;
        IO_BITMASK_SET(LVD->VD3CR, mask3, ctrl3);
    }

    mask1 = ctrl1 = mask2 = ctrl2 = mask3 = ctrl3 = 0;

    if (pConfig1)
    {
        while (HAL_LVD_GetVDXFlag(LVD_VOLDT_1))
        {
            ;
        }
        mask1 = HAL_LVD_VD1_RSTEN;
        ctrl1 = ((pConfig1->reset) ? HAL_LVD_VD1_RSTEN : 0);
        IO_BITMASK_SET(LVD->VD1CR, mask1, ctrl1);
    }

    if (pConfig3)
    {
        while (HAL_LVD_GetVDXFlag(3))
        {
            ;
        }
        ctrl3 = ((pConfig3->reset) ? HAL_LVD_VD3_RSTEN : 0);
        mask3 = HAL_LVD_VD3_RSTEN;
        IO_BITMASK_SET(LVD->VD3CR, mask3, ctrl3);
    }

    HAL_RequestIrq(LVD_IRQn, LVD_IRQ_PRIORITY, HAL_LVD_IRQHandler, 0);

    //only vd2 has interrupt function.
    HAL_LVD_EnableInterrupt(LVD_VOLDT_2);

}


/* NOTICE: only vd2 supports interrput on GM6721 */
void HAL_LVD_EnableInterrupt(LVD_VoltageDetection_T voltage)
{
    if (voltage == LVD_VOLDT_2)
    {
        /* Enable voltage detector 2 interrupt */
        //IO_BITS_SET(LVD->VD2CR, LVD_CR_VOLDT2_INTEN);
    }
    HAL_IRQ_ENABLE(LVD_IRQn);
}

void HAL_LVD_DisableInterrupt(LVD_VoltageDetection_T voltage)
{
    if (voltage == LVD_VOLDT_2)
    {
        /* Disable voltage detector 2 interrupt */
        //IO_BITS_CLEAR(LVD->VD2CR, LVD_CR_VOLDT2_INTEN);
    }
    HAL_IRQ_DISABLE(LVD_IRQn);
}

void HAL_LVD_ClearInterruptFlag(LVD_VoltageDetection_T voltage)
{
    if (voltage == LVD_VOLDT_2)
    {
        /* Clear voltage detector 2 interrupt flag */
        IO_BITS_SET(LVD->VD2SR, HAL_LVD_VD2_FLAG);
    }
}

// Lock the configuration of voltage detector.
// One-way bit which can be reset by LVD reset.
// Once lock, the bits LEVEL, RSTEN, EN and THR cannot be re-modified.
void HAL_LVD_Lock(LVD_VoltageDetection_T voltage)
{
    if (voltage == LVD_VOLDT_1)
    {
        IO_BITS_SET(LVD->VD2SR, HAL_LVD_VD1_LOCK);
    }
    else if (voltage == LVD_VOLDT_2)
    {
        IO_BITS_SET(LVD->VD2SR, HAL_LVD_VD2_LOCK);
    }
    else if (voltage == LVD_VOLDT_3)
    {
        IO_BITS_SET(LVD->VD2SR, HAL_LVD_VD3_LOCK);
    }
}

void HAL_LVD_ResetLevel(LVD_VoltageDetection_T voltage, int level)
{
    uint32_t ctrl;
    uint32_t mask;
    ctrl = mask = 0;

    if (voltage == LVD_VOLDT_1)
    {
        ctrl |= HAL_LVD_VD1_LEVEL_FIELD(level);
        mask |=  HAL_LVD_VD1_LEVEL;
        IO_BITMASK_SET(LVD->VD1CR, mask, ctrl);
    }
    else if (voltage == LVD_VOLDT_2)
    {
        ctrl |= HAL_LVD_VD2_LEVEL_FIELD(level);
        mask |=  HAL_LVD_VD2_LEVEL;
        IO_BITMASK_SET(LVD->VD2CR, mask, ctrl);
    }
    else if (voltage == LVD_VOLDT_3)
    {
        ctrl |= HAL_LVD_VD3_LEVEL_FIELD(level);
        mask |=  HAL_LVD_VD3_LEVEL;
        IO_BITMASK_SET(LVD->VD3CR, mask, ctrl);
    }
}

int HAL_LVD_CheckResetStatus()
{
    return IO_BITS_GET(LVDStatus->RST_STATUS, HAL_LVD_RST_STATUS_LVD);
}

void HAL_LVD_ClearResetStatus()
{
    IO_BITMASK_SET(LVDStatus->RST_STATUS, HAL_LVD_RST_STATUS_LVD, HAL_LVD_RST_STATUS_LVD);
}
