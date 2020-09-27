/**
 * @file edgui_canvas.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include "edgui_canvas.h"
#include "../edgui_misc/edgui_math.h"
#include "../edgui_draw/edgui_draw.h"
#include "../edgui_core/edgui_refr.h"

#if EDGUI_USE_CANVAS != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_canvas_signal(edgui_obj_t * canvas, edgui_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;
static edgui_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a canvas object
 * @param par pointer to an object, it will be the parent of the new canvas
 * @param copy pointer to a canvas object, if not NULL then the new object will be copied from it
 * @return pointer to the created canvas
 */
edgui_obj_t * edgui_canvas_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("canvas create started");

    /*Create the ancestor of canvas*/
    edgui_obj_t * new_canvas = edgui_img_create(par, copy);
    edgui_mem_assert(new_canvas);
    if(new_canvas == NULL) return NULL;

    /*Allocate the canvas type specific extended data*/
    edgui_canvas_ext_t * ext = edgui_obj_allocate_ext_attr(new_canvas, sizeof(edgui_canvas_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_canvas);
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_canvas);

    /*Initialize the allocated 'ext' */
    ext->dsc.header.always_zero = 0;
    ext->dsc.header.cf          = EDGUI_IMG_CF_TRUE_COLOR;
    ext->dsc.header.h           = 0;
    ext->dsc.header.w           = 0;
    ext->dsc.data_size          = 0;
    ext->dsc.data               = NULL;

    edgui_img_set_src(new_canvas, &ext->dsc);

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_canvas, edgui_canvas_signal);

    /*Init the new canvas canvas*/
    if(copy == NULL) {

    }
    /*Copy an existing canvas*/
    else {
        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_canvas);
    }

    EDGUI_LOG_INFO("canvas created");

    return new_canvas;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a buffer for the canvas.
 * @param buf a buffer where the content of the canvas will be.
 * The required size is (edgui_img_color_format_get_px_size(cf) * w * h) / 8)
 * It can be allocated with `edgui_mem_alloc()` or
 * it can be statically allocated array (e.g. static edgui_color_t buf[100*50]) or
 * it can be an address in RAM or external SRAM
 * @param canvas pointer to a canvas object
 * @param w width of the canvas
 * @param h height of the canvas
 * @param cf color format. The following formats are supported:
 *      EDGUI_IMG_CF_TRUE_COLOR, EDGUI_IMG_CF_TRUE_COLOR_CHROMA_KEYED, EDGUI_IMG_CF_INDEXES_1/2/4/8BIT
 *
 */
void edgui_canvas_set_buffer(edgui_obj_t * canvas, void * buf, edgui_coord_t w, edgui_coord_t h, edgui_img_cf_t cf)
{
    edgui_canvas_ext_t * ext = edgui_obj_get_ext_attr(canvas);

    ext->dsc.header.cf = cf;
    ext->dsc.header.w  = w;
    ext->dsc.header.h  = h;
    ext->dsc.data      = buf;
    ext->dsc.data_size = (edgui_img_color_format_get_px_size(cf) * w * h) / 8;

    edgui_img_set_src(canvas, &ext->dsc);
}

/**
 * Set the color of a pixel on the canvas
 * @param canvas pointer to canvas object
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @param c color of the point
 */
void edgui_canvas_set_px(edgui_obj_t * canvas, edgui_coord_t x, edgui_coord_t y, edgui_color_t c)
{
    edgui_canvas_ext_t * ext = edgui_obj_get_ext_attr(canvas);

    edgui_img_buf_set_px_color(&ext->dsc, x, y, c);
    edgui_obj_invalidate(canvas);
}

/**
 * Set the palette color of a canvas with index format. Valid only for `EDGUI_IMG_CF_INDEXED1/2/4/8`
 * @param canvas pointer to canvas object
 * @param id the palette color to set:
 *   - for `EDGUI_IMG_CF_INDEXED1`: 0..1
 *   - for `EDGUI_IMG_CF_INDEXED2`: 0..3
 *   - for `EDGUI_IMG_CF_INDEXED4`: 0..15
 *   - for `EDGUI_IMG_CF_INDEXED8`: 0..255
 * @param c the color to set
 */
void edgui_canvas_set_palette(edgui_obj_t * canvas, uint8_t id, edgui_color_t c)
{
    edgui_canvas_ext_t * ext = edgui_obj_get_ext_attr(canvas);

    edgui_img_buf_set_palette(&ext->dsc, id, c);
    edgui_obj_invalidate(canvas);
}

/**
 * Set a style of a canvas.
 * @param canvas pointer to canvas object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_canvas_set_style(edgui_obj_t * canvas, edgui_canvas_style_t type, const edgui_style_t * style)
{
    switch(type) {
        case EDGUI_CANVAS_STYLE_MAIN: edgui_img_set_style(canvas, EDGUI_IMG_STYLE_MAIN, style); break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the color of a pixel on the canvas
 * @param canvas
 * @param x x coordinate of the point to set
 * @param y x coordinate of the point to set
 * @return color of the point
 */
edgui_color_t edgui_canvas_get_px(edgui_obj_t * canvas, edgui_coord_t x, edgui_coord_t y)
{
    edgui_canvas_ext_t * ext    = edgui_obj_get_ext_attr(canvas);
    const edgui_style_t * style = edgui_canvas_get_style(canvas, EDGUI_CANVAS_STYLE_MAIN);

    return edgui_img_buf_get_px_color(&ext->dsc, x, y, style);
}

/**
 * Get the image of the canvas as a pointer to an `edgui_img_dsc_t` variable.
 * @param canvas pointer to a canvas object
 * @return pointer to the image descriptor.
 */
edgui_img_dsc_t * edgui_canvas_get_img(edgui_obj_t * canvas)
{
    edgui_canvas_ext_t * ext = edgui_obj_get_ext_attr(canvas);

    return &ext->dsc;
}

/**
 * Get style of a canvas.
 * @param canvas pointer to canvas object
 * @param type which style should be get
 * @return style pointer to the style
 */
const edgui_style_t * edgui_canvas_get_style(const edgui_obj_t * canvas, edgui_canvas_style_t type)
{
    const edgui_style_t * style = NULL;

    switch(type) {
        case EDGUI_CANVAS_STYLE_MAIN: style = edgui_img_get_style(canvas, EDGUI_IMG_STYLE_MAIN); break;
        default: style = NULL;
    }

    return style;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Copy a buffer to the canvas
 * @param canvas pointer to a canvas object
 * @param to_copy buffer to copy. The color format has to match with the canvas's buffer color
 * format
 * @param w width of the buffer to copy
 * @param h height of the buffer to copy
 * @param x left side of the destination position
 * @param y top side of the destination position
 */
void edgui_canvas_copy_buf(edgui_obj_t * canvas, const void * to_copy, edgui_coord_t x, edgui_coord_t y, edgui_coord_t w, edgui_coord_t h)
{
    edgui_canvas_ext_t * ext = edgui_obj_get_ext_attr(canvas);
    if(x + w >= ext->dsc.header.w || y + h >= ext->dsc.header.h) {
        EDGUI_LOG_WARN("edgui_canvas_copy_buf: x or y out of the canvas");
        return;
    }

    uint32_t px_size   = edgui_img_color_format_get_px_size(ext->dsc.header.cf) >> 3;
    uint32_t px        = ext->dsc.header.w * y * px_size + x * px_size;
    uint8_t * to_copy8 = (uint8_t *)to_copy;
    edgui_coord_t i;
    for(i = 0; i < h; i++) {
        memcpy((void *)&ext->dsc.data[px], to_copy8, w * px_size);
        px += ext->dsc.header.w * px_size;
        to_copy8 += w * px_size;
    }
}

/**
 * Rotate and image and store the result on a canvas.
 * @param canvas pointer to a canvas object
 * @param img pointer to an image descriptor.
 *             Can be the image descriptor of an other canvas too (`edgui_canvas_get_img()`).
 * @param angle the angle of rotation (0..360);
 * @param offset_x offset X to tell where to put the result data on destination canvas
 * @param offset_y offset X to tell where to put the result data on destination canvas
 * @param pivot_x pivot X of rotation. Relative to the source canvas
 *                Set to `source width / 2` to rotate around the center
 * @param pivot_y pivot Y of rotation. Relative to the source canvas
 *                Set to `source height / 2` to rotate around the center
 */
void edgui_canvas_rotate(edgui_obj_t * canvas, edgui_img_dsc_t * img, int16_t angle, edgui_coord_t offset_x, edgui_coord_t offset_y,
                      int32_t pivot_x, int32_t pivot_y)
{
    edgui_canvas_ext_t * ext_dst = edgui_obj_get_ext_attr(canvas);
    const edgui_style_t * style  = edgui_canvas_get_style(canvas, EDGUI_CANVAS_STYLE_MAIN);
    int32_t sinma             = edgui_trigo_sin(-angle);
    int32_t cosma             = edgui_trigo_sin(-angle + 90); /* cos */

    int32_t img_width   = img->header.w;
    int32_t img_height  = img->header.h;
    int32_t dest_width  = ext_dst->dsc.header.w;
    int32_t dest_height = ext_dst->dsc.header.h;

    int32_t x;
    int32_t y;
    for(x = -offset_x; x < dest_width - offset_x; x++) {
        for(y = -offset_y; y < dest_height - offset_y; y++) {
            /*Get the target point relative coordinates to the pivot*/
            int32_t xt = x - pivot_x;
            int32_t yt = y - pivot_y;

            /*Get the source pixel from the upscaled image*/
            int32_t xs = ((cosma * xt - sinma * yt) >> (EDGUI_TRIGO_SHIFT - 8)) + pivot_x * 256;
            int32_t ys = ((sinma * xt + cosma * yt) >> (EDGUI_TRIGO_SHIFT - 8)) + pivot_y * 256;

            /*Get the integer part of the source pixel*/
            int xs_int = xs >> 8;
            int ys_int = ys >> 8;

            if(xs_int >= img_width)
                continue;
            else if(xs_int < 0)
                continue;

            if(ys_int >= img_height)
                continue;
            else if(ys_int < 0)
                continue;

            /*Get the fractional part of the source pixel*/
            int xs_fract = xs & 0xff;
            int ys_fract = ys & 0xff;

            /* If the fractional < 0x70 mix the source pixel with the left/top pixel
             * If the fractional > 0x90 mix the source pixel with the right/bottom pixel
             * In the 0x70..0x90 range use the unchanged source pixel */

            int xn;      /*x neightboor*/
            edgui_opa_t xr; /*x mix ratio*/
            if(xs_fract < 0x70) {
                xn = xs_int - 1;
                xr = xs_fract * 2;
            } else if(xs_fract > 0x90) {
                xn = xs_int + 1;
                xr = (0xFF - xs_fract) * 2;
            } else {
                xn = xs_int;
                xr = 0xFF;
            }

            /*Handle under/overflow*/
            if(xn >= img_width)
                continue;
            else if(xn < 0)
                continue;

            int yn;      /*y neightboor*/
            edgui_opa_t yr; /*y mix ratio*/
            if(ys_fract < 0x70) {
                yn = ys_int - 1;
                yr = ys_fract * 2;
            } else if(ys_fract > 0x90) {
                yn = ys_int + 1;
                yr = (0xFF - ys_fract) * 2;
            } else {
                yn = ys_int;
                yr = 0xFF;
            }

            /*Handle under/overflow*/
            if(yn >= img_height)
                continue;
            else if(yn < 0)
                continue;

            /*Get the mixture of the original source and the neightboor pixels in both directions*/
            edgui_color_t c_dest_int = edgui_img_buf_get_px_color(img, xs_int, ys_int, style);

            if(edgui_img_color_format_is_chroma_keyed(img->header.cf)) {
                edgui_color_t ct = EDGUI_COLOR_TRANSP;
                if(c_dest_int.full == ct.full) continue;
            }

            edgui_color_t c_dest_xn = edgui_img_buf_get_px_color(img, xn, ys_int, style);
            edgui_color_t c_dest_yn = edgui_img_buf_get_px_color(img, xs_int, yn, style);
            edgui_color_t x_dest    = edgui_color_mix(c_dest_int, c_dest_xn, xr);
            edgui_color_t y_dest    = edgui_color_mix(c_dest_int, c_dest_yn, yr);
            edgui_color_t color_res = edgui_color_mix(x_dest, y_dest, EDGUI_OPA_50);

            if(x + offset_x >= 0 && x + offset_x < dest_width && y + offset_y >= 0 && y + offset_y < dest_height) {
                /*If the image has no alpha channel just simple set the result color on the canvas*/
                if(edgui_img_color_format_has_alpha(img->header.cf) == false) {
                    edgui_img_buf_set_px_color(&ext_dst->dsc, x + offset_x, y + offset_y, color_res);
                } else {
                    /*Get result pixel opacity*/
                    edgui_opa_t opa_int = edgui_img_buf_get_px_alpha(img, xs_int, ys_int);
                    edgui_opa_t opa_xn  = edgui_img_buf_get_px_alpha(img, xn, ys_int);
                    edgui_opa_t opa_yn  = edgui_img_buf_get_px_alpha(img, xs_int, yn);
                    edgui_opa_t opa_x   = (opa_int * xr + (opa_xn * (255 - xr))) >> 8;
                    edgui_opa_t opa_y   = (opa_int * yr + (opa_yn * (255 - yr))) >> 8;
                    edgui_opa_t opa_res = (opa_x + opa_y) / 2;
                    if(opa_res <= EDGUI_OPA_MIN) continue;

                    edgui_color_t bg_color = edgui_img_buf_get_px_color(&ext_dst->dsc, x + offset_x, y + offset_y, style);

                    /*If the canvas has no alpha but the image has mix the image's color with
                     * canvas*/
                    if(edgui_img_color_format_has_alpha(ext_dst->dsc.header.cf) == false) {
                        if(opa_res < EDGUI_OPA_MAX) color_res = edgui_color_mix(color_res, bg_color, opa_res);
                        edgui_img_buf_set_px_color(&ext_dst->dsc, x + offset_x, y + offset_y, color_res);
                    }
                    /*Both the image and canvas has alpha channel. Some extra calculation is
                       required*/
                    else {
                        edgui_opa_t bg_opa = edgui_img_buf_get_px_alpha(&ext_dst->dsc, x + offset_x, y + offset_y);
                        /* Pick the foreground if it's fully opaque or the Background is fully
                         * transparent*/
                        if(opa_res >= EDGUI_OPA_MAX || bg_opa <= EDGUI_OPA_MIN) {
                            edgui_img_buf_set_px_color(&ext_dst->dsc, x + offset_x, y + offset_y, color_res);
                            edgui_img_buf_set_px_alpha(&ext_dst->dsc, x + offset_x, y + offset_y, opa_res);
                        }
                        /*Opaque background: use simple mix*/
                        else if(bg_opa >= EDGUI_OPA_MAX) {
                            edgui_img_buf_set_px_color(&ext_dst->dsc, x + offset_x, y + offset_y,
                                                    edgui_color_mix(color_res, bg_color, opa_res));
                        }
                        /*Both colors have alpha. Expensive calculation need to be applied*/
                        else {

                            /*Info:
                             * https://en.wikipedia.org/wiki/Alpha_compositing#Analytical_derivation_of_the_over_operator*/
                            edgui_opa_t opa_res_2 = 255 - ((uint16_t)((uint16_t)(255 - opa_res) * (255 - bg_opa)) >> 8);
                            if(opa_res_2 == 0) {
                                opa_res_2 = 1; /*never happens, just to be sure*/
                            }
                            edgui_opa_t ratio = (uint16_t)((uint16_t)opa_res * 255) / opa_res_2;

                            edgui_img_buf_set_px_color(&ext_dst->dsc, x + offset_x, y + offset_y,
                                                    edgui_color_mix(color_res, bg_color, ratio));
                            edgui_img_buf_set_px_alpha(&ext_dst->dsc, x + offset_x, y + offset_y, opa_res_2);
                        }
                    }
                }
            }
        }
    }

    edgui_obj_invalidate(canvas);
}

/**
 * Fill the canvas with color
 * @param canvas pointer to a canvas
 * @param color the background color
 */
void edgui_canvas_fill_bg(edgui_obj_t * canvas, edgui_color_t color)
{
    edgui_img_dsc_t * dsc = edgui_canvas_get_img(canvas);

    uint32_t x = dsc->header.w * dsc->header.h;
    uint32_t y;
    for(y = 0; y < dsc->header.h; y++) {
        for(x = 0; x < dsc->header.w; x++) {
            edgui_img_buf_set_px_color(dsc, x, y, color);
        }
    }
}

/**
 * Draw a rectangle on the canvas
 * @param canvas pointer to a canvas object
 * @param x left coordinate of the rectangle
 * @param y top coordinate of the rectangle
 * @param w width of the rectangle
 * @param h height of the rectangle
 * @param style style of the rectangle (`body` properties are used except `padding`)
 */
void edgui_canvas_draw_rect(edgui_obj_t * canvas, edgui_coord_t x, edgui_coord_t y, edgui_coord_t w, edgui_coord_t h,
                         const edgui_style_t * style)
{
    edgui_img_dsc_t * dsc = edgui_canvas_get_img(canvas);

    /* Create a dummy display to fool the edgui_draw function.
     * It will think it draws to real screen. */
    edgui_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    edgui_area_t coords;
    coords.x1 = x;
    coords.y1 = y;
    coords.x2 = x + w - 1;
    coords.y2 = y + h - 1;

    edgui_disp_t disp;
    memset(&disp, 0, sizeof(edgui_disp_t));

    edgui_disp_buf_t disp_buf;
    edgui_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    edgui_area_copy(&disp_buf.area, &mask);

    edgui_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

#if EDGUI_ANTIALIAS
    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    edgui_color_t ctransp = EDGUI_COLOR_TRANSP;
    if(dsc->header.cf == EDGUI_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
        style->body.main_color.full == ctransp.full &&
        style->body.grad_color.full == ctransp.full)
    {
        disp.driver.antialiasing = 0;
    }
#endif

    edgui_disp_t * refr_ori = edgui_refr_get_disp_refreshing();
    edgui_refr_set_disp_refreshing(&disp);

    edgui_draw_rect(&coords, &mask, style, EDGUI_OPA_COVER);

    edgui_refr_set_disp_refreshing(refr_ori);
}

/**
 * Draw a text on the canvas.
 * @param canvas pointer to a canvas object
 * @param x left coordinate of the text
 * @param y top coordinate of the text
 * @param max_w max width of the text. The text will be wrapped to fit into this size
 * @param style style of the text (`text` properties are used)
 * @param txt text to display
 * @param align align of the text (`EDGUI_LABEL_ALIGN_LEFT/RIGHT/CENTER`)
 */
void edgui_canvas_draw_text(edgui_obj_t * canvas, edgui_coord_t x, edgui_coord_t y, edgui_coord_t max_w, const edgui_style_t * style,
                         const char * txt, edgui_label_align_t align)
{
    edgui_img_dsc_t * dsc = edgui_canvas_get_img(canvas);

    /* Create a dummy display to fool the edgui_draw function.
     * It will think it draws to real screen. */
    edgui_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    edgui_area_t coords;
    coords.x1 = x;
    coords.y1 = y;
    coords.x2 = x + max_w - 1;
    coords.y2 = dsc->header.h - 1;

    edgui_disp_t disp;
    memset(&disp, 0, sizeof(edgui_disp_t));

    edgui_disp_buf_t disp_buf;
    edgui_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    edgui_area_copy(&disp_buf.area, &mask);

    edgui_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

    edgui_disp_t * refr_ori = edgui_refr_get_disp_refreshing();
    edgui_refr_set_disp_refreshing(&disp);

    edgui_txt_flag_t flag;
    switch(align) {
        case EDGUI_LABEL_ALIGN_LEFT: flag = EDGUI_TXT_FLAG_NONE; break;
        case EDGUI_LABEL_ALIGN_RIGHT: flag = EDGUI_TXT_FLAG_RIGHT; break;
        case EDGUI_LABEL_ALIGN_CENTER: flag = EDGUI_TXT_FLAG_CENTER; break;
        default: flag = EDGUI_TXT_FLAG_NONE; break;
    }

    edgui_draw_label(&coords, &mask, style, EDGUI_OPA_COVER, txt, flag, NULL, EDGUI_LABEL_TEXT_SEL_OFF, EDGUI_LABEL_TEXT_SEL_OFF,
                  NULL);

    edgui_refr_set_disp_refreshing(refr_ori);
}

/**
 * Draw an image on the canvas
 * @param canvas pointer to a canvas object
 * @param src image source. Can be a pointer an `edgui_img_dsc_t` variable or a path an image.
 * @param style style of the image (`image` properties are used)
 */
void edgui_canvas_draw_img(edgui_obj_t * canvas, edgui_coord_t x, edgui_coord_t y, const void * src, const edgui_style_t * style)
{
    edgui_img_dsc_t * dsc = edgui_canvas_get_img(canvas);

    /* Create a dummy display to fool the edgui_draw function.
     * It will think it draws to real screen. */
    edgui_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    edgui_img_header_t header;
    edgui_res_t res = edgui_img_decoder_get_info(src, &header);
    if(res != EDGUI_RES_OK) {
        EDGUI_LOG_WARN("edgui_canvas_draw_img: Couldn't get the image data.");
        return;
    }

    edgui_area_t coords;
    coords.x1 = x;
    coords.y1 = y;
    coords.x2 = x + header.w - 1;
    coords.y2 = y + header.h - 1;

    edgui_disp_t disp;
    memset(&disp, 0, sizeof(edgui_disp_t));

    edgui_disp_buf_t disp_buf;
    edgui_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    edgui_area_copy(&disp_buf.area, &mask);

    edgui_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

    edgui_disp_t * refr_ori = edgui_refr_get_disp_refreshing();
    edgui_refr_set_disp_refreshing(&disp);

    edgui_draw_img(&coords, &mask, src, style, EDGUI_OPA_COVER);

    edgui_refr_set_disp_refreshing(refr_ori);
}

/**
 * Draw a line on the canvas
 * @param canvas pointer to a canvas object
 * @param points point of the line
 * @param point_cnt number of points
 * @param style style of the line (`line` properties are used)
 */
void edgui_canvas_draw_line(edgui_obj_t * canvas, const edgui_point_t * points, uint32_t point_cnt, const edgui_style_t * style)
{
    edgui_img_dsc_t * dsc = edgui_canvas_get_img(canvas);

    /* Create a dummy display to fool the edgui_draw function.
     * It will think it draws to real screen. */
    edgui_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    edgui_disp_t disp;
    memset(&disp, 0, sizeof(edgui_disp_t));

    edgui_disp_buf_t disp_buf;
    edgui_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    edgui_area_copy(&disp_buf.area, &mask);

    edgui_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

#if EDGUI_ANTIALIAS
    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    edgui_color_t ctransp = EDGUI_COLOR_TRANSP;
    if(dsc->header.cf == EDGUI_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
        style->body.main_color.full == ctransp.full &&
        style->body.grad_color.full == ctransp.full)
    {
        disp.driver.antialiasing = 0;
    }
#endif

    edgui_disp_t * refr_ori = edgui_refr_get_disp_refreshing();
    edgui_refr_set_disp_refreshing(&disp);

    uint32_t i;
    for(i = 0; i < point_cnt - 1; i++) {
        edgui_draw_line(&points[i], &points[i + 1], &mask, style, EDGUI_OPA_COVER);
    }

    edgui_refr_set_disp_refreshing(refr_ori);
}

/**
 * Draw a polygon on the canvas
 * @param canvas pointer to a canvas object
 * @param points point of the polygon
 * @param point_cnt number of points
 * @param style style of the polygon (`body.main_color` and `body.opa` is used)
 */
void edgui_canvas_draw_polygon(edgui_obj_t * canvas, const edgui_point_t * points, uint32_t point_cnt, const edgui_style_t * style)
{
    edgui_img_dsc_t * dsc = edgui_canvas_get_img(canvas);

    /* Create a dummy display to fool the edgui_draw function.
     * It will think it draws to real screen. */
    edgui_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    edgui_disp_t disp;
    memset(&disp, 0, sizeof(edgui_disp_t));

    edgui_disp_buf_t disp_buf;
    edgui_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    edgui_area_copy(&disp_buf.area, &mask);

    edgui_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

#if EDGUI_ANTIALIAS
    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    edgui_color_t ctransp = EDGUI_COLOR_TRANSP;
    if(dsc->header.cf == EDGUI_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
        style->body.main_color.full == ctransp.full &&
        style->body.grad_color.full == ctransp.full)
    {
        disp.driver.antialiasing = 0;
    }
#endif

    edgui_disp_t * refr_ori = edgui_refr_get_disp_refreshing();
    edgui_refr_set_disp_refreshing(&disp);

    edgui_draw_polygon(points, point_cnt, &mask, style, EDGUI_OPA_COVER);

    edgui_refr_set_disp_refreshing(refr_ori);
}

/**
 * Draw an arc on the canvas
 * @param canvas pointer to a canvas object
 * @param x origo x  of the arc
 * @param y origo y of the arc
 * @param r radius of the arc
 * @param start_angle start angle in degrees
 * @param end_angle end angle in degrees
 * @param style style of the polygon (`body.main_color` and `body.opa` is used)
 */
void edgui_canvas_draw_arc(edgui_obj_t * canvas, edgui_coord_t x, edgui_coord_t y, edgui_coord_t r, int32_t start_angle,
                        int32_t end_angle, const edgui_style_t * style)
{
    edgui_img_dsc_t * dsc = edgui_canvas_get_img(canvas);

    /* Create a dummy display to fool the edgui_draw function.
     * It will think it draws to real screen. */
    edgui_area_t mask;
    mask.x1 = 0;
    mask.x2 = dsc->header.w - 1;
    mask.y1 = 0;
    mask.y2 = dsc->header.h - 1;

    edgui_disp_t disp;
    memset(&disp, 0, sizeof(edgui_disp_t));

    edgui_disp_buf_t disp_buf;
    edgui_disp_buf_init(&disp_buf, (void *)dsc->data, NULL, dsc->header.w * dsc->header.h);
    edgui_area_copy(&disp_buf.area, &mask);

    edgui_disp_drv_init(&disp.driver);

    disp.driver.buffer  = &disp_buf;
    disp.driver.hor_res = dsc->header.w;
    disp.driver.ver_res = dsc->header.h;

#if EDGUI_ANTIALIAS
    /*Disable anti-aliasing if drawing with transparent color to chroma keyed canvas*/
    edgui_color_t ctransp = EDGUI_COLOR_TRANSP;
    if(dsc->header.cf == EDGUI_IMG_CF_TRUE_COLOR_CHROMA_KEYED &&
        style->body.main_color.full == ctransp.full &&
        style->body.grad_color.full == ctransp.full)
    {
        disp.driver.antialiasing = 0;
    }
#endif

    edgui_disp_t * refr_ori = edgui_refr_get_disp_refreshing();
    edgui_refr_set_disp_refreshing(&disp);

    edgui_draw_arc(x, y, r, &mask, start_angle, end_angle, style, EDGUI_OPA_COVER);

    edgui_refr_set_disp_refreshing(refr_ori);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the canvas
 * @param canvas pointer to a canvas object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_canvas_signal(edgui_obj_t * canvas, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(canvas, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_canvas";
    }

    return res;
}

#endif
