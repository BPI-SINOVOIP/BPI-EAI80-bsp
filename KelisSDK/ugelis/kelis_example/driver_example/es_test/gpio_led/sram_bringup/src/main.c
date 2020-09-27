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
 * @brief       gpio source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <gm_hal_gpio.h>


/* Change this if you have an LED connected to a custom port */
#define PORT     "GPIOC"

#define GPIO_NUMBER           ((uint32_t)16)
#define EXTI_MODE             ((uint32_t)0x10000000)

#define GPIO_PIN_CNT   (176)

#define SW0_GPIO_PIN   8    /*PC8*/
#define RET_OK       0x00
typedef int     RET_CODE;
//static int cnt = 0;
void GPIO_FILTER_FUNCTION(void);
static uint32_t HAL_GPIO_GetInstance(GPIO_Device_T *pGPIO);


static GPIO_Device_T *const s_GPIOBases[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI, GPIOJ, GPIOK};
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

struct device *dev;
//static struct gpio_callback gpio_cb;
#define READ_REG(REG)         ((REG))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

void button_pressed(struct device *gpiob, struct gpio_callback *cb,
                    uint32_t pins)
{
    printk("Button pressed\n");
    /*gpio_pin_write(dev, LED_GPIO_PIN, cnt % 2);*/
}

#define GPIO_LEVEL_TEST GPIO_DRIVING_LEVEL3

static void GPIOPinOutConfig(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All & (~(GPIO_PIN_7)) ; //& (~(GPIO_PIN_8));
    //GPIO_InitStruct.pin =  GPIO_PIN_All    ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    printk("line=%d \n", __LINE__);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All;//& (~(GPIO_PIN_10)) & (~(GPIO_PIN_11)) ;
    //GPIO_InitStruct.pin =  GPIO_PIN_All;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    printk("xx line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All;// & (~(GPIO_PIN_5)) & (~(GPIO_PIN_7)) ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT ;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All & (~(GPIO_PIN_6)) & (~(GPIO_PIN_14)) ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8; //GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_LEVEL_TEST;
    HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


}

static void GPIOPinInPutOutConfig(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;//& (~(GPIO_PIN_7)) ;//& (~(GPIO_PIN_8));
    //GPIO_InitStruct.pin =  GPIO_PIN_All    ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    printk("line=%d \n", __LINE__);

    GPIO_InitStruct.pin =  GPIO_PIN_All;//& (~(GPIO_PIN_10)) & (~(GPIO_PIN_11)) ;
    //GPIO_InitStruct.pin =  GPIO_PIN_All;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);

    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);

    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    printk("xx line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All;// & (~(GPIO_PIN_5)) & (~(GPIO_PIN_7)) ;  // uart
    GPIO_InitStruct.mode = GPIO_MODE_INPUT ;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All & (~(GPIO_PIN_6)) & (~(GPIO_PIN_14)) ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);

}


void LedOnOff(GPIO_Pin_T pin, uint8_t onOff)
{
    //k_sleep(1000);

    if (onOff == 1)
    {
        PIN_CLEAR(pin);
    }
    else
    {
        PIN_SET(pin);
    }
}

RET_CODE GPIO_OUTPUT_main(void)
{
    RET_CODE ret = RET_OK;

    uint32_t i = 0;

    GPIOPinOutConfig();

    for (i = 0; i < GPIO_PIN_CNT; i++)
    {
        HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i), GPIO_PIN_SET);
    }

    return ret;

}

RET_CODE GPIO_INPUT_OUTPUT_main(void)
{
    RET_CODE ret = RET_OK;

    uint32_t i = 0;

    GPIOPinInPutOutConfig();

    for (i = 16; i < 32; i++) // GPIOB OUT
    {
        HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i), GPIO_PIN_SET);
    }
    return ret;

}


RET_CODE GPIO_INPUT_OUTPUT_DEMO1_main(void)
{
    RET_CODE ret = RET_OK;

    uint32_t i = 0;

    GPIO_PinConfig_T GPIO_InitStruct;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));


#if 0
    GPIOPinInPutOutConfig();
    while (1)
    {

        for (i = 5; i < 9; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i))) /*GPIOA5,GPIOA6,GPIOA7,GPIO A8*/
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i + 16), GPIO_PIN_SET); /*GPIOB5,GPIOB6,GPIOB7,GPIO B8*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i + 16), GPIO_PIN_RESET); /*GPIOB5,GPIOB6,GPIOB7,GPIO B8*/
            }
        }

    }
#else
    GPIO_InitStruct.pin =  GPIO_PIN_All;  //& (~(GPIO_PIN_7)) ;//& (~(GPIO_PIN_8));
    //GPIO_InitStruct.pin =  GPIO_PIN_All    ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL3;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    while (1)
    {

        for (i = 0; i < 16; i++)
        {
            //if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i))) /*GPIOA5,GPIOA6,GPIOA7,GPIO A8*/
            //if(i == 0)
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i), GPIO_PIN_SET); /*GPIOB5,GPIOB6,GPIOB7,GPIO B8*/
            }
            //else
            {
                //  HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i),GPIO_PIN_RESET); /*GPIOB5,GPIOB6,GPIOB7,GPIO B8*/
            }
        }

    }

#endif

    return ret;

}


#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))


void patch_config(void) /*FIX HIRC UART PRINT ISSUE*/
{
    HW32_REG(0x1ffff0a8) = 0xfffffbac;
    HW32_REG(0x1ffff0ac) = 0xff1fffff;
    HW32_REG(0x1ffff0b0) = 0xff3fffff;
}

#if 1
#define GPIO_RX_PIN GPIOA0
void GPIO_FILTER_FUNCTION(void)
{

    GPIO_InputGlitchFilter_T GPIO_InputGlitchFilter;
    int i = 0;

    //  memset(&GPIO_InputGlitchFilter,0,sizeof(GPIO_InputGlitchFilter_T));
    GPIO_InputGlitchFilter.parent_clk_src_sel = DB_PARENT_SRC_CLK_SEL_FCLK;
    GPIO_InputGlitchFilter.child_clk_sel = PAD_FLT_CLK3;
    GPIO_InputGlitchFilter.prescaler =  FLT_CLK3_PSC_DIV_64K;
    GPIO_InputGlitchFilter.db_bypass_en = ENABLE;

    printk("line=%d, Func:%s \n", __LINE__, __FUNCTION__);

    for (i = 0; i < 168; i++)
    {
        if (i >= 16 && (i < 32))
        {
            continue;
        }
        printk("i=%d, Func:%s \n", i, __FUNCTION__);
        HAL_GPIO_SetPinFilter(GPIO_BIT_ADDR(i), &GPIO_InputGlitchFilter);
    }

    printk("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
}

#endif


#define GPIO_DRIVING_LEVEL_TEST  GPIO_DRIVING_LEVEL0
void GPIO_INPUT_OUTPUT_TEST(void)
{

    GPIO_PinConfig_T GPIO_InitStruct;
    int i = 0;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;//& (~(GPIO_PIN_7)) ;//& (~(GPIO_PIN_8));
    //GPIO_InitStruct.pin =  GPIO_PIN_All    ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    printk("line=%d \n", __LINE__);

    GPIO_InitStruct.pin =  GPIO_PIN_All;//& (~(GPIO_PIN_10)) & (~(GPIO_PIN_11)) ;
    //GPIO_InitStruct.pin =  GPIO_PIN_All;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);

    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);

    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    printk("xx line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All;// & (~(GPIO_PIN_5)) & (~(GPIO_PIN_7)) ;  // uart
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT ;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);

    GPIO_InitStruct.pin =  GPIO_PIN_All & (~(GPIO_PIN_0)) & (~(GPIO_PIN_1)) & (~(GPIO_PIN_2)) & (~(GPIO_PIN_3)) & (~(GPIO_PIN_4));
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All & (~(GPIO_PIN_6)) & (~(GPIO_PIN_14)) ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);


    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

    printk("line=%d \n", __LINE__);
    int j = 0;
#if 0
    while (1)
    {

        for (i = 85; i < 90; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)))
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 85), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 85), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
        }
        for (i = 90; i < 92; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)))
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 81), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 81), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
        }

        for (i = 92; i < 101; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)))
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 80), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 80), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
        }
        for (i = 102; i < 117; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)))
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 81), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 81), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
        }

        if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(117)))
        {
            HAL_GPIO_PinWrite(GPIO_BIT_ADDR(37), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
        }
        else
        {
            HAL_GPIO_PinWrite(GPIO_BIT_ADDR(37), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
        }

        for (i = 119; i < 123; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)))
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 81), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 81), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
        }

        for (i = 127; i < 140; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)))
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 85), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 85), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
        }
        for (i = 141; i < 144; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)))
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 86), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 86), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
        }

        for (i = 144; i < 150; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)))
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 85), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 85), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
        }

        if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(151)))
        {
            HAL_GPIO_PinWrite(GPIO_BIT_ADDR(66), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
        }
        else
        {
            HAL_GPIO_PinWrite(GPIO_BIT_ADDR(66), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
        }

        for (i = 152; i < 169; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)))
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 84), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i - 84), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
        }
    }
#endif
    while (1)
    {
        for (i = 0; i < 16; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(i)))
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i + 16), GPIO_PIN_SET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i + 16), GPIO_PIN_RESET);    /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
            }
        }

    }

}

#define GPIO_DRIVING_LEVEL_TEST  GPIO_DRIVING_LEVEL0

void GPIO_OUTPUT_OD_TEST(void)
{

    GPIO_PinConfig_T GPIO_InitStruct;
    int i = 0;
    unsigned int open_drain = 0xFFFFFFFF;

    unsigned int open_drain_spel = 0xFFFFFFFF & (~(GPIO_PIN_6 << 16)) & (~(GPIO_PIN_14 << 16)) ;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;//& (~(GPIO_PIN_7)) ;//& (~(GPIO_PIN_8));

    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    GPIO_InitStruct.alternate = 0;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);


    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All & (~(GPIO_PIN_6)) & (~(GPIO_PIN_14)) ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL_TEST;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    WRITE_REG(*(volatile uint32_t *) 0x40000238, open_drain);
    WRITE_REG(*(volatile uint32_t *) 0x4000023C, open_drain);

    WRITE_REG(*(volatile uint32_t *) 0x40000240, open_drain);

    WRITE_REG(*(volatile uint32_t *) 0x40000244, READ_REG(*(volatile uint32_t *) 0x40000244) | open_drain_spel); // GPIOG,GPIOH

    WRITE_REG(*(volatile uint32_t *) 0x40000248, open_drain);

    WRITE_REG(*(volatile uint32_t *) 0x4000024C, open_drain);

    while (1)
    {
        for (i = 0; i < 176; i++)
        {
            if (i == 116 || i == 121)
            {
                continue;
            }
            HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i), GPIO_PIN_RESET); /*GPIOB0, ...GPIOB5,GPIOB6,GPIOB7 ... GPIO B15*/
        }

    }


}

uint32_t read_data;
void main(void)
{

    printk("BringUp:Hello World! %s\n", CONFIG_ARCH);

    patch_config();

    read_data = READ_REG(*(volatile uint32_t *) 0x40000070);
    read_data |= (1 << 1);
    WRITE_REG(*(volatile uint32_t *) 0x40000070, read_data);

    read_data = READ_REG(*(volatile uint32_t *) 0x40000080);
    read_data |= (1 << 1);
    WRITE_REG(*(volatile uint32_t *) 0x40000080, read_data);

    printk("SRAM step0 \n");
#if 0
    GPIO_OUTPUT_main();
    while (1);
#endif
    //GPIO_INPUT_OUTPUT_DEMO1_main();
    //GPIO_INPUT_OUTPUT_main();

#if 0
    GPIO_INPUT_OUTPUT_TEST();
#endif

    GPIO_OUTPUT_OD_TEST();

    while (1)
    {
        printk("gpio demo\n");
        k_sleep(1000);
    }

}
