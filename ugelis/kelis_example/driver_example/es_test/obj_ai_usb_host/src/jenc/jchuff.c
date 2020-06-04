#include "jinclude.h"

void  jhuff_flush_buffer(jpeg_compress_info *cinfo);
int bit_num(int a);
void jhuff_write_bits(jpeg_compress_info *cinfo, UINT32 data, int len);
void  jhuff_flush_buffer(jpeg_compress_info *cinfo) ;
void  jhuff_encode_one_block(jpeg_compress_info *cinfo, DCTBLOCK src,
                             JHUFF_TBL *dc_tbl, JHUFF_TBL *ac_tbl);


extern void jutl_write_byte(UINT8 c, char *f);

/* huffman encode OUTPUT
 */
union buffer_tag
{
    UINT32 all;
    UINT8 c[4];
};
static union buffer_tag buffer;
static int bits_in_buffer;

int bit_num(int a)
{
    int bit = 0;
    if (a == 0)
    {
        return 0;
    }
    a = a > 0 ? a : -a;
    while ((1 << bit) <= a)
    {
        bit++;
    }
    return bit;
}

void jhuff_write_bits(jpeg_compress_info *cinfo, UINT32 data, int len)
{
#ifdef DEBUG
    printf("(b:%d;%d/%d)", bits_in_buffer, data, len);
#endif
    UINT32 tdata;

    while (len > 0)
    {
        tdata = (data << (32 - len));
        buffer.all |= ((tdata >> bits_in_buffer));
        bits_in_buffer += len;
        /* output buffer */
        if (bits_in_buffer >= 32)
        {
            int i;
            /* must be BIG ENDIAN */
            for (i = 3; i >= 0; i--)
            {
                jutl_write_byte(buffer.c[i], cinfo->output);
                if (buffer.c[i] == 0xFF)
                {
                    jutl_write_byte(0x00, cinfo->output);
                }
            }
            buffer.all = 0;
            len = bits_in_buffer - 32;
            bits_in_buffer = 0;
        }
        else
        {
            len = 0;
        }
    }
#ifdef DEBUG
    printf("..buffer..%08X\n", buffer.all);
#endif
}

void  jhuff_flush_buffer(jpeg_compress_info *cinfo)
{
    int i;
    i = 3;
#ifdef DEBUG
    printf(" <FB:%08X,%d ", buffer.all, bits_in_buffer);
#endif

    while (bits_in_buffer > 8)
    {
        jutl_write_byte(buffer.c[i], cinfo->output);
        if (buffer.c[i] == 0xFF)
        {
            jutl_write_byte(0x00, cinfo->output);
        }
        i--;
        bits_in_buffer -= 8;
    }
    if (bits_in_buffer > 0)
    {
        buffer.c[i] |= ((unsigned char)(0xFF) >> bits_in_buffer);
        jutl_write_byte(buffer.c[i], cinfo->output);
        if (buffer.c[i] == 0xFF)
        {
            jutl_write_byte(0x00, cinfo->output);
        }
    }
    buffer.all = 0;
    bits_in_buffer = 0;
}

/* huffman encode
 */
void  jhuff_encode_one_block(jpeg_compress_info *cinfo, DCTBLOCK src,
                             JHUFF_TBL *dc_tbl, JHUFF_TBL *ac_tbl)
{
    int diff, ssss, rrrr, code;
    int rs, k;

    /* encode DC */
    diff = src[0];
    ssss = bit_num(diff);
#ifdef DEBUG
    printf(" <DC:%d/%d\n", diff, ssss);
#endif
    jhuff_write_bits(cinfo, dc_tbl->ecode[ssss], dc_tbl->esize[ssss]);
    if (diff < 0)
    {
        diff -= 1;
    }
    jhuff_write_bits(cinfo, diff, ssss);

    /* encode AC */
    rrrr = 0;
    for (k = 1; k < DCTSIZE2; k++)
    {
        if (src[k] == 0)
        {
            rrrr++;
            continue;
        }
        else
        {
            while (rrrr > 15)
            {
                /* write EHUFSI(0xF0) bits of EHUFCO(0xF0) */
#ifdef DEBUG
                show_msg(" <AC:15,0\n");
#endif
                jhuff_write_bits(cinfo, ac_tbl->ecode[0xF0], ac_tbl->esize[0xF0]);
                rrrr -= 16;
            }
            ssss = bit_num(src[k]);
            rs = ((rrrr << 4) | ssss);
#ifdef DEBUG
            printf(" <AC:%d,%d\n", rrrr, src[k]);
#endif
            /* write EHUFSI(rs) bits of EHUFCO(rs) */
            jhuff_write_bits(cinfo, ac_tbl->ecode[rs], ac_tbl->esize[rs]);
            code = src[k];
            if (code < 0)
            {
                code -= 1;
            }
            /* write ssss bits of rs */
            jhuff_write_bits(cinfo, code, ssss);
            rrrr = 0;
        }
    }
    if (rrrr != 0)
    {
        /* write EOB */
#ifdef DEBUG
        show_msg(" <AC:EOB\n");
#endif
        jhuff_write_bits(cinfo, ac_tbl->ecode[0x00], ac_tbl->esize[0x00]);
    }
}


