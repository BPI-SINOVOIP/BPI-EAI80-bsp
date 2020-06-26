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
#include <lvd.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
#define DBG_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printf
#endif

struct device *lvd_dev;

static int vd1_level = LVD_VD1_Level_6;
static int vd2_level = LVD_VD2_Level_7;
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
    PR_DEBUG("\n!!!Low Voltage Warning!!!\n");
}

void print_info(int index, int on, float level, int reset, int interrupt)
{
    PR_DEBUG("lvd%d:  %-3s  %-.2fv  %10s\n",
             index,
             on ? "on" : "off",
             level,
             reset ? "reset" : (interrupt ? "interrupt" : "")
            );
}

void lvd_information()
{
    PR_DEBUG("-------------------------------------\n");
    print_info(LVD_1, enable_vd1, lvd1_level[vd1_level], enable_vd1_reset, 0);
    print_info(LVD_2, enable_vd2, lvd2_level[vd2_level], 0, 1);
    print_info(LVD_3, enable_vd3, lvd3_level[vd3_level], enable_vd3_reset, 0);
    PR_DEBUG("-------------------------------------\n");
}

static void basic_init(LVD_Config *pVD1, LVD_Config *pVD2, LVD_Config *pVD3)
{
    memset(pVD1, 0, sizeof(LVD_Config));
    memset(pVD2, 0, sizeof(LVD_Config));
    memset(pVD3, 0, sizeof(LVD_Config));

    pVD1->enable    = 1;
    pVD1->threshold = 0x1;
    pVD1->int_call  = lvd_callback;

    //vd2 has no reset function.
    pVD1->reset = TRUE;

    *pVD2 = *pVD1;
    *pVD3 = *pVD1;

    pVD1->detectionLevel = vd1_level;
    pVD2->detectionLevel = vd2_level;
    pVD3->detectionLevel = vd3_level;

    lvd_information();
}

int main(void)
{
    PR_DEBUG("\n\nLvd Test Start\n");
    lvd_dev = device_get_binding("GMLVD");
    if (!lvd_dev)
    {
        PR_DEBUG("Cann't get GM-LVD Device\n");
    }

    lvd_cfg lvd_info;
    memset(&lvd_info, 0, sizeof(lvd_cfg));

    if (lvd_check_reset_status(lvd_dev))
    {
        PR_DEBUG("(last reset is caused by VOL_DET or POR)\n");
        lvd_clear_reset_status(lvd_dev);
    }

    basic_init(&lvd_info.lvd1, &lvd_info.lvd2, &lvd_info.lvd3);

    lvd_enable(lvd_dev, &lvd_info);

    while (1)
    {
        if (lvd_get_VDX_flag(lvd_dev, LVD_1))
        {
            PR_DEBUG("Current VCCIO < VD1LVL\n");
        }
        if (lvd_get_VDX_flag(lvd_dev, LVD_2))
        {
            PR_DEBUG("Current VCCIO < VD2LVL\n");
        }
        if (lvd_get_VDX_flag(lvd_dev, LVD_3))
        {
            PR_DEBUG("Current VDDCORE < VD3LVL\n");
        }
        k_sleep(500);
        PR_DEBUG(".");
    }
}



