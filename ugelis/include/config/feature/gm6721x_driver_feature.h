#ifndef __GM6721_DRIVER_FEATURE_H__
#define __GM6721_DRIVER_FEATURE_H__
/********************************************/
/* HALs */
/********************************************/
#define CONFIG_HAS_CMSIS            1
#define CONFIG_LIBMETAL_SRC_PATH    "libmetal"
#define CONFIG_HAS_GM_HAL           1
#define CONFIG_GM_HAL_BUILTIN       1
#define CONFIG_GM_HAL_CPU           1
#define CONFIG_GM_HAL_CACHE         1
#define CONFIG_GM_HAL_DMA           1
#define CONFIG_GM_HAL_GPIO          1
#define CONFIG_GM_HAL_CLOCK         1
#define CONFIG_GM_HAL_RESET         1

/********************************************/
/* Timer Drivers */
/********************************************/
#define CONFIG_CORTEX_M_SYSTICK 1
#define CONFIG_SYSTEM_CLOCK_INIT_PRIORITY 0
#define CONFIG_CLOCK_CONTROL 1

/********************************************/
/* gm6721 system clock setting */
/********************************************/

/********************************************/
/* *****************************************************You can only choose one of the following two options!!!*********************************************** */
/********************************************/
#define CONFIG_USE_INTERNAL_CLOCK 1
#define CONFIG_SYSCLK_PLLKDP150_BY_HIRC 1
#define CONFIG_SYSCLK_PRESCALER 0


#define CONFIG_PM
/*config powermode*/
#ifdef CONFIG_PM
    #define CONFIG_POWER_GM 1
    #define CONFIG_GM_HAL_POWER 1
    #define CONFIG_DEVICE_POWER_MANAGEMENT 1
#endif

#define CONFIG_GPIO
/*config gpio*/
#ifdef CONFIG_GPIO
    #define CONFIG_GPIO_GM 1
    #define CONFIG_GPIO_GM_A 1
    #define CONFIG_GPIO_GM_A_NAME "GPIOA"
    #define CONFIG_GPIO_GM_A_IRQ_PRI 1
    #define CONFIG_GPIO_GM_B 1
    #define CONFIG_GPIO_GM_B_NAME "GPIOB"
    #define CONFIG_GPIO_GM_B_IRQ_PRI 1
    #define CONFIG_GPIO_GM_C 1
    #define CONFIG_GPIO_GM_C_NAME "GPIOC"
    #define CONFIG_GPIO_GM_C_IRQ_PRI 1
    #define CONFIG_GPIO_GM_D 1
    #define CONFIG_GPIO_GM_D_NAME "GPIOD"
    #define CONFIG_GPIO_GM_D_IRQ_PRI 1
    #define CONFIG_GPIO_GM_E 1
    #define CONFIG_GPIO_GM_E_NAME "GPIOE"
    #define CONFIG_GPIO_GM_E_IRQ_PRI 1
    #define CONFIG_GPIO_GM_F 1
    #define CONFIG_GPIO_GM_F_NAME "GPIOF"
    #define CONFIG_GPIO_GM_F_IRQ_PRI 1
    #define CONFIG_GPIO_GM_G 1
    #define CONFIG_GPIO_GM_G_NAME "GPIOG"
    #define CONFIG_GPIO_GM_G_IRQ_PRI 1
    #define CONFIG_GPIO_GM_H 1
    #define CONFIG_GPIO_GM_H_NAME "GPIOH"
    #define CONFIG_GPIO_GM_H_IRQ_PRI 1
    #define CONFIG_GPIO_GM_I 1
    #define CONFIG_GPIO_GM_I_NAME "GPIOI"
    #define CONFIG_GPIO_GM_I_IRQ_PRI 1
    #define CONFIG_GPIO_GM_J 1
    #define CONFIG_GPIO_GM_J_NAME "GPIOJ"
    #define CONFIG_GPIO_GM_J_IRQ_PRI 1
    #define CONFIG_GPIO_GM_K 1
    #define CONFIG_GPIO_GM_K_NAME "GPIOK"
    #define CONFIG_GPIO_GM_K_IRQ_PRI 1
#endif

#define CONFIG_PINMUX
/*config pinmux*/
#ifdef CONFIG_PINMUX
    #define CONFIG_PINMUX_NAME "PINMUX"
    #define CONFIG_PINMUX_INIT_PRIORITY 45
    #define CONFIG_PINMUX_GM 1
    #define CONFIG_PINMUX_GM_PORTA 1
    #define CONFIG_PINMUX_GM_PORTA_NAME "porta"
    #define CONFIG_PINMUX_GM_PORTB 1
    #define CONFIG_PINMUX_GM_PORTB_NAME "portb"
    #define CONFIG_PINMUX_GM_PORTC 1
    #define CONFIG_PINMUX_GM_PORTC_NAME "portc"
    #define CONFIG_PINMUX_GM_PORTD 1
    #define CONFIG_PINMUX_GM_PORTD_NAME "portd"
    #define CONFIG_PINMUX_GM_PORTE 1
    #define CONFIG_PINMUX_GM_PORTE_NAME "porte"
    #define CONFIG_PINMUX_GM_PORTF 1
    #define CONFIG_PINMUX_GM_PORTF_NAME "portf"
    #define CONFIG_PINMUX_GM_PORTG 1
    #define CONFIG_PINMUX_GM_PORTG_NAME "portg"
    #define CONFIG_PINMUX_GM_PORTH 1
    #define CONFIG_PINMUX_GM_PORTH_NAME "porth"
    #define CONFIG_PINMUX_GM_PORTI 1
    #define CONFIG_PINMUX_GM_PORTI_NAME "porti"
    #define CONFIG_PINMUX_GM_PORTJ 1
    #define CONFIG_PINMUX_GM_PORTJ_NAME "portj"
    #define CONFIG_PINMUX_GM_PORTK 1
    #define CONFIG_PINMUX_GM_PORTK_NAME "portk"
#endif

/*config adc*/
#ifdef CONFIG_ADC
    #define CONFIG_SYS_LOG_ADC_LEVEL 0
    #define CONFIG_ADC_INIT_PRIORITY 80
    #define CONFIG_ADC_0 1
    #define CONFIG_ADC_0_NAME "ADC_0"
    #define CONFIG_ADC_0_IRQ_PRI 2
    #define CONFIG_ADC_1 1
    #define CONFIG_ADC_1_NAME "ADC_1"
    #define CONFIG_ADC_1_IRQ_PRI 2
    #define CONFIG_ADC_2 1
    #define CONFIG_ADC_2_NAME "ADC_2"
    #define CONFIG_ADC_2_IRQ_PRI 2
    #define CONFIG_ADC_GM 1
    #define CONFIG_ADC_GM_CALIBRATION 1
    #define CONFIG_ADC_GM_DUMMY_CONVERSION 1
    #define CONFIG_ADC_GM_SERIAL 1
    #define CONFIG_ADC_GM_SINGLESHOT 1
    #define CONFIG_ADC_GM_RISING_EDGE 1
    #define CONFIG_ADC_GM_SAMPLE_WIDTH 31
    #define CONFIG_ADC_GM_SERIAL_DELAY 1
    #define CONFIG_ADC_GM_CLOCK_RATIO 1024
#endif

/*config dac*/
#ifdef CONFIG_DAC
    #define CONFIG_DAC_GM   1
    #define CONFIG_DAC_0    1
    #define CONFIG_DAC_1    1
    #define CONFIG_DAC_0_NAME "DAC_0"
    #define CONFIG_DAC_1_NAME "DAC_1"
#endif

/*config acmp*/
#ifdef CONFIG_ACMP
    #define CONFIG_ACMP_GM  1
    #define CONFIG_ACMP_0_NAME  "ACMP_0"
#endif

#ifdef CONFIG_CAN
    #define CONFIG_GM_HAL_CAN  =  1
    #define CONFIG_CAN_GM      =  1
#endif

#ifdef CONFIG_WATCHDOG
    #ifdef CONFIG_IWDG_GM
        /******************IWDG******************/
        #define CONFIG_GM_HAL_IWDG
        #define CONFIG_IWDG_GM_DEVICE_NAME "IWDG"
        #define CONFIG_IWDG_GM_PRESCALER 64
        #define CONFIG_IWDG_GM_RELOAD_COUNTER 500
    #endif /*end CONFIG_IWDG_GM*/

    /******************WWDG******************/
    #ifdef CONFIG_WWDG_GM
        #define CONFIG_GM_HAL_WWDG
        #define CONFIG_WWDG_GM_DEVICE_NAME "WWDG"
        #define CONFIG_WWDG_GM_RELOAD_COUNTER 5000
        #define CONFIG_WWDG_GM_WINDOW_VALUE 3000
        #define CONFIG_WWDG_GM_MODE 0
        #define CONFIG_WWDG_GM_WINLEN 15
        #define CONFIG_WWDG_GM_DIV_FACTOR 31
    #endif /*end CONFIG_WWDG_GM*/
#endif /*end CONFIG_WATCHDOG*/

/*config rtc*/
#ifdef CONFIG_RTC
    #define CONFIG_GM_HAL_RTC  1
    #define CONFIG_RTC_GM      1
#endif

/*config lvd*/
#ifdef CONFIG_LVD
    #define CONFIG_GM_HAL_LVD  1
    #define CONFIG_LVD_GM      1
#endif

/********************************************/
/* camera Configuration */
/********************************************/
/*config the first camera protocol*/
/*hardware IIC camera*/
#if defined(CONFIG_OV5640)||defined(CONFIG_BF3007)
    #define CONFIG_I2C_0  1
    #define CONFIG_CAMERA_0_I2C_NAME CONFIG_I2C_0_NAME
    //#define CONFIG_I2C_GPIO       //i2c gpio use
    //#define CONFIG_CAMERA_0_I2C_NAME CONFIG_I2C_GPIO_0_NAME
    #if defined(CONFIG_OV5640)
        #define CONFIG_OV5640_0 1   //to make the default camera is camera 0, and make it compatible with previous code
    #elif defined(CONFIG_BF3007)
        #define CONFIG_BF3007_0 1
    #endif
    /*gpio sccb camera*/
#elif defined(CONFIG_OV7725)||defined(CONFIG_GC0308)||defined(CONFIG_GC2145)
    #define CONFIG_SCCB_0
    #define CONFIG_CAMERA_0_SCCB_NAME   "SCCB_0"
    #if defined(CONFIG_OV7725)
        #define CONFIG_OV7725_0
    #elif defined(CONFIG_GC0308)
        #define CONFIG_GC0308_0
    #elif defined(CONFIG_GC2145)
        #define CONFIG_GC2145_0
    #endif
#endif
/*config the second camera protocol*/
/*hardware IIC camera*/
#if defined(CONFIG_OV5640_1)||defined(CONFIG_BF3007_1)
    #define CONFIG_I2C_2  1
    #define CONFIG_CAMERA_1_I2C_NAME CONFIG_I2C_2_NAME
    /*gpio sccb camera*/
#elif defined(CONFIG_OV7725_1)||defined(CONFIG_GC0308_1)||defined(CONFIG_GC2145_1)
    #define CONFIG_SCCB_1
    #define CONFIG_CAMERA_1_SCCB_NAME   "SCCB_1"
#endif

/*config dcmi selection*/
#if defined(CONFIG_OV5640_0)||defined(CONFIG_BF3007_0)||defined(CONFIG_OV7725_0)||defined(CONFIG_GC0308_0)||defined(CONFIG_GC2145_0)
    #define CONFIG_DCMI_0
#endif
#if defined(CONFIG_OV5640_1)||defined(CONFIG_BF3007_1)||defined(CONFIG_OV7725_1)||defined(CONFIG_GC0308_1)||defined(CONFIG_GC2145_1)
    #define CONFIG_DCMI_1
#endif
/*config camera kinds*/
#if defined(CONFIG_OV5640_0)||defined(CONFIG_OV5640_1)
    #define CAMERA_OV5640
#endif
#if defined(CONFIG_BF3007_0)||defined(CONFIG_BF3007_1)
    #define CAMERA_BF3007
#endif
#if defined(CONFIG_OV7725_0)||defined(CONFIG_OV7725_1)
    #define CAMERA_OV7725
#endif
#if defined(CONFIG_GC0308_0)||defined(CONFIG_GC0308_1)
    #define CAMERA_GC0308
#endif
#if defined(CONFIG_GC2145_0)||defined(CONFIG_GC2145_1)
    #define CAMERA_GC2145
#endif

/********************************************/
/* TOUCHSCREEN Configuration */
/********************************************/
#if defined(CONFIG_GT9147) || defined (CONFIG_FT5426) || defined(CONFIG_GT911)
    #define CONFIG_TOUCHSCREEN  1
    #if defined(LQFP176_DEMO_BOARD)
        #define CONFIG_I2C_1    1
        #define TOUCH_I2C_NAME  CONFIG_I2C1
    #elif defined(BANPIBOARD_QFN80PIN)
        #define CONFIG_I2C_2    1
        #define TOUCH_I2C_NAME  CONFIG_I2C2
    #else
        #define CONFIG_TOUCH_I2C_GPIO   1
    #endif
#endif

/********************************************/
/* PDM Configuration */
/********************************************/
#ifdef CONFIG_PDMA
    #define CONFIG_PDM_INTERRUPT_DRIVEN 1
    #define CONFIG_PDMA                 1
    #define CONFIG_PDMA_NAME      "PDMA"
#endif

#ifdef CONFIG_PDMB
    #define CONFIG_PDM_INTERRUPT_DRIVEN 1
    #define CONFIG_PDMB                 1
    #define CONFIG_PDMB_NAME      "PDMB"
#endif

/********************************************/
/* I2C Configuration */
/********************************************/
#ifdef CONFIG_I2C_0
    #define CONFIG_I2C_INTERRUPT_DRIVEN 1
    #define CONFIG_I2C0                 1
    #define CONFIG_I2C_0_NAME      "I2C_0"
#endif

#ifdef CONFIG_I2C_1
    #define CONFIG_I2C_INTERRUPT_DRIVEN 1
    #define CONFIG_I2C1                 1
    #define CONFIG_I2C_1_NAME      "I2C_1"
#endif

#ifdef CONFIG_I2C_2
    #define CONFIG_I2C_INTERRUPT_DRIVEN 1
    #define CONFIG_I2C2                 1
    #define CONFIG_I2C_2_NAME      "I2C_2"
#endif

/********************************************/
/* SAI Configuration */
/********************************************/
#ifdef CONFIG_SAIA
    #define CONFIG_SAI_INTERRUPT_DRIVEN 1
    #define CONFIG_SAIA                 1
    #define CONFIG_SAIA_NAME      "SAI_A"
#endif

#ifdef CONFIG_SAIB
    #define CONFIG_SAI_INTERRUPT_DRIVEN 1
    #define CONFIG_SAIB                 1
    #define CONFIG_SAIB_NAME      "SAI_B"
#endif


/*config TOUCH*/
#if defined (CONFIG_TOUCH_I2C_GPIO)
    #define CONFIG_I2C_GPIO_1  1
    #define TOUCH_I2C_NAME  CONFIG_I2C_GPIO_1_NAME
    #define CONFIG_I2C_GPIO_1_NAME "I2C_1"
    #if defined (LQFP176_DEMO_BOARD)
        #define CONFIG_I2C_GPIO_1_GPIO "GPIOD"
        #define CONFIG_I2C_GPIO_1_SCL_PIN 6
        #define CONFIG_I2C_GPIO_1_SDA_PIN 7
    #elif defined(QFN80_DEMO_BOARD_V3)
        /*qfn80 demo board v3.0(black board)*/
        #define CONFIG_I2C_GPIO_1_GPIO "GPIOC"
        #define CONFIG_I2C_GPIO_1_SCL_PIN 10
        #define CONFIG_I2C_GPIO_1_SDA_PIN 11
    #elif defined(BANPIBOARD_QFN80PIN)
        #define CONFIG_I2C_GPIO_1_GPIO "GPIOC"
        #define CONFIG_I2C_GPIO_1_SCL_PIN 9
        #define CONFIG_I2C_GPIO_1_SDA_PIN 8
    #else
        /*qfn80 demo board v2.0*/
        #define CONFIG_I2C_GPIO_1_GPIO "GPIOC"
        #define CONFIG_I2C_GPIO_1_SCL_PIN 9
        #define CONFIG_I2C_GPIO_1_SDA_PIN 11
    #endif
#endif

/*camera use i2c gpio*/
#ifdef CONFIG_I2C_GPIO
    #define CONFIG_I2C_GPIO_0 1
    #define CONFIG_I2C_GPIO_0_NAME "I2C_0"
    #if defined(CHIP_F6721B)
        #define CONFIG_I2C_GPIO_0_GPIO "GPIOI"
        #define CONFIG_I2C_GPIO_0_SCL_PIN 2
        #define CONFIG_I2C_GPIO_0_SDA_PIN 3
    #else
        #ifdef GM6721_EVB_QFN40PIN
            #define CONFIG_I2C_GPIO_0_GPIO "GPIOC"
        #else
            #define CONFIG_I2C_GPIO_0_GPIO "GPIOI"
        #endif
        #define CONFIG_I2C_GPIO_0_SCL_PIN 0
        #define CONFIG_I2C_GPIO_0_SDA_PIN 1
    #endif
#endif

/********************************************/
/* MMC Configuration */
/********************************************/
#ifdef CONFIG_DISPLAY
    #define CONFIG_DISPLAY_GM6721X 1
    #define CONFIG_GM_HAL_VOUT 1
#endif

/********************************************/
/* CAN Configuration */
/********************************************/
#ifdef CONFIG_CAN
    #define CONFIG_CAN_GM        1
    #define CONFIG_GM_HAL_CAN    1
#endif

/********************************************/
/* MMC Configuration */
/********************************************/
#ifdef CONFIG_MMC
    #define CONFIG_MMC_GM        1
    #define CONFIG_GM_HAL_MMC    1
    #define CONFIG_DMA           1
    #define CONFIG_DMA_GM        1
    #define CONFIG_GM_HAL_DMA    1
#endif

#ifdef CONFIG_CRYPTO
    #define CONFIG_CRYPTO_INIT_PRIORITY 60
    #define CONFIG_CRYPTO_GM_DEV_NAME  "CRYPTO_GM_DEV"
#endif

#ifdef CONFIG_ENTROPY
    #define CONFIG_ENTROPY_NAME "ENTROPY_0"
#endif

#ifdef  CONFIG_SFLASH
    #define CONFIG_SFLASH_DEV_NAME              "SFLASH_GM_DEV"
    #define CONFIG_SFLASH_SECTOR_LEN              0x1000     //4KB
    #define CONFIG_SFLASH_BLOCK32_LEN             0x8000     //32KB
    #define CONFIG_SFLASH_BLOCK_LEN               0x10000    //64KB
    #define CONFIG_SFLASH_PAGE_LEN                0x100      //256B

    #if (defined NORFLASH_32M)
        #define CONFIG_SFLASH_SIZE                    0x2000000   //16MB
    #else
        #define CONFIG_SFLASH_SIZE                    0x1000000   //32MB
    #endif
    #define CONFIG_SFLASH_SECTOR_MASK             (0xFFF)
    #define CONFIG_SFLASH_PAGE_MASK               (0xFF)
#endif

#endif


