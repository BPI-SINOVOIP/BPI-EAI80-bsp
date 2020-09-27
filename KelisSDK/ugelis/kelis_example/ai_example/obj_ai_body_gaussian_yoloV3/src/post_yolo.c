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
                if (box_iou_v3(a, b) > nms_thresh)
                {
                    /*printf("debug dets[%d].prob[%d]=%f\n",j,k ,dets[j].prob[k]);*/

                    dets[j].prob[k] = 0;
                }
            }
        }
    }
}


box get_gaussian_yolo_box(float *x, float *biases, int n, int index, int i, int j, int lw, int lh, int w, int h, int stride)
{
    box b;

    b.w = exp_f(x[index + 4 * stride]) * biases[2 * n] / w;
    b.h = exp_f(x[index + 6 * stride]) * biases[2 * n + 1] / h;
    b.x = (i + x[index + 0 * stride]) / lw;
    b.y = (j + x[index + 2 * stride]) / lh;

    return b;
}

void correct_gaussian_yolo_boxes(detection *dets, int n, int w, int h, int netw, int neth, int relative, int letter)
{
    int i;
    int new_w = 0;
    int new_h = 0;
    if (letter)
    {
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
    }
    else
    {
        new_w = netw;
        new_h = neth;
    }

    for (i = 0; i < n; ++i)
    {
        box b = dets[i].bbox;
        b.x = (b.x - (netw - new_w) / 2. / netw) / ((float)new_w / netw);
        b.y = (b.y - (neth - new_h) / 2. / neth) / ((float)new_h / neth);
        b.w *= (float)netw / new_w;
        b.h *= (float)neth / new_h;
        if (!relative)
        {
            b.x *= w;
            b.w *= w;
            b.y *= h;
            b.h *= h;
        }
        dets[i].bbox = b;
    }
}

static int entry_gaussian_index(layer_post l, int location, int entry)
{
    int n =   location / (l.w * l.h);
    int loc = location % (l.w * l.h);
    return n * l.w * l.h * (8 + l.classes + 1) + entry * l.w * l.h + loc;
}

int gaussian_yolo_num_detections(layer_post l, float thresh)
{
    int i, n;
    int count = 0;
    for (i = 0; i < l.w * l.h; ++i)
    {
        for (n = 0; n < l.out_ch; ++n)
        {
            int obj_index  = entry_gaussian_index(l, n * l.w * l.h + i, 8);
            if (l.output[obj_index] > thresh)
            {
                ++count;
            }
        }
    }
    return count;
}

int get_gaussian_yolo_detections(layer_post l, int w, int h, int netw, int neth, float thresh, int relative, detection *dets, int letter)
{
    int i, j, n;
    float *predictions = l.output;
    //if (l.batch == 2) avg_flipped_gaussian_yolo(l);
    int count = 0;

    for (i = 0; i < l.w * l.h; ++i)
    {
        int row = i / l.w;
        int col = i % l.w;
        for (n = 0; n < l.out_ch; ++n)
        {
            int obj_index  = entry_gaussian_index(l, n * l.w * l.h + i, 8);
            float objectness = predictions[obj_index];

            if (objectness <= thresh)
            {
                continue;    // incorrect behavior for Nan values
            }

            if (objectness > thresh)
            {
                int box_index = entry_gaussian_index(l, n * l.w * l.h + i, 0);
                dets[count].bbox = get_gaussian_yolo_box(predictions, l.biases, l.mask[n], box_index, col, row, l.w, l.h, netw, neth, l.w * l.h);
                dets[count].objectness = objectness;
                dets[count].classes = l.classes;

                dets[count].uc[0] = predictions[entry_gaussian_index(l, n * l.w * l.h + i, 1)]; // tx uncertainty
                dets[count].uc[1] = predictions[entry_gaussian_index(l, n * l.w * l.h + i, 3)]; // ty uncertainty
                dets[count].uc[2] = predictions[entry_gaussian_index(l, n * l.w * l.h + i, 5)]; // tw uncertainty
                dets[count].uc[3] = predictions[entry_gaussian_index(l, n * l.w * l.h + i, 7)]; // th uncertainty

                for (j = 0; j < l.classes; ++j)
                {
                    int class_index = entry_gaussian_index(l, n * l.w * l.h + i, 9 + j);
                    float uc_aver = (dets[count].uc[0] + dets[count].uc[1] + dets[count].uc[2] + dets[count].uc[3]) / 4.0;
                    float prob = objectness * predictions[class_index] * (1.0 - uc_aver);
                    dets[count].prob[j] = (prob > thresh) ? prob : 0;
                }
                ++count;
            }
        }
    }

    correct_gaussian_yolo_boxes(dets, count, w, h, netw, neth, relative, letter);

    return count;
}

static int num_detections(network *net, float thresh)
{
    int i = 0;
    int s = 0;
    for (i = 0; i < net->n; ++i)
    {
        layer_post l = net->layers[i];
        if (l.post_type == GAUSSIAN_YOLO)
        {
            s += gaussian_yolo_num_detections(l, thresh);
        }
    }
    return s;
}

static detection *make_network_boxes(network *net, float thresh, int *num)
{
    layer_post l = net->layers[net->n - 1];
    int i;
    int nboxes = num_detections(net, thresh);
    if (num)
    {
        *num = nboxes;
    }
    detection *dets = (detection *)malloc_ext(0, nboxes * sizeof(detection));
    for (i = 0; i < nboxes; ++i)
    {
        dets[i].prob = (float *)malloc_ext(0, l.classes * sizeof(float));
        // tx,ty,tw,th uncertainty
        dets[i].uc = (float *)(float *)malloc_ext(0, 4 * sizeof(float));  // Gaussian_YOLOv3
    }
    return dets;
}

static void fill_network_boxes(network *net, int image_w, int image_h, float thresh, detection *dets)
{
    int j;
    for (j = 0; j < net->n; ++j)
    {
        layer_post l = net->layers[j];

        if (l.post_type == GAUSSIAN_YOLO)
        {
            int count = get_gaussian_yolo_detections(l, image_w, image_h, net->w, net->h, thresh, 1, dets, 0);
            dets += count;
        }
    }
}

static detection *get_network_boxes(network *net, int image_w, int image_h, float thresh, int *num)
{
    detection *dets = make_network_boxes(net, thresh, num);
    fill_network_boxes(net, image_w, image_h, thresh, dets);
    return dets;
}

detection *easynet_post_yolo(struct network *net, int *nboxes, float score_thresh, float nms_thresh, int image_w, int image_h)
{
    detection *dets = NULL;

    /* assume that the last layer is YOLO */
    struct layer_post l = net->layers[net->n - 1];

    dets = get_network_boxes(net, image_w, image_h, score_thresh, nboxes);

    do_nms_sort(dets, *nboxes, l.classes, nms_thresh);

    return dets;
}

static void free_detections(detection *dets, int n)
{
    int i;
    for (i = 0; i < n; ++i)
    {
        free_ext(0, dets[i].prob);

        if (dets[i].uc)
        {
            free_ext(0, dets[i].uc);
        }
    }
    free_ext(0, dets);
}

void easynet_post_yolo_free(detection *dets, int nboxes)
{
    free_detections(dets, nboxes);
}
