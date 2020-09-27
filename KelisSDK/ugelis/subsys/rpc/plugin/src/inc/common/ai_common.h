#ifndef __AI_COMMON_H__
#define __AI_COMMON_H__

#include <kernel.h>

void *malloc_ext(uint8_t memx,uint32_t size);
void free_ext(uint8_t memx,void *ptr);

#if 0
#define malloc malloc_ext
#define free   free_ext
#endif
#define SIZE_1K             (1<<10)
#define SIZE_1M             (1<<20)
#define CAM_CROP_W 160
#define CAM_CROP_H 120
#define CAM_CROP_SIZE (CAM_CROP_W*CAM_CROP_H*3)
#define LCD_UI_W	96
#define LCD_UI_H	96

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
    #define FACEAI_FLASH_START  (0x10ad2000)
#else   //16M
    #define FACEAI_FLASH_START  (0x10050000)
#endif

#define WEI_BIN_FLASH_START (FACEAI_FLASH_START)
#define CMD_BIN_FLASH_START (WEI_BIN_FLASH_START+WEI_BIN_SIZE)
#define BAK_BIN_FLASH_START (CMD_BIN_FLASH_START+CMD_BIN_SIZE)
typedef struct
{
	unsigned char *cam_buf_tmp;      /*Used for bounding box cord, width * heighth * 4, blending on layer 0*/
	unsigned char *cam_buf_cropped;  /*Not Used*/
	unsigned char *cam_buf_detected; /*Captured buffer*/
	unsigned char *cam_buf_validate; /*Used for object validate*/
}extra_buffer_t;

typedef enum
{
	DET_BODY = 0,
	DET_GEST = 1,
	DET_OBJS = 2,
	DET_NOTDEF = 0xff
}detection_type;


/*For post YOLO*/
typedef struct Box {
   double box[4];
   double score;
   int class;
   struct Box * next;
} Box_t;

#define IMG_COUNT_BYTES          4
#define IMG_CAM_PIXELS_COUNT_BYTES  0x24C00   /*224 * 224 * 3 from camera*/
#define IMG_VAD_PIXELS_COUNT_BYTES  0x24C04   /*224 * 224 * 3 + 2 + 2 for CNN computation*/
#define IMG_DUMP_COUNT           1  // 4   //8
#define IMG_DUMP_PIXELS          (IMG_COUNT_BYTES + (IMG_CAM_PIXELS_COUNT_BYTES + IMG_VAD_PIXELS_COUNT_BYTES) * IMG_DUMP_COUNT)

#if 0
typedef struct
{
	unsigned char camrgb[IMG_CAM_PIXELS_COUNT_BYTES];
	unsigned char vadrgb[IMG_VAD_PIXELS_COUNT_BYTES];
}object_imgdata;

typedef struct
{
	unsigned char *input_data_addr;  /*Point to the buffer of to-be-detected picture*/
	struct k_sem validate_lock;      /*Thread sync mechanism*/
	extra_buffer_t ex_buf;
	int   stop_disp;                 /*Stop display on LCD*/
	float threshold;
	int   one_frame_detect_started;  /*Start detect one frame*/
	int   show_demo_pic;
	int   show_demopic_onetime;
	int   cap_show;                  /*This causes LCD show the detected-object pic*/
	int   pic_saved;                 /*save pictures*/
	
	Box_t box[100];                  /*Box for detected objects*/
	int   boxcnt;
	
	uint32_t unreg_imgcnt;
	uint32_t reg_imgcnt;
	detection_type det_type;              /*0:Body , 1: Gesture */
	detection_type det_type_cpy;          /*0:Body , 1: Gesture */
	
	object_imgdata *unregdumpdata;        /*pointer to the image data buffer for dump debug [IMG_DUMP_COUNT]*/ 	 
	object_imgdata *regdumpdata;          /*pointer to the image data buffer for dump debug [IMG_DUMP_COUNT]*/ 	 
	
	char **object_name;              /*Object Name*/	
	/*might have other variables such as name*/	
}ObjDetData;
#endif

#endif
