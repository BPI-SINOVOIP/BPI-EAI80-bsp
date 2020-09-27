/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          lvd_demo.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2017/06/15
 *
 * @brief        Test Code for Low Voltage Detect on Board GM6611/6601B/GM6611B
 *
 *                  GM6611B:
 *                  common usage:
 *                      VD2 > VD1 > VD3
 *                  use VD2 to warning that a dangerous voltage occurs, then VD1 to reset the chip.
 *                  (vd2/vd1 - vcc)
 *                  (vd2/vd1 - vdd-core)
 * @note
 *    2017/06/15, Angrad.Yang, V1.0.0
 *        Initial version.
 */
#include <ugelis.h>
#include <misc/printk.h>
#include <gm_soc.h>
#include <gm_hal_lvd.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
#define DBG_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printf
#endif

static int vd1_level = LVD_VD1_Level_4;
static int vd2_level = LVD_VD2_Level_5;
static int vd3_level = LVD_VD3_Level_3;

static int enable_vd1 = 1;
static int enable_vd2 = 1;
static int enable_vd3 = 1;

static int enable_vd1_reset = 1;
static int enable_vd3_reset = 1;


static float lvd1_level[] =
{
    1.70, 1.80, 2.00, 2.20,
    2.40, 2.60, 2.80, 3.00,
};

static float lvd2_level[] =
{
    1.80, 1.90, 2.00, 2.20,
    2.40, 2.60, 2.80, 3.00,
};

static float lvd3_level[] =
{
    0.90, 0.95, 1.00, 1.05,
};

void lvd_callback(int arg)
{
    PR_DEBUG("\n>>>>>Lvd2 Callback\n");
}

void print_info(int index, int on, float level, int reset, int interrupt)
{
    printf("lvd%d:  %-3s  %-.2fv  %10s\n",
           index,
           on ? "on" : "off",
           level,
           reset ? "reset" : (interrupt ? "interrupt" : "")
          );
}

void lvd_information()
{
    PR_DEBUG("-------------------------------------\n");
    print_info(LVD_VOLDT_1, enable_vd1, lvd1_level[vd1_level], enable_vd1_reset, 0);
    print_info(LVD_VOLDT_2, enable_vd2, lvd2_level[vd2_level], 0, 1);
    print_info(LVD_VOLDT_3, enable_vd3, lvd3_level[vd3_level], enable_vd3_reset, 0);
    PR_DEBUG("-------------------------------------\n");
}

int main(void)
{
    LVD_Config_T lvdInfo1, lvdInfo2, lvdInfo3;
    LVD_Config_T *pVd1, *pVd2, *pVd3;

    memset(&lvdInfo1, 0, sizeof(LVD_Config_T));
    memset(&lvdInfo2, 0, sizeof(LVD_Config_T));
    memset(&lvdInfo3, 0, sizeof(LVD_Config_T));

    pVd1 = pVd2 = pVd3 = NULL;

    PR_DEBUG("\n\nlvd test start......\n");

    if (HAL_LVD_CheckResetStatus())
    {
        PR_DEBUG("(last reset is caused by VOL_DET or POR)\n");
        HAL_LVD_ClearResetStatus();
    }

    lvdInfo1.enable     = enable_vd1;
    lvdInfo2.enable     = enable_vd2;
    lvdInfo3.enable     = enable_vd3;

    lvdInfo2.threshold  =  0x1;
    lvdInfo2.int_call   = lvd_callback;

    lvdInfo1.detectionLevel = vd1_level;
    lvdInfo2.detectionLevel = vd2_level;
    lvdInfo3.detectionLevel = vd3_level;

    lvd_information();

    lvdInfo1.reset          = enable_vd1_reset;
    lvdInfo2.reset          = 0;            //vd2 has no reset function.
    lvdInfo3.reset          = enable_vd3_reset;

    if (lvdInfo1.enable)
    {
        pVd1 = &lvdInfo1;
    }
    if (lvdInfo2.enable)
    {
        pVd2 = &lvdInfo2;
    }
    if (lvdInfo3.enable)
    {
        pVd3 = &lvdInfo3;
    }

    HAL_LVD_Init(pVd1, pVd2, pVd3);

    //only vd2 has interrupt function.
    HAL_LVD_EnableInterrupt(LVD_VOLDT_2);

    while (1)
    {
        if (HAL_LVD_GetVDXFlag(LVD_VOLDT_1))
        {
            PR_DEBUG("Current VCCIO < VD1LVL\n");
        }
        if (HAL_LVD_GetVDXFlag(LVD_VOLDT_2))
        {
            PR_DEBUG("Current VCCIO < VD2LVL\n");
        }
        if (HAL_LVD_GetVDXFlag(LVD_VOLDT_3))
        {
            PR_DEBUG("Current VDDCORE < VD3LVL\n");
        }
        k_sleep(500);
        PR_DEBUG(".");
    }
}



