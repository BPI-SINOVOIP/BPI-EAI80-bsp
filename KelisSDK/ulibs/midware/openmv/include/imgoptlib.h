#ifndef __IMAGEOPTLIB_H__
#define __IMAGEOPTLIB_H__  

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//#define ARMC4_SIMULATOR
	
/* This type MUST be 8-bit */
typedef uint8_t BYTE;

/* These types MUST be 16-bit */
typedef int16_t SHORT;
typedef uint16_t WORD;
typedef uint16_t WCHAR;

/* These types MUST be 16-bit or 32-bit */
typedef int INT;
typedef unsigned int UINT;

/* These types MUST be 32-bit */
typedef int32_t LONG;
typedef uint32_t DWORD;

/* This type MUST be 64-bit (Remove this for C89 compatibility) */
typedef uint64_t QWORD;

#define FALSE 0
#define TRUE  1

#define RET_SUCCESS 0
#define RET_ERROR   1

/**
  \brief   Count leading zeros
  \details Counts the number of leading zeros of a data value.
  \param [in]  value  Value to count the leading zeros
  \return             number of leading zeros in value
 */
#define __CLZ                             __clz


typedef enum image_bpp
{
    IMAGE_BPP_BINARY,       // BPP = 0
    IMAGE_BPP_GRAYSCALE,    // BPP = 1
    IMAGE_BPP_RGB565,       // BPP = 2
    IMAGE_BPP_BAYER,        // BPP = 3
    IMAGE_BPP_JPEG          // BPP > 3
} image_bpp_t;

typedef struct image {
    int w;
    int h;
    int bpp;
    union {
        uint8_t *pixels;
        uint8_t *data;
    } u;
} image_t;

typedef enum  jpeg_subsample {
    JPEG_SUBSAMPLE_1x1 = 0x11,  // 1x1 chroma subsampling (No subsampling)
    JPEG_SUBSAMPLE_2x1 = 0x21,  // 2x2 chroma subsampling
    JPEG_SUBSAMPLE_2x2 = 0x22,  // 2x2 chroma subsampling
} jpeg_subsample_t;

/** decoded symbol type. */
typedef enum zbar_type {
    ZBAR_TYPE_NONE        =      0,  /**< no symbol decoded */
    ZBAR_TYPE_PARTIAL     =      1,  /**< intermediate status */
    ZBAR_TYPE_EAN8        =      8,  /**< EAN-8 */
    ZBAR_TYPE_UPCE        =      9,  /**< UPC-E */
    ZBAR_TYPE_ISBN10      =     10,  /**< ISBN-10 (from EAN-13). @since 0.4 */
    ZBAR_TYPE_UPCA        =     12,  /**< UPC-A */
    ZBAR_TYPE_EAN13       =     13,  /**< EAN-13 */
    ZBAR_TYPE_ISBN13      =     14,  /**< ISBN-13 (from EAN-13). @since 0.4 */
    ZBAR_TYPE_I25         =     25,  /**< Interleaved 2 of 5. @since 0.4 */
    ZBAR_TYPE_CODE39      =     39,  /**< Code 39. @since 0.4 */
    ZBAR_TYPE_PDF417      =     57,  /**< PDF417. @since 0.6 */
    ZBAR_TYPE_QRCODE      =     64,  /**< QR Code. @since 0.10 */
    ZBAR_TYPE_CODE128     =    128,  /**< Code 128 */
    ZBAR_TYPE_SYMBOL      = 0x00ff,  /**< mask for base symbol type */
    ZBAR_TYPE_ADDON2      = 0x0200,  /**< 2-digit add-on flag */
    ZBAR_TYPE_ADDON5      = 0x0500,  /**< 5-digit add-on flag */
    ZBAR_TYPE_ADDON       = 0x0700,  /**< add-on flag mask */
} zbar_type_t;


/* imgoptlib memory pool */
int   imgoptlib_mempool_init(void);
int   imgoptlib_mempool_deinit(void);
void *imgoptlib_malloc(size_t size);
void  imgoptlib_free(void *ptr);
void *imgoptlib_calloc(size_t nmemb, size_t size);
void *imgoptlib_realloc(void *ptr, size_t requested_size);

/* estream api for easy stream processing */
int      imgoptlib_estream_init(void);
int      imgoptlib_estream_deinit(void);
uint8_t *imgoptlib_estream_get_frame(int size);
int      imgoptlib_estream_return_frame(uint8_t *addr);
int      imgoptlib_estream_exec(uint8_t *src, uint8_t dst); /* It is stream source if src== NULL, it is stream sink if dst== NULL */ 
int      imgoptlib_estream_route(void); 
int      imgoptlib_estream_connect(void); 


/* jpeg */
int imgoptlib_jpeg_init(void);
int imgoptlib_jpeg_compress(image_t *src, image_t *dst, int quality);

/* dmtx */
int imgoptlib_dmtx_decode(unsigned char *pxl, const size_t width, const size_t height,
                                     unsigned char **output, int *outputsize);

/* zbar */
int imgoptlib_zbar_decode(unsigned char *pxl, const size_t width, const size_t height,
                                     unsigned char **output, int *outputsize, zbar_type_t *type);


/* other: dmtx/zbar/npu APIs is still in different folder, will move them here later */



#ifdef __cplusplus
}
#endif

#endif /* __IMAGEOPTLIB_H__ */
