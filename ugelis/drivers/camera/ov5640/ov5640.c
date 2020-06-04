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
#include <i2c.h>
#include <misc/printk.h>

#include <camera/camera_sensor.h>
#include "ov5640_af_firmware.h"
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>
#include <gm_hal_vin.h>

#ifdef CONFIG_EXTEND_IO
    #include "gpio_pcf8574.h"
#endif

#define OV5640_ID      0x5640
#define OV5640_ADDR    0x3c//0x78

struct regval_list
{
    uint16_t reg;
    uint8_t  val;
};

/** Runtime driver data */
struct ov5640_data
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
    bool     load_af_firmware;
    enum camera_output_format pixfmt;
};

/* convenience defines */
#define DEV_DATA(dev)                           \
    ((struct ov5640_data * const)(dev)->driver_data)

/*
 * The default register settings
 *
 */
static const struct regval_list ov5640_init_default_setting[] =
{
    {0x3008, 0x42}, // software power down, bit[6]
    {0x3103, 0x03}, // system clock from PLL, bit[1]
    {0x3017, 0xff}, // FREX, Vsync, HREF, PCLK, D[9:6] output enable
    {0x3018, 0xff}, // D[5:0], GPIO[1:0] output enable
    {0x3034, 0x1a}, // MIPI 10-bit
    {0x3037, 0x13}, // PLL root divider, bit[4], PLL pre-divider, bit[3:0]
    {0x3108, 0x01}, // PCLK root divider, bit[5:4], SCLK2x root divider, bit[3:2]

    // SCLK root divider, bit[1:0]
    {0x3630, 0x36},
    {0x3631, 0x0e},
    {0x3632, 0xe2},
    {0x3633, 0x12},
    {0x3621, 0xe0},
    {0x3704, 0xa0},
    {0x3703, 0x5a},
    {0x3715, 0x78},
    {0x3717, 0x01},
    {0x370b, 0x60},
    {0x3705, 0x1a},
    {0x3905, 0x02},
    {0x3906, 0x10},
    {0x3901, 0x0a},
    {0x3731, 0x12},
    {0x3600, 0x08}, // VCM control
    {0x3601, 0x33}, // VCM control
    {0x302d, 0x60}, // system control
    {0x3620, 0x52},
    {0x371b, 0x20},
    {0x471c, 0x50},
    {0x3a13, 0x43}, // pre-gain = 1.047x
    {0x3a18, 0x00}, // gain ceiling
    {0x3a19, 0xf8}, // gain ceiling = 15.5x
    {0x3635, 0x13},
    {0x3636, 0x03},
    {0x3634, 0x40},
    {0x3622, 0x01},
    // 50/60Hz detection
    {0x3c01, 0x34}, // Band auto, bit[7]
    {0x3c04, 0x28}, // threshold low sum
    {0x3c05, 0x98}, // threshold high sum
    {0x3c06, 0x00}, // light meter 1 threshold[15:8]
    {0x3c07, 0x08}, // light meter 1 threshold[7:0]
    {0x3c08, 0x00}, // light meter 2 threshold[15:8]
    {0x3c09, 0x1c}, // light meter 2 threshold[7:0]
    {0x3c0a, 0x9c}, // sample number[15:8]
    {0x3c0b, 0x40}, // sample number[7:0]
    {0x3810, 0x00}, // Timing Hoffset[11:8]
    {0x3811, 0x10}, // Timing Hoffset[7:0]
    {0x3812, 0x00}, // Timing Voffset[10:8]
    {0x3708, 0x64},
    {0x4001, 0x02}, // BLC start from line 2
    {0x4005, 0x1a}, // BLC always update
    {0x3000, 0x00}, // enable blocks
    {0x3004, 0xff}, // enable clocks
    {0x300e, 0x58}, // MIPI power down, DVP enable
    {0x302e, 0x00},
    {0x4300, 0x30}, // YUV 422, YUYV
    {0x501f, 0x00}, // YUV 422
    {0x440e, 0x00},
    {0x5000, 0xa7}, // Lenc on, raw gamma on, BPC on, WPC on, CIP on
    // AEC target
    {0x3a0f, 0x30}, // stable range in high
    {0x3a10, 0x28}, // stable range in low
    {0x3a1b, 0x30}, // stable range out high
    {0x3a1e, 0x26}, // stable range out low
    {0x3a11, 0x60}, // fast zone high
    {0x3a1f, 0x14}, // fast zone low
    // Lens correction
    {0x5800, 0x23},
    {0x5801, 0x14},
    {0x5802, 0x0f},
    {0x5803, 0x0f},
    {0x5804, 0x12},
    {0x5805, 0x26},
    {0x5806, 0x0c},
    {0x5807, 0x08},
    {0x5808, 0x05},
    {0x5809, 0x05},
    {0x580a, 0x08},

    {0x580b, 0x0d},
    {0x580c, 0x08},
    {0x580d, 0x03},
    {0x580e, 0x00},
    {0x580f, 0x00},
    {0x5810, 0x03},
    {0x5811, 0x09},
    {0x5812, 0x07},
    {0x5813, 0x03},
    {0x5814, 0x00},
    {0x5815, 0x01},
    {0x5816, 0x03},
    {0x5817, 0x08},
    {0x5818, 0x0d},
    {0x5819, 0x08},
    {0x581a, 0x05},
    {0x581b, 0x06},
    {0x581c, 0x08},
    {0x581d, 0x0e},
    {0x581e, 0x29},
    {0x581f, 0x17},
    {0x5820, 0x11},
    {0x5821, 0x11},
    {0x5822, 0x15},
    {0x5823, 0x28},
    {0x5824, 0x46},
    {0x5825, 0x26},
    {0x5826, 0x08},
    {0x5827, 0x26},
    {0x5828, 0x64},
    {0x5829, 0x26},
    {0x582a, 0x24},
    {0x582b, 0x22},
    {0x582c, 0x24},
    {0x582d, 0x24},
    {0x582e, 0x06},
    {0x582f, 0x22},
    {0x5830, 0x40},
    {0x5831, 0x42},
    {0x5832, 0x24},
    {0x5833, 0x26},
    {0x5834, 0x24},
    {0x5835, 0x22},
    {0x5836, 0x22},
    {0x5837, 0x26},
    {0x5838, 0x44},
    {0x5839, 0x24},
    {0x583a, 0x26},
    {0x583b, 0x28},
    {0x583c, 0x42},
    {0x583d, 0xce}, // lenc BR offset
    // AWB
    {0x5180, 0xff}, // AWB B block
    {0x5181, 0xf2}, // AWB control
    {0x5182, 0x00}, // [7:4] max local counter, [3:0] max fast counter
    {0x5183, 0x14}, // AWB advanced
    {0x5184, 0x25},
    {0x5185, 0x24},
    {0x5186, 0x09},
    {0x5187, 0x09},
    {0x5188, 0x09},
    {0x5189, 0x75},
    {0x518a, 0x54},
    {0x518b, 0xe0},
    {0x518c, 0xb2},
    {0x518d, 0x42},
    {0x518e, 0x3d},
    {0x518f, 0x56},
    {0x5190, 0x46},
    {0x5191, 0xf8}, // AWB top limit
    {0x5192, 0x04}, // AWB bottom limit
    {0x5193, 0x70}, // red limit
    {0x5194, 0xf0}, // green limit
    {0x5195, 0xf0}, // blue limit
    {0x5196, 0x03}, // AWB control
    {0x5197, 0x01}, // local limit
    {0x5198, 0x04},
    {0x5199, 0x12},
    {0x519a, 0x04},
    {0x519b, 0x00},
    {0x519c, 0x06},
    {0x519d, 0x82},
    {0x519e, 0x38}, // AWB control
    // Gamma
    {0x5480, 0x01}, // Gamma bias plus on, bit[0]
    {0x5481, 0x08},
    {0x5482, 0x14},
    {0x5483, 0x28},
    {0x5484, 0x51},
    {0x5485, 0x65},
    {0x5486, 0x71},
    {0x5487, 0x7d},
    {0x5488, 0x87},
    {0x5489, 0x91},
    {0x548a, 0x9a},
    {0x548b, 0xaa},
    {0x548c, 0xb8},
    {0x548d, 0xcd},
    {0x548e, 0xdd},
    {0x548f, 0xea},
    {0x5490, 0x1d},
    // color matrix
    {0x5381, 0x1e}, // CMX1 for Y
    {0x5382, 0x5b}, // CMX2 for Y
    {0x5383, 0x08}, // CMX3 for Y
    {0x5384, 0x0a}, // CMX4 for U
    {0x5385, 0x7e}, // CMX5 for U
    {0x5386, 0x88}, // CMX6 for U
    {0x5387, 0x7c}, // CMX7 for V
    {0x5388, 0x6c}, // CMX8 for V
    {0x5389, 0x10}, // CMX9 for V
    {0x538a, 0x01}, // sign[9]
    {0x538b, 0x98}, // sign[8:1]
    // UV adjust
    {0x5580, 0x06}, // saturation on, bit[1]
    {0x5583, 0x40},
    {0x5584, 0x10},
    {0x5589, 0x10},
    {0x558a, 0x00},
    {0x558b, 0xf8},
    {0x501d, 0x40}, // enable manual offset of contrast
    // CIP
    {0x5300, 0x08}, // CIP sharpen MT threshold 1
    {0x5301, 0x30}, // CIP sharpen MT threshold 2
    {0x5302, 0x10}, // CIP sharpen MT offset 1
    {0x5303, 0x00}, // CIP sharpen MT offset 2
    {0x5304, 0x08}, // CIP DNS threshold 1
    {0x5305, 0x30}, // CIP DNS threshold 2
    {0x5306, 0x08}, // CIP DNS offset 1
    {0x5307, 0x16}, // CIP DNS offset 2
    {0x5309, 0x08}, // CIP sharpen TH threshold 1
    {0x530a, 0x30}, // CIP sharpen TH threshold 2
    {0x530b, 0x04}, // CIP sharpen TH offset 1
    {0x530c, 0x06}, // CIP sharpen TH offset 2
    {0x5025, 0x00},
    {0x3008, 0x02}, // wake up from standby, bit[6]
    {0x4740, 0x21}, //VSYNC
};

static const struct regval_list ov5640_setting_30fps_QVGA_320_240[] =
{
    {0x3035, 0x11}, {0x3036, 0x46}, {0x3c07, 0x08},
    {0x3820, 0x41}, {0x3821, 0x07}, {0x3814, 0x31},
    {0x3815, 0x31}, {0x3800, 0x00}, {0x3801, 0x00},
    {0x3802, 0x00}, {0x3803, 0x04}, {0x3804, 0x0a},
    {0x3805, 0x3f}, {0x3806, 0x07}, {0x3807, 0x9b},
    {0x3808, 0x01}, {0x3809, 0x40}, {0x380a, 0x00},
    {0x380b, 0xf0}, {0x380c, 0x07}, {0x380d, 0x68},
    {0x380e, 0x03}, {0x380f, 0xd8}, {0x3813, 0x06},
    {0x3618, 0x00}, {0x3612, 0x29}, {0x3709, 0x52},
    {0x370c, 0x03}, {0x3a02, 0x17}, {0x3a03, 0x10},
    {0x3a14, 0x17}, {0x3a15, 0x10}, {0x4004, 0x02},
    {0x3002, 0x1c}, {0x3006, 0xc3}, {0x4713, 0x03},
    {0x4407, 0x04}, {0x460b, 0x35}, {0x460c, 0x22},
    {0x4837, 0x22}, {0x3824, 0x02}, {0x5001, 0xa3},
    {0x3503, 0x00},
};

static const struct regval_list ov5640_setting_15fps_QVGA_320_240[] =
{
    {0x3035, 0x21}, {0x3036, 0x46}, {0x3c07, 0x08},
    {0x3820, 0x41}, {0x3821, 0x07}, {0x3814, 0x31},
    {0x3815, 0x31}, {0x3800, 0x00}, {0x3801, 0x00},
    {0x3802, 0x00}, {0x3803, 0x04}, {0x3804, 0x0a},
    {0x3805, 0x3f}, {0x3806, 0x07}, {0x3807, 0x9b},
    {0x3808, 0x01}, {0x3809, 0x40}, {0x380a, 0x00},
    {0x380b, 0xf0}, {0x380c, 0x07}, {0x380d, 0x68},
    {0x380e, 0x03}, {0x380f, 0xd8}, {0x3813, 0x06},
    {0x3618, 0x00}, {0x3612, 0x29}, {0x3709, 0x52},
    {0x370c, 0x03}, {0x3a02, 0x0b}, {0x3a03, 0x88},
    {0x3a14, 0x0b}, {0x3a15, 0x88}, {0x4004, 0x02},
    {0x3002, 0x1c}, {0x3006, 0xc3}, {0x4713, 0x03},
    {0x4407, 0x04}, {0x460b, 0x35}, {0x460c, 0x22},
    {0x4837, 0x22}, {0x3824, 0x02}, {0x5001, 0xa3},
    {0x3503, 0x00},
};

static const struct regval_list ov5640_setting_30fps_VGA_640_480[] =
{
    {0x3035, 0x11}, {0x3036, 0x46}, {0x3c07, 0x08},
    {0x3820, 0x41}, {0x3821, 0x07}, {0x3814, 0x31},
    {0x3815, 0x31}, {0x3800, 0x00}, {0x3801, 0x00},
    {0x3802, 0x00}, {0x3803, 0x04}, {0x3804, 0x0a},
    {0x3805, 0x3f}, {0x3806, 0x07}, {0x3807, 0x9b},
    {0x3808, 0x02}, {0x3809, 0x80}, {0x380a, 0x01},
    {0x380b, 0xe0}, {0x380c, 0x07}, {0x380d, 0x68},
    {0x380e, 0x03}, {0x380f, 0xd8}, {0x3813, 0x06},
    {0x3618, 0x00}, {0x3612, 0x29}, {0x3709, 0x52},
    {0x370c, 0x03}, {0x3a02, 0x17}, {0x3a03, 0x10},
    {0x3a14, 0x17}, {0x3a15, 0x10}, {0x4004, 0x02},
    {0x3002, 0x1c}, {0x3006, 0xc3}, {0x4713, 0x03},
    {0x4407, 0x04}, {0x460b, 0x35}, {0x460c, 0x22},
    {0x4837, 0x22}, {0x3824, 0x02}, {0x5001, 0xa3},
    {0x3503, 0x00},
};

static const struct regval_list ov5640_setting_15fps_VGA_640_480[] =
{
    {0x3035, 0x21}, {0x3036, 0x46}, {0x3c07, 0x08},
    {0x3820, 0x41}, {0x3821, 0x07}, {0x3814, 0x31},
    {0x3815, 0x31}, {0x3800, 0x00}, {0x3801, 0x00},
    {0x3802, 0x00}, {0x3803, 0x04}, {0x3804, 0x0a},
    {0x3805, 0x3f}, {0x3806, 0x07}, {0x3807, 0x9b},
    {0x3808, 0x02}, {0x3809, 0x80}, {0x380a, 0x01},
    {0x380b, 0xe0}, {0x380c, 0x07}, {0x380d, 0x68},
    {0x380e, 0x03}, {0x380f, 0xd8}, {0x3813, 0x06},
    {0x3618, 0x00}, {0x3612, 0x29}, {0x3709, 0x52},
    {0x370c, 0x03}, {0x3a02, 0x0b}, {0x3a03, 0x88},
    {0x3a14, 0x0b}, {0x3a15, 0x88}, {0x4004, 0x02},
    {0x3002, 0x1c}, {0x3006, 0xc3}, {0x4713, 0x03},
    {0x4407, 0x04}, {0x460b, 0x35}, {0x460c, 0x22},
    {0x4837, 0x22}, {0x3824, 0x02}, {0x5001, 0xa3},
    {0x3503, 0x00},
};

static const struct regval_list ov5640_setting_30fps_XGA_1024_768[] =
{
    {0x3035, 0x11}, {0x3036, 0x69}, {0x3c07, 0x07},
    {0x3820, 0x41}, {0x3821, 0x07}, {0x3814, 0x11},
    {0x3815, 0x11}, {0x3800, 0x00}, {0x3801, 0x00},
    {0x3802, 0x00}, {0x3803, 0x00}, {0x3804, 0x0a},
    {0x3805, 0x3f}, {0x3806, 0x07}, {0x3807, 0x9f},
    {0x3808, 0x04}, {0x3809, 0x00}, {0x380a, 0x03},
    {0x380b, 0x00}, {0x380c, 0x0b}, {0x380d, 0x1c},
    {0x380e, 0x07}, {0x380f, 0xb0}, {0x3813, 0x04},
    {0x3618, 0x04}, {0x3612, 0x2b}, {0x3709, 0x12},
    {0x370c, 0x00}, {0x3a02, 0x07}, {0x3a03, 0xb0},
    {0x3a14, 0x07}, {0x3a15, 0xb0}, {0x4004, 0x06},
    {0x3002, 0x1c}, {0x3006, 0xc3}, {0x4713, 0x03},
    {0x4407, 0x04}, {0x460b, 0x35}, {0x460c, 0x22},
    {0x4837, 0x22}, {0x3824, 0x02}, {0x5001, 0xa3},
    {0x3503, 0x00},
};

static const struct regval_list ov5640_setting_15fps_XGA_1024_768[] =
{
    {0x3035, 0x21}, {0x3036, 0x69}, {0x3c07, 0x07},
    {0x3820, 0x41}, {0x3821, 0x07}, {0x3814, 0x11},
    {0x3815, 0x11}, {0x3800, 0x00}, {0x3801, 0x00},
    {0x3802, 0x00}, {0x3803, 0x00}, {0x3804, 0x0a},
    {0x3805, 0x3f}, {0x3806, 0x07}, {0x3807, 0x9f},
    {0x3808, 0x04}, {0x3809, 0x00}, {0x380a, 0x03},
    {0x380b, 0x00}, {0x380c, 0x0b}, {0x380d, 0x1c},
    {0x380e, 0x07}, {0x380f, 0xb0}, {0x3813, 0x04},
    {0x3618, 0x04}, {0x3612, 0x2b}, {0x3709, 0x12},
    {0x370c, 0x00}, {0x3a02, 0x07}, {0x3a03, 0xb0},
    {0x3a14, 0x07}, {0x3a15, 0xb0}, {0x4004, 0x06},
    {0x3002, 0x1c}, {0x3006, 0xc3}, {0x4713, 0x03},
    {0x4407, 0x04}, {0x460b, 0x35}, {0x460c, 0x22},
    {0x4837, 0x22}, {0x3824, 0x02}, {0x5001, 0xa3},
    {0x3503, 0x00},
};

static const struct regval_list ov5640_setting_30fps_720P_1280_720[] =
{
    {0x3035, 0x21}, {0x3036, 0x69}, {0x3c07, 0x07},
    {0x3820, 0x41}, {0x3821, 0x07}, {0x3814, 0x31},
    {0x3815, 0x31}, {0x3800, 0x00}, {0x3801, 0x00},
    {0x3802, 0x00}, {0x3803, 0xfa}, {0x3804, 0x0a},
    {0x3805, 0x3f}, {0x3806, 0x06}, {0x3807, 0xa9},
    {0x3808, 0x05}, {0x3809, 0x00}, {0x380a, 0x02},
    {0x380b, 0xd0}, {0x380c, 0x07}, {0x380d, 0x64},
    {0x380e, 0x02}, {0x380f, 0xe4}, {0x3813, 0x04},
    {0x3618, 0x00}, {0x3612, 0x29}, {0x3709, 0x52},
    {0x370c, 0x03}, {0x3a02, 0x02}, {0x3a03, 0xe0},
    {0x3a14, 0x02}, {0x3a15, 0xe0}, {0x4004, 0x02},
    {0x3002, 0x1c}, {0x3006, 0xc3}, {0x4713, 0x03},
    {0x4407, 0x04}, {0x460b, 0x37}, {0x460c, 0x20},
    {0x4837, 0x16}, {0x3824, 0x04}, {0x5001, 0x83},
    {0x3503, 0x00},
};

static const struct regval_list ov5640_setting_15fps_720P_1280_720[] =
{
    {0x3035, 0x41}, {0x3036, 0x69}, {0x3c07, 0x07},
    {0x3820, 0x41}, {0x3821, 0x07}, {0x3814, 0x31},
    {0x3815, 0x31}, {0x3800, 0x00}, {0x3801, 0x00},
    {0x3802, 0x00}, {0x3803, 0x00}, {0x3804, 0x0a},
    {0x3805, 0x3f}, {0x3806, 0x07}, {0x3807, 0x9f},
    {0x3808, 0x05}, {0x3809, 0x00}, {0x380a, 0x02},
    {0x380b, 0xd0}, {0x380c, 0x0b}, {0x380d, 0x1c},
    {0x380e, 0x07}, {0x380f, 0xb0}, {0x3813, 0x04},
    {0x3618, 0x00}, {0x3612, 0x29}, {0x3709, 0x52},
    {0x370c, 0x03}, {0x3a02, 0x02}, {0x3a03, 0xe0},
    {0x3a14, 0x02}, {0x3a15, 0xe0}, {0x4004, 0x02},
    {0x3002, 0x1c}, {0x3006, 0xc3}, {0x4713, 0x03},
    {0x4407, 0x04}, {0x460b, 0x37}, {0x460c, 0x20},
    {0x4837, 0x16}, {0x3824, 0x04}, {0x5001, 0xa3},
    {0x3503, 0x00},
};

static const struct regval_list ov5640_setting_15fps_QSXGA_2592_1944[] =
{
    {0x3035, 0x11}, {0x3036, 0x69}, {0x3c07, 0x07},
    {0x3820, 0x40}, {0x3821, 0x06}, {0x3814, 0x11},
    {0x3815, 0x11}, {0x3800, 0x00}, {0x3801, 0x00},
    {0x3802, 0x00}, {0x3803, 0x00}, {0x3804, 0x0a},
    {0x3805, 0x3f}, {0x3806, 0x07}, {0x3807, 0x9f},
    {0x3808, 0x0a}, {0x3809, 0x20}, {0x380a, 0x07},
    {0x380b, 0x98}, {0x380c, 0x0b}, {0x380d, 0x1c},
    {0x380e, 0x07}, {0x380f, 0xb0}, {0x3813, 0x04},
    {0x3618, 0x04}, {0x3612, 0x2b}, {0x3709, 0x12},
    {0x370c, 0x00}, {0x4004, 0x06}, {0x3002, 0x00},
    {0x3006, 0xff}, {0x4713, 0x02}, {0x4407, 0x04},
    {0x460b, 0x35}, {0x460c, 0x22}, {0x4837, 0x16},
    {0x3824, 0x04}, {0x5001, 0x83}, {0x3503, 0x00},
};

static const struct regval_list ov5640_setting_FMT_RAW[] =
{
    {0x4300, 0x00}, {0x501f, 0x00},
    {0x3820, 0x46}, {0x3821, 0x00},
};

static const struct regval_list ov5640_setting_FMT_YUV422[] =
{
    {0x4300, 0x30}, {0x501f, 0x00},
    {0x3820, 0x46}, {0x3821, 0x06},
};

#ifdef CONFIG_OV5640
static const struct regval_list ov5640_setting_FMT_RGB565[] =
{
    {0x4300, 0x6f}, {0x501f, 0x01},
    {0x3820, 0x46}, {0x3821, 0x06},
};
#else
static const struct regval_list ov5640_setting_FMT_RGB565[] =
{
    {0x4300, 0x6f}, {0x501f, 0x01},
    {0x3820, 0x46}, {0x3821, 0x00},
};
#endif

static const struct regval_list ov5640_setting_FMT_JPEG[] =
{
    {0x4300, 0x30}, {0x501f, 0x00},
    {0x3820, 0x46}, {0x3821, 0x20},
    {0x4004, 0x06}, {0x3002, 0x00},
    {0x3006, 0xff}, {0x4713, 0x02},
};

/*
 * The brightness setttings
 */
static const struct regval_list ov5640_brightness_neg4_regs[] =
{
    {0x5587, 0x40},
    {0x5588, 0x09},
};
static const struct regval_list ov5640_brightness_neg3_regs[] =
{
    {0x5587, 0x30},
    {0x5588, 0x09},
};
static const struct regval_list ov5640_brightness_neg2_regs[] =
{
    {0x5587, 0x20},
    {0x5588, 0x09},
};
static const struct regval_list ov5640_brightness_neg1_regs[] =
{
    {0x5587, 0x10},
    {0x5588, 0x09},
};
static const struct regval_list ov5640_brightness_zero_regs[] =
{
    {0x5587, 0x00},
    {0x5588, 0x01},
};
static const struct regval_list ov5640_brightness_pos1_regs[] =
{
    {0x5587, 0x10},
    {0x5588, 0x01},
};
static const struct regval_list ov5640_brightness_pos2_regs[] =
{
    {0x5587, 0x20},
    {0x5588, 0x01},
};
static const struct regval_list ov5640_brightness_pos3_regs[] =
{
    {0x5587, 0x30},
    {0x5588, 0x01},
};
static const struct regval_list ov5640_brightness_pos4_regs[] =
{
    {0x5587, 0x40},
    {0x5588, 0x01},
};

/*
 * The contrast setttings
 */
static const struct regval_list ov5640_contrast_neg3_regs[] =
{
    {0x5585, 0x14},
    {0x5586, 0x14},
};
static const struct regval_list ov5640_contrast_neg2_regs[] =
{
    {0x5585, 0x18},
    {0x5586, 0x18},
};
static const struct regval_list ov5640_contrast_neg1_regs[] =
{
    {0x5585, 0x1c},
    {0x5586, 0x1c},
};
static const struct regval_list ov5640_contrast_zero_regs[] =
{
    {0x5585, 0x00},
    {0x5586, 0x20},
};
static const struct regval_list ov5640_contrast_pos1_regs[] =
{
    {0x5585, 0x10},
    {0x5586, 0x24},
};
static const struct regval_list ov5640_contrast_pos2_regs[] =
{
    {0x5585, 0x18},
    {0x5586, 0x28},
};
static const struct regval_list ov5640_contrast_pos3_regs[] =
{
    {0x5585, 0x1c},
    {0x5586, 0x2c},
};

/*
 * The saturation setttings
 */
static const struct regval_list ov5640_saturation_neg3_regs[] =
{
    {0x5381, 0x1c},
    {0x5382, 0x5a},
    {0x5383, 0x06},
    {0x5384, 0x0c},
    {0x5385, 0x30},
    {0x5386, 0x3d},
    {0x5387, 0x3e},
    {0x5388, 0x3d},
    {0x5389, 0x01},
    {0x538b, 0x98},
    {0x538a, 0x01},
};

static const struct regval_list ov5640_saturation_neg2_regs[] =
{
    {0x5381, 0x1c},
    {0x5382, 0x5a},
    {0x5383, 0x06},
    {0x5384, 0x10},
    {0x5385, 0x3d},
    {0x5386, 0x4d},
    {0x5387, 0x4e},
    {0x5388, 0x4d},
    {0x5389, 0x01},
    {0x538b, 0x98},
    {0x538a, 0x01},
};

static const struct regval_list ov5640_saturation_neg1_regs[] =
{
    {0x5381, 0x1c},
    {0x5382, 0x5a},
    {0x5383, 0x06},
    {0x5384, 0x15},
    {0x5385, 0x52},
    {0x5386, 0x66},
    {0x5387, 0x68},
    {0x5388, 0x66},
    {0x5389, 0x02},
    {0x538b, 0x98},
    {0x538a, 0x01},
};

static const struct regval_list ov5640_saturation_zero_regs[] =
{
    {0x5381, 0x1c},
    {0x5382, 0x5a},
    {0x5383, 0x06},
    {0x5384, 0x1a},
    {0x5385, 0x66},
    {0x5386, 0x80},
    {0x5387, 0x82},
    {0x5388, 0x80},
    {0x5389, 0x02},
    {0x538b, 0x98},
    {0x538a, 0x01},
};

static const struct regval_list ov5640_saturation_pos1_regs[] =
{
    {0x5381, 0x1c},
    {0x5382, 0x5a},
    {0x5383, 0x06},
    {0x5384, 0x1f},
    {0x5385, 0x7a},
    {0x5386, 0x9a},
    {0x5387, 0x9c},
    {0x5388, 0x9a},
    {0x5389, 0x02},
    {0x538b, 0x98},
    {0x538a, 0x01},
};

static const struct regval_list ov5640_saturation_pos2_regs[] =
{
    {0x5381, 0x1c},
    {0x5382, 0x5a},
    {0x5383, 0x06},
    {0x5384, 0x24},
    {0x5385, 0x8f},
    {0x5386, 0xb3},
    {0x5387, 0xb6},
    {0x5388, 0xb3},
    {0x5389, 0x03},
    {0x538b, 0x98},
    {0x538a, 0x01},
};

static const struct regval_list ov5640_saturation_pos3_regs[] =
{
    {0x5381, 0x1c},
    {0x5382, 0x5a},
    {0x5383, 0x06},
    {0x5384, 0x2b},
    {0x5385, 0xab},
    {0x5386, 0xb6},
    {0x5387, 0xba},
    {0x5388, 0xb6},
    {0x5389, 0x04},
    {0x538b, 0x98},
    {0x538a, 0x01},
};

/*
 * The exposure target setttings
 */
static const struct regval_list ov5640_ev_neg3_regs[] =
{
    {0x3a0f, 0x10},
    {0x3a10, 0x08},
    {0x3a1b, 0x10},
    {0x3a1e, 0x08},
    {0x3a11, 0x20},
    {0x3a1f, 0x10},
};

static const struct regval_list ov5640_ev_neg2_regs[] =
{
    {0x3a0f, 0x20},
    {0x3a10, 0x18},
    {0x3a1b, 0x41},
    {0x3a1e, 0x20},
    {0x3a11, 0x18},
    {0x3a1f, 0x10},
};

static const struct regval_list ov5640_ev_neg1_regs[] =
{
    {0x3a0f, 0x30},
    {0x3a10, 0x28},
    {0x3a1b, 0x61},
    {0x3a1e, 0x30},
    {0x3a11, 0x28},
    {0x3a1f, 0x10},
};

static const struct regval_list ov5640_ev_zero_regs[] =
{
    {0x3a0f, 0x38},
    {0x3a10, 0x30},
    {0x3a1b, 0x61},
    {0x3a1e, 0x38},
    {0x3a11, 0x30},
    {0x3a1f, 0x10},
};

static const struct regval_list ov5640_ev_pos1_regs[] =
{
    {0x3a0f, 0x40},
    {0x3a10, 0x38},
    {0x3a1b, 0x71},
    {0x3a1e, 0x40},
    {0x3a11, 0x38},
    {0x3a1f, 0x10},
};

static const struct regval_list ov5640_ev_pos2_regs[] =
{
    {0x3a0f, 0x50},
    {0x3a10, 0x48},
    {0x3a1b, 0x90},
    {0x3a1e, 0x50},
    {0x3a11, 0x48},
    {0x3a1f, 0x20},
};

static const struct regval_list ov5640_ev_pos3_regs[] =
{
    {0x3a0f, 0x60},
    {0x3a10, 0x58},
    {0x3a1b, 0xa0},
    {0x3a1e, 0x60},
    {0x3a11, 0x58},
    {0x3a1f, 0x20},
};

/*
 * The white balance setttings
 */
static const struct regval_list ov5640_wb_auto_regs[] =
{
    {0x3406, 0x0 },
    {0x3400, 0x4 },
    {0x3401, 0x0},
    {0x3402, 0x4 },
    {0x3403, 0x0 },
    {0x3404, 0x4 },
    {0x3405, 0x0},
};

static const struct regval_list ov5640_wb_sunny_regs[] =
{
    {0x3406, 0x1 },
    {0x3400, 0x6 },
    {0x3401, 0x1c},
    {0x3402, 0x4 },
    {0x3403, 0x0 },
    {0x3404, 0x4 },
    {0x3405, 0xf3},
};

static const struct regval_list ov5640_wb_office_regs[] =
{
    {0x3406, 0x1 },
    {0x3400, 0x5 },
    {0x3401, 0x48},
    {0x3402, 0x4 },
    {0x3403, 0x0 },
    {0x3404, 0x7 },
    {0x3405, 0xcf},
};

static const struct regval_list ov5640_wb_cloudy_regs[] =
{
    {0x3406, 0x1 },
    {0x3400, 0x6 },
    {0x3401, 0x48},
    {0x3402, 0x4 },
    {0x3403, 0x0 },
    {0x3404, 0x4 },
    {0x3405, 0xd3},
};

static const struct regval_list ov5640_wb_home_regs[] =
{
    {0x3406, 0x1 },
    {0x3400, 0x4 },
    {0x3401, 0x10},
    {0x3402, 0x4 },
    {0x3403, 0x0 },
    {0x3404, 0x8 },
    {0x3405, 0x40},
};

static int ov5640_read(struct device *dev, uint16_t reg, uint8_t *value)
{
    const struct ov5640_data *data = DEV_DATA(dev);
    uint8_t addr_buffer[2];
    struct i2c_msg msg;
    int ret = 0;

    addr_buffer[1] = reg & 0xFF;
    addr_buffer[0] = reg >> 8;
    msg.buf = addr_buffer;
    msg.len = 2;
    msg.flags = I2C_MSG_WRITE/*|I2C_MSG_STOP*/;



    ret = i2c_transfer(data->i2c, &msg, 1, OV5640_ADDR);

    msg.buf = value;
    msg.len = 1;
    msg.flags = I2C_MSG_READ/*|I2C_MSG_STOP*/;
    ret = i2c_transfer(data->i2c, &msg, 1, OV5640_ADDR);
    return ret;
}

static int ov5640_write(struct device *dev, uint16_t reg, uint8_t value)
{
    const struct ov5640_data *data = DEV_DATA(dev);
    uint8_t buf[3];
    struct i2c_msg msg;
    int ret = 0;

    buf[0] = reg >> 8;
    buf[1] = reg & 0xFF;
    buf[2] = value;
    msg.buf = buf;
    msg.len = 3;
    msg.flags = I2C_MSG_WRITE | I2C_MSG_STOP;

    ret = i2c_transfer(data->i2c, &msg, 1, OV5640_ADDR);
    return ret;
}

/*
* Write a list of register settings;
*/
static int ov5640_write_array(struct device *dev, const struct regval_list *regs, int array_size)
{
    int i = 0;
    int ret = 0;

    if (!regs)
    {
        return -ENOTSUP;
    }

    while (i < array_size)
    {
        ret = ov5640_write(dev, regs->reg, regs->val);
        if (ret != 0)
        {
            return -ENOTSUP;
        }

        i++;
        regs++;
    }

    return 0;
}

static int ov5640_set_brightness(struct device *dev, int value)
{
    int ret;

    switch (value)
    {
        case -4:
            ret = ov5640_write_array(dev, ov5640_brightness_neg4_regs, ARRAY_SIZE(ov5640_brightness_neg4_regs));
            break;
        case -3:
            ret = ov5640_write_array(dev, ov5640_brightness_neg3_regs, ARRAY_SIZE(ov5640_brightness_neg3_regs));
            break;
        case -2:
            ret = ov5640_write_array(dev, ov5640_brightness_neg2_regs, ARRAY_SIZE(ov5640_brightness_neg2_regs));
            break;
        case -1:
            ret = ov5640_write_array(dev, ov5640_brightness_neg1_regs, ARRAY_SIZE(ov5640_brightness_neg1_regs));
            break;
        case 0:
            ret = ov5640_write_array(dev, ov5640_brightness_zero_regs, ARRAY_SIZE(ov5640_brightness_zero_regs));
            break;
        case 1:
            ret = ov5640_write_array(dev, ov5640_brightness_pos1_regs, ARRAY_SIZE(ov5640_brightness_pos1_regs));
            break;
        case 2:
            ret = ov5640_write_array(dev, ov5640_brightness_pos2_regs, ARRAY_SIZE(ov5640_brightness_pos2_regs));
            break;
        case 3:
            ret = ov5640_write_array(dev, ov5640_brightness_pos3_regs, ARRAY_SIZE(ov5640_brightness_pos3_regs));
            break;
        case 4:
            ret = ov5640_write_array(dev, ov5640_brightness_pos4_regs, ARRAY_SIZE(ov5640_brightness_pos4_regs));
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

static int ov5640_set_contrast(struct device *dev, int value)
{
    int ret;

    switch (value)
    {
        case -3:
            ret = ov5640_write_array(dev, ov5640_contrast_neg3_regs, ARRAY_SIZE(ov5640_contrast_neg3_regs));
            break;
        case -2:
            ret = ov5640_write_array(dev, ov5640_contrast_neg2_regs, ARRAY_SIZE(ov5640_contrast_neg2_regs));
            break;
        case -1:
            ret = ov5640_write_array(dev, ov5640_contrast_neg1_regs, ARRAY_SIZE(ov5640_contrast_neg1_regs));
            break;
        case 0:
            ret = ov5640_write_array(dev, ov5640_contrast_zero_regs, ARRAY_SIZE(ov5640_contrast_zero_regs));
            break;
        case 1:
            ret = ov5640_write_array(dev, ov5640_contrast_pos1_regs, ARRAY_SIZE(ov5640_contrast_pos1_regs));
            break;
        case 2:
            ret = ov5640_write_array(dev, ov5640_contrast_pos2_regs, ARRAY_SIZE(ov5640_contrast_pos2_regs));
            break;
        case 3:
            ret = ov5640_write_array(dev, ov5640_contrast_pos3_regs, ARRAY_SIZE(ov5640_contrast_pos3_regs));
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

static int ov5640_set_saturation(struct device *dev, int value)
{
    int ret;

    switch (value)
    {
        case -3:
            ret = ov5640_write_array(dev, ov5640_saturation_neg3_regs, ARRAY_SIZE(ov5640_saturation_neg3_regs));
            break;
        case -2:
            ret = ov5640_write_array(dev, ov5640_saturation_neg2_regs, ARRAY_SIZE(ov5640_saturation_neg2_regs));
            break;
        case -1:
            ret = ov5640_write_array(dev, ov5640_saturation_neg1_regs, ARRAY_SIZE(ov5640_saturation_neg1_regs));
            break;
        case 0:
            ret = ov5640_write_array(dev, ov5640_saturation_zero_regs, ARRAY_SIZE(ov5640_saturation_zero_regs));
            break;
        case 1:
            ret = ov5640_write_array(dev, ov5640_saturation_pos1_regs, ARRAY_SIZE(ov5640_saturation_pos1_regs));
            break;
        case 2:
            ret = ov5640_write_array(dev, ov5640_saturation_pos2_regs, ARRAY_SIZE(ov5640_saturation_pos2_regs));
            break;
        case 3:
            ret = ov5640_write_array(dev, ov5640_saturation_pos3_regs, ARRAY_SIZE(ov5640_saturation_pos3_regs));
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

static int ov5640_set_sharpness(struct device *dev, int value)
{
    if (value < 33)
    {
        ov5640_write(dev, 0x5308, 0x65);
        ov5640_write(dev, 0x5302, value);
    }
    else
    {
        ov5640_write(dev, 0x5308, 0x25);
        ov5640_write(dev, 0x5300, 0x08);
        ov5640_write(dev, 0x5301, 0x30);
        ov5640_write(dev, 0x5302, 0x10);
        ov5640_write(dev, 0x5303, 0x00);
        ov5640_write(dev, 0x5309, 0x08);
        ov5640_write(dev, 0x530a, 0x30);
        ov5640_write(dev, 0x530b, 0x04);
        ov5640_write(dev, 0x530c, 0x06);
    }

    return 0;
}

static int ov5640_set_flash_mode(struct device *dev, int value)
{
    ov5640_write(dev, 0x3016, 0x02);
    ov5640_write(dev, 0x301C, 0x02);

    if (value)
    {
        ov5640_write(dev, 0x3019, 0x02);
    }
    else
    {
        ov5640_write(dev, 0x3019, 0x00);
    }

    return 0;
}

static int ov5640_set_exposure(struct device *dev, int value)
{
    int ret;

    switch (value)
    {
        case -3:
            ret = ov5640_write_array(dev, ov5640_ev_neg3_regs, ARRAY_SIZE(ov5640_ev_neg3_regs));
            break;
        case -2:
            ret = ov5640_write_array(dev, ov5640_ev_neg2_regs, ARRAY_SIZE(ov5640_ev_neg2_regs));
            break;
        case -1:
            ret = ov5640_write_array(dev, ov5640_ev_neg1_regs, ARRAY_SIZE(ov5640_ev_neg1_regs));
            break;
        case 0:
            ret = ov5640_write_array(dev, ov5640_ev_zero_regs, ARRAY_SIZE(ov5640_ev_zero_regs));
            break;
        case 1:
            ret = ov5640_write_array(dev, ov5640_ev_pos1_regs, ARRAY_SIZE(ov5640_ev_pos1_regs));
            break;
        case 2:
            ret = ov5640_write_array(dev, ov5640_ev_pos2_regs, ARRAY_SIZE(ov5640_ev_pos2_regs));
            break;
        case 3:
            ret = ov5640_write_array(dev, ov5640_ev_pos3_regs, ARRAY_SIZE(ov5640_ev_pos3_regs));
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

static int ov5640_set_wb(struct device *dev, enum light_mode value)
{
    int ret;

    switch (value)
    {
        case WB_AUTO:
            ret = ov5640_write_array(dev, ov5640_wb_auto_regs, ARRAY_SIZE(ov5640_wb_auto_regs));
            break;
        case WB_SUNNY:
            ret = ov5640_write_array(dev, ov5640_wb_sunny_regs, ARRAY_SIZE(ov5640_wb_sunny_regs));
            break;
        case WB_OFFICE:
            ret = ov5640_write_array(dev, ov5640_wb_office_regs, ARRAY_SIZE(ov5640_wb_office_regs));
            break;
        case WB_CLOUDY:
            ret = ov5640_write_array(dev, ov5640_wb_cloudy_regs, ARRAY_SIZE(ov5640_wb_cloudy_regs));
            break;
        case WB_HOME:
            ret = ov5640_write_array(dev, ov5640_wb_home_regs, ARRAY_SIZE(ov5640_wb_home_regs));
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

static int ov5640_set_single_af(struct device *dev)
{
    uint8_t val = 0;
    uint16_t retry = 1000;

    ov5640_write(dev, 0x3023, 0x01);
    ov5640_write(dev, 0x3022, 0x03);

    do
    {
        ov5640_read(dev, 0x3023, &val);
        if (val == 0x00)
        {
            break;
        }
        k_sleep(5);
    } while (--retry);

    if (!retry)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int ov5640_set_continueous_af(struct device *dev)
{
    ov5640_write(dev, 0x3022, 0x04);
    ov5640_write(dev, 0x3022, 0x80);

    return 0;
}

/* restore the last set video mode after chip power-on */
static int ov5640_restore_mode(struct device *dev)
{
    return ov5640_write_array(dev, ov5640_init_default_setting, ARRAY_SIZE(ov5640_init_default_setting));
}

static int ov5640_init_controls(struct device *dev)
{
    ov5640_set_wb(dev, WB_AUTO);
    ov5640_set_saturation(dev, 0);
    ov5640_set_brightness(dev, 0);
    ov5640_set_contrast(dev, 0);
    ov5640_set_sharpness(dev, 33);
    ov5640_set_continueous_af(dev);

    return 0;
}

static int ov5640_init_af(struct device *dev)
{
    uint16_t i;
    uint16_t addr = 0x8000;
    uint8_t val = 0;
    uint16_t retry = 1000;

    /* reset sensor MCU  */
    ov5640_write(dev, 0x3000, 0x20);

    /* download af firmware */
    for (i = 0; i < sizeof(ov5640_af_firmware); i++)
    {
        ov5640_write(dev, addr, ov5640_af_firmware[i]);
        addr++;
    }

    /* start af firmware */
    ov5640_write(dev, 0x3022, 0x00);
    ov5640_write(dev, 0x3023, 0x00);
    ov5640_write(dev, 0x3024, 0x00);
    ov5640_write(dev, 0x3025, 0x00);
    ov5640_write(dev, 0x3026, 0x00);
    ov5640_write(dev, 0x3027, 0x00);
    ov5640_write(dev, 0x3028, 0x00);
    ov5640_write(dev, 0x3029, 0x7f);
    ov5640_write(dev, 0x3000, 0x00);

    i = 0;

    do
    {
        ov5640_read(dev, 0x3029, &val);
        if (val == 0x70)
        {
            break;
        }
        k_sleep(5);
    } while (--retry);

    if (!retry)
    {
        return -ENOTSUP;
    }

    return 0;
}

static void ov5640_power(struct device *dev, bool enable)
{
    const struct ov5640_data *data = DEV_DATA(dev);

#ifdef CONFIG_EXTEND_IO
    pcf8574_write(2, enable ? 0 : 1);
#else
    if (data->pwdn_port)
    {
        gpio_pin_write(data->pwdn_port, data->pwdn_pin, enable ? 0 : 1);
    }
#endif
}

static void ov5640_reset(struct device *dev)
{
    const struct ov5640_data *data = DEV_DATA(dev);

    if (data->rst_port)
    {
        gpio_pin_write(data->rst_port, data->rst_pin, 0);

        ov5640_power(dev, false);

        k_sleep(5);

        ov5640_power(dev, true);

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
static inline int ov5640_set_power(struct device *dev, bool on)
{
    struct ov5640_data *data = DEV_DATA(dev);
    int ret = 0;
    uint8_t id_high, id_low;
    uint16_t id = 0;

    if (on)
    {
        ov5640_reset(dev);
        ov5640_power(dev, true);

        /* Read sensor Model ID */
        ov5640_read(dev, 0x300a, &id_high);

        id = id_high << 8;

        ov5640_read(dev, 0x300b, &id_low);

        id |= id_low;

        if (id != OV5640_ID)
        {
            ret = -ENOTSUP;
            goto power_off;
        }

        ov5640_write(dev, 0x3103, 0x11);
        ov5640_write(dev, 0x3008, 0x82);

        k_sleep(10);

        ret = ov5640_restore_mode(dev);
        if (ret)
        {
            goto power_off;
        }

        data->load_af_firmware = true;

        return 0;
    }

power_off:
    ov5640_power(dev, false);

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
static inline int ov5640_set_fmt(struct device *dev, enum camera_output_format format, enum camera_mode_id id, enum camera_frame_rate frame_rate)
{
    struct ov5640_data *data = DEV_DATA(dev);
    int ret = 0;

    switch (id)
    {
        case MODE_QVGA_320_240:
            if (frame_rate == FPS_15)
            {
                ret = ov5640_write_array(dev, ov5640_setting_15fps_QVGA_320_240, ARRAY_SIZE(ov5640_setting_15fps_QVGA_320_240));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 320;
                    data->pic_height = 240;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = ov5640_write_array(dev, ov5640_setting_30fps_QVGA_320_240, ARRAY_SIZE(ov5640_setting_30fps_QVGA_320_240));
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
                ret = ov5640_write_array(dev, ov5640_setting_15fps_VGA_640_480, ARRAY_SIZE(ov5640_setting_15fps_VGA_640_480));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 640;
                    data->pic_height = 480;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = ov5640_write_array(dev, ov5640_setting_30fps_VGA_640_480, ARRAY_SIZE(ov5640_setting_30fps_VGA_640_480));
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
        case MODE_XGA_1024_768:
            if (frame_rate == FPS_15)
            {
                ret = ov5640_write_array(dev, ov5640_setting_15fps_XGA_1024_768, ARRAY_SIZE(ov5640_setting_15fps_XGA_1024_768));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 1024;
                    data->pic_height = 768;
                }
            }
            else
            {
                return -ENOTSUP;
            }
            break;
        case MODE_720P_1280_720:
            if (frame_rate == FPS_15)
            {
                ret = ov5640_write_array(dev, ov5640_setting_15fps_720P_1280_720, ARRAY_SIZE(ov5640_setting_15fps_720P_1280_720));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 1280;
                    data->pic_height = 720;
                }
            }
            else
            {
                return -ENOTSUP;
            }
            break;
        case MODE_QSXGA_2592_1944:
            if (frame_rate == FPS_15)
            {
                ret = ov5640_write_array(dev, ov5640_setting_15fps_QSXGA_2592_1944, ARRAY_SIZE(ov5640_setting_15fps_QSXGA_2592_1944));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 2592;
                    data->pic_height = 1944;
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
        case VIDEO_FMT_RAW:
            ret = ov5640_write_array(dev, ov5640_setting_FMT_RAW, ARRAY_SIZE(ov5640_setting_FMT_RAW));
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_RAW;
            }
            break;
        case VIDEO_FMT_YUV422:
            ret = ov5640_write_array(dev, ov5640_setting_FMT_YUV422, ARRAY_SIZE(ov5640_setting_FMT_YUV422));
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_YUV422;
            }
            break;
        case VIDEO_FMT_RGB565:
            ret = ov5640_write_array(dev, ov5640_setting_FMT_RGB565, ARRAY_SIZE(ov5640_setting_FMT_RGB565));
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_RGB565;
            }
            break;
        case VIDEO_FMT_JPEG:
            ret = ov5640_write_array(dev, ov5640_setting_FMT_JPEG, ARRAY_SIZE(ov5640_setting_FMT_JPEG));
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_JPEG;
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
static int ov5640_set_framesize(struct device *dev, uint16_t xsize, uint16_t ysize)
{
    struct ov5640_data *data = DEV_DATA(dev);

    data->pic_width = xsize;
    data->pic_height = ysize;

    ov5640_write(dev, 0x3808, (uint8_t)(xsize >> 8));
    ov5640_write(dev, 0x3809, (uint8_t)(xsize & 0xFF));
    ov5640_write(dev, 0x380a, (uint8_t)(ysize >> 8));
    ov5640_write(dev, 0x380b, (uint8_t)(ysize & 0xFF));

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
static inline int ov5640_get_fmt(struct device *dev, struct video_format *fmt)
{
    struct ov5640_data *data = DEV_DATA(dev);

    fmt->width = data->pic_width;
    fmt->height = data->pic_height;
    fmt->framerate = data->framerate;
    fmt->pixfmt = data->pixfmt;

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
static inline int ov5640_s_ctrl(struct device *dev, uint32_t cmd, void *arg)
{
    struct ov5640_data *data = DEV_DATA(dev);
    struct video_format *fmt;
    int ret;
    int value;

    switch (cmd)
    {
        case CAMERA_CID_BRIGHTNESS:
            value = *(int *)arg;
            ret = ov5640_set_brightness(dev, value);
            break;
        case CAMERA_CID_CONTRAST:
            value = *(int *)arg;
            ret = ov5640_set_contrast(dev, value);
            break;
        case CAMERA_CID_SATURATION:
            value = *(int *)arg;
            ret = ov5640_set_saturation(dev, value);
            break;
        case CAMERA_CID_SHARPNESS:
            value = *(int *)arg;
            ret = ov5640_set_sharpness(dev, value);
            break;
        case CAMERA_CID_FLASH_MODE:
            value = *(int *)arg;
            ret = ov5640_set_flash_mode(dev, value);
            break;
        case CAMERA_CID_FOCUS_AUTO:
            value = *(enum af_mode *)arg;

            if (data->load_af_firmware)
            {
                ret = ov5640_init_af(dev);
                if (ret)
                {
                    return -ENOTSUP;
                }
                data->load_af_firmware = false;
            }

            if (value == AF_SINGLE)
            {
                ret = ov5640_set_single_af(dev);
            }
            else if (value == AF_CONTINUEOUS)
            {
                ret = ov5640_set_continueous_af(dev);
            }
            else
            {
                return -ENOTSUP;
            }
            break;
        case CAMERA_CID_EXPOSURE:
            value = *(int *)arg;
            ret = ov5640_set_exposure(dev, value);
            break;
        case CAMERA_CID_WHITE_BALANCE:
            value = *(enum light_mode *)arg;
            ret = ov5640_set_wb(dev, value);
            break;
        case CAMERA_CID_FRAME_SIZE:
            fmt = (struct video_format *)arg;
            ret = ov5640_set_framesize(dev, fmt->width, fmt->height);
            break;
        default:
            break;
    }

    return 0;
}

static const struct camera_driver_api ov5640_drv_api_funcs =
{
    .set_power = ov5640_set_power,
    .set_fmt = ov5640_set_fmt,
    .get_fmt = ov5640_get_fmt,
    .ioctl = ov5640_s_ctrl,
};

static int ov5640_init(struct device *dev)
{
    struct ov5640_data *data = DEV_DATA(dev);
    int ret = 0;
    uint32_t config_i2c0 = 0x14;

    data->i2c = device_get_binding(data->i2c_name);

    if (data->i2c == NULL)
    {
        return -ENOTSUP;
    }

    i2c_configure(data->i2c, config_i2c0);


#ifdef CONFIG_EXTEND_IO
    pcf8574_init();
#else
    /* GPIO clock and reset */
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_VIN);
    HAL_Reset_Module(RESET_VIN_SW_RSTJ);

    data->pwdn_port = device_get_binding(data->pwdn_port_name);

    if (data->pwdn_port)
    {
        gpio_pin_configure(data->pwdn_port, data->pwdn_pin, GPIO_DIR_OUT | GPIO_PUD_PULL_UP);
    }
#endif

    data->rst_port = device_get_binding(data->rst_port_name);

    if (data->rst_port)
    {
        gpio_pin_configure(data->rst_port, data->rst_pin, GPIO_DIR_OUT | GPIO_PUD_PULL_UP);
    }

    data->load_af_firmware = true;

#if 0
    ret = ov5640_set_power(dev, true);
    if (ret < 0)
    {
        return ret;
    }

    ret = ov5640_set_fmt(dev, VIDEO_FMT_RGB565, MODE_VGA_640_480, FPS_15);
    if (ret < 0)
    {
        return ret;
    }

    ret = ov5640_init_controls(dev);
    if (ret < 0)
    {
        return ret;
    }
#endif

    return 0;
}

#ifdef CONFIG_OV5640_0
static struct ov5640_data ov5640_dev_data_0 =
{
    .i2c_name = CONFIG_CAMERA_0_I2C_NAME,
    .rst_port_name = CAMERA_0_GPIO_RST_PORT,
    .pwdn_port_name = CAMERA_0_GPIO_PWDN_PORT,
    .rst_pin = CAMERA_0_GPIO_RST_PIN,
    .pwdn_pin = CAMERA_0_GPIO_PWDN_PIN,
    .pic_width = 640,
    .pic_height = 480,
    .framerate = 15,
    .pixfmt = VIDEO_FMT_RGB565,
};

DEVICE_AND_API_INIT(ov5640_0, OV5640_0_DEV_NAME,
                    ov5640_init, &ov5640_dev_data_0, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &ov5640_drv_api_funcs);
#endif

#ifdef CONFIG_OV5640_1
static struct ov5640_data ov5640_dev_data_1 =
{
    .i2c_name = CONFIG_CAMERA_1_I2C_NAME,
    .rst_port_name = CAMERA_1_GPIO_RST_PORT,
    .pwdn_port_name = CAMERA_1_GPIO_PWDN_PORT,
    .rst_pin = CAMERA_1_GPIO_RST_PIN,
    .pwdn_pin = CAMERA_1_GPIO_PWDN_PIN,
    .pic_width = 640,
    .pic_height = 480,
    .framerate = 15,
    .pixfmt = VIDEO_FMT_RGB565,
};

DEVICE_AND_API_INIT(ov5640_1, OV5640_1_DEV_NAME,
                    ov5640_init, &ov5640_dev_data_1, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &ov5640_drv_api_funcs);
#endif

