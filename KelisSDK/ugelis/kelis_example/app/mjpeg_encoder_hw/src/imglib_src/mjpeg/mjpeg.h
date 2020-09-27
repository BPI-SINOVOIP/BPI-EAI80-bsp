
#ifndef __MJPEG_H__
#define __MJPEG_H__

#include "ff_wrapper.h"
#include "jpeg.h"

// Mjpeg class
typedef struct py_mjpeg_obj
{
    //mp_obj_base_t base;
    int width;
    int height;
    unsigned int frames;
    unsigned int bytes;
    FIL *fp;
} py_mjpeg_obj_t;

py_mjpeg_obj_t *py_mjpeg_open(const char *path, int width, int height);
void mjpeg_add_frame(FIL *fp, unsigned int *frames, unsigned int *bytes, image_t *img, int quality);
void mjpeg_close(FIL *fp, unsigned int *frames, unsigned int *bytes, float fps);
int  gm_roundf_c(float x);

#endif
