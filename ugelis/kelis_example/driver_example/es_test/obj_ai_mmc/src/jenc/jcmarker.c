#include <stdio.h>

#include <jinclude.h>
#include <jcint.h>

extern void jutl_write_byte(UINT8 c, char *f);

typedef enum
{
    SOF0 = 0xC0,    /* basic dct */
    SOF1 = 0xC1,    /* extend dct */

    DHT  = 0xC4,    /* huffman table defination */

    SOI  = 0xD8,    /* start of image */
    EOI  = 0xD9,    /* end of image */
    SOS  = 0xDA,    /* start of scan */
    DQT  = 0xDB,    /* quant table defination */

    COM  = 0xDE     /* common */
} MARKER;

void jmkr_write_image_start(jpeg_compress_info *cinfo)
{
    jutl_write_byte(0xFF, cinfo->output);
    jutl_write_byte(SOI, cinfo->output);
}

void jmkr_write_image_end(jpeg_compress_info *cinfo)
{
    jutl_write_byte(0xFF, cinfo->output);
    jutl_write_byte(EOI, cinfo->output);
}

void jmkr_write_frame_head(jpeg_compress_info *cinfo)
{
    int i;
    UINT16 Lf, X, Y;
    UINT8  P, Nf, C, H, V, Tq;
    jutl_write_byte(0xFF, cinfo->output);
    jutl_write_byte(SOF0, cinfo->output);
    Nf = cinfo->num_comp;
    P  = cinfo->precision;
    X  = cinfo->image_width;
    Y  = cinfo->image_height;
    Lf = 8 + 3 * Nf;
    jutl_write_byte(Lf >> 8, cinfo->output);
    jutl_write_byte(Lf & 0xFF, cinfo->output);
    jutl_write_byte(P, cinfo->output);
    jutl_write_byte(Y >> 8, cinfo->output);
    jutl_write_byte(Y & 0xFF, cinfo->output);
    jutl_write_byte(X >> 8, cinfo->output);
    jutl_write_byte(X & 0xFF, cinfo->output);
    jutl_write_byte(Nf, cinfo->output);
    for (i = 0; i < Nf; i++)
    {
        C = cinfo->comp[i].comp_id;
        H = cinfo->comp[i].h_factor;
        V = cinfo->comp[i].v_factor;
        Tq = cinfo->comp[i].quant_tbl_no;
        jutl_write_byte(C, cinfo->output);
        jutl_write_byte(((H << 4) | V), cinfo->output);
        jutl_write_byte(Tq, cinfo->output);
    }
}

void jmkr_write_scan_head(jpeg_compress_info *cinfo)
{
    int i;
    UINT16 Ls;
    UINT8  Ns, Ss, Se, Ah, Al, Cs, Td, Ta;
    jutl_write_byte(0xFF, cinfo->output);
    jutl_write_byte(SOS, cinfo->output);
    Ns = cinfo->num_comp;
    Ls = 6 + 2 * Ns;
    Ss = 0;  /* const!!! not change. */
    Se = 63;
    Ah = Al = 0;
    jutl_write_byte(Ls >> 8, cinfo->output);
    jutl_write_byte(Ls & 0xFF, cinfo->output);
    jutl_write_byte(Ns, cinfo->output);
    for (i = 0; i < Ns; i++)
    {
        Cs = cinfo->comp[i].comp_id;
        Td = cinfo->comp[i].dc_tbl_no;
        Ta = cinfo->comp[i].ac_tbl_no;
        jutl_write_byte(Cs, cinfo->output);
        jutl_write_byte((Td << 4 | Ta), cinfo->output);
    }
    jutl_write_byte(Ss, cinfo->output);
    jutl_write_byte(Se, cinfo->output);
    jutl_write_byte(((Ah << 4) | Al), cinfo->output);
}

void jmkr_write_quant_table(jpeg_compress_info *cinfo)
{
    int i, j;
    for (i = 0; i < NUM_QUANT_TBLS; i++)
    {
        if (!cinfo->quant_table[i])
        {
            continue;
        }
        /* marker */
        jutl_write_byte(0xFF, cinfo->output);
        jutl_write_byte(DQT, cinfo->output);
        /* length, always 43h */
        jutl_write_byte(0x00, cinfo->output);
        jutl_write_byte(0x43, cinfo->output);
        /* ouput Pq & Tq */
        jutl_write_byte(i & 0xFF, cinfo->output);
        /* all element */
        for (j = 0; j < DCTSIZE2; j++)
        {
            UINT8 Q = cinfo->quant_table[i]->quantval[j];
            jutl_write_byte(Q, cinfo->output);
        }
    }
}

void jmkr_write_huff_table(jpeg_compress_info *cinfo)
{
    int i, j, num_val;
    UINT16 Lh;
    /* output DC HUFFMAN CODE */
    for (i = 0; i < NUM_HUFF_TBLS; i++)
    {
        num_val = 0;
        if (!cinfo->dc_huff_table[i])
        {
            continue;
        }
        /* marker */
        jutl_write_byte(0xFF, cinfo->output);
        jutl_write_byte(DHT, cinfo->output);
        for (j = 1; j <= 16; j++)
        {
            num_val += cinfo->dc_huff_table[i]->bits[j];
        }
        /* length, always 43h */
        Lh = 3 + 16 + num_val;
        jutl_write_byte(Lh >> 8, cinfo->output);
        jutl_write_byte(Lh & 0xFF, cinfo->output);
        /* output identify */
        jutl_write_byte(i, cinfo->output);
        /* output bits */
        for (j = 1; j <= 16; j++)
        {
            jutl_write_byte(cinfo->dc_huff_table[i]->bits[j], cinfo->output);
        }
        for (j = 0; j < num_val; j++)
        {
            jutl_write_byte(cinfo->dc_huff_table[i]->huffval[j], cinfo->output);
        }
    }
    /* output AC HUFFMAN CODE */
    for (i = 0; i < NUM_HUFF_TBLS; i++)
    {
        num_val = 0;
        if (!cinfo->ac_huff_table[i])
        {
            continue;
        }
        /* marker */
        jutl_write_byte(0xFF, cinfo->output);
        jutl_write_byte(DHT, cinfo->output);
        for (j = 1; j <= 16; j++)
        {
            num_val += cinfo->ac_huff_table[i]->bits[j];
        }
        /* length, always 43h */
        Lh = 3 + 16 + num_val;
        jutl_write_byte(Lh >> 8, cinfo->output);
        jutl_write_byte(Lh & 0xFF, cinfo->output);
        /* output identify */
        jutl_write_byte((0x10 | i), cinfo->output);
        /* output bits */
        for (j = 1; j <= 16; j++)
        {
            jutl_write_byte(cinfo->ac_huff_table[i]->bits[j], cinfo->output);
        }
        for (j = 0; j < num_val; j++)
        {
            jutl_write_byte(cinfo->ac_huff_table[i]->huffval[j], cinfo->output);
        }
    }
}

void jmkr_write_jfif(jpeg_compress_info *cinfo)
{
    static const UINT8 jfcode[] =
    {
        0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46,
        0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01,
        0x00, 0x00
    };
    int i;
    for (i = 0; i < 18; i++)
    {
        jutl_write_byte(jfcode[i], cinfo->output);
    }
}

void jmkr_write_dri(jpeg_compress_info *cinfo)
{
    static const UINT8 code[] =
    {
        0xFF, 0xDD, 0x00, 0x04, 0x00, 0x01
    };
    int i;
    for (i = 0; i < 6; i++)
    {
        jutl_write_byte(code[i], cinfo->output);
    }
}


void jmkr_write_start(jpeg_compress_info *cinfo)
{
    jmkr_write_image_start(cinfo);
    jmkr_write_jfif(cinfo);
    jmkr_write_quant_table(cinfo);
    jmkr_write_frame_head(cinfo);
    jmkr_write_huff_table(cinfo);
    /*  jmkr_write_dri(cinfo);*/
    jmkr_write_scan_head(cinfo);
}

void jmkr_write_end(jpeg_compress_info *cinfo)
{
    jmkr_write_image_end(cinfo);
}


