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
#include <mem_cfg.h>
#include <gm_hal_vin.h>
#include <gpio.h>
#include "gm_hal_kdp.h"
#include <stdlib.h>
#include <gm_hal_tim.h>
#include <pinmux.h>
#include <imlib.h>
#include "app_processor.h"
#include "post_yolo.h"

//#define USE_UARTPROTOCOL
#ifdef USE_UARTPROTOCOL
    #include "protocol/protocol.h"
#endif

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
#define STACKSIZE                4096
#define CAM_PIC_SIZE  (INPUT_HEIGHT * INPUT_HEIGHT * 3)
#define MAX_OBJECTCLASS_CNT 20
#define MAX_GESTURES_CNT 2

#define VOL_PORT     "GPIOB"
#define VOL_GPIO_PIN 12

void camera_isr(struct device *dev, uint32_t line_num);

#define SHOW_POSITION
#ifdef SHOW_POSITION
    #include <obj_info.h>
    static int show_position_init_done = 0;
    static camera_position_param_t camera_position_param;
#endif

#define ILED_AUTO_ONOFF
#ifdef ILED_AUTO_ONOFF
    #define ILED_PORT     "GPIOC"
    #define ILED_GPIO_PIN 2

    #define MIN_PWM_DUTY  800
    #define MAX_PWM_DUTY 1100

    static struct Infrared_led_mgr ctrl_mgr;
    /* TIM handle declaration */
    TIM_Handle_T TimeHandle;
    struct device *gpioc;
    static uint16_t duty;
#endif


extern void protocol_start();
extern void protocol_send();
obj_show_t  objout;


//#define USE_MOTOR
//#define MOTOR_PRINTF_DEBUG
#ifdef MOTOR_PRINTF_DEBUG
    #define MOTOR_PRINTF  printf
#else
    #define MOTOR_PRINTF(...)
#endif

static int max_body_index;
static int max_body_width;
static int filter_dist = 10;
static int max_body_x;
static int max_body_dist;


#define USB_JPEG
#ifdef USB_JPEG
    #include "usb_virtual_camera.h"
    //  #include "jpeg.h"               //wait for midware jpeg function
    static int usb_jpeg_enable = 0;
#endif

#define WIFI_JPEG
#ifdef WIFI_JPEG
    #include "wifi.h"
    static int wifi_jpeg_enable = 0;
#endif

K_THREAD_STACK_DEFINE(stack_area, STACKSIZE);
static struct k_thread task_thread;

extern struct device *dcmi;
extern struct device *ltdc;
extern uint32_t show_buf_addr;

#define _GENERIC_SECTION_ZI(segment) __attribute__((section(STRINGIFY(segment)),zero_init))
__aligned(32) unsigned char RecvBuffer[IMAGE_BUFFER_SIZE] _GENERIC_SECTION_ZI(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned short inputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned short OutputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned char OPS_WEI_BASE[OPS_BIN_SIZE_OPTIMIZE]_GENERIC_SECTION_ZI(mem_ai_data);  /*more than 4 MB are needed for YOLO*/

uint8_t lcd_inited = 0;
ObjDetData obj_det_data;
#define MAX_OBJECTCLASS_CNT 20
#define MAX_GESTURES_CNT 2

/*usb buffer*/
struct fb_manager pfrm_mgr;
struct frm_buf last_buf;
struct requestbuffers reqbufs;
struct frm_buf buf;
extern uint8_t *g_UsbDeviceVideoMjpegData;

__aligned(32) unsigned char encode_jpeg[INPUT_WIDTH * INPUT_HEIGHT * 3] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned char camera_jpeg[0x78000] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned char usb_jpeg[INPUT_WIDTH * INPUT_HEIGHT * 3] _GENERIC_SECTION_ZI(mem_ai_data);

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

    printf("cam_buf_detected : 0x%08x cam_buf_validate : 0x%08x \n", obj_det_data.ex_buf.cam_buf_detected, obj_det_data.ex_buf.cam_buf_validate);

    obj_det_data.ex_buf.cam_buf_validate[IMAGE_BUFFER_SIZE - 1] = 0; /*Specified for CNN, Last*/
    obj_det_data.ex_buf.cam_buf_validate[IMAGE_BUFFER_SIZE - 2] = 0; /*Specified for CNN, Index*/

    obj_det_data.ex_buf.cam_buf_tmp = malloc_ext(0, 224 * 224 * 4); /*A, R, G ,B, it's layer 1, blending on layer 2*/

    printf("cam_buf_tmp : 0x%08x \n", obj_det_data.ex_buf.cam_buf_tmp);

    obj_det_data.ex_buf.camera_jpeg_buf = &camera_jpeg;
    obj_det_data.ex_buf.encode_jpeg_buf = &encode_jpeg;
    obj_det_data.ex_buf.usb_jpeg_buf = &usb_jpeg;

    memset((void *)obj_det_data.ex_buf.cam_buf_tmp, 0x00, 224 * 224 * 4);

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

    obj_det_data.cmd.usb = 0;                   /*Usb transfer flag*/
    if (usb_jpeg_enable)
    {
        obj_det_data.cmd.usb = 1;
    }

    obj_det_data.cmd.wifi = 0;                  /*Wifi transfer flag*/
    if (wifi_jpeg_enable)
    {
        obj_det_data.cmd.wifi = 1;
    }

    k_sem_init(&obj_det_data.validate_lock, 0, 1); //UINT_MAX
}

static int usb_init()
{
    USB_DeviceApplicationInit();
    pfrm_mgr.pic_width = INPUT_WIDTH;
    pfrm_mgr.pic_height = INPUT_HEIGHT;

    queue_init(&(pfrm_mgr.q));
    reqbufs.count = 3;
    reqbufs.mem_start = obj_det_data.ex_buf.usb_jpeg_buf;
    reqbufs.length = INPUT_WIDTH * INPUT_HEIGHT * 3;
    usb_create_bufs(&pfrm_mgr, &reqbufs);

    buf.index = 0;
    usb_query_buf(&pfrm_mgr, &buf);

    g_UsbDeviceVideoMjpegData = buf.addr;
    last_buf.index = 0;
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

    memset(inputImage, 0x00, 401408);
    memset(OutputImage, 0x00, 401408);

    /*Initial the SRAM specified for KDP310*/
    for (i = 0; i < 0x40000; i += 4)
    {
        *(volatile uint32_t *)(0x20020000 + i) = 0x00000000;
    }

    init_ai_data();
    init_shell(&obj_det_data);
    camera_config(camera_isr);
    printf("Init camera Done \n");
    ai_init_storage();
    usb_init();
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

#ifdef ILED_AUTO_ONOFF
                    fb_convert_color(ltdc, dis_buf.addr, ctrl_mgr.gray_pic, FMT_RGB888, FMT_L8, 224, 224);

                    src_img.pixels = (uint32_t)ctrl_mgr.gray_pic;

                    imlib_image_mean(&src_img, 128, &mean, &mean_dev);

                    //printk("mean: %d, mean_dev: %d \n", mean, mean_dev);

                    if (mean_dev < abs(mean))
                    {
                        if (mean > 0)
                        {
                            ctrl_mgr.dark_cnt = 0;
                            ctrl_mgr.normal_cnt++;
                            //printf("brightness is too big \n");
                        }
                        else if (mean < 0)
                        {
                            ctrl_mgr.dark_cnt++;
                            ctrl_mgr.normal_cnt = 0;
                            //printf("brightness is too little \n");
                        }
                        else
                        {
                            ctrl_mgr.dark_cnt = 0;
                            ctrl_mgr.normal_cnt++;
                        }
                    }
                    else
                    {
                        ctrl_mgr.dark_cnt = 0;
                        ctrl_mgr.normal_cnt++;
                    }
#endif
                }
            }

            show_buf_addr = dis_buf.addr;

        }
        /*Release the buffer for next camera capturing*/
        dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);
    }
}

void img_jpeg()
{
    uint32_t state;
    int ret = 0;
    k_sched_lock();
    camera_jpeg_config();
    k_sched_unlock();
    //k_sleep(500);     //set delay because the first jpeg is very dark
}

static void cambuf_process_thread()
{
    struct dcmi_buf dis_buf;
    uint32_t demoaddr = 0;
    uint32_t state;
    uint8_t frm_idx = 0xff;
    int ret;
    while (1)
    {
        if (obj_det_data.show_demo_pic)
        {
            demoaddr = (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0xC0000000;  //0x10400000
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

#ifdef  USE_MOTOR
extern void Motor_RoundCCW(void);
extern void Motor_RoundCW(void);
extern void Motor_RoundStop(void);
void Motor_Process(int dist)
{
    MOTOR_PRINTF("dist: %d ms\n", dist);
    MOTOR_PRINTF("filter_dist: %d ms\n", filter_dist);

    if ((dist < filter_dist) && (dist > -filter_dist))
    {
        motor_setop(3);

        return ;
    }

    if (dist < 0)
    {
        motor_setop(2);
    }
    else
    {
        motor_setop(1);
    }
}
#endif

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
    max_body_index = -1;
    max_body_width = 0;
    max_body_x  = -1;
    max_body_dist = 0;
    for (k = 0; k < classes; ++k)
    {
        objout.num = nboxes;
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

            objout.obj[i].off_l     = box_x_min;
            objout.obj[i].off_t     = box_y_min;
            objout.obj[i].width     = box_x_max - box_x_min;
            objout.obj[i].height    = box_y_max - box_y_min;
            if (max_body_index == -1)
            {
                max_body_index = i;
                max_body_width = objout.obj[i].width;
                max_body_x = box_x_min;
            }
            else
            {
                if (objout.obj[i].width > max_body_width)
                {
                    max_body_index = i;
                    max_body_width = objout.obj[i].width;
                    max_body_x = box_x_min;
                }
            }

            /*This will draw rectangle on Layer 1*/
            LCD_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, obj_colordrawn[k]);

#ifdef SHOW_POSITION
            if (i == 0)
            {
                //OBJ_DBG_PRINTF("===================================================\n");
            }
            //OBJ_DBG_PRINTF("(%4.0f, %4.0f, %4.0f, %4.0f)\t", box_x_min, box_y_min, box_x_max, box_y_max);
            float x_scale = 320.0 / 240.0;//224*224是宽被压扁了的图片，x方向要拉伸到标准比例320*240来进行三角计算，因为f是按320*240来标定的
            if (show_position_init_done == 0)
            {
                camera_position_param.height_to_ground = 250;
                camera_position_param.angle_to_ground = 0.7853981633974483;//45
                camera_position_param.focal_len = 117.2;
                camera_position_param.pic_width = 224 * x_scale;
                camera_position_param.pic_height = 224;
                obj_info_set_camera_param(camera_position_param);
            }
            uint32_t start_time = k_uptime_get_32();
            obj_info_t person_info = obj_info_get((len_t)(box_x_min + box_x_max) * x_scale / 2.0, (len_t)box_y_max, (len_t)(box_x_min + box_x_max) * x_scale / 2.0, (len_t)box_y_min);
            //person_info_t person_info = get_person_info((len_t)(box_x_min+box_x_max)/2.0, (len_t)box_y_max, (len_t)(box_x_min+box_x_max)/2.0, (len_t)box_y_min);
            uint32_t end_time = k_uptime_get_32();
            //OBJ_DBG_PRINTF("cost: %d ms\n", end_time - start_time);
            char tmpstr[128] = { 0 };
            uint8_t   dist = (uint8_t)(sqrt(fabs(person_info.x) * fabs(person_info.x) + person_info.z * person_info.z) * 10);
            uint8_t   angle = (uint8_t)person_info.angle;
            objout.obj[i].off_z     = dist;
            objout.obj[i].angle     = angle;
            //OBJ_DBG_PRINTF(tmpstr, "(L:%.1f-F:%.1f-H:%.1f)", fabs(person_info.x), person_info.z, person_info.h);
            //OBJ_DBG_PRINTF(tmpstr, "(Dist:%.1f-AG:%.1f)", sqrt(fabs(person_info.x)*fabs(person_info.x) + person_info.z * person_info.z), person_info.angle);
            //OBJ_DBG_PRINTF("%s\n", tmpstr);
#endif
        }
    }
#ifdef  USE_MOTOR
    if (max_body_index != -1)
    {
        int     dist;
        int     middle;

        middle = (224 - max_body_width) >> 1;
        dist   = max_body_x - middle;
        MOTOR_PRINTF("max_body_width: %d ms\n", max_body_width);
        MOTOR_PRINTF("max_body_x: %d ms\n", max_body_x);
        MOTOR_PRINTF("max_body_index: %d ms\n", max_body_index);
        Motor_Process(dist);
    }
#endif
#ifdef USE_UARTPROTOCOL
    /*=====================================*/
    protocol_send();
#endif
    OBJ_DBG_PRINTF("%d Real Box found\n", count);
    /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
    fb_ioctl(ltdc, 0, FB_X_SETADDR, buf); //show_buf_addr
}

#ifdef ILED_AUTO_ONOFF
static int tim8_pinmux_config(struct device *port)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;

    pin_cfg.pin = BIT(PINMUX_GPIO_PIN_5);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = GPIOC5_AF5_TIM8_CH1;
    ret = pinmux_pin_set(port, PINMUX_GPIO_PIN_5, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    return 0;
}

static RET_CODE TIM_PWMSetting(TIM_Device_T *TIMx)
{
    TIM_OutputCompareInit_T sOCConfig;
    memset(&sOCConfig, 0, sizeof(TIM_OutputCompareInit_T));

    TimeHandle.instance = TIMx;
    TimeHandle.init.prescaler = 4;
    TimeHandle.init.period = 1600;
    TimeHandle.init.clockDivision = 0;
    TimeHandle.init.counterMode = TIM_COUNTERMODE_UP;

    if (HAL_TIM_InitPWM(&TimeHandle) != HAL_OK)
    {
        return HAL_ERROR;
    }

    sOCConfig.ocMode = TIM_OCMODE_PWM1;
    sOCConfig.ocPolarity = TIM_OCPOLARITY_LOW;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = MIN_PWM_DUTY;

    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}
#endif

void body_detect(void)
{
    uint32_t i = 0;
    uint32_t t_last, t_now, t_total;
    struct easynet_dev *dev;

    detection *dets;
    layer_post layer[2];
    RET_CODE ret = RET_OK;

    uint32_t state;
    uint8_t frm_idx = 0xff;
    struct dcmi_buf dis_buf;
    image_t img;
    uint8_t quality = 90;

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

#ifdef ILED_AUTO_ONOFF
        if (ctrl_mgr.onoff == 0 && ctrl_mgr.dark_cnt > 50)
        {
            ctrl_mgr.onoff = 1;

            duty = MIN_PWM_DUTY;
            TIMx_SetPWMDuty(TIM8, TRUE, MIN_PWM_DUTY);
            HAL_TIM_StartPWM(&TimeHandle, TIM8, TIM_CHANNEL_1);

            gpio_pin_write(gpioc, ILED_GPIO_PIN, 1);
            printf("open the light \n");

            ctrl_mgr.dark_cnt = 0;
            ctrl_mgr.normal_cnt = 0;
        }

        if (ctrl_mgr.onoff == 1 && ctrl_mgr.dark_cnt > 50)
        {
            ctrl_mgr.onoff = 1;

            duty += 50;

            if (duty > MAX_PWM_DUTY)
            {
                duty = MAX_PWM_DUTY;
            }

            printf(" ste pwm duty: %d \n", duty);

            TIMx_SetPWMDuty(TIM8, TRUE, duty);

            ctrl_mgr.dark_cnt = 0;
            ctrl_mgr.normal_cnt = 0;
        }

        if (ctrl_mgr.onoff == 1 && ctrl_mgr.normal_cnt > 50)
        {
            ctrl_mgr.onoff = 0;

            gpio_pin_write(gpioc, ILED_GPIO_PIN, 0);

            duty = MIN_PWM_DUTY;
            HAL_TIM_StopPWM(&TimeHandle, TIM8, TIM_CHANNEL_1);

            printf("close the light \n");

            ctrl_mgr.dark_cnt = 0;
            ctrl_mgr.normal_cnt = 0;
        }
#endif

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

        OBJ_DBG_PRINTF("t_now- t_last = %d ms, post_yolo_easy=%d ms \n", t_now - t_last, t_now - last);
        display(dets, layer[0].classes, nboxes,  thresh, &object_colordrawn[0], &show_buf_addr);

        if (obj_det_data.cmd.usb)
        {
            //img_jpeg();//camera jpeg
            img.w = INPUT_WIDTH;
            img.h = INPUT_HEIGHT;
            jpeg_init(quality);//set jpeg quality

            state = irq_lock();
            ret = usb_request_buf(&pfrm_mgr, &frm_idx);
            irq_unlock(state);

            if (ret == true)
            {
                memcpy(obj_det_data.ex_buf.encode_jpeg_buf, show_buf_addr, 224 * 224 * 3);
                img.data = obj_det_data.ex_buf.encode_jpeg_buf;
                printf("addr:%x\n", pfrm_mgr.frm_info[frm_idx].fb_start);
                jpeg_write(&img, pfrm_mgr.frm_info[frm_idx].fb_start, quality);

                state = irq_lock();
                qbuf(&(pfrm_mgr.q), frm_idx);
                irq_unlock(state);
            }
        }
        else if (obj_det_data.cmd.wifi)
        {
            obj_det_data.stop_disp = 1;
            wifi_init();
            img_jpeg();
            while (obj_det_data.cmd.wifi)
            {
                ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);
                if (ret == 0)
                {
                    wifi_transfer(&obj_det_data);

                    /*Release the buffer for next camera capturing*/
                    dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);
                    HAL_VIN_Enable((VIN_Device_T *)0x40025000);
                }
            }
            k_sched_lock();
            camera_config(camera_isr);
            k_sched_unlock();
            obj_det_data.stop_disp = 0;
        }
        HAL_VIN_Enable((VIN_Device_T *)0x40025000);
        OBJ_DBG_PRINTF("Done!\n");
    }

    easynet_post_region_free(dets, nboxes);
    free_ext(0, tmp_buf);
}

int main(void)
{
    RET_CODE ret = RET_OK;

    struct device *gpiob = device_get_binding(VOL_PORT);
    gpio_pin_configure(gpiob, VOL_GPIO_PIN, GPIO_DIR_OUT);
    gpio_pin_write(gpiob, VOL_GPIO_PIN, 0);

#ifdef ILED_AUTO_ONOFF
    gpioc = device_get_binding(ILED_PORT);
    gpio_pin_configure(gpioc, ILED_GPIO_PIN, GPIO_DIR_OUT);
    gpio_pin_write(gpioc, ILED_GPIO_PIN, 0);

    /** Configure pinmux for an available TIM9 device */
    struct device *tim8_portc = device_get_binding(CONFIG_PINMUX_GM_PORTC_NAME);
    if (tim8_portc != NULL)
    {
        ret = tim8_pinmux_config(tim8_portc);
    }

    TIM_PWMSetting(TIM8);
#endif

    OBJ_DBG_PRINTF("body detect demo begin\n");
    device_init();

#ifdef USE_UARTPROTOCOL
    protocol_start();
#endif

#ifdef  USE_MOTOR
    Motor_Init();
    //Motor_Example();
#endif

    ret = ai_load_ops_data(OPS_BIN_FLASH_START, OPS_BIN_SIZE_OPTIMIZE, IMAGE_BIN_FLASH_START, IMAGE_BUFFER_SIZE,  OPS_WEI_BASE, RecvBuffer);
    body_detect();
    OBJ_DBG_PRINTF("body detect demo exit\n");

    return ret;
}
