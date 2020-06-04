/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_hal_gpio.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       GPIO source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <gm_common.h>
#include <gm_soc.h>
#include <gm_hal_gpio.h>
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>

#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF
#endif

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


/*******************************************************************************
* Constants and macros
*******************************************************************************/
#define EXTI_MODE             ((uint32_t)0x10000000)
#define PINMUX0_MASK          ((uint32_t)0x10000000)
#define PINMUX1_MASK          ((uint32_t)0x20000000)
#define PINMUX2_MASK          ((uint32_t)0x30000000)
#define GPIO_NUMBER           ((uint32_t)16)

/*******************************************************************************
* Global variables
*******************************************************************************/

/*******************************************************************************
* Static variables
*******************************************************************************/
/** @brief Pointers to GPIO bases for each instance. */
static GPIO_Device_T *const s_GPIOBases[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI, GPIOJ, GPIOK};
const IRQn_Type s_GPIOIrqs[] = {GPIOA_IRQn, GPIOB_IRQn, GPIOC_IRQn, GPIOD_IRQn, GPIOE_IRQn, GPIOF_IRQn, GPIOG_IRQn, GPIOH_IRQn, \
                                GPIOI_IRQn, GPIOJ_IRQn, GPIOK_IRQn
                               };

/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/
/**
 * @brief Get instance number for gpio module.
 *
 * @param pGPIO GPIO base address.
 */
static uint32_t HAL_GPIO_GetInstance(GPIO_Device_T *pGPIO);

/*******************************************************************************
* Codes
*******************************************************************************/
static uint32_t HAL_GPIO_GetInstance(GPIO_Device_T *pGPIO)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < MAX_GPIO_NUM; instance++)
    {
        if (s_GPIOBases[instance] == pGPIO)
        {
            break;
        }
    }

    return instance;
}

void HAL_GPIOA_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOA, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);

    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOA, extpin);
    }
}

void HAL_GPIOB_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOB, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);

    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOB, extpin);
    }
}

void HAL_GPIOC_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOC, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);

    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOC, extpin);
    }
}

void HAL_GPIOD_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOD, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);
    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOD, extpin);
    }
}

void HAL_GPIOE_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOE, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);
    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOE, extpin);
    }
}

void HAL_GPIOF_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOF, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);
    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOF, extpin);
    }
}

void HAL_GPIOG_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOG, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);

    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOG, extpin);
    }
}

void HAL_GPIOH_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOH, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);
    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOH, extpin);
    }
}

void HAL_GPIOI_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOI, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);
    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOI, extpin);
    }
}

void HAL_GPIOJ_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOJ, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);
    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOJ, extpin);
    }
}

void HAL_GPIOK_InterruptHandler(void)
{
    unsigned int extpin = 0xFFFF;
    unsigned int status = GPIO_EXTI_GET_IT(GPIOK, extpin);

    PR_DEBUG("Fun[%s],status=0x%x\n", __FUNCTION__, status);
    if (status)
    {
        GPIO_EXTI_CLEAR_IT(GPIOK, extpin);
    }
}


/**
  * @brief  The following  function are used to reset GPIO peripheral clock.
  * @note  software has to reset peripheral  before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_GPIO_SW_Reset(void)
{
    HAL_Reset_Module(RESET_GPIO_SW_RSTJ);
}

/**
  * @brief  The following  functions are used to enable GPIO peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application
  *         software has to enable clock before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_GPIO_CLK_ENABLE(void)
{
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_GPIO);
}

/**
  * @brief  The following  functions are used to disable GPIO peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application
  *         software has to enable clock before using peripheral
  * @param void
  *
  * @retval None
  *
  */
void HAL_GPIO_CLK_DISABLE(void)
{
    HAL_CLOCK_PeripheralClockDisable0(SYS_ENCLK0_GPIO);
}

void HAL_GPIO_RequestIrq(int gpio_type)
{
    if (gpio_type == 0)
    {
        HAL_RequestIrq(GPIOA_IRQn, GPIOA_IRQ_PRIORITY, HAL_GPIOA_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOA_IRQn);
    }
    else if (gpio_type == 1)
    {
        HAL_RequestIrq(GPIOB_IRQn, GPIOB_IRQ_PRIORITY, HAL_GPIOB_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOB_IRQn);
    }
    else if (gpio_type == 2)
    {
        HAL_RequestIrq(GPIOC_IRQn, GPIOC_IRQ_PRIORITY, HAL_GPIOC_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOC_IRQn);
    }
    else if (gpio_type == 3)
    {
        HAL_RequestIrq(GPIOD_IRQn, GPIOD_IRQ_PRIORITY, HAL_GPIOD_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOD_IRQn);
    }
    else if (gpio_type == 4)
    {
        HAL_RequestIrq(GPIOE_IRQn, GPIOE_IRQ_PRIORITY, HAL_GPIOE_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOE_IRQn);
    }
    else if (gpio_type == 5)
    {
        HAL_RequestIrq(GPIOF_IRQn, GPIOF_IRQ_PRIORITY, HAL_GPIOF_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOF_IRQn);
    }
    else if (gpio_type == 6)
    {
        HAL_RequestIrq(GPIOG_IRQn, GPIOI_IRQ_PRIORITY, HAL_GPIOG_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOG_IRQn);
    }
    else if (gpio_type == 7)
    {
        HAL_RequestIrq(GPIOH_IRQn, GPIOI_IRQ_PRIORITY, HAL_GPIOH_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOH_IRQn);
    }
    else if (gpio_type == 8)
    {
        HAL_RequestIrq(GPIOI_IRQn, GPIOI_IRQ_PRIORITY, HAL_GPIOI_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOI_IRQn);
    }
    else if (gpio_type == 9)
    {
        HAL_RequestIrq(GPIOJ_IRQn, GPIOI_IRQ_PRIORITY, HAL_GPIOJ_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOJ_IRQn);
    }
    else if (gpio_type == 10)
    {
        HAL_RequestIrq(GPIOK_IRQn, GPIOI_IRQ_PRIORITY, HAL_GPIOK_InterruptHandler, 0);
        HAL_IRQ_ENABLE(GPIOK_IRQn);
    }
}

/**
  * @brief  Initializes the GPIO according to the specified
  *         parameters in the GPIO_PinConfig_T.
  * @param  pGPIO: pointer to a GPIO_Device_T structure.
  * @param config: pointer to a GPIO_PinConfig_T structure that contains
  *         the configuration information for GPIO module
  * @retval HAL status
  */
RET_CODE HAL_GPIO_Init(GPIO_Device_T *pGPIO, GPIO_PinConfig_T *pConfig)
{
    uint32_t position;
    uint32_t instance = 0;
    uint32_t ioPosition = 0x00;
    uint32_t ioCurrent = 0x00;
    uint32_t temp = 0x00;
    int shift = 0;
    int pull_index = 0;


    instance = HAL_GPIO_GetInstance(pGPIO);

    if ((instance >= MAX_GPIO_NUM) || (!IS_GPIO_PIN(pConfig->pin)))
    {
        return -1;;
    }

    HAL_GPIO_CLK_ENABLE();
    HAL_GPIO_SW_Reset();

    if (pConfig->mode == GPIO_MODE_RESTORE_DEFAULT)
    {
        HAL_GPIO_DeInit(pGPIO, pConfig);
        return 0;
    }

    /* Configure the port pins */
    for (position = 0; position < GPIO_NUMBER; position++)
    {
        /* Get the IO position */
        ioPosition = ((uint32_t)0x01) << position;
        /* Get the current IO position */
        ioCurrent = (uint32_t)(pConfig->pin) & ioPosition;

        if (ioCurrent == ioPosition)
        {
            /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
            if ((pConfig->mode == GPIO_MODE_INPUT) || (pConfig->mode == GPIO_MODE_IT_LOW) || \
                    (pConfig->mode == GPIO_MODE_IT_HIGH) || (pConfig->mode == GPIO_MODE_IT_FALLING) || \
                    (pConfig->mode == GPIO_MODE_IT_RISING))
            {
                pGPIO->IMCR = ioCurrent;
                IO_BITMASK_SET(SYSIO->IO_MODE_REG[instance], (0x03 << (position << 1)), 0x0);
            }
            else if (pConfig->mode == GPIO_MODE_OUTPUT)
            {
                pGPIO->OMCR = ioCurrent;
                IO_BITMASK_SET(SYSIO->IO_MODE_REG[instance], (0x03 << (position << 1)), 0x0);

                if (pConfig->driving > 3)
                {
                    pConfig->driving = GPIO_DRIVING_LEVEL0;
                }
                IO_BITMASK_SET(SYSIO->IO_DS_REG[instance], (GPIO_DRIVING_MASK << (position << 1)), ((pConfig->driving & GPIO_DRIVING_MASK) << (position << 1)));
            }
            else if (pConfig->mode == GPIO_MODE_ANALOG)
            {
                IO_BITMASK_SET(SYSIO->IO_MODE_REG[instance], (0x03 << (position << 1)), (0x3 << (position << 1)));

            }
#if defined(CHIP_F6721B)
            else if (pConfig->mode == GPIO_MODE_ANALOG_2)
            {
                IO_BITMASK_SET(SYSIO->IO_MODE_REG[instance], (0x03 << (position << 1)), (0x2 << (position << 1)));

            }
#endif
            /* In case of Alternate function mode selection */
            if (pConfig->mode == GPIO_MODE_AF)
            {
                IO_BITMASK_SET(SYSIO->IO_MODE_REG[instance], (0x03 << (position << 1)), (0x1 << (position << 1)));
                temp = (instance << 1) + (position >> 3);

                IO_BITMASK_SET(SYSIO->IO_AF_REG[temp], (0xF << ((position & 7) << 2)), ((pConfig->alternate) << ((position & 7) << 2)));
            }

            shift = (instance % 2) * 16;
            /* Activate the Pull-up or Pull-down  resistor for the current IO */
            if ((pConfig->mode == GPIO_MODE_INPUT) || (pConfig->mode == GPIO_MODE_IT_LOW) || \
                    (pConfig->mode == GPIO_MODE_IT_HIGH) || (pConfig->mode == GPIO_MODE_IT_FALLING) || \
                    (pConfig->mode == GPIO_MODE_IT_RISING) || (pConfig->mode == GPIO_MODE_AF))
            {

                pull_index = instance >> 1;

                if (pConfig->pull == GPIO_PULLUP)
                {
                    IO_BITMASK_SET(SYSIO->IO_PU_REG[pull_index], (GPIO_PULL_EABLE << (position + shift)), (GPIO_PULL_EABLE << (position + shift)));
                }
                else if (pConfig->pull == GPIO_PULLDOWN)
                {
                    IO_BITMASK_SET(SYSIO->IO_PD_REG[pull_index], (GPIO_PULL_EABLE << (position + shift)), (GPIO_PULL_EABLE << (position + shift)));
                }
                else
                {
                    IO_BITMASK_SET(SYSIO->IO_PU_REG[pull_index], (GPIO_PULL_EABLE << (position + shift)), (GPIO_PULL_DISABLE << (position + shift)));
                    IO_BITMASK_SET(SYSIO->IO_PD_REG[pull_index], (GPIO_PULL_EABLE << (position + shift)), (GPIO_PULL_DISABLE << (position + shift)));
                }
            }
            /* Configure the External Interrupt for the current IO */
            if ((pConfig->mode & EXTI_MODE) == EXTI_MODE)
            {
                /* Set type of Interrupt Trigger */
                if ((pConfig->mode == GPIO_MODE_IT_LOW) || (pConfig->mode == GPIO_MODE_IT_HIGH))
                {
                    pGPIO->ITCR = ioCurrent;
                }
                else if ((pConfig->mode == GPIO_MODE_IT_FALLING) || (pConfig->mode == GPIO_MODE_IT_RISING))
                {
                    pGPIO->ITSR = ioCurrent;
                }

                /* Set polarity of Interrupt Trigger */
                if ((pConfig->mode == GPIO_MODE_IT_LOW) || (pConfig->mode == GPIO_MODE_IT_FALLING))
                {
                    pGPIO->IPCR = ioCurrent;
                }
                else if ((pConfig->mode == GPIO_MODE_IT_HIGH) || (pConfig->mode == GPIO_MODE_IT_RISING))
                {
                    pGPIO->IPSR = ioCurrent;
                }
                /* Enable Interrupt */
                pGPIO->IER = ioCurrent;
                HAL_GPIO_RequestIrq(instance);

            }
        }
    }

    return 0;
}

/**
  * @brief  DeInitializes the GPIO according to the specified
  *         parameters in the GPIO_PinConfig_T.
  * @param  pGPIO: pointer to a GPIO_Device_T structure.
  * @param config: pointer to a GPIO_PinConfig_T structure that contains
  *         the configuration information for GPIO module
  * @retval HAL status
  */
RET_CODE HAL_GPIO_DeInit(GPIO_Device_T *pGPIO, GPIO_PinConfig_T *pConfig)
{
    uint32_t position;
    uint32_t instance = 0;
    uint32_t ioPosition = 0x00;
    uint32_t ioCurrent = 0x00;
    uint32_t temp = 0x00;
    int shift = 0;
    int pull_index = 0;

    instance = HAL_GPIO_GetInstance(pGPIO);

    if ((instance >= MAX_GPIO_NUM) || (!IS_GPIO_PIN(pConfig->pin)))
    {
        return -1;;
    }

    /* Configure the port pins */
    for (position = 0; position < GPIO_NUMBER; position++)
    {
        /* Get the IO position */
        ioPosition = ((uint32_t)0x01) << position;
        /* Get the current IO position */
        ioCurrent = (uint32_t)(pConfig->pin) & ioPosition;

        if (ioCurrent == ioPosition)
        {
            /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
            pGPIO->IMCR = ioCurrent;

            /*driving*/
            IO_BITMASK_SET(SYSIO->IO_DS_REG[instance], (GPIO_DRIVING_MASK << (position << 1)), ((GPIO_DRIVING_LEVEL0 & GPIO_DRIVING_MASK) << (position << 1)));
            IO_BITMASK_SET(SYSIO->IO_MODE_REG[instance], (0x03 << (position << 1)), (0 << (position << 1)));
            temp = (instance << 1) + (position >> 3);
            IO_BITMASK_SET(SYSIO->IO_AF_REG[temp], (0xF << ((position & 7) << 2)), (0 << ((position & 7) << 2)));

            shift = (instance % 2) * 16;
            pull_index = instance >> 1;
            IO_BITMASK_SET(SYSIO->IO_PU_REG[pull_index], (GPIO_PULL_EABLE << (position + shift)), (GPIO_PULL_DISABLE << (position + shift)));
            IO_BITMASK_SET(SYSIO->IO_PD_REG[pull_index], (GPIO_PULL_EABLE << (position + shift)), (GPIO_PULL_DISABLE << (position + shift)));

            /* Disable the External Interrupt for the current IO */
            pGPIO->IDR = ioCurrent;
            pGPIO->ITCR = ioCurrent;
            pGPIO->IPCR = ioCurrent;
        }
    }

    return 0;
}

void HAL_GPIO_PinWrite(GPIO_Pin_T pin, GPIO_PinState_T state)
{
    if (state == GPIO_PIN_RESET)
    {
        PIN_CLEAR(pin);
    }
    else
    {
        PIN_SET(pin);
    }
}

GPIO_PinState_T HAL_GPIO_PinRead(GPIO_Pin_T pin)
{
    GPIO_PinState_T state = GPIO_PIN_RESET;

    if (PIN_GET(pin))
    {
        state = GPIO_PIN_SET;
    }

    return state;
}

void HAL_GPIO_PinToggle(GPIO_Pin_T pin)
{
    if (PIN_GET(pin))
    {
        PIN_CLEAR(pin);
    }
    else
    {
        PIN_SET(pin);
    }
}

void HAL_GPIO_SetPinDirection(GPIO_Pin_T pin, GPIO_PinDir_T direction)
{
    GPIO_Device_T *pGPIO = (GPIO_Device_T *)((((uint32_t)(pin)) >> 12) << 12);
    uint32_t bitmask = 0;

    bitmask = (((((uint32_t)(pin)) << 22) >> 24) << (((((uint32_t)(pin)) >> 11) & 0x1) ? 8 : 0));

    /* Configure IO Direction mode (Input, Output) */
    if (direction == GPIO_PIN_INPUT)
    {
        pGPIO->IMCR = bitmask;
    }
    else if (direction == GPIO_PIN_OUTPUT)
    {
        pGPIO->OMCR = bitmask;
    }
}

RET_CODE HAL_GPIO_PesConfig(GPIO_Pin_T pin, GPIO_PESConfig_T *gpioPesCfg)
{
    GPIO_Device_T *pGPIO = (GPIO_Device_T *)((((uint32_t)(pin)) >> 12) << 12);
    if (gpioPesCfg == NULL)
    {
        return -1;
    }
    if (gpioPesCfg->pin_sel > 15 || gpioPesCfg->channel_sel > 12 || gpioPesCfg->action_sel > 3 || gpioPesCfg->event_action_en > 1)
    {
        return -1;
    }

    IO_BITMASK_SET(pGPIO->EAC, ((unsigned int)0xF << 28), ((unsigned int)(gpioPesCfg->pin_sel) << 28));
    IO_BITMASK_SET(pGPIO->EAC, (0xF << 16), ((gpioPesCfg->channel_sel) << 16));
    IO_BITMASK_SET(pGPIO->EAC, (0x3 << 4), ((gpioPesCfg->action_sel) << 4));
    IO_BITMASK_SET(pGPIO->EAC, (1 << 0), ((gpioPesCfg->event_action_en) << 0));

    return 0;
}

RET_CODE HAL_GPIO_SetPinFilter(GPIO_Pin_T pin, GPIO_InputGlitchFilter_T *pFilter)
{
    GPIO_Device_T *pGPIO = (GPIO_Device_T *)((((uint32_t)(pin)) >> 12) << 12);
    uint32_t bitmask = 0;
    uint32_t instance = 0;
    uint32_t position = 0;
    uint32_t temp = 0x00;
    uint16_t ioCurrent = 0;

    instance = HAL_GPIO_GetInstance(pGPIO);

    if (instance >= MAX_GPIO_NUM)
    {
        return -1;
    }

    bitmask = (((((uint32_t)(pin)) << 22) >> 24) << (((((uint32_t)(pin)) >> 11) & 0x1) ? 8 : 0));

    ioCurrent = (uint16_t)bitmask & 0xffff;

    switch (ioCurrent)
    {
        case GPIO_PIN_0:
            position = 0;
            break;
        case GPIO_PIN_1:
            position = 1;
            break;
        case GPIO_PIN_2:
            position = 2;
            break;
        case GPIO_PIN_3:
            position = 3;
            break;
        case GPIO_PIN_4:
            position = 4;
            break;
        case GPIO_PIN_5:
            position = 5;
            break;
        case GPIO_PIN_6:
            position = 6;
            break;
        case GPIO_PIN_7:
            position = 7;
            break;
        case GPIO_PIN_8:
            position = 8;
            break;
        case GPIO_PIN_9:
            position = 9;
            break;
        case GPIO_PIN_10:
            position = 10;
            break;
        case GPIO_PIN_11:
            position = 11;
            break;
        case GPIO_PIN_12:
            position = 12;
            break;
        case GPIO_PIN_13:
            position = 13;
            break;
        case GPIO_PIN_14:
            position = 14;
            break;
        case GPIO_PIN_15:
            position = 15;
            break;
        default:
            break;
    }

    position = position;
    if (pFilter->db_bypass_en)
    {
        if (instance == 0 || instance == 1)
        {
            temp = (instance == 0) ? bitmask : (bitmask << 16);
            IO_BIT_SET(SYSIO->IO_BPS_REG[0], temp);
        }
        else if (instance == 2 || instance == 3)
        {
            temp = (instance == 2) ? bitmask : (bitmask << 16);
            IO_BIT_SET(SYSIO->IO_BPS_REG[1], temp);
        }
        else if (instance == 4 || instance == 5)
        {
            temp = (instance == 4) ? bitmask : (bitmask << 16);
            IO_BIT_SET(SYSIO->IO_BPS_REG[2], temp);
        }
        else if (instance == 6 || instance == 7)
        {
            temp = (instance == 6) ? bitmask : (bitmask << 16);
            IO_BIT_SET(SYSIO->IO_BPS_REG[3], temp);
        }
        else if (instance == 8 || instance == 9)
        {
            temp = (instance == 8) ? bitmask : (bitmask << 16);
            IO_BIT_SET(SYSIO->IO_BPS_REG[4], temp);
        }
        else if (instance == 10)
        {
            temp = (instance == 10) ? bitmask : (bitmask << 16);
            IO_BIT_SET(SYSIO->IO_BPS_REG[5], temp);
        }

    }
    else
    {
        if (instance == 0 || instance == 1)
        {
            temp = (instance == 0) ? bitmask : (bitmask << 16);
            IO_BIT_CLEAR(SYSIO->IO_BPS_REG[0], temp);
        }
        else if (instance == 2 || instance == 3)
        {
            temp = (instance == 2) ? bitmask : (bitmask << 16);
            IO_BIT_CLEAR(SYSIO->IO_BPS_REG[1], temp);
        }
        else if (instance == 4 || instance == 5)
        {
            temp = (instance == 4) ? bitmask : (bitmask << 16);
            IO_BIT_CLEAR(SYSIO->IO_BPS_REG[2], temp);
        }
        else if (instance == 6 || instance == 7)
        {
            temp = (instance == 6) ? bitmask : (bitmask << 16);
            IO_BIT_CLEAR(SYSIO->IO_BPS_REG[3], temp);
        }
        else if (instance == 8 || instance == 9)
        {
            temp = (instance == 8) ? bitmask : (bitmask << 16);
            IO_BIT_CLEAR(SYSIO->IO_BPS_REG[4], temp);
        }
        else if (instance == 10)
        {
            temp = (instance == 10) ? bitmask : (bitmask << 16);
            IO_BIT_CLEAR(SYSIO->IO_BPS_REG[5], temp);
        }

        IO_BITMASK_SET(SYSDBCLKREG->DB_CLK_SRC_CTRL, (0x3 << (instance << 1)), (pFilter->child_clk_sel & 0x3) << (instance << 1));

        if (pFilter->parent_clk_src_sel != DB_PARENT_SRC_CLK_SEL_FCLK)
        {
            pFilter->parent_clk_src_sel = DB_PARENT_SRC_CLK_SEL_LIRC;
        }
        IO_BITMASK_SET(SYSDBCLKREG->DB_CLK_DIV_CTRL, SYS_FLTCPS_PARENT_CLK_SRC_MASK, SYS_FLTCPS_CLK_SRC_FIELD(pFilter->parent_clk_src_sel));

#if (defined(CHIP_F6721B))
        IO_BIT_SET(SYSDBCLKREG->DB_CLK_DIV_CTRL, 1 << 16);
#endif

        switch (pFilter->child_clk_sel)
        {
            case PAD_FLT_CLK1:
                IO_BITMASK_SET(SYSDBCLKREG->DB_CLK_DIV_CTRL, SYS_FLTCPS_CLK1_MASK | SYS_FLTCPS_CLK2_MASK | SYS_FLTCPS_CLK3_MASK, SYS_FLTCPS_CLK1_FIELD(pFilter->prescaler));
                break;
            case PAD_FLT_CLK2:
                IO_BITMASK_SET(SYSDBCLKREG->DB_CLK_DIV_CTRL, SYS_FLTCPS_CLK1_MASK | SYS_FLTCPS_CLK2_MASK | SYS_FLTCPS_CLK3_MASK, SYS_FLTCPS_CLK2_FIELD(pFilter->prescaler));
                break;
            case PAD_FLT_CLK3:
                IO_BITMASK_SET(SYSDBCLKREG->DB_CLK_DIV_CTRL, SYS_FLTCPS_CLK1_MASK | SYS_FLTCPS_CLK2_MASK | SYS_FLTCPS_CLK3_MASK, SYS_FLTCPS_CLK3_FIELD(pFilter->prescaler));
                break;
            case PAD_IRC_LS_CLK:
                break;

            default:
                break;
        }
    }

    return 0;
}
