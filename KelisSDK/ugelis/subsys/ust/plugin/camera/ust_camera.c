#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <camera/dcmi.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>
#include <gm_hal_imp.h>
#include <display/fb_display.h>
#include <stdlib.h>

#include <ust_core.h>
#include <ust_camera.h>

#include "img_data.h"

//#define UST_CAMERA_DEBUG 1
#ifdef UST_CAMERA_DEBUG
    #define ust_camera_pr_debug       printk
#else
    #define ust_camera_pr_debug(...)  do{}while(0)
#endif



#define DEV_UST_CAMERA_NAME  	              "ust_camera_dev"
#define UST_CAMERA_QUEUE_MAX_OBJECT_NUM       5


#define IMP_DEVICE (IMP_Device_T *)(0x40024000)

#define IMAGE_W  320
#define IMAGE_H  240

//#define CAPTURE_JPEG
//#define CAPTURE_YUV422
#define CAPTURE_RGB565

typedef enum
{
    INPUT_FROM_PIC,
    INPUT_FROM_CAM,
}INPUT_MODE;
INPUT_MODE input_mode = INPUT_FROM_PIC;//INPUT_FROM_CAM;//



static struct device *dcmi;
#ifdef CONFIG_OV5640_0
static struct device *camera_sensor_0;
#endif
#ifdef CONFIG_OV5640_1
static struct device *camera_sensor_1;
#endif


static int ust_camera_config(void)
{
    int ret = 0;
    struct dcmi_requestbuffers reqbufs;
    struct dcmi_buf buf;
    struct dcmi_format fmt;
    struct dcmi_crop_rect rect;
    enum light_mode awb;
    enum af_mode auto_foucs;
    int saturation, brightness, contrast, sharpness;
    uint32_t pic_width = IMAGE_W;	    
	uint32_t pic_height = IMAGE_H;	

#ifdef CONFIG_OV5640_0
    camera_sensor_0 = device_get_binding(OV5640_0_DEV_NAME);

    camera_set_power(camera_sensor_0, true);

#if (defined CAPTURE_JPEG)
    camera_set_fmt(camera_sensor_0, VIDEO_FMT_JPEG, MODE_QSXGA_2592_1944, FPS_15);
#elif (defined CAPTURE_YUV422)
    camera_set_fmt(camera_sensor_0, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_15);
#else
    camera_set_fmt(camera_sensor_0, VIDEO_FMT_RGB565, MODE_QVGA_320_240, FPS_15);
#endif
    
    awb = WB_AUTO;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_WHITE_BALANCE, &awb);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
    
    saturation = 0;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_SATURATION, &saturation);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
    
    brightness = 0;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_BRIGHTNESS, &brightness);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
    
    contrast = 0;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_CONTRAST, &contrast);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
    
    sharpness = 33;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_SHARPNESS, &sharpness);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
    
    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor_0, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if(ret != 0)
    {
        return -ENOTSUP;
    }	
#endif

#ifdef CONFIG_OV5640_1
    camera_sensor_1 = device_get_binding(OV5640_1_DEV_NAME);

    camera_set_power(camera_sensor_1, true);

#if (defined CAPTURE_JPEG)
    camera_set_fmt(camera_sensor_1, VIDEO_FMT_JPEG, MODE_QSXGA_2592_1944, FPS_15);
#elif (defined CAPTURE_YUV422)
    camera_set_fmt(camera_sensor_1, VIDEO_FMT_YUV422, MODE_QVGA_320_240, FPS_15);
#else
    camera_set_fmt(camera_sensor_1, VIDEO_FMT_RGB565, MODE_QVGA_320_240, FPS_15);
#endif
    
    awb = WB_AUTO;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_WHITE_BALANCE, &awb);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
    
    saturation = 0;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_SATURATION, &saturation);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
    
    brightness = 0;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_BRIGHTNESS, &brightness);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
    
    contrast = 0;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_CONTRAST, &contrast);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
    
    sharpness = 33;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_SHARPNESS, &sharpness);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
    
    auto_foucs = AF_SINGLE;//AF_CONTINUEOUS;
    ret = camera_ioctl(camera_sensor_1, CAMERA_CID_FOCUS_AUTO, &auto_foucs);
    if(ret != 0)
    {
        return -ENOTSUP;
    }
#endif

    dcmi = device_get_binding(CONFIG_VIN_NAME);

#if (defined CAPTURE_JPEG)
    fmt.pix_fmt = PIX_FMT_JPEG;
    fmt.width = 2592;
    fmt.height = 1944;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);
        
    reqbufs.count = 1;
    reqbufs.mem_start = 0xc0a00000;
    reqbufs.length = 6*1024*1024;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);
#elif(defined CAPTURE_YUV422)
    fmt.pix_fmt = PIX_FMT_YUV422;
    fmt.width = pic_width;
    fmt.height = pic_height;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    rect.top = 0;
    rect.left = 0;
    rect.width = 2*pic_width - 1;
    rect.height = pic_height - 1;

    dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);

	/* only support YUV422 TO RGB888 */
    uint32_t onoff = 1;
    dcmi_ioctl(dcmi, VIDIOC_CONV_COLORSPACE, &onoff);
	
    reqbufs.count = 4;
    reqbufs.mem_start = 0xc0a00000;//0x20020000;//0xc0000000;
    reqbufs.length =  pic_width*pic_height*3*4;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);
#else
    fmt.pix_fmt = PIX_FMT_RGB565;
    fmt.width = pic_width;
    fmt.height = pic_height;
    dcmi_set_fmt_vid_cap(dcmi, &fmt);

    rect.top = 0;
    rect.left = 0;
    rect.width = 2*pic_width - 1;
    rect.height = pic_height - 1;

    dcmi_ioctl(dcmi, VIDIOC_S_CROP, &rect);

	
    reqbufs.count = 4;
    reqbufs.mem_start = 0xc0a00000;//0x20020000;//0xc0000000;
    reqbufs.length =  pic_width*pic_height*2*4;
    dcmi_ioctl(dcmi, VIDIOC_CREATE_BUFS, &reqbufs);
#endif
    
    buf.index = 0;
    dcmi_ioctl(dcmi, VIDIOC_QUERYBUF, &buf);
    
    //dcmi_irq_callback_set(dcmi, camera_isr);
    
    //ret = dcmi_start_streaming(dcmi, VIN_MODE_SNAPSHOT); 
    ret = dcmi_start_streaming(dcmi, VIN_MODE_CONTINUOUS); 
    if(ret != 0)
    {
        return -ENOTSUP;
    }

    return 0;
}

static int ust_camera_change_mode(uint32_t src, uint32_t dst, uint32_t input_pixfmt, uint32_t output_pixfmt, \
	uint32_t input_offset, uint32_t output_offset, uint32_t pic_width, uint32_t pic_height)
{
    IMP_Device_T *pIMP = IMP_DEVICE;
    IMP_Config_T config;
    IMP_LayerCfg_T layerCfg;
    IMP_ScaleCfg_T scaleCfg;
    uint32_t pixel_num = 0;
    uint32_t timeout = 0;
    
    
    
    memset(&config, 0, sizeof(IMP_Config_T));
    config.mode = IMP_M2M_PFC;
    config.colorMode = output_pixfmt;
    config.outputOffset = output_offset;
    HAL_IMP_Init(pIMP, &config);

    memset(&layerCfg, 0, sizeof(IMP_LayerCfg_T));
    layerCfg.inputColorMode = input_pixfmt;
    layerCfg.inputOffset = input_offset;
	layerCfg.alphaMode = IMP_NO_MODIF_ALPHA;
    HAL_IMP_ConfigLayer(pIMP, &layerCfg, 1);

    pixel_num = pic_width - input_offset;

    memset(&scaleCfg, 0, sizeof(IMP_ScaleCfg_T));

#if defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_NARROW) || defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_MAGNIFY)
    float h_scale = 0.0;
    float w_scale = 0.0;
    w_scale = (float)pixel_num/(SCALE_WIN_WIDTH - output_offset);
    h_scale = (float)pic_height/SCALE_WIN_HEIGHT;
	
    scaleCfg.enable = TRUE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pic_height;
	scaleCfg.scaleWidth = (SCALE_WIN_WIDTH - output_offset);
    scaleCfg.scaleHeight = SCALE_WIN_HEIGHT;
	scaleCfg.svi = fast_floorf(h_scale);
	scaleCfg.svf = fast_roundf((float)(h_scale - scaleCfg.svi)*256);
	scaleCfg.shi = fast_floorf(w_scale);
	scaleCfg.shf = fast_roundf((float)(w_scale - scaleCfg.shi)*256);
#else
    scaleCfg.enable = FALSE;
    scaleCfg.oriWidth = pixel_num;
    scaleCfg.oriHeight = pic_height; 
#endif

    HAL_IMP_Start(pIMP, (uint32_t)src, (uint32_t)dst, &scaleCfg);

    while(__HAL_IMP_GET_FLAG(pIMP, IMP_FLAG_TC) == 0)
    {
        timeout++;
        //if(timeout>0X1FFFFF)break; 
    }

    __HAL_IMP_CLEAR_FLAG(pIMP, IMP_FLAG_TC);
	
    return 0;
}



static int ust_element_workstatus_fun(struct element *element, enum pipeLinestatus status)
{
    int ret = 0;

	if(status == UST_PIPE_READY)
    {
        if(input_mode == INPUT_FROM_CAM)
        {
            ust_camera_config();
        }
        ust_camera_pr_debug("[ust_camera][%s]: Plugin Ready\n", __FUNCTION__);
	}
    else if(status == UST_PIPE_PALY)
	{    
        ust_camera_pr_debug("[ust_camera][%s]: Plugin PALY\n", __FUNCTION__);
	}
    else if(status == UST_PIPE_STOP)
    {
        ust_camera_pr_debug("[ust_camera][%s]: Plugin STOP\n", __FUNCTION__);
	}

    return ret;
}

static int ust_srcpad_chain_fun(UST_BufDesc_T *bufDesc,struct element *element)
{
    UNUSED(bufDesc);   
    
    int ret = 0;
    //UST_MemPool_T *memPool =(UST_MemPool_T *)(element->prv);
    UST_Pad_T *currentSrcPad= NULL;
    UST_BufDesc_T *pushBufdesc = NULL;
    struct dcmi_buf dis_buf;   
    uint8_t *img_buf = NULL;
    uint32_t img_size = 0;
    uint32_t img_width = IMAGE_W;
    uint32_t img_height = IMAGE_H;
    int timeout = 0;

    /* transfer to Gray */
    if(input_mode == INPUT_FROM_CAM)
    {
        while(timeout++ < 10)
        {
            ret = dcmi_ioctl(dcmi, VIDIOC_DQBUF, &dis_buf);
            if(ret == 0)
            {
                break;
            }
        } 
        if(ret != 0)
        {          
            ust_camera_pr_debug("[ust_camera][%s]: Irogn: VIDIOC_DQBUF not get\n", __FUNCTION__);
            ret = 0;//not get frame, not an error
            goto out;
        }

        img_size = img_width*img_height;
        img_buf = (uint8_t *)malloc(img_size);
        if(img_buf == NULL)
        {          
            ust_camera_pr_debug("[ust_camera][%s]: malloc img_buf fail\n", __FUNCTION__);
            ret = - ENOMEM;
            goto out;
        }
    
        ust_camera_change_mode(dis_buf.addr, (uint32_t)img_buf, IMP_INPUT_RGB565, FMT_L8, 0, 0, img_width, img_height);     

        /* release camera buffer */
        dcmi_ioctl(dcmi, VIDIOC_QBUF, &dis_buf);
        /* continue the vin */
        HAL_VIN_Enable((VIN_Device_T *)0x40025000);
    }
    else
    {
        img_size = img_width*img_height;
        img_buf = (uint8_t *)malloc(img_size);
        if(img_buf == NULL)
        {          
            ust_camera_pr_debug("[ust_camera][%s]: malloc img_buf fail\n", __FUNCTION__);
            ret = - ENOMEM;
            goto out;
        }
        memcpy(img_buf, img_gray_lena, sizeof(img_gray_lena));
    }
    
    pushBufdesc = UST_AllocBufDes();
    if(!pushBufdesc)
    {
        ust_camera_pr_debug("[ust_camera][%s]: Err: UST_AllocBufDes NULL\n", __FUNCTION__);
        free(img_buf);
        ret = - ENOMEM;
        goto out;
    }

    pushBufdesc->data   = img_buf;
    pushBufdesc->length = img_size;
    pushBufdesc->type   = UST_DATA_FROM_MALLOC;
    pushBufdesc->prv    = &img_width;

	/*get currnet elemet peer pad and push to it*/
    currentSrcPad = element->srcPadManage.padTable[0];

    ust_camera_pr_debug("[ust_camera][%s] BufDes[out]:%p \n", __FUNCTION__, pushBufdesc);
    ret = UST_PadPush(currentSrcPad, pushBufdesc);
    if(ret)
    {
        ust_camera_pr_debug("[ust_camera][%s]: Err: UST_PadPush Need Trace\n", __FUNCTION__);
        goto out;
    }
    
out:
    
    return ret;
}

static int ust_prepare_element(struct device *dev)
{
    int ret = 0;

    UST_Element_T *element = NULL;
    UST_Pad_T *srcPad = NULL;

    /*alloc src element and config it*/
    element = UST_AllocElement(ELEMENT_CAMERA);
    if(!element){
        ret = -ENOMEM;
        goto out;
    }

    element->prv = dev;
    UST_RegisterElementWorkStatusFun(element, ust_element_workstatus_fun);

    /*alloc pad and config it*/
    srcPad = UST_AllocPad(ELEMENT_CAMERA_PAD_SRC);
    if(!srcPad){
        ret = -ENOMEM;
        goto out;
    }
    /*set srcpad chian cb*/
    UST_RegisterPadChainFun(srcPad, ust_srcpad_chain_fun);

    /*add src pad to element*/
    ret = UST_AddPadToElement(element,UST_SRC_PAD,srcPad);
    if(ret){
        goto out;
    }

#if 0
    /*alloc data mempool for queue later*/
    memPool = UST_CreatObjectMemoryPool(UST_CAMERA_QUEUE_MAX_OBJECT_NUM, UST_CAMERA_DATA_SIZE);
    if(!memPool){
        ret = -ENOMEM;
        goto out;
    }
    element->prv = memPool;
#endif

out:

    return ret;
}

static int ust_prepare_queue()
{
#ifdef CONFIG_UST_PLUGIN_CAMERA_QUEUE
    /*alloc queue*/
    UST_QueueElement_T *queueElement = NULL;
    int ret = 0;
    queueElement = UST_AllocQueueElement(ELEMENT_CAMERA_QUEUE,ELEMENT_CAMERA_QUEUE_PAD_SINK,
                                         ELEMENT_CAMERA_QUEUE_PAD_SRC, CONFIG_UST_PLUGIN_CAMERA_QUEUE_MAX_NUM);

    if(!queueElement){
        ust_camera_pr_debug("[ust_camera][%s] UST_AllocQueueElement NULL\n",__FUNCTION__);
        ret = -ENOMEM;
        goto out;
    }

    /*link queue*/
    UST_Pad_T *srcElementSrcPad = NULL;
    UST_Pad_T *queueElementSinkPad = NULL;
    UST_Element_T *srcElement = NULL;

    srcElement = UST_GetElement(ELEMENT_CAMERA);
    if(!srcElement){
        ret = -ENOMEM;
        goto out;
    }
    queueElement = (UST_QueueElement_T *)(UST_GetElement(ELEMENT_CAMERA_QUEUE));
    if(!queueElement){
        ret = -ENOMEM;
        goto out;
    }

    srcElementSrcPad = UST_GetPadFromElement(srcElement, UST_SRC_PAD, ELEMENT_CAMERA_PAD_SRC);
    if(!srcElementSrcPad){
        ret = -ENOMEM;
        goto out;
    }

    queueElementSinkPad = UST_GetPadFromElement((UST_Element_T *)queueElement, UST_SINK_PAD, ELEMENT_CAMERA_QUEUE_PAD_SINK);
    if(!queueElementSinkPad){
        ret = -ENOMEM;
        goto out;
    }

    UST_PadConnect(srcElementSrcPad, queueElementSinkPad);
out:
    return ret;

#else
    return 0;
#endif

}


static int ust_element_init(struct device *dev)
{

	int ret = 0;

    ret = ust_prepare_element(dev);
    if(ret){
        ust_camera_pr_debug("[ust_camera][%s]: Err:  ust_prepare_element\n", __FUNCTION__);
        goto out;
    }

    ret = ust_prepare_queue();
    if(ret){
        ust_camera_pr_debug("[ust_camera][%s] ust_prepare_queue\n",__FUNCTION__);
        goto out;
    }

out:
	return ret;
}



DEVICE_DEFINE(ust_camera, DEV_UST_CAMERA_NAME, ust_element_init, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;








