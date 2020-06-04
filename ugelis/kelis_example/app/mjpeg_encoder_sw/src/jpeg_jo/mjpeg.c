/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * A super simple MJPEG encoder.
 *
 */

#include "jpeg.h"
#include "ff_wrapper.h"
#include <math.h>
#include "mjpeg.h"
#include <malloc_ext.h>
#include <math.h>
#include "ugelis.h"

#define MALLOC_LEN      (1024*64)

#define SIZE_OFFSET             (1*4)
#define MICROS_OFFSET           (8*4)
#define FRAMES_OFFSET           (12*4)
#define RATE_0_OFFSET           (19*4)
#define LENGTH_0_OFFSET         (21*4)
#define RATE_1_OFFSET           (33*4)
#define LENGTH_1_OFFSET         (35*4)
#define MOVI_OFFSET             (54*4)

int inline roundf_c(float x)
{
    //int n;
    float r = x - (int) x;
    return (x > 0) ? ((r > 0.49999) ? (int)(x + 0.5) : (int)x) : ((r < -0.49999) ? ((int)(x - 0.5)) : (int)x);
}

int  gm_roundf_c(float x)
{
    return roundf_c(x);
}


void mjpeg_open(FIL *fp, int width, int height)
{
    write_data(fp, "RIFF", 4); // FOURCC fcc; - 0
    write_long(fp, 0); // DWORD cb; size - updated on close - 1
    write_data(fp, "AVI ", 4); // FOURCC fcc; - 2

    write_data(fp, "LIST", 4); // FOURCC fcc; - 3
    write_long(fp, 192); // DWORD cb; - 4
    write_data(fp, "hdrl", 4); // FOURCC fcc; - 5

    write_data(fp, "avih", 4); // FOURCC fcc; - 6
    write_long(fp, 56); // DWORD cb; - 7
    write_long(fp, 0); // DWORD dwMicroSecPerFrame; micros - updated on close - 8
    write_long(fp, 0); // DWORD dwMaxBytesPerSec; updated on close - 9
    write_long(fp, 4); // DWORD dwPaddingGranularity; - 10
    write_long(fp, 0); // DWORD dwFlags; - 11
    write_long(fp, 0); // DWORD dwTotalFrames; frames - updated on close - 12
    write_long(fp, 0); // DWORD dwInitialFrames; - 13
    write_long(fp, 1); // DWORD dwStreams; - 14
    write_long(fp, 0); // DWORD dwSuggestedBufferSize; - 15
    write_long(fp, width); // DWORD dwWidth; - 16
    write_long(fp, height); // DWORD dwHeight; - 17
    write_long(fp, 1000); // DWORD dwScale; - 18
    write_long(fp, 0); // DWORD dwRate; rate - updated on close - 19
    write_long(fp, 0); // DWORD dwStart; - 20
    write_long(fp, 0); // DWORD dwLength; length - updated on close - 21

    write_data(fp, "LIST", 4); // FOURCC fcc; - 22
    write_long(fp, 116); // DWORD cb; - 23
    write_data(fp, "strl", 4); // FOURCC fcc; - 24

    write_data(fp, "strh", 4); // FOURCC fcc; - 25
    write_long(fp, 56); // DWORD cb; - 26
    write_data(fp, "vids", 4); // FOURCC fccType; - 27
    write_data(fp, "MJPG", 4); // FOURCC fccHandler; - 28
    write_long(fp, 0); // DWORD dwFlags; - 29
    write_word(fp, 0); // WORD wPriority; - 30
    write_word(fp, 0); // WORD wLanguage; - 30.5
    write_long(fp, 0); // DWORD dwInitialFrames; - 31
    write_long(fp, 1000); // DWORD dwScale; - 32
    write_long(fp, 0); // DWORD dwRate; rate - updated on close - 33
    write_long(fp, 0); // DWORD dwStart; - 34
    write_long(fp, 0); // DWORD dwLength; length - updated on close - 35
    write_long(fp, 0); // DWORD dwSuggestedBufferSize; - 36
    write_long(fp, 10000); // DWORD dwQuality; - 37
    write_long(fp, 0); // DWORD dwSampleSize; - 38
    write_word(fp, 0); // short int left; - 39
    write_word(fp, 0); // short int top; - 39.5
    write_word(fp, 0); // short int right; - 40
    write_word(fp, 0); // short int bottom; - 40.5

    write_data(fp, "strf", 4); // FOURCC fcc; - 41
    write_long(fp, 40); // DWORD cb; - 42
    write_long(fp, 40); // DWORD biSize; - 43
    write_long(fp, width); // LONG biWidth; - 44
    write_long(fp, height); // LONG biHeight; - 45
    write_word(fp, 1); // WORD biPlanes; - 46
    write_word(fp, 24); // WORD biBitCount; - 46.5
    write_data(fp, "MJPG", 4); // DWORD biCompression; - 47
    write_long(fp, 0); // DWORD biSizeImage; - 48
    write_long(fp, 0); // LONG biXPelsPerMeter; - 49
    write_long(fp, 0); // LONG biYPelsPerMeter; - 50
    write_long(fp, 0); // DWORD biClrUsed; - 51
    write_long(fp, 0); // DWORD biClrImportant; - 52

    write_data(fp, "LIST", 4); // FOURCC fcc; - 53
    write_long(fp, 0); // DWORD cb; movi - updated on close - 54
    write_data(fp, "movi", 4); // FOURCC fcc; - 55
}

/*224 + 8* nframe+ pmjpeg->bytes*/
void mjpeg_add_frame(FIL *fp, uint32_t *frames, uint32_t *bytes, image_t *img, int quality)
{
    write_data(fp, "00dc", 4); // FOURCC fcc;

    *frames += 1;

    uint32_t size = MALLOC_LEN;
    uint8_t *buffer = (uint8_t *)malloc_ext(0, MALLOC_LEN);  //fb_alloc_all(&size);
    //image_t out = { .w=img->w, .h=img->h, .bpp=size, .pixels= buffer };
    image_t out;
    out.w = img->w;
    out.h = img->h;
    out.bpp = size;
    out.pixels = buffer;

    // When jpeg_compress needs more memory than in currently allocated it
    // will try to realloc. MP will detect that the pointer is outside of
    // the heap and return NULL which will cause an out of memory error.
    jpeg_compress(img, &out, quality, true);
    int pad = (((out.bpp + 3) / 4) * 4) - out.bpp;

    write_long(fp, out.bpp + pad); // DWORD cb;

    //fclose(fp);
    //while(1);

    printf("out.bpp + pad=%d\n", out.bpp + pad);

    write_data(fp, out.pixels, out.bpp + pad); // reading past okay

    *bytes += out.bpp + pad;

}

void mjpeg_close(FIL *fp, uint32_t *frames, uint32_t *bytes, float fps)
{
    // Needed
    file_seek(fp, SIZE_OFFSET); // maybe error
    write_long(fp, 216 + (*frames * 8) + *bytes);
    // Needed
    file_seek(fp, MICROS_OFFSET);
    write_long(fp, (!(int)gm_roundf_c(fps)) ? 0 :
               (int)gm_roundf_c(1000000 / fps));
    write_long(fp, (!(*frames)) ? 0 :
               (int)gm_roundf_c((((*frames * 8) + *bytes) * fps) / *frames));
    // Needed
    file_seek(fp, FRAMES_OFFSET);
    write_long(fp, *frames);
    // Probably not needed but writing it just in case.
    file_seek(fp, RATE_0_OFFSET);

    printf("roundf(fps) = %d,fps=%f\n", (int)gm_roundf_c(fps), fps);

    write_long(fp, (int)gm_roundf_c(fps * 1000));
    // Probably not needed but writing it just in case.
    file_seek(fp, LENGTH_0_OFFSET);
    write_long(fp, (!(int)gm_roundf_c(fps)) ? 0 :
               (int)gm_roundf_c((*frames * 1000) / fps));
    // Probably not needed but writing it just in case.
    file_seek(fp, RATE_1_OFFSET);
    write_long(fp, (int)gm_roundf_c(fps * 1000));
    // Probably not needed but writing it just in case.
    file_seek(fp, LENGTH_1_OFFSET);
    write_long(fp, (!(int)gm_roundf_c(fps)) ? 0 :
               (int)gm_roundf_c((*frames * 1000) / fps));
    // Needed
    file_seek(fp, MOVI_OFFSET);
    write_long(fp, 4 + (*frames * 8) + *bytes);
    file_close(fp);
}

py_mjpeg_obj_t *py_mjpeg_open(const char *path, int width, int height)
{
    py_mjpeg_obj_t *mjpeg = (py_mjpeg_obj_t *)malloc_ext(0, sizeof(py_mjpeg_obj_t)); //m_new_obj(py_mjpeg_obj_t);
    mjpeg->width  = width ;// py_helper_keyword_int(n_args, args, 1, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_width), MAIN_FB()->w);
    mjpeg->height = height ;//py_helper_keyword_int(n_args, args, 2, kw_args, MP_OBJ_NEW_QSTR(MP_QSTR_height), MAIN_FB()->h);
    mjpeg->frames = 0;
    mjpeg->bytes = 0;

    printf("func[%s],line=%d \n", __FUNCTION__, __LINE__);

#ifdef  BUF_DEBUG

#else
    mjpeg->fp = fopen(path, "wb+");
#endif
    printf("func[%s],line=%d \n", __FUNCTION__, __LINE__);

    mjpeg_open(mjpeg->fp, mjpeg->width, mjpeg->height);

    printf("func[%s] \n", __FUNCTION__);

    return mjpeg;
}
