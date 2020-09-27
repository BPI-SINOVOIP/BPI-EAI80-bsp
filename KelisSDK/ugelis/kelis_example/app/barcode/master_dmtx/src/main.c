/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2008, 2009 Mike Laughton. All rights reserved.
 * Copyright 2010-2016 Vadim A. Misbakh-Soloviov. All rights reserved.
 * Copyright 2016 Tim Zaman. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact:
 * Vadim A. Misbakh-Soloviov <dmtx@mva.name>
 * Mike Laughton <mike@dragonflylogic.com>
 *
 * \file simple_test.c
 */
 
#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <gm_irq.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <dmtx.h>
#include "data.h"

//#define FPINTF fprintf
#define FPRINTF(...) do{}while(0)


void main(void)
{
    const size_t width = 640, height = 480, bytesPerPixel = 1;
    unsigned char  *pxl = AnsiChar;
    
    DmtxImage      *img;
    DmtxDecode     *dec;
    DmtxRegion     *reg;
    DmtxMessage    *msg;
    
    uint32_t time_start, time_end;
    
    printk("start\n");
	time_start = k_uptime_get_32();

    img = dmtxImageCreate(pxl, width, height, DmtxPack8bppK);
    assert(img != NULL);

    dec = dmtxDecodeCreate(img, 1);
    assert(dec != NULL);

    reg = dmtxRegionFindNext(dec, NULL);
    if(reg != NULL) {
      msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);

      FPRINTF(stdout, "msg->arraySize :  \"%zd\"\n", msg->arraySize );
      FPRINTF(stdout, "msg->codeSize  :  \"%zd\"\n", msg->codeSize  );
      FPRINTF(stdout, "msg->outputSize:  \"%zd\"\n", msg->outputSize);
      int oned = sqrt(msg->arraySize);
      for (int i=0; i<msg->arraySize; i++){
         FPRINTF(stdout, " %c.", msg->array[i]);
         if (i%oned==oned-1){
            FPRINTF(stdout, "\n");
         }
      }
      FPRINTF(stdout, "\n\n");
      for (int j=0; j<msg->codeSize; j++){
         FPRINTF(stdout, " %c.", msg->code[j]);
      }
      FPRINTF(stdout, "\n\n");
      for (int k=0; k<msg->outputSize; k++){
         FPRINTF(stdout, " %c.", msg->output[k]);
      }
      FPRINTF(stdout, "\n\n");

      if(msg != NULL) {
         fputs("output: \"", stdout);
         fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
         fputs("\"\n", stdout);
         dmtxMessageDestroy(&msg);
      }
      dmtxRegionDestroy(&reg);
    }

    dmtxDecodeDestroy(&dec);
    dmtxImageDestroy(&img);

    FPRINTF(stdout, "%d\n", getSizeIdxFromSymbolDimension(12, 12));

	time_end = k_uptime_get_32();
	printk("end, cost %d\n", time_end-time_start);
   
    return 0;
}


