#ifndef __AI_COMMON_H__
#define __AI_COMMON_H__

#include <kernel.h>

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

typedef struct
{
    int   show_demo_pic;
    Box_t box[100];                  /*Box for detected objects*/
    int   boxcnt;
    detection_type det_type;              /*0:Body , 1: Gesture */
    detection_type det_type_cpy;          /*0:Body , 1: Gesture */

    /*might have other variables such as name*/
} ObjDetData;

#endif
