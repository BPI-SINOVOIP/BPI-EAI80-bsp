//#include "stdafx.h"
#include "webrtc_vad.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "vad_sp.h"
#include "vad_core.h"
//#include "wav_proccess.h"
#define SRAMEX   0

void *malloc_ext(uint8_t memx, uint32_t size)
{
    /*use ugelis malloc*/
}

void free_ext(uint8_t memx, void *ptr)
{
    /*use ugelis free*/
}

#define malloc(size)    malloc_ext(SRAMEX, size)
#define free(ptr)    free_ext(SRAMEX, ptr)


static const int kInitCheck = 42;
static const int kValidRates[] = { 8000, 16000, 32000, 48000 };
static const size_t kRatesSize = sizeof(kValidRates) / sizeof(*kValidRates);
static const int kMaxFrameLengthMs = 30;

VadInst *WebRtcVad_Create()
{
    VadInstT *self = (VadInstT *)malloc(sizeof(VadInstT));

    //WebRtcSpl_Init();
    self->init_flag = 0;

    return (VadInst *)self;
}

void WebRtcVad_Free(VadInst *handle)
{
    free(handle);
}

// TODO(bjornv): Move WebRtcVad_InitCore() code here.
int WebRtcVad_Init(VadInst *handle)
{
    // Initialize the core VAD component.
    return WebRtcVad_InitCore((VadInstT *) handle);
}

// TODO(bjornv): Move WebRtcVad_set_mode_core() code here.
int WebRtcVad_set_mode(VadInst *handle, int mode)
{
    VadInstT *self = (VadInstT *) handle;

    if (handle == NULL)
    {
        return -1;
    }
    if (self->init_flag != kInitCheck)
    {
        return -1;
    }

    return WebRtcVad_set_mode_core(self, mode);
}

int WebRtcVad_Process(VadInst *handle, int fs, const int16_t *audio_frame,
                      size_t frame_length)
{
    int vad = -1;
    VadInstT *self = (VadInstT *) handle;

    if (handle == NULL)
    {
        return -1;
    }

    if (self->init_flag != kInitCheck)
    {
        return -1;
    }
    if (audio_frame == NULL)
    {
        return -1;
    }
    if (WebRtcVad_ValidRateAndFrameLength(fs, frame_length) != 0)
    {
        return -1;
    }

    if (fs == 48000)
    {
        //vad = WebRtcVad_CalcVad48khz(self, audio_frame, frame_length);
    }
    else if (fs == 32000)
    {
        //vad = WebRtcVad_CalcVad32khz(self, audio_frame, frame_length);
    }
    else if (fs == 16000)
    {
        vad = WebRtcVad_CalcVad16khz(self, audio_frame, frame_length);
    }
    else if (fs == 8000)
    {
        vad = WebRtcVad_CalcVad8khz(self, audio_frame, frame_length);
    }

    //if (vad > 0) {
    //  vad = 1;
    //}
    return vad;
}

int WebRtcVad_ValidRateAndFrameLength(int rate, size_t frame_length)
{
    int return_value = -1;
    size_t i;
    int valid_length_ms;
    size_t valid_length;

    // We only allow 10, 20 or 30 ms frames. Loop through valid frame rates and
    // see if we have a matching pair.
    for (i = 0; i < kRatesSize; i++)
    {
        if (kValidRates[i] == rate)
        {
            for (valid_length_ms = 10; valid_length_ms <= kMaxFrameLengthMs;
                    valid_length_ms += 10)
            {
                valid_length = (size_t)(kValidRates[i] / 1000 * valid_length_ms);
                if (frame_length == valid_length)
                {
                    return_value = 0;
                    break;
                }
            }
            break;
        }
    }

    return return_value;
}