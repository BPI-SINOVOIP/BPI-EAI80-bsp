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
    face_extract(res, fd->detected_buf);
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

void face_detect_init(int w, int h)
{
    face_init(w, h);
}
