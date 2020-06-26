#include "faceDetection.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ObjDetectorAPI.h"
#include "math.h"

//#include "kuart.h"
cvFacedetectParameters detParam;
int _height;
int _width;
float _fov;
float _disWidthProduct; //pixels in image * distance = Constant, set the constant
int fd_detect_face(uint8_t *frame, int width, int height, FaceDetected  *faces, uint32 *count)
{

    cvFacedetectResult result[25];
    float adjacentLength = ((_width * 0.5f)  /  tan(_fov * 0.5f * PI / 180.0f));  //ctg
    float oppositeLength;
    object_detection_process(frame, width, height, width, &detParam,  20, count, result);

    uint32 i = 0;
    for (; i < *count; i++)
    {
        faces[i].x = result[i].topx;
        faces[i].y = result[i].topy;
        faces[i].width = result[i].width;
        faces[i].height = result[i].height;

        faces[i].distance = _disWidthProduct / (result[i].height);

        oppositeLength = (faces[i].x + faces[i].width * 0.5) - (_width * 0.5) ;
        faces[i].angle = atan(oppositeLength / adjacentLength) * 180 / PI;

    }
    return 0;
}

int fd_detect_face_enhanced(uint8 *frame, FaceDetectedCircle *faces, unsigned int *count)
{
    //cout<<"use fd_detect_face_enhanced"<<endl;
    uint32 local_count = 0;
    cvFacedetectResult result[25];
    int i = 0;
    *count = 0;

    object_detection_process(frame, _width, _height, _width, &detParam,  20, &local_count, result);
    if (local_count > 0)
    {
        //cout<<"local_cout > 0"<<endl;
        *count =  local_count;
        for (; i < local_count; i++)
        {
            faces[i].x = result[i].topx + 0.5 * result[i].width;
            faces[i].y = result[i].topy + 0.5 * result[i].height;
            faces[i].radius = result[i].width * 0.5; // inner bounding circle of the bounding rectangular box
        }
        return 0;
    }

    return 0;
}



void fd_free()
{
    deinit_object_detection();
}

int fd_init(int width, int height)
{

    bool ret = init_object_detection(height, width, &detParam);
    _width = width;
    _height = height;


    if (ret == false)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int  fd_set_param(char *name, float value)
{
    if (strcmp(name, "alertFactor") == 0 || strcmp(name, "alertfactor") == 0)
    {
        set_param(name, value);
        return 0;
    }
    else if (strcmp(name, "speedFactor") == 0 || strcmp(name, "speedfactor") == 0)
    {
        set_param(name, value);
        return 0;
    }
    else if (strcmp(name, "fov") == 0 || strcmp(name, "FOV") == 0)
    {
        _fov = value;
        return 0;
    }
    else if (strcmp(name, "disWidthProduct") == 0 || strcmp(name, "diswidthproduct") == 0)
    {
        _disWidthProduct = value;
        return 0;
    }
    else
    {
        return -1;
    }

}
