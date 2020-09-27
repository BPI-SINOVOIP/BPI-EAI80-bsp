/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gpio.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/07/19
 *
 * @brief        GPIO APIs for application.
 *
 * @note
 *    2016/07/19, mark.lee, V1.0.0
 *        Initial version.
 */

#include <driver/gpio.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
#define EXTI_MODE             ((uint32_t)0x10000000)

#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
#define PINMUX0_MASK          ((uint32_t)0x10000000)
#define PINMUX1_MASK          ((uint32_t)0x20000000)
#define PINMUX2_MASK          ((uint32_t)0x40000000)
#endif

#define GPIO_NUMBER           ((uint32_t)16)

/*******************************************************************************
* Global variables
*******************************************************************************/


/*******************************************************************************
* Static variables
*******************************************************************************/
#if (defined(CHIP_GM6601) || defined(CHIP_GM6621) || defined(CHIP_GM6601B))
/** @brief Pointers to GPIO bases for each instance. */
static GPIO_Device_T *const s_GPIOBases[] = {GPIOA, GPIOB, GPIOC, GPIOD};

/** @brief Pointers to GPIO IRQ number for each instance. */
const IRQn_Type s_GPIOIrqs[] = {GPIOA_IRQn, GPIOB_IRQn, GPIOC_IRQn, GPIOD_IRQn};

#elif defined(CHIP_GM6611)
/** @brief Pointers to GPIO bases for each instance. */
static GPIO_Device_T *const s_GPIOBases[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};

/** @brief Pointers to GPIO IRQ number for each instance. */
const IRQn_Type s_GPIOIrqs[] = {GPIOA_IRQn, GPIOB_IRQn, GPIOC_IRQn, GPIOD_IRQn, GPIOE_IRQn};

#elif defined(CHIP_GM6611B)
/** @brief Pointers to GPIO bases for each instance. */
static GPIO_Device_T *const s_GPIOBases[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF};

/** @brief Pointers to GPIO IRQ number for each instance. */
const IRQn_Type s_GPIOIrqs[] = {GPIO0_IRQn, GPIO1_IRQn, GPIO2_IRQn, GPIO3_IRQn, GPIO4_IRQn, GPIO5_IRQn};

#endif

#if defined(CHIP_GM6611)
/** Analog function selection */
static uint8_t s_AnalogFuncSel[MAX_GPIO_NUM][GPIO_NUMBER] =
{
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,0,1,6,5,4,2,3,31,30,29,28,27,26,
25,24,23,22,21,20,19,18,17,16,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
};
#endif

#if defined(CHIP_GM6611B)
/** Analog function selection */
static uint8_t s_AnalogFuncSel[MAX_GPIO_NUM][GPIO_NUMBER] =
{
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,0,1,6,5,4,2,3,31,30,29,28,27,26,
25,24,23,22,21,20,19,18,17,16,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,
};
#endif

#if (defined(CHIP_GM6601B))
/** Analog function selection */
static uint8_t s_AnalogFuncSel[MAX_GPIO_NUM][GPIO_NUMBER] =
{
0,1,32,32,32,2,3,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,28,29,30,31,16,17,18,19,20,21,22,
23,24,25,26,27,32,32,32,32,32,32,32,32,32,32,32,
32,4,5,6,32,32,32,32,32,32,32,32,32,32,32,32,
};
#endif

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
static uint32_t GPIO_GetInstance(GPIO_Device_T *pGPIO);

#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
/**
 * @brief Sets the Pull-up resistor for the current IO.
 *
 * @param instance The number of gpio module.
 * @param position The selected pins.
 * @param pull     Specifies the Pull-up or Pull-up X2 activation for the selected pins.
 *                 This parameter can be a value of @ref GPIO_pull_define.
 */
static void GPIO_SetPullUp(uint32_t instance, uint32_t position, uint32_t pull);
#endif
/*******************************************************************************
* Codes
*******************************************************************************/
static uint32_t GPIO_GetInstance(GPIO_Device_T *pGPIO)
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

#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
static void GPIO_SetPullUp(uint32_t instance, uint32_t position, uint32_t pull)
{
#if (!defined(CHIP_GM6601B))
    uint32_t tmp_pos = 0x00;
#endif

#if (defined(CHIP_GM6601))
    if (instance == 0)
    {
        IO_BITMASK_SET(SYSDEV->PUR[0], (GPIO_PULL_MASK << (position << 1)), (pull << (position << 1)));
    }
    else if (instance == 1)
    {
        if (position < 5)
        {
            IO_BITMASK_SET(SYSDEV->PUR[1], (GPIO_PULL_MASK << (position << 1)), (pull << (position << 1)));
        }
        else if (position > 4 && position < 14)
        {
            tmp_pos = position + 2;
            IO_BITMASK_SET(SYSDEV->PUR[1], (GPIO_PULL_MASK << (tmp_pos << 1)), (pull << (tmp_pos << 1)));
        }
        else if (position > 13)
        {
            tmp_pos = (position == 14) ? 0:1;
            IO_BITMASK_SET(SYSDEV->PUR[2], (GPIO_PULL_MASK << (tmp_pos << 1)), (pull << (tmp_pos << 1)));
        }
    }
    else if (instance == 2)
    {
        if (position < 14)
        {
            tmp_pos = position + 2;
            IO_BITMASK_SET(SYSDEV->PUR[2], (GPIO_PULL_MASK << (tmp_pos << 1)), (pull << (tmp_pos << 1)));
        }
        else if (position > 13)
        {
            tmp_pos = (position == 14) ? 0:1;
            IO_BITMASK_SET(SYSDEV->PUR[3], (GPIO_PULL_MASK << (tmp_pos << 1)), (pull << (tmp_pos << 1)));
        }
    }
    else if (instance == 3)
    {
        if (position == 0)
        {
            tmp_pos = position + 2;
        }
        else if (position > 0 && position < 4)
        {
            tmp_pos = position + 6;
        }
        else if (position > 3 && position < 6)
        {
            tmp_pos = position + 1;
        }
        else
        {
            tmp_pos = position + 4;
        }

        IO_BITMASK_SET(SYSDEV->PUR[3], (GPIO_PULL_MASK << (tmp_pos << 1)), (pull << (tmp_pos << 1)));
    }
#endif

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
    if ((instance == 0) || (instance == 1))
    {
        tmp_pos = (instance == 0) ? position : (position + 16);
        IO_BITMASK_SET(SYSDEV->PUR[0], (GPIO_PULL_MASK << tmp_pos), (pull << tmp_pos));
    }
    else if ((instance == 2) || (instance == 3))
    {
        tmp_pos = (instance == 2) ? position : (position + 16);
        IO_BITMASK_SET(SYSDEV->PUR[1], (GPIO_PULL_MASK << tmp_pos), (pull << tmp_pos));
    }
    else if ((instance == 4) || (instance == 5))
    {
    	tmp_pos = (instance == 4) ? position : (position + 16);
        IO_BITMASK_SET(SYSDEV->PUR[2], (GPIO_PULL_MASK << tmp_pos), (pull << tmp_pos));
    }
#endif

#if (defined(CHIP_GM6601B))
    if (instance == 0)
    {
        IO_BITMASK_SET(SYSDEV->PUR[0], (GPIO_PULL_MASK << (position << 1)), (pull << (position << 1)));
    }
    else if (instance == 1)
    {
        IO_BITMASK_SET(SYSDEV->PUR[1], (GPIO_PULL_MASK << (position << 1)), (pull << (position << 1)));
    }
    else if (instance == 2)
    {
        IO_BITMASK_SET(SYSDEV->PUR[2], (GPIO_PULL_MASK << (position << 1)), (pull << (position << 1)));
    }
    else if (instance == 3)
    {
        IO_BITMASK_SET(SYSDEV->PUR[3], (GPIO_PULL_MASK << (position << 1)), (pull << (position << 1)));
    }
#endif
}
#endif

RET_CODE GPIO_Init(GPIO_Device_T *pGPIO, GPIO_PinConfig_T *pConfig)
{
    uint32_t position;
    uint32_t instance = 0;
    uint32_t ioPosition = 0x00;
    uint32_t ioCurrent = 0x00;
#if (defined(CHIP_GM6621))
    uint32_t temp = 0x00;
#endif

    instance = GPIO_GetInstance(pGPIO);

    if ((instance >= MAX_GPIO_NUM) || (!IS_GPIO_PIN(pConfig->pin)))
    {
        return RET_ERROR;
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

#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
                /* Clear alternte function mode selection */
                pGPIO->AFSR = ioCurrent;
#endif

#if (defined(CHIP_GM6621))
                IO_BITMASK_SET(SYSDEV->MODE[instance], (0x03 << (position << 1)), 0x0);
#endif
            }
            else if (pConfig->mode == GPIO_MODE_OUTPUT)
            {
                pGPIO->OMCR = ioCurrent;
#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
                /* Clear alternte function mode selection */
                pGPIO->AFSR = ioCurrent;
#endif

#if (defined(CHIP_GM6621))
                IO_BITMASK_SET(SYSDEV->MODE[instance], (0x03 << (position << 1)), 0x0);
#endif
            }
            else if (pConfig->mode == GPIO_MODE_ANALOG)
            {
#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
                pGPIO->AFCR = ioCurrent;
#endif

#if (defined(CHIP_GM6621))
                IO_BITMASK_SET(SYSDEV->MODE[instance], (0x03 << (position << 1)), (0x2 << (position << 1)));
#endif
            }

            /* In case of Alternate function mode selection */
            if (pConfig->mode == GPIO_MODE_AF)
            {
#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
                pGPIO->AFCR = ioCurrent;
                if(pConfig->alternate & PINMUX1_MASK)
                {
                    IO_BIT_SET(SYSDEV->PINMUX[1], (0x1 << (pConfig->alternate & 0x1F)));
                }
                else if (pConfig->alternate & PINMUX0_MASK)
                {
                    IO_BIT_SET(SYSDEV->PINMUX[0], (0x1 << (pConfig->alternate & 0x1F)));
                }
#endif

#if (defined(CHIP_GM6621))
                IO_BITMASK_SET(SYSDEV->MODE[instance], (0x03 << (position << 1)), (0x1 << (position << 1)));

                temp = (instance << 1) + (position >> 3);
                IO_BITMASK_SET(SYSDEV->AFR[temp], (0xF << ((position & 7) << 2)), ((pConfig->alternate) << ((position & 7) << 2)));
#endif
            }

#if (defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
            if ((pConfig->mode == GPIO_MODE_AF) && ((pConfig->alternate & PINMUX2_MASK) == PINMUX2_MASK))
            {
                IO_BIT_SET(SYSDEV->PINMUX[2], (0x1 << (pConfig->alternate & 0x1F)));
            }

            /* In case of Analog function selection */
            if ((pConfig->mode == GPIO_MODE_ANALOG) && (s_AnalogFuncSel[instance][position] < 32))
            {
                IO_BIT_SET(SYSDEV->ANASEL, (0x1 << s_AnalogFuncSel[instance][position]));
            }
#endif

            /* Activate the Pull-up resistor for the current IO */
#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
            GPIO_SetPullUp(instance, position, pConfig->pull);
#endif

#if (defined(CHIP_GM6621))
            IO_BITMASK_SET(SYSDEV->PUR[instance], (GPIO_PULL_MASK << (position << 1)), ((pConfig->pull) << (position << 1)));
#endif

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

                NVIC_EnableIRQ(s_GPIOIrqs[instance]);
            }
        }
    }

    return RET_OK;
}


RET_CODE GPIO_DeInit(GPIO_Device_T *pGPIO, uint16_t pin, uint32_t alternate)
{
    uint32_t position;
    uint32_t instance = 0;
    uint32_t ioPosition = 0x00;
    uint32_t ioCurrent = 0x00;
#if (defined(CHIP_GM6621))
    uint32_t temp = 0x00;
#endif

    instance = GPIO_GetInstance(pGPIO);

    if ((instance >= MAX_GPIO_NUM) || (!IS_GPIO_PIN(pin)))
    {
        return RET_ERROR;
    }

    /* Configure the port pins */
    for (position = 0; position < GPIO_NUMBER; position++)
    {
        /* Get the IO position */
        ioPosition = ((uint32_t)0x01) << position;
        /* Get the current IO position */
        ioCurrent = (uint32_t)(pin) & ioPosition;

        if (ioCurrent == ioPosition)
        {
#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
            /* Clear alternte function mode selection */
            pGPIO->AFSR = ioCurrent;

            if(alternate & PINMUX1_MASK)
            {
                IO_BIT_CLEAR(SYSDEV->PINMUX[1], (0x1 << (alternate & 0x1F)));
            }
            else if (alternate & PINMUX0_MASK)
            {
                IO_BIT_CLEAR(SYSDEV->PINMUX[0], (0x1 << (alternate & 0x1F)));
            }
#endif

#if (defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
            if ((alternate == GPIOD10_AF2_SWDCLK) || (alternate == GPIOD11_AF2_SWDDIO))
            {
                IO_BITMASK_SET(SYSDEV->PINMUX[2], 0xFF, 0x5A);
            }
  #endif

  #if (defined(CHIP_GM6601B))
            if ((alternate == GPIOA12_AF2_SWDCLK) || (alternate == GPIOA13_AF2_SWDDIO))
            {
                IO_BITMASK_SET(SYSDEV->PINMUX[2], 0xFF, 0x5A);
            }
  #endif

            /* In case of Analog function selection */
            if (s_AnalogFuncSel[instance][position] < 32)
            {
                IO_BIT_CLEAR(SYSDEV->ANASEL, (0x1 << s_AnalogFuncSel[instance][position]));
            }
#endif

#if (defined(CHIP_GM6621))
            IO_BITMASK_SET(SYSDEV->MODE[instance], (0x03 << (position << 1)), 0x0);

            temp = (instance << 1) + (position >> 3);
            IO_BITMASK_SET(SYSDEV->AFR[temp], (0xF << ((position & 7) << 2)), 0x0);
#endif

            /* Clear the Pull-up resistor for the current IO */
#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B))
            GPIO_SetPullUp(instance, position, GPIO_NOPULL);
#endif

#if (defined(CHIP_GM6621))
            IO_BITS_CLEAR(SYSDEV->PUR[instance], (GPIO_PULL_MASK << (position << 1)));
#endif

            /* Disable the External Interrupt for the current IO */
            pGPIO->IDR = ioCurrent;
            pGPIO->IPCR = ioCurrent;
            pGPIO->ITCR = ioCurrent;
        }
    }

    return RET_OK;
}


void GPIO_PinWrite(GPIO_Pin_T pin, GPIO_PinState_T state)
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

unsigned int PIN_GET_TEST(GPIO_Pin_T pin)
{
   volatile unsigned int val = 0;

   val = *(pin);
//   val = *((volatile  unsigned int *)0x40012820);

   printf("val=0x%x\n", val);
   return  val;
}

GPIO_PinState_T GPIO_PinRead(GPIO_Pin_T pin)
{
    GPIO_PinState_T state = GPIO_PIN_RESET;

    if (PIN_GET(pin))
//    if (PIN_GET_TEST(pin))
    {
        state = GPIO_PIN_SET;
    }

    return state;
}

void GPIO_PinToggle(GPIO_Pin_T pin)
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

void GPIO_SetPinDirection(GPIO_Pin_T pin, GPIO_PinDir_T direction)
{
    GPIO_Device_T *pGPIO = (GPIO_Device_T *)((((uint32_t)(pin))>>12)<<12);
    uint32_t bitmask = 0;

    bitmask = (((((uint32_t)(pin))<<22)>>24)<<(((((uint32_t)(pin))>>11)&0x1)?8:0));

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

RET_CODE GPIO_SetPinFilter(GPIO_Pin_T pin, GPIO_InputGlitchFilter_T *pFilter)
{
    GPIO_Device_T *pGPIO = (GPIO_Device_T *)((((uint32_t)(pin))>>12)<<12);
    uint32_t bitmask = 0;
    uint32_t instance = 0;
 #if (!defined(CHIP_GM6621))
    uint32_t position = 0;
    uint16_t ioCurrent = 0;
 #endif
    uint32_t temp = 0x00;


    instance = GPIO_GetInstance(pGPIO);

    if (instance >= MAX_GPIO_NUM)
    {
        return RET_ERROR;
    }

    bitmask = (((((uint32_t)(pin))<<22)>>24)<<(((((uint32_t)(pin))>>11)&0x1)?8:0));

#if (!defined(CHIP_GM6621))
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
#endif

    if (pFilter->bypass)
    {
#if (defined(CHIP_GM6601))
        if (instance == 0)
        {
             IO_BIT_SET(SYSDEV->FLTBP[0], (1 << position));
        }
        else if (instance == 1)
        {
            if (position < 5)
            {
                IO_BIT_SET(SYSDEV->FLTBP[0], ((1 << position) << 16));
            }
            else if (position > 4 && position < 14)
            {
                position += 2;
                IO_BIT_SET(SYSDEV->FLTBP[0], ((1 << position) << 16));
            }
            else if (position > 13)
            {
                temp = (position == 14) ? 0:1;
                IO_BIT_SET(SYSDEV->FLTBP[1], (1 << temp));
            }
        }
        else if (instance == 2)
        {
            if (position < 14)
            {
                position += 2;
                IO_BIT_SET(SYSDEV->FLTBP[1], (1 << position));
            }
            else if (position > 13)
            {
                 position = (position == 14) ? 0:1;
                 IO_BIT_SET(SYSDEV->FLTBP[1], ((1 << position) << 16));
            }
        }
        else if (instance == 3)
        {
            if (position == 0)
            {
                 position += 2;
            }
            else if (position > 0 && position < 4)
            {
                 position += 6;
            }
            else if (position > 3 && position < 6)
            {
                 position += 1;
            }
            else
            {
                 position += 4;
            }
            IO_BIT_SET(SYSDEV->FLTBP[1], ((1 << position) << 16));
        }

#else
        if (instance == 0 || instance == 1)
        {
            temp = (instance == 0) ? bitmask : (bitmask << 16);
            IO_BIT_SET(SYSDEV->FLTBP[0], temp);   /*0x100*/
        }
        else if (instance == 2 || instance == 3)
        {
            temp = (instance == 2) ? bitmask : (bitmask << 16);
            IO_BIT_SET(SYSDEV->FLTBP[1], temp);
        }
#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
        else if (instance == 4 || instance == 5 )
        {
        	temp = (instance == 4) ? bitmask : (bitmask << 16);
            IO_BIT_SET(SYSDEV->FLTBP[2], temp);
        }
#endif
#endif
    }
    else
    {
#if (defined(CHIP_GM6601))
        if (instance == 0)
        {
             IO_BIT_CLEAR(SYSDEV->FLTBP[0], (1 << position));
            IO_BITMASK_SET(SYSDEV->FLTCS[0], (0x3 << (position << 1)), ((pFilter->clock) << (position << 1)));
        }
        else if (instance == 1)
        {
            if (position < 5)
            {
                IO_BIT_CLEAR(SYSDEV->FLTBP[0], ((1 << position) << 16));
                IO_BITMASK_SET(SYSDEV->FLTCS[1], (0x3 << (position << 1)), ((pFilter->clock) << (position << 1)));
            }
            else if (position > 4 && position < 14)
            {
                position += 2;
                IO_BIT_CLEAR(SYSDEV->FLTBP[0], ((1 << position) << 16));
                IO_BITMASK_SET(SYSDEV->FLTCS[1], (0x3 << (position << 1)), ((pFilter->clock) << (position << 1)));
            }
            else if (position > 13)
            {
                position = (position == 14) ? 0:1;
                IO_BIT_CLEAR(SYSDEV->FLTBP[1], (1 << position));
                IO_BITMASK_SET(SYSDEV->FLTCS[2], (0x3 << (position << 1)), ((pFilter->clock) << (position << 1)));
            }
        }
        else if (instance == 2)
        {
            if (position < 14)
            {
                position += 2;
                IO_BIT_CLEAR(SYSDEV->FLTBP[1], (1 << position));
                IO_BITMASK_SET(SYSDEV->FLTCS[2], (0x3 << (position << 1)), ((pFilter->clock) << (position << 1)));
            }
            else if (position > 13)
            {
                 position = (position == 14) ? 0:1;
                 IO_BIT_CLEAR(SYSDEV->FLTBP[1], ((1 << position) << 16));
                 IO_BITMASK_SET(SYSDEV->FLTCS[3], (0x3 << (position << 1)), ((pFilter->clock) << (position << 1)));
            }
        }
        else if (instance == 3)
        {
             if (position == 0)
            {
                 position += 2;
            }
            else if (position > 0 && position < 4)
            {
                 position += 6;
            }
            else if (position > 3 && position < 6)
            {
                 position += 1;
            }
            else
            {
                 position += 4;
            }
             IO_BIT_CLEAR(SYSDEV->FLTBP[1], ((1 << position) << 16));
             IO_BITMASK_SET(SYSDEV->FLTCS[3], (0x3 << (position << 1)), ((pFilter->clock) << (position << 1)));
        }
#else
        if (instance == 0 || instance == 1)
        {
            temp = (instance == 0) ? bitmask : (bitmask << 16);
            IO_BIT_CLEAR(SYSDEV->FLTBP[0], temp);
        }
        else if (instance == 2 || instance == 3)
        {
            temp = (instance == 2) ? bitmask : (bitmask << 16);
            IO_BIT_CLEAR(SYSDEV->FLTBP[1], temp);
        }
#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
        else if (instance == 4 || instance == 5)
        {
        	temp = (instance == 4) ? bitmask : (bitmask << 16);
            IO_BIT_CLEAR(SYSDEV->FLTBP[2], bitmask);   /*0x100*/
        }
#endif

#if (defined(CHIP_GM6601B))
        IO_BITMASK_SET(SYSDEV->FLTCS[instance], (0x3 << (position << 1)), ((pFilter->clock) << (position << 1)));  /*0x58*/
#endif

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
    IO_BITMASK_SET(SYSDEV->DBCS[instance], (0x3 << (position << 1)), ((pFilter->clock) << (position << 1)));  /*0x58*/
#endif

#if (defined(CHIP_GM6621))
        IO_BITMASK_SET(SYSDEV->DBCS[instance], 0x3, (pFilter->clock & 0x3));
#endif

#endif

        switch (pFilter->clock)
        {
            case PAD_FLT_CLK0:  /*0x48*/
#if (defined(CHIP_GM6621) || defined(CHIP_GM6611B) || defined(CHIP_GM6611))
                IO_BITMASK_SET(SYSDEV->CLK2, SYS_FLTCPS_CLK0_MASK | SYS_FLTCPS_CLK1_MASK | \
                               SYS_FLTCPS_CLK2_MASK, SYS_FLTCPS_CLK0_FIELD(pFilter->prescaler));
#else
                IO_BITMASK_SET(SYSDEV->FLTCPS, SYS_FLTCPS_CLK0_MASK | SYS_FLTCPS_CLK1_MASK | \
                               SYS_FLTCPS_CLK2_MASK, SYS_FLTCPS_CLK0_FIELD(pFilter->prescaler));
#endif
                break;
            case PAD_FLT_CLK1:
#if (defined(CHIP_GM6621) || defined(CHIP_GM6611B) || defined(CHIP_GM6611))
                 IO_BITMASK_SET(SYSDEV->CLK2, SYS_FLTCPS_CLK0_MASK | SYS_FLTCPS_CLK1_MASK | \
                                SYS_FLTCPS_CLK2_MASK, SYS_FLTCPS_CLK1_FIELD(pFilter->prescaler));
#else
                 IO_BITMASK_SET(SYSDEV->FLTCPS, SYS_FLTCPS_CLK0_MASK | SYS_FLTCPS_CLK1_MASK | \
                                SYS_FLTCPS_CLK2_MASK, SYS_FLTCPS_CLK1_FIELD(pFilter->prescaler));
#endif
                break;
            case PAD_FLT_CLK2:
#if (defined(CHIP_GM6621) || defined(CHIP_GM6611B) || defined(CHIP_GM6611))
                IO_BITMASK_SET(SYSDEV->CLK2, SYS_FLTCPS_CLK0_MASK | SYS_FLTCPS_CLK1_MASK | \
                               SYS_FLTCPS_CLK2_MASK, SYS_FLTCPS_CLK2_FIELD(pFilter->prescaler));
#else
                IO_BITMASK_SET(SYSDEV->FLTCPS, SYS_FLTCPS_CLK0_MASK | SYS_FLTCPS_CLK1_MASK | \
                               SYS_FLTCPS_CLK2_MASK, SYS_FLTCPS_CLK2_FIELD(pFilter->prescaler));
#endif
                break;
            case PAD_IRC_LS_CLK:
                break;
            default:
                break;
        }

    }

    return RET_OK;
}

#if (defined(CHIP_GM6601B))
void GPIO_TriggerADC(GPIO_Device_T *pGPIO, uint32_t pin,  uint8_t enable)
{
    if (enable == TRUE)
    {
        pGPIO->TRIGEN |= pin;
    }
    else
    {
        pGPIO->TRIGEN &=  ~pin;
    }
}
#endif

