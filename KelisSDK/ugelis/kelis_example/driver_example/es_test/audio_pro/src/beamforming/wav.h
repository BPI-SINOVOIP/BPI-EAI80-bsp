/* Created on 2016-08-15
 * Author: Zhang Binbin
 */

#ifndef WAV_H_
#define WAV_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sys_beam.h"

struct WavHeader
{
    char riff[4]; // "riff"
    unsigned int size;
    char wav[4];  // "WAVE"
    char fmt[4];  // "fmt "
    unsigned int fmt_size; //
    unsigned short format; //
    unsigned short channels;
    unsigned int sample_rate;
    unsigned int bytes_per_second; //
    unsigned short block_size;
    unsigned short bit;  //
    char data[4]; // "data"
    unsigned int data_size; //
};

class WavReader
{
public:
    WavReader(const char *filename)
    {
        FILE *fp = f_open(filename, "r");
        if (NULL == fp)
        {
            perror(filename);
            exit(1);
        }

        WavHeader header;
        f_read(&header, 1, sizeof(header), fp);
        if (header.fmt_size < 16)
        {
            printf("WaveData: expect PCM format data to have fmt chunk of at least size 16.\n");
            exit(1);
        }
        else if (header.fmt_size > 16)
        {
            int offset = 44 - 8 + header.fmt_size - 16;
            fseek(fp, offset, SEEK_SET);
            f_read(header.data, 8, sizeof(char), fp);
        }
        // check "riff" "WAVE" "fmt " "data"
        // only support one sub data chunk
        num_channel_ = header.channels;
        sample_rate_ = header.sample_rate;
        bits_per_sample_ = header.bit;
        int num_data = header.data_size / (bits_per_sample_ / 8);
        data_ = new float[num_data];
        num_sample_ = num_data / num_channel_;

        for (int i = 0; i < num_data; i++)
        {
            switch (bits_per_sample_)
            {
                case 8:
                {
                    char sample;
                    f_read(&sample, 1, sizeof(char), fp);
                    data_[i] = (float)sample;
                    break;
                }
                case 16:
                {
                    short sample;
                    f_read(&sample, 1, sizeof(short), fp);
                    data_[i] = (float)sample;
                    break;
                }
                case 32:
                {
                    int sample;
                    f_read(&sample, 1, sizeof(int), fp);
                    data_[i] = (float)sample;
                    break;
                }
                default:
                    fprintf(stderr, "unsupported quantization bits");
                    exit(1);
            }
        }
        fclose(fp);
    }

    int NumChannel() const
    {
        return num_channel_;
    }
    int SampleRate() const
    {
        return sample_rate_;
    }
    int BitsPerSample() const
    {
        return bits_per_sample_;
    }
    int NumSample() const
    {
        return num_sample_;
    }

    ~WavReader()
    {
        if (data_ != NULL)
        {
            delete data_;
        }
    }

    const float *Data() const
    {
        return data_;
    }

private:
    int num_channel_;
    int sample_rate_;
    int bits_per_sample_;
    int num_sample_; // sample points per channel
    float *data_;
};

class WavWriter
{
public:
    WavWriter(const float *data, int num_sample,
              int num_channel, int sample_rate,
              int bits_per_sample):
        data_(data), num_sample_(num_sample),
        num_channel_(num_channel),
        sample_rate_(sample_rate),
        bits_per_sample_(bits_per_sample) {}

    void Write(const char *filename)
    {
        FILE *fp = f_open(filename, "w");
        // init char 'riff' 'WAVE' 'fmt ' 'data'
        WavHeader header;
        char wav_header[44] =
        {
            0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00,
            0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
            0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x64, 0x61, 0x74, 0x61,
            0x00, 0x00, 0x00, 0x00
        };
        memcpy(&header, wav_header, sizeof(header));
        header.channels = num_channel_;
        header.bit = bits_per_sample_;
        header.sample_rate = sample_rate_;
        header.data_size = num_sample_ * num_channel_ * (bits_per_sample_ / 8);
        header.size = sizeof(header) - 8 + header.data_size;
        header.bytes_per_second = sample_rate_ * num_channel_ *
                                  (bits_per_sample_ / 8);
        header.block_size = num_channel_ * (bits_per_sample_ / 8);

        fwrite(&header, 1, sizeof(header), fp);

        for (int i = 0; i < num_sample_; i++)
        {
            for (int j = 0; j < num_channel_; j++)
            {
                switch (bits_per_sample_)
                {
                    case 8:
                    {
                        char sample = (char)data_[i * num_channel_ + j];
                        fwrite(&sample, 1, sizeof(sample), fp);
                        break;
                    }
                    case 16:
                    {
                        short sample = (short)data_[i * num_channel_ + j];
                        fwrite(&sample, 1, sizeof(sample), fp);
                        break;
                    }
                    case 32:
                    {
                        int sample = (int)data_[i * num_channel_ + j];
                        fwrite(&sample, 1, sizeof(sample), fp);
                        break;
                    }
                }
            }
        }
        fclose(fp);
    }
private:
    const float *data_;
    int num_sample_; // total float points in data_
    int num_channel_;
    int sample_rate_;
    int bits_per_sample_;
};

#endif
