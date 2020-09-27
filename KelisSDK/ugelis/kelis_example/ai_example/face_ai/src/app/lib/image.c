/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          image.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Image preprocessor interface
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

void image_zoom(unsigned char *pSrcImg, unsigned char *pDstImg, int nSrcWidth, int nSrcHeight, int dst_w, int dst_h)
{
    float fRateW = 0;
    float fRateH = 0;

    int i = 0;
    int j = 0;

    float fX, fY;


    fRateW = dst_w * 1.0f / nSrcWidth;
    fRateH = dst_h * 1.0f / nSrcHeight;

    int iStepSrcImg = nSrcWidth;
    int iStepDstImg = dst_w;

    int iX, iY;

    unsigned char bUpLeft, bUpRight, bDownLeft, bDownRight;
    unsigned char gUpLeft, gUpRight, gDownLeft, gDownRight;
    unsigned char rUpLeft, rUpRight, rDownLeft, rDownRight;

    unsigned char b, g, r;


    /*  int w=nHeight * fRateH;
      int h=nWidth * fRateW;*/
    for (i = 0; i < dst_h ; i++)
    {
        for (j = 0; j < dst_w ; j++)
        {
            fX = ((float)j) / fRateW;
            fY = ((float)i) / fRateH;

            iX = (int)fX;
            iY = (int)fY;

            bUpLeft  = pSrcImg[iY * iStepSrcImg * 3 + iX * 3 + 0];
            bUpRight = pSrcImg[iY * iStepSrcImg * 3 + (iX + 1) * 3 + 0];

            bDownLeft  = pSrcImg[(iY + 1) * iStepSrcImg * 3 + iX * 3 + 0];
            bDownRight = pSrcImg[(iY + 1) * iStepSrcImg * 3 + (iX + 1) * 3 + 0];

            gUpLeft  = pSrcImg[iY * iStepSrcImg * 3 + iX * 3 + 1];
            gUpRight = pSrcImg[iY * iStepSrcImg * 3 + (iX + 1) * 3 + 1];

            gDownLeft  = pSrcImg[(iY + 1) * iStepSrcImg * 3 + iX * 3 + 1];
            gDownRight = pSrcImg[(iY + 1) * iStepSrcImg * 3 + (iX + 1) * 3 + 1];

            rUpLeft  = pSrcImg[iY * iStepSrcImg * 3 + iX * 3 + 2];
            rUpRight = pSrcImg[iY * iStepSrcImg * 3 + (iX + 1) * 3 + 2];

            rDownLeft  = pSrcImg[(iY + 1) * iStepSrcImg * 3 + iX * 3 + 2];
            rDownRight = pSrcImg[(iY + 1) * iStepSrcImg * 3 + (iX + 1) * 3 + 2];

            b = bUpLeft * (iX + 1 - fX) * (iY + 1 - fY) + bUpRight * (fX - iX) * (iY + 1 - fY)
                + bDownLeft * (iX + 1 - fX) * (fY - iY) + bDownRight * (fX - iX) * (fY - iY);

            g = gUpLeft * (iX + 1 - fX) * (iY + 1 - fY) + gUpRight * (fX - iX) * (iY + 1 - fY)
                + gDownLeft * (iX + 1 - fX) * (fY - iY) + gDownRight * (fX - iX) * (fY - iY);

            r = rUpLeft * (iX + 1 - fX) * (iY + 1 - fY) + rUpRight * (fX - iX) * (iY + 1 - fY)
                + rDownLeft * (iX + 1 - fX) * (fY - iY) + rDownRight * (fX - iX) * (fY - iY);

            if (iY >= 0 && iY <= nSrcHeight * 2 && iX >= 0 && iX <= nSrcWidth * 2)
            {
                pDstImg[i * iStepDstImg * 3 + j * 3 + 0] = b;        //B
                pDstImg[i * iStepDstImg * 3 + j * 3 + 1] = g;        //G
                pDstImg[i * iStepDstImg * 3 + j * 3 + 2] = r;        //R
            }
        }
    }
}

void image_crop(unsigned char *src, unsigned char *dst_addr, int x, int y, int w, int h, unsigned int src_w)
{
    unsigned char *dst = &(dst_addr[0]);
    unsigned int crop_x_start   = x;
    unsigned int crop_y_start   = y;
    unsigned int crop_x_end     = crop_x_start + w;
    unsigned int crop_y_end     = crop_y_start + h;

    int c = 0, l = 0;
    unsigned char *src_rgb = 0;
    for (l = crop_y_start; l < crop_y_end; l++)
    {
        for (c = crop_x_start; c < crop_x_end; c++)
        {
            src_rgb = src + l * src_w * 3 + c * 3;
            *dst++ = *(src_rgb);//b
            *dst++ = *(src_rgb + 1);//g
            *dst++ = *(src_rgb + 2);//r
        }
    }
}

void image_split(unsigned char *src, unsigned char *dst, int w, int h)
{
    unsigned char *dst_b = &(dst[0]);
    unsigned char *dst_g = &(dst[w * h]);
    unsigned char *dst_r = &(dst[w * h * 2]);

    int c = 0, l = 0, i = 0;
    unsigned char *src_rgb = 0;
    for (l = 0; l < h; l++)
    {
        for (c = 0; c < w; c++)
        {
            //rgb ===> bbggrr(H---L)
            src_rgb = src + (l * w + c) * 3;
            *(dst_r + i) = *(src_rgb);//b
            *(dst_g + i) = *(src_rgb + 1);//g
            *(dst_b + i) = *(src_rgb + 2);//r
            i++;
        }
    }
}
