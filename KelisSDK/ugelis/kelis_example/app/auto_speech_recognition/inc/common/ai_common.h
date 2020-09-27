#ifndef __AI_COMMON_H__
#define __AI_COMMON_H__

#include <kernel.h>
#include "face_main.h"
#include <stdio.h>

void *malloc_ext(uint8_t memx, uint32_t size);
void free_ext(uint8_t memx, void *ptr);


#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)

#define CAM_BUF_WIDTH       320
#define CAM_BUF_HEIGHT      240
#define CAM_BUF_PIXEL_SIZE  3   //RGB888
#define CAM_BUF_FRAME_CNT   3
#define CAM_BUF_SIZE        ((CAM_BUF_WIDTH)*(CAM_BUF_HEIGHT)*(CAM_BUF_PIXEL_SIZE)*(CAM_BUF_FRAME_CNT))

#define MEM_RESERVE_START   (CONFIG_SDRAM_BASE_ADDRESS + (CONFIG_SDRAM_SIZE - CONFIG_KERNEL_MEM_RESVER_SIZE) * 1024)

#define MEM_IDTLIST_BASE    (MEM_RESERVE_START)
#define MEM_IDTLIST_SIZE    (2*1024)

#define MEM_UI_BASE         (MEM_IDTLIST_BASE + MEM_IDTLIST_SIZE)
#define MEM_UI_SIZE         (256*SIZE_1K)

#define MEM_CAM_BASE        (MEM_UI_BASE + MEM_UI_SIZE)//(0x10020000)
#define MEM_CAM_SIZE        (CAM_BUF_SIZE)

#define CAM_CROP_W          160
#define CAM_CROP_H          120
#define CAM_CROP_SIZE       (CAM_CROP_W*CAM_CROP_H*3)
#define CAM_DISP_W          160
#define CAM_DISP_H          120
#define CAM_DISP_SIZE       (CAM_DISP_W*CAM_DISP_H*3)
#define LCD_UI_W            220
#define LCD_UI_H            220
#define CAPTURE_SMALL_W     42
#define CAPTURE_SMALL_H     42

#define DIST_MAX_CMP_CNT    5
#define MAX_PERSON_CNT      30
#define ONE_PERSON_PICS_CNT 10
#define DIST_BUF_SIZE       (MAX_PERSON_CNT*ONE_PERSON_PICS_CNT)

/* BINARY MACRO */
#define WEI_BIN_SIZE        (6*SIZE_1M)
#define CMD_BIN_SIZE        (4*SIZE_1K)
#define BAK_BIN_SIZE        (1*SIZE_1M)

/* SDCARD MEMORY MAP */
#define WEI_BIN_SDCARD_START (0x1000000)
#define CMD_BIN_SDCARD_START (0x1600000)
#define BAK_BIN_SDCARD_START (0x1608000)

/* FLASH MEMORY MAP */
#ifdef FLASH_SIZE_32M
    #define FACEAI_FLASH_START  (0x00ad2000)
#else   //16M
    #define FACEAI_FLASH_START  (0x00050000)
#endif

#define WEI_BIN_FLASH_START (FACEAI_FLASH_START)
#define CMD_BIN_FLASH_START (WEI_BIN_FLASH_START+WEI_BIN_SIZE)
#define BAK_BIN_FLASH_START (CMD_BIN_FLASH_START+CMD_BIN_SIZE)

typedef struct
{
    unsigned char *tmp;
    unsigned char *cropped;         /* buffer that is used to be stored one cropped face image */
    unsigned char *detected;        /* buffer that is used to be detected face */
    unsigned char *scaleup;         /* buffer that is used to be scaled up by IMP to be shown */
} extra_buffer_t;

struct frame_data
{
    char frame[TEST_BUFFER_SIZE];
};

typedef struct frame_data frame_data_t;

typedef struct
{
    frame_data_t *data;
    int index;
} base_backup_t;

typedef struct
{
    frame_data_t *data;             /* backup the data for debug */
    int ignore[CAPTURE_BACKUP_CNT]; /* is used to ignore some unexcepted images */
    int index;                      /* current capture index */
    int cap_cnt;                    /* indicate how many images would be captured with one times of capture */
} capture_backup_t;

typedef struct
{
    frame_data_t *data;
    int index;
} validate_backup_t;

typedef struct
{
    float dist[DIST_BUF_SIZE];
    int label[DIST_BUF_SIZE];
    int index[DIST_BUF_SIZE];
    int freq[MAX_PERSON_CNT];
    float dist_aver[MAX_PERSON_CNT];
    float dist_min;
} distance_calc_t;

typedef struct
{
    int person_img_cnt[MAX_PERSON_CNT];
    int total_imgs_cnt;
    int person_cnt;
    unsigned char *network_input;
    unsigned char *wei_addr;
    unsigned char *cmd_addr;
    int need_reply;
    int reply_value;
    int skip_camera_stream;         /* no need to care about camera stream when in capture mode */
    float threshold;                /* to adjust the network_predict confidence */
    int frame_ready;                /* the camera buffer is ready to be detected */
    int has_person;
    int validate_started;
    int capture_started;
    struct k_sem predict_lock;
    int show_detected_face;         /* show the detected face image on the left lower corner of the screen */
    int cap_index;
    extra_buffer_t cam_buf;
    char **person_name;
    base_backup_t base_backup;      /* backup the base images */
    distance_calc_t dist;
    int debug_print;
    int network_predict;                /* stop network_prediction for debug */
    capture_backup_t capture_backup;
    validate_backup_t validate_backup;
    struct device *lcd_dev;
    struct device *cam_dev;
    node_t *base_head;
    node_t *previous_node;
    struct k_thread task_thread;
    int thumb_w;
    int thumb_h;
    int image_w;
    int image_h;
    int text_area_y_start;
    int ui_w;
    int ui_h;
} FaceAIData;

#endif
