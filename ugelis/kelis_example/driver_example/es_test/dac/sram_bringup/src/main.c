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
* @brief       DAC source file
*
* @note
*              2019/01/24, Richard.Liu, v1.0.0
*                  Initial version.
*
**/

#include <ugelis.h>
#include <misc/printk.h>
#include <gm_hal_gpio.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <dac.h>
#include <pinmux.h>


//#define TEST_CASE0  0    /*Reset and clock test**/

//#define TEST_CASE1  0       /*DAC register test*/
//#define TEST_CASE2  0     /*DAC dma test*/
//#define TEST_CASE3  0     /*DAC interrupt test*/
//#define TEST_CASE4  0      /*DAC basic test*/
//#define TEST_CASE5  1         /*DAC noise test*/

//#define TEST_CASE6      /*DAC traingle wave test*/

//#define TEST_CASE7        /*DAC dual control test*/

#define TEST_CASE8  1 /*DAC sample and hold test*/

#define PR_DEBUG  printk

/*******************************************************************************
* Constants and macros
*******************************************************************************/

/*******************************************************************************
* Reference variables
*******************************************************************************/
struct gls_data
{
    struct device *dac;
    struct dac_config_data dac_data;
};

struct gls_data g_dac_data;

struct gls_data g_dac1_data;


/*******************************************************************************
* Local function prototypes
*******************************************************************************/

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
static void gpio_reset_release(void)
{
    HW32_REG(0x40000070) |= (1 << 1);
}

static void gpio_clk_enable(void)
{
    HW32_REG(0x40000080) |= (1 << 1);
}

static void patch_config(void) /*FIX HIRC UART PRINT ISSUE*/
{
    HW32_REG(0x1ffff0a8) = 0xfffffbac;
    HW32_REG(0x1ffff0ac) = 0xff1fffff;
    HW32_REG(0x1ffff0b0) = 0xff3fffff;

}

static void DAC_Callback(DAC_Handle_T *pHandle, DAC_Event_T event, unsigned long Param)
{
    static int cnt = 0;

    printk("negativePin and positivePin voltage match the trigger mode, cnt=[%d]\n", cnt++);
    if (event == DAC_MATCH_EVENT)
    {
        printk("negativePin and positivePin voltage match the trigger mode, cnt=[%d]\n", cnt++);
    }
}

static int dac_demo_app_noise(void)
{
    unsigned int data = 0;
    int i = 0;
    int j = 0;
    int k = 0;

    common_init();

    g_dac_data.dac = device_get_binding(CONFIG_DAC_0_NAME);
    if (g_dac_data.dac == NULL)
    {
        printk("Failed to get dac device.");
        return -EINVAL;
    }

    g_dac_data.dac_data.config_data.dacEn = 1;
    g_dac_data.dac_data.config_data.dacDataMode = DAC_DATA_MODE_12BIT;
    g_dac_data.dac_data.config_data.dacSoftTrigleEn = DAC_SINGLE_SOFT_TRIG;
    g_dac_data.dac_data.config_data.waveType = DAS_WAVE_NOISE_EN;
    g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = 0;
    g_dac_data.dac_data.config_data.msAM = 15;

#if 0
    g_dac1_data.dac = device_get_binding(CONFIG_DAC_1_NAME);
    //g_dac1_data.dac_data.callback = DAC_Callback;

    if (g_dac1_data.dac == NULL)
    {
        printk("Failed to get acmp device.");
        return -EINVAL;
    }

    dac_config(g_dac1_data.dac, &g_dac1_data.dac_data);
    dac_enable(g_dac1_data.dac);
#endif

    int test_value[4]   = {0x0, 0xFFFFFFFF, 0x5A5A5A5A, 0xA5A5A5A5};
    int error   = 0;
    int data_out;
    int data_expect;
    int repeat_num  =   10;
#if 1
    for (i = 0 ; i < 4 ; i ++)
    {
        printf("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        printf("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = test_value[i];
            dac_config(g_dac_data.dac, &g_dac_data.dac_data);
            dac_enable(g_dac_data.dac);
            dac_triger(g_dac_data.dac, &g_dac_data.dac_data);

            small_delay();
            dac_read(g_dac_data.dac, &g_dac_data.dac_data);
            data_out    = g_dac_data.dac_data.dac_data_out;
            data_expect = HAL_DAC_Noise_Result(test_value[i], 0xFFF, (j + i * repeat_num));
            if (data_out != data_expect)
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **\n");
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out == data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += HAL_DAC_Result_Check(data_out, data_expect, 12);
        }
    }
#endif

    dac_disable(g_dac_data.dac);

#if 1

    //CMSDK_DAC->DAC_CTRL &= (~(1<<0 | 3<< 8));
    //CMSDK_DAC->DAC_CTRL = 1<<0 | 1<<8 | 15<<12 | 1<<20; //Enable DAC , 10-bit mode , noise enable , unmask

    g_dac_data.dac_data.config_data.dacEn = 1;
    g_dac_data.dac_data.config_data.dacDataMode = DAC_DATA_MODE_10BIT;
    g_dac_data.dac_data.config_data.dacSoftTrigleEn = DAC_SINGLE_SOFT_TRIG;
    g_dac_data.dac_data.config_data.waveType = DAS_WAVE_NOISE_EN;
    g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = 0;
    g_dac_data.dac_data.config_data.msAM = 15;

    for (i = 0 ; i < 4 ; i ++)
    {

        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            //CMSDK_DAC->DAC_TRIG    = 1<<0;
            g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = test_value[i];
            dac_config(g_dac_data.dac, &g_dac_data.dac_data);
            dac_enable(g_dac_data.dac);
            dac_triger(g_dac_data.dac, &g_dac_data.dac_data);

            small_delay();
            dac_read(g_dac_data.dac, &g_dac_data.dac_data);
            data_out    = g_dac_data.dac_data.dac_data_out;
            data_expect = HAL_DAC_Noise_Result(test_value[i], 0xFFF, (j + i * repeat_num));
            if (data_out != ((data_expect << 2 | 0x3) & 0xFFF))
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **, line=%d\n", __LINE__);
                };
                error += 1;
            }
            else
            {
                printf("**Success : data_out == data_expect **\n");
            }
            printf("  ==========data out	= 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += HAL_DAC_Result_Check(data_out, data_expect, 10);
        }
    }
    dac_disable(g_dac_data.dac);
#endif

#if 1

    g_dac_data.dac_data.config_data.dacEn = 1;
    g_dac_data.dac_data.config_data.dacDataMode = DAC_DATA_MODE_8BIT;
    g_dac_data.dac_data.config_data.dacSoftTrigleEn = DAC_SINGLE_SOFT_TRIG;
    g_dac_data.dac_data.config_data.waveType = DAS_WAVE_NOISE_EN;
    g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = 0;
    g_dac_data.dac_data.config_data.msAM = 15;
    //CMSDK_DAC->DAC_CTRL &= (~(1<<0 | 3<<8));
    //CMSDK_DAC->DAC_CTRL = 1<<0 | 1<<8 | 15<<12 | 2<<20; //Enable DAC , 8-bit mode , noise enable , unmask

    for (i = 0 ; i < 4 ; i ++)
    {
        g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = test_value[i];
        printf("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            dac_config(g_dac_data.dac, &g_dac_data.dac_data);
            dac_enable(g_dac_data.dac);
            dac_triger(g_dac_data.dac, &g_dac_data.dac_data);
            small_delay();
            dac_read(g_dac_data.dac, &g_dac_data.dac_data);
            data_out    = g_dac_data.dac_data.dac_data_out;
            data_expect = HAL_DAC_Noise_Result(test_value[i], 0xFFF, (j + i * repeat_num));
            if (data_out != ((data_expect << 4 | 0xF) & 0xFFF))
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **, line=%d\n", __LINE__);
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out == data_expect **\n");
            }
            printf("  ==========data out	= 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += HAL_DAC_Result_Check(data_out, data_expect, 8);
        }
    }

    dac_disable(g_dac_data.dac);
#endif

#if 1
    g_dac_data.dac_data.config_data.dacEn = 1;
    g_dac_data.dac_data.config_data.dacDataMode = DAC_DATA_MODE_12BIT;
    g_dac_data.dac_data.config_data.dacSoftTrigleEn = DAC_SINGLE_SOFT_TRIG;
    g_dac_data.dac_data.config_data.waveType = DAS_WAVE_NOISE_EN;
    g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = 0;
    g_dac_data.dac_data.config_data.msAM = 5;

    //CMSDK_DAC->DAC_CTRL &= (~(1<<0 | 3<<8));
    //CMSDK_DAC->DAC_CTRL = 1<<0 | 1<<8 | 5<<12; //Enable DAC , 12-bit mode , noise enable , [5:0]umask
    //printf("control_data = 0x%x\n",CMSDK_DAC->DAC_CTRL);


    for (i = 0 ; i < 4 ; i ++)
    {
        g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            dac_config(g_dac_data.dac, &g_dac_data.dac_data);
            dac_enable(g_dac_data.dac);
            dac_triger(g_dac_data.dac, &g_dac_data.dac_data);
            small_delay();
            dac_read(g_dac_data.dac, &g_dac_data.dac_data);
            data_out    = g_dac_data.dac_data.dac_data_out;
            data_expect = HAL_DAC_Noise_Result(test_value[i], 0x3F, (j + i * repeat_num));
            if (data_out != data_expect)
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **, line=%d\n", __LINE__);
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out	= 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += HAL_DAC_Result_Check(data_out, data_expect, 12);
        }
    }

    dac_disable(g_dac_data.dac);
    g_dac_data.dac_data.config_data.dacEn = 1;
    g_dac_data.dac_data.config_data.dacDataMode = DAC_DATA_MODE_10BIT;
    g_dac_data.dac_data.config_data.dacSoftTrigleEn = DAC_SINGLE_SOFT_TRIG;
    g_dac_data.dac_data.config_data.waveType = DAS_WAVE_NOISE_EN;
    g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = 0;
    g_dac_data.dac_data.config_data.msAM = 5;

    //CMSDK_DAC->DAC_CTRL &= (~(1<<0 | 3<< 8));
    //CMSDK_DAC->DAC_CTRL = 1<<0 | 1<<8 | 5<<12 | 1<<20; //Enable DAC , 10-bit mode , noise enable , [5:0]unmask

    for (i = 0 ; i < 4 ; i ++)
    {
        g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            dac_config(g_dac_data.dac, &g_dac_data.dac_data);
            dac_enable(g_dac_data.dac);
            dac_triger(g_dac_data.dac, &g_dac_data.dac_data);
            small_delay();
            dac_read(g_dac_data.dac, &g_dac_data.dac_data);
            data_out    = g_dac_data.dac_data.dac_data_out;
            data_expect = HAL_DAC_Noise_Result(test_value[i], 0x3F, (j + i * repeat_num));
            if (data_out != ((data_expect << 2 | 0x3) & 0xFFF))
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **, line=%d\n", __LINE__);
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out	= 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += HAL_DAC_Result_Check(data_out, data_expect, 10);
        }
    }
    dac_disable(g_dac_data.dac);

    g_dac_data.dac_data.config_data.dacEn = 1;
    g_dac_data.dac_data.config_data.dacDataMode = DAC_DATA_MODE_8BIT;
    g_dac_data.dac_data.config_data.dacSoftTrigleEn = DAC_SINGLE_SOFT_TRIG;
    g_dac_data.dac_data.config_data.waveType = DAS_WAVE_NOISE_EN;
    g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = 0;
    g_dac_data.dac_data.config_data.msAM = 5;

    //CMSDK_DAC->DAC_CTRL &= (~(1<<0 | 3<<8));
    //CMSDK_DAC->DAC_CTRL = 1<<0 | 1<<8 | 5<<12 | 2<<20; //Enable DAC , 8-bit mode , noise enable , [5:0]unmask

    for (i = 0 ; i < 4 ; i ++)
    {
        g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = test_value[i];
        printf("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            dac_config(g_dac_data.dac, &g_dac_data.dac_data);
            dac_enable(g_dac_data.dac);
            dac_triger(g_dac_data.dac, &g_dac_data.dac_data);
            small_delay();
            dac_read(g_dac_data.dac, &g_dac_data.dac_data);
            data_out    = g_dac_data.dac_data.dac_data_out;
            data_expect = HAL_DAC_Noise_Result(test_value[i], 0x3F, (j + i * repeat_num));
            if (data_out != ((data_expect << 4 | 0xF) & 0xFFF))
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **, line=%d\n", __LINE__);
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out	= 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += HAL_DAC_Result_Check(data_out, data_expect, 8);
        }
    }

#endif
    //printf("control_data = 0x%x\n",CMSDK_DAC->DAC_CTRL);


    printf("exit func[%s]... \n", __FUNCTION__);

    return 0;
}

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))

static int dac_demo_app(void)
{
    unsigned int data = 0;
    int i = 0;
    int j = 0;
    int k = 0;

    common_init();

    g_dac_data.dac = device_get_binding(CONFIG_DAC_0_NAME);
    if (g_dac_data.dac == NULL)
    {
        printk("Failed to get dac device.");
        return -EINVAL;
    }

    g_dac_data.dac_data.config_data.dacEn = 1;
    g_dac_data.dac_data.config_data.dacDataMode = DAC_DATA_MODE_12BIT;
    g_dac_data.dac_data.config_data.dacSoftTrigleEn = DAC_HARDWARE_TRIG;
    g_dac_data.dac_data.config_data.waveType = DAC_WAVE_DISABLE;
    g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = 0;
    g_dac_data.dac_data.config_data.msAM = 15;
    g_dac_data.dac_data.config_data.triggerMode = DAC_HARDWARE_TRIG;


    int test_value[4]   = {0x0, 0xFFFFFFFF, 0x5A5A5A5A, 0xA5A5A5A5};
    int error   = 0;
    int data_out;
    int data_expect;
    int repeat_num  =   10;
#if 1
    for (i = 0 ; i < 4 ; i ++)
    {
        printf("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        printf("-------------------------------------------------\n");
        g_dac_data.dac_data.config_data.dacComConfig.dacHoldData = test_value[i];
        dac_config(g_dac_data.dac, &g_dac_data.dac_data);
        dac_enable(g_dac_data.dac);
        dac_triger(g_dac_data.dac, &g_dac_data.dac_data);

        k_sleep(1000);
        HW32_REG(0x4000B000 + 0x8) = 0;
        k_sleep(1000);

        small_delay();
        dac_read(g_dac_data.dac, &g_dac_data.dac_data);
        data_out    = g_dac_data.dac_data.dac_data_out;

    }
#endif
    return 0;
}


static void dump_reg(DAC_Device_T *pDAC, bool flag)
{
    if (flag != 0)
    {
        printk("pDAC->DAC_CTRL=0x%x\n", pDAC->DAC_CTRL);
        //PR_DEBUG("pDAC->DAC_TRIG=0x%x\n", pDAC->DAC_TRIG);
#if 1
        printk("pDAC->DAC_DH=0x%x\n", pDAC->DAC_DH);
        printk("pDAC->DAC_DO=0x%x\n", pDAC->DAC_DO);

        printk("pDAC->DAC_IE=0x%x\n", pDAC->DAC_IE);
        printk("pDAC->DAC_STATUS=0x%x\n", pDAC->DAC_STATUS);
        printk("pDAC->DAC_SHST=0x%x\n", pDAC->DAC_SHST);
        printk("pDAC->DAC_SHHT=0x%x\n", pDAC->DAC_SHHT);
        printk("pDAC->DAC_SHRF=0x%x\n", pDAC->DAC_SHRF);
#endif
    }
}

static void dump_common_reg(DAC_ComDevice_T *pDACCom, bool flag)
{
    if (flag != 0)
    {
        PR_DEBUG("pDACCom->DAC_DDH=0x%x\n", pDACCom->DAC_DDH);
    }
}

#if defined(TEST_CASE1)
void dac_reg_access(DAC_Device_T *pDAC)
{
    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    dump_reg(pDAC, 1);
    PR_DEBUG("write reg: func[%s]\n", __FUNCTION__);
    PR_DEBUG("===========================\n\n");

    pDAC->DAC_CTRL = 0xFFFFFFFF;
    pDAC->DAC_STRIG = 0xFFFFFFFF;
#if 1
    pDAC->DAC_HTRIG = 0xFFFFFFFF;
    pDAC->DAC_DH = 0xFFFFFFFF;
    pDAC->DAC_DO = 0xFFFFFFFF;

    pDAC->DAC_IE = 0xFFFFFFFF;
    pDAC->DAC_STATUS = 0xFFFFFFFF;
    pDAC->DAC_SHST = 0xFFFFFFFF;
    pDAC->DAC_SHHT = 0xFFFFFFFF;
    pDAC->DAC_SHRF = 0xFFFFFFFF;
#endif

    dump_reg(pDAC, 1);
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

#if defined(TEST_CASE0)
void dac_clk_test(DAC_Device_T *pDAC)
{
    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    dump_reg(pDAC, 1);
    PR_DEBUG("write reg: func[%s]\n", __FUNCTION__);
    PR_DEBUG("===========================\n\n");
    pDAC->DAC_CTRL = 0xFFFFFFFF;
    pDAC->DAC_STRIG = 0xFFFFFFFF;
    pDAC->DAC_HTRIG = 0xFFFFFFFF;
    pDAC->DAC_DH = 0xFFFFFFFF;
    pDAC->DAC_DO = 0xFFFFFFFF;

    pDAC->DAC_IE = 0xFFFFFFFF;
    pDAC->DAC_STATUS = 0xFFFFFFFF;
    pDAC->DAC_SHST = 0xFFFFFFFF;
    pDAC->DAC_SHHT = 0xFFFFFFFF;
    pDAC->DAC_SHRF = 0xFFFFFFFF;

    dump_reg(pDAC, 1);
    PR_DEBUG("===========================\n\n");
    PR_DEBUG("after pDAC clock gate reg: func[%s]\n", __FUNCTION__);

    IO_BITMASK_SET(SYSRSTCLKREG->CLK_EN_CTRL[0], (1 << 11), (0 << 11)); // gating clock
    dump_reg(pDAC, 1);
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}

#endif

#if defined(TEST_CASE0)
void dac_reset_test(DAC_Device_T *pDAC)
{
    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    dump_reg(pDAC, 1);
    PR_DEBUG("write reg: func[%s]\n", __FUNCTION__);
    PR_DEBUG("===========================\n\n");
    pDAC->DAC_CTRL = 0xFFFFFFFF;
    pDAC->DAC_STRIG = 0xFFFFFFFF;
    pDAC->DAC_HTRIG = 0xFFFFFFFF;
    pDAC->DAC_DH = 0xFFFFFFFF;
    pDAC->DAC_DO = 0xFFFFFFFF;

    pDAC->DAC_IE = 0xFFFFFFFF;
    pDAC->DAC_STATUS = 0xFFFFFFFF;
    pDAC->DAC_SHST = 0xFFFFFFFF;
    pDAC->DAC_SHHT = 0xFFFFFFFF;
    pDAC->DAC_SHRF = 0xFFFFFFFF;

    dump_reg(pDAC, 1);
    PR_DEBUG("===========================\n\n");
    PR_DEBUG("after pDAC RESET reg: func[%s]\n", __FUNCTION__);

    IO_BITMASK_SET(SYSRSTCLKREG->IP_RST_CTRL[0], (1 << 10), (0 << 10));
    DAC_RESET();
    dump_reg(pDAC, 1);
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

/*
1.Enable DAC interrupt
2.Write DAC data hold register
3.Trigger DAC to load the data
4.Check the DAC interrupt
*/
#if defined(TEST_CASE3)
void dac_interrupt_test(DAC_Device_T *pDAC)
{
    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    dump_reg(pDAC, 1);
    PR_DEBUG("write reg: func[%s]\n", __FUNCTION__);
    PR_DEBUG("===========================\n\n");

    DAC_INTERRUPT_ENABLE(pDAC);
    DAC_SET_DATAHOLD_REG(pDAC, 0xFFF);
    DAC_ENABLE(pDAC);
    DAC_SOFT_TRIG(pDAC);

    dump_reg(pDAC, 1);
    PR_DEBUG("===========================\n\n");
    PR_DEBUG("after pDAC RESET reg: func[%s]\n", __FUNCTION__);
    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

// 1792/4096 * 3.308
static RET_CODE DAC_ConfigPinmux(void)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

#if (defined(CHIP_GM6721))
    /*set digtal pin as input , inorde to not affect analog input*/
    pGPIO = GPIOC;
    GPIO_InitStruct.pin  = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;
#endif
    ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        PR_DEBUG("Err:dac_ConfigPinmux\n");
    }

    return ret;
}


#if defined(TEST_CASE4)
/*
1.Enable DAC1/2
2.Configure the DAC pinmux
3.Write DAC data hold register
4.Trigger DAC to load the data
5.Check the voltage DAC PIN
*/
void dac_basic_test()
{
    int i = 0;
    DAC_Device_T *pDAC = DAC0_BASE;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    DAC_ConfigPinmux();

    for (i = 0; i < 2; i++)
    {
        PR_DEBUG("=======DAC%d func[%s] begin====================\n\n", i, __FUNCTION__);
        dump_reg(pDAC, 1);
        if (i == 0)
        {
            pDAC = DAC0_BASE;
        }
        else
        {
            pDAC = DAC1_BASE;
        }

        DAC_SET_DATAHOLD_REG(pDAC, 0X6ff);  // 0XFFF  /*41~4042*/
        DAC_ENABLE(pDAC);
        DAC_SOFT_TRIG(pDAC);
        PR_DEBUG("=======DAC%d func[%s] middle====================\n\n", i, __FUNCTION__);
        dump_reg(pDAC, 1);
        PR_DEBUG("=======DAC%d func[%s] end====================\n\n", i, __FUNCTION__);
    }

    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

/*
1.Enable DAC1/2
2.Enable the DAC1/2 noise function
3.Configure the DAC noise bit value and mask bit
4.Configure the DAC pinmux
5.Write DAC data hold register
6.Trigger DAC to load the data
7.Check the voltage DAC PIN
*/

#if defined(TEST_CASE5)

void small_delay();
int dac_result_check(int data_out, int expect_data_out, int convert_mode);
int dac_noise_test(DAC_Device_T *CMSDK_DAC);
int dac_noise_result(int data_hold, int mask, int shift_num);

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

int dac_noise_demo(void)
{
    int err = 0;

    DAC_ConfigPinmux();

    //Enable DAC clock
    HW32_REG(0x40000080) |= 1 << 11;
    //Release DAC reset
    HW32_REG(0x40000070) |= 1 << 11;

    printf("----------------DAC noise test begin  -----------------\n");
    err = dac_noise_test(DAC0_DEV);
    if (err != 0)
    {
        printf("**Error : DAC1 noise test failed !!! ** \n");
    }
    else
    {
        printf("**Success : DAC1 noise test pass !!! **\n");
    }

    err = dac_noise_test(DAC1_DEV);
    if (err != 0)
    {
        printf("**Error : DAC2 noise test failed !!! ** \n");
    }
    else
    {
        printf("**Success : DAC2 noise test pass !!! **\n");
    }

    printf("----------------DAC noise test end  -----------------\n");

    return 0;
}

int dac_noise_test(DAC_Device_T *CMSDK_DAC)
{
    int test_value[4]   = {0x0, 0xFFFFFFFF, 0x5A5A5A5A, 0xA5A5A5A5};
    int error   = 0;
    int data_out;
    int data_expect;
    int i   = 0;
    int j   = 0;
    int repeat_num  =   10;

    CMSDK_DAC->DAC_CTRL = 1 << 0 | 1 << 8 | 15 << 12; //Enable DAC , 12-bit mode , noise enable , unmask
    //printf("control_data = 0x%x\n",CMSDK_DAC->DAC_CTRL);

    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        printf("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        printf("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->SDAC_TRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_noise_result(test_value[i], 0xFFF, (j + i * repeat_num));
            if (data_out != data_expect)
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **\n");
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out == data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 12);
        }
    }

#if 1

    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 1 << 8 | 15 << 12 | 1 << 20; //Enable DAC , 10-bit mode , noise enable , unmask

    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_TRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_noise_result(test_value[i], 0xFFF, (j + i * repeat_num));
            if (data_out != ((data_expect << 2 | 0x3) & 0xFFF))
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **\n");
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out == data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 10);
        }
    }

    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 1 << 8 | 15 << 12 | 2 << 20; //Enable DAC , 8-bit mode , noise enable , unmask

    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        printf("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_TRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_noise_result(test_value[i], 0xFFF, (j + i * repeat_num));
            if (data_out != ((data_expect << 4 | 0xF) & 0xFFF))
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **\n");
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out == data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 8);
        }
    }


    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 1 << 8 | 5 << 12; //Enable DAC , 12-bit mode , noise enable , [5:0]umask
    //printf("control_data = 0x%x\n",CMSDK_DAC->DAC_CTRL);


    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_TRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_noise_result(test_value[i], 0x3F, (j + i * repeat_num));
            if (data_out != data_expect)
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **\n");
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 12);
        }
    }

    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 1 << 8 | 5 << 12 | 1 << 20; //Enable DAC , 10-bit mode , noise enable , [5:0]unmask

    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_TRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_noise_result(test_value[i], 0x3F, (j + i * repeat_num));
            if (data_out != ((data_expect << 2 | 0x3) & 0xFFF))
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **\n");
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 10);
        }
    }

    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 1 << 8 | 5 << 12 | 2 << 20; //Enable DAC , 8-bit mode , noise enable , [5:0]unmask

    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        printf("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_TRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_noise_result(test_value[i], 0x3F, (j + i * repeat_num));
            if (data_out != ((data_expect << 4 | 0xF) & 0xFFF))
            {
                while (1)
                {
                    printf("**Error : data_out != data_expect **\n");
                }
                error += 1;
            }
            else
            {
                printf("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 8);
        }
    }

#endif
    //printf("control_data = 0x%x\n",CMSDK_DAC->DAC_CTRL);

    return error;
}

void small_delay()
{
    int i = 0;
    int a = 0;

    for (i = 0; i < 5; i++)
    {
        a++;
    }

    return;
}


int dac_result_check(int data_out, int expect_data_out, int convert_mode)
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
        while (1)
        {
            printf("**Error : convert result != expect result\n");
        }
        error += 1;
    }
    else
    {
        printf("**Success : convert result = expect result\n");
    }

    printf("  ======expect_result   = %f ======\n", expect_result);
    printf("  ======convert_result  = %f ======\n\n", convert_result);

    return  error;
}

int dac_noise_result(int data_hold, int mask, int shift_num)
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

/*
1.Enable DAC1/2
2.Enable the DAC1/2 traingle wave function
3.Configure the DAC traingel wave base value and mask bit
4.Configure the DAC pinmux
5.Write DAC data hold register
6.Trigger DAC to load the data
7.Check the voltage DAC PIN
*/

#if defined(TEST_CASE6) //DAC traingle wave test

void small_delay();
int dac_result_check(int data_out, int expect_data_out, int convert_mode);
int dac_triangle_test(DAC_Device_T *CMSDK_DAC);
volatile int dac_triangle_result(int data_hold, int mask, int shift_num);

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

int dac_traingle_wave_demo(void)
{
    int err = 0;

    //Enable DAC clock
    HW32_REG(0x40000080) |= 1 << 11;
    //Release DAC reset
    HW32_REG(0x40000070) |= 1 << 11;
    HW32_REG(0x400000A0) |= 1 << 24;
    DAC_ConfigPinmux();

    puts("----------------DAC triangle test begin  -----------------\n");
    err = dac_triangle_test(DAC0_DEV);
    if (err != 0)
    {
        puts("**Error : DAC1 triangle test failed !!! ** \n");
    }
    else
    {
        puts("**Success : DAC1 triangle test pass !!! **\n");
    }

    err = dac_triangle_test(DAC1_DEV);
    if (err != 0)
    {
        puts("**Error : DAC2 triangle test failed !!! ** \n");
    }
    else
    {
        puts("**Success : DAC2 triangle test pass !!! **\n");
    }

    HW32_REG(0x40000070) &= ~(1 << 11);
    puts("----------------DAC triangle test end  -----------------\n");

    return 0;
}

#define TEST_LEN  4096
int dac_triangle_test(DAC_Device_T *CMSDK_DAC)
{
    int test_value[4]   = {0x0, 0xFFFFFFFF, 0x5A5A5A5A, 0xA5A5A5A5};
    int error   = 0;
    int data_out;
    int data_expect;
    int i   = 0;
    int j   = 0;
    //int repeat_num  =   5;

    int repeat_num  =   1;

    //for coverage
    for (i = 0; i < 16; i++)
    {
        CMSDK_DAC->DAC_CTRL |= i << 12;
        CMSDK_DAC->DAC_CTRL &= ~(i << 12);
    }

    CMSDK_DAC->DAC_CTRL = 1 << 0 | 2 << 8 | 11 << 12; //Enable DAC , 12-bit mode , triangle wave enable ,max value : 0xFFF

#if 1
    while (1)
    {

        for (i = 0 ; i < TEST_LEN ; i++)
        {
            CMSDK_DAC->DAC_DH   = i;//test_value[i];
            //puts("-------------------------------------------------\n");
            // printf("-----------------test_value = 0x%x --------------\n",test_value[i]);
            // puts("-------------------------------------------------\n");
            for (j = 0 ; j < repeat_num ; j++)
            {
                CMSDK_DAC->DAC_STRIG    = 1 << 0;
                //  small_delay();
                data_out    = CMSDK_DAC->DAC_DO;
                //data_expect = dac_triangle_result(test_value[i],0xFFF,(j + i*repeat_num));
                if (data_out != data_expect)
                {
                    // puts("**Error : data_out != data_expect **\n");
                    error += 1;
                }
                else
                {
                    //puts("**Success : data_out != data_expect **\n");
                }
                //printf("  ==========data out    = 0x%x ========   \n",data_out);
                //printf("  ==========data expect = 0x%x ========   \n\n",data_expect);

                //error += dac_result_check(data_out, data_expect, 12);
            }
        }

    }
#else

    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 2 << 8 | 11 << 12 | 1 << 20; //Enable DAC , 10-bit mode , triangle wave enable , max value : 0xFFF

    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x --------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_STRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_triangle_result(test_value[i], 0xFFF, (j + i * repeat_num));
            if (data_out != ((data_expect << 2 | 0x3) & 0xFFF))
            {
                puts("**Error : data_out != data_expect **\n");
                error += 1;
            }
            else
            {
                puts("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 10);
        }
    }

    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 2 << 8 | 11 << 12 | 2 << 20; //Enable DAC , 8-bit mode , triangle wave enable , max value : 0xFFF

    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_STRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_triangle_result(test_value[i], 0xFFF, (j + i * repeat_num));
            if (data_out != ((data_expect << 4 | 0xF) & 0xFFF))
            {
                puts("**Error : data_out != data_expect **\n");
                error += 1;
            }
            else
            {
                puts("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 8);
        }
    }


    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 2 << 8 | 5 << 12; //Enable DAC , 12-bit mode , noise enable , max value : 63
    //printf("control_data = 0x%x\n",CMSDK_DAC->DAC_CTRL);


    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_STRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_triangle_result(test_value[i], 0x3F, (j + i * repeat_num));
            if (data_out != data_expect)
            {
                puts("**Error : data_out != data_expect **\n");
                error += 1;
            }
            else
            {
                puts("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 12);
        }
    }

    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 2 << 8 | 5 << 12 | 1 << 20; //Enable DAC , 10-bit mode , noise enable , max value : 63

    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_STRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_triangle_result(test_value[i], 0x3F, (j + i * repeat_num));
            if (data_out != ((data_expect << 2 | 0x3) & 0xFFF))
            {
                puts("**Error : data_out != data_expect **\n");
                error += 1;
            }
            else
            {
                puts("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 10);
        }
    }

    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 2 << 8 | 5 << 12 | 2 << 20; //Enable DAC , 8-bit mode , noise enable , max value : 63

    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_STRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_triangle_result(test_value[i], 0x3F, (j + i * repeat_num));
            if (data_out != ((data_expect << 4 | 0xF) & 0xFFF))
            {
                puts("**Error : data_out != data_expect **\n");
                error += 1;
            }
            else
            {
                puts("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 8);
        }
    }

    CMSDK_DAC->DAC_CTRL &= (~(1 << 0 | 3 << 8));
    CMSDK_DAC->DAC_CTRL = 1 << 0 | 2 << 8 | 2 << 12; //Enable DAC , 12-bit mode , noise enable , max value : 7
    //printf("control_data = 0x%x\n",CMSDK_DAC->DAC_CTRL);


    for (i = 0 ; i < 4 ; i ++)
    {
        CMSDK_DAC->DAC_DH   = test_value[i];
        puts("-------------------------------------------------\n");
        printf("-----------------test_value = 0x%x ---------------\n", test_value[i]);
        puts("-------------------------------------------------\n");
        for (j = 0 ; j < repeat_num ; j++)
        {
            CMSDK_DAC->DAC_STRIG    = 1 << 0;
            small_delay();
            data_out    = CMSDK_DAC->DAC_DO;
            data_expect = dac_triangle_result(test_value[i], 0x7, (j + i * repeat_num));
            if (data_out != data_expect)
            {
                puts("**Error : data_out != data_expect **\n");
                error += 1;
            }
            else
            {
                puts("**Success : data_out != data_expect **\n");
            }
            printf("  ==========data out    = 0x%x ========   \n", data_out);
            printf("  ==========data expect = 0x%x ========   \n\n", data_expect);

            error += dac_result_check(data_out, data_expect, 12);
        }
    }

#endif
    //printf("control_data = 0x%x\n",CMSDK_DAC->DAC_CTRL);

    //dac_triangle_result(0x0 , 140 , 0);
    //dac_triangle_result(0x0 , 5, 34);
    //dac_triangle_result(0x0 , 1 , 2);

    return error;
}

void small_delay()
{
    int i = 0;
    int a = 0;

    for (i = 0; i < 5; i++)
    {
        a ++;
    }

    return;
}

int dac_result_check(int data_out, int expect_data_out, int convert_mode)
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
        puts("**Error : convert result != expect result\n");
        error += 1;
    }
    else
    {
        puts("**Success : convert result = expect result\n");
    }

    printf("  ======expect_result   = %f ======\n", expect_result);
    printf("  ======convert_result  = %f ======\n\n", convert_result);

    return  error;
}


volatile int dac_triangle_result(int data_hold, int max_value, int repeat_num)
{
    int i = 0;
    volatile int counter_value = 0x0;
    volatile int counter_direct  = 1;
    int data_out    = 0;

    if (repeat_num == 0)
    {
        counter_value   = 0;
        //printf("counter_value  = %d\n",counter_value);
    }
    else
    {
        for (i = 0 ; i < repeat_num ; i++)
        {
            if (counter_value == max_value)
            {
                counter_value   = counter_value - 1;
                counter_direct  = 0;
            }
            else if (counter_value == 0)
            {
                counter_value   = counter_value + 1;
                counter_direct  = 1;
            }
            else
            {
                if (counter_direct == 1)
                {
                    counter_value   = counter_value + 1;
                }
                else if (counter_direct == 0)
                {
                    counter_value   = counter_value - 1;
                }
            }
            //printf("counter_value  = %d\n",counter_value);
        }
    }
    //printf("final counter_value = %d\n",counter_value);
    data_out    = (data_hold + counter_value) & 0xFFF;
    return data_out;
}

#endif


/*
1.Enable DAC1 and DAC2 both
2.Configure the DAC pinmux
3.Write the DAC dual data hold register
4.Trigger the DAC1/DAC2 to load data by setting the Dual data register
5.Check the voltage of the DAC1 and DAC2
*/

#if defined(TEST_CASE7)  //DAC dual control test
void dac_dual_control_test(void)
{
    int i = 0;
    DAC_Device_T *pDAC = DAC0_BASE;
    DAC_ComDevice_T *pDAC_COM = DAC_COM_BASE;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    DAC_ConfigPinmux();

    for (i = 0; i < 2; i++)
    {
        PR_DEBUG("=======DAC%d func[%s] begin====================\n\n", i, __FUNCTION__);

        if (i == 0)
        {
            pDAC = DAC0_BASE;
            dump_reg(pDAC, 1);
            DAC0_SET_DOUL_HOLD_DATA(pDAC_COM, 0x6FF);
        }
        else
        {
            pDAC = DAC1_BASE;
            dump_reg(pDAC, 1);
            DAC1_SET_DOUL_HOLD_DATA(pDAC_COM, 0x6FF);
        }

        DAC_ENABLE(pDAC);

        PR_DEBUG("=======DAC%d func[%s] middle====================\n\n", i, __FUNCTION__);
        dump_reg(pDAC, 1);
        dump_common_reg(pDAC_COM, 1);

    }

    DAC_DUAL_SOFT_TRIG(pDAC_COM);
    PR_DEBUG("=======DAC%d func[%s] middle2====================\n\n", i, __FUNCTION__);
    dump_common_reg(pDAC_COM, 1);
    PR_DEBUG("=======DAC%d func[%s] end====================\n\n", i, __FUNCTION__);

    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}
#endif

/*
1.Enable DAC1/DAC2
2.Configure the DAC pinmux
3.Enable the DAC sample and hold function
4.Configure the sample time , hold time and refresh time
3.Write the DAC dual data hold register
4.Trigger the DAC1/DAC2 to load data by setting the Dual data register
5.Check the voltage of the DAC1/DAC2
*/

#if defined(TEST_CASE8)  //DAC sample and hold test
void dac_sample_hold_test()
{
    int i = 0;
    DAC_Device_T *pDAC = DAC0_DEV;
    DAC_ComDevice_T *pDAC_COM = DAC_COM_DEV;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    DAC_ConfigPinmux();

    for (i = 0; i < 2; i++)
    {
        PR_DEBUG("=======DAC%d func[%s] begin====================\n\n", i, __FUNCTION__);
        dump_reg(pDAC, 1);
        if (i == 0)
        {
            pDAC = DAC0_BASE;
            DAC_SET_SAMPLE_HOLD_SAME_TIME(pDAC, 0x3FF);
            DAC_SET_SAMPLE_HOLD_HOLD_TIME(pDAC, 0x3FF);
            DAC_SET_SAMPLE_HOLD_REFRESH_TIME(pDAC, 0xFF);

            DAC0_SET_DOUL_HOLD_DATA(pDAC_COM, 0xFFF);
        }
        else
        {
            pDAC = DAC1_BASE;
            DAC_SET_SAMPLE_HOLD_SAME_TIME(pDAC, 0x3FF);
            DAC_SET_SAMPLE_HOLD_HOLD_TIME(pDAC, 0x3FF);
            DAC_SET_SAMPLE_HOLD_REFRESH_TIME(pDAC, 0xFF);

            DAC1_SET_DOUL_HOLD_DATA(pDAC_COM, 0xFFF);
        }

        DAC_ENABLE(pDAC);

        PR_DEBUG("=======DAC%d func[%s] middle====================\n\n", i, __FUNCTION__);
        dump_reg(pDAC, 1);
        dump_common_reg(pDAC_COM, 1);
        PR_DEBUG("=======DAC%d func[%s] end====================\n\n", i, __FUNCTION__);
    }

    DAC_DUAL_SOFT_TRIG(pDAC_COM);
    dump_common_reg(pDAC_COM, 1);

    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);

}
#endif

/*
1.Enable DAC1/2
2.Configure the DAC pinmux
3.Write DAC data hold register
4.Trigger DAC to load the data
5.Check the voltage DAC PIN
2.  DAC （2个）
1)  D_DAC_EN可配置
2)  D_DAC_SHEN可配置
3)  D_DAC_CODE[11：0]可配置

*/

#define HOLD_VALUE   (0x6FF)
void dac_ES_test()
{
    int i = 0;
    DAC_Device_T *pDAC = DAC0_BASE;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    DAC_ConfigPinmux();

    for (i = 0; i < 2; i++)
    {
        PR_DEBUG("=======DAC%d func[%s] begin====================\n\n", i, __FUNCTION__);
        dump_reg(pDAC, 1);
        if (i == 0)
        {
            pDAC = DAC0_BASE;
        }
        else
        {
            pDAC = DAC1_BASE;
        }

        DAC_SET_DATAHOLD_REG(pDAC, 0X6ff);  // 0XFFF  /*41~4042*/  /*3) D_DAC_CODE[11：0]可配置*/

        DAC_ENABLE(pDAC); /*1)  D_DAC_EN可配置*/
        DAC_SHEN_ENABLE(pDAC); /*2) D_DAC_SHEN可配置*/

        DAC_SOFT_TRIG(pDAC);
        PR_DEBUG("=======DAC%d func[%s] middle====================\n\n", i, __FUNCTION__);
        dump_reg(pDAC, 1);
        PR_DEBUG("=======DAC%d func[%s] end====================\n\n", i, __FUNCTION__);
    }

    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}


void dac_ES_test_demo()
{
    int i = 0;
    int j = 0;
    DAC_Device_T *pDAC = DAC0_BASE;
    DAC_Device_T *pDAC1 = DAC1_BASE;
    HW32_REG(0x400000A0) |= 1 << 24;  // DAC clock select

    //DAC_CLK_SEL(1);

    DAC_CLKEN();
    DAC_RESET();

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    DAC_ConfigPinmux();

    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            pDAC = DAC0_BASE;
        }
        else
        {
            pDAC = DAC1_BASE;
        }

        for (j = 0; j < HOLD_VALUE + 1; j++)
        {
            DAC_SET_DATAHOLD_REG(pDAC, j);  // 0XFFF  /*41~4042*/  /*3) D_DAC_CODE[11：0]可配置*/

            DAC_ENABLE(pDAC); /*1)  D_DAC_EN可配置*/
            //DAC_SHEN_ENABLE(pDAC); /*2)   D_DAC_SHEN可配置*/

            DAC_SOFT_TRIG(pDAC);
            PR_DEBUG("=======DAC%d func[%s] middle======== j=%d =========\n\n", i, __FUNCTION__, j);
            dump_reg(pDAC, 1);
            PR_DEBUG("=======DAC%d func[%s] end========== j=%d  ==========\n\n", i, __FUNCTION__, j);
            k_sleep(100);
        }
    }


    PR_DEBUG("exit: func[%s]\n", __FUNCTION__);
}

void common_init(void)
{
    patch_config();
    gpio_clk_enable();
    k_sleep(100);
    gpio_reset_release();
    k_sleep(100);

    DAC_CLK_SEL(LIRC_CLOCK);
    //  HW32_REG(0x400000A0) |= 1 << 24;  // DAC clock select
    DAC_CLKEN();
    DAC_RESET();
}

void dac_basic_debug(int data_hold)
{
    int i = 0;
    DAC_Device_T *pDAC = DAC0_BASE;

    PR_DEBUG("enter: func[%s]\n", __FUNCTION__);
    DAC_ConfigPinmux();

    for (i = 0; i < 1; i++)
    {
        PR_DEBUG("=======DAC%d func[%s] begin====================\n\n", i, __FUNCTION__);
        //dump_reg(pDAC, 1);
        if (i == 0)
        {
            pDAC = DAC0_BASE;
        }
        else
        {
            pDAC = DAC1_BASE;
        }

        DAC_SET_DATAHOLD_REG(pDAC, data_hold);  // 0XFFF  /*41~4042*/
        DAC_ENABLE(pDAC);
        DAC_SOFT_TRIG(pDAC);
        PR_DEBUG("=======DAC%d func[%s] middle====================\n\n", i, __FUNCTION__);
        dump_reg(pDAC, 1);
        PR_DEBUG("=======DAC%d func[%s] end====================\n\n", i, __FUNCTION__);
    }

    PR_DEBUG("exit: func[%s] while(1)\n", __FUNCTION__);


}


static void HAL_ADC_CLOCK_STATIC(int en)
{
    if (en == 0)
    {
        *(volatile unsigned int *)(0x40000080) &= ~(1 << 12); //disabled the ADC clock
    }
    else
    {
        *(volatile unsigned int *)(0x40000080) |= (1 << 12); //enable (release) the ADC clock, ADC1 clock
        *(volatile unsigned int *)(0x40000080) |= (1 << 13);
        *(volatile unsigned int *)(0x40000080) |= (1 << 20);
    }
}


#include <adc.h>
#define PR_DEBUG            printk


static ADC_Common_TypeDef *pComDev = ADC_COM_DEV;


static void adc_clk_sel(int adc_sel, int clk_val)
{
    if (clk_val < 0 || clk_val > 3)
    {
        return;
    }
    if (adc_sel == 0)
    {
        HW32_REG(0x4000009c) &= ~(3 << 0);
        HW32_REG(0x4000009c) |= (clk_val << 0); // ADC0 CLK select
    }
    else if (adc_sel == 1)
    {
        HW32_REG(0x4000009c) &= ~(3 << 2);
        HW32_REG(0x4000009c) |= (clk_val << 2); // ADC1 CLK select
    }
    else if (adc_sel == 2)
    {
        HW32_REG(0x4000009c) &= ~(3 << 4);
        HW32_REG(0x4000009c) |= (clk_val << 4); // ADC2 CLK select
    }
}

static void adc_clk_div(int adc_sel, int clk_div_val)
{
    if (clk_div_val < 0 || clk_div_val > 15)
    {
        return;
    }
    if (adc_sel == 0)
    {
        HW32_REG(0x4000009c) &= ~(0xF << 8);
        HW32_REG(0x4000009c) |= (clk_div_val << 8); // ADC0 CLK div select
    }
    else if (adc_sel == 1)
    {
        HW32_REG(0x4000009c) &= ~(0xF << 24);
        HW32_REG(0x4000009c) |= (clk_div_val << 24); // ADC1 CLK div select
    }
    else if (adc_sel == 2)
    {
        HW32_REG(0x4000009c) &= ~(0xF << 28);
        HW32_REG(0x4000009c) |= (clk_div_val << 28); // ADC2 CLK div select
    }
}

static void udma_reset_release(void)
{
    HW32_REG(0x40000070) |= (1 << 2);
}

static void udma_clk_enable(void)
{
    HW32_REG(0x40000080) |= (1 << 2);
}


typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

static int adc_pinumx_config(struct device *port)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_uartPinMux[2] =
    {
        {PINMUX_GPIO_PIN_0, GPIOC, 0},
    };

    pin_cfg.pin = BIT(s_uartPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_ANALOG;
    pin_cfg.pull = GPIO_NOPULL;
    ret = pinmux_pin_set(port, s_uartPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}

unsigned int adc_basic_debug(ADC_Device_T *pADC)
{
    vint16_t i = 0;
    int channel_num = 0;
    int k = 0;

    udma_clk_enable();
    udma_reset_release();
    gpio_clk_enable();
    gpio_reset_release();
    patch_config();

    //HAL_ADC_CLOCK(0);
    HAL_ADC_CLOCK_STATIC(1);
    HAL_ADC_RESET();

    adc_clk_sel(0, 0);
    adc_clk_sel(1, 0);
    adc_clk_sel(2, 0);

    adc_clk_div(0, 7);
    adc_clk_div(1, 7);
    adc_clk_div(2, 7);

    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);

    adc_pinumx_config(portc);


    pADC->CTRL = 0;
    pADC->ST |= pADC->ST;

    pADC->CTRL &= ~ADC_CTRL_WM;
    pADC->CTRL |= ADC_CTRL_SINGLE_MODE | ADC_CTRL_EN;
    pADC->CTRL &= ~(ADC_CTRL_DCONTEN);

    pADC->CTRL &= ~(3 << 8);
    pADC->CTRL |= ADC_RESOLUTION_12BIT << 8;

    pADC->RCHN2 &= ~(0xF << 28);
    pADC->RCHN2 |= (0x0 << 28); /*Sequence length*/

    pADC->RCHN0 = 0x1E;   /*Channel selection*/
    pADC->RCHN1 = 0;

    pADC->RCHN2 = 0;

    //pComDev->EXCHEN = 1;

    static int cnt = 0;
    static int save_val = 0;
    int  value = 0;

    HAL_ADC_Sample_Time(pADC);

    do
    {
        HAL_ADC_Triger(pADC, ADC_SW_REGU_TRIGER, ADC_SW_TRIGER, ADC_HW_TRIGER_NONE);
        while ((pADC->ST & ADC_ST_EOCE) != ADC_ST_EOCE);

        for (channel_num = 0; channel_num < 1; channel_num++)
        {
            value = pADC->DRST;
            save_val += value;
        }

        cnt++;

        PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, value);

        if (cnt >= 128)
        {
            PR_DEBUG("adc avd=[%d]\n", save_val / 128);
            cnt = 0;
            save_val = 0;
        }
        k_sleep(20);
        k++;

        pADC->ST = ADC_ST_EOCE;
        while (pADC->ST & ADC_ST_EOCE == ADC_ST_EOCE);

        i++;
    } while (k < 5);
    PR_DEBUG("i=[%d], pADC->DRST=[%d]\n", i, value);
    PR_DEBUG("ADC_SingleModeNbitTest end\n");

    return value;

}

#if 0
    /*
    GM6721 测试发现ADC偏差较大，且偏差现象和XAVREFH以及Vin信号在芯片内部都被衰减导致的偏差吻合，需要麻烦皓哥修改测试程序来验证这一假设。

    1） Test case1: Pad'ADC'DAC'Pad
    将ADC的转换结果作为DAC的输入，对比ADC输入和DAC输出的差别。
    这种case抵消了XAVREFH偏差，理论上只有Vin偏差，为固定偏差，若测试结果表现为均匀的偏差，则能证实确实Vin在内部被衰减。
    代码需求：使能ADC1和DAC1，将ADC1_OUT_D[11:0]赋给D_DAC1_CODE[11：0],测试DAC1输出。
    */
#endif

void debug_adc_and_dac()
{
    unsigned int adc_value = 0;
    common_init();
    adc_value = adc_basic_debug(ADC_DEV0);
    dac_basic_debug(adc_value);
}

#if 0
    /*
    2） Test case2：DAC'ADC
    信号走内部通道，将DAC的输出，给ADC去转换，对比DAC输入和ADC输出的差别。
    这种case理论上抵消了XAVREFH的偏差，且没有Vin的偏差，测试结果应该表现为无偏差（偏差很小）。
    代码需求：使能ADC1和DAC1，ADC选择通道30（DAC1_OUT）,观察ADC1_OUT_D[11：0]
    */
#endif


//unsigned int dac_value=0;  //wj

void debug_dac_and_adc()
{
    //for(dac_value=127; dac_value<4096; )   //wj

    //{
    unsigned int dac_value = 3711;//0xFFF;
    unsigned int adc_value = 0;
    common_init();
    dac_basic_debug(dac_value);
    adc_value = adc_basic_debug(ADC_DEV0);
    PR_DEBUG("adc_value=[%d]\n", adc_value);

    //dac_value+=127;
    // }
    while (1);

}


void dac_demo(void)
{
    int i = 0;
    uint32_t val = 0;

    PR_DEBUG("Enter func[%s]\n", __FUNCTION__);

    common_init();

#if defined(TEST_CASE0)
    dac_clk_test(DAC0_DEV);
    dac_reset_test(DAC0_DEV);
#endif

#if defined(TEST_CASE1)
    printf("DAC0 Start Test\n\n");
    dac_reg_access(DAC0_DEV);
    printf("DAC1 Start Test\n\n");
    dac_reg_access(DAC1_DEV);
    printf("DAC Start End\n\n");
#endif

#if defined(TEST_CASE3)
    dac_interrupt_test(DAC0_DEV);
#endif

#if defined(TEST_CASE4)
    dac_basic_test();
#endif

#if defined(TEST_CASE5)
    dac_noise_demo();
#endif

#if defined(TEST_CASE6)
    dac_traingle_wave_demo();
#endif


#if defined(TEST_CASE7)
    dac_dual_control_test();
#endif

#if defined(TEST_CASE8)
    dac_sample_hold_test();
#endif
    PR_DEBUG("Exit func[%s] while(1) ...\n", __FUNCTION__);
    while (1);
    return;
}

void main(void)
{
    PR_DEBUG("Enter func[%s]\n", __FUNCTION__);
    dac_demo();
    //dac_demo_app();

    //debug_adc_and_dac();

    //debug_dac_and_adc();
    while (1)
    {
        k_sleep(3000);
        PR_DEBUG("midlle func[%s]\n", __FUNCTION__);
    }

    PR_DEBUG("Exit func[%s]\n", __FUNCTION__);
}
