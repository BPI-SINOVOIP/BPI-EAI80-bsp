#ifndef __WAVPLAY_H
#define __WAVPLAY_H
//#include "sys.h"
#include "sys_sai.h"




//#define WAV_SAI_TX_DMA_BUFSIZE    4096        //����WAV TX DMA �����С(����192Kbps@24bit��ʱ��,��Ҫ����4096��Ų��Ῠ)
#define WAV_SAI_TX_DMA_BUFSIZE    4096*2
#define RD_BUFSIZE    1024*64

//RIFF��
typedef struct
{
    u32 ChunkID;            //chunk id;����̶�Ϊ"RIFF",��0X46464952
    u32 ChunkSize ;         //���ϴ�С;�ļ��ܴ�С-8
    u32 Format;             //��ʽ;WAVE,��0X45564157
} ChunkRIFF ;
//fmt��
typedef struct
{
    u32 ChunkID;            //chunk id;����̶�Ϊ"fmt ",��0X20746D66
    u32 ChunkSize ;         //�Ӽ��ϴ�С(������ID��Size);����Ϊ:20.
    u16 AudioFormat;        //��Ƶ��ʽ;0X01,��ʾ����PCM;0X11��ʾIMA ADPCM
    u16 NumOfChannels;      //ͨ������;1,��ʾ������;2,��ʾ˫����;
    u32 SampleRate;         //������;0X1F40,��ʾ8Khz
    u32 ByteRate;           //�ֽ�����;
    u16 BlockAlign;         //�����(�ֽ�);
    u16 BitsPerSample;      //�����������ݴ�С;4λADPCM,����Ϊ4
    //  u16 ByteExtraData;      //���ӵ������ֽ�;2��; ����PCM,û���������
} ChunkFMT;
//fact��
typedef struct
{
    u32 ChunkID;            //chunk id;����̶�Ϊ"fact",��0X74636166;
    u32 ChunkSize ;         //�Ӽ��ϴ�С(������ID��Size);����Ϊ:4.
    u32 NumOfSamples;       //����������;
} ChunkFACT;
//LIST��
typedef struct
{
    u32 ChunkID;            //chunk id;����̶�Ϊ"LIST",��0X74636166;
    u32 ChunkSize ;         //�Ӽ��ϴ�С(������ID��Size);����Ϊ:4.
} ChunkLIST;

//data��
typedef struct
{
    u32 ChunkID;            //chunk id;����̶�Ϊ"data",��0X5453494C
    u32 ChunkSize ;         //�Ӽ��ϴ�С(������ID��Size)
} ChunkDATA;

//wavͷ
typedef struct
{
    ChunkRIFF riff; //riff��
    ChunkFMT fmt;   //fmt��
    //  ChunkFACT fact; //fact�� ����PCM,û������ṹ��
    ChunkDATA data; //data��
} __WaveHeader;

//wav ���ſ��ƽṹ��
typedef struct
{
    u16 audioformat;            //��Ƶ��ʽ;0X01,��ʾ����PCM;0X11��ʾIMA ADPCM
    u16 nchannels;              //ͨ������;1,��ʾ������;2,��ʾ˫����;
    u16 blockalign;             //�����(�ֽ�);
    u32 datasize;               //WAV���ݴ�С

    u32 totsec ;                //���׸�ʱ��,��λ:��
    u32 cursec ;                //��ǰ����ʱ��

    u32 bitrate;                //������(λ��)
    u32 samplerate;             //������
    u16 bps;                    //λ��,����16bit,24bit,32bit

    u32 datastart;              //����֡��ʼ��λ��(���ļ������ƫ��)
} __wavctrl;


u8 wav_decode_init(u8 *fname, __wavctrl *wavx);
u32 wav_buffill(u8 *buf, u16 size, u8 bits);
void wav_sai_dma_tx_callback(void);
u8 wav_play_song(u8 *buf, int len);
#endif
