/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          face_detect.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Detector to extract face
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#include <kernel.h>

#include "face_detect.h"

#include <camera/dcmi.h>
#include <display/fb_display.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "ai_common.h"
#include "processor.h"
#include "math.h"
#include "lcd.h"
#include "camera.h"
#include "draw.h"
#include <board.h>
#include <gm_hal_vin.h>
#include <gpio.h>
#include "gm_hal_kdp.h"
#include <stdlib.h>
#include <pinmux.h>
#include <imlib.h>
#include <gm_hal_clock.h>
#include "app_processor.h"
#include <gm_hal_reset.h>
#include "dev/cmd.h"
#include "dev/dev.h"
#include "inc/config.h"
#include "inc/common.h"
#include "worker/worker.h"
#include "worker/pre_worker.h"

/*
 * One frame is stored in 'cam_buf.detected' as same as 'cam_buf.tmp' before start to face detect.
 * The coordinate and shape argument could be calculated from function 'face_extract',
 * if there is a face in the frame 'cam_buf.detected'.
 * Since the face is deteced, it will be extracted from buffer 'cam_buf.tmp' exactly with
 * the coordinate and shape argument and then be zoomed up to matched with the required size of the network.
 * But the format of this scaled image is RGBRGBRGB..., which is different from splitted RRRGGGBBB,
 * the input format of the trained network, so the final step is image_split.
 */

static void face_detect(face_detect_t *fd, void *param)
{
    int i = 0;
    int res[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    //unsigned int t_last, t_now, delta;
    //t_last = k_cycle_get_32();
    //face_extract(res, fd->detected_buf);
    //t_now = k_cycle_get_32();
    //delta = t_now - t_last;
    //printf("face_detect cost %f\n", delta * 1.0f / sys_clock_hw_cycles_per_sec);
    for (i = 0; i < 20; i += 4)
    {
        if (res[i + 2] == 0)
        {
            break;
        }
        fd->x = res[i];
        fd->y = res[i + 1];
        fd->w = res[i + 2];
        fd->h = res[i + 2];
        //printf("rect(%d %d %d %d)\n", fd->x, fd->y, fd->w, fd->h);
        fd->finish_cb(fd, param);
    }
}

void face_detect_thread(void *data, void *param, void *p3)
{
    printf("easynet_detect_thread\n");
    uint32_t demoaddr = 0;
    face_detect_t *fd = (face_detect_t *)data;

    worker_t *self = (worker_t *)param;
    pre_worker_info_t *pre = (pre_worker_info_t *)self->data;

    while (1)
    {
        if (fd->frame_ready)
        {
            /*cam_buf_validate now has the KDP 310 format*/
            rgb_split2rrggbb(pre->buf.detected, pre->buf.tmp, INPUT_WIDTH, INPUT_HEIGHT);

            fd->finish_cb(fd, param);

            fd->frame_ready = 0;
        }

        k_yield();
        if (fd->exit)
        {
            break;
        }
    }

}

#if 0
void face_detect_thread(void *data, void *param, void *p3)
{
    printf("face detect thread start!\n");
    face_detect_t *fd = (face_detect_t *)data;
    while (1)
    {
        if (fd->frame_ready)
        {
            face_detect(fd, param);
            fd->frame_ready = 0;
        }
        k_yield();
        if (fd->exit)
        {
            break;
        }
    }
}
#endif

void face_detect_init(int w, int h)
{
    //face_init(w, h);
}
