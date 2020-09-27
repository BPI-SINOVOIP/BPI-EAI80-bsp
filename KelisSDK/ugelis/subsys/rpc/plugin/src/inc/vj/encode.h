#ifndef _JENCODE_H
#define _JENCODE_H

//#include <stm32f4xx.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

/****************************************************************************
	�������ӹ�����
	GD STM32F407ѧϰ��
	�Ա��꣺http://shop71381140.taobao.com/
	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
****************************************************************************/	 
#define DCTSIZE   8
#define DCTSIZE2 64
#define MAX_COMPONENT  4
#define NUM_HUFF_TBLS  2  //huffman�������,����ϵͳ��Ϊ2
#define NUM_QUANT_TBLS 4  //�����������,Ĭ��Ϊ4,������ʹ��2
#define MAX_BLOCK_IN_MCU 10 //ISO�涨��
#define DC_HUFF_TBL 0     
#define AC_HUFF_TBL 1//�������������ڱ�ѡ�������
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
//ɫ�ʿռ��б�
typedef enum {
  	JCS_UNKNOWN,
  	JCS_GRAY,
  	JCS_RGB,
  	JCS_CMYK,
  	JCS_YCbCr,
  	JCS_YUV
} JC_COLORSPACE;
//״̬�б�
typedef enum {
  	JC_CREATING,
  	JC_SETTING,
  	JC_START,
  	JC_FINISH,
  	JC_DESTORING
} JC_STATE;
//��־�ֽ�
typedef enum {
  	SOF0=0xC0,//basic dct 
  	SOF1=0xC1,//extend dct
  	DHT =0xC4,//����huffman��
  	SOI =0xD8,//֡��ʼ
  	EOI =0xD9,//֡����
  	SOS =0xDA,//ɨ�迪ʼ
  	DQT =0xDB,//����������
  	COM =0xDE //common
} MARKER;
//Ԫ���ṹ��
typedef struct {
	u8 comp_id;//Ԫ��ID
	u8 comp_index;//Ԫ����JPEG�ļ���Ϣ�е�λ��
	int h_factor;
	int v_factor;
	int quant_tbl_no;//���ѡ��
	int dc_tbl_no;//ֱ��������������ѡ��
	int ac_tbl_no;//����������������ѡ��
	int num_dct_table;
	DCTBLOCK *dct_table;
	DCTVAL preval;
} jpeg_component_info;
//ѹ���ļ���Ϣ�ṹ��
typedef struct {
  	int image_width;  
  	int image_height;
  	char *output; //JPEG�������ָ��
  	int precision;//��������,Ĭ��Ϊ8
  	float quality;
  	JC_COLORSPACE in_color_space;
  	int num_comp;//��ɫԪ����,Ĭ��Ϊ3(YCrCb��ɫͼ)
  	jpeg_component_info comp[MAX_COMPONENT];//JPEGԪ����Ϣ
  	int mcu_per_row;//����MCU��Ԫ��
  	int mcu_rows;   //ÿ��MCU��Ԫ��
  	int block_in_mcu;//ÿ��MCU��Ԫ�Ŀ��� 
  	int mcu_width;//MCU��Ԫˮƽ����
  	int mcu_height; //MCU��Ԫ��ֱ����
  	int mcu_blocks; //MCU��Ԫ��� 
  	int max_h_factor;
  	int max_v_factor; 
  	JHUFF_TBL *dc_huff_table[NUM_HUFF_TBLS];//ֱ��Huffman��
  	JHUFF_TBL *ac_huff_table[NUM_HUFF_TBLS];//����Huffman��  
  	JQUANT_TBL *quant_table[NUM_QUANT_TBLS];//������
    JSAMPLE *inbuf;//��ɫת��,��ɨ�軺����
  	int inbuf_width;  //�����
  	int inbuf_height; //�����
  	int inbuf_size;   //��������С,����ͬ�ڻ����*�����
  	JC_STATE state;//״̬����
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


void Face_Dectection(int* res, unsigned int addr);//����������
/////////////////��ʼ��������/////////////////////////////////////////////////////
jpeg_compress_info *jpeg_create_compress(void);//����ѹ���ļ���Ϣ
void jpeg_set_default(jpeg_compress_info *cinfo);//����Ĭ�ϲ��� 
void jint_std_quant_tables(jpeg_compress_info *cinfo);//���ñ�׼������
void jint_std_huff_tables (jpeg_compress_info *cinfo);//���ñ�׼huffman�� 
void jint_add_quant_table(jpeg_compress_info *cinfo,u16 which_tbl,const u16 *base_tbl,float quality);//��������� 						   
void jint_add_huff_table(jpeg_compress_info *cinfo,u16 which_tbl,int what_tbl,const u8 *bits,const u8 *value);//���huffman��
void jutl_zigzag_sort(DCTBLOCK dptr);//Z�任 
void jint_calc_huff_tbl(JHUFF_TBL *htblptr);//����huffman��
void jpeg_calc_value(jpeg_compress_info *cinfo);
/////////////////дͷ��Ϣ������///////////////////////////////////////////////////
void jpeg_start_compress(jpeg_compress_info *cinfo);//��ʼѹ������,д��ͷ��Ϣ 
void jmkr_write_start(jpeg_compress_info *cinfo);//Z������
void jmkr_write_image_start(jpeg_compress_info *cinfo);//д���ļ���ʼ��־
void jmkr_write_jfif(jpeg_compress_info *cinfo);//д��APP0Ӧ�����ݶ�
void jmkr_write_quant_table(jpeg_compress_info *cinfo);//д��������
void jmkr_write_frame_head(jpeg_compress_info *cinfo);//д��֡ͷ
void jmkr_write_huff_table(jpeg_compress_info *cinfo);//д���������
void jmkr_write_scan_head(jpeg_compress_info *cinfo);//д��ɨ�迪ʼ��־
void jmkr_write_end(jpeg_compress_info *cinfo);//д�ļ�������־ 
/////////////////ɨ����뺯����///////////////////////////////////////////////////
void jint_process_rows(jpeg_compress_info *cinfo, int* res);//��ѹ������
void jutl_cc_rgb2ycc(JSAMPLE *data,int num, int* res);//RGBתYUV
void jint_process_mcu(jpeg_compress_info *cinfo);//����Ԫ���е�MCU���� 
void dsp_fdct_8x8(short *dct_data, unsigned num_fdcts);//������ɢ���ұ任
void jhuff_encode_one_block(jpeg_compress_info *cinfo,DCTBLOCK src,JHUFF_TBL *dc_tbl,JHUFF_TBL *ac_tbl);//huffman���� 
void jpeg_finish_compress(jpeg_compress_info *cinfo);//��ɻ�������ʣ�����ݵ�ѹ������,��д�ļ�β������ļ�
/////////////////////����������///////////////////////////////////////////////////
void jhuff_flush_buffer(jpeg_compress_info *cinfo);//���/�ͷŻ�����
void jhuff_write_bits(jpeg_compress_info *cinfo,u32 data,int len);//д���� 
int  bit_num(int a); 

#endif
