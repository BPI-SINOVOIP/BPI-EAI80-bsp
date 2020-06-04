/**
 * @file edgui_img_decoder.h
 *
 */

#ifndef EDGUI_IMG_DEOCER_H
#define EDGUI_IMG_DEOCER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui_conf.h"
#else
#include "../edgui/edgui_conf.h"
#endif

#include <stdint.h>
#include "../edgui_misc/edgui_fs.h"
#include "../edgui_misc/edgui_types.h"
#include "../edgui_misc/edgui_area.h"
#include "../edgui_core/edgui_style.h"

/*********************
 *      DEFINES
 *********************/
/*If image pixels contains alpha we need to know how much byte is a pixel*/
#if EDGUI_COLOR_DEPTH == 1 || EDGUI_COLOR_DEPTH == 8
#define EDGUI_IMG_PX_SIZE_ALPHA_BYTE 2
#elif EDGUI_COLOR_DEPTH == 16
#define EDGUI_IMG_PX_SIZE_ALPHA_BYTE 3
#elif EDGUI_COLOR_DEPTH == 32
#define EDGUI_IMG_PX_SIZE_ALPHA_BYTE 4
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Source of image. */
enum {
    EDGUI_IMG_SRC_VARIABLE, /** Binary/C variable */
    EDGUI_IMG_SRC_FILE, /** File in filesystem */
    EDGUI_IMG_SRC_SYMBOL, /** Symbol (@ref edgui_symbol_def.h) */
    EDGUI_IMG_SRC_UNKNOWN, /** Unknown source */
};

typedef uint8_t edgui_img_src_t;
/**
 * edgui image header
 */
typedef struct
{

    /* The first 8 bit is very important to distinguish the different source types.
     * For more info see `edgui_img_get_src_type()` in edgui_img.c */
    uint32_t cf : 5;          /* Color format: See `edgui_img_color_format_t`*/
    uint32_t always_zero : 3; /*It the upper bits of the first byte. Always zero to look like a
                                 non-printable character*/

    uint32_t reserved : 2; /*Reserved to be used later*/

    uint32_t w : 11; /*Width of the image map*/
    uint32_t h : 11; /*Height of the image map*/
} edgui_img_header_t;

/*Image color format*/
enum {
    EDGUI_IMG_CF_UNKNOWN = 0,

    EDGUI_IMG_CF_RAW,              /**< Contains the file as it is. Needs custom decoder function*/
    EDGUI_IMG_CF_RAW_ALPHA,        /**< Contains the file as it is. The image has alpha. Needs custom decoder
                                   function*/
    EDGUI_IMG_CF_RAW_CHROMA_KEYED, /**< Contains the file as it is. The image is chroma keyed. Needs
                                   custom decoder function*/

    EDGUI_IMG_CF_TRUE_COLOR,              /**< Color format and depth should match with EDGUI_COLOR settings*/
    EDGUI_IMG_CF_TRUE_COLOR_ALPHA,        /**< Same as `EDGUI_IMG_CF_TRUE_COLOR` but every pixel has an alpha byte*/
    EDGUI_IMG_CF_TRUE_COLOR_CHROMA_KEYED, /**< Same as `EDGUI_IMG_CF_TRUE_COLOR` but EDGUI_COLOR_TRANSP pixels
                                          will be transparent*/

    EDGUI_IMG_CF_INDEXED_1BIT, /**< Can have 2 different colors in a palette (always chroma keyed)*/
    EDGUI_IMG_CF_INDEXED_2BIT, /**< Can have 4 different colors in a palette (always chroma keyed)*/
    EDGUI_IMG_CF_INDEXED_4BIT, /**< Can have 16 different colors in a palette (always chroma keyed)*/
    EDGUI_IMG_CF_INDEXED_8BIT, /**< Can have 256 different colors in a palette (always chroma keyed)*/

    EDGUI_IMG_CF_ALPHA_1BIT, /**< Can have one color and it can be drawn or not*/
    EDGUI_IMG_CF_ALPHA_2BIT, /**< Can have one color but 4 different alpha value*/
    EDGUI_IMG_CF_ALPHA_4BIT, /**< Can have one color but 16 different alpha value*/
    EDGUI_IMG_CF_ALPHA_8BIT, /**< Can have one color but 256 different alpha value*/

    EDGUI_IMG_CF_RESERVED_15,              /**< Reserved for further use. */
    EDGUI_IMG_CF_RESERVED_16,              /**< Reserved for further use. */
    EDGUI_IMG_CF_RESERVED_17,              /**< Reserved for further use. */
    EDGUI_IMG_CF_RESERVED_18,              /**< Reserved for further use. */
    EDGUI_IMG_CF_RESERVED_19,              /**< Reserved for further use. */
    EDGUI_IMG_CF_RESERVED_20,              /**< Reserved for further use. */
    EDGUI_IMG_CF_RESERVED_21,              /**< Reserved for further use. */
    EDGUI_IMG_CF_RESERVED_22,              /**< Reserved for further use. */
    EDGUI_IMG_CF_RESERVED_23,              /**< Reserved for further use. */

    EDGUI_IMG_CF_USER_ENCODED_0,          /**< User holder encoding format. */
    EDGUI_IMG_CF_USER_ENCODED_1,          /**< User holder encoding format. */
    EDGUI_IMG_CF_USER_ENCODED_2,          /**< User holder encoding format. */
    EDGUI_IMG_CF_USER_ENCODED_3,          /**< User holder encoding format. */
    EDGUI_IMG_CF_USER_ENCODED_4,          /**< User holder encoding format. */
    EDGUI_IMG_CF_USER_ENCODED_5,          /**< User holder encoding format. */
    EDGUI_IMG_CF_USER_ENCODED_6,          /**< User holder encoding format. */
    EDGUI_IMG_CF_USER_ENCODED_7,          /**< User holder encoding format. */
};
typedef uint8_t edgui_img_cf_t;

/** Image header it is compatible with
 * the result from image converter utility*/
typedef struct
{
    edgui_img_header_t header;
    uint32_t data_size;
    const uint8_t * data;
} edgui_img_dsc_t;

/* Decoder function definitions */

struct _edgui_img_decoder;
struct _edgui_img_decoder_dsc;

/**
 * Get info from an image and store in the `header`
 * @param src the image source. Can be a pointer to a C array or a file name (Use
 * `edgui_img_src_get_type` to determine the type)
 * @param header store the info here
 * @return EDGUI_RES_OK: info written correctly; EDGUI_RES_INV: failed
 */
typedef edgui_res_t (*edgui_img_decoder_info_f_t)(struct _edgui_img_decoder * decoder, const void * src,
                                            edgui_img_header_t * header);

/**
 * Open an image for decoding. Prepare it as it is required to read it later
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor. `src`, `style` are already initialized in it.
 */
typedef edgui_res_t (*edgui_img_decoder_open_f_t)(struct _edgui_img_decoder * decoder, struct _edgui_img_decoder_dsc * dsc);

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
typedef edgui_res_t (*edgui_img_decoder_read_line_f_t)(struct _edgui_img_decoder * decoder, struct _edgui_img_decoder_dsc * dsc,
                                                 edgui_coord_t x, edgui_coord_t y, edgui_coord_t len, uint8_t * buf);

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
typedef void (*edgui_img_decoder_close_f_t)(struct _edgui_img_decoder * decoder, struct _edgui_img_decoder_dsc * dsc);

typedef struct _edgui_img_decoder
{
    edgui_img_decoder_info_f_t info_cb;
    edgui_img_decoder_open_f_t open_cb;
    edgui_img_decoder_read_line_f_t read_line_cb;
    edgui_img_decoder_close_f_t close_cb;

#if EDGUI_USE_USER_DATA
    edgui_img_decoder_user_data_t user_data;
#endif
} edgui_img_decoder_t;

/**Describe an image decoding session. Stores data about the decoding*/
typedef struct _edgui_img_decoder_dsc
{
    /**The decoder which was able to open the image source*/
    edgui_img_decoder_t * decoder;

    /**The image source. A file path like "S:my_img.png" or pointer to an `edgui_img_dsc_t` variable*/
    const void * src;

    /**Style to draw the image.*/
    const edgui_style_t * style;

    /**Type of the source: file or variable. Can be set in `open` function if required*/
    edgui_img_src_t src_type;

    /**Info about the opened image: color format, size, etc. MUST be set in `open` function*/
    edgui_img_header_t header;

    /** Pointer to a buffer where the image's data (pixels) are stored in a decoded, plain format.
     *  MUST be set in `open` function*/
    const uint8_t * img_data;

    /** How much time did it take to open the image. [ms]
     *  If not set `edgui_img_cache` will measure and set the time to open*/
    uint32_t time_to_open;

    /**A text to display instead of the image when the image can't be opened.
     * Can be set in `open` function or set NULL. */
    const char * error_msg;

    /**Store any custom data here is required*/
    void * user_data;
} edgui_img_decoder_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the image decoder module
 */
void edgui_img_decoder_init(void);

/**
 * Get information about an image.
 * Try the created image decoder one by one. Once one is able to get info that info will be used.
 * @param src the image source. Can be
 *  1) File name: E.g. "S:folder/img1.png" (The drivers needs to registered via `edgui_fs_add_drv()`)
 *  2) Variable: Pointer to an `edgui_img_dsc_t` variable
 *  3) Symbol: E.g. `EDGUI_SYMBOL_OK`
 * @param header the image info will be stored here
 * @return EDGUI_RES_OK: success; EDGUI_RES_INV: wasn't able to get info about the image
 */
edgui_res_t edgui_img_decoder_get_info(const char * src, edgui_img_header_t * header);

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
edgui_res_t edgui_img_decoder_open(edgui_img_decoder_dsc_t * dsc, const void * src, const edgui_style_t * style);

/**
 * Read a line from an opened image
 * @param dsc pointer to `edgui_img_decoder_dsc_t` used in `edgui_img_decoder_open`
 * @param x start X coordinate (from left)
 * @param y start Y coordinate (from top)
 * @param len number of pixels to read
 * @param buf store the data here
 * @return EDGUI_RES_OK: success; EDGUI_RES_INV: an error occurred
 */
edgui_res_t edgui_img_decoder_read_line(edgui_img_decoder_dsc_t * dsc, edgui_coord_t x, edgui_coord_t y, edgui_coord_t len,
                                  uint8_t * buf);

/**
 * Close a decoding session
 * @param dsc pointer to `edgui_img_decoder_dsc_t` used in `edgui_img_decoder_open`
 */
void edgui_img_decoder_close(edgui_img_decoder_dsc_t * dsc);

/**
 * Create a new image decoder
 * @return pointer to the new image decoder
 */
edgui_img_decoder_t * edgui_img_decoder_create(void);

/**
 * Delete an image decoder
 * @param decoder pointer to an image decoder
 */
void edgui_img_decoder_delete(edgui_img_decoder_t * decoder);

/**
 * Set a callback to get information about the image
 * @param decoder pointer to an image decoder
 * @param info_cb a function to collect info about an image (fill an `edgui_img_header_t` struct)
 */
void edgui_img_decoder_set_info_cb(edgui_img_decoder_t * decoder, edgui_img_decoder_info_f_t info_cb);

/**
 * Set a callback to open an image
 * @param decoder pointer to an image decoder
 * @param open_cb a function to open an image
 */
void edgui_img_decoder_set_open_cb(edgui_img_decoder_t * decoder, edgui_img_decoder_open_f_t open_cb);

/**
 * Set a callback to a decoded line of an image
 * @param decoder pointer to an image decoder
 * @param read_line_cb a function to read a line of an image
 */
void edgui_img_decoder_set_read_line_cb(edgui_img_decoder_t * decoder, edgui_img_decoder_read_line_f_t read_line_cb);

/**
 * Set a callback to close a decoding session. E.g. close files and free other resources.
 * @param decoder pointer to an image decoder
 * @param close_cb a function to close a decoding session
 */
void edgui_img_decoder_set_close_cb(edgui_img_decoder_t * decoder, edgui_img_decoder_close_f_t close_cb);



/**
 * Get info about a built-in image
 * @param decoder the decoder where this function belongs
 * @param src the image source: pointer to an `edgui_img_dsc_t` variable, a file path or a symbol
 * @param header store the image data here
 * @return EDGUI_RES_OK: the info is successfully stored in `header`; EDGUI_RES_INV: unknown format or other error.
 */
edgui_res_t edgui_img_decoder_built_in_info(edgui_img_decoder_t * decoder, const void * src, edgui_img_header_t * header);

/**
 * Open a built in image
 * @param decoder the decoder where this function belongs
 * @param dsc pointer to decoder descriptor. `src`, `style` are already initialized in it.
 * @return EDGUI_RES_OK: the info is successfully stored in `header`; EDGUI_RES_INV: unknown format or other error.
 */
edgui_res_t edgui_img_decoder_built_in_open(edgui_img_decoder_t * decoder, edgui_img_decoder_dsc_t * dsc);

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
                                                  edgui_coord_t y, edgui_coord_t len, uint8_t * buf);

/**
 * Close the pending decoding. Free resources etc.
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 */
void edgui_img_decoder_built_in_close(edgui_img_decoder_t * decoder, edgui_img_decoder_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEMPL_H*/
