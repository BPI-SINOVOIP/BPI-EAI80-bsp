#include <display/fb_display.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "ai_common.h"
#include "processor.h"
#include "math.h"
#include "lcd.h"
#include "draw.h"
#include <board.h>
#include <gm_hal_vin.h>
#include <gpio.h>
#include "gm_hal_kdp.h"
#include <stdlib.h>
#include <gm_hal_tim.h>
#include <pinmux.h>
#include <imlib.h>
#include "app_processor.h"
#include "post_yolo.h"

extern int aimodule_init_device(void);
extern float *g_yolo_out_buffer[2];
extern int16_t g_route_buffer;

#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)
#define OPS_BIN_SIZE        (5*SIZE_1M)
#define OPS_BIN_SIZE_OPTIMIZE (4*SIZE_1M)

#define  TINY_YOLOV2    1
#define  TINY_YOLOV3    2

#define AI_FLASH_START_OFFSET  (0x00050000)

#define OPS_BIN_FLASH_START (AI_FLASH_START_OFFSET)
#define IMAGE_BIN_FLASH_START (OPS_BIN_FLASH_START+OPS_BIN_SIZE)
#define INPUT_WIDTH    224
#define INPUT_HEIGHT   224
#define IMAGE_BUFFER_SIZE   (INPUT_WIDTH * INPUT_HEIGHT * 3) + 2 /*input_fpga.out.bin, 0x24C02*/

#define BUFFERSIZE 200704                 /*INPUT_WIDTH * INPUT_HEIGHT * 4*/
#define BLACK         0x000000
#define BLUE          0x0000FF

#define CAM_PIC_SIZE  (INPUT_HEIGHT * INPUT_HEIGHT * 3)

#define MAX_GESTURES_CNT 2


extern struct device *ltdc;
extern uint32_t show_buf_addr;

#define _GENERIC_SECTION_ZI(segment) __attribute__((section(STRINGIFY(segment)),zero_init))
__aligned(32) unsigned char RecvBuffer[IMAGE_BUFFER_SIZE] _GENERIC_SECTION_ZI(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned short inputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned short OutputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned char OPS_WEI_BASE[OPS_BIN_SIZE_OPTIMIZE]_GENERIC_SECTION_ZI(mem_ai_data);  /*more than 4 MB are needed for YOLO*/


//ObjDetData obj_det_data;
#define MAX_GESTURES_CNT 2

static char *gesture_name[MAX_GESTURES_CNT] =
{
    "fist",
    "palm"
};

static uint32_t object_colordrawn[MAX_GESTURES_CNT] =
{
    0xFF0000,
    0x00FF00,

};


int device_init(void)
{
    int i = 0;

    aimodule_init_device();
    unsigned char *pic_buf;
    pic_buf  = (float *)malloc_ext(0, 224 * 224 * 4);
    rrggbb_split2rgb((uint8_t *)RecvBuffer, (uint8_t *)pic_buf, 224, 224);
    lcd_config(pic_buf);
    ai_init_storage();
    memset(inputImage, 0x00, INPUT_WIDTH * INPUT_HEIGHT * 4);
    memset(OutputImage, 0x00, INPUT_WIDTH * INPUT_HEIGHT * 4);
    free_ext(0, pic_buf);

    return 0;
}

void predict(struct easynet_dev *dev, unsigned char *ops_data, unsigned short *pInputImage, unsigned short *pOutputImage)
{
    int ret = 0;
    unsigned short *poutbuf = NULL;

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    /* Process all ops */
    do
    {
        ret = easynet_ops_process(dev);

        if (ret == 0x06 || ret == 0x01)
        {
            if (ret == 0x06)
            {
                poutbuf =  dev->predict_out;
                dev = easynet_process_update(ops_data, pInputImage, pOutputImage);
                dev->predict_out = poutbuf;
            }
            break;
        }
    } while (1);

    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
}

void display(detection *dets, int classes, int nboxes, float thresh, uint32_t *obj_colordrawn, unsigned char *buf)
{
    int k = 0;
    int i = 0;
    int count = 0;
    float box_x_min, box_y_min, box_x_max, box_y_max;
    for (k = 0; k < classes; ++k)
    {
        for (i = 0; i < nboxes; i ++)
        {
            OBJ_DBG_PRINTF("class: %d\n\r", k);
            if (dets[i].prob[k] < thresh)
            {
                continue;
            }
            else
            {
                OBJ_DBG_PRINTF("%lf %lf %lf %lf,%d,%lf;\n", dets[i].bbox.x, dets[i].bbox.y, dets[i].bbox.w, dets[i].bbox.h, k, dets[i].prob[k]);
                count++;
            }

            box_y_min = (dets[i].bbox.y - dets[i].bbox.h / 2) * 224;
            box_x_min = (dets[i].bbox.x - dets[i].bbox.w / 2) * 224;
            box_y_max = (dets[i].bbox.y + dets[i].bbox.h / 2) * 224;
            box_x_max = (dets[i].bbox.x + dets[i].bbox.w / 2) * 224;

            if (box_x_min < 0)
            {
                box_x_min = 0;
            }
            if (box_y_min < 0)
            {
                box_y_min = 0;
            }
            if (box_x_max > 223)
            {
                box_x_max = 223;
            }

            if (box_y_max > 223)
            {
                box_y_max = 223;
            }

            /*This will draw rectangle on Layer 1*/
            LCD_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, obj_colordrawn[k]);
        }
    }

    OBJ_DBG_PRINTF("%d Real Box found\n", count);
    /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
    fb_ioctl(ltdc, 0, FB_X_SETADDR, buf); //show_buf_addr
}

void gesture_detect(void)
{
    uint32_t i = 0;
    uint32_t t_last, t_now, t_total;
    struct easynet_dev *dev;
    uint32_t demoaddr = 0;

    detection *dets;
    layer_post layer[2];
    RET_CODE ret = RET_OK;
    unsigned char *pic_buf;

    float biases_anchors7x7[10] = {0.9779, 1.5104, 1.3569, 2.2835, 1.7299, 2.7640, 2.1525, 3.6051, 2.8186, 4.7123};
    int nboxes;
    float thresh = 0.3;
    float nms_thresh = 0.45;
    float *tmp_buf = NULL;

    for (i = 0; i < TINY_YOLOV2; i++)
    {
        if (i == 0)
        {
            layer[i].mask =  NULL;
            layer[i].biases = &biases_anchors7x7[0];
            layer[i].w = 7;
            layer[i].h = 7;
        }
        else
        {
            layer[i].mask =  NULL;
            layer[i].biases = NULL;
            layer[i].w = 14;
            layer[i].h = 14;
        }

        layer[i].softmax = 1;

        layer[i].net_h = 224;
        layer[i].net_w = 224;
        layer[i].out_ch = 5;
        layer[i].n = 5;

        layer[i].classes = 2;
        layer[i].coords = 4;

        layer[i].post_type = REGION;

        layer[i].out_cnt = layer[i].w * layer[i].h * layer[i].out_ch * (layer[i].classes + layer[i].coords + 1);
        layer[i].output = (float *)malloc_ext(0, layer[i].out_cnt * sizeof(float));

    }

    tmp_buf = (float *)malloc_ext(0, layer[0].out_cnt * sizeof(float));

    pic_buf  = (float *)malloc_ext(0, 224 * 224 * 4);

    dev = easynet_process_init(OPS_WEI_BASE, &inputImage[0], &OutputImage[0]);

    while (1)
    {
        dev->input =  RecvBuffer;
        predict(dev, OPS_WEI_BASE, &inputImage[0], &OutputImage[0]);

        dets = easynet_post_region(layer, &nboxes, thresh, nms_thresh, dev->predict_out, tmp_buf);

        rrggbb_split2rgb((uint8_t *)RecvBuffer, (uint8_t *)pic_buf, 224, 224);
        fb_ioctl(ltdc, 0, FB_X_SETADDR, &pic_buf);
        k_sleep(500);
        display(dets, layer[0].classes, nboxes,  thresh, &object_colordrawn[0], &show_buf_addr);
        OBJ_DBG_PRINTF("Done!\n");
    }

    easynet_post_region_free(dets, nboxes);
    free_ext(0, tmp_buf);
    free_ext(0, pic_buf);
}

int main(void)
{
    RET_CODE ret = RET_OK;

    OBJ_DBG_PRINTF("gesture detect demo begin\n");
    device_init();
    ret = ai_load_ops_data(OPS_BIN_FLASH_START, OPS_BIN_SIZE_OPTIMIZE, IMAGE_BIN_FLASH_START, IMAGE_BUFFER_SIZE,  OPS_WEI_BASE, RecvBuffer);
    gesture_detect();
    OBJ_DBG_PRINTF("gesture detect demo exit\n");

    return ret;
}
