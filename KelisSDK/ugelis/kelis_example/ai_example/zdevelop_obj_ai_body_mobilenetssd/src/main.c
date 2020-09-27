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
#include <gm_hal_tim.h>
#include <pinmux.h>
#include <imlib.h>
#include "app_processor.h"
#include "post_yolo.h"
#include "debug.h"

#define YOLO_VERSION_3   3
#define YOLO_VERSION_2   2

#define OPTIMIZATION_LEVEL_1  1
#define LAYER_BUFFER_NUM    6
#define OPTIMIZATION_LEVEL_2  2

#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)
#define OPS_BIN_SIZE        (14*SIZE_1M)
#define OPS_BIN_SIZE_OPTIMIZE  OPS_BIN_SIZE

#define  TINY_YOLOV2    1
#define  TINY_YOLOV3    2

#define AI_FLASH_START_OFFSET  (0x00050000)

#define OPS_BIN_FLASH_START (AI_FLASH_START_OFFSET)
#define IMAGE_BIN_FLASH_START (OPS_BIN_FLASH_START+OPS_BIN_SIZE)
#define INPUT_WIDTH    300
#define INPUT_HEIGHT   300
#define IMAGE_BUFFER_SIZE   (INPUT_WIDTH * INPUT_HEIGHT * 3) + 2

#define BUFFERSIZE    (0x15F900*2) //   (0x62000*2)   //           /*INPUT_WIDTH * INPUT_HEIGHT * 4*/    112*112*32 
#define BLACK         0x000000
#define BLUE          0x0000FF
#define STACKSIZE                4096
#define CAM_PIC_SIZE  (INPUT_HEIGHT * INPUT_HEIGHT * 3)
#define MAX_OBJECTCLASS_CNT 20
#define MAX_GESTURES_CNT 2
#define MAX_OBJECTCLASS_CNT 20
#define MAX_GESTURES_CNT 2

extern int aimodule_init_device(void);

void camera_isr(struct device *dev, uint32_t line_num);

K_THREAD_STACK_DEFINE(stack_area, STACKSIZE);
static struct k_thread task_thread;

extern struct device *dcmi;
extern struct device *ltdc;
extern uint32_t show_buf_addr;

#define _GENERIC_SECTION_ZI(segment) __attribute__((section(STRINGIFY(segment)),zero_init))
__aligned(32) unsigned char RecvBuffer[IMAGE_BUFFER_SIZE] _GENERIC_SECTION_ZI(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned char OPS_WEI_BASE[OPS_BIN_SIZE_OPTIMIZE]_GENERIC_SECTION_ZI(mem_ai_data);  /*more than 4 MB are needed for YOLO*/

__aligned(32) unsigned short LayerBuffer[2][BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);

uint8_t lcd_inited = 0;
ObjDetData obj_det_data;
int g_app_exit = 1;

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

void init_ai_data(void)
{
    obj_det_data.ex_buf.cam_buf_detected = malloc_ext(0, INPUT_HEIGHT * INPUT_WIDTH * 3);
    obj_det_data.ex_buf.cam_buf_validate = malloc_ext(0, IMAGE_BUFFER_SIZE);

    printf("cam_buf_detected : 0x%08x cam_buf_validate : 0x%08x \n", obj_det_data.ex_buf.cam_buf_detected, obj_det_data.ex_buf.cam_buf_validate);

    obj_det_data.ex_buf.cam_buf_validate[IMAGE_BUFFER_SIZE - 1] = 0; /*Specified for CNN, Last*/
    obj_det_data.ex_buf.cam_buf_validate[IMAGE_BUFFER_SIZE - 2] = 0; /*Specified for CNN, Index*/

    obj_det_data.ex_buf.cam_buf_tmp = malloc_ext(0, INPUT_HEIGHT * INPUT_WIDTH * 4); /*A, R, G ,B, it's layer 1, blending on layer 2*/

    printf("cam_buf_tmp : 0x%08x \n", obj_det_data.ex_buf.cam_buf_tmp);

    memset((void *)obj_det_data.ex_buf.cam_buf_tmp, 0x00, INPUT_HEIGHT * INPUT_WIDTH * 4);

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

void deinit_ai_data(void)
{
    free_ext(0, obj_det_data.ex_buf.cam_buf_detected);
    free_ext(0, obj_det_data.ex_buf.cam_buf_validate);
    free_ext(0, obj_det_data.ex_buf.cam_buf_tmp);
}

int device_init(void)
{
    int i = 0;
#if 1
    /*enable a stable CPU clock*/
    *(volatile uint32_t *)(0x40000090) |= 2 << 0;

    /*power on kdp310 and reset*/
    *(volatile uint32_t *)(0x1FFFF0B0) &= ~(1 << 18);
    *(volatile uint32_t *)(0x40000070) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;
    *(volatile uint32_t *)(0x40000080) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;

    /*improve system powerformance*/
    *(volatile uint32_t *)(0x40039288) |= 0x3F << 16;
    *(volatile uint32_t *)(0x4000018C) |= 3;

    *(volatile uint32_t *)(0x4003928C) &= ~0xF0;

    *(volatile uint32_t *)(0x4000009c) |= 1 << 16;
#else
    aimodule_init_device();
    *(volatile uint32_t *)(0x4003928C) &= ~0xF0;
#endif

    memset(&LayerBuffer[0][0], 0x00, BUFFERSIZE * sizeof(unsigned short));
    memset(&LayerBuffer[0][1], 0x00, BUFFERSIZE * sizeof(unsigned short));

    /*Initial the SRAM specified for KDP310*/
    for (i = 0; i < 0x40000; i += 4)
    {
        *(volatile uint32_t *)(0x20020000 + i) = 0x00000000;
    }

    init_ai_data();
    init_shell(&obj_det_data);
    //camera_config(camera_isr);
    printf("Init camera Done \n");
    ai_init_storage();

    return 0;
}

/*Called every frame data captured by Camera*/
void camera_isr(struct device *dev, uint32_t line_num)
{
    struct dcmi_buf dis_buf;
    int ret = 0;
    image_t src_img;

    int mean;
    int mean_dev;

    src_img.bpp = IMAGE_BPP_GRAYSCALE;
    src_img.w = 224;
    src_img.h = 224;

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
            demoaddr = (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0xC0000000;
            rrggbb_split2rgb((uint8_t *)RecvBuffer, (uint8_t *)demoaddr, 224, 224);
            fb_ioctl(ltdc, 0, FB_X_SETADDR, &demoaddr);
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

void display(detection *dets, int classes, int nboxes, float thresh, uint32_t *obj_colordrawn, unsigned char *buf)
{
    int k = 0;
    int i = 0;
    int count = 0;
    float box_x_min, box_y_min, box_x_max, box_y_max;

    if (dets == NULL || nboxes == 0)
    {
        return;
    }

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
                printf("%lf %lf %lf %lf,%d,%lf;\n", dets[i].bbox.x, dets[i].bbox.y, dets[i].bbox.w, dets[i].bbox.h, k, dets[i].prob[k]);
                count++;
            }

            box_y_min = (dets[i].bbox.y) * 224;
            box_x_min = (dets[i].bbox.x) * 224;
            box_y_max = (dets[i].bbox.h) * 224;
            box_x_max = (dets[i].bbox.w) * 224;

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
            //LCD_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, obj_colordrawn[k]);
        }
    }

    printf("%d Real Box found\n", count);
    /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
    //fb_ioctl(ltdc, 0, FB_X_SETADDR, buf); //show_buf_addr
}

void body_detect(void)
{
    uint32_t i = 0;
    uint32_t t_last, t_now, t_total;
    struct easynet_dev *dev;

    detection *dets;
    layer_post layer[2];
    RET_CODE ret = RET_OK;

    float biases_anchors7x7[10] = {0.24, 0.40, 0.30, 0.67, 0.55, 0.51, 0.69, 0.76, 0.94, 0.92};
    int nboxes;
    float thresh = 0.3;
    float nms_thresh = 0.3;
    float *tmp_buf = NULL;
    int mask_7x7[] = {0, 1, 2, 3, 4};
    extern uint32_t t_Hw, t_BankGeneral, t_CrossPut, t_dwcon ;

    for (i = 0; i < 2; i++)
    {
        if (i == 0)
        {
            layer[i].mask =  &mask_7x7[0];
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

        layer[i].yolo_version = YOLO_VERSION_2;
        layer[i].softmax = 1;

        layer[i].net_h = 224;
        layer[i].net_w = 224;
        layer[i].out_ch = 5;
        layer[i].n = 5;

        layer[i].classes = 1;
        layer[i].coords = 4;

        layer[i].post_type = YOLO;

        layer[i].out_cnt = layer[i].w * layer[i].h * layer[i].out_ch * (layer[i].classes + layer[i].coords + 1);
        layer[i].output = (float *)malloc_ext(0, layer[i].out_cnt * sizeof(float));

    }

    tmp_buf = (float *)malloc_ext(0, layer[0].out_cnt * sizeof(float));

    struct easynet_dev_cfg dev_cfg;
    dev_cfg.bn_epsilon = 0.0; /* Used default epsilon */
    dev_cfg.optimization = OPTIMIZATION_LEVEL_2;
    dev_cfg.layer_buffer_num = LAYER_BUFFER_NUM;

    dev_cfg.layer_buffer[0] = &LayerBuffer[0][0];
    dev_cfg.layer_buffer[1] = &LayerBuffer[1][0];

    dev_cfg.middle_layer_buffer[0] = NULL;
    dev_cfg.middle_layer_buffer[1] = NULL;
    dev_cfg.middle_layer_buffer[2] = NULL;
    dev_cfg.middle_layer_buffer[3] = NULL;
    dev_cfg.middle_layer_buffer[4] = NULL;
    dev_cfg.middle_layer_buffer[5] = NULL;
    dev_cfg.middle_layer_buffer[6] = NULL;
#if 0
    dev_cfg.layer_buffer[2] = NULL;
    dev_cfg.layer_buffer[3] = NULL;
    dev_cfg.layer_buffer[4] = NULL;
    dev_cfg.layer_buffer[5] = NULL;
#else
    //dev_cfg.layer_buffer[4] = &LayerBuffer4[4][0];

#endif

    dev = easynet_process_init(&dev_cfg, OPS_WEI_BASE);
#if 0
    k_thread_create(&task_thread, stack_area,
                    K_THREAD_STACK_SIZEOF(stack_area),
                    (k_thread_entry_t)cambuf_process_thread,  NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
#endif

    while (g_app_exit == 1)
    {
        //k_sem_take(&obj_det_data.validate_lock, K_FOREVER);

        extern int cntt;

        obj_det_data.show_demo_pic = 1;
        if (obj_det_data.show_demo_pic)
        {
            t_Hw = 0;
            t_BankGeneral = 0;
            t_CrossPut = 0;
            t_dwcon = 0;
            cntt = 0;
            memset(0x20020000, 0x00, 0x40000);
            memset(&LayerBuffer[0][0], 0x00, BUFFERSIZE * sizeof(unsigned short));
            memset(&LayerBuffer[1][0], 0x00, BUFFERSIZE * sizeof(unsigned short));

            t_last = k_uptime_get_32();
            dev->input = RecvBuffer;
            predict(dev, OPS_WEI_BASE);
        }
        else
        {
            dev->input = obj_det_data.ex_buf.cam_buf_validate;
            t_last = k_uptime_get_32();
            predict(dev, OPS_WEI_BASE);
        }

        uint32_t t_last_post = k_uptime_get_32();

        /*memcpy((unsigned char*)dev->predict_out, region, sizeof(region));*/

        dets = easynet_post_yolo(layer, &nboxes, thresh, nms_thresh, dev->predict_out, tmp_buf);

        t_now = k_uptime_get_32();

        printf("t_Hw =%d, t_BankGeneral = %d, t_CrossPut = %d, t_dwcon = %d\n", t_Hw, t_BankGeneral, t_CrossPut, t_dwcon);
        printf("all(t_now- t_last) = %d ms, post_yolo_easy=%d ms \n", t_now - t_last, t_now - t_last_post);
        display(dets, layer[0].classes, nboxes,  thresh, &object_colordrawn[0], &show_buf_addr);

        //HAL_VIN_Enable((VIN_Device_T *)0x40025000);
        easynet_post_yolo_free(dets, nboxes);
        OBJ_DBG_PRINTF("Done!\n");
    }

    free_ext(0, tmp_buf);
    deinit_ai_data();
}

int main(void)
{
    RET_CODE ret = RET_OK;

    printf("body detect demo begin\n");
    device_init();
    ret = ai_load_ops_data(OPS_BIN_FLASH_START, OPS_BIN_SIZE_OPTIMIZE, IMAGE_BIN_FLASH_START, IMAGE_BUFFER_SIZE,  OPS_WEI_BASE, RecvBuffer);
    body_detect();
    OBJ_DBG_PRINTF("body detect demo exit\n");

    return ret;
}
