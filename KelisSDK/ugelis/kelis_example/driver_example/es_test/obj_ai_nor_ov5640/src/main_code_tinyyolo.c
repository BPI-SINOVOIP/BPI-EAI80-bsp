#include <stdio.h>

#include <stdint.h>
#include <gm_common.h>

#include <ugelis.h>
#include <board.h>
#include <device.h>

#include <misc/util.h>
#include <misc/printk.h>

#include <dma.h>
#include "tinyyolo_main.h"
#include "math.h"
#include "ai_common.h"
#include "stdlib.h"
#include <ctype.h>
#include <mem_cfg.h>

#define PRINTF(...)  do{} while(0);
#define PRINTK(...) do{} while(0);

typedef short int s16;
typedef unsigned short u16;

unsigned char RecvBuffer[TEST_BUFFER_SIZE] __in_section_unique(mem_ai_data);        /* Buffer for Receiving Demo Data */
__aligned(32) unsigned short inputImage[BUFFERSIZE] __in_section_unique(mem_ai_data);
__aligned(32) unsigned short OutputImage[BUFFERSIZE] __in_section_unique(mem_ai_data);
unsigned char CMD_BASE[1024] __in_section_unique(mem_ai_data);

__aligned(32) unsigned char WEI_BASE [4559800]__in_section_unique(mem_ai_data);      /*more than 4 MB are needed for YOLO*/

uint8_t g_prt = 0;
extern ObjDetData obj_det_data;

void call_cnn(unsigned char *addr)
{

    //Xil_Out32(CORE_BASEADDR+12, 1);
    uint32_t i = 0;

#ifdef CNN_TIME_TEST
    uint32_t t_last, t_now;
    t_last = k_uptime_get_32();
#endif

    HW32_REG(KDP_CTRL) |= 1 << 5;//4

    pre_processing(addr);

#ifdef CNN_TIME_TEST
    t_now = k_uptime_get_32();
    PRINTF("pre_processing %d ms \n", t_now - t_last);
#endif

    //layer infromation here
    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 0, 0, 16, 112, 112, 16, 11, 0, 0, 16, 0, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 0);
#if 0
    PRINTK("layer 1_1 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }

    PRINTK("layer 1_1 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif

    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 20, 0, 16, 112, 112, 16, 10, 11, 0, 16, 32, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 10);
#if 0
    PRINTK("layer 1_2 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }


    PRINTK("layer 1_2 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif
    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 40, 0, 16, 112, 112, 16, 10, 21, 0, 16, 64, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 20);
#if 0
    PRINTK("layer 1_3 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }


    PRINTK("layer 1_3 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif
    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 60, 0, 16, 112, 112, 16, 10, 31, 0, 16, 96, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 30);

#if 0
    PRINTK("layer 1_4 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }


    PRINTK("layer 1_4 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif

    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 80, 0, 16, 112, 112, 16, 10, 41, 0, 16, 128, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 40);

#if 0
    PRINTK("layer 1_5 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }


    PRINTK("layer 1_5 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif


    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 100, 0, 16, 112, 112, 16, 10, 51, 0, 16, 160, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 50);

#if 0

    PRINTK("layer 1_6 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }


    PRINTK("layer 1_6 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif

    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 120, 0, 16, 112, 112, 16, 10, 61, 0, 16, 192, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 60);

#if 0
    PRINTK("layer 1_7 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }


    PRINTK("layer 1_7 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif

    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 140, 0, 16, 112, 112, 16, 10, 71, 0, 16, 224, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 70);

#if 0
    PRINTK("layer 1_8 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }


    PRINTK("layer 1_8 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif

    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 160, 0, 16, 112, 112, 16, 10, 81, 0, 16, 256, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 80);

#if 0
    PRINTK("layer 1_9 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }


    PRINTK("layer 1_9 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif

    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 180, 0, 16, 112, 112, 16, 10, 91, 0, 16, 288, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 90);

#if 0

    PRINTK("layer 1_10 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }


    PRINTK("layer 1_10 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif

    call_layer(1, 0, 0, 1, 1, 4, 224, 224, 24, 200, 0, 16, 112, 112, 16, 11, 101, 0, 16, 320, 32, WEI_BASE + 0, 896, 0, 0, 0, 0, 100);

#if 0
    PRINTK("layer 1_11 OutputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", OutputImage[i]);
    }


    PRINTK("layer 1_11 InputImage: \n");
    for (i = 0; i < 16; i++)
    {
        PRINTK("0x%08x ", inputImage[i]);
    }
#endif

    call_layer(2, 0, 0, 1, 1, 16, 112, 112, 32, 0, 0, 32, 56, 56, 16, 12, 0, 0, 32, 448, 32, WEI_BASE + 896, 6400, 1, 0, 0, 0, 0);
    call_layer(2, 0, 0, 1, 1, 16, 112, 112, 32, 23, 0, 32, 56, 56, 16, 12, 12, 0, 32, 480, 32, WEI_BASE + 896, 6400, 1, 0, 0, 0, 12);
    call_layer(2, 0, 0, 1, 1, 16, 112, 112, 32, 47, 0, 32, 56, 56, 16, 12, 24, 0, 32, 512, 32, WEI_BASE + 896, 6400, 1, 0, 0, 0, 24);
    call_layer(2, 0, 0, 1, 1, 16, 112, 112, 32, 71, 0, 32, 56, 56, 16, 12, 36, 0, 32, 544, 32, WEI_BASE + 896, 6400, 1, 0, 0, 0, 36);
    call_layer(2, 0, 0, 1, 1, 16, 112, 112, 24, 95, 0, 32, 56, 56, 16, 8, 48, 0, 32, 576, 32, WEI_BASE + 896, 6400, 1, 0, 0, 0, 48);


    call_layer(3, 0, 0, 1, 1, 32, 56, 56, 32, 0, 0, 64, 28, 28, 16, 12, 0, 0, 64, 608, 32, WEI_BASE + 7296, 25088, 0, 0, 0, 0, 0);
    call_layer(3, 0, 0, 1, 1, 32, 56, 56, 32, 23, 0, 64, 28, 28, 16, 12, 12, 0, 64, 640, 32, WEI_BASE + 7296, 25088, 0, 0, 0, 0, 12);
    call_layer(3, 0, 0, 1, 1, 32, 56, 56, 16, 47, 0, 64, 28, 28, 8, 4, 24, 0, 64, 672, 32, WEI_BASE + 7296, 25088, 0, 0, 0, 0, 24);



    call_layer(4, 0, 0, 1, 0, 64, 28, 28, 32, 0, 0, 128, 14, 14, 0, 0, 0, 0, 128, 704, 90, WEI_BASE + 32384, 2071552, 1, 0, 0, 0, 0);
    call_layer(5, 0, 0, 0, 0, 128, 14, 14, 32, 0, 1, 256, 7, 7, 0, 0, 0, 0, 256, 704, 90, WEI_BASE + 32384, 2071552, 0, 0, 0, 0, 0);
    call_layer(6, 0, 0, 0, 1, 256, 7, 7, 32, 0, 0, 512, 7, 7, 8, 7, 0, 0, 512, 704, 90, WEI_BASE + 32384, 2071552, 0, 1, 0, 0, 0);

    call_layer(7, 0, 0, 1, 0, 512, 7, 7, 8, 0, 0, 256, 7, 7, 8, 7, 0, 0, 256, 794, 64, WEI_BASE + 2103936, 2363392, 0, 0, 0, 0, 0);
    call_layer(8, 0, 0, 0, 1, 256, 7, 7, 8, 0, 1, 256, 7, 7, 8, 7, 0, 0, 256, 794, 64, WEI_BASE + 2103936, 2363392, 1, 0, 0, 1, 0);

    call_layer(9, 0, 0, 1, 1, 256, 7, 7, 8, 0, 0, 35, 7, 7, 8, 7, 0, 0, 36, 858, 30, WEI_BASE + 4467328, 107800, 0, 0, 1, 0, 0);

    //    Xil_Out32(CORE_BASEADDR+8, 0);
    //    Xil_Out32(CORE_BASEADDR+12, 0);

    HW32_REG(KDP_CTRL) &= ~(1 << 6);
    HW32_REG(KDP_CTRL) &= ~(1 << 5);
}



/*
 * 1. layer_index: Index of Layer, will be used in the future version
 * 2. first: first cut of layer, will be used in the future version
 * 3. last: last cut of layer, will be used in the future version
 * 4. load_input: if there is need to move data from DDR to SRAM
 * 5. load_output: if there is need to move data from SRAN to DDR
 * 6. NumChannelInput: Layer input channel number
 * 7. NumRowInput: Layer input row number
 * 8. NumColInput: Layer input column number
 * 9. lengthInput: SRAM input row number, must be multiple of 8
 * 10. start_addrInput: SRAM start index of row from NumRowInput
 * 11. isAB: Getdata from which bank. 1 -> AB, 0 -> CD
 * 12. NumChannelOutput: Layer output feature number
 * 13. NumRowOutput: Layer output row number
 * 14. NumColOutput: Layer output column number
 * 15. lengthSRAMOutput: SRAM output row number, must be multiple of 8
 * 16. lengthOutput: Valid output row length
 * 17. start_addrOutput: SRAM start index of row from NumRowOutput
 * 18. cmdaddr: command base address
 * 19. cmdlen: command byte length
 * 20. weightaddr: weight base address
 * 21. weightlen: weight length
 * 22. moveData: need to move data from outputBuffer to InputBuffer
 */

#ifdef CNN_TIME_TEST
    uint8_t gGetdata = 0;
    uint8_t gInputdata = 0;
    uint8_t gPutdata = 0;
    uint8_t gMaxPool = 0;

    uint32_t gGetdataTime_ms = 0;
    uint32_t gPutdata_ms = 0;
    uint32_t gMaxPool_ms = 0;
#endif

void call_layer(int layer_index, int first, int last, int load_input, int load_output, int NumChannelInput, int NumRowInput, int NumColInput, int lengthInput, int start_addrInput, int isAB, int NumChannelOutput, int NumRowOutput, int NumColOutput, int lengthSRAMOutput, int lengthOutput, int start_addrOutput, int startChannel, int lenChannel, int cmdaddr, int cmdlen, int weightaddr, int weightlen, int source, int pooling, int directStart, int directEnd, int SRAM_start)
{

#ifdef CNN_TIME_TEST
    uint32_t t_last, t_now;
#endif

    if (load_input == 1 || load_output == 1)
    {
        if (load_input == 1)
        {
            if (directStart != 1)
            {
#ifdef CNN_TIME_TEST
                gPutdata ++;
                t_last = k_uptime_get_32();
#endif
                PutData(start_addrInput, lengthInput, NumColInput, NumChannelInput, NumRowInput, source);

#ifdef CNN_TIME_TEST
                t_now = k_uptime_get_32();
                PRINTF("gPutdata: %d, t_now - t_last: %d ms \n", gPutdata, t_now - t_last);
                gPutdata_ms += (t_now - t_last);
#endif
            }
            if (startChannel == 0)
            {
                dma_send(weightlen, weightaddr);
            }
            Put_cmd(cmdaddr, cmdlen);
            startCNN();
        }
        if (load_output == 1)
        {
            if (load_input == 1)
            {
                source = 1 - source;
            }
            waitCNN();
            if (directEnd == 1)
            {
                return;
            }
#ifdef CNN_TIME_TEST
            gGetdata ++;
            t_last = k_uptime_get_32();
#endif
            GetData_new_mem(start_addrOutput, SRAM_start, lengthSRAMOutput, lengthOutput, NumColOutput, NumChannelOutput, NumRowOutput, isAB, startChannel, lenChannel, source);
#ifdef CNN_TIME_TEST
            t_now = k_uptime_get_32();
            PRINTF("gGetdata: %d, t_now - t_last: %d ms \n", gGetdata, t_now - t_last);
            gGetdataTime_ms += (t_now - t_last);
#endif
            if (pooling == 1)
            {

#ifdef CNN_TIME_TEST
                gMaxPool ++;
                t_last = k_uptime_get_32();
#endif
                maxPool(NumRowOutput, NumColOutput, NumChannelOutput, source);

#ifdef CNN_TIME_TEST
                t_now = k_uptime_get_32();
                PRINTF("gMaxPool: %d, t_now - t_last: %d ms \n", gMaxPool, t_now - t_last);
                gMaxPool_ms += (t_now - t_last);
#endif
            }
        }
    }
}

/*****************************************************************************/
/**
*
* This function will do pre-processing of data (data / 256 - 0.5)
* Then reformat the data into convolution format (8 line in a group)
*
* @param    none
*
* @return   none
*
* @note     None.
*
******************************************************************************/
#if 1
void pre_processing(unsigned char *addr)
{
    int r;
    int c;
    int channel;
    int data;
    unsigned short temp_data;
    int index = 0;
    index = 0;
    /*
    * Move data from UART buffer to input Buffer and do pre-processing, also convert data from 8 bit to 16 bit
    */
    for (channel = 0; channel < 4; ++channel)
    {
        for (r = 0; r < INPUT_HEIGHT; ++r)
        {
            for (c = 0; c < INPUT_WIDTH; ++c)
            {
                if (channel != 3)
                {
                    data = addr[channel * INPUT_HEIGHT * INPUT_WIDTH + r * INPUT_WIDTH + c];
                    temp_data = data;
                    inputImage[index++] = temp_data;
                }
                else
                {
                    inputImage[index++] = 0;
                }
            }
        }
    }
}
#endif


/*****************************************************************************/
/**
*
* This function puts the command into command BRAM
*
* @param    start of index, end of index
*
* @return   none
*
* @note     None.
*
******************************************************************************/
void Put_cmd(int start, int length)
{
    int data = 0;
    int tmp, i, Index;

    for (Index = 0; Index < length; Index++)
    {
        tmp = CMD_BASE[Index + start];

        data = data * 256;
        data = data + tmp;
        if (Index % 2 == 1)
        {
            i = Index / 2;
            HW32_REG(BRAM_CMD_BASEARDDR + i * 4) = data;
            data = 0;
        }
    }
}

/**
*
* This function put data of correct order into SRAM
*
*
*
* @param    start index, length
*
* @return   None
*
* @note     None.
*
*******************************************************************************/

void PutData(int start, int length, int NumCol, int NumChannel, int NumRow, int source)
{
    int channel, i, r, c, temp_r, temp_ch, tmp, j, rowStartSram, firsthalf, addressfirstHalf;
    int data = 0, secondhalf = NumCol * NumRow;

    for (rowStartSram = 0; rowStartSram < 2; ++rowStartSram)
    {
        //select high or lower bit to write
        //Xil_Out32(CORE_BASEADDR+8, rowStartSram);

        if (rowStartSram == 1)
        {
            HW32_REG(KDP_CTRL) |= 1 << 6; /*Write Low 16Bit*/
        }

        if (rowStartSram == 0)
        {
            HW32_REG(KDP_CTRL) &= ~(1 << 6); /*Write High 16Bit*/
        }

        //select odd or even channel
        for (i = 0; i < 2 ; ++i)
        {

            for (temp_r = rowStartSram; temp_r < 8; temp_r += 2)
            {
                j = (temp_r / 2) % 4 * 0x4000; //choose small bank according to row number
                addressfirstHalf = i * 0x10000 + BRAM_OUTPUT_A_BASEARDDR + j;

                for (channel = i; channel < NumChannel; channel += 2)
                {
                    temp_ch = secondhalf * channel;

                    for (r = temp_r; r < length; r += 8)
                    {

#ifdef IMP_TRANSFER_DATA
                        firsthalf = (r + start) * NumCol + temp_ch; // re-setup the DMA
                        if (r + start < NumRow)
                        {
                            if (source == 0)
                            {
#if 0
                                for (c = 0; c < NumCol; ++c)
                                {
                                    *(volatile uint32_t *)(addressfirstHalf) = inputImage[firsthalf++];// DMA send column No's continous data
                                    addressfirstHalf += 4;
                                }
#endif
                                imp_trans((uint32_t)inputImage + firsthalf * 2, 0, (uint32_t)addressfirstHalf, 0, NumCol, 1, 1, 0);
                                addressfirstHalf += NumCol * 4;
                            }
                            else
                            {
#if 0
                                for (c = 0; c < NumCol; ++c)
                                {
                                    *(volatile uint32_t *)(addressfirstHalf) = OutputImage[firsthalf++];// DMA send column No's continous data
                                    addressfirstHalf += 4;
                                }
#endif
                                imp_trans((uint32_t)OutputImage + firsthalf * 2, 0, (uint32_t)addressfirstHalf, 0, NumCol, 1, 1, 0);
                                addressfirstHalf += NumCol * 4;
                            }
                        }
                        else
                        {
                            for (c = 0; c < NumCol; ++c)
                            {
                                *(volatile uint32_t *)(addressfirstHalf) = 0; // send 0
                                addressfirstHalf += 4;
                            }
                        }
#else
                        firsthalf = (r + start) * NumCol + temp_ch;
                        for (c = 0; c < NumCol; ++c)
                        {
                            if (r + start < NumRow)
                            {
                                if (source == 0)
                                {
                                    tmp = inputImage[firsthalf++];
                                }
                                else
                                {
                                    tmp = OutputImage[firsthalf++];
                                }
                            }
                            else
                            {
                                tmp = 0;
                            }
                            data = tmp;
                            //Xil_Out32(addressfirstHalf, data);
                            *(volatile uint32_t *)(addressfirstHalf) = data;
                            addressfirstHalf += 4;
                        }
#endif
                    }
                }
            }
        }
    }
}


/*****************************************************************************/
/**
*
* This function get data of from SRAM
*
*
*
* @param    start index, length
*
* @return   None
*
* @note     None.
*
*******************************************************************************/
void GetData(int start, int lengthSRAMOutput, int length, int NumCol, int NumChannel, int NumRow, int isAB, int startChannel, int lenChannel)
{
    int k1;
    int k2;
    int temp_r = 0;
    int temp_c = 0;
    int temp_channel = 0;
    int k1_1, k1_2, k2_1, k2_2;
    int channel = 0;
    int r = 0;
    int c = 0;
    int padding_height = lengthSRAMOutput;
    int padding_width = NumCol;

    int channelPad;
    if (obj_det_data.det_type == DET_BODY)
    {
        //channelPad = (NumChannel % 2 == 1) ? NumChannel + 1 : NumChannel;
        channelPad = lenChannel;
    }

    if (obj_det_data.det_type == DET_GEST)
    {
        channelPad = lenChannel;
    }

    for (channel = 0; channel < (channelPad / 2); ++channel)
    {
        /*
             * 4 lines in SRAM, each time we get 32 bit corresponding to 2 data in a column
             */
        for (r = 0; r < 4; ++r)
        {
            for (c = 0; c < (padding_height / 4 / 2) * padding_width; ++c)
            {
                temp_r = r;
                temp_c = c;
                temp_r =  r + c / padding_width * 4;
                temp_c = c % padding_width;
                if (temp_c < NumCol && temp_r * 2 < length)
                {
                    if (isAB == 1)
                    {
                        k1 = HW32_REG(BRAM_OUTPUT_A_BASEARDDR + c * 4 + r * 0x4000 + channel * 4 * (padding_height / 4 / 2) * padding_width);
                        k2 = HW32_REG(BRAM_OUTPUT_B_BASEARDDR + c * 4 + r * 0x4000 + channel * 4 * (padding_height / 4 / 2) * padding_width);
                    }
                    else
                    {
                        k1 = HW32_REG(BRAM_OUTPUT_C_BASEARDDR + c * 4 + r * 0x4000 + channel * 4 * (padding_height / 4 / 2) * padding_width);
                        k2 = HW32_REG(BRAM_OUTPUT_D_BASEARDDR + c * 4 + r * 0x4000 + channel * 4 * (padding_height / 4 / 2) * padding_width);
                    }
                    k1_1 = k1 >> 16;
                    k1_2 = k1 & 0xFFFF;
                    k2_1 = k2 >> 16;
                    k2_2 = k2 & 0xFFFF;

                    OutputImage[(channel * 2 + startChannel)*NumCol * NumRow + (temp_r * 2 + start) * NumCol + temp_c] = k1_1;
                    OutputImage[(channel * 2 + startChannel + 1)*NumCol * NumRow + (temp_r * 2 + start) * NumCol + temp_c] = k2_1;
                    if (temp_r * 2 + 1 < length)
                    {
                        OutputImage[(channel * 2 + startChannel)*NumCol * NumRow + (temp_r  * 2 + 1 + start) * NumCol + temp_c] = k1_2;
                        OutputImage[(channel * 2 + startChannel + 1)*NumCol * NumRow + (temp_r  * 2 + 1 + start) * NumCol + temp_c] = k2_2;
                    }
                }
            }
        }
    }
}


void GetData_new_mem(int start, int SRAMStart, int lengthSRAMOutput, int length, int NumCol, int NumChannel, int NumRow, int isAB, int startChannel, int lenChannel, int source)
{
    int channel, i, r, c, temp_r, temp_ch, tmp, j, rowStartHL, firsthalf, addressfirstHalf, first_SRAM_base;
    int secondhalf = NumCol * NumRow;

    int offset = start - SRAMStart;
    int updated_length = length + offset;
    u16 data;
    int base_address = isAB == 1 ? BRAM_OUTPUT_A_BASEARDDR : BRAM_OUTPUT_C_BASEARDDR;

    for (rowStartHL = 0; rowStartHL < 2; ++rowStartHL)
    {

        //select high or lower bit to write
        //Xil_Out32(CORE_BASEADDR+8, rowStartHL);
        if (rowStartHL == 1)
        {
            HW32_REG(KDP_CTRL) |= 1 << 6;
        }

        if (rowStartHL == 0)
        {
            HW32_REG(KDP_CTRL) &= ~(1 << 6);
        }

        //#ifndef IMP_TRANSFER_DATA
#if 0
        //select channel
        for (i = 0; i < 2 ; ++i)
        {
            first_SRAM_base = base_address + i * 0x10000;

            for (temp_r = rowStartHL; temp_r < 8; temp_r += 2)
            {
                j = (temp_r / 2) % 4 * 0x4000; //choose small bank according to row number
                addressfirstHalf = first_SRAM_base + j;

                for (channel = i; channel < lenChannel; channel += 2)
                {
                    temp_ch = (channel + startChannel) * secondhalf;

                    for (r = temp_r; r < lengthSRAMOutput; r += 8)
                    {
                        firsthalf = temp_ch + (r - offset + start) * NumCol;

                        for (c = 0; c < NumCol; ++c)
                        {
                            if (r >= offset && r < updated_length)
                            {
                                //data = Xil_In32(addressfirstHalf);
                                data = *(volatile uint32_t *)(addressfirstHalf);

                                if (source == 0)
                                {
                                    inputImage[firsthalf++] = data;
                                }
                                else
                                {
                                    OutputImage[firsthalf++] = data;
                                }
                            }
                            addressfirstHalf += 4;
                        }
                    }
                }

            }
        }
#else
        for (i = 0; i < 2 ; ++i)
        {
            first_SRAM_base = base_address + i * 0x10000;

            for (temp_r = rowStartHL; temp_r < 8; temp_r += 2)
            {
                j = (temp_r / 2) % 4 * 0x4000; //choose small bank according to row number
                addressfirstHalf = first_SRAM_base + j;

                for (channel = i; channel < lenChannel; channel += 2)
                {
                    temp_ch = (channel + startChannel) * secondhalf;

                    for (r = temp_r; r < lengthSRAMOutput; r += 8)
                    {
                        firsthalf = temp_ch + (r - offset + start) * NumCol;
                        if (r >= offset && r < updated_length)
                        {
                            data = *(volatile uint32_t *)(addressfirstHalf);
                            if (source == 0)
                            {
#if 1
                                for (c = 0; c < NumCol; ++c)
                                {
                                    data = *(volatile uint32_t *)(addressfirstHalf);
                                    inputImage[firsthalf++] = data;
                                    addressfirstHalf += 4;
                                }
#endif
                                //                                imp_trans( (uint32_t)addressfirstHalf, 0, (uint32_t)inputImage + firsthalf * 2,  0, NumCol, 1, 0, 1);
                                //                                addressfirstHalf += NumCol * 4;
                            }
                            else
                            {
#if 1
                                for (c = 0; c < NumCol; ++c)
                                {
                                    data = *(volatile uint32_t *)(addressfirstHalf);
                                    OutputImage[firsthalf++] = data;
                                    addressfirstHalf += 4;
                                }
#endif
                                /*
                                                              if ((firsthalf <= 0x11E0) && (firsthalf >= 0x1000 ))
                                                              {
                                                                   __asm("nop");
                                                                   __asm("nop");
                                                                   __asm("nop");
                                                                   __asm("nop");
                                                                   __asm("nop");
                                                              }
                                */
                                //                              imp_trans( (uint32_t)addressfirstHalf, 0, (uint32_t)OutputImage + firsthalf * 2,  0, NumCol, 1, 0, 1);
                                //                              addressfirstHalf += NumCol * 4;
                            }
                        }
                        else
                        {
                            for (c = 0; c < NumCol; ++c)
                            {
                                addressfirstHalf += 4;
                            }
                        }
                    }
                }
            }
        }
#endif
    }

}


/**
*
* This function run average pooling
*
* @param a array
*
* @return
*
*
* @note     None.
*
******************************************************************************/
#if 0
void AveragePooling(int NumCol, int NumRow, int NumChannel, int validCol, int validRow, float *array, int source)
{
    float localSum, n;
    int r, c, feature;
    float norm_denominator = 0;
    for (feature = 0; feature < NumChannel; ++feature)
    {
        localSum = 0;
        for (r = 0; r < validRow; ++r)
        {
            for (c = 0; c < validCol; ++c)
            {
                if (source == 1)
                {
                    n = OutputImage[feature * NumRow * NumCol + r * NumCol + c] / 256.0;
                }
                else
                {
                    n = inputImage[feature * NumRow * NumCol + r * NumCol + c] / 256.0;
                }
                localSum += n;
            }
        }
        localSum = localSum / (validRow * validCol);
        array[feature] = localSum;
        norm_denominator += pow(array[feature], 2);
    }
    norm_denominator = sqrt(norm_denominator);
    for (r = 0; r < NumChannel; ++r)
    {
        array[r] = array[r] / norm_denominator;
    }
}
#endif

void maxPool(int Numrow, int Numcolumn, int Numchannel, int source)
{
    int channel, row, column, i, max_index, ttt;
    s16 data[4];
    u16 *curBuffer = source == 0 ? inputImage : OutputImage;
    for (channel = 0; channel < Numchannel; ++channel)
    {
        ttt = channel * Numcolumn * Numrow;
        for (row = 0; row < Numrow; ++row)
        {
            for (column = 0; column < Numcolumn; ++column)
            {
                max_index = 0;
                data[0] = curBuffer[ttt + row * Numcolumn + column];
                if (row < Numrow - 1 && column < Numcolumn - 1)
                {
                    data[1] = curBuffer[ttt + row * Numcolumn + column + 1];
                    data[2] = curBuffer[ttt + (row + 1) * Numcolumn + column];
                    data[3]  = curBuffer[ttt + (row + 1) * Numcolumn + column + 1];
                }
                else if (column == Numcolumn - 1 && row < Numrow - 1)
                {
                    data[1] = -32768;
                    data[2] = curBuffer[ttt + (row + 1) * Numcolumn + column];
                    data[3] = -32768;
                }
                else if (row == Numrow - 1 && column < Numcolumn - 1)
                {
                    data[1] = curBuffer[ttt + row * Numcolumn + column + 1];
                    data[2] = -32768;
                    data[3] = -32768;
                }
                else
                {
                    data[1] = -32768;
                    data[2] = -32768;
                    data[3] = -32768;
                }

                for (i = 1; i < 4; ++i)
                {
                    if (data[i] > data[max_index])
                    {
                        max_index = i;
                    }
                }
                curBuffer[ttt + row * Numcolumn + column] = data[max_index];
            }
        }
    }

}



int nms_comparator(const void *pa, const void *pb)
{
    Box_t a = *(Box_t *)pa;
    Box_t b = *(Box_t *)pb;
    double diff = a.score - b.score;

    if (diff < 0)
    {
        return 1;
    }
    else if (diff > 0)
    {
        return -1;
    }
    return 0;
}


double overlap(double l1, double r1, double l2, double r2)
{
    double left = l1 > l2 ? l1 : l2;
    double right = r1 < r2 ? r1 : r2;
    return right - left;

}

double box_intersection(Box_t a, Box_t b)
{
    double w = overlap(a.box[1], a.box[3], b.box[1], b.box[3]);
    double h = overlap(a.box[0], a.box[2], b.box[0], b.box[2]);
    if (w < 0 || h < 0)
    {
        return 0;
    }
    double area = w * h;
    return area;
}

double box_union(Box_t a, Box_t b)
{
    double i = box_intersection(a, b);
    double u = (a.box[2] - a.box[0]) * (a.box[3] - a.box[1]) + (b.box[2] - b.box[0]) * (b.box[3] - b.box[1]) - i;
    return u;
}

double box_iou(Box_t a, Box_t b)
{
    double c;
    if (box_intersection(a, b) / box_intersection(a, a) > box_intersection(a, b) / box_intersection(b, b))
    {
        c = box_intersection(a, b) / box_intersection(a, a);
    }
    else
    {
        c = box_intersection(a, b) / box_intersection(b, b);
    }
    if (c < box_intersection(a, b) / box_union(a, b))
    {
        c = box_intersection(a, b) / box_union(a, b);
    }
    return c;

}

/*--------------------buble Sort ---------------------*/
void bubleSort(Box_t data[], int n)
{
    int i, j;
    Box_t temp;
    for (j = 0; j < n - 1; j++)
    {
        for (i = 0; i < n - j - 1; i++)
        {
            if (data[i].score < data[i + 1].score) //descent: < ascent: >
            {
                temp = data[i];
                data[i] = data[i + 1];
                data[i + 1] = temp;
            }
        }
    }
}



void PostYolo(void)
{
    int i = 0, j = 0, r = 0, c = 0, channel = 0, maxIndex = 0;
    double box_x = 0, box_y = 0, box_w = 0, box_h = 0, box_confidence = 0, sum = 0, maxData = 0 ;
    double thresh = 0;
    int out_width, out_height, box_channel, confidence_channel, box_num, class_channel ;
    double nms_thresh;
    int NumClasses, gap;
    int cnt = 0;
    double anchers[5][2];
    Box_t *head = NULL;
    Box_t *p = NULL;
    Box_t *previous = NULL;
    double *box = NULL;
    double *box_class_probs = NULL;
    int flag = 0;
    int tmp_pkt = 0;

    if (obj_det_data.det_type == DET_BODY)
    {
        thresh = 0.5;  //0.6
        out_width = 7;
        out_height = 7;
        box_channel = 4;
        confidence_channel = 1;
        box_num = 5;
        class_channel = 1;
    }


    box = malloc_ext(0, out_width * out_height * box_num * box_channel * sizeof(double));
    box_class_probs = malloc_ext(0, out_height * out_width * box_num * class_channel * sizeof(double));

    if (obj_det_data.det_type == DET_BODY)
    {
        nms_thresh = 0.5;
        NumClasses = 1;
        gap = box_channel + confidence_channel + class_channel;
    }

    memset(obj_det_data.box, 0x00, sizeof(Box_t) * 100);
    obj_det_data.boxcnt = 0;

    if (obj_det_data.det_type == DET_BODY)
    {
        anchers[0][0] = 0.57;
        anchers[0][1] = 1.42;

        anchers[1][0] = 1.44;
        anchers[1][1] = 3.71;

        anchers[2][0] = 2.62;
        anchers[2][1] = 6.4;

        anchers[3][0] = 4.64;
        anchers[3][1] = 7.91;

        anchers[4][0] = 8.23;
        anchers[4][1] = 9.51;
    }

    for (channel = 0; channel < 5; ++channel)
    {
        for (r = 0; r < 7; ++r)
        {
            for (c = 0; c < 7; ++c)
            {
                //Extract data from CNN result
                box_x = (1 / (1 + exp(-(double)(((s16)OutputImage[channel * gap * 7 * 7 + r * 7 + c]) / 256.0))) + c) / 7;
                box_y = (1 / (1 + exp(-(double)(((s16)OutputImage[(channel * gap + 1) * 7 * 7 + r * 7 + c]) / 256.0))) + r) / 7;
                box_w = exp((double)(((s16)OutputImage[(channel * gap + 2) * 7 * 7 + r * 7 + c]) / 256.0)) * anchers[channel][0] / 7;
                box_h = exp((double)(((s16)OutputImage[(channel * gap + 3) * 7 * 7 + r * 7 + c]) / 256.0)) * anchers[channel][1] / 7;
                box_confidence = 1 / (1 + exp(-(double)(((s16)OutputImage[(channel * gap + 4) * 7 * 7 + r * 7 + c]) / 256.0)));

                //box
                box[channel * 7 * 7 + r * 7 + c] = (box_y - (box_h / 2)) * 224;
                box[7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] = (box_x - (box_w / 2)) * 224;
                box[7 * 7 * 5 * 2 + channel * 7 * 7 + r * 7 + c] = (box_y + (box_h / 2)) * 224;
                box[7 * 7 * 5 * 3 + channel * 7 * 7 + r * 7 + c] = (box_x + (box_w / 2)) * 224;

                //get sum
                sum = 0;
                maxData = 0;
                maxIndex = 0;
                flag = 0;
                for (i = 0; i < NumClasses; ++i)
                {
                    box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] = exp((double)(((s16)OutputImage[(channel * gap + 5 + i) * 7 * 7 + r * 7 + c]) / 256.0));
                    sum += box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c];
                }

                //find max data
                for (i = 0; i < NumClasses; ++i)
                {
                    box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] = box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] / sum * box_confidence;
                    if (box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] >= thresh && box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c] > maxData)
                    {
                        maxData = box_class_probs[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c];
                        maxIndex = i;
                        flag = 1;
                    }
                }

                if (flag == 1)
                {
                    Box_t *currentBox = malloc_ext(0, sizeof(Box_t));   //malloc(0, sizeof(Box_t))
                    currentBox -> class = maxIndex;
                    currentBox -> score = maxData;
                    currentBox -> next = NULL;
                    for (i = 0; i < 4; ++i)
                    {
                        currentBox -> box[i] = box[i * 7 * 7 * 5 + channel * 7 * 7 + r * 7 + c];
                    }
                    if (head == NULL)
                    {
                        head = currentBox;
                        previous = currentBox;
                    }
                    else
                    {
                        previous -> next = currentBox;
                        previous = currentBox;
                    }
                }
            }
        }
    }

    Box_t *currentBox = head;

    //for each class, select the corresponding box
    Box_t *s = malloc_ext(0, 20 * sizeof(Box_t)); //malloc(0, 20 * sizeof(Box_t));//calloc(20,sizeof(Box_t));

    for (i = 0; i < NumClasses; ++i)
    {
        currentBox = head;
        int Numbox = 0;
        while (currentBox != NULL)
        {
            if (currentBox -> class == i)
            {
                s[Numbox].score = currentBox -> score;
                for (j = 0; j < 4; ++j)
                {
                    s[Numbox].box[j] = currentBox -> box[j];
                }
                ++Numbox;
            }
            p = currentBox;
            currentBox = currentBox -> next;
            free_ext(0, p);
            p = NULL;
        }
        if (Numbox == 1)
        {

            obj_det_data.box[obj_det_data.boxcnt] = s[0];
            obj_det_data.box[obj_det_data.boxcnt].class = i;
            obj_det_data.boxcnt ++;
#if 0
            PRINTF("class: %d\n\r", i);
            PRINTF("score: %lf\n\r", s[0].score);
            PRINTF("box_y_min: %lf\n\r", s[0].box[0] < 0 ? 0 : s[0].box[0]);
            PRINTF("box_x_min: %lf\n\r", s[0].box[1] < 0 ? 0 : s[0].box[1]);
            PRINTF("box_y_max: %lf\n\r", s[0].box[2] < 0 ? 0 : s[0].box[2]);
            PRINTF("box_x_max: %lf\n\r", s[0].box[3] < 0 ? 0 : s[0].box[3]);
#endif
        }
        else if (Numbox >= 2)
        {
            //qsort(s,Numbox,sizeof(Box_t),nms_comparator);/*wizephen.wang*/
            //quicksort(s,Numbox,sizeof(Box_t),nms_comparator); /*qsort means quick sort ???*/
            //quicksort(s,Numbox,sizeof(Box_t),1); /*qsort means quick sort ???*/
            bubleSort(s, Numbox);
            for (r = 0; r < Numbox; ++r)
            {
                for (c = r + 1; c < Numbox; ++c)
                {
                    if (box_iou(s[r], s[c]) > nms_thresh)
                    {
                        s[c].score = 0;
                    }
                }
            }
            for (r = 0; r < Numbox; ++r)
            {
                if (s[r].score > 0)
                {
                    obj_det_data.box[obj_det_data.boxcnt] = s[r];
                    obj_det_data.box[obj_det_data.boxcnt].class = i;
                    obj_det_data.boxcnt ++;
                }
            }
        }
    }

    free_ext(0, s);
    free_ext(0, box);
    free_ext(0, box_class_probs);
}

void startCNN(void)
{
    //HW32_REG(KDP_CTRL) = 1;
    HW32_REG(KDP_CTRL) |= 1 << 0;
}

void waitCNN(void)
{
    int ready;
    ready = reg_read(KDP_STATUS);
    while (ready != 1)
    {
        ready = reg_read(KDP_STATUS);
    }
}

int reg_read(int addr)
{
    return HW32_REG(addr);
}

void reg_write(int addr, int data)
{
    HW32_REG(addr) = data;
}
