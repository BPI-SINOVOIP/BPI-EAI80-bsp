
#include <time.h>
#include <math.h>

#include <obj_info.h>

#define PI         3.14159

static len_t   CAMERA_HEIGHT;//in mm
static angle_t ANGLE;//in rad
static len_t   CAMERA_F;//in pixel
static len_t   PIC_WIDTH;//in pixel
static len_t   PIC_HEIGHT;//in pixel


/* set position param of camera, this info can get by engineer who' in charge of fixing the camera */
int obj_info_set_camera_param(camera_position_param_t camera_position_param)
{
    CAMERA_HEIGHT = camera_position_param.height_to_ground;
    ANGLE = camera_position_param.angle_to_ground;
    CAMERA_F = camera_position_param.focal_len;
    PIC_WIDTH = camera_position_param.pic_width;
    PIC_HEIGHT = camera_position_param.pic_height;

    return 0;
}

static obj_info_t get_obj_info_lower(len_t foot_x, len_t foot_y, len_t head_x, len_t head_y, angle_t Angle_AcBcAg, len_t Len_OcBc)
{
    /* change to pic axis */
    len_t Foot_X = foot_x - (len_t)PIC_WIDTH / 2.0;
    len_t Foot_Y = (len_t)PIC_HEIGHT / 2.0 - foot_y;
    len_t Head_X = head_x - (len_t)PIC_WIDTH / 2.0;
    len_t Head_Y = (len_t)PIC_HEIGHT / 2.0 - head_y;

    len_t Len_OcO = CAMERA_F;
    len_t Len_PyO = fabs(Foot_Y);
    len_t Len_PxO = fabs(Foot_X);
    len_t Len_BcQc = Len_OcO / Len_PyO * Len_OcBc;
    len_t Len_QcOc = sqrt(Len_BcQc * Len_BcQc + Len_OcBc * Len_OcBc);
    len_t Len_PyOc = sqrt(Len_OcO * Len_OcO + Len_PyO * Len_PyO);
    len_t Len_PcQc = Len_PxO / Len_PyOc * Len_QcOc;

    angle_t Angle_BcQcQg = atan(Len_OcBc / Len_BcQc);
    angle_t Angle_BcQgQc = PI - Angle_BcQcQg - Angle_AcBcAg;
    len_t Len_QgQc = Len_BcQc * (sin(Angle_AcBcAg) / sin(Angle_BcQgQc));
    len_t Len_PgQg = (Len_QcOc - Len_QgQc) / Len_QcOc * Len_PcQc;
    len_t Len_BcQg = sin(Angle_BcQcQg) / sin(Angle_BcQgQc) * Len_BcQc;

#if 0//angle in ground plane
    angle_t Angle_PgBcQg = atan(Len_PgQg / Len_BcQg) * (180.0 / PI);
#else//angle in optic axis plane
    angle_t Angle_PgBcQg = atan(Len_PxO / Len_PyOc) * (180.0 / PI);
#endif

    obj_info_t person_info;
    person_info.x = (Foot_X < 0) ? Len_PgQg : -Len_PgQg;
    person_info.z = Len_BcQg;
    person_info.h = 0;
    person_info.angle = (Foot_X < 0) ? (90.0 - Angle_PgBcQg) : (90.0 + Angle_PgBcQg);

    return person_info;
}

static obj_info_t get_person_info_higher(len_t foot_x, len_t foot_y, len_t head_x, len_t head_y, angle_t Angle_AcBcAg, len_t Len_OcBc)
{
    /* change to pic axis */
    len_t Foot_X = foot_x - (len_t)PIC_WIDTH / 2.0;
    len_t Foot_Y = (len_t)PIC_HEIGHT / 2.0 - foot_y;
    len_t Head_X = head_x - (len_t)PIC_WIDTH / 2.0;
    len_t Head_Y = (len_t)PIC_HEIGHT / 2.0 - head_y;

    len_t Len_OcO = CAMERA_F;
    len_t Len_PyO = fabs(Foot_Y);
    len_t Len_PxO = fabs(Foot_X);

    angle_t Angle_OOcPy = atan(Len_PyO / Len_OcO);
    angle_t Angle_BcOcPy = Angle_OOcPy + PI / 2.0;
    angle_t Angle_OcBcQg = PI / 2.0 - Angle_AcBcAg;
    angle_t Angle_BcQgOc = PI - Angle_BcOcPy - Angle_OcBcQg;
    len_t Len_BcQg = sin(Angle_BcOcPy) / sin(Angle_BcQgOc) * Len_OcBc;

    len_t Len_OcQg = sin(Angle_OcBcQg) / sin(Angle_BcQgOc) * Len_OcBc;
    len_t Len_OcPy = sqrt(Len_OcO * Len_OcO + Len_PyO * Len_PyO);
    len_t Len_PgQg = Len_PxO / Len_OcPy * Len_OcQg;

#if 0//angle in ground plane
    angle_t Angle_PgBcQg = atan(Len_PgQg / Len_BcQg) * (180.0 / PI);
#else//angle in optic axis plane
    angle_t Angle_PgBcQg = atan(Len_PxO / Len_OcPy) * (180.0 / PI);
#endif

    obj_info_t person_info;
    person_info.x = (Foot_X < 0) ? Len_PgQg : -Len_PgQg;
    person_info.z = Len_BcQg;
    person_info.h = 0;
    person_info.angle = (Foot_X < 0) ? (90.0 - Angle_PgBcQg) : (90.0 + Angle_PgBcQg);

    return person_info;
}

/*
assume that the optic axis is not parallel to the ground
input: foot_x, foot_y, head_x, head_y is in pixel cordinary(pixel unit)
output: obj_info_t is in camera cordinary(cm unit)
get_obj_info_lower(): the foot is lower than the optic axis
get_person_info_higher(): the foot is higher than the optic axis
*/
obj_info_t obj_info_get(len_t foot_x, len_t foot_y, len_t head_x, len_t head_y)
{
    static angle_t Angle_AcBcAg;
    static len_t Len_OcBc;
    static int init_flag = 0;

    Angle_AcBcAg = ANGLE;
    Len_OcBc = CAMERA_HEIGHT;

    obj_info_t person_info;
    if (foot_y > (len_t)PIC_HEIGHT / 2.0)
    {
        person_info = get_obj_info_lower(foot_x, foot_y, head_x, head_y, Angle_AcBcAg, Len_OcBc);
    }
    else
    {
        person_info = get_person_info_higher(foot_x, foot_y, head_x, head_y, Angle_AcBcAg, Len_OcBc);
    }

    return person_info;
}