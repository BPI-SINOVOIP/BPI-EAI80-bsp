#include <string.h>
#include "encode.h"
#include "ff.h"
#include "type.h"
#include <time.h>
//#include <MeasureTime.h>
#include <stdio.h>
//#include "kuart.h"
#include "faceDetection.h"
#include <kernel.h>

//#include "stm324xg_eval_lcd.h"

#define CAM_CROP_W 160
#define CAM_CROP_H 120

#define KUART_Output(a) \
    do \
    {  \
        printf(a); \
        printf("\n"); \
    }while(0)

uint8_t *pGray;
uint8_t *pGrayShrinkDest;


uint8_t matAt(uint8_t *ptr, int cols, int x, int y);
uint8_t *matAtAddr(uint8_t *ptr, int cols, int x, int y);
uint8_t matAt(uint8_t *ptr, int cols, int x, int y)
{
    return *(ptr + y + cols * x);
}
uint8_t *matAtAddr(uint8_t *ptr, int cols, int x, int y)
{
    return (ptr + y + cols * x);
}

static int isInitialized = 0;
void Face_Dectection(int *res_, uint32_t addr)
{
    unsigned int t_last, t_now, delta;

    char str[10];
    //  KUART_Output("ENTER JPEG_ENCODE");


    pGray = (uint8_t *)((uint32_t)addr);

    u16 i, j, color;


    int next_line = 0;
    int image_width  = CAM_CROP_W;
    int image_height = CAM_CROP_H;
    float r, g, b;

    int x = 0,  y = 0;

    //  KUART_Output("RGB888");
    unsigned char *PP = (unsigned  char *)((uint32_t)addr); //SRAM adrr
    for (y = 0; y < image_height; y++)
    {
        for (x = 0; x < image_width; x++)
        {
            b = *PP++;
            g = *PP++;
            r = *PP++;
#ifdef RGB2Y
            u8 temp = 0.29900f * r + 0.58700f * g + 0.11400f * b + 0.5;
#else
            u8 temp = r;
#endif
            *pGray = temp;
            pGray++;
        }
    }
    pGray = (uint8_t *)((uint32_t)addr);
    pGrayShrinkDest = (uint8_t *)((uint32_t)addr);

    int final_img_height, final_img_width;

    /**
      * 调节这2个参数来进行图片压缩，1.0表示不压缩，0.5表示压缩成长宽的一半，2.0表示将图片放大2倍，以此类推
     */
    float Sx = 1.0;
    float Sy = 1.0;

    final_img_width = image_width * Sy;
    final_img_height = image_height * Sx;

#if 0
    int count = 0;
    int x1, x2, y1, y2;
    float temp1, temp2;

    for (y = 0; y < final_img_height; y++)
    {
        for (x = 0; x < final_img_width; x++)
        {
            x1 = (int)(x / Sx);
            x2 = x1 + 1;
            y1 = (int)(y / Sy);
            y2 = y1 + 1;

            //ÅÐ¶Ï±ß½ç
            if (y2 >= image_height || x2 >= image_width)
            {
                *(matAtAddr(pGrayShrinkDest, final_img_width, y, x)) = matAt(pGray, image_width, y1, x1);
                continue;
            }

            temp1 = (x2 - x / Sx) * matAt(pGrayShrinkDest, image_width, y1, x1) + (x / Sx - x1) * matAt(pGrayShrinkDest, image_width, y1, x2);
            temp2 = (x2 - x / Sx) * matAt(pGrayShrinkDest, image_width, y2, x1) + (x / Sx - x1) * matAt(pGrayShrinkDest, image_width, y2, x2);
            *(matAtAddr(pGrayShrinkDest, final_img_width, y, x)) = (uint8_t)((y2 - y / Sy) * temp1 + (uint8_t)(y / Sy - y1) * temp2);

            // sprintf(str, "count: %d, temp1: %f, temp2: %f, value: %d ", count, temp1, temp2, matAt(pGrayShrinkDest, final_img_width, y, x));
            // KUART_Output(str);
        }
    }
    //===============================================================================================================================
#endif
    FaceDetected result[25];
    uint32 nFace = 0;
    if (!isInitialized)
    {
        fd_init(final_img_width, final_img_height);
        fd_set_param("alertFactor", 1);
        fd_set_param("speedFactor", 1.2);
        fd_set_param("fov", 49.0);
        fd_set_param("disWidthProduct", 58.0);

        isInitialized = 1;
    }
    t_last = k_cycle_get_32();
    fd_detect_face(pGrayShrinkDest, final_img_width, final_img_height, result, &nFace);

    t_now = k_cycle_get_32();
    delta = t_now - t_last;

    KUART_Output("TIME_USED: ");
    sprintf(str, "%f", delta * 1.0f / sys_clock_hw_cycles_per_sec);
    KUART_Output(str);

    int k = 0;
    if (nFace >= 1)
    {
        nFace = 1;
    }
    sprintf(str, "%u", nFace);
    KUART_Output("detected face: ");
    KUART_Output(str);

    int temp_ = 0;
    for (k = 0; k < nFace; k++)
    {

        sprintf(str, "%d", result[k].x);
        res_[temp_++] = 1.0f * result[k].x / Sx;

        KUART_Output("result x: ");
        KUART_Output(str);

        sprintf(str, "%d", result[k].y);
        res_[temp_++] = 1.0f * result[k].y / Sy;

        KUART_Output("result y: ");
        KUART_Output(str);

        sprintf(str, "%d", result[k].width);

        res_[temp_++] = 1.0f * result[k].width / Sx;
        KUART_Output("result width: ");
        KUART_Output(str);

        sprintf(str, "%d", result[k].height);

        res_[temp_++] = 1.0f * result[k].height / Sy;
        KUART_Output("result height: ");
        KUART_Output(str);
        res_[temp_] = 1000;

    }
    fd_free();
}
