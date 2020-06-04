/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gt911.c
 *
 * @author      Christal.Xu
 *
 * @version     v1.0.0
 *
 * @date        2020/05/06
 *
 * @brief       The driver of touchpanel gt911
 *
 * @note
 *              2020/05/06, Christal.Xu, v1.1.0
 *
 *
 **/

#include <kernel.h>
#include <i2c.h>
#include <gpio.h>
#include <ts.h>
#include "gt911.h"

#define TS_REMOVE_SHARK_PERIOD  20
#define REPEAT_SCAN_PREIOD      10

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

const uint16_t gt911_tpx_tbl[TS_GT911_SLOT_CNT] = {GT_TP1_REG, GT_TP2_REG, GT_TP3_REG, GT_TP4_REG, GT_TP5_REG};
global_ts_cb gTouchCallback;

int gt911_repeat_scan(struct k_work *work);
static void ts_gt911_scan_thread(void *p1, void *p2, void *p3);

static int gt911_read(struct device *dev, uint16_t reg, uint8_t *value, uint8_t len)
{
    ts_gt911_claim_reg(dev);
    const struct gt911_data_t *data = DEV_DATA(dev);
    uint8_t addr_buffer[2];
    int ret = 0;
    struct i2c_msg msg[2];

    addr_buffer[0] = reg >> 8;
    addr_buffer[1] = reg & 0xFF;
    msg[0].buf = addr_buffer;
    msg[0].len = 2;
    msg[0].flags = I2C_MSG_WRITE/*|I2C_MSG_STOP*/;

    //ret = i2c_transfer(data->i2c, &msg, 1, GT911_ADDR);

    k_sleep(2);
    msg[1].buf = value;
    msg[1].len = len;
    msg[1].flags = I2C_MSG_RESTART | I2C_MSG_READ | I2C_MSG_STOP/*|I2C_MSG_STOP*/;

    ret = i2c_transfer(data->i2c, &msg, 2, GT911_ADDR);
    ts_gt911_release_reg(dev);
    return ret;
}

static int gt911_write(struct device *dev, uint16_t reg, uint8_t value, int len)
{
    ts_gt911_claim_reg(dev);
    const struct gt911_data_t *data = DEV_DATA(dev);
    uint8_t buf[3];
    struct i2c_msg msg;
    int ret = 0;

    buf[0] = reg >> 8;
    buf[1] = reg & 0xFF;
    buf[2] = value;
    msg.buf = buf;
    msg.len = len + 2;
    msg.flags = I2C_MSG_WRITE | I2C_MSG_STOP;

    ret = i2c_transfer(data->i2c, &msg, 1, GT911_ADDR);
    ts_gt911_release_reg(dev);
    return ret;
}

/*
* Write a list of register settings;
*/
static int gt911_write_array(struct device *dev, const struct regval_list *regs, int array_size)
{
    int i = 0;
    int ret = 0;

    if (!regs)
    {
        return -ENOTSUP;
    }

    while (i < array_size)
    {
        ret = gt911_write(dev, regs->reg, regs->val, 1);
        if (ret != 0)
        {
            return -ENOTSUP;
        }

        i++;
        regs++;
    }
    return 0;
}

const uint8_t GT911_CFG_TBL[] =
{
    0x50, 0x20, 0x03, 0xE0, 0x01, 0x05, 0x0D, 0x00, 0x01, 0x08,
    0x28, 0x05, 0x50, 0x32, 0x03, 0x05, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8C, 0x28, 0x0C,
    0x3B, 0x3D, 0x0C, 0x08, 0x00, 0x00, 0x01, 0x02, 0x03, 0x1D,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x64, 0x32, 0x00, 0x00,
    0x00, 0x1E, 0x50, 0x94, 0xD5, 0x02, 0x07, 0x00, 0x00, 0x04,
    0x9B, 0x21, 0x00, 0x72, 0x28, 0x00, 0x57, 0x31, 0x00, 0x42,
    0x3B, 0x00, 0x35, 0x48, 0x00, 0x35, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14,
    0x16, 0x18, 0x1A, 0x1C, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0F,
    0x10, 0x12, 0x13, 0x14, 0x16, 0x18, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xB5, 0x01
};

static int gt911_send_cfg(struct device *dev, uint8_t mode)
{
    uint8_t buf[2];
    uint8_t i = 0;
    buf[0] = 0;
    buf[1] = mode;
    for (i = 0; i < sizeof(GT911_CFG_TBL); i++)
    {
        buf[0] += GT911_CFG_TBL[i];
        gt911_write(dev, GT_CFGS_REG + i, GT911_CFG_TBL[i], 1);
    }
    buf[0] = (~buf[0]) + 1;
    gt911_write(dev, GT_CHECK_REG, buf[0], 1);
    gt911_write(dev, GT_CHECK_REG + 1, buf[1], 1);
    return 0;
}

static int gt911_read_cfg(struct device *dev)
{
    uint8_t i = 0;
    uint8_t k = 0;
    uint8_t tmp = 0;
    for (i = 0; i < 184; i++)
    {
        gt911_read(dev, GT_CFGS_REG + i, &tmp, 1);
        printf("0x%02X,", tmp);
        k++;
        if (k == 10)
        {
            printf("\n");
            k = 0;
        }
    }
    printf("\n");
    gt911_read(dev, GT_CHECK_REG, &tmp, 1);
    printf("CHECK_REG[0]:%X\n", tmp);
    gt911_read(dev, GT_CHECK_REG + 1, &tmp, 1);
    printf("CHECK_REG[1]:%X\n", tmp);
}

static int gt911_init(struct device *dev)
{
    struct gt911_data_t *data = DEV_DATA(dev);
    uint8_t ret = 0;
    uint8_t id_tmp[3];
    uint32_t id;
    uint8_t tmp;
    struct device *rst_gpio, *int_gpio;

    data->i2c = device_get_binding(data->i2c_name);
    if (data->i2c == NULL)
    {
        return -ENOTSUP;
    }

    i2c_configure(data->i2c, 0x14);
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

    data->int_port = device_get_binding(data->int_port_name);
    if (data->int_port)
    {
        gpio_pin_configure(data->int_port, data->int_pin, GPIO_DIR_OUT);
    }

    gpio_pin_write(data->rst_port, data->rst_pin, 0);
    gpio_pin_write(data->int_port, data->int_pin, 1);
    k_sleep(1);
    gpio_pin_write(data->rst_port, data->rst_pin, 1);
    k_sleep(10);
    gpio_pin_write(data->int_port, data->int_pin, 0);
    k_sleep(55);
    /* Set INT as input */
    gpio_pin_configure(data->int_port, data->int_pin, GPIO_DIR_IN);

    gt911_read(dev, GT_ID_REG, id_tmp, 3);//ascii 9 1 1
    id = id_tmp[2] | id_tmp[1] << 8 | id_tmp[0] << 16;
    printf("id:%x", id);
    if (id != GT911_ID)
    {
        printf("read id error\n");
        return -ENOTSUP;
    }
    gt911_write(dev, GT_CTRL_REG, 0x02, 1); //soft reset
    //gt911_read(GT_CFGS_REG,&tmp);
    /*update version if needed, new LCD don't need, default version is 0x41
    when need to update version, config 0x8047~0x80FE, and calculate 0x80FF,
    then write 0x01 to 0x80FF to save config to LCD flash.
    */
    if (0/*tmp<0x60*/)
    {
        printf("VERSION: 0x%X\n", tmp);
        gt911_send_cfg(dev, 1);
        gt911_read_cfg(dev);
    }
    k_sleep(10);
    gt911_write(dev, GT_CTRL_REG, 0x00, 1);//end soft reset
    //GT_Read_Cfg();//debug use
    return ret;
}

static void gt911_set_dir(struct device *dev, bool dir)
{
    struct gt911_data_t *data = DEV_DATA(dev);

    data->ts_dir = dir;
}

static bool gt911_get_dir(struct device *dev)
{
    struct gt911_data_t *data = DEV_DATA(dev);

    return data->ts_dir;
}

static int ts_gt911_delaywork_init(struct device *dev)
{
    int ret = 0;

    struct gt911_data_t *data = DEV_DATA(dev);

    k_delayed_work_init(&data->delaywork, gt911_repeat_scan);

    k_work_q_start(&data->workq, wrokq_tack, WORKQ_STACK_SIZE, 0);

    return ret;
}

static int ts_gt911_init(struct device *dev)
{
    int ret = 0;

    /* mutex for mul task access sd hardware*/
    ts_gt911_claim_init(dev);

    ret = gt911_init(dev);
    if (ret)
    {
        ts_err("%s: Err:%d GT911_Init\n", __FUNCTION__, ret);
        return ret;
    }

    /*init delaywork for repeat scan*/
    ts_gt911_delaywork_init(dev);


    /*start a thead for scan*/

    k_thread_create(&ts_thread, ts_scan_tack,
                    TS_SCAN_STACK_SIZE,
                    (k_thread_entry_t)ts_gt911_scan_thread,
                    (void *)dev, 0, 0,
                    -1, 0, K_NO_WAIT);
    return ret;
}

static bool ts_gt911_get_status(struct device *dev, uint8_t *status)
{
    gt911_read(dev, GT_GSTID_REG, status, 1);
    k_sleep(10);
    gt911_write(dev, GT_GSTID_REG, 0x00, 1);
    if ((*status & 0xf) && ((*status & 0xf) < 6))
    {
        return TS_PRESS;
    }
    else
    {
        return TS_RELEASE;
    }
}

static void ts_set_press_status(struct gt911_data_t *data, uint8_t status)
{
    uint8_t key = 0;
    key = irq_lock();
    data->ts_press = status;
    irq_unlock(key);
}

int gt911_repeat_scan(struct k_work *work)
{
    int ret = 0;
    bool status;
    uint8_t mode;
    uint8_t i = 0;
    uint8_t report = 1;
    uint8_t buf[6];
    uint16_t *regtable = gt911_tpx_tbl;
    uint32_t abs_table_x[TS_GT911_SLOT_CNT];
    uint32_t abs_table_y[TS_GT911_SLOT_CNT];
    struct ts_input_event event;
    struct gt911_data_t *data = CONTAINER_OF(work, struct gt911_data_t, delaywork);
    struct device *dev = device_get_binding(GT911_DEV_NAME);

    status = ts_gt911_get_status(dev, &mode);
    if (status == TS_PRESS)
    {
        k_sleep(TS_REMOVE_SHARK_PERIOD);
        status = ts_gt911_get_status(dev, &mode);
        if (status == TS_PRESS)
        {
            ts_debug("\nts is press:%s\n", __FUNCTION__);
            /*convet to touch press bit map*/
            mode = ~(0xff << (mode & 0xf));

            /*ts press ,send event*/
            for (i = 0; i < TS_GT911_SLOT_CNT; i++)
            {
                // printk("gt911_repeat_scan:mode:0x%x\n",mode);
                if (mode & (1 << i))
                {
                    /*slot is press*/
                    gt911_read(dev, regtable[i], buf, 4); //read xy postion
                    if (data->ts_dir == TS_DIR_THWARTWISE) //horizontal screen
                    {
                        abs_table_x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        abs_table_y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                    }
                    else
                    {
                        abs_table_y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        abs_table_x[i] = 480 - (((uint16_t)buf[3] << 8) + buf[2]);
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

static void ts_gt911_scan_thread(void *p1, void *p2, void *p3)
{
    int ret = 0;
    uint8_t mode = 0;
    bool status = TS_RELEASE;
    uint8_t i = 0;
    uint8_t buf[4];
    uint32_t abs_table_x[TS_GT911_SLOT_CNT];
    uint32_t abs_table_y[TS_GT911_SLOT_CNT];
    struct ts_input_event event;

    struct device *dev = p1;
    struct gt911_data_t *data = DEV_DATA(dev);
    uint16_t *regtable = gt911_tpx_tbl;

    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1)
    {

        k_sleep(NORMAL_SCAN_PREIOD);

        ts_set_press_status(data, TS_RELEASE);

        status = ts_gt911_get_status(dev, &mode);
        if (status == TS_PRESS)
        {
            k_sleep(TS_REMOVE_SHARK_PERIOD);
            status = ts_gt911_get_status(dev, &mode);
            /*convet to touch press bit map*/
            mode = ~(0xff << (mode & 0xf));

            if (status == TS_PRESS)
            {

                ts_debug("\nts press:%s\n", __FUNCTION__);
                for (i = 0; i < TS_GT911_SLOT_CNT; i++)
                {
                    //  printk("ts_gt911_scan_thread:mode:0x%x\n",mode);
                    if (mode & (1 << i))
                    {
                        /*slot is press*/
                        gt911_read(dev, regtable[i], buf, 4); //read xy position
                        if (data->ts_dir == TS_DIR_THWARTWISE) //horizontal screen
                        {
                            abs_table_x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                            abs_table_y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                        }
                        else
                        {
                            abs_table_y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                            abs_table_x[i] = 480 - (((uint16_t)buf[3] << 8) + buf[2]);
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
                k_sleep(10);
                status = ts_gt911_get_status(dev, &mode);
                //printf("status:%x\n", status);
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

static const struct ts_driver_api gt911_drv_api_funcs =
{
    .set_ts_dir      = gt911_set_dir,
    .get_ts_dir      = gt911_get_dir,
};



static struct gt911_data_t gt911_dev_data =
{
    .i2c_name = TOUCH_I2C_NAME,
    .rst_port_name = LCD_TS_RST_PORT,
    .rst_pin = LCD_TS_RST_PIN,
    .int_port_name = LCD_TS_INT_PORT,
    .int_pin = LCD_TS_INT_PIN,
    .ts_dir   = TS_DIR_THWARTWISE,//TS_DIR_ERECT,
    .ts_press = TS_RELEASE,
};

DEVICE_AND_API_INIT(gt911, GT911_DEV_NAME,
                    ts_gt911_init, &gt911_dev_data, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &gt911_drv_api_funcs);
