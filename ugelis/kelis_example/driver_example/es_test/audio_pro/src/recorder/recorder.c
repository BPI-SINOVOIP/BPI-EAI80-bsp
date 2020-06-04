#include "recorder.h"
#include "audioplay.h"
//#include "ff.h"
#include "sai.h"
#include "string.h"
//#include "vad.h"
#include <gm_hal_dma.h>

#if 1
    extern DMA_Handle_T saiRxDMAHandle;
    extern DMA_Handle_T saiTxDMAHandle;
    extern void SAI_DMAConfig(CMSDK_SAI_TypeDef *pSAI, uint8_t dmaRead, DMA_Handle_T *pDMAHandle);
    extern uint32_t malloc_ext(uint8_t memx, uint32_t size);
    extern void free_ext(uint8_t memx, void *ptr);

    #define malloc(size)    malloc_ext(0, size)
    #define free(ptr)    free_ext(0, ptr)
#endif

#define PLAY_TEST 0

#define DATA_LEN (16000*2*4)
#define L_DATA_LEN (3200*3) //0.3s
//u8 buf_wav_pingpong[2][DATA_LEN] = {0};
u8 *buf_wav_temp;//[L_DATA_LEN];
//u8* buf_wav = buf_wav_pingpong;
int cycle_buf_cnt = 0;
int cycle_unit = 0;
char *cycle_buf;
int cycle_buf_next_index = 0;
//struct k_sem rec_data_lock;
//char *temp_save_buf = malloc(32000*3 + 16000 * 2 * 1);
char temp_save_buf[32000 * 3 + 16000 * 2 * 1];

//录音 SAI_DMA接收中断服务函数.在中断里面写入数据
//进入PCM 录音模式
void recoder_enter_rec_mode(void)
{
    SAIB_Init(); // saib rx, get audio input data
    SAIA_Init(); // saia rx, send audio data to w8978. todo
}

//进入PCM 放音模式
void recoder_enter_play_mode(void)
{
    WM8978_Init();
    WM8978_Init();                  //初始化WM8978
    WM8978_HPvol_Set(63, 63);       //耳机音量设置
    WM8978_SPKvol_Set(63);          //喇叭音量设置


    WM8978_ADDA_Cfg(1, 0);  //开启DAC
    WM8978_Input_Cfg(0, 0, 0); //关闭输入通道
    WM8978_Output_Cfg(1, 0); //开启DAC输出


    WM8978_MIC_Gain(63);            //MIC增益设置为0
    WM8978_SPKvol_Set(63);      //喇叭音量设置
    WM8978_I2S_Cfg(2, 0);

    SAIA_Init(); // saia tx, send audio data to w8978
}

//初始化WAV头.
void recoder_wav_init(__WaveHeader *wavhead) //初始化WAV头
{
    wavhead->riff.ChunkID = 0X46464952; //"RIFF"
    wavhead->riff.ChunkSize = 0;        //还未确定,最后需要计算
    wavhead->riff.Format = 0X45564157;  //"WAVE"
    wavhead->fmt.ChunkID = 0X20746D66;  //"fmt "
    wavhead->fmt.ChunkSize = 16;        //大小为16个字节
    wavhead->fmt.AudioFormat = 0X01;    //0X01,表示PCM;0X01,表示IMA ADPCM
    wavhead->fmt.NumOfChannels = 1;     //双声道
    wavhead->fmt.SampleRate = REC_SAMPLERATE_16000; //设置采样速率
    wavhead->fmt.ByteRate = wavhead->fmt.SampleRate * 2; //字节速率=采样率*通道数*(ADC位数/8)
    wavhead->fmt.BlockAlign = 2;        //块大小=通道数*(ADC位数/8)
    wavhead->fmt.BitsPerSample = 16;    //16位PCM
    wavhead->data.ChunkID = 0X61746164; //"data"
    wavhead->data.ChunkSize = 0;        //数据大小,还需要计算
}

//显示录音时间和码率
//tsec:秒钟数.
//提示信息
//mode:0,录音模式;1,放音模式
//WAV录音
//#define DATA_LEN 1024*65
//mfcc needs 96k byte data, 16k*3s*2byte
#define DATA_LEN_3S 1024*96
#define DATA_LEN_1S 1024*32

//u8 WAV_DATA[DATA_LEN] = {0};
//u8 buf_wav[1024*(DATA_LEN_3S+1)] = {0};
u32 data_cnt = 0;

char *buf_wav_3s_1 = NULL;
char *buf_wav_3s_2 = NULL;

u32 buf_wav_3s_1_full = 0;
u32 buf_wav_3s_2_full = 0;
u32 data_is_speach    = 0;
char *buf_wav_1s = NULL;
char *buf_wav_vad = NULL;

u32 tmp_cnt = 0;
u16 rxdat = 0;
u32 rxdat_cnt = 0;
u32 sai_driver_1s_buf_done = 0;
int one_buf_done = 0;
u32 new_len = 0;
u32 dma_mode = 1;

//extern Vad vad;
__WaveHeader wavhead;
#if 1

void wav_write_header(char *buf, char *wavdata, int wavsize)
{
    __WaveHeader wavhead;
    int header_size = sizeof(__WaveHeader);
    memset(&wavhead, 0, header_size);

    /*init wav header*/
    recoder_wav_init(&wavhead);

    wavhead.riff.ChunkSize = wavsize + 36;  //整个文件的大小-8;
    wavhead.data.ChunkSize = wavsize;   //数据大小

    memcpy(buf, &wavhead, header_size);
    memcpy(buf + header_size, wavdata, wavsize);
}

void set_recorder_newlen(u32 len)
{
    new_len = len;
}

void set_dma_mode(int mode)
{
    printf("set dma mode %d\n", mode);
    dma_mode = mode;
}



//显示录音时间和码率
//tsec:秒钟数.
//提示信息
//mode:0,录音模式;1,放音模式
//WAV录音
int read_complete = 0;
int wav_recorder_wav(u8 *rec_buf, u32 pcm_data_len)
{
    u16 rxdat        = 0;
    u32 rxdat_cnt    = 0;
    u32 rec_done     = 0;
    //    u32 pcmlen      = 0;          //wav数据大小(字节数,不包括文件头!!)

    if (pcm_data_len > DATA_LEN)
    {
        printf(" error, wav_data_len is too large \n");
    }

    if (!dma_mode)
    {
        while ((!rec_done)) //有录音
        {
            if (!((CMSDK_SAI_B->SAI_SR & (7 << 16)) == (0 << 16))) //fifo not empty
            {
                rxdat = (u16)(CMSDK_SAI_B->SAI_DR);
                memcpy(rec_buf + rxdat_cnt, &rxdat, 2 * sizeof(u8));
                rxdat_cnt = rxdat_cnt + 2;
                if (new_len != pcm_data_len)
                {
                    pcm_data_len = new_len;
                }
                if (rxdat_cnt >= pcm_data_len)
                {
                    rec_done = 1;
                    //memcpy buf_wav to vad/mfcc buf
                    //callback_func
                }
            }
        }
    }
    else //dma_mode
    {
        static int index = 0;
        while (!rec_done) //有录音
        {
            buf_wav_temp = cycle_buf + cycle_unit * cycle_buf_next_index;
            printf("%x %x %d %d\n", buf_wav_temp, cycle_buf, cycle_unit, cycle_buf_next_index);
            memset(buf_wav_temp, 0, sizeof(u8)*cycle_unit);
            SAI_DMAConfig(CMSDK_SAI_B, 1, &saiRxDMAHandle);
            SAI_Read_Start(buf_wav_temp, cycle_unit / 2);
            SAIB_Init();
            while (!read_complete)
            {
                //              k_sem_take(&rec_data_lock, K_FOREVER);
                //k_sleep(1); // todo
            };

            cycle_buf_next_index = (cycle_buf_next_index + 1) % cycle_buf_cnt;
            index++;
            if (index == 10)
            {
                wav_write_header(temp_save_buf, cycle_buf + cycle_buf_next_index - 10, (16000 * 2 * 0.3 * 10));
                printf("dump binary memory F:\\\\yangy\\\\1.wav 0x%x 0x%x\n", temp_save_buf, temp_save_buf + 96000);
                printf("dsadsad\n");
                index = 0;
            }
            read_complete = 0;
            //memcpy(rec_buf + rxdat_cnt, buf_wav_temp, sizeof(u8)*L_DATA_LEN);
            //rxdat_cnt += L_DATA_LEN;
            //if (new_len != pcm_data_len)
            //{
            //     pcm_data_len = new_len;
            //  }
            //     printf("%d %d %d\n", rxdat_cnt, new_len, pcm_data_len);
            //if(rxdat_cnt >= pcm_data_len){
            //         printf("rec_done\n");
            //  rec_done = 1;
            //memcpy buf_wav to vad/mfcc buf
            //callback_func
            //}
        }
    }

    //rec_done = 0;
    //pcmlen += rxdat_cnt;

    /*copy data to user buf*/
    //memcpy((u8*)rec_buf, (u8*)buf_wav, header_cnt + rxdat_cnt);
    //    printf("record done\n");
    return 0;//pcmlen;
}


#if 0
void wav_recorder_buf(u8 *rec_buf, u32 wav_data_len)
{
    u32 header_cnt   = 0;
    u16 rxdat        = 0;
    u32 rxdat_cnt    = 0;
    u32 rec_done     = 0;

    if (wav_data_len > DATA_LEN)
    {
        printf(" error, wav_data_len is too large \n");
    }
    while ((!rec_done)) //有录音
    {
        if (!((CMSDK_SAI_B->SAI_SR & (7 << 16)) == (0 << 16))) //fifo not empty
        {
            rxdat = (u16)(CMSDK_SAI_B->SAI_DR);
            memcpy(buf_wav + rxdat_cnt + header_cnt, &rxdat, 2 * sizeof(u8));
            rxdat_cnt = rxdat_cnt + 2;

            if ((rxdat_cnt >= wav_data_len))
            {
                rxdat_cnt = 0;
                rec_done = 1;

                //memcpy buf_wav to vad/mfcc buf
                //callback_func
            }
        }
    }

    /*copy data to user buf*/
    memcpy((u8 *)rec_buf, (u8 *)buf_wav, rxdat_cnt);
    printf("record done\n");
}
#endif

#endif

void sai_input_init(char *buf, int unit, int cnt)
{
    //cycle_buf = buf;
    //cycle_unit = unit;
    //cycle_buf_cnt = cnt;
    //cycle_buf_next_index = 0;
    //k_sem_init(&rec_data_lock, 0, UINT_MAX);
    recoder_enter_rec_mode();
}

int sai_input_get_count(int dest)
{
    while (1)
    {
        if (dest > (cycle_buf_cnt * 3 / 4))
        {
            if (cycle_buf_next_index >= 0 || cycle_buf_next_index > dest)
            {
                break;
            }
        }
        else
        {
            if (cycle_buf_next_index > dest)
            {
                break;
            }
        }
        //k_sleep(1); // todo
    }
    return cycle_buf_next_index;
}

//sai_vad_mfcc_main::sai_vad_mfcc_init
//--->sai_vad_mfcc_main::record_thread
//  --->sai_input_process
//     --->sai_vad_mfcc_main::onewav_buffer_finished
void sai_input_process(u32 pcm_data_len)
{
    int pcm_len;
    while (1)
    {
        new_len = pcm_data_len;
        pcm_len = wav_recorder_wav(0, pcm_data_len);//wav_recorder_wav(buf_wav_pingpong[current_buf_index], pcm_data_len);
    }
}

char *sai_input_start(u8 *rec_buf, u32 wav_data_len)
{
    return wav_recorder_wav(rec_buf, wav_data_len);
}

void sai_output_init(void)
{
    //printf("temp_save_buf %x\n", temp_save_buf);
    recoder_enter_play_mode();
}

void sai_output_start(void)
{
    u16 index = 0;
    u8 *pname = 0;
    pname = malloc(30);
    sprintf((char *)pname, "0:RECORDER/REC%05d.wav", index);
}

