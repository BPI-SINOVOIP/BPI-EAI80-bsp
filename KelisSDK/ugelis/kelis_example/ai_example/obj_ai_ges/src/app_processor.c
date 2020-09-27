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

#include "app_processor.h"
#include <imlib.h>

/**
*
* This function run average pooling
*
* @param a array
*
* @return
*
*
* @note     None.
*
******************************************************************************/

static int nms_comparator(const void *pa, const void *pb)
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

/*--------------------buble Sort ---------------------*/
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

/*Feed rrggbb format to KDP310 for further training */
void rgb_split2rrggbb(uint8_t *src, uint8_t *dst, int w, int h)
{
    uint8_t *dst_b = &(dst[0]);
    uint8_t *dst_g = &(dst[w * h]);
    uint8_t *dst_r = &(dst[w * h * 2]);

    int c = 0, l = 0, i = 0;
    uint8_t *src_rgb = 0;
    for (l = 0; l < h; l++)
    {
        for (c = 0; c < w; c++)
        {
            src_rgb = src + (l * w + c) * 3;
            *(dst_b + i) = *(src_rgb + 2);    //b
            *(dst_g + i) = *(src_rgb + 1);    //g
            *(dst_r + i) = *(src_rgb);        //r
            i++;
        }
    }
}

float gm_exp(float x)
{
    x = 1.0 + x / 256.0;
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

void rrggbb_split2rgb(uint8_t *src, uint8_t *dst, int w, int h)
{
    int c = 0, l = 0, i = 0;
    uint8_t *src_rgb = src;
    uint8_t *dst_rgb = dst;

    for (i = 0; i < w * h; i ++)
    {
        *dst_rgb = *(src_rgb +  w * h * 2 + i);
        dst_rgb ++;
        *dst_rgb = *(src_rgb + w * h + i);
        dst_rgb ++;
        *dst_rgb = *(src_rgb + i);
        dst_rgb ++;
    }
}

int imlib_image_mean(image_t *src, int threshold, int *mean, int *mean_dev)
{
    int w = src->w;
    int h = src->h;
    int n = w * h;
    uint8_t *data = src->pixels;

    uint32_t s = 0;
    int sum = 0;

    for (int i = 0; i < n; i += 2)
    {
        s += data[i + 0] + data[i + 1];
    }

    if (n % 2)
    {
        s += data[n - 1];
    }

    /* mean */
    *mean = s / n - threshold;

    for (int j = 0; j < n; j++)
    {
        int diff = data[j] - 128;
        int abs_tmp = abs(diff - (*mean));
        sum += abs_tmp;
    }

    *mean_dev = sum / n;
}

int PostYolo(ObjDetData *p_objdet_data, short int *p_OutputImage, float *d_box, float *box_class_probs, Box_t *s_box)
{
    int i = 0, j = 0, r = 0, c = 0, channel = 0, maxIndex = 0;
    float box_x = 0, box_y = 0, box_w = 0, box_h = 0, box_confidence = 0, sum = 0, maxData = 0 ;
    float thresh = 0;
    int out_width, out_height, box_channel, confidence_channel, box_num, class_channel ;
    float nms_thresh;
    int NumClasses, gap;
    int cnt = 0;
    float anchers[5][2];
    Box_t *head = NULL;
    Box_t *previous = NULL;
    float *box = d_box;
    Box_t *p = NULL;
    int flag = 0;
    int tmp_pkt = 0;

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
        out_width = 7;
        out_height = 7;
        box_channel = 4;
        confidence_channel = 1;
        box_num = 5;
        class_channel = 1;
    }
    else if (p_objdet_data->det_type == DET_GEST)
    {
        out_width = 7;
        out_height = 7;
        box_channel = 4;
        confidence_channel = 1;
        box_num = 5;
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
            OBJ_DBG_PRINTF("class: %d\n\r", i);
            OBJ_DBG_PRINTF("score: %lf\n\r", s[0].score);
            OBJ_DBG_PRINTF("box_y_min: %lf\n\r", s[0].box[0] < 0 ? 0 : s[0].box[0]);
            OBJ_DBG_PRINTF("box_x_min: %lf\n\r", s[0].box[1] < 0 ? 0 : s[0].box[1]);
            OBJ_DBG_PRINTF("box_y_max: %lf\n\r", s[0].box[2] < 0 ? 0 : s[0].box[2]);
            OBJ_DBG_PRINTF("box_x_max: %lf\n\r", s[0].box[3] < 0 ? 0 : s[0].box[3]);

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
