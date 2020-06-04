/**
 * @file edgui_port_disp_templ.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <gm_hal_cache.h>
#include <gm_hal_imp.h>
#include "edgui_port_disp.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(edgui_disp_drv_t *disp_drv, const edgui_area_t *area, edgui_color_t *color_p);
#if EDGUI_USE_GPU
    static void gpu_blend(edgui_disp_drv_t *disp_drv, edgui_color_t *dest, const edgui_color_t *src, uint32_t length, edgui_opa_t opa);
    static void gpu_fill(edgui_disp_drv_t *disp_drv, edgui_color_t *dest_buf, edgui_coord_t dest_width, const edgui_area_t *fill_area, edgui_color_t color);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
struct device *gpio_lcd;
struct device *ltdc;
#define USE_IMP    1
struct fb_displayinfo info;
uint8_t pixfmt = FMT_RGB888;
/**********************
 *      MACROS
 **********************/
uint32_t ui_buf_addr = MEM_UI_BASE;
edgui_disp_buf_t disp_buf;
edgui_color_t *draw_buf;                      /*A buffer for 10 rows*/
static edgui_disp_drv_t disp_drv;
static int32_t x1_flush;
static int32_t y1_flush;
static int32_t x2_flush;
static int32_t y2_fill;
static int32_t y_fill_act;
static const edgui_color_t *buf_to_flush;
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void edgui_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /* edgui requires a buffer where it draws the objects. The buffer's has to be greater than 1 display row
     *
     * There are three buffering configurations:
     * 1. Create ONE buffer with some rows:
     *      edgui will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer with some rows:
     *      edgui will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable edgui to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Create TWO screen-sized buffer:
     *      Similar to 2) but the buffer have to be screen sized. When edgui is ready it will give the
     *      whole frame to display. This way you only need to change the frame buffer's address instead of
     *      copying the pixels.
     * */

    /* Example for 1) */
    memset((void *)(MEM_DRAW_BASE), 0x00, MEM_DRAW_SIZE);
    static edgui_color_t *draw_buf1 = (edgui_color_t *)MEM_DRAW_BASE;
    static edgui_disp_buf_t buf;
    edgui_disp_buf_init(&buf, draw_buf1, NULL, EDGUI_HOR_RES_MAX * EDGUI_VER_RES_MAX);
    /*-----------------------------------
     * Register the display in edgui
     *----------------------------------*/
    edgui_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = EDGUI_HOR_RES_MAX;
    disp_drv.ver_res = EDGUI_VER_RES_MAX;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.buffer = &buf;

#if EDGUI_USE_GPU
    /*Optionally add functions to access the GPU. (Only in buffered mode, EDGUI_VDB_SIZE != 0)*/

    /*Blend two color array using opacity*/
    disp_drv.gpu_blend_cb = gpu_blend;

    /*Fill a memory array with a color*/
    disp_drv.gpu_fill_cb = gpu_fill;
#endif

    /*Finally register the driver*/
    edgui_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Initialize your display and the required peripherals. */
static void disp_init(void)
{
    struct fb_wininfo wininfo;
    struct fb_layerinfo layerinfo;

    gpio_lcd = device_get_binding(LCD_BL_PORT);
    /* Set LED pin as output */
    gpio_pin_configure(gpio_lcd, LCD_BL_PIN, GPIO_DIR_OUT);
    gpio_pin_write(gpio_lcd, LCD_BL_PIN, 1);

    ltdc = device_get_binding(CONFIG_VOUT_NAME);

    fb_get_displayinfo(ltdc, &info);

    layerinfo.alpha = 255;
    layerinfo.alpha0 = 0;
    layerinfo.pixformat = FMT_RGB565;
    layerinfo.blending_factor1 = 6;
    layerinfo.blending_factor2 = 7;
    layerinfo.color = 0X000000;
    layerinfo.fb_start = (uint32_t) ui_buf_addr;

    fb_ioctl(ltdc, 0, FB_X_INITLAYER, &layerinfo);

    wininfo.x_start = 0;
    wininfo.y_start = 0;
    wininfo.width   = EDGUI_HOR_RES_MAX;
    wininfo.height  = EDGUI_VER_RES_MAX;
    memset((void *)(ui_buf_addr), 0x00, MEM_UI_SIZE);
    fb_ioctl(ltdc, 0, FB_X_SETSIZE, &wininfo);
    fb_ioctl(ltdc, 0, FB_X_SETPOS, &wininfo);
}

static void put_px(int32_t x, int32_t y, edgui_color_t color_p)
{
    edgui_color_t *start = (edgui_color_t *)ui_buf_addr;
    edgui_color_t *src = start + y * EDGUI_HOR_RES_MAX + x;
    *src = color_p;
}


/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'edgui_disp_flush_ready()' has to be called when finished. */
static void disp_flush(edgui_disp_drv_t *disp_drv, const edgui_area_t *area, edgui_color_t *color_p)
{
#if USE_IMP
    uint32_t  dest_width;
    uint32_t  copy_width;
    uint32_t  color_width;
    void *src_addr;
    void *dst_addr;
    edgui_color_t *start = (edgui_color_t *)ui_buf_addr;
    edgui_color_t *src_color;
    edgui_color_t *dst_color;

    /*Return if the area is out the screen*/
    if (area->x2 < 0)
    {
        return;
    }
    if (area->y2 < 0)
    {
        return;
    }
    if (area->x1 > EDGUI_HOR_RES_MAX - 1)
    {
        return;
    }
    if (area->y1 > EDGUI_VER_RES_MAX - 1)
    {
        return;
    }

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > EDGUI_HOR_RES_MAX - 1 ? EDGUI_HOR_RES_MAX - 1 : area->x2;
    int32_t act_y2 = area->y2 > EDGUI_VER_RES_MAX - 1 ? EDGUI_VER_RES_MAX - 1 : area->y2;

    int32_t x = act_x1;
    int32_t y = act_y1;
    color_width = act_x2 - act_x1 + 1;
    copy_width = color_width * (EDGUI_COLOR_DEPTH >> 3);
    dest_width = EDGUI_HOR_RES_MAX * (EDGUI_COLOR_DEPTH >> 3);
    dst_color = start + act_y1 * EDGUI_HOR_RES_MAX + act_x1;
    src_color = color_p;
    for (y = act_y1; y <= act_y2; y++)
    {
        memcpy((void *)dst_color, (void *)src_color, copy_width);
        dst_color += EDGUI_HOR_RES_MAX;
        src_color += color_width;
    }
#else
    int32_t x;
    int32_t y;
    for (y = area->y1; y <= area->y2; y++)
    {
        for (x = area->x1; x <= area->x2; x++)
        {
            /* Put a pixel to the display. For example: */
            put_px(x, y, *color_p);
            color_p++;
        }
    }

#endif
    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    edgui_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/
#if EDGUI_USE_GPU

/* If your MCU has hardware accelerator (GPU) then you can use it to blend to memories using opacity
 * It can be used only in buffered mode (EDGUI_VDB_SIZE != 0 in edgui_conf.h)*/
static void gpu_blend(edgui_disp_drv_t *disp_drv, edgui_color_t *dest, const edgui_color_t *src, uint32_t length, edgui_opa_t opa)
{
#if USE_IMP
    uint32_t i;
    struct fb_mixcolorbulk mix;
    mix.fg_addr = src;
    mix.bg_addr = dest;
    mix.dst_addr = dest;
    mix.alpha  = opa;
    mix.fg_pixfmt = IMP_INPUT_RGB565;
    mix.bg_pixfmt = IMP_INPUT_RGB565;
    mix.dst_pixfmt = IMP_OUTPUT_RGB565;
    mix.xsize = length;
    mix.ysize = 1;

    HAL_DCACHE_InvalidAll();
    if (length > 8)
    {
        fb_mix_colorsbulk(ltdc, &mix);
    }
    else
    {
        for (i = 0; i < length; i++)
        {
            dest[i] = edgui_color_mix(src[i], dest[i], opa);
        }
    }
#else
    /*It's an example code which should be done by your GPU*/
    uint32_t i;
    for (i = 0; i < length; i++)
    {
        dest[i] = edgui_color_mix(src[i], dest[i], opa);
        //dest[i] = src[i];
    }
#endif
}

/* If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color
 * It can be used only in buffered mode (EDGUI_VDB_SIZE != 0 in edgui_conf.h)*/
static void gpu_fill(edgui_disp_drv_t *disp_drv, edgui_color_t *dest_buf, edgui_coord_t dest_width,
                     const edgui_area_t *fill_area, edgui_color_t color)
{
#if USE_IMP
    struct fb_fillrect fillrect;
    uint32_t x, y;
    HAL_DCACHE_InvalidAll();
    uint32_t x_width = fill_area->x2 - fill_area->x1 + 1;
    uint32_t y_width = fill_area->y2 - fill_area->y1 + 1;
    if ((x_width > 2) || (y_width > 2))
    {
        fillrect.xsize = x_width;
        fillrect.ysize = y_width;
        fillrect.pixfmt = FMT_RGB565;
        fillrect.offline = dest_width - fillrect.xsize;
        fillrect.dst_addr = dest_buf + (fill_area->y1 * dest_width + fill_area->x1);
        fillrect.color = (uint32_t)edgui_color_to32(color);//RGB888value
        fb_fill_rect(ltdc, &fillrect);
    }
    else
    {
        dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
        for (y = fill_area->y1; y <= fill_area->y2; y++)
        {
            for (x = fill_area->x1; x <= fill_area->x2; x++)
            {
                dest_buf[x] = color;
            }
            dest_buf += dest_width;  /*Go to the next line*/
        }
    }


#else
    /*It's an example code which should be done by your GPU*/
    uint32_t x, y;
    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
    for (y = fill_area->y1; y <= fill_area->y2; y++)
    {
        for (x = fill_area->x1; x <= fill_area->x2; x++)
        {
            dest_buf[x] = color;
        }
        dest_buf += dest_width;  /*Go to the next line*/
    }
#endif
}

#endif  /*EDGUI_USE_GPU*/
