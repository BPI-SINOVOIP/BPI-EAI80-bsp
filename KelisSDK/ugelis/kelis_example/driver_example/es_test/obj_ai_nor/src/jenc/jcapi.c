/* JPEG API Implemention File.
 */
#include <stdlib.h>
#include <string.h>
//#include <mem.h>

#include "jinclude.h"

#include "jcapi.h"
#include "jcint.h"

//extern unsigned char dcttab[3][512];
//extern unsigned char inbuf_buf[11520];


extern unsigned char dcttab[3 * 2 * 2][512 * 2 * 2];
extern unsigned char inbuf_buf[11520 * 2 * 2];


extern void  jhuff_flush_buffer(jpeg_compress_info *cinfo);
extern jpeg_compress_info info1;

/* Create a compress information object.
 */
jpeg_compress_info *jpeg_create_compress(void)
{
    jpeg_compress_info *cinfo = &info1;
    //(jpeg_compress_info *)malloc(sizeof(jpeg_compress_info));

    /* initialize all element to zero */
    memset((void *)(cinfo), 0, sizeof(jpeg_compress_info));
    cinfo->state = JC_CREATING;
    return cinfo;
}

/* Destory compress information object.
 */
void jpeg_destory_compress(jpeg_compress_info *cinfo)
{
    int i;

    if (cinfo->state != JC_FINISH)
        ;
    cinfo->state = JC_DESTORING;

    /* free */
    //free(cinfo->inbuf);
    //for (i=0; i<cinfo->num_comp; i++) {
    //  free(cinfo->comp[i].dct_table);
    //}
    //for (i=0; i<NUM_QUANT_TBLS; i++) {
    //  free(cinfo->quant_table[i]);
    //}
    //for (i=0; i<NUM_HUFF_TBLS; i++) {
    // free(cinfo->dc_huff_table[i]);
    // free(cinfo->ac_huff_table[i]);
    //}

    //free(cinfo);
}

/* Set default value of jpeg compress.
 * In our implemention we set Forward-DCT based JPEG Compress basic value.
 * Like : 8 (not 12) for precision or 3 for num_comp.
 *
 * If you want use other Method of JPEG Compress, please set the cinfo self,
 * not use this function. But don't forget invoke jpeg_calc_value to auto set,
 * and initialize process buffer.
 */
void jpeg_set_default(jpeg_compress_info *cinfo)
{
    cinfo->precision = 8;
    cinfo->in_color_space = JCS_RGB;
    cinfo->quality = 1.0f;

    /* It's a good method to avoid foo code. */
#define SET_COMP(TARGET,ID,H,V,Tq,Td,Ta) \
    ( TARGET.comp_id = (ID),\
      TARGET.comp_index = (ID),\
      TARGET.h_factor = (H),\
      TARGET.v_factor = (V),\
      TARGET.quant_tbl_no = (Tq),\
      TARGET.dc_tbl_no = (Td),\
      TARGET.ac_tbl_no = (Ta) )

    /* init color component, as YCbCr. */
    cinfo->num_comp = 3;
    SET_COMP(cinfo->comp[0], 1, 2, 2, 0, 0, 0);
    SET_COMP(cinfo->comp[1], 2, 1, 1, 1, 1, 1);
    SET_COMP(cinfo->comp[2], 3, 1, 1, 1, 1, 1);

    /* set standrad quant tables */
    jint_std_quant_tables(cinfo);
    /* set standrad huffman tables */
    jint_std_huff_tables(cinfo);

    /* calc other value */
    jpeg_calc_value(cinfo);
}

/* Calculate some useful value and Alloc buffer memory.
 */
void jpeg_calc_value(jpeg_compress_info *cinfo)
{
    int i, h, v, bn;
    int size;
    jpeg_component_info *pcomp;

    /* AUTO set */
    cinfo->state = JC_SETTING;

    bn = 0;
    cinfo->max_h_factor = 0;
    cinfo->max_v_factor = 0;

    /* loop every component information
     */
    for (i = 0; i < cinfo->num_comp; i++)
    {
        pcomp = &cinfo->comp[i];
        h = pcomp->h_factor;
        v = pcomp->v_factor;

        if (cinfo->max_h_factor < h)
        {
            cinfo->max_h_factor = h;
        }
        if (cinfo->max_v_factor < v)
        {
            cinfo->max_v_factor = v;
        }
        bn += (h * v);

        /* WARMING !
           Malloc buffer here, but free it in jpeg_destory_compress() funtion.
           So client must invoke it when finish compress. */
        pcomp->num_dct_table = h * v;
        size = sizeof(DCTBLOCK) * pcomp->num_dct_table;
        pcomp->dct_table = (DCTBLOCK *)(dcttab[i]);//malloc(size);
        memset((void *)(pcomp->dct_table), 0, size);
    }

    if (bn > MAX_BLOCK_IN_MCU)
    {
        /*~ err ~*/;
        return ;
    }

    cinfo->block_in_mcu = bn;
    cinfo->mcu_width  = cinfo->max_h_factor * DCTSIZE;
    cinfo->mcu_height = cinfo->max_v_factor * DCTSIZE;
    cinfo->mcu_blocks = cinfo->mcu_width * cinfo->mcu_height;

    /* set value about MCU
     */
    cinfo->mcu_per_row = cinfo->image_width / cinfo->mcu_width;
    if (cinfo->image_width  % cinfo->mcu_width != 0)
    {
        cinfo->mcu_per_row++;
    }
    cinfo->mcu_rows    = cinfo->image_height / cinfo->mcu_height;
    if (cinfo->image_height % cinfo->mcu_height != 0)
    {
        cinfo->mcu_rows++;
    }

    /* set input buffer memory
     */
    cinfo->inbuf_width  = cinfo->mcu_per_row * cinfo->mcu_width;
    cinfo->inbuf_height = cinfo->mcu_height;
    cinfo->inbuf_size = cinfo->inbuf_width * cinfo->inbuf_height
                        * cinfo->num_comp  * sizeof(JSAMPLE);
    cinfo->inbuf = (JSAMPLE *)inbuf_buf;//malloc( cinfo->inbuf_size);
    memset((void *)(cinfo->inbuf), 0, cinfo->inbuf_size);

    /* reset state value
     */
    cinfo->next_line = 0;
}

/* Read image rows and compress.
 */
void jpeg_write_scanline(jpeg_compress_info *cinfo,
                         JSAMPLE *samp_row)
{
    int x, c; /* for loop */
    int pos, size, last;
    //int pos_s, pos_t, bsize;

    if (cinfo->state != JC_START)
    {
        return ;
    }

    /* copy samp_row to color process buffer(cinfo->inbuf).
     */
    size = cinfo->num_comp * sizeof(JSAMPLE);
    pos  = cinfo->inbuf_width * size * (cinfo->next_line % cinfo->inbuf_height);

    //printk("Initaddree:0x%x finish address:0x%08x\n",(&cinfo->inbuf[0]),(&cinfo->inbuf[pos]+cinfo->image_width * size));
    //if((&cinfo->inbuf[pos]+cinfo->image_width * size) > &cinfo->inbuf[pos] +11520);
    ///{
    //  printk("####################################################\n");
    // }

    memcpy(&cinfo->inbuf[pos], samp_row, cinfo->image_width * size);
    //printk("#######\n");

    /* full-filled buffer last-cols
     */
    last = cinfo->image_width - 1;
    pos += cinfo->image_width * size;
    for (x = 0; x < cinfo->inbuf_width - cinfo->image_width; x++)
    {
        for (c = 0; c < cinfo->num_comp; c++)
        {
            //printk("2222finish address:0x%08x\n",&(cinfo->inbuf[pos+x*cinfo->num_comp+c]));
            cinfo->inbuf[pos + x * cinfo->num_comp + c] = samp_row[last * cinfo->num_comp + c];
        }
    }/*
  last  = cinfo->image_width;
  bsize = cinfo->inbuf_width - cinfo->image_width;
  pos_s = pos + (cinfo->image_width - bsize)*size;
  pos_t = pos + last*size;
  memcpy(&cinfo->inbuf[pos_t], &cinfo->inbuf[pos_s], bsize*size);
  */
    cinfo->next_line++;

#ifdef DEBUG
    show_inbuf(cinfo, "--- scan line copy ---");
#endif

    /* When buffer full-filled we compress this data to output. */
    if (cinfo->next_line % cinfo->inbuf_height == 0)
    {
        /* Compress code here.
         */
        jint_process_rows(cinfo);
        memset((void *)(cinfo->inbuf), 0, cinfo->inbuf_size);
    }
}

/* Make compress start.
 * Write compress head to output.
 */
void jpeg_start_compress(jpeg_compress_info *cinfo)
{
    int i;
    if (cinfo->state != JC_SETTING)
    {
        return ;
    }
    /* Add jpeg file heads , include file head & frame head &
       scan head. */
    jmkr_write_start(cinfo);

    /* identify compress start */
    cinfo->state = JC_START;
    /* init stated-variable */
    cinfo->next_line = 0;
    for (i = 0; i < cinfo->num_comp; i++)
    {
        cinfo->comp[i].preval = 0;
    }
}

/* Finish last compress.
 * Compress last data in internal buffer &
 * Write compress end-heads to output.
 */
void jpeg_finish_compress(jpeg_compress_info *cinfo)
{
    int i, startline, rowlen;
    JSAMPLE *data;
    if (cinfo->state != JC_START)
    {
        return ;
    }

    /* Finish compress data in cinfo->inbuf. */
    if (cinfo->next_line % cinfo->inbuf_height != 0)
    {
        rowlen = cinfo->inbuf_width * cinfo->num_comp * sizeof(JSAMPLE);
        startline = cinfo->next_line % cinfo->inbuf_height;
        data  = &cinfo->inbuf[(startline - 1) * rowlen];
        if (startline != 0)
        {
            for (i = startline; i < cinfo->inbuf_height; i++)
            {
                memcpy(&cinfo->inbuf[i * rowlen], data, rowlen);
            }
        }
#ifdef DEBUG
        show_inbuf(cinfo, "--- finish compress ---");
#endif
        jint_process_rows(cinfo);
    }

    jhuff_flush_buffer(cinfo);
    /* Add jpeg file End-Heads .
     * Free memory at jpeg_destroy_compress(cinfo).
     */
    jmkr_write_end(cinfo);

    /* identify compress finish */
    cinfo->state = JC_FINISH;
}

