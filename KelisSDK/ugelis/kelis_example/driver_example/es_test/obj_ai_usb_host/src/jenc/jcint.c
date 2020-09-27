/* We used function ceil(); */
//#include <math.h>
#include <ugelis.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jinclude.h>
#include <jcint.h>

extern JQUANT_TBL  JQUANT_TBL_2[2];

extern JHUFF_TBL  JHUFF_TBL_4[4];

extern void jutl_cc_rgb2ycc(JSAMPLE *data, int num);
extern void dsp_fdct_8x8(short *dct_data, unsigned num_fdcts);
extern void jutl_zigzag_sort(DCTBLOCK dptr);
extern void  jhuff_encode_one_block(jpeg_compress_info *cinfo, DCTBLOCK src,
                                    JHUFF_TBL *dc_tbl, JHUFF_TBL *ac_tbl);
extern void  show_dcttable(DCTBLOCK val, char *message);

/* This function is internal implemention, you shouldn't use this function
 * nomarlly. But If you want to invoke it, you must clear that 'cinfo->inbuf'
 * must all (full-filled) with you valide data. Otherwise you will not get right
 * compress data.
 */
void jint_process_rows(jpeg_compress_info *cinfo)
{

    int mn; /* current mcu in buffer */
    JSAMPLE *mcu_start;
    int mcu_delta;

    JSAMPLE *pdata;

    /* Color convertion invoke select.
     * Note: We just implemente RGB -> YCrCb converte.
     */
    if (cinfo->in_color_space == JCS_RGB)
        jutl_cc_rgb2ycc(cinfo->inbuf,
                        cinfo->inbuf_width * cinfo->inbuf_height);
#ifdef DEBUG
    show_inbuf(cinfo, "--- color convertion ---");
#endif

    /* Process each of MCU data.
     * Separate the 'inbuf' data to each color component's dct table,
     * and 'Make' down sample for every component.
     */
    mcu_start = &cinfo->inbuf[0];
    mcu_delta = cinfo->mcu_width * cinfo->num_comp * sizeof(JSAMPLE);
    for (mn = 0; mn < cinfo->mcu_per_row; mn++)
    {
        /* For every color component.
         * Down sample and save to each color component.
         */
        int cn;  /* component counter */
        for (cn = 0; cn < cinfo->num_comp; cn++)
        {
            JSAMPLE *comp_start = (JSAMPLE *)(mcu_start + cn);
            /* init data-point sample delta */
            int h_samp_delta = cinfo->max_h_factor / cinfo->comp[cn].h_factor
                               * cinfo->num_comp * sizeof(JSAMPLE);
            int v_samp_delta = cinfo->max_v_factor / cinfo->comp[cn].v_factor
                               * cinfo->num_comp * sizeof(JSAMPLE)
                               * cinfo->inbuf_width;

            /* for every block */
            int bn;  /* block counter */
            int pn;  /* point of data */
            for (bn = 0; bn < cinfo->comp[cn].num_dct_table; bn++)
            {
                /* init block start address */
                JSAMPLE *block_start = (JSAMPLE *)(comp_start
                                                   + (bn % cinfo->comp[cn].h_factor) * DCTSIZE * h_samp_delta
                                                   + (bn / cinfo->comp[cn].v_factor) * DCTSIZE * v_samp_delta);
                /* dct table element */
                DCTVAL *dct = (DCTVAL *)&cinfo->comp[cn].dct_table[bn];

                /* for each element  */
                for (pn = 0; pn < DCTSIZE2; pn++)
                {

                    if (pn % DCTSIZE == 0)
                    {
                        pdata = (JSAMPLE *)(pn / DCTSIZE * v_samp_delta + block_start);
                    }
                    dct[pn] = *pdata;
                    pdata += h_samp_delta;
                }
            }
        } /* End of color component loop */
        mcu_start += mcu_delta;

#ifdef DEBUG
        show_comp_dcttable(cinfo, "--- down sample ---");
#endif

        /* Process current MCU data.
         */
        jint_process_mcu(cinfo);
        /* for example : call jint_process_mcu(cinfo) */
    }
}

/* Process one MCU data in all component.
 */

extern volatile int temp_dct1[64];

void jint_process_mcu(jpeg_compress_info *cinfo)
{
    /* process every color component */
    int cn, tn;
    for (cn = 0; cn < cinfo->num_comp; cn++)
    {
        /* Variable used in component. */
        DCTBLOCK   *dct_table = cinfo->comp[cn].dct_table;
        JQUANT_TBL *quant_table = cinfo->quant_table[cinfo->comp[cn].quant_tbl_no];
        JHUFF_TBL  *dc_table = cinfo->dc_huff_table[cinfo->comp[cn].dc_tbl_no];
        JHUFF_TBL  *ac_table = cinfo->ac_huff_table[cinfo->comp[cn].ac_tbl_no];

        /* process every DCT table -- 'Block'
         */
        for (tn = 0; tn < cinfo->comp[cn].num_dct_table; tn++)
        {
            DCTVAL tmp = 0;
            DCTVAL *val = dct_table[tn];
            int i;
#ifdef DEBUG
            show_dcttable(val, "\n\t[basic val]");
#endif

            /* data offset for Forward-DCT
             */
            for (i = 0; i < DCTSIZE2; i++)
            {
                val[i] -= 128;
            }
#ifdef DEBUG
            show_dcttable(val, "[val -= 128]");
#endif

            /* Forward-DCT calculate
             */
            //jjj
            /* fdct_8x8(val); */

            for (i = 0; i < 64; i++)
            {
                temp_dct1[i] = val[i];
            }

            dsp_fdct_8x8((short *)val, 1);
            //show_dcttable(val, "[fdct_8x8]");
            //#ifdef DEBUG
            //show_dcttable(val, "[fdct_8x8]");
            //#endif

            /* Zig-Zag Sort
             */
            jutl_zigzag_sort((INT16 *)val);
            //jutl_zigzag_sort((DCTBLOCK *)val);
#ifdef DEBUG
            show_dcttable(val, "[zig-zag sort]");
#endif

            /* Quant
             */
            for (i = 0; i < DCTSIZE2; i++)
            {
                val[i] = (DCTVAL)((float)val[i] / (float)quant_table->quantval[i] + 0.5);
            }
#ifdef DEBUG
            show_dcttable(quant_table->quantval, "[quant-table]");
            show_dcttable(val, "[quant]");
#endif

            /* Difference encode calculate */
            tmp = val[0];
            val[0] -= cinfo->comp[cn].preval;
            cinfo->comp[cn].preval = tmp;
#ifdef DEBUG
            show_dcttable(val, "[diff code]");
#endif

            /* Huffman */
            jhuff_encode_one_block(cinfo, val, dc_table, ac_table);
        }
    }

    /*
    {
      static int rstm = 0;
      jhuff_flush_buffer(cinfo);
      rstm &= 0x07;
      jutl_write_byte(0xFF, cinfo->output);
      jutl_write_byte(0xD0|rstm, cinfo->output);
      rstm ++;
    }
    */
}

void jint_add_quant_table(jpeg_compress_info *cinfo,
                          unsigned int which_tbl, const UINT16 *base_tbl,
                          float quality)
{
    int i;
    DCTVAL *qtblptr;
    cinfo->quant_table[which_tbl] = &JQUANT_TBL_2[which_tbl];//(JQUANT_TBL *)malloc(sizeof(JQUANT_TBL));

    qtblptr = cinfo->quant_table[which_tbl]->quantval;
    for (i = 0; i < DCTSIZE2; i++)
    {
        qtblptr[i] = (INT16)(base_tbl[i] * quality + 0.5);
    }
    jutl_zigzag_sort(qtblptr);
}

void jint_std_quant_tables(jpeg_compress_info *cinfo)
{
    static const INT16 std_luminance_quant_tbl[DCTSIZE2] =
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
    static const INT16 std_chrominance_quant_tbl[DCTSIZE2] =
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
    jint_add_quant_table(cinfo, 0, std_luminance_quant_tbl, cinfo->quality);
    jint_add_quant_table(cinfo, 1, std_chrominance_quant_tbl, cinfo->quality);
}

void jint_add_huff_table(jpeg_compress_info *cinfo,
                         unsigned int which_tbl, int what_tbl,
                         const UINT8 *bits, const UINT8 *value)
{
    JHUFF_TBL **hpp;   /* this value used to Left-Value */
    int nsymbols, len;

    if (which_tbl > NUM_HUFF_TBLS)
    {
        return;
    }
    //exit(1);

    if (what_tbl == 0) /* DC table */
    {
        hpp = cinfo->dc_huff_table;
    }
    else if (what_tbl == 1) /* AC table */
    {
        hpp = cinfo->ac_huff_table;
    }
    else
    {
        return ;
    }
    //exit(1);

    /* set value of 'bits' */
    hpp[which_tbl] = &JHUFF_TBL_4[which_tbl * 2 + what_tbl]; //(JHUFF_TBL *)malloc1;//(sizeof(JHUFF_TBL));
    memcpy(hpp[which_tbl]->bits, bits, sizeof(hpp[which_tbl]->bits));

    /* calculate # of symbols in huffval */
    nsymbols = 0;
    for (len = 1; len <= 16; len++)
    {
        nsymbols += bits[len];
    }
    /* set huffval */
    memcpy(hpp[which_tbl]->huffval, value, nsymbols * sizeof(UINT8));

    /* calculate and set huffman code & size table
     * across this we can use 'ECODE & ESIZE' table in huffman encoding.
     */
    memset(hpp[which_tbl]->ecode, 0, sizeof(hpp[which_tbl]->ecode));
    memset(hpp[which_tbl]->esize, 0, sizeof(hpp[which_tbl]->esize));
    jint_calc_huff_tbl(hpp[which_tbl]);
}

void jint_std_huff_tables(jpeg_compress_info *cinfo)
{
    /* All these table specified by JPEG */

    static const UINT8 bits_dc_luminance[17] =
    { /* 0-base */ 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
    static const UINT8 val_dc_luminance[] =
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    static const UINT8 bits_dc_chrominance[17] =
    { /* 0-base */ 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
    static const UINT8 val_dc_chrominance[] =
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    static const UINT8 bits_ac_luminance[17] =
    { /* 0-base */ 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
    static const UINT8 val_ac_luminance[] =
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

    static const UINT8 bits_ac_chrominance[17] =
    { /* 0-base */ 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
    static const UINT8 val_ac_chrominance[] =
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

    jint_add_huff_table(cinfo, 0, DC_HUFF_TBL,
                        bits_dc_luminance, val_dc_luminance);
    jint_add_huff_table(cinfo, 0, AC_HUFF_TBL,
                        bits_ac_luminance, val_ac_luminance);
    jint_add_huff_table(cinfo, 1, DC_HUFF_TBL,
                        bits_dc_chrominance, val_dc_chrominance);
    jint_add_huff_table(cinfo, 1, AC_HUFF_TBL,
                        bits_ac_chrominance, val_ac_chrominance);
}

/* generate huffman table
 */
static UINT16 gtmp_hsize[256];
static UINT16 gtmp_hcode[256];

void jint_calc_huff_tbl(JHUFF_TBL *htblptr)
{
    int i, j;
    int k, lastk, code, si;

    /* generate EHUFFSI table */
    k = 0;
    for (i = 1; i <= 16; i++)
    {
        for (j = 1; j <= htblptr->bits[i]; j++)
        {
            gtmp_hsize[k] = i;
            k++;
        }
    }
    gtmp_hsize[k] = 0;
    lastk = k;

    /* generate EHUFFCO table */
    k = 0;
    code = 0;
    si = gtmp_hsize[0];
    while (1)
    {
        while (1)
        {
            gtmp_hcode[k] = code;
            code++;
            k++;
            if (gtmp_hsize[k] != si)
            {
                break;
            }
        }
        if (gtmp_hsize[k] == 0)
        {
            break;
        }
        while (1)
        {
            code = code << 1;
            si++;
            if (gtmp_hsize[k] == si)
            {
                break;
            }
        }
    }

    /* order_codes */
    k = 0;
    do
    {
        i = htblptr->huffval[k];
        htblptr->ecode[i] = gtmp_hcode[k];
        htblptr->esize[i] = gtmp_hsize[k];
        k++;
    } while (k < lastk);
}


