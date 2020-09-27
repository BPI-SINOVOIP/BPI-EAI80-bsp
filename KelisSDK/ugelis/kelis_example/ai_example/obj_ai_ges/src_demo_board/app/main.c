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
#include <faceai/faceai.h>

typedef struct app
{
    dev_cmd_t shell;
} app_t;

void app_init(app_t *app)
{
    dev_init();
    stream_init();
    cmd_init(&app->shell);
}

void app_deinit(app_t *app)
{
    cmd_exit(&app->shell);
    stream_deinit();
    dev_exit();
}

void app_start()
{
    stream_start_videoview();
}

void app_stop()
{
    stream_stop_videoview();
    ai_bodydetect_deinit();
}

int app_bind_event()
{
    cmd_register("objectai_exit", objectai_exit, 0);
    cmd_register("objectai_enter", objectai_enter, 0);
    return 0;
}

int app_unbind_event()
{
    cmd_unregister("objectai_exit");
    return 0;
}

void ai_bodydetect_deinit(void)
{

}

int main(void)
{
    app_t app;

    printf("body detect demo begin\n");

    app_init(&app);
    objectai_enter();
    app_start();
    while (1)
    {
        k_sleep(500);
    }

    app_stop();

    printf("body detect demo exit\n");

    return 0;
}
