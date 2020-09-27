/**
*
* Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
*
* @file        main.c
*
* @author      Richard.Liu
*
* @version     v1.0.0
*
* @date        2019/01/24
*
* @brief       DAC source file
*
* @note
*              2019/01/24, Richard.Liu, v1.0.0
*                  Initial version.
*
**/

#include <ugelis.h>
#include <gm_hal_gpio.h>
#include <gm_hal_dac.h>
#include <gm_hal_dma.h>
#include "data_array.h"

#define TEST_VOICE

#define PR_DEBUG  printf

#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))

typedef struct
{
    uint32_t wavlen;
    uint32_t formart;
    uint32_t CHnum;
    uint32_t SampleRate;
    uint32_t speed;
    uint32_t ajust;
    uint32_t SampleBits;
    uint32_t DATAlen;
} wav_handle;


uint32_t Get_num(uint8_t *pbuf, uint8_t len);
void DAC1_SetData(uint16_t data);
static RET_CODE DAC_ConfigPinmux(void);
uint8_t WAV_Init(uint8_t *wav_buf);
int Check_Ifo(uint8_t *pbuf1, uint8_t *pbuf2);


wav_handle wav1;
uint32_t CHanalnum, Bitnum;


static RET_CODE DAC_ConfigPinmux(void)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /*set digtal pin as input , inorde to not affect analog input*/
    pGPIO = GPIOC;
    GPIO_InitStruct.pin  = GPIO_PIN_11;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    //GPIO_InitStruct.analog  = GPIOA15_ANF3_XDACOUT1;

    ret = HAL_GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        PR_DEBUG("Err:dac_ConfigPinmux\n");
    }

    return ret;
}


uint8_t WAV_Init(uint8_t *wav_buf)
{

    if (Check_Ifo(wav_buf, "RIFF"))

    {
        return 1;
    }

    wav1.wavlen = Get_num(wav_buf + 4, 4);

    printf("\n\rwav1.wavlen = %ld\n\r", wav1.wavlen);

    //if(Check_Ifo(wav_buf+8,"WAVE"))return 2;//WAVE????

    //if(Check_Ifo(wav_buf+12,"fmt "))return 3;//fmt????

    wav1.formart = Get_num(wav_buf + 20, 2); //????

    printf("\n\rwav1.formart = %d\n\r", wav1.formart);


    wav1.CHnum = Get_num(wav_buf + 22, 2); //???

    printf("\n\rwav1.CHnum = %d\n\r", wav1.CHnum);

    CHanalnum = wav1.CHnum;

    wav1.SampleRate = Get_num(wav_buf + 24, 4); //???

    printf("\n\rwav1.SampleRate = %ld\n\r", wav1.SampleRate);


    wav1.speed = Get_num(wav_buf + 28, 4); //??????

    printf("\n\rwav1.speed = %ld\n\r", wav1.speed);


    wav1.ajust = Get_num(wav_buf + 32, 2); //??????

    printf("\n\rwav1.ajust = %d\n\r", wav1.ajust);



    wav1.SampleBits = Get_num(wav_buf + 34, 2); //??????

    printf("\n\rwav1.SampleBits = %d\n\r", wav1.SampleBits);

    Bitnum = wav1.SampleBits;

    wav1.DATAlen = Get_num(wav_buf + 40, 4); //????

    printf("\n\rwav1.DATAlen = %d\n\r", wav1.DATAlen);

    if (wav1.wavlen < 0x100000)
    {
        printf("\n\rwav1.wavlen = %dkb\n\r", (wav1.wavlen) >> 10);
    }

    else
    {
        printf("\n\rwav1.wavlen = %dMb\n\r", (wav1.wavlen) >> 20);
    }

    if (wav1.formart == 1)
    {
        printf("\n\rWAV PCM\n\r");
    }

    if (wav1.CHnum == 1)
    {
        printf("\n\rsingle\n\r");
    }

    else
    {
        printf("\n\rstereo\n\r");
    }

    printf("\n\rwav1.SampleRate = %dkHz\n\r", (wav1.SampleRate) / 1000);

    printf("\n\rwav1.speed = %dbps\n\r", (wav1.speed) / 1000);

    printf("\n\rwav1.SampleBits = %dbit\n\r", wav1.SampleBits);


    return 0;

}

int Check_Ifo(uint8_t *pbuf1, uint8_t *pbuf2)
{

    uint8_t i;
    for (i = 0; i < 4; i++)
    {
        if (*pbuf1 != *pbuf2)
        {
            return 1;
        }
        pbuf1++;
        pbuf2++;
    }

    return 0;
}

uint32_t Get_num(uint8_t *pbuf, uint8_t len)
{
    uint32_t num;
    if (len == 2)
    {
        num = (pbuf[1] << 8) | pbuf[0];
    }
    else if (len == 4)
    {
        num = (pbuf[3] << 24) | (pbuf[2] << 16) | (pbuf[1] << 8) | pbuf[0];
    }
    return num;

}

void DAC1_SetData(uint16_t data)
{
    DAC0_DEV->DAC_DH   = data;
    DAC0_DEV->DAC_STRIG    = 1 << 0;
}


void common_init(void)
{
    DAC_CLK_SEL(LIRC_CLOCK);
    HW32_REG(0x400000A0) |= 1 << 24;  // DAC clock select
    DAC_CLKEN();
    DAC_RESET();
}

void voice_test(DAC_Device_T *CMSDK_DAC)
{
    int i   = 0;
    int j   = 0;
    uint32_t DApc = 0;
    uint16_t temp;
    uint8_t *wav_buf = &voice_data[0];
    DAC_ConfigPinmux();

    CMSDK_DAC->DAC_CTRL = 1 << 0; //Enable DAC , 12-bit mode
    while (1)
    {
        for (DApc = 0 ; DApc < sizeof(voice_data) / sizeof(voice_data[0]) ;)
        {
            if (CHanalnum == 1)
            {

                if (Bitnum == 8)
                {
                    CMSDK_DAC->DAC_DH = wav_buf[DApc];
                    CMSDK_DAC->DAC_STRIG    = 1 << 0;
                    DApc++;
                }
                else if (Bitnum == 16)
                {
                    //SysTick_DelayUS(45);
                    k_sleep(45);
                    temp = (((uint8_t)(wav_buf[DApc + 1] - 0x80) << 4) | (wav_buf[DApc] >> 4));
                    CMSDK_DAC->DAC_DH = temp;
                    CMSDK_DAC->DAC_STRIG    = 1 << 0;
                    DApc += 2;

                }
            }
            else if (CHanalnum == 2)
            {

                if (Bitnum == 8)
                {
                    CMSDK_DAC->DAC_DH = wav_buf[DApc];
                    DApc++;
                    CMSDK_DAC->DAC_STRIG    = 1 << 0;
                }

                else if (Bitnum == 16)
                {
                    CMSDK_DAC->DAC_DH = (((uint8_t)(wav_buf[DApc + 1] - 0x80) << 4) | (wav_buf[DApc] >> 4));
                    DApc += 2;
                    CMSDK_DAC->DAC_STRIG    = 1 << 0;

                }
            }
        }
        //SysTick_DelayMS(1000);
        k_sleep(1000);
    }
}

int main(void)
{
    common_init();
    //DAC_ConfigPinmux();
    WAV_Init(voice_data);
    voice_test(DAC0_DEV);
    while (1)
    {
        k_sleep(3000);
        PR_DEBUG("midlle func[%s]\n", __FUNCTION__);
    }


}
