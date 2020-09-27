#include <ugelis.h>

#include "protocol.h"
#include "obj_info.h"

void set_personinfo(Frame_T *frame, obj_show_t *objdata)
{
    uint32_t        data    = 0;

    if ((NULL == frame) || (objdata == NULL))
    {
        return;
    }

    frame->frameHeader1     = FRAME_HEADER1;
    frame->frameHeader2     = FRAME_HEADER2;
    frame->destAddr         = BROADCAST_ADDR;
    frame->srcAddr          = CAMERAL_BOARD_ADDR;
    frame->frameLen         = 0x03 + objdata->num * 10;
    frame->cmd              = FRAME_PERSON_INFO_CMD;
    frame->frameInfo[0]     = objdata->num;
    data                    += frame->frameLen;
    data                    += frame->cmd;
    data                    += frame->frameInfo[0];
    if (objdata->num != 0)
    {
        for (int i = 0; i < objdata->num; i++)
        {
            frame->frameInfo[1 + i * 10] = objdata->obj[i].off_l & 0xff;
            frame->frameInfo[2 + i * 10] = (objdata->obj[i].off_l & 0xff00) >> 8;
            frame->frameInfo[3 + i * 10] = objdata->obj[i].off_t & 0xff;
            frame->frameInfo[4 + i * 10] = (objdata->obj[i].off_t & 0xff00) >> 8;
            frame->frameInfo[5 + i * 10] = objdata->obj[i].width & 0xff;
            frame->frameInfo[6 + i * 10] = (objdata->obj[i].width & 0xff00) >> 8;
            frame->frameInfo[7 + i * 10] = objdata->obj[i].height & 0xff;
            frame->frameInfo[8 + i * 10] = (objdata->obj[i].height & 0xff) >> 8;
            frame->frameInfo[9 + i * 10] = objdata->obj[i].off_z;
            frame->frameInfo[10 + i * 10] = objdata->obj[i].angle;
            data                    += frame->frameInfo[1 + i * 10];
            data                    += frame->frameInfo[2 + i * 10];
            data                    += frame->frameInfo[3 + i * 10];
            data                    += frame->frameInfo[4 + i * 10];
            data                    += frame->frameInfo[5 + i * 10];
            data                    += frame->frameInfo[6 + i * 10];
            data                    += frame->frameInfo[7 + i * 10];
            data                    += frame->frameInfo[8 + i * 10];
            data                    += frame->frameInfo[9 + i * 10];
            data                    += frame->frameInfo[10 + i * 10];
        }
    }

    frame->check            = (data & 0xff);
}