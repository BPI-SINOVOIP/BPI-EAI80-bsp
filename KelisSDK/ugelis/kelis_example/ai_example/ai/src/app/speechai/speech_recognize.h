#ifndef __SPEECH_RECOGNIZE_H__
#define __SPEECH_RECOGNIZE_H__

#include <worker/worker.h>
#include <gm_hal_pdm.h>

#define DSPOTTER_FRAME_SIZE      960                  // Must be multiple times of 4, 960 is equal to 30 ms data
//#define DSPOTTER_FRAME_SIZE        16000*3
#define DMA_BUFFER_SIZE          (DSPOTTER_FRAME_SIZE*2)
#define RBUF_SIZE                (DSPOTTER_FRAME_SIZE*100)
#define DSPOTTER_MEM_SIZE        105000               // Please modify this number by the return value of DSpotter_GetMemoryUsage_XXX.

typedef enum
{
    ASR_STATUS_EXIT      = 0x00,
    ASR_STATUS_ENTRY     = 0x01
} ASR_Status_T;

typedef struct speech_recognized_data
{
    short      g_rdbuf[2][DSPOTTER_FRAME_SIZE / sizeof(int16_t)];
    uint32_t   g_dmabuffer[DMA_BUFFER_SIZE / sizeof(int32_t)];
    ASR_Status_T asr_status;
    void        *g_hRingBuffer;
    char        *g_byaRingBuffer;
    char         g_byDSpotterMem[DSPOTTER_MEM_SIZE];          // The memory for DSpotter engine
    int          g_nMode;
    int          g_nVolumeScalePercentage;
    PDM_Handle_T g_PDMSlvHandle;
    void        *hDSpotter;
    int          last_cmd_index;
} speech_recognized_data_t;

void speech_recognize_thread(void *data, void *param, void *not_used);
int speech_recognize_init(easynet_proc_param_t *param);
int speech_recognize_exit(easynet_proc_param_t *param);
int speech_recognize_frame_in(easynet_proc_param_t *param);

#endif //__SPEECH_RECOGNIZE_H__

