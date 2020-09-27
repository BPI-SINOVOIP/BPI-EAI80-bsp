#ifndef __WIFI_OTA_H__
#define __WIFI_OTA_H__

#define UPDATE_WEIGHTS     0x1A
#define UPDATE_SYSTEM      0x1B
#define UPDATE_DATA        0x1C
#define INFO_ADDRESS_IN_FLASH   0

typedef struct
{
    uint32_t part_num;              //0x00000003
    uint32_t info_addr;             //0x10000000
    uint32_t info_size;             //0x00001000
    uint32_t info_load_addr;        //0x00000000
    uint32_t boot1_addr;            //0x10001000
    uint32_t boot1_size;            //0x00002e7c(not fixed)
    uint32_t boot1_load_addr;       //0x20001000
    uint32_t system_addr;           //0x10010000
    uint32_t system_size;           //0x00002cf0(not fixed)
    uint32_t system_load_addr;      //0xc0010000
} info_t;


#endif
