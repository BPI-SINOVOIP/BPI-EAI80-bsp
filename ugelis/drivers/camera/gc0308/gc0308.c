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
#include "camera/gpio_sccb.h"
#include "gc0308_reg.h"

#define GC0308_ID      0x9B
#define GC0308_ADDR    0x42

struct regval_list
{
    uint8_t reg;
    uint8_t val;
};

/** Runtime driver data */
struct gc0308_data
{
    struct device *sccb;
    struct device *rst_port;
    struct device *pwdn_port;
    char *rst_port_name;
    char *pwdn_port_name;
    char *sccb_name;
    uint8_t  rst_pin;
    uint8_t  pwdn_pin;
    uint16_t pic_width;
    uint16_t pic_height;
    uint8_t  framerate;
    enum camera_output_format pixfmt;
};

/* convenience defines */
#define DEV_DATA(dev)                           \
    ((struct gc0308_data * const)(dev)->driver_data)

/*
 * The default register settings
 *
 */
static const struct regval_list gc0308_init_default_setting[] =
{
    {0xfe, 0x00},   //soft reset and page select [7]:reset [0]:page
    {0x01, 0x0a},   //H blanking
    {0x02, 0x0c},   //V blanking
    {0xe3, 0x7d},   //

    /*exposure level*/
    {0xe4, 0x02},   //level 1 high
    {0xe5, 0x71},   //level 1 low
    {0xe6, 0x03},   //level 2 high
    {0xe7, 0xe8},   //level 2 low
    {0xe8, 0x07},   //level 3 high
    {0xe9, 0x53},   //level 3 low
    {0xea, 0x0d},   //level 4 high
    {0xeb, 0x2f},   //level 4 low

    {0x05, 0x00},   //row start
    {0x06, 0x00},
    {0x07, 0x00},   //column start
    {0x08, 0x00},
    {0x09, 0x01},   //window heigh
    {0x0a, 0xe8},
    {0x0b, 0x02},   //window width
    {0x0c, 0x88},
    {0x0d, 0x02},   //row time from frame start to first HSYNC valid
    {0x0e, 0x02},   //row time from last HSYNC valid to frame end
    {0x0f, 0x01},   //vb and hb
    {0x10, 0x22},   //rsh width
    {0x11, 0xfd},   //tsp width
    {0x12, 0x2a},   //sh delay
    {0x13, 0x00},
    {0x14, 0x13},   //CISCTL mode1:mirror, upside down
    {0x15, 0x0a},
    {0x16, 0x05},
    {0x17, 0x01},
    {0x18, 0x44},
    {0x19, 0x44},
    {0x1a, 0x1e},
    {0x1b, 0x00},
    {0x1c, 0xc1},
    {0x1d, 0x08},
    {0x1e, 0x60},
    {0x1f, 0x1a},
    {0x20, 0xff},   //block enable
    {0x21, 0xf8},
    {0x22, 0x57},
    {0x24, 0xa2},   //output format
    {0x25, 0x0f},   //output enable
    {0x26, 0x02},

    {0x2f, 0x01},   //debug mode

    {0x30, 0xf7},
    {0x31, 0x40},
    {0x32, 0x00},
    {0x39, 0x08},
    {0x3a, 0x20},
    {0x3b, 0x20},
    {0x3c, 0x00},
    {0x3d, 0x00},
    {0x3e, 0x00},
    {0x3f, 0x00},
    {0x50, 0x10},
    {0x53, 0x82},
    {0x54, 0x80},
    {0x55, 0x80},
    {0x56, 0x82},
    {0x8b, 0x40},
    {0x8c, 0x40},
    {0x8d, 0x40},
    {0x8e, 0x2e},
    {0x8f, 0x2e},
    {0x90, 0x2e},
    {0x91, 0x3c},
    {0x92, 0x50},
    {0x5d, 0x12},
    {0x5e, 0x1a},
    {0x5f, 0x24},
    {0x60, 0x07},
    {0x61, 0x15},
    {0x62, 0x08},
    {0x64, 0x03},
    {0x66, 0xe8},
    {0x67, 0x86},
    {0x68, 0xa2},
    {0x69, 0x18},
    {0x6a, 0x0f},
    {0x6b, 0x00},
    {0x6c, 0x5f},
    {0x6d, 0x8f},
    {0x6e, 0x55},
    {0x6f, 0x38},
    {0x70, 0x15},
    {0x71, 0x33},
    {0x72, 0xdc},
    {0x73, 0x80},
    {0x74, 0x02},
    {0x75, 0x3f},
    {0x76, 0x02},
    {0x77, 0x36},
    {0x78, 0x88},
    {0x79, 0x81},
    {0x7a, 0x81},
    {0x7b, 0x22},
    {0x7c, 0xff},
    {0x93, 0x48},
    {0x94, 0x00},
    {0x95, 0x05},
    {0x96, 0xe8},
    {0x97, 0x40},
    {0x98, 0xf0},
    {0xb1, 0x38},
    {0xb2, 0x38},
    {0xbd, 0x38},
    {0xbe, 0x36},
    {0xd0, 0xc9},
    {0xd1, 0x10},
    {0xd2, 0x90},
    {0xd3, 0x80},
    {0xd5, 0xf2},
    {0xd6, 0x16},
    {0xdb, 0x92},
    {0xdc, 0xa5},
    {0xdf, 0x23},
    {0xd9, 0x00},
    {0xda, 0x00},
    {0xe0, 0x09},
    {0xec, 0x20},
    {0xed, 0x04},
    {0xee, 0xa0},
    {0xef, 0x40},
    {0x80, 0x03},
    {0x80, 0x03},
    {0x9f, 0x10},
    {0xa0, 0x20},
    {0xa1, 0x38},
    {0xa2, 0x4e},
    {0xa3, 0x63},
    {0xa4, 0x76},
    {0xa5, 0x87},
    {0xa6, 0xa2},
    {0xa7, 0xb8},
    {0xa8, 0xca},
    {0xa9, 0xd8},
    {0xaa, 0xe3},
    {0xab, 0xeb},
    {0xac, 0xf0},
    {0xad, 0xf8},
    {0xae, 0xfd},
    {0xaf, 0xff},
    {0xc0, 0x00},
    {0xc1, 0x10},
    {0xc2, 0x1c},
    {0xc3, 0x30},
    {0xc4, 0x43},
    {0xc5, 0x54},
    {0xc6, 0x65},
    {0xc7, 0x75},
    {0xc8, 0x93},
    {0xc9, 0xb0},
    {0xca, 0xcb},
    {0xcb, 0xe6},
    {0xcc, 0xff},
    {0xf0, 0x02},
    {0xf1, 0x01},
    {0xf2, 0x01},
    {0xf3, 0x30},
    {0xf9, 0x9f},
    {0xfa, 0x78},
    {0xfe, 0x01},   //
    {0x00, 0xf5},
    {0x02, 0x1a},
    {0x0a, 0xa0},
    {0x0b, 0x60},
    {0x0c, 0x08},
    {0x0e, 0x4c},
    {0x0f, 0x39},
    {0x11, 0x3f},
    {0x12, 0x72},
    {0x13, 0x13},
    {0x14, 0x42},
    {0x15, 0x43},
    {0x16, 0xc2},
    {0x17, 0xa8},
    {0x18, 0x18},
    {0x19, 0x40},
    {0x1a, 0xd0},
    {0x1b, 0xf5},
    {0x70, 0x40},
    {0x71, 0x58},
    {0x72, 0x3},
    {0x73, 0x48},
    {0x74, 0x20},
    {0x75, 0x60},
    {0x77, 0x20},
    {0x78, 0x32},
    {0x30, 0x03},
    {0x31, 0x40},
    {0x32, 0xe0},
    {0x33, 0xe0},
    {0x34, 0xe0},
    {0x35, 0xb0},
    {0x36, 0xc0},
    {0x37, 0xc0},
    {0x38, 0x04},
    {0x39, 0x09},
    {0x3a, 0x12},
    {0x3b, 0x1c},
    {0x3c, 0x28},
    {0x3d, 0x31},
    {0x3e, 0x44},
    {0x3f, 0x57},
    {0x40, 0x6c},
    {0x41, 0x81},
    {0x42, 0x94},
    {0x43, 0xa7},
    {0x44, 0xb8},
    {0x45, 0xd6},
    /*crop window 0x46~0x4c*/
    {0x46, 0xee},   //crop window
    {0x47, 0x0d},   //crop window
    //{0x48,},  //
    {0xfe, 0x00},

};

static const struct regval_list gc0308_setting_30fps_QVGA_320_240[] =
{
    {0xfe, 0x00},   //select page 0
    {0x28, 0x00},   //clock
    {0x46, 0x80},   //crop window enable[7],[5:4]y0[9:8],[2:0]x0[10:8]
    {0x47, 0x00},   //y0[7:0]
    {0x48, 0x00},   //x0[7:0]
    {0x49, 0x00},   //crop height,[0]height[8]
    {0x4a, 0xf0},   //crop height,height[7:0]
    {0x4b, 0x01},   //crop width,[1:0]width[9:8]
    {0x4c, 0x40},   //crop width,width[7:0]

    /*sub sample 4:1 to scale img*/
    {0xfe, 0x01},   //select page 1
    {0x54, 0x22},   //subsample
    {0x55, 0x01},   //submode
    {0x56, 0x00},   //sub_row_N1
    {0x57, 0x00},   //sub_row_N2
    {0x58, 0x00},   //sub_col_N1
    {0x59, 0x00},   //sub_col_N2

    //{0x50,0x00},  //frame close
    //{0x51,0x00},  //frame close
    //{0x52,0x00},  //frame close
};

static const struct regval_list gc0308_setting_15fps_QVGA_320_240[] =
{
    {0xfe, 0x00},   //select page 0
    {0x28, 0x10},   //clock
    {0x46, 0x80},   //crop window enable[7],[5:4]y0[9:8],[2:0]x0[10:8]
    {0x47, 0x00},   //y0[7:0]
    {0x48, 0x00},   //x0[7:0]
    {0x49, 0x00},   //crop height,[0]height[8]
    {0x4a, 0xf0},   //crop height,height[7:0]
    {0x4b, 0x01},   //crop width,[1:0]width[9:8]
    {0x4c, 0x40},   //crop width,width[7:0]

    /*sub sample 4:1 to scale img*/
    {0xfe, 0x01},   //select page 1
    {0x54, 0x22},   //subsample
    {0x55, 0x01},   //submode
    {0x56, 0x00},   //sub_row_N1
    {0x57, 0x00},   //sub_row_N2
    {0x58, 0x00},   //sub_col_N1
    {0x59, 0x00},   //sub_col_N2
};

static const struct regval_list gc0308_setting_30fps_VGA_640_480[] =
{
    {0xfe, 0x00},   //select page 0
    {0x28, 0x00},   //clock
    {0x46, 0x80},   //crop window enable[7],[5:4]y0[9:8],[2:0]x0[10:8]
    {0x47, 0x00},   //y0[7:0]
    {0x48, 0x00},   //x0[7:0]
    {0x49, 0x01},   //crop height,[0]height[8]
    {0x4a, 0xe0},   //crop height,height[7:0]
    {0x4b, 0x02},   //crop width,[1:0]width[9:8]
    {0x4c, 0x80},   //crop width,width[7:0]

    /*sub sample to scale img*/
    {0xfe, 0x01},   //select page 1
    {0x54, 0x11},   //subsample
    {0x55, 0x00},   //submode
    {0x56, 0x00},   //sub_row_N1
    {0x57, 0x00},   //sub_row_N2
    {0x58, 0x00},   //sub_col_N1
    {0x59, 0x00},   //sub_col_N2
};

static const struct regval_list gc0308_setting_15fps_VGA_640_480[] =
{
    {0xfe, 0x00},   //select page 0
    {0x28, 0x10},   //clock
    {0x46, 0x80},   //crop window enable[7],[5:4]y0[9:8],[2:0]x0[10:8]
    {0x47, 0x00},   //y0[7:0]
    {0x48, 0x00},   //x0[7:0]
    {0x49, 0x01},   //crop height,[0]height[8]
    {0x4a, 0xe0},   //crop height,height[7:0]
    {0x4b, 0x02},   //crop width,[1:0]width[9:8]
    {0x4c, 0x80},   //crop width,width[7:0]

    /*sub sample to scale img*/
    {0xfe, 0x01},   //select page 1
    {0x54, 0x11},   //subsample
    {0x55, 0x00},   //submode
    {0x56, 0x00},   //sub_row_N1
    {0x57, 0x00},   //sub_row_N2
    {0x58, 0x00},   //sub_col_N1
    {0x59, 0x00},   //sub_col_N2
};

/*
 * The white balance setttings
 */
static const struct regval_list gc0308_wb_auto_regs[] =
{
    {0xfe, 0x00 },  //select page 0
    {0x22, 0x57 },  //awb enable
};

static const struct regval_list gc0308_wb_sunny_regs[] =
{
    {0xfe, 0x00 },  //select page 0
    {0x22, 0x57 },  //awb enable
    {0x5a, 0x30 },  //AWB R gain, default 0x50
    {0x5b, 0x40 },  //AWB G gain, default 0x40
    {0x5c, 0x30 },  //AWB B gain, default 0x48
};

static const struct regval_list gc0308_wb_office_regs[] =
{
    {0xfe, 0x00 },
    {0x22, 0x30 },
    {0x5a, 0x30 },
    {0x5b, 0x40 },
    {0x5c, 0x52 },
};

static const struct regval_list gc0308_wb_cloudy_regs[] =
{
    {0xfe, 0x00 },
    {0x22, 0x30 },
    {0x5a, 0x38 },
    {0x5b, 0x40 },
    {0x5c, 0x36 },
};

static const struct regval_list gc0308_wb_home_regs[] =
{
    {0xfe, 0x00 },
    {0x22, 0x30 },
    {0x5a, 0x28 },
    {0x5b, 0x40 },
    {0x5c, 0x58 },
};

static int gc0308_read(struct device *dev, uint8_t reg, uint8_t *value)
{
    const struct gc0308_data *data = DEV_DATA(dev);
    sccb_start(data->sccb);
    sccb_sendbyte(data->sccb, GC0308_ADDR);
    sccb_sendbyte(data->sccb, reg);
    sccb_stop(data->sccb);

    sccb_start(data->sccb);
    sccb_sendbyte(data->sccb, GC0308_ADDR | 0x01);
    *value = sccb_readbyte(data->sccb);
    sccb_nack(data->sccb);
    sccb_stop(data->sccb);

    return 0;
}

static int gc0308_write(struct device *dev, uint8_t reg, uint8_t value)
{
    const struct gc0308_data *data = DEV_DATA(dev);
    int res = 0;

    sccb_start(data->sccb);

    if (sccb_sendbyte(data->sccb, GC0308_ADDR))
    {
        res = 1;
    }
    if (sccb_sendbyte(data->sccb, reg))
    {
        res = 1;
    }

    if (sccb_sendbyte(data->sccb, value))
    {
        res = 1;
    }

    sccb_stop(data->sccb);

    return res;
}

/*
* Write a list of register settings;
*/
static int gc0308_write_array(struct device *dev, const struct regval_list *regs, int array_size)
{
    int i = 0;
    int ret = 0;

    if (!regs)
    {
        return -ENOTSUP;
    }

    while (i < array_size)
    {
        ret = gc0308_write(dev, regs->reg, regs->val);
        if (ret != 0)
        {
            return -ENOTSUP;
        }

        i++;
        regs++;
    }

    return 0;
}

static int gc0308_set_brightness(struct device *dev, int value)
{
    int ret;
    uint8_t bright_value = 0;

    switch (value)
    {
        case 4:
            bright_value = 0xf8;
            break;
        case 3:
            bright_value = 0xb8;
            break;
        case 2:
            bright_value = 0x88;
            break;
        case 1:
            bright_value = 0x68;
            break;
        case 0:
            bright_value = 0x48;
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
    ret = gc0308_write(dev, 0xfe, 0x00);
    ret = gc0308_write(dev, 0xd2, 0x80);
    ret = gc0308_write(dev, 0xd4, 0x00);
    ret = gc0308_write(dev, 0xd3, bright_value);

    if (ret < 0)
    {
        return ret;
    }

    return 0;
}

static int gc0308_set_contrast(struct device *dev, int value)
{
    int ret = -1;

    ret = gc0308_write(dev, 0xfe, 0x00);
    ret = gc0308_write(dev, 0xb4, 0x80);
    if (value >= -4 && value <= 4)
    {
        ret = gc0308_write(dev, 0xb3, ((4 + value) << 4));
    }

    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int gc0308_set_saturation(struct device *dev, int value)
{
    int ret = -1;

    ret = gc0308_write(dev, 0xfe, 0x00);
    if (value >= -4 && value <= 4)
    {
        ret = gc0308_write(dev, 0xb1, (value + 4) << 4);
        ret = gc0308_write(dev, 0xb2, (value + 4) << 4);
    }

    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int gc0308_set_wb(struct device *dev, enum light_mode value)
{
    int ret;

    switch (value)
    {
        case WB_AUTO:
            ret = gc0308_write_array(dev, gc0308_wb_auto_regs, ARRAY_SIZE(gc0308_wb_auto_regs));
            break;
        case WB_SUNNY:
            ret = gc0308_write_array(dev, gc0308_wb_sunny_regs, ARRAY_SIZE(gc0308_wb_sunny_regs));
            break;
        case WB_OFFICE:
            ret = gc0308_write_array(dev, gc0308_wb_office_regs, ARRAY_SIZE(gc0308_wb_office_regs));
            break;
        case WB_CLOUDY:
            ret = gc0308_write_array(dev, gc0308_wb_cloudy_regs, ARRAY_SIZE(gc0308_wb_cloudy_regs));
            break;
        case WB_HOME:
            ret = gc0308_write_array(dev, gc0308_wb_home_regs, ARRAY_SIZE(gc0308_wb_home_regs));
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
static int gc0308_restore_mode(struct device *dev)
{
    return gc0308_write_array(dev, gc0308_init_default_setting, ARRAY_SIZE(gc0308_init_default_setting));
}

static int gc0308_init_controls(struct device *dev)
{
    gc0308_set_wb(dev, WB_AUTO);
    gc0308_set_saturation(dev, 0);
    gc0308_set_brightness(dev, 0);
    gc0308_set_contrast(dev, 0);

    return 0;
}


static void gc0308_reset_power(struct device *dev)
{
    const struct gc0308_data *data = DEV_DATA(dev);
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
static inline int gc0308_set_power(struct device *dev, bool on)
{
    int ret = 0;
    uint8_t id;
    uint16_t mid = 0;
    uint16_t pid = 0;

    if (on)
    {
        gc0308_reset_power(dev);
        k_sleep(10);

        /* Read sensor Model ID */
        gc0308_read(dev, 0x00, &id);

        if (id != GC0308_ID)
        {
            printk("read mid fail \n");
            ret = -ENOTSUP;
        }

        ret = gc0308_restore_mode(dev);

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
static inline int gc0308_set_fmt(struct device *dev, enum camera_output_format format, enum camera_mode_id id, enum camera_frame_rate frame_rate)
{
    struct gc0308_data *data = DEV_DATA(dev);
    uint8_t reg;
    uint16_t w, h;
    int ret = 0;

    ret = gc0308_read(dev, OUTPUT_FMT, &reg);

    switch (id)
    {
        case MODE_QVGA_320_240:
            if (frame_rate == FPS_15)
            {
                ret = gc0308_write_array(dev, gc0308_setting_15fps_QVGA_320_240, ARRAY_SIZE(gc0308_setting_15fps_QVGA_320_240));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 320;
                    data->pic_height = 240;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = gc0308_write_array(dev, gc0308_setting_30fps_QVGA_320_240, ARRAY_SIZE(gc0308_setting_30fps_QVGA_320_240));
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
                ret = gc0308_write_array(dev, gc0308_setting_15fps_VGA_640_480, ARRAY_SIZE(gc0308_setting_15fps_VGA_640_480));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 640;
                    data->pic_height = 480;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = gc0308_write_array(dev, gc0308_setting_30fps_VGA_640_480, ARRAY_SIZE(gc0308_setting_30fps_VGA_640_480));
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
            ret = gc0308_write(dev, 0xfe, 0x00);
            reg = OUTPUT_SET_FMT(reg, OUTPUT_FMT_RGB565);
            ret = gc0308_write(dev, OUTPUT_FMT, reg);
            ret = gc0308_write(dev, 0x26, 0x02);
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_RGB565;
            }
            break;
        case VIDEO_FMT_YUV422:
            ret = gc0308_write(dev, 0xfe, 0x00);
            reg = OUTPUT_SET_FMT(reg, OUTPUT_FMT_YUV2);
            ret = gc0308_write(dev, OUTPUT_FMT, reg);
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_YUV422;
            }
            break;
        case VIDEO_FMT_RAW:
            ret = gc0308_write(dev, 0xfe, 0x00);
            reg = OUTPUT_SET_FMT(reg, OUTPUT_FMT_BAYER);
            ret = gc0308_write(dev, OUTPUT_FMT, reg);
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
static int gc0308_set_framesize(struct device *dev, uint16_t xsize, uint16_t ysize)
{
    struct gc0308_data *data = DEV_DATA(dev);

    data->pic_width = xsize;
    data->pic_height = ysize;

    gc0308_write(dev, 0x4b, (uint8_t)(xsize >> 8));
    gc0308_write(dev, 0x4c, (uint8_t)(xsize & 0xFF));
    gc0308_write(dev, 0x49, (uint8_t)(ysize >> 8));
    gc0308_write(dev, 0x4a, (uint8_t)(ysize & 0xFF));

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
static inline int gc0308_get_fmt(struct device *dev, struct video_format *fmt)
{
    struct gc0308_data *data = DEV_DATA(dev);

    fmt->width = data->pic_width;
    fmt->height = data->pic_height;
    fmt->framerate = data->framerate;
    fmt->pixfmt = data->pixfmt;

    return 0;
}

static int gc0308_set_exposure(struct device *dev, int exp_level)
{
    int ret;
    uint8_t reg;

    ret = gc0308_write(dev, 0xfe, 0x00);
    switch (exp_level)
    {
        case 0:
            ret = gc0308_write(dev, 0xe4, 0x02);
            ret = gc0308_write(dev, 0xe5, 0x58);
            ret = gc0308_write(dev, 0xec, 0x00);
        case 1:
            ret = gc0308_write(dev, 0xe6, 0x03);
            ret = gc0308_write(dev, 0xe7, 0x84);
            ret = gc0308_write(dev, 0xec, 0x10);
        case 2:
            ret = gc0308_write(dev, 0xe8, 0x07);
            ret = gc0308_write(dev, 0xe9, 0x08);
            ret = gc0308_write(dev, 0xec, 0x20);
        case 3:
            ret = gc0308_write(dev, 0xea, 0x0d);
            ret = gc0308_write(dev, 0xeb, 0x7a);
            ret = gc0308_write(dev, 0xec, 0x30);
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
static inline int gc0308_s_ctrl(struct device *dev, uint32_t cmd, void *arg)
{
    struct gc0308_data *data = DEV_DATA(dev);
    struct video_format *fmt;
    int ret;
    int value;

    switch (cmd)
    {
        case CAMERA_CID_BRIGHTNESS:
            value = *(int *)arg;
            ret = gc0308_set_brightness(dev, value);
            break;
        case CAMERA_CID_CONTRAST:
            value = *(int *)arg;
            ret = gc0308_set_contrast(dev, value);
            break;
        case CAMERA_CID_SATURATION:
            value = *(int *)arg;
            ret = gc0308_set_saturation(dev, value);
            break;
        case CAMERA_CID_SHARPNESS:
            break;
        case CAMERA_CID_FLASH_MODE:
            break;
        case CAMERA_CID_FOCUS_AUTO:
            break;
        case CAMERA_CID_EXPOSURE:
            value = *(int *)arg;
            ret = gc0308_set_exposure(dev, value);
            break;
        case CAMERA_CID_WHITE_BALANCE:
            value = *(enum light_mode *)arg;
            ret = gc0308_set_wb(dev, value);
            break;
        case CAMERA_CID_FRAME_SIZE:
            fmt = (struct video_format *)arg;
            ret = gc0308_set_framesize(dev, fmt->width, fmt->height);
            break;
        default:
            break;
    }

    return 0;
}

static const struct camera_driver_api gc0308_drv_api_funcs =
{
    .set_power = gc0308_set_power,
    .set_fmt = gc0308_set_fmt,
    .get_fmt = gc0308_get_fmt,
    .ioctl = gc0308_s_ctrl,
};

static int gc0308_init(struct device *dev)
{
    struct gc0308_data *data = DEV_DATA(dev);

    data->sccb = device_get_binding(data->sccb_name);

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

#ifdef CONFIG_GC0308_0
static struct gc0308_data gc0308_dev_data_0 =
{
    .sccb_name = CONFIG_CAMERA_0_SCCB_NAME,
    .pic_width = 320,
    .pic_height = 240,
    .framerate = 15,
    .pixfmt = VIDEO_FMT_RGB565,
    .rst_port_name = CAMERA_0_GPIO_RST_PORT,
    .rst_pin = CAMERA_0_GPIO_RST_PIN,
    .pwdn_port_name = CAMERA_0_GPIO_PWDN_PORT,
    .pwdn_pin = CAMERA_0_GPIO_PWDN_PIN,
};

DEVICE_AND_API_INIT(gc0308_0, GC0308_0_DEV_NAME,
                    gc0308_init, &gc0308_dev_data_0, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &gc0308_drv_api_funcs);
#endif
#ifdef CONFIG_GC0308_1
static struct gc0308_data gc0308_dev_data_1 =
{
    .sccb_name = CONFIG_CAMERA_1_SCCB_NAME,
    .pic_width = 320,
    .pic_height = 240,
    .framerate = 15,
    .pixfmt = VIDEO_FMT_RGB565,
    .rst_port_name = CAMERA_1_GPIO_RST_PORT,
    .rst_pin = CAMERA_1_GPIO_RST_PIN,
    .pwdn_port_name = CAMERA_1_GPIO_PWDN_PORT,
    .pwdn_pin = CAMERA_1_GPIO_PWDN_PIN,
};

DEVICE_AND_API_INIT(gc0308_1, GC0308_1_DEV_NAME,
                    gc0308_init, &gc0308_dev_data_1, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &gc0308_drv_api_funcs);
#endif

