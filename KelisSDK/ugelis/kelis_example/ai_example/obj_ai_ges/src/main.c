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

//#define USE_UARTPROTOCOL
#ifdef USE_UARTPROTOCOL
    #include "protocol/protocol.h"
#endif
//#define OBJ_PRINTF printf
#define OBJ_PRINTF(...)
//#define OBJ_DBG_PRINTF printf
#define OBJ_DBG_PRINTF do{} while(0);


#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)

/* OPS BINARY MACRO */
#define OPS_BIN_SIZE        (5*SIZE_1M)

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
#define CAM_PIC_SIZE  (224 * 224 * 3)
#define MAX_OBJECTCLASS_CNT 20
#define MAX_GESTURES_CNT 2

void *malloc_ext(uint8_t memx, uint32_t size);
void free_ext(uint8_t memx, void *ptr);
void rrggbb_split2rgb(uint8_t *src, uint8_t *dst, int w, int h);
void camera_isr(struct device *dev, uint32_t line_num);
#ifdef USE_UARTPROTOCOL
    extern void protocol_start();
    extern void protocol_send();
#endif


K_THREAD_STACK_DEFINE(stack_area, STACKSIZE);
static struct k_thread task_thread;

extern struct device *dcmi;
extern struct device *ltdc;
extern uint32_t show_buf_addr;
extern uint32_t t_Hw, t_MaxPool, t_Layer, t_AvgPool, t_WriteOut, t_DataFetch, t_Input ;

#define _GENERIC_SECTION_ZI(segment) __attribute__((section(STRINGIFY(segment)),zero_init))
__aligned(32) unsigned char RecvBuffer[IMAGE_BUFFER_SIZE] _GENERIC_SECTION_ZI(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned short inputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned short OutputImage[BUFFERSIZE] _GENERIC_SECTION_ZI(mem_ai_data);
__aligned(32) unsigned char OPS_WEI_BASE[OPS_BIN_SIZE]_GENERIC_SECTION_ZI(mem_ai_data);  /*more than 4 MB are needed for YOLO*/

ObjDetData obj_det_data;
objshow_t   objout;

uint8_t lcd_inited = 0;
detection_type gdet_type = DET_NOTDEF;

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
    obj_det_data.ex_buf.cam_buf_detected = malloc_ext(0, 224 * 224 * 3);
    obj_det_data.ex_buf.cam_buf_validate = malloc_ext(0, IMAGE_BUFFER_SIZE);

    OBJ_PRINTF("cam_buf_detected : 0x%08x cam_buf_validate : 0x%08x \n", obj_det_data.ex_buf.cam_buf_detected, obj_det_data.ex_buf.cam_buf_validate);

    obj_det_data.ex_buf.cam_buf_validate[IMAGE_BUFFER_SIZE - 1] = 0; /*Specified for CNN, Last*/
    obj_det_data.ex_buf.cam_buf_validate[IMAGE_BUFFER_SIZE - 2] = 0; /*Specified for CNN, Index*/

    obj_det_data.ex_buf.cam_buf_tmp = malloc_ext(0, 224 * 224 * 4);//malloc     /*A, R, G ,B, it's layer 1, blending on layer 2*/

    OBJ_PRINTF("cam_buf_tmp : 0x%08x \n", obj_det_data.ex_buf.cam_buf_tmp);

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

    k_sem_init(&obj_det_data.validate_lock, 0, 1);
}

int init(detection_type det_type)
{
    init_ai_data(det_type);
    init_shell(&obj_det_data);
    camera_config(camera_isr);
    OBJ_PRINTF("Init camera Done \n");
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
            //rrggbb_split2rgb((uint8_t *)RecvBuffer, (uint8_t *)demoaddr, 224, 224);  image_bin
            // rrggbb_split2rgb((uint8_t *)image_bin, (uint8_t *)demoaddr, 224, 224);
            //fb_ioctl(ltdc, 0, FB_X_SETADDR, &demoaddr);
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

void predict(struct easynet_dev *dev)
{
    int ret = 0;

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);

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

int main(void)
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
    aimodule_init_device();
#else
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
#endif
    /*print the buffer address*/
    OBJ_PRINTF("inputImage: 0x%08x, OutputImage: 0x%08x \n", inputImage, OutputImage);
    OBJ_PRINTF("OPS_WEI_BASE: 0x%08x, RecvBuffer: 0x%08x \n", OPS_WEI_BASE, RecvBuffer);

    /*Init for Object AI*/
    init(DET_GEST);

    OBJ_PRINTF("read ops and image weights begin\n");
    t_last = k_uptime_get_32();
    ret = ai_load_ops_data(OPS_BIN_FLASH_START, OPS_BIN_SIZE, IMAGE_BIN_FLASH_START, IMAGE_BUFFER_SIZE,  OPS_WEI_BASE, RecvBuffer);
    t_now = k_uptime_get_32();
    OBJ_PRINTF("ai_load_ops_data time consume %d ms \n", t_now - t_last);
    OBJ_PRINTF("read ops and image weights end, ret = %d\n", ret);
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

#ifdef USE_UARTPROTOCOL
    protocol_start();
#endif

    while (1)
    {
        OBJ_DBG_PRINTF("==== \n");
        k_sem_take(&obj_det_data.validate_lock, 3000); //K_FOREVER   // richard debug
        OBJ_DBG_PRINTF("**** \n");

        t_Hw = 0, t_MaxPool = 0, t_Layer = 0, t_AvgPool = 0, t_WriteOut = 0, t_DataFetch = 0, t_Input = 0;

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

            d_box = malloc_ext(0, out_width * out_height * box_num * box_channel * sizeof(float));
            box_class_probs = malloc_ext(0, out_height * out_width * box_num * class_channel * sizeof(float));
            s_box = malloc_ext(0, 20 * sizeof(Box_t));
            OBJ_PRINTF("AI Model Loaded Done !\n");
        }

        /*in case of stack overflow, it's better to spawn a thread for tiny YOLO*/
        t_last = k_uptime_get_32();

        /*debug use*/
        obj_det_data.show_demo_pic = 0;

        if (obj_det_data.show_demo_pic)
        {
            /*for CNN computation*/
            dev->input = RecvBuffer;//image_bin;// RecvBuffer;//image_bin;//  RecvBuffer;
            predict(dev);
        }
        else
        {
            dev->input = obj_det_data.ex_buf.cam_buf_validate;
            predict(dev);
        }

        t_now = k_uptime_get_32();

        int t = t_now - t_last;

        ret = PostYolo(&obj_det_data, &OutputImage[0], d_box, box_class_probs, s_box);

        if (ret != RET_OK)
        {
            OBJ_DBG_PRINTF("PostYolo failed\n");
        }

        t_now = k_uptime_get_32();
        OBJ_DBG_PRINTF("predict = %d ms \n", t);
        OBJ_DBG_PRINTF("all = %d ms \n", t_now - t_last);

        OBJ_DBG_PRINTF("t_Hw = %d ms , t_MaxPool= %d ms ,  t_WriteOut= %d ms , t_DataFetch = %d ms, t_Input = %d ms\n",
                       t_Hw, t_MaxPool, t_WriteOut, t_DataFetch, t_Input);

        OBJ_DBG_PRINTF("%d Box found\n", obj_det_data.boxcnt);

        objout.num = obj_det_data.boxcnt;
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

            objout.obj[i].objid     = obj_det_data.box[i].class;
            objout.obj[i].off_l     = box_x_min;
            objout.obj[i].off_t     = box_y_min;
            objout.obj[i].width     = box_x_max - box_x_min;
            objout.obj[i].height    = box_y_max - box_y_min;

            /*This will draw rectangle on Layer 1*/
            LCD_draw_rectangle(box_x_min, box_y_min, box_x_max, box_y_max, object_colordrawn[obj_det_data.box[i].class]);

        }

        /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
        fb_ioctl(ltdc, 0, FB_X_SETADDR, &show_buf_addr);
        HAL_VIN_Enable((VIN_Device_T *)0x40025000);

#ifdef USE_UARTPROTOCOL
        /*=====================================*/
        protocol_send();
#endif

        OBJ_DBG_PRINTF("Done!\n");
    }

    free_ext(0, s_box);
    free_ext(0, d_box);
    free_ext(0, box_class_probs);
}
