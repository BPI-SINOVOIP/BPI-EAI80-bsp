/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <device.h>
#include <gm_hal_i2c.h>
#include "board.h"
#include <ztest.h>
#include "i2c_test.h"
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>
#include <gm_common.h>
#include <pinmux.h>

#define TIMEOUT_NO_WAIT      0x00000000
#define TIMEOUT_WAIT_FOREVER 0xFFFFFFFF
#define TIMEOUT_NO_BLOCKING  0xFFFFFFFE
static volatile uint32_t s_mstTransCmplt = 0;
static volatile uint32_t s_slvTransCmplt = 0;


/* m_testcase_index is incremented after each test to make test possible
 * switch to next testcase.
 */
volatile static uint32_t m_testcase_index = 0;

/* m_testvalue contains value set in interrupt callback to point whether
 * first or second interrupt was fired.
 */
volatile static uint32_t m_testvalue  = 0;
void i2cMasterCallback(struct I2C_Handle *hI2C, uint8_t event, unsigned long param)
{
    if (event == I2C_EVENT_TRANSFER_DONE)
    {
        s_mstTransCmplt = 1;
    }

    if (event == I2C_EVENT_TX_FIFO_OVERFLOW)
    {

    }

    if (event == I2C_EVENT_RX_FIFO_OVERFLOW)
    {

    }

    if (event == I2C_EVENT_RX_FIFO_UNDERFLOW)
    {

    }

    if (event == I2C_EVENT_ARB_LOST)
    {

    }

    if (event == I2C_EVENT_TERMINATED)
    {

    }

    if (event == I2C_EVENT_DMAERROR)
    {

    }

    if (event == I2C_EVNET_TRANSFER_ERROR)
    {

    }
}

void i2cSlvCallback(struct I2C_Handle *hI2C, uint8_t event, unsigned long param)
{
    if (event == I2C_EVENT_TRANSFER_DONE)
    {
        s_slvTransCmplt = 1;
    }

    if (event == I2C_EVENT_TX_FIFO_OVERFLOW)
    {

    }

    if (event == I2C_EVENT_RX_FIFO_OVERFLOW)
    {

    }

    if (event == I2C_EVENT_RX_FIFO_UNDERFLOW)
    {

    }

    if (event == I2C_EVENT_ARB_LOST)
    {

    }

    if (event == I2C_EVENT_TERMINATED)
    {

    }

    if (event == I2C_EVENT_DMAERROR)
    {

    }

    if (event == I2C_EVNET_TRANSFER_ERROR)
    {

    }
}
/** Write one byte data to EEPROM-24C02 */
static RET_CODE Write_DataToEeprom(I2C_Handle_T *hI2C, uint16_t devAddr, uint8_t memAddr,
                                   uint8_t data, int32_t timeout)
{
    RET_CODE error = RET_OK;
    uint8_t buf[2];

    buf[0] = memAddr;
    buf[1] = data;

    /*
     * Based on 24C02 I2C on BYTE WRITE protocol.
     * The memory address here is treated as only
     * one byte of data.
     */
    error = I2C_MasterTransmit(hI2C, devAddr, buf, 2, timeout);

    return error;
    k_sleep(20);
}

/** Read data out from EEPROM-24C02 */
static RET_CODE Read_DatafromEeprom(I2C_Handle_T *hI2C, uint16_t devAddr, uint8_t memAddr,
                                    uint8_t *pData, uint16_t size, int32_t timeout)
{
    RET_CODE error = RET_OK;

    /* Based on 24C02 I2C on SEQUENTIAL RANDOM READ protocol */


    error = I2C_MasterTransmit(hI2C, devAddr, &memAddr, 1, timeout);
    if (error != RET_OK)
    {
        return error;
    }

    /* Read expected bytes of data out */
    k_sleep(20);
    error = I2C_MasterReceive(hI2C, devAddr, pData, size, timeout);
    return error;
}
static int test_i2c_eeprom(void)
{
#define I2C_MASTER_DEVICE       (I2C2)
#define I2C_MAX_DATALEN          4

    /* (2'b 0101), so that the EEPROM address is 2'b 1010,EEPROM_DEVICE_SELECT */
#define EEPROM_DEVICE_CODE      0x05
    /* Chip Enable (E2:E0 = 2'b 000) */
#define EEPROM_DEVICE_SELECT    0x00
    /* EEPROM 24C02 Device Address */
#define EEPROM_DEVICE_ADDR      (EEPROM_DEVICE_CODE << 4 | EEPROM_DEVICE_SELECT)

    I2C_Handle_T g_i2cMstHandle;
    uint8_t g_rdbuf[I2C_MAX_DATALEN];

    I2C_Config_T i2cMstConfig;
    uint8_t i = 0;
    uint8_t wrbuf[I2C_MAX_DATALEN];

    /* Configure I2C master for I2C1 */
    i2cMstConfig.addrMode     = I2C_7BIT_MODE;
    //i2cMstConfig.callback     = i2cMasterCallback;
    i2cMstConfig.callback     = NULL;
    i2cMstConfig.masterTxMode = I2C_TARGET_ADDRESS_MODE;
    i2cMstConfig.speedMode    = I2C_STANDARD_MODE;
    i2cMstConfig.baudrate     = 100;  /*in Kbps unit*/
    i2cMstConfig.workMode     = I2C_MASTER_MODE;
    i2cMstConfig.enableDMA    = FALSE;

    /* Initiailize I2C perihperal as master */

    if (I2C_Init(I2C_MASTER_DEVICE, &i2cMstConfig, &g_i2cMstHandle) != RET_OK)
    {
        return RET_ERROR;
    }
    for (i = 0; i < I2C_MAX_DATALEN; i++)
    {
        wrbuf[i]  = 0xAA + i;
        /* Write data while memory address is increasing */
        if (Write_DataToEeprom(&g_i2cMstHandle, EEPROM_DEVICE_ADDR, i, wrbuf[i], 100000) != RET_OK)
        {
            return RET_ERROR;
        }
        k_sleep(5);
    }

    if (Read_DatafromEeprom(&g_i2cMstHandle, EEPROM_DEVICE_ADDR, 0, g_rdbuf, I2C_MAX_DATALEN, 100000) != RET_OK)
    {
        return RET_ERROR;
    }

    //while (s_mstTransCmplt == 0) {};
    //while (s_slvTransCmplt == 0) {};
    s_slvTransCmplt = s_mstTransCmplt = 0;

    for (i = 0; i < I2C_MAX_DATALEN; i++)
    {
        if (wrbuf[i] != g_rdbuf[i])
        {
            return RET_ERROR;
        }
    }
    I2C_DeInit(&g_i2cMstHandle);
    TC_PRINT("I2C EEPROM TEST  PASS!\n");
    return TC_PASS;
}
static int test_i2c_loop(void)
{
#define __CPU_HALT_DEBUG__      __asm ("bkpt 0")

#define I2C_MASTER_DEVICE       (I2C0)
#define I2C_SLAVE_DEVICE        (I2C2)

#define I2C_LOCAL_ADDRESS       (0x56)
#define I2C_TARGET_ADDRESS      I2C_LOCAL_ADDRESS

#define I2C_MAX_DATALEN         17

    I2C_Handle_T g_i2cMstHandle;
    I2C_Handle_T g_i2cSlvHandle;

    static uint8_t s_wrBuf[I2C_MAX_DATALEN] = {0};
    static uint8_t s_rdBuf[I2C_MAX_DATALEN] = {0};

    uint8_t i = 0;

    I2C_Config_T i2cMstConfig;
    I2C_Config_T i2cSlvConfig;
    RET_CODE retval;

    /* Configure I2C master */
    i2cMstConfig.addrMode     = I2C_7BIT_MODE;//10
    i2cMstConfig.callback     = i2cMasterCallback;
    i2cMstConfig.masterTxMode = I2C_GENERAL_CALL_MODE;
    i2cMstConfig.speedMode    = I2C_STANDARD_MODE;
    i2cMstConfig.baudrate     = 100;  /*in Kbps unit*/
    i2cMstConfig.workMode     = I2C_MASTER_MODE;
    i2cMstConfig.enableDMA    = FALSE;

    /* Initiailize I2C perihperal as master */
    if (I2C_Init(I2C_MASTER_DEVICE, &i2cMstConfig, &g_i2cMstHandle) != RET_OK)
    {
        return RET_ERROR;
    }

    /* Configure I2C slave */
    i2cSlvConfig.addrMode     = I2C_7BIT_MODE;//10
    i2cSlvConfig.callback     = i2cSlvCallback;
    i2cSlvConfig.masterTxMode = I2C_GENERAL_CALL_MODE;
    i2cSlvConfig.speedMode    = I2C_STANDARD_MODE;
    i2cSlvConfig.workMode     = I2C_SLAVE_MODE;
    /* The slave own address */
    i2cSlvConfig.localAddress = I2C_LOCAL_ADDRESS;

    /* Initiailize I2C perihperal as slave */
    if (I2C_Init(I2C_SLAVE_DEVICE, &i2cSlvConfig, &g_i2cSlvHandle) != RET_OK)
    {
        return RET_ERROR;
    }
    for (i = 0; i < I2C_MAX_DATALEN; i ++)
    {
        s_wrBuf[i] = i + 0x1;
        s_rdBuf[i] = 0;
    }

    /* I2C Slave receives a length of data */
    retval = I2C_SlaveReceive(&g_i2cSlvHandle, s_rdBuf, I2C_MAX_DATALEN, TIMEOUT_NO_BLOCKING);
    //retval = I2C_SlaveReceive(&g_i2cSlvHandle, s_rdBuf, sizeof(s_rdBuf)/sizeof(uint8_t) ,100000);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }
    /* I2C Master transmits a length of data */

    retval = I2C_MasterTransmit(&g_i2cMstHandle, I2C_TARGET_ADDRESS, s_wrBuf, I2C_MAX_DATALEN, TIMEOUT_NO_BLOCKING);
    //retval = I2C_MasterTransmit(&g_i2cMstHandle, I2C_TARGET_ADDRESS, s_wrBuf, sizeof(s_wrBuf)/sizeof(uint8_t), 100000);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    /* Wait till data transcation completes */
    while (s_mstTransCmplt == 0) {};
    while (s_slvTransCmplt == 0) {};
    s_slvTransCmplt = s_mstTransCmplt = 0;

    for (i = 0; i < I2C_MAX_DATALEN; i ++)
    {
        if (s_rdBuf[i] != s_wrBuf[i])
        {
            return RET_ERROR;
        }
    }
    TC_PRINT("I2C LOOP TEST  PASS!\n");
    if (I2C_DeInit(&g_i2cMstHandle) != RET_OK)
    {
        return RET_ERROR;
    }

    if (I2C_DeInit(&g_i2cSlvHandle) != RET_OK)
    {
        return RET_ERROR;
    }

    return TC_PASS;
}

static int test_i2c_dma(void)
{
#define __CPU_HALT_DEBUG__      __asm ("bkpt 0")

#define I2C_MASTER_DEVICE       (I2C2)
#define I2C_SLAVE_DEVICE        (I2C0)

#define I2C_LOCAL_ADDRESS       (0x56)
#define I2C_TARGET_ADDRESS      I2C_LOCAL_ADDRESS

#define I2C_MAX_DATALEN         4

    I2C_Handle_T g_i2cMstHandle;
    I2C_Handle_T g_i2cSlvHandle;

    static uint8_t s_wrBuf[I2C_MAX_DATALEN] = {0};
    static uint8_t s_rdBuf[I2C_MAX_DATALEN] = {0};


    static uint16_t s_dmabuffer[2] = {0};
    static uint32_t s_dmaBufferLen  = 2;

    uint8_t i = 0;

    I2C_Config_T i2cMstConfig;
    I2C_Config_T i2cSlvConfig;
    RET_CODE retval;

    /* Configure I2C master */
    i2cMstConfig.addrMode     = I2C_7BIT_MODE;//10
    //i2cMstConfig.addrMode     = I2C_10BIT_MODE;//10
    i2cMstConfig.callback     = i2cMasterCallback;
    //i2cMstConfig.callback     = NULL;
    i2cMstConfig.masterTxMode = I2C_TARGET_ADDRESS_MODE;//I2C_GENERAL_CALL_MODE;//
    i2cMstConfig.speedMode    = I2C_FAST_MODE;//I2C_STANDARD_MODE;
    //i2cMstConfig.baudrate     = 100;  /*in Kbps unit*/
    i2cMstConfig.baudrate     = 100;  /*in Kbps unit*/
    //i2cMstConfig.baudrate     = 10;  /*in Kbps unit*/
    i2cMstConfig.workMode     = I2C_MASTER_MODE;
    //i2cMstConfig.enableDMA    = FALSE;
#if 0
    i2cMstConfig.enableDMA    = TRUE;
    /*Ignore the right following two configurations, if DMA is not active*/
    i2cMstConfig.pdmaBuffer      = s_dmabuffer;
    i2cMstConfig.dmaBufferLength = s_dmaBufferLen;
#endif
    //
    /* Initiailize I2C perihperal as master */
    if (I2C_Init(I2C_MASTER_DEVICE, &i2cMstConfig, &g_i2cMstHandle) != RET_OK)
    {
        return RET_ERROR;
    }

    /* Configure I2C slave */
    i2cSlvConfig.addrMode     = I2C_7BIT_MODE;//7
    //i2cSlvConfig.addrMode     = I2C_10BIT_MODE;//10
    i2cSlvConfig.callback     = i2cSlvCallback;
    //i2cSlvConfig.callback     = NULL;
    i2cSlvConfig.masterTxMode = I2C_TARGET_ADDRESS_MODE; //I2C_GENERAL_CALL_MODE;//
    i2cSlvConfig.speedMode    = I2C_FAST_MODE;//I2C_STANDARD_MODE;
    i2cSlvConfig.workMode     = I2C_SLAVE_MODE;
    /* The slave own address */
    i2cSlvConfig.localAddress = I2C_LOCAL_ADDRESS;

#if 1
    i2cSlvConfig.enableDMA    = TRUE;
    /*Ignore the right following two configurations, if DMA is not active*/
    i2cSlvConfig.pdmaBuffer      = s_dmabuffer;
    i2cSlvConfig.dmaBufferLength = s_dmaBufferLen;
#endif
    /* Initiailize I2C perihperal as slave */
    if (I2C_Init(I2C_SLAVE_DEVICE, &i2cSlvConfig, &g_i2cSlvHandle) != RET_OK)
    {
        return RET_ERROR;
    }
    for (i = 0; i < I2C_MAX_DATALEN; i ++)
    {
        s_wrBuf[i] = i + 0x71;

        s_rdBuf[i] = 0;
    }

    /* I2C Slave receives a length of data */
    retval = I2C_SlaveReceive(&g_i2cSlvHandle, s_rdBuf, sizeof(s_rdBuf) / sizeof(uint8_t), TIMEOUT_NO_BLOCKING);
    //retval = I2C_SlaveReceive(&g_i2cSlvHandle, s_rdBuf, sizeof(s_rdBuf)/sizeof(uint8_t) , 100000);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }


    /* I2C Master transmits a length of data */
    retval = I2C_MasterTransmit(&g_i2cMstHandle, I2C_TARGET_ADDRESS, s_wrBuf, sizeof(s_wrBuf) / sizeof(uint8_t), TIMEOUT_NO_BLOCKING);
    // retval = I2C_MasterTransmit(&g_i2cMstHandle, I2C_TARGET_ADDRESS, s_wrBuf, sizeof(s_wrBuf)/sizeof(uint8_t), 100000);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    /* Wait till data transcation completes */
    while (s_mstTransCmplt == 0) {};
    while (s_slvTransCmplt == 0) {};

    s_slvTransCmplt = s_mstTransCmplt = 0;

    for (i = 0; i < sizeof(s_wrBuf) / sizeof(uint8_t); i ++)
    {
        if (s_rdBuf[i] != s_wrBuf[i])
        {
            return RET_ERROR;
        }
    }
    TC_PRINT("I2C DMA TEST  PASS!\n");
    if (I2C_DeInit(&g_i2cMstHandle) != RET_OK)
    {
        return RET_ERROR;
    }

    if (I2C_DeInit(&g_i2cSlvHandle) != RET_OK)
    {
        return RET_ERROR;
    }

    return TC_PASS;
}

void i2c_test(void)
{
    printf("================I2C Test Start=================\n\n\n");
    if (m_testcase_index == 0)
    {
        zassert_true(test_i2c_eeprom() == TC_PASS, NULL);
        m_testcase_index ++;
    }
    if (m_testcase_index == 1)
    {
        zassert_true(test_i2c_loop() == TC_PASS, NULL);
        m_testcase_index ++;
    }
    if (m_testcase_index == 2)
    {
        zassert_true(test_i2c_dma() == TC_PASS, NULL);
    }

    if (m_testcase_index > 1)
    {
        m_testcase_index = 0;
        m_testvalue = 0;
    }
    printf("================I2C Test End=================\n\n\n");
}
