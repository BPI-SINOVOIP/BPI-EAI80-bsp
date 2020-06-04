#ifndef OBJ_DETECTORC_H
#define OBJ_DETECTORC_H


#include "ObjDetectorAPI.h"


#define CV_MAX(V1, V2)  ((V1)>(V2) ? (V1) : (V2))
#define CV_MIN(V1, V2)  ((V1)<(V2) ? (V1) : (V2))
#define SWAP(a,b) { float32_t t=(a);(a)=(b);(b)=t; }


#define MIN_OF_BB_SIZE 10
#define FACE_MODEL_N_STAGES            14
#define FACE_MODEL_X_SIZE              20
#define FACE_MODEL_Y_SIZE              20
#define FACE_MODEL_N_DEPTH              1
#define FACE_MODEL_N_CAT_COUNT         256
#define FACE_MODEL_N_FEATURE           444
#define FACE_MODEL_N_NODE_COUNT        1
#define FACE_MODEL_N_INTRENAL_NODE     8
#define FACE_MAX_CANDIDATE_RECT        100
#define DEFAULT_MIN_NEIGHBOR              2

void *malloc_ext(uint8_t memx, uint32_t size);
void free_ext(uint8_t memx, void *ptr);


typedef struct
{
    const uint16_t *p[2]; /* => changed to 2 pointers which is optimal for Q6. this might be different for other implementation */
} ifcvFeature;

int32_t absdef(int32_t a);
uint32_t icvGrouprectangular(icvRect *rectList, uint32_t rectNum, uint32_t groupThreshold, icvRect *groupResult);
uint32_t icvObjectDetection(uint8_t *__restrict src, uint32_t srcWidth, uint32_t srcHeight, uint32_t srcStride, cvFacedetectParameters *para, uint32_t maxDetectedFaceNum, uint32_t *resultFaceNum, cvFacedetectResult *result, void *scratchBuf);
//void     icvhistogramEqualizeImage( const uint8_t * __restrict src, uint32_t srcWidth, uint32_t srcHeight, uint32_t srcStride, uint8_t * __restrict dst);
//void     icvhistogramEqualizeImage();
void     cvIntegrateImage(const uint8_t *__restrict src, unsigned int srcWidth, unsigned int srcHeight, unsigned int srcStride, uint16_t *__restrict dst, unsigned int dstStride);
//void icvhistogramEqualizeImage(uint32_t srcWidth, uint32_t srcHeight, uint32_t srcStride, uint8_t * __restrict dst);
//id icvhistogramEqualizeImage(const uint8_t * __restrict src, uint32_t srcWidth, uint32_t srcHeight, uint32_t srcStride);
void icvhistogramEqualizeImage(const uint8_t *__restrict src, uint32_t srcWidth, uint32_t srcHeight, uint32_t srcStride, uint8_t *__restrict dst);
void     cvScaleDown(const uint8_t *__restrict src, uint32_t srcWidth, uint32_t srcHeight, uint32_t srcStride, uint8_t *__restrict dst, uint32_t dstWidth, uint32_t dstHeight, uint32_t dstStride);
float32_t median(float32_t *mediaArray, int32_t number, float32_t *medianBuf);
float32_t fast_median_sort(float32_t *medianArray, int32_t number);
// int32_t   icvRunFeature(ifcvFeature *feature, uint32_t offset, int32_t * confidance, int sumStride, uint32_t x,uint16_t num_n_stages);
void      icvPickFace(uint32_t detFaceNum, uint32_t resultFaceNum, icvRect *buff, cvFacedetectResult *result);

#endif
