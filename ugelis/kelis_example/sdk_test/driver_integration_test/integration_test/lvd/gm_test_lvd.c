/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       adc demo source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <device.h>
#include <gm_common.h>
#include <board.h>
#include <ztest.h>
#include <gm_hal_lvd.h>
#include "gm_test_lvd.h"

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printk
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printk
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printk
#endif
#include <ugelis.h>
#include <device.h>
#include <gm_hal_lvd.h>
#include "board.h"
#include <ztest.h>
#include "gm_test_lvd.h"
#include <lvd.h>

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

static int lvd2_int_flag = 0;

static void gm_test_lvd_callback(int arg)
{
    lvd2_int_flag = 1;
    TC_PRINT("\n>>>>>Lvd2 Callback\n");
}

static void gm_test_lvd_print_info(int index, int on, float level, int reset, int interrupt)
{
    TC_PRINT("lvd%d:  %-3s  %-.2fv  %10s\n",
             index,
             on ? "on" : "off",
             level,
             reset ? "reset" : (interrupt ? "interrupt" : "")
            );
}

static int gm_test_lvd_query()
{
    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Lvd Query Mode Test\n");
    TC_PRINT("----------------------------------\n");

    struct device *lvd_dev;
    lvd_cfg data;

    int j = 0;

    lvd_dev = device_get_binding("GMLVD");
    if (!lvd_dev)
    {
        TC_PRINT("Cann't get GM-IWDG Device\n");
        return TC_FAIL;
    }


    /* Verify VD1 */
    memset(&data, 0, sizeof(lvd_cfg));
    data.lvd1.enable          = 1;
    data.lvd1.threshold       = 0x1;

    /*
     * LVD_VD1_Level_0 could not be test on sram,
     * becuase evb board could not work well below 1.75v.
     *
     * LVD_VD1_Level_0 LVD_VD1_Level_1 could not be test on sdram,
     * becuase evb board could not work well below 2.2v.
     */
    for (j = LVD_VD1_Level_7; j >= LVD_VD1_Level_4; j--)
    {
        data.lvd1.detectionLevel  = j;
        while (lvd_get_VDX_flag(lvd_dev, LVD_VOLDT_1))
        {
            TC_PRINT("!!Warning!! Pleases raise the voltage to eliminate this warning, then reduce the voltage again\n");
            k_sleep(1000);
        }
        lvd_enable(lvd_dev, &data);
        TC_PRINT("level(%d/%d)\t", j, LVD_VD1_Level_7);
        gm_test_lvd_print_info(1, 1, lvd1_level[j], 0, 0);
        while (1)
        {
            if (lvd_get_VDX_flag(lvd_dev, LVD_VOLDT_1))
            {
                TC_PRINT("Current VCCIO < VD1LVL\n");
                break;
            }
            k_sleep(500);
            TC_PRINT(".");
        }
    }
    data.lvd1.detectionLevel  = LVD_VD1_Level_0;
    TC_PRINT("!!Warning!! Pleases raise the voltage to 3.3v, so that vd2 test could be started.\n");
    lvd_enable(lvd_dev, &data);
    while (lvd_get_VDX_flag(lvd_dev, LVD_VOLDT_1))
    {
        TC_PRINT("!!Warning!! Pleases raise the voltage to 3.3v, so that vd2 test could be started.\n");
        k_sleep(1000);
    }

    /* Verify VD2 */
    data.lvd2.enable          = 1;
    data.lvd2.threshold       = 0x1;
    data.lvd2.int_call        = gm_test_lvd_callback;

    for (j = LVD_VD2_Level_7; j >= LVD_VD2_Level_4; j--)
    {
        data.lvd2.detectionLevel  = j;
        while (lvd_get_VDX_flag(lvd_dev, LVD_VOLDT_2))
        {
            TC_PRINT("!!Warning!! Pleases raise the voltage to eliminate this warning, then reduce the voltage again\n");
            k_sleep(1000);
        }
        lvd_enable(lvd_dev, &data);
        TC_PRINT("level(%d/%d)\t", j, LVD_VD2_Level_7);
        gm_test_lvd_print_info(2, 1, lvd2_level[j], 0, 0);
        while (1)
        {
            if (lvd_get_VDX_flag(lvd_dev, LVD_VOLDT_2))
            {
                TC_PRINT("Current VCCIO < VD2LVL\n");
                break;
            }
            k_sleep(500);
            TC_PRINT(".");
        }
    }

    data.lvd2.detectionLevel  = LVD_VD2_Level_0;
    TC_PRINT("!!Warning!! Pleases raise the voltage to 3.3v, so that reset test could be started.\n");
    lvd_enable(lvd_dev, &data);
    while (lvd_get_VDX_flag(lvd_dev, LVD_VOLDT_2))
    {
        TC_PRINT("!!Warning!! Pleases raise the voltage to 3.3v, so that reset test could be started.\n");
        k_sleep(1000);
    }
    data.lvd2.enable          = 0;
    lvd_enable(lvd_dev, &data);

    return TC_PASS;
}

static int gm_test_lvd_reset()
{
    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Lvd Reset Test\n");
    TC_PRINT("----------------------------------\n");

    struct device *lvd_dev;
    lvd_cfg data;

    lvd_dev = device_get_binding("GMLVD");
    if (!lvd_dev)
    {
        TC_PRINT("Cann't get GM-IWDG Device\n");
        return TC_FAIL;
    }

    TC_PRINT("!!Warning!! Wait 10s to raise the voltage to 3.3v\n");
    int time = 10;
    while (time--)
    {
        TC_PRINT(". ");
        k_sleep(1000);
    }

    if (lvd_check_reset_status(lvd_dev))
    {
        TC_PRINT("Last reset is caused by VOL_DET or POR\n");
        lvd_clear_reset_status(lvd_dev);
        return TC_PASS;
    }

    TC_PRINT("Reset function is on, reduce the voltage to %-.2fv \n", lvd1_level[LVD_VD1_Level_6]);
    gm_test_lvd_print_info(1, 1, lvd1_level[LVD_VD1_Level_6], 1, 0);
    /* Verify VD1 */
    memset(&data, 0, sizeof(lvd_cfg));
    data.lvd1.enable          = 1;
    data.lvd1.threshold       = 0x1;
    data.lvd1.reset           = 1;
    data.lvd1.detectionLevel  = LVD_VD1_Level_6;
    lvd_enable(lvd_dev, &data);

    return TC_PASS;
}

int gm_test_lvd_setting()
{
    UNUSED(gm_test_lvd_reset);
    UNUSED(gm_test_lvd_query);

    struct device *lvd_dev;
    lvd_cfg data;

    lvd_dev = device_get_binding("GMLVD");

    TC_PRINT("\n----------------------------------\n");
    TC_PRINT("Lvd Setting Test\n");
    TC_PRINT("----------------------------------\n");

    if (lvd_check_reset_status(lvd_dev))
    {
        TC_PRINT("Last reset is caused by VOL_DET or POR\n");
        lvd_clear_reset_status(lvd_dev);
        return TC_PASS;
    }

    memset(&data, 0, sizeof(lvd_cfg));
    data.lvd1.enable          = 1;
    data.lvd1.threshold       = 0x1;
    data.lvd1.reset           = 1;
    data.lvd1.detectionLevel  = LVD_VD1_Level_6;

    data.lvd2.enable          = 1;
    data.lvd2.threshold       = 0x1;
    data.lvd2.detectionLevel  = LVD_VD1_Level_7;
    data.lvd2.int_call        = gm_test_lvd_callback;

    lvd_enable(lvd_dev, &data);

    gm_test_lvd_print_info(1, 1, lvd1_level[LVD_VD1_Level_6], 1, 0);
    gm_test_lvd_print_info(2, 1, lvd2_level[LVD_VD1_Level_7], 0, 1);

    return TC_PASS;
}


void gm_test_lvd(void)
{
    printf("================GM_LVD Test Start=================\n\n\n");
#ifdef CONFIG_ZTEST_GM_LVD_INTEGRAL_CASE
    zassert_true(gm_test_lvd_query() == TC_PASS, NULL);
    zassert_true(gm_test_lvd_reset() == TC_PASS, NULL);
#else
    zassert_true(gm_test_lvd_setting() == TC_PASS, NULL);
#endif
    printf("sysclk:%d,date:%s,time:%s\n", SYS_CLOCK, __DATE__, __TIME__);
    printf("================LVD Test  End==================\n\n\n");
}

