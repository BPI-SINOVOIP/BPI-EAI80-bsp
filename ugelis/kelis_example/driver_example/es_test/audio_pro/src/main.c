#include <ugelis.h>
#include <gm_hal_uart.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>

/*sample rate 16k Hz*/
#define SAMP_FREQ 16000

/*100ms buffer for vad test*/
#define BUF_100MS_SIZE 1600
char buf_100ms_1[BUF_100MS_SIZE];
char buf_100ms_2[BUF_100MS_SIZE];

/*2300ms buffer, for original audio data from mics, 16000*2.3*/
#define BUF_2300MS_SIZE 36800
char buf_2300ms_ori_1[BUF_2300MS_SIZE];
char buf_2300ms_ori_2[BUF_2300MS_SIZE];

/*2300ms buffer, for beamform output data, 16000*2.3*/
char buf_2300ms_beam_out[BUF_2300MS_SIZE];

//extern int vad_init();
//extern int ns_init();
//extern int beamforming_init();
//extern int agc_init();

static int sys_init()
{
    //sai_input_init();//todo
    //sai_output_init();//todo
    //vad_init();
    //ns_init();
    //beamforming_init();
    //agc_init();
    //sai_vad_mfcc_init();
    return 0;
}



#define REC_THREAD_STACK_SIZE 512
#define VAD_THREAD_STACK_SIZE 512
#define BEAM_THREAD_STACK_SIZE 512
#define NS_THREAD_STACK_SIZE 512
#define AGC_THREAD_STACK_SIZE 512
#define WAV_THREAD_STACK_SIZE 512
K_THREAD_STACK_DEFINE(rec_thread_stack, REC_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(vad_thread_stack, VAD_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(beam_thread_stack, BEAM_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(ns_thread_stack, NS_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(agc_thread_stack, AGC_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(wav_thread_stack, WAV_THREAD_STACK_SIZE);

struct k_thread rec_thread_data;
struct k_thread vad_thread_data;
struct k_thread beam_thread_data;
struct k_thread ns_thread_data;
struct k_thread agc_thread_data;
struct k_thread wav_thread_data;
struct k_sem rec_sem;
struct k_sem vad_sem;
struct k_sem beam_sem;
struct k_sem ns_sem;
struct k_sem agc_sem;
struct k_sem wav_sem;

//#define CAN_DEFINE_MSGQ(name, size) K_MSGQ_DEFINE(name, sizeof(struct can_msg), size, 4)
//CAN_DEFINE_MSGQ(led_msgq, 2);
//CAN_DEFINE_MSGQ(str_msgq, 5);

/*
void thread_init()
{
    k_thread_spawn(record_stack_area, STACKSIZE, record_thread, NULL, NULL, NULL,
                   //K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    k_thread_spawn(vad_stack_area, STACKSIZE, vad_thread, NULL, NULL, NULL,
                  //K_PRIO_PREEMPT(3), 0, K_NO_WAIT);



    k_tid_t tx_tid = k_thread_create(&rec_thread_data, rec_thread_stack,
                 K_THREAD_STACK_SIZEOF(rec_thread_stack),
                 tx_thread,
                 can_dev, NULL, NULL,
                 TX_THREAD_PRIORITY, 0, K_NO_WAIT);
    if (!tx_tid) {
    printk("ERROR spawning tx_thread\n");
    }

    k_tid_t led_tid = k_thread_create(&vad_thread_data, vad_thread_stack,
                  K_THREAD_STACK_SIZEOF(vad_thread_stack),
                  led_thread,
                  &led_msgq, can_dev, led_gpio_dev,
                  TX_THREAD_PRIORITY, 0, K_NO_WAIT);
    if (!led_tid) {
        printk("ERROR spawning led_thread\n");
    }

    k_tid_t str_tid = k_thread_create(&beam_thread_data,
                      beam_thread_stack,
                      K_THREAD_STACK_SIZEOF(beam_thread_stack),
                      rx_str_thread,
                      &str_msgq, can_dev, NULL,
                      TX_THREAD_PRIORITY, 0, K_NO_WAIT);
    if (!str_tid) {
        printk("ERROR spawning str_thread\n");
    }

    k_tid_t str_tid = k_thread_create(&ns_thread_data,
                      beam_thread_stack,
                      K_THREAD_STACK_SIZEOF(beam_thread_stack),
                      rx_str_thread,
                      &str_msgq, can_dev, NULL,
                      TX_THREAD_PRIORITY, 0, K_NO_WAIT);
    if (!str_tid) {
        printk("ERROR spawning str_thread\n");
    }

        k_tid_t str_tid = k_thread_create(&agc_thread_data,
                      beam_thread_stack,
                      K_THREAD_STACK_SIZEOF(beam_thread_stack),
                      rx_str_thread,
                      &str_msgq, can_dev, NULL,
                      TX_THREAD_PRIORITY, 0, K_NO_WAIT);
    if (!str_tid) {
        printk("ERROR spawning str_thread\n");
    }

        k_tid_t str_tid = k_thread_create(&wav_thread_data,
                      beam_thread_stack,
                      K_THREAD_STACK_SIZEOF(beam_thread_stack),
                      rx_str_thread,
                      &str_msgq, can_dev, NULL,
                      TX_THREAD_PRIORITY, 0, K_NO_WAIT);
    if (!str_tid) {
        printk("ERROR spawning str_thread\n");
    }
}
*/
//todo

//todo

int rec_process(unsigned int pcm_data_len)
{
    int pcm_len;
    while (1)
    {
        //todo
        pcm_len = wav_recorder_wav(buf_100ms_1, pcm_data_len);//wav_recorder_wav(buf_wav_pingpong[current_buf_index], pcm_data_len);
        //pcm_len = wav_recorder_wav(buf100ms_2, pcm_data_len);
    }
}

int rec_100ms()
{
    int len_100ms = 160 * 2;
    rec_process(len_100ms);

}

int rec_2100ms()
{
    int len_100ms = 160 * 2;
    rec_process(len_100ms);
}



int cal_tdoa()
{

}

int beamform()
{

}

int ns_process()
{

}

int agc_process()
{

}


int wav_file()
{

}

int main(void)
{
    int vad_ret = 0;

    sys_init();
    //thread_init();

    while (1)
    {
        // 1. rec 100ms data1, 100ms data 2
        rec_100ms();

        // 2. vad process
        vad_ret = vad_process(buf_100ms_1);
        if (vad_ret == 1)
        {
            rec_2100ms();
            break;
        }
    }

    while (1)
    {
        // 1. cal tdoa
        cal_tdoa();


        // 2. beamforming
        beamform();

        // 3. ns
        ns_process();


        // 4. agc
        agc_process();

        // 5. wav_file
        wav_file();

    }
}
