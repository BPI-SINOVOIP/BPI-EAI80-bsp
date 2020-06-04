/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        ft5426.c
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2020/02/17
 *
 * @brief       The driver of touchpanel ft5426
 *
 * @note
 *              2020/02/17, Angrad.Yang, v1.0.0
 *                  Initial version.
 *
 **/

#include <kernel.h>
#include <i2c.h>
#include <gpio.h>
#include <touch.h>
#include <ts.h>
#include "ft5426.h"

#define TS_REMOVE_SHARK_PERIOD  20
#define REPEAT_SCAN_PREIOD      30

#define CONFIG_LTDC_ATK7V13_DEFAULT_HEIGHT 600

//#define TS_DEBUG  1
#define TS_ERR    1
//#define TS_WARING 1

#ifndef TS_DEBUG
    #define ts_debug(...)      do {} while (0)
#else
    #define ts_debug           printk
#endif

#ifndef TS_ERR
    #define ts_err(...)      do {} while (0)
#else
    #define ts_err           printk
#endif

#ifndef TS_WARING
    #define ts_warning(...)      do {} while (0)
#else
    #define ts_warning           printk
#endif

static uint8_t wrokq_tack[WORKQ_STACK_SIZE];
static uint8_t ts_scan_tack[TS_SCAN_STACK_SIZE];
static struct k_thread ts_thread;

static void ft5426_repeat_scan(struct k_work *work);
static void ts_ft5426_scan_thread(void *p1, void *p2, void *p3);

const uint16_t ft5426_tpx_tbl[TS_FT5426_SLOT_CNT] = {FT_TP1_REG, FT_TP2_REG, FT_TP3_REG, FT_TP4_REG, FT_TP5_REG};
global_ts_cb gTouchCallback;

static int ft5426_read(struct device *dev, uint16_t reg, uint8_t *value, uint8_t len)
{
    ts_ft5426_claim_reg(dev);
    const struct ft5426_data_t *data = DEV_DATA(dev);
    uint8_t addr_buffer;
    int ret = 0;
    struct i2c_msg msg[2];

    addr_buffer = reg & 0xFF;
    msg[0].buf = &addr_buffer;
    msg[0].len = 1;
    msg[0].flags = I2C_MSG_WRITE/*|I2C_MSG_STOP*/;

    k_sleep(2);

    msg[1].buf = value;
    msg[1].len = len;
    msg[1].flags = I2C_MSG_RESTART | I2C_MSG_READ | I2C_MSG_STOP/*|I2C_MSG_STOP*/;

    ret = i2c_transfer(data->i2c, msg, 2, FT5426_ADDR);
    ts_ft5426_release_reg(dev);
    return ret;
}

static int ft5426_write(struct device *dev, uint16_t reg, uint8_t *value, int len)
{
    ts_ft5426_claim_reg(dev);
    const struct ft5426_data_t *data = DEV_DATA(dev);
    uint8_t buf[3];
    struct i2c_msg msg;
    int ret = 0;

    buf[0] = reg & 0xFF;
    buf[1] = value;
    msg.buf = buf;
    msg.len = len + 1;
    msg.flags = I2C_MSG_WRITE | I2C_MSG_STOP;

    ret = i2c_transfer(data->i2c, &msg, 1, FT5426_ADDR);
    ts_ft5426_release_reg(dev);
    return ret;
}

static uint8_t ft5426_init(struct device *dev)
{
    struct ft5426_data_t *data = DEV_DATA(dev);
    uint8_t ret = 0;
    uint8_t temp[3];

    data->i2c = device_get_binding(data->i2c_name);
    if (data->i2c == NULL)
    {
        return -ENOTSUP;
    }
    ret = i2c_configure(data->i2c, 0x14);
    if (ret < 0)
    {
        printk("I2C config failed\n");
        return -ENOTSUP;
    }
    data->rst_port = device_get_binding(data->rst_port_name);
    if (data->rst_port)
    {
        gpio_pin_configure(data->rst_port, data->rst_pin, GPIO_DIR_OUT);
    }
    gpio_pin_write(data->rst_port, data->rst_pin, 0);

    k_sleep(20);
    gpio_pin_write(data->rst_port, data->rst_pin, 1);
    k_sleep(50);

    ft5426_read(dev, ID_G_CIPHER, &temp[0], 1);
    ft5426_read(dev, ID_G_FIRMID, &temp[1], 1);
    ft5426_read(dev, D_G_FT5201ID, &temp[2], 1);
    ts_debug("ChipVendor: 0x%x, Firm: 0x%x, FT5201: 0x%x\n", temp[0], temp[1], temp[2]);

    temp[0] = 0;
    ft5426_write(dev, FT_DEVIDE_MODE, temp, 1);  //Enter normal mode

    ft5426_write(dev, FT_ID_G_MODE, temp, 1);    //Enter query mode

    temp[0] = 22;                           //smaller is more sensitive
    ft5426_write(dev, FT_ID_G_THGROUP, temp, 1); //Set valid touching detect threshold

    temp[0] = 12;                           //Period Active, max is 14, can not be smaller than 12
    ft5426_write(dev, FT_ID_G_PERIODACTIVE, temp, 1);

    //read the version number£¬reference value: 0x0001
    ft5426_read(dev, FT_ID_G_LIB_VERSION, &temp[0], 2);
    if ((temp[0] == 0X30 && temp[1] == 0X03) || temp[1] == 0X01 || temp[1] == 0X02) //version:0X3003/0X0001/0X0002
    {
        ts_debug("CTP ID:%x\r\n", ((uint16_t)temp[0] << 8) + temp[1]);
        return 0;
    }

    return 1;
}

static void ft5426_set_dir(struct device *dev, bool dir)
{
    struct ft5426_data_t *data = DEV_DATA(dev);

    data->ts_dir = dir;
}

static bool ft5426_get_dir(struct device *dev)
{
    struct ft5426_data_t *data = DEV_DATA(dev);

    return data->ts_dir;
}

static int ts_ft5426_delaywork_init(struct device *dev)
{
    int ret = 0;

    struct ft5426_data_t *data = DEV_DATA(dev);

    k_delayed_work_init(&data->delaywork, ft5426_repeat_scan);

    k_work_q_start(&data->workq, wrokq_tack, WORKQ_STACK_SIZE, 0);

    return ret;
}

static int ts_ft5426_init(struct device *dev)
{
    int ret = 0;

    /* mutex for mul task access sd hardware*/
    ts_ft5426_claim_init(dev);

    ret = ft5426_init(dev);
    if (ret)
    {
        ts_err("%s: Err:%d FT5426_Init\n", __FUNCTION__, ret);
        return ret;
    }

    /*init delaywork for repeat scan*/
    ts_ft5426_delaywork_init(dev);


    /*start a thead for scan*/

    k_thread_create(&ts_thread, ts_scan_tack,
                    TS_SCAN_STACK_SIZE,
                    (k_thread_entry_t)ts_ft5426_scan_thread,
                    (void *)dev, 0, 0,
                    -1, 0, K_NO_WAIT);
    return ret;
}

static bool ts_ft5426_get_status(struct device *dev, uint8_t *status)
{
    ft5426_read(dev, FT_REG_NUM_FINGER, status, 1);
    if ((*status & 0xf) && ((*status & 0xf) < 6))
    {
        return TS_PRESS;
    }
    else
    {
        return TS_RELEASE;
    }
}

static void ts_set_press_status(struct ft5426_data_t *data, uint8_t status)
{
    uint8_t key = 0;
    key = irq_lock();
    data->ts_press = status;
    irq_unlock(key);
}

/*no relse scan */
static void ft5426_repeat_scan(struct k_work *work)
{
    int ret = 0;
    uint8_t mode;
    bool status;
    uint8_t report = 1;
    uint8_t i = 0;
    uint8_t buf[4];
    uint16_t *regtable = ft5426_tpx_tbl;
    uint32_t abs_table_x[TS_FT5426_SLOT_CNT];
    uint32_t abs_table_y[TS_FT5426_SLOT_CNT];
    struct ts_input_event event;
    struct ft5426_data_t *data = CONTAINER_OF(work, struct ft5426_data_t, delaywork);
    struct device *dev = device_get_binding(FT5426_DEV_NAME);

    status = ts_ft5426_get_status(dev, &mode);
    if (status == TS_PRESS)
    {
        k_sleep(TS_REMOVE_SHARK_PERIOD);
        status = ts_ft5426_get_status(dev, &mode);
        if (status == TS_PRESS)
        {
            ts_debug("\nts is press:%s\n", __FUNCTION__);
            /*convet to touch press bit map*/
            mode = ~(0xff << (mode & 0xf));
            /*ts press ,send event*/
            for (i = 0; i < TS_FT5426_SLOT_CNT; i++)
            {
                // printk("ft5426_repeat_scan:mode:0x%x\n",mode);
                if (mode & (1 << i))
                {
                    /*slot is press*/
                    ft5426_read(dev, regtable[i], buf, 4); //read xy postion
                    if ((buf[0] & 0xf0) != 0x80)
                    {
                        /*not send event*/
                        abs_table_x[i] = abs_table_y[i] = 0;
                        ts_set_press_status(data, TS_RELEASE);
                        ts_warning("###########waring:%s##############\n", __FUNCTION__);
                        continue;
                    }
                    if (data->ts_dir == TS_DIR_THWARTWISE) //horizontal screen
                    {
                        abs_table_y[i] = ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];
                        abs_table_x[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
                    }
                    else
                    {
                        /*need to check*/
                        abs_table_x[i] = CONFIG_LTDC_ATK7V13_DEFAULT_HEIGHT - (((uint16_t)(buf[0] & 0X0F) << 8) + buf[1]);
                        abs_table_y[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
                    }
                    report = 1;
                    /*send event*/
                    event.press_status = TS_PRESS;
                    event.type         = ABS_XY;
                    event.x_abs        = abs_table_x[i];
                    event.y_abs        = abs_table_y[i];
                    if (gTouchCallback)
                    {
                        report = (*gTouchCallback)(i, abs_table_x[i], abs_table_y[i]);
                    }
                    if (report)
                    {
                        ret = put_ts_event(&event, TS_EVENT_TIMEOUT);
                        if (ret)
                        {
                            ts_warning("evnet buf is full,user should read as quickly\n");
                        }
                    }
                    ts_debug("%s:x[%d]:%d,y[%d]:%d\r\n", __FUNCTION__, i, abs_table_x[i], i, abs_table_y[i]);
                }
            }
        }

        /*set ts status*/
        ts_set_press_status(data, TS_PRESS);

        /*start delay work*/
        k_delayed_work_submit_to_queue(&data->workq, &data->delaywork, REPEAT_SCAN_PREIOD);

    }
    else
    {

        if (data->ts_press == TS_PRESS)
        {
            ts_debug("ts is release:%s\n", __FUNCTION__);
            /*send release event*/
            event.press_status = TS_RELEASE;
            event.type         = ABS_XY;
            event.x_abs        = 0;
            event.y_abs        = 0;
            ret = put_ts_event(&event, TS_EVENT_TIMEOUT);
            if (ret)
            {
                ts_warning("evnet buf is full,user should read as quickly\n");
            }
            ts_set_press_status(data, TS_RELEASE);
        }
    }
}

static void ts_ft5426_scan_thread(void *p1, void *p2, void *p3)
{
    int ret = 0;
    uint8_t mode = 0;
    bool status = TS_RELEASE;
    uint8_t i = 0;
    uint8_t buf[4];
    uint32_t abs_table_x[TS_FT5426_SLOT_CNT];
    uint32_t abs_table_y[TS_FT5426_SLOT_CNT];
    struct ts_input_event event;

    struct device *dev = p1;
    struct ft5426_data_t *data = DEV_DATA(dev);
    uint16_t *regtable = ft5426_tpx_tbl;

    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1)
    {

        k_sleep(NORMAL_SCAN_PREIOD);

        ts_set_press_status(data, TS_RELEASE);

        status = ts_ft5426_get_status(dev, &mode);
        if (status == TS_PRESS)
        {
            k_sleep(TS_REMOVE_SHARK_PERIOD);
            status = ts_ft5426_get_status(dev, &mode);
            /*convet to touch press bit map*/
            mode = ~(0xff << (mode & 0xf));
            if (status == TS_PRESS)
            {

                ts_debug("\nts press:%s\n", __FUNCTION__);
                for (i = 0; i < TS_FT5426_SLOT_CNT; i++)
                {
                    //  printk("ts_ft5426_scan_thread:mode:0x%x\n",mode);
                    if (mode & (1 << i))
                    {
                        /*slot is press*/
                        //ft5426_rd_reg(data->FT5426_TPX_TBL[i],buf,4); //read xy position
                        ft5426_read(dev, regtable[i], buf, 4); //read xy position

                        if ((buf[0] & 0Xf0) != 0x80)
                        {
                            abs_table_x[i] = abs_table_y[i] = 0;
                            ts_warning("###########waring:%s##############\n", __FUNCTION__);
                            ts_set_press_status(data, TS_PRESS);
                            continue;
                        }
                        if (data->ts_dir == TS_DIR_THWARTWISE) //horizontal screen
                        {
                            abs_table_y[i] = ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];
                            abs_table_x[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
                        }
                        else
                        {
                            abs_table_x[i] = CONFIG_LTDC_ATK7V13_DEFAULT_HEIGHT - (((uint16_t)(buf[0] & 0X0F) << 8) + buf[1]);
                            abs_table_y[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
                        }
                        /*send event*/
                        event.press_status = TS_PRESS;
                        event.type         = ABS_XY;
                        event.x_abs        = abs_table_x[i];
                        event.y_abs        = abs_table_y[i];
                        ret = put_ts_event(&event, TS_EVENT_TIMEOUT);
                        if (ret)
                        {
                            ts_warning("evnet buf is full,user should read as quickly\n");
                        }
                        /*send event*/
                        ts_debug("%s:x[%d]:%d,y[%d]:%d\r\n", __FUNCTION__, i, abs_table_x[i], i, abs_table_y[i]);
                    }

                }
                /*input ts xy asb*/
                ts_set_press_status(data, TS_PRESS);
                /*start delay work*/
                k_delayed_work_submit_to_queue(&data->workq, &data->delaywork, REPEAT_SCAN_PREIOD);
            }
        }

        if (data->ts_press == TS_PRESS)
        {
            /*wait ts relese*/
            do
            {
                k_sleep(3);
                status = ts_ft5426_get_status(dev, &mode);
            } while (status == TS_PRESS);

            /*send release event*/
            event.press_status = TS_RELEASE;
            event.type         = ABS_XY;
            event.x_abs        = 0;
            event.y_abs        = 0;
            ret = put_ts_event(&event, TS_EVENT_TIMEOUT);
            if (ret)
            {
                ts_warning("evnet buf is full,user should read as quickly\n");
            }
            ts_set_press_status(data, TS_RELEASE);
            ts_debug("ts release:%s\n", __FUNCTION__);
        }
    }
}

static const struct ts_driver_api ft5426_driver_api_funcs =
{
    .set_ts_dir      = ft5426_set_dir,
    .get_ts_dir      = ft5426_get_dir,
};

static struct ft5426_data_t ft5426_dev_data =
{
    .i2c_name = TOUCH_I2C_NAME,
    .rst_port_name = LCD_TS_RST_PORT,
    .rst_pin = LCD_TS_RST_PIN,
    .int_port_name = LCD_TS_INT_PORT,
    .int_pin = LCD_TS_INT_PIN,
    .ts_dir   = TS_DIR_THWARTWISE,//TS_DIR_ERECT,
    .ts_press = TS_RELEASE,
};

DEVICE_AND_API_INIT(ft5426, FT5426_DEV_NAME,
                    ts_ft5426_init, &ft5426_dev_data, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &ft5426_driver_api_funcs);