#include <camera/dcmi.h>
#include <display/fb_display.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>

#include "ai_common.h"
#include "tinyyolo_main.h"
#include "math.h"
#include "lcd.h"
#include "camera.h"
#include "draw.h"
#include <mem_cfg.h>
#include <gpio.h>
#include <gm_hal_vin.h>
#include "spi/spi_drv.h"
#include <gm_hal_tim.h>
#include <pinmux.h>

#include <imlib.h>
#include <stdio.h>
#include <stdlib.h>

extern unsigned char RecvBuffer[];
extern unsigned char CMD_BASE[];
extern unsigned char WEI_BASE[];

extern unsigned short inputImage[];
extern unsigned short OutputImage[];

#define UGELIS_NEW_VERSION 1

extern struct device *dcmi;
extern struct device *ltdc;

ObjDetData obj_det_data;

node_t *base_head = NULL;
node_t *previousNode = NULL;

#define CAM_PIC_SIZE  (224 * 224 * 3)


#define PRINTF(...)  do{} while(0);

#define PRINTK(...)  do{} while(0);

#define OBJ_PRINTF PRINTK
//#define OBJ_PRINTF(...)  do{} while(0);

//#define OBJ_DBG_PRINTF PRINTK
#define OBJ_DBG_PRINTF do{} while(0);

extern uint32_t show_buf_addr;

#define MAX_OBJECTCLASS_CNT 20
#define MAX_GESTURES_CNT 2

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

/* TIM handle declaration */
TIM_Handle_T TimeHandle;

struct ctrl_info
{
    uint8_t dark_cnt;
    uint8_t normal_cnt;
    uint8_t onoff;
};

static struct ctrl_info ctrl_mgr;

uint8_t *gray_pic;

/* change this to enable pull-up/pull-down */
#define PULL_UP 0

/* Change this if you have an LED connected to a custom port */
#define PORT_EN  "GPIOA"

#define LED_GPIO_PIN   2   /*PC10*/

struct device *gpioa;

void rrggbb_split2rgb(uint8_t *src, uint8_t *dst, int w, int h);

/*Feed rrggbb format to KDP310 for further training */
void rgb_split2rrggbb(uint8_t *src, uint8_t *dst, int w, int h)
{
    uint8_t *dst_b = &(dst[0]);
    uint8_t *dst_g = &(dst[w * h]);
    uint8_t *dst_r = &(dst[w * h * 2]);

    int c = 0, l = 0, i = 0;
    uint8_t *src_rgb = 0;
    for (l = 0; l < h; l++)
    {
        for (c = 0; c < w; c++)
        {
            src_rgb = src + (l * w + c) * 3;
            *(dst_b + i) = *(src_rgb + 2);    //b
            *(dst_g + i) = *(src_rgb + 1);    //g
            *(dst_r + i) = *(src_rgb);        //r
            i++;
        }
    }
}

//#define BACKGROUND_ANALYSE
#ifdef BACKGROUND_ANALYSE
    uint8_t gray_pic_fg[224 * 224];
#endif


uint8_t lcd_inited = 0;

/*Called every frame data captured by Camera*/
void camera_isr(struct device *dev, uint32_t line_num)
{
    struct dcmi_buf dis_buf;
    image_t src_img;
    int ret = 0;
    int mean;
    int mean_dev;

    src_img.bpp = IMAGE_BPP_GRAYSCALE;
    src_img.w = 224;
    src_img.h = 224;

    if (!obj_det_data.stop_disp)
    {
        ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);

        /*Camera can display Real-Time, we initialize LCD to fix
          snowflake sreen issue when display the first image frame*/
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

                    fb_convert_color(ltdc, dis_buf.addr, gray_pic, FMT_RGB888, FMT_L8, 224, 224);

                    src_img.pixels = (uint32_t)gray_pic;

                    imlib_image_mean(&src_img, 128, &mean, &mean_dev);

                    //printk("mean: %d, mean_dev: %d \n", mean, mean_dev);

                    if (mean_dev < abs(mean))
                    {
                        if (mean > 0)
                        {
                            ctrl_mgr.dark_cnt = 0;
                            ctrl_mgr.normal_cnt++;
                            //printk("brightness is too big \n");
                        }
                        else if (mean < 0)
                        {
                            ctrl_mgr.dark_cnt++;
                            ctrl_mgr.normal_cnt = 0;
                            //printk("brightness is too little \n");
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

#ifdef BACKGROUND_ANALYSE
                    VibeForground((unsigned char *)gray_pic, gray_pic_fg, 224, 224);
#endif
                }
            }

            show_buf_addr = dis_buf.addr;
            //fb_ioctl(ltdc, 0, FB_X_SETADDR, &(dis_buf.addr));
        }
        /*Release the buffer for next camera capturing*/
        dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);
    }

    //HAL_VIN_Enable((VIN_Device_T *)0x40025000);
}

#ifndef UGELIS_NEW_VERSION
    #define STACKSIZE                4096
    char __noinit __stack stack_area[STACKSIZE];
#else
    #define STACKSIZE                4096

    K_THREAD_STACK_DEFINE(stack_area, STACKSIZE);

    static struct k_thread task_thread;
#endif


static void cambuf_process_thread()
{
    uint32_t demoaddr = 0;

    while (1)
    {
        if (obj_det_data.show_demo_pic)
        {
            demoaddr = (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024 + 0xC0000000;  //0x10400000
            if (obj_det_data.show_demopic_onetime)
            {
                rrggbb_split2rgb((uint8_t *)RecvBuffer, (uint8_t *)demoaddr, 224, 224);
                fb_ioctl(ltdc, 0, FB_X_SETADDR, &demoaddr);
                obj_det_data.show_demopic_onetime = 0;
                k_sem_give(&obj_det_data.validate_lock);
            }
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

void init_ai_data()
{
    gray_pic = malloc_ext(0, 224 * 224);
    obj_det_data.ex_buf.cam_buf_detected = malloc_ext(0, 224 * 224 * 3); //malloc
    obj_det_data.ex_buf.cam_buf_validate = malloc_ext(0, TEST_BUFFER_SIZE);//malloc

    PRINTF("cam_buf_detected : 0x%08x cam_buf_validate : 0x%08x \n", obj_det_data.ex_buf.cam_buf_detected, obj_det_data.ex_buf.cam_buf_validate);

    obj_det_data.ex_buf.cam_buf_validate[TEST_BUFFER_SIZE - 1] = 0; /*Specified for CNN, Last*/
    obj_det_data.ex_buf.cam_buf_validate[TEST_BUFFER_SIZE - 2] = 0; /*Specified for CNN, Index*/

    obj_det_data.input_data_addr = obj_det_data.ex_buf.cam_buf_validate;

    obj_det_data.boxcnt = 0;
    obj_det_data.object_name = object_name;
    obj_det_data.stop_disp = 0;                 /*Stop display on LCD*/
    obj_det_data.threshold = 0.6;
    obj_det_data.one_frame_detect_started = 0;  /*Start detect one frame*/

    obj_det_data.show_demo_pic = 0;
    obj_det_data.show_demopic_onetime = 0;
    obj_det_data.cap_show = 1; //0

    obj_det_data.object_name = object_name;

    obj_det_data.unreg_imgcnt = 0;
    obj_det_data.reg_imgcnt = 0;
    obj_det_data.det_type = DET_BODY;      // DET_GEST
    obj_det_data.det_type_cpy = DET_BODY;  // DET_GEST

    obj_det_data.pic_saved = 0;            /*save picsture option*/

    k_sem_init(&obj_det_data.validate_lock, 0, 1); //UINT_MAX
}

void rrggbb_split2rgb(uint8_t *src, uint8_t *dst, int w, int h)
{
    int c = 0, l = 0, i = 0;
    uint8_t *src_rgb = src;
    uint8_t *dst_rgb = dst;

    for (i = 0; i < w * h; i ++)
    {
        *dst_rgb = *(src_rgb +  w * h * 2 + i);
        dst_rgb ++;
        *dst_rgb = *(src_rgb + w * h + i);
        dst_rgb ++;
        *dst_rgb = *(src_rgb + i);
        dst_rgb ++;
    }
}

int init()
{
    init_ai_data();
    init_shell(&obj_det_data);
    camera_config(camera_isr);
    PRINTK("Init camera Done \n");

    return 0;
}

#define BLACK         0x000000
#define BLUE          0x0000FF

//extern unsigned short OutputImage[BUFFERSIZE] ;
detection_type gdet_type = DET_NOTDEF;

unsigned int pt_buf = 0;

static void load_bodydet_ai_model(void)
{
    uint32_t t_now, t_last;
    PRINTK("========Load Body-Detection Model ========= \n");
    t_last = k_uptime_get_32();
    /*load weight*/
    ReadPage(0x10050000, 4559728, (uint8_t *)WEI_BASE);
    PRINTK("load weight done \n");
    /*load command*/
    ReadPage(0x10550000, 888, (uint8_t *)CMD_BASE);
    PRINTK("load command done \n");
    /*load demo picture*/
    ReadPage(0x10551000, 150530, (uint8_t *)RecvBuffer);
    t_now = k_uptime_get_32();
    PRINTK("load demp picture done, time-consume %d ms \n", t_now - t_last);
}

static void spi_flash_init(void)
{
    Init(0, 0, 0);
}


static void uart_trim()
{
    //*(volatile uint32_t *)0x1ffff0a8  = 0xfffffbab;
    //*(volatile uint32_t *)0x1ffff0ac  = 0xff88ffff;
    //*(volatile uint32_t *)0x1ffff0b0  = 0x7f3ffffc;

    *(volatile uint32_t *)0x1ffff0a8  = 0xfffffb5d;
    *(volatile uint32_t *)0x1ffff0ac  = 0xffacffff;
    *(volatile uint32_t *)0x1ffff0b0  = 0xff3fffff;
}

static int tim9_pinmux_config(struct device *port)
{
    int ret = 0;
    GPIO_PinConfig_T pin_cfg;

    pin_cfg.pin = BIT(PINMUX_GPIO_PIN_3);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_NOPULL;
    pin_cfg.alternate = GPIOA3_AF3_TIM9_CH1;
    ret = pinmux_pin_set(port, PINMUX_GPIO_PIN_3, (unsigned int)&pin_cfg);
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
    sOCConfig.ocPolarity = TIM_OCPOLARITY_HIGH;
    sOCConfig.ocFastMode = TIM_OCFAST_DISABLE;
    sOCConfig.pulse = 200;

    if (HAL_TIM_ConfigPWMChannel(&TimeHandle, TIMx, &sOCConfig, TIM_CHANNEL_1) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

#define SHOW_POSITION
#ifdef SHOW_POSITION
    #include <obj_info.h>
    static int show_position_init_done = 0;
    static camera_position_param_t camera_position_param;
#endif


int main(void)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;
    uint32_t kk = 0;
    uint32_t t_last, t_now;
    float box_x_min, box_y_min, box_x_max, box_y_max;
    char str[10];
    int ret = 0;

    uart_trim();

    /*Reset All IP*/
    //*(volatile uint32_t *)(0x40000070) = 0xFFFFFFFF;
    memset(inputImage, 0x00, 401408);
    memset(OutputImage, 0x00, 401408);

    /*enable a stable CPU clock*/
    *(volatile uint32_t *)(0x40000090) |= 2 << 0;
    /*power on kdp310 and reset*/
    *(volatile uint32_t *)(0x1FFFF0B0) &= ~(1 << 18);
    *(volatile uint32_t *)(0x40000070) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;
    *(volatile uint32_t *)(0x40000080) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;

    /*improve system powerformance*/
    *(volatile uint32_t *)(0x40039288) |= 0x3F << 16;
    *(volatile uint32_t *)(0x4000018C) |= 3;
    *(volatile uint32_t *)(0x4000009c) |= 2 << 16;

    ctrl_mgr.dark_cnt = 0;
    ctrl_mgr.normal_cnt = 0;
    ctrl_mgr.onoff = 0;

    /*print the buffer address*/
    OBJ_PRINTF("inputImage: 0x%08x \n", inputImage);
    OBJ_PRINTF("OutputImage: 0x%08x \n", OutputImage);

    OBJ_PRINTF("CMD_BASE: 0x%08x \n", CMD_BASE);

    OBJ_PRINTF("WEI_BASE: 0x%08x \n", WEI_BASE);

    OBJ_PRINTF("RecvBuffer: 0x%08x \n", RecvBuffer);

    OBJ_PRINTF("CMD_BASE: 0x%08x, WEI_BASE: 0x%08x, RecvBuffer: 0x%08x \n",  CMD_BASE, WEI_BASE, RecvBuffer);

    /*Init for Object AI*/
    init();

    /*Initial the SRAM specified for KDP310*/
    spi_flash_init();


#ifndef UGELIS_NEW_VERSION
    k_thread_spawn(stack_area, STACKSIZE, cambuf_process_thread, NULL, NULL, NULL,
                   K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
#else
    k_thread_create(&task_thread, stack_area,
                    K_THREAD_STACK_SIZEOF(stack_area),
                    (k_thread_entry_t)cambuf_process_thread,  NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
#endif

    /** Configure pinmux for an available TIM9 device */
    struct device *tim9_porta = device_get_binding(CONFIG_PINMUX_GM_PORTA_NAME);
    if (tim9_porta != NULL)
    {
        ret = tim9_pinmux_config(tim9_porta);
    }

    TIM_PWMSetting(TIM9);

    if (HAL_TIM_StartPWM(&TimeHandle, TIM9, TIM_CHANNEL_1) != HAL_OK)
    {
        return HAL_ERROR;
    }

    gpioa = device_get_binding(PORT_EN);

    /* Set LED pin as output */
    gpio_pin_configure(gpioa, LED_GPIO_PIN, GPIO_DIR_OUT);

    gpio_pin_write(gpioa, LED_GPIO_PIN, 0);

    while (1)
    {
        OBJ_DBG_PRINTF("==== \n");
        k_sem_take(&obj_det_data.validate_lock, 3000); //K_FOREVER
        OBJ_DBG_PRINTF("**** \n");

#ifdef CNN_TIME_TEST
        extern uint8_t gGetdata;
        extern uint8_t gInputdata;
        extern uint8_t gPutdata;
        extern uint8_t gMaxPool;
        extern uint32_t gGetdataTime_ms;
        extern uint32_t gPutdata_ms;
        extern uint32_t gMaxPool_ms;

        gGetdata = 0;
        gInputdata = 0;
        gPutdata = 0;
        gMaxPool = 0;
        gGetdataTime_ms = 0;
        gPutdata_ms = 0;
        gMaxPool_ms = 0;
#endif

        if (ctrl_mgr.onoff == 0 && ctrl_mgr.dark_cnt > 50)
        {
            ctrl_mgr.onoff = 1;

            gpio_pin_write(gpioa, LED_GPIO_PIN, 1);

            printk("open the light \n");

            ctrl_mgr.dark_cnt = 0;
            ctrl_mgr.normal_cnt = 0;
        }


        if (ctrl_mgr.onoff == 1 && ctrl_mgr.normal_cnt > 50)
        {
            ctrl_mgr.onoff = 0;

            gpio_pin_write(gpioa, LED_GPIO_PIN, 0);

            printk("close the light \n");

            ctrl_mgr.dark_cnt = 0;
            ctrl_mgr.normal_cnt = 0;
        }


        if (gdet_type != obj_det_data.det_type_cpy)
        {
            OBJ_PRINTF("Load AI Model %s ... \n", obj_det_data.det_type_cpy == 0 ? "Body" : "Gesture");

            if (obj_det_data.det_type_cpy == DET_BODY)
            {
                load_bodydet_ai_model();

                obj_det_data.det_type = DET_BODY;
            }

            gdet_type = obj_det_data.det_type_cpy;
            OBJ_PRINTF("AI Model Loaded Done !\n");
        }

        /*in case of stack overflow, it's better to spawn a thread for tiny YOLO*/
        t_last = k_uptime_get_32();

        /*debug use*/
        //      obj_det_data.show_demo_pic = 1;

        if (obj_det_data.show_demo_pic)
        {
            /*for CNN computation*/
            call_cnn(RecvBuffer);
        }
        else
        {
            call_cnn(obj_det_data.ex_buf.cam_buf_validate);
        }

#ifdef CNN_TIME_TEST
        t_last = k_uptime_get_32();
#endif
        PostYolo();
        t_now = k_uptime_get_32();
        OBJ_DBG_PRINTF("t_now- t_last = %d ms \n", t_now - t_last);
        OBJ_DBG_PRINTF("%d Box found\n", obj_det_data.boxcnt);

#ifdef CNN_TIME_TEST
        OBJ_PRINTF("Getdata: %d, PutData:%d, MaxPool:%d ms \n", gGetdataTime_ms, gPutdata_ms, gMaxPool_ms);
#endif

        /*Layer 1*/
        //LCD_clear();

        for (i = 0; i < obj_det_data.boxcnt; i ++)
        {
            OBJ_DBG_PRINTF("class: %d\n\r", obj_det_data.box[i].class);

            if (obj_det_data.det_type == DET_BODY)
            {
                OBJ_DBG_PRINTF("class name: person\n\r");
            }

            OBJ_DBG_PRINTF("score: %lf\n\r", obj_det_data.box[i].score);
            box_y_min = obj_det_data.box[i].box[0] < 0 ? 0 : obj_det_data.box[i].box[0];
            box_x_min = obj_det_data.box[i].box[1] < 0 ? 0 : obj_det_data.box[i].box[1];
            box_y_max = obj_det_data.box[i].box[2] < 0 ? 0 : obj_det_data.box[i].box[2];
            box_x_max = obj_det_data.box[i].box[3] < 0 ? 0 : obj_det_data.box[i].box[3];

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

            /*This will draw rectangle on Layer 1*/
            LCD_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, object_colordrawn[obj_det_data.box[i].class]);

#ifdef SHOW_POSITION
            if (i == 0)
            {
                printf("===================================================\n");
            }
            printf("(%4.0f, %4.0f, %4.0f, %4.0f)\t", box_x_min, box_y_min, box_x_max, box_y_max);
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
            //printf("cost: %d ms\n", end_time - start_time);
            char tmpstr[128] = { 0 };
            //sprintf(tmpstr, "(L:%.1f-F:%.1f-H:%.1f)", fabs(person_info.x), person_info.z, person_info.h);
            sprintf(tmpstr, "(Dist:%.1f-AG:%.1f)", sqrt(fabs(person_info.x)*fabs(person_info.x) + person_info.z * person_info.z), person_info.angle);
            printf("%s\n", tmpstr);
#endif
        }

        /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
        fb_ioctl(ltdc, 0, FB_X_SETADDR, &show_buf_addr);
        HAL_VIN_Enable((VIN_Device_T *)0x40025000);

        /*=====================================*/

        OBJ_DBG_PRINTF("Done!\n");
    }
    exit();
}

