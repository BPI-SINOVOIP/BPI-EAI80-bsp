#ifndef __IMG_H__
#define __IMG_H__

void image_zoom(unsigned char *pSrcImg, unsigned char *pDstImg, int nSrcWidth, int nSrcHeight, int dst_w, int dst_h);
void image_crop(unsigned char *src, unsigned char *dst_addr, int x, int y, int w, int h, unsigned int src_w);
void image_split(unsigned char *src, unsigned char *dst, int w, int h);

#endif
