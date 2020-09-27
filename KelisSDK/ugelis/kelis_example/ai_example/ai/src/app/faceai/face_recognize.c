/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          face_recognize.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/03/03
 *
 * @brief         Detector to extract face
 *
 * @note
 *    2020/03/03, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <gm_hal_kdp.h>

#include "face_recognize.h"
#include "easynet/g_fc_allbias.h"
#include "math.h"
#include "processor.h"

#define OUTPUT_WIDTH        3
#define OUTPUT_HEIGHT       3
#define FC_CHANNEL          512
#define FC_OUT              256

#define WEIGHT_LENGTH       (0x5045c0 + 0x20000)

extern unsigned char WEI_BASE[];
extern unsigned short inputImage[];
extern unsigned short OutputImage[];

float OutputVectoror[FC_CHANNEL] AI_SECTION;
float OutputVectorFC[FC_OUT] AI_SECTION;

void FC(float *fcVector, char *fc_weight)
{
    float *g_fc_allbias = (float *)_acg_fc_allbias;
    float norm_denominator = 0;
    int i, j, feature;
    float sum;

    i = FC_OUT;
    for (j = 0; j < FC_OUT; ++j)
    {
        sum = 0;
        for (feature = 0; feature < FC_CHANNEL; ++feature)
        {
            sum += (OutputVectoror[feature] * g_fc_allbias[i]);
            ++i;
        }

        fcVector[j] = sum + g_fc_allbias[j];
        norm_denominator += (fcVector[j] * fcVector[j]);
    }
    norm_denominator = sqrt(norm_denominator);

    for (j = 0; j < FC_OUT; ++j)
    {
        fcVector[j] = fcVector[j] / norm_denominator;
    }
}

void AveragePooling(int NumCol, int NumRow, int NumChannel, int validCol, int validRow)
{
    float localSum, n;
    int r, c, feature;
    for (feature = 0; feature < NumChannel; ++feature)
    {
        localSum = 0;
        for (r = 0; r < validRow; ++r)
        {
            for (c = 0; c < validCol; ++c)
            {
                n = OutputImage[feature * NumRow * NumCol + r * NumCol + c] / 256.0;
                localSum += n;
            }
        }
        localSum = localSum / (validRow * validCol);
        OutputVectoror[feature] = localSum;
    }
}

static float *predict(easynet_proc_param_t *ep)
{
    int ret = 0;
    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
    /* Process all ops */
    do
    {
        //unsigned int t_last, t_now, delta;
        //t_last = k_cycle_get_32();
        //ui_draw_rgb(ep->dev->input, 100, 0, 96, 96);

        ret = easynet_ops_process(ep->dev);
        //t_now = k_cycle_get_32();
        //delta = t_now - t_last;
        //printf("easynet_ops_process cost %f\n", delta * 1.0f / sys_clock_hw_cycles_per_sec);

        if (ret == 0x06 || ret == 0x01)
        {
            if (ret == 0x06)
            {
                AveragePooling(OUTPUT_WIDTH, OUTPUT_HEIGHT, FC_CHANNEL, OUTPUT_WIDTH, OUTPUT_HEIGHT);
                FC((float *)&OutputVectorFC, (char *)(ep->ops + WEIGHT_LENGTH));
                easynet_process_update((unsigned char *)ep->ops, &inputImage[0], &OutputImage[0]);
                return OutputVectorFC;
            }
            return 0;
        }
    } while (1);
    //HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
    //HAL_CNN_Bram_hfwd_rw_en(DISABLE);

    //return 0;
}

void face_recognize_thread(void *data, void *param, void *not_used)
{
    easynet_proc_param_t *ep = (easynet_proc_param_t *)data;
    float *vector = 0;
    int ret = 0;

    while (ep->running)
    {
        ret = k_sem_take(&ep->predict_lock, 1000);
        if (ret == 0)
        {
            ep->ops = (int)&WEI_BASE[0];
            vector = predict(ep);
            ep->process_cb(vector, param);
        }
    }
    ep->finish_cb(0, param);

}

int face_recognize_init(easynet_proc_param_t *ep)
{
    ep->running = 1;
    ep->dev = easynet_process_init((char *)ep->ops, &inputImage[0], &OutputImage[0]);
    ep->dev->isfaceai = 1;
    k_sem_init(&ep->predict_lock, 0, UINT_MAX);
    return 0;
}

int face_recognize_exit(easynet_proc_param_t *ep)
{
    ep->running = 0;
    return 0;
}

int face_recognize_frame_in(easynet_proc_param_t *ep)
{
    ep->dev->input = ep->in;
    k_sem_give(&ep->predict_lock);
    return 0;
}
