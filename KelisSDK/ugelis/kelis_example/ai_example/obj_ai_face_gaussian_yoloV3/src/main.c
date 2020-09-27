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
#include "gm_hal_kdp.h"
#include <stdlib.h>
#include <pinmux.h>
#include "app_processor.h"
#include "post_yolo.h"

#ifdef IMG_RECT //defined in ai_common.h
    #include <gm_hal_imp.h>
    #include "image_rect.h"
#endif

extern int aimodule_init_device(void);

#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)
#define OPS_BIN_SIZE   0x365f00//(4*SIZE_1M)

#define OPTIMIZATION_LEVEL_1  1
#define LAYER_BUFFER_NUM    4
#define OPTIMIZATION_LEVEL_2  2

#define AI_FLASH_START_OFFSET  (0x00150000)//there is only 260kb for system.bin when use 0x00050000 

#define OPS_BIN_FLASH_START (AI_FLASH_START_OFFSET)
#define IMAGE_BIN_FLASH_START (OPS_BIN_FLASH_START+ (4*SIZE_1M))
#define INPUT_WIDTH    288
#define INPUT_HEIGHT   288
#define IMAGE_BUFFER_SIZE   (INPUT_WIDTH * INPUT_HEIGHT * 3) + 2 /*input_fpga.out.bin, 0x24C02*/

#define LAYER_BUFFER_SIZE 331776                 /*INPUT_WIDTH * INPUT_HEIGHT * 4*/
#define MIN_LAYER_BUFFER_SIZE 18*18*128

#define BLACK         0x000000
#define BLUE          0x0000FF
#define STACKSIZE                4096
#define CAM_PIC_SIZE  (INPUT_HEIGHT * INPUT_HEIGHT * 3)
#define MAX_OBJECTCLASS_CNT 20

void camera_isr(struct device *dev, uint32_t line_num);

K_THREAD_STACK_DEFINE(stack_area, STACKSIZE);
static struct k_thread task_thread;

extern struct device *dcmi;
extern struct device *ltdc;
extern uint32_t show_buf_addr;

#define _GENERIC_SECTION_ZI(segment) __attribute__((section(STRINGIFY(segment)),zero_init))
//__aligned(32) unsigned char RecvBuffer[IMAGE_BUFFER_SIZE] _GENERIC_SECTION_ZI(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned short ConvBuffer[LAYER_BUFFER_SIZE * 2 + MIN_LAYER_BUFFER_SIZE * 2] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned char OPS_WEI_BASE[OPS_BIN_SIZE]_GENERIC_SECTION_ZI(mem_ai_data);  /*more than 4 MB are needed for YOLO*/

#ifdef IMG_RECT
    __aligned(32) unsigned char ProcessImg[288 * 288 * 3 * 2]_GENERIC_SECTION_ZI(mem_ai_data); /*0x79800 is needed*/
    __aligned(32) unsigned char ProcessImgSplit[288 * 288 * 3]_GENERIC_SECTION_ZI(mem_ai_data); /*0x3cc00 is needed*/
    #define LCD_WIDTH   480
    #define LCD_HEIGHT  272
    static struct fb_scalerinfo scaler;
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
    obj_det_data.ex_buf.cam_buf_detected = NULL;//malloc_ext(0, INPUT_WIDTH * INPUT_HEIGHT * 3);
#ifdef IMG_RECT
    obj_det_data.ex_buf.cam_buf_validate = ProcessImgSplit;
#else
    obj_det_data.ex_buf.cam_buf_validate = (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0xC0000000; //malloc_ext(0, IMAGE_BUFFER_SIZE);
#endif
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
                    //memcpy(obj_det_data.ex_buf.cam_buf_detected, (void *)dis_buf.addr, CAM_PIC_SIZE);
#ifdef IMG_RECT
                    scaler.src = dis_buf.addr;
                    scaler.dst = ProcessImg;
                    scaler.src_w = 480;
                    scaler.src_h = 288;
                    scaler.dst_w = 288;
                    scaler.dst_h = 288;
                    scaler.src_fmt = IMP_INPUT_RGB888;
                    scaler.dst_fmt = IMP_OUTPUT_RGB888;

                    fb_scaler(ltdc, &scaler);

                    obj_det_data.ex_buf.cam_buf_detected = ProcessImg;
#else
                    obj_det_data.ex_buf.cam_buf_detected = dis_buf.addr;
#endif
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
            //rrggbb_split2rgb((uint8_t *)RecvBuffer, (uint8_t *)demoaddr, INPUT_WIDTH, INPUT_HEIGHT);
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
                rgb_split2rrggbb(obj_det_data.ex_buf.cam_buf_detected, obj_det_data.ex_buf.cam_buf_validate, INPUT_WIDTH, INPUT_HEIGHT);
                k_sem_give(&obj_det_data.validate_lock);
                obj_det_data.one_frame_detect_started = 0;
            }
        }

        k_yield();
    }
}

void predict(struct easynet_dev *dev, unsigned char *ops_data)
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
                dev = easynet_process_update(dev->cfg, ops_data);
                dev->predict_out = poutbuf;
            }
            break;
        }
    } while (1);

    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
}

#ifdef IMG_RECT
void display(detection *dets, int classes, int nboxes, float thresh, uint32_t *obj_colordrawn, unsigned char *buf)
{
    int k = 0;
    int i = 0;
    int count = 0;
    int w, h, s;
    float box_x_min, box_y_min, box_x_max, box_y_max;
    float box_x_min_draw, box_y_min_draw, box_x_max_draw, box_y_max_draw;
    rect_t face_rt;
    img_rect_t img_rt;
    s = 0;
    for (k = 0; k < classes; ++k)
    {
        for (i = 0; i < nboxes; i ++)
            //for (i = 0; i < 1; i ++)
        {
            OBJ_DBG_PRINTF("class: %d\n\r", k);
            if (dets[i].prob[k] < thresh)
            {
                continue;
            }

            box_y_min = (dets[i].bbox.y - dets[i].bbox.h / 2) * LCD_HEIGHT;
            box_x_min = (dets[i].bbox.x - dets[i].bbox.w / 2) * LCD_WIDTH;
            box_y_max = (dets[i].bbox.y + dets[i].bbox.h / 2) * LCD_HEIGHT;
            box_x_max = (dets[i].bbox.x + dets[i].bbox.w / 2) * LCD_WIDTH;

            if (box_x_min < 0)
            {
                box_x_min = 0;
            }
            if (box_y_min < 0)
            {
                box_y_min = 0;
            }
            if (box_x_max > 479)
            {
                box_x_max = 479;
            }

            if (box_y_min > 271/*287*/)
            {
                box_y_min = 270;//287;
            }

            if (box_y_max > 271/*287*/)
            {
                box_y_max = 271;//287;
            }

            //printf("%lf %lf %lf %lf,%d,%lf;\n", box_x_min, box_y_min, box_x_max, box_y_max, k, dets[i].prob[k]);
            //printf("w:%f h:%f\n",box_x_max - box_x_min, box_y_max - box_y_min);
            /*This will draw rectangle on Layer 1*/
            w = box_x_max - box_x_min;
            h = box_y_max - box_y_min;
            if (w * h > s)
            {
                s = w * h;
                box_x_max_draw = box_x_max;
                box_x_min_draw = box_x_min;
                box_y_max_draw = box_y_max;
                box_y_min_draw = box_y_min;
            }
            count++;
        }
    }
    face_rt.x_min = box_x_min_draw;
    face_rt.x_max = box_x_max_draw;
    face_rt.y_min = box_y_min_draw;
    face_rt.y_max = box_y_max_draw;
    img_rt.x_center = (box_x_min_draw + box_x_max_draw) / 2 - 96;
    img_rt.y_center = (box_y_min_draw + box_y_max_draw) / 2 - 130;
    img_rt.w = 192;
    img_rt.h = 200;
    //LCD_ui_clear();
    if ((face_rt.x_min + face_rt.x_max + face_rt.y_min + face_rt.y_max) != 0)
    {
        ui_draw_rectpic(&img_rt, &face_rt, gImage_image_rect);
    }
    //LCD_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, obj_colordrawn[k]);
    OBJ_DBG_PRINTF("%d Real Box found\n", count);
    /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
    fb_ioctl(ltdc, 0, FB_X_SETADDR, buf); //show_buf_addr
}
#else
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

            box_y_min = (dets[i].bbox.y - dets[i].bbox.h / 2) * INPUT_HEIGHT;
            box_x_min = (dets[i].bbox.x - dets[i].bbox.w / 2) * INPUT_WIDTH;
            box_y_max = (dets[i].bbox.y + dets[i].bbox.h / 2) * INPUT_HEIGHT;
            box_x_max = (dets[i].bbox.x + dets[i].bbox.w / 2) * INPUT_WIDTH;

            if (box_x_min < 0)
            {
                box_x_min = 0;
            }
            if (box_y_min < 0)
            {
                box_y_min = 0;
            }
            if (box_x_max > 287)
            {
                box_x_max = 287;
            }

            if (box_y_min > 271/*287*/)
            {
                box_y_min = 270;//287;
            }

            if (box_y_max > 271/*287*/)
            {
                box_y_max = 271;//287;
            }

            //printf("%lf %lf %lf %lf,%d,%lf;\n", box_x_min, box_y_min, box_x_max, box_y_max, k, dets[i].prob[k]);

            /*This will draw rectangle on Layer 1*/
            LCD_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, obj_colordrawn[k]);

            count++;
        }
    }

    OBJ_DBG_PRINTF("%d Real Box found\n", count);
    /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
    fb_ioctl(ltdc, 0, FB_X_SETADDR, buf); //show_buf_addr
}
#endif
void face_detect(void)
{
    int out_width, out_height, box_channel, confidence_channel, box_num, class_channel ;
    uint32_t i = 0;
    uint32_t t_last, t_now, t_total;

    struct easynet_dev *dev;
    struct easynet_dev_cfg dev_cfg;
    network net ;
    detection *dets;
    RET_CODE ret = RET_OK;

    int mask_9x9[4] = {4, 5, 6, 7};
    int mask_18x18[4] = {1, 2, 3, 4};
    float biases_anchors[16] = {7, 12,  10, 19,  16, 27,  23, 47,  34, 33,  44, 72,  77, 112, 129, 169}; // face detect

    int nboxes;
    float thresh = 0.5;
    float nms_thresh = 0.45;  /*float nms_thresh = 0.40*/

    dev_cfg.bn_epsilon = 0.0; /* Used default epsilon */
    dev_cfg.optimization = OPTIMIZATION_LEVEL_2;
    dev_cfg.layer_buffer_num = LAYER_BUFFER_NUM;
    dev_cfg.thresh = thresh;

    for (i = 0; i < LAYER_BUFFER_NUM; i++)
    {
        if (i < 2)
        {
            dev_cfg.layer_buffer[i] = ConvBuffer + i * LAYER_BUFFER_SIZE;
        }
        else
        {
            dev_cfg.layer_buffer[i] = ConvBuffer + 2 * LAYER_BUFFER_SIZE + (i - 2) * MIN_LAYER_BUFFER_SIZE;
        }
    }

    net.w = INPUT_WIDTH;
    net.h = INPUT_HEIGHT;
    net.n = 2;
    net.layers = malloc_ext(0, net.n * sizeof(layer_post));

    for (i = 0; i < net.n; i++)
    {
        if (i == 0)
        {
            net.layers[i].mask =  &mask_9x9[0];;
            net.layers[i].biases = &biases_anchors[0];
            net.layers[i].w = 9;
            net.layers[i].h = 9;
        }
        else
        {
            net.layers[i].mask =  &mask_18x18[0];;
            net.layers[i].biases = &biases_anchors[0];
            net.layers[i].w = 18;
            net.layers[i].h = 18;
        }

        net.layers[i].softmax = 0;
        net.layers[i].out_ch = 4;
        net.layers[i].classes = 1;
        net.layers[i].out_cnt = net.layers[i].w * net.layers[i].h * net.layers[i].out_ch * (net.layers[i].classes + 8 + 1);
        net.layers[i].post_type = GAUSSIAN_YOLO;
        net.layers[i].output = (float *)malloc_ext(0, net.layers[i].out_cnt * sizeof(float));

        dev_cfg.yolo_buffer[i] = net.layers[i].output;
    }

    dev = easynet_process_init(&dev_cfg, OPS_WEI_BASE);

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
            //nms_thresh = 0.40;
            /*for CNN computation*/
            dev->input = NULL;//RecvBuffer;
            predict(dev, OPS_WEI_BASE);
        }
        else
        {
            dev->input = obj_det_data.ex_buf.cam_buf_validate;
            predict(dev, OPS_WEI_BASE);
        }

        uint32_t last = k_uptime_get_32();

        dets = easynet_post_yolo(&net, &nboxes, thresh, nms_thresh, net.w, net.h);

        t_now = k_uptime_get_32();
        printf("t_now- t_last = %d ms, post_yolo_easy=%d ms \n", t_now - t_last, t_now - last);

        display(dets, net.layers[0].classes, nboxes,  thresh, &object_colordrawn[0], &show_buf_addr);
        easynet_post_yolo_free(dets, nboxes);
        HAL_VIN_Enable((VIN_Device_T *)0x40025000);

        OBJ_DBG_PRINTF("Done!\n");
    }
    //easynet_post_yolo_free(dets, nboxes);
}

int main(void)
{
    RET_CODE ret = RET_OK;

    OBJ_DBG_PRINTF("body detect demo begin\n");
    device_init();

    ret = ai_load_ops_data(OPS_BIN_FLASH_START, OPS_BIN_SIZE, IMAGE_BIN_FLASH_START, IMAGE_BUFFER_SIZE,  OPS_WEI_BASE, NULL/*RecvBuffer*/);
    face_detect();
    OBJ_DBG_PRINTF("body detect demo exit\n");
    return 0;
}
