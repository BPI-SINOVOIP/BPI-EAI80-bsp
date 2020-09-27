#ifndef __POSITION_H__
#define __POSITION_H__

typedef float len_t;
typedef double angle_t;

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

int obj_info_set_camera_param(camera_position_param_t camera_position_param);

obj_info_t obj_info_get(len_t foot_x, len_t foot_y, len_t head_x, len_t head_y);

#endif
