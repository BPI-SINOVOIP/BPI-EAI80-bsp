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
#include <gm_hal_clock.h>
#include "app_processor.h"
#include <gm_hal_reset.h>

#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)
#define OPS_BIN_SIZE        (5*SIZE_1M)

#define PULL_UP 0
/* Change this if you have an LED connected to a custom port */
#define PORT_EN  "GPIOA"
#define LED_GPIO_PIN   2   /*PA02*/

/* FLASH MEMORY MAP */
#ifdef FLASH_SIZE_32M
    #define AI_FLASH_START_OFFSET  (0x00ad2000)
#else   //16M
    #define AI_FLASH_START_OFFSET  (0x00050000)
#endif

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

void camera_isr(struct device *dev, uint32_t line_num);

K_THREAD_STACK_DEFINE(stack_area, STACKSIZE);
static struct k_thread task_thread;

extern struct device *dcmi;
extern struct device *ltdc;
extern uint32_t show_buf_addr;

#define _GENERIC_SECTION_ZI(segment) __attribute__((section(STRINGIFY(segment)),zero_init))
__aligned(32) unsigned char RecvBuffer[IMAGE_BUFFER_SIZE] _GENERIC_SECTION_ZI(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned short inputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned short OutputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned char OPS_WEI_BASE[OPS_BIN_SIZE]_GENERIC_SECTION_ZI(mem_ai_data);  /*more than 4 MB are needed for YOLO*/


struct ctrl_info
{
    uint8_t dark_cnt;
    uint8_t normal_cnt;
    uint8_t onoff;
};

uint8_t lcd_inited = 0;
ObjDetData obj_det_data;
/* TIM handle declaration */
TIM_Handle_T TimeHandle;

#define MAX_OBJECTCLASS_CNT 20
#define MAX_GESTURES_CNT 2

//#define BACKGROUND_ANALYSE
#ifdef BACKGROUND_ANALYSE
    uint8_t gray_pic_fg[224 * 224];
#endif

//#define SHOW_POSITION
#ifdef SHOW_POSITION
    #include <obj_info.h>
    static int show_position_init_done = 0;
    static camera_position_param_t camera_position_param;
#endif

detection_type gdet_type = DET_NOTDEF;
static struct ctrl_info ctrl_mgr;
uint8_t *gray_pic;
struct device *gpioa;

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

void init_ai_data(detection_type det_type)
{
    gray_pic = malloc_ext(0, 224 * 224);
    obj_det_data.ex_buf.cam_buf_detected = malloc_ext(0, 224 * 224 * 3); //malloc
    obj_det_data.ex_buf.cam_buf_validate = malloc_ext(0, IMAGE_BUFFER_SIZE);

    printf("cam_buf_detected : 0x%08x cam_buf_validate : 0x%08x \n", obj_det_data.ex_buf.cam_buf_detected, obj_det_data.ex_buf.cam_buf_validate);

    obj_det_data.ex_buf.cam_buf_validate[IMAGE_BUFFER_SIZE - 1] = 0; /*Specified for CNN, Last*/
    obj_det_data.ex_buf.cam_buf_validate[IMAGE_BUFFER_SIZE - 2] = 0; /*Specified for CNN, Index*/

    obj_det_data.ex_buf.cam_buf_tmp = malloc_ext(0, 224 * 224 * 4);//malloc     /*A, R, G ,B, it's layer 1, blending on layer 2*/

    printf("cam_buf_tmp : 0x%08x \n", obj_det_data.ex_buf.cam_buf_tmp);

    memset((void *)obj_det_data.ex_buf.cam_buf_tmp, 0x00, 224 * 224 * 4);
    obj_det_data.input_data_addr = obj_det_data.ex_buf.cam_buf_validate;

    obj_det_data.boxcnt = 0;
    obj_det_data.object_name = object_name;
    obj_det_data.stop_disp = 0;                 /*Stop display on LCD*/
    obj_det_data.threshold = 0.6;
    obj_det_data.one_frame_detect_started = 0;  /*Start detect one frame*/

    obj_det_data.show_demo_pic = 0;
    obj_det_data.show_demopic_onetime = 0;
    obj_det_data.cap_show = 1;

    obj_det_data.object_name = object_name;
    obj_det_data.det_type =  det_type;
    obj_det_data.det_type_cpy = det_type ;

    obj_det_data.pic_saved = 0;

    k_sem_init(&obj_det_data.validate_lock, 0, 1); //UINT_MAX
}

int init(detection_type det_type)
{
    init_ai_data(det_type);
    init_shell(&obj_det_data);
    camera_config(camera_isr);
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
#if 0
                    fb_convert_color(ltdc, dis_buf.addr, gray_pic, FMT_RGB888, FMT_L8, 224, 224);

                    src_img.pixels = (uint32_t)gray_pic;

                    //imlib_image_mean(&src_img, 128, &mean, &mean_dev);

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
#endif
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

            //rrggbb_split2rgb((uint8_t *)image_bin, (uint8_t *)demoaddr, 224, 224);
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

void predict(struct easynet_dev *dev)
{
    int ret = 0;

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    /* Process all ops */
    do
    {
        ret = easynet_ops_process(dev);

        if (ret == 0x06 || ret == 0x01)
        {
            if (ret == 0x06)
            {
                dev = easynet_process_update(OPS_WEI_BASE, &inputImage[0], &OutputImage[0]);
            }
            break;
        }
    } while (1);

    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
}

static int tim9_pinmux_config(struct device *port)
{
#if 0
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
#endif

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

void body_detect(void)
{
    int out_width, out_height, box_channel, confidence_channel, box_num, class_channel ;
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t k = 0;
    uint32_t kk = 0;
    uint32_t t_last, t_now;
    float box_x_min, box_y_min, box_x_max, box_y_max;
    char str[10];
    struct easynet_dev *dev;
    float *d_box = NULL;
    float *box_class_probs = NULL;
    Box_t *s_box = NULL;

    RET_CODE ret = RET_OK;

    /*Reset All IP*/
    memset(inputImage, 0x00, 401408);
    memset(OutputImage, 0x00, 401408);

#if 0
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
    /*enable a stable CPU clock*/
    *(volatile uint32_t *)(0x40000090) |= 3 << 0;  /*fast kdp run 300MHZ*/

    *(volatile uint32_t *)(0x40000070) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;

    *(volatile uint32_t *)(0x40000080) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;

    /*improve system powerformance*/
    *(volatile uint32_t *)(0x40039288) |= 0x3F << 16;

    *(volatile uint32_t *)(0x40000188) |= 1 << 0;

    *(volatile uint32_t *)(0x4000018C) &= ~3;
    *(volatile uint32_t *)(0x4000018C) |= 2;

    while ((*(volatile uint32_t *)(0x400001A4) & (1 << 5)) != (1 << 5));

    *(volatile uint32_t *)(0x4000009c) |= 2 << 16;

#endif
#endif

    ctrl_mgr.dark_cnt = 0;
    ctrl_mgr.normal_cnt = 0;
    ctrl_mgr.onoff = 0;

    /*Init for Object AI*/
    init(DET_BODY);

    /*print the buffer address*/
    OBJ_PRINTF("inputImage: 0x%08x \n", inputImage);
    OBJ_PRINTF("OutputImage: 0x%08x \n", OutputImage);

    printf("read ops and image weights begin\n");
    t_last = k_uptime_get_32();
    ret = ai_load_ops_data(OPS_BIN_FLASH_START, OPS_BIN_SIZE, IMAGE_BIN_FLASH_START, IMAGE_BUFFER_SIZE,  OPS_WEI_BASE, RecvBuffer);
    t_now = k_uptime_get_32();
    printf("ai_load_ops_data time consume %d ms \n", t_now - t_last);
    printf("read ops and image weights end, ret = %d\n", ret);
    dev = easynet_process_init(OPS_WEI_BASE, &inputImage[0], &OutputImage[0]);

    /*Initial the SRAM specified for KDP310*/
    for (i = 0; i < 0x40000; i += 4)
    {
        *(volatile uint32_t *)(0x20020000 + i) = 0x00000000;
    }

    k_thread_create(&task_thread, stack_area,
                    K_THREAD_STACK_SIZEOF(stack_area),
                    (k_thread_entry_t)cambuf_process_thread,  NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);


    /** Configure pinmux for an available TIM9 device */
    //struct device *tim9_porta = device_get_binding(CONFIG_PINMUX_GM_PORTA_NAME);
    //if (tim9_porta != NULL)
    //  ret = tim9_pinmux_config(tim9_porta);

    //TIM_PWMSetting(TIM9);

    //if(HAL_TIM_StartPWM(&TimeHandle, TIM9, TIM_CHANNEL_1) != HAL_OK)
    //{
    //   return HAL_ERROR;
    //}

    //gpioa = device_get_binding(PORT_EN);

    /* Set LED pin as output */
    //gpio_pin_configure(gpioa, LED_GPIO_PIN, GPIO_DIR_OUT);

    //gpio_pin_write(gpioa, LED_GPIO_PIN, 0);

    while (1)
    {
        OBJ_DBG_PRINTF("==== \n");
        k_sem_take(&obj_det_data.validate_lock, 3000); //K_FOREVER
        OBJ_DBG_PRINTF("**** \n");

        if (ctrl_mgr.onoff == 0 && ctrl_mgr.dark_cnt > 50)
        {
            ctrl_mgr.onoff = 1;

            //gpio_pin_write(gpioa, LED_GPIO_PIN, 1);

            printk("open the light \n");

            ctrl_mgr.dark_cnt = 0;
            ctrl_mgr.normal_cnt = 0;
        }

        if (ctrl_mgr.onoff == 1 && ctrl_mgr.normal_cnt > 50)
        {
            ctrl_mgr.onoff = 0;

            //gpio_pin_write(gpioa, LED_GPIO_PIN, 0);

            printk("close the light \n");

            ctrl_mgr.dark_cnt = 0;
            ctrl_mgr.normal_cnt = 0;
        }

        if (gdet_type != obj_det_data.det_type_cpy)
        {
            OBJ_PRINTF("Load AI Model %s ... \n", obj_det_data.det_type_cpy == 0 ? "Body" : "Gesture");
            if (obj_det_data.det_type_cpy == DET_BODY)
            {
                obj_det_data.det_type = DET_BODY;
            }

            if (obj_det_data.det_type_cpy == DET_GEST)
            {
                obj_det_data.det_type = DET_GEST;
            }

            gdet_type = obj_det_data.det_type_cpy;

            if (obj_det_data.det_type == DET_BODY)
            {
                out_width = 7;
                out_height = 7;
                box_channel = 4;
                confidence_channel = 1;
                box_num = 5;
                class_channel = 1;
            }
            else if (obj_det_data.det_type == DET_GEST)
            {
                out_width = 7;
                out_height = 7;
                box_channel = 4;
                confidence_channel = 1;
                box_num = 5;
                class_channel = 2;
            }

            d_box = malloc_ext(0, out_width * out_height * box_num * box_channel * sizeof(float)); //malloc(0, out_width*out_height*box_num*box_channel * sizeof(double));//calloc(out_width*out_height*box_num*box_channel,sizeof(double));
            box_class_probs = malloc_ext(0, out_height * out_width * box_num * class_channel * sizeof(float)); //malloc(0, out_height*out_width*box_num*class_channel * sizeof(double));//calloc(out_height*out_width*box_num*class_channel,sizeof(double));
            s_box = malloc_ext(0, 20 * sizeof(Box_t));
            OBJ_PRINTF("AI Model Loaded Done !\n");
        }

        /*in case of stack overflow, it's better to spawn a thread for tiny YOLO*/
        t_last = k_uptime_get_32();

        /*debug use*/
        obj_det_data.show_demo_pic = 0;
        t_last = k_uptime_get_32();
        if (obj_det_data.show_demo_pic)
        {
            /*for CNN computation*/
            //dev->input = image_bin;//  RecvBuffer;

            dev->input = RecvBuffer;
            predict(dev);
        }
        else
        {
            dev->input = obj_det_data.ex_buf.cam_buf_validate;
            predict(dev);
        }

        ret = PostYolo(&obj_det_data, &OutputImage[0], d_box, box_class_probs, s_box);
        t_now = k_uptime_get_32();
        if (ret != RET_OK)
        {
            printf("PostYolo failed\n");
        }
        //printf("t_now- t_last = %d ms \n", t_now - t_last);
        //printf("%d Box found\n", obj_det_data.boxcnt);
        max_body_index = -1;
    	max_body_width = 0;
    	max_body_x  = -1;
    	max_body_dist = 0;

        for (i = 0; i < obj_det_data.boxcnt; i ++)
        {
            OBJ_DBG_PRINTF("class: %d\n\r", obj_det_data.box[i].class);
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

        }
        /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
        fb_ioctl(ltdc, 0, FB_X_SETADDR, &show_buf_addr);
        HAL_VIN_Enable((VIN_Device_T *)0x40025000);

        /*=====================================*/

        OBJ_DBG_PRINTF("Done!\n");
    }

    free_ext(0, s_box);
    free_ext(0, d_box);
    free_ext(0, box_class_probs);
}

int main(void)
{
    printf("body detect demo begin\n");
    aimodule_init_device();
#ifdef  USE_MOTOR
    Motor_Init();
    //Motor_Example();
#endif

    body_detect();
    printf("body detect demo exit\n");
    return 0;
}
