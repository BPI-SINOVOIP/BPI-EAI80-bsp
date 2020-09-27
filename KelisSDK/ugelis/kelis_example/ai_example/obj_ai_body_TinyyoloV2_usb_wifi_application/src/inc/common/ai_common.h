#ifndef __AI_COMMON_H__
#define __AI_COMMON_H__

#include <kernel.h>

typedef struct
{
    unsigned char *cam_buf_tmp;      /*Used for bounding box cord, width * heighth * 4, blending on layer 0*/
    unsigned char *cam_buf_cropped;  /*Not Used*/
    unsigned char *cam_buf_detected; /*Captured buffer*/
    unsigned char *cam_buf_validate; /*Used for object validate*/
    unsigned char *camera_jpeg_buf;  /*Jpeg buf used for usb and wifi*/
    unsigned char *encode_jpeg_buf;
    unsigned char *usb_jpeg_buf;     /**/
} extra_buffer_t;

typedef enum
{
    DET_BODY = 0,
    DET_GEST = 1,
    DET_OBJS = 2,
    DET_NOTDEF = 0xff
} detection_type;


/*For post YOLO*/
typedef struct Box
{
    float box[4];
    float score;
    int class;
    struct Box *next;
} Box_t;

/*User cmds*/
typedef struct CMD
{
    int usb;
    int wifi;
} CMD_t;

/*Jpeg informations*/
typedef struct JPEG
{
    uint32_t jpeg_addr;
    uint32_t jpeg_lenth;
    uint32_t jpeg_timestamp;
} JPEG_t;

typedef struct
{
    struct k_sem validate_lock;      /*Thread sync mechanism*/
    extra_buffer_t ex_buf;
    int   stop_disp;                 /*Stop display on LCD*/
    float threshold;
    int   one_frame_detect_started;  /*Start detect one frame*/
    int   show_demo_pic;
    int   show_demopic_onetime;
    int   cap_show;                  /*This causes LCD show the detected-object pic*/
    int   pic_saved;                 /*save pictures*/
    int   boxcnt;
    char **object_name;              /*Object Name*/
    CMD_t cmd;                       /*user cmds for usb or wifi transfer*/
    JPEG_t jpeg;                     /*jpeg informations:lenth, address, timestamp*/
    /*might have other variables such as name*/
} ObjDetData;

#endif
