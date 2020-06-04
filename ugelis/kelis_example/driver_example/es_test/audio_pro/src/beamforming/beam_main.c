
#include <ugelis.h>
#include <fs/fs_interface.h>
#include "fs.h"
#include "string.h"
#include <gm_hal_crypto.h>
#include <string.h>
#include <assert.h>

#include "utils.h"
//#include "fft.h"
#include "sys_beam.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../jerry-libm/math.h"
#include "sys_beam.h"


//extern uint32_t malloc_ext(uint8_t memx,uint32_t size);
//extern void free_ext(uint8_t memx,void *ptr);
//#define malloc(size)  malloc_ext(0, size)
//#define free(ptr)    free_ext(0, ptr)

#define GM6721_AUDIO_FILE_1 "0:channel_1.wav"
#define GM6721_AUDIO_FILE_2 "0:channel_2.wav"
#define GM6721_AUDIO_FILE_3 "0:channel_3.wav"
#define GM6721_AUDIO_FILE_4 "0:channel_4.wav"

//struct fs_file_t filep_hdl_tmp;

//Fast Fourier Transform
//=======================

// #define M_PI 3.141592653589793238462643383279502
static void make_sintbl(int n, float *sintbl)
{
    int i, n2, n4, n8;
    float c, s, dc, ds, t;

    n2 = n / 2;
    n4 = n / 4;
    n8 = n / 8;
    t = sin(M_PI / n);
    dc = 2 * t * t;
    ds = sqrt(dc * (2 - dc));
    t = 2 * dc;
    c = sintbl[n4] = 1;
    s = sintbl[0] = 0;
    for (i = 1; i < n8; i++)
    {
        c -= dc;
        dc += t * c;
        s += ds;
        ds -= t * s;
        sintbl[i] = s;
        sintbl[n4 - i] = c;
    }
    if (n8 != 0)
    {
        sintbl[n8] = sqrt(0.5);
    }
    for (i = 0; i < n4; i++)
    {
        sintbl[n2 - i] = sintbl[i];
    }
    for (i = 0; i < n2 + n4; i++)
    {
        sintbl[i + n2] = -sintbl[i];
    }
}

static void make_bitrev(int n, int *bitrev)
{
    int i, j, k, n2;

    n2 = n / 2;
    i = j = 0;
    for (;;)
    {
        bitrev[i] = j;
        if (++i >= n)
        {
            break;
        }
        k = n2;
        while (k <= j)
        {
            j -= k;
            k /= 2;
        }
        j += k;
    }
}

//x:实部  y:虚部  n：fft长度
int fft(float *x, float *y, int n)
{
    static int    last_n = 0;     /* previous n */
    static int    *bitrev = NULL; /* bit reversal table */
    static float *sintbl = NULL; /* trigonometric function table */
    int i, j, k, ik, h, d, k2, n4, inverse;
    float t, s, c, dx, dy;

    /* preparation */
    if (n < 0)
    {
        n = -n;
        inverse = 1; /* inverse transform */
    }
    else
    {
        inverse = 0;
    }
    n4 = n / 4;
    if (n != last_n || n == 0)
    {
        last_n = n;
        if (sintbl != NULL)
        {
            free(sintbl);
        }
        if (bitrev != NULL)
        {
            free(bitrev);
        }
        if (n == 0)
        {
            return 0;    /* free the memory */
        }
        sintbl = (float *)malloc((n + n4) * sizeof(float));
        bitrev = (int *)malloc(n * sizeof(int));
        if (sintbl == NULL || bitrev == NULL)
        {
            //Error("%s in %f(%d): out of memory\n", __func__, __FILE__, __LINE__);
            return 1;
        }
        make_sintbl(n, sintbl);
        make_bitrev(n, bitrev);
    }

    /* bit reversal */
    for (i = 0; i < n; i++)
    {
        j = bitrev[i];
        if (i < j)
        {
            t = x[i];
            x[i] = x[j];
            x[j] = t;
            t = y[i];
            y[i] = y[j];
            y[j] = t;
        }
    }

    /* transformation */
    for (k = 1; k < n; k = k2)
    {
        h = 0;
        k2 = k + k;
        d = n / k2;
        for (j = 0; j < k; j++)
        {
            c = sintbl[h + n4];
            if (inverse)
            {
                s = -sintbl[h];
            }
            else
            {
                s = sintbl[h];
            }
            for (i = j; i < n; i += k2)
            {
                ik = i + k;
                dx = s * y[ik] + c * x[ik];
                dy = c * y[ik] - s * x[ik];
                x[ik] = x[i] - dx;
                x[i] += dx;
                y[ik] = y[i] - dy;
                y[i] += dy;
            }
            h += d;
        }
    }
    if (inverse)
    {
        /* divide by n in case of the inverse transformation */
        for (i = 0; i < n; i++)
        {
            x[i] /= n;
            y[i] /= n;
        }
    }
    return 0;  /* finished successfully */
}

int check_file_dir_exists(const char *path)
{
    int res;
    struct fs_dirent entry;

    /* Verify fs_stat() */
    res = fs_stat(path, &entry);

    return !res;
}



#if 1
/*65536 - 44*/
#define DATA_UNM  65492/2
#define NUM_CHANNEL 4

#define SEEK_SET 0

float *delay_data1 = NULL;
float *delay_data2 = NULL;
float *delay_data3 = NULL;
float *delay_data4 = NULL;
float *out         = NULL;
int wav_file_len   = 0;

void get_all_channel_data()
{
#if 0
    struct fs_file_t *fp_read  = NULL;
    int read_len   = 0;
    int cnt = 0;
    int res = 0;

    /*test*/
    if (check_file_dir_exists(GM6721_AUDIO_FILE_1))
    {
        printf("Opening existing file %s\n", GM6721_AUDIO_FILE_1);
    }
    else
    {
        printf("Creating new file %s\n", GM6721_AUDIO_FILE_1);
    }



    /*file 1*/
    //fp_read = fs_open("F:\\workspace\\audio_prj\\data\\16mic_data\\lowSNRdata\\yes1_0db_noise_wgn.wav", "rb");
    res = fs_open(fp_read, GM6721_AUDIO_FILE_1);
    if (res)
    {
        printf("Failed opening file [%d]\n", res);
        return res;
    }

    if (fp_read == NULL)
    {
        printf("Open file fail");
    }
    fs_seek(fp_read, 44, SEEK_SET);

    int i          = 0;
    short sample   = 0;
    for (i = 0; i < DATA_UNM; i ++)
    {
        read_len = fs_read(fp_read, &sample, 2);
        //read_len = fs_read(&sample,1,sizeof(short),fp_read);
        delay_data1[i] = (float)sample;
    }
    fs_seek(fp_read, 0, SEEK_SET); //finish read file once, return to start
    fs_close(fp_read);

    /*file 2*/
    i          = 0;
    sample     = 0;

    //fp_read = fs_open("F:\\workspace\\audio_prj\\tdoa_test\\yes2_5db_noise_wgn.wav", "rb");
    //fp_read = fs_open("F:\\workspace\\audio_prj\\data\\16mic_data\\lowSNRdata\\yes2_0db_noise_wgn.wav", "rb");
    res = fs_open(fp_read, GM6721_AUDIO_FILE_2);
    if (res)
    {
        printf("Failed opening file [%d]\n", res);
        return res;
    }
    if (fp_read == NULL)
    {
        printf("Open file fail");
    }
    fs_seek(fp_read, 44, SEEK_SET);
    for (i = 0; i < DATA_UNM; i ++)
    {
        read_len = fs_read(fp_read, &sample, 2);
        //read_len = fs_read(&sample,1,sizeof(short),fp_read);
        delay_data2[i] = (float)sample;
    }
    fs_seek(fp_read, 0, SEEK_SET); //finish read file once, return to start
    fs_close(fp_read);

    /*file 3*/
    i          = 0;
    sample     = 0;

    //fp_read = fs_open("F:\\workspace\\audio_prj\\tdoa_test\\yes3_5db_noise_wgn.wav", "rb");
    //fp_read = fs_open("F:\\workspace\\audio_prj\\data\\16mic_data\\lowSNRdata\\yes3_0db_noise_wgn.wav", "rb");
    res = fs_open(fp_read, GM6721_AUDIO_FILE_3);
    if (res)
    {
        printf("Failed opening file [%d]\n", res);
        return res;
    }
    if (fp_read == NULL)
    {
        printf("Open file fail");
    }
    fs_seek(fp_read, 44, SEEK_SET);
    for (i = 0; i < DATA_UNM; i ++)
    {
        read_len = fs_read(fp_read, &sample, 2);
        //read_len = fs_read(&sample,1,sizeof(short),fp_read);
        delay_data3[i] = (float)sample;
    }
    fs_seek(fp_read, 0, SEEK_SET); //finish read file once, return to start
    fs_close(fp_read);

    /*file 4*/
    i          = 0;
    sample     = 0;

    //fp_read = fs_open("F:\\workspace\\audio_prj\\tdoa_test\\yes4_5db_noise_wgn.wav", "rb");
    //fp_read = fs_open("F:\\workspace\\audio_prj\\data\\16mic_data\\lowSNRdata\\yes4_0db_noise_wgn.wav", "rb");
    res = fs_open(fp_read, GM6721_AUDIO_FILE_4);
    if (res)
    {
        printf("Failed opening file [%d]\n", res);
        return res;
    }
    if (fp_read == NULL)
    {
        printf("Open file fail");
    }
    fs_seek(fp_read, 44, SEEK_SET);
    for (i = 0; i < DATA_UNM; i ++)
    {
        read_len = fs_read(fp_read, &sample, 2);
        //read_len = fs_read(&sample,1,sizeof(short),fp_read);
        delay_data4[i] = (float)sample;
    }
    fs_seek(fp_read, 0, SEEK_SET); //finish read file once, return to start
    fs_close(fp_read);

#endif
}


void save_beamform_data()
{
#if 0
    /*file op*/
    struct fs_file_t *fp_write = NULL;
    struct fs_file_t *fp_read  = NULL;
    char tmp[1024] = {0};

    int i          = 0;
    short sample     = 0;

    /*file 1*/

#if 1
    fp_read = fs_open("F:\\workspace\\audio_prj\\data\\16mic_data\\lowSNRdata\\yes1_0db_noise_wgn.wav", "rb");
    fp_write = fs_open("F:\\workspace\\audio_prj\\data\\16mic_data\\lowSNRdata\\yes1_0db_noise_wgn_gsc.wav", "wb+");
#endif


    if ((fp_write == NULL) || (fp_read == NULL))
    {
        printf("Open file fail");
    }

    /*copy wav header*/

    int cnt = 0;
    /*read wav header*/
    int read_len = fs_read(fp_read, tmp, 44);
    //int read_len = fs_read(tmp,1,44,fp_read);
    if (read_len != 44)
    {
        printf("read file fail");
        return;
    }

    /*write wav header*/
    fs_write(fp_write, tmp, 44);
    //fs_write(tmp,1,44,fp_write);
    cnt = cnt + 44;


    for (i = 0; i < DATA_UNM; i++)
    {
        sample = (short)out[i];
        fs_write(fp_write, &sample, 2);
        //fs_write(&sample,1,sizeof(short),fp_write);
    }
    fs_close(fp_read);
    fs_close(fp_write);
#endif
}


// Reference:
// 1. Microphone Array Signal Processing(chappter 9: Direction-of-Arrival and Time-Difference-of-Arrival Estimation)

// Calc tdoa(time delay of arrival
// using GCC-PHAT(Gerneral Cross Correlation - Phase Transform)
// @params data : in format channel0, channel1
// @params ref : reference_channel
// @params margin: margin [-tao, tao]
void GccPhatTdoa(const float *data, int num_channel, int num_sample,
                 int ref, int margin, int *tdoa)
{
    assert(data != NULL);
    assert(ref >= 0 && ref < num_channel);
    assert(margin <= num_sample / 2);
    // constrait the number data points to 2^n
    int num_points = UpperPowerOfTwo(num_sample);
    int half = num_points / 2;
    //float *win_data = (float *)calloc(sizeof(float), num_points * num_channel);
    float *win_data = (float *)malloc(num_points * num_channel * 4);

    // copy data and apply window
    for (int i = 0; i < num_channel; i++)
    {
        memcpy(win_data + i * num_points, data + i * num_sample,
               sizeof(float) * num_sample);
        Hamming(win_data, num_sample);
    }

    //float *fft_real = (float *)calloc(sizeof(float), num_points * num_channel);
    float *fft_real = (float *)malloc(num_points * num_channel * 4);

    //float *fft_img = (float *)calloc(sizeof(float), num_points * num_channel);
    float *fft_img = (float *)malloc(num_points * num_channel * 4);
    // do fft
    for (int i = 0; i < num_channel; i++)
    {
        memcpy(fft_real + i * num_points, win_data + i * num_points,
               sizeof(float) * num_points);
        fft(fft_real + i * num_points, fft_img + i * num_points, num_points);
    }

    //float *corr_real = (float *)calloc(sizeof(float), num_points * num_channel);
    float *corr_real = (float *)malloc(num_points * num_channel * 4);

    //float *corr_img = (float *)calloc(sizeof(float), num_points * num_channel);
    float *corr_img = (float *)malloc(num_points * num_channel * 4);

    // do gcc-phat
    for (int i = 0; i < num_channel; i++)
    {
        if (i != ref)
        {
            // * do fft cross correlation, fft(i) fft(ref)*
            // (a + bj) (c + dj)* = (a + bj) (c - dj) = (ac + bd) + (bc - ad)j
            for (int j = 0; j < num_points; j++)
            {
                int m = ref * num_points + j, n = i * num_points + j;
                corr_real[n] = fft_real[n] * fft_real[m] + fft_img[n] * fft_img[m];
                corr_img[n] = fft_img[n] * fft_real[m] - fft_real[n] * fft_img[m];
                float length = sqrt(corr_real[n] * corr_real[n] +
                                    corr_img[n] * corr_img[n]);
                corr_real[n] /= length;
                corr_img[n] /= length;
            }
            // * do inserse fft
            fft(corr_real + i * num_points, corr_img + i * num_points, -num_points);
            // * rearange idft index(fftshift), make num_points / 2 as the center
            for (int j = 0; j < half; j++)
            {
                float t = corr_real[i * num_points + j];
                corr_real[i * num_points + j] = corr_real[i * num_points + j + half];
                corr_real[i * num_points + j + half] = t;
            }
            // * select max
            int max_j = half - margin;
            assert(max_j >= 0);
            float max = corr_real[i * num_points + max_j];
            for (int j = half - margin; j < half + margin; j++)
            {
                if (corr_real[i * num_points + j] > max)
                {
                    max = corr_real[i * num_points + j];
                    max_j = j;
                }
            }
            tdoa[i] = max_j - half;
        }
        else
        {
            tdoa[i] = 0;
        }
    }

    // free all
    free(win_data);
    free(fft_real);
    free(fft_img);
    free(corr_real);
    free(corr_img);
}

// Delay & Sum
// @params data : in format channel0, channel1
void DelayAndSum(const float *data, int num_channel, int num_sample,
                 int *tdoa, float *out)
{
    for (int i = 0; i < num_sample; i++)
    {
        int count = 0;
        float sum = 0.0;
        for (int j = 0; j < num_channel; j++)
        {
            if (i + tdoa[j] >= 0 && i + tdoa[j] < num_sample)
            {
                sum += data[j * num_sample + i + tdoa[j]];
                count++;
            }
        }
        assert(count > 0);
        out[i] = sum / count;
    }
}

/*tdoa, delay and sun test*/
int beam_demo()
{
    /*init fs, sd card*/
    //fs_init_ext(NULL, "0:"); /*using mmc 2: ; using usb 0: ;*/
    // When test this, first modify beamforming.h drop the hamming window

    /*get all data*/
    get_all_channel_data();
    //float *all_data = (float *)malloc(sizeof(float), wav_file_len * NUM_CHANNEL);
    float *all_data = (float *)malloc(wav_file_len * NUM_CHANNEL * 4);

    memcpy(all_data, delay_data1, sizeof(float) * wav_file_len);
    memcpy(all_data + wav_file_len, delay_data2, sizeof(float) * wav_file_len);
    memcpy(all_data + 2 * wav_file_len, delay_data3, sizeof(float) * wav_file_len);
    memcpy(all_data + 3 * wav_file_len, delay_data4, sizeof(float) * wav_file_len);

    /*cal tdoa of the 4 channels*/
    int tdoa[NUM_CHANNEL] = {0};
    GccPhatTdoa(all_data, NUM_CHANNEL, wav_file_len, 0, wav_file_len / 2, tdoa);

    for (int i = 0; i < NUM_CHANNEL; i++)
    {
        printf("%d\n", tdoa[i]);
    }

    /*cal output data, using delay and sum*/
    //out = (float *)malloc(sizeof(float), wav_file_len);
    out = (float *)malloc(wav_file_len * 4);

    DelayAndSum(all_data, NUM_CHANNEL, wav_file_len, tdoa, out);

    /*save output data to wav file*/
    save_beamform_data();
    free(delay_data1);
    free(delay_data2);
    free(delay_data3);
    free(all_data);
    free(out);
    printf("Done\n");
    return 0;


}

#endif
/*toda test*/
