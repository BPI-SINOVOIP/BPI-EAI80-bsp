#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "qipan_rgb_320x240.h"
#include "jpeg.h"

image_t img;
#define  JPG_LEN          (100*1024)


int main(int argc, char *argv[])
{
    img.w = WIDTH;
    img.h = HEGIHT;
    img.data = &buf_rgb888[0];

    unsigned char *dst_jpeg = (uint8_t *)malloc_ext(0, JPG_LEN);

    printf("main start...\n");
    jpeg_encode(&img, dst_jpeg, 90);
    free_ext(0, dst_jpeg);
    printf("main end\n");
    return 0;
}