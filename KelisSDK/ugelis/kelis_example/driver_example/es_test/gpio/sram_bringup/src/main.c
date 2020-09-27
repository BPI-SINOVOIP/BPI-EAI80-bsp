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
#include <misc/printk.h>
#include <gm_hal_gpio.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>

/*Gpio output  Test*/

//#define TEST_CASE_1

/*Gpio Input Test*/
//#define TEST_CASE_2

/*Gpio External Interrupt  Test*/
//#define TEST_CASE_3

/*Gpio glitch filter Test*/
//#define TEST_CASE_4

/*Gpio bitband Test*/
//#define TEST_CASE_5

/*  Respond to Event Sent by PES*/
#define TEST_CASE_RESPOND_PES_6

/*Gpio glitch filter Test*/
#define TEST_CASE_12

#define __CPU_HALT_DEBUG__    do { } while(0)
#define BITBAND(addr, bitnum) ((addr&0xf0000000)+0x2000000+((addr&0xfffff)<<5)+(bitnum<<2))
#define MEMADDR(addr)  *((__IO uint32_t*)(addr))
#define SysTick_DelayMS(ms)   k_sleep(ms)
#define     HW32_REG(ADDRESS)   (*((volatile uint32_t *)(ADDRESS)))
#define     HW16_REG(ADDRESS)   (*((volatile uint32_t *)(ADDRESS)))
#define     HW8_REG(ADDRESS)   (*((volatile uint32_t *)(ADDRESS)))


#define PORT     "GPIOB"
#define SW0_GPIO_PIN   1

/******************************************************************************
* Local types
******************************************************************************/


/*******************************************************************************
* Global variables
**
*****************************************************************************/
#ifdef TEST_CASE_3
    struct device *dev;
    struct device *gpiob;
    static struct gpio_callback gpio_cb;
#endif

/*******************************************************************************
* Static variables
*******************************************************************************/

/*******************************************************************************
* Reference variables
*******************************************************************************/

/*******************************************************************************
* Local function prototypes
*******************************************************************************/
int lowbyte_clr_test(unsigned int addr_base);
int lowbyte_set_test(unsigned int addr_base);
int lowbyte_toggle_test(unsigned int addr_base);
int highbyte_toggle_test(unsigned int addr_base);
int highbyte_clr_test(unsigned int addr_base);
int highbyte_set_test(unsigned int addr_base);
int register_wr_test(unsigned int addr_base);
int cpu_pes_lowbyte(unsigned int addr_base);
int cpu_pes_highbyte(unsigned int addr_base);

static RET_CODE GPIO_Output_testcase(GPIO_Device_T *pGPIO, uint32_t pin)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    uint32_t instance = 0;
    uint32_t position = 0;
    uint32_t ioPosition = 0x00;
    uint32_t ioCurrent = 0x00;
    uint32_t pin_num = 0;
    RET_CODE ret = RET_OK;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    if (pGPIO == GPIOA)
    {
        instance = 0;
    }
    else if (pGPIO == GPIOB)
    {
        instance = 1;
    }
    else if (pGPIO == GPIOC)
    {
        instance = 2;
    }
    else if (pGPIO == GPIOD)
    {
        instance = 3;
    }
    else if (pGPIO == GPIOE)
    {
        instance = 4;
    }
    else if (pGPIO == GPIOF)
    {
        instance = 5;
    }
    else if (pGPIO == GPIOG)
    {
        instance = 6;
    }
    else if (pGPIO == GPIOH)
    {
        instance = 7;
    }
    else if (pGPIO == GPIOI)
    {
        instance = 8;
    }
    else if (pGPIO == GPIOJ)
    {
        instance = 9;
    }
    else if (pGPIO == GPIOK)
    {
        instance = 10;
    }

    for (position = 0; position < 16; position++)
    {
        /* Get the IO position */
        ioPosition = ((uint32_t)0x01) << position;
        /* Get the current IO position */
        ioCurrent = (uint32_t)(pin) & ioPosition;

        if (ioCurrent == ioPosition)
        {
            GPIO_InitStruct.pin = pin;
            GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
            GPIO_InitStruct.pull = GPIO_NOPULL;
            GPIO_InitStruct.alternate = 0;
            ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
            if (ret != RET_OK)
            {
                return RET_ERROR;
            }

            pin_num = position + instance * 16;

            if (instance == 0 && position == 5)
            {
                printf("debug\n");
            }

            HAL_GPIO_PinWrite(GPIO_BIT_ADDR(pin_num), GPIO_PIN_RESET);

            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != GPIO_PIN_RESET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x,position=%d, output low level x unit test fail!\n", (unsigned int)pGPIO, pin, position);
                return RET_ERROR;
            }

            HAL_GPIO_PinWrite(GPIO_BIT_ADDR(pin_num), GPIO_PIN_SET);

            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != GPIO_PIN_SET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, output high level x unit test fail!\n", (unsigned int)pGPIO, pin);

                return RET_ERROR;
            }

            HAL_GPIO_PinToggle(GPIO_BIT_ADDR(pin_num));

            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != GPIO_PIN_RESET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, toggle output unit test fail!\n", (unsigned int)pGPIO, pin);

                return RET_ERROR;
            }

            pGPIO->DOR |= (1 << position);
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != GPIO_PIN_SET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, output low level xx unit test fail!\n", (unsigned int)pGPIO, pin);

                return RET_ERROR;
            }

            pGPIO->DOR &= ~(1 << position);
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != GPIO_PIN_RESET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, output high level xx unit test fail!\n", (unsigned int)pGPIO, pin);

                return RET_ERROR;
            }

            pGPIO->OBTR |= (1 << position);
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != GPIO_PIN_SET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, toggle output unit test fail!\n", (unsigned int)pGPIO, pin);
                return RET_ERROR;
            }

            pGPIO->OBSR |= (1 << position);
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != GPIO_PIN_RESET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, output low level xxx unit test fail!\n", (unsigned int)pGPIO, pin);
                return RET_ERROR;
            }

            pGPIO->OBSR |= (1 << position) << 16;
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != GPIO_PIN_SET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, output high level xxx unit test fail!\n", (unsigned int)pGPIO, pin);
                return RET_ERROR;
            }

            pGPIO->OBTR |= (1 << position);
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != GPIO_PIN_RESET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, toggle output unit test fail!\n", (unsigned int)pGPIO, pin);
                return RET_ERROR;
            }
            pGPIO->OBSR |= (1 << position) | ((1 << position) << 16);
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != GPIO_PIN_SET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, output high level xxxx unit test fail!\n", (unsigned int)pGPIO, pin);
                return RET_ERROR;
            }
        }
    }

    return RET_OK;
}

static RET_CODE GPIO_Input_testcase(GPIO_Device_T *pGPIO, uint32_t pin, GPIO_PinState_T state)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    uint32_t instance = 0;
    uint32_t position = 0;
    uint32_t ioPosition = 0x00;
    uint32_t ioCurrent = 0x00;
    uint32_t pin_num = 0;
    RET_CODE ret = RET_OK;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    if (pGPIO == GPIOA)
    {
        instance = 0;
    }
    else if (pGPIO == GPIOB)
    {
        instance = 1;
    }
    else if (pGPIO == GPIOC)
    {
        instance = 2;
    }
    else if (pGPIO == GPIOD)
    {
        instance = 3;
    }
    else if (pGPIO == GPIOE)
    {
        instance = 4;
    }
    else if (pGPIO == GPIOF)
    {
        instance = 5;
    }
    else if (pGPIO == GPIOG)
    {
        instance = 6;
    }
    else if (pGPIO == GPIOH)
    {
        instance = 7;
    }
    else if (pGPIO == GPIOI)
    {
        instance = 8;
    }
    else if (pGPIO == GPIOJ)
    {
        instance = 9;
    }
    else if (pGPIO == GPIOK)
    {
        instance = 10;
    }

    for (position = 0; position < 16; position++)
    {
        /* Get the IO position */
        ioPosition = ((uint32_t)0x01) << position;
        /* Get the current IO position */
        ioCurrent = (uint32_t)(pin) & ioPosition;

        if (ioCurrent == ioPosition)
        {
            GPIO_InitStruct.pin = pin;
            GPIO_InitStruct.mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.pull = GPIO_NOPULL;
            GPIO_InitStruct.alternate = 0;
            ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
            if (ret != RET_OK)
            {
                return RET_ERROR;
            }

            pin_num = position + instance * 16;
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR(pin_num)) != state)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x,position=%d, input read unit test fail!\n", (unsigned int)pGPIO, pin, position);
                return RET_ERROR;
            }
        }
    }

    return RET_OK;
}

static void gpio_reset_release(void)
{
    HW32_REG(0x40000070) |= (1 << 1);
}

static void gpio_clk_enable(void)
{
    HW32_REG(0x40000080) |= (1 << 1);
}

void patch_config(void) /*FIX HIRC UART PRINT ISSUE*/
{
    HW32_REG(0x1ffff0a8) = 0xfffffbac;
    HW32_REG(0x1ffff0ac) = 0xff1fffff;
    HW32_REG(0x1ffff0b0) = 0xff3fffff;
}


#define GPIO_RX_PIN GPIOA0
void gpio_filter_demo(void)
{
#ifdef TEST_CASE_4
    GPIO_InputGlitchFilter_T GPIO_InputGlitchFilter;
    int i = 0;

    memset(&GPIO_InputGlitchFilter, 0, sizeof(GPIO_InputGlitchFilter_T));
    GPIO_InputGlitchFilter.parent_clk_src_sel = DB_PARENT_SRC_CLK_SEL_FCLK;
    GPIO_InputGlitchFilter.child_clk_sel = PAD_FLT_CLK3;
    GPIO_InputGlitchFilter.prescaler =  FLT_CLK3_PSC_DIV_64K;
    GPIO_InputGlitchFilter.db_bypass_en = ENABLE;

    GPIO_PinConfig_T GPIO_InitStruct;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL0;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL0;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_All ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.driving = GPIO_DRIVING_LEVEL0;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    printk("line=%d, Func:%s \n", __LINE__, __FUNCTION__);

    for (i = 0; i < 16; i++)
    {
        HAL_GPIO_SetPinFilter(GPIO_BIT_ADDR((i + 32)), &GPIO_InputGlitchFilter); /*GPIOC*/
        HAL_GPIO_PinWrite(GPIO_BIT_ADDR((i + 16)), GPIO_PIN_SET); /*GPIOB*/
    }

    while (1)
    {
        for (i = 0; i < 16; i++)
        {
            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR((i + 32)))) /*GPIOC*/
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR((i + 48)), GPIO_PIN_SET);    /*GPIOD*/
            }
            else
            {
                HAL_GPIO_PinWrite(GPIO_BIT_ADDR((i + 48)), GPIO_PIN_RESET);
            }
        }
    }

    printk("line=%d, Func:%s \n", __LINE__, __FUNCTION__);
#endif
}

void button_pressed(struct device *gpio, struct gpio_callback *cb, uint32_t pins)
{
    printk("Button pressed\n");
}

void gpio_interrpute_demo(void)
{
#ifdef TEST_CASE_3
    int val ;
    int i = 0;
    GPIO_PinConfig_T GPIO_InitStruct;
    gpio_clk_enable();
    gpio_reset_release();
    GPIO_InitStruct.pin = GPIO_PIN_All;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    for (i = 0; i < 16; i++)
    {
        HAL_GPIO_PinWrite(GPIO_BIT_ADDR(i), GPIO_PIN_SET);
    }

    dev = device_get_binding(PORT);
    gpiob = dev;

    /* Set key pin as interrupt active flalling mode */
    gpio_pin_configure(dev, SW0_GPIO_PIN, GPIO_INT | GPIO_PUD_PULL_DOWN | GPIO_MODE_IT_RISING);
    gpio_init_callback(&gpio_cb, button_pressed, BIT(SW0_GPIO_PIN));

    gpio_add_callback(gpiob, &gpio_cb);
    gpio_pin_enable_callback(gpiob, SW0_GPIO_PIN);

    while (1)
    {
        gpio_pin_read(dev, SW0_GPIO_PIN, &val);
        printk("key vale = [%d]\n", val);
        k_sleep(1000);
    }
#endif
}

int pes_triger_gpio(void)
{
    unsigned int gpio_base;
    int err_code = 0;
    int i = 0;
    unsigned int s_gpio_base[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, GPIOE_BASE, GPIOF_BASE, GPIOG_BASE, GPIOH_BASE, GPIOI_BASE, GPIOJ_BASE, GPIOK_BASE};
    char *log[12] = {"GPIOA", "GPIOB", "GPIOC", "GPIOD", "GPIOE", "GPIOF", "GPIOG", "GPIOH", "GPIOI", "GPIOJ", "GPIOK"};

    GPIO_PESConfig_T gpioPesCfg;
    gpioPesCfg.action_sel = 1;
    gpioPesCfg.channel_sel = 1;
    gpioPesCfg.event_action_en = ENABLE;
    gpioPesCfg.pin_sel = PES_SEL_GPIO_PIN_1;
    //HAL_GPIO_PesConfig(GPIOA0, &gpioPesCfg);

    printk("func:%s begin\n", __FUNCTION__);
    while (i < sizeof(s_gpio_base) / sizeof(s_gpio_base[0]))
    {
        gpio_base = s_gpio_base[i];
        err_code += register_wr_test(gpio_base);
        err_code += lowbyte_clr_test(gpio_base);
        err_code += lowbyte_set_test(gpio_base);
        err_code += lowbyte_toggle_test(gpio_base);
        err_code += highbyte_clr_test(gpio_base);
        err_code += highbyte_set_test(gpio_base);
        err_code += highbyte_toggle_test(gpio_base);
        err_code += cpu_pes_lowbyte(gpio_base);
        err_code += cpu_pes_highbyte(gpio_base);

        if (err_code != 0)
        {
            printf("%s PES test failed, err_code = %d\n", log[i], err_code);
        }
        else
        {
            printf("%s PES  test pass\n", log[i]);
        }
        i++;
    }
    printk("func:%s end\n", __FUNCTION__);

    return 0;
}

int register_wr_test(unsigned int addr_base)
{
    int err_code = 0;
    unsigned int data_out;
    HW32_REG(addr_base + 0xc00) = 0x00000000;
    data_out    =   HW32_REG(addr_base + 0xc00);
    if (data_out != 0x00000000)
    {
        err_code += 1;
        printf("write 0x00000000 to pes_register wrong , data = 0x%x\n", data_out);
    }
    HW32_REG(addr_base + 0xc00) = 0xffffffff;
    data_out    =   HW32_REG(addr_base + 0xc00);
    if (data_out != 0xf00f0031)
    {
        err_code += 1;
        printf("write 0xffffffff to pes_register wrong , data = 0x%x\n", data_out);
    }
    HW32_REG(addr_base + 0xc00) = 0xaaaaaaaa;
    data_out    =   HW32_REG(addr_base + 0xc00);
    if (data_out != 0xa00a0020)
    {
        err_code += 1;
        printf("write 0xaaaaaaaa to pes_register wrong , data = 0x%x\n", data_out);
    }
    HW32_REG(addr_base + 0xc00) = 0x55555555;
    data_out    =   HW32_REG(addr_base + 0xc00);
    if (data_out != 0x50050011)
    {
        err_code += 1;
        printf("write 0x55555555 to pes_register wrong , data = 0x%x\n", data_out);
    }
    if (!err_code)
    {
        return 0;
    }
    else
    {
        return 1;
    }

}

int cpu_pes_lowbyte(unsigned int addr_base)
{
    int err_code = 0;
    unsigned int data_out;

    HW32_REG(addr_base + 0x4) = 0x12345678;
    HW32_REG(addr_base + 0xc00) = 0x50060001;   //configure PES register to chose channel 6 for GPIOA GPIOB GPIOC GPIOD
    HW32_REG(0x40037004) = 0x00400000;      //trigger PES channel 6 output a level to GPIO

    HW32_REG(addr_base + 0x4) = 0x000000ff;
    data_out    =   HW32_REG(addr_base + 0xc00);
    if (data_out != 0x50060101)
    {
        err_code += 1;
        printk("cpu_pes conflict bit can not work\n");
    }
    HW32_REG(addr_base + 0xc00) = data_out;
    data_out = HW32_REG(addr_base + 0xc00);
    if (data_out != 0x50060001)
    {
        err_code += 1;
        printk("cpu_pes conflict status can not be cleared\n");
    }

    HW32_REG(0x40037004) = 0x00000000;
    HW32_REG(addr_base + 0x4) = 0x00000000;
    HW32_REG(addr_base + 0xc00) = 0x00000000;

    if (!err_code)
    {
        return 0;
    }
    else
    {
        return 1;
    }

}

int cpu_pes_highbyte(unsigned int addr_base)
{
    int err_code = 0;
    unsigned int data_out;

    HW32_REG(addr_base + 0x4) = 0x12345678;
    HW32_REG(addr_base + 0xc00) = 0xa0070021;   //configure PES register to chose channel 7 for GPIOA GPIOB GPIOC GPIOD
    HW32_REG(0x40037004) = 0x00800000;      //trigger PES output a level to GPIO

    HW32_REG(addr_base + 0x4) = 0x000000ff;
    data_out    =   HW32_REG(addr_base + 0xc00);
    if (data_out != 0xa0070121)
    {
        err_code += 1;
        printk("cpu_pes conflict bit can not work\n");
    }
    HW32_REG(addr_base + 0xc00) = data_out;
    data_out = HW32_REG(addr_base + 0xc00);
    if (data_out != 0xa0070021)
    {
        err_code += 1;
        printk("cpu_pes conflict status can not be cleared\n");
    }

    HW32_REG(0x40037004) = 0x00000000;
    HW32_REG(addr_base + 0x4) = 0x00000000;
    HW32_REG(addr_base + 0xc00) = 0x00000000;

    if (!err_code)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int lowbyte_clr_test(unsigned int addr_base)
{
    int err_code = 0;
    unsigned int data_out;
    uint32_t    data_mask = 0x0000ffff;

    HW32_REG(addr_base + 0x4) = 0x12345678;
    //HW32_REG(addr_base + 0xc00) = 0x50000001;      //configure PES register to chose channel 0 for GPIOA GPIOB GPIOC GPIOD
    HW32_REG(addr_base + 0xc00) = 0x50090001;      //configure PES register to chose channel 9 for GPIOA GPIOB GPIOC GPIOD
    //HW32_REG(0x40037004) = 0x00000001;      //trigger PES channel 0 output a pulse to GPIO
    HW32_REG(0x40037004) = 0x00000200;      //trigger PES channel 9 output a pulse to GPIO
    data_out = HW32_REG(addr_base + 0x4);
    if ((data_out & data_mask) != (0x12345658 & (~(1 << 5)) & data_mask))
    {
        err_code += 1;
        printk("lowbyte clear error , data = 0x%x\n", data_out);
    }
    if (!err_code)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int lowbyte_set_test(unsigned int addr_base)
{
    int err_code = 0;
    unsigned int data_out;
    uint32_t    data_mask = 0x0000ffff;

    HW32_REG(addr_base + 0x4) = 0xffffffef;
    //HW32_REG(addr_base + 0xc00) = 0x40010011;   //configure PES register to chose channel 1 for GPIOA GPIOB GPIOC GPIOD
    //HW32_REG(0x40037004) = 0x00000002;      //trigger PES channel 1 output a pulse to GPIO
    HW32_REG(addr_base + 0xc00) = 0x400a0011;   //configure PES register to chose channel 10 for GPIOA GPIOB GPIOC GPIOD
    HW32_REG(0x40037004) = 0x00000400;      //trigger PES channel 10 output a pulse to GPIO
    data_out = HW32_REG(addr_base + 0x4);
    if ((data_out & data_mask) != ((0xffffffef | (1 << 4)) & data_mask))
    {
        err_code += 1;
        printk("lowbyte set error , data = 0x%x\n", data_out);
    }
    if (!err_code)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int lowbyte_toggle_test(unsigned int addr_base)
{
    int err_code = 0;
    unsigned int data_out;
    uint32_t    data_mask = 0x0000ffff;

    HW32_REG(addr_base + 0x4) = 0xffffffff;
    //HW32_REG(addr_base + 0xc00) = 0x70020021;   //configure PES register to chose channel 2 for GPIOA GPIOB GPIOC GPIOD
    //HW32_REG(0x40037004) = 0x00000004;      //trigger PES channel 2  output a pulse to GPIO
    HW32_REG(addr_base + 0xc00) = 0x700b0021;   //configure PES register to chose channel 11 for GPIOA GPIOB GPIOC GPIOD
    HW32_REG(0x40037004) = 0x00000800;      //trigger PES channel 11 output a pulse to GPIO
    data_out = HW32_REG(addr_base + 0x4);
    if ((data_out & data_mask) != ((0xffffffff ^ (1 << 7)) & data_mask))
    {
        err_code += 1;
        printk("lowbyte toggle error , data = 0x%x\n", data_out);
    }
    if (!err_code)
    {
        return 0;
    }
    else
    {
        return 1;
    }

}

int highbyte_clr_test(unsigned int addr_base)
{
    int err_code = 0;
    unsigned int data_out;
    uint32_t    data_mask = 0x0000ffff;

    HW32_REG(addr_base + 0x4) = 0xffffffff;
    HW32_REG(addr_base + 0xc00) = 0xa0030001;   //configure PES register to chose channel 3 for GPIOA GPIOB GPIOC GPIOD
    HW32_REG(0x40037004) = 0x00000008;      //trigger PES channel 3 output a pulse to GPIO
    data_out = HW32_REG(addr_base + 0x4);
    if ((data_out & data_mask) != ((0xffffffff & (~(1 << 10))) & data_mask))
    {
        err_code += 1;
        printk("highbyte clear error , data = 0x%x\n", data_out);
    }
    if (!err_code)
    {
        return 0;
    }
    else
    {
        return 1;
    }

}

int highbyte_set_test(unsigned int addr_base)
{
    int err_code = 0;
    unsigned int data_out;
    uint32_t    data_mask = 0x0000ffff;

    HW32_REG(addr_base + 0x4) = 0x00000000;
    HW32_REG(addr_base + 0xc00) = 0x90040011;   //configure PES register to chose channel 4 for GPIOA GPIOB GPIOC GPIOD
    HW32_REG(0x40037004) = 0x00000010;      //trigger PES output a pulse to GPIO
    data_out = HW32_REG(addr_base + 0x4);
    if ((data_out & data_mask) != ((0x00000000 | (1 << 9)) & data_mask))
    {
        err_code += 1;
        printf("highbyte set error , data = 0x%x\n", data_out);
    }
    if (!err_code)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int highbyte_toggle_test(unsigned int addr_base)
{
    int err_code = 0;
    unsigned int data_out;
    uint32_t    data_mask = 0x00001fff;

    HW32_REG(addr_base + 0x4) = 0x00000000;
    HW32_REG(addr_base + 0xc00) = 0xb0050021;   //configure PES register to chose channel 5 for GPIOA GPIOB GPIOC GPIOD
    HW32_REG(0x40037004) = 0x00000020;      //trigger PES output a pulse to GPIO
    data_out = HW32_REG(addr_base + 0x4);
    if ((data_out & data_mask) != ((0x00000000 | (1 << 11)) & data_mask))
    {
        err_code += 1;
        printf("highbyte toggle error , data = 0x%x\n", data_out);
    }
    if (!err_code)
    {
        return 0;
    }
    else
    {
        return 1;
    }

}

void gpio_demo(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;
    uint8_t i = 0;
    unsigned int addr1, addr2;

    unsigned int s_gpio_base[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, GPIOE_BASE, GPIOF_BASE, GPIOG_BASE, GPIOH_BASE, GPIOI_BASE, GPIOJ_BASE, GPIOK_BASE};
    char *log[12] = {"GPIOA", "GPIOB", "GPIOC", "GPIOD", "GPIOE", "GPIOF", "GPIOG", "GPIOH", "GPIOI", "GPIOJ", "GPIOK"};
    int index = 0;

    patch_config();
    gpio_clk_enable();
    gpio_reset_release();
    index = 0;
    i = 0;
    ret = RET_OK;
    addr1 = 0;
    addr2 = 0;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

#if defined(TEST_CASE_1)
    printf("gpio output unit test begin \r\n");

    while (index < sizeof(s_gpio_base) / sizeof(s_gpio_base[0]))
    {
        for (i = 0; i < sizeof(s_gpio_base) / sizeof(unsigned int); i++)
        {
            if (s_gpio_base[index] == GPIOA_BASE)
            {
                /*if ((i==5) || (i == 6) || (i==7) || (i==8))
                    continue;*/
            }
            else if (s_gpio_base[index] == GPIOE_BASE)
            {
                if ((i == 5) || (i == 7)) /* uart3*/
                {
                    continue;
                }
            }
            else if (s_gpio_base[index] == GPIOH_BASE)
            {
                if ((i == 6) || (i == 14)) /* X-JTMS_SWDIO ,X-JTCK_SWCLK*/
                {
                    continue;
                }
            }
            GPIO_Output_testcase((GPIO_Device_T *)s_gpio_base[index], (1 << i));

        }
        printf("gpio output unit %s OK! gpio_base=[0x%x] \r\n", log[index], s_gpio_base[index]);
        index++;
    }

    printf("gpio output unit  end \r\n");

#endif

#if defined(TEST_CASE_2)
    printf("gpio input unit test\r\n");

#if 1//def GPIOA_AND_GPIOB
    for (i = 0; i < 16; i++)
    {
        if ((i == 5) || (i == 6) || (i == 7) || (i == 8) || (i == 9) || (i == 10) || (i == 11)) /* XIN ,XOUT*/
        {
            continue;
        }
        GPIO_Output_testcase(GPIOA, (1 << i));
    }

    for (i = 0; i < 16; i++)
    {
        if ((i == 5) || (i == 6) || (i == 7) || (i == 8) || (i == 9) || (i == 10) || (i == 11)) /* XIN ,XOUT*/
        {
            continue;
        }
        GPIO_Input_testcase(GPIOB, (1 << i), GPIO_PIN_SET);
    }

    printf("gpio GPIOA output and GPIOB input unit GPIOK end \r\n");

    ret = HAL_GPIO_DeInit(GPIOA, GPIO_PIN_All, 0);
    ret = HAL_GPIO_DeInit(GPIOB, GPIO_PIN_All, 0);

    for (i = 0; i < 16; i++)
    {
        if ((i == 5) || (i == 6) || (i == 7) || (i == 8) || (i == 9) || (i == 10) || (i == 11)) /* XIN ,XOUT*/
        {
            continue;
        }
        GPIO_Output_testcase(GPIOB, (1 << i));
    }

    for (i = 0; i < 16; i++)
    {
        if ((i == 5) || (i == 6) || (i == 7) || (i == 8) || (i == 9) || (i == 10) || (i == 11)) /* XIN ,XOUT*/
        {
            continue;
        }
        GPIO_Input_testcase(GPIOA, (1 << i), GPIO_PIN_SET);
    }
    printf("gpio GPIOB output and GPIOA input unit end \r\n");
#endif

#ifdef GPIOC_AND_GPIOD

    for (i = 0; i < 16; i++)
    {
        if ((i == 2) || (i == 3) || (i == 4) || (i == 5) || (i == 6) || (i == 7) || (i == 8) || (i == 9) || (i == 10) || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Output_testcase(GPIOC, (1 << i));
    }

    for (i = 0; i < 16; i++)
    {
        if ((i == 2) || (i == 3) || (i == 4) || (i == 5) || (i == 6) || (i == 7) || (i == 8) || (i == 9) || (i == 10) || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Input_testcase(GPIOD, (1 << i), GPIO_PIN_SET);
    }

    printf("gpio GPIOC output and GPIOD input unit GPIOK end \r\n");

    ret = HAL_GPIO_DeInit(GPIOC, GPIO_PIN_All, 0);
    ret = HAL_GPIO_DeInit(GPIOD, GPIO_PIN_All, 0);

    for (i = 0; i < 16; i++)
    {
        if ((i == 2) || (i == 3) || (i == 4) || (i == 5) || (i == 6) || (i == 7) || (i == 8) || (i == 9) || (i == 10) || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Output_testcase(GPIOD, (1 << i));
    }

    for (i = 0; i < 16; i++)
    {
        if ((i == 2) || (i == 3) || (i == 4) || (i == 5) || (i == 6) || (i == 7) || (i == 8) || (i == 9) || (i == 10) || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Input_testcase(GPIOC, (1 << i), GPIO_PIN_SET);
    }
    printf("gpio GPIOD output and GPIOC input unit end \r\n");
#endif

#ifdef GPIOI_AND_GPIOJ
    addr1 = GPIOI;
    addr2 = GPIOJ;

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 2) || (i == 4) || (i == 6)  || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Output_testcase(addr1, (1 << i));
    }

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 2) || (i == 4) || (i == 6)  || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Input_testcase(addr2, (1 << i), GPIO_PIN_SET);
    }

    printf("gpio GPIOI output and GPIOJ input unit GPIOK end \r\n");

    ret = HAL_GPIO_DeInit(addr1, GPIO_PIN_All, 0);
    ret = HAL_GPIO_DeInit(addr2, GPIO_PIN_All, 0);

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 2) || (i == 4) || (i == 6)  || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Output_testcase(addr2, (1 << i));
    }

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 2) || (i == 4) || (i == 6)  || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Input_testcase(addr1, (1 << i), GPIO_PIN_SET);
    }
    printf("gpio GPIOJ output and GPIOI input unit end \r\n");
#endif

#ifdef GPIOE_AND_GPIOF
    addr1 = GPIOE;
    addr2 = GPIOF;

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 1) || (i == 3) || (i == 5) || (i == 7)  || (i == 13) || (i == 14) || (i == 15))
        {
            continue;
        }
        GPIO_Output_testcase(addr1, (1 << i));
    }

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 1) || (i == 3) || (i == 5) || (i == 7)  || (i == 13) || (i == 14) || (i == 15))
        {
            continue;
        }
        GPIO_Input_testcase(addr2, (1 << i), GPIO_PIN_SET);
    }

    printf("gpio GPIOE output and GPIOF input unit GPIOK end \r\n");

    ret = HAL_GPIO_DeInit(addr1, GPIO_PIN_All & (~GPIO_PIN_5) & (~GPIO_PIN_7), 0);
    ret = HAL_GPIO_DeInit(addr2, GPIO_PIN_All & (~GPIO_PIN_5) & (~GPIO_PIN_7), 0);

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 1) || (i == 3) || (i == 5) || (i == 7)  || (i == 13) || (i == 14) || (i == 15))
        {
            continue;
        }
        GPIO_Output_testcase(addr2, (1 << i));
    }

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 1) || (i == 3) || (i == 5) || (i == 7)  || (i == 13) || (i == 14) || (i == 15))
        {
            continue;
        }
        GPIO_Input_testcase(addr1, (1 << i), GPIO_PIN_SET);
    }
    printf("gpio GPIOF output and GPIOE input unit end \r\n");
#endif

#ifdef GPIOG_AND_GPIOH
    addr1 = GPIOG;
    addr2 = GPIOH;

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 2) || (i == 4) || (i == 6)  || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Output_testcase(addr1, (1 << i));
    }

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 2) || (i == 4) || (i == 6)  || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Input_testcase(addr2, (1 << i), GPIO_PIN_SET);
    }

    printf("gpio GPIOG output and GPIOH input unit end \r\n");

    ret = HAL_GPIO_DeInit(addr1, GPIO_PIN_All, 0);
    ret = HAL_GPIO_DeInit(addr2, GPIO_PIN_All, 0);

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 2) || (i == 4) || (i == 6)  || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Output_testcase(addr2, (1 << i));
    }

    for (i = 0; i < 16; i++)
    {
        if ((i == 0) || (i == 2) || (i == 4) || (i == 6)  || (i == 12) || (i == 14))
        {
            continue;
        }
        GPIO_Input_testcase(addr1, (1 << i), GPIO_PIN_SET);
    }
    printf("gpio GPIOH output and GPIOG input unit end \r\n");
#endif

#endif

#ifdef TEST_CASE_3
    gpio_interrpute_demo();
#endif

#ifdef TEST_CASE_4
    gpio_filter_demo();
#endif

#if defined(TEST_CASE_5)

    int m = 0;
    unsigned int gpio_addr[] = {GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, GPIOE_BASE, GPIOF_BASE, GPIOG_BASE, GPIOH_BASE, GPIOI_BASE, GPIOJ_BASE, GPIOK_BASE};
    printf("gpio bitband unit test \r\n");
    GPIO_InitStruct.pin = GPIO_PIN_All;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;

    while (m < 11)
    {
        if (m == 4 || m == 7 || m == 10)
        {
            if (m == 4)
            {
                GPIO_InitStruct.pin = GPIO_PIN_All & (~GPIO_PIN_5) & (~GPIO_PIN_7);
            }
            else if (m == 7)
            {
                GPIO_InitStruct.pin = GPIO_PIN_All & (~GPIO_PIN_6) & (~GPIO_PIN_14);
            }
            else
            {
                GPIO_InitStruct.pin = GPIO_PIN_0 | GPIO_PIN_1 |  GPIO_PIN_2 |  GPIO_PIN_3 \
                                      |  GPIO_PIN_4 |  GPIO_PIN_5 |  GPIO_PIN_6 |  GPIO_PIN_7 | GPIO_PIN_8 ;
            }
        }
        else
        {
            GPIO_InitStruct.pin = GPIO_PIN_All;
        }

        ret = HAL_GPIO_Init((GPIO_Device_T *)gpio_addr[m], &GPIO_InitStruct);

        for (i = 0; i < 16; i++)
        {
            if (m == 7)
            {
                if ((i == 6) || (i == 14)) /* X-JTMS_SWDIO ,X-JTCK_SWCLK, GPIOH*/
                {
                    continue;
                }
            }
            if (m == 4)
            {
                if ((i == 5) || (i == 7)) /*  GPIOE*/
                {
                    continue;
                }
            }
            if (m == 10)
            {
                if (i > 8)  /*  GPIOK*/
                {
                    break;
                }
            }

            MEMADDR(BITBAND(gpio_addr[m], i)) = 0x1;

            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR((i + m * 16))) != GPIO_PIN_SET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, addr=0x%x,  output high level unit test fail!\n", gpio_addr[m], i, (unsigned int)GPIO_BIT_ADDR((i + m * 16)));
            }

            SysTick_DelayMS(10);
            MEMADDR(BITBAND(gpio_addr[m], i)) = 0x0;

            if (HAL_GPIO_PinRead(GPIO_BIT_ADDR((i + m * 16))) != GPIO_PIN_RESET)
            {
                printf("gpio baseaddr: 0x%x, pin num: 0x%x, addr=0x%x,  output low level unit test fail!\n", gpio_addr[m], i, (unsigned int)GPIO_BIT_ADDR((i + m * 16)));
            }
        }
        m++;
    }

    printf("gpio bitband unit test end");
#endif

#ifdef TEST_CASE_RESPOND_PES_6
    pes_triger_gpio();
    return ;
#endif

}

void main(void)
{
    int i = 0;

    printk("SRAM BringUp:Hello World! %s\n", CONFIG_ARCH);

    gpio_demo();

    while (1)
    {
        printk("\n GPIO ES TEST DEMO %d \n", i);
        k_sleep(2000);
        i++;
    }
}
