#include <ugelis.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <gm_hal_crypto.h>


#define DMA_MODE_TEST    0


#define TDES_ENCRYPT     1
#define TDES_DECRYPT     0

static uint8_t INFIFO_TRANS_DONE = FALSE;
static uint8_t OUTFIFO_TRANS_DONE = FALSE;

/*
 * DES and 3DES test vectors from:
 *
 * http://csrc.nist.gov/groups/STM/cavp/documents/des/tripledes-vectors.zip
 */

/* DES/TDES ECB mode test data */
static const unsigned char des3_test_keys[24] =
{
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
    0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23
};

static const unsigned char des3_test_buf[8] =
{
    0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74
};

static const unsigned char des3_test_ecb_dec[3][8] =
{
    { 0xCD, 0xD6, 0x4F, 0x2F, 0x94, 0x27, 0xC1, 0x5D },
    { 0x69, 0x96, 0xC8, 0xFA, 0x47, 0xA2, 0xAB, 0xEB },
    { 0x83, 0x25, 0x39, 0x76, 0x44, 0x09, 0x1A, 0x0A }
};

static const unsigned char des3_test_ecb_enc[3][8] =
{
    { 0x6A, 0x2A, 0x19, 0xF4, 0x1E, 0xCA, 0x85, 0x4B },
    { 0x03, 0xE6, 0x9F, 0x5B, 0xFA, 0x58, 0xEB, 0x42 },
    { 0xDD, 0x17, 0xE8, 0xB8, 0xB4, 0x37, 0xD2, 0x32 }
};


/* DES/TDES CBC mode test data */
static const unsigned char des3_test_iv[8] =
{
    0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
};

static const unsigned char des3_test_cbc_dec[3][8] =
{
    { 0x12, 0x9F, 0x40, 0xB9, 0xD2, 0x00, 0x56, 0xB3 },
    { 0x47, 0x0E, 0xFC, 0x9A, 0x6B, 0x8E, 0xE3, 0x93 },
    { 0xC5, 0xCE, 0xCF, 0x63, 0xEC, 0xEC, 0x51, 0x4C }
};

static const unsigned char des3_test_cbc_enc[3][8] =
{
    { 0x54, 0xF1, 0x5A, 0xF6, 0xEB, 0xE3, 0xA4, 0xB4 },
    { 0x35, 0x76, 0x11, 0x56, 0x5F, 0xA1, 0x8E, 0x4D },
    { 0xCB, 0x19, 0x1F, 0x85, 0xD1, 0xED, 0x84, 0x39 }
};


void DES_TestCallback(struct CRYP_Handle *hCryp, CRYP_Event_T event, unsigned long param)
{
    switch (event)
    {
        case CRYP_EVENT_INFIFO_TRANS_DONE:
            INFIFO_TRANS_DONE = TRUE;
            break;
        case CRYP_EVENT_OUTFIFO_TRANS_DONE:
            OUTFIFO_TRANS_DONE = TRUE;
            break;
        default:
            printk("unknown event.\n");
            break;
    }
}


/** DES/TDES ECB mode test */
int CRYP_DESEcbTest(CRYP_Handle_T *crypHandle, CRYP_Config_T *config)
{
    int ret = 0, i, j, u, v;
    uint8_t buf[64];

    printk("DES/TDES ECB test start.\n");

    memset(config, 0, sizeof(CRYP_Config_T));
    config->algoMode = CRYP_ALGOMODE_ECB;
    config->dataType = CRYP_DATATYPE_8B;
    config->callback = DES_TestCallback;

    for (i = 0; i < 6; i++)
    {
        u = i >> 1;
        v = i  & 1;
        if (u == 1)
        {
            continue;
        }
        printk("  DES%c-ECB-%3d (%s): ",
               (u == 0) ? ' ' : '3', 56 + u * 56,
               (v == TDES_DECRYPT) ? "dec" : "enc");

        if (u == 0)
        {
            config->algo = CRYP_ALGO_DES;
        }
        else
        {
            config->algo = CRYP_ALGO_TDES;
        }

#if 1
        config->keySel = CRYP_KEYFROM_REG;
#else
#if 1
        if (config->algo == CRYP_ALGO_DES)
        {
            config->keySel = CRYP_KEYFROM_OTP;
            config->keyPos = CRYP_DES_OTP_LOW;
        }
        else
        {
            config->keySel = CRYP_KEYFROM_OTP;
            config->keyPos = CRYP_OTP_K6;//CRYP_OTP_K4;
        }
#else
        if (config->algo == CRYP_ALGO_DES)
        {
            config->keySel = CRYP_KEYFROM_KEYBUF;
            config->keyPos = CRYP_DES_KEYBUF_LOW;//CRYP_DES_KEYBUF_HIGH;
        }
        else
        {
            config->keySel = CRYP_KEYFROM_KEYBUF;
            config->keyPos = CRYP_KEYBUF_RD_K6;//CRYP_KEYBUF_RD_K0;
        }
#endif
#endif

        config->pKey = des3_test_keys;
        HAL_CRYP_Init(crypHandle, config);

        memcpy(buf, des3_test_buf, 8);
        if (v == TDES_DECRYPT)
        {
            for (j = 0; j < 10000; j++)
            {
                INFIFO_TRANS_DONE = FALSE;
                if (u == 0)
                {
                    HAL_CRYP_DES_Decrypt(crypHandle, buf, 8, buf);
                }
                else
                {
                    HAL_CRYP_TDES_Decrypt(crypHandle, buf, 8, buf);
                }
                while (INFIFO_TRANS_DONE != TRUE)
                {
                    HAL_DelayMs(1);
                }
            }
            while (OUTFIFO_TRANS_DONE != TRUE)
            {
                HAL_DelayMs(1);
            }
            OUTFIFO_TRANS_DONE = FALSE;

            if (memcmp(buf, des3_test_ecb_dec[u], 8) != 0)
            {
                printk("  DES%c-ECB-%3d (%s): fail.\n",
                       (u == 0) ? ' ' : '3', 56 + u * 56,
                       (v == TDES_DECRYPT) ? "dec" : "enc");
                ret = -1;
                //goto EXIT;
            }
            HAL_CRYP_DeInit(crypHandle);
        }
        else
        {
            for (j = 0; j < 10000; j++)
            {
                INFIFO_TRANS_DONE = FALSE;
                if (u == 0)
                {
                    HAL_CRYP_DES_Encrypt(crypHandle, buf, 8, buf);
                }
                else
                {
                    HAL_CRYP_TDES_Encrypt(crypHandle, buf, 8, buf);
                }
                while (INFIFO_TRANS_DONE != TRUE)
                {
                    HAL_DelayMs(1);
                }
            }
            while (OUTFIFO_TRANS_DONE != TRUE)
            {
                HAL_DelayMs(1);
            }
            OUTFIFO_TRANS_DONE = FALSE;

            if (memcmp(buf, des3_test_ecb_enc[u], 8) != 0)
            {
                printk("  DES%c-ECB-%3d (%s): fail.\n",
                       (u == 0) ? ' ' : '3', 56 + u * 56,
                       (v == TDES_DECRYPT) ? "dec" : "enc");
                ret = -2;
                //goto EXIT;
            }
            HAL_CRYP_DeInit(crypHandle);
        }

        printk("  DES%c-ECB-%3d (%s): pass.\n",
               (u == 0) ? ' ' : '3', 56 + u * 56,
               (v == TDES_DECRYPT) ? "dec" : "enc");
    }

EXIT:

    HAL_CRYP_DeInit(crypHandle);
    printk("DES/TDES ECB test done.\n");

    return ret;
}


/** DES/TDES CBC mode test */
int CRYP_DESCbcTest(CRYP_Handle_T *crypHandle, CRYP_Config_T *config)
{
    int ret = 0, i, j, u, v;
    uint8_t buf[64];
    unsigned char tmp[8];
    unsigned char prv[8];

    printk("DES/TDES CBC test start.\n");

    memset(config, 0, sizeof(CRYP_Config_T));
    config->algoMode = CRYP_ALGOMODE_CBC;
    config->dataType = CRYP_DATATYPE_8B;
    config->keySel = CRYP_KEYFROM_REG;
    config->callback = DES_TestCallback;
#if DMA_MODE_TEST
    //DMA mode test
    config->enableDMA = TRUE;
#endif

    for (i = 0; i < 6; i++)
    {
        u = i >> 1;
        v = i  & 1;
        if (u == 1)
        {
            continue;
        }
        printk("  DES%c-CBC-%3d (%s): ",
               (u == 0) ? ' ' : '3', 56 + u * 56,
               (v == TDES_DECRYPT) ? "dec" : "enc");

        if (u == 0)
        {
            config->algo = CRYP_ALGO_DES;
        }
        else
        {
            config->algo = CRYP_ALGO_TDES;
        }
        config->pKey = des3_test_keys;
        config->pInitVect = des3_test_iv;
        config->IVSize = CRYP_VECTSIZE_64B;
        HAL_CRYP_Init(crypHandle, config);

#if DMA_MODE_TEST
        if (u == 0)
        {
            /* DMA test */
            memcpy(buf, des3_test_buf, 8);
            HAL_CRYP_DES_Decrypt(crypHandle, buf, 8, prv);
            HAL_CRYP_DES_Encrypt(crypHandle, prv, 8, buf);
            if (memcmp(buf, des3_test_buf, 8) != 0)
            {
                ret = -1;
                printk("  DES%c-CBC-%3d (%s): fail.\n",
                       (u == 0) ? ' ' : '3', 56 + u * 56,
                       "DMA");
                //goto EXIT;
            }
        }
        else
        {
            memcpy(buf, des3_test_buf, 8);
            HAL_CRYP_TDES_Decrypt(crypHandle, buf, 8, prv);
            HAL_CRYP_TDES_Encrypt(crypHandle, prv, 8, buf);
            if (memcmp(buf, des3_test_buf, 8) != 0)
            {
                ret = -1;
                printk("  DES%c-CBC-%3d (%s): fail.\n",
                       (u == 0) ? ' ' : '3', 56 + u * 56,
                       "DMA");
                //goto EXIT;
            }
        }
#else
        memcpy(buf, des3_test_buf, 8);
        memcpy(prv, des3_test_iv,  8);
        if (v == TDES_DECRYPT)
        {
            for (j = 0; j < 10000; j++)
            {
                INFIFO_TRANS_DONE = FALSE;
                if (u == 0)
                {
                    HAL_CRYP_DES_Decrypt(crypHandle, buf, 8, buf);
                }
                else
                {
                    HAL_CRYP_TDES_Decrypt(crypHandle, buf, 8, buf);
                }
                while (INFIFO_TRANS_DONE != TRUE)
                {
                    HAL_DelayMs(1);
                }
            }
            while (OUTFIFO_TRANS_DONE != TRUE)
            {
                HAL_DelayMs(1);
            }
            OUTFIFO_TRANS_DONE = FALSE;

            if (memcmp(buf, des3_test_cbc_dec[u], 8) != 0)
            {
                printk("  DES%c-CBC-%3d (%s): fail.\n",
                       (u == 0) ? ' ' : '3', 56 + u * 56,
                       (v == TDES_DECRYPT) ? "dec" : "enc");
                ret = -1;
                //goto EXIT;
            }
            HAL_CRYP_DeInit(crypHandle);
        }
        else
        {
            for (j = 0; j < 10000; j++)
            {
                INFIFO_TRANS_DONE = FALSE;
                if (u == 0)
                {
                    HAL_CRYP_DES_Encrypt(crypHandle, buf, 8, buf);
                }
                else
                {
                    HAL_CRYP_TDES_Encrypt(crypHandle, buf, 8, buf);
                }
                while (INFIFO_TRANS_DONE != TRUE)
                {
                    HAL_DelayMs(1);
                }

                memcpy(tmp, prv, 8);
                memcpy(prv, buf, 8);
                memcpy(buf, tmp, 8);
            }
            memcpy(buf, prv, 8);

            while (OUTFIFO_TRANS_DONE != TRUE)
            {
                HAL_DelayMs(1);
            }
            OUTFIFO_TRANS_DONE = FALSE;

            if (memcmp(buf, des3_test_cbc_enc[u], 8) != 0)
            {
                printk("  DES%c-CBC-%3d (%s): fail.\n",
                       (u == 0) ? ' ' : '3', 56 + u * 56,
                       (v == TDES_DECRYPT) ? "dec" : "enc");
                ret = -2;
                //goto EXIT;
            }
            HAL_CRYP_DeInit(crypHandle);
        }
#endif

        printk("  DES%c-CBC-%3d (%s): pass.\n",
               (u == 0) ? ' ' : '3', 56 + u * 56,
               (v == TDES_DECRYPT) ? "dec" : "enc");
    }

EXIT:

    HAL_CRYP_DeInit(crypHandle);
    printk("DES/TDES CBC test done.\n");

    return ret;
}


