/**
*
* Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
*
* @file        tim.h
*
* @author      Edward.Zhu
*
* @version     v1.0.0
*
* @date        2019/04/23
*
* @brief       TIM Driver APIs.
*
* @note
*              2019/04/23, Edward.Zhu, v1.0.0
*                  Initial version.
*
**/

#ifndef _TIM_H_
#define _TIM_H_

#include <errno.h>
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TIMx_COUNTERMODE_UP                 ((uint32_t)0x00000000U)
#define TIMx_COUNTERMODE_DOWN               ((uint32_t)0x00000010U)
#define TIMx_COUNTERMODE_CENTERALIGNED1     ((uint32_t)0x00000020U)
#define TIMx_COUNTERMODE_CENTERALIGNED2     ((uint32_t)0x00000040U)
#define TIMx_COUNTERMODE_CENTERALIGNED3     ((uint32_t)0x00000060U)

#define TIMx_OCPOLARITY_HIGH                ((uint32_t)0x00000000U)
#define TIMx_OCPOLARITY_LOW                 ((uint32_t)0x00000002U)

#define TIMx_OCFAST_DISABLE                 ((uint32_t)0x00000000U)
#define TIMx_OCFAST_ENABLE                  ((uint32_t)0x00000004U)

#define TIMx_CHANNEL_1                      ((uint32_t)0x00000000U)
#define TIMx_CHANNEL_2                      ((uint32_t)0x00000004U)
#define TIMx_CHANNEL_3                      ((uint32_t)0x00000008U)
#define TIMx_CHANNEL_4                      ((uint32_t)0x0000000CU)
#define TIMx_CHANNEL_ALL                    ((uint32_t)0x00000018U)

#define TIMx_OCMODE_TIMING                  ((uint32_t)0x00000000U)
#define TIMx_OCMODE_ACTIVE                  ((uint32_t)0x00000010U)
#define TIMx_OCMODE_INACTIVE                ((uint32_t)0x00000020U)
#define TIMx_OCMODE_TOGGLE                  ((uint32_t)0x00000030U)
#define TIMx_OCMODE_FORCED_INACTIVE         ((uint32_t)0x00000040U)
#define TIMx_OCMODE_FORCED_ACTIVE           ((uint32_t)0x00000050U)
#define TIMx_OCMODE_PWM1                    ((uint32_t)0x00000060U)
#define TIMx_OCMODE_PWM2                    ((uint32_t)0x00000070U)

#define TIMx_ICPOLARITY_RISING              ((uint32_t)0x00000000U)
#define TIMx_ICPOLARITY_FALLING             ((uint32_t)0x00000002U)
#define TIMx_ICPOLARITY_BOTHEDGE            ((uint32_t)0x0000000AU)

#define TIMx_ICSELECTION_DIRECTTI           ((uint32_t)0x00000001U)       /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to IC1, IC2, IC3 or IC4, respectively */
#define TIMx_ICSELECTION_INDIRECTTI         ((uint32_t)0x00000002U)       /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to IC2, IC1, IC4 or IC3, respectively */
#define TIMx_ICSELECTION_TRC                ((uint32_t)0x00000003U)         /*!< TIM Input 1, 2, 3 or 4 is selected to be connected to TRC */

#define TIMx_ICPSC_DIV1                     ((uint32_t)0x00000000U)       /*!< Capture performed each time an edge is detected on the capture input */
#define TIMx_ICPSC_DIV2                     ((uint32_t)0x00000004U)          /*!< Capture performed once every 2 events */
#define TIMx_ICPSC_DIV4                     ((uint32_t)0x00000008U)          /*!< Capture performed once every 4 events */
#define TIMx_ICPSC_DIV8                     ((uint32_t)0x0000000CU)            /*!< Capture performed once every 8 events */

#define DTx_DELAY_1                         ((uint8_t)0x7FU)
#define DTx_DELAY_2                         ((uint8_t)0x3FU)
#define DTx_DELAY_3                         ((uint8_t)0x1FU)
#define DTx_DELAY_4                         ((uint8_t)0x1FU)

typedef void (*tim_callback)(struct device *dev, u32_t event, u32_t param);

typedef struct
{
    uint32_t prescaler;
    uint32_t counterMode;
    uint32_t period;
    uint32_t clockDivision;
    uint32_t repetitionCounter;
    tim_callback callback;
} tim_timebaseinit;

typedef struct
{
    uint32_t ocMode;
    uint32_t pulse;
    uint32_t ocPolarity;
    uint32_t ocNPolarity;
    uint32_t ocFastMode;
    uint32_t ocIdleState;
    uint32_t ocNIdleState;
} tim_outputcompareinit;

typedef struct
{
    uint32_t ocMode;
    uint32_t pulse;
    uint32_t ocPolarity;
    uint32_t ocNPolarity;
    uint32_t ocIdleState;
    uint32_t ocNIdleState;
    uint32_t icPolarity;
    uint32_t icSelection;
    uint32_t icFilter;
    tim_callback callback;
} tim_opminit;

typedef struct
{
    uint32_t icPolarity;
    uint32_t icSelection;
    uint32_t icPrescaler;
    uint32_t icFilter;
} tim_inputcaptureinit;

typedef struct
{
    uint32_t encoderMode;
    uint32_t ic1Polarity;
    uint32_t ic1Selection;
    uint32_t ic1Prescaler;
    uint32_t ic1Filter;
    uint32_t ic2Polarity;
    uint32_t ic2Selection;
    uint32_t ic2Prescaler;
    uint32_t ic2Filter;
} tim_encodeinit;

typedef struct
{
    uint32_t clockSource;
    uint32_t clockPolarity;
    uint32_t clockPrescaler;
    uint32_t clockFilter;
} tim_clkconfig;

typedef struct
{
    uint32_t clearInputState;
    uint32_t clearInputSource;
    uint32_t clearInputPolarity;
    uint32_t clearInputPrescaler;
    uint32_t clearInputFilter;
} tim_clearinputconfig;

typedef struct
{
    uint32_t  slaveMode;
    uint32_t  inputTrigger;
    uint32_t  triggerPolarity;
    uint32_t  triggerPrescaler;
    uint32_t  triggerFilter;
} tim_slvconfig;

typedef struct
{
    uint32_t ic1Polarity;
    uint32_t ic1Prescaler;
    uint32_t ic1Filter;
    uint32_t commutation_Delay;
} tim_hallsensorinit;

typedef struct
{
    uint32_t  masterOutputTrigger;
    uint32_t  masterSlaveMode;
} tim_mstconfig;

typedef struct
{
    uint32_t lockLevel;
    uint32_t deadTime;
    uint32_t breakState;
    uint32_t breakPolarity;
    uint32_t automaticOutput;
} tim_bdtconfig;

typedef struct
{
    uint32_t dmaBurstLength;
    uint32_t dmaBaseAddress;
} tim_dmaconfig;

typedef enum
{
    HAL_TIMx_ACTIVE_CHANNEL_CLEARED = 0x0,  /*!< All active channels cleared */
    HAL_TIMx_ACTIVE_CHANNEL_1 = 0x1,        /*!< The active channel is 1 */
    HAL_TIMx_ACTIVE_CHANNEL_2 = 0x2,        /*!< The active channel is 2 */
    HAL_TIMx_ACTIVE_CHANNEL_3 = 0x4,        /*!< The active channel is 3 */
    HAL_TIMx_ACTIVE_CHANNEL_4 = 0x8,        /*!< The active channel is 4 */
} tim_activechannel;

typedef enum
{
    HAL_TIMx_UNLOCKED = 0x00U,
    HAL_TIMx_LOCKED   = 0x01U
} tim_lock;

typedef int (*tim_api_timebase_init)(struct device *dev);
typedef int (*tim_api_outputcompare_init)(struct device *dev);
typedef int (*tim_api_pwm_init)(struct device *dev, tim_timebaseinit *init);
typedef int (*tim_api_inputcapture_init)(struct device *dev, tim_timebaseinit *init);
typedef int (*tim_api_onepulse_init)(struct device *dev, uint32_t opm);
typedef int (*tim_api_encoder_init)(struct device *dev,  tim_encodeinit *cfg);
typedef int (*tim_api_hallsensor_init)(struct device *dev,  tim_hallsensorinit *cfg);
typedef int (*tim_api_timebase_start)(struct device *dev);
typedef int (*tim_api_outputcompare_start)(struct device *dev, uint32_t ch);
typedef int (*tim_api_pwm_start)(struct device *dev, uint32_t ch);
typedef int (*tim_api_readcapturevalue_start)(struct device *dev, uint32_t ch);
typedef int (*tim_api_inputcapture_start)(struct device *dev, uint32_t ch);
typedef int (*tim_api_onepulse_start)(struct device *dev, uint32_t ch);
typedef int (*tim_api_encoder_start)(struct device *dev, uint32_t ch);
typedef int (*tim_api_hallsensor_start)(struct device *dev);
typedef int (*tim_api_occhannel_cfg)(struct device *dev, tim_outputcompareinit *cfg, uint32_t ch);
typedef int (*tim_api_icchannel_cfg)(struct device *dev, tim_inputcaptureinit *cfg, uint32_t ch);
typedef int (*tim_api_pwmchannel_cfg)(struct device *dev, tim_outputcompareinit *cfg, uint32_t ch);
typedef int (*tim_api_opmchannel_cfg)(struct device *dev, tim_opminit *cfg, uint32_t ch1, uint32_t ch2);
typedef int (*tim_api_mastersyn_config)(struct device *dev, tim_mstconfig *cfg);
typedef int (*tim_api_slavesyn_config)(struct device *dev, tim_slvconfig *cfg);
typedef int (*tim_api_bdt_config)(struct device *dev, tim_bdtconfig *cfg);
typedef int (*tim_api_pin_set)(struct device *dev, uint32_t pwm, uint32_t period_cycles, uint32_t pulse_cycles);
typedef int (*tim_api_get_cycles_per_sec)(struct device *dev, uint32_t pwm, uint64_t *cycles);

struct tim_driver_api
{
    tim_api_timebase_init timebase_init;
    tim_api_outputcompare_init outputcompare_init;
    tim_api_pwm_init pwm_init;
    tim_api_inputcapture_init inputcapture_init;
    tim_api_onepulse_init onepulse_init;
    tim_api_encoder_init encoder_init;
    tim_api_hallsensor_init hallsensor_init;
    tim_api_timebase_start timebase_start;
    tim_api_outputcompare_start outputcompare_start;
    tim_api_pwm_start pwm_start;
    tim_api_readcapturevalue_start readcapturevalue_start;
    tim_api_inputcapture_start inputcapture_start;
    tim_api_onepulse_start onepulse_start;
    tim_api_encoder_start encoder_start;
    tim_api_hallsensor_start hallsensor_start;
    tim_api_occhannel_cfg occhannel_cfg;
    tim_api_icchannel_cfg icchannel_cfg;
    tim_api_pwmchannel_cfg pwmchannel_cfg;
    tim_api_opmchannel_cfg opmchannel_cfg;
    tim_api_mastersyn_config mastersyn_config;
    tim_api_slavesyn_config slavesyn_config;
    tim_api_bdt_config bdt_config;
    tim_api_pin_set pin_set;
};

__syscall int tim_timebase_init(struct device *dev);

static inline int _impl_tim_timebase_init(struct device *dev)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->timebase_init(dev);
}

__syscall int tim_outputcompare_init(struct device *dev);

static inline int _impl_tim_outputcompare_init(struct device *dev)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->outputcompare_init(dev);
}

__syscall int tim_pwm_init(struct device *dev, tim_timebaseinit *init);

static inline int _impl_tim_pwm_init(struct device *dev, tim_timebaseinit *init)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->pwm_init(dev, init);
}

__syscall int tim_inputcapture_init(struct device *dev, tim_timebaseinit *init);

static inline int _impl_tim_inputcapture_init(struct device *dev, tim_timebaseinit *init)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->inputcapture_init(dev, init);
}

__syscall int tim_onepulse_init(struct device *dev, uint32_t opm);

static inline int _impl_tim_onepulse_init(struct device *dev, uint32_t opm)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->onepulse_init(dev, opm);
}

__syscall int tim_encoder_init(struct device *dev, tim_encodeinit *cfg);

static inline int _impl_tim_encoder_init(struct device *dev, tim_encodeinit *cfg)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->encoder_init(dev, cfg);
}

__syscall int tim_hallsensor_init(struct device *dev, tim_hallsensorinit *cfg);

static inline int _impl_tim_hallsensor_init(struct device *dev, tim_hallsensorinit *cfg)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->hallsensor_init(dev, cfg);
}

__syscall int tim_timebase_start(struct device *dev);

static inline int _impl_tim_timebase_start(struct device *dev)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->timebase_start(dev);
}

__syscall int tim_outputcompare_start(struct device *dev, uint32_t ch);

static inline int _impl_tim_outputcompare_start(struct device *dev, uint32_t ch)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->outputcompare_start(dev, ch);
}

__syscall int tim_pwm_start(struct device *dev, uint32_t ch);

static inline int _impl_tim_pwm_start(struct device *dev, uint32_t ch)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->pwm_start(dev, ch);
}

__syscall int tim_readcapturevalue_start(struct device *dev, uint32_t ch);

static inline int _impl_tim_readcapturevalue_start(struct device *dev, uint32_t ch)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->readcapturevalue_start(dev, ch);
}

__syscall int tim_inputcapture_start(struct device *dev, uint32_t ch);

static inline int _impl_tim_inputcapture_start(struct device *dev, uint32_t ch)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->inputcapture_start(dev, ch);
}

__syscall int tim_onepulse_start(struct device *dev, uint32_t ch);

static inline int _impl_tim_onepulse_start(struct device *dev, uint32_t ch)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->onepulse_start(dev, ch);
}

__syscall int tim_encoder_start(struct device *dev, uint32_t ch);

static inline int _impl_tim_encoder_start(struct device *dev, uint32_t ch)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->encoder_start(dev, ch);
}

__syscall int tim_hallsensor_start(struct device *dev);

static inline int _impl_tim_hallsensor_start(struct device *dev)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->hallsensor_start(dev);
}

__syscall int tim_occhannel_cfg(struct device *dev, tim_outputcompareinit *cfg, uint32_t ch);

static inline int _impl_tim_occhannel_cfg(struct device *dev, tim_outputcompareinit *cfg, uint32_t ch)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->occhannel_cfg(dev, cfg, ch);
}

__syscall int tim_icchannel_cfg(struct device *dev, tim_inputcaptureinit *cfg, uint32_t ch);

static inline int _impl_tim_icchannel_cfg(struct device *dev, tim_inputcaptureinit *cfg, uint32_t ch)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->icchannel_cfg(dev, cfg, ch);
}

__syscall int tim_pwmchannel_cfg(struct device *dev, tim_outputcompareinit *cfg, uint32_t ch);

static inline int _impl_tim_pwmchannel_cfg(struct device *dev, tim_outputcompareinit *cfg, uint32_t ch)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->pwmchannel_cfg(dev, cfg, ch);
}

__syscall int tim_opmchannel_cfg(struct device *dev, tim_opminit *cfg, uint32_t ch1, uint32_t ch2);

static inline int _impl_tim_opmchannel_cfg(struct device *dev, tim_opminit *cfg, uint32_t ch1, uint32_t ch2)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->opmchannel_cfg(dev, cfg, ch1, ch2);
}

__syscall int tim_mastersyn_config(struct device *dev, tim_mstconfig *cfg);

static inline int _impl_tim_mastersyn_cfg(struct device *dev, tim_mstconfig *cfg)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->mastersyn_config(dev, cfg);
}

__syscall int tim_slavesyn_config(struct device *dev, tim_slvconfig *cfg);

static inline int _impl_tim_slavesyn_config(struct device *dev, tim_slvconfig *cfg)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->slavesyn_config(dev, cfg);
}

__syscall int tim_bdt_config(struct device *dev, tim_bdtconfig *cfg);

static inline int _impl_tim_bdt_config(struct device *dev, tim_bdtconfig *cfg)
{
    struct tim_driver_api *api ;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->bdt_config(dev, cfg);
}


__syscall int tim_pin_set_cycles(struct device *dev, u32_t pwm, u32_t period, u32_t pulse);

static inline int _impl_tim_pin_set_cycles(struct device *dev, uint32_t pwm, uint32_t period, uint32_t pulse)
{
    struct tim_driver_api *api;

    api = (struct tim_driver_api *)dev->driver_api;

    return api->pin_set(dev, pwm, period, pulse);
}

__syscall int tim_get_cycles_per_sec(struct device *dev, u32_t pwm, u64_t *cycles);


#ifdef __cplusplus
}
#endif

#include <syscalls/tim.h>
#endif /* _TIM_H_ */

