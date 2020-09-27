#ifndef _IMAGE
#define _IMAGE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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

typedef struct image
{
    int w;
    int h;
    int bpp;
    union
    {
        uint8_t *pixels;
        uint8_t *data;
    } u;
} image_t;

typedef enum  jpeg_subsample
{
    JPEG_SUBSAMPLE_1x1 = 0x11,  // 1x1 chroma subsampling (No subsampling)
    JPEG_SUBSAMPLE_2x1 = 0x21,  // 2x2 chroma subsampling
    JPEG_SUBSAMPLE_2x2 = 0x22,  // 2x2 chroma subsampling
} jpeg_subsample_t;


int imagelib_mem_init(void);
int imagelib_mem_deinit(void);
int imagelib_mem_clear(void);
void *imagelib_malloc(size_t size);
void imagelib_free(void *ptr);
void *imagelib_calloc(size_t nmemb, size_t size);
void *imagelib_realloc(void *ptr, size_t requested_size);

/* estream api for easy stream processing */
int estream_init(void);
uint8_t *estream_get_frame(int size);
int estream_return_frame(uint8_t *addr);
int estream_exec(uint8_t *src, uint8_t dst); /* It is stream source if src== NULL, it is stream sink if dst== NULL */
int estream_route(void);
int estream_connect(void);


#ifdef __cplusplus
}
#endif

#endif /* _IMAGE */
