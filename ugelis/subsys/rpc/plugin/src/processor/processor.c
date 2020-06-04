/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        processor.c
 *
 * @author      Wen.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/03/19
 *
 * @brief       KDP 310 runner for easynet
 *
 * @note
 *              2019/03/19, Wen.Liu, v1.0.0
 *                  Initial version.
 *
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "layer.h"
#include "kdp.h"
#include "ops.h"
#include "processor.h"
#include <stdio.h>
#include <stdint.h>
#include <gm_common.h>
#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <dma.h>
#include "ai_common.h"
#include "math.h"
#include "gm_hal_kdp.h"
#include <stdio.h>
#include <stdint.h>
#include "math.h"
#include "stdlib.h"
#include <ctype.h>
#include <mem_cfg.h>

static int kdp_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param);

static struct easynet_dev kdp_dev =
{
    NULL,
    kdp_ops_process,
    0,
    0,
    0,
};

static kdp_processor_t kdprocessor;


#ifdef CNN_TIME_TEST
uint8_t gGetdata = 0;
uint8_t gInputdata = 0;
uint8_t gPutdata = 0;
uint8_t gMaxPool = 0;

uint32_t gGetdataTime_ms = 0;
uint32_t gPutdata_ms = 0;
uint32_t gMaxPool_ms = 0;
#endif

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

int nms_comparator(const void *pa, const void *pb){
    Box_t a = *(Box_t *)pa;
    Box_t b = *(Box_t *)pb;
    double diff = a.score - b.score;

    if(diff < 0) return 1;
    else if(diff > 0) return -1;
    return 0;
}


double overlap(double l1, double r1, double l2, double r2){
    double left = l1 > l2 ? l1 : l2;
    double right = r1 < r2 ? r1 : r2;
    return right - left;

}

double box_intersection(Box_t a, Box_t b){
    double w = overlap(a.box[1], a.box[3], b.box[1], b.box[3]);
    double h = overlap(a.box[0], a.box[2], b.box[0], b.box[2]);
    if(w < 0 || h < 0) return 0;
    double area = w*h;
    return area;
}

double box_union(Box_t a, Box_t b)
{
    double i = box_intersection(a, b);
    double u = (a.box[2] - a.box[0])*(a.box[3] - a.box[1]) + (b.box[2] - b.box[0])*(b.box[3] - b.box[1]) - i;
    return u;
}

double box_iou(Box_t a, Box_t b){
    double c;
    if (box_intersection(a, b)/box_intersection(a,a)>box_intersection(a, b)/box_intersection(b,b))
        c=box_intersection(a, b)/box_intersection(a,a);
    else
        c=box_intersection(a, b)/box_intersection(b,b);
    if (c<box_intersection(a, b)/box_union(a, b))
    c=box_intersection(a, b)/box_union(a, b);
    return c;

}

/*--------------------buble Sort ---------------------*/ 
void bubleSort(Box_t data[], int n) 
{ 
     int i,j;
	 Box_t temp;
     for(j=0;j<n-1;j++) 
	 { 
	     for(i=0;i<n-j-1;i++) 
		 { 
	    	if(data[i].score<data[i+1].score) //descent: < ascent: >
			{
                temp = data[i];
                data[i] = data[i+1];
                data[i+1] = temp;
            }
	     }
	}  
}


static void reg_write(int addr, int data)
{
    HW32_REG(addr) = data;
}

static void start_kdp(void)
{
	HAL_CNN_Start();
}

//uint32_t diff_max = 0, diff_min = 0, diff_sum = 0;
void wait_kdp(void)
{
//	uint32_t t_last, t_now,tmp_t = 0;
//	t_last = k_uptime_get_32();
	HAL_CNN_Wait();
//	t_now = k_uptime_get_32();
//	if(t_now > t_last)
//	{
//		tmp_t = t_now - t_last;
//		diff_sum += tmp_t;
//	}
//	if(tmp_t > diff_max )
//		diff_max = tmp_t;
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
void pre_processing(struct easynet_ops_param *param, unsigned char *image_input_addr, unsigned short int *inputImage)
{
    struct op_image_input_param *op_param = (struct op_image_input_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int r;
    int c;
    int channel;
    int data;
    unsigned short temp_data;
    uint32_t ch_num, row_num, col_num;
    int index = 0;
    index = 0;

    ch_num = op_param->ch;
    row_num = op_param->row;
    col_num = op_param->col;

    /*
    * Move data from UART buffer to input Buffer and do pre-processing, also convert data from 8 bit to 16 bit
    */
    for (channel = 0; channel < ch_num; ++channel)
    {
        for (r = 0; r < row_num; ++r)
        {
            for (c = 0; c < col_num; ++c)
            {
                if (channel != ch_num - 1)
                {
#if 0                
                    data = image_input_addr[channel * row_num * col_num + r * col_num + c];
                    temp_data = data;
                    inputImage[index++] = temp_data;
#else
                    inputImage[index++] = image_input_addr[channel * row_num * col_num + r * col_num + c];;
#endif
                }
                else
                {
                    inputImage[index++] = 0;
                }
            }
        }
    }
}


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
static void put_cmd(unsigned char *cmd_addr_start, int start, int length)
{
    int data = 0;
    int tmp, i, index;
	int cmd_cnt = 0;

    for (index = 0; index < length; index++)
    {
    	if(index%2 == 0)
    	{
        	tmp = cmd_addr_start[index + start +1] << 8| cmd_addr_start[index + start];
			data = tmp;
    	}

        if (index % 2 == 1)
        {
            i = index / 2;
            reg_write(BRAM_CMD_BASEARDDR + i * 4, data);
            data = 0;
        }
    }

}

#if 1
void maxPool(struct easynet_ops_param *param, unsigned short int  *outputImage)
{
    struct op_maxpool_param *op_param = (struct op_maxpool_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t ch_num, row_num, col_num, out_row, out_col, kernel_size, stride_size;
    int32_t pad;
    int32_t ch, row, col, n, m;
	uint32_t Numrow,  Numcolumn, Numchannel;

    int32_t w_offset = -op_param->pad_left;
    int32_t h_offset = -op_param->pad_left;

	int32_t tmp_r_n = 0;

    ch_num  = op_param->ch_num;
    row_num = op_param->row_num;
    col_num = op_param->col_num;
    out_row = op_param->out_row;
    out_col = op_param->out_col;
    kernel_size = op_param->pool_size;
    stride_size = op_param->pool_stride;

	Numrow = row_num;
	Numcolumn = col_num;
	Numchannel = ch_num;
	int channel, column, i, max_index, ttt;
	short int  data[4];
	unsigned short int* curBuffer = outputImage;
	for(channel = 0; channel < Numchannel; ++channel){
		ttt = channel*Numcolumn*Numrow;
		for(row = 0; row < Numrow; ++row){
			for(column = 0; column < Numcolumn; ++column){
				max_index = 0;
				tmp_r_n = row*Numcolumn;
				
				data[0] = curBuffer[ttt + tmp_r_n + column];
				if(row < Numrow - 1 && column < Numcolumn - 1){
					data[1] = curBuffer[ttt + tmp_r_n + column + 1];
					data[2] = curBuffer[ttt + tmp_r_n+ Numcolumn + column];
					data[3]  = curBuffer[ttt + tmp_r_n+ Numcolumn + column + 1];
				}else if(column == Numcolumn - 1 && row < Numrow - 1){
					data[1] = -32768;
					data[2] = curBuffer[ttt + tmp_r_n + Numcolumn + column];
					data[3] = -32768;
				}else if(row == Numrow - 1 && column < Numcolumn - 1){
					data[1] = curBuffer[ttt + tmp_r_n + column + 1];
					data[2] = -32768;
					data[3] = -32768;
				}else{
					data[1] = -32768;
					data[2] = -32768;
					data[3] = -32768;
				}

				for(i = 1; i < 4; ++i){
					if(data[i] > data[max_index]) max_index = i;
				}
				curBuffer[ttt + tmp_r_n + column] = data[max_index];
			}
		}
	}
}
#else
void maxPool(struct easynet_ops_param *param, unsigned short int  *outputImage)
{
	struct op_maxpool_param *op_param = (struct op_maxpool_param *)(((void *)param) + sizeof(struct easynet_ops_param));
	uint32_t ch_num, row_num, col_num, out_row, out_col, kernel_size, stride_size;
	int32_t pad;
	int32_t ch, row, col, n, m;
	uint32_t Numrow,  Numcolumn, Numchannel;
	uint32_t Numtotal;
	uint32_t Curttt = 0,Currow = 0,Curtmp;

    int32_t w_offset = -op_param->pad_left;
    int32_t h_offset = -op_param->pad_left;

    ch_num  = op_param->ch_num;
    row_num = op_param->row_num;
    col_num = op_param->col_num;
    out_row = op_param->out_row;
    out_col = op_param->out_col;
    kernel_size = op_param->pool_size;
    stride_size = op_param->pool_stride;

	Numrow = row_num;
	Numcolumn = col_num;
	Numchannel = ch_num;
	Numtotal  = Numcolumn * Numrow;
	int channel, column, i, max_index, ttt;
	short int  data[4];
	unsigned short int* curBuffer = outputImage;
    for(channel = 0; channel < Numchannel; ++channel)
    {
           ttt = Curttt;
           for(row = 0; row < Numrow; ++row)
           {
                for(column = 0; column < Numcolumn; ++column)
                {
                     max_index = 0;
                     Curtmp = ttt + Currow + column;
                     data[0] = curBuffer[Curtmp];
                     if(row < Numrow - 1 && column < Numcolumn - 1)
                     {
                           data[1] = curBuffer[Curtmp + 1];
                           data[2] = curBuffer[Curtmp + Numcolumn];
                           data[3]  = curBuffer[Curtmp + Numcolumn + 1];
                     }else if(column == Numcolumn - 1 && row < Numrow - 1){
                           data[1] = -32768;
                           data[2] = curBuffer[Curtmp + Numcolumn];
                           data[3] = -32768;
                     }else if(row == Numrow - 1 && column < Numcolumn - 1){
                           data[1] = curBuffer[Curtmp + Numcolumn + 1];
                           data[2] = -32768;
                           data[3] = -32768;
                     }else{
                           data[1] = -32768;
                           data[2] = -32768;
                           data[3] = -32768;
                     }

                     for(i = 1; i < 4; ++i){
                           if(data[i] > data[max_index]) max_index = i;
                     }
                     curBuffer[Curtmp + Numcolumn] = data[max_index];
                     Currow += Numcolumn;
                }
           }
           Curttt += Numtotal;
     }
}
#endif


int put_cnt = 0;

void PutData(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage)
{
    struct op_datafetch_param *op_param = (struct op_datafetch_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int actualPadSize;
    int channel, begin, index, i, r, c, temp_r, sum_r;
    uint32_t start, length, NumCol, NumChannel, NumRow, buffer, rowStartHL, rowOff, chOff, addressBank;
	unsigned short *input;
	struct conv_hw_context *cxt;

	uint32_t bank_a_addr = BRAM_OUTPUT_A_BASEARDDR;
	uint32_t bank_c_addr = BRAM_OUTPUT_C_BASEARDDR;
    
    start = op_param->start_row;
    length = op_param->len_row;
    NumCol = op_param->col;
    NumChannel = op_param->ch;
    NumRow = op_param->row;
    buffer = op_param->buffer;
        
    actualPadSize = length * NumCol * NumChannel;
    
    //odd channel number
    index = 0;
    int data = 0, tmp = 0, j = 0, p = 0, m = 0;

	for(rowStartHL = 0; rowStartHL < 2; ++rowStartHL)
    {
        if (rowStartHL == 1) /* Odd row , lower half */
        {
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }

        if (rowStartHL == 0)  /* Even row , higher half */
        {
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);;
        }

		for(i = 0; i < 2 ; ++i)
        {
            for(temp_r = rowStartHL; temp_r < 8; temp_r += 2)
            {
                j = (temp_r >> 1) % 4;
				addressBank = i * 0x10000 + j * 0x4000;
				
                if (buffer == 0)
                {
                    addressBank += bank_a_addr;
					input = inputImage;
				}
				else
				{
				    addressBank += bank_c_addr;
					input = outputImage;
				}				
                
				for(channel = i, m = 0; channel < NumChannel; channel += 2, m++)
                {
                    chOff = NumCol * NumRow * channel;
                    for(r = 0; r < length; r += 8)
                    {
                    
                        sum_r = temp_r + r;
						rowOff = (sum_r + start) * NumCol + chOff;
		
                        if(sum_r + start < NumRow)
                        {

#if 1							
                            for(c = rowOff; c < (NumCol+rowOff); ++c, addressBank += 4)
							{
								HW32_REG(addressBank) = input[c];		
								//put_cnt++;
                            }
#else
							    imp_trans((uint32_t)input + rowOff * 2, 0, (uint32_t)addressBank, 0, NumCol , 1, 1, 0);      
                                addressBank += NumCol * 4;

#endif
                           
                        }
                        else
                        {
                            for(c = 0; c < NumCol; ++c, addressBank += 4)
						    {
								HW32_REG(addressBank) = 0;
								//put_cnt++;
							}
							
                        }
                    }
                }
            }
        }
    }
}

static void print_op(int code)
{
    char *opers[] = {"Hw", "Wait", "MaxPool", "AvgPool", "Cnnected", "WriteOut", "DataFetch", "Input", "Switch"};
    if (code <= 8)
    {
        printf("(%d)%s\n", code, opers[code]);
    }
    else
    {
        printf("(%d)%s\n", code, "default");
    }
}

void GetData(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage)
{
    struct op_writeout_param *op_param = (struct op_writeout_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t start, lengthSRAMOutput, length, NumCol, NumChannel, NumRow, buffer, startChannel, lenChannel;
	uint32_t addressBank, chOff, rowOff, rowStartHL, temp_r, i, j, channel, r, c;
    int padding_height, padding_width, padding_channel;
	uint32_t bank_a_addr = BRAM_OUTPUT_A_BASEARDDR;
	uint32_t bank_c_addr = BRAM_OUTPUT_C_BASEARDDR;
	unsigned short *output;

    start = op_param->start_row;
    lengthSRAMOutput = op_param->len_row;
    length = op_param->valid_len_row;
    NumCol = op_param->out_col;
    NumChannel = op_param->out_ch;
    NumRow = op_param->out_row;
    buffer = op_param->buffer;
    startChannel = op_param->start_channel;
    lenChannel = op_param->len_channel;

    padding_height = lengthSRAMOutput;
    padding_width = NumCol;
    padding_channel = lenChannel;

#if 0	
    /*
     * Take data from two banks, and each bank contains channelNum / 2 data
     */
    for(rowStartHL = 0; rowStartHL < 2; ++rowStartHL)
    {
        if (rowStartHL == 1)  /* Odd row , lower half */
        {
			HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }

        if (rowStartHL == 0)  /* Even row , higher half */
        {
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
        }

		for(i = 0; i < 2 ; ++i)
        {
            for(temp_r = rowStartHL; temp_r < 8; temp_r += 2)
            {
                j = (temp_r >> 1) % 4;
				addressBank = i * 0x10000 + j * 0x4000;
				
                if (buffer == 0)
                {
                    addressBank += bank_a_addr;
					output = inputImage;
				}
				else
				{
				    addressBank += bank_c_addr;
					output = outputImage;
				}

				for(channel = i; channel < lenChannel; channel += 2)
				{
				    chOff = NumCol * NumRow * (channel + startChannel);
                    for(r = temp_r; r < lengthSRAMOutput; r += 8)
				    {
						rowOff = chOff + (r + start) * NumCol;
						if (r < length)
	                    {
						    for(c = 0; c < NumCol; ++c, addressBank += 4)
						    {
						        output[c + rowOff] = HW32_REG(addressBank);	
                            }
						}
						else
						{
						    for(c = 0; c < NumCol; ++c, addressBank += 4);
						}
                    }
				}
	        }
	    }
    }
#else

	if (buffer == 0)
	{

		/*
	     * Take data from two banks, and each bank contains channelNum / 2 data
	     */
	    for(rowStartHL = 0; rowStartHL < 2; ++rowStartHL)
	    {
	        if (rowStartHL == 1)  /* Odd row , lower half */
	        {
				HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
	        }

	        if (rowStartHL == 0)  /* Even row , higher half */
	        {
	            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
	        }

			for(i = 0; i < 2 ; ++i)
	        {
	            for(temp_r = rowStartHL; temp_r < 8; temp_r += 2)
	            {
	                j = (temp_r >> 1) % 4;
					addressBank = i * 0x10000 + j * 0x4000;
					
	      
	                addressBank += bank_a_addr;
					output = inputImage;

	
					for(channel = i; channel < lenChannel; channel += 2)
					{
					    chOff = NumCol * NumRow * (channel + startChannel);
	                    for(r = temp_r; r < lengthSRAMOutput; r += 8)
					    {
							rowOff = chOff + (r + start) * NumCol;
							if (r < length)
		                    {
							    for(c = 0; c < NumCol; ++c, addressBank += 4)
							    {
							        output[c + rowOff] = HW32_REG(addressBank);	
	                            }
							}
							else
							{
							    for(c = 0; c < NumCol; ++c, addressBank += 4);
							}
	                    }
					}
		        }
		    }
	    }
	}
	else
	{
		/*
	     * Take data from two banks, and each bank contains channelNum / 2 data
	     */
	    for(rowStartHL = 0; rowStartHL < 2; ++rowStartHL)
	    {
	        if (rowStartHL == 1)  /* Odd row , lower half */
	        {
				HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
	        }

	        if (rowStartHL == 0)  /* Even row , higher half */
	        {
	            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);
	        }

			for(i = 0; i < 2 ; ++i)
	        {
	            for(temp_r = rowStartHL; temp_r < 8; temp_r += 2)
	            {
	                j = (temp_r >> 1) % 4;
					addressBank = i * 0x10000 + j * 0x4000;
					
					addressBank += bank_c_addr;
				    output = outputImage;
					for(channel = i; channel < lenChannel; channel += 2)
					{
					    chOff = NumCol * NumRow * (channel + startChannel);
	                    for(r = temp_r; r < lengthSRAMOutput; r += 8)
					    {
							rowOff = chOff + (r + start) * NumCol;
							if (r < length)
		                    {
							    for(c = 0; c < NumCol; ++c, addressBank += 4)
							    {
							        output[c + rowOff] = HW32_REG(addressBank);	
	                            }
							}
							else
							{
							    for(c = 0; c < NumCol; ++c, addressBank += 4);
							}
	                    }
					}
		        }
		    }
	    }
	
	}
#endif
}

static void layerNumber(struct easynet_ops_param *param)
{ 
    struct op_layer_param *op_param = (struct op_layer_param *)(((void *)param) + sizeof(struct easynet_ops_param));

	//printf("\n====layerNumber op_param->conv = %d\n", op_param->conv);
}

#define DEBUG_TIME 1
typedef struct RPC_inputdata_private
{
	struct easynet_ops_param *param;
	unsigned char *input;
	unsigned short int *network_input;	
} RPC_inputdata_private_T;

//#define DEBUG_TIME 1

uint32_t t_Hw = 0, t_MaxPool = 0, t_Layer = 0, t_AvgPool = 0, t_WriteOut = 0, t_DataFetch=0, t_Input = 0;

static int kdp_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param)
{
    int ret = TRUE;
    struct op_hardware_param *op_param = (struct op_hardware_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    kdp_processor_t *processor = &kdprocessor;
    #ifdef DEBUG_TIME
    uint32_t t_last, t_now;
    t_last = k_uptime_get_32();
    #endif

	//print_op(param->op);
    switch (param->op)
    {
        case OP_HW:
			HAL_DMA_Send(op_param->weight_len, processor->weight_addr_start + op_param->weight_offset);
            put_cmd(processor->cmd_addr_start, op_param->cmd_offset, op_param->cmd_len); //len 222 ; cmd_offset: 0
            start_kdp();
			wait_kdp();
            #ifdef DEBUG_TIME
	        t_now = k_uptime_get_32();
            t_Hw += t_now - t_last;
            #endif
            break;
        case OP_WAIT:
            /*ret = kdp_status();*/
            break;
        case OP_MAXPOOL:
			maxPool(param, processor->network_input);
            #ifdef DEBUG_TIME
            t_now = k_uptime_get_32();
            t_MaxPool += t_now - t_last;
            #endif
            break;
		case OP_LAYER:
            layerNumber(param);
            #ifdef DEBUG_TIME
            t_now = k_uptime_get_32();
            t_Layer += t_now - t_last;
            #endif
			break;
        case OP_AVGPOOL:
            #ifdef DEBUG_TIME
            t_now = k_uptime_get_32();
            t_AvgPool += t_now - t_last;
            #endif
            break;
        case OP_CONNECTED:
            break;
        case OP_WRITEOUT:
			GetData(param, processor->network_input, processor->network_output);
            #ifdef DEBUG_TIME
            t_now = k_uptime_get_32();
            t_WriteOut += t_now - t_last;
            #endif
            break;
        case OP_DATAFETCH:
			PutData(param, processor->network_input, processor->network_output);
            #ifdef DEBUG_TIME
            t_now = k_uptime_get_32();
            t_DataFetch += t_now - t_last;
            #endif
            break;
        case OP_INPUT:			
            pre_processing(param, dev->input, processor->network_input);
            #ifdef DEBUG_TIME
            t_now = k_uptime_get_32();
            t_Input += t_now - t_last;
            #endif
            break;
        case OP_SWITCH_INOUT:
            break;
        default:
            break;
    }

    return ret;

}

struct easynet_dev *easynet_process_init(char *seq_buf, unsigned short *pInputImage, unsigned short *pOutputImage)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = &pInputImage[0];
    processor->network_output = &pOutputImage[0];

    len = ops_parse(seq_buf, &dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;

    return &kdp_dev;
}

struct easynet_dev *easynet_process_update(char *seq_buf,unsigned short *pInputImage, unsigned short *pOutputImage)
{
    kdp_processor_t *processor = &kdprocessor;
    struct easynet_dev *dev = &kdp_dev;
    int len;

    processor->network_input = &pInputImage[0];
    processor->network_output = &pOutputImage[0];

    len = ops_parse(seq_buf, &dev->param, &processor->cmd_addr_start, &processor->weight_addr_start);
    dev->cur_addr = 0;
    dev->len = len;

    return &kdp_dev;
}

void easynet_process_deinit()
{

}
