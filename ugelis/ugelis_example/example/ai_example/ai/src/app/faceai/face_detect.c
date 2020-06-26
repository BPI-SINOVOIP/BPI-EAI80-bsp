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
#include <string.h>

#include "face_detect.h"
#include "inc/config.h"
#include "worker/painter_worker.h"
#include "faceai/face_detect/encode.h"
#include "lib/image.h"

/*
 * One frame is stored in 'cam_buf.detected' as same as 'cam_buf.tmp' before start to face detect.
 * The coordinate and shape argument could be calculated from function 'face_extract',
 * if there is a face in the frame 'cam_buf.detected'.
 * Since the face is deteced, it will be extracted from buffer 'cam_buf.tmp' exactly with
 * the coordinate and shape argument and then be zoomed up to matched with the required size of the network.
 * But the format of this scaled image is RGBRGBRGB..., which is different from splitted RRRGGGBBB,
 * the input format of the trained network, so the final step is image_split.
 */
static int face_detect(pre_proc_param_t *pp)
{
    int x, y, w, h;
    int res[4] = {0, 0, 0, 0};
    int orig_x, orig_y, orig_w, orig_h;
    worker_pre_sizes_t *sz = &(pp->sizes);
    paint_rect_t rect[3];
    memset(rect, 0, sizeof(paint_rect_t) * 3);

    face_extract(res, pp->detected_buf);

    if (res[2] == 0)
    {
        return 0;
    }

    x = res[0];
    y = res[1];
    w = res[2];
    h = res[2];

    /* Crop the detected rect where the face in from the original frame */
    orig_x = ui_rel(x, sz->src_w, sz->input_w, 0);
    orig_y = ui_rel(y, sz->src_h, sz->input_h, 0);
    orig_w = ui_rel(w, sz->src_w, sz->input_w, 0);
    orig_h = ui_rel(h, sz->src_h, sz->input_h, 0);

    image_crop((unsigned char *)pp->original_buf, (unsigned char *)pp->detected_buf, orig_x, orig_y, orig_w, orig_h, sz->src_w);
    //ui_draw_rgb24(pp->detected_buf, 0, 0, orig_w, orig_h);

    /* Zoom the cropped face image, which can be any size, up/down to the pointed size
       that equals to the input of AI model */
    image_zoom((unsigned char *)pp->detected_buf, (unsigned char *)pp->original_buf, orig_w, orig_h, sz->ai_w, sz->ai_h);
    //ui_draw_rgb24(pp->original_buf, 10, 20, sz->ai_w, sz->ai_h);

    rect[0].x0 = x;
    rect[0].y0 = y;
    rect[0].x1 = w;
    rect[0].y1 = h;
    rect[0].wh_flag = 1;
    rect[0].color = UI_FOCUS_COLOR;
    rect[0].rel = 1;
    rect[0].rel_w = sz->input_w;
    rect[0].rel_h = sz->input_h;
    rect[0].use   = 1;
    pp->painter->run(pp->painter, CMD_PAINT_RECT, rect);

    return pp->original_buf;
}

int face_detect_init(pre_proc_param_t *param)
{
    int frame_pixels = 3;
    worker_pre_sizes_t *sz = &(param->sizes);

    param->original_buf = (int)kmalloc(sz->src_w * sz->src_h * frame_pixels);
    param->detected_buf = (int)kmalloc(sz->src_w * sz->src_h * frame_pixels);
    face_init(sz->input_w, sz->input_h);
    param->painter = get_worker("painter");

    return 0;
}

int face_detect_exit(pre_proc_param_t *param)
{
    pre_proc_param_t *pp = (pre_proc_param_t *)param;
    kfree((void *)pp->original_buf);
    kfree((void *)pp->detected_buf);

    return 0;
}

int face_detect_frame_in(pre_proc_param_t *pp)
{
    int frame_pixels = 3;
    worker_pre_sizes_t *sz = &(pp->sizes);
    if (pp->in[0] && pp->original_buf)
    {
        memcpy((void *)pp->original_buf, pp->in[0], sz->src_w * sz->src_h * frame_pixels);
        //ui_draw_rgb24(param->original_buf, 0, 0, sz->src_w, sz->src_h);
    }
    if (pp->in[1] && pp->detected_buf)
    {
        memcpy((void *)pp->detected_buf, pp->in[1], sz->input_w * sz->input_h);
        //ui_draw_l8(param->detected_buf, 0, 0, sz->input_w, sz->input_h);
    }
    return face_detect(pp);
}
