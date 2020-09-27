#include <string.h>
#include "encode.h"
//#include "ff.h"
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

#if 0
#define KUART_Output(a) \
    do \
    {  \
        printf("%s\n", a); \
    }while(0)
#else
#define KUART_Output(...)
#endif

uint16_t *P;

uint8_t *P_;
uint32_t IMG_ADDR;


void face_extract(int *res_, uint32_t addr)
{
    char str[10];
    unsigned int t_last, t_now, delta;
    //  KUART_Output("ENTER JPEG_ENCODE");

    //  KUART_Init();

    IMG_ADDR = addr;
    P_ = (uint8_t *)((uint32_t)IMG_ADDR);

    unsigned int x, y;

    P = (uint16_t *)((uint32_t)IMG_ADDR); //SRAM adrr

    int image_height = CAM_CROP_W;
    int image_width = CAM_CROP_H;
    float r, g, b;
#ifdef RGB565
    while (next_line < image_height)
    {
        for (i = 0; i < image_width; i++)
        {
            color = *P;


            r = (float)((u8)((color & 0xf800) >> 8));

            g = (float)((u8)((color & 0x07e0) >> 3));

            b = (float)((u8)((color & 0x001f) << 3));

            u8 temp = 0.29900f * r + 0.58700f * g + 0.11400f * b + 0.5;

            *P_ = temp;

            P++;
            P_++;

        }
        next_line++;
    }
#else
    //  KUART_Output("RGB888");
    unsigned char *PP = (unsigned  char *)((uint32_t)IMG_ADDR); //SRAM adrr
    for (y = 0; y < image_height; y++)
    {
        for (x = 0; x < image_width; x++)
        {
            b = *PP++;
            g = *PP++;
            r = *PP++;
            u8 temp = 0.29900f * r + 0.58700f * g + 0.11400f * b + 0.5;
            *P_ = temp;
            P_++;
        }
    }
#endif
    FaceDetected result[25];
    uint32 nFace = 0;
    P_ = (uint8_t *)((uint32_t)IMG_ADDR);
    int Width = CAM_CROP_W;
    int Height = CAM_CROP_H;
    fd_init(Width, Height);

    fd_set_param("alertFactor", 4);
    fd_set_param("speedFactor", 1.2);

    fd_set_param("fov", 49.0);
    fd_set_param("disWidthProduct", 58.0);
    //      MeasureTimeStart();
    t_last = k_cycle_get_32();
    nFace = 0;
    fd_detect_face(P_, Width, Height, result, &nFace);

    t_now = k_cycle_get_32();
    delta = t_now - t_last;

    //      KUART_Output("TIME_USED: ");
    sprintf(str, "%f", delta * 1.0f / sys_clock_hw_cycles_per_sec);
    //      KUART_Output(str);

    int k = 0;
    if (nFace >= 1)
    {
        nFace = 1;
    }
    sprintf(str, "%u", nFace);
    //      KUART_Output("detected face: ");
    //      KUART_Output(str);

    int temp_ = 0;
    for (k = 0; k < nFace; k++)
    {

        sprintf(str, "%d", result[k].x);
        res_[temp_++] = result[k].x;

        KUART_Output("result x: ");
        KUART_Output(str);

        sprintf(str, "%d", result[k].y);
        res_[temp_++] = result[k].y;

        KUART_Output("result y: ");
        KUART_Output(str);

        sprintf(str, "%d", result[k].width);

        res_[temp_++] = result[k].width;
        KUART_Output("result width: ");
        KUART_Output(str);

        sprintf(str, "%d", result[k].height);

        res_[temp_++] = result[k].height;
        KUART_Output("result height: ");
        KUART_Output(str);
        res_[temp_] = 1000;

    }
    fd_free();
}
