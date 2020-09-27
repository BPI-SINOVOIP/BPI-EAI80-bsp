#ifndef __POSITION_H__
#define __POSITION_H__
#include <ugelis.h>

typedef float len_t;
typedef double angle_t;

#define MAX_PERSON  8

typedef struct t_person_info
{
    len_t x;
    len_t z;
    len_t h;
    angle_t angle;
} obj_info_t;

typedef struct t_camera_position_param
{
    len_t   height_to_ground;//in mm
    angle_t angle_to_ground;//in rad
    len_t   focal_len;//in pixel
    len_t   pic_width;//in pixel
    len_t   pic_height;//in pixel
} camera_position_param_t;

typedef struct t_person_obj
{
    uint16_t     off_l;
    uint16_t     off_t;
    uint16_t     width;
    uint16_t     height;
    uint8_t     off_z;
    uint8_t     angle;
} obj_person_t;

typedef struct t_person_show
{
    uint8_t         num;
    obj_person_t    obj[MAX_PERSON];
} obj_show_t;

int obj_info_set_camera_param(camera_position_param_t camera_position_param);

obj_info_t obj_info_get(len_t foot_x, len_t foot_y, len_t head_x, len_t head_y);


obj_info_t obj_info_get_2(len_t foot_x, len_t foot_y, len_t head_x, len_t head_y);

#endif
