
#ifndef __JPEG_H__
#define __JPEG_H__

#include "ff_wrapper.h"
#include "image.h"


#define bool int
#define false 0
#define true 1


typedef struct
{
    unsigned char *buf;
    unsigned char *beginbuf;
    unsigned char *seek_set_buf;
    unsigned char seek_flag;
    unsigned int  mjpegencodelen;
} imagedatabuf_t;

//bool jpeg_compress(image_t *src, image_t *dst, int quality, bool realloc);

void *malloc_ext(unsigned char memx, unsigned int size);
void free_ext(unsigned char memx, void *ptr);
int jpeg_hal_init(void);
int jpeg_hal_compress(image_t *src, image_t *dst, int quality);


#endif
