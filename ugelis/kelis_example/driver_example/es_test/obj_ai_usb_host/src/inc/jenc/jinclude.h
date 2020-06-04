#ifndef JINCLUDE_H
#define JINCLUDE_H

#include <stdio.h>

/* define basic type
 */
typedef int   INT32;
typedef short INT16;
typedef char  INT8;
typedef unsigned int   UINT32;
typedef unsigned short UINT16;
typedef unsigned char  UINT8;

/* jpeg input sample unit 0~255
 */
typedef UINT8 JSAMPLE;

/* define useful constant value
 */
#define DCTSIZE   8
#define DCTSIZE2 64

#define MAX_COMPONENT  4
#define NUM_HUFF_TBLS  2  /* # of huffman table, for basic 2, for other 4.   */
#define NUM_QUANT_TBLS 4  /* # of quant table, default 4, but we just use 2. */

#define MAX_BLOCK_IN_MCU 10 /* ISO Specifid. */

/* this two value not only used in table select but also for evalute */
#define DC_HUFF_TBL 0
#define AC_HUFF_TBL 1

typedef INT16  DCTVAL;
typedef DCTVAL DCTBLOCK[DCTSIZE2];

typedef struct
{
    DCTBLOCK quantval;
} JQUANT_TBL;

//17+256 + 1024

typedef struct
{
    UINT8 bits[17]; /* zero not use */
    UINT8 huffval[256];

    UINT16 esize[256];
    UINT16 ecode[256];
} JHUFF_TBL;

typedef enum
{
    JC_CREATING,
    JC_SETTING,
    JC_START,
    JC_FINISH,
    JC_DESTORING
} JC_STATE;

typedef enum
{
    JCS_UNKNOWN,  /* for error */
    JCS_GRAY,
    JCS_RGB,
    JCS_CMYK,
    JCS_YCbCr,
    JCS_YUV
} JC_COLORSPACE;

/* Basic struct define start here.
 */

/* component struct */
typedef struct
{
    /* component identify */
    UINT8 comp_id;    /* 0 ~ 255 */
    UINT8 comp_index; /* index in jpeg_compress_info.comp[...] */
    /* information in frame head component table */
    int h_factor;
    int v_factor;
    int quant_tbl_no; /* quant table selector */
    /* information in scan head component table */
    int dc_tbl_no;    /* DC cofficent huffman code table selector */
    int ac_tbl_no;    /* AC cofficent huffman code table selector */
    /* compress buffer used to process this 'color' compress.
     * initialize it in jpeg_calculate_value().
     */
    int   num_dct_table;
    DCTBLOCK *dct_table;
    /* prev-value */
    DCTVAL preval;
} jpeg_component_info;

/* compress information struct */
typedef struct
{
    /* Basic value, client should specified this before jpeg_set_value().
     */
    int image_width;
    int image_height;

    //FILE *output;
    char *output;

    /* basic forward DCT based config value. */
    int precision;  /* sample precision,  default to 8 */
    float quality;

    /* color component value */
    JC_COLORSPACE in_color_space;
    int num_comp;   /* # of color component, default to 3 for YCrCb */
    jpeg_component_info comp[MAX_COMPONENT];
    /* components value */

    /* Value need to be calculate , in jpeg_set_value().
     */
    int mcu_per_row;  /* # of MCUs across image. */
    int mcu_rows;     /* # of MCU rows in image. */
    int block_in_mcu; /* # of blocks per MCU */

    int mcu_width;    /* # of pixels per MCU, horizontally */
    int mcu_height;   /* # of pixels per MCU, vertically */
    int mcu_blocks;   /* mcu_width * mcu_height */

    int max_h_factor;
    int max_v_factor;

    JHUFF_TBL *dc_huff_table[NUM_HUFF_TBLS];
    JHUFF_TBL *ac_huff_table[NUM_HUFF_TBLS];

    JQUANT_TBL *quant_table[NUM_QUANT_TBLS];

    /* We cache the image to this buffer for Color-Convent & Down-Sample. We use
     * write_scan_line to fill value to this buffer and when this buffer full
     * filled we make a compress of row-MCU.
     */
    JSAMPLE *inbuf;
    int inbuf_width;  /* # of pixel across buffer  */
    int inbuf_height; /* # of pixel rows in buffer */
    int inbuf_size;   /* buffer size, NOT EQUATE inbuf_width * inbuf_height ! */

    /* State value, will changed in processing
     */
    JC_STATE state;
    int  next_line;

} jpeg_compress_info;

#endif /*JINCLUDE_H*/


