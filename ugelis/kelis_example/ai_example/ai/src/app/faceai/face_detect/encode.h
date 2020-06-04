#ifndef _JENCODE_H
#define _JENCODE_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

void face_init(int w, int h);
void face_extract(int *res, unsigned int addr);

#endif
