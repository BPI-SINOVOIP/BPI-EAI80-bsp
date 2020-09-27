#include <ugelis.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "face_main.h"
#include "protocol.h"

extern FaceAIData *faceai_data;
extern faceinfo_t faceout;

static void show_person()
{
    int i = 0;
    FaceAIData *fad = faceai_data;
    PRINTK("Total imgs cnt %d\n", fad->total_imgs_cnt);
    PRINTK("Total persons cnt %d\n", fad->person_cnt);
    PRINTK("Current base:\n");
    for (i = 0; i < fad->person_cnt; i++)
    {
        PRINTK("[%d]\t%s(%d)\n", i, fad->person_name[i], fad->person_img_cnt[i]);
    }
}

void set_faceinfo(Frame_T *frame, faceinfo_t *facedata)
{
    if ((NULL == frame) || (facedata == NULL))
    {
        return;
    }

    frame->frameHeader1     = FRAME_HEADER1;
    frame->frameHeader2     = FRAME_HEADER2;
    frame->destAddr         = BROADCAST_ADDR;
    frame->srcAddr          = CAMERAL_BOARD_ADDR;
    frame->frameLen         = 0x08;
    frame->cmd              = 0x2A;
    frame->frameInfo[0]     = facedata->num;
    if (facedata->num != 0)
    {
        frame->frameInfo[1] = (facedata->faceid) << 2;
        frame->frameInfo[2] = facedata->off_l;
        frame->frameInfo[3] = facedata->off_t;
        frame->frameInfo[4] = facedata->width;
        frame->frameInfo[5] = facedata->height;
    }
    uint32_t        data    = 0;
    data                    += frame->frameLen;
    data                    += frame->cmd;
    data                    += frame->frameInfo[0];
    data                    += frame->frameInfo[1];
    data                    += frame->frameInfo[2];
    data                    += frame->frameInfo[3];
    data                    += frame->frameInfo[4];
    data                    += frame->frameInfo[5];
    frame->check            = (data & 0xff);
}

void faceai_capstart()
{
    int i = 0;
    FaceAIData *fad             = faceai_data;
    fad->skip_camera_stream     = 0;
    fad->capture_started        = 1;
    fad->capture_backup.index   = 0;

    for (i = 0; i < CAPTURE_BACKUP_CNT; i++)
    {
        fad->capture_backup.ignore[i] = 0;
    }
}

void faceai_capstop(uint32_t id)
{
    fad->capture_started        = 0;
    fad->skip_camera_stream     = 0;
    if (id < 1)
    {
        PRINTK("Usage: import [label] [name]\n");

        return ;
    }
    if (!fad->person_name[id])
    {
        fad->person_name[id] = malloc_ext(0, 16);
        fad->person_cnt++;
    }
    PRINTK("importing\n");
    strcpy(fad->person_name[id], id);
    PRINTK("imported\n");
}

void faceai_delbase(int number, uint8_t info2, uint8_t  info3)
{
    for (int i = 0; i < 8; i++)
    {
        if ((1 << i) & info2)
        {
            if (i < fad->person_cnt)
            {
                delete_record(fad, i);
                strcpy(fad->person_name[i], "deleted");
            }
        }
    }

    for (int i = 0; i < 8; i++)
    {
        if ((1 << i) & info3)
        {
            if ((i + 8) < fad->person_cnt)
            {
                delete_record(fad, (i + 8));
                strcpy(fad->person_name[i + 8], "deleted");
            }
        }
    }
}