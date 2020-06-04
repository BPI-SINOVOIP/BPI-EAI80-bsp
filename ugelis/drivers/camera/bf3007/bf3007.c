/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <kernel.h>
#include <arch/cpu.h>
#include <board.h>
#include <init.h>
#include <gpio.h>
#include <misc/printk.h>

#include <camera/camera_sensor.h>
#include <i2c.h>
#include "bf3007_reg.h"

#define BF3007_ID      0x30
#define BF3007_VER     0x0a
#define BF3007_ADDR    0x6e

struct regval_list
{
    uint8_t reg;
    uint8_t val;
};

/** Runtime driver data */
struct bf3007_data
{
    struct device *i2c;
    struct device *rst_port;
    struct device *pwdn_port;
    char *i2c_name;
    char *rst_port_name;
    char *pwdn_port_name;
    uint8_t  rst_pin;
    uint8_t  pwdn_pin;
    uint16_t pic_width;
    uint16_t pic_height;
    uint8_t  framerate;
    enum camera_output_format pixfmt;
};

/* convenience defines */
#define DEV_DATA(dev)                           \
    ((struct bf3007_data * const)(dev)->driver_data)

/*
 * The default register settings
 *
 */
static const struct regval_list bf3007_init_default_setting[] =
{
    {0xfe, 0x00},
    {0x01, 0x1c},
    {0xf0, 0x00}, //bit[0]:0,normal mode;1,pal or ntsc mode
    {0xf1, 0x00},
    {0xfe, 0x00},
    {0x00, 0x8e}, //bit[5]:1,mirror;bit[4]:1,flip;bit[3]:1,black sun enable
    {0x01, 0x3c},
    {0x03, 0x03}, //27M:0x00   24M:0x03
    {0x04, 0x03},
    {0x0d, 0x1a}, //Bit[7:0]:Control the falling edge of PRST for avoid black sun
    {0x10, 0x30},
    {0x5c, 0x9a},
    {0xfe, 0x01},
    {0xe1, 0x00},
    {0xe2, 0xe0},
    {0xe3, 0x53},
    {0xe5, 0x08},
    {0xe6, 0xd0},
    {0xfe, 0x00},
    {0xc8, 0xc0},
    {0xc9, 0x10},
    {0xca, 0x01},
    {0xcb, 0xa1},
    {0xcc, 0x00},
    {0xcd, 0x78},
    {0xce, 0xf0}, //{0xca,0xcb,0xcc,0xcd,0xce}:window
    {0xcf, 0x80},
    {0xfe, 0x01},
    {0x0b, 0x10}, //Lens shading gain of R
    {0x0c, 0x10}, //Lens shading gain of G1
    {0x0d, 0x10}, //Lens shading gain of B
    {0x0e, 0x10}, //Lens shading gain of G0
    {0xfe, 0x00},
    {0x7a, 0x20},
    {0x7c, 0x28}, //gamma offset
    {0x7d, 0x28}, //gamma offset
    {0xfe, 0x00},
    {0x80, 0x4c}, //gamma for bright scene
    {0x81, 0x3b},
    {0x82, 0x35},
    {0x83, 0x2a},
    {0x84, 0x22},
    {0x85, 0x19},
    {0x86, 0x12},
    {0x87, 0x0d},
    {0x88, 0x0b},
    {0x89, 0x09},
    {0x8a, 0x08},
    {0x8b, 0x07},
    {0x8c, 0x06},
    {0x8d, 0x05},
    {0x8e, 0x04},
    {0xfe, 0x00},
    {0x98, 0x04},
    {0x9b, 0x07}, //0x9b[7:4]:???,????;???,???;
    {0x9c, 0xd6}, //0x9c[5:4]:???,????;???,???;
    {0x9d, 0x73},
    {0x9e, 0xf4},
    {0x9f, 0x00},
    {0xa0, 0x08},
    {0xa2, 0x54}, //0xa2[7:4]:???,????;???,???;
    {0xa3, 0x78}, //??????,bit[7:4]positive,bit[3:0]negative
    {0xa7, 0x50},
    {0xa9, 0x48},
    {0xaa, 0x45}, //??????,bit[7:4]positive,bit[3:0]negative
    {0xab, 0x1f},
    {0xfe, 0x01},
    {0x64, 0x80},
    {0x65, 0x30}, //??????AE????????
    {0x66, 0x04}, //bit[3]GLB_GAIN0 written,0:disable,1:enable;bit[2]0:AGC disable,1:AGC enable;bit[0]0:AEC disable,1:enable
    {0x67, 0x09}, //bit[5:4]center window select;bit[2:0]weight selet
    {0x6b, 0x00},
    {0x6c, 0x40},
    {0x6f, 0x45}, //bit[6;0]Y target value
    {0x70, 0x65}, //0x75
    {0x72, 0xde}, //0x72[2]: 1,choose 50HZ step; 0,choose 60HZ step
    {0x74, 0x04}, //bit[5:0]:??????step?
    {0x76, 0x81}, //??????
    {0x77, 0x8b}, //50hz banding
    {0x78, 0x74}, //60hz banding
    {0x7b, 0x02},
    {0x7d, 0xb5},
    {0x7e, 0x20},
    {0x7f, 0x86},
    {0x81, 0x0C}, //GLB_MIN1 8 bits
    {0x82, 0x16}, //GLB_MAX1 8 bits
    {0x83, 0x20}, //GLB_MIN2 8 bits
    {0x84, 0x28}, //GLB_MAX2 8 bits
    {0x85, 0x70}, //GLB_MAX3 8 bits
    {0x86, 0x08},
    {0x8b, 0x3b},
    {0x8c, 0x40},
    {0xfe, 0x01},
    {0x98, 0xaa},
    {0x9b, 0x0d},
    {0x9c, 0x18},
    {0x9d, 0x0a},
    {0x9e, 0x16},
    {0x9f, 0xaa},
    {0xa1, 0x21}, //AWB
    {0xa2, 0x06},
    {0xa3, 0x3f},
    {0xa4, 0x01},
    {0xa5, 0x3b},
    {0xa7, 0x1e},
    {0xa8, 0x1c},
    {0xa9, 0x14},
    {0xaa, 0x14},
    {0xab, 0x12},
    {0xac, 0x30},
    {0xae, 0x59},
    {0xb2, 0x99},
    {0xb3, 0x22},
    {0xb4, 0x22},
    {0xb5, 0x00},
    {0xb6, 0x98}, //bit[7]:1,outdoor enable
    {0xfe, 0x02},
    {0x32, 0x1f},
    {0x33, 0x30},
    {0x2c, 0x90}, //color for bright scene
    {0x2d, 0x03},
    {0x2e, 0x86},
    {0x2f, 0x86},
    {0x30, 0x02},
    {0x31, 0x9a},
    {0x34, 0x95}, //color for outdoor
    {0x35, 0x03},
    {0x36, 0x8c},
    {0x37, 0x8c},
    {0x38, 0x02},
    {0x39, 0x9a},
    {0xfe, 0x02},
    {0x50, 0xe0},
    {0x51, 0x8f},
    {0x52, 0xf0},
    {0x53, 0xA0}, //Cb saturation for bright scene
    {0x54, 0xA0}, //Cr saturation for bright scene
    {0xfe, 0x02},
    {0x60, 0x87},
    {0x64, 0xff},
    {0x66, 0xa0}, //???
    {0x68, 0xf0}, //bit[7]:1,??????????;
    {0x67, 0x20}, //????????
    {0xfe, 0x02},
    {0x6c, 0x40},
    {0x6d, 0x23},
    {0xfe, 0x80},
    {0x7b, 0x30},
    {0x80, 0x40}, //gamma for dark scene
    {0x81, 0x2e},
    {0x82, 0x2a},
    {0x83, 0x28},
    {0x84, 0x23},
    {0x85, 0x1f},
    {0x86, 0x18},
    {0x87, 0x12},
    {0x88, 0x10},
    {0x89, 0x0c},
    {0x8a, 0x0b},
    {0x8b, 0x0a},
    {0x8c, 0x08},
    {0x8d, 0x06},
    {0x8e, 0x06},
    {0xfe, 0x82},
    {0x2c, 0x90}, //color for dark scene
    {0x2d, 0x03},
    {0x2e, 0x86},
    {0x2f, 0x86},
    {0x30, 0x03},
    {0x31, 0x98},
    {0xfe, 0x82},
    {0x53, 0xc0}, //Cb saturation for balck scene
    {0x54, 0xa0}, //Cr saturation for black scene
    {0xfe, 0x80},
    {0xa3, 0x34}, //????????
    {0x9b, 0x67}, //??????
    {0xa7, 0x00},
};

static const struct regval_list bf3007_setting_30fps_QVGA_320_240[] =
{
    {0xfe, 0x00},
    {0xca, 0x01}, //row start x4
    {0xcb, 0x51}, //row end
    {0xcc, 0x00}, //col start
    {0xcd, 0x3c}, //col end
    {0xce, 0x00},

    {0xfe, 0x01}, //
    {0xe4, 0x01}, //xclk
};

static const struct regval_list bf3007_setting_15fps_QVGA_320_240[] =
{
    {0xfe, 0x00},
    {0xca, 0x01}, //row start x4
    {0xcb, 0x51}, //row end
    {0xcc, 0x00}, //col start
    {0xcd, 0x3c}, //col end
    {0xce, 0x00},

    {0xfe, 0x01}, //
    {0xe4, 0x00}, //xclk/2
};

static const struct regval_list bf3007_setting_30fps_VGA_640_480[] =
{
    {0xfe, 0x00},
    {0xca, 0x01}, //row start x4
    {0xcb, 0xa1}, //row end
    {0xcc, 0x00}, //col start
    {0xcd, 0x78}, //col end
    {0xce, 0x00},

    {0xfe, 0x01}, //
    {0xe4, 0x01}, //xclk
};

static const struct regval_list bf3007_setting_15fps_VGA_640_480[] =
{
    {0xfe, 0x00},
    {0xca, 0x01}, //row start x4
    {0xcb, 0xa1}, //row end
    {0xcc, 0x00}, //col start
    {0xcd, 0x78}, //col end
    {0xce, 0x00},

    {0xfe, 0x01}, //
    {0xe4, 0x00}, //xclk/2
};

/*
 * The white balance setttings
 */
static const struct regval_list bf3007_wb_auto_regs[] =
{
    {0x13, 0x07},
    {0x01, 0x15},
    {0x02, 0x24},
    {0x6a, 0x81},
    {0x23, 0x66},
    {0xff, 0xff}
};

static const struct regval_list bf3007_wb_sunny_regs[] =
{
    {0x13, 0x05},
    {0x01, 0x54},
    {0x02, 0x5c},
    {0x6a, 0x81},
    {0x23, 0x66},
    {0xff, 0xff}
};

static const struct regval_list bf3007_wb_office_regs[] =
{
    {0x13, 0x05},
    {0x01, 0x1f},
    {0x02, 0x15},
    {0x6a, 0x81},
    {0x23, 0x66},
    {0xff, 0xff}
};

static const struct regval_list bf3007_wb_cloudy_regs[] =
{
    {0x13, 0x05},
    {0x01, 0x10},
    {0x02, 0x28},
    {0x6a, 0x81},
    {0x23, 0x66},
    {0xff, 0xff}
};

static const struct regval_list bf3007_wb_home_regs[] =
{
    {0x13, 0x05},
    {0x01, 0x24},
    {0x02, 0x1e},
    {0x6a, 0x81},
    {0x23, 0x66},
    {0xff, 0xff}
};

static int bf3007_read(struct device *dev, uint8_t reg, uint8_t *value)
{
    const struct bf3007_data *data = DEV_DATA(dev);
    struct i2c_msg msg;
    uint8_t *buf = NULL;
    int ret = 0;

    buf = &reg;
    msg.buf = buf;
    msg.len = 1;
    msg.flags = I2C_MSG_WRITE/*|I2C_MSG_STOP*/;

    ret = i2c_transfer(data->i2c, &msg, 1, BF3007_ADDR);
    msg.buf = value;
    msg.len = 1;
    msg.flags = I2C_MSG_READ;
    ret = i2c_transfer(data->i2c, &msg, 1, BF3007_ADDR);

    return ret;

    return 0;
}

static int bf3007_write(struct device *dev, uint8_t reg, uint8_t value)
{
    const struct bf3007_data *data = DEV_DATA(dev);
    uint8_t buf[2];
    struct i2c_msg msg;
    int ret = 0;

    buf[0] = reg;
    buf[1] = value;
    msg.buf = buf;
    msg.len = 2;
    msg.flags = I2C_MSG_WRITE | I2C_MSG_STOP;

    ret = i2c_transfer(data->i2c, &msg, 1, BF3007_ADDR);
    return ret;
}

/*
* Write a list of register settings;
*/
static int bf3007_write_array(struct device *dev, const struct regval_list *regs, int array_size)
{
    int i = 0;
    int ret = 0;

    if (!regs)
    {
        return -ENOTSUP;
    }

    while (i < array_size)
    {
        ret = bf3007_write(dev, regs->reg, regs->val);
        if (ret != 0)
        {
            return -ENOTSUP;
        }

        i++;
        regs++;
    }

    return 0;
}

static int bf3007_set_brightness(struct device *dev, int value)
{
    int ret;
    uint8_t bright_value = 0;

    switch (value)
    {
        case 4:
            bright_value = 0x60;
            break;
        case 3:
            bright_value = 0x58;
            break;
        case 2:
            bright_value = 0x50;
            break;
        case 1:
            bright_value = 0x48;
            break;
        case 0:
            bright_value = 0x40;
            break;
        case -1:
            bright_value = 0x38;
            break;
        case -2:
            bright_value = 0x28;
            break;
        case -3:
            bright_value = 0x18;
            break;
        case -4:
            bright_value = 0x08;
            break;
        default:
            return -ENOTSUP;
    }
    ret = bf3007_write(dev, 0xfe, 0x01);
    ret = bf3007_write(dev, 0x6f, bright_value);

    if (ret < 0)
    {
        return ret;
    }

    return 0;
}

static int bf3007_set_contrast(struct device *dev, int value)
{
    int ret = -1;
    int reg;
    ret = bf3007_read(dev, 0x8f, &reg);
    reg &= 0xcf;
    reg |= 0x10;
    ret = bf3007_write(dev, 0xfe, 0x00);
    ret = bf3007_write(dev, 0x8f, reg);
    if (value >= -4 && value <= 4)
    {
        ret = bf3007_write(dev, 0x7c, 0x28 + (value * 0x08));
        ret = bf3007_write(dev, 0x7d, 0x28 + (value * 0x08));
    }

    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int bf3007_set_saturation(struct device *dev, int value)
{
    int ret = -1;
    int reg;
    ret = bf3007_read(dev, 0xf1, &reg);
    reg |= 1 << 5;
    ret = bf3007_write(dev, 0xfe, 0x02);
    ret = bf3007_write(dev, 0xf1, reg);

    if (value >= -4 && value <= 4)
    {
        ret = bf3007_write(dev, 0x53, (value + 8) << 4);
        ret = bf3007_write(dev, 0x54, (value + 8) << 4);
    }

    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int bf3007_set_sharpness(struct device *dev, int value)
{
    int ret = -1;
    int reg;
    ret = bf3007_write(dev, 0xfe, 0x00);
    ret = bf3007_read(dev, 0x98, &reg);
    reg |= 0x08;
    ret = bf3007_write(dev, 0x98, 0x02);

    ret = bf3007_read(dev, 0xa7, &reg);
    reg &= 0x7f;
    ret = bf3007_write(dev, 0xa7, reg);

    if (value >= 0 && value <= 7)
    {
        ret = bf3007_write(dev, 0xa3, (value) << 4);
    }

    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int bf3007_set_wb(struct device *dev, enum light_mode value)
{
    int ret;

    switch (value)
    {
        case WB_AUTO:
            ret = bf3007_write_array(dev, bf3007_wb_auto_regs, ARRAY_SIZE(bf3007_wb_auto_regs));
            break;
        case WB_SUNNY:
            ret = bf3007_write_array(dev, bf3007_wb_sunny_regs, ARRAY_SIZE(bf3007_wb_sunny_regs));
            break;
        case WB_OFFICE:
            ret = bf3007_write_array(dev, bf3007_wb_office_regs, ARRAY_SIZE(bf3007_wb_office_regs));
            break;
        case WB_CLOUDY:
            ret = bf3007_write_array(dev, bf3007_wb_cloudy_regs, ARRAY_SIZE(bf3007_wb_cloudy_regs));
            break;
        case WB_HOME:
            ret = bf3007_write_array(dev, bf3007_wb_home_regs, ARRAY_SIZE(bf3007_wb_home_regs));
            break;
        default:
            return -ENOTSUP;
    }

    if (ret < 0)
    {
        return ret;
    }

    return 0;
}

/* restore the last set video mode after chip power-on */
static int bf3007_restore_mode(struct device *dev)
{
    return bf3007_write_array(dev, bf3007_init_default_setting, ARRAY_SIZE(bf3007_init_default_setting));
}

static int bf3007_init_controls(struct device *dev)
{
    bf3007_set_wb(dev, WB_AUTO);
    bf3007_set_saturation(dev, 0);
    bf3007_set_brightness(dev, 0);
    bf3007_set_contrast(dev, 0);

    return 0;
}


static void bf3007_reset_power(struct device *dev)
{
    const struct bf3007_data *data = DEV_DATA(dev);
    if (data->rst_port)
    {
        gpio_pin_write(data->rst_port, data->rst_pin, 0);

        gpio_pin_write(data->pwdn_port, data->pwdn_pin, 1);

        k_sleep(5);

        gpio_pin_write(data->pwdn_port, data->pwdn_pin, 0);

        k_sleep(5);

        gpio_pin_write(data->rst_port, data->rst_pin, 1);

        k_sleep(20);
    }
}

/**
 * @brief Puts subdevice in power saving mode or normal operation mode.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param on .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int bf3007_set_power(struct device *dev, bool on)
{
    int ret = 0;
    uint8_t id_high, id_low;
    uint8_t id, version;
    uint16_t mid = 0;
    uint16_t pid = 0;

    if (on)
    {
        bf3007_reset_power(dev);
        k_sleep(10);

        /* Read sensor Model ID */
        bf3007_read(dev, PID, &id);
        if (id != BF3007_ID)
        {
            printk("read mid fail \n");
            ret = -ENOTSUP;
        }

        /* Read sensor Model ID */
        bf3007_read(dev, VER, &version);
        if (version != BF3007_VER)
        {
            printk("read mid fail \n");
            ret = -ENOTSUP;
        }

        ret = bf3007_restore_mode(dev);

        k_sleep(30);

        return 0;
    }

    return ret;
}


/**
 * @brief Setting camera output format.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fmt .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int bf3007_set_fmt(struct device *dev, enum camera_output_format format, enum camera_mode_id id, enum camera_frame_rate frame_rate)
{
    struct bf3007_data *data = DEV_DATA(dev);
    uint8_t reg;
    uint16_t w, h;
    int ret = 0;
    switch (id)
    {
        case MODE_QVGA_320_240:
            if (frame_rate == FPS_15)
            {
                ret = bf3007_write_array(dev, bf3007_setting_15fps_QVGA_320_240, ARRAY_SIZE(bf3007_setting_15fps_QVGA_320_240));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 320;
                    data->pic_height = 240;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = bf3007_write_array(dev, bf3007_setting_30fps_QVGA_320_240, ARRAY_SIZE(bf3007_setting_30fps_QVGA_320_240));
                if (ret == 0)
                {
                    data->framerate = 30;
                    data->pic_width = 320;
                    data->pic_height = 240;
                }
            }
            else
            {
                return -ENOTSUP;
            }

            break;
        case MODE_VGA_640_480:
            if (frame_rate == FPS_15)
            {
                ret = bf3007_write_array(dev, bf3007_setting_15fps_VGA_640_480, ARRAY_SIZE(bf3007_setting_15fps_VGA_640_480));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 640;
                    data->pic_height = 480;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = bf3007_write_array(dev, bf3007_setting_30fps_VGA_640_480, ARRAY_SIZE(bf3007_setting_30fps_VGA_640_480));
                if (ret == 0)
                {
                    data->framerate = 30;
                    data->pic_width = 640;
                    data->pic_height = 480;
                }
            }
            else
            {
                return -ENOTSUP;
            }

            break;
        default:
            return -ENOTSUP;
    }

    switch (format)
    {
        case VIDEO_FMT_RGB565:
            return -ENOTSUP;
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_RGB565;
            }
            break;
        case VIDEO_FMT_YUV422:
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_YUV422;
            }
            break;
        case VIDEO_FMT_RAW:
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_RAW;
            }
            break;

        default:
            return -ENOTSUP;
    }

    return ret;
}

/**
 * @brief Setting camera output frame size.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param xsize       pixel number of a line.
 * @param ysize       line number.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static int bf3007_set_framesize(struct device *dev, uint16_t xsize, uint16_t ysize)
{
    struct bf3007_data *data = DEV_DATA(dev);

    data->pic_width = xsize;
    data->pic_height = ysize;

    bf3007_write(dev, 0xcb, (uint8_t)(xsize >> 2 + 1));
    bf3007_write(dev, 0xcd, (uint8_t)(ysize >> 2));

    return 0;
}

/**
 * @brief Get camera output format.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fmt .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int bf3007_get_fmt(struct device *dev, struct video_format *fmt)
{
    struct bf3007_data *data = DEV_DATA(dev);

    fmt->width = data->pic_width;
    fmt->height = data->pic_height;
    fmt->framerate = data->framerate;
    fmt->pixfmt = data->pixfmt;

    return 0;
}

static int bf3007_set_exposure(struct device *dev, int exp_level)
{
    int ret;
    uint8_t reg;

    ret = bf3007_write(dev, 0xfe, 0x00);
    switch (exp_level)
    {
        case 0:
            ret = bf3007_write(dev, 0xe4, 0x02);
            ret = bf3007_write(dev, 0xe5, 0x58);
            ret = bf3007_write(dev, 0xec, 0x00);
        case 1:
            ret = bf3007_write(dev, 0xe6, 0x03);
            ret = bf3007_write(dev, 0xe7, 0x84);
            ret = bf3007_write(dev, 0xec, 0x10);
        case 2:
            ret = bf3007_write(dev, 0xe8, 0x07);
            ret = bf3007_write(dev, 0xe9, 0x08);
            ret = bf3007_write(dev, 0xec, 0x20);
        case 3:
            ret = bf3007_write(dev, 0xea, 0x0d);
            ret = bf3007_write(dev, 0xeb, 0x7a);
            ret = bf3007_write(dev, 0xec, 0x30);
    }
    if (ret < 0)
    {
        return ret;
    }

    return 0;
}

/**
 * @brief camera io command operation.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param cmd  io command.
 * @param arg  .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int bf3007_s_ctrl(struct device *dev, uint32_t cmd, void *arg)
{
    struct bf3007_data *data = DEV_DATA(dev);
    struct video_format *fmt;
    int ret;
    int value;

    switch (cmd)
    {
        case CAMERA_CID_BRIGHTNESS:
            value = *(int *)arg;
            ret = bf3007_set_brightness(dev, value);
            break;
        case CAMERA_CID_CONTRAST:
            value = *(int *)arg;
            ret = bf3007_set_contrast(dev, value);
            break;
        case CAMERA_CID_SATURATION:
            value = *(int *)arg;
            ret = bf3007_set_saturation(dev, value);
            break;
        case CAMERA_CID_SHARPNESS:
            value = *(int *)arg;
            ret = bf3007_set_sharpness(dev, value);
            break;
        case CAMERA_CID_FLASH_MODE:
            break;
        case CAMERA_CID_FOCUS_AUTO:
            break;
        case CAMERA_CID_EXPOSURE:
            value = *(int *)arg;
            ret = bf3007_set_exposure(dev, value);
            break;
        case CAMERA_CID_WHITE_BALANCE:
            value = *(enum light_mode *)arg;
            ret = bf3007_set_wb(dev, value);
            break;
        case CAMERA_CID_FRAME_SIZE:
            fmt = (struct video_format *)arg;
            ret = bf3007_set_framesize(dev, fmt->width, fmt->height);
            break;
        default:
            break;
    }

    return 0;
}

static const struct camera_driver_api bf3007_drv_api_funcs =
{
    .set_power = bf3007_set_power,
    .set_fmt = bf3007_set_fmt,
    .get_fmt = bf3007_get_fmt,
    .ioctl = bf3007_s_ctrl,
};

static int bf3007_init(struct device *dev)
{
    struct bf3007_data *data = DEV_DATA(dev);
    uint32_t config_i2c0 = 0x14;

    data->i2c = device_get_binding(data->i2c_name);

    if (data->i2c == NULL)
    {
        return -ENOTSUP;
    }
    i2c_configure(data->i2c, config_i2c0);

    data->pwdn_port = device_get_binding(data->pwdn_port_name);

    if (data->pwdn_port)
    {
        gpio_pin_configure(data->pwdn_port, data->pwdn_pin, GPIO_DIR_OUT | GPIO_PUD_PULL_UP);
    }

    data->rst_port = device_get_binding(data->rst_port_name);

    if (data->rst_port)
    {
        gpio_pin_configure(data->rst_port, data->rst_pin, GPIO_DIR_OUT | GPIO_PUD_PULL_UP);
    }

    return 0;
}

#ifdef CONFIG_OVBF3007_0
static struct bf3007_data bf3007_dev_data_0 =
{
    .i2c_name = CONFIG_CAMERA_0_I2C_NAME,
    .pic_width = 320,
    .pic_height = 240,
    .framerate = 15,
    .pixfmt = VIDEO_FMT_RGB565,
    .rst_port_name = CAMERA_0_GPIO_RST_PORT,
    .pwdn_port_name = CAMERA_0_GPIO_PWDN_PORT,
    .rst_pin = CAMERA_0_GPIO_RST_PIN,
    .pwdn_pin = CAMERA_0_GPIO_PWDN_PIN,
};

DEVICE_AND_API_INIT(bf3007_0, BF3007_0_DEV_NAME,
                    bf3007_init, &bf3007_dev_data_0, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &bf3007_drv_api_funcs);
#endif

#ifdef CONFIG_OVBF3007_1
static struct bf3007_data bf3007_dev_data_1 =
{
    .i2c_name = CONFIG_CAMERA_1_I2C_NAME,
    .pic_width = 320,
    .pic_height = 240,
    .framerate = 15,
    .pixfmt = VIDEO_FMT_RGB565,
    .rst_port_name = CAMERA_1_GPIO_RST_PORT,
    .pwdn_port_name = CAMERA_1_GPIO_PWDN_PORT,
    .rst_pin = CAMERA_1_GPIO_RST_PIN,
    .pwdn_pin = CAMERA_1_GPIO_PWDN_PIN,
};

DEVICE_AND_API_INIT(bf3007_1, BF3007_1_DEV_NAME,
                    bf3007_init, &bf3007_dev_data_1, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &bf3007_drv_api_funcs);
#endif