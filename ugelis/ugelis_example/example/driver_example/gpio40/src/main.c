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
 * @date        2019/03/27
 *
 * @brief       GPIO Demo Calls GPIO Driver
 *
 * @note
 *              2019/03/27, Richard.Liu, v1.0.0
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

#define     HW32_REG(ADDRESS)   (*((volatile uint32_t *)(ADDRESS)))

#define OUTPUT0_GPIO_PIN   0
#define OUTPUT1_GPIO_PIN   1
#define OUTPUT2_GPIO_PIN   2

#define SW0_INPUT_GPIO_PIN   0
#define SW1_INPUT_GPIO_PIN   1

#define PORT    "GPIOA"

void patch_config(void) /*FIX HIRC UART PRINT ISSUE*/
{
    HW32_REG(0x1ffff0a8) = 0xfffffbac;
    HW32_REG(0x1ffff0ac) = 0xff1fffff;
    HW32_REG(0x1ffff0b0) = 0xff3fffff;
}

void gpio_output_demo(void)
{
    struct device *dev;

    dev = device_get_binding(PORT);

    gpio_pin_configure(dev, OUTPUT0_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_configure(dev, OUTPUT1_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_configure(dev, OUTPUT2_GPIO_PIN, GPIO_MODE_OUTPUT);

    while (1)
    {
        gpio_pin_write(dev, OUTPUT0_GPIO_PIN, GPIO_PIN_RESET);
        gpio_pin_write(dev, OUTPUT1_GPIO_PIN, GPIO_PIN_RESET);
        gpio_pin_write(dev, OUTPUT2_GPIO_PIN, GPIO_PIN_RESET);

        printk("Run Func[%s]\n", __FUNCTION__);
        k_sleep(1000);

        gpio_pin_write(dev, OUTPUT0_GPIO_PIN, GPIO_PIN_SET);
        gpio_pin_write(dev, OUTPUT1_GPIO_PIN, GPIO_PIN_SET);
        gpio_pin_write(dev, OUTPUT2_GPIO_PIN, GPIO_PIN_SET);

        k_sleep(1000);
    }
}

void wifi_gpio(void)
{
    struct device *PB, *PC, *PE, *PF, *PJ;

    printk("Run Func[%s]\n", __FUNCTION__);

    PB = device_get_binding("GPIOB");
    PC = device_get_binding("GPIOC");
    PE = device_get_binding("GPIOE");
    PF = device_get_binding("GPIOF");
    PJ = device_get_binding("GPIOJ");

    gpio_pin_configure(PC, 5, GPIO_MODE_INPUT);
    gpio_pin_configure(PJ, 1, GPIO_MODE_INPUT);
    gpio_pin_configure(PJ, 2, GPIO_MODE_INPUT);
    gpio_pin_configure(PJ, 3, GPIO_MODE_INPUT);
    gpio_pin_configure(PJ, 4, GPIO_MODE_INPUT);

    gpio_pin_configure(PB, 15, GPIO_MODE_OUTPUT);
    gpio_pin_write(PB, 15, GPIO_PIN_SET);
}

void gpio40_demo(void)
{
    int val ;
    struct device *PB, *PC, *PE, *PF;

    printk("Run Func[%s]\n", __FUNCTION__);

    PB = device_get_binding("GPIOB");
    PC = device_get_binding("GPIOC");
    PE = device_get_binding("GPIOE");
    PF = device_get_binding("GPIOF");

    gpio_pin_configure(PB, 13, GPIO_MODE_INPUT);
    gpio_pin_configure(PC, 04, GPIO_MODE_INPUT);
    //gpio_pin_configure(PB, 15, GPIO_MODE_INPUT);

    gpio_pin_configure(PB, 10, GPIO_MODE_OUTPUT);

    gpio_pin_configure(PE, 2, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE, 1, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE, 9, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PB, 5, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PB, 7, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE,10, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PC,11, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PC,10, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PC, 9, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE, 4, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE,15, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PF, 0, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PF, 2, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE, 8, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PF, 4, GPIO_MODE_OUTPUT);

    gpio_pin_configure(PC, 2, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PC, 3, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE, 7, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE,11, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE,12, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE,13, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PC, 8, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE,14, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE, 3, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PF, 1, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PF, 3, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE, 6, GPIO_MODE_OUTPUT);
    gpio_pin_configure(PE, 5, GPIO_MODE_OUTPUT);

    while (1)
    {
        printk("LED ON\n");
	gpio_pin_write(PB,10, GPIO_PIN_RESET);

        gpio_pin_read(PB, 13, &val);
        printk("WKU_KEY = [%d]\n", val);
        gpio_pin_read(PC, 04, &val);
        printk("VBAT-DET = [%d]\n", val);
        //gpio_pin_read(PB, 15, &val);
        //printk("XPB15 = [%d]\n", val);

	gpio_pin_write(PE, 2, GPIO_PIN_SET);
	gpio_pin_write(PE, 1, GPIO_PIN_SET);
	gpio_pin_write(PE, 9, GPIO_PIN_SET);
	gpio_pin_write(PB, 5, GPIO_PIN_SET);
	gpio_pin_write(PB, 7, GPIO_PIN_SET);
	gpio_pin_write(PE,10, GPIO_PIN_SET);
	gpio_pin_write(PC,11, GPIO_PIN_SET);
	gpio_pin_write(PC,10, GPIO_PIN_SET);
	gpio_pin_write(PC, 9, GPIO_PIN_SET);
	gpio_pin_write(PE, 4, GPIO_PIN_SET);
	gpio_pin_write(PE,15, GPIO_PIN_SET);
	gpio_pin_write(PF, 0, GPIO_PIN_SET);
	gpio_pin_write(PF, 2, GPIO_PIN_SET);
	gpio_pin_write(PE, 8, GPIO_PIN_SET);
	gpio_pin_write(PF, 4, GPIO_PIN_SET);

	gpio_pin_write(PC, 2, GPIO_PIN_SET);
	gpio_pin_write(PC, 3, GPIO_PIN_SET);
	gpio_pin_write(PE, 7, GPIO_PIN_SET);
	gpio_pin_write(PE,11, GPIO_PIN_SET);
	gpio_pin_write(PE,12, GPIO_PIN_SET);
	gpio_pin_write(PE,13, GPIO_PIN_SET);
	gpio_pin_write(PC, 8, GPIO_PIN_SET);
	gpio_pin_write(PE,14, GPIO_PIN_SET);
	gpio_pin_write(PE, 3, GPIO_PIN_SET);
	gpio_pin_write(PF, 1, GPIO_PIN_SET);
	gpio_pin_write(PF, 3, GPIO_PIN_SET);
	gpio_pin_write(PE, 6, GPIO_PIN_SET);
	gpio_pin_write(PE, 5, GPIO_PIN_SET);

        k_sleep(1000);

        printk("LED OFF\n");
	gpio_pin_write(PB,10, GPIO_PIN_SET);

	gpio_pin_write(PE, 2, GPIO_PIN_RESET);
	gpio_pin_write(PE, 1, GPIO_PIN_RESET);
	gpio_pin_write(PE, 9, GPIO_PIN_RESET);
	gpio_pin_write(PB, 5, GPIO_PIN_RESET);
	gpio_pin_write(PB, 7, GPIO_PIN_RESET);
	gpio_pin_write(PE,10, GPIO_PIN_RESET);
	gpio_pin_write(PC,11, GPIO_PIN_RESET);
	gpio_pin_write(PC,10, GPIO_PIN_RESET);
	gpio_pin_write(PC, 9, GPIO_PIN_RESET);
	gpio_pin_write(PE, 4, GPIO_PIN_RESET);
	gpio_pin_write(PE,15, GPIO_PIN_RESET);
	gpio_pin_write(PF, 0, GPIO_PIN_RESET);
	gpio_pin_write(PF, 2, GPIO_PIN_RESET);
	gpio_pin_write(PE, 8, GPIO_PIN_RESET);
	gpio_pin_write(PF, 4, GPIO_PIN_RESET);

	gpio_pin_write(PC, 2, GPIO_PIN_RESET);
	gpio_pin_write(PC, 3, GPIO_PIN_RESET);
	gpio_pin_write(PE, 7, GPIO_PIN_RESET);
	gpio_pin_write(PE,11, GPIO_PIN_RESET);
	gpio_pin_write(PE,12, GPIO_PIN_RESET);
	gpio_pin_write(PE,13, GPIO_PIN_RESET);
	gpio_pin_write(PC, 8, GPIO_PIN_RESET);
	gpio_pin_write(PE,14, GPIO_PIN_RESET);
	gpio_pin_write(PE, 3, GPIO_PIN_RESET);
	gpio_pin_write(PF, 1, GPIO_PIN_RESET);
	gpio_pin_write(PF, 3, GPIO_PIN_RESET);
	gpio_pin_write(PE, 6, GPIO_PIN_RESET);
	gpio_pin_write(PE, 5, GPIO_PIN_RESET);

        k_sleep(1000);
    }
}

/*Using wire connect input pin and output pin*/
void gpio_input_demo(void)
{
    int val0 = 0;
    int val1 = 0;
    int val2 = 0;
    struct device *dev;

    dev = device_get_binding(PORT);

    gpio_pin_configure(dev, SW0_INPUT_GPIO_PIN, GPIO_MODE_INPUT);
    gpio_pin_configure(dev, SW1_INPUT_GPIO_PIN, GPIO_MODE_INPUT);

    gpio_pin_configure(dev, OUTPUT2_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_write(dev, OUTPUT2_GPIO_PIN, GPIO_PIN_SET);

    while (1)
    {
        gpio_pin_read(dev, SW0_INPUT_GPIO_PIN, &val0);
        printk("key0 val = [%d]\n", val0);
        gpio_pin_read(dev, SW1_INPUT_GPIO_PIN, &val1);
        printk("key1 val = [%d]\n", val1);
        gpio_pin_read(dev, OUTPUT2_GPIO_PIN, &val2);
        printk("output val = [%d]\n", val2);
        k_sleep(1000);
    }

}

void button_pressed(struct device *gpiob, struct gpio_callback *cb,
                    uint32_t pins)
{
    printk("Button pressed\n");
}

void gpio_interrput_demo(void)
{
    int val ;
    struct device *dev;
    static struct gpio_callback gpio_cb;

    dev = device_get_binding(PORT);
    gpio_pin_configure(dev, OUTPUT2_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_pin_write(dev, OUTPUT2_GPIO_PIN, GPIO_PIN_SET);

    /* Set key pin as interrupt active flalling mode */
    gpio_pin_configure(dev, SW0_INPUT_GPIO_PIN, GPIO_INT | GPIO_PUD_PULL_DOWN | GPIO_MODE_IT_RISING);
    gpio_init_callback(&gpio_cb, button_pressed, BIT(SW0_INPUT_GPIO_PIN));

    gpio_add_callback(dev, &gpio_cb);
    gpio_pin_enable_callback(dev, SW0_INPUT_GPIO_PIN);

    while (1)
    {
        gpio_pin_read(dev, SW0_INPUT_GPIO_PIN, &val);
        printk("key value = [%d]\n", val);
        k_sleep(1000);
    }
}

void main(void)
{
    patch_config();

    printk("SRAM BringUp:GPIO40 DEMO! %s (20200707 V0.4)\n", CONFIG_ARCH);
    /*gpio_output_demo();*/
    /*gpio_input_demo();*/
    //gpio_interrput_demo();
    wifi_gpio();
    gpio40_demo();
}
