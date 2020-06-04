#ifndef OBJ_DETECTOR_API_H
#define OBJ_DETECTOR_API_H

#include "faceDetection.h"

typedef short               int16_t;
typedef int                 int32_t;
//typedef long long           int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef float               float32_t;
typedef unsigned long long  uint64_t;
typedef unsigned char       bool;
typedef struct
{
    float32_t x;
    float32_t y;
} Point2Df;

typedef struct
{
    uint32_t topx;
    uint32_t topy;
    uint32_t width;
    uint32_t height;
} icvRect;


/*
// -----------------------------------------------------------------------------
/// @brief
///   Face detection result structure for one face
///
/// @remark
///   If the ROIflag is set as 1, the bounding box will be used as mask for face detection.
///   The face detection will be performed in the bounding box area instead of the whole image.
// -----------------------------------------------------------------------------
*/
typedef struct
{
    /// flag to inidate whether the bounding box will be used as mask for face detection
    uint32_t ROIflag;
    /// top left x coordinate of the bounding box
    uint32_t topx;
    /// top left y coordinate of the bounding box
    uint32_t topy;
    /// width of the bounding box
    uint32_t width;
    /// height of the bounding box
    uint32_t height;
    /// face ID for tracking
    uint32_t ID;

    //distance for car based on camera height and camera focus;
    float distance; // meters.
    float TTC; //time to collision
    //
    //  clock_t curr_time_stamp;


} cvFacedetectResult;

// -----------------------------------------------------------------------------
/// @brief
///   Face detection parameter structure
///
/// @remark
///
// -----------------------------------------------------------------------------
typedef struct
{

    uint32_t minSize;

    uint32_t maxSize;

    uint32_t stepSize;

    //   float32_t scaleFactor;

    float32_t focal_length;//mm
    float32_t camera_height;//height, meters


} cvFacedetectParameters;


bool object_detection_process(uint8_t *__restrict src,
                              uint32_t srcWidth,
                              uint32_t srcHeight,
                              uint32_t srcStride,
                              cvFacedetectParameters *para,
                              uint32_t maxDetectedFaceNum,
                              uint32_t *resultFaceNum,
                              cvFacedetectResult *result
                             );

bool init_object_detection(uint32_t Height, uint32_t Width, cvFacedetectParameters *params);

void deinit_object_detection(void);

int set_param(char *name, float value);

#endif
