/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        app_processor.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/10/15
 *
 * @brief       app_processor source file
 *
 * @note
 *              2019/10/15, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

//#include <easynet/easynet.h>
#include <gm_hal_vin.h>
#include <gm_hal_kdp.h>

#include "obj_recognize.h"
#include "inc/config.h"
#include "worker/painter_worker.h"
#include "processor.h"


#define INPUT_WIDTH         224
#define INPUT_HEIGHT        224

extern unsigned char WEI_BASE[];
extern unsigned short inputImage[];
extern unsigned short OutputImage[];

ObjDetData gobj_det_data;

static int max_body_index;
static int max_body_width;

#define MAX_OBJECTCLASS_CNT 20
#define MAX_GESTURES_CNT 2

static uint32_t object_colordrawn[MAX_OBJECTCLASS_CNT] =
{
    UI_FOCUS_COLOR,
    UI_FOCUS_COLOR2,
    0xFF00FF,
    0xFF0040,
    0x00FF40,
    0xFF0040,
    0xFF0080,
    0x00FF80,
    0xFF0080,
    0xFF00C0,
    0x00FFC0,
    0xFF00C0,
    0xFF00FF,
    0x00FFFF,
    0xFF00FF,
    0xFF40FF,
    0x40FFFF,
    0xFF40FF,
    0xFF80FF,
    0x80FFFF,
};


static void predict(easynet_proc_param_t *ep, unsigned char *ops, unsigned short *inputbuf, unsigned short *outputbuf)
{
    int ret = 0;

    struct easynet_dev *dev = ep->dev;

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    /* Process all ops */
    do
    {
        ret = easynet_ops_process(dev);

        if (ret == 0x06 || ret == 0x01)
        {
            if (ret == 0x06)
            {
                dev = easynet_process_update(ops, inputbuf, outputbuf);
            }
            break;
        }
    } while (1);

    HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
}

struct easynet_dev *ai_bodydetect_init(easynet_proc_param_t *ep, ObjDetData *pobj_det, unsigned char *ops, unsigned short *inputbuf, unsigned short *outputbuf)
{
    int i = 0;
    struct easynet_dev *dev;
    memset(inputbuf, 0x00, INPUT_WIDTH * INPUT_HEIGHT * 4 * sizeof(unsigned short));
    memset(outputbuf, 0x00, INPUT_WIDTH * INPUT_HEIGHT * 4 * sizeof(unsigned short));
    obj_recognized_data_t *data = (obj_recognized_data_t *)ep->data;

    pobj_det->boxcnt = 0;
    pobj_det->det_type     =  data->type;
    pobj_det->det_type_cpy = data->type;

    dev = easynet_process_init((char *)ops, inputbuf, outputbuf);

    /*Initial the SRAM specified for KDP310*/
    for (i = 0; i < 0x40000; i += 4)
    {
        *(volatile uint32_t *)(BRAM_OUTPUT_A_BASEARDDR + i) = 0x00000000;
    }

    return dev;
}

int nms_comparator(const void *pa, const void *pb)
{
    Box_t a = *(Box_t *)pa;
    Box_t b = *(Box_t *)pb;
    float diff = a.score - b.score;

    if (diff < 0)
    {
        return 1;
    }
    else if (diff > 0)
    {
        return -1;
    }
    return 0;
}

static float overlap(float l1, float r1, float l2, float r2)
{
    float left = l1 > l2 ? l1 : l2;
    float right = r1 < r2 ? r1 : r2;
    return right - left;

}

static float box_intersection(Box_t a, Box_t b)
{
    float w = overlap(a.box[1], a.box[3], b.box[1], b.box[3]);
    float h = overlap(a.box[0], a.box[2], b.box[0], b.box[2]);
    if (w < 0 || h < 0)
    {
        return 0;
    }
    float area = w * h;
    return area;
}

static float box_union(Box_t a, Box_t b)
{
    float i = box_intersection(a, b);
    float u = (a.box[2] - a.box[0]) * (a.box[3] - a.box[1]) + (b.box[2] - b.box[0]) * (b.box[3] - b.box[1]) - i;
    return u;
}

static float box_iou(Box_t a, Box_t b)
{
    float c;
    if (box_intersection(a, b) / box_intersection(a, a) > box_intersection(a, b) / box_intersection(b, b))
    {
        c = box_intersection(a, b) / box_intersection(a, a);
    }
    else
    {
        c = box_intersection(a, b) / box_intersection(b, b);
    }
    if (c < box_intersection(a, b) / box_union(a, b))
    {
        c = box_intersection(a, b) / box_union(a, b);
    }
    return c;

}

static void bubleSort(Box_t data[], int n)
{
    int i, j;
    Box_t temp;
    for (j = 0; j < n - 1; j++)
    {
        for (i = 0; i < n - j - 1; i++)
        {
            if (data[i].score < data[i + 1].score) //descent: < ascent: >
            {
                temp = data[i];
                data[i] = data[i + 1];
                data[i + 1] = temp;
            }
        }
    }
}

float gm_exp(float x)
{
    x = 1.0f + x / 256.0f;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    return x;
}

int post_yolo(ObjDetData *p_objdet_data, short int *p_OutputImage, float *d_box, float *box_class_probs, Box_t *s_box)
{
    int i = 0, j = 0, r = 0, c = 0, channel = 0, maxIndex = 0;
    float box_x = 0, box_y = 0, box_w = 0, box_h = 0, box_confidence = 0, sum = 0, maxData = 0 ;
    float thresh = 0;
    int box_channel, confidence_channel, class_channel;
    float nms_thresh;
    int NumClasses, gap;
    float anchers[5][2];
    Box_t *head = NULL;
    Box_t *previous = NULL;
    float *box = d_box;
    Box_t *p = NULL;
    int flag = 0;

    //for each class, select the corresponding box
    Box_t *s = s_box;

    if (d_box == NULL || box_class_probs == NULL || s_box == NULL)
    {
        return -1;
    }

    if (p_objdet_data->det_type == DET_BODY)
    {
        thresh = 0.5;
    }
    else if (p_objdet_data->det_type == DET_GEST)
    {
        thresh = 0.3;
    }

    if (p_objdet_data->det_type == DET_BODY)
    {
        box_channel = 4;
        confidence_channel = 1;
        class_channel = 1;
    }
    else if (p_objdet_data->det_type == DET_GEST)
    {
        box_channel = 4;
        confidence_channel = 1;
        class_channel = 2;
    }

    if (p_objdet_data->det_type == DET_BODY)
    {
        nms_thresh = 0.5;
        NumClasses = 1;
        gap = box_channel + confidence_channel + class_channel;
    }

    else if (p_objdet_data->det_type == DET_GEST)
    {
        nms_thresh = 0.3;

        NumClasses = class_channel;
        gap = box_channel + confidence_channel + class_channel;
    }

    memset(p_objdet_data->box, 0x00, sizeof(Box_t) * 100);
    p_objdet_data->boxcnt = 0;

    if (p_objdet_data->det_type == DET_BODY)
    {
        anchers[0][0] = 0.57;
        anchers[0][1] = 1.42;

        anchers[1][0] = 1.44;
        anchers[1][1] = 3.71;

        anchers[2][0] = 2.62;
        anchers[2][1] = 6.4;

        anchers[3][0] = 4.64;
        anchers[3][1] = 7.91;

        anchers[4][0] = 8.23;
        anchers[4][1] = 9.51;
    }

    else if (p_objdet_data->det_type == DET_GEST)
    {
        anchers[0][0] = 0.28;
        anchers[0][1] = 0.47;

        anchers[1][0] = 0.60;
        anchers[1][1] = 0.69;

        anchers[2][0] = 0.91;
        anchers[2][1] = 1.01;

        anchers[3][0] = 1.76;
        anchers[3][1] = 2.47;

        anchers[4][0] = 4.01;
        anchers[4][1] = 5.40;
    }

    for (channel = 0; channel < 5; ++channel)
    {
        for (r = 0; r < 7; ++r)
        {
            for (c = 0; c < 7; ++c)
            {
                //Extract data from CNN result
                box_x = (1 / (1 + gm_exp(-(float)(((short int)p_OutputImage[channel * gap * 7 * 7 + r * 7 + c]) / 256.0))) + c) / 7;
                box_y = (1 / (1 + gm_exp(-(float)(((short int)p_OutputImage[(channel * gap + 1) * 7 * 7 + r * 7 + c]) / 256.0))) + r) / 7;
                box_w = gm_exp((float)(((short int)p_OutputImage[(channel * gap + 2) * 7 * 7 + r * 7 + c]) / 256.0)) * anchers[channel][0] / 7;
                box_h = gm_exp((float)(((short int)p_OutputImage[(channel * gap + 3) * 7 * 7 + r * 7 + c]) / 256.0)) * anchers[channel][1] / 7;
                box_confidence = 1 / (1 + gm_exp(-(float)(((short int)p_OutputImage[(channel * gap + 4) * 7 * 7 + r * 7 + c]) / 256.0)));

                //box
                box[channel * 7 * 7 + r * 7 + c] = (box_y - (box_h / 2)) * 224;
                box[7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] = (box_x - (box_w / 2)) * 224;
                box[7 * 7 * 5 * 2 + channel * 7 * 7 + r * 7 + c] = (box_y + (box_h / 2)) * 224;
                box[7 * 7 * 5 * 3 + channel * 7 * 7 + r * 7 + c] = (box_x + (box_w / 2)) * 224;

                //get sum
                sum = 0;
                maxData = 0;
                maxIndex = 0;
                flag = 0;
                for (i = 0; i < NumClasses; ++i)
                {
                    box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] = gm_exp((float)(((short int)p_OutputImage[(channel * gap + 5 + i) * 7 * 7 + r * 7 + c]) / 256.0));
                    sum += box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c];
                }

                //find max data
                for (i = 0; i < NumClasses; ++i)
                {
                    box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] = box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] / sum * box_confidence;
                    if (box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] >= thresh && box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] > maxData)
                    {
                        maxData = box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c];
                        maxIndex = i;
                        flag = 1;
                    }
                }

                if (flag == 1)
                {
                    Box_t *currentBox = malloc_ext(0, sizeof(Box_t));
                    if (currentBox == NULL)
                    {
                        return -1;
                    }
                    currentBox->class = maxIndex;
                    currentBox->score = maxData;
                    currentBox->next = NULL;
                    for (i = 0; i < 4; ++i)
                    {
                        currentBox->box[i] = box[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c];
                    }
                    if (head == NULL)
                    {
                        head = currentBox;
                        previous = currentBox;
                    }
                    else
                    {
                        previous->next = currentBox;
                        previous = currentBox;
                    }
                }
            }
        }
    }

    Box_t *currentBox = head;

    for (i = 0; i < NumClasses; ++i)
    {
        currentBox = head;
        int Numbox = 0;
        while (currentBox != NULL)
        {
            if (currentBox -> class == i)
            {
                s[Numbox].score = currentBox->score;
                for (j = 0; j < 4; ++j)
                {
                    s[Numbox].box[j] = currentBox->box[j];
                }
                ++Numbox;
            }
            p = currentBox;
            currentBox = currentBox->next;
            if (p != NULL)
            {
                free_ext(0, p);
            }
            p = NULL;
        }
        if (Numbox == 1)
        {

            p_objdet_data->box[p_objdet_data->boxcnt] = s[0];
            p_objdet_data->box[p_objdet_data->boxcnt].class = i;
            p_objdet_data->boxcnt++;
            //printf("class: %d\n\r", i);
            //printf("score: %lf\n\r", s[0].score);
            //printf("box_y_min: %lf\n\r", s[0].box[0] < 0 ? 0 : s[0].box[0]);
            //printf("box_x_min: %lf\n\r", s[0].box[1] < 0 ? 0 : s[0].box[1]);
            //printf("box_y_max: %lf\n\r", s[0].box[2] < 0 ? 0 : s[0].box[2]);
            //printf("box_x_max: %lf\n\r", s[0].box[3] < 0 ? 0 : s[0].box[3]);

        }
        else if (Numbox >= 2)
        {
            bubleSort(s, Numbox);
            for (r = 0; r < Numbox; ++r)
            {
                for (c = r + 1; c < Numbox; ++c)
                {
                    if (box_iou(s[r], s[c]) > nms_thresh)
                    {
                        s[c].score = 0;
                    }
                }
            }
            for (r = 0; r < Numbox; ++r)
            {
                if (s[r].score > 0)
                {
                    p_objdet_data->box[p_objdet_data->boxcnt] = s[r];
                    p_objdet_data->box[p_objdet_data->boxcnt].class = i;
                    p_objdet_data->boxcnt ++;
                }
            }
        }
    }

    return 0;
}

void ai_body_detect(easynet_proc_param_t *ep, ObjDetData *pobj_det, unsigned char *ops, unsigned short *inputbuf, unsigned short *outputbuf)
{
    int out_width, out_height, box_channel,  box_num, class_channel ;
    uint32_t i = 0;
    float box_x_min, box_y_min, box_x_max, box_y_max;
    float *d_box = NULL;
    float *box_class_probs = NULL;
    Box_t *s_box = NULL;
    RET_CODE ret = RET_OK;
    paint_rect_t rect[3];
    memset(rect, 0, sizeof(paint_rect_t) * 3);
    obj_recognized_data_t *data = (obj_recognized_data_t *)ep->data;
    out_width = 7;
    out_height = 7;
    box_channel = 4;
    box_num = 5;
    class_channel = data->class_channel;

    d_box = (float *)kmalloc(out_width * out_height * box_num * box_channel * sizeof(float));
    box_class_probs = (float *)kmalloc(out_height * out_width * box_num * class_channel * sizeof(float));
    s_box = (Box_t *)kmalloc(20 * sizeof(Box_t));

    predict(ep, ops, inputbuf, outputbuf);

    ret = post_yolo(pobj_det, (short *)outputbuf, d_box, box_class_probs, s_box);
    if (ret != RET_OK)
    {
        printf("post_yolo failed\n");
    }
    max_body_index = -1;
    max_body_width = 0;

    for (i = 0; i < pobj_det->boxcnt; i ++)
    {
        //printf("class: %d\n\r", pobj_det->box[i].class);
        //printf("score: %lf\n\r", pobj_det->box[i].score);
        box_y_min = pobj_det->box[i].box[0] < 0 ? 0 : pobj_det->box[i].box[0];
        box_x_min = pobj_det->box[i].box[1] < 0 ? 0 : pobj_det->box[i].box[1];
        box_y_max = pobj_det->box[i].box[2] < 0 ? 0 : pobj_det->box[i].box[2];
        box_x_max = pobj_det->box[i].box[3] < 0 ? 0 : pobj_det->box[i].box[3];

        if (box_x_max > 223)
        {
            box_x_max = 223;
        }

        if (box_y_max > 223)
        {
            box_y_max = 223;
        }

        if (max_body_index == -1)
        {
            max_body_index = i;
            max_body_width = box_x_max - box_x_min;
        }
        else
        {
            if ((box_x_max - box_x_min) > max_body_width)
            {
                max_body_index = i;
                max_body_width = box_x_max - box_x_min;
            }
        }
        /*This will draw rectangle on Layer 1*/
        if (i < 3)
        {
            //printf("box_x_min: %lf\n\r", box_x_min);
            //printf("box_y_min: %lf\n\r", box_y_min);
            //printf("box_x_max: %lf\n\r", box_x_max);
            //printf("box_y_max: %lf\n\r", box_y_max);

            rect[i].x0  = (int)box_x_min;
            rect[i].y0  = (int)box_y_min;
            rect[i].x1  = (int)box_x_max;
            rect[i].y1  = (int)box_y_max;
            rect[i].wh_flag = 0;
            rect[i].rel = 1;
            rect[i].rel_w = 224;
            rect[i].rel_h = 224;
            rect[i].color = object_colordrawn[pobj_det->box[i].class] | 0xFF000000;
            rect[i].use   = 1;
        }
    }
    ep->painter->run(ep->painter, CMD_PAINT_RECT, rect);
    /*Set the address for LCD display, Layer 1, after rectangle & object are drawn*/
    HAL_VIN_Enable((VIN_Device_T *)0x40025000);

    kfree(s_box);
    kfree(d_box);
    kfree(box_class_probs);
}

void obj_recognize_thread(void *data, void *param, void *not_used)
{
    easynet_proc_param_t *ep = (easynet_proc_param_t *)data;
    int ret = 0;
    ep->painter = get_worker("painter");

    while (ep->running)
    {
        ret = k_sem_take(&ep->predict_lock, 1000);
        if (ret == 0)
        {
            ep->ops = (int)&WEI_BASE[0];
            //ui_draw_rgb(ep->dev->input, 10, 10, 224, 224);
            ai_body_detect(ep, &gobj_det_data, (unsigned char *)ep->ops, &inputImage[0], &OutputImage[0]);
            ep->process_cb(0, param);
            ep->processed = 1;
        }
    }
    ep->finish_cb(0, param);
}

int obj_recognize_init(easynet_proc_param_t *ep)
{
    ep->running = 1;
    ep->processed = 1;
    k_sem_init(&ep->predict_lock, 0, UINT_MAX);
    ep->dev = ai_bodydetect_init(ep, &gobj_det_data, WEI_BASE, &inputImage[0], &OutputImage[0]);
    return 0;
}

int obj_recognize_exit(easynet_proc_param_t *ep)
{
    ep->running = 0;
    return 0;
}

int obj_recognize_frame_in(easynet_proc_param_t *ep)
{
    if (ep->processed)
    {
        ep->dev->input = ep->in;
        //ui_draw_rgb(param->dev->input, 10, 10, 224, 224);
        ep->processed = 0;
        k_sem_give(&ep->predict_lock);
    }
    return 0;
}
