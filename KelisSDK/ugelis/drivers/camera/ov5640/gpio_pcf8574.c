 /*
  * opyright (C) 2016 Gree Microelectronics. 
  *
  * SPDX-License-Identifier: Apache-2.0
  *
  * Driver for pcf8574 I2C GPIO expanders
  */
#include <errno.h>
#include <kernel.h>
#include <ugelis.h>
      
#include <arch/cpu.h>
#include <misc/__assert.h>
#include <board.h>
#include <init.h>
#include <gpio.h>
#include <i2c_gpio.h>

#include "gpio_pcf8574.h"

#define PCF8574_ADDR 	0x40

struct device *i2c;

static uint8_t pcf8574_readbyte(void)
{
    uint8_t buf;
    
    i2c_read(i2c, &buf, 1, PCF8574_ADDR);
    
	return buf;
}

static void pcf8574_writebyte(uint8_t data)
{
    uint8_t buf[2];

    buf[0] = data;

    i2c_write(i2c, buf, 1, PCF8574_ADDR);	 	 
}

void pcf8574_write(uint8_t bit, uint8_t status)
{
    uint8_t data;
    
    data = pcf8574_readbyte(); 
    if(status == 0)
    {
        data &= ~(1<<bit); 
    }
    else 
    {
        data |= 1<<bit;
    }
    
    pcf8574_writebyte(data); 
} 

uint8_t pcf8574_init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();           
    
    GPIO_Initure.Pin = GPIO_PIN_12;           
    GPIO_Initure.Mode = GPIO_MODE_INPUT;      
    GPIO_Initure.Pull = GPIO_PULLUP;          
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    i2c = device_get_binding(CONFIG_I2C_GPIO_1_NAME);
	
    i2c_configure(i2c, I2C_SPEED_SET(I2C_SPEED_FAST)|I2C_MODE_MASTER);
    
    pcf8574_writebyte(0XFF);	 
    
	return 0;
}

