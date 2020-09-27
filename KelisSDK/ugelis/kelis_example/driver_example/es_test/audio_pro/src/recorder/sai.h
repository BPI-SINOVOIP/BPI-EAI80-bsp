#ifndef __SAI_H
#define __SAI_H
#include "sys_sai.h"
//extern SAI_HandleTypeDef SAI1A_Handler;        //SAI1 Block A���
//extern SAI_HandleTypeDef SAI1B_Handler;        //SAI1 Block B���
//extern DMA_HandleTypeDef SAI1_TXDMA_Handler;   //DMA���;��
//extern DMA_HandleTypeDef SAI1_RXDMA_Handler;   //DMA���վ��

extern void (*sai_tx_callback)(void);           //sai tx�ص�����ָ��
extern void (*sai_rx_callback)(void);           //sai rx�ص�����

void SAIA_Init();
void SAIB_Init();
void SAIA_DMA_Enable(void);
void SAIB_DMA_Enable(void);
u8 SAIA_SampleRate_Set(u32 samplerate);
void SAIA_TX_DMA_Init(u8 *buf0, u8 *buf1, u16 num, u8 width);
void SAIB_RX_DMA_Init(u8 *buf0, u8 *buf1, u16 num, u8 width);
void SAI_Play_Start(void);
void SAI_Play_Stop(void);
void SAI_Rec_Start(void);
void SAI_Rec_Stop(void);
#endif
