#ifndef __OBJ_RECOGNIZE_H__
#define __OBJ_RECOGNIZE_H__

#include <worker/worker.h>

typedef struct
{
    unsigned char *cam_buf_tmp;      /*Used for bounding box cord, width * heighth * 4, blending on layer 0*/
    unsigned char *cam_buf_cropped;  /*Not Used*/
    unsigned char *cam_buf_detected; /*Captured buffer*/
    unsigned char *cam_buf_validate; /*Used for object validate*/
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

typedef struct
{
    unsigned char *input_data_addr;  /*Point to the buffer of to-be-detected picture*/
    struct k_sem validate_lock;      /*Thread sync mechanism*/
    extra_buffer_t ex_buf;
    int   stop_disp;                 /*Stop display on LCD*/
    float threshold;
    int   one_frame_detect_started;  /*Start detect one frame*/
    int   cap_show;                  /*This causes LCD show the detected-object pic*/
    int   pic_saved;                 /*save pictures*/
    Box_t box[100];                  /*Box for detected objects*/
    int   boxcnt;
    detection_type det_type;              /*0:Body , 1: Gesture */
    detection_type det_type_cpy;          /*0:Body , 1: Gesture */

    char **object_name;              /*Object Name*/
    /*might have other variables such as name*/
} ObjDetData;

typedef struct obj_recognized_data
{
    int class_channel;
    detection_type type;
} obj_recognized_data_t;

void obj_recognize_thread(void *data, void *param, void *not_used);
int obj_recognize_init(easynet_proc_param_t *param);
int obj_recognize_exit(easynet_proc_param_t *param);
int obj_recognize_frame_in(easynet_proc_param_t *param);

#endif //__OBJ_RECOGNIZE_H__

