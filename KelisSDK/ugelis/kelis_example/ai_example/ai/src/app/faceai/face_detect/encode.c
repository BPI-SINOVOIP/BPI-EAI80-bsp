#include <string.h>
#include "encode.h"
#include "type.h"
#include <time.h>
#include <stdio.h>
#include "faceDetection.h"
#include <kernel.h>

#define KUART_Output(...)

static int fd_w = 0;
static int fd_h = 0;

//888 to gray
void rgb2L(unsigned char *PP, int w, int h)
{
    unsigned char *P_ = PP;
    int x, y;
    float r, g, b;
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            b = *PP++;
            g = *PP++;
            r = *PP++;
            *P_ = 0.29900f * r + 0.58700f * g + 0.11400f * b + 0.5f;
            P_++;
        }
    }
}
void face_init(int w, int h)
{
    fd_w = w;
    fd_h = h;
    fd_init(fd_w, fd_h);
    fd_set_param("alertFactor", 4);
    fd_set_param("speedFactor", 1.2);
    fd_set_param("fov", 49.0);
    fd_set_param("disWidthProduct", 58.0);
}

void face_extract(int *res_, uint32_t addr)
{
    FaceDetected result[25];
    uint32 nFace = 0;
    int k = 0, temp_ = 0;

    //rgb2L(addr, Width, Height);

    fd_detect_face((uint8 *)addr, fd_w, fd_h, result, &nFace);

    if (nFace >= 1)
    {
        nFace = 1;
    }
    for (k = 0; k < nFace; k++)
    {
        res_[temp_++] = result[k].x;
        res_[temp_++] = result[k].y;
        res_[temp_++] = result[k].width;
        res_[temp_++] = result[k].height;
        res_[temp_] = 1000;
    }
    fd_free();
}
