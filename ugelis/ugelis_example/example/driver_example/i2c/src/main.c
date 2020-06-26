#include <ugelis.h>
#include <device.h>
#include <i2c.h>
#include <board.h>
//#include <i2c_gm.h>

#define RX_I2C_NAME "I2C0"
#define TX_I2C_NAME "I2C2"
extern uint32_t s_mstTransCmplt;
extern uint32_t s_slvTransCmplt;
#define HW32_REG(ADDRESS)  (*((volatile unsigned long  *)(ADDRESS)))
#define  I2C_USERBUFFER_SIZE  17    /**< Specify user buffer size */
static uint8_t wrbuf[I2C_USERBUFFER_SIZE] = {0};
static uint8_t rdbuf[I2C_USERBUFFER_SIZE] = {0};

#define TIMEOUT_NO_WAIT      0x00000000 /**< Transaction is over after function returns, it returns RET_ERROR if transaction is not finished. */
#define TIMEOUT_WAIT_FOREVER 0xFFFFFFFF /**< Function is about to block till transaction is over */
#define TIMEOUT_NO_BLOCKING  0xFFFFFFFE /**< Function is never blocked, user will be notified by callback after transaction is over */

#define I2C_LOCAL_ADDRESS       (0x56)
#define I2C_TARGET_ADDRESS      I2C_LOCAL_ADDRESS

/* (2'b 0101), so that the EEPROM address is 2'b 1010,EEPROM_DEVICE_SELECT */
#define EEPROM_DEVICE_CODE      0x05
/* Chip Enable (E2:E0 = 2'b 000) */
#define EEPROM_DEVICE_SELECT    0x00
/* EEPROM 24C02 Device Address */
#define EEPROM_DEVICE_ADDR      (EEPROM_DEVICE_CODE << 4 | EEPROM_DEVICE_SELECT)

static int test_i2c_irq_and_poll(void)
{
    static uint8_t s_wrBuf[I2C_USERBUFFER_SIZE] = {0};
    static uint8_t s_rdBuf[I2C_USERBUFFER_SIZE] = {0};
    RET_CODE retval;
    int i = 0;
    int sent_char;
    for (i = 0; i < I2C_USERBUFFER_SIZE; i++)
    {
        s_wrBuf[i] = i + 1;
        s_rdBuf[i] = 0;
    }
    uint32_t config_i2c0 = 0x22;
    uint32_t config_i2c2 = 0x32;
    struct i2c_msg msgs0, msgs2;
    msgs0.buf = (uint8_t *)s_rdBuf;
    msgs0.len = I2C_USERBUFFER_SIZE;
    msgs0.flags = I2C_MSG_READ;
    msgs0.timeout = TIMEOUT_NO_BLOCKING;

    msgs2.buf = (uint8_t *)s_wrBuf;
    msgs2.len = I2C_USERBUFFER_SIZE;
    msgs2.flags = I2C_MSG_WRITE;
    msgs2.timeout = TIMEOUT_NO_BLOCKING;
    struct device *i2c_dev = device_get_binding(CONFIG_I2C_0_NAME);
    if (i2c_dev == NULL)
    {
        return RET_ERROR;
    }
    i2c_configure(i2c_dev, config_i2c0);

    struct device *i2c_dev2 = device_get_binding(CONFIG_I2C_2_NAME);
    if (i2c_dev2 == NULL)
    {
        return RET_ERROR;
    }
    i2c_configure(i2c_dev2, config_i2c2);

    retval = i2c_transfer(i2c_dev, &msgs0, 1, I2C_TARGET_ADDRESS);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    retval = i2c_transfer(i2c_dev2, &msgs2, 1, I2C_TARGET_ADDRESS);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    /* Wait till data transcation completes */
    while (s_mstTransCmplt == 0) {};
    while (s_slvTransCmplt == 0) {};
    s_slvTransCmplt = s_mstTransCmplt = 0;
    for (i = 0; i < I2C_USERBUFFER_SIZE; i++)
    {
        if (s_wrBuf[i] != s_rdBuf[i])
        {
            return RET_ERROR;
        }
        printf("reveive data %d is %d\n", i, s_rdBuf[i]);
    }
    printf("test_i2c_irq_and_poll ok.\n");
    return RET_OK;
}
static int test_i2ctx_and_eeprom_i2crx(void)
{
    uint32_t read_data;
    uint8_t trans_data[2] = {0};
    static uint8_t  g_rdbuf[I2C_USERBUFFER_SIZE] = {0};
    static uint8_t  g_wrbuf[I2C_USERBUFFER_SIZE] = {0};
    RET_CODE retval;
    struct device *i2c_dev2 = device_get_binding(CONFIG_I2C_2_NAME);
    if (i2c_dev2 == NULL)
    {
        return RET_ERROR;
    }
    uint32_t config_i2c = 0x14;
    i2c_configure(i2c_dev2, config_i2c);
    struct i2c_msg msgs1, msgs2, msgs3;
    uint32_t i = 0;
    uint8_t memaddr = 0;
    for (i = 0; i < I2C_USERBUFFER_SIZE; i++)
    {
        g_wrbuf[i] = i + 0xAA;
        trans_data[0] = i;
        trans_data[1] = g_wrbuf[i];
        msgs1.buf = (uint8_t *)trans_data;
        msgs1.len = 2;
        msgs1.flags = I2C_MSG_WRITE;
        msgs1.timeout = TIMEOUT_WAIT_FOREVER;
        retval = i2c_transfer(i2c_dev2, &msgs1, 1, EEPROM_DEVICE_ADDR);
        k_sleep(10);
    }
    /* Based on 24C02 I2C on SEQUENTIAL RANDOM READ protocol */
    msgs2.buf = &memaddr;
    msgs2.len = 1;
    msgs2.flags = I2C_MSG_WRITE;
    msgs2.timeout = TIMEOUT_WAIT_FOREVER;
    retval = i2c_transfer(i2c_dev2, &msgs2, 1, EEPROM_DEVICE_ADDR);
    k_sleep(20);
    /* Read expected bytes of data out */
    msgs3.buf = (uint8_t *)g_rdbuf;
    msgs3.len = I2C_USERBUFFER_SIZE;
    msgs3.flags = I2C_MSG_READ;
    msgs3.timeout = TIMEOUT_WAIT_FOREVER;
    retval = i2c_transfer(i2c_dev2, &msgs3, 1, EEPROM_DEVICE_ADDR);

    for (i = 0; i < I2C_USERBUFFER_SIZE; i++)
    {
        if (g_wrbuf[i] != g_rdbuf[i])
        {
            return RET_ERROR;
        }
        printf("reveive data %d is %d\n", i, g_rdbuf[i]);
    }
    printf("test_i2ctx_and_eeprom_i2crx ok.\n");
    return RET_OK;
}

static int test_i2c_and_dma(void)
{
    static uint8_t s_wrBuf[I2C_USERBUFFER_SIZE] = {0};
    static uint8_t s_rdBuf[I2C_USERBUFFER_SIZE] = {0};
    RET_CODE retval;
    int i = 0;
    int sent_char;
    for (i = 0; i < I2C_USERBUFFER_SIZE; i++)
    {
        s_wrBuf[i] = i + 1;
        s_rdBuf[i] = 0;
    }
    uint32_t config_i2c0 = 0x62;
    uint32_t config_i2c2 = 0x12;
    struct i2c_msg msgs0, msgs2;
    msgs0.buf = (uint8_t *)s_rdBuf;
    msgs0.len = I2C_USERBUFFER_SIZE;
    msgs0.flags = I2C_MSG_READ;
    msgs0.timeout = TIMEOUT_NO_BLOCKING;

    msgs2.buf = (uint8_t *)s_wrBuf;
    msgs2.len = I2C_USERBUFFER_SIZE;
    msgs2.flags = I2C_MSG_WRITE;
    msgs2.timeout = TIMEOUT_WAIT_FOREVER;
    struct device *i2c_dev = device_get_binding(CONFIG_I2C_0_NAME);
    if (i2c_dev == NULL)
    {
        return RET_ERROR;
    }
    i2c_configure(i2c_dev, config_i2c0);

    struct device *i2c_dev2 = device_get_binding(CONFIG_I2C_2_NAME);
    if (i2c_dev2 == NULL)
    {
        return RET_ERROR;
    }
    i2c_configure(i2c_dev2, config_i2c2);

    retval = i2c_transfer(i2c_dev, &msgs0, 1, I2C_TARGET_ADDRESS);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    retval = i2c_transfer(i2c_dev2, &msgs2, 1, I2C_TARGET_ADDRESS);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    /* Wait till data transcation completes */
    //while (s_mstTransCmplt == 0) {};
    while (s_slvTransCmplt == 0) {};
    s_slvTransCmplt = s_mstTransCmplt = 0;
    for (i = 0; i < I2C_USERBUFFER_SIZE; i++)
    {
        if (s_wrBuf[i] != s_rdBuf[i])
        {
            return RET_ERROR;
        }
        printf("reveive data %d is %d\n", i, s_rdBuf[i]);
    }
    printf("test_i2c_and_dma ok.\n");
    return RET_OK;
}
void main(void)
{
    printf("test I2Cx printk.\n");
    //test_i2c_irq_and_poll();
    test_i2ctx_and_eeprom_i2crx();
    //test_i2c_and_dma();
    while (1);
}

