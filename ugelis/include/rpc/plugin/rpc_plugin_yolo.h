
#ifndef _RPC_YOLO_H_
#define _RPC_YOLO_H_


#ifdef __cplusplus
extern "C" {
#endif



#include <ugelis/types.h>

typedef struct
{
    uint8_t *img;  /* input img buffer */
    uint8_t *bbox; /* output img buffer */
}YOLO_Data_T;

#define DEV_RPC_YOLO_NAME			"RPC_YOLO_DEV"

#define IMG_CAM_PIXELS_COUNT_BYTES  0x24C00   /*224 * 224 * 3 from camera*/
#define IMG_VAD_PIXELS_COUNT_BYTES  0x24C04   /*224 * 224 * 3 + 2 + 2 for CNN computation*/


/*For post YOLO*/
typedef struct Box {
   double box[4];
   double score;
   int class;
   struct Box * next;
} Box_t;

typedef struct
{
	unsigned char camrgb[IMG_CAM_PIXELS_COUNT_BYTES];
	unsigned char vadrgb[IMG_VAD_PIXELS_COUNT_BYTES];
}object_imgdata;


typedef enum
{
	DET_BODY = 0,
	DET_GEST = 1,
	DET_OBJS = 2,
	DET_NOTDEF = 0xff
}detection_type;


typedef struct
{
	unsigned char *cam_buf_tmp;      /*Used for bounding box cord, width * heighth * 4, blending on layer 0*/
	unsigned char *cam_buf_cropped;  /*Not Used*/
	unsigned char *cam_buf_detected; /*Captured buffer*/
	unsigned char *cam_buf_validate; /*Used for object validate*/
}extra_buffer_t;

typedef struct
{
	unsigned char *input_data_addr;  /*Point to the buffer of to-be-detected picture*/
	struct k_sem validate_lock;      /*Thread sync mechanism*/
	int reserved[2];                 // must be fix  ,when k_sem using  CONFIG_POLL
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

typedef struct
{
	Box_t *box;                  /*Box for detected objects*/
	int   boxcnt;
	detection_type det_type;              /*0:Body , 1: Gesture */
	detection_type det_type_cpy;          /*0:Body , 1: Gesture */ 
	/*might have other variables such as name*/	
}ObjBodyData;
#ifdef __cplusplus
}
#endif

#endif /* _RPC_YOLO_H_ */
