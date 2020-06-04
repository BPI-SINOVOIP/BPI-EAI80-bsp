#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <imlib.h>
#include <display/fb_display.h>
#include <gm_hal_vout.h>
#include <gm_hal_imp.h>
#include <gm_irq.h>

#include <ust_core.h>
#include <ust_lcd.h>


//#define UST_LCD_DEBUG 1
#ifdef UST_LCD_DEBUG
    #define ust_lcd_pr_debug       printk
#else
    #define ust_lcd_pr_debug(...)  do{}while(0)
#endif


#define DEV_UST_LCD_NAME  	       "ust_lcd_dev"


#define IMAGE_W  320
#define IMAGE_H  240

/* Change this if you have an LED connected to a custom port */
#define LCD_GPIO_PORT	 "GPIOB"
#define LCD_GPIO_PIN     13   /*PB13*/


static struct device *gpiob;
static struct device *ltdc;
static struct fb_displayinfo info;
static uint8_t pixfmt = FMT_RGB888;


static uint32_t time_cur = 0;
static uint32_t time_last = 0;



static void ust_lcd_set_pixfmt(uint32_t output_pixfmt)
{
    
    uint32_t pre_multiply = 0; 
	
#if defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_NARROW) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_NARROW) \
    || defined(TEST_CASE_MODE1_ARGB8888_RGB888_SCALE_MAGNIFY) || defined(TEST_CASE_MODE2_RGB888_ARGB8888_SCALE_MAGNIFY)
    struct fb_wininfo wininfo;
	wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = SCALE_WIN_WIDTH;
    wininfo.height = SCALE_WIN_HEIGHT;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);
#endif

	HAL_VOUT_SetPixelFormat((VOUT_Device_T *)0x40026000, output_pixfmt, 0);
    
    if(output_pixfmt == FMT_ARGB8888 || output_pixfmt == FMT_ARGB1555
		|| output_pixfmt == FMT_ARGB4444 || output_pixfmt == FMT_AL44
		|| output_pixfmt == FMT_AL88)
    {
        pre_multiply = 1;
    }
    else
    {
        pre_multiply = 0;
    }

    fb_ioctl(ltdc, 0, FB_X_SETPREMUL, &pre_multiply);
}


static void ust_lcd_isr(struct device *dev)
{
    //do nothing
    return;
}

void ust_lcd_config(void)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;
	
    gpiob = device_get_binding(LCD_GPIO_PORT);

    /* Set LED pin as output */
    gpio_pin_configure(gpiob, LCD_GPIO_PIN, GPIO_DIR_OUT);
    
    //gpio_pin_write(gpiob, LCD_GPIO_PIN, 0);

    gpio_pin_write(gpiob, LCD_GPIO_PIN, 1);
        
    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

#if (defined CAPTURE_YUV422)
    pixfmt = FMT_RGB888;
    info.pic_width = 240;
    info.pic_height = 160;
#else
    pixfmt = FMT_RGB565;
    info.pic_width = IMAGE_W;//240;
    info.pic_height = IMAGE_H;//160;
#endif

#ifdef CONFIG_OV5640_0
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);
#endif

#ifdef CONFIG_OV5640_1
    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = pixfmt;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = 0xc0a00000;

    fb_ioctl(ltdc, 1, FB_X_INITLAYER, &layerinfo);
#endif

#ifdef CONFIG_OV5640_0
    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);

    fb_irq_line_enable(ltdc, (info.pic_height-10));
    fb_irq_callback_set(ltdc, ust_lcd_isr);
#endif

#ifdef CONFIG_OV5640_1
	wininfo.x_start = 200;
    wininfo.y_start = 100;
    wininfo.width = info.pic_width;
    wininfo.height = info.pic_height;

    fb_ioctl(ltdc, 1, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 1, FB_X_SETPOS, &wininfo);
#endif
}


static int ust_element_workstatus_fun(struct element *element, enum pipeLinestatus status)
{
    int ret = 0;

	if(status == UST_PIPE_READY)
    {
        ust_lcd_config();
        ust_lcd_pr_debug("[ust_lcd][%s]: Plugin Ready\n", __FUNCTION__);
	}
    else if(status == UST_PIPE_PALY)
    {
        ust_lcd_pr_debug("[ust_lcd][%s]: Plugin Play\n", __FUNCTION__);
	}
    else if(status == UST_PIPE_STOP)
	{
        ust_lcd_pr_debug("[ust_lcd][%s]: Plugin Stop\n", __FUNCTION__);
	}


    return ret;
}

static int ust_sinkpad_chain_fun(UST_BufDesc_T *bufDesc,struct element *element)
{
    int ret = 0;
    uint32_t addr = 0;
    static UST_BufDesc_T *lastDesc = NULL;

    ust_lcd_pr_debug("[ust_lcd][%s] BufDes[in]:%p \n", __FUNCTION__, bufDesc);

    addr = (uint32_t)bufDesc->data;

    ust_lcd_set_pixfmt(FMT_L8);
    fb_ioctl(ltdc, 0, FB_X_SETADDR, &(addr));

    time_cur = k_uptime_get_32();
    printk("cost: %d ms\n", (time_cur-time_last));
    time_last = time_cur;
             
    /*free pre bufdes*/
    if(lastDesc != NULL)
    {
        UST_FreeBufDes(lastDesc);
    }
    lastDesc = bufDesc;

   return ret;
}

static int ust_element_init(struct device *dev)
{
	UST_Element_T *element = NULL;
	UST_Pad_T *sinkPad = NULL;
	int ret = 0;
    
	/*alloc element and config it*/
    element = UST_AllocElement(ELEMENT_LCD);
	if(!element){
        ust_lcd_pr_debug("[ust_lcd][%s]: Err:  UST_AllocElement NULL\n", __FUNCTION__);
        return -ENOMEM;
	}

	element->prv = dev;
	UST_RegisterElementWorkStatusFun(element, ust_element_workstatus_fun);

	/*alloc pad and config it*/
	sinkPad = UST_AllocPad(ELEMENT_LCD_PAD_SINK);
	if(!sinkPad){
        ust_lcd_pr_debug("[ust_lcd][%s]: Err:  UST_AllocSinkPad NULL\n", __FUNCTION__);
        return -ENOMEM;
	}
	/*set srcpad chian cb*/
	UST_RegisterPadChainFun(sinkPad, ust_sinkpad_chain_fun);

	/*add sink pad to element*/
	ret = UST_AddPadToElement(element,UST_SINK_PAD,sinkPad);
	if(ret){
        ust_lcd_pr_debug("[ust_lcd][%s]: Err:  UST_AddSinkPadToElement\n", __FUNCTION__);
        goto out;
	}

out:
	return ret;
}




DEVICE_DEFINE(ust_lcd, DEV_UST_LCD_NAME, ust_element_init, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;


