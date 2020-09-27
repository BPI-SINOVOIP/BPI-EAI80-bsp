/**
 * libdmtx - Data Matrix Encoding/Decoding Library
 * Copyright 2007, 2008, 2009 Mike Laughton. All rights reserved.
 *
 * See LICENSE file in the main project directory for full
 * terms of use and distribution.
 *
 * Contact: Mike Laughton <mike@dragonflylogic.com>
 *
 * \file rotate_test.c
 */

#include "rotate_test.h"

#define MIN(x,y) ((x < y) ? x : y)
#define MAX(x,y) ((x > y) ? x : y)

GLfloat view_rotx = 0.0, view_roty = 0.0, view_rotz = 0.0;
GLfloat angle = 0.0;

GLuint barcodeTexture;
GLint barcodeList;

DmtxImage *gImage = NULL;
unsigned char *capturePxl = NULL;
unsigned char *texturePxl = NULL;
unsigned char *passOnePxl = NULL;
unsigned char *passTwoPxl = NULL;

char *gFilename[] = { "test_image18.png"
                    , "test_image16.png"
                    , "test_image17.png"
                    , "test_image01.png"
                    , "test_image05.png"
                    , "test_image06.png"
                    , "test_image07.png"
                    , "test_image12.png"
                    , "test_image13.png"
                    , "test_image08.png"
                    , "test_image09.png"
                    , "test_image10.png"
                    , "test_image04.png"
                    , "test_image11.png"
                    , "test_image02.png"
                    , "test_image03.png"
                    , "test_image14.png"
                    , "test_image15.png" };
int gFileIdx = 0;
int gFileCount = 18;

/**
 *
 *
 */
int main(int argc, char *argv[])
{
   int             i;
   int             count;
   int             done;
   int             width, height;
   SDL_Event       event;
   SDL_Surface     *screen;
   unsigned char   outputString[1024];
   DmtxDecode      *dec;
   DmtxRegion      *reg;
   DmtxMessage     *msg;
   DmtxTime        timeout;

   /* Initialize display window */
   screen = initDisplay();

   /* Load input image to DmtxImage */
   texturePxl = loadTextureImage(&width, &height);
   assert(texturePxl != NULL);

   capturePxl = (unsigned char *)malloc(width * height * 3);
   assert(capturePxl != NULL);

   passOnePxl = (unsigned char *)malloc(width * height * 3);
   assert(passOnePxl != NULL);

   passTwoPxl = (unsigned char *)malloc(width * height * 3);
   assert(passTwoPxl != NULL);

   done = 0;
   while(!done) {

      SDL_Delay(50);

      while(SDL_PollEvent(&event))
         done = HandleEvent(&event, screen);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      DrawGeneratedImage(screen);

      memset(passOnePxl, 0x00, width * height * 3);
      memset(passTwoPxl, 0x00, width * height * 3);

      /* Capture screenshot of generated image */
      glReadPixels(2, 324, width, height, GL_RGB, GL_UNSIGNED_BYTE, capturePxl);
      gImage = dmtxImageCreate(capturePxl, width, height, DmtxPack24bppRGB);
      assert(gImage != NULL);

      /* Pixels from glReadPixels are Y-flipped according to libdmtx */
      dmtxImageSetProp(gImage, DmtxPropImageFlip, DmtxFlipY);

      /* Start fresh scan */
      dec = dmtxDecodeCreate(gImage, 1);
      assert(dec != NULL);

      for(;;) {
         timeout = dmtxTimeAdd(dmtxTimeNow(), 500);

         reg = dmtxRegionFindNext(dec, &timeout);
         if(reg != NULL) {
            msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
            if(msg != NULL) {
               fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
               fputc('\n', stdout);
               dmtxMessageDestroy(&msg);
            }
            dmtxRegionDestroy(&reg);
         }
         break;
      }

      dmtxDecodeDestroy(&dec);
      dmtxImageDestroy(&gImage);

      DrawBorders(screen);
/*    DrawPane2(screen, passOnePxl); */
/*    DrawPane4(screen, passTwoPxl); */

      SDL_GL_SwapBuffers();
   }

   free(passTwoPxl);
   free(passOnePxl);
   free(capturePxl);
   free(texturePxl);

   exit(0);
}
