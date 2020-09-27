

/*
 * Copyright (c) 2016 Linaro Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <kernel.h>
#include <device.h>
#include <init.h>
#include <soc.h>

#ifdef CONFIG_GPIO_KEY
    #include <input.h>
    #include<key/gpio_key.h>

#endif


#ifdef CONFIG_GPIO_KEY
/**
 * one relly board need have one borad.c(soc maybe the same or different soc)
 * creat board key: differernt board have different board resource
 */

static struct gpio_keys_button key_button[]  =
{
    {
        .gpio_port      = "GPIOA",              /* KEYUP */
        .pin            = 0,
        .pullup         = false,
        .code           = KEY_UP,
        .press_level    = 1,
        .desc           = "keyup"
    },
    {
        .gpio_port      = "GPIOH",                /* k0 */
        .pin            = 3,
        .pullup         = true,
        .code           = KEY_0,
        .press_level    = 0,
        .desc           = "key0"

    },

    {
        .gpio_port      = "GPIOH",                /* k1 */
        .pin            = 2,
        .pullup         = true,
        .code           = KEY_1,
        .press_level    = 0,
        .desc           = "key1"

    },


    {
        .gpio_port      = "GPIOC",                /* k2 */
        .pin            = 13,
        .pullup         = true,
        .code           = KEY_2,
        .press_level    = 0,
        .desc           = "key2"

    },

};


static struct gpio_keys_button_platform_data platform_data =
{
    .share_delay   = 25, /*ms*/
    .nr            = ARRAY_SIZE(key_button),
    .buttons_table = (struct gpio_keys_button *)(&key_button),
};

#endif

static int board_resource_init()
{
#ifdef CONFIG_GPIO_KEY
    register_button_platform_data(&platform_data);
#endif
    return 0;
}


SYS_INIT(board_resource_init, PRE_KERNEL_2,
         CONFIG_KERNEL_INIT_PRIORITY_DEVICE);



