#include <shell/shell.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ai_common.h"
#include "draw.h"


ObjDetData *objdet_data;

#define MY_SHELL_MODULE "ai_cmd"

static int shell_cmd_help(int argc, char *argv[])
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    //printf("cap start\n\tCapture one picture and stop camera display\n");
    //printf("cap stop\n\tDisplay camera video\n");
    printf("resume\n\tDisplay camera video\n");
    printf("demo\n\tDisplay the bounding box for input picture\n");
    printf("setthr <value>\n\tSet the threshold(value*100), eg: 0.68=> setthr 68\n");
    printf("\n");
    return 0;
}

static struct shell_cmd commands[] =
{
    { "help1", shell_cmd_help, NULL},
    { NULL, NULL }
};

int shell_input(int argc, char *argv[])
{
    if (strcmp(argv[0], "demo") == 0)
    {
        objdet_data->show_demo_pic = 1;
        objdet_data->one_frame_detect_started = 1;
        objdet_data->show_demopic_onetime = 1;
    }
    else if (strcmp(argv[0], "resume") == 0)
    {
        objdet_data->show_demo_pic = 0;
        objdet_data->one_frame_detect_started = 0;
        objdet_data->show_demopic_onetime = 0;
        objdet_data->cap_show = 1; // always as 1;
    }
    else if (strcmp(argv[0], "setthr") == 0)
    {
        if (argc > 1)
        {
            objdet_data->threshold = atoi(argv[1]) * 1.0 / 100;
            printf("Set threshold to %f\n", objdet_data->threshold);
        }
    }
    else if (strcmp(argv[0], "cap_show") == 0)
    {
        objdet_data->cap_show = 1;
    }
    else if (strcmp(argv[0], "type") == 0)
    {
        if (argc > 1)
        {
            if (atoi(argv[1]) > DET_GEST)
            {
                printf("Wrong det_type set \n");
            }
            else
            {

            }
        }
    }
    else if (strcmp(argv[0], "save") == 0)
    {
        /*this allows system to stop recording pics into the default buffer*/
        objdet_data->pic_saved = 1;
    }

    return 0;
}


#ifdef __USE_KEIL_
static struct shell_module(__shell__name) __used \
__attribute__((section(".shell_module_"))) = { \
                                               .module_name = MY_SHELL_MODULE, \
                                               .commands = commands, \
                                               .prompt = NULL \
                                             };
#endif

void init_shell(ObjDetData *objdetdata)
{
    objdet_data = objdetdata;
#ifndef __USE_KEIL_
    SHELL_REGISTER(MY_SHELL_MODULE, commands);
#endif
    shell_register_default_module(MY_SHELL_MODULE);
    shell_register_app_cmd_handler(shell_input);
}
