#include "worker/painter_worker.h"
#include "inc/config.h"

#ifdef EDGUI_CONF_INCLUDE_SIMPLE
    #include "edgui.h"
    #include "edgui_conf.h"
#else
    #include <edgui/edgui.h>
    #include <edgui/edgui_conf.h>
#endif

#include "edgui_prj_conf.h"


K_THREAD_STACK_DEFINE(edgui_stack_area, 2 * 1024);

typedef struct edgui_info
{
    gui_info_t gui;
    struct k_mutex mutex;
    int running;
    k_tid_t task_id;
    struct k_thread task_thread;
} edgui_info_t;

edgui_info_t edgui;
static painter_event_cb gUiEvent = 0;

static void edgui_show_pic(paint_info_t *info)
{

}

static void edgui_show_text(paint_info_t *info)
{

}

static void edgui_show_char(paint_info_t *info)
{

}

static void edgui_show_rect(paint_rect_t *rects)
{
    int i = 0;
    for (i = 0; i < 3; i++)
    {
        paint_rect_t *rect = rects + i;
        if (!rect->use)
        {
            continue;
        }
        rect->x0 = ui_rel(rect->x0, CAM_DISP_W, rect->rel_w, CAM_DISP_X);
        rect->y0 = ui_rel(rect->y0, CAM_DISP_H, rect->rel_h, CAM_DISP_Y);
        if (rect->wh_flag)
        {
            rect->x1 = rect->x0 + ui_rel(rect->x1, CAM_DISP_W, rect->rel_w, 0) - 1;
            rect->y1 = rect->y0 + ui_rel(rect->y1, CAM_DISP_H, rect->rel_h, 0) - 1;
        }
        else
        {
            rect->x1 = ui_rel(rect->x1, CAM_DISP_W, rect->rel_w, CAM_DISP_X);
            rect->y1 = ui_rel(rect->y1, CAM_DISP_H, rect->rel_h, CAM_DISP_Y);
        }
    }
}

static void edgui_fill_frame(paint_info_t *info)
{

}

static void edgui_exit(gui_info_t *gui)
{

}

static void edgui_set_event_cb(void *param)
{
    gUiEvent = param;
}

static void ui_thread(void *data, edgui_info_t *edgui, void *not_used)
{
    printf("ui_thread start\n");
    while (edgui->running)
    {
        edgui_tick_inc(10);
        edgui_task_handler();
        k_sleep(10);
    }
}

gui_info_t *edgui_start()
{

    edgui_init();
    edgui_port_disp_init();
    edgui_port_indev_init();

    edgui_test_imgbtn_1();

    edgui.running = 1;
    edgui.task_id = k_thread_create(&edgui.task_thread, edgui_stack_area,
                                    K_THREAD_STACK_SIZEOF(edgui_stack_area),
                                    (k_thread_entry_t)(ui_thread),
                                    0, &edgui, 0,
                                    K_PRIO_PREEMPT(0), 0, K_NO_WAIT);

    edgui.gui.data = &edgui;
    edgui.gui.show_pic   = edgui_show_pic;
    edgui.gui.show_text  = edgui_show_text;
    edgui.gui.show_char  = edgui_show_char;
    edgui.gui.fill_frame = edgui_fill_frame;
    edgui.gui.show_rect  = edgui_show_rect;
    edgui.gui.exit       = edgui_exit;
    edgui.gui.set_event_cb = edgui_set_event_cb;

    return &edgui.gui;
}
