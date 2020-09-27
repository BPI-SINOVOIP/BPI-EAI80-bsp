/*------------------------------------------------------------------------
 *  Copyright 2007-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#include <config_zbar.h>
#include <stdlib.h>     /* malloc, free, abs */
#include <string.h>     /* memset */

#include <zbar.h>
#include "svg.h"

#ifdef DEBUG_SCANNER
# define DEBUG_LEVEL (DEBUG_SCANNER)
#endif
#include "debug.h"

#ifndef ZBAR_FIXED
# define ZBAR_FIXED 5
#endif
#define ROUND (1 << (ZBAR_FIXED - 1))

/* FIXME add runtime config API for these */
#ifndef ZBAR_SCANNER_THRESH_MIN
# define ZBAR_SCANNER_THRESH_MIN  4
#endif

#ifndef ZBAR_SCANNER_THRESH_INIT_WEIGHT
# define ZBAR_SCANNER_THRESH_INIT_WEIGHT .44
#endif
#define THRESH_INIT ((unsigned)((ZBAR_SCANNER_THRESH_INIT_WEIGHT       \
                                 * (1 << (ZBAR_FIXED + 1)) + 1) / 2))

#ifndef ZBAR_SCANNER_THRESH_FADE
# define ZBAR_SCANNER_THRESH_FADE 8
#endif

#ifndef ZBAR_SCANNER_EWMA_WEIGHT
# define ZBAR_SCANNER_EWMA_WEIGHT .78
#endif
#define EWMA_WEIGHT ((unsigned)((ZBAR_SCANNER_EWMA_WEIGHT              \
                                 * (1 << (ZBAR_FIXED + 1)) + 1) / 2))

/* scanner state */
struct zbar_scanner_s {
    zbar_decoder_t *decoder; /* associated bar width decoder */
    unsigned y1_min_thresh; /* minimum threshold */

    unsigned x;             /* relative scan position of next sample */
    int y0[4];              /* short circular buffer of average intensities */

    int y1_sign;            /* slope at last crossing */
    unsigned y1_thresh;     /* current slope threshold */

    unsigned cur_edge;      /* interpolated position of tracking edge */
    unsigned last_edge;     /* interpolated position of last located edge */
    unsigned width;         /* last element width */
    int y1_1;
    unsigned width_coef;    /* 0x100000000/width */
    unsigned thresh_coef;
};

zbar_scanner_t *zbar_scanner_create (zbar_decoder_t *dcode)
{
    zbar_scanner_t *scn = zbar_malloc(sizeof(zbar_scanner_t));
    scn->decoder = dcode;
    scn->y1_min_thresh = ZBAR_SCANNER_THRESH_MIN;
    zbar_scanner_reset(scn);
    return(scn);
}

void zbar_scanner_destroy (zbar_scanner_t *scn)
{
    zbar_free(scn);
}

zbar_symbol_type_t zbar_scanner_reset (zbar_scanner_t *scn)
{
    memset(&scn->x, 0, sizeof(zbar_scanner_t) + (size_t)((char*)scn - (char*)&scn->x));
    scn->y1_thresh = scn->y1_min_thresh;
    if(scn->decoder)
        zbar_decoder_reset(scn->decoder);
    return(ZBAR_NONE);
}

unsigned zbar_scanner_get_width (const zbar_scanner_t *scn)
{
    return(scn->width);
}

unsigned zbar_scanner_get_edge (const zbar_scanner_t *scn,
                                unsigned offset,
                                int prec)
{
    unsigned edge = scn->last_edge - offset - (1 << ZBAR_FIXED) - ROUND;
    prec = ZBAR_FIXED - prec;
    if(prec > 0)
        return(edge >> prec);
    else if(!prec)
        return(edge);
    else
        return(edge << -prec);
}

zbar_color_t zbar_scanner_get_color (const zbar_scanner_t *scn)
{
    return((scn->y1_sign <= 0) ? ZBAR_SPACE : ZBAR_BAR);
}

static inline unsigned calc_thresh (zbar_scanner_t *scn)
{
    /* threshold 1st to improve noise rejection */
    unsigned thresh = scn->y1_thresh;
    if((thresh <= scn->y1_min_thresh) || !scn->width) {
        dprintf(1, " tmin=%d", scn->y1_min_thresh);
        return(scn->y1_min_thresh);
    }
    /* slowly return threshold to min */
    unsigned dx = (scn->x << ZBAR_FIXED) - scn->last_edge;
    unsigned long t = thresh * dx;
    t /= scn->width;
    t /= ZBAR_SCANNER_THRESH_FADE;
    dprintf(1, " thr=%d t=%ld x=%d last=%d.%d (%d)",
            thresh, t, scn->x, scn->last_edge >> ZBAR_FIXED,
            scn->last_edge & ((1 << ZBAR_FIXED) - 1), dx);
    if(thresh > t) {
        thresh -= t;
        if(thresh > scn->y1_min_thresh)
            return(thresh);
    }
    scn->y1_thresh = scn->y1_min_thresh;
    return(scn->y1_min_thresh);
}

static inline zbar_symbol_type_t process_edge (zbar_scanner_t *scn,
                                               int y1)
{
    if(!scn->y1_sign)
        scn->last_edge = scn->cur_edge = (1 << ZBAR_FIXED) + ROUND;
    else if(!scn->last_edge)
        scn->last_edge = scn->cur_edge;

    scn->width = scn->cur_edge - scn->last_edge;
    if (scn->width == 0)
    {
        scn->width_coef = 0x7f000000;
    }
    else
    {
        scn->width_coef = (unsigned int)(((unsigned long long)0x100000000)/scn->width);
    }
	 // printf("x= %d width = %d\n", scn->x, scn->width);
    dprintf(1, " sgn=%d cur=%d.%d w=%d (%s)\n",
            scn->y1_sign, scn->cur_edge >> ZBAR_FIXED,
            scn->cur_edge & ((1 << ZBAR_FIXED) - 1), scn->width,
            ((y1 > 0) ? "SPACE" : "BAR"));
    scn->last_edge = scn->cur_edge;

#if DEBUG_SVG > 1
    svg_path_moveto(SVG_ABS, scn->last_edge - (1 << ZBAR_FIXED) - ROUND, 0);
#endif

    /* pass to decoder */
    if(scn->decoder)
        return(zbar_decode_width(scn->decoder, scn->width));
    return(ZBAR_PARTIAL);
}

inline zbar_symbol_type_t zbar_scanner_flush (zbar_scanner_t *scn)
{
    if(!scn->y1_sign)
        return(ZBAR_NONE);

    unsigned x = (scn->x << ZBAR_FIXED) + ROUND;

    if(scn->cur_edge != x || scn->y1_sign > 0) {
        dprintf(1, "flush0:");
        zbar_symbol_type_t edge = process_edge(scn, -scn->y1_sign);
        scn->cur_edge = x;
        scn->y1_sign = -scn->y1_sign;
        return(edge);
    }

    scn->y1_sign = scn->width = 0;
		
    scn->width_coef = 0x7f000000;
    if(scn->decoder)
        return(zbar_decode_width(scn->decoder, 0));
    return(ZBAR_PARTIAL);
}

zbar_symbol_type_t zbar_scanner_new_scan (zbar_scanner_t *scn)
{
    zbar_symbol_type_t edge = ZBAR_NONE;
    while(scn->y1_sign) {
        zbar_symbol_type_t tmp = zbar_scanner_flush(scn);
        if(tmp < 0 || tmp > edge)
            edge = tmp;
    }

    /* reset scanner and associated decoder */
    memset(&scn->x, 0, sizeof(zbar_scanner_t) + (char*)scn - (char*)&scn->x);
    scn->y1_thresh = scn->y1_min_thresh;
    scn->width_coef = 0x7f000000;
    if(scn->decoder)
        zbar_decoder_new_scan(scn->decoder);
    return(edge);
}

/*
 * y0_2-y0_3  R6:abs(y1_1)  R7:abs(y1_2) R3:y1_0 R4:y1_1  R5:y1_2 R8: y0_1
 * Firstly: y1_0 = y1_1 = y1_2 =0; y0_1=y;    (R3 = R4 = R5 = 0; R8 = y, R2=y)
 * t0 = (y - y0_1)*EWMA_WEIGHT; y0_0 = (y0_0 + t0>>ZBAR_FIXED); y0_1= y0_0;  
 * if (abs(y1_1) < abs(y1_2) && y1_1*y1_2 >=0) y1_1 = y1_2;
 * if (y2_1> 0 && y2_1 * y2_2 >=0) continue;
 */

/*
 *     
 *             <<----------------------
 *
 *             3   2   3   2   3   2   3         2nd differentials     
 *            / \ / \ / \ / \ / \ / \ / \
 *           4   5   6   7   4   5   6   7       1st differentials
 *          / \ / \ / \ / \ / \ / \ / \ / \
 *             4   5   6   7   4   5             weighted moving average
 *
 */
/*
 *R0: image_addr       Source buffer of image
 *R1: y2_1-y2_2/scn    y2_1-y2_2/Pointer to scn
 *R2-R3:               y2_1/y2_2
 *R4-R7:               y1_0/y1_1/y1_2/y1_3
 *R8:                  ((x<<ZBAR_FIXED) - scn->last_edge)
 *R9:                  Thresh
 *R10:                 Constant
 *R11:                 X
 *R12:                 Step 
 *zbar_scanner_t
 *x                    #0x8
 *y1_thresh            #0x20
 *last_edge            #0x28        
 *width                #0x2c
 *y1_1                 #0x30
 *width_coef           #0x34
 *thresh_coef          #0x38
 */
unsigned int c_update_edge(zbar_scanner_t *scn, int y3_1, int y2_1, int x)
{
    //return scn->width_coef;
    
    zbar_symbol_type_t edge = ZBAR_NONE;
    int y1_1 = scn->y1_1;
	  
	  scn->x = x;
    /* check for 1st sign change */
    char y1_rev = (scn->y1_sign > 0) ? y1_1 < 0 : y1_1 > 0;
    if(y1_rev)
        /* intensity change reversal - finalize previous edge */
        edge = process_edge(scn, y1_1);

    if(y1_rev || (abs(scn->y1_sign) < abs(y1_1))) {
        scn->y1_sign = y1_1;

        /* adaptive thresholding */
        /* start at multiple of new min/max */
        scn->y1_thresh = (abs(y1_1) * THRESH_INIT + ROUND) >> ZBAR_FIXED;
        dprintf(1, "\tthr=%d", scn->y1_thresh);
        if(scn->y1_thresh < scn->y1_min_thresh)
            scn->y1_thresh = scn->y1_min_thresh;

        /* update current edge */
        scn->cur_edge = 1 << ZBAR_FIXED;
        if(!y3_1)
            scn->cur_edge >>= 1;
        else if(y2_1)
            /* interpolate zero crossing */
            scn->cur_edge -= ((y2_1 << ZBAR_FIXED) + 1) / y3_1;
       scn->cur_edge += x << ZBAR_FIXED;
				
       dprintf(1, "\n");
    }
		
		return scn->width_coef;
}

//int y1_1_array[1024];
__asm void asm_zbar_scan_y(unsigned char *p, zbar_scanner_t *scn, int step, int total_len)
{
        EXTERN c_update_edge
	//EXTERN y1_1_array
        PUSH    {R4-R12, LR}
	//			LDR     R4, =y1_1_array
//        VMOV    S13, R4				
        MOV     R12, R2
				VMOV    S1,  R1
				VMOV    S3,  R3
				LDRB    R5,  [R0]           //Get y0_1
				MOV     R6,  #0             //Get y1_1
				MOV     R7,  #0             //Get y1_2
				MOV     R3,  #0             //Get y2_2
				MOV     R11, #0             //Set x
				MOV     R10, #25
				VMOV    S12, R12
				LDR     R8, [R1, #0x28]     //Get last_edge from scn
				LDR     R9, [R1, #0x20]     //Get new thresh from scn
				LDR     R14,[R1, #0x34]     //Get weight_coef
        //EWMA_WEIGHT 25 #ZBAR_FIXED 5
loop_zbar_scan_y		
        //  R4/R5/R6/R7/R2/R3  : y0_0/y1_0/y1_1/y1_2/y2_1/y2_2
        LDRB   R2, [R0]             //Get pixel value
        SUB    R2, R2, R5
        MUL    R2, R10
        ADD    R4, R5, R2, ASR #5   //Get y0_0
        ASR    R5, R2, #5           //Get y1_0				    
//				VMOV   R2, S13
//        STR    R6, [R2], #4
//				VMOV   S13,R2        				
				MUL    R2, R3, R6           //if (y2_2*y1_1 >= 0 ) y1_1 =  y1_1 else y1_1 = y1_2
        CMP    R2, #0				
				RSBSEQ R2, R7, #0           //if (y1_1 == 0 && (0 - y1_2) > 0) y1_1 = y1_1
				MOVGT  R7, R6               //R7: fixed y1_1
        SUBS   R2, R5, R6           //Get y2_1
        SUB    R1, R2, R3           //Get y2_1-y2_2
				BEQ    calc_thresh1
	      MULS   R3, R2, R3           //Get y2_1*y2_2
        BGE    exit_calc_thresh1
calc_thresh1	
        MUL    R3, R7, R7
				CMP    R3, #16              //Compare with min_thresh
				BLT    exit_calc_thresh1
				CMP    R9, #4
        BLE    update_edge1		
				ADD    R3, R11, #1
				RSB    R3, R8, R3, LSL #ZBAR_FIXED
        MUL    R3, R9, R3
				SMMUL  R3, R3, R14          //t = dx*(thresh/width/ZBAR_SCANNER_THRESH_FADE); Set thresh-coef to zero if width == 0
        SUBS   R3, R9, R3, LSR #3   //thresh - t/ZBAR_SCANNER_THRESH_FADE;
				MOVLE  R9, #4 
				BLE    update_edge1
				//MOV    R9, R3             //Save new thresh, always descrease even thresh < min_thresh 
        CMP    R7, #0               //if (thresh > abs(y1_1)) exit_thresh else update_edge  
				SUBGE  R3, R3, R7  
        ADDLT  R3, R3, R7
        CMP    R3, #0				
				BGT    exit_calc_thresh1
update_edge1				
				VMOV   S4, R0
				VMOV   R0, S1               //Get scn
				VMOV   S6, R2
				//MOV    R2, R7             //Get y2_1
				ADD    R3, R11, #0          //Get x
				STR    R7, [R0, #0x30]      //Save fixed y1_1 to scn
				BL     c_update_edge
        MOV    R14,R0
				VMOV   R0, S1               //Get scn
				LDR    R8, [R0, #0x28]      //Get last_edge from scn
				LDR    R9, [R0, #0x20]      //Get new thresh from scn
				VMOV   R0, S4	
        VMOV   R2, S6
        VMOV   R12, S12				
exit_calc_thresh1				

        //  R7/R4/R5/R6/R3/R2  : y0_0/y1_0/y1_1/y1_2/y2_1/y2_2 
        LDRB   R3, [R0, R12]        //Get pixel value
        SUB    R3, R3, R4
        MUL    R3, R10
				ADD    R7, R4, R3, ASR #5   //Get y0_0
				ASR    R4, R3, #5           //Get y1_0
				MUL    R3, R5, R2           //if (y2_2*y1_1 >= 0 ) y1_1 =  y1_1 else y1_1 = y1_2       
				CMP    R3, #0
        RSBSEQ R2, R6, #0           //if (y1_1 == 0 && (0 - y1_2) > 0) y1_1 = y1_1
				MOVGT  R6, R5                
        SUBS   R3, R4, R5           //Get y2_1
        SUB    R1, R3, R2           //Get y2_1-y2_2
				BEQ    calc_thresh2
        MULS   R2, R3, R2           //Get y2_1*y2_2				
        BGE    exit_calc_thresh2
calc_thresh2	
        MUL    R2, R6, R6
				CMP    R2, #16               //Compare with min_thresh
				BLT    exit_calc_thresh2
				CMP    R9, #4
        BLE    update_edge2		
				ADD    R2, R11, #1
				RSB    R2, R8, R2, LSL #ZBAR_FIXED
        MUL    R2, R9, R2
				SMMUL  R2, R2, R14          //t = dx*(thresh/width/ZBAR_SCANNER_THRESH_FADE); Set thresh-coef to zero if width == 0
        SUBS   R2, R9, R2, LSR #3   //thresh - t/ZBAR_SCANNER_THRESH_FADE;
				MOVLE  R9, #4 
				BLE    update_edge2    
				//MOV    R9, R2               //Save new thresh, always descrease even thresh < min_thresh 
				CMP    R6, #0
				SUBGE  R2, R2, R6  
        ADDLT  R2, R2, R6 
        CMP    R2, #0				
				BGT    exit_calc_thresh2
update_edge2
        VMOV   S4, R0
				VMOV   R0, S1               //Get scn
				MOV    R2, R3               //Get y2_1
				VMOV   S7, R3
				ADD    R3, R11, #1          //Get x
				STR    R6, [R0, #0x30]      //Save y1_1 to scn
				BL     c_update_edge
        MOV    R14,R0
				VMOV   R0, S1               //Get scn
				LDR    R8, [R0, #0x28]      //Get last_edge from scn
				LDR    R9, [R0, #0x20]      //Get new thresh from scn
        VMOV   R0, S4	
        VMOV   R3, S7	
        VMOV   R12, S12		
exit_calc_thresh2

        //  R6/R7/R4/R5/R2/R3  : y0_0/y1_0/y1_1/y1_2/y2_1/y2_2 
        LDRB   R2, [R0, R12, LSL #1]//Get pixel value
				SUB    R2, R2, R7
        MUL    R2, R10
				ADD    R6, R7, R2, ASR #5   //Get y0_0
				ASR    R7, R2, #5           //Get y1_0
				MUL    R2, R4, R3           //if (y2_2*y1_1 >= 0 ) y1_1 =  y1_1 else y1_1 = y1_2             
				CMP    R2, #0
        RSBSEQ R2, R5, #0           //if (y1_1 == 0 && (0 - y1_2) > 0) y1_1 = y1_1
				MOVGT  R5, R4
        SUBS   R2, R7, R4           //Get y2_1
        SUB    R1, R2, R3           //Get y2_1-y2_2
				BEQ    calc_thresh3
				MULS   R3, R2, R3           //Get y2_1*y2_2		
        BGE    exit_calc_thresh3
calc_thresh3
        MUL    R3, R5, R5
				CMP    R3, #16              //Compare with min_thresh
				BLT    exit_calc_thresh3
				CMP    R9, #4
        BLE    update_edge3		
				ADD    R3, R11, #2
				RSB    R3, R8, R3, LSL #ZBAR_FIXED
				MUL    R3, R9, R3
				SMMUL  R3, R3, R14          //t = dx*(thresh/width/ZBAR_SCANNER_THRESH_FADE); Set thresh-coef to zero if width == 0
        SUBS   R3, R9, R3, LSR #3   //thresh - t/ZBAR_SCANNER_THRESH_FADE;
				MOVLE  R9, #4 
				BLE    update_edge3
				//MOV    R9, R3               //Save new thresh, always descrease even thresh < min_thresh 
				CMP    R5, #0               //if (thresh > abs(y1_1)) exit_thresh else update_edge  
				SUBGE  R3, R3, R5  
        ADDLT  R3, R3, R5
        CMP    R3, #0				
				BGT    exit_calc_thresh3
update_edge3				
				VMOV   S4, R0
				VMOV   R0, S1               //Get scn
				VMOV   S6, R2
				//MOV    R2, R7               //Get y2_1
				ADD    R3, R11, #2          //Get x
				STR    R5, [R0, #0x30]      //Save y1_1 to scn
				BL     c_update_edge
        MOV    R14,R0
				VMOV   R0, S1               //Get scn
				LDR    R8, [R0, #0x28]        //Get last_edge from scn
				LDR    R9, [R0, #0x20]        //Get new thresh from scn
				VMOV   R0, S4	
        VMOV   R2, S6				
        VMOV   R12, S12						
exit_calc_thresh3

        //  R5/R6/R7/R4/R3/R2  : y0_0/y1_0/y1_1/y1_2/y2_1/y2_2 
        ADD    R0, R0, R12, LSL #1
				LDRB   R3, [R0, R12]        //Get pixel value
				SUB    R3, R3, R6
        MUL    R3, R10
				ADD    R5, R6, R3, ASR #5   //Get y0_0
				ASR    R6, R3, #5           //Get y1_0
				MUL    R3, R7, R2           //if (y2_2*y1_1 >= 0 ) y1_1 =  y1_1 else y1_1 = y1_2      
				CMP    R3, #0           
        RSBSEQ R2, R4, #0           //if (y1_1 == 0 && (0 - y1_2) > 0) y1_1 = y1_1
				MOVGT  R4, R7
        SUBS   R3, R6, R7           //Get y2_1
        SUB    R1, R3, R2           //Get y2_1-y2_2
				BEQ    calc_thresh4
				MULS   R2, R3, R2           //Get y2_1*y2_2				
        BGE    exit_calc_thresh4
calc_thresh4			
        MUL    R2, R4, R4
				CMP    R2, #16               //Compare with min_thresh
				BLT    exit_calc_thresh4
        CMP    R9, #4
        BLE    update_edge4				
				ADD    R2, R11, #3
				RSB    R2, R8, R2, LSL #ZBAR_FIXED
				MUL    R2, R9, R2
				SMMUL  R2, R2, R14          //t = dx*(thresh/width/ZBAR_SCANNER_THRESH_FADE); Set thresh-coef to zero if width == 0
        SUBS   R2, R9, R2, LSR #3  //thresh - t/ZBAR_SCANNER_THRESH_FADE;
				MOVLE  R9, #4 
				BLE    update_edge4
				//MOV    R9, R2              //Save new thresh, always descrease even thresh < min_thresh 
				CMP    R4, #0               //if (thresh > abs(y1_1)) exit_thresh else update_edge  
				SUBGE  R2, R2, R4  
        ADDLT  R2, R2, R4
        CMP    R2, #0				
				BGT    exit_calc_thresh4
update_edge4				
				VMOV   S4, R0
				VMOV   R0, S1               //Get scn
				MOV    R2, R3               //Get y2_1
				VMOV   S7, R3
				ADD    R3, R11, #3          //Get x
				STR    R4, [R0, #0x30]      //Save y1_1 to scn
				BL     c_update_edge
        MOV    R14,R0
				VMOV   R0, S1               //Get scn
				LDR    R8, [R0, #0x28]      //Get last_edge from scn
				LDR    R9, [R0, #0x20]      //Get new thresh from scn
        VMOV   R0, S4	
        VMOV   R3, S7		
        VMOV   R12, S12						
exit_calc_thresh4

				ADD    R0, R0, R12, LSL #1
				ADD    R11, #4
				VMOV   R1,  S3              //Get total_len
				CMP    R11, R1
				BLT    loop_zbar_scan_y
				
				VMOV   R0,  S1               //Get scn
				STR    R11, [R0, #8]
				STR    R8,  [R0, #0x28]     //Get last_edge from scn
				STR    R9,  [R0, #0x20]     //Get new thresh from scn
        MOV    R0, R11
        POP    {R4-R12, LR}                 
        BX     LR
}

unsigned int process_pixels_count = 0;
unsigned int process_edge_count = 0;

zbar_symbol_type_t zbar_scan_y (zbar_scanner_t *scn,
                                int y)
{
    /* FIXME calc and clip to max y range... */
    /* retrieve short value history */
    register int x = scn->x;
    register int y0_1 = scn->y0[(x - 1) & 3];
    register int y0_0 = y0_1;
    process_pixels_count++;
    if(x) {
        /* update weighted moving average */
        y0_0 += ((int)((y - y0_1) * EWMA_WEIGHT)) >> ZBAR_FIXED;
        scn->y0[x & 3] = y0_0;
    }
    else
        y0_0 = y0_1 = scn->y0[0] = scn->y0[1] = scn->y0[2] = scn->y0[3] = y;
    register int y0_2 = scn->y0[(x - 2) & 3];
    register int y0_3 = scn->y0[(x - 3) & 3];
    /* 1st differential @ x-1 */
    register int y1_1 = y0_1 - y0_2;
    {
        register int y1_2 = y0_2 - y0_3;
        if((abs(y1_1) < abs(y1_2)) &&
           ((y1_1 >= 0) == (y1_2 >= 0)))
            y1_1 = y1_2;
    }

    /* 2nd differentials @ x-1 & x-2 */
    register int y2_1 = y0_0 - (y0_1 * 2) + y0_2;
    register int y2_2 = y0_1 - (y0_2 * 2) + y0_3;

    dprintf(1, "scan: x=%d y=%d y0=%d y1=%d y2=%d",
            x, y, y0_1, y1_1, y2_1);

    zbar_symbol_type_t edge = ZBAR_NONE;
    /* 2nd zero-crossing is 1st local min/max - could be edge */
    if((!y2_1 ||
        ((y2_1 > 0) ? y2_2 < 0 : y2_2 > 0)) &&
       (calc_thresh(scn) <= abs(y1_1)))
    {
        process_edge_count++;
        /* check for 1st sign change */
        char y1_rev = (scn->y1_sign > 0) ? y1_1 < 0 : y1_1 > 0;
        if(y1_rev)
            /* intensity change reversal - finalize previous edge */
            edge = process_edge(scn, y1_1);

        if(y1_rev || (abs(scn->y1_sign) < abs(y1_1))) {
            scn->y1_sign = y1_1;

            /* adaptive thresholding */
            /* start at multiple of new min/max */
            scn->y1_thresh = (abs(y1_1) * THRESH_INIT + ROUND) >> ZBAR_FIXED;
            dprintf(1, "\tthr=%d", scn->y1_thresh);
            if(scn->y1_thresh < scn->y1_min_thresh)
                scn->y1_thresh = scn->y1_min_thresh;

            /* update current edge */
            int d = y2_1 - y2_2;
            scn->cur_edge = 1 << ZBAR_FIXED;
            if(!d)
                scn->cur_edge >>= 1;
            else if(y2_1)
                /* interpolate zero crossing */
                scn->cur_edge -= ((y2_1 << ZBAR_FIXED) + 1) / d;
            scn->cur_edge += x << ZBAR_FIXED;
            dprintf(1, "\n");
        }
    }
    else
        dprintf(1, "\n");
    /* FIXME add fall-thru pass to decoder after heuristic "idle" period
       (eg, 6-8 * last width) */
    scn->x = x + 1;
    return(edge);
}

/* undocumented API for drawing cutesy debug graphics */
void zbar_scanner_get_state (const zbar_scanner_t *scn,
                             unsigned *x,
                             unsigned *cur_edge,
                             unsigned *last_edge,
                             int *y0,
                             int *y1,
                             int *y2,
                             int *y1_thresh)
{
    register int y0_0 = scn->y0[(scn->x - 1) & 3];
    register int y0_1 = scn->y0[(scn->x - 2) & 3];
    register int y0_2 = scn->y0[(scn->x - 3) & 3];
    if(x) *x = scn->x - 1;
    if(cur_edge) *cur_edge = scn->cur_edge;
    if(last_edge) *last_edge = scn->last_edge;
    if(y0) *y0 = y0_1;
    if(y1) *y1 = y0_1 - y0_2;
    if(y2) *y2 = y0_0 - (y0_1 * 2) + y0_2;
    /* NB not quite accurate (uses updated x) */
    zbar_scanner_t *mut_scn = (zbar_scanner_t*)scn;
    if(y1_thresh) *y1_thresh = calc_thresh(mut_scn);
    dprintf(1, "\n");
}
