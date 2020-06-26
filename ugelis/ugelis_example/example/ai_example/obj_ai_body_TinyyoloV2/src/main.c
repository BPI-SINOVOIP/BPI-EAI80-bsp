#include <camera/dcmi.h>
#include <display/fb_display.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "ai_common.h"
#include "processor.h"
#include "lcd.h"
#include "camera.h"
#include "draw.h"
#include <board.h>
#include <gm_hal_vin.h>
#include <gpio.h>
#include "gm_hal_kdp.h"
#include <stdlib.h>
#include <pinmux.h>
#include "app_processor.h"
#include "post_yolo.h"


extern int aimodule_init_device(void);

#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)
#define OPS_BIN_SIZE        (4*SIZE_1M)

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
#define STACKSIZE                4096
#define CAM_PIC_SIZE  (INPUT_HEIGHT * INPUT_HEIGHT * 3)
#define MAX_OBJECTCLASS_CNT 20

#define VOL_PORT     "GPIOB"
#define VOL_GPIO_PIN 12

void camera_isr(struct device *dev, uint32_t line_num);


K_THREAD_STACK_DEFINE(stack_area, STACKSIZE);
static struct k_thread task_thread;

extern struct device *dcmi;
extern struct device *ltdc;
extern uint32_t show_buf_addr;

#ifdef KEIL_DEBUG

#define _GENERIC_SECTION_ZI(segment) __attribute__((section(STRINGIFY(segment)),zero_init))
__aligned(32) unsigned char RecvBuffer[IMAGE_BUFFER_SIZE] _GENERIC_SECTION_ZI(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned short inputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned short OutputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned char OPS_WEI_BASE[OPS_BIN_SIZE]_GENERIC_SECTION_ZI(mem_ai_data);  /*more than 4 MB are needed for YOLO*/
#else
__aligned(32) unsigned char RecvBuffer[IMAGE_BUFFER_SIZE] __in_section_unique(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned short inputImage[BUFFERSIZE]    __in_section_unique(mem_ai_data);
__aligned(32) unsigned short OutputImage[BUFFERSIZE]   __in_section_unique(mem_ai_data);
__aligned(32) unsigned char OPS_WEI_BASE[OPS_BIN_SIZE]  __in_section_unique(mem_ai_data);  /*more than 4 MB are needed for YOLO*/	

#endif

uint8_t lcd_inited = 0;
ObjDetData obj_det_data;
#define MAX_OBJECTCLASS_CNT 20

static char *object_name[MAX_OBJECTCLASS_CNT] =
{
    "aeroplane",
    "bicyle",
    "bird",
    "boat",
    "bottle",
    "bus",
    "car",
    "cat",
    "chair",
    "cow",
    "diningtable",
    "dog",
    "horse",
    "motorbike",
    "person",
    "pottedplant",
    "sheep",
    "sofa",
    "train",
    "tvmonitor"
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

void init_ai_data(void)
{
#ifdef ILED_AUTO_ONOFF
    ctrl_mgr.gray_pic = malloc_ext(0, 224 * 224);
#endif

    obj_det_data.ex_buf.cam_buf_detected = malloc_ext(0, 224 * 224 * 3);
    obj_det_data.ex_buf.cam_buf_validate = malloc_ext(0, IMAGE_BUFFER_SIZE);

    //printf("cam_buf_detected : 0x%08x cam_buf_validate : 0x%08x \n", obj_det_data.ex_buf.cam_buf_detected, obj_det_data.ex_buf.cam_buf_validate);

    obj_det_data.ex_buf.cam_buf_validate[IMAGE_BUFFER_SIZE - 1] = 0; /*Specified for CNN, Last*/
    obj_det_data.ex_buf.cam_buf_validate[IMAGE_BUFFER_SIZE - 2] = 0; /*Specified for CNN, Index*/

    obj_det_data.boxcnt = 0;
    obj_det_data.object_name = object_name;
    obj_det_data.stop_disp = 0;                 /*Stop display on LCD*/
    obj_det_data.threshold = 0.6;
    obj_det_data.one_frame_detect_started = 0;  /*Start detect one frame*/

    obj_det_data.show_demo_pic = 0;
    obj_det_data.show_demopic_onetime = 0;
    obj_det_data.cap_show = 1;

    obj_det_data.object_name = object_name;
    obj_det_data.pic_saved = 0;

    k_sem_init(&obj_det_data.validate_lock, 0, 1); //UINT_MAX
}

int device_init(void)
{
    int i = 0;
#if 0
    /*enable a stable CPU clock*/
    *(volatile uint32_t *)(0x40000090) |= 2 << 0;

    /*power on kdp310 and reset*/
    *(volatile uint32_t *)(0x1FFFF0B0) &= ~(1 << 18);
    *(volatile uint32_t *)(0x40000070) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;
    *(volatile uint32_t *)(0x40000080) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;

    /*improve system powerformance*/
    *(volatile uint32_t *)(0x40039288) |= 0x3F << 16;
    *(volatile uint32_t *)(0x4000018C) |= 3;
    *(volatile uint32_t *)(0x4000009c) |= 1 << 16;
#else
    aimodule_init_device();
#endif

    memset(inputImage, 0x00, BUFFERSIZE);
    memset(OutputImage, 0x00, BUFFERSIZE);

    /*Initial the SRAM specified for KDP310*/
    for (i = 0; i < 0x40000; i += 4)
    {
        *(volatile uint32_t *)(0x20020000 + i) = 0x00000000;
    }

    init_ai_data();
    camera_config(camera_isr);
    //printf("Init camera Done \n");
    ai_init_storage();

    return 0;
}

/*Called every frame data captured by Camera*/
void camera_isr(struct device *dev, uint32_t line_num)
{
    struct dcmi_buf dis_buf;
    int ret = 0;

#ifdef ILED_AUTO_ONOFF
    image_t src_img;

    int mean;
    int mean_dev;

    src_img.bpp = IMAGE_BPP_GRAYSCALE;
    src_img.w = 224;
    src_img.h = 224;
#endif 

    if (!obj_det_data.stop_disp)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);

        /*Camera can display Real-Time, we initialize LCD to fix  snowflake sreen issue when display the first image frame*/
        if (!lcd_inited)
        {
            lcd_config(dis_buf.addr);
            lcd_inited = 1;
        }
        /*Set the address for LCD display, Layer 0*/
        if (!obj_det_data.show_demo_pic)
        {
            if (ret == 0)
            {
                if (!obj_det_data.one_frame_detect_started)
                {
                    /*Captured Picture data*/
                    memcpy(obj_det_data.ex_buf.cam_buf_detected, (void *)dis_buf.addr, CAM_PIC_SIZE);
                    obj_det_data.one_frame_detect_started = 1;
                }
            }

            show_buf_addr = dis_buf.addr;

        }
        /*Release the buffer for next camera capturing*/
        dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);
    }
}

static void cambuf_process_thread()
{
    uint32_t demoaddr = 0;

    while (1)
    {
        if (obj_det_data.show_demo_pic)
        {
            demoaddr = (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0xC0000000;  //0x10400000
            rrggbb_split2rgb((uint8_t *)RecvBuffer, (uint8_t *)demoaddr, 224, 224);
            //fb_ioctl(ltdc, 0, FB_X_SETADDR, &demoaddr);
             show_buf_addr = demoaddr;
            obj_det_data.show_demopic_onetime = 0;
            k_sem_give(&obj_det_data.validate_lock);

        }
        else
        {
            if (obj_det_data.one_frame_detect_started)
            {
                /*cam_buf_validate now has the KDP 310 format*/
                rgb_split2rrggbb(obj_det_data.ex_buf.cam_buf_detected, obj_det_data.ex_buf.cam_buf_validate, 224, 224);
                k_sem_give(&obj_det_data.validate_lock);
                obj_det_data.one_frame_detect_started = 0;
            }
        }

        k_yield();
    }
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

			OBJ_DBG_PRINTF("%lf %lf %lf %lf,%d,%lf;\n", dets[i].bbox.x, dets[i].bbox.y, dets[i].bbox.w, dets[i].bbox.h, k, dets[i].prob[k]);

            /*This will draw rectangle on Layer 1*/
            LCD_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, obj_colordrawn[k]);
        }
    }

    OBJ_DBG_PRINTF("%d Real Box found\n", count);
    /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
    fb_ioctl(ltdc, 0, FB_X_SETADDR, buf); //show_buf_addr
}


void body_detect(void)
{
    uint32_t i = 0;
    uint32_t t_last, t_now, t_total;
    struct easynet_dev *dev;

    detection *dets;
    layer_post layer[2];
    RET_CODE ret = RET_OK;

    float biases_anchors7x7[10] = {0.2901, 0.6292, 0.7770, 1.7555, 1.4236, 3.5353, 2.6815, 4.8071, 5.1381, 5.6096};
    int nboxes;
    float thresh = 0.5;
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

        layer[i].classes = 1;
        layer[i].coords = 4;

        layer[i].post_type = REGION;

        layer[i].out_cnt = layer[i].w * layer[i].h * layer[i].out_ch * (layer[i].classes + layer[i].coords + 1);
        layer[i].output = (float *)malloc_ext(0, layer[i].out_cnt * sizeof(float));

    }

    tmp_buf = (float *)malloc_ext(0, layer[0].out_cnt * sizeof(float));

    dev = easynet_process_init(OPS_WEI_BASE, &inputImage[0], &OutputImage[0]);

    k_thread_create(&task_thread, stack_area,
                    K_THREAD_STACK_SIZEOF(stack_area),
                    (k_thread_entry_t)cambuf_process_thread,  NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);

    while (1)
    {
        k_sem_take(&obj_det_data.validate_lock, K_FOREVER);

        t_last = k_uptime_get_32();

        obj_det_data.show_demo_pic = 0;
        if (obj_det_data.show_demo_pic)
        {
            dev->input = RecvBuffer;
            predict(dev, OPS_WEI_BASE, &inputImage[0], &OutputImage[0]);
        }
        else
        {
            dev->input = obj_det_data.ex_buf.cam_buf_validate;
            predict(dev, OPS_WEI_BASE, &inputImage[0], &OutputImage[0]);
        }

        uint32_t last = k_uptime_get_32();

        dets = easynet_post_region(layer, &nboxes, thresh, nms_thresh, dev->predict_out, tmp_buf);

        t_now = k_uptime_get_32();

        //printf("t_now- t_last = %d ms, post_yolo_easy=%d ms \n", t_now - t_last, t_now - last);
        display(dets, layer[0].classes, nboxes,  thresh, &object_colordrawn[0], &show_buf_addr);

        HAL_VIN_Enable((VIN_Device_T *)0x40025000);

        OBJ_DBG_PRINTF("Done!\n");
    }

    easynet_post_region_free(dets, nboxes);
    free_ext(0, tmp_buf);
}

int main(void)
{
    RET_CODE ret = RET_OK;

    OBJ_DBG_PRINTF("body detect demo begin\n");
    device_init();
    ret = ai_load_ops_data(OPS_BIN_FLASH_START, OPS_BIN_SIZE, IMAGE_BIN_FLASH_START, IMAGE_BUFFER_SIZE,  OPS_WEI_BASE, RecvBuffer);
    body_detect();
    OBJ_DBG_PRINTF("body detect demo exit\n");

    return ret;
}
