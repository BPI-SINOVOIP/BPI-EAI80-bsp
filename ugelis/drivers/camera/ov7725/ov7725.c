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
#include "ov7725_reg.h"
#include "camera/gpio_sccb.h"

#define OV7725_MID     0x7FA2
#define OV7725_PID     0x7721

#define OV7725_ADDR    0x42

struct regval_list
{
    uint8_t reg;
    uint8_t val;
};

/** Runtime driver data */
struct ov7725_data
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
    ((struct ov7725_data * const)(dev)->driver_data)

/*
 * The default register settings
 *
 */
static const struct regval_list ov7725_init_default_setting[] =
{
    {COM7,      0x46}, //QVGA RGB565
    {CLKRC,     0x00}, //clock config
    {HSTART,    0x3f},
    {HSIZE,     0x50},
    {VSTRT,     0x03},
    {VSIZE,     0x78},
    {HREF,      0x00},
    {HOutSize,  0x50},
    {VOutSize,  0x78},
    {COM10,     0x02},
    {COM12,     0x03},
    {EXHCH,     0x00},

    /*DSP control*/
    {TGT_B,     0x7f},
    {FixGain,   0x09},
    {AWB_Ctrl0, 0xe0},
    {DSP_Ctrl1, 0x7f},
    {DSP_Ctrl2, 0x00},
    {DSP_Ctrl3, 0x00},
    {DSP_Ctrl4, 0x00},
    {DSPAuto,   0xff},

    /*AGC AEC AWB*/
    {COM8,      0xf0},
    {COM4,      0x81}, /*Pll AEC CONFIG*/
    {COM6,      0xc5},
    {COM9,      0x11},
    {BDBase,    0x7F},
    {BDMStep,   0x03},
    {AEW,       0x40},
    {AEB,       0x30},
    {VPT,       0xa1},
    {EXHCL,     /*0x9e*/0x00},
    {AWBCtrl3,  0xaa},
    {COM8,      0xff},

    /*matrix shapness brightness contrast*/
    {EDGE1,     0x08},
    {DNSOff,    0x01},
    {EDGE2,     0x03},
    {EDGE3,     0x00},
    {MTX1,      0xb0},
    {MTX2,      0x9d},
    {MTX3,      0x13},
    {MTX4,      0x16},
    {MTX5,      0x7b},
    {MTX6,      0x91},
    {MTX_Ctrl,  0x1e},
    {BRIGHT,    0x08},
    {CNST,      0x20},
    {UVADJ0,    0x81},
    {SDE,       0X06},
    //{UFix,      0x80},
    //{VFix,      0x80},
    {USAT,      0x65},
    {VSAT,      0x65},
    {HUECOS,    0X80},
    {HUESIN,    0X80},

    /*GAMMA config*/
    {GAM1,      0x0c},
    {GAM2,      0x16},
    {GAM3,      0x2a},
    {GAM4,      0x4e},
    {GAM5,      0x61},
    {GAM6,      0x6f},
    {GAM7,      0x7b},
    {GAM8,      0x86},
    {GAM9,      0x8e},
    {GAM10,     0x97},
    {GAM11,     0xa4},
    {GAM12,     0xaf},
    {GAM13,     0xc5},
    {GAM14,     0xd7},
    {GAM15,     0xe8},
    {SLOP,      0x20},

    // Lens Correction, should be tuned with real camera module
    {LC_CTR,        0x01}, // Enable LC and use 1 coefficient for all 3 channels
    {LC_RADI,       0x30}, // The radius of the circle where no compensation applies
    {LC_COEF,       0x30}, // RGB Lens correction coefficient

    {COM3,      0x90/*0x10*/},/*Horizontal mirror image*///[7]:V flip, [6]:H mirror

    /*night mode auto frame rate control*/
    {COM5,      0xf5},

    /*reversal*/
    //{HREF, 0x80}
};

static const struct regval_list ov7725_setting_30fps_QVGA_320_240[] =
{
    {CLKRC,     0x00},
    {COM4,      0x41},
    {HSTART,    0x3f},
    {HSIZE,     0x50},
    {VSTRT,     0x03},
    {VSIZE,     0x78},
    {HREF,      0x00},
    {HOutSize,  0x50},
    {VOutSize,  0x78},
};

static const struct regval_list ov7725_setting_15fps_QVGA_320_240[] =
{
    {CLKRC,     0x01},
    {COM4,      0x41},
    {HSTART,    0x3f},
    {HSIZE,     0x50},
    {VSTRT,     0x03},
    {VSIZE,     0x78},
    {HREF,      0x00},
    {HOutSize,  0x50},
    {VOutSize,  0x78},
};

static const struct regval_list ov7725_setting_30fps_VGA_640_480[] =
{
    {COM4,      0x41},
    {CLKRC,     0x01},
    {HSTART,    0x23},
    {HSIZE,     0xA0},
    {VSTRT,     0x07},
    {VSIZE,     0xF0},
    {HREF,      0x00},
    {HOutSize,  0xA0},
    {VOutSize,  0xF0},
    {COM5,      0x65},
};

static const struct regval_list ov7725_setting_15fps_VGA_640_480[] =
{
    {COM4,      0x41},
    {CLKRC,     0x01},
    {HSTART,    0x23},
    {HSIZE,     0xA0},
    {VSTRT,     0x07},
    {VSIZE,     0xF0},
    {HREF,      0x00},
    {HOutSize,  0xA0},
    {VOutSize,  0xF0},
};

/*
 * The white balance setttings
 */
static const struct regval_list ov7725_wb_auto_regs[] =
{
    {0x13, 0xff },
    {0x0e, 0x65 },
    {0x2d, 0x00 },
    {0x2e, 0x00 },
};

static const struct regval_list ov7725_wb_sunny_regs[] =
{
    {0x13, 0xfd },
    {0x01, 0x5a },
    {0x02, 0x5c },
    {0x0e, 0x65 },
    {0x2d, 0x00 },
    {0x2e, 0x00 },
};

static const struct regval_list ov7725_wb_office_regs[] =
{
    {0x13, 0xfd },
    {0x01, 0x84 },
    {0x02, 0x4c },
    {0x0e, 0x65 },
    {0x2d, 0x00 },
    {0x2e, 0x00 },
};

static const struct regval_list ov7725_wb_cloudy_regs[] =
{
    {0x13, 0xfd },
    {0x01, 0x58 },
    {0x02, 0x60 },
    {0x0e, 0x65 },
    {0x2d, 0x00 },
    {0x2e, 0x00 },
};

static const struct regval_list ov7725_wb_home_regs[] =
{
    {0x13, 0xfd },
    {0x01, 0x96 },
    {0x02, 0x40 },
    {0x0e, 0x65 },
    {0x2d, 0x00 },
    {0x2e, 0x00 },
};

static int ov7725_read(struct device *dev, uint8_t reg, uint8_t *value)
{
    const struct ov7725_data *data = DEV_DATA(dev);
    sccb_start(data->sccb);
    sccb_sendbyte(data->sccb, OV7725_ADDR);
    sccb_sendbyte(data->sccb, reg);
    sccb_stop(data->sccb);

    sccb_start(data->sccb);
    sccb_sendbyte(data->sccb, OV7725_ADDR | 0x01);
    *value = sccb_readbyte(data->sccb);
    sccb_nack(data->sccb);
    sccb_stop(data->sccb);

    return 0;
}

static int ov7725_write(struct device *dev, uint8_t reg, uint8_t value)
{
    const struct ov7725_data *data = DEV_DATA(dev);
    int res = 0;

    sccb_start(data->sccb);

    if (sccb_sendbyte(data->sccb, OV7725_ADDR))
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
static int ov7725_write_array(struct device *dev, const struct regval_list *regs, int array_size)
{
    int i = 0;
    int ret = 0;

    if (!regs)
    {
        return -ENOTSUP;
    }

    while (i < array_size)
    {
        ret = ov7725_write(dev, regs->reg, regs->val);
        if (ret != 0)
        {
            return -ENOTSUP;
        }

        i++;
        regs++;
    }

    return 0;
}

static int ov7725_set_brightness(struct device *dev, int value)
{
    int ret;
    uint8_t bright_value = 0;
    uint8_t sign = 0;

    switch (value)
    {
        case 4:
            bright_value = 0x48;
            sign = 0x06;
            break;
        case 3:
            bright_value = 0x38;
            sign = 0x06;
            break;
        case 2:
            bright_value = 0x28;
            sign = 0x06;
            break;
        case 1:
            bright_value = 0x18;
            sign = 0x06;
            break;
        case 0:
            bright_value = 0x08;
            sign = 0x06;
            break;
        case -1:
            bright_value = 0x08;
            sign = 0x0e;
            break;
        case -2:
            bright_value = 0x18;
            sign = 0x0e;
            break;
        case -3:
            bright_value = 0x28;
            sign = 0x0e;
            break;
        case -4:
            bright_value = 0x38;
            sign = 0x0e;
            break;
        default:
            return -ENOTSUP;
    }

    ret = ov7725_write(dev, BRIGHT, bright_value);
    ret = ov7725_write(dev, SIGN, sign);

    if (ret < 0)
    {
        return ret;
    }

    return 0;
}

static int ov7725_set_contrast(struct device *dev, int value)
{
    int ret = -1;

    if (value >= -4 && value <= 4)
    {
        ret = ov7725_write(dev, CNST, (0x30 - (4 - value) * 4));
    }

    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int ov7725_set_saturation(struct device *dev, int value)
{
    int ret = -1;

    if (value >= -4 && value <= 4)
    {
        ret = ov7725_write(dev, USAT, (value + 4) << 4);
        ret = ov7725_write(dev, VSAT, (value + 4) << 4);
    }

    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int ov7725_set_wb(struct device *dev, enum light_mode value)
{
    int ret;

    switch (value)
    {
        case WB_AUTO:
            ret = ov7725_write_array(dev, ov7725_wb_auto_regs, ARRAY_SIZE(ov7725_wb_auto_regs));
            break;
        case WB_SUNNY:
            ret = ov7725_write_array(dev, ov7725_wb_sunny_regs, ARRAY_SIZE(ov7725_wb_sunny_regs));
            break;
        case WB_OFFICE:
            ret = ov7725_write_array(dev, ov7725_wb_office_regs, ARRAY_SIZE(ov7725_wb_office_regs));
            break;
        case WB_CLOUDY:
            ret = ov7725_write_array(dev, ov7725_wb_cloudy_regs, ARRAY_SIZE(ov7725_wb_cloudy_regs));
            break;
        case WB_HOME:
            ret = ov7725_write_array(dev, ov7725_wb_home_regs, ARRAY_SIZE(ov7725_wb_home_regs));
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
static int ov7725_restore_mode(struct device *dev)
{
    return ov7725_write_array(dev, ov7725_init_default_setting, ARRAY_SIZE(ov7725_init_default_setting));
}

static int ov7725_init_controls(struct device *dev)
{
    ov7725_set_wb(dev, WB_AUTO);
    ov7725_set_saturation(dev, 0);
    ov7725_set_brightness(dev, 0);
    ov7725_set_contrast(dev, 0);

    return 0;
}


static void ov7725_reset_power(struct device *dev)
{
    const struct ov7725_data *data = DEV_DATA(dev);
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
static inline int ov7725_set_power(struct device *dev, bool on)
{
    int ret = 0;
    uint8_t id_high, id_low;
    uint16_t mid = 0;
    uint16_t pid = 0;

    if (on)
    {
        ov7725_reset_power(dev);
        ov7725_write(dev, COM7, COM7_RESET); //software reset sensor
        k_sleep(10);

        /* Read sensor Model ID */
        ov7725_read(dev, 0x1c, &id_high);

        mid = id_high << 8;

        ov7725_read(dev, 0x1d, &id_low);

        mid |= id_low;

        if (mid != OV7725_MID)
        {
            printk("read mid fail \n");
            ret = -ENOTSUP;
        }

        /* Read sensor Model ID */
        ov7725_read(dev, 0x0a, &id_high);

        pid = id_high << 8;

        ov7725_read(dev, 0x0b, &id_low);

        pid |= id_low;

        if (pid != OV7725_PID)
        {
            printk("read pid fail \n");
            ret = -ENOTSUP;
        }

        ret = ov7725_restore_mode(dev);

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
static inline int ov7725_set_fmt(struct device *dev, enum camera_output_format format, enum camera_mode_id id, enum camera_frame_rate frame_rate)
{
    struct ov7725_data *data = DEV_DATA(dev);
    uint8_t reg;
    uint16_t w, h;
    int ret = 0;

    ret = ov7725_read(dev, COM7, &reg);

    switch (id)
    {
        case MODE_QVGA_320_240:
            if (frame_rate == FPS_15)
            {
                ret = ov7725_write_array(dev, ov7725_setting_15fps_QVGA_320_240, ARRAY_SIZE(ov7725_setting_15fps_QVGA_320_240));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 320;
                    data->pic_height = 240;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = ov7725_write_array(dev, ov7725_setting_30fps_QVGA_320_240, ARRAY_SIZE(ov7725_setting_30fps_QVGA_320_240));
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

            w = 320;
            h = 240;
            reg = COM7_SET_RES(reg, COM7_RES_QVGA);

            break;
        case MODE_VGA_640_480:
            if (frame_rate == FPS_15)
            {
                ret = ov7725_write_array(dev, ov7725_setting_15fps_VGA_640_480, ARRAY_SIZE(ov7725_setting_15fps_VGA_640_480));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 640;
                    data->pic_height = 480;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = ov7725_write_array(dev, ov7725_setting_30fps_VGA_640_480, ARRAY_SIZE(ov7725_setting_30fps_VGA_640_480));
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

            w = 640;
            h = 480;
            reg = COM7_SET_RES(reg, COM7_RES_VGA);

            break;
        default:
            return -ENOTSUP;
    }

    ov7725_write(dev, EXHCH, ((w & 0x3) | ((h & 0x1) << 2)));

    switch (format)
    {
        case VIDEO_FMT_RGB565:
            reg = COM7_SET_FMT(reg, COM7_FMT_RGB | COM7_FMT_RGB565);
            ret = ov7725_write(dev, DSP_Ctrl4, DSP_Ctrl4_YUV_RGB);
            ret = ov7725_write(dev, COM7, reg);
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_RGB565;
            }
            break;
        case VIDEO_FMT_YUV422:
            reg = COM7_SET_FMT(reg, COM7_FMT_YUV | COM7_FMT_GBR422/*|COM7_FMT_RGB565*/);
            ret = ov7725_write(dev, DSP_Ctrl4, DSP_Ctrl4_YUV_RGB);
            ret = ov7725_write(dev, COM7, reg);
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_YUV422;
            }
            break;
        case VIDEO_FMT_RAW:
            reg = COM7_SET_FMT(reg, COM7_FMT_P_BAYER);
            ret = ov7725_write(dev, DSP_Ctrl4, DSP_Ctrl4_RAW8);
            ret = ov7725_write(dev, COM7, reg);
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
static int ov7725_set_framesize(struct device *dev, uint16_t xsize, uint16_t ysize)
{
    struct ov7725_data *data = DEV_DATA(dev);

    data->pic_width = xsize;
    data->pic_height = ysize;

    ov7725_write(dev, HSIZE, (uint8_t)(xsize >> 2));
    ov7725_write(dev, HOutSize, (uint8_t)(xsize >> 2));
    ov7725_write(dev, VSIZE, (uint8_t)(ysize >> 1));
    ov7725_write(dev, VOutSize, (uint8_t)(ysize >> 1));

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
static inline int ov7725_get_fmt(struct device *dev, struct video_format *fmt)
{
    struct ov7725_data *data = DEV_DATA(dev);

    fmt->width = data->pic_width;
    fmt->height = data->pic_height;
    fmt->framerate = data->framerate;
    fmt->pixfmt = data->pixfmt;

    return 0;
}

static int ov7725_set_exposure(struct device *dev, int exposure)
{
    int ret;
    uint8_t reg;

    ret = ov7725_read(dev, COM8, &reg);

    if (exposure >= 0)
    {
        reg &= 0xfe;
        ret = ov7725_write(dev, COM8, reg);
        ret = ov7725_write(dev, AEC, ((exposure >> 0) & 0xFF));
        ret = ov7725_write(dev, AECH, ((exposure >> 8) & 0xFF));
    }
    else
    {
        reg |= 0x01;
        ret = ov7725_write(dev, COM8, reg);
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
static inline int ov7725_s_ctrl(struct device *dev, uint32_t cmd, void *arg)
{
    struct ov7725_data *data = DEV_DATA(dev);
    struct video_format *fmt;
    int ret;
    int value;

    switch (cmd)
    {
        case CAMERA_CID_BRIGHTNESS:
            value = *(int *)arg;
            ret = ov7725_set_brightness(dev, value);
            break;
        case CAMERA_CID_CONTRAST:
            value = *(int *)arg;
            ret = ov7725_set_contrast(dev, value);
            break;
        case CAMERA_CID_SATURATION:
            value = *(int *)arg;
            ret = ov7725_set_saturation(dev, value);
            break;
        case CAMERA_CID_SHARPNESS:
            break;
        case CAMERA_CID_FLASH_MODE:
            break;
        case CAMERA_CID_FOCUS_AUTO:
            break;
        case CAMERA_CID_EXPOSURE:
            value = *(int *)arg;
            ret = ov7725_set_exposure(dev, value);
            break;
        case CAMERA_CID_WHITE_BALANCE:
            value = *(enum light_mode *)arg;
            ret = ov7725_set_wb(dev, value);
            break;
        case CAMERA_CID_FRAME_SIZE:
            fmt = (struct video_format *)arg;
            ret = ov7725_set_framesize(dev, fmt->width, fmt->height);
            break;
        default:
            break;
    }

    return 0;
}

static const struct camera_driver_api ov7725_drv_api_funcs =
{
    .set_power = ov7725_set_power,
    .set_fmt = ov7725_set_fmt,
    .get_fmt = ov7725_get_fmt,
    .ioctl = ov7725_s_ctrl,
};

static int ov7725_init(struct device *dev)
{
    struct ov7725_data *data = DEV_DATA(dev);

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

#ifdef CONFIG_OV7725_0
static struct ov7725_data ov7725_dev_data_0 =
{
    .sccb_name = CONFIG_CAMERA_0_SCCB_NAME,
    .pic_width = 320,
    .pic_height = 240,
    .framerate = 15,
    .pixfmt = VIDEO_FMT_RGB565,
    .rst_port_name = CAMERA_0_GPIO_RST_PORT,
    .pwdn_port_name = CAMERA_0_GPIO_PWDN_PORT,
    .rst_pin = CAMERA_0_GPIO_RST_PIN,
    .pwdn_pin = CAMERA_0_GPIO_PWDN_PIN,
};

DEVICE_AND_API_INIT(ov7725_0, OV7725_0_DEV_NAME,
                    ov7725_init, &ov7725_dev_data_0, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &ov7725_drv_api_funcs);
#endif

#ifdef CONFIG_OV7725_1
static struct ov7725_data ov7725_dev_data_1 =
{
    .sccb_name = CONFIG_CAMERA_1_SCCB_NAME,
    .pic_width = 320,
    .pic_height = 240,
    .framerate = 15,
    .pixfmt = VIDEO_FMT_RGB565,
    .rst_port_name = CAMERA_1_GPIO_RST_PORT,
    .pwdn_port_name = CAMERA_1_GPIO_PWDN_PORT,
    .rst_pin = CAMERA_1_GPIO_RST_PIN,
    .pwdn_pin = CAMERA_1_GPIO_PWDN_PIN,
};

DEVICE_AND_API_INIT(ov7725_1, OV7725_1_DEV_NAME,
                    ov7725_init, &ov7725_dev_data_1, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &ov7725_drv_api_funcs);
#endif


