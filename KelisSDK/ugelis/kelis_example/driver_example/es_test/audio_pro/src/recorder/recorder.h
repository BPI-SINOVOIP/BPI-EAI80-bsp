#ifndef __RECORDER_H
#define __RECORDER_H
//#include "sys.h"
//#include "ff.h"
#include "wavplay.h"

//#define malloc malloc_ext
//#define free   free_ext

#define SAI_RX_DMA_BUF_SIZE     1024*8      //定义RX DMA 数组大小
#define SAI_RX_FIFO_SIZE        2           //定义接收FIFO大小

//#define REC_SAMPLERATE            44100       //采样率,44.1Khz
#define REC_SAMPLERATE_16000        16000      //采样率,16Khz

unsigned char rec_sai_fifo_read(unsigned char **buf);
unsigned char rec_sai_fifo_write(unsigned char *buf);

void rec_sai_dma_rx_callback(void);
void recoder_enter_rec_mode(void);
void recoder_wav_init(__WaveHeader *wavhead);
void recoder_msg_show(unsigned int tsec, unsigned int kbps);
void recoder_remindmsg_show(unsigned char mode);
void recoder_new_pathname(unsigned char *pname);
char *wav_recorder(void);

#endif
