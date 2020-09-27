/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        tinyyolo_demo.c
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       TINYYOLO_DEMO Source file For easynet
 *
 * @note
 *              2019/01/24, Angrad.Yang, v1.0.0
 *                  Initial version.
 *
 **/

#include "network.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "ops.h"
#include "post_yolo.h"
#include "image.h"

#define MAX_COMPILE_BUFFER_SIZE (100*1024*1024)
#define LAYER_BUFFER_SIZE   0x400000
#define LAYER_BUFFER_NUM    6
#define MAX_OUTPUT_SIZE 1024*1024

char *img_bin_fix  = "./examples/compiler/input_fix8.bin";

//#define DEBUG_WEIGHT_NPU

//#define DEBUG_GES 1
//#define DEBUG_PERSON 1
//#define DEBUG_FACE  1
//#define DEBUG_YOLO_V2 1

//#define DEBUG_POSENET

//#define TINY_YOLO_V2
//#define MOBILENET
//#define MOBILENET_SSD
#define INSIGHTFACE  1

#ifdef DEBUG_WEIGHT_NPU
    #ifdef  DEBUG_GES
        char *img_bin_file = "./examples/compiler/input.bin";
        char *cfg_file     = "/Resource/richardliu/EasyNet/easynet_new_20190423/easynet_up/easynet/examples/compiler/ges_model/ges.cfg";
        //char *cfg_file     = "./examples/compiler/face.cfg";
        char *weight_file  = "/Resource/richardliu/EasyNet/easynet_new_20190423/easynet_up/easynet/examples/compiler/ges_model/ges_full_weights.bin";
        //char *weight_file  = "./examples/compiler/face.bin";
        char *exe_bin_file   = "/Resource/richardliu/EasyNet/easynet_new_20190423/easynet_up/easynet/examples/compiler/ges_model/ges_debug.ops";
        char *boxes_file   = "./examples/compiler/bbox.txt";
        char *EXE_CMD      = "python ./examples/compiler/draw_boxes.py ./examples/compiler/fisheye_3.jpg ./examples/compiler/bbox.txt";
    #endif

    #ifdef  DEBUG_PERSON
        char *img_bin_file = "./examples/compiler/input.bin";
        char *cfg_file     = "./examples/compiler/person.cfg";
        char *weight_file  = "./examples/compiler/person.weights";
        char *exe_bin_file   = "./examples/compiler/person_debug.ops";
        char *boxes_file   = "./examples/compiler/bbox.txt";
        char *EXE_CMD      = "python ./examples/compiler/draw_boxes.py ./examples/compiler/fisheye_3.jpg ./examples/compiler/bbox.txt";
    #endif

    #ifdef  DEBUG_FACE
        char *img_bin_file = "./examples/compiler/input.bin";
        char *cfg_file     = "/Resource/richardliu/EasyNet/easynet_new_20190423/easynet_up/easynet/examples/compiler/face_model/face.cfg";
        char *weight_file  = "/Resource/richardliu/EasyNet/easynet_new_20190423/easynet_up/easynet/examples/compiler/face_model/face.bin";
        char *exe_bin_file   = "/Resource/richardliu/EasyNet/easynet_new_20190423/easynet_up/easynet/examples/compiler/face_model/face_debug.ops";
        char *boxes_file   = "./examples/compiler/bbox.txt";
        char *EXE_CMD      = "python ./examples/compiler/draw_boxes.py ./examples/compiler/fisheye_3.jpg ./examples/compiler/bbox.txt";
    #endif

#else
    #ifdef DEBUG_YOLO_V2
        char *img_file     = "./examples/compiler/yolo_v2/dog.jpg";
        char *img_bin_file = "./examples/compiler/yolo_v2/input.bin";
        char *cfg_file     = "./examples/compiler/yolo_v2/keras2easynet.cfg";
        char *weight_file  = "./examples/compiler/yolo_v2/keras2easynet.weights";
        char *exe_bin_file = "./examples/compiler/yolo_v2/keras2easynet.ops";
        char *boxes_file   = "./examples/compiler/yolo_v2/bbox.txt";
        char *EXE_CMD      = "python ./examples/compiler/yolo_v2/draw_boxes.py ./examples/compiler/yolo_v2/dog.jpg ./examples/compiler/yolo_v2/bbox.txt";
    #endif

    #ifdef DEBUG_POSENET
        char *img_bin_file = "./examples/compiler/posenet/input.bin";
        char *cfg_file     = "./examples/compiler/posenet/keras2easynet.cfg";
        char *weight_file  = "./examples/compiler/posenet/keras2easynet.weights";
        char *exe_bin_file = "./examples/compiler/posenet/keras2easynet.ops";
        char *boxes_file   = "./examples/compiler/posenet/bbox.txt";
        char *EXE_CMD      = "python ./examples/compiler/posenet/draw_boxes.py ./examples/compiler/posenet/person.jpg ./examples/compiler/posenet/bbox.txt";
    #endif
    #ifdef TINY_YOLO_V2
        char *img_bin_file = "./examples/compiler/tiny_yolo_v2/input.bin";
        char *cfg_file     = "./examples/compiler/tiny_yolo_v2/keras2easynet.cfg";
        char *weight_file  = "./examples/compiler/tiny_yolo_v2/keras2easynet.weights";
        char *exe_bin_file = "./examples/compiler/tiny_yolo_v2/keras2easynet.ops";
        char *boxes_file   = "./examples/compiler/tiny_yolo_v2/bbox.txt";
        char *EXE_CMD      = "python ./examples/compiler/tiny_yolo_v2/draw_boxes.py ./examples/compiler/tiny_yolo_v2/fisheye_3.jpg ./examples/compiler/tiny_yolo_v2/bbox.txt";
    #endif
    #ifdef MOBILENET
        char *img_bin_file = "./examples/compiler/mobilenet/input.bin";
        char *cfg_file     = "./examples/compiler/mobilenet/keras2easynet.cfg";
        char *weight_file  = "./examples/compiler/mobilenet/keras2easynet.weights";
        char *exe_bin_file = "./examples/compiler/mobilenet/keras2easynet.ops";
        char *boxes_file   = "./examples/compiler/mobilenet/bbox.txt";
        char *EXE_CMD      = "python ./examples/compiler/mobilenet/draw_boxes.py ./examples/compiler/mobilenet/person.jpg ./examples/compiler/mobilenet/bbox.txt";
    #endif
    #ifdef MOBILENET_SSD
        char *img_file     = "./examples/compiler/mobilenet_ssd/dog_300X300.jpg";
        char *img_bin_file = "./examples/compiler/mobilenet_ssd/input.bin";
        char *cfg_file     = "./examples/compiler/mobilenet_ssd/keras2easynet.cfg";
        char *weight_file  = "./examples/compiler/mobilenet_ssd/keras2easynet.weights";
        char *exe_bin_file = "./examples/compiler/mobilenet_ssd/keras2easynet.ops";
        char *boxes_file   = "./examples/compiler/mobilenet_ssd/bbox.txt";
        char *EXE_CMD      = "python ./examples/compiler/mobilenet_ssd/draw_boxes.py ./examples/compiler/mobilenet_ssd/dog_300X300.jpg ./examples/compiler/mobilenet_ssd/bbox.txt";
        char *uchar_img_bin_file = "./examples/compiler/mobile_ssd_uchar_small.bin";
    #endif
    #ifdef INSIGHTFACE
        char *img_file     = "./examples/compiler/insightface/0015_01.jpg";
        char *img_bin_file = "./examples/compiler/insightface/input.bin";
        char *cfg_file     = "./examples/compiler/insightface/keras2easynet.cfg";
        char *weight_file  = "./examples/compiler/insightface/keras2easynet.weights";
        char *exe_bin_file = "./examples/compiler/insightface/keras2easynet.ops";
        char *boxes_file   = "./examples/compiler/insightface/bbox.txt";
        char *EXE_CMD      = "python ./examples/compiler/insightface/draw_boxes.py ./examples/compiler/insightface/0015_01.jpg ./examples/compiler/insightface/bbox.txt";
        char *uchar_img_bin_file = "./examples/compiler/faceinsight_96x96_uchar_small.bin";

    #endif
#endif

float conv_output[MAX_OUTPUT_SIZE];
uint8_t ConvBuffer[LAYER_BUFFER_NUM * LAYER_BUFFER_SIZE];

void load_float_input(char *filename, float *X, int size)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        file_error(filename);
    }

    fread(X, sizeof(float), size, fp);

    fclose(fp);
}

void save_exe_bin(char *filename, void *X, int size)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        file_error(filename);
    }

    fwrite(X, 1, size, fp);

    fclose(fp);
}

void load_input(char *filename, float *X, int size)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        file_error(filename);
    }

    fread(X, sizeof(data_t), size, fp);

    fclose(fp);
}

void load_convert_uint16(float *X, uint16_t *Y, int size)
{
    int i;
    float temp;

    for (i = 0; i < size; i++)
    {
        temp = X[i] * 256;
        temp += (temp >= 0) ? 0.5f : -0.5f;
        Y[i] = (unsigned short int)temp;
    }
}

void load_convert_uint16_V2(float *X, uint16_t *Y, int size)
{
    int i;
    float temp;

    for (i = 0; i < size; i++)
    {
        temp = X[i] * 255;
        Y[i] = (unsigned short int)temp;
    }
}

void save_input(char *filename, data_t *X, int size)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        file_error(filename);
    }

    fread(X, sizeof(data_t), size, fp);

    fclose(fp);
}

void postyolo(struct network *net)
{

    int nboxes;
    int i, k;
    struct layer l = net->layers[net->n - 1];
    float thresh = 0.3;
    float nms_thresh = 0.3;
    detection *dets = post_yolo(net, &nboxes, thresh, nms_thresh, net->w, net->h);

    FILE *fw = fopen(boxes_file, "w");
    char data[64];
    for (k = 0; k < l.classes; ++k)
    {
        for (i = 0; i < nboxes; i++)
        {
            if (dets[i].prob[k] > thresh)
            {
                //xmin ymin xmax ymax classes score;
                sprintf(data, "%f %f %f %f,%d,%f;", dets[i].bbox.x, dets[i].bbox.y, dets[i].bbox.w, dets[i].bbox.h, k, dets[i].prob[k]);
                printf("%s\n", data);
                fwrite(data, sizeof(char), strlen(data), fw);
            }
        }
    }
    fclose(fw);

    post_yolo_free(dets, nboxes);

    char *exe_cmd = EXE_CMD;
    printf("%s\n", exe_cmd);
    system(exe_cmd);

}

struct easynet_ops_param *kdp_ops_compile(struct easynet_dev *dev, network *net, void *bin, uint32_t max_len);
extern int g_tiny_yolov3_flag;
int main(void)
{
    float *X;
    uint16_t *Y;
    network net;
    parser *ps;
    struct easynet_dev *dev;
    void *ops;
    int len, ret;
    int i, out_ch, ch, row, col;
    struct easynet_dev_cfg dev_cfg;

    //printf("start tinyyolo demo\n");

#ifdef _WIN64
    setvbuf(stdout, NULL, _IONBF, 0);
#endif
    //g_tiny_yolov3_flag = 1;


    ps = parser_get("easynet");

    net = ps->load_model(cfg_file);

#ifdef DEBUG_WEIGHT_NPU
    ps->load_param_ex(&net, weight_file, WEIGHT_NPU);
#else
    //ps->load_param(net, weight_file);
    ps->load_param_ex(&net, weight_file, WEIGHT_DEFAULT);

#endif

    int in_cnt = net.w * net.h * net.in_ch;

    X = malloc(sizeof(data_t) * in_cnt);
    Y = malloc(sizeof(uint16_t) * in_cnt);

#if defined(DEBUG_YOLO_V2) || defined(MOBILENET_SSD)
    image im = load_image_color(img_file, 0, 0);
    image sized = letterbox_image(im, net.w, net.h);
    X = sized.data;
#if 0
    load_convert_uint16_V2(X, Y, in_cnt);

    printf("in_cnt = %d\n", in_cnt);

    unsigned char *u8_buf = (unsigned char *)malloc(sizeof(uint8_t) * in_cnt * 2);
    convert_float2uint8(X, u8_buf, in_cnt);
    save_char_binary(uchar_img_bin_file, u8_buf, in_cnt);
#else
    load_input_byte2unit16(uchar_img_bin_file, Y, in_cnt);
#endif
#elif defined(INSIGHTFACE)
    image im0 = facenet_image_proc(img_file, 96, 96);
    X = (float *)im0.data;
    load_convert_uint16_V2(X, Y, in_cnt);
    int dst_len = (96 * 96 * 3);
#if 0
    uint8_t *u8_dst = (uint8_t *)malloc(sizeof(uint8_t) * dst_len);;
    convert_float2uint8(X, u8_dst, dst_len);
    save_char_binary(uchar_img_bin_file, u8_dst, dst_len);
#else
    load_input_byte2unit16(uchar_img_bin_file, Y, dst_len);
#endif

#else
    load_input(img_bin_file, X, in_cnt);
    load_convert_uint16(X, Y, in_cnt);
#endif
    //network_predict(&net, X);
    //postyolo(&net);
#if 0
    forward_network_step_start(net, X);

    for (i = 0; i < net.n; i++)
    {
        layer l = net.layers[i];
        forward_network_one_layer(i);
        if (l.type == CONVOLUTIONAL)
        {
            for (out_ch = 0; out_ch < l.out_ch; out_ch++)
            {
                for (ch = 0; ch < l.in_ch; ch++)
                {
                    for (row = 0; row < l.out_h; row++)
                    {
                        for (col = 0; col < l.out_w; col++)
                        {
                            conv_verify_step(out_ch, ch, row, col, 0.0, 0.0);
                        }
                    }
                }
            }
        }

    }

    post_yolo(net);
#endif
#if 1
    forward_network_step_start(net, X);

    //network_predict(net, X);
    dev_cfg.bn_epsilon = 0.0; /* Used default epsilon */
    dev_cfg.optimization = OPTIMIZATION_LEVEL_1;
    dev_cfg.layer_buffer_num = LAYER_BUFFER_NUM;
    for (i = 0; i < LAYER_BUFFER_NUM; i++)
    {
        dev_cfg.layer_buffer[i] = ConvBuffer + i * LAYER_BUFFER_SIZE;
    }
    dev = easynet_ops_init(&dev_cfg);



    /* Allocate compiling buffer */
    ops = malloc(MAX_COMPILE_BUFFER_SIZE);

    /* Compile network into ops stream */
    len = easynet_ops_get(dev, &net, ops, MAX_COMPILE_BUFFER_SIZE);

    save_exe_bin(exe_bin_file, ops, len);

    /* Load ops stream */
    easynet_ops_load(dev, ops, len);

    easynet_ops_start(dev, (void *)Y, (void *)conv_output);

    /* Process all ops */
    do
    {
        ret = easynet_ops_process(dev);

        if (ret == RET_OVER || ret == RET_ERROR)
        {
            break;
        }
    } while (1);



    layer l = net.layers[net.n - 1];
    net.input = conv_output;
    l.forward(l, net);

    postyolo(&net);

    free(ops);

#endif
    free(X);

    free_network(&net);

    printf("Bye!\n");
    return 0;
}
