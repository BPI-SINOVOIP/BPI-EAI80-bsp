#ifndef __SYS_SAI_H
#define __SYS_SAI_H
//#include "sys.h"
#define     __IO    volatile             /*!< Defines 'read / write' permissions*/

#ifndef NULL
    #define NULL ((void *)0)
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned int  vu32;
typedef unsigned short vu16;
typedef unsigned char  vu8;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define DMA_SAI_EN 1

//unsigned int len = 4096;
char rxmem[1024 * 8]; //len

#define  SAI_A_BASE_ADDR      0x40032000
#define  SAI_B_BASE_ADDR      0x40032400

typedef struct
{
    __IO   uint32_t  SAI_CR1    ;  //0x00
    __IO   uint32_t  SAI_CR2    ;  //0x04
    __IO   uint32_t  SAI_FRCR   ;  //0x08
    __IO   uint32_t  SAI_SLOTR  ;  //0x0C
    __IO   uint32_t  SAI_IM     ;  //0x10
    __IO   uint32_t  SAI_SR     ;  //0x14
    __IO   uint32_t  RESERVED   ;  //0x18
    __IO   uint32_t  SAI_DR     ;  //0x1C
    __IO   uint32_t  SAI_CKCTL  ;  //0x20

} CMSDK_SAI_TypeDef;


#define CMSDK_SAI_A             ((CMSDK_SAI_TypeDef   *) SAI_A_BASE_ADDR)
#define CMSDK_SAI_B             ((CMSDK_SAI_TypeDef   *) SAI_B_BASE_ADDR)

#endif
