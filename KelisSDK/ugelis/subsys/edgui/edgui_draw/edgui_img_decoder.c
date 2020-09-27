/**
 * @file edgui_img_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_img_decoder.h"
#include "../edgui_draw/edgui_draw_img.h"
#include "../edgui_misc/edgui_ll.h"
#include "../edgui_misc/edgui_color.h"
#include "../edgui_misc/edgui_gc.h"

#if defined(EDGUI_GC_INCLUDE)
#include EDGUI_GC_INCLUDE
#endif /* EDGUI_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/
#define CF_BUILT_IN_FIRST EDGUI_IMG_CF_TRUE_COLOR
#define CF_BUILT_IN_LAST EDGUI_IMG_CF_ALPHA_8BIT

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
#if EDGUI_USE_FILESYSTEM
    edgui_fs_file_t * f;
#endif
    edgui_color_t * palette;
} edgui_img_decoder_built_in_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_img_decoder_built_in_line_true_color(edgui_img_decoder_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y,
                                                        edgui_coord_t len, uint8_t * buf);
static edgui_res_t edgui_img_decoder_built_in_line_alpha(edgui_img_decoder_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y,
                                                   edgui_coord_t len, uint8_t * buf);
static edgui_res_t edgui_img_decoder_built_in_line_indexed(edgui_img_decoder_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y,
                                                     edgui_coord_t len, uint8_t * buf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the image decoder module
 * */
void edgui_img_decoder_init(void)
{
    edgui_ll_init(&EDGUI_GC_ROOT(_edgui_img_defoder_ll), sizeof(edgui_img_decoder_t));

    edgui_img_decoder_t * decoder;

    /*Create a decoder for the built in color format*/
    decoder = edgui_img_decoder_create();
    if(decoder == NULL) {
        EDGUI_LOG_WARN("edgui_img_decoder_init: out of memory");
        edgui_mem_assert(decoder);
        return;
    }

    edgui_img_decoder_set_info_cb(decoder, edgui_img_decoder_built_in_info);
    edgui_img_decoder_set_open_cb(decoder, edgui_img_decoder_built_in_open);
    edgui_img_decoder_set_read_line_cb(decoder, edgui_img_decoder_built_in_read_line);
    edgui_img_decoder_set_close_cb(decoder, edgui_img_decoder_built_in_close);
}

/**
 * Get information about an image.
 * Try the created image decoder one by one. Once one is able to get info that info will be used.
 * @param src the image source. E.g. file name or variable.
 * @param header the image info will be stored here
 * @return EDGUI_RES_OK: success; EDGUI_RES_INV: wasn't able to get info about the image
 */
edgui_res_t edgui_img_decoder_get_info(const char * src, edgui_img_header_t * header)
{
    header->always_zero = 0;

    edgui_res_t res = EDGUI_RES_INV;
    edgui_img_decoder_t * d;
    EDGUI_LL_READ(EDGUI_GC_ROOT(_edgui_img_defoder_ll), d)
    {
        res = EDGUI_RES_INV;
        if(d->info_cb) {
            res = d->info_cb(d, src, header);
            if(res == EDGUI_RES_OK) break;
        }
    }

    return res;
}

/**
 * Open an image.
 * Try the created image decoder one by one. Once one is able to open the image that decoder is save in `dsc`
 * @param dsc describe a decoding session. Simply a pointer to an `edgui_img_decoder_dsc_t` variable.
 * @param src the image source. Can be
 *  1) File name: E.g. "S:folder/img1.png" (The drivers needs to registered via `edgui_fs_add_drv()`)
 *  2) Variable: Pointer to an `edgui_img_dsc_t` variable
 *  3) Symbol: E.g. `EDGUI_SYMBOL_OK`
 * @param style the style of the image
 * @return EDGUI_RES_OK: opened the image. `dsc->img_data` and `dsc->header` are set.
 *         EDGUI_RES_INV: none of the registered image decoders were able to open the image.
 */
edgui_res_t edgui_img_decoder_open(edgui_img_decoder_dsc_t * dsc, const void * src, const edgui_style_t * style)
{
    dsc->style     = style;
    dsc->src_type  = edgui_img_src_get_type(src);
    dsc->user_data = NULL;

    if(dsc->src_type == EDGUI_IMG_SRC_FILE) {
        uint16_t fnlen = strlen(src);
        dsc->src = edgui_mem_alloc(fnlen + 1);
        strcpy((char *)dsc->src, src);
    } else {
        dsc->src       = src;
    }

    edgui_res_t res = EDGUI_RES_INV;

    edgui_img_decoder_t * d;
    EDGUI_LL_READ(EDGUI_GC_ROOT(_edgui_img_defoder_ll), d)
    {
        /*Info an Open callbacks are required*/
        if(d->info_cb == NULL || d->open_cb == NULL) continue;

        res = d->info_cb(d, src, &dsc->header);
        if(res != EDGUI_RES_OK) continue;

        dsc->error_msg = NULL;
        dsc->img_data  = NULL;
        dsc->decoder   = d;

        res = d->open_cb(d, dsc);

        /*Opened successfully. It is a good decoder to for this image source*/
        if(res == EDGUI_RES_OK) break;
    }

    if(res == EDGUI_RES_INV) {
        memset(dsc, 0, sizeof(edgui_img_decoder_dsc_t));
    }

    return res;
}

/**
 * Read a line from an opened image
 * @param dsc pointer to `edgui_img_decoder_dsc_t` used in `edgui_img_decoder_open`
 * @param x start X coordinate (from left)
 * @param y start Y coordinate (from top)
 * @param len number of pixels to read
 * @param buf store the data here
 * @return EDGUI_RES_OK: success; EDGUI_RES_INV: an error occurred
 */
edgui_res_t edgui_img_decoder_read_line(edgui_img_decoder_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y, edgui_coord_t len, uint8_t * buf)
{
    edgui_res_t res = EDGUI_RES_INV;
    if(dsc->decoder->read_line_cb) res = dsc->decoder->read_line_cb(dsc->decoder, dsc, x, y, len, buf);

    return res;
}

/**
 * Close a decoding session
 * @param dsc pointer to `edgui_img_decoder_dsc_t` used in `edgui_img_decoder_open`
 */
void edgui_img_decoder_close(edgui_img_decoder_dsc_t * dsc)
{
    if(dsc->decoder) {
        if(dsc->decoder->close_cb) dsc->decoder->close_cb(dsc->decoder, dsc);

        if(dsc->src_type == EDGUI_IMG_SRC_FILE) {
            edgui_mem_free(dsc->src);
            dsc->src = NULL;
        }
    }
}

/**
 * Create a new image decoder
 * @return pointer to the new image decoder
 */
edgui_img_decoder_t * edgui_img_decoder_create(void)
{
    edgui_img_decoder_t * decoder;
    decoder = edgui_ll_ins_head(&EDGUI_GC_ROOT(_edgui_img_defoder_ll));
    edgui_mem_assert(decoder);
    if(decoder == NULL) return NULL;

    memset(decoder, 0, sizeof(edgui_img_decoder_t));

    return decoder;
}

/**
 * Delete an image decoder
 * @param decoder pointer to an image decoder
 */
void edgui_img_decoder_delete(edgui_img_decoder_t * decoder)
{
    edgui_ll_rem(&EDGUI_GC_ROOT(_edgui_img_defoder_ll), decoder);
    edgui_mem_free(decoder);
}

/**
 * Set a callback to get information about the image
 * @param decoder pointer to an image decoder
 * @param info_cb a function to collect info about an image (fill an `edgui_img_header_t` struct)
 */
void edgui_img_decoder_set_info_cb(edgui_img_decoder_t * decoder, edgui_img_decoder_info_f_t info_cb)
{
    decoder->info_cb = info_cb;
}

/**
 * Set a callback to open an image
 * @param decoder pointer to an image decoder
 * @param open_cb a function to open an image
 */
void edgui_img_decoder_set_open_cb(edgui_img_decoder_t * decoder, edgui_img_decoder_open_f_t open_cb)
{
    decoder->open_cb = open_cb;
}

/**
 * Set a callback to a decoded line of an image
 * @param decoder pointer to an image decoder
 * @param read_line_cb a function to read a line of an image
 */
void edgui_img_decoder_set_read_line_cb(edgui_img_decoder_t * decoder, edgui_img_decoder_read_line_f_t read_line_cb)
{
    decoder->read_line_cb = read_line_cb;
}

/**
 * Set a callback to close a decoding session. E.g. close files and free other resources.
 * @param decoder pointer to an image decoder
 * @param close_cb a function to close a decoding session
 */
void edgui_img_decoder_set_close_cb(edgui_img_decoder_t * decoder, edgui_img_decoder_close_f_t close_cb)
{
    decoder->close_cb = close_cb;
}

/**
 * Get info about a built-in image
 * @param decoder the decoder where this function belongs
 * @param src the image source: pointer to an `edgui_img_dsc_t` variable, a file path or a symbol
 * @param header store the image data here
 * @return EDGUI_RES_OK: the info is successfully stored in `header`; EDGUI_RES_INV: unknown format or other error.
 */
edgui_res_t edgui_img_decoder_built_in_info(edgui_img_decoder_t * decoder, const void * src, edgui_img_header_t * header)
{
    (void)decoder; /*Unused*/

    edgui_img_src_t src_type = edgui_img_src_get_type(src);
    if(src_type == EDGUI_IMG_SRC_VARIABLE) {
        edgui_img_cf_t cf = ((edgui_img_dsc_t *)src)->header.cf;
        if(cf < CF_BUILT_IN_FIRST || cf > CF_BUILT_IN_LAST) return EDGUI_RES_INV;

        header->w  = ((edgui_img_dsc_t *)src)->header.w;
        header->h  = ((edgui_img_dsc_t *)src)->header.h;
        header->cf = ((edgui_img_dsc_t *)src)->header.cf;
    }
#if EDGUI_USE_FILESYSTEM
    else if(src_type == EDGUI_IMG_SRC_FILE) {
        edgui_fs_file_t file;
        edgui_fs_res_t res;
        uint32_t rn;
        res = edgui_fs_open(&file, src, EDGUI_FS_MODE_RD);
        if(res == EDGUI_FS_RES_OK) {
            res = edgui_fs_read(&file, header, sizeof(edgui_img_header_t), &rn);
            edgui_fs_close(&file);
        }

        if(header->cf < CF_BUILT_IN_FIRST || header->cf > CF_BUILT_IN_LAST) return EDGUI_RES_INV;

    }
#endif
    else if(src_type == EDGUI_IMG_SRC_SYMBOL) {
        /*The size depend on the font but it is unknown here. It should be handled outside of the
         * function*/
        header->w = 1;
        header->h = 1;
        /* Symbols always have transparent parts. Important because of cover check in the design
         * function. The actual value doesn't matter because edgui_draw_label will draw it*/
        header->cf = EDGUI_IMG_CF_ALPHA_1BIT;
    } else {
        EDGUI_LOG_WARN("Image get info found unknown src type");
        return EDGUI_RES_INV;
    }
    return EDGUI_RES_OK;
}

/**
 * Open a built in image
 * @param decoder the decoder where this function belongs
 * @param dsc pointer to decoder descriptor. `src`, `style` are already initialized in it.
 * @return EDGUI_RES_OK: the info is successfully stored in `header`; EDGUI_RES_INV: unknown format or other error.
 */
edgui_res_t edgui_img_decoder_built_in_open(edgui_img_decoder_t * decoder, edgui_img_decoder_dsc_t * dsc)
{
    /*Open the file if it's a file*/
    if(dsc->src_type == EDGUI_IMG_SRC_FILE) {
#if EDGUI_USE_FILESYSTEM

        /*Support only "*.bin" files*/
        if(strcmp(edgui_fs_get_ext(dsc->src), "bin")) return EDGUI_RES_INV;

        edgui_fs_file_t f;
        edgui_fs_res_t res = edgui_fs_open(&f, dsc->src, EDGUI_FS_MODE_RD);
        if(res != EDGUI_FS_RES_OK) {
            EDGUI_LOG_WARN("Built-in image decoder can't open the file");
            return EDGUI_RES_INV;
        }

        /*If the file was open successfully save the file descriptor*/
        if(dsc->user_data == NULL) {
            dsc->user_data = edgui_mem_alloc(sizeof(edgui_img_decoder_built_in_data_t));
            if(dsc->user_data == NULL) {
                EDGUI_LOG_ERROR("img_decoder_built_in_open: out of memory");
                edgui_mem_assert(dsc->user_data);
            }
            memset(dsc->user_data, 0, sizeof(edgui_img_decoder_built_in_data_t));
        }

        edgui_img_decoder_built_in_data_t * user_data = dsc->user_data;
        user_data->f                               = edgui_mem_alloc(sizeof(f));
        if(user_data->f == NULL) {
            EDGUI_LOG_ERROR("img_decoder_built_in_open: out of memory");
            edgui_mem_assert(user_data->f);
        }

        memcpy(user_data->f, &f, sizeof(f));

#else
        EDGUI_LOG_WARN("Image built-in decoder cannot read file because EDGUI_USE_FILESYSTEM = 0");
        return EDGUI_RES_INV;
#endif
    }

    edgui_img_cf_t cf = dsc->header.cf;
    /*Process true color formats*/
    if(cf == EDGUI_IMG_CF_TRUE_COLOR || cf == EDGUI_IMG_CF_TRUE_COLOR_ALPHA || cf == EDGUI_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        if(dsc->src_type == EDGUI_IMG_SRC_VARIABLE) {
            /* In case of uncompressed formats the image stored in the ROM/RAM.
             * So simply give its pointer*/
            dsc->img_data = ((edgui_img_dsc_t *)dsc->src)->data;
            return EDGUI_RES_OK;
        } else {
            /*If it's a file it need to be read line by line later*/
            dsc->img_data = NULL;
            return EDGUI_RES_OK;
        }
    }
    /*Process indexed images. Build a palette*/
    else if(cf == EDGUI_IMG_CF_INDEXED_1BIT || cf == EDGUI_IMG_CF_INDEXED_2BIT || cf == EDGUI_IMG_CF_INDEXED_4BIT ||
            cf == EDGUI_IMG_CF_INDEXED_8BIT) {

#if EDGUI_IMG_CF_INDEXED
        uint8_t px_size       = edgui_img_color_format_get_px_size(cf);
        uint32_t palette_size = 1 << px_size;

        /*Allocate the palette*/
        if(dsc->user_data == NULL) {
            dsc->user_data = edgui_mem_alloc(sizeof(edgui_img_decoder_built_in_data_t));
            if(dsc->user_data == NULL) {
                EDGUI_LOG_ERROR("img_decoder_built_in_open: out of memory");
                edgui_mem_assert(dsc->user_data);
            }
            memset(dsc->user_data, 0, sizeof(edgui_img_decoder_built_in_data_t));
        }

        edgui_img_decoder_built_in_data_t * user_data = dsc->user_data;
        user_data->palette                         = edgui_mem_alloc(palette_size * sizeof(edgui_color_t));
        if(user_data->palette == NULL) {
            EDGUI_LOG_ERROR("img_decoder_built_in_open: out of memory");
#if EDGUI_USE_FILESYSTEM
            edgui_mem_assert(user_data->f);
#endif
        }

        if(dsc->src_type == EDGUI_IMG_SRC_FILE) {
            /*Read the palette from file*/
#if EDGUI_USE_FILESYSTEM
            edgui_fs_seek(user_data->f, 4); /*Skip the header*/
            edgui_fs_read(user_data->f, user_data->palette, palette_size * sizeof(edgui_color_t), NULL);
#else
            EDGUI_LOG_WARN("Image built-in decoder can read the palette because EDGUI_USE_FILESYSTEM = 0");
            return EDGUI_RES_INV;
#endif
        } else {
            /*The palette begins in the beginning of the image data. Just point to it.*/
            edgui_color32_t * palette_p = (edgui_color32_t *)((edgui_img_dsc_t *)dsc->src)->data;

            uint32_t i;
            for(i = 0; i < palette_size; i++) {
                user_data->palette[i] = edgui_color_make(palette_p[i].ch.red, palette_p[i].ch.green, palette_p[i].ch.blue);
            }
        }

        dsc->img_data = NULL;
        return EDGUI_RES_OK;
#else
        EDGUI_LOG_WARN("Indexed (palette) images are not enabled in edgui_conf.h. See EDGUI_IMG_CF_INDEXED");
        return EDGUI_RES_INV;
#endif
    }
    /*Alpha indexed images. */
    else if(cf == EDGUI_IMG_CF_ALPHA_1BIT || cf == EDGUI_IMG_CF_ALPHA_2BIT || cf == EDGUI_IMG_CF_ALPHA_4BIT ||
            cf == EDGUI_IMG_CF_ALPHA_8BIT) {
#if EDGUI_IMG_CF_ALPHA
        dsc->img_data = NULL;
        return EDGUI_RES_OK; /*Nothing to process*/
#else
        EDGUI_LOG_WARN("Alpha indexed images are not enabled in edgui_conf.h. See EDGUI_IMG_CF_ALPHA");
        return EDGUI_RES_INV;
#endif
    }
    /*Unknown format. Can't decode it.*/
    else {
        /*Free the potentially allocated memories*/
        edgui_img_decoder_built_in_close(decoder, dsc);

        EDGUI_LOG_WARN("Image decoder open: unknown color format")
        return EDGUI_RES_INV;
    }
}

/**
 * Decode `len` pixels starting from the given `x`, `y` coordinates and store them in `buf`.
 * Required only if the "open" function can't return with the whole decoded pixel array.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 * @param x start x coordinate
 * @param y start y coordinate
 * @param len number of pixels to decode
 * @param buf a buffer to store the decoded pixels
 * @return EDGUI_RES_OK: ok; EDGUI_RES_INV: failed
 */
edgui_res_t edgui_img_decoder_built_in_read_line(edgui_img_decoder_t * decoder, edgui_img_decoder_dsc_t * dsc, edgui_coord_t x,
                                                  edgui_coord_t y, edgui_coord_t len, uint8_t * buf)
{
    (void)decoder; /*Unused*/

    edgui_res_t res = EDGUI_RES_INV;

    if(dsc->header.cf == EDGUI_IMG_CF_TRUE_COLOR || dsc->header.cf == EDGUI_IMG_CF_TRUE_COLOR_ALPHA ||
       dsc->header.cf == EDGUI_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        /* For TRUE_COLOR images read line required only for files.
         * For variables the image data was returned in `open`*/
        if(dsc->src_type == EDGUI_IMG_SRC_FILE) {
            res = edgui_img_decoder_built_in_line_true_color(dsc, x, y, len, buf);
        }
    } else if(dsc->header.cf == EDGUI_IMG_CF_ALPHA_1BIT || dsc->header.cf == EDGUI_IMG_CF_ALPHA_2BIT ||
              dsc->header.cf == EDGUI_IMG_CF_ALPHA_4BIT || dsc->header.cf == EDGUI_IMG_CF_ALPHA_8BIT) {

        res = edgui_img_decoder_built_in_line_alpha(dsc, x, y, len, buf);
    } else if(dsc->header.cf == EDGUI_IMG_CF_INDEXED_1BIT || dsc->header.cf == EDGUI_IMG_CF_INDEXED_2BIT ||
              dsc->header.cf == EDGUI_IMG_CF_INDEXED_4BIT || dsc->header.cf == EDGUI_IMG_CF_INDEXED_8BIT) {
        res = edgui_img_decoder_built_in_line_indexed(dsc, x, y, len, buf);
    } else {
        EDGUI_LOG_WARN("Built-in image decoder read not supports the color format");
        return EDGUI_RES_INV;
    }

    return res;
}

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
void edgui_img_decoder_built_in_close(edgui_img_decoder_t * decoder, edgui_img_decoder_dsc_t * dsc)
{
    (void)decoder; /*Unused*/

    edgui_img_decoder_built_in_data_t * user_data = dsc->user_data;
    if(user_data) {
#if EDGUI_USE_FILESYSTEM
        if(user_data->f) {
            edgui_fs_close(user_data->f);
            edgui_mem_free(user_data->f);
        }
#endif
        if(user_data->palette) edgui_mem_free(user_data->palette);

        edgui_mem_free(user_data);

        dsc->user_data = NULL;
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static edgui_res_t edgui_img_decoder_built_in_line_true_color(edgui_img_decoder_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y,
                                                        edgui_coord_t len, uint8_t * buf)
{
#if EDGUI_USE_FILESYSTEM
    edgui_img_decoder_built_in_data_t * user_data = dsc->user_data;
    edgui_fs_res_t res;
    uint8_t px_size = edgui_img_color_format_get_px_size(dsc->header.cf);

    uint32_t pos = ((y * dsc->header.w + x) * px_size) >> 3;
    pos += 4; /*Skip the header*/
    res = edgui_fs_seek(user_data->f, pos);
    if(res != EDGUI_FS_RES_OK) {
        EDGUI_LOG_WARN("Built-in image decoder seek failed");
        return EDGUI_RES_INV;
    }
    uint32_t btr = len * (px_size >> 3);
    uint32_t br  = 0;
    edgui_fs_read(user_data->f, buf, btr, &br);
    if(res != EDGUI_FS_RES_OK || btr != br) {
        EDGUI_LOG_WARN("Built-in image decoder read failed");
        return EDGUI_RES_INV;
    }

    return EDGUI_RES_OK;
#else
    EDGUI_LOG_WARN("Image built-in decoder cannot read file because EDGUI_USE_FILESYSTEM = 0");
    return EDGUI_RES_INV;
#endif
}

static edgui_res_t edgui_img_decoder_built_in_line_alpha(edgui_img_decoder_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y,
                                                   edgui_coord_t len, uint8_t * buf)
{

#if EDGUI_IMG_CF_ALPHA
    const edgui_opa_t alpha1_opa_table[2]  = {0, 255};          /*Opacity mapping with bpp = 1 (Just for compatibility)*/
    const edgui_opa_t alpha2_opa_table[4]  = {0, 85, 170, 255}; /*Opacity mapping with bpp = 2*/
    const edgui_opa_t alpha4_opa_table[16] = {0,  17, 34,  51,  /*Opacity mapping with bpp = 4*/
                                           68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};

    /*Simply fill the buffer with the color. Later only the alpha value will be modified.*/
    edgui_color_t bg_color = dsc->style->image.color;
    edgui_coord_t i;
    for(i = 0; i < len; i++) {
#if EDGUI_COLOR_DEPTH == 8 || EDGUI_COLOR_DEPTH == 1
        buf[i * EDGUI_IMG_PX_SIZE_ALPHA_BYTE] = bg_color.full;
#elif EDGUI_COLOR_DEPTH == 16
        /*Because of Alpha byte 16 bit color can start on odd address which can cause crash*/
        buf[i * EDGUI_IMG_PX_SIZE_ALPHA_BYTE] = bg_color.full & 0xFF;
        buf[i * EDGUI_IMG_PX_SIZE_ALPHA_BYTE + 1] = (bg_color.full >> 8) & 0xFF;
#elif EDGUI_COLOR_DEPTH == 32
        *((uint32_t *)&buf[i * EDGUI_IMG_PX_SIZE_ALPHA_BYTE]) = bg_color.full;
#else
#error "Invalid EDGUI_COLOR_DEPTH. Check it in edgui_conf.h"
#endif
    }

    const edgui_opa_t * opa_table = NULL;
    uint8_t px_size            = edgui_img_color_format_get_px_size(dsc->header.cf);
    uint16_t mask              = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    edgui_coord_t w = 0;
    uint32_t ofs = 0;
    int8_t pos   = 0;
    switch(dsc->header.cf) {
        case EDGUI_IMG_CF_ALPHA_1BIT:
            w = (dsc->header.w >> 3); /*E.g. w = 20 -> w = 2 + 1*/
            if(dsc->header.w & 0x7) w++;
            ofs += w * y + (x >> 3); /*First pixel*/
            pos       = 7 - (x & 0x7);
            opa_table = alpha1_opa_table;
            break;
        case EDGUI_IMG_CF_ALPHA_2BIT:
            w = (dsc->header.w >> 2); /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            if(dsc->header.w & 0x3) w++;
            ofs += w * y + (x >> 2); /*First pixel*/
            pos       = 6 - ((x & 0x3) * 2);
            opa_table = alpha2_opa_table;
            break;
        case EDGUI_IMG_CF_ALPHA_4BIT:
            w = (dsc->header.w >> 1); /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            if(dsc->header.w & 0x1) w++;
            ofs += w * y + (x >> 1); /*First pixel*/
            pos       = 4 - ((x & 0x1) * 4);
            opa_table = alpha4_opa_table;
            break;
        case EDGUI_IMG_CF_ALPHA_8BIT:
            w = dsc->header.w; /*E.g. x = 7 -> w = 7 (bytes)*/
            ofs += w * y + x;  /*First pixel*/
            pos = 0;
            break;
    }

#if EDGUI_USE_FILESYSTEM
    edgui_img_decoder_built_in_data_t * user_data = dsc->user_data;
    uint8_t fs_buf[EDGUI_HOR_RES_MAX];
#endif

    const uint8_t * data_tmp = NULL;
    if(dsc->src_type == EDGUI_IMG_SRC_VARIABLE) {
        const edgui_img_dsc_t * img_dsc = dsc->src;

        data_tmp = img_dsc->data + ofs;
    } else {
#if EDGUI_USE_FILESYSTEM
        edgui_fs_seek(user_data->f, ofs + 4); /*+4 to skip the header*/
        edgui_fs_read(user_data->f, fs_buf, w, NULL);
        data_tmp = fs_buf;
#else
        EDGUI_LOG_WARN("Image built-in alpha line reader can't read file because EDGUI_USE_FILESYSTEM = 0");
        data_tmp = NULL; /*To avoid warnings*/
        return EDGUI_RES_INV;
#endif
    }

    uint8_t byte_act = 0;
    uint8_t val_act;
    for(i = 0; i < len; i++) {
        val_act = (data_tmp[byte_act] & (mask << pos)) >> pos;

        buf[i * EDGUI_IMG_PX_SIZE_ALPHA_BYTE + EDGUI_IMG_PX_SIZE_ALPHA_BYTE - 1] =
            dsc->header.cf == EDGUI_IMG_CF_ALPHA_8BIT ? val_act : opa_table[val_act];

        pos -= px_size;
        if(pos < 0) {
            pos = 8 - px_size;
            data_tmp++;
        }
    }

    return EDGUI_RES_OK;

#else
    EDGUI_LOG_WARN("Image built-in alpha line reader failed because EDGUI_IMG_CF_ALPHA is 0 in edgui_conf.h");
    return EDGUI_RES_INV;
#endif
}

static edgui_res_t edgui_img_decoder_built_in_line_indexed(edgui_img_decoder_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y,
                                                     edgui_coord_t len, uint8_t * buf)
{

#if EDGUI_IMG_CF_INDEXED
    uint8_t px_size = edgui_img_color_format_get_px_size(dsc->header.cf);
    uint16_t mask   = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    edgui_coord_t w = 0;
    int8_t pos   = 0;
    uint32_t ofs = 0;
    switch(dsc->header.cf) {
        case EDGUI_IMG_CF_INDEXED_1BIT:
            w = (dsc->header.w >> 3); /*E.g. w = 20 -> w = 2 + 1*/
            if(dsc->header.w & 0x7) w++;
            ofs += w * y + (x >> 3); /*First pixel*/
            ofs += 8;                /*Skip the palette*/
            pos = 7 - (x & 0x7);
            break;
        case EDGUI_IMG_CF_INDEXED_2BIT:
            w = (dsc->header.w >> 2); /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            if(dsc->header.w & 0x3) w++;
            ofs += w * y + (x >> 2); /*First pixel*/
            ofs += 16;               /*Skip the palette*/
            pos = 6 - ((x & 0x3) * 2);
            break;
        case EDGUI_IMG_CF_INDEXED_4BIT:
            w = (dsc->header.w >> 1); /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            if(dsc->header.w & 0x1) w++;
            ofs += w * y + (x >> 1); /*First pixel*/
            ofs += 64;               /*Skip the palette*/
            pos = 4 - ((x & 0x1) * 4);
            break;
        case EDGUI_IMG_CF_INDEXED_8BIT:
            w = dsc->header.w; /*E.g. x = 7 -> w = 7 (bytes)*/
            ofs += w * y + x;  /*First pixel*/
            ofs += 1024;       /*Skip the palette*/
            pos = 0;
            break;
    }

    edgui_img_decoder_built_in_data_t * user_data = dsc->user_data;

#if EDGUI_USE_FILESYSTEM
    uint8_t fs_buf[EDGUI_HOR_RES_MAX];
#endif
    const uint8_t * data_tmp = NULL;
    if(dsc->src_type == EDGUI_IMG_SRC_VARIABLE) {
        const edgui_img_dsc_t * img_dsc = dsc->src;
        data_tmp                     = img_dsc->data + ofs;
    } else {
#if EDGUI_USE_FILESYSTEM
        edgui_fs_seek(user_data->f, ofs + 4); /*+4 to skip the header*/
        edgui_fs_read(user_data->f, fs_buf, w, NULL);
        data_tmp = fs_buf;
#else
        EDGUI_LOG_WARN("Image built-in indexed line reader can't read file because EDGUI_USE_FILESYSTEM = 0");
        data_tmp = NULL; /*To avoid warnings*/
        return EDGUI_RES_INV;
#endif
    }

    uint8_t byte_act = 0;
    uint8_t val_act;
    edgui_coord_t i;
    edgui_color_t * cbuf = (edgui_color_t *)buf;
    for(i = 0; i < len; i++) {
        val_act = (data_tmp[byte_act] & (mask << pos)) >> pos;
        cbuf[i] = user_data->palette[val_act];

        pos -= px_size;
        if(pos < 0) {
            pos = 8 - px_size;
            data_tmp++;
        }
    }

    return EDGUI_RES_OK;
#else
    EDGUI_LOG_WARN("Image built-in indexed line reader failed because EDGUI_IMG_CF_INDEXED is 0 in edgui_conf.h");
    return EDGUI_RES_INV;
#endif
}
