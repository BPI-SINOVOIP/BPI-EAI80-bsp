/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>

#include <imlib.h>
#include <display/fb_display.h>
#include <camera/dcmi.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>
#include <gm_hal_vout.h>
#include <gm_hal_imp.h>
#include <gm_irq.h>

#include <rpc_core.h>
#include <rpc_plugin_imp.h>


#include "img_data.h"


#define SAMPLE_DEBUG 1

#ifdef SAMPLE_DEBUG
    #define sample_debug       printk
#else
    #define sample_debug(...)  do{}while(0);
#endif



#define LCD_GPIO_PORT    "GPIOB"
#define LCD_GPIO_PIN     13   /*PB13*/


static struct device *ltdc;
struct fb_displayinfo info;

static uint8_t pic_in[320 * 240 * 3];
static uint8_t pic_out[320 * 240 * 3];

static int32_t time_old = 0;
static int32_t time_new = 0;
static int32_t time_diff = 0;

static int32_t time_1 = 0;
static int32_t time_2 = 0;






int main(void)
{
    RPC_Package_T pack;
    IMP_Data_T imp_data;
    RET_CODE ret = RET_OK;
    uint32_t image_width;
    uint32_t image_height;
    uint32_t addr = 0;

    *(volatile uint32_t *)0x40000090 |= (0x2 << 0);


    /* prepare buffer */
    memset(pic_out, 0, sizeof(pic_out));
    memcpy(pic_in, img_gray_lena, sizeof(img_gray_lena));
    image_width = DATA_WIDTH;
    image_height = DATA_HEIGHT;

    while (1)
    {
        //k_sleep(1000);
        //memset(pic_out, 0, sizeof(pic_out));/* clear pic_out to check result */

        time_old = k_uptime_get_32();

#if 0
        /* prepare private data */
        imp_data.impCmd = IMP_CMD_SOBEL;
        imp_data.pSrc = pic_in;
        imp_data.pDst = pic_out;
        imp_data.width = image_width;
        imp_data.height = image_height / 4;
        /* prepare rpc pack */
        pack.cmd = RPC_CMD_IMP;
        pack.privData = &imp_data;
        ret = RPC_MasterSendPack(&pack);
        if (ret != RET_OK)
        {
            sample_debug("!!! RPC_MasterSendPack wrong \n");
            continue;
        }

        time_1 = k_uptime_get_32();
        imlib_sobel(pic_in + (image_height / 4)*image_width, image_height / 4, image_width, pic_out + (image_height / 4)*image_width);
        time_2 = k_uptime_get_32();

        //if(RPC_MasterWaitResp(&pack, TIMEOUT_WAIT_FOREVER) != RET_OK)
        if ((RPC_MasterWaitResp(&pack, 2000) != RET_OK) || (pack.result != RET_OK))
        {
            sample_debug("!!! sobel_slave wrong \n");
            continue;
        }
#else
        imlib_sobel(pic_in, image_height, image_width, pic_out);
#endif

        time_new = k_uptime_get_32();
        time_diff = time_new - time_old;
        sample_debug("s:%d\n", time_diff);
        //sample_debug("m:%d\n", time_2-time_1);

        addr = (uint32_t)pic_out;
        //fb_ioctl(ltdc, 0, FB_X_SETADDR, &(addr));
    }

    return 0;
}

