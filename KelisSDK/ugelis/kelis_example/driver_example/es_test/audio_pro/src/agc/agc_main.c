// agc_test_180830.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"

#if 1
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//using https://github.com/mackron/dr_libs/blob/master/dr_wav.h  decode
//#define DR_WAV_IMPLEMENTATION

//#include "dr_wav.h"
#include "agc.h"

#ifndef nullptr
    #define nullptr 0
#endif

#ifndef MIN
    #define  MIN(A, B)        ((A) < (B) ? (A) : (B))
#endif

// count time
#include <stdint.h>
int agcProcess(int16_t *buffer, uint32_t sampleRate, size_t samplesCount, int16_t agcMode)
{
    if (buffer == nullptr)
    {
        return -1;
    }
    if (samplesCount == 0)
    {
        return -1;
    }
    WebRtcAgcConfig agcConfig;
    agcConfig.compressionGaindB = 9; // default 9 dB
    agcConfig.limiterEnable = 1; // default kAgcTrue (on)
    agcConfig.targetLevelDbfs = 3; // default 3 (-3 dBOv)
    int minLevel = 0;
    int maxLevel = 255;
    size_t samples = MIN(160, sampleRate / 100);//总的采样点数
    if (samples == 0)
    {
        return -1;
    }
    const int maxSamples = 320;
    int16_t *input = buffer;
    size_t nTotal = (samplesCount / samples); //总的采样帧数，每帧 160 个采样点，在16000k时
    void *agcInst = WebRtcAgc_Create();
    if (agcInst == NULL)
    {
        return -1;
    }
    int status = WebRtcAgc_Init(agcInst, minLevel, maxLevel, agcMode, sampleRate);
    if (status != 0)
    {
        printf("WebRtcAgc_Init fail\n");
        WebRtcAgc_Free(agcInst);
        return -1;
    }
    status = WebRtcAgc_set_config(agcInst, agcConfig);
    if (status != 0)
    {
        printf("WebRtcAgc_set_config fail\n");
        WebRtcAgc_Free(agcInst);
        return -1;
    }
    size_t num_bands = 1;
    int inMicLevel, outMicLevel = -1;
    int16_t out_buffer[maxSamples];
    int16_t *out16 = out_buffer;
    uint8_t saturationWarning = 1;                 //if overflow happens, when after agc, max value > 65536, 鏄惁鏈夋孩鍑哄彂鐢燂紝澧炵泭鏀惧ぇ浠ュ悗鐨勬渶澶у�艰秴杩囦簡65536
    int16_t echo = 0;                                 //if agc conside echo effect. 澧炵泭鏀惧ぇ鏄惁鑰冭檻鍥炲０褰卞搷
    for (int i = 0; i < nTotal; i++)
    {
        inMicLevel = 0;
        int nAgcRet = WebRtcAgc_Process(agcInst, (const int16_t *const *) &input, num_bands, samples,
                                        (int16_t *const *) &out16, inMicLevel, &outMicLevel, echo,
                                        &saturationWarning);

        if (nAgcRet != 0)
        {
            printf("failed in WebRtcAgc_Process\n");
            WebRtcAgc_Free(agcInst);
            return -1;
        }
        memcpy(input, out_buffer, samples * sizeof(int16_t));
        input += samples;
    }
    WebRtcAgc_Free(agcInst);
    return 1;
}

void auto_gain(char *in_file, char *out_file)
{
    //sample rate,                               闊抽閲囨牱鐜?    uint32_t sampleRate = 0;
    //all sample point count,                    鎬婚煶棰戦噰鏍锋暟
    uint32_t sampleRate = 0;
    uint64_t inSampleCount = 0;
    //int16_t *inBuffer = wavRead_int16(in_file, &sampleRate, &inSampleCount);
    int16_t inBuffer[1000] = {0};
    //if load audio data success,                           濡傛灉鍔犺浇鎴愬姛
    printf("sampleRate: %d, inSampleCount: %d\n ", sampleRate, inSampleCount);
    if (inBuffer != nullptr)
    {
        //  kAgcModeAdaptiveAnalog , anglog volume adjust  妯℃嫙闊抽噺璋冭妭
        //  kAgcModeAdaptiveDigital, auto agc,  鑷�傚簲澧炵泭
        //  kAgcModeFixedDigital , fix agc ,鍥哄畾澧炵泭
        //double startTime = now();

        agcProcess(inBuffer, sampleRate, inSampleCount, kAgcModeAdaptiveDigital);

        //double elapsed_time = calcElapsed(startTime, now());
        //printf("time: %d ms\n ", (int) (elapsed_time * 1000));
        //wavWrite_int16(out_file, inBuffer, sampleRate, inSampleCount);
        //free(inBuffer);
    }
}

#define in_file  "F:\\workspace\\audio_prj\\agc_test\\data\\agc_in.wav"
#define out_file "F:\\workspace\\audio_prj\\agc_test\\data\\agc_out.wav"

int agc_demo(int argc, char *argv[])
{
    auto_gain(in_file, out_file);
    return 0;
}
#endif