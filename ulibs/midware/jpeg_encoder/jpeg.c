#include "jpeg.h"
#include <stdio.h>

static int jpeg_processDU(jpeg_buf_t *jpeg_buf, signed char *CDU, float *fdtbl, int DC, const uint16_t (*HTDC)[2], const uint16_t (*HTAC)[2]);
static void jpeg_init(int quality);

// Software JPEG implementation.
#define FIX_0_382683433  ((int32_t)   98)
#define FIX_0_541196100  ((int32_t)  139)
#define FIX_0_707106781  ((int32_t)  181)
#define FIX_1_306562965  ((int32_t)  334)
#define DESCALE(x, y)   (x>>y)
#define MULTIPLY(x, y)  DESCALE((x) * (y), 8)


int g_gm_index = 0;
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

int asm inline fast_floorf_in(float x)
{
    VCVT.S32.F32 S2, S0
    VMOV  R0, S2
    BX LR
}

int inline floorf_c_fast(float x)
{
    return fast_floorf_in(x);
}

int inline ceilf_c_fast(float x)
{
    return x > 0 ? floorf_c_fast(x + 0.9999f) : floorf_c_fast(x);
}

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

int asm inline fast_roundf_in(float x)
{
    VCVTR.S32.F32 S2, S0
    VMOV  R0, S2
    BX LR
}

int inline roundf_c(float x)
{
    //int n;
    float r = x - (int) x;
    return (x > 0) ? ((r > 0.49999) ? (int)(x + 0.5) : (int)x) : ((r < -0.49999) ? ((int)(x - 0.5)) : (int)x);
}

#if 0
int inline roundf_c(float x)
{
    int n;
    float r;
    n = (int) x;
    r = x - n;

    if (x > 0)
    {
        if (r > 0.49999)
        {
            return (int)(x + 0.5);
        }
    }
    else
    {
        if (r < -0.49999)
        {
            return (int)(x - 0.5);
        }
    }

    return (int)x;
}


#endif

int inline fast_roundf(float x)
{
    //return fast_roundf_in(x);
    return roundf_c(x);
}

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

static void jpeg_writeBits(jpeg_buf_t *jpeg_buf, const uint16_t *bs)
{
    jpeg_buf->bitc += bs[1];
    jpeg_buf->bitb |= bs[0] << (24 - jpeg_buf->bitc);

    while (jpeg_buf->bitc > 7)
    {
        uint8_t c = (jpeg_buf->bitb >> 16) & 255;
        jpeg_put_char(jpeg_buf, c);
        if (c == 255)
        {
            jpeg_put_char(jpeg_buf, 0);
        }
        jpeg_buf->bitb <<= 8;
        jpeg_buf->bitc -= 8;
    }
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

static int jpeg_processDU(jpeg_buf_t *jpeg_buf, signed char *CDU, float *fdtbl, int DC, const uint16_t (*HTDC)[2], const uint16_t (*HTAC)[2])
{
    int DU[64];
    int DUQ[64];
    int z1, z2, z3, z4, z5, z11, z13;
    int t0, t1, t2, t3, t4, t5, t6, t7, t10, t11, t12, t13;
    const uint16_t EOB[2] = { HTAC[0x00][0], HTAC[0x00][1] };
    const uint16_t M16zeroes[2] = { HTAC[0xF0][0], HTAC[0xF0][1] };

    // DCT rows
    for (int i = 8, *p = DU; i > 0; i--, p += 8, CDU += 8)
    {
        t0 = CDU[0] + CDU[7];
        t1 = CDU[1] + CDU[6];
        t2 = CDU[2] + CDU[5];
        t3 = CDU[3] + CDU[4];

        t7 = CDU[0] - CDU[7];
        t6 = CDU[1] - CDU[6];
        t5 = CDU[2] - CDU[5];
        t4 = CDU[3] - CDU[4];

        // Even part
        t10 = t0 + t3;
        t13 = t0 - t3;
        t11 = t1 + t2;
        t12 = t1 - t2;
        z1 = MULTIPLY(t12 + t13, FIX_0_707106781); // c4

        p[0] = t10 + t11;
        p[4] = t10 - t11;
        p[2] = t13 + z1;
        p[6] = t13 - z1;

        // Odd part
        t10 = t4 + t5;// phase 2
        t11 = t5 + t6;
        t12 = t6 + t7;

        // The rotator is modified from fig 4-8 to avoid extra negations.
        z5 = MULTIPLY(t10 - t12, FIX_0_382683433); // c6
        z2 = MULTIPLY(t10, FIX_0_541196100) + z5; // 1.306562965f-c6
        z4 = MULTIPLY(t12, FIX_1_306562965) + z5; // 1.306562965f+c6
        z3 = MULTIPLY(t11, FIX_0_707106781); // c4
        z11 = t7 + z3;    // phase 5
        z13 = t7 - z3;

        p[5] = z13 + z2;// phase 6
        p[3] = z13 - z2;
        p[1] = z11 + z4;
        p[7] = z11 - z4;
    }


    // DCT columns
    for (int i = 8, *p = DU; i > 0; i--, p++)
    {

        t0 = p[0]  + p[56];
        t1 = p[8]  + p[48];
        t2 = p[16] + p[40];
        t3 = p[24] + p[32];

        t7 = p[0]  - p[56];
        t6 = p[8]  - p[48];
        t5 = p[16] - p[40];
        t4 = p[24] - p[32];

        // Even part
        t10 = t0 + t3;  // phase 2
        t13 = t0 - t3;
        t11 = t1 + t2;
        t12 = t1 - t2;
        z1 = MULTIPLY(t12 + t13, FIX_0_707106781); // c4

        p[0] = t10 + t11;       // phase 3
        p[32] = t10 - t11;
        p[16] = t13 + z1;       // phase 5
        p[48] = t13 - z1;

        // Odd part
        t10 = t4 + t5;      // phase 2
        t11 = t5 + t6;
        t12 = t6 + t7;

        // The rotator is modified from fig 4-8 to avoid extra negations.
        z5 = MULTIPLY(t10 - t12, FIX_0_382683433); // c6
        z2 = MULTIPLY(t10, FIX_0_541196100) + z5; // 1.306562965f-c6
        z4 = MULTIPLY(t12, FIX_1_306562965) + z5; // 1.306562965f+c6
        z3 = MULTIPLY(t11, FIX_0_707106781); // c4
        z11 = t7 + z3;      // phase 5
        z13 = t7 - z3;

        p[40] = z13 + z2;// phase 6
        p[24] = z13 - z2;
        p[8] = z11 + z4;
        p[56] = z11 - z4;
    }

    // first non-zero element in reverse order
    int end0pos = 0;
    // Quantize/descale/zigzag the coefficients
    for (int i = 0; i < 64; ++i)
    {
        DUQ[s_jpeg_ZigZag[i]] = fast_roundf(DU[i] * fdtbl[i]);
        if (s_jpeg_ZigZag[i] > end0pos && DUQ[s_jpeg_ZigZag[i]])
        {
            end0pos = s_jpeg_ZigZag[i];
        }
    }

    // Encode DC
    int diff = DUQ[0] - DC;
    if (diff == 0)
    {
        jpeg_writeBits(jpeg_buf, HTDC[0]);
    }
    else
    {
        uint16_t bits[2];
        jpeg_calcBits(diff, bits);
        jpeg_writeBits(jpeg_buf, HTDC[bits[1]]);
        jpeg_writeBits(jpeg_buf, bits);
    }

    // Encode ACs
    if (end0pos == 0)
    {
        jpeg_writeBits(jpeg_buf, EOB);
        return DUQ[0];
    }

    for (int i = 1; i <= end0pos; ++i)
    {
        int startpos = i;
        for (; DUQ[i] == 0 && i <= end0pos ; ++i)
        {
        }

        int nrzeroes = i - startpos;
        if (nrzeroes >= 16)
        {
            int lng = nrzeroes >> 4;
            for (int nrmarker = 1; nrmarker <= lng; ++nrmarker)
            {
                jpeg_writeBits(jpeg_buf, M16zeroes);
            }
            nrzeroes &= 15;
        }
        uint16_t bits[2];
        jpeg_calcBits(DUQ[i], bits);
        jpeg_writeBits(jpeg_buf, HTAC[(nrzeroes << 4) + bits[1]]);
        jpeg_writeBits(jpeg_buf, bits);
    }
    if (end0pos != 63)
    {
        jpeg_writeBits(jpeg_buf, EOB);
    }
    return DUQ[0];
}

static void jpeg_init(int quality)
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
    jpeg_init(quality);

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
        for (int x = 0; x < width; x += 8)
        {
            signed char YDU[64], UDU[64], VDU[64];
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

                    unsigned char b = imageData[p + 0], g = imageData[p + ofsG], r = imageData[p + ofsB];

                    YDU[pos] = (signed char)(+0.29900f * r + 0.58700f * g + 0.11400f * b - 128);
                    UDU[pos] = (signed char)(-0.16874f * r - 0.33126f * g + 0.50000f * b);
                    VDU[pos] = (signed char)(+0.50000f * r - 0.41869f * g - 0.08131f * b);
                }
            }

            DCY = jpeg_processDU(&jpeg_buf,  YDU, fdtbl_Y, DCY, YDC_HT, YAC_HT);
            DCU = jpeg_processDU(&jpeg_buf, UDU, fdtbl_UV, DCU, UVDC_HT, UVAC_HT);
            DCV = jpeg_processDU(&jpeg_buf,  VDU, fdtbl_UV, DCV, UVDC_HT, UVAC_HT);
        }
    }

    // Do the bit alignment of the EOI marker
    static const unsigned short fillBits[] = {0x7F, 7};
    jpeg_writeBits(&jpeg_buf, fillBits);

    // EOI
    jpeg_put_char(&jpeg_buf, 0xFF);
    jpeg_put_char(&jpeg_buf, 0xD9);

    end = k_uptime_get_32();
    time = end - start;
    printf("jpeg encode cost time = [%d] ms\n", time);
    printf("jpeg_buf.idx = [%d]\n", jpeg_buf.idx);

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

    printf("fp = 0x%x\r\n", fp);
    //while (1);

    return true;

}

void jpeg_encode(image_t *img, uint8_t *buffer, int quality)
{
    uint32_t size;
    //uint8_t *buffer = &data_debug[0]  ;//= fb_alloc_all(&size);
    //image_t out = { .w=img->w, .h=img->h, .bpp=size, .pixels=buffer };

    image_t out;
    out.w = img->w;
    out.h = img->h;
    out.bpp = size;
    out.pixels = buffer;

    printf("fast_roundf(3.1) = %d\n", fast_roundf(3.1));
    printf("fast_roundf(3.6) = %d\n", fast_roundf(3.6));
    printf("fast_roundf(3.5) = %d\n", fast_roundf(3.5));
    printf("fast_roundf(-3.1) = %d\n", fast_roundf(-3.1));
    printf("fast_roundf(-3.6) = %d\n", fast_roundf(-3.6));
    printf("fast_roundf(-3.5) = %d\n", fast_roundf(-3.5));

    // When jpeg_compress needs more memory than in currently allocated it
    // will try to realloc. MP will detect that the pointer is outside of
    // the heap and return NULL which will cause an out of memory error.
    openmv_jpeg_compress(img, &out, quality, false);
}

