#include "touchpad.h"

K_THREAD_STACK_DEFINE(touch_stack_area, 1024);

Touch_Screen_t touchScreen;
static void touch_detect_thread(void *data, void *param, void *p3)
{
    dev_touch_t *self = (dev_touch_t *)data;
    printf("touch_detect_thread\n");
    while (1)
    {
        touchpad_read(self);
        k_yield();
    }
}

static int touch_thread_start(dev_touch_t *self)
{
    k_thread_create(&self->task_thread, touch_stack_area,
                    K_THREAD_STACK_SIZEOF(touch_stack_area),
                    (k_thread_entry_t)touch_detect_thread,
                    self, 0, 0,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
}

int touchpad_init(dev_touch_t *touch_dev)
{
    touch_dev->gt9147 = device_get_binding(GT9147_DEV_NAME);
    touchScreen.dir = 1;
    touch_thread_start(touch_dev);
}

bool touchpad_read(dev_touch_t *dev)
{
    int  i;
    int32_t  yr;
    int32_t  xr;
    static int last_x = 0;
    static int last_y = 0;
    Touch_Sta_T touchsta;

    touch_scan(dev->gt9147);
    if (touchScreen.touchPoints[0].valid)
    {
        yr = touchScreen.touchPoints[0].y;
        xr = touchScreen.touchPoints[0].x;
        if (yr <= 0)
        {
            yr = 0;
        }
        else if (yr > 272)
        {
            yr = 271;
        }

        if (xr <= 0)
        {
            xr = 0;
        }
        else if (xr > 480)
        {
            xr = 480 - 1;
        }

        touchsta = touchScreen.touchPoints[0].touchSta;
        if ((touchsta == DOWN) || (touchsta == MOVE))
        {
            //printf("down\n");
            last_x = xr;
            last_y = yr;
        }
        else if (touchsta == UP)
        {
            //printf("up\n");
            //dev->touch_cb.func(GPIO_KEY_SHORTPRESS, dev->touch_cb.arg);
        }
    }
    else
    {

    }
    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}
