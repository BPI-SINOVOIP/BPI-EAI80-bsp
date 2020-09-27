#ifndef __POST_YOLO_H__
#define __POST_YOLO_H__

//#define OBJ_PRINTF printk
#define OBJ_PRINTF(...)  do{} while(0);
//#define OBJ_DBG_PRINTF printf
#define OBJ_DBG_PRINTF do{} while(0);
typedef enum
{
    RELU, LEAKY, LOGISTIC, LINEAR
} ACTIVATION;

typedef enum
{
    CONVOLUTIONAL,
    DECONVOLUTIONAL,
    CONNECTED,
    MAXPOOL,
    SOFTMAX,
    DETECTION,
    DROPOUT,
    CROP,
    ROUTE,
    COST,
    NORMALIZATION,
    AVGPOOL,
    LOCAL,
    SHORTCUT,
    ACTIVE,

    BATCHNORM,
    NETWORK,

    REGION,
    YOLO,

    REORG,
    UPSAMPLE,
    LOGXENT,

    FLATTEN,
    MTCNN_PNET,
    MTCNN_RNET_ONET,

    DEPTHWISE_CONVOLUTIONAL,
    BLANK,
    GAUSSIAN_YOLO
} LAYER_TYPE;

enum
{
    ENTRY_X          = 0,
    ENTRY_Y          = 1,
    ENTRY_W          = 2,
    ENTRY_H          = 3,
    ENTRY_CONFIDENCE = 4,
    ENTRY_CLASS      = 5
};

typedef struct
{
    float x, y, w, h;
} box;

typedef struct detection
{
    box bbox;
    int classes;
    float *prob;
    float *mask;
    union
    {
        float objectness;
        float confidence;
    };
    int sort_class;
    float *uc; // Gaussian_YOLOv3 - tx,ty,tw,th uncertainty
} detection;

typedef struct layer_post
{
    int h;
    int w;
    int out_ch;             //output channel
    float *biases;
    float *output;
    int out_cnt;
    //yolo
    int classes;
    int *mask;
    float *softmax_tmp;
    int coords;
    int post_type;
    int softmax;
} layer_post;

typedef struct network
{
    int n;
    int h;
    int w;
    layer_post *layers;
} network;

#define SWAPINIT(a, es) swaptype = ((char *)a - (char *)0) % sizeof(long) || \
                                   es % sizeof(long) ? 2 : es == sizeof(long)? 0 : 1;

/*
 * Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".
 */
#define swapcode(TYPE, parmi, parmj, n) {               \
        size_t i = (n) / sizeof (TYPE);                 \
        TYPE *pi = (TYPE *) (parmi);                    \
        TYPE *pj = (TYPE *) (parmj);                    \
        do {                                            \
            TYPE    t = *pi;                        \
            *pi++ = *pj;                            \
            *pj++ = t;                              \
        } while (--i > 0);                              \
    }

static __inline void swapfunc(char *a, char *b, size_t n, int swaptype)
{
    if (swaptype <= 1)
    {
        swapcode(long, a, b, n)
    }
    else
        swapcode(char, a, b, n)
    }

#define swap(a, b)                                      \
    if (swaptype == 0) {                            \
        long t = *(long *)(a);                  \
        *(long *)(a) = *(long *)(b);            \
        *(long *)(b) = t;                       \
    } else                                          \
        swapfunc(a, b, es, swaptype)

#define vecswap(a, b, n)        if ((n) > 0) swapfunc(a, b, n, swaptype)

detection *easynet_post_yolo(struct network *net, int *nboxes, float score_thresh, float nms_thresh, int image_w, int image_h);
void easynet_post_yolo_free(detection *dets, int nboxes);

#endif
