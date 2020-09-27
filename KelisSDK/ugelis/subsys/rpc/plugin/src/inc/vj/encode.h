#ifndef _JENCODE_H
#define _JENCODE_H

//#include <stm32f4xx.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

/****************************************************************************
	金龙电子工作室
	GD STM32F407学习板
	淘宝店：http://shop71381140.taobao.com/
	本程序只供学习使用，未经作者许可，不得用于其它任何用途
****************************************************************************/	 
#define DCTSIZE   8
#define DCTSIZE2 64
#define MAX_COMPONENT  4
#define NUM_HUFF_TBLS  2  //huffman表的数量,基本系统中为2
#define NUM_QUANT_TBLS 4  //量化表的数量,默认为4,但我们使用2
#define MAX_BLOCK_IN_MCU 10 //ISO规定的
#define DC_HUFF_TBL 0     
#define AC_HUFF_TBL 1//这两个变量用于表选择和评估
#define BGR565toRGB565(BGR) ((BGR&0x1f)<<11)+(BGR&0x7e0)+((BGR&0xF800)>>11)
#define SET_COMP(TARGET,ID,H,V,Tq,Td,Ta)\
(TARGET.comp_id=(ID),\
TARGET.comp_index=(ID),\
TARGET.h_factor=(H),\
TARGET.v_factor=(V),\
TARGET.quant_tbl_no=(Tq),\
TARGET.dc_tbl_no=(Td),\
TARGET.ac_tbl_no=(Ta))

typedef short  DCTVAL;
typedef u8 JSAMPLE;
typedef DCTVAL DCTBLOCK[DCTSIZE2];

typedef struct {
  	DCTBLOCK quantval;
} JQUANT_TBL;

typedef struct {
  	u8 bits[17];
  	u8 huffval[256]; 
  	u16 esize[256];
  	u16 ecode[256];
} JHUFF_TBL;
//色彩空间列表
typedef enum {
  	JCS_UNKNOWN,
  	JCS_GRAY,
  	JCS_RGB,
  	JCS_CMYK,
  	JCS_YCbCr,
  	JCS_YUV
} JC_COLORSPACE;
//状态列表
typedef enum {
  	JC_CREATING,
  	JC_SETTING,
  	JC_START,
  	JC_FINISH,
  	JC_DESTORING
} JC_STATE;
//标志字节
typedef enum {
  	SOF0=0xC0,//basic dct 
  	SOF1=0xC1,//extend dct
  	DHT =0xC4,//定义huffman表
  	SOI =0xD8,//帧开始
  	EOI =0xD9,//帧结束
  	SOS =0xDA,//扫描开始
  	DQT =0xDB,//定义量化表
  	COM =0xDE //common
} MARKER;
//元件结构体
typedef struct {
	u8 comp_id;//元件ID
	u8 comp_index;//元件在JPEG文件信息中的位置
	int h_factor;
	int v_factor;
	int quant_tbl_no;//表号选择
	int dc_tbl_no;//直流分量哈夫曼表选择
	int ac_tbl_no;//交流分量哈夫曼表选择
	int num_dct_table;
	DCTBLOCK *dct_table;
	DCTVAL preval;
} jpeg_component_info;
//压缩文件信息结构体
typedef struct {
  	int image_width;  
  	int image_height;
  	char *output; //JPEG数据输出指针
  	int precision;//采样精度,默认为8
  	float quality;
  	JC_COLORSPACE in_color_space;
  	int num_comp;//颜色元件数,默认为3(YCrCb彩色图)
  	jpeg_component_info comp[MAX_COMPONENT];//JPEG元件信息
  	int mcu_per_row;//横向MCU单元数
  	int mcu_rows;   //每行MCU单元数
  	int block_in_mcu;//每个MCU单元的块数 
  	int mcu_width;//MCU单元水平像素
  	int mcu_height; //MCU单元垂直像素
  	int mcu_blocks; //MCU单元面积 
  	int max_h_factor;
  	int max_v_factor; 
  	JHUFF_TBL *dc_huff_table[NUM_HUFF_TBLS];//直流Huffman表
  	JHUFF_TBL *ac_huff_table[NUM_HUFF_TBLS];//交流Huffman表  
  	JQUANT_TBL *quant_table[NUM_QUANT_TBLS];//量化表
    JSAMPLE *inbuf;//颜色转换,行扫描缓冲区
  	int inbuf_width;  //缓冲宽
  	int inbuf_height; //缓冲高
  	int inbuf_size;   //缓冲区大小,不等同于缓冲宽*缓冲高
  	JC_STATE state;//状态变量
  	int  next_line; 
} jpeg_compress_info;

typedef u8 JSAMPLE;

union buffer_tag 
{
  	u32 all;
  	u8  c[4];
};

static const u8 jfcode[]= 
{
	0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 
	0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00
};
static const u16 std_luminance_quant_tbl[DCTSIZE2]=
{
	16,  11,  10,  16,  24,  40,  51,  61,
	12,  12,  14,  19,  26,  58,  60,  55,
	14,  13,  16,  24,  40,  57,  69,  56,
	14,  17,  22,  29,  51,  87,  80,  62,
	18,  22,  37,  56,  68, 109, 103,  77,
	24,  35,  55,  64,  81, 104, 113,  92,
	49,  64,  78,  87, 103, 121, 120, 101,
	72,  92,  95,  98, 112, 100, 103,  99
};
static const u16 std_chrominance_quant_tbl[DCTSIZE2]=
{
	17,  18,  24,  47,  99,  99,  99,  99,
	18,  21,  26,  66,  99,  99,  99,  99,
	24,  26,  56,  99,  99,  99,  99,  99,
	47,  66,  99,  99,  99,  99,  99,  99,
	99,  99,  99,  99,  99,  99,  99,  99,
	99,  99,  99,  99,  99,  99,  99,  99,
	99,  99,  99,  99,  99,  99,  99,  99,
	99,  99,  99,  99,  99,  99,  99,  99
};
static const char zz_order[DCTSIZE2]= 
{
	 0,  1,  5,  6, 14, 15, 27, 28,
	 2,  4,  7, 13, 16, 26, 29, 42,
	 3,  8, 12, 17, 25, 30, 41, 43,
	 9, 11, 18, 24, 31, 40, 44, 53,
	10, 19, 23, 32, 39, 45, 52, 54,
	20, 22, 33, 38, 46, 51, 55, 60,
	21, 34, 37, 47, 50, 56, 59, 61,
	35, 36, 48, 49, 57, 58, 62, 63
};
static const u8 bits_dc_luminance[17]={ 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
static const u8 val_dc_luminance[]={ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }; 
static const u8 bits_dc_chrominance[17]={ 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
static const u8 val_dc_chrominance[]={ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };  
static const u8 bits_ac_luminance[17]={ 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
static const u8 bits_ac_chrominance[17]={ 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
static const u8 val_ac_luminance[]=
{ 
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa 
};  
static const u8 val_ac_chrominance[]=
{ 
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa 
}; 


void Face_Dectection(int* res, unsigned int addr);//编码主函数
/////////////////初始化函数组/////////////////////////////////////////////////////
jpeg_compress_info *jpeg_create_compress(void);//创建压缩文件信息
void jpeg_set_default(jpeg_compress_info *cinfo);//设置默认参数 
void jint_std_quant_tables(jpeg_compress_info *cinfo);//设置标准量化表
void jint_std_huff_tables (jpeg_compress_info *cinfo);//设置标准huffman表 
void jint_add_quant_table(jpeg_compress_info *cinfo,u16 which_tbl,const u16 *base_tbl,float quality);//添加量化表 						   
void jint_add_huff_table(jpeg_compress_info *cinfo,u16 which_tbl,int what_tbl,const u8 *bits,const u8 *value);//添加huffman表
void jutl_zigzag_sort(DCTBLOCK dptr);//Z变换 
void jint_calc_huff_tbl(JHUFF_TBL *htblptr);//生成huffman表
void jpeg_calc_value(jpeg_compress_info *cinfo);
/////////////////写头信息函数组///////////////////////////////////////////////////
void jpeg_start_compress(jpeg_compress_info *cinfo);//开始压缩编码,写入头信息 
void jmkr_write_start(jpeg_compress_info *cinfo);//Z形排序
void jmkr_write_image_start(jpeg_compress_info *cinfo);//写入文件开始标志
void jmkr_write_jfif(jpeg_compress_info *cinfo);//写入APP0应用数据段
void jmkr_write_quant_table(jpeg_compress_info *cinfo);//写入量化表
void jmkr_write_frame_head(jpeg_compress_info *cinfo);//写入帧头
void jmkr_write_huff_table(jpeg_compress_info *cinfo);//写入哈夫曼表
void jmkr_write_scan_head(jpeg_compress_info *cinfo);//写入扫描开始标志
void jmkr_write_end(jpeg_compress_info *cinfo);//写文件结束标志 
/////////////////扫描编码函数组///////////////////////////////////////////////////
void jint_process_rows(jpeg_compress_info *cinfo, int* res);//行压缩函数
void jutl_cc_rgb2ycc(JSAMPLE *data,int num, int* res);//RGB转YUV
void jint_process_mcu(jpeg_compress_info *cinfo);//处理元件中的MCU数据 
void dsp_fdct_8x8(short *dct_data, unsigned num_fdcts);//快速离散余弦变换
void jhuff_encode_one_block(jpeg_compress_info *cinfo,DCTBLOCK src,JHUFF_TBL *dc_tbl,JHUFF_TBL *ac_tbl);//huffman编码 
void jpeg_finish_compress(jpeg_compress_info *cinfo);//完成缓冲区中剩下数据的压缩编码,并写文件尾到输出文件
/////////////////////其它函数组///////////////////////////////////////////////////
void jhuff_flush_buffer(jpeg_compress_info *cinfo);//清除/释放缓冲区
void jhuff_write_bits(jpeg_compress_info *cinfo,u32 data,int len);//写比特 
int  bit_num(int a); 

#endif
