/*
 * Copyright (c) 2016, Gree Instruments Incorporated
 *
 */
#ifndef __I2C_GM66X_H__
#define __I2C_GM66X_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

/*gree sdk include*/
#include <gm_hal_i2c.h>
#include <i2c.h>
#define I2C_ID_0    0
#define I2C_ID_1    1
#define I2C_ID_2    2

#define  I2C0_DEV_NAME     i2c0_dev
#define  I2C1_DEV_NAME     i2c1_dev
#define  I2C2_DEV_NAME     i2c2_dev
#ifdef CONFIG_I2C0
#define  I2C0_DRV_NAME     "I2C_0"
#endif
#ifdef CONFIG_I2C1
#define  I2C1_DRV_NAME     "I2C_1"
#endif
#ifdef CONFIG_I2C2
#define  I2C2_DRV_NAME     "I2C_2"
#endif
//typedef void (*i2c_irq_callback_t)(uint8_t event, unsigned long param);
#if 0
typedef struct
{
    I2C_AddrMode_T     addrMode;      /**< I2C address mode */
    I2C_WorkMode_T     workMode;      /**< I2C working mode */
    I2C_SpeedMode_T    speedMode;     /**< I2C speed mode */
    I2C_MasterTxMode_T masterTxMode;  /**< I2C Tx mode */
    uint8_t            enableDMA;     /**< I2C DMA usage option */

    uint32_t localAddress;  /**< I2C local address, valid only in slave mode*/
    uint32_t baudrate;      /**< I2C baudrate */

    uint16_t *pdmaBuffer;       /**< DMA buffer, it must be specified when DMA is enabled and DMA_MEMORY_TO_PERIPH
                                is the direction, must be 16Bit aligned by user app */
    uint16_t  dmaBufferLength;  /**< DMA buffer length*/

    I2C_Callback_T callback;    /**< callback function */
} I2C_Config_T;


typedef struct I2C_Handle
{
    uint8_t          i2c_id;        /**< I2C id */
    I2C_State_T      state;        /**< I2C data-transmission state */
    I2C_Device_T    *pDevice;      /**< I2C device handler */
    I2C_ErrorCode_T  errorCode;    /**< I2C error code */
#if DMA_66XX_OK
    DMA_Handle_T    *hDMA      ;   /**< DMA device handler */
#endif
    uint8_t          enableDMA;    /**< I2C DMA usage option */

    uint8_t  txFifoDepth;  /**< I2C TX FIFO depth */
    uint8_t  rxFifoDepth;  /**< I2C RX FIFO depth */
    uint8_t  txThreshld;   /**< I2C TX threshold */
    uint8_t  rxThreshld;   /**< I2C RX threshold */
    uint8_t *pTxBuffer;    /**< Pointer to Tx buffer */
    uint8_t *pRxBuffer;    /**< Pointer to Rx buffer */

    uint16_t  txLength;    /**< Data length for transfering */
    uint16_t  txRemained;  /**< Data remained for transfering */
    uint16_t  rxLength;    /**< Data length for receiving */
    uint16_t  rxRemained;  /**< Data remained for receiving */
    uint16_t *pdmaBuffer;  /**< DMA buffer, it must be specified when DMA is enabled and DMA_MEMORY_TO_PERIPH
                                is the direction, must be 16Bit aligned by user app */
    uint16_t  dmaBufferLength;  /**< DMA buffer length */
    I2C_Callback_T callback;    /**< callback function */

} I2C_Handle_T;
#endif

//struct i2c_gm_dev_data_t {
//#ifdef CONFIG_I2C_INTERRUPT_DRIVEN
//    i2c_irq_callback_t cb; /**< Callback function pointer */
//#endif /* CONFIG_UART_INTERRUPT_DRIVEN */
//  /* clock device */
//  struct device *clock;
//  I2C_Handle_T *hI2C;
//};

#ifdef __cplusplus
}
#endif

#endif




