/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        tinyyolov3_simu.c
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2019/09/25
 *
 * @brief       The simulator for tinyyolo v3.
 *
 * @note
 *              2019/09/25, Angrad.Yang, v1.0.0
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
#include "kdp310/kdp.h"
#include "dump_network.h"
#include "operator.h"
#define LAYER_BUFFER_NUM    6
#define MAX_COMPILE_BUFFER_SIZE (100*1024*1024)
#define LAYER_BUFFER_SIZE   0x400000

char *CONFIG_FILE  = "./examples/tinyyolov2_simu/keras2easynet.cfg";
char *WEIGHT_FILE  = "./examples/tinyyolov2_simu/keras2easynet.wt";
char *BOXES_FILE   = "./examples/tinyyolov2_simu/bbox.txt";

char *INPUT_FILE   = "./examples/tinyyolov2_simu/tinyyolov2_input_float_picture.bin";

char *INPUT_FILE_UCHAR   = "./examples/tinyyolov2_simu/tinyyolov2_uchar_picture.bin";

#define EXE_CMD     "python ../res/script/draw_boxes.py --img ../res/pic/fist_504.jpg "\
    "--box ./examples/tinyyolov2_simu/bbox.txt --yolo v3"
char *exe_bin_file2   = "./examples/tinyyolov2_simu/tinyyolov2_m_cross_ops.bin";

network *gNet;
easynet_t *gParam;

int g_ges_output_sel = 0;

void simudata_dump(struct easynet_dev *dev, int16_t *output, int index)
{
    int layer_buf_index = (int)output;
    //struct conv_hw_context *cxt;
    int c, w, h;
    char filename[64];
    //cxt = ((struct conv_hw_context *)dev->context);

    //int16_t *data = cxt->layer_buffer[layer_buf_index];
    int16_t *data = gParam->dev_cfg.layer_buffer[layer_buf_index];
    layer l = gNet->layers[index];
    printf("layer index %d\n", index);
    //printf("addr:%x %dx%dx%d\n", data, l.out_ch, l.out_h, l.out_w);
    sprintf(filename, "%03d_[%s]_%d_%d_%d.txt", index, layer_type_to_string(l.type),
            l.out_ch, l.out_h, l.out_w);

    printf("file %s\n", filename);
    dump_data dd;
    dd.cnt = l.out_ch * l.out_h * l.out_w;
    dd.dat = data;
    dd.src_type = DumpDataSrcFix2Float;
    dd.dst_type = DumpDataDstTxt;
    dd.file_name = filename;
    dump(&dd);

}

void postregion(struct network *net, int w, int h, float thresh)
{
    int nboxes;
    int i, k;
    float nms = 0.45;

    struct layer l = net->layers[net->n - 1];


    detection *dets = get_network_boxes(net, 224, 224, thresh,  &nboxes);

    if (nms)
    {
        do_nms_sort(dets, nboxes, l.classes, nms, net->yolo_version);
    }

    FILE *fw = fopen(BOXES_FILE, "w");
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

void forward_region_layer(const layer l, network net);


char *region_bin_file   = "./examples/tinyyolov2_simu/tinyyolov2_region_.bin";

static void save_exe_bin(char *filename, void *X, int size)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        file_error(filename);
    }
    fwrite(X, 1, size, fp);
    fclose(fp);
}


void postregion_prepare(network *net, void *context)
{
    int i = 0;
    int k = 0;
    char filename[32];
    struct conv_hw_context *hw_cxt = (struct conv_hw_context *)context;
    int yolo_idx = 0;

    for (i = 0; i < net->n; ++i)
    {
        layer *l = net->layers + i;
        if (l->type == REGION)
        {
            printf("step2\n");
            //save_exe_bin(region_bin_file, hw_cxt->layer_buffer[0], l->out_cnt * 4);
            short int *tmp_input = (short int *)hw_cxt->layer_buffer[1];
            if (g_ges_output_sel == OPTIMIZATION_LEVEL_0)
            {
                tmp_input = (short int *)hw_cxt->layer_buffer[1];
            }


            for (k = 0; k < l->out_cnt; k++)
            {
                net->input[k] = l->output[k] = (float)tmp_input[k] / 256;
            }

            forward_region_layer(*l, *net);
        }
    }

    printf("step9\n");
}

void config_debughelper(easynet_t *param)
{
    struct conv_hw_context *cxt;

    /* Configurate debug info */
    cxt = ((struct conv_hw_context *)param->dev->context);
    cxt->debug_helper.dst_hw_idx      = 0;
    cxt->debug_helper.dst_conv_out_ch = 0;
    cxt->debug_helper.dst_conv_idx    = 0;
    cxt->debug_helper.dst_spad_off    = 0;
    cxt->debug_helper.print_cnt       = 30;
    cxt->debug_helper.enable          = 0;
}

void dump_input(easynet_t *param)
{
    network *net = param->net;
    /* Dump input */
    dump_data dd;
    dd.cnt = net->in_ch * net->w * net->h;
    dd.dat = net->input;
    dd.src_type = DumpDataSrcFix2Float;
    dd.dst_type = DumpDataDstTxt;
    dd.file_name = "input.txt";
    dump(&dd);

}

uint16_t *prepare_input(easynet_t *param)
{
    int in_cnt;
    network *net = param->net;
    uint16_t *input;

    uint8_t *input_uchar;

    /* 3. Get model input */
    in_cnt = net->w * net->h * net->in_ch;

    float *X = malloc(sizeof(float) * in_cnt);

    input = calloc(in_cnt, sizeof(uint16_t));

    input_uchar = calloc(in_cnt, sizeof(uint8_t));

#if 0
    load_float_binary(INPUT_FILE, X, in_cnt);
    convert_float2uint16(X, input, in_cnt);

    convert_float2uint8(X, input_uchar, in_cnt);

    char *save_uchar_bin_file   = "./examples/tinyyolov2_simu/tinyyolov2_uchar_picture.bin";
    save_exe_bin(save_uchar_bin_file, input_uchar, in_cnt);
#else
    load_input_byte2unit16(INPUT_FILE_UCHAR, input, in_cnt);
#endif

    free(X);

    return input;
}

int main()
{

    int len;
    int16_t *input;
    easynet_t param;

    float thresh = 0.5;

    /* Allocate essential memory for simulator */
    easynet_simu_init(&param);

    param.config_file = CONFIG_FILE;
    param.weight_file = WEIGHT_FILE;
    param.weight_type = WEIGHT_DEFAULT;
    param.dev_cfg.optimization = OPTIMIZATION_LEVEL_2;
    g_ges_output_sel = param.dev_cfg.optimization;
    param.dev_cfg.bn_square_sigma_only = 0;

    param.dev_cfg.layer_buffer_num = LAYER_BUFFER_NUM;

    param.image_w = 224;
    param.image_h = 224;

    /* Parse model and prepare for compile */
    easynet_init(&param);

    /* Compile network to operations */
    easynet_compile(&param);
    save_exe_bin(exe_bin_file2, param.ops, param.ops_len);

    /* Config debughelper before simulate */
    config_debughelper(&param);

    gNet = param.net;
    gParam = &param;
    param.dev->hw_cb = simudata_dump;

    input = prepare_input(&param);

    /* Forward simulation */
    easynet_simulate(&param, input);

    postregion_prepare(param.net, param.dev->context);

    postregion(param.net, param.image_w, param.image_h, thresh);

    /* De-initialized */
    free(input);
    easynet_deinit(&param);
    easynet_simu_deinit(&param);
}
