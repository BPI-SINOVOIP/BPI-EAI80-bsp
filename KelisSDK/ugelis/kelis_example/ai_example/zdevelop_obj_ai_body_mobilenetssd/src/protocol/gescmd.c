#include <ugelis.h>

#include "protocol.h"
#include "ai_common.h"

void set_gestureinfo(Frame_T *frame, objshow_t *objdata)
{
    uint32_t        data    = 0;
    uint32_t        index = 0;

    if ((NULL == frame) || (objdata == NULL))
    {
        return;
    }

    frame->frameHeader1     = FRAME_HEADER1;
    frame->frameHeader2     = FRAME_HEADER2;
    frame->destAddr         = BROADCAST_ADDR;
    frame->srcAddr          = CAMERAL_BOARD_ADDR;
    frame->frameLen         = 0x03 + objdata->num * 5;
    frame->cmd              = 0x1E;
    frame->frameInfo[0]     = objdata->num;
    data                    += frame->frameLen;
    data                    += frame->cmd;
    data                    += frame->frameInfo[0];
    index = 1;
    if (objdata->num != 0)
    {
        for (int i = 0; i < objdata->num; i++)
        {
            frame->frameInfo[index++] = objdata->obj[i].objid;
            frame->frameInfo[index++] = objdata->obj[i].off_l;
            frame->frameInfo[index++] = objdata->obj[i].off_t;
            frame->frameInfo[index++] = objdata->obj[i].width;
            frame->frameInfo[index++] = objdata->obj[i].height;
            data                    += objdata->obj[i].objid;
            data                    += objdata->obj[i].off_l;
            data                    += objdata->obj[i].off_t;
            data                    += objdata->obj[i].width;
            data                    += objdata->obj[i].height;
        }
    }

    frame->check            = (data & 0xff);
}