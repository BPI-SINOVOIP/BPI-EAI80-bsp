
/* public domain Simple, Minimalistic JPEG writer - http://jonolick.com
 *
 * Quick Notes:
 *  Based on a javascript jpeg writer
 *  JPEG baseline (no JPEG progressive)
 *  Supports 1, 3 or 4 component input. (luminance, RGB or RGBX)
 *
 * Latest revisions:
 *  1.52 (2012-22-11) Added support for specifying Luminance, RGB, or RGBA via comp(onents) argument (1, 3 and 4 respectively).
 *  1.51 (2012-19-11) Fixed some warnings
 *  1.50 (2012-18-11) MT safe. Simplified. Optimized. Reduced memory requirements. Zero allocations. No namespace polution. Approx 340 lines code.
 *  1.10 (2012-16-11) compile fixes, added docs,
 *      changed from .h to .cpp (simpler to bootstrap), etc
 *  1.00 (2012-02-02) initial release
 *
 * Basic usage:
 *  char *foo = new char[128*128*4]; // 4 component. RGBX format, where X is unused
 *  jo_write_jpg("foo.jpg", foo, 128, 128, 4, 90); // comp can be 1, 3, or 4. Lum, RGB, or RGBX respectively.
 *
 * */

#include <ugelis.h>
#include <stdio.h>
#include <stdlib.h>
#include "jpeg.h"
#include <stdint.h>
#include <kernel.h>
#include <misc/printk.h>
#include <device.h>
#include <gm_common.h>
#include <board.h>
#include <gpio.h>
#include <misc/util.h>

#define bool int
#define false 0
#define true 1

#define uint8_t  unsigned char
#define uint32_t  unsigned int

typedef struct
{
    int idx;
    int length;
    uint8_t *buf;
    int bitc, bitb;
    bool realloc;
    bool overflow;
} jpeg_buf_t;

typedef enum  jpeg_subsample
{
    JPEG_SUBSAMPLE_1x1 = 0x11,  // 1x1 chroma subsampling (No subsampling)
    JPEG_SUBSAMPLE_2x1 = 0x21,  // 2x2 chroma subsampling
    JPEG_SUBSAMPLE_2x2 = 0x22,  // 2x2 chroma subsampling
} jpeg_subsample_t;

float ceilf_c(float x)
{
    int n;
    float r;
    n = (int) x;
    r = (float) n;
    r = r + (x > r);
    return r;
}

float floorf_c(float x)
{
    int n;
    float r;
    n = (int) x;
    r = (float) n;
    r = r - (r > x);
    return r;
}

int inline fast_floorf(float x)
{
    int i;
#ifdef __USE_KEIL_
#pragma asm volatile ("vcvt.S32.f32  %[r], %[x]\n" : [r] "=t" (i) : [x] "t"  (x));
#pragma endasm
#else
    __asm__ volatile(
                    "vcvt.S32.f32  %[r], %[x]\n"
                    : [r] "=t"(i)
                    : [x] "t"(x));
#endif
    return i;
}

int inline fast_ceilf(float x)
{
    int i;
    x += 0.9999f;
#ifdef __USE_KEIL_
#pragma asm volatile ("vcvt.S32.f32  %[r], %[x]\n" : [r] "=t" (i) : [x] "t"  (x));
#pragma endasm
#else
    __asm__ volatile(
                    "vcvt.S32.f32  %[r], %[x]\n"
                    : [r] "=t"(i)
                    : [x] "t"(x));
#endif
    return i;
}

//#define debug_ceilf  fast_ceilf
//#define debug_floorf fast_floorf

#define debug_ceilf  ceilf_c   // ceilf
#define debug_floorf floorf_c // floorf
inline int jpeg_put_char(jpeg_buf_t *jpeg_buf, int c)
{
    jpeg_buf->buf[jpeg_buf->idx++] = c;

    return c;
}
inline int jpeg_put_bytes(jpeg_buf_t *jpeg_buf, const unsigned char *data, size_t size)
{
    memcpy(jpeg_buf->buf + jpeg_buf->idx, data, size);
    jpeg_buf->idx += size;

    return size;
}

// Quantization tables
float fdtbl_Y[64], fdtbl_UV[64];
unsigned char YTable[64], UVTable[64];

static const uint8_t s_jpeg_ZigZag[] =
{
    0,  1,   5,  6, 14, 15, 27, 28,
    2,  4,   7, 13, 16, 26, 29, 42,
    3,  8,  12, 17, 25, 30, 41, 43,
    9,  11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63
};

static const uint8_t YQT[] =
{
    16, 11, 10, 16, 24,  40,  51,  61,
    12, 12, 14, 19, 26,  58,  60,  55,
    14, 13, 16, 24, 40,  57,  69,  56,
    14, 17, 22, 29, 51,  87,  80,  62,
    18, 22, 37, 56, 68,  109, 103, 77,
    24, 35, 55, 64, 81,  104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};

static const uint8_t UVQT[] =
{
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};

static const float aasf[] = { 1.0f * 2.828427125f, 1.387039845f * 2.828427125f, 1.306562965f * 2.828427125f, 1.175875602f * 2.828427125f, 1.0f * 2.828427125f, 0.785694958f * 2.828427125f, 0.541196100f * 2.828427125f, 0.275899379f * 2.828427125f };


// Constants that don't pollute global namespace
static const unsigned char std_dc_luminance_nrcodes[] = {0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
static const unsigned char std_dc_luminance_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
static const unsigned char std_ac_luminance_nrcodes[] = {0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d};
static const unsigned char std_ac_luminance_values[] =
{
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
};

static const unsigned char std_dc_chrominance_nrcodes[] = {0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
static const unsigned char std_dc_chrominance_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
static const unsigned char std_ac_chrominance_nrcodes[] = {0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77};
static const unsigned char std_ac_chrominance_values[] =
{
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
};


// Huffman tables
static const unsigned short YDC_HT[256][2] = { {0, 2}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3}, {14, 4}, {30, 5}, {62, 6}, {126, 7}, {254, 8}, {510, 9}};
static const unsigned short UVDC_HT[256][2] = { {0, 2}, {1, 2}, {2, 2}, {6, 3}, {14, 4}, {30, 5}, {62, 6}, {126, 7}, {254, 8}, {510, 9}, {1022, 10}, {2046, 11}};
static const unsigned short YAC_HT[256][2] =
{
    {10, 4}, {0, 2}, {1, 2}, {4, 3}, {11, 4}, {26, 5}, {120, 7}, {248, 8}, {1014, 10}, {65410, 16}, {65411, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {12, 4}, {27, 5}, {121, 7}, {502, 9}, {2038, 11}, {65412, 16}, {65413, 16}, {65414, 16}, {65415, 16}, {65416, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {28, 5}, {249, 8}, {1015, 10}, {4084, 12}, {65417, 16}, {65418, 16}, {65419, 16}, {65420, 16}, {65421, 16}, {65422, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {58, 6}, {503, 9}, {4085, 12}, {65423, 16}, {65424, 16}, {65425, 16}, {65426, 16}, {65427, 16}, {65428, 16}, {65429, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {59, 6}, {1016, 10}, {65430, 16}, {65431, 16}, {65432, 16}, {65433, 16}, {65434, 16}, {65435, 16}, {65436, 16}, {65437, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {122, 7}, {2039, 11}, {65438, 16}, {65439, 16}, {65440, 16}, {65441, 16}, {65442, 16}, {65443, 16}, {65444, 16}, {65445, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {123, 7}, {4086, 12}, {65446, 16}, {65447, 16}, {65448, 16}, {65449, 16}, {65450, 16}, {65451, 16}, {65452, 16}, {65453, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {250, 8}, {4087, 12}, {65454, 16}, {65455, 16}, {65456, 16}, {65457, 16}, {65458, 16}, {65459, 16}, {65460, 16}, {65461, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {504, 9}, {32704, 15}, {65462, 16}, {65463, 16}, {65464, 16}, {65465, 16}, {65466, 16}, {65467, 16}, {65468, 16}, {65469, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {505, 9}, {65470, 16}, {65471, 16}, {65472, 16}, {65473, 16}, {65474, 16}, {65475, 16}, {65476, 16}, {65477, 16}, {65478, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {506, 9}, {65479, 16}, {65480, 16}, {65481, 16}, {65482, 16}, {65483, 16}, {65484, 16}, {65485, 16}, {65486, 16}, {65487, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {1017, 10}, {65488, 16}, {65489, 16}, {65490, 16}, {65491, 16}, {65492, 16}, {65493, 16}, {65494, 16}, {65495, 16}, {65496, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {1018, 10}, {65497, 16}, {65498, 16}, {65499, 16}, {65500, 16}, {65501, 16}, {65502, 16}, {65503, 16}, {65504, 16}, {65505, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {2040, 11}, {65506, 16}, {65507, 16}, {65508, 16}, {65509, 16}, {65510, 16}, {65511, 16}, {65512, 16}, {65513, 16}, {65514, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {65515, 16}, {65516, 16}, {65517, 16}, {65518, 16}, {65519, 16}, {65520, 16}, {65521, 16}, {65522, 16}, {65523, 16}, {65524, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {2041, 11}, {65525, 16}, {65526, 16}, {65527, 16}, {65528, 16}, {65529, 16}, {65530, 16}, {65531, 16}, {65532, 16}, {65533, 16}, {65534, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
};
static const unsigned short UVAC_HT[256][2] =
{
    {0, 2}, {1, 2}, {4, 3}, {10, 4}, {24, 5}, {25, 5}, {56, 6}, {120, 7}, {500, 9}, {1014, 10}, {4084, 12}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {11, 4}, {57, 6}, {246, 8}, {501, 9}, {2038, 11}, {4085, 12}, {65416, 16}, {65417, 16}, {65418, 16}, {65419, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {26, 5}, {247, 8}, {1015, 10}, {4086, 12}, {32706, 15}, {65420, 16}, {65421, 16}, {65422, 16}, {65423, 16}, {65424, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {27, 5}, {248, 8}, {1016, 10}, {4087, 12}, {65425, 16}, {65426, 16}, {65427, 16}, {65428, 16}, {65429, 16}, {65430, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {58, 6}, {502, 9}, {65431, 16}, {65432, 16}, {65433, 16}, {65434, 16}, {65435, 16}, {65436, 16}, {65437, 16}, {65438, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {59, 6}, {1017, 10}, {65439, 16}, {65440, 16}, {65441, 16}, {65442, 16}, {65443, 16}, {65444, 16}, {65445, 16}, {65446, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {121, 7}, {2039, 11}, {65447, 16}, {65448, 16}, {65449, 16}, {65450, 16}, {65451, 16}, {65452, 16}, {65453, 16}, {65454, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {122, 7}, {2040, 11}, {65455, 16}, {65456, 16}, {65457, 16}, {65458, 16}, {65459, 16}, {65460, 16}, {65461, 16}, {65462, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {249, 8}, {65463, 16}, {65464, 16}, {65465, 16}, {65466, 16}, {65467, 16}, {65468, 16}, {65469, 16}, {65470, 16}, {65471, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {503, 9}, {65472, 16}, {65473, 16}, {65474, 16}, {65475, 16}, {65476, 16}, {65477, 16}, {65478, 16}, {65479, 16}, {65480, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {504, 9}, {65481, 16}, {65482, 16}, {65483, 16}, {65484, 16}, {65485, 16}, {65486, 16}, {65487, 16}, {65488, 16}, {65489, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {505, 9}, {65490, 16}, {65491, 16}, {65492, 16}, {65493, 16}, {65494, 16}, {65495, 16}, {65496, 16}, {65497, 16}, {65498, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {506, 9}, {65499, 16}, {65500, 16}, {65501, 16}, {65502, 16}, {65503, 16}, {65504, 16}, {65505, 16}, {65506, 16}, {65507, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {2041, 11}, {65508, 16}, {65509, 16}, {65510, 16}, {65511, 16}, {65512, 16}, {65513, 16}, {65514, 16}, {65515, 16}, {65516, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {16352, 14}, {65517, 16}, {65518, 16}, {65519, 16}, {65520, 16}, {65521, 16}, {65522, 16}, {65523, 16}, {65524, 16}, {65525, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {1018, 10}, {32707, 15}, {65526, 16}, {65527, 16}, {65528, 16}, {65529, 16}, {65530, 16}, {65531, 16}, {65532, 16}, {65533, 16}, {65534, 16}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
};

static void jpeg_writeBits(jpeg_buf_t *fp, int *pbitBuf, int *pbitCnt, const unsigned short *bs)
{

    int bitCnt = *pbitCnt;
    int bitBuf = *pbitBuf;

    bitCnt += bs[1];
    bitBuf |= bs[0] << (24 - bitCnt);
    while (bitCnt >= 8)
    {
        unsigned char c = (bitBuf >> 16) & 255;
        jpeg_put_char(fp, c);
        if (c == 255)
        {
            jpeg_put_char(fp, 0);
        }
        bitBuf <<= 8;
        bitCnt -= 8;
    }
    *pbitCnt = bitCnt;
    *pbitBuf = bitBuf;

}

inline void jpeg_DCT(float *pd0, float *pd1, float *pd2, float *pd3, float *pd4, float *pd5, float *pd6, float *pd7)
{
    float tmp0 = *pd0 + *pd7;
    float tmp7 = *pd0 - *pd7;
    float tmp1 = *pd1 + *pd6;
    float tmp6 = *pd1 - *pd6;
    float tmp2 = *pd2 + *pd5;
    float tmp5 = *pd2 - *pd5;
    float tmp3 = *pd3 + *pd4;
    float tmp4 = *pd3 - *pd4;

    // Even part
    float tmp10 = tmp0 + tmp3;  // phase 2
    float tmp13 = tmp0 - tmp3;
    float tmp11 = tmp1 + tmp2;
    float tmp12 = tmp1 - tmp2;

    *pd0 = tmp10 + tmp11;       // phase 3
    *pd4 = tmp10 - tmp11;

    float z1 = (tmp12 + tmp13) * 0.707106781f; // c4
    *pd2 = tmp13 + z1;      // phase 5
    *pd6 = tmp13 - z1;

    // Odd part
    tmp10 = tmp4 + tmp5;        // phase 2
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    // The rotator is modified from fig 4-8 to avoid extra negations.
    float z5 = (tmp10 - tmp12) * 0.382683433f; // c6
    float z2 = tmp10 * 0.541196100f + z5; // c2-c6
    float z4 = tmp12 * 1.306562965f + z5; // c2+c6
    float z3 = tmp11 * 0.707106781f; // c4

    float z11 = tmp7 + z3;      // phase 5
    float z13 = tmp7 - z3;

    *pd5 = z13 + z2;            // phase 6
    *pd3 = z13 - z2;
    *pd1 = z11 + z4;
    *pd7 = z11 - z4;

}

static void jpeg_DCT_ori(float *pd0, float *pd1, float *pd2, float *pd3, float *pd4, float *pd5, float *pd6, float *pd7)
{
    float d0 = *pd0;
    float d1 = *pd1;
    float d2 = *pd2;
    float d3 = *pd3;
    float d4 = *pd4;
    float d5 = *pd5;
    float d6 = *pd6;
    float d7 = *pd7;


    float tmp0 = d0 + d7;
    float tmp7 = d0 - d7;
    float tmp1 = d1 + d6;
    float tmp6 = d1 - d6;
    float tmp2 = d2 + d5;
    float tmp5 = d2 - d5;
    float tmp3 = d3 + d4;
    float tmp4 = d3 - d4;

    // Even part
    float tmp10 = tmp0 + tmp3;  // phase 2
    float tmp13 = tmp0 - tmp3;
    float tmp11 = tmp1 + tmp2;
    float tmp12 = tmp1 - tmp2;

    d0 = tmp10 + tmp11;         // phase 3
    d4 = tmp10 - tmp11;

    float z1 = (tmp12 + tmp13) * 0.707106781f; // c4
    d2 = tmp13 + z1;        // phase 5
    d6 = tmp13 - z1;

    // Odd part
    tmp10 = tmp4 + tmp5;        // phase 2
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    // The rotator is modified from fig 4-8 to avoid extra negations.
    float z5 = (tmp10 - tmp12) * 0.382683433f; // c6
    float z2 = tmp10 * 0.541196100f + z5; // c2-c6
    float z4 = tmp12 * 1.306562965f + z5; // c2+c6
    float z3 = tmp11 * 0.707106781f; // c4

    float z11 = tmp7 + z3;      // phase 5
    float z13 = tmp7 - z3;

    d5 = z13 + z2;          // phase 6
    d3 = z13 - z2;
    d1 = z11 + z4;
    d7 = z11 - z4;

    *pd0 = d0;
    *pd1 = d1;
    *pd2 = d2;
    *pd3 = d3;
    *pd4 = d4;
    *pd5 = d5;
    *pd6 = d6;
    *pd7 = d7;

}


static void jpeg_calcBits(int val, unsigned short bits[2])
{
    int tmp1 = val < 0 ? -val : val;
    val = val < 0 ? val - 1 : val;
    bits[1] = 1;
    while (tmp1 >>= 1)
    {
        ++bits[1];
    }
    bits[0] = val & ((1 << bits[1]) - 1);
}

static int jpeg_processDU(jpeg_buf_t *fp, int *bitBuf, int *bitCnt, float *CDU, float *fdtbl, int DC, const unsigned short HTDC[256][2], const unsigned short HTAC[256][2])
{
    const unsigned short EOB[2] = { HTAC[0x00][0], HTAC[0x00][1] };
    const unsigned short M16zeroes[2] = { HTAC[0xF0][0], HTAC[0xF0][1] };

    // DCT rows
    for (int dataOff = 0; dataOff < 64; dataOff += 8)
    {
        jpeg_DCT(&CDU[dataOff], &CDU[dataOff + 1], &CDU[dataOff + 2], &CDU[dataOff + 3], &CDU[dataOff + 4], &CDU[dataOff + 5], &CDU[dataOff + 6], &CDU[dataOff + 7]);
    }
    // DCT columns
    for (int dataOff = 0; dataOff < 8; ++dataOff)
    {
        jpeg_DCT(&CDU[dataOff], &CDU[dataOff + 8], &CDU[dataOff + 16], &CDU[dataOff + 24], &CDU[dataOff + 32], &CDU[dataOff + 40], &CDU[dataOff + 48], &CDU[dataOff + 56]);
    }
    // Quantize/descale/zigzag the coefficients
    int DU[64];
    for (int i = 0; i < 64; ++i)
    {
        float v = CDU[i] * fdtbl[i];
        //DU[s_jpeg_ZigZag[i]] = (int)(v < 0 ? (-debug_floorf(-(v - 0.5f))) : debug_floorf((v + 0.5f)));

        DU[s_jpeg_ZigZag[i]] = (int)(v < 0 ? debug_ceilf(v - 0.5f) : debug_floorf(v + 0.5f));
    }

    // Encode DC
    int diff = DU[0] - DC;
    if (diff == 0)
    {
        jpeg_writeBits(fp, bitBuf, bitCnt, HTDC[0]);
    }
    else
    {
        unsigned short bits[2];
        jpeg_calcBits(diff, bits);
        jpeg_writeBits(fp, bitBuf, bitCnt, HTDC[bits[1]]);
        jpeg_writeBits(fp, bitBuf, bitCnt, bits);
    }
    // Encode ACs
    int end0pos = 63;
    for (; (end0pos > 0) && (DU[end0pos] == 0); --end0pos)
    {
    }
    // end0pos = first element in reverse order !=0
    if (end0pos == 0)
    {
        jpeg_writeBits(fp, bitBuf, bitCnt, EOB);
        return DU[0];
    }
    for (int i = 1; i <= end0pos; ++i)
    {
        int startpos = i;
        for (; DU[i] == 0 && i <= end0pos; ++i)
        {
        }
        int nrzeroes = i - startpos;
        if (nrzeroes >= 16)
        {
            int lng = nrzeroes >> 4;
            for (int nrmarker = 1; nrmarker <= lng; ++nrmarker)
            {
                jpeg_writeBits(fp, bitBuf, bitCnt, M16zeroes);
            }
            nrzeroes &= 15;
        }
        unsigned short bits[2];
        jpeg_calcBits(DU[i], bits);
        jpeg_writeBits(fp, bitBuf, bitCnt, HTAC[(nrzeroes << 4) + bits[1]]);
        jpeg_writeBits(fp, bitBuf, bitCnt, bits);
    }
    if (end0pos != 63)
    {
        jpeg_writeBits(fp, bitBuf, bitCnt, EOB);
    }
    return DU[0];
}

void jpeg_init(int quality)
{
    int i = 0;
    int row = 0;
    int col = 0;
    int k = 0;

    quality = quality ? quality : 90;
    quality = quality < 1 ? 1 : quality > 100 ? 100 : quality;
    quality = quality < 50 ? 5000 / quality : 200 - quality * 2;

    printf("quality = %d\n", quality);

    for (i = 0; i < 64; ++i)
    {
        int yti = (YQT[i] * quality + 50) / 100;
        YTable[s_jpeg_ZigZag[i]] = yti < 1 ? 1 : yti > 255 ? 255 : yti;
        int uvti  = (UVQT[i] * quality + 50) / 100;
        UVTable[s_jpeg_ZigZag[i]] = uvti < 1 ? 1 : uvti > 255 ? 255 : uvti;
    }

    for (row = 0, k = 0; row < 8; ++row)
    {
        for (col = 0; col < 8; ++col, ++k)
        {
            fdtbl_Y[k]  = 1 / (YTable [s_jpeg_ZigZag[k]] * aasf[row] * aasf[col]);
            fdtbl_UV[k] = 1 / (UVTable[s_jpeg_ZigZag[k]] * aasf[row] * aasf[col]);
        }
    }
}


bool openmv_jpeg_compress(image_t *src, image_t *dst, int quality, bool realloc)
{
    // JPEG buffer
    jpeg_buf_t  jpeg_buf =
    {
        .idx = 0,
        .buf = dst->pixels,
        .length = dst->bpp,
        .bitc = 0,
        .bitb = 0,
        .realloc = realloc,
        .overflow = false,
    };
    // Initialize quantization tables
    //jpeg_init(quality);

    jpeg_subsample_t jpeg_subsample;

    if (quality >= 60)
    {
        jpeg_subsample = JPEG_SUBSAMPLE_1x1;
    }
    else if (quality > 35)
    {
        jpeg_subsample = JPEG_SUBSAMPLE_2x1;
    }
    else     // <= 35
    {
        jpeg_subsample = JPEG_SUBSAMPLE_2x2;
    }

    int i = 0;
    int row = 0;
    int col = 0;
    int k = 0;
    unsigned int end, start = 0;
    unsigned int time;

    const void *data = src->data;
    int width = src->w;
    int height = src->h;
    int comp = 3;

    if (!data || !width || !height || comp > 4 || comp < 1 || comp == 2)
    {
        return false;
    }

    unsigned char *fp = jpeg_buf.buf;


    start = k_uptime_get_32();

    // Write Headers
    static const unsigned char head0[] = { 0xFF, 0xD8, 0xFF, 0xE0, 0, 0x10, 'J', 'F', 'I', 'F', 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0xFF, 0xDB, 0, 0x84, 0 };
    jpeg_put_bytes(&jpeg_buf, head0, sizeof(head0));
    jpeg_put_bytes(&jpeg_buf, YTable, sizeof(YTable));
    jpeg_put_char(&jpeg_buf, 1);
    jpeg_put_bytes(&jpeg_buf, UVTable, sizeof(UVTable));
    const unsigned char head1[] = { 0xFF, 0xC0, 0, 0x11, 8, height >> 8, height & 0xFF, width >> 8, width & 0xFF, 3, 1, 0x11, 0, 2, 0x11, 1, 3, 0x11, 1, 0xFF, 0xC4, 0x01, 0xA2, 0 };
    jpeg_put_bytes(&jpeg_buf, head1, sizeof(head1));
    jpeg_put_bytes(&jpeg_buf, std_dc_luminance_nrcodes + 1,  sizeof(std_dc_luminance_nrcodes) - 1);
    jpeg_put_bytes(&jpeg_buf, std_dc_luminance_values,  sizeof(std_dc_luminance_values));
    jpeg_put_char(&jpeg_buf, 0x10); // HTYACinfo
    jpeg_put_bytes(&jpeg_buf, std_ac_luminance_nrcodes + 1,  sizeof(std_ac_luminance_nrcodes) - 1);
    jpeg_put_bytes(&jpeg_buf, std_ac_luminance_values,  sizeof(std_ac_luminance_values));
    jpeg_put_char(&jpeg_buf, 1); // HTUDCinfo
    jpeg_put_bytes(&jpeg_buf, std_dc_chrominance_nrcodes + 1, sizeof(std_dc_chrominance_nrcodes) - 1);
    jpeg_put_bytes(&jpeg_buf, std_dc_chrominance_values,  sizeof(std_dc_chrominance_values));
    jpeg_put_char(&jpeg_buf, 0x11); // HTUACinfo
    jpeg_put_bytes(&jpeg_buf, std_ac_chrominance_nrcodes + 1,  sizeof(std_ac_chrominance_nrcodes) - 1);
    jpeg_put_bytes(&jpeg_buf, std_ac_chrominance_values, sizeof(std_ac_chrominance_values));
    static const unsigned char head2[] = { 0xFF, 0xDA, 0, 0xC, 3, 1, 0, 2, 0x11, 3, 0x11, 0, 0x3F, 0 };
    jpeg_put_bytes(&jpeg_buf, head2, sizeof(head2));

    // Encode 8x8 macroblocks
    const unsigned char *imageData = (const unsigned char *)data;
    int DCY = 0, DCU = 0, DCV = 0;
    int bitBuf = 0, bitCnt = 0;
    int ofsG = comp > 1 ? 1 : 0, ofsB = comp > 1 ? 2 : 0;
    for (int y = 0; y < height; y += 8)
    {
        if (y == 0)
        {
            //start = k_uptime_get_32();
        }

        for (int x = 0; x < width; x += 8)
        {
            float YDU[64], UDU[64], VDU[64];
            for (int row = y, pos = 0; row < y + 8; ++row)
            {
                for (int col = x; col < x + 8; ++col, ++pos)
                {
                    int p = row * width * comp + col * comp;
                    if (row >= height)
                    {
                        p -= width * comp * (row + 1 - height);
                    }
                    if (col >= width)
                    {
                        p -= comp * (col + 1 - width);
                    }

                    //float r = imageData[p+0], g = imageData[p+ofsG], b = imageData[p+ofsB];
                    float b = imageData[p + 0], g = imageData[p + ofsG], r = imageData[p + ofsB];

                    YDU[pos] = +0.29900f * r + 0.58700f * g + 0.11400f * b - 128;
                    UDU[pos] = -0.16874f * r - 0.33126f * g + 0.50000f * b;
                    VDU[pos] = +0.50000f * r - 0.41869f * g - 0.08131f * b;
                }
            }

            DCY = jpeg_processDU(&jpeg_buf, &bitBuf, &bitCnt, YDU, fdtbl_Y, DCY, YDC_HT, YAC_HT);
            DCU = jpeg_processDU(&jpeg_buf, &bitBuf, &bitCnt, UDU, fdtbl_UV, DCU, UVDC_HT, UVAC_HT);
            DCV = jpeg_processDU(&jpeg_buf, &bitBuf, &bitCnt, VDU, fdtbl_UV, DCV, UVDC_HT, UVAC_HT);
        }
        if (y == 0)
        {
            end = k_uptime_get_32();
        }
    }

    // Do the bit alignment of the EOI marker
    static const unsigned short fillBits[] = {0x7F, 7};
    jpeg_writeBits(&jpeg_buf, &bitBuf, &bitCnt, fillBits);

    // EOI
    jpeg_put_char(&jpeg_buf, 0xFF);
    jpeg_put_char(&jpeg_buf, 0xD9);

    end = k_uptime_get_32();
    time = end - start;
    printf("jpeg encode cost time = [%d]\n", time);
    printf("jpeg_buf.idx = [%d]\n", jpeg_buf.idx);

#if 0
    int m = 0;

    for (m = 0; m < jpeg_buf.idx; m++)
    {
        if (m < 100)
        {
            if (m % 8 == 0)
            {
                printf("\n");
            }
            printf("0x%2x ",  fp[m]);
        }
    }

    printf("\r\n");
#endif

    return true;

}

void jpeg_write(image_t *img, uint32_t dst,  int quality)
{
    uint32_t size;
    image_t out;
    out.w = img->w;
    out.h = img->h;
    out.bpp = size;
    out.pixels = dst;

    // When jpeg_compress needs more memory than in currently allocated it
    // will try to realloc. MP will detect that the pointer is outside of
    // the heap and return NULL which will cause an out of memory error.
    openmv_jpeg_compress(img, &out, quality, false);
}
