#include <stdio.h>
#include <string.h>
#include <ts.h>
#include <display/fb_display.h>

#include "ugui.h"
#include "worker/painter_worker.h"
#include "inc/config.h"
#include "ui/picture_raw.h"

K_THREAD_STACK_DEFINE(ugui_stack_area, 2 * 1024);

typedef struct ugui_info
{
    struct k_thread task_thread;
    k_tid_t task_id;
    paint_rect_t rect[3];
    paint_rect_t last_rect[3];
    struct k_mutex mutex;
    void *ts_data;
    int running;
    gui_info_t gui;
} ugui_info_t;

ugui_info_t ugui;

UG_GUI uGUI;
#define MAX_OBJECTS 10

UG_TEXTBOX textbox_1;
UG_WINDOW window_1;
UG_WINDOW window_2;
UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];
UG_OBJECT obj_buff_wnd_2[1];
int last_btn_id = 0;
static painter_event_cb gUiEvent = 0;

static int frame_cnt = 0x7FFFFFFF;

void gui_clear_sync()
{
    frame_cnt = 0x7FFFFFFF;
    ui_clear_area(CAM_DISP_X, 0, CAM_DISP_W, CAM_DISP_H);
}

int touch_read_xy(int *x, int *y, void *data)
{
    int ret = -1;
    struct ts_input_event event;
    ret = read_ts_event(&event, 50);//K_FOREVER);
    if (!ret)
    {
        if (event.type == ABS_XY)
        {
            if (event.press_status == TS_PRESS)
            {
                /*ts press*/
                *x = event.x_abs;
                *y = event.y_abs;
                return 1;
            }
            else
            {
                /*ts release*/
                return 0;
            }
        }
    }
    return -1;
}

static  void ui_thread(void *data, ugui_info_t *ugui, void *not_used)
{
    printf("ui_thread start\n");
    int x, y, i;
    int ret;

    while (ugui->running)
    {
        ret = touch_read_xy(&x, &y, ugui->ts_data);
        if (ret == 1)   //pressed
        {
            UG_TouchUpdate(x, y, TOUCH_STATE_PRESSED);        // If touch data is valid, Update uGUI Touch
        }
        else if (ret == 0)
        {
            UG_TouchUpdate(-1, -1, TOUCH_STATE_RELEASED);
        }
        if (ugui->rect[0].update)
        {
            k_mutex_lock(&ugui->mutex, K_FOREVER);
            for (i = 0; i < 3; i++)
            {
                paint_rect_t *rect = &ugui->rect[i];
                paint_rect_t *last_rect = &ugui->last_rect[i];
                if (last_rect->use)
                {
                    UG_DrawRoundFrame(last_rect->x0, last_rect->y0, last_rect->x1, last_rect->y1, 10, (last_rect->color & 0x00FFFFFF));
                    UG_DrawRoundFrame(last_rect->x0 + 1, last_rect->y0 + 1, last_rect->x1 - 1, last_rect->y1 - 1, 10, (last_rect->color & 0x00FFFFFF));
                }
                if (rect->use)
                {
                    UG_DrawRoundFrame(rect->x0, rect->y0, rect->x1, rect->y1, 10, rect->color);
                    UG_DrawRoundFrame(rect->x0 + 1, rect->y0 + 1, rect->x1 - 1, rect->y1 - 1, 10, rect->color);
                }
                ugui->last_rect[i] = ugui->rect[i];
                ugui->rect[i].update = 0;
            }
            k_mutex_unlock(&ugui->mutex);
            frame_cnt = 3;  //retain frames
        }
        else if (frame_cnt-- < 0)
        {
            gui_clear_sync();
        }
        UG_Update();
        k_sleep(50);
    }
    printf("bye ui_thread\n");
}


int create_image_button(UG_WINDOW *win, ugui_icon_info_t *info, int id)
{
    image_btn *btn = &info->btn;

    btn->id = id;
    btn->x = info->pos_x;
    btn->y = info->pos_y;
    btn->w = info->w;
    btn->h = info->h;
    btn->img_normal.width   = info->w;
    btn->img_normal.height  = info->h;
    btn->img_normal.bpp     = BMP_BPP_16,
         btn->img_normal.colors  = BMP_RGB888,
              memcpy(&btn->img_pressed, &btn->img_normal, sizeof(btn->img_normal));
    memcpy(&btn->img_hide, &btn->img_normal, sizeof(btn->img_normal));
    btn->img_pressed.p      = (void *)info->pressed;
    btn->img_normal.p       = (void *)info->normal;
    btn->img_hide.p         = (void *)info->hide;
    UG_ImageCreate(win, &btn->image, btn->id, btn->x, btn->y, btn->x + btn->w - 1, btn->y + btn->h - 1);
    UG_ImageSetBMP(win, btn->id, &btn->img_normal);

    return 0;
}

void image_hide(UG_WINDOW *win, int id)
{
    UG_ImageSetBMP(win, id, &icons[id].btn.img_hide);
    UG_OBJECT *obj = _UG_SearchObject(win, OBJ_TYPE_IMAGE, id);
    obj->state &= ~(OBJ_STATE_TOUCH_ENABLE);
}

void image_show(UG_WINDOW *win, int id)
{
    UG_ImageSetBMP(win, id, &icons[id].btn.img_normal);
    UG_OBJECT *obj = _UG_SearchObject(win, OBJ_TYPE_IMAGE, id);
    obj->state |= OBJ_STATE_TOUCH_ENABLE;
}

void window_1_callback(UG_MESSAGE *msg)
{
    if (msg->type == MSG_TYPE_OBJECT)
    {
        if (msg->id == OBJ_TYPE_BUTTON || msg->id == OBJ_TYPE_IMAGE)
        {
            if (msg->event == OBJ_EVENT_PRESSED)
            {
                switch (msg->sub_id)
                {
                    case BTN_ID_1:
                    case BTN_ID_2:
                    case BTN_ID_3:
                    case BTN_ID_4:
                        if (last_btn_id != msg->sub_id)
                        {
                            UG_ImageSetBMP(&window_1, last_btn_id, &icons[last_btn_id].btn.img_normal);
                            /* faceai */
                            if (msg->sub_id == BTN_ID_1)
                            {
                                image_show(&window_1, BTN_ID_5);
                                image_show(&window_1, BTN_ID_6);
                            }
                            else
                            {
                                image_hide(&window_1, BTN_ID_5);
                                image_hide(&window_1, BTN_ID_6);
                            }
                        }
                        last_btn_id = msg->sub_id;
                    case BTN_ID_5:
                    case BTN_ID_6:
                        UG_TextboxSetForeColor(&window_1, TXB_ID_0, 0xFFFFFFFF);
                        UG_TextboxSetText(&window_1, TXB_ID_0, "AI Demo");
                        UG_ImageSetBMP(&window_1, msg->sub_id, &icons[msg->sub_id].btn.img_pressed);
                        _UG_UpdateObjects(&window_1);
                        //ui_clear_area(CAM_DISP_X, 0, CAM_DISP_W, CAM_DISP_H);
                        if (gUiEvent)
                        {
                            gUiEvent(msg->sub_id);
                        }
                        break;
                }
            }
            if (msg->event == OBJ_EVENT_RELEASED)
            {
                switch (msg->sub_id)
                {

                    case BTN_ID_5:
                    case BTN_ID_6:
                        UG_ImageSetBMP(&window_1, msg->sub_id, &icons[msg->sub_id].btn.img_normal);
                        break;
                }
            }
        }
    }
}

static void ugui_show_pic(paint_info_t *info)
{
    ui_draw_pic(info->x, info->y, info->w, info->h, (unsigned char *)info->pic, 128);
}

static void ugui_show_text(paint_info_t *info)
{
    UG_TextboxSetForeColor(&window_1, TXB_ID_0, info->color);
    UG_TextboxSetText(&window_1, TXB_ID_0, info->str);
}

static void ugui_show_char(paint_info_t *info)
{
    ui_draw_chstr(info->x, info->y, info->w, info->h, info->size, info->str, info->color);
}

static void ugui_show_rect(paint_rect_t *rects)
{
    int i = 0;
    k_mutex_lock(&ugui.mutex, K_FOREVER);
    memset(ugui.rect, 0, sizeof(paint_rect_t) * 3);
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
        ugui.rect[i] = *rect;
        ugui.rect[i].update = 1;
    }
    k_mutex_unlock(&ugui.mutex);
}

static void ugui_fill_frame(paint_info_t *info)
{
    dev_ui_t *ui = ui_get_handler();
    int y;
    int y2 = info->y + info->h  - 1;
    int ui_width   = ui->w * ui->pixel_size;
    int dest_width = info->w * ui->pixel_size;
    int dest_buf = ui->buf + (ui->w * info->y + info->x) * ui->pixel_size;
    for (y = info->y; y <= y2; y++)
    {
        memset((void *)dest_buf, info->color & 0xFF, dest_width);
        dest_buf += ui_width;
    }
}

static void ugui_exit(gui_info_t *gui)
{
    ugui_info_t *ugui = gui->data;
    ugui->running = 0;
}

static void ugui_set_event_cb(void *param)
{
    gUiEvent = (painter_event_cb)param;
}

gui_info_t *ugui_start()
{
    int i = 0;
    UG_Init(&uGUI, (UG_Point_Func)ui_draw_point, DEV_LCD_W, DEV_LCD_H);

    UG_FontSelect(&FONT_8X12);
    UG_SetBackcolor(C_BLACK);
    UG_SetForecolor(C_YELLOW);

    UG_WindowCreate(&window_1, obj_buff_wnd_1, MAX_OBJECTS, window_1_callback);

    UG_WindowSetXStart(&window_1, 0);
    UG_WindowSetYStart(&window_1, 0);
    UG_WindowSetXEnd(&window_1, CAM_DISP_X - 1);
    UG_WindowSetYEnd(&window_1, CAM_DISP_H - 1);
    UG_WindowSetStyle(&window_1, WND_STYLE_HIDE_TITLE);

    for (i = 0; i < sizeof(icons) / sizeof(icons[0]); i++)
    {
        if (START_APP == APP_BODY && i == 1)
        {
            continue;
        }
        create_image_button(&window_1, &icons[i], i);
    }

    UG_OBJECT *obj = _UG_SearchObject(&window_1, OBJ_TYPE_IMAGE, 0);
    obj->state &= ~(OBJ_STATE_TOUCH_ENABLE);

    last_btn_id = START_APP + 1;
    UG_ImageSetBMP(&window_1, last_btn_id, &icons[last_btn_id].btn.img_pressed);

    UG_WindowShow(&window_1);

    UG_TextboxCreate(&window_1, &textbox_1, TXB_ID_0, 0, 15 * DESIGN_W, CAM_DISP_X - 2, 30 + 15 * DESIGN_W - 1);
    UG_TextboxSetFont(&window_1, TXB_ID_0, &FONT_8X12);
    UG_TextboxSetText(&window_1, TXB_ID_0, "AI Demo");//This is a \n test sample window!");
    UG_TextboxSetForeColor(&window_1, TXB_ID_0, 0xFFFFFFFF);
    UG_TextboxSetBackColor(&window_1, TXB_ID_0, UI_BKGRD_COLOR);
    UG_TextboxSetAlignment(&window_1, TXB_ID_0, ALIGN_CENTER);

    UG_Update();

    ugui.running = 1;
    ugui.task_id = k_thread_create(&ugui.task_thread, ugui_stack_area,
                                   K_THREAD_STACK_SIZEOF(ugui_stack_area),
                                   (k_thread_entry_t)(ui_thread),
                                   0, &ugui, 0,
                                   K_PRIO_PREEMPT(0), 0, K_NO_WAIT);
    k_mutex_init(&ugui.mutex);
    memset(ugui.rect, 0, sizeof(paint_rect_t) * 3);
    memset(ugui.last_rect, 0, sizeof(paint_rect_t) * 3);

    ugui.gui.data = &ugui;
    ugui.gui.show_pic   = (gui_func)ugui_show_pic;
    ugui.gui.show_text  = (gui_func)ugui_show_text;
    ugui.gui.show_char  = (gui_func)ugui_show_char;
    ugui.gui.fill_frame = (gui_func)ugui_fill_frame;
    ugui.gui.show_rect  = (gui_func)ugui_show_rect;
    ugui.gui.exit       = (gui_func)ugui_exit;
    ugui.gui.set_event_cb = (gui_func)ugui_set_event_cb;
    return &ugui.gui;
}
