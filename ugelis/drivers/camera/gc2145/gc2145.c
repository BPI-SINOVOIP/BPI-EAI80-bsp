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
#include "gc2145_reg.h"

#define GC2145_ID      0x2145
#define GC2145_ADDR    0x78 //write:0x78 read:0x79

struct regval_list
{
    uint8_t reg;
    uint8_t val;
};

/** Runtime driver data */
struct gc2145_data
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
    ((struct gc2145_data * const)(dev)->driver_data)

/*
 * The default register settings
 *
 */
static const struct regval_list gc2145_init_default_setting[] =
{
    {0xfe, 0xf0},
    {0xfe, 0xf0},
    {0xfe, 0xf0},
    {0xfc, 0x06},
    {0xf6, 0x00},
    {0xf7, 0x1d},
    {0xf8, 0x84},
    {0xfa, 0x10},
    {0xf9, 0xfe},
    {0xf2, 0x00},
    /////////////////////////////////////////////////
    //////////////////ISP reg//////////////////////
    ////////////////////////////////////////////////////
    {0xfe, 0x00},
    {0x03, 0x04},
    {0x04, 0xe2},
    {0x09, 0x00},
    {0x0a, 0x00},
    {0x0b, 0x00},
    {0x0c, 0x00},

    {0x0d, 0x04},//window size
    {0x0e, 0xb0},
    {0x0f, 0x06},
    {0x10, 0x40},

    {0x12, 0x2e},
    {0x17, 0x14}, //mirror
    {0x18, 0x22},
    {0x19, 0x0e},
    {0x1a, 0x01},
    {0x1b, 0x4b},
    {0x1c, 0x07},
    {0x1d, 0x10},
    {0x1e, 0x88},
    {0x1f, 0x78},
    {0x20, 0x03},
    {0x21, 0x40},
    {0x22, 0xa0},
    {0x24, 0x16},
    {0x25, 0x01},
    {0x26, 0x10},
    {0x2d, 0x60},
    {0x30, 0x01},
    {0x31, 0x90},
    {0x33, 0x06},
    {0x34, 0x01},
    /////////////////////////////////////////////////
    //////////////////ISP reg////////////////////
    /////////////////////////////////////////////////
    {0xfe, 0x00},
    {0x80, 0x7f},
    {0x81, 0x26},
    {0x82, 0xfa},
    {0x83, 0x00},
    {0x84, 0x02},
    {0x86, 0x02},
    {0x88, 0x03},
    {0x89, 0x23},   //switch first byte and second byte
    {0x85, 0x08},
    {0x8a, 0x00},
    {0x8b, 0x00},
    {0xb0, 0x55},
    {0xc3, 0x00},
    {0xc4, 0x80},
    {0xc5, 0x90},
    {0xc6, 0x3b},
    {0xc7, 0x46},
    {0xec, 0x06},
    {0xed, 0x04},
    {0xee, 0x60},
    {0xef, 0x90},
    {0xb6, 0x01},

    {0x90, 0x01},//crop function
    {0x91, 0x01},
    {0x92, 0x68},
    {0x93, 0x01},
    {0x94, 0xe0},
    {0x95, 0x01},
    {0x96, 0xe0},
    {0x97, 0x02},
    {0x98, 0x80},
    /////////////////////////////////////////
    /////////// BLK ////////////////////////
    /////////////////////////////////////////
    {0xfe, 0x00},
    {0x40, 0x42},
    {0x41, 0x00},
    {0x43, 0x5b},
    {0x5e, 0x00},
    {0x5f, 0x00},
    {0x60, 0x00},
    {0x61, 0x00},
    {0x62, 0x00},
    {0x63, 0x00},
    {0x64, 0x00},
    {0x65, 0x00},
    {0x66, 0x20},
    {0x67, 0x20},
    {0x68, 0x20},
    {0x69, 0x20},
    {0x76, 0x00},
    {0x6a, 0x08},
    {0x6b, 0x08},
    {0x6c, 0x08},
    {0x6d, 0x08},
    {0x6e, 0x08},
    {0x6f, 0x08},
    {0x70, 0x08},
    {0x71, 0x08},
    {0x76, 0x00},
    {0x72, 0xf0},
    {0x7e, 0x3c},
    {0x7f, 0x00},
    {0xfe, 0x02},
    {0x48, 0x15},
    {0x49, 0x00},
    {0x4b, 0x0b},
    {0xfe, 0x00},
    ////////////////////////////////////////
    /////////// AEC ////////////////////////
    ////////////////////////////////////////
    {0xfe, 0x01},
    {0x01, 0x04},
    {0x02, 0xc0},
    {0x03, 0x04},
    {0x04, 0x90},
    {0x05, 0x30},
    {0x06, 0x90},
    {0x07, 0x30},
    {0x08, 0x80},
    {0x09, 0x00},
    {0x0a, 0x82},
    {0x0b, 0x11},
    {0x0c, 0x10},
    {0x11, 0x10},
    {0x13, 0x7b},
    {0x17, 0x00},
    {0x1c, 0x11},
    {0x1e, 0x61},
    {0x1f, 0x35},
    {0x20, 0x40},
    {0x22, 0x40},
    {0x23, 0x20},
    {0xfe, 0x02},
    {0x0f, 0x04},
    {0xfe, 0x01},
    {0x12, 0x35},
    {0x15, 0xb0},
    {0x10, 0x31},
    {0x3e, 0x28},
    {0x3f, 0xb0},
    {0x40, 0x90},
    {0x41, 0x0f},

    /////////////////////////////
    //////// INTPEE /////////////
    /////////////////////////////
    {0xfe, 0x02},
    {0x90, 0x6c},
    {0x91, 0x03},
    {0x92, 0xcb},
    {0x94, 0x33},
    {0x95, 0x84},
    {0x97, 0x65},
    {0xa2, 0x11},
    {0xfe, 0x00},
    /////////////////////////////
    //////// DNDD///////////////
    /////////////////////////////
    {0xfe, 0x02},
    {0x80, 0xc1},
    {0x81, 0x08},
    {0x82, 0x05},
    {0x83, 0x08},
    {0x84, 0x0a},
    {0x86, 0xf0},
    {0x87, 0x50},
    {0x88, 0x15},
    {0x89, 0xb0},
    {0x8a, 0x30},
    {0x8b, 0x10},
    /////////////////////////////////////////
    /////////// ASDE ////////////////////////
    /////////////////////////////////////////
    {0xfe, 0x01},
    {0x21, 0x04},
    {0xfe, 0x02},
    {0xa3, 0x50},
    {0xa4, 0x20},
    {0xa5, 0x40},
    {0xa6, 0x80},
    {0xab, 0x40},
    {0xae, 0x0c},
    {0xb3, 0x46},
    {0xb4, 0x64},
    {0xb6, 0x38},
    {0xb7, 0x01},
    {0xb9, 0x2b},
    {0x3c, 0x04},
    {0x3d, 0x15},
    {0x4b, 0x06},
    {0x4c, 0x20},
    {0xfe, 0x00},
    /////////////////////////////////////////
    /////////// GAMMA   ////////////////////////
    /////////////////////////////////////////

    ///////////////////gamma1////////////////////
#if 1
    {0xfe, 0x02},
    {0x10, 0x09},
    {0x11, 0x0d},
    {0x12, 0x13},
    {0x13, 0x19},
    {0x14, 0x27},
    {0x15, 0x37},
    {0x16, 0x45},
    {0x17, 0x53},
    {0x18, 0x69},
    {0x19, 0x7d},
    {0x1a, 0x8f},
    {0x1b, 0x9d},
    {0x1c, 0xa9},
    {0x1d, 0xbd},
    {0x1e, 0xcd},
    {0x1f, 0xd9},
    {0x20, 0xe3},
    {0x21, 0xea},
    {0x22, 0xef},
    {0x23, 0xf5},
    {0x24, 0xf9},
    {0x25, 0xff},
#else
    {0xfe, 0x02},
    {0x10, 0x0a},
    {0x11, 0x12},
    {0x12, 0x19},
    {0x13, 0x1f},
    {0x14, 0x2c},
    {0x15, 0x38},
    {0x16, 0x42},
    {0x17, 0x4e},
    {0x18, 0x63},
    {0x19, 0x76},
    {0x1a, 0x87},
    {0x1b, 0x96},
    {0x1c, 0xa2},
    {0x1d, 0xb8},
    {0x1e, 0xcb},
    {0x1f, 0xd8},
    {0x20, 0xe2},
    {0x21, 0xe9},
    {0x22, 0xf0},
    {0x23, 0xf8},
    {0x24, 0xfd},
    {0x25, 0xff},
    {0xfe, 0x00},
#endif
    {0xfe, 0x00},
    {0xc6, 0x20},
    {0xc7, 0x2b},
    ///////////////////gamma2////////////////////
#if 1
    {0xfe, 0x02},
    {0x26, 0x0f},
    {0x27, 0x14},
    {0x28, 0x19},
    {0x29, 0x1e},
    {0x2a, 0x27},
    {0x2b, 0x33},
    {0x2c, 0x3b},
    {0x2d, 0x45},
    {0x2e, 0x59},
    {0x2f, 0x69},
    {0x30, 0x7c},
    {0x31, 0x89},
    {0x32, 0x98},
    {0x33, 0xae},
    {0x34, 0xc0},
    {0x35, 0xcf},
    {0x36, 0xda},
    {0x37, 0xe2},
    {0x38, 0xe9},
    {0x39, 0xf3},
    {0x3a, 0xf9},
    {0x3b, 0xff},
#else
    ////Gamma outdoor
    {0xfe, 0x02},
    {0x26, 0x17},
    {0x27, 0x18},
    {0x28, 0x1c},
    {0x29, 0x20},
    {0x2a, 0x28},
    {0x2b, 0x34},
    {0x2c, 0x40},
    {0x2d, 0x49},
    {0x2e, 0x5b},
    {0x2f, 0x6d},
    {0x30, 0x7d},
    {0x31, 0x89},
    {0x32, 0x97},
    {0x33, 0xac},
    {0x34, 0xc0},
    {0x35, 0xcf},
    {0x36, 0xda},
    {0x37, 0xe5},
    {0x38, 0xec},
    {0x39, 0xf8},
    {0x3a, 0xfd},
    {0x3b, 0xff},
#endif
    ///////////////////////////////////////////////
    ///////////YCP ///////////////////////
    ///////////////////////////////////////////////
    {0xfe, 0x02},
    {0xd1, 0x32},
    {0xd2, 0x32},
    {0xd3, 0x40},
    {0xd6, 0xf0},
    {0xd7, 0x10},
    {0xd8, 0xda},
    {0xdd, 0x14},
    {0xde, 0x86},
    {0xed, 0x80},
    {0xee, 0x00},
    {0xef, 0x3f},
    {0xd8, 0xd8},
    ///////////////////abs/////////////////
    {0xfe, 0x01},
    {0x9f, 0x40},
    /////////////////////////////////////////////
    //////////////////////// LSC ///////////////
    //////////////////////////////////////////
    {0xfe, 0x01},
    {0xc2, 0x14},
    {0xc3, 0x0d},
    {0xc4, 0x0c},
    {0xc8, 0x15},
    {0xc9, 0x0d},
    {0xca, 0x0a},
    {0xbc, 0x24},
    {0xbd, 0x10},
    {0xbe, 0x0b},
    {0xb6, 0x25},
    {0xb7, 0x16},
    {0xb8, 0x15},
    {0xc5, 0x00},
    {0xc6, 0x00},
    {0xc7, 0x00},
    {0xcb, 0x00},
    {0xcc, 0x00},
    {0xcd, 0x00},
    {0xbf, 0x07},
    {0xc0, 0x00},
    {0xc1, 0x00},
    {0xb9, 0x00},
    {0xba, 0x00},
    {0xbb, 0x00},
    {0xaa, 0x01},
    {0xab, 0x01},
    {0xac, 0x00},
    {0xad, 0x05},
    {0xae, 0x06},
    {0xaf, 0x0e},
    {0xb0, 0x0b},
    {0xb1, 0x07},
    {0xb2, 0x06},
    {0xb3, 0x17},
    {0xb4, 0x0e},
    {0xb5, 0x0e},
    {0xd0, 0x09},
    {0xd1, 0x00},
    {0xd2, 0x00},
    {0xd6, 0x08},
    {0xd7, 0x00},
    {0xd8, 0x00},
    {0xd9, 0x00},
    {0xda, 0x00},
    {0xdb, 0x00},
    {0xd3, 0x0a},
    {0xd4, 0x00},
    {0xd5, 0x00},
    {0xa4, 0x00},
    {0xa5, 0x00},
    {0xa6, 0x77},
    {0xa7, 0x77},
    {0xa8, 0x77},
    {0xa9, 0x77},
    {0xa1, 0x80},
    {0xa2, 0x80},

    {0xfe, 0x01},
    {0xdf, 0x0d},
    {0xdc, 0x25},
    {0xdd, 0x30},
    {0xe0, 0x77},
    {0xe1, 0x80},
    {0xe2, 0x77},
    {0xe3, 0x90},
    {0xe6, 0x90},
    {0xe7, 0xa0},
    {0xe8, 0x90},
    {0xe9, 0xa0},
    {0xfe, 0x00},
    ///////////////////////////////////////////////
    /////////// AWB////////////////////////
    ///////////////////////////////////////////////
    {0xfe, 0x01},
    {0x4f, 0x00},
    {0x4f, 0x00},
    {0x4b, 0x01},
    {0x4f, 0x00},

    {0x4c, 0x01}, // D75
    {0x4d, 0x71},
    {0x4e, 0x01},
    {0x4c, 0x01},
    {0x4d, 0x91},
    {0x4e, 0x01},
    {0x4c, 0x01},
    {0x4d, 0x70},
    {0x4e, 0x01},
    {0x4c, 0x01}, // D65
    {0x4d, 0x90},
    {0x4e, 0x02},
    {0x4c, 0x01},
    {0x4d, 0xb0},
    {0x4e, 0x02},
    {0x4c, 0x01},
    {0x4d, 0x8f},
    {0x4e, 0x02},
    {0x4c, 0x01},
    {0x4d, 0x6f},
    {0x4e, 0x02},
    {0x4c, 0x01},
    {0x4d, 0xaf},
    {0x4e, 0x02},
    {0x4c, 0x01},
    {0x4d, 0xd0},
    {0x4e, 0x02},
    {0x4c, 0x01},
    {0x4d, 0xf0},
    {0x4e, 0x02},
    {0x4c, 0x01},
    {0x4d, 0xcf},
    {0x4e, 0x02},
    {0x4c, 0x01},
    {0x4d, 0xef},
    {0x4e, 0x02},
    {0x4c, 0x01},//D50
    {0x4d, 0x6e},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0x8e},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0xae},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0xce},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0x4d},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0x6d},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0x8d},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0xad},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0xcd},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0x4c},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0x6c},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0x8c},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0xac},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0xcc},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0xcb},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0x4b},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0x6b},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0x8b},
    {0x4e, 0x03},
    {0x4c, 0x01},
    {0x4d, 0xab},
    {0x4e, 0x03},
    {0x4c, 0x01},//CWF
    {0x4d, 0x8a},
    {0x4e, 0x04},
    {0x4c, 0x01},
    {0x4d, 0xaa},
    {0x4e, 0x04},
    {0x4c, 0x01},
    {0x4d, 0xca},
    {0x4e, 0x04},
    {0x4c, 0x01},
    {0x4d, 0xca},
    {0x4e, 0x04},
    {0x4c, 0x01},
    {0x4d, 0xc9},
    {0x4e, 0x04},
    {0x4c, 0x01},
    {0x4d, 0x8a},
    {0x4e, 0x04},
    {0x4c, 0x01},
    {0x4d, 0x89},
    {0x4e, 0x04},
    {0x4c, 0x01},
    {0x4d, 0xa9},
    {0x4e, 0x04},
    {0x4c, 0x02},//tl84
    {0x4d, 0x0b},
    {0x4e, 0x05},
    {0x4c, 0x02},
    {0x4d, 0x0a},
    {0x4e, 0x05},
    {0x4c, 0x01},
    {0x4d, 0xeb},
    {0x4e, 0x05},
    {0x4c, 0x01},
    {0x4d, 0xea},
    {0x4e, 0x05},
    {0x4c, 0x02},
    {0x4d, 0x09},
    {0x4e, 0x05},
    {0x4c, 0x02},
    {0x4d, 0x29},
    {0x4e, 0x05},
    {0x4c, 0x02},
    {0x4d, 0x2a},
    {0x4e, 0x05},
    {0x4c, 0x02},
    {0x4d, 0x4a},
    {0x4e, 0x05},
    //{0x4c, 0x02}, //A
    //{0x4d, 0x6a},
    //{0x4e, 0x06},
    {0x4c, 0x02},
    {0x4d, 0x8a},
    {0x4e, 0x06},
    {0x4c, 0x02},
    {0x4d, 0x49},
    {0x4e, 0x06},
    {0x4c, 0x02},
    {0x4d, 0x69},
    {0x4e, 0x06},
    {0x4c, 0x02},
    {0x4d, 0x89},
    {0x4e, 0x06},
    {0x4c, 0x02},
    {0x4d, 0xa9},
    {0x4e, 0x06},
    {0x4c, 0x02},
    {0x4d, 0x48},
    {0x4e, 0x06},
    {0x4c, 0x02},
    {0x4d, 0x68},
    {0x4e, 0x06},
    {0x4c, 0x02},
    {0x4d, 0x69},
    {0x4e, 0x06},
    {0x4c, 0x02},//H
    {0x4d, 0xca},
    {0x4e, 0x07},
    {0x4c, 0x02},
    {0x4d, 0xc9},
    {0x4e, 0x07},
    {0x4c, 0x02},
    {0x4d, 0xe9},
    {0x4e, 0x07},
    {0x4c, 0x03},
    {0x4d, 0x09},
    {0x4e, 0x07},
    {0x4c, 0x02},
    {0x4d, 0xc8},
    {0x4e, 0x07},
    {0x4c, 0x02},
    {0x4d, 0xe8},
    {0x4e, 0x07},
    {0x4c, 0x02},
    {0x4d, 0xa7},
    {0x4e, 0x07},
    {0x4c, 0x02},
    {0x4d, 0xc7},
    {0x4e, 0x07},
    {0x4c, 0x02},
    {0x4d, 0xe7},
    {0x4e, 0x07},
    {0x4c, 0x03},
    {0x4d, 0x07},
    {0x4e, 0x07},

    {0x4f, 0x01},
    {0x50, 0x80},
    {0x51, 0xa8},
    {0x52, 0x47},
    {0x53, 0x38},
    {0x54, 0xc7},
    {0x56, 0x0e},
    {0x58, 0x08},
    {0x5b, 0x00},
    {0x5c, 0x74},
    {0x5d, 0x8b},
    {0x61, 0xdb},
    {0x62, 0xb8},
    {0x63, 0x86},
    {0x64, 0xc0},
    {0x65, 0x04},
    {0x67, 0xa8},
    {0x68, 0xb0},
    {0x69, 0x00},
    {0x6a, 0xa8},
    {0x6b, 0xb0},
    {0x6c, 0xaf},
    {0x6d, 0x8b},
    {0x6e, 0x50},
    {0x6f, 0x18},
    {0x73, 0xf0},
    {0x70, 0x0d},
    {0x71, 0x60},
    {0x72, 0x80},
    {0x74, 0x01},
    {0x75, 0x01},
    {0x7f, 0x0c},
    {0x76, 0x70},
    {0x77, 0x58},
    {0x78, 0xa0},
    {0x79, 0x5e},
    {0x7a, 0x54},
    {0x7b, 0x58},
    {0xfe, 0x00},
    //////////////////////////////////////////
    ///////////CC////////////////////////
    //////////////////////////////////////////
    {0xfe, 0x02},
    {0xc0, 0x01},
    {0xc1, 0x44},
    {0xc2, 0xfd},
    {0xc3, 0x04},
    {0xc4, 0xF0},
    {0xc5, 0x48},
    {0xc6, 0xfd},
    {0xc7, 0x46},
    {0xc8, 0xfd},
    {0xc9, 0x02},
    {0xca, 0xe0},
    {0xcb, 0x45},
    {0xcc, 0xec},
    {0xcd, 0x48},
    {0xce, 0xf0},
    {0xcf, 0xf0},
    {0xe3, 0x0c},
    {0xe4, 0x4b},
    {0xe5, 0xe0},
    //////////////////////////////////////////
    ///////////ABS ////////////////////
    //////////////////////////////////////////
    {0xfe, 0x01},
    {0x9f, 0x40},
    {0xfe, 0x00},
    //////////////////////////////////////
    ///////////  OUTPUT   ////////////////
    //////////////////////////////////////
    {0xfe, 0x00},
    {0xf2, 0x0f},
    ///////////////dark sun////////////////////
    {0xfe, 0x02},
    {0x40, 0xbf},
    {0x46, 0xcf},
    {0xfe, 0x00},

    //////////////frame rate 50Hz/////////
    {0xfe, 0x00},
    {0x05, 0x01},
    {0x06, 0x56},
    {0x07, 0x00},
    {0x08, 0x32},
    {0xfe, 0x01},
    {0x25, 0x00},
    {0x26, 0xfa},
    {0x27, 0x04},
    {0x28, 0xe2}, //20fps
    {0x29, 0x06},
    {0x2a, 0xd6}, //14fps
    {0x2b, 0x07},
    {0x2c, 0xd0}, //12fps
    {0x2d, 0x0b},
    {0x2e, 0xb8}, //8fps
    {0xfe, 0x00},
};

static const struct regval_list gc2145_setting_30fps_QVGA_320_240[] =
{
    {0xfe, 0x00},   //select page 0
    {0x90, 0x01},   //crop function, the background pixels is 1600x1200,need to crop in the mid
    {0x91, 0x01},   //y start[10:8]
    {0x92, 0xe0},   //y start[7:0]
    {0x93, 0x02},   //x start[10:8]
    {0x94, 0x80},   //x start[7:0]
    {0x95, 0x00},   //crop height[10:8]
    {0x96, 0xf0},   //crop height[7:0]
    {0x97, 0x01},   //crop width[10:8]
    {0x98, 0x40},   //crop width[7:0]

    //{0xfa, 0x00},   //clock divide by, default value is 0x11, when set 0x00, pixel clock is 100M, VIN capability is 50M
};

static const struct regval_list gc2145_setting_15fps_QVGA_320_240[] =
{
    {0xfe, 0x00},   //select page 0
    {0x90, 0x01},   //crop function, the background pixels is 1600x1200,need to crop in the mid
    {0x91, 0x01},   //y start[10:8]
    {0x92, 0xe0},   //y start[7:0]
    {0x93, 0x02},   //x start[10:8]
    {0x94, 0x80},   //x start[7:0]
    {0x95, 0x00},   //crop height[10:8]
    {0x96, 0xf0},   //crop height[7:0]
    {0x97, 0x01},   //crop width[10:8]
    {0x98, 0x40},   //crop width[7:0]

    //{0xfa, 0x00},   //clock divide by, default value is 0x11, when set 0x00, pixel clock is 100M, VIN capability is 50M
};

static const struct regval_list gc2145_setting_30fps_VGA_640_480[] =
{
    {0xfe, 0x00},   //select page 0
    {0x90, 0x01},   //crop function, the background pixels is 1600x1200,need to crop in the mid
    {0x91, 0x01},   //y start[10:8]
    {0x92, 0x68},   //y start[7:0]
    {0x93, 0x01},   //x start[10:8]
    {0x94, 0xe0},   //x start[7:0]
    {0x95, 0x01},   //crop height[10:8]
    {0x96, 0xe0},   //crop height[7:0]
    {0x97, 0x02},   //crop width[10:8]
    {0x98, 0x80},   //crop width[7:0]

    //{0xfa, 0x00},   //clock divide by, default value is 0x11, when set 0x00, pixel clock is 100M, VIN capability is 50M
};

static const struct regval_list gc2145_setting_15fps_VGA_640_480[] =
{
    {0xfe, 0x00},   //select page 0
    {0x90, 0x01},   //crop function, the background pixels is 1600x1200,need to crop in the mid
    {0x91, 0x01},   //y start[10:8]
    {0x92, 0x68},   //y start[7:0]
    {0x93, 0x01},   //x start[10:8]
    {0x94, 0xe0},   //x start[7:0]
    {0x95, 0x01},   //crop height[10:8]
    {0x96, 0xe0},   //crop height[7:0]
    {0x97, 0x02},   //crop width[10:8]
    {0x98, 0x80},   //crop width[7:0]

    //{0xfa, 0x00},   //clock divide by, default value is 0x11, when set 0x00, pixel clock is 100M, VIN capability is 50M
};

static const struct regval_list gc2145_setting_30fps_XGA_1024_768[] =
{
    {0xfe, 0x00},   //select page 0
    {0x90, 0x01},   //crop function, the background pixels is 1600x1200,need to crop in the mid
    {0x91, 0x00},   //y start[10:8]
    {0x92, 0xd8},   //y start[7:0]
    {0x93, 0x01},   //x start[10:8]
    {0x94, 0x20},   //x start[7:0]
    {0x95, 0x03},   //crop height[10:8]
    {0x96, 0x00},   //crop height[7:0]
    {0x97, 0x04},   //crop width[10:8]
    {0x98, 0x00},   //crop width[7:0]

    //{0xfa, 0x00},   //clock divide by, default value is 0x11, when set 0x00, pixel clock is 100M, VIN capability is 50M
};

static const struct regval_list gc2145_setting_15fps_XGA_1024_768[] =
{
    {0xfe, 0x00},   //select page 0
    {0x90, 0x01},   //crop function, the background pixels is 1600x1200,need to crop in the mid
    {0x91, 0x00},   //y start[10:8]
    {0x92, 0xd8},   //y start[7:0]
    {0x93, 0x01},   //x start[10:8]
    {0x94, 0x20},   //x start[7:0]
    {0x95, 0x03},   //crop height[10:8]
    {0x96, 0x00},   //crop height[7:0]
    {0x97, 0x04},   //crop width[10:8]
    {0x98, 0x00},   //crop width[7:0]

    //{0xfa, 0x00},   //clock divide by, default value is 0x11, when set 0x00, pixel clock is 100M, VIN capability is 50M
};

static const struct regval_list gc2145_setting_30fps_720P_1280_720[] =
{
    {0xfe, 0x00},   //select page 0
    {0x90, 0x01},   //crop function, the background pixels is 1600x1200,need to crop in the mid
    {0x91, 0x00},   //y start[10:8]
    {0x92, 0xf0},   //y start[7:0]
    {0x93, 0x00},   //x start[10:8]
    {0x94, 0xa0},   //x start[7:0]
    {0x95, 0x02},   //crop height[10:8]
    {0x96, 0xd0},   //crop height[7:0]
    {0x97, 0x05},   //crop width[10:8]
    {0x98, 0x00},   //crop width[7:0]

    //{0xfa, 0x00},   //clock divide by, default value is 0x11, when set 0x00, pixel clock is 100M, VIN capability is 50M
};

static const struct regval_list gc2145_setting_15fps_720P_1280_720[] =
{
    {0xfe, 0x00},   //select page 0
    {0x90, 0x01},   //crop function, the background pixels is 1600x1200,need to crop in the mid
    {0x91, 0x00},   //y start[10:8]
    {0x92, 0xf0},   //y start[7:0]
    {0x93, 0x00},   //x start[10:8]
    {0x94, 0xa0},   //x start[7:0]
    {0x95, 0x02},   //crop height[10:8]
    {0x96, 0xd0},   //crop height[7:0]
    {0x97, 0x05},   //crop width[10:8]
    {0x98, 0x00},   //crop width[7:0]

    //{0xfa, 0x00},   //clock divide by, default value is 0x11, when set 0x00, pixel clock is 100M, VIN capability is 50M
};

static const struct regval_list gc2145_wb_sunny_regs[] =
{
    {0xfe, 0x00 },  //select page 0
    {0xb3, 0x70 },  //AWB R gain, default 0x40
    {0xb4, 0x40 },  //AWB G gain, default 0x40
    {0xb5, 0x50 },  //AWB B gain, default 0x40
};

static const struct regval_list gc2145_wb_office_regs[] =
{
    {0xfe, 0x00 },  //select page 0
    {0xb3, 0x50 },  //AWB R gain, default 0x40
    {0xb4, 0x40 },  //AWB G gain, default 0x40
    {0xb5, 0xa8 },  //AWB B gain, default 0x40
};

static const struct regval_list gc2145_wb_cloudy_regs[] =
{
    {0xfe, 0x00 },
    {0xb3, 0x58 },  //AWB R gain, default 0x40
    {0xb4, 0x40 },  //AWB G gain, default 0x40
    {0xb5, 0x50 },  //AWB B gain, default 0x40
};

static const struct regval_list gc2145_wb_home_regs[] =
{
    {0xfe, 0x00 },
    {0xb3, 0xa0 },  //AWB R gain, default 0x40
    {0xb4, 0x45 },  //AWB G gain, default 0x40
    {0xb5, 0x40 },  //AWB B gain, default 0x40
};

static int gc2145_read(struct device *dev, uint8_t reg, uint8_t *value)
{
    const struct gc2145_data *data = DEV_DATA(dev);
    sccb_start(data->sccb);
    sccb_sendbyte(data->sccb, GC2145_ADDR);
    sccb_sendbyte(data->sccb, reg);
    sccb_stop(data->sccb);

    sccb_start(data->sccb);
    sccb_sendbyte(data->sccb, GC2145_ADDR | 0x01);
    *value = sccb_readbyte(data->sccb);
    sccb_nack(data->sccb);
    sccb_stop(data->sccb);

    return 0;
}

static int gc2145_write(struct device *dev, uint8_t reg, uint8_t value)
{
    const struct gc2145_data *data = DEV_DATA(dev);
    int res = 0;

    sccb_start(data->sccb);

    if (sccb_sendbyte(data->sccb, GC2145_ADDR))
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
static int gc2145_write_array(struct device *dev, const struct regval_list *regs, int array_size)
{
    int i = 0;
    int ret = 0;

    if (!regs)
    {
        return -ENOTSUP;
    }

    while (i < array_size)
    {
        ret = gc2145_write(dev, regs->reg, regs->val);
        if (ret != 0)
        {
            return -ENOTSUP;
        }

        i++;
        regs++;
    }

    return 0;
}

static int gc2145_set_brightness(struct device *dev, int value)
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
    ret = gc2145_write(dev, 0xfe, 0x02);
    ret = gc2145_write(dev, 0xd3, bright_value);

    if (ret < 0)
    {
        return ret;
    }

    return 0;
}

static int gc2145_set_contrast(struct device *dev, int value)
{
    int ret = -1;

    ret = gc2145_write(dev, 0xfe, 0x02);
    if (value >= -4 && value <= 4)
    {
        ret = gc2145_write(dev, 0xd4, ((8 + value) << 4));
    }

    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int gc2145_set_saturation(struct device *dev, int value)
{
    int ret = -1;

    ret = gc2145_write(dev, 0xfe, 0x02);
    if (value >= -4 && value <= 4)
    {
        ret = gc2145_write(dev, 0xd0, (value + 4) << 4);
    }

    if (ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int gc2145_set_wb(struct device *dev, enum light_mode value)
{
    int ret;
    int reg_value;

    switch (value)
    {
        case WB_AUTO:
            gc2145_write(dev, 0xfe, 0x00);
            ret = gc2145_read(dev, 0x82, &reg_value);
            reg_value = reg_value | 0x02;   //enable awb
            gc2145_write(dev, 0x82, reg_value);
            break;
        case WB_SUNNY:
            gc2145_write(dev, 0xfe, 0x00);
            ret = gc2145_read(dev, 0x82, &reg_value);
            reg_value = reg_value & 0xfd;   //disable awb
            gc2145_write(dev, 0x82, reg_value);
            ret = gc2145_write_array(dev, gc2145_wb_sunny_regs, ARRAY_SIZE(gc2145_wb_sunny_regs));
            break;
        case WB_OFFICE:
            gc2145_write(dev, 0xfe, 0x00);
            ret = gc2145_read(dev, 0x82, &reg_value);
            reg_value = reg_value & 0xfd;   //disable awb
            gc2145_write(dev, 0x82, reg_value);
            ret = gc2145_write_array(dev, gc2145_wb_office_regs, ARRAY_SIZE(gc2145_wb_office_regs));
            break;
        case WB_CLOUDY:
            gc2145_write(dev, 0xfe, 0x00);
            ret = gc2145_read(dev, 0x82, &reg_value);
            reg_value = reg_value & 0xfd;   //disable awb
            gc2145_write(dev, 0x82, reg_value);
            ret = gc2145_write_array(dev, gc2145_wb_cloudy_regs, ARRAY_SIZE(gc2145_wb_cloudy_regs));
            break;
        case WB_HOME:
            gc2145_write(dev, 0xfe, 0x00);
            ret = gc2145_read(dev, 0x82, &reg_value);
            reg_value = reg_value & 0xfd;   //disable awb
            gc2145_write(dev, 0x82, reg_value);
            ret = gc2145_write_array(dev, gc2145_wb_home_regs, ARRAY_SIZE(gc2145_wb_home_regs));
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
static int gc2145_restore_mode(struct device *dev)
{
    return gc2145_write_array(dev, gc2145_init_default_setting, ARRAY_SIZE(gc2145_init_default_setting));
}

static int gc2145_init_controls(struct device *dev)
{
    gc2145_set_wb(dev, WB_AUTO);
    gc2145_set_saturation(dev, 0);
    gc2145_set_brightness(dev, 0);
    gc2145_set_contrast(dev, 0);

    return 0;
}


static void gc2145_reset_power(struct device *dev)
{
    const struct gc2145_data *data = DEV_DATA(dev);
    if (data->rst_port)
    {
        gpio_pin_write(data->pwdn_port, data->pwdn_pin, 1);
        gpio_pin_write(data->pwdn_port, data->pwdn_pin, 0);

        k_sleep(5);
        gpio_pin_write(data->rst_port, data->rst_pin, 0);
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
static inline int gc2145_set_power(struct device *dev, bool on)
{
    int ret = 0;
    uint8_t id_high, id_low;
    uint16_t id;

    if (on)
    {
        gc2145_reset_power(dev);
        k_sleep(10);

        /* Read sensor Model ID */
        gc2145_read(dev, 0xf0, &id_high);
        gc2145_read(dev, 0xf1, &id_low);

        id = (id_high << 8) | id_low;
        if (id != GC2145_ID)
        {
            printk("read camera id fail \n");
            ret = -ENOTSUP;
        }

        ret = gc2145_restore_mode(dev);

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
static inline int gc2145_set_fmt(struct device *dev, enum camera_output_format format, enum camera_mode_id id, enum camera_frame_rate frame_rate)
{
    struct gc2145_data *data = DEV_DATA(dev);
    uint8_t reg;
    uint16_t w, h;
    int ret = 0;

    ret = gc2145_read(dev, OUTPUT_FMT, &reg);

    switch (id)
    {
        case MODE_QVGA_320_240:
            if (frame_rate == FPS_15)
            {
                ret = gc2145_write_array(dev, gc2145_setting_15fps_QVGA_320_240, ARRAY_SIZE(gc2145_setting_15fps_QVGA_320_240));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 320;
                    data->pic_height = 240;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = gc2145_write_array(dev, gc2145_setting_30fps_QVGA_320_240, ARRAY_SIZE(gc2145_setting_30fps_QVGA_320_240));
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
                ret = gc2145_write_array(dev, gc2145_setting_15fps_VGA_640_480, ARRAY_SIZE(gc2145_setting_15fps_VGA_640_480));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 640;
                    data->pic_height = 480;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = gc2145_write_array(dev, gc2145_setting_30fps_VGA_640_480, ARRAY_SIZE(gc2145_setting_30fps_VGA_640_480));
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
                ret = gc2145_write_array(dev, gc2145_setting_15fps_XGA_1024_768, ARRAY_SIZE(gc2145_setting_15fps_XGA_1024_768));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 1024;
                    data->pic_height = 768;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = gc2145_write_array(dev, gc2145_setting_30fps_XGA_1024_768, ARRAY_SIZE(gc2145_setting_30fps_XGA_1024_768));
                if (ret == 0)
                {
                    data->framerate = 30;
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
                ret = gc2145_write_array(dev, gc2145_setting_15fps_720P_1280_720, ARRAY_SIZE(gc2145_setting_15fps_720P_1280_720));
                if (ret == 0)
                {
                    data->framerate = 15;
                    data->pic_width = 1280;
                    data->pic_height = 720;
                }
            }
            else if (frame_rate == FPS_30)
            {
                ret = gc2145_write_array(dev, gc2145_setting_30fps_720P_1280_720, ARRAY_SIZE(gc2145_setting_30fps_720P_1280_720));
                if (ret == 0)
                {
                    data->framerate = 30;
                    data->pic_width = 1280;
                    data->pic_height = 720;
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
            ret = gc2145_write(dev, 0xfe, 0x00);
            reg = OUTPUT_SET_FMT(reg, OUTPUT_FMT_RGB565);
            ret = gc2145_write(dev, OUTPUT_FMT, reg);
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_RGB565;
            }
            break;
        case VIDEO_FMT_YUV422:
            ret = gc2145_write(dev, 0xfe, 0x00);
            reg = OUTPUT_SET_FMT(reg, OUTPUT_FMT_YUV0);
            ret = gc2145_write(dev, OUTPUT_FMT, reg);
            if (ret == 0)
            {
                data->pixfmt = VIDEO_FMT_YUV422;
            }
            break;
        case VIDEO_FMT_RAW:
            ret = gc2145_write(dev, 0xfe, 0x00);
            reg = OUTPUT_SET_FMT(reg, OUTPUT_FMT_BAYER);
            ret = gc2145_write(dev, OUTPUT_FMT, reg);
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
static int gc2145_set_framesize(struct device *dev, uint16_t xsize, uint16_t ysize)
{
    struct gc2145_data *data = DEV_DATA(dev);
    uint16_t xstart, ystart;
    xstart = (1600 - xsize) >> 1;
    ystart = (1200 - ysize) >> 1;
    data->pic_width = xsize;
    data->pic_height = ysize;

    gc2145_write(dev, 0xfe, 0x00);
    gc2145_write(dev, 0x90, 0x01);
    gc2145_write(dev, 0x91, (uint8_t)(ystart >> 8));
    gc2145_write(dev, 0x92, (uint8_t)(ystart & 0xff));
    gc2145_write(dev, 0x93, (uint8_t)(xstart >> 8));
    gc2145_write(dev, 0x94, (uint8_t)(xstart & 0xff));
    gc2145_write(dev, 0x95, (uint8_t)(ysize >> 8));
    gc2145_write(dev, 0x96, (uint8_t)(ysize & 0xff));
    gc2145_write(dev, 0x97, (uint8_t)(xsize >> 8));
    gc2145_write(dev, 0x98, (uint8_t)(xsize & 0xff));

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
static inline int gc2145_get_fmt(struct device *dev, struct video_format *fmt)
{
    struct gc2145_data *data = DEV_DATA(dev);

    fmt->width = data->pic_width;
    fmt->height = data->pic_height;
    fmt->framerate = data->framerate;
    fmt->pixfmt = data->pixfmt;

    return 0;
}

static int gc2145_set_exposure(struct device *dev, int exp_level)
{
    int ret;
    uint8_t reg;

    ret = gc2145_write(dev, 0xfe, 0x01);
    switch (exp_level)
    {
        case 0:
            ret = gc2145_write(dev, 0x13, 0x60);        //default value is 0x50
        case 1:
            ret = gc2145_write(dev, 0x13, 0x70);
        case 2:
            ret = gc2145_write(dev, 0x13, 0x85);
        case 3:
            ret = gc2145_write(dev, 0x13, 0x95);
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
static inline int gc2145_s_ctrl(struct device *dev, uint32_t cmd, void *arg)
{
    struct gc2145_data *data = DEV_DATA(dev);
    struct video_format *fmt;
    int ret;
    int value;

    switch (cmd)
    {
        case CAMERA_CID_BRIGHTNESS:
            value = *(int *)arg;
            ret = gc2145_set_brightness(dev, value);
            break;
        case CAMERA_CID_CONTRAST:
            value = *(int *)arg;
            ret = gc2145_set_contrast(dev, value);
            break;
        case CAMERA_CID_SATURATION:
            value = *(int *)arg;
            ret = gc2145_set_saturation(dev, value);
            break;
        case CAMERA_CID_SHARPNESS:
            break;
        case CAMERA_CID_FLASH_MODE:
            break;
        case CAMERA_CID_FOCUS_AUTO:
            break;
        case CAMERA_CID_EXPOSURE:
            value = *(int *)arg;
            ret = gc2145_set_exposure(dev, value);
            break;
        case CAMERA_CID_WHITE_BALANCE:
            value = *(enum light_mode *)arg;
            ret = gc2145_set_wb(dev, value);
            break;
        case CAMERA_CID_FRAME_SIZE:
            fmt = (struct video_format *)arg;
            ret = gc2145_set_framesize(dev, fmt->width, fmt->height);
            break;
        default:
            break;
    }

    return 0;
}

static const struct camera_driver_api gc2145_drv_api_funcs =
{
    .set_power = gc2145_set_power,
    .set_fmt = gc2145_set_fmt,
    .get_fmt = gc2145_get_fmt,
    .ioctl = gc2145_s_ctrl,
};

static int gc2145_init(struct device *dev)
{
    struct gc2145_data *data = DEV_DATA(dev);

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

#ifdef CONFIG_GC2145_0
static struct gc2145_data gc2145_dev_data_0 =
{
    .sccb_name = CONFIG_CAMERA_0_SCCB_NAME,
    .pic_width = 320,
    .pic_height = 240,
    .framerate = 15,
    .pixfmt = VIDEO_FMT_YUV422,
    .rst_port_name = CAMERA_0_GPIO_RST_PORT,
    .rst_pin = CAMERA_0_GPIO_RST_PIN,
    .pwdn_port_name = CAMERA_0_GPIO_PWDN_PORT,
    .pwdn_pin = CAMERA_0_GPIO_PWDN_PIN,
};

DEVICE_AND_API_INIT(gc2145_0, GC2145_0_DEV_NAME,
                    gc2145_init, &gc2145_dev_data_0, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &gc2145_drv_api_funcs);
#endif

#ifdef CONFIG_GC2145_1
static struct gc2145_data gc2145_dev_data_1 =
{
    .sccb_name = CONFIG_CAMERA_1_SCCB_NAME,
    .pic_width = 320,
    .pic_height = 240,
    .framerate = 15,
    .pixfmt = VIDEO_FMT_YUV422,
    .rst_port_name = CAMERA_1_GPIO_RST_PORT,
    .rst_pin = CAMERA_1_GPIO_RST_PIN,
    .pwdn_port_name = CAMERA_1_GPIO_PWDN_PORT,
    .pwdn_pin = CAMERA_1_GPIO_PWDN_PIN,
};

DEVICE_AND_API_INIT(gc2145_1, GC2145_1_DEV_NAME,
                    gc2145_init, &gc2145_dev_data_1, NULL,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &gc2145_drv_api_funcs);
#endif

