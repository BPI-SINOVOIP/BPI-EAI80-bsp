#ifndef JCINIT_H
#define JCINIT_H

/* We initalnate the down sample & color convert routine in this function.
 * So it's "Hard Coded" in our implemention, you cann't change this unless re-
 * implemention this function by your self. Sorry !
 */
/* data process */
void jint_process_rows(jpeg_compress_info *cinfo);
void jint_process_mcu(jpeg_compress_info *cinfo);

/* parameter configure */
void jint_std_quant_tables(jpeg_compress_info *cinfo);
void jint_add_quant_table(jpeg_compress_info *cinfo,
                          unsigned int which_tbl, const UINT16 *base_tbl,
                          float quality);
void jint_std_huff_tables(jpeg_compress_info *cinfo);
void jint_add_huff_table(jpeg_compress_info *cinfo,
                         unsigned int which_tbl, int what_tbl,
                         const UINT8 *bits, const UINT8 *value);
void jint_calc_huff_tbl(JHUFF_TBL *htblptr);


/**
 * Function Used to Write JPEG Marker.
 * Implement at jmarker.c
 */
void jmkr_write_jfif(jpeg_compress_info *cinfo);
void jmkr_write_dri(jpeg_compress_info *cinfo);

void jmkr_write_image_start(jpeg_compress_info *cinfo);
void jmkr_write_image_end(jpeg_compress_info *cinfo);

void jmkr_write_frame_head(jpeg_compress_info *cinfo);
void jmkr_write_scan_head(jpeg_compress_info *cinfo);

void jmkr_write_quant_table(jpeg_compress_info *cinfo);
void jmkr_write_huff_table(jpeg_compress_info *cinfo);

void jmkr_write_start(jpeg_compress_info *cinfo);
void jmkr_write_end(jpeg_compress_info *cinfo);

#endif /*JCINIT_H*/


