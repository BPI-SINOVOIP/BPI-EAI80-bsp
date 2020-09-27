#include <camera/dcmi.h>
#include <display/fb_display.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include "processor.h"
#include <gm_hal_vin.h>
#include <gm_hal_otp.h>
#include <gm_hal_reset.h>
#include <gm_hal_clock.h>

#include "ai_common.h"
#include "ai_shell.h"
#include "math.h"
#include "lcd.h"
#include "camera.h"
#include "draw.h"
#include "algo.h"
#include "img.h"
#include "storage.h"
//#include "face_ai_img_data.h"

extern unsigned short inputImage[];
extern unsigned short OutputImage[];

//#define AI_FACE_DEBUG
#ifdef AI_FACE_DEBUG
    #define FACE_DEBUG   printf
#else
    #define FACE_DEBUG(...)
#endif

//#define USE_UARTPROTOCOL

void face_extract(int *res_, uint32_t addr);

extern unsigned char INPUT_BASE[];
extern unsigned char WEI_BASE[];
extern float OutputVectorFC[];
extern frame_data_t VALIDATE_BACKUP[];
extern frame_data_t RECORD_BASE_BACKUP[];
extern frame_data_t CAPTURE_BACKUP[];

int debug_ai = 0;
struct easynet_dev *gdev ;

extern void protocol_start();
extern void protocol_send();
faceinfo_t  faceout;
//#define USE_IMP

FaceAIData faceai;
#define UNKNOWN_PERSON "unknown"

K_THREAD_STACK_DEFINE(stack_area, 4 * SIZE_1K);

static char *recorded_person_name[MAX_PERSON_CNT] = { };

void print_float(float array[], int maxlen)
{
    int i;

    for (i = 0; i < maxlen; i++)
    {
        FACE_DEBUG("%.2f ", array[i]);
    }
    FACE_DEBUG("\n");

    return ;
}

void print_int(int array[], int maxlen)
{
    int i;

    for (i = 0; i < maxlen; i++)
    {
        FACE_DEBUG("%4d ", array[i]);
    }
    FACE_DEBUG("\n");

    return ;
}

//clear ui on the bottom of the video
void ui_clear_textarea(FaceAIData *fad)
{
    lcd_clear_area(1, fad->text_area_y_start, fad->ui_w - 2, fad->ui_h - fad->text_area_y_start - 1);
}

void ui_clear_cam()
{
    lcd_clear_area(1, 1, CAM_CROP_W - 1, CAM_CROP_H - 1);
}

void ai_show_text(char *text)
{
    int x = INPUT_WIDTH;
    int y = CAM_CROP_H + 3;
    int width = LCD_UI_W - 3;
    int height = 24;
    lcd_clear_area(1, y, width, LCD_UI_H - y - 1);
    lcd_draw_text(x, y, LCD_UI_W - x - 1, height, height, text, RECT_COLOR);
}

void show_capture(FaceAIData *fad, frame_data_t *data, int ignore[], int start, int end)
{
    int thumb_w = fad->thumb_w;
    int thumb_h = fad->thumb_h;
    int image_w = fad->image_w;
    int image_h = fad->image_h;
    int text_area_y_start = fad->text_area_y_start;
    int ui_w = fad->ui_w;
    int ui_h = fad->ui_h;
    char *small_img = (char *)malloc_ext(0, thumb_w * thumb_h * 3);
    char dist_str[16];
    unsigned int i = 0;
    unsigned int x = 1;
    unsigned int y = text_area_y_start;

    ui_clear_textarea(fad);

    for (i = start; i < end; i++)
    {
        if (ignore && ignore[i])
        {
            continue;
        }
        image_zoom((unsigned char *)&data[i], small_img, image_w, image_h, thumb_w, thumb_h);
        lcd_draw_rgb24(small_img, x, y, thumb_w, thumb_h);
        sprintf(dist_str, "%d", i);
        lcd_draw_text(x, y, 16, 16, 16, dist_str, 0xFFFF0000);
        x += thumb_w;

        if ((x >= ui_w) || (x + thumb_w >= ui_w))
        {
            x = 1;
            y += thumb_h;
            if (y > ui_h)
            {
                break;
            }
        }
    }
    free_ext(0, small_img);
}

/*
 * One frame is stored in 'cam_buf.detected' as same as 'cam_buf.tmp' before start to face detect.
 * The coordinate and shape argument could be calculated from function 'face_extract',
 * if there is a face in the frame 'cam_buf.detected'.
 * Since the face is deteced, it will be extracted from buffer 'cam_buf.tmp' exactly with
 * the coordinate and shape argument and then be zoomed up to matched with the required size of the network.
 * But the format of this scaled image is RGBRGBRGB..., which is different from splitted RRRGGGBBB,
 * the input format of the trained network, so the final step is image_split.
 */

static void face_detect(FaceAIData *fad)
{
    unsigned int t_last, t_now, delta;
    int res[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int index_ = 0;
    char dist_str[32];
    face_extract(res, (uint32_t)fad->cam_buf.detected);
    for (index_ = 0; index_ < 20; index_ += 4)
    {
        if (res[index_ + 2] == 0)
        {
            faceout.num = 0;
#ifdef USE_UARTPROTOCOL
            protocol_send();
#endif
            break;
        }
        int w = res[index_ + 2];
        int h = res[index_ + 2];

        faceout.num     = 1;
        faceout.off_l   = res[index_];
        faceout.off_t   = res[index_ + 1];
        faceout.width   = w;
        faceout.height  = h;

#ifdef USE_UARTPROTOCOL
        protocol_send();
#endif

        //set_ui_layer(0,0);//res[index_], res[index_+1]);
        FACE_DEBUG("rect(%d %d %d %d)\n", res[index_], res[index_ + 1], w, h);

        t_last = k_cycle_get_32();
        image_crop(fad->cam_buf.tmp, fad->cam_buf.cropped, res[index_], res[index_ + 1], w, h, CAM_CROP_W);
        t_now = k_cycle_get_32();
        delta = t_now - t_last;
        FACE_DEBUG("crop %f\n", delta * 1.0f / sys_clock_hw_cycles_per_sec);

        t_last = k_cycle_get_32();
        image_zoom(fad->cam_buf.cropped, fad->cam_buf.tmp, w, h, fad->image_w, fad->image_h);
        t_now = k_cycle_get_32();
        delta = t_now - t_last;
        FACE_DEBUG("zoom %f\n", delta * 1.0f / sys_clock_hw_cycles_per_sec);

        t_last = k_cycle_get_32();
        image_split(fad->cam_buf.tmp, fad->network_input, fad->image_w, fad->image_h);
        t_now = k_cycle_get_32();
        delta = t_now - t_last;
        FACE_DEBUG("split %f\n", delta * 1.0f / sys_clock_hw_cycles_per_sec);

        fad->network_input[TEST_BUFFER_SIZE - 2] = 9; //label
        fad->network_input[TEST_BUFFER_SIZE - 1] = 1; //last
        if (fad->capture_started)
        {
            ui_clear_cam();
            fad->skip_camera_stream = 1;
            sprintf(dist_str, "cap %d/%d", fad->capture_backup.index + 1, fad->capture_backup.cap_cnt);
            ai_show_text(dist_str);
            lcd_draw_rectangle(res[index_], res[index_ + 1], res[index_] + w - 1, res[index_ + 1] + h - 1, WARN_COLOR);
            lcd_draw_rgb24(fad->network_input, 1, fad->text_area_y_start, fad->image_w, fad->image_h);
            if (fad->capture_backup.index < fad->capture_backup.cap_cnt)
            {
                memcpy(&fad->capture_backup.data[fad->capture_backup.index], fad->network_input, TEST_BUFFER_SIZE);
                fad->capture_backup.index += 1;
                k_busy_wait(50000);
                fad->skip_camera_stream = 0;
            }
            else //capture finished
            {
                ui_clear_cam();
                //show_capture(fad, fad->capture_backup.data, fad->capture_backup.ignore, 0, fad->capture_backup.index);
            }
        }
        else
        {
            if (fad->show_detected_face)
            {
                ui_clear_textarea(fad);
                lcd_draw_rectangle(res[index_], res[index_ + 1], res[index_] + w - 1, res[index_ + 1] + h - 1, WARN_COLOR);
                lcd_draw_rgb24(fad->network_input, 1, fad->text_area_y_start, fad->image_w, fad->image_h);
            }
            k_sem_give(&fad->predict_lock);
        }
    }
}
static void face_detect_thread(void *p1, void *p2, void *p3)
{
    FaceAIData *fad = (FaceAIData *)p1;
    FACE_DEBUG("face detect thread start!");
    while (1)
    {
        if (fad->frame_ready)
        {
            face_detect(fad);
            //lcd_draw_rectangle(res[index_ + 1],res[index_], res[index_ + 2],res[index_ + 2]);
            fad->frame_ready = 0;
        }
        k_yield();
    }
}

/*
    bin struction:
    Low Bytes ---> High Bytes
    |------------------------------Header------------------------------|-------------data-----------|
    |MAGIC   |VER.|LabelCnt |LabelName0|LabelName1|...|LabelName[Cnt-1]|Pic0     |Pic1|Pic2|...|PicN|
    |8Bytes  |6B  |2B       |16B       |16B       |...|16B             |(w*h*3+2)|    |    |...|    |
    |ANGRAD__|1.0 |         |          |          |...|                |         |    |    |   |    |

    Data:
    |------------------------------Pic0--------------------------------------------------|
    |         w*h          |       w*h            |              w*h     |1B   |1B       |
    |RRRRRRRRRRRRRRRRRR...R|GGGGGGGGGGGGGGGGGG...G|BBBBBBBBBBBBBBBBBB...B|Label|Last Flag|
*/

int parse_base(FaceAIData *fad, char *content_bytes)
{
    char magic[8];
    char version[6];
    unsigned short label_cnt;
    int i = 0;
    int label_name_off = 16;
    int label_name_size = 16;
    int data_off;

    memcpy(magic, content_bytes, 8);
    memcpy(version, content_bytes + 8, 6);
    memcpy(&label_cnt, content_bytes + 14, 2);
    FACE_DEBUG("magic:%s version:%s label_cnt:%d\n", magic, version, label_cnt);

    for (i = 0; i < label_cnt; i++)
    {
        fad->person_name[i] = (char *)malloc_ext(0, 16);
        fad->person_cnt++;
        memcpy(fad->person_name[i], content_bytes + label_name_off, label_name_size);
        FACE_DEBUG("%s\n", fad->person_name[i]);
        label_name_off += label_name_size;
    }
    data_off = 16 + label_cnt * 16;

    for (i = 0; i < label_cnt; i++)
    {
        FACE_DEBUG("%d: %s\n", i, fad->person_name[i]);
    }

    return data_off;
}


int import_record(struct easynet_dev *dev, FaceAIData *fad, int backup_base)
{
    int label = 0;
    int last  = 0;
    static int m = 0;
    struct easynet_dev *dev1;
    node_t *currentNode;
    label = fad->network_input[TEST_BUFFER_SIZE - 2];
    last  = fad->network_input[TEST_BUFFER_SIZE - 1];
    FACE_DEBUG("label = %d\n\r", label);
    if (backup_base && (fad->base_backup.index < RECORD_BACKUP_CNT))
    {
        FACE_DEBUG("base_backup (%d/%d)\n", fad->base_backup.index + 1, RECORD_BACKUP_CNT);
        memcpy(&fad->base_backup.data[fad->base_backup.index], fad->network_input, TEST_BUFFER_SIZE);
        fad->base_backup.index += 1;
    }
    //lcd_draw_rgb24(fad->network_input, 1, fad->text_area_y_start, fad->image_w, fad->image_h);

    if (0)
    {
        //dev->input = (unsigned char *)&face_ai_img_data[0];
        predict_new(dev);
    }
    else
    {
        dev1 = easynet_process_init(faceai.wei_addr);
        wait_data_ready(dev1, fad);
        predict_new(dev1);
    }

    AveragePooling(OUTPUT_WIDTH, OUTPUT_HEIGHT, FC_CHANNEL, OUTPUT_WIDTH, OUTPUT_HEIGHT);

    if (fad->previous_node == NULL)
    {
        fad->previous_node = malloc_ext(0, sizeof(node_t));
        if (fad->previous_node == NULL)
        {
            FACE_DEBUG("Failed to malloc_ext\b\r");
            return (-1);
        }
        fad->base_head = fad->previous_node;
        currentNode = fad->base_head;
    }
    else
    {
        currentNode = malloc_ext(0, sizeof(node_t));
        if (currentNode == NULL)
        {
            FACE_DEBUG("Failed to malloc_ext\b\r");
            return (-1);
        }
        fad->previous_node->next = currentNode;
    }
    currentNode->label = label;
    currentNode->next = NULL;
    float *fcArray = malloc_ext(0, sizeof(float) * FC_OUT);

    //  uint32_t t_last, t_now;
    //  t_last = k_uptime_get_32();
    FC(fcArray);
    //t_now = k_uptime_get_32();
    //printf("FC consume %d ms \n", t_now - t_last);

    currentNode->baseVector = fcArray;
    fad->previous_node = currentNode;
    fad->total_imgs_cnt++;
    fad->person_img_cnt[label]++;
    return last;
}

int delete_record(FaceAIData *fad, int label)
{
    node_t *cur = fad->base_head;
    node_t *pre = NULL, *next = NULL;
    int cnt = 0;

    while (cur != NULL)
    {
        next = cur->next;
        if (cur->label == label)
        {
            if (pre == NULL)
            {
                fad->base_head = next;
            }
            else
            {
                pre->next = next;
            }
            if (cur->baseVector)
            {
                free_ext(0, cur->baseVector);
                cur->baseVector = NULL;
            }
        }
        if (cur->label == label)
        {
            cnt++;
            free_ext(0, cur);
        }
        else
        {
            pre = cur;
        }
        cur = next;
    }
    fad->total_imgs_cnt -= cnt;
    fad->person_img_cnt[label] -= cnt;
    if (cnt > 0)
    {
        fad->person_cnt -= 1;
    }
    FACE_DEBUG("label [%d] has been deleted %d vectors\n", label, cnt);
    return cnt;
}

int calc_distance(FaceAIData *fad)
{
    node_t *currentNode = fad->base_head;
    float prev_sum = fad->threshold * 1.0;
    float min = 1000.0;   // since other 128D has been normalized, 1000 should be sufficiently large
    float sum = 0;
    int i = 0;
    int j = 0;
    FACE_DEBUG("~~~THRESHOLD: %f FC_CHANNEL:%d\n", prev_sum, FC_CHANNEL);
    int currentLable = 0;

    int per_f = 0;
    float sum_dist = 0;
    int record_cnt = 0;
    int person_freq = 0;
    int person_label = fad->person_cnt;
    int buf_size = fad->total_imgs_cnt;

    for (i = 0; i < buf_size; i++)
    {
        fad->dist.index[i] = i;
    }
    for (i = 0; i < MAX_PERSON_CNT; i++)
    {
        fad->dist.freq[i] = 0;
    }
    sum_dist = 0;

    while (currentNode != NULL)
    {
        sum = 0;
        for (i = 0; i < FC_OUT; ++i)
        {
            sum += pow(currentNode ->baseVector[i] - OutputVectorFC[i], 2);
        }
        sum = sqrt(sum);
        fad->dist.dist[j] = sum;
        fad->dist.label[j] = currentNode->label;
        j++;
        if (sum < min)
        {
            min = sum;
            currentLable = currentNode->label;
        }
        if (fad->debug_print)
        {
            FACE_DEBUG("current base: %d\tand dist: %f\n", currentNode ->label, sum);
        }
        currentNode = currentNode ->next;
    }

    /* calc aver */
    for (i = 0; i < fad->person_cnt; i++)
    {
        for (j = 0; j < buf_size; j++)
        {
            if (fad->dist.label[j] == i)
            {
                sum_dist += fad->dist.dist[j];
                per_f++;
            }
        }
        fad->dist.dist_aver[i] = sum_dist / per_f;
    }

    quicksort(fad->dist.dist, fad->dist.index, buf_size, 0, buf_size - 1);

    fad->dist.dist_min = fad->dist.dist[0];

    /* get frequency */
    for (i = 0; i < buf_size && record_cnt < DIST_MAX_CMP_CNT; i++)
    {
        if (fad->dist.dist[i] < fad->threshold)
        {
            fad->dist.freq[fad->dist.label[fad->dist.index[i]]]++;
            record_cnt++;
        }
    }

    if (fad->debug_print)
    {
        FACE_DEBUG("average distance:\n");
        print_float(fad->dist.dist_aver, fad->person_cnt);

        FACE_DEBUG("Sorted distance:\n");
        print_float(fad->dist.dist, buf_size);

        FACE_DEBUG("Corresponding index:\n");
        for (i = 0; i < buf_size; i++)
        {
            FACE_DEBUG("%4d ", fad->dist.label[fad->dist.index[i]]);
        }
        FACE_DEBUG("\n");

        FACE_DEBUG("Corresponding number of occurrences:\n");
        print_int(fad->dist.freq, fad->person_cnt);
    }

    for (i = 0; i < fad->person_cnt; i++)
    {
        /* Occurence frequency as first comparision */
        if ((fad->dist.freq[i] > person_freq))
        {
            if (person_label == fad->person_cnt) //index == 0
            {
                person_freq = fad->dist.freq[i];
                person_label = i;
            }
            else
            {
                if (fad->dist.freq[i] - person_freq > (DIST_MAX_CMP_CNT / 2))
                {
                    person_freq = fad->dist.freq[i];
                    person_label = i;
                }
                else if (fad->dist.dist_aver[i] < fad->dist.dist_aver[person_label])
                {
                    person_freq = fad->dist.freq[i];
                    person_label = i;
                }
            }
        }
        /* compare average value secondarily */
        else if (fad->dist.freq[i] == person_freq)
        {
            if (fad->dist.dist_aver[i] < fad->dist.dist_aver[person_label])
            {
                person_freq = fad->dist.freq[i];
                person_label = i;
            }
        }
    }
    if (person_label == fad->person_cnt)
    {
        FACE_DEBUG("unknown person index %d freq: %d\n", person_label, person_freq);
    }
    else
    {
        FACE_DEBUG("person[%s] index %d freq: %d\n", fad->person_name[person_label], person_label, person_freq);
    }
#if 0
    if (min >= fad->threshold)
    {
        currentLable = maxBase + 1;
        FACE_DEBUG("UNKOWN Person\n");
    }
    else
    {
        FACE_DEBUG("This Person is %d\n", currentLable);
    }
#endif
    return (person_label);
}

void camera_isr(struct device *dev, uint32_t line_num)
{
    int ret = 0;
    struct dcmi_buf dis_buf;
    FaceAIData *fad = &faceai;
#ifdef CAPTURE_JPEG
    //dcmi_stop_streaming(fad->cam_dev);
#endif

    if (!fad->skip_camera_stream)
    {
        ret = dcmi_ioctl(fad->cam_dev, VIDIOC_DQBUF, &dis_buf);
        if (ret == 0)
        {
#if 0
            if (start_crop_flag)
            {
                image_crop((uint8_t *)dis_buf.addr, CAM_CROP_W);
                fad->network_input[TEST_BUFFER_SIZE - 2] = 9; //label
                fad->network_input[TEST_BUFFER_SIZE - 1] = 1; //last
                start_crop_flag = 0;
            }
#endif
            if (!fad->frame_ready)
            {
                memcpy(fad->cam_buf.detected, (void *)dis_buf.addr, CAM_CROP_SIZE);
                memcpy(fad->cam_buf.tmp, fad->cam_buf.detected, CAM_CROP_SIZE);
                fad->frame_ready = 1;
            }
#ifdef USE_IMP
            ai_imp_scaleup((uint32_t)dis_buf.addr, (uint32_t)fad->cam_buf.scaleup, CAM_CROP_W, CAM_CROP_H, CAM_DISP_W, CAM_DISP_H);
            fb_ioctl(fad->lcd_dev, 0, FB_X_SETADDR, &(fad->cam_buf.scaleup));
#else
            fb_ioctl(fad->lcd_dev, 0, FB_X_SETADDR, &(dis_buf.addr));
#endif
            dcmi_ioctl(fad->cam_dev, VIDIOC_QBUF, &dis_buf);
        }
    }
}

void ai_init_data(FaceAIData *fad)
{
    int i = 0;
    fad->person_name = recorded_person_name;
    for (i = 0; i < MAX_PERSON_CNT; i++)
    {
        fad->person_img_cnt[i] = 0;
    }
    fad->person_cnt         = 0;
    fad->total_imgs_cnt     = 0;
    fad->skip_camera_stream = 0;
    fad->frame_ready        = 0;
    fad->capture_started    = 0;
    fad->network_predict    = 1;
    fad->show_detected_face = 1;
    fad->threshold          = THRESHOLD;
    fad->base_head          = NULL;
    fad->previous_node      = NULL;

    fad->cam_buf.tmp        = malloc_ext(0, CAM_CROP_SIZE);
    fad->cam_buf.cropped    = malloc_ext(0, CAM_CROP_SIZE);
    fad->cam_buf.detected   = malloc_ext(0, CAM_CROP_SIZE);
    fad->cam_buf.scaleup    = malloc_ext(0, CAM_DISP_SIZE);

    k_sem_init(&fad->predict_lock, 0, UINT_MAX);

    fad->base_backup.index      = 0;
    fad->capture_backup.index   = 0;
    fad->validate_backup.index  = 0;
    fad->capture_backup.cap_cnt = CAPTURE_BACKUP_CNT;

    fad->network_input          = INPUT_BASE;
    fad->base_backup.data       = RECORD_BASE_BACKUP;
    fad->capture_backup.data    = CAPTURE_BACKUP;
    fad->validate_backup.data   = VALIDATE_BACKUP;
    fad->wei_addr               = WEI_BASE;
    fad->debug_print            = 0;

    fad->thumb_w            = CAPTURE_SMALL_W;
    fad->thumb_h            = CAPTURE_SMALL_H;
    fad->image_w            = INPUT_WIDTH;
    fad->image_h            = INPUT_HEIGHT;
    fad->text_area_y_start  = CAM_CROP_H + 3;
    fad->ui_w               = LCD_UI_W;
    fad->ui_h               = LCD_UI_H;

}

void deinit(FaceAIData *fad)
{
    node_t *currentNode, *last;
    if (fad->cam_buf.tmp)
    {
        free_ext(0, fad->cam_buf.tmp);
        fad->cam_buf.tmp = NULL;
    }
    if (fad->cam_buf.cropped)
    {
        free_ext(0, fad->cam_buf.cropped);
        fad->cam_buf.cropped = NULL;
    }
    if (fad->cam_buf.detected)
    {
        free_ext(0, fad->cam_buf.detected);
        fad->cam_buf.detected = NULL;
    }
    currentNode = fad->base_head;
    while (currentNode != NULL)
    {
        last = currentNode;
        currentNode = currentNode ->next;
        if (last->baseVector)
        {
            free_ext(0, last->baseVector);
            last->baseVector = NULL;
        }
        free_ext(0, last);
        last = NULL;
    }
}

void ai_reading_callback(char *str)
{
    ai_show_text(str);
}

int ai_init_module(FaceAIData *fad)
{
    int ret = 0;

    fad->cam_dev = ai_init_camera(camera_isr);
    fad->lcd_dev = ai_init_lcd();
    lcd_clear();

#ifdef USE_IMP
    ai_init_imp();
#endif
    ai_init_storage();

    /*Initial the SRAM specified for KDP310 */
    memset((void *)0x20020000, 0x00, 256 * SIZE_1K);

    return ret;
}

int ai_init(FaceAIData *fad)
{
    int ret = 0;

    ret = ai_init_module(fad);
    if (ret)
    {
        FACE_DEBUG("%s failed\n", __FUNCTION__);
        return ret;
    }

    ai_init_data(fad);
    ai_init_shell(fad);

    return ret;
}

void predict_process(struct easynet_dev *dev, FaceAIData *fad)
{
    while (1)
    {
        int ret = 0;
        dev = easynet_process_init(faceai.wei_addr);
        ret = k_sem_take(&fad->predict_lock, 5000);
        if (ret == 0)
        {
            predict(dev, fad);
            while (!fad->network_predict)
            {
                k_busy_wait(1000000);
            }
        }
    }
}

static void ai_load_opsall_data(unsigned char *ops_data, FaceAIData *fad)
{
    int ret = ai_load_ops_data(OPS_BIN_FLASH_START, OPS_BIN_SIZE, IMAGE_BIN_FLASH_START, BASE_SIZE_RECODRD,  ops_data, (unsigned char *)fad->base_backup.data);
}

void import_base(struct easynet_dev *dev, FaceAIData *fad)
{
    int data_off;
    char *orig_base = NULL;
    orig_base = (unsigned char *)RECORD_BASE_BACKUP;

    ai_show_text("importing");

    data_off = parse_base(fad, orig_base);
    orig_base += data_off;
    while (1)
    {
        memcpy(fad->network_input, orig_base, TEST_BUFFER_SIZE);
        if (import_record(dev, fad, 0))
        {
            break;
        }
        orig_base += TEST_BUFFER_SIZE;
    }
}


void create_face_detect_thread(FaceAIData *fad)
{
    k_thread_create(&fad->task_thread, stack_area,
                    K_THREAD_STACK_SIZEOF(stack_area),
                    (k_thread_entry_t)face_detect_thread,  fad, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
}

void predict_new(struct easynet_dev *dev)
{
    int ret = 0;
    /* Process all ops */
    do
    {
        ret = easynet_ops_process(dev);

        if (ret == 0x06 || ret == 0x01)
        {
            if (ret == 0x06)
            {
                dev = easynet_process_init(faceai.wei_addr);
            }
            easynet_process_update(faceai.wei_addr, &inputImage[0], &OutputImage[0]);
            break;
        }
    } while (1);
}

void wait_data_ready(struct easynet_dev *dev, FaceAIData *fad)
{
    dev->input = fad->network_input;
}


int predict(struct easynet_dev *dev, FaceAIData *fad)
{
    int time = 3;
    time = 2;
    uint32_t color;
    char *name = UNKNOWN_PERSON;
    char dist_str[32];
    unsigned int t_last, t_now, delta;
    int m = 0;
    FACE_DEBUG("start validating :\n");
    t_last = k_cycle_get_32();
    memcpy(&fad->validate_backup.data[fad->validate_backup.index], fad->network_input, TEST_BUFFER_SIZE);

    fad->validate_backup.index += 1;
    if (fad->validate_backup.index >= VALIDATE_BACKUP_CNT)
    {
        fad->validate_backup.index = 0;
    }

    wait_data_ready(dev, fad);

    int ret = 0;
    /* Process all ops */
    do
    {
        ret = easynet_ops_process(dev);

        if (ret == 0x06 || ret == 0x01)
        {
            if (ret == 0x06)
            {
                FACE_DEBUG("debug end xx\n");
                //                              easynet_ops_process(dev);
                AveragePooling(OUTPUT_WIDTH, OUTPUT_HEIGHT, FC_CHANNEL, OUTPUT_WIDTH, OUTPUT_HEIGHT);
                FC((float *)&OutputVectorFC);
                int pred = calc_distance(fad);
                color = 0xFF00FF00;
                name = fad->person_name[pred];
                if (pred == fad->person_cnt)
                {
                    color = 0xFFFF0000;
                    name = UNKNOWN_PERSON;
                }
                t_now = k_cycle_get_32();
                delta = t_now - t_last;
                FACE_DEBUG("validate net time %f\n", delta * 1.0f / sys_clock_hw_cycles_per_sec);
                FACE_DEBUG(" finished   xxx\n");
                sprintf(dist_str, "%f", fad->dist.dist_min);
                ui_clear_cam();
                time = 1;
                while (time > 0)
                {
                    lcd_draw_text(fad->image_w, fad->text_area_y_start, 200, 24, 24, dist_str, color);
                    lcd_draw_text(fad->image_w, fad->text_area_y_start + 24, 200, 24, 24, name, color);
                    time--;
                }

                easynet_process_update(faceai.wei_addr, &inputImage[0], &OutputImage[0]);
            }

            break;
        }
    } while (1);

    return 0;
}

int start_ai_demo()
{
    struct easynet_dev *dev;

    int ret = 0;
    FaceAIData *fad = &faceai;

#ifdef USE_UARTPROTOCOL
    protocol_start();
#endif

    ret = ai_init(fad);

    ai_load_opsall_data(faceai.wei_addr, fad) ;
    dev = easynet_process_init(faceai.wei_addr);
    gdev = dev;

    //FC_Init();

    import_base(dev, fad);
    ai_show_text("predicting");
    create_face_detect_thread(fad);
    if (!faceai.wei_addr)
    {
        return -1;
    }

    dev = easynet_process_init(faceai.wei_addr);
    gdev = dev;
    predict_process(dev, fad);

    deinit(fad);

    return ret;
}


static int aimodule_init_device(void)
{
#if 1

    *(volatile uint32_t *)(0x1FFFF0B0) &= ~(1 << 18);

    HAL_Reset_Module(RESET_STF_SW_RSTJ);
    HAL_Reset_Module(RESET_KDPSRAM_SW_RSTJ);//*(volatile uint32_t *)(0x40000090) |= 3 << 0;  /*fast kdp run 300MHZ*/
    HAL_Reset_Module(RESET_KDP_SW_RSTJ);
    HAL_Reset_Module(RESET_IMP_SW_RSTJ);

    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_STF);
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_KDPSRAM);
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_KDP);
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_IMP);

    /*improve system powerformance*/
    *(volatile uint32_t *)(0x40039288) |= 0x3F << 16;

    *(volatile uint32_t *)(0x40000188) |= 1 << 0;

    HAL_CLOCK_AdjustPLLKDPDefaultDiv(2, 0, 0, 0);

    while ((*(volatile uint32_t *)(0x400001A4) & (1 << 5)) != (1 << 5));

    //*(volatile uint32_t *)(0x4003928C) = 0x00666606;
    /*
        6.4.    SDRAM Master AHB Bus Control Register (SDR_BC)

        Master1(VIN) AHB access Attribute
        Bit10: BL1_RD_PREFETCH_EN: Prefetch 16 bytes for burst=1 request.
        Bit9: BL4_RD_PREFETCH_EN: Prefetch 16x4 bytes for burst=4 request.
        Bit8: Reserved
    */

    *(volatile uint32_t *)(0x4003928C) &= ~0xF0;


    *(volatile uint32_t *)(0x4000009c) |= 2 << 16;
#else

    /*

    FCLK_SEL: The free running clock source selection bit.
    If the clock source which you want to switch is not ready, FCLK will still run in the quondam source, and the more information can be got from FCLK_SEL_SHD.
    3'B000: 8MHz of HIRC
    3'B001: 8MHz of HOSC
    3'B010: 150MHz of PLLKDP
    3'B011: 180~216MHz of PLL
    3'B100: 100MHz of PLLKDP
    3'B101: 172MHz of PLLSAI
    Default value¡êo3'B000

    */

    /*enable a stable CPU clock*/
    *(volatile uint32_t *)(0x40000090) |= 2 << 0;

    /*power on kdp310 and reset*/
    *(volatile uint32_t *)(0x1FFFF0B0) &= ~(1 << 18);
    *(volatile uint32_t *)(0x40000070) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;
    *(volatile uint32_t *)(0x40000080) |= 1 << 14 | 1 << 15 | 1 << 16 | 1 << 17;

    /*improve system powerformance*/
    *(volatile uint32_t *)(0x40039288) |= 0x3F << 16;

    *(volatile uint32_t *)(0x40000188) |= 1 << 0;


    while ((*(volatile uint32_t *)(0x400001A4) & (1 << 5)) != (1 << 5));
    /*

    D_PLLKDP_DIVP: The PLLKDP output clock divider P selection:
    8'h00~01: NA
    8'h02~FF: divider by 2~255
    Default value¡êo8'b00000010
    */
    *(volatile uint32_t *)(0x4000018C) |= 3;

    //*(volatile uint32_t *)(0x4003928C) = 0x00666606;
    *(volatile uint32_t *)(0x4003928C) &= ~0xF0;

    /*
    KDP_CLK_SEL:
    KDP clock source setting
    2'B00: sourced by system clock;
    2'B01: sourced by 180~216MHz of PLL clock;
    2'B10: sourced by PLLKDP 300MHz clock;
    2'B11: reserved;
    */

    *(volatile uint32_t *)(0x4000009c) |= 1 << 16;
#endif

    memset(inputImage, 0, BUFFERSIZE * sizeof(unsigned short));
    memset(OutputImage, 0, BUFFERSIZE * sizeof(unsigned short));
}


int main(void)
{
    int ret = 0;


    aimodule_init_device();

    ret = start_ai_demo();

    return ret;
}
