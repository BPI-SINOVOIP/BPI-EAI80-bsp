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
    frame->frameLen         = 0x03 + objdata->num * 6;
    frame->cmd              = 0x1B;
    frame->frameInfo[0]     = objdata->num;
    data                    += frame->frameLen;
    data                    += frame->cmd;
    data                    += frame->frameInfo[0];
    if (objdata->num != 0)
    {
        for (int i = 0; i < objdata->num; i++)
        {
            frame->frameInfo[1 + i] = objdata->obj[i].off_l;
            frame->frameInfo[2 + i] = objdata->obj[i].off_t;
            frame->frameInfo[3 + i] = objdata->obj[i].width;
            frame->frameInfo[4 + i] = objdata->obj[i].height;
            frame->frameInfo[5 + i] = objdata->obj[i].off_z;
            frame->frameInfo[6 + i] = objdata->obj[i].angle;
            data                    += frame->frameInfo[1 + i];
            data                    += frame->frameInfo[2 + i];
            data                    += frame->frameInfo[3 + i];
            data                    += frame->frameInfo[4 + i];
            data                    += frame->frameInfo[5 + i];
            data                    += frame->frameInfo[6 + i];
        }
    }

    frame->check            = (data & 0xff);
}