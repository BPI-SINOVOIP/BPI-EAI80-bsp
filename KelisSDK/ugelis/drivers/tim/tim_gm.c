/*
 * Copyright (c) 2016, Gree Instruments Incorporated.
 *
 */

/*uGelis include files*/
#include <logging/sys_log.h>
#include <errno.h>
#include <device.h>
#include <init.h>
#include <kernel.h>
#include <tim.h>
#include <soc.h>
#include <misc/util.h>
#include <gm_hal_tim.h>
#include "tim_gm.h"

#define DEBUG()  __asm("BKPT")

#define CHANNEL_LENGTH 4

//#define TIM_DEBUG 1
#define TIM_ERR   1

#ifndef TIM_DEBUG
    #define tim_debug(...)      do {} while (0)
#else
    #define tim_debug           printk
#endif

#ifndef TIM_ERR
    #define tim_err(...)        do {} while (0)
#else
    #define tim_err             printk
#endif


static int gm_tim_init(struct device *dev);
static int gm_tim_timebase_init(struct device *dev);
static int gm_tim_outputcompare_init(struct device *dev);
static int gm_tim_pwm_init(struct device *dev, tim_timebaseinit *init);
static int gm_tim_inputcapture_init(struct device *dev, tim_timebaseinit *init);
static int gm_tim_onepulse_init(struct device *dev, uint32_t opm);
static int gm_tim_encoder_init(struct device *dev, tim_encodeinit *cfg);
static int gm_tim_hallsensor_init(struct device *dev, tim_hallsensorinit *cfg);
static int gm_tim_irq_config(struct device *dev);

static int gm_tim_timebase_start(struct device *dev);
static int gm_tim_outputcompare_start(struct device *dev, uint32_t ch);
static int gm_tim_pwm_start(struct device *dev, uint32_t ch);
static int gm_tim_readcapturevalue_start(struct device *dev, uint32_t ch);
static int gm_tim_inputcapture_start(struct device *dev, uint32_t ch);
static int gm_tim_onepulse_start(struct device *dev, uint32_t ch);
static int gm_tim_encoder_start(struct device *dev, uint32_t ch);
static int gm_tim_hallsensor_start(struct device *dev);
static int gm_tim_occhannel_cfg(struct device *dev, tim_outputcompareinit *cfg, uint32_t ch);
static int gm_tim_icchannel_cfg(struct device *dev, tim_inputcaptureinit *cfg, uint32_t ch);
static int gm_tim_pwmchannel_cfg(struct device *dev, tim_outputcompareinit *cfg, uint32_t ch);
static int gm_tim_opmchannel_cfg(struct device *dev, tim_opminit *cfg, uint32_t ch1, uint32_t ch2);
static int gm_tim_mastersyn_config(struct device *dev, tim_mstconfig *cfg);
static int gm_tim_slavesyn_config(struct device *dev, tim_slvconfig *cfg);
static int gm_tim_bdt_config(struct device *dev, tim_bdtconfig *cfg);
static int gm_tim_pin_set(struct device *dev, uint32_t pwm, uint32_t period_cycles, uint32_t pulse_cycles);

static const struct tim_driver_api tim_driver_api_funcs =
{
    .timebase_init = gm_tim_timebase_init,
    .outputcompare_init = gm_tim_outputcompare_init,
    .pwm_init = gm_tim_pwm_init,
    .inputcapture_init = gm_tim_inputcapture_init,
    .onepulse_init = gm_tim_onepulse_init,
    .encoder_init = gm_tim_encoder_init,
    .hallsensor_init = gm_tim_hallsensor_init,
    .timebase_start = gm_tim_timebase_start,
    .outputcompare_start = gm_tim_outputcompare_start,
    .pwm_start = gm_tim_pwm_start,
    .readcapturevalue_start = gm_tim_readcapturevalue_start,
    .inputcapture_start = gm_tim_inputcapture_start,
    .onepulse_start = gm_tim_onepulse_start,
    .encoder_start = gm_tim_encoder_start,
    .hallsensor_start = gm_tim_hallsensor_start,
    .occhannel_cfg = gm_tim_occhannel_cfg,
    .icchannel_cfg = gm_tim_icchannel_cfg,
    .pwmchannel_cfg = gm_tim_pwmchannel_cfg,
    .opmchannel_cfg = gm_tim_opmchannel_cfg,
    .mastersyn_config = gm_tim_mastersyn_config,
    .slavesyn_config = gm_tim_slavesyn_config,
    .bdt_config = gm_tim_bdt_config,
    .pin_set = gm_tim_pin_set,
};

#ifdef TIM1_LABEL
static TIM_Handle_T tim1_handle;
static struct tim_gm_data_t tim1_data =
{
    .tim_handle = &tim1_handle,
};

static const struct tim_gm_config_t tim1_config =
{
    .base = (u32_t)TIM1_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim1, CONFIG_TIM1_NAME,
              gm_tim_init, NULL, &tim1_data, &tim1_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);

#endif

#ifdef TIM2_LABEL
static TIM_Handle_T tim2_handle;

static struct tim_gm_data_t tim2_data =
{
    .tim_handle = &tim2_handle,

};

static const struct tim_gm_config_t tim2_config =
{
    .base = (u32_t)TIM2_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim2, CONFIG_TIM2_NAME,
              gm_tim_init, NULL, &tim2_data, &tim2_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif

#ifdef TIM3_LABEL
static TIM_Handle_T tim3_handle;

static struct tim_gm_data_t tim3_data =
{
    .tim_handle = &tim3_handle,

};

static const struct tim_gm_config_t tim3_config =
{
    .base = (u32_t)TIM3_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim3, CONFIG_TIM3_NAME,
              gm_tim_init, NULL, &tim3_data, &tim3_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif

#ifdef TIM4_LABEL
static TIM_Handle_T tim4_handle;

static struct tim_gm_data_t tim4_data =
{
    .tim_handle = &tim4_handle,

};

static const struct tim_gm_config_t tim4_config =
{
    .base = (u32_t)TIM4_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim4, CONFIG_TIM4_NAME,
              gm_tim_init, NULL, &tim4_data, &tim4_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif

#ifdef TIM5_LABEL
static TIM_Handle_T tim5_handle;

static struct tim_gm_data_t tim5_data =
{
    .tim_handle = &tim5_handle,

};

static const struct tim_gm_config_t tim5_config =
{
    .base = (u32_t)TIM5_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim5, CONFIG_TIM5_NAME,
              gm_tim_init, NULL, &tim5_data, &tim5_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif

#ifdef TIM8_LABEL
static TIM_Handle_T tim8_handle;

static struct tim_gm_data_t tim8_data =
{
    .tim_handle = &tim8_handle,

};

static const struct tim_gm_config_t tim8_config =
{
    .base = (u32_t)TIM8_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim8, CONFIG_TIM8_NAME,
              gm_tim_init, NULL, &tim8_data, &tim8_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif

#ifdef CHIP_GM6721
#ifdef TIM9_LABEL
static TIM_Handle_T tim9_handle;

static struct tim_gm_data_t tim9_data =
{
    .tim_handle = &tim9_handle,

};

static const struct tim_gm_config_t tim9_config =
{
    .base = (u32_t)TIM9_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim9, CONFIG_TIM9_NAME,
              gm_tim_init, NULL, &tim9_data, &tim9_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif

#ifdef TIM10_LABEL
static TIM_Handle_T tim10_handle;

static struct tim_gm_data_t tim10_data =
{
    .tim_handle = &tim10_handle,

};

static const struct tim_gm_config_t tim10_config =
{
    .base = (u32_t)TIM10_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim10, CONFIG_TIM10_NAME,
              gm_tim_init, NULL, &tim10_data, &tim10_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif

#ifdef TIM11_LABEL
static TIM_Handle_T tim11_handle;

static struct tim_gm_data_t tim11_data =
{
    .tim_handle = &tim11_handle,

};

static const struct tim_gm_config_t tim11_config =
{
    .base = (u32_t)TIM11_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim11, CONFIG_TIM11_NAME,
              gm_tim_init, NULL, &tim11_data, &tim11_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif

#ifdef TIM12_LABEL
static TIM_Handle_T tim12_handle;

static struct tim_gm_data_t tim12_data =
{
    .tim_handle = &tim12_handle,

};

static const struct tim_gm_config_t tim12_config =
{
    .base = (u32_t)TIM12_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim12, CONFIG_TIM12_NAME,
              gm_tim_init, NULL, &tim12_data, &tim12_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif

#ifdef TIM13_LABEL
static TIM_Handle_T tim13_handle;

static struct tim_gm_data_t tim13_data =
{
    .tim_handle = &tim13_handle,

};

static const struct tim_gm_config_t tim13_config =
{
    .base = (u32_t)TIM13_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim13, CONFIG_TIM13_NAME,
              gm_tim_init, NULL, &tim13_data, &tim13_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif

#ifdef TIM14_LABEL
static TIM_Handle_T tim14_handle;

static struct tim_gm_data_t tim14_data =
{
    .tim_handle = &tim14_handle,

};

static const struct tim_gm_config_t tim14_config =
{
    .base = (u32_t)TIM14_BASE_ADDRESS,
};

DEVICE_DEFINE(gm_tim14, CONFIG_TIM14_NAME,
              gm_tim_init, NULL, &tim14_data, &tim14_config,
              POST_KERNEL, DEVICE_PRIORITY_TIM,
              &tim_driver_api_funcs);
#endif
#endif


static void gm_tim_isr(struct device *dev)
{
    if (DEV_STRUCT(dev) == TIM1)
    {
        HAL_TIM1_InterruptHandler();
    }
    else if (DEV_STRUCT(dev) == TIM2)
    {
        HAL_TIM2_InterruptHandler();
    }
    else if (DEV_STRUCT(dev) == TIM3)
    {
        HAL_TIM3_InterruptHandler();
    }
    else if (DEV_STRUCT(dev) == TIM4)
    {
        HAL_TIM4_InterruptHandler();
    }
    else if (DEV_STRUCT(dev) == TIM5)
    {
        HAL_TIM5_InterruptHandler();
    }
    else if (DEV_STRUCT(dev) == TIM8)
    {
        HAL_TIM8_InterruptHandler();
    }

#ifdef CHIP_GM6721
    else if (DEV_STRUCT(dev) == TIM9)
    {
        HAL_TIM9_InterruptHandler();
    }
    else if (DEV_STRUCT(dev) == TIM10)
    {
        HAL_TIM10_InterruptHandler();
    }
    else if (DEV_STRUCT(dev) == TIM11)
    {
        HAL_TIM11_InterruptHandler();
    }
    else if (DEV_STRUCT(dev) == TIM12)
    {
        HAL_TIM12_InterruptHandler();
    }
    else if (DEV_STRUCT(dev) == TIM13)
    {
        HAL_TIM13_InterruptHandler();
    }
    else if (DEV_STRUCT(dev) == TIM14)
    {
        HAL_TIM14_InterruptHandler();
    }
#endif

    else
    {

    }
}

static struct device DEVICE_NAME_GET(gm_tim1);
static int gm_tim_irq_config(struct device *dev)
{
    int ret = 0;

    if (DEV_STRUCT(dev) == TIM1)
    {
        IRQ_CONNECT(TIM1_IRQn, TIM1_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim1), 0);
        irq_enable(TIM1_IRQn);
    }
    else if (DEV_STRUCT(dev) == TIM2)
    {
        IRQ_CONNECT(TIM2_IRQn, TIM2_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim2), 0);
        irq_enable(TIM2_IRQn);
    }
    else if (DEV_STRUCT(dev) == TIM3)
    {
        IRQ_CONNECT(TIM3_IRQn, TIM3_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim3), 0);
        irq_enable(TIM3_IRQn);
    }
    else if (DEV_STRUCT(dev) == TIM4)
    {
        IRQ_CONNECT(TIM4_IRQn, TIM4_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim4), 0);
        irq_enable(TIM4_IRQn);
    }
    else if (DEV_STRUCT(dev) == TIM5)
    {
        IRQ_CONNECT(TIM5_IRQn, TIM5_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim5), 0);
        irq_enable(TIM5_IRQn);
    }
    else if (DEV_STRUCT(dev) == TIM8)
    {
        IRQ_CONNECT(TIM8_IRQn, TIM8_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim8), 0);
        irq_enable(TIM8_IRQn);
    }

#ifdef CHIP_GM6721
    else if (DEV_STRUCT(dev) == TIM9)
    {
        IRQ_CONNECT(TIM9_IRQn, TIM9_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim9), 0);
        irq_enable(TIM9_IRQn);
    }
    else if (DEV_STRUCT(dev) == TIM10)
    {
        IRQ_CONNECT(TIM10_IRQn, TIM10_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim10), 0);
        irq_enable(TIM10_IRQn);
    }
    else if (DEV_STRUCT(dev) == TIM11)
    {
        IRQ_CONNECT(TIM11_IRQn, TIM11_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim11), 0);
        irq_enable(TIM11_IRQn);
    }
    else if (DEV_STRUCT(dev) == TIM12)
    {
        IRQ_CONNECT(TIM12_IRQn, TIM2_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim12), 0);
        irq_enable(TIM12_IRQn);
    }
    else if (DEV_STRUCT(dev) == TIM13)
    {
        IRQ_CONNECT(TIM13_IRQn, TIM13_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim13), 0);
        irq_enable(TIM13_IRQn);
    }
    else if (DEV_STRUCT(dev) == TIM14)
    {
        IRQ_CONNECT(TIM14_IRQn, TIM14_IRQ_PRIORITY, gm_tim_isr, DEVICE_GET(gm_tim14), 0);
        irq_enable(TIM14_IRQn);
    }
#endif

    else
    {
        ret = - ENXIO;
    }
    return ret;

}

static int gm_tim_init(struct device *dev)
{
    int ret = 0;
    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    ret = gm_tim_irq_config(dev);
    if (ret)
    {
        tim_err("%s:gm_tim_irq_config err:%d\n", __FUNCTION__, ret);
        goto out;
    }

    ret = HAL_TIM_InitCommon(hTIM);
    if (ret)
    {
        tim_err("Err:%x HAL_TIM_Common_Init\n");
    }
    else
    {
        tim_debug("TIM Common Init OK\n");
    }

out:
    return ret;

}

static int gm_tim_timebase_init(struct device *dev)
{
    int ret = 0;

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM  = data->tim_handle;

    ret = gm_tim_init(dev);
    if (!ret)
    {
        tim_debug("tim common init ok\n");
    }
    else
    {
        tim_debug("tim common init fail\n");
    }

    ret  = HAL_TIM_InitTimeBase(hTIM);
    if (ret)
    {
        tim_err("%s:gm_tim_timebase_init err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_outputcompare_init(struct device *dev)
{
    int ret = 0;

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM  = data->tim_handle;

    ret = gm_tim_init(dev);
    if (!ret)
    {
        tim_debug("tim common init ok\n");
    }
    else
    {
        tim_debug("tim common init fail\n");
    }

    ret  = HAL_TIM_InitOutputCompare(hTIM);
    if (ret)
    {
        tim_err("%s:gm_tim_outputcompare_init err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_pwm_init(struct device *dev, tim_timebaseinit *init)
{
    int ret = 0;

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM  = data->tim_handle;
    hTIM->instance = DEV_STRUCT(dev);
    ret = gm_tim_init(dev);
    if (!ret)
    {
        tim_debug("tim common init ok\n");
    }
    else
    {
        tim_debug("tim common init fail\n");
    }

    hTIM->init.prescaler = init->prescaler;
    hTIM->init.clockDivision = init->clockDivision;
    hTIM->init.counterMode = init->counterMode;

    hTIM->init.period = init->period;
    ret  = HAL_TIM_InitPWM(hTIM);
    if (ret)
    {
        tim_err("%s:gm_tim_pwm_init err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}


static int gm_tim_inputcapture_init(struct device *dev, tim_timebaseinit *init)
{
    int ret = 0;

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM  = data->tim_handle;
    hTIM->instance = DEV_STRUCT(dev);
    ret = gm_tim_init(dev);
    if (!ret)
    {
        tim_debug("tim common init ok\n");
    }
    else
    {
        tim_debug("tim common init fail\n");
    }
    hTIM->init.prescaler = init->prescaler;
    hTIM->init.clockDivision = init->clockDivision;
    hTIM->init.counterMode = init->counterMode;
    hTIM->init.period = init->period;
    hTIM->init.callback = (TIM_IRQCallback_T)init->callback;


    HAL_TIM_InitCommon(hTIM);


    ret  = HAL_TIM_InitInputCapture(hTIM);
    if (ret)
    {
        tim_err("%s:gm_tim_inputcapture_init err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_onepulse_init(struct device *dev, uint32_t opm)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);
    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM  = data->tim_handle;

    ret = gm_tim_init(dev);
    if (!ret)
    {
        tim_debug("tim common init ok\n");
    }
    else
    {
        tim_debug("tim common init fail\n");
    }

    ret  = HAL_TIM_InitOnePulse(hTIM, TIMx, opm);
    if (ret)
    {
        tim_err("%s:gm_tim_onepulse_init err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_encoder_init(struct device *dev, tim_encodeinit *cfg)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);
    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM  = data->tim_handle;

    TIM_EncoderInit_T encodeInit;

    ret = gm_tim_init(dev);
    if (!ret)
    {
        tim_debug("tim common init ok\n");
    }
    else
    {
        tim_debug("tim common init fail\n");
    }

    encodeInit.encoderMode = cfg->encoderMode;
    encodeInit.ic1Filter = cfg->ic1Filter;
    encodeInit.ic1Polarity = cfg->ic1Polarity;
    encodeInit.ic1Prescaler = cfg->ic1Prescaler;
    encodeInit.ic1Selection = cfg->ic1Selection;

    ret  = HAL_TIM_InitEncoder(hTIM, TIMx, &encodeInit);
    if (ret)
    {
        tim_err("%s:gm_tim_encoder_init err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_hallsensor_init(struct device *dev, tim_hallsensorinit *cfg)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);
    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM  = data->tim_handle;

    TIM_HallSensorInit_T hallsensorInit;

    ret = gm_tim_init(dev);
    if (!ret)
    {
        tim_debug("tim common init ok\n");
    }
    else
    {
        tim_debug("tim common init fail\n");
    }

    hallsensorInit.commutation_Delay = cfg->commutation_Delay;
    hallsensorInit.ic1Filter = cfg->ic1Filter;
    hallsensorInit.ic1Polarity = cfg->ic1Polarity;
    hallsensorInit.ic1Prescaler = cfg->ic1Prescaler;

    ret  = HAL_TIM_InitHallSensor(hTIM, TIMx, &hallsensorInit);
    if (ret)
    {
        tim_err("%s:gm_tim_hallsensor_init err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_timebase_start(struct device *dev)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    ret  = HAL_TIM_StartTimeBase(hTIM, TIMx);
    if (ret)
    {
        tim_err("%s:gm_tim_timebase_start err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_outputcompare_start(struct device *dev, uint32_t ch)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    ret  = HAL_TIM_StartOutputCompare(hTIM, TIMx, ch);
    if (ret)
    {
        tim_err("%s:gm_tim_outputcompare_start err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_pwm_start(struct device *dev, uint32_t ch)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    hTIM->instance = DEV_STRUCT(dev);
    ret  = HAL_TIM_StartPWM(hTIM, TIMx, ch);
    if (ret)
    {
        tim_err("%s:gm_tim_pwm_start err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_readcapturevalue_start(struct device *dev, uint32_t ch)
{
    int value = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    hTIM->instance = DEV_STRUCT(dev);
    value  = HAL_TIM_ReadCapturedValue(hTIM, TIMx, ch);

    return value;
}
static int gm_tim_inputcapture_start(struct device *dev, uint32_t ch)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;
    IO_BITMASK_SET(*(volatile uint32_t *)(0x40007100), 0xfff, 0x000);
    hTIM->instance = DEV_STRUCT(dev);
    ret  = HAL_TIM_StartInputCapture(hTIM, TIMx, ch);
    if (ret)
    {
        tim_err("%s:gm_tim_inputcapture_start err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_onepulse_start(struct device *dev, uint32_t ch)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    ret  = HAL_TIM_StartOnePulse(hTIM, TIMx, ch);
    if (ret)
    {
        tim_err("%s:gm_tim_onepulse_start err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_encoder_start(struct device *dev, uint32_t ch)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    ret  = HAL_TIM_StartEncoder(hTIM, TIMx, ch);
    if (ret)
    {
        tim_err("%s:gm_tim_encoder_start err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_hallsensor_start(struct device *dev)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    ret  = HAL_TIM_StartHallSensor(hTIM, TIMx);
    if (ret)
    {
        tim_err("%s:gm_tim_hallsensor_start err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_occhannel_cfg(struct device *dev, tim_outputcompareinit *cfg, uint32_t ch)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    TIM_OutputCompareInit_T ocConfig;

    ocConfig.ocFastMode = cfg->ocFastMode;
    ocConfig.ocIdleState = cfg->ocIdleState;
    ocConfig.ocMode = cfg->ocMode;
    ocConfig.ocNIdleState = cfg->ocNIdleState;
    ocConfig.ocNPolarity = cfg->ocNPolarity;

    ret  = HAL_TIM_ConfigOCChannel(hTIM, TIMx, &ocConfig, ch);
    if (ret)
    {
        tim_err("%s:gm_tim_occhannel_cfg err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_icchannel_cfg(struct device *dev, tim_inputcaptureinit *cfg, uint32_t ch)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    TIM_InputCaptureInit_T icConfig;

    icConfig.icFilter = cfg->icFilter;
    icConfig.icPolarity = cfg->icPolarity;
    icConfig.icPrescaler = cfg->icPrescaler;
    icConfig.icSelection = cfg->icSelection;

    ret  = HAL_TIM_ConfigICChannel(hTIM, TIMx, &icConfig, ch);
    if (ret)
    {
        tim_err("%s:gm_tim_icchannel_cfg err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_pwmchannel_cfg(struct device *dev, tim_outputcompareinit *cfg, uint32_t ch)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    TIM_OutputCompareInit_T ocConfig;

    ocConfig.ocMode = cfg->ocMode;
    ocConfig.pulse = cfg->pulse;
    ocConfig.ocPolarity = cfg->ocPolarity;
    ocConfig.ocFastMode = cfg->ocFastMode;
    ocConfig.ocIdleState = cfg->ocIdleState;
    ocConfig.ocNIdleState = cfg->ocNIdleState;
    ocConfig.ocNPolarity = cfg->ocNPolarity;

    ret  = HAL_TIM_ConfigPWMChannel(hTIM, TIMx, &ocConfig, ch);
    if (ret)
    {
        tim_err("%s:gm_tim_pwmchannel_cfg err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_opmchannel_cfg(struct device *dev, tim_opminit *cfg, uint32_t ch1, uint32_t ch2)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    TIM_OnePulseInit_T opmConfig;

    opmConfig.icFilter = cfg->icFilter;
    opmConfig.icPolarity = cfg->icPolarity;
    opmConfig.icSelection = cfg->icSelection;
    opmConfig.ocIdleState = cfg->ocIdleState;

    ret  = HAL_TIM_ConfigOPMChannel(hTIM, TIMx, &opmConfig, ch1, ch2);
    if (ret)
    {
        tim_err("%s:gm_tim_opmchannel_cfg err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_mastersyn_config(struct device *dev, tim_mstconfig *cfg)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    TIM_MasterConfig_T masterConfig;

    masterConfig.masterOutputTrigger = cfg->masterOutputTrigger;
    masterConfig.masterSlaveMode = cfg->masterSlaveMode;

    ret  = HAL_TIM_ConfigMasterSyn(hTIM, TIMx, &masterConfig);
    if (ret)
    {
        tim_err("%s:gm_tim_mastersyn_config err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_slavesyn_config(struct device *dev, tim_slvconfig *cfg)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    TIM_SlaveConfig_T slaveConfig;

    slaveConfig.inputTrigger = cfg->inputTrigger;
    slaveConfig.slaveMode = cfg->slaveMode;
    slaveConfig.triggerFilter = cfg->triggerFilter;
    slaveConfig.triggerPolarity = cfg->triggerPolarity;
    slaveConfig.triggerPrescaler = cfg->triggerPrescaler;

    ret  = HAL_TIM_ConfigSlaveSyn(hTIM, TIMx, &slaveConfig);
    if (ret)
    {
        tim_err("%s:gm_tim_slavesyn_config err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

static int gm_tim_bdt_config(struct device *dev, tim_bdtconfig *cfg)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);

    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    TIM_BreakDeadTimeConfig_T bdtConfig;

    bdtConfig.deadTime = cfg->deadTime;
    bdtConfig.automaticOutput = cfg->automaticOutput;
    bdtConfig.breakPolarity = cfg->breakPolarity;
    bdtConfig.breakState = cfg->breakState;
    bdtConfig.lockLevel = cfg->lockLevel;

    //TIM_BreakDeadTimeConfig_T *sBreakDeadTimeConfig;
    //TIM_BreakDeadTimeConfig_T *sBreakDeadTimeConfig = cfg;
    //ret  = HAL_TIM_ConfigBreakDeadTime(hTIM, TIMx, sBreakDeadTimeConfig);

    ret  = HAL_TIM_ConfigBreakDeadTime(hTIM, TIMx, &bdtConfig);
    if (ret)
    {
        tim_err("%s:gm_tim_bdt_config err:%d\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}

/*
 * Set the period and pulse width for a PWM pin.
 *
 * Parameters
 * dev: Pointer to PWM device structure
 * pwm: PWM channel to set
 * period_cycles: Period (in timer count)
 * pulse_cycles: Pulse width (in timer count).
 *
 * return 0, or negative errno code
 */
static int gm_tim_pin_set(struct device *dev, uint32_t pwm, uint32_t period_cycles, uint32_t pulse_cycles)
{
    int ret = 0;
    TIM_Device_T *TIMx = DEV_STRUCT(dev);
    struct tim_gm_data_t *data = DEV_DATA(dev);
    TIM_Handle_T *hTIM = data->tim_handle;

    TIM_OutputCompareInit_T sOCConfig;
    uint32_t channel;

    if (period_cycles == 0 || pulse_cycles > period_cycles)
    {
        return -EINVAL;
    }

    /* configure channel */
    channel = (pwm - 1) * CHANNEL_LENGTH;

    if (!IS_TIM_CC1_INSTANCE(DEV_STRUCT(dev)) ||
            !IS_TIM_CCX_INSTANCE(DEV_STRUCT(dev), channel))
    {
        return -ENOTSUP;
    }

    /* Configure Timer IP */
    hTIM->instance = DEV_STRUCT(dev);
    hTIM->init.prescaler = 0;
    hTIM->init.clockDivision = 0;
    hTIM->init.counterMode = TIM_COUNTERMODE_UP;
    hTIM->init.repetitionCounter = 0;

    /* Set period value */
    hTIM->init.period = period_cycles - 1;

    HAL_TIM_InitPWM(hTIM);

    /* Configure PWM channel */
    sOCConfig.ocMode       = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity   = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode   = TIM_OCFAST_DISABLE;
    sOCConfig.ocNPolarity  = TIM_OCNPOLARITY_HIGH;
    sOCConfig.ocNIdleState = TIM_OCNIDLESTATE_RESET;
    sOCConfig.ocIdleState  = TIM_OCIDLESTATE_RESET;

    /* Set the pulse value */
    sOCConfig.pulse = pulse_cycles;

    HAL_TIM_ConfigPWMChannel(hTIM, TIMx, &sOCConfig, channel);

    return HAL_TIM_StartPWM(hTIM, TIMx, channel);
}