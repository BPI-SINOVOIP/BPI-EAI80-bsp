#include <stdlib.h>
#include "post_yolo.h"
#include <string.h>

static inline float exp_f(float x)
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

static inline float gm_min(float a, float b)
{
    return a < b ? a : b;
}

static float overlap(float x1, float x2, float x3, float x4)
{
    float l1 = x1 - x2 / 2;
    float l2 = x3 - x4 / 2;
    float left = l1 > l2 ? l1 : l2;
    float r1 = x1 + x2 / 2;
    float r2 = x3 + x4 / 2;
    float right = r1 < r2 ? r1 : r2;

    return right - left;
}

static float box_intersection(box a, box b)
{
    float w = overlap(a.x, a.w, b.x, b.w);
    float h = overlap(a.y, a.h, b.y, b.h);
    if (w < 0 || h < 0)
    {
        return 0;
    }
    float area = w * h;
    return area;
}

#if 0
float box_union_v3(box a, box b)
{
    float i = box_intersection(a, b);
    float u = a.w * a.h + b.w * b.h - i;
    return u;
}

static float box_iou_v3(box a, box b)
{
    return box_intersection(a, b) / box_union_v3(a, b);
}
#else
static float box_union_v3(box a, box b, float intersect)
{
    float w1, h1, w2, h2;
    w1 = a.w - a.x;
    h1 = a.h - a.y;
    w2 = b.w - b.x;
    h2 = b.h - b.y;
    float u = w1 * h1 + w2 * h2 - intersect;
    return u;
}

static float box_iou_v3(box a, box b)
{
    float c;
    float i_a = box_intersection(a, a);
    float i_b = box_intersection(b, b);
    float i_c = box_intersection(a, b);
    float iou_a = (i_a == 0) ? 0 : i_c / i_a;
    float iou_b = (i_b == 0) ? 0 : i_c / i_b;
    float iou_c = i_c / box_union_v3(a, b, i_c);

    if (iou_a > iou_b)
    {
        c = iou_a;
    }
    else
    {
        c = iou_b;
    }

    if (c < iou_c)
    {
        c = iou_c;
    }

    return c;

}
#endif
static int nms_comparator(const void *pa, const void *pb)
{
    detection a = *(detection *)pa;
    detection b = *(detection *)pb;
    float diff = 0;
    if (b.sort_class >= 0)
    {
        diff = a.prob[b.sort_class] - b.prob[b.sort_class];
    }
    else
    {
        diff = a.confidence - b.confidence;
    }
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

static  char *med3(char *a, char *b, char *c, int (*cmp)(const void *, const void *))
{
    return cmp(a, b) < 0 ?
           (cmp(b, c) < 0 ? b : (cmp(a, c) < 0 ? c : a))
           : (cmp(b, c) > 0 ? b : (cmp(a, c) < 0 ? a : c));
}

static void gm_qsort(void *aa, size_t n, size_t es, int (*cmp)(const void *, const void *))
{
    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    int cmp_result, swaptype, swap_cnt;
    size_t d, r;
    char *a = (char *)aa;

loop:
    SWAPINIT(a, es);
    swap_cnt = 0;
    if (n < 7)
    {
        for (pm = (char *)a + es; pm < (char *) a + n * es; pm += es)
            for (pl = pm; pl > (char *) a && cmp(pl - es, pl) > 0;
                    pl -= es)
            {
                swap(pl, pl - es);
            }
        return;
    }
    pm = (char *)a + (n / 2) * es;
    if (n > 7)
    {
        pl = (char *)a;
        pn = (char *)a + (n - 1) * es;
        if (n > 40)
        {
            d = (n / 8) * es;
            pl = med3(pl, pl + d, pl + 2 * d, cmp);
            pm = med3(pm - d, pm, pm + d, cmp);
            pn = med3(pn - 2 * d, pn - d, pn, cmp);
        }
        pm = med3(pl, pm, pn, cmp);
    }
    swap(a, pm);
    pa = pb = (char *)a + es;

    pc = pd = (char *)a + (n - 1) * es;
    for (;;)
    {
        while (pb <= pc && (cmp_result = cmp(pb, a)) <= 0)
        {
            if (cmp_result == 0)
            {
                swap_cnt = 1;
                swap(pa, pb);
                pa += es;
            }
            pb += es;
        }
        while (pb <= pc && (cmp_result = cmp(pc, a)) >= 0)
        {
            if (cmp_result == 0)
            {
                swap_cnt = 1;
                swap(pc, pd);
                pd -= es;
            }
            pc -= es;
        }
        if (pb > pc)
        {
            break;
        }
        swap(pb, pc);
        swap_cnt = 1;
        pb += es;
        pc -= es;
    }
    if (swap_cnt == 0)    /* Switch to insertion sort */
    {
        for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
            for (pl = pm; pl > (char *) a && cmp(pl - es, pl) > 0;
                    pl -= es)
            {
                swap(pl, pl - es);
            }
        return;
    }

    pn = (char *)a + n * es;
    r = gm_min(pa - (char *)a, pb - pa);
    vecswap(a, pb - r, r);
    r = gm_min(pd - pc, pn - pd - es);
    vecswap(pb, pn - r, r);
    if ((r = pb - pa) > es)
    {
        gm_qsort(a, r / es, es, cmp);
    }
    if ((r = pd - pc) > es)
    {
        /* Iterate rather than recurse to save stack space */
        a = pn - r;
        n = r / es;
        goto loop;
    }
}

static void do_nms_sort(detection *dets, int total, int classes, float nms_thresh)
{
    int i, j, k;
    k = total - 1;
    for (i = 0; i <= k; ++i)
    {
        if (dets[i].confidence == 0)
        {
            detection swap = dets[i];
            dets[i] = dets[k];
            dets[k] = swap;
            --k;
            --i;
        }
    }
    total = k + 1;
    for (k = 0; k < classes; ++k)
    {
        for (i = 0; i < total; ++i)
        {
            dets[i].sort_class = k;
        }
        gm_qsort(dets, total, sizeof(detection), nms_comparator);

        for (i = 0; i < total; ++i)
        {
            if (dets[i].prob[k] == 0)
            {
                continue;
            }
            box a = dets[i].bbox;
            for (j = i + 1; j < total; ++j)
            {
                box b = dets[j].bbox;
                float iou = box_iou_v3(a, b);
                if (box_iou_v3(a, b) > nms_thresh)
                {
                    /*printf("debug dets[%d].prob[%d]=%f\n",j,k ,dets[j].prob[k]);*/

                    dets[j].prob[k] = 0;
                }
            }
        }
    }
}

//n: anchor index (0~4)
//entry: (0:x, 1:y, 2:w, 3:h, 4:confidence, 5:classes)
//loc: cell index (0~48)
static int entry_index(layer_post l, int location, int entry)
{
    int n =   location / (l.w * l.h);
    int loc = location % (l.w * l.h);
    return n * l.w * l.h * (4 + l.classes + 1) + entry * l.w * l.h + loc; //4:box_channel 1: confidence
}

static box get_yolo_box(float *x, float *biases, int n, int index, int i, int j, int lw, int lh, int w, int h, int stride)
{
    box b;
    b.x = (i + x[index + 0 * stride]) / lw;
    b.y = (j + x[index + 1 * stride]) / lh;

    b.w = exp_f(x[index + 2 * stride]) * biases[2 * n]   / w;
    b.h = exp_f(x[index + 3 * stride]) * biases[2 * n + 1] / h;

    return b;
}

void correct_yolo_boxes_tiny_yolov3(detection *dets, int n, int w, int h, int netw, int neth)
{
    int i;
    int new_w = 0;
    int new_h = 0;
    if (((float)netw / w) < ((float)neth / h))
    {
        new_w = netw;
        new_h = (h * netw) / w;
    }
    else
    {
        new_h = neth;
        new_w = (w * neth) / h;
    }
    for (i = 0; i < n; ++i)
    {
        box b = dets[i].bbox;
        b.x = (b.x - (netw - new_w) / 2. / netw) / ((float)new_w / netw);
        b.y = (b.y - (neth - new_h) / 2. / neth) / ((float)new_h / neth);
        b.w *= (float)netw / new_w;
        b.h *= (float)neth / new_h;

        dets[i].bbox = b;
    }
}

static int yolo_num_detections(layer_post l, float thresh)
{
    int i, n;
    int count = 0;
    for (i = 0; i < l.w * l.h; ++i)
    {
        for (n = 0; n < l.out_ch; ++n)
        {
            int confidence_index  = entry_index(l, n * l.w * l.h + i, ENTRY_CONFIDENCE);
            if (l.output[confidence_index] > thresh)
            {
                ++count;
            }
        }
    }
    return count;
}

static int get_yolo_detections(layer_post l, int w, int h, int netw, int neth, float thresh, detection *dets)
{
    int i, j, n;
    int count = 0;
    float *predictions = l.output;

    for (i = 0; i < l.w * l.h; ++i)
    {
        int row = i / l.w;
        int col = i % l.w;
        for (n = 0; n < l.out_ch; ++n)
        {
            int confidence_index  = entry_index(l, n * l.w * l.h + i, ENTRY_CONFIDENCE);
            float confidence = predictions[confidence_index];
            if (confidence <= thresh)
            {
                continue;
            }
            int box_index  = entry_index(l, n * l.w * l.h + i, ENTRY_X);
            //mask: 0,1,2,3,4
            dets[count].bbox = get_yolo_box(predictions, l.biases, l.mask[n], box_index, col, row, l.w, l.h, netw, neth, l.w * l.h);
            dets[count].confidence = confidence;
            dets[count].classes = l.classes;
            for (j = 0; j < l.classes; ++j)
            {
                int class_index = entry_index(l, n * l.w * l.h + i, ENTRY_CLASS + j);
                float prob = confidence * predictions[class_index];
                dets[count].prob[j] = (prob > thresh) ? prob : 0;

                /*printf("dets[%d].prob[%d]=%f\n",count, j,dets[count].prob[j]);*/
            }

            ++count;
        }
    }

    correct_yolo_boxes_tiny_yolov3(dets, count, w, h, netw, neth);

    return count;
}

static int num_detections(layer_post layer0, layer_post layer1, float thresh)
{
    int i;
    int s = 0;

    if (layer0.post_type == REGION)
    {
        s += layer0.w * layer0.h * layer0.n;
    }
    else
    {
        s += yolo_num_detections(layer0, thresh);
        s += yolo_num_detections(layer1, thresh);
    }

    return s;
}


static detection *make_network_boxes(layer_post layer0, layer_post layer1, float thresh, int *num)
{
    int i;

    int nboxes = num_detections(layer0, layer1, thresh);
    if (num)
    {
        *num = nboxes;
    }

    detection *det = (detection *)malloc_ext(0, nboxes * sizeof(detection));
    for (i = 0; i < nboxes; ++i)
    {
        det[i].prob = (float *)malloc_ext(0, layer0.classes * sizeof(float));
    }
    return det;
}

void correct_region_boxes(detection *dets, int n, int image_w, int image_h, int netw, int neth, int relative)
{
    int i;
    int new_w = 0;
    int new_h = 0;
    if (((float)netw / image_w) < ((float)neth / image_h))
    {
        new_w = netw;
        new_h = (image_h * netw) / image_w;
    }
    else
    {
        new_h = neth;
        new_w = (image_w * neth) / image_h;
    }
    for (i = 0; i < n; ++i)
    {
        box b = dets[i].bbox;
        b.x = (b.x - (netw - new_w) / 2. / netw) / ((float)new_w / netw);
        b.y = (b.y - (neth - new_h) / 2. / neth) / ((float)new_h / neth);
        b.w *= (float)netw / new_w;
        b.h *= (float)neth / new_h;
        dets[i].bbox = b;
    }
}

box get_region_box(float *x, float *biases, int n, int index, int i, int j, int w, int h, int stride)
{
    box b;
    b.x = (i + x[index + 0 * stride]) / w;
    b.y = (j + x[index + 1 * stride]) / h;
    b.w = exp_f(x[index + 2 * stride]) * biases[2 * n]   / w;
    b.h = exp_f(x[index + 3 * stride]) * biases[2 * n + 1] / h;
    return b;
}

void get_region_detections(layer_post l, int image_w, int image_h, int netw, int neth, float thresh, int *map, float tree_thresh, int relative, detection *dets)
{
    int i, j, n, z;
    float *predictions = l.output;
    int size_tm = l.w * l.h;

    for (i = 0; i < size_tm; ++i)
    {
        int row = i / l.w;
        int col = i % l.w;
        for (n = 0; n < l.n; ++n)
        {
            int index = n * size_tm + i;
            for (j = 0; j < l.classes; ++j)
            {
                dets[index].prob[j] = 0;
            }
            int obj_index  = entry_index(l, n * size_tm + i, l.coords);
            int box_index  = entry_index(l, n * size_tm + i, 0);
            int mask_index = entry_index(l, n * size_tm + i, 4);
            float scale =  predictions[obj_index];
            dets[index].bbox = get_region_box(predictions, l.biases, n, box_index, col, row, l.w, l.h, size_tm);
            dets[index].objectness = scale > thresh ? scale : 0;
            if (dets[index].mask)
            {
                for (j = 0; j < l.coords - 4; ++j)
                {
                    dets[index].mask[j] = l.output[mask_index + j * size_tm];
                }
            }

            int class_index = entry_index(l, n * size_tm + i, l.coords + 1);

            if (dets[index].objectness)
            {
                for (j = 0; j < l.classes; ++j)
                {
                    int class_index = entry_index(l, n * size_tm + i, l.coords + 1 + j);
                    float prob = scale * predictions[class_index];
                    dets[index].prob[j] = (prob > thresh) ? prob : 0;
#if 0
                    if (dets[index].prob[j] > 0.00001)
                    {
                        OBJ_PRINTF("dets[%d].prob[%d] = %f\n", index, j, dets[index].prob[j]);

                        OBJ_PRINTF("x=%f,y=%f,w=%f,h=%f\n", dets[i].bbox.x, dets[i].bbox.y, dets[i].bbox.w, dets[i].bbox.h);
                    }
#endif
                }
            }

        }
    }
    correct_region_boxes(dets, size_tm * l.n, image_w, image_h, netw, neth, relative);
}


static void fill_network_boxes(layer_post layer0, layer_post layer1, int w, int h, float thresh, detection *dets)
{
    int net_w = w;
    int net_h = h;
    int count;
    int j;

    if (layer0.post_type == REGION)
    {
        get_region_detections(layer0, w, h, w, h, thresh, NULL, 0.5, 1, dets);
        dets += layer0.w * layer0.h * layer0.n;
    }
    else
    {
        for (j = 0; j < 2; ++j)
        {
            if (j == 0)
            {
                count = get_yolo_detections(layer0, w, h, net_w, net_h, thresh, dets);
            }
            else
            {
                count = get_yolo_detections(layer1, w, h, net_w, net_h, thresh, dets);
            }
            dets += count;
        }
    }
}

static detection *get_network_boxes(layer_post layer0, layer_post layer1, int w, int h, float thresh, int *num)
{
    detection *dets;
    static detection *s_det = NULL;
    static int s_num = 0;


    if (layer0.post_type == REGION)
    {
        if (s_det == NULL)
        {
            dets = make_network_boxes(layer0, layer1, thresh, num);
            s_det = dets;
            s_num = *num;
        }
        else
        {
            dets  = s_det;
            *num = s_num;
        }
    }
    else
    {
        dets = make_network_boxes(layer0, layer1, thresh, num);
    }

    fill_network_boxes(layer0, layer1, w, h, thresh, dets);

    return dets;
}

detection *post_yolo_easynet(layer_post l[2], int *nboxes, float thresh, float nms_thresh, float *out, float *foutput)
{
    int i, k;
    int ch, row, col, off;
    detection *dets = NULL;
    *nboxes = 0;
    int out_val;
    int net_w =  l[0].net_w;
    int net_h = l[0].net_h;

    memcpy(l[0].output, out, l[0].out_cnt * sizeof(float));
    memcpy(l[1].output, foutput, l[1].out_cnt * sizeof(float));

    dets = get_network_boxes(l[0], l[1],  net_w, net_h, thresh, nboxes);
    do_nms_sort(dets, *nboxes, l[0].classes, nms_thresh);

    return dets;
}

detection *easynet_post_region(layer_post l[2], int *nboxes, float thresh, float nms_thresh,  short int *srcinput, float *databuf)
{
    int i, k;
    int ch, row, col, off;
    detection *dets = NULL;
    *nboxes = 0;
    int out_val;
    int net_w =  l[0].net_w;
    int net_h = l[0].net_h;

    for (k = 0; k < l[0].out_cnt; k++)
    {
        databuf[k] = l[0].output[k] = (float)srcinput[k] / 256;
    }

    forward_region_layer(l[0], databuf, l[0].output);

    dets = get_network_boxes(l[0], l[1],  net_w, net_h, thresh, nboxes);
    do_nms_sort(dets, *nboxes, l[0].classes, nms_thresh);

    return dets;
}

static inline float logistic_activate(float x)
{
    return 1. / (1. + exp_f(-x));
}

static  inline float activate(float x, ACTIVATION a)
{
    switch (a)
    {
        case LOGISTIC:
            return logistic_activate(x);
    }

    OBJ_PRINTF("warning need activate !!!\n");
    return 0;
}

void activate_array(float *x, const int n, const ACTIVATION a)
{
    int i;
    for (i = 0; i < n; ++i)
    {
        x[i] = activate(x[i], a);
    }
}

void softmax_ext(float *input, int n, float temp, int stride, float *output)
{
    int i;
    float sum = 0;
    float largest = -(3.402823466e+38F);
    for (i = 0; i < n; ++i)
    {
        if (input[i * stride] > largest)
        {
            largest = input[i * stride];
        }
    }
    for (i = 0; i < n; ++i)
    {
        float e = exp_f(input[i * stride] / temp - largest / temp);
        sum += e;
        output[i * stride] = e;
    }
    for (i = 0; i < n; ++i)
    {
        output[i * stride] /= sum;
    }
}

void softmax_cpu_ext(float *input, int n, int batch, int batch_offset, int groups, int group_offset, int stride, float temp, float *output)
{
    int g, b;
    for (b = 0; b < batch; ++b)
    {
        for (g = 0; g < groups; ++g)
        {
            softmax_ext(input + b * batch_offset + g * group_offset, n, temp, stride, output + b * batch_offset + g * group_offset);
        }
    }
}

void forward_region_layer(const layer_post l, float *rintput, float *routput)
{
    int i, j, b = 0, t, n;

    for (n = 0; n < l.n; ++n)
    {
        int index = entry_index(l,  n * l.w * l.h, 0);
        activate_array(l.output + index, 2 * l.w * l.h, LOGISTIC);;

        index = entry_index(l,  n * l.w * l.h, l.coords);

        activate_array(l.output + index,   l.w * l.h, LOGISTIC);

        OBJ_PRINTF("xxl.output[%d] = %f\n", index, l.output[index]);
        index = entry_index(l,  n * l.w * l.h, l.coords + 1);
    }

    if (l.softmax == 1)
    {
        int index = entry_index(l,  0, l.coords + 1);
        softmax_cpu_ext(rintput + index, l.classes, l.n, l.out_cnt / l.n, l.w * l.h, 1, l.w * l.h, 1, l.output + index);
    }
}

static void free_detections(detection *dets, int n)
{
    int i;
    for (i = 0; i < n; ++i)
    {
        free_ext(0, dets[i].prob);
    }
    free_ext(0, dets);
}

void easynet_post_region_free(detection *dets, int nboxes)
{
    free_detections(dets, nboxes);
}

void post_yolo_free_easynet(detection *dets, layer_post layer_yolo, int nboxes)
{
    free_detections(dets, nboxes);
}
