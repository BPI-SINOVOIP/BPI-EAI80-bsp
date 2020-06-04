/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        main.c
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       Sample code for adc test case.
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#include <ugelis.h>
#include <misc/printk.h>
#include <edgui/edgui.h>

extern void demo_create(void);
extern void edgui_port_disp_init(void);
extern void edgui_port_indev_init(void);
extern void edgui_init(void);
void edgui_test_theme_1(edgui_theme_t *th);
extern void edgui_tutorial_animations(void);
extern void edgui_tutorial_objects(void);
extern void edgui_test_theme_2(void);

void main(void)
{
    edgui_theme_t *th[4];
    uint32_t    count = 0;
    uint32_t    index = 0;

    edgui_init();
    edgui_port_disp_init();
    edgui_port_indev_init();
    th[0] = edgui_theme_material_init(20, NULL);
    th[1] = edgui_theme_night_init(200, NULL);
    th[2] = edgui_theme_alien_init(210, NULL);
    th[3] = edgui_theme_zen_init(50, NULL);
    edgui_test_theme_1(th[1]);
    edgui_test_theme_2();
    //edgui_tutorial_animations();
    //demo_create();

    struct k_thread task_thread;
    video_demo(&task_thread);

    while (1)
    {
        edgui_tick_inc(10);
        edgui_task_handler();
        k_sleep(10);
        count++;
        if (count >= 200)
        {
            //edgui_theme_set_current(th[index]);
            index++;
            if (index > 3)
            {
                index = 0;
            }

            count = 0;
        }
    }
}

