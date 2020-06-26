/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file spi_transfer.c
 *
 * @version 1.0.0
 *
 * @date 2016/07/19
 *
 * @contents Verification and example code of Cyclic redundancy check (CRC).
 *
 * @history
 *  Date                    By              Comments                    Ver.
 *  =========       =========        =========           =========
 *  2015/07/19      Wen Liu          Create                  1.0.0
 **********************************************************************/
#include <ugelis.h>
#include <device.h>
#include <gm_common.h>
#include <misc/printk.h>
#include <board.h>
#include <misc/util.h>
#include <gm_hal_gpio.h>
#include <gm_hal_uart.h>
#include <gm_hal_spi.h>
#include <pinmux.h>
#include <gpio.h>

/******************************************************************************
* Global variables
******************************************************************************/
SPI_Handle_T g_spi1Handle;

/******************************************************************************
* Constants and macros
******************************************************************************/

static uint8_t s_txData[20]; /**< User transmission buffer */
static uint8_t s_rxData[20]; /**< User reception buffer */

/******************************************************************************
* Local functions
******************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/

/** Configure pinmux for an available SPI device */

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

Device_Pinmux_T s_spi0PinMux[4] =
{
    {PINMUX_GPIO_PIN_2, GPIOC, GPIOC2_AF3_SPI0_SS},
    {PINMUX_GPIO_PIN_3, GPIOC, GPIOC3_AF3_SPI0_SCK},
    {PINMUX_GPIO_PIN_4, GPIOC, GPIOC4_AF3_SPI0_MISO},
    {PINMUX_GPIO_PIN_5, GPIOC, GPIOC5_AF3_SPI0_MOSI},
};

Device_Pinmux_T s_spi1PinMux[4] =
{
    {PINMUX_GPIO_PIN_8, GPIOC, GPIOC8_AF3_SPI1_SS},
    {PINMUX_GPIO_PIN_9, GPIOC, GPIOC9_AF3_SPI1_SCK},
    {PINMUX_GPIO_PIN_10, GPIOC, GPIOC10_AF3_SPI1_MISO},
    {PINMUX_GPIO_PIN_11, GPIOC, GPIOC11_AF3_SPI1_MOSI},
};

static int SPI_ConfigPinmux(SPI_Device_T *pSPI)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;
    struct device *portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);
    if (pSPI == SPI0_DEV)
    {
        /** SPI0 GPIO Configuration */
        pin_cfg.pin = BIT(s_spi0PinMux[0].pin);
        pin_cfg.mode = GPIO_MODE_AF;
        pin_cfg.pull = GPIO_NOPULL;
        pin_cfg.alternate = s_spi0PinMux[0].alternate;
        ret = pinmux_pin_set(portc, s_spi0PinMux[0].pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }

        pin_cfg.pin = BIT(s_spi0PinMux[1].pin);
        pin_cfg.mode = GPIO_MODE_AF;
        pin_cfg.pull = GPIO_NOPULL;
        pin_cfg.alternate = s_spi0PinMux[1].alternate;
        ret = pinmux_pin_set(portc, s_spi0PinMux[1].pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }

        pin_cfg.pin = BIT(s_spi0PinMux[2].pin);
        pin_cfg.mode = GPIO_MODE_AF;
        pin_cfg.pull = GPIO_NOPULL;
        pin_cfg.alternate = s_spi0PinMux[2].alternate;
        ret = pinmux_pin_set(portc, s_spi0PinMux[2].pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }

        pin_cfg.pin = BIT(s_spi0PinMux[3].pin);
        pin_cfg.mode = GPIO_MODE_AF;
        pin_cfg.pull = GPIO_NOPULL;
        pin_cfg.alternate = s_spi0PinMux[3].alternate;
        ret = pinmux_pin_set(portc, s_spi0PinMux[3].pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }
        return 0;
    }
    else if (pSPI == SPI1_DEV)
    {
        /** SPI1 GPIO Configuration */
        pin_cfg.pin = BIT(s_spi1PinMux[0].pin);
        pin_cfg.mode = GPIO_MODE_AF;
        pin_cfg.pull = GPIO_NOPULL;
        pin_cfg.alternate = s_spi1PinMux[0].alternate;
        ret = pinmux_pin_set(portc, s_spi1PinMux[0].pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }

        pin_cfg.pin = BIT(s_spi1PinMux[1].pin);
        pin_cfg.mode = GPIO_MODE_AF;
        pin_cfg.pull = GPIO_NOPULL;
        pin_cfg.alternate = s_spi1PinMux[1].alternate;
        ret = pinmux_pin_set(portc, s_spi1PinMux[1].pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }

        pin_cfg.pin = BIT(s_spi1PinMux[2].pin);
        pin_cfg.mode = GPIO_MODE_AF;
        pin_cfg.pull = GPIO_NOPULL;
        pin_cfg.alternate = s_spi1PinMux[2].alternate;
        ret = pinmux_pin_set(portc, s_spi1PinMux[2].pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }

        pin_cfg.pin = BIT(s_spi1PinMux[3].pin);
        pin_cfg.mode = GPIO_MODE_AF;
        pin_cfg.pull = GPIO_NOPULL;
        pin_cfg.alternate = s_spi1PinMux[3].alternate;
        ret = pinmux_pin_set(portc, s_spi1PinMux[3].pin, (unsigned int)&pin_cfg);
        if (ret)
        {
            return ret;
        }
        return 0;
    }
}

void spi1Callback(SPI_Handle_T *hSPI, uint8_t event, unsigned long param)
{
    return;
}

int main(void)
{
    SPI_Config_T spiConfig;
    SPI_TransferConfig_T spiTransferConfig;
    int i;
    SPI_Handle_T *spiHandle;
    SPI_ConfigPinmux(SPI1_DEV);
    printf("Sample code of spi driver \n");

    /* Initialize spi device with CPOL = 0, CPHA = 0, Bit Order = LSB,
        Mode = MASTER, Single wire = FALSE, ... */
    spiHandle = &g_spi1Handle;

    /* Get spi default configuration */
    HAL_SPI_GetDefaultConfig(&spiConfig, &spiTransferConfig, SPI_MASTER_MODE);

    spiConfig.mode = SPI_MASTER_MODE;
    spiConfig.format = SPI_FORMAT_MOTOLORA;
    spiConfig.polarity = SPI_CLOCK_POLARITY_LOW;
    spiConfig.phase = SPI_CLOCK_PHASE_MIDDLE;
    spiConfig.bitOrder = SPI_BIT_ORDER_LSB;
    spiConfig.singleWire = FALSE;
    spiConfig.baudRate = 20000;
    spiConfig.callback = spi1Callback;
    HAL_SPI_Init(SPI1_DEV, &spiConfig, spiHandle);

    /* Initialize spi device with changeable configuration about data transferring,
       such as fixed frames, duplex mode, dma usage, fifo threshhold, continuous
       data frames, etc. */

    /* SPI transfers one fixed command frame and multiple data frames */
    spiTransferConfig.frameConfig.mask = (SPI_MASK_DATA_FRAME | SPI_MASK_COMMAND_FRAME);
    /* Command frame width */
    spiTransferConfig.frameConfig.commandBits = 8;
    /* Address frame width */
    spiTransferConfig.frameConfig.addressBits = 0;
    /* Dummy frame width */
    spiTransferConfig.frameConfig.dummyBits = 0;
    /* Data frame width */
    spiTransferConfig.frameConfig.dataBits = 8;
    /* Command frame value */
    spiTransferConfig.frameConfig.command = 0x50;

    spiTransferConfig.duplex = SPI_FULL_DUPLEX;
    spiTransferConfig.txWatermark = 4;
    spiTransferConfig.rxWatermark = 4;
    spiTransferConfig.continuedData = TRUE;
    spiTransferConfig.enableRxDMA = FALSE;
    spiTransferConfig.enableTxDMA = FALSE;
    spiTransferConfig.continuousDataCount = 5;

    HAL_SPI_TransferConfig(spiHandle, &spiTransferConfig);

    HAL_SPI_EnableInterrupts(spiHandle, (SPI_RX_INT | SPI_TX_INT));

    /* Transmitted data is here */
    for (i = 0; i < 20; i++)
    {
        s_txData[i] = i + 2;
    }

    /* SPI transmission and reception test in full-duplex mode */
    /* You have to connect CS0, SCLK, MISO, MOSI signals of two demo boards(GRJ542)
       for spi demo test, for example,
       CS0:  CN7-17 ---- CN7-17
       SCLK: CN7-11 ---- CN7-11
       MOSI: CN7-25(as master) ---- CN7-21(as slave)
       MISO: CN7-21(as master) ---- CN7-25(as slave)
       GND:  CN10-GND ---- CN10-GND
    */
    HAL_SPI_TransmitReceive(spiHandle, &s_txData[0], &s_rxData[0], 18, 18, TIMEOUT_WAIT_FOREVER);

    while (1) {};

}
