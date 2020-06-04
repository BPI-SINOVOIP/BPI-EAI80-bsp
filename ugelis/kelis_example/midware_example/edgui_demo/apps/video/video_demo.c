/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          video_demo.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/02/20
 *
 * @brief         Start a floating video demo
 *
 * @note
 *    2020/02/20, Angrad.Yang, V1.0.0
 *        Initial version.
 */


#include <worker/worker.h>
#include <ts.h>
#include <inc/config.h>


K_THREAD_STACK_DEFINE(stack_area, 1 * 1024);


int stream_init()
{
    worker_t *cam   = get_worker("camera");
    worker_t *lcd   = get_worker("lcd");

    cam->init(cam, 0);
    lcd->init(lcd, 0);
}


int stream_start_videoview()
{
    worker_t *cam = get_worker("camera");
    worker_t *lcd = get_worker("lcd");
    cam->next[0] = lcd;
    cam->run(cam, 0, 0);

    return 0;
}

extern global_ts_cb gTouchCallback;

uint32_t demo_ts_cb(int index, int32_t x, int32_t y)
{
    worker_t *lcd = get_worker("lcd");
    if (index == 0)
    {
        worker_layer_rect_t rect;
        lcd->run(lcd, CMD_LCD_GETPOS, &rect);

        if (x < rect.x || x > rect.x + rect.w || y < rect.y ||  y > rect.y + rect.h)
        {
            return 1;
        }
        else
        {
            x = x - rect.w / 2;
            y = y - rect.h / 2;
            if (x + rect.w > DEV_LCD_W)
            {
                x = DEV_LCD_W - rect.w;
            }
            if (y + rect.y > DEV_LCD_H)
            {
                y = DEV_LCD_H - rect.h;
            }
            if (x < 0)
            {
                x = 0;
            }
            if (y < 0)
            {
                y = 0;
            }
            rect.x = x;
            rect.y = y;
            lcd->run(lcd, CMD_LCD_SETPOS, &rect);
            return 0;
        }
    }
    return 1;
}

void demo_thread(void *p1, void *p2, void *p3)
{
    stream_init();
    stream_start_videoview();
    gTouchCallback = demo_ts_cb;
}

void video_demo(struct k_thread *task_thread)
{
    k_thread_create(task_thread, stack_area,
                    K_THREAD_STACK_SIZEOF(stack_area),
                    (k_thread_entry_t)demo_thread,
                    0, 0, 0,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
}
