#include <ugelis.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ai_common.h"
#include "draw.h"
#include "ai_shell.h"
#include <shell/shell.h>

extern unsigned char INPUT_BASE[];
extern unsigned char WEI_BASE[];
extern struct easynet_dev *gdev;
extern void protocol_enter();
FaceAIData *faceai_data;
static void print_person()
{
    int i = 0;
    FaceAIData *fad = faceai_data;
    printf("Total imgs cnt %d\n", fad->total_imgs_cnt);
    printf("Total persons cnt %d\n", fad->person_cnt);
    printf("Current base:\n");
    for (i = 0; i < fad->person_cnt; i++)
    {
        printf("[%d]\t%s(%d)\n", i, fad->person_name[i], fad->person_img_cnt[i]);
    }
}

#define MY_SHELL_MODULE "ai_cmd"

static int shell_cmd_help(int argc, char *argv[])
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    printf("pause\n\tPause validating picture\n");
    printf("resume\n\tResume validating picture\n");
    printf("cap start\n\tCapture one picture and stop camera display\n");
    printf("cap stop\n\tDisplay camera video\n");
    printf("cap show\n\tShow the detected face\n");
    printf("cap hide\n\tHide the detected face\n");
    printf("cap ignore <m> <n> ... \n\tIgnore the capture, indexM, indexN...etc.\n");
    printf("cap cnt <cnt>\n\tSet cap cnt\n");
    printf("import <label> <person_name>\n\tImport the last capture to the base with <label>\n");
    printf("setthr <value>\n\tSet the threshold(value*100), eg: 0.68=> setthr 68\n");
    printf("gdb info\n\tShow cmd to dump base images and loadbase\n");
    printf("loadbase <cnt>\n\tRestore the baseimages from backup addr, which can get from cmd 'gdb info'\n");
    printf("showbase <label>\n\tShow the images of one person\n");
    printf("delbase <label>\n\tDelete the vector of one person\n");
    printf("rename <label> <newname>\n\tRename label to newname\n");
    printf("print <hide|show>\n\tHide/Show the result\n");
    printf("\n");
    printf("Flow to import img to base:\n");
    printf("\t1. cap start\n");
    printf("\t2. import <label> <person_name>\n");
    return 0;
}

#define USE_UARTPROTOCOL

static struct shell_cmd commands[] =
{
    { "help1", shell_cmd_help, NULL},
    { NULL, NULL }
};

static int aishell_input(int argc, char *argv[])
{
    int i = 0;
    FaceAIData *fad = faceai_data;
    if (strcmp(argv[0], "pause") == 0)
    {
        fad->network_predict = 0;
        printf("Paused!\n");
    }
    else if (strcmp(argv[0], "resume") == 0)
    {
        fad->network_predict = 1;
        printf("Resumed!\n");
    }
    else if (strcmp(argv[0], "cap") == 0)
    {
        if (strcmp(argv[1], "start") == 0)
        {
            ai_show_text("capturing");
            fad->skip_camera_stream = 0;
            fad->capture_started = 1;
            fad->capture_backup.index = 0;

            for (i = 0; i < CAPTURE_BACKUP_CNT; i++)
            {
                fad->capture_backup.ignore[i] = 0;
            }
        }
        else if (strcmp(argv[1], "stop") == 0)
        {
            fad->capture_started = 0;
            fad->skip_camera_stream = 0;
            ai_show_text("predicting");
        }
        else if (strcmp(argv[1], "show") == 0)
        {
            fad->show_detected_face = 1;
        }
        else if (strcmp(argv[1], "hide") == 0)
        {
            fad->show_detected_face = 0;
        }
        else if (strcmp(argv[1], "ignore") == 0)
        {
            int num = 0;
            for (i = 0; i < argc - 2; i++)
            {
                num = atoi(argv[2 + i]);
                if (num >= 0 && num < CAPTURE_BACKUP_CNT)
                {
                    printf("ignore index %d\n", num);
                    fad->capture_backup.ignore[num] = 1;
                }
            }
            //show_capture(fad, fad->capture_backup.data, fad->capture_backup.ignore, 0, fad->capture_backup.index);
        }
        else if (strcmp(argv[1], "cnt") == 0)
        {
            int num = 0;
            num = atoi(argv[2]);
            if (num >= 1 && num <= CAPTURE_BACKUP_CNT)
            {
                fad->capture_backup.cap_cnt = num;
            }
            else
            {
                printf("invalid cnt %d (%d~%d)\n", num, 1, CAPTURE_BACKUP_CNT);
            }
        }
    }
    else if (strcmp(argv[0], "import") == 0)
    {
        int label = -1;
        int i = 0;
        if (argc < 1)
        {
            printf("Usage: import [label] [name]\n");
            return 0;
        }
        fad->skip_camera_stream = 0;
        if (argc == 3)
        {
            label = atoi(argv[1]);
        }
        else if (argc == 2)
        {
            label = fad->person_cnt;
        }
        if (!fad->person_name[label])
        {
            fad->person_name[label] = malloc_ext(0, 16);
            fad->person_cnt++;
        }
        ai_show_text("importing");
        strcpy(fad->person_name[label], argv[argc - 1]);
        for (i = 0; i < fad->capture_backup.index; i++)
        {
            if (fad->capture_backup.ignore[i])
            {
                continue;
            }
            printf("Import the capture to base [%d] %s (%d)\n", label, fad->person_name[label], i);
            memcpy(fad->network_input, &fad->capture_backup.data[i], TEST_BUFFER_SIZE);
            fad->network_input[TEST_BUFFER_SIZE - 2] = label; //label
            import_record(gdev, fad, 1);
        }
        print_person();
        ai_show_text("imported");
    }
    else if (strcmp(argv[0], "setthr") == 0)
    {
        if (argc > 1)
        {
            fad->threshold = atoi(argv[1]) * 1.0 / 100;
            printf("Set threshold to %f\n", fad->threshold);
        }
    }
    else if (strcmp(argv[0], "list") == 0)
    {
        print_person();
    }
    else if (strcmp(argv[0], "gdb") == 0)
    {
        if (strcmp(argv[1], "info") == 0)
        {
            int size = fad->total_imgs_cnt * TEST_BUFFER_SIZE;
            void *addr = &fad->base_backup.data[0];
            printf("---gdb dump base images cmd:\n\t"
                   "dump binary memory E:\\\\yangy\\\\workspace\\\\record_base_backup.bin 0x%p 0x%p\n", addr, addr + size);
            printf("---finish dumping base images\n\n");

            size = VALIDATE_BACKUP_CNT * TEST_BUFFER_SIZE;
            addr = &fad->validate_backup.data[0];
            printf("---gdb dump validated image cmd:\n\t"
                   "dump binary memory E:\\\\yangy\\\\workspace\\\\validate_backup.bin 0x%p 0x%p\n", addr, addr + size);
            printf("---finish dumping validated image\n\n");

            printf("---load base cmd:\n\t"
                   "restore E:\\\\yangy\\\\workspace\\\\restored_base.bin binary 0x%p\n",
                   &fad->base_backup.data[fad->base_backup.index]);
            printf("---finish loading base cmd\n\n");
            size = fad->capture_backup.cap_cnt * TEST_BUFFER_SIZE;
            addr = &fad->capture_backup.data[0];

            printf("---gdb dump captured image cmd:\n\t"
                   "dump binary memory E:\\\\yangy\\\\workspace\\\\capture_backup.bin 0x%p 0x%p\n", addr, addr + size);
            printf("---finish dumping captured image\n\n");
            printf("---finish restoring commands\n\n");

            addr = fad->wei_addr;
            printf("---gdb restore weights:\n\t"
                   "restore E:\\\\yangy\\\\workspace\\\\full_weights_2.bin binary 0x%p\n", addr);
            printf("---finish restoring weights\n\n");

            addr = fad->network_input;
            printf("---gdb restore validate picture:\n\t"
                   "restore E:\\\\yangy\\\\workspace\\\\input_out.bin binary 0x%p\n", addr);
            printf("---finish restoring validate picture\n\n");

        }
    }
    else if (strcmp(argv[0], "loadbase") == 0)
    {
        int cnt = 0;
        int i = 0;
        int base_label = -1;
        int last_label = -1;
        int new_label = -1;
        if (argc < 2)
        {
            printf("loadbase [cnt]\n");
            return -1;
        }
        cnt = atoi(argv[1]);
        void *addr = &fad->base_backup.data[0];
        printf("restore %d base images from 0x%p\n", cnt, addr);

        for (i = 0; i < cnt; i++)
        {
            memcpy(fad->network_input, &fad->base_backup.data[fad->base_backup.index], TEST_BUFFER_SIZE);
            base_label = fad->network_input[TEST_BUFFER_SIZE - 2];
            printf("base_label %d\n", base_label);
            if (last_label != base_label)
            {
                last_label = base_label;
                new_label = fad->person_cnt;
                if (!fad->person_name[new_label])
                {
                    fad->person_name[new_label] = malloc_ext(0, 16);
                    fad->person_cnt++;
                    sprintf(fad->person_name[new_label], "base%d", base_label);
                }
            }
            fad->network_input[TEST_BUFFER_SIZE - 2] = new_label;
            addr = &fad->base_backup.data[fad->base_backup.index];
            *((unsigned char *)addr + TEST_BUFFER_SIZE - 2) = new_label;
            printf("Import the capture to base [%d] %s (%d) %d%%\n", new_label, fad->person_name[new_label], i, (i + 1) * 100 / cnt);
            import_record(gdev, fad, 0);

            fad->base_backup.index += 1;
        }
        printf("loadbase finished!\n");
    }
    else if (strcmp(argv[0], "rename") == 0)
    {
        if (argc < 3)
        {
            printf("rename <label> <newname>\n");
            return -1;
        }
        int label = atoi(argv[1]);
        if (label < fad->person_cnt)
        {
            strcpy(fad->person_name[label],  argv[2]);
        }
        printf("label %d is renamed to %s\n", label, fad->person_name[label]);
    }
    else if (strcmp(argv[0], "print") == 0)
    {
        if (strcmp(argv[1], "hide") == 0)
        {
            fad->debug_print = 0;
        }
        else if (strcmp(argv[1], "show") == 0)
        {
            fad->debug_print = 1;
        }
    }
    else if (strcmp(argv[0], "exit_shell") == 0)
    {
#ifdef USE_UARTPROTOCOL
        protocol_enter();
#endif
    }
    else if (strcmp(argv[0], "showbase") == 0)
    {
        int label = -1;
        int i = 0;
        int start_index = 0;
        if (argc < 2)
        {
            printf("showbase [label]\n");
            return -1;
        }
        label = atoi(argv[1]);
        for (i = 0; i < label; i++)
        {
            start_index += fad->person_img_cnt[i];
        }
        show_capture(fad, fad->base_backup.data, NULL, start_index, start_index + fad->person_img_cnt[label]);
    }
    else if (strcmp(argv[0], "delbase") == 0)
    {
        int label = -1;
        if (argc < 2)
        {
            printf("delbase [label]\n");
            return -1;
        }
        label = atoi(argv[1]);
        if (label < fad->person_cnt)
        {
            delete_record(fad, label);
            strcpy(fad->person_name[label], "deleted");
            print_person();

        }
    }
    return 0;
}
void ai_init_shell(FaceAIData *data)
{
    faceai_data = data;

    SHELL_REGISTER(MY_SHELL_MODULE, commands);
    shell_register_default_module(MY_SHELL_MODULE);
    shell_register_app_cmd_handler(aishell_input);
}
