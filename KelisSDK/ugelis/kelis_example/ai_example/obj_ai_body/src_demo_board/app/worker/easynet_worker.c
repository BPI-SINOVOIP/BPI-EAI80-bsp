/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          easynet.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Upper layer to call easynet.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

//#include <easynet/easynet.h>

#include "easynet_worker.h"
//#include "post_worker.h"
#include "worker.h"

#include <camera/dcmi.h>
#include <display/fb_display.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "ai_common.h"
#include "processor.h"
#include "math.h"
#include "lcd.h"
#include "camera.h"
#include "draw.h"
#include <board.h>
#include <gm_hal_vin.h>
#include <gpio.h>
#include "gm_hal_kdp.h"
#include <stdlib.h>
#include <pinmux.h>
#include <imlib.h>
#include <gm_hal_clock.h>
#include "app_processor.h"
#include <gm_hal_reset.h>
#include "dev/cmd.h"
#include "dev/dev.h"
#include "inc/config.h"
#include "inc/common.h"
#include "worker/worker.h"

#define INPUT_WIDTH    224
#define INPUT_HEIGHT   224
#define IMAGE_BUFFER_SIZE   (INPUT_WIDTH * INPUT_HEIGHT * 3) + 2 /*input_fpga.out.bin, 0x24C02*/
#define BUFFERSIZE          (INPUT_WIDTH * INPUT_HEIGHT * 4)

extern uint32_t show_buf_addr;

#define _GENERIC_SECTION_ZI(segment) __attribute__((section(STRINGIFY(segment)),zero_init))
__aligned(32) unsigned char RecvBuffer[IMAGE_BUFFER_SIZE] _GENERIC_SECTION_ZI(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned short ginputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned short goutputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned char WEI_BASE[OPS_BIN_SIZE]_GENERIC_SECTION_ZI(mem_ai_data);

ObjDetData gobj_det_data;

K_THREAD_STACK_DEFINE(easynet_stack_area, 4 * 1024);

static int max_body_index;
static int max_body_width;
static int filter_dist = 10;
static int max_body_x;
static int max_body_dist;

static void predict_process(worker_t *self, easynet_worker_t *easynet);

#define MAX_OBJECTCLASS_CNT 20
#define MAX_GESTURES_CNT 2
static char *gesture_name[MAX_GESTURES_CNT] =
{
    "fist",
    "palm"
};

static uint32_t object_colordrawn[MAX_OBJECTCLASS_CNT] =
{
    0xFF0000,
    0x00FF00,
    0xFF00FF,
    0xFF0040,
    0x00FF40,
    0xFF0040,
    0xFF0080,
    0x00FF80,
    0xFF0080,
    0xFF00C0,
    0x00FFC0,
    0xFF00C0,
    0xFF00FF,
    0x00FFFF,
    0xFF00FF,
    0xFF40FF,
    0x40FFFF,
    0xFF40FF,
    0xFF80FF,
    0x80FFFF,
};

static void easynet_detect_thread(void *data, void *param, void *p3)
{
    printf("easynet_detect_thread\n");

    worker_t *self = (worker_t *)data;
    predict_process(self, self->data);
}
static void predict(easynet_worker_t *peasynet, unsigned char *ops, unsigned short *inputbuf, unsigned short *outputbuf)
{
    int ret = 0;

    struct easynet_dev *dev = peasynet->dev;

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    /* Process all ops */
    do
    {
        ret = easynet_ops_process(dev);

        if (ret == 0x06 || ret == 0x01)
        {
            if (ret == 0x06)
            {
                dev = easynet_process_update(ops, inputbuf, outputbuf);
            }
            break;
        }
    } while (1);

    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
}

void ai_body_detect(easynet_worker_t *easynet, ObjDetData *pobj_det, unsigned char *ops, unsigned short *inputbuf, unsigned short *outputbuf)
{
    int out_width, out_height, box_channel, confidence_channel, box_num, class_channel ;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;
    uint32_t kk = 0;
    uint32_t t_last, t_now;
    float box_x_min, box_y_min, box_x_max, box_y_max;
    float *d_box = NULL;
    float *box_class_probs = NULL;
    Box_t *s_box = NULL;
    RET_CODE ret = RET_OK;

    struct easynet_dev *dev = easynet->dev;

    /*debug use*/
    pobj_det->show_demo_pic = 0;

    out_width = 7;
    out_height = 7;
    box_channel = 4;
    confidence_channel = 1;
    box_num = 5;
    class_channel = 1;

    d_box = kmalloc(out_width * out_height * box_num * box_channel * sizeof(float));
    box_class_probs = kmalloc(out_height * out_width * box_num * class_channel * sizeof(float));
    s_box = kmalloc(20 * sizeof(Box_t));

    if (pobj_det->show_demo_pic)
    {
        /*for CNN computation*/
        dev->input = RecvBuffer;
        predict(easynet, ops, inputbuf, outputbuf);
    }
    else
    {
        predict(easynet, ops, inputbuf, outputbuf);
    }

    ret = post_yolo(pobj_det, outputbuf, d_box, box_class_probs, s_box);
    t_now = k_uptime_get_32();
    if (ret != RET_OK)
    {
        printf("post_yolo failed\n");
    }

    max_body_index = -1;
    max_body_width = 0;
    max_body_x  = -1;
    max_body_dist = 0;

    for (i = 0; i < pobj_det->boxcnt; i ++)
    {
        OBJ_DBG_PRINTF("class: %d\n\r", pobj_det->box[i].class);
        OBJ_DBG_PRINTF("score: %lf\n\r", pobj_det->box[i].score);
        box_y_min = pobj_det->box[i].box[0] < 0 ? 0 : pobj_det->box[i].box[0];
        box_x_min = pobj_det->box[i].box[1] < 0 ? 0 : pobj_det->box[i].box[1];
        box_y_max = pobj_det->box[i].box[2] < 0 ? 0 : pobj_det->box[i].box[2];
        box_x_max = pobj_det->box[i].box[3] < 0 ? 0 : pobj_det->box[i].box[3];

        OBJ_DBG_PRINTF("box_y_min: %lf\n\r", box_y_min);
        OBJ_DBG_PRINTF("box_x_min: %lf\n\r", box_x_min);
        OBJ_DBG_PRINTF("box_y_max: %lf\n\r", box_y_max);
        OBJ_DBG_PRINTF("box_x_max: %lf\n\r", box_x_max);

        if (box_x_max > 223)
        {
            box_x_max = 223;
        }

        if (box_y_max > 223)
        {
            box_y_max = 223;
        }

        if (max_body_index == -1)
        {
            max_body_index = i;
            max_body_width = box_x_max - box_x_min;
            max_body_x = box_x_min;
        }
        else
        {
            if ((box_x_max - box_x_min) > max_body_width)
            {
                max_body_index = i;
                max_body_width = box_x_max - box_x_min;
                max_body_x = box_x_min;
            }
        }

        ui_set_addr(show_buf_addr);

        /*This will draw rectangle on Layer 1*/
        ui_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, object_colordrawn[pobj_det->box[i].class]);
    }
    /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/

    lcd_set_frame_layercamera(show_buf_addr);
    HAL_VIN_Enable((VIN_Device_T *)0x40025000);
    OBJ_DBG_PRINTF("Done!\n");


    kfree(s_box);
    kfree(d_box);
    kfree(box_class_probs);
}

static void predict_process(worker_t *self, easynet_worker_t *easynet)
{
    while (1)
    {
        int ret = 0;
        printf("==== \n");
        ret = k_sem_take(&easynet->predict_lock, 5000);
        printf("**** \n");
        if (ret == 0)
        {
            easynet_worker_t *easynet = (easynet_worker_t *)self->data;
            easynet->ops = &WEI_BASE[0];
            ai_body_detect(easynet, &gobj_det_data, (unsigned char *)easynet->ops, &ginputImage[0], &goutputImage[0]);

            /* network is finished, to do post work */
            //do_next_process(self, CMD_POST_FACEFRAME, vector);
        }
        if (easynet->exit)
        {
            break;
        }
    }
}

struct easynet_dev *ai_bodydetect_init(ObjDetData *pobj_det, unsigned char *ops, unsigned short *inputbuf, unsigned short *outputbuf)
{
    int i = 0;
    struct easynet_dev *dev;
    memset(inputbuf, 0x00, INPUT_WIDTH * INPUT_HEIGHT * 4 * sizeof(unsigned short));
    memset(outputbuf, 0x00, INPUT_WIDTH * INPUT_HEIGHT * 4 * sizeof(unsigned short));

    pobj_det->boxcnt = 0;
    pobj_det->show_demo_pic = 0;
    pobj_det->det_type =  DET_BODY;
    pobj_det->det_type_cpy = DET_BODY ;

    dev = easynet_process_init(ops, inputbuf, outputbuf);

    /*Initial the SRAM specified for KDP310*/
    for (i = 0; i < 0x40000; i += 4)
    {
        *(volatile uint32_t *)(BRAM_OUTPUT_A_BASEARDDR + i) = 0x00000000;
    }

    printf("Init camera Done \n");

    return dev;

}

static int easynet_worker_init(worker_t *self, void *param)
{
    easynet_worker_t *easynet = kmalloc(sizeof(easynet_worker_t));
    easynet->ops = param;
    easynet->exit = 0;
    k_sem_init(&easynet->predict_lock, 0, UINT_MAX);
    easynet->dev = ai_bodydetect_init(&gobj_det_data, WEI_BASE, &ginputImage[0], &goutputImage[0]);
    self->data = easynet;

    k_thread_create(&easynet->task_thread, easynet_stack_area,
                    K_THREAD_STACK_SIZEOF(easynet_stack_area),
                    (k_thread_entry_t)easynet_detect_thread,
                    self, 0, 0,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
}


static int easynet_worker_run(worker_t *self, int cmd, void *param)
{
    easynet_worker_t *easynet = (easynet_worker_t *)self->data;
    if (cmd == CMD_EASYNET_DATAREADY)
    {
        easynet->dev->input = param;
        k_sem_give(&easynet->predict_lock);
    }
    else if (cmd == CMD_EASYNET_FRAMEREADY)
    {
        //do_next_process(self, CMD_POST_NORMALFRAME, 0);
    }
}

static int easynet_worker_deinit(worker_t *self)
{
    easynet_worker_t *easynet = (easynet_worker_t *)self->data;
    easynet->exit = 1;
    kfree(self->data);
}

worker_t easynet_worker =
{
    .run    = easynet_worker_run,
    .init   = easynet_worker_init,
    .deinit = easynet_worker_deinit,
};
