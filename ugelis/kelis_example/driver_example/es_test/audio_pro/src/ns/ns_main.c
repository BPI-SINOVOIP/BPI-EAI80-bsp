
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
#include "arm_math_vs.h"
#include "arm_common_tables_vs.h"
#include "../jerry-libm/math.h"
#include "string.h"
#include "arm_math_vs.h"
#include "arm_common_tables_vs.h"
#include "config_vs.h"
#include <kernel.h>
#include <stdint.h>
#include "../jerry-libm/jerry-libm-internal.h"
#include "noise_suppression.h"

#include "wav_data_code_16bit.h"

#define nullptr 0

#define SAMP_FREQ 16000
#define MFCC_DEC_BITS 2
#define FRAME_SHIFT_MS 10
#define FRAME_SHIFT ((int16_t)(SAMP_FREQ * 0.001 * FRAME_SHIFT_MS)) // 160
#define NUM_MFCC_COEFFS 40
#define AUDIO_LEN_MS     (3000)
#define FRAME_LEN_MS 30
#define FRAME_LEN ((int16_t)(SAMP_FREQ * 0.001 * FRAME_LEN_MS)) //480
#define MFCC_DEC_BITS 2

#define NUM_FRAMES ((AUDIO_LEN_MS - (FRAME_LEN_MS - FRAME_SHIFT_MS))/FRAME_SHIFT_MS) // 298
#define MFCC_BUFFER_SIZE (NUM_FRAMES*NUM_MFCC_COEFFS)
#define FLT_MAX  3.40282347e+38F
#define DBL_MAX  1.79769313486231571e+308

#define FLT_MIN  1.175494351e-38F
#define DBL_MIN  2.22507385850720138e-308
#define SRAMEX   0
#define WAVE_DATA_16BIT_LEN   (94*1024/2)

#define logf   log
#define sqrtf   sqrt
#define expf    exp
#define q7_t   char
#define int16_t short int
#define int32_t  int
#define uint32_t  unsigned int

/*
uint32_t malloc_ext(uint8_t memx,uint32_t size)
{
}
void free_ext(uint8_t memx,void *ptr)
{
}

#define malloc(size)    malloc_ext(SRAMEX, size)
#define free(ptr)    free_ext(SRAMEX, ptr)
*/

enum nsLevel
{
    kLow,
    kModerate,
    kHigh,
    kVeryHigh
};

static float S16ToFloat_C(int16_t v)
{
    if (v > 0)
    {
        return ((float) v) / (float) INT16_MAX;
    }

    return (((float) v) / ((float) - INT16_MIN));
}

void S16ToFloat(const int16_t *src, size_t size, float *dest)
{
    size_t i;
    for (i = 0; i < size; ++i)
    {
        dest[i] = S16ToFloat_C(src[i]);
    }
}

static int16_t FloatToS16_C(float v)
{
    static const float kMaxRound = (float) INT16_MAX - 0.5f;
    static const float kMinRound = (float) INT16_MIN + 0.5f;
    if (v > 0)
    {
        v *= kMaxRound;
        return v >= kMaxRound ? INT16_MAX : (int16_t)(v + 0.5f);
    }

    v *= -kMinRound;
    return v <= kMinRound ? INT16_MIN : (int16_t)(v - 0.5f);
}

void FloatToS16(const float *src, size_t size, int16_t *dest)
{
    size_t i;
    for (i = 0; i < size; ++i)
    {
        dest[i] = FloatToS16_C(src[i]);
    }
}

float s_inf_buffer[320];
float s_outf_buffer[320];

int nsProcess(int16_t *buffer, size_t sampleRate, int samplesCount, enum nsLevel level)
{
    if (buffer == nullptr)
    {
        return -1;
    }
    if (samplesCount == 0)
    {
        return -1;
    }
    size_t samples = MIN(160, sampleRate / 100);
    if (samples == 0)
    {
        return -1;
    }
    const int maxSamples = 320;
    int num_bands = 1;
    int16_t *input = buffer;
    size_t nTotal = (samplesCount / samples);
    int d_cnt = 0;
    int k = 0;
    double x = 5.0;

    NsHandle *nsHandle = WebRtcNs_Create();

    int status = WebRtcNs_Init(nsHandle, sampleRate);
    if (status != 0)
    {
        printf("WebRtcNs_Init fail\n");
        return -1;
    }
    status = WebRtcNs_set_policy(nsHandle, level);
    if (status != 0)
    {
        printf("WebRtcNs_set_policy fail\n");
        return -1;
    }

    printf("before cmu(-5.0)=%f\r\n", (exp(x) - exp(-x)) / (exp(x) + exp(-x)));

    for (int i = 0; i < nTotal; i++)
    {

        //printf("before i=%d\r\n", i);
        //float s_inf_buffer[maxSamples];
        //float s_outf_buffer[maxSamples];
        S16ToFloat(input, samples, s_inf_buffer);

        float *s_nsIn[1] = {s_inf_buffer};   //ns input[band][data]
        float *s_nsOut[1] = {s_outf_buffer};  //ns output[band][data]

        WebRtcNs_Analyze(nsHandle, s_nsIn[0]);


        WebRtcNs_Process(nsHandle, (const float * const *) s_nsIn, num_bands, s_nsOut);
        FloatToS16(s_outf_buffer, samples, input);

#if 0

        if ((i == nTotal - 1))
        {
            while (d_cnt < 100)
            {
                printf("magn[%d]=%f\r\n", d_cnt, s_outf_buffer[d_cnt]);
                d_cnt++;


            }
            return 0;
        }


#endif
        input += samples;
    }
    WebRtcNs_Free(nsHandle);

    return 1;
}

void noise_suppression(char *in_file, char *out_file)
{
    //音频采样率
    uint32_t sampleRate = 0;
    //总音频采样数
    uint32_t inSampleCount = 0;


    //printf("test111 inSampleCount = %d\n", inSampleCount);

    int16_t *inBuffer = &g_audio_data[0];// = wavRead_int16(in_file, &sampleRate, &inSampleCount);

    sampleRate =  16000;
    inSampleCount = 16000;// sizeof(g_audio_data);

    printf("test inSampleCount = %d\n", inSampleCount);

    //如果加载成功
    if (inBuffer != nullptr)
    {
        nsProcess(inBuffer, sampleRate, inSampleCount, kModerate);
        //wavWrite_int16(out_file, inBuffer, sampleRate, inSampleCount);

        //free(inBuffer);
    }
}

int ns_demo(void)
{
    printf("WebRtc Noise Suppression\n");

    char *in_file = &g_audio_data[0];
    char drive[3];
    char dir[256];
    char fname[256];
    char ext[256];
    char out_file[30] = "out_audio_ns.pcm";

    double end, start = 0;
    double time;

    start = k_uptime_get_32();

    noise_suppression(in_file, out_file);

    end = k_uptime_get_32();
    time = end - start;

    printf("===>WebRtc cost time = %f\n", time);
    printf("WebRtc Noise Suppression end\n");
    while (1)
    {
        k_sleep(5000);
        //printf("sleep(5)\n");
    }
    return 0;
}

