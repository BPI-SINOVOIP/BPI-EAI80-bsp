
/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>
//#include "arm_math_vs.h"
//#include "arm_common_tables_vs.h"
//#include "../jerry-libm/math.h"
#include "string.h"
//#include "arm_math_vs.h"
//#include "config_vs.h"
#include <kernel.h>
#include <stdint.h>
//#include "../jerry-libm/jerry-libm-internal.h"

#include "vad/16du_wav_data_code_16bit.h"
#include "vad/vad_core.h"

#define SRAMEX   0
#define int32_t  int
#define uint32_t  unsigned int

extern uint32_t malloc_ext(uint8_t memx, uint32_t size);
extern void free_ext(uint8_t memx, void *ptr);
#define malloc(size)    malloc_ext(SRAMEX, size)
#define free(ptr)    free_ext(SRAMEX, ptr)
void *vadInst ;

enum VAD_MODE_SELECT
{
    QUALITY_MODE = 0,
    LOW_BITRATE_MODE,
    AGGRESSIVE_MODE,
    VERY_AGGRESSIVE_MODE
};


typedef enum
{
    kSpeech,
    kSilence
} VadState;


typedef struct vad_
{
    int sampleRate;
    int vad_mode;
    int per_ms_frames;
    int  keep_weight;
    struct VadInstT_  *vadInst;
    VadState state;
} Vad;


int webrtcvad_init(Vad *vad, int sampleRate, int vad_mode, int per_ms_frames)
{
    vad->sampleRate = sampleRate;
    vad->vad_mode = vad_mode;    /* Aggressiveness mode (0, 1, 2, or 3)*/
    vad->per_ms_frames = MAX(MIN(30, per_ms_frames), 10);
    vad->keep_weight = 0;

    vad->vadInst = WebRtcVad_Create();
    if (vad->vadInst == NULL)
    {
        return -1;
    }
    int status = WebRtcVad_Init((VadInst *)vad->vadInst);
    if (status != 0)
    {
        printf("WebRtcVad_Init fail\n");
        WebRtcVad_Free((VadInst *)vadInst);
        return -1;
    }
    status = WebRtcVad_set_mode((VadInst *)vad->vadInst, vad_mode);
    if (status != 0)
    {
        printf("WebRtcVad_set_mode fail\n");
        WebRtcVad_Free((VadInst *)vad->vadInst);
        return -1;
    }

    return status;
}

int webrtcvad_deinit(void)
{
    WebRtcVad_Free((VadInst *)vadInst);
}

int IsSpeech(Vad *vad, int16_t *buffer, unsigned int len)
{
    struct VadInstT_  *vadInst;
    int ret = -1;
    uint32_t start_time = 0;
    uint32_t end_time = 0;
    uint32_t time = 0;

    int cnt = 0;
    int g_result_data[500] = {0};
    int per_ms_frames = 30;
    int keep_weight = vad->keep_weight;
    int vad_flag = 0;

    int16_t mode = 2;/* Aggressiveness mode (0, 1, 2, or 3)*/
    int per_ms = 30;
    unsigned int samples = vad->sampleRate * (vad->per_ms_frames) / 1000;

    int16_t *input_buf = buffer;
    unsigned int nframe = (len / samples);
    vadInst = vad->vadInst;
    vad->state = kSilence;


    start_time = k_uptime_get_32();
    for (int i = 0; i < nframe; i++)
    {

        int nVadRet = WebRtcVad_Process((VadInst *)vadInst, vad->sampleRate, input_buf, samples);

        if (nVadRet == -1)
        {
            printf("failed in WebRtcVad_Process\n");
            WebRtcVad_Free((VadInst *)vadInst);
            return -1;
        }
        else
        {
            g_result_data[cnt] = nVadRet;
            //printf("nVadRet = %d\n", nVadRet);

            cnt++;
            if (cnt > 2000)
            {
                cnt = 0;
            }
            input_buf += samples;
        }
    }

    int m = 0;
    while (m < cnt)
    {
        printf(" %d \n", g_result_data[m]);
        if ((m++ % 10) == 0)
        {
            printf("=====================cnt=%d==================\n", m);
        }
    }
    printf("\n");
    end_time = k_uptime_get_32();

    time = end_time - start_time;
    printf("time =%u\r\n", time);
    return ret;
}

void vad_demo(void)
{
    int m = 0;
    int k = 0;
    int audio_len = 0;
    Vad vad_parm;
    int sampleRate = 16000;
    int vad_mode = VERY_AGGRESSIVE_MODE;
    int per_ms_frames = 10;/*(10,20,30ms)*/
    int  voice_flag = 0;

    audio_len = sizeof(audio_data_16bit) / sizeof(signed short int);

    webrtcvad_init(&vad_parm, sampleRate, vad_mode, per_ms_frames);
    voice_flag = IsSpeech(&vad_parm, &audio_data_16bit[0], audio_len);
    WebRtcVad_Free((VadInst *)vad_parm.vadInst);
}

void vad_process(void)
{
    vad_demo();
}
