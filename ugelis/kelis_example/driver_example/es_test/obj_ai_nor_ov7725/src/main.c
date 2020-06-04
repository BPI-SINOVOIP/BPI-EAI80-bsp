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

#include <stdio.h>
#include <stdlib.h>

/** add by colin **/
#include <misc/printk.h>
#include <gm_hal_uart.h>
#include <driver/uart/gm_ll_uart.h>
/** end **/

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



//uint8_t camvedio_read_done = 0;
//uint8_t thread_write_done = 0;

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

struct buf_info
{
    uint8_t last_layer0_index;
    uint8_t cur_layer0_index;
};

static struct buf_info buf_mgr;


/* change this to enable pull-up/pull-down */
#define PULL_UP 0

/* Change this if you have an LED connected to a custom port */
#define PORT     "GPIOJ"

#define LED_GPIO_PIN   10   /*PC10*/
#define LED1_GPIO_PIN  11   /*PC11*/
#define SW0_GPIO_PIN   3    /*PB03*/

static int cnt = 0;

struct device *dev;

void rrggbb_split2rgb(uint8_t *src, uint8_t *dst, int w, int h);

#if 1
/** add by colin **/
#define FRAME_HEADER1 0x5E
#define FRAME_HEADER2 0x5E
#define MAX_INFO_LEN  78
#define MAX_RX_BUF    256
#define MAX_TX_BUF    256
#define UART_BAUD_RATE 9600L//19200L
#define FRAME_LENGTH_INDEX 4


typedef struct
{
    uint8_t   frameHeader1;                  /**<Frame Header 1 */
    uint8_t   frameHeader2;                  /**<Frame Header 2 */
    uint8_t   destAddr;                      /**< Dest Address */
    uint8_t   srcAddr;                       /**< Source Address */
    uint8_t   frameLen;                      /**< Frame Length */
    uint8_t   cmd;                           /**< commander */
    uint8_t   frameInfo[MAX_INFO_LEN];       /**< Frame Information */
    uint8_t   check;                         /**< Verfication Byte */
} Frame_T;


/**
 * Device Address
 */
typedef enum
{
    BROADCAST_ADDR          = 0x00,  /**< Broadcast Device Address */
    CAMERAL_BOARD_ADDR      = 0xFF,  /**< Cameral   Device Address */
    HOUSE_AIR_COND_ADDR     = 0xF0,  /**<Household Air Conditon Address  */
    MERCHANT_AIR_COND_ADDR  = 0xF1   /**<Merchant Air Conditon Address  */
} Device_Addr_T;

/**
 * Device Address
 */
typedef enum
{
    FRAME_CTRL_STATUS_CMD         = 0x1A,  /**< The Commander Means The Frame Type Is Control Status Frame */
    FRAME_PERSON_INFO_CMD         = 0x1B,  /**< The Commander Means The Frame Type Is Person Information Frame */
    FRAME_SET_CMD                 = 0x1C,  /**< The Commander Means The Frame Type Is Setting Frame */
    FRAME_DEVICE_INFO_CMD         = 0x1D,  /**< The Commander Means The Frame Type Is Device Info Frame */
    FRAME_CTRL_ANSWER_CMD         = 0x8A,  /**< The Commander Means The Frame Type Is Control Answer Frame */
    FRAME_SET_ANSWER_CMD          = 0x8C,  /**< The Commander Means The Frame Type Is Setting Answer Frame */
    FRAME_DEVICE_INFO_ANSWER_CMD  = 0x8D   /**< The Commander Means The Frame Type Is Device Info Answer Frame */
} Frame_Type_Cmd_T;

static UART_Handle_T g_uartHandle;
static uint8_t g_rxData[MAX_RX_BUF];
//static uint8_t g_txNoPersonFramData[MAX_TX_BUF];
static uint8_t g_txPersonFrameData[MAX_TX_BUF];


static volatile uint32_t s_rcvSize = 0;  /**< Size of received data */
uint8_t  g_frame_length = 0;
uint8_t  g_frame_recv_finished = 0;/**<Means the frame data is receive finished*/
uint8_t  g_frame_be_read = 0;

#define STACKSIZE                4096

K_THREAD_STACK_DEFINE(rxdata_parse_stack_area, STACKSIZE);

static struct k_thread rxdata_parse_task_thread;

/****Begin : Define send data timer ****/
static void ctrl_frame_work_handler(struct k_work *work)
{
    //Send control status frame data
    PRINTK("Send control status frame data\n");
}
static void person_frame_work_handler(struct k_work *work)
{
    //Send control status frame data
    int i = 8;
    int j = 0;
    int timeout = 0;

    PRINTK("colin1:");
    for (i = 0; i < 8; i++)
    {
        PRINTK("%x ", g_txPersonFrameData[i]);
        HAL_UART_PutChar((&g_uartHandle)->device, g_txPersonFrameData[i]);
    }
    PRINTK("\n");

    //for(j = 0; j < 3; j++)//Colin>>Change 3 to 2 as below,because 300ms send data ,if 3*100ms will send the same data twice
    for (j = 0; j < 3; j++)
    {
        timeout = 7;
        k_sleep(30);  //the slave board will send the response data 30ms later after recieved data

        while (timeout--)
        {
            //PRINTK("%d\n",timeout);
            k_sleep(9);

            //printk("\n time %lf\n",this_time);
            if (1 == g_frame_recv_finished)
            {
                g_frame_be_read = 1;
                goto FUNC_RETURN;
            }
        }


        PRINTK("colin2:");

        for (i = 0; i < 8; i++)
        {
            PRINTK("%x ", g_txPersonFrameData[i]);
            HAL_UART_PutChar((&g_uartHandle)->device, g_txPersonFrameData[i]);
        }

    }
FUNC_RETURN:
    return;
}


K_WORK_DEFINE(ctrl_frame_work, ctrl_frame_work_handler);
K_WORK_DEFINE(person_frame_work, person_frame_work_handler);
//static struct k_work ctrl_frame_work;
//static struct k_work person_frame_work;


static void ctrl_frame_timer_handler(struct k_timer *timer)
{
    k_work_submit(&ctrl_frame_work);
}
static void person_frame_timer_handler(struct k_timer *timer)
{
    k_work_submit(&person_frame_work);
}

K_TIMER_DEFINE(ctrl_frame_timer, ctrl_frame_timer_handler, NULL);
K_TIMER_DEFINE(person_frame_timer, person_frame_timer_handler, NULL);

/****End : Define send data timer ****/


/*Clear the RX or TX buffer*/
static void clear_frame_buffer(uint8_t *buf, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++)
    {
        buf[i] = 0x00;
    }
}


#if 0
void set_frame(Frame_T *frame, uint8_t destAddr, uint8_t srcAddr, uint8_t frameLen, uint8_t cmd, uint8_t *frameInfo, uint8_t check)
{
    if (NULL == frame)
    {
        return;
    }
    frame->frameHeader1 = FRAME_HEADER1;
    frame->frameHeader2 = FRAME_HEADER2;
    frame->destAddr     = destAddr;
    frame->srcAddr      = srcAddr;
    frame->frameLen     = frameLen;
    frame->cmd          = cmd;
    frame->frameInfo    = frameInfo;
    frame->check        = check;
}
#endif

static uint8_t checksum(uint8_t *data, uint8_t from, uint8_t to)
{
    uint8_t i;
    int sum;
    sum = 0;
    for (i = from; i < to; i++)
    {
        sum += data[i];
    }
    return (uint8_t)(sum & 0xFFu);
}

static void set_person_frame_data(uint8_t num)
{
    clear_frame_buffer(g_txPersonFrameData, MAX_TX_BUF);
    g_txPersonFrameData[0] = FRAME_HEADER1;
    g_txPersonFrameData[1] = FRAME_HEADER2;
    g_txPersonFrameData[2] = BROADCAST_ADDR;
    g_txPersonFrameData[3] = CAMERAL_BOARD_ADDR;
    g_txPersonFrameData[4] = 3;
    g_txPersonFrameData[5] = FRAME_PERSON_INFO_CMD;
    g_txPersonFrameData[6] = num;
    g_txPersonFrameData[7] = (3 + FRAME_PERSON_INFO_CMD + num) & 0xFF;
}


static bool b_check_rxdata_valid(uint8_t *rxdata, uint32_t size)
{
    int from  = 0;
    int to    = 0;
    uint8_t frame_length = rxdata[FRAME_LENGTH_INDEX];
    from = FRAME_LENGTH_INDEX;   //4: frame length position index
    to = FRAME_LENGTH_INDEX + frame_length;

    if ((FRAME_HEADER1 == rxdata[0]) && (FRAME_HEADER2 == rxdata[1])
            && (checksum(rxdata, from, to) == rxdata[to]))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void uart_recv_data_callback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{
    uint32_t index;

    if (event == UART_EVENT_RX_CHAR)
    {
        index = s_rcvSize++;
        g_rxData[index] = param;
        if (FRAME_LENGTH_INDEX == index)
        {
            g_frame_length = param;
        }
        else if ((FRAME_LENGTH_INDEX + g_frame_length) == index)
        {
            g_frame_recv_finished = 1;
        }
    }
    else
    {
        //do nothing
    }
}

void uart_recv_data(UART_Device_T *pUART)
{
    UART_Config_T uartConfig;
    UART_Handle_T *hUart = &g_uartHandle;
    UART_TransferConfig_T transConfig;
    UART_TransferConfig_T *pTransConfig = &transConfig;
    //uint16_t copiedSize = 0;

    /* Uart device configuration */
    uartConfig.baudRate = UART_BAUD_RATE;
    uartConfig.parity = UART_PARITY_EVEN;
    uartConfig.wordLength = UART_DATABITS_8;
    uartConfig.stopBits = UART_STOPBITS_1;
    uartConfig.clockSrc = 8000000;
    uartConfig.callback = uart_recv_data_callback; //use callback to receive data
    HAL_UART_Init(hUart, pUART, &uartConfig);

    /* Data transfer configuration(Disable tx fifo mode) */
    pTransConfig->enableFifo = FALSE;

    /*Disable tx and rx fifo ,not enable rxDMA and txDMA*/
    pTransConfig->txWatermark = 0;
    pTransConfig->rxWatermark = 0;
    pTransConfig->enableRxDMA = FALSE;
    pTransConfig->enableTxDMA = FALSE;

    HAL_UART_TransferConfig(hUart, pTransConfig);

    /* Enable  general uart rx interrupt */
    HAL_UART_EnableInterrupts(hUart->device, UART_RX_INT);

    /* Initialize variable about recevied size */
    s_rcvSize = 0;
    //g_event = UART_EVENT_NONE;//Colin>>This is used?
    /* Reset receive buffer (padding with 0x00) */
    clear_frame_buffer(g_rxData, MAX_RX_BUF);
}

static void rxdata_parse_process_thread()
{
    while (1)
    {
        if (1 == g_frame_be_read)//g_frame_be_read is seted means g_frame_recv_finished is also seted
        {
            g_frame_be_read = 0;
            g_frame_recv_finished = 0;
            s_rcvSize = 0;
            if (TRUE == b_check_rxdata_valid(g_rxData, MAX_RX_BUF))
            {
                //parse the data;but parsing the data is no useful

            }
            clear_frame_buffer(g_rxData, MAX_RX_BUF);
        }

        k_sleep(5);
        k_yield();
    }
}
/** end **/
#endif

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

uint8_t lcd_inited = 0;

/*Called every frame data captured by Camera*/
void camera_isr(struct device *dev, uint32_t line_num)
{
    struct dcmi_buf dis_buf;
    int ret = 0;

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

    //lcd_config();
    //PRINTK("Init camera \n");

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

#if 1
    /***colin>>Begin::used for uart protocal**/
    /**1. Open uart receive data handle*/
    //uart_recv_data(UART0);
    uart_recv_data(UART2);

    /**2. Create task to parse the received data  */
    k_tid_t tid = k_thread_create(&rxdata_parse_task_thread, rxdata_parse_stack_area,
                                  K_THREAD_STACK_SIZEOF(rxdata_parse_stack_area), (k_thread_entry_t)rxdata_parse_process_thread,  NULL, NULL, NULL,
                                  K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    k_thread_start(tid);

    /**3. Start timer to send data*/
    k_timer_start(&ctrl_frame_timer, 300, 300);
    k_timer_start(&person_frame_timer, 2000, 2000);
#endif

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

    buf_mgr.last_layer0_index = 0xff;
    buf_mgr.cur_layer0_index = 0xff;

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
        }

        /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
        fb_ioctl(ltdc, 0, FB_X_SETADDR, &show_buf_addr);
        HAL_VIN_Enable((VIN_Device_T *)0x40025000);

        //add by colin
        set_person_frame_data(obj_det_data.boxcnt);

        /*=====================================*/

        OBJ_DBG_PRINTF("Done!\n");
    }
    exit();
}

