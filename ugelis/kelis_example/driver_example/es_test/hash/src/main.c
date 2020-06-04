#include <ugelis.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <gm_hal_hash.h>

/* DMA mode test */
#define DMA_MODE_TEST   0


#define HASH_TIMEOUT_VAL        5      //ms

static HASH_Handle_T hashHandle;
static HASH_Config_T config;
static uint8_t dataTransDone = FALSE;
static uint8_t dataTransErr = FALSE;
static uint8_t dataAvailable = FALSE;

/*
 * FIPS-180-1 test vectors
 */
static const unsigned char sha1_test_buf[3][60] __attribute__((aligned(4))) =
                //static  unsigned char sha1_test_buf[3][60] __in_section_unique(sram_data)=
{
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};
//unsigned char * sha1_test_buf;/* = (uint32_t)0xc0010100;*/


static const int sha1_test_buflen[3] =
{
    3, 56, 1000
};

static const unsigned char sha1_test_sum[3][20] =
{
    {
        0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
        0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D
    },
    {
        0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
        0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1
    },
    {
        0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
        0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F
    }
};

/*
 * FIPS-180-2 test vectors
 */
static const unsigned char sha256_test_buf[3][60] __attribute__((aligned(4))) =
{
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};

static const int sha256_test_buflen[3] =
{
    3, 56, 1000
};

static const unsigned char sha256_test_sum[6][32] =
{
    /*
     * SHA-224 test vectors
     */
    {
        0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8, 0x22,
        0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2, 0x55, 0xB3,
        0x2A, 0xAD, 0xBC, 0xE4, 0xBD, 0xA0, 0xB3, 0xF7,
        0xE3, 0x6C, 0x9D, 0xA7
    },
    {
        0x75, 0x38, 0x8B, 0x16, 0x51, 0x27, 0x76, 0xCC,
        0x5D, 0xBA, 0x5D, 0xA1, 0xFD, 0x89, 0x01, 0x50,
        0xB0, 0xC6, 0x45, 0x5C, 0xB4, 0xF5, 0x8B, 0x19,
        0x52, 0x52, 0x25, 0x25
    },
    {
        0x20, 0x79, 0x46, 0x55, 0x98, 0x0C, 0x91, 0xD8,
        0xBB, 0xB4, 0xC1, 0xEA, 0x97, 0x61, 0x8A, 0x4B,
        0xF0, 0x3F, 0x42, 0x58, 0x19, 0x48, 0xB2, 0xEE,
        0x4E, 0xE7, 0xAD, 0x67
    },

    /*
     * SHA-256 test vectors
     */
    {
        0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
        0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
        0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
        0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD
    },
    {
        0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
        0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
        0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
        0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1
    },
    {
        0xCD, 0xC7, 0x6E, 0x5C, 0x99, 0x14, 0xFB, 0x92,
        0x81, 0xA1, 0xC7, 0xE2, 0x84, 0xD7, 0x3E, 0x67,
        0xF1, 0x80, 0x9A, 0x48, 0xA4, 0x97, 0x20, 0x0E,
        0x04, 0x6D, 0x39, 0xCC, 0xC7, 0x11, 0x2C, 0xD0
    }
};

/*
 * FIPS-180-2 test vectors
 */
static const unsigned char sha512_test_buf[3][120] __attribute__((aligned(4))) =
{
    { "abc" },
    {
        "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
        "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
    },
    { "" }
};

static const int sha512_test_buflen[3] =
{
    3, 112, 1000
};

static const unsigned char sha512_test_sum[6][64] =
{
    /*
     * SHA-384 test vectors
     */
    {
        0xCB, 0x00, 0x75, 0x3F, 0x45, 0xA3, 0x5E, 0x8B,
        0xB5, 0xA0, 0x3D, 0x69, 0x9A, 0xC6, 0x50, 0x07,
        0x27, 0x2C, 0x32, 0xAB, 0x0E, 0xDE, 0xD1, 0x63,
        0x1A, 0x8B, 0x60, 0x5A, 0x43, 0xFF, 0x5B, 0xED,
        0x80, 0x86, 0x07, 0x2B, 0xA1, 0xE7, 0xCC, 0x23,
        0x58, 0xBA, 0xEC, 0xA1, 0x34, 0xC8, 0x25, 0xA7
    },
    {
        0x09, 0x33, 0x0C, 0x33, 0xF7, 0x11, 0x47, 0xE8,
        0x3D, 0x19, 0x2F, 0xC7, 0x82, 0xCD, 0x1B, 0x47,
        0x53, 0x11, 0x1B, 0x17, 0x3B, 0x3B, 0x05, 0xD2,
        0x2F, 0xA0, 0x80, 0x86, 0xE3, 0xB0, 0xF7, 0x12,
        0xFC, 0xC7, 0xC7, 0x1A, 0x55, 0x7E, 0x2D, 0xB9,
        0x66, 0xC3, 0xE9, 0xFA, 0x91, 0x74, 0x60, 0x39
    },
    {
        0x9D, 0x0E, 0x18, 0x09, 0x71, 0x64, 0x74, 0xCB,
        0x08, 0x6E, 0x83, 0x4E, 0x31, 0x0A, 0x4A, 0x1C,
        0xED, 0x14, 0x9E, 0x9C, 0x00, 0xF2, 0x48, 0x52,
        0x79, 0x72, 0xCE, 0xC5, 0x70, 0x4C, 0x2A, 0x5B,
        0x07, 0xB8, 0xB3, 0xDC, 0x38, 0xEC, 0xC4, 0xEB,
        0xAE, 0x97, 0xDD, 0xD8, 0x7F, 0x3D, 0x89, 0x85
    },

    /*
     * SHA-512 test vectors
     */
    {
        0xDD, 0xAF, 0x35, 0xA1, 0x93, 0x61, 0x7A, 0xBA,
        0xCC, 0x41, 0x73, 0x49, 0xAE, 0x20, 0x41, 0x31,
        0x12, 0xE6, 0xFA, 0x4E, 0x89, 0xA9, 0x7E, 0xA2,
        0x0A, 0x9E, 0xEE, 0xE6, 0x4B, 0x55, 0xD3, 0x9A,
        0x21, 0x92, 0x99, 0x2A, 0x27, 0x4F, 0xC1, 0xA8,
        0x36, 0xBA, 0x3C, 0x23, 0xA3, 0xFE, 0xEB, 0xBD,
        0x45, 0x4D, 0x44, 0x23, 0x64, 0x3C, 0xE8, 0x0E,
        0x2A, 0x9A, 0xC9, 0x4F, 0xA5, 0x4C, 0xA4, 0x9F
    },
    {
        0x8E, 0x95, 0x9B, 0x75, 0xDA, 0xE3, 0x13, 0xDA,
        0x8C, 0xF4, 0xF7, 0x28, 0x14, 0xFC, 0x14, 0x3F,
        0x8F, 0x77, 0x79, 0xC6, 0xEB, 0x9F, 0x7F, 0xA1,
        0x72, 0x99, 0xAE, 0xAD, 0xB6, 0x88, 0x90, 0x18,
        0x50, 0x1D, 0x28, 0x9E, 0x49, 0x00, 0xF7, 0xE4,
        0x33, 0x1B, 0x99, 0xDE, 0xC4, 0xB5, 0x43, 0x3A,
        0xC7, 0xD3, 0x29, 0xEE, 0xB6, 0xDD, 0x26, 0x54,
        0x5E, 0x96, 0xE5, 0x5B, 0x87, 0x4B, 0xE9, 0x09
    },
    {
        0xE7, 0x18, 0x48, 0x3D, 0x0C, 0xE7, 0x69, 0x64,
        0x4E, 0x2E, 0x42, 0xC7, 0xBC, 0x15, 0xB4, 0x63,
        0x8E, 0x1F, 0x98, 0xB1, 0x3B, 0x20, 0x44, 0x28,
        0x56, 0x32, 0xA8, 0x03, 0xAF, 0xA9, 0x73, 0xEB,
        0xDE, 0x0F, 0xF2, 0x44, 0x87, 0x7E, 0xA6, 0x0A,
        0x4C, 0xB0, 0x43, 0x2C, 0xE5, 0x77, 0xC3, 0x1B,
        0xEB, 0x00, 0x9C, 0x5C, 0x2C, 0x49, 0xAA, 0x2E,
        0x4E, 0xAD, 0xB2, 0x17, 0xAD, 0x8C, 0xC0, 0x9B
    }
};


/* HMAC test data */
static const unsigned char hmac_inner_key[] __attribute__((aligned(4))) =
{
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
};

static const unsigned char hmac_test_buf[20] __attribute__((aligned(4))) = "Hi There";

static const unsigned char hmac_sum_sha1[] =
{
    0xb6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64, 0xe2, 0x8b, 0xc0, 0xb6,
    0xfb, 0x37, 0x8c, 0x8e, 0xf1, 0x46, 0xbe, 0x00
};
static const unsigned char hmac_sum_sha224[] =
{
    0x89, 0x6f, 0xb1, 0x12, 0x8a, 0xbb, 0xdf, 0x19, 0x68, 0x32, 0x10, 0x7c, 0xd4, 0x9d, 0xf3, 0x3f,
    0x47, 0xb4, 0xb1, 0x16, 0x99, 0x12, 0xba, 0x4f, 0x53, 0x68, 0x4b, 0x22
};

static const unsigned char hmac_sum_sha256[] =
{
    0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53, 0x5c, 0xa8, 0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b,
    0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83, 0x3d, 0xa7, 0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7
};

static const unsigned char hmac_sum_sha384[] =
{
    0xaf, 0xd0, 0x39, 0x44, 0xd8, 0x48, 0x95, 0x62, 0x6b, 0x08, 0x25, 0xf4, 0xab, 0x46, 0x90, 0x7f,
    0x15, 0xf9, 0xda, 0xdb, 0xe4, 0x10, 0x1e, 0xc6, 0x82, 0xaa, 0x03, 0x4c, 0x7c, 0xeb, 0xc5, 0x9c,
    0xfa, 0xea, 0x9e, 0xa9, 0x07, 0x6e, 0xde, 0x7f, 0x4a, 0xf1, 0x52, 0xe8, 0xb2, 0xfa, 0x9c, 0xb6
};

static const unsigned char hmac_sum_sha512[] =
{
    0x87, 0xaa, 0x7c, 0xde, 0xa5, 0xef, 0x61, 0x9d, 0x4f, 0xf0, 0xb4, 0x24, 0x1a, 0x1d, 0x6c, 0xb0,
    0x23, 0x79, 0xf4, 0xe2, 0xce, 0x4e, 0xc2, 0x78, 0x7a, 0xd0, 0xb3, 0x05, 0x45, 0xe1, 0x7c, 0xde,
    0xda, 0xa8, 0x33, 0xb7, 0xd6, 0xb8, 0xa7, 0x02, 0x03, 0x8b, 0x27, 0x4e, 0xae, 0xa3, 0xf4, 0xe4,
    0xbe, 0x9d, 0x91, 0x4e, 0xeb, 0x61, 0xf1, 0x70, 0x2e, 0x69, 0x6c, 0x20, 0x3a, 0x12, 0x68, 0x54
};


/* swapping test */
uint32_t    sha1_msg_len        =   3  ;
uint32_t    sha1_tc0_msg[16]    =   {0x61626300};
uint32_t    sha1_tc1_msg[16]    =   {0x63006162};
uint32_t    sha1_tc2_msg[16]    =   {0x00636261};
uint32_t    sha1_tc3_msg[16]    =   {0x00c64686};
uint32_t    sha1_tc0_digest[5]  =   {0xa9993e36,
                                     0x4706816a,
                                     0xba3e2571,
                                     0x7850c26c,
                                     0x9cd0d89d
                                    };
uint32_t    sha1_tc1_digest[5]  =   {0x3e36a999,
                                     0x816a4706,
                                     0x2571ba3e,
                                     0xc26c7850,
                                     0xd89d9cd0
                                    };
uint32_t    sha1_tc2_digest[5]  =   {0x363e99a9,
                                     0x6a810647,
                                     0x71253eba,
                                     0x6cc25078,
                                     0x9dd8d09c
                                    };
uint32_t    sha1_tc3_digest[5]  =   {0x6c7c9995,
                                     0x568160e2,
                                     0x8ea47c5d,
                                     0x36430a1e,
                                     0xb91b0b39
                                    };


void HASH_Callback(struct HASH_Handle *hHash, HASH_Event_T event, unsigned long param)
{
    if (event == HASH_EVENT_TRANS_DONE)
    {
        dataTransDone = TRUE;
    }
    else if (event == HASH_EVENT_TRANS_ERR)
    {
        dataTransErr = TRUE;
    }
    else if (event == HASH_EVENT_DIGEST_AVAILABEL)
    {
        dataAvailable = TRUE;
    }
}

void HASH_SHA1_test(HASH_Handle_T *hHash, HASH_Config_T *config)
{
    uint32_t i = 0;
#if 0
    uint32_t j = 0;
    uint8_t buf[1024] __attribute__((aligned(4)));
#endif
    uint8_t sha1Sum[20];

    for (i = 0; i < 3; i++)
    {

        printk("  SHA-1 test #%d: ", i + 1);
#if 0
        //test sdram stored data, flash store code
        if (i == 0)
        {
            sha1_test_buf = (uint32_t)0xc0010100;
            *((uint32_t *)sha1_test_buf) = 0x00636261;
        }
        else if (i == 1)
        {
            sha1_test_buf = (uint32_t)0xc0010200;
            memcpy(sha1_test_buf, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
        }
#endif

        memset(config, 0, sizeof(HASH_Config_T));
        config->algo = HASH_ALGO_SHA1;
        config->dataType = HASH_DATATYPE_8B;
        config->mode = HASH_ALGOMODE_HASH;
        config->callback = HASH_Callback;
#if DMA_MODE_TEST
        config->enableDMA = TRUE;
        config->mulDmaTrans = FALSE;
#endif
        HAL_HASH_Init(hHash, config);
        if (i == 2)
        {
            printk("not support multi-dma now.\n");
            continue;
#if 0
            memset(buf, 'a', 1000);
            for (j = 0; j < 999; j++)
            {
                dataTransDone = FALSE;
                HAL_HASH_Start(hHash, buf, 1000);
#if 1
                while (dataTransDone != TRUE)
                {
                    HAL_DelayMs(1);
                }
#endif
            }
            HAL_HASH_Start(hHash, buf, 1000);
#endif
        }
        else
        {
            HAL_HASH_Start(hHash, (uint8_t *)(sha1_test_buf[i]), sha1_test_buflen[i]);
            //HAL_HASH_Start(hHash, sha1_test_buf, sha1_test_buflen[i]);
        }
#if 1
        while (dataTransDone != TRUE)
        {
            HAL_DelayMs(1);
        }
#endif
        dataTransDone = FALSE;
        memset(sha1Sum, 0, sizeof(sha1Sum));
        HAL_HASH_Final(hHash, sha1Sum, HASH_TIMEOUT_VAL);
        if (memcmp(sha1Sum, sha1_test_sum[i], 20) != 0)
        {
            printk("fail.\n");
        }
        else
        {
            printk("passes.\n");
        }

        HAL_HASH_DeInit(hHash);
    }

}


void HASH_SHA256_test(HASH_Handle_T *hHash, HASH_Config_T *config)
{
    uint32_t i = 0, j = 0, k = 0, z = 0;
    uint8_t buf[1024];
    uint8_t sha256Sum[32];

    for (i = 0; i < 6; i++)
    {
        j = i % 3;
        k = i < 3;
        printk("  SHA-%d test #%d: ", 256 - k * 32, j + 1);

        memset(config, 0, sizeof(HASH_Config_T));
        if (k == 0)
        {
            config->algo = HASH_ALGO_SHA256;
        }
        else
        {
            config->algo = HASH_ALGO_SHA224;
        }
        config->dataType = HASH_DATATYPE_8B;
        config->mode = HASH_ALGOMODE_HASH;
        config->callback = HASH_Callback;
#if DMA_MODE_TEST
        config->enableDMA = TRUE;
        config->mulDmaTrans = FALSE;
#endif

        HAL_HASH_Init(hHash, config);

        if (j == 2)
        {
            printk("not support multi-dma now.\n");
            continue;
#if 0
            memset(buf, 'a', 1000);
            for (z = 0; z < 999; z++)
            {
                dataTransDone = FALSE;
                HAL_HASH_Start(hHash, buf, 1000);
                while (dataTransDone != TRUE)
                {
                    HAL_DelayMs(1);
                }
            }
            HAL_HASH_Start(hHash, buf, 1000);
#endif
        }
        else
        {
            HAL_HASH_Start(hHash, sha256_test_buf[j], sha256_test_buflen[j]);
        }

        while (dataTransDone != TRUE)
        {
            HAL_DelayMs(1);
        }
        dataTransDone = FALSE;
        HAL_HASH_Final(hHash, sha256Sum, HASH_TIMEOUT_VAL);

        if (memcmp(sha256Sum, sha256_test_sum[i], 32 - k * 4) != 0)
        {
            printk("fail.\n");
        }
        else
        {
            printk("passes.\n");
        }

        HAL_HASH_DeInit(hHash);
    }

}


void HASH_SHA512_test(HASH_Handle_T *hHash, HASH_Config_T *config)
{
    uint32_t i = 0, j = 0, k = 0, z = 0;
    uint8_t buf[1024];
    uint8_t sha512Sum[64];

    for (i = 0; i < 6; i++)
    {
        j = i % 3;
        k = i < 3;
        printk("  SHA-%d test #%d: ", 512 - k * 128, j + 1);

        memset(config, 0, sizeof(HASH_Config_T));
        if (k == 0)
        {
            config->algo = HASH_ALGO_SHA512;
        }
        else
        {
            config->algo = HASH_ALGO_SHA384;
        }
        config->dataType = HASH_DATATYPE_8B;
        config->mode = HASH_ALGOMODE_HASH;
        config->callback = HASH_Callback;
#if DMA_MODE_TEST
        config->enableDMA = TRUE;
        config->mulDmaTrans = FALSE;
#endif

        HAL_HASH_Init(hHash, config);

        if (j == 2)
        {
            printk("not support multi-dma now.\n");
            continue;
#if 0
            memset(buf, 'a', 1000);
            for (z = 0; z < 999; z++)
            {
                //dataTransDone = FALSE;
                dataAvailable = FALSE;
                HAL_HASH_Start(hHash, buf, 1000);
                //while (dataTransDone != TRUE)
                while (dataAvailable != TRUE)
                {
                    HAL_DelayMs(1);
                }
            }
            HAL_HASH_Start(hHash, buf, 1000);
#endif
        }
        else
        {
            HAL_HASH_Start(hHash, sha512_test_buf[j], sha512_test_buflen[j]);
        }

        while (dataTransDone != TRUE)
        {
            HAL_DelayMs(1);
        }
        dataTransDone = FALSE;
        HAL_HASH_Final(hHash, sha512Sum, HASH_TIMEOUT_VAL);

        if (memcmp(sha512Sum, sha512_test_sum[i], 64 - k * 16) != 0)
        {
            printk("fail.\n");
        }
        else
        {
            printk("passes.\n");
        }

        HAL_HASH_DeInit(hHash);
    }

}

uint32_t hmac_key[4] =
{
    0x1234567, 0x89abcdef, 0x23456789, 0xabcdef01
};


void OTP_Program(uint32_t addr, uint32_t *key, uint32_t len)
{
    uint32_t i = 0;
    uint32_t tmp = 0;

    *((volatile uint32_t *)0x1FFFF014) &= ~(0x01 << 0); //disable OTP
    *((volatile uint32_t *)0x1FFFF010) = 0x02;   //program mode
    *((volatile uint32_t *)0x1FFFF014) |= (0x01 << 0); //enable OTP

    for (i = 0; i < len; i++)
    {
        *((volatile uint32_t *)0x1FFFF01C) = ((addr >> 2));
        *((volatile uint32_t *)0x1FFFF028) = key[i];
        addr += 4;

        *((volatile uint32_t *)0x1FFFF018) &= ~(0x01 << 1);
        *((volatile uint32_t *)0x1FFFF020) |= 0x01;
        tmp = *((volatile uint32_t *)0x1FFFF024);
        while (tmp & 0x01)
        {
            ;
        }

        *((volatile uint32_t *)0x1FFFF018) |= (0x01 << 1);
        *((volatile uint32_t *)0x1FFFF020) |= 0x01;
        while (tmp & 0x01)
        {
            ;
        }
    }

}

uint32_t OTP_Read(uint32_t addr)
{
    uint32_t data = 0;

    *((volatile uint32_t *)0x1FFFF014) &= ~(0x01 << 0); //disable OTP
    *((volatile uint32_t *)0x1FFFF010) = 0;   //program mode
    *((volatile uint32_t *)0x1FFFF014) |= (0x01 << 0); //enable OTP

    data = *((volatile uint32_t *)(0x1FFF8000 + addr));

    return data;
}

void HASH_HMAC_test(HASH_Handle_T *hHash, HASH_Config_T *config)
{
    uint8_t hmacSum[64];

    memset(config, 0, sizeof(HASH_Config_T));
#if 1
    /* HMAC key from DIN test */
    config->keySel = HASH_KEYFROM_DIN;
    config->pKey = hmac_inner_key;
    config->keySize = sizeof(hmac_inner_key);
#else
    /* HMAC key from OTP test */
    config->keySel = HASH_KEYFROM_OTP;
    config->otpPos = HMAC_OTP_K7;
    config->keySize = 16;
#endif

    /* SHA1 HMAC */
    config->algo = HASH_ALGO_SHA1;
    config->dataType = HASH_DATATYPE_8B;
    config->mode = HASH_ALGOMODE_HMAC;
    config->callback = HASH_Callback;
#if DMA_MODE_TEST
    config->enableDMA = TRUE;
    config->mulDmaTrans = FALSE;
#endif

    HAL_HASH_Init(hHash, config);

    HAL_HASH_Start(hHash, hmac_test_buf, strlen(hmac_test_buf));
#if DMA_MODE_TEST
    while (dataTransDone != TRUE)
    {
        HAL_DelayMs(1);
    }
#endif
    dataTransDone = FALSE;
    HAL_HASH_Final(hHash, hmacSum, HASH_TIMEOUT_VAL);
    if (memcmp(hmac_sum_sha1, hmacSum, 20) != 0)
    {
        printk("    HMAC SHA1 test fail.\n");
    }
    else
    {
        printk("    HMAC SHA1 test success.\n");
    }

    HAL_HASH_DeInit(hHash);

    /* SHA224 HMAC */
    config->algo = HASH_ALGO_SHA224;
    config->dataType = HASH_DATATYPE_8B;
    config->mode = HASH_ALGOMODE_HMAC;
    config->callback = HASH_Callback;
#if DMA_MODE_TEST
    config->enableDMA = TRUE;
    config->mulDmaTrans = FALSE;
#endif

    HAL_HASH_Init(hHash, config);

    HAL_HASH_Start(hHash, hmac_test_buf, strlen(hmac_test_buf));
#if DMA_MODE_TEST
    while (dataTransDone != TRUE)
    {
        HAL_DelayMs(1);
    }
#endif
    dataTransDone = FALSE;
    HAL_HASH_Final(hHash, hmacSum, HASH_TIMEOUT_VAL);
    if (memcmp(hmac_sum_sha224, hmacSum, 28) != 0)
    {
        printk("    HMAC SHA224 test fail.\n");
    }
    else
    {
        printk("    HMAC SHA224 test success.\n");
    }

    HAL_HASH_DeInit(hHash);

    /* SHA256 HMAC */
    config->algo = HASH_ALGO_SHA256;
    config->dataType = HASH_DATATYPE_8B;
    config->mode = HASH_ALGOMODE_HMAC;
    config->callback = HASH_Callback;
#if DMA_MODE_TEST
    config->enableDMA = TRUE;
    config->mulDmaTrans = FALSE;
#endif

    HAL_HASH_Init(hHash, config);

    HAL_HASH_Start(hHash, hmac_test_buf, strlen(hmac_test_buf));
#if DMA_MODE_TEST
    while (dataTransDone != TRUE)
    {
        HAL_DelayMs(1);
    }
#endif
    dataTransDone = FALSE;
    HAL_HASH_Final(hHash, hmacSum, HASH_TIMEOUT_VAL);
    if (memcmp(hmac_sum_sha256, hmacSum, 32) != 0)
    {
        printk("    HMAC SHA256 test fail.\n");
    }
    else
    {
        printk("    HMAC SHA256 test success.\n");
    }

    HAL_HASH_DeInit(hHash);

    /* SHA384 HMAC */
    config->algo = HASH_ALGO_SHA384;
    config->dataType = HASH_DATATYPE_8B;
    config->mode = HASH_ALGOMODE_HMAC;
    config->callback = HASH_Callback;
#if DMA_MODE_TEST
    config->enableDMA = TRUE;
    config->mulDmaTrans = FALSE;
#endif

    HAL_HASH_Init(hHash, config);

    HAL_HASH_Start(hHash, hmac_test_buf, strlen(hmac_test_buf));
#if DMA_MODE_TEST
    while (dataTransDone != TRUE)
    {
        HAL_DelayMs(1);
    }
#endif
    dataTransDone = FALSE;
    HAL_HASH_Final(hHash, hmacSum, HASH_TIMEOUT_VAL);
    if (memcmp(hmac_sum_sha384, hmacSum, 48) != 0)
    {
        printk("    HMAC SHA384 test fail.\n");
    }
    else
    {
        printk("    HMAC SHA384 test success.\n");
    }

    HAL_HASH_DeInit(hHash);

    /* SHA512 HMAC */
    config->algo = HASH_ALGO_SHA512;
    config->dataType = HASH_DATATYPE_8B;
    config->mode = HASH_ALGOMODE_HMAC;
    config->callback = HASH_Callback;
#if DMA_MODE_TEST
    config->enableDMA = TRUE;
    config->mulDmaTrans = FALSE;
#endif

    HAL_HASH_Init(hHash, config);

    HAL_HASH_Start(hHash, hmac_test_buf, strlen(hmac_test_buf));
#if DMA_MODE_TEST
    while (dataTransDone != TRUE)
    {
        HAL_DelayMs(1);
    }
#endif
    dataTransDone = FALSE;
    HAL_HASH_Final(hHash, hmacSum, HASH_TIMEOUT_VAL);
    if (memcmp(hmac_sum_sha512, hmacSum, 64) != 0)
    {
        printk("    HMAC SHA512 test fail.\n");
    }
    else
    {
        printk("    HMAC SHA512 test success.\n");
    }

    HAL_HASH_DeInit(hHash);
}


void HASH_Swap_test(HASH_Handle_T *hHash, HASH_Config_T *config)
{
    uint32_t i = 0, j = 0;
    uint8_t buf[1024] __attribute__((aligned(4)));
    uint8_t sha1Sum[20];

    memset(config, 0, sizeof(HASH_Config_T));
    config->algo = HASH_ALGO_SHA1;
    config->mode = HASH_ALGOMODE_HASH;
    config->callback = HASH_Callback;
#if DMA_MODE_TEST
    config->enableDMA = TRUE;
    config->mulDmaTrans = FALSE;
#endif

    printk("  SHA-1 HASH_DATATYPE_32B test: ");
    config->dataType = HASH_DATATYPE_32B;
    HAL_HASH_Init(hHash, config);
    HAL_HASH_Start(hHash, (uint8_t *)sha1_tc0_msg, 3);
#if 1
    while (dataTransDone != TRUE)
    {
        HAL_DelayMs(1);
    }
#endif
    dataTransDone = FALSE;
    memset(sha1Sum, 0, sizeof(sha1Sum));
    HAL_HASH_Final(hHash, sha1Sum, HASH_TIMEOUT_VAL);
    if (memcmp(sha1Sum, (uint8_t *) sha1_tc0_digest, 20) != 0)
    {
        printk("fail.\n");
    }
    else
    {
        printk("passes.\n");
    }

    HAL_HASH_DeInit(hHash);

    printk("  SHA-1 HASH_DATATYPE_16B test: ");
    config->dataType = HASH_DATATYPE_16B;
    HAL_HASH_Init(hHash, config);
    HAL_HASH_Start(hHash, (uint8_t *)sha1_tc1_msg, 3);
#if 1
    while (dataTransDone != TRUE)
    {
        HAL_DelayMs(1);
    }
#endif
    dataTransDone = FALSE;
    memset(sha1Sum, 0, sizeof(sha1Sum));
    HAL_HASH_Final(hHash, sha1Sum, HASH_TIMEOUT_VAL);
    if (memcmp(sha1Sum, (uint8_t *) sha1_tc1_digest, 20) != 0)
    {
        printk("fail.\n");
    }
    else
    {
        printk("passes.\n");
    }

    HAL_HASH_DeInit(hHash);

    printk("  SHA-1 HASH_DATATYPE_8B test: ");
    config->dataType = HASH_DATATYPE_8B;
    HAL_HASH_Init(hHash, config);
    HAL_HASH_Start(hHash, (uint8_t *)sha1_tc2_msg, 3);
#if 1
    while (dataTransDone != TRUE)
    {
        HAL_DelayMs(1);
    }
#endif
    dataTransDone = FALSE;
    memset(sha1Sum, 0, sizeof(sha1Sum));
    HAL_HASH_Final(hHash, sha1Sum, HASH_TIMEOUT_VAL);
    if (memcmp(sha1Sum, (uint8_t *) sha1_tc2_digest, 20) != 0)
    {
        printk("fail.\n");
    }
    else
    {
        printk("passes.\n");
    }

    HAL_HASH_DeInit(hHash);

    printk("  SHA-1 HASH_DATATYPE_1B test: ");
    config->dataType = HASH_DATATYPE_1B;
    HAL_HASH_Init(hHash, config);
    HAL_HASH_Start(hHash, (uint8_t *)sha1_tc3_msg, 3);
#if 1
    while (dataTransDone != TRUE)
    {
        HAL_DelayMs(1);
    }
#endif
    dataTransDone = FALSE;
    memset(sha1Sum, 0, sizeof(sha1Sum));
    HAL_HASH_Final(hHash, sha1Sum, HASH_TIMEOUT_VAL);
    if (memcmp(sha1Sum, (uint8_t *) sha1_tc3_digest, 20) != 0)
    {
        printk("fail.\n");
    }
    else
    {
        printk("passes.\n");
    }

    HAL_HASH_DeInit(hHash);
}




int main(void)
{
    uint32_t addr = 0x50;
    uint32_t data;

#if 0
    sha1_test_buf[0][0] = 'a';
    sha1_test_buf[0][1] = 'b';
    sha1_test_buf[0][2] = 'c';

    strcpy(&sha1_test_buf[1][0], "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
#endif
    //gm6721_sdram_init();

    //sha1_test_buf = (uint32_t)0xc0010100;
    //*((uint32_t *)sha1_test_buf) = 0x00636261;
    //memcpy((void *)(*sha1_test_buf), (void *)"abc", 3);
    //strcpy((char *)sha1_test_buf, "abc");
    //memcpy(sha1_test_buf[1], "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);


#if 0
    addr = 0xC0;
    //OTP_Program(addr, hmac_key, 4);
    data = OTP_Read(addr);

    /* CRYP_DBG_EN shadow register */
    //addr = 0x1ffff0c8;
    //*((volatile uint32_t *)addr) = 0xFFFFFFF0;

    printk("HMAC test start:\n");
    HASH_HMAC_test(&hashHandle, &config);
    printk("HMAC test end:\n");
#else
    printk("SHA1 test start:\n");
    HASH_SHA1_test(&hashHandle, &config);
    printk("SHA1 test end:\n");

    printk("SHA224/256 test start:\n");
    HASH_SHA256_test(&hashHandle, &config);
    printk("SHA224/256 test end:\n");

    printk("SHA384/512 test start:\n");
    HASH_SHA512_test(&hashHandle, &config);
    printk("SHA384/512 test end:\n");

    printk("HMAC test start:\n");
    HASH_HMAC_test(&hashHandle, &config);
    printk("HMAC test end:\n");

    printk("HASH swap test start:\n");
    HASH_Swap_test(&hashHandle, &config);
    printk("HASH swap test end:\n");

#endif
    return 0;
}
