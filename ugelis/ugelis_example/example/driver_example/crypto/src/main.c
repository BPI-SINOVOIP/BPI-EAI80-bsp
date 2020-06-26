/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file            main.c
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 *
 * @date           2019/03/18
 *
 * @brief           Sample code of trng Driver.
 *
 * @note
 *    2019/03/18, terry.wu, V1.0.0
 *        Initial version.
 *
 */

#include <device.h>
#include <ugelis.h>
#include <string.h>
#include <crypto/cipher.h>
#include <stdio.h>
#include <misc/printk.h>
#include <device.h>

#ifdef  FALSE
    #undef  FALSE
#endif
#define FALSE   (0)

#ifdef  TRUE
    #undef  TRUE
#endif
#define TRUE    (1)


/* AES CTR mode test data */
static unsigned char aes_test_ctr_key[16] =
{
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static unsigned char aes_test_ctr_plaintext[64] =
{
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
    0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
};

static unsigned char aes_test_ctr_nonce_counter[16] =
{
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

static unsigned char aes_test_ctr_dec[64] =
{
    0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
    0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce,
    0x98, 0x06, 0xf6, 0x6b, 0x79, 0x70, 0xfd, 0xff,
    0x86, 0x17, 0x18, 0x7b, 0xb9, 0xff, 0xfd, 0xff,
    0x5a, 0xe4, 0xdf, 0x3e, 0xdb, 0xd5, 0xd3, 0x5e,
    0x5b, 0x4f, 0x09, 0x02, 0x0d, 0xb0, 0x3e, 0xab,
    0x1e, 0x03, 0x1d, 0xda, 0x2f, 0xbe, 0x03, 0xd1,
    0x79, 0x21, 0x70, 0xa0, 0xf3, 0x00, 0x9c, 0xee
};

static unsigned char aes_test_ctr_enc[64] =
{
    0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26,
    0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce,
    0x98, 0x06, 0xf6, 0x6b, 0x79, 0x70, 0xfd, 0xff,
    0x86, 0x17, 0x18, 0x7b, 0xb9, 0xff, 0xfd, 0xff,
    0x5a, 0xe4, 0xdf, 0x3e, 0xdb, 0xd5, 0xd3, 0x5e,
    0x5b, 0x4f, 0x09, 0x02, 0x0d, 0xb0, 0x3e, 0xab,
    0x1e, 0x03, 0x1d, 0xda, 0x2f, 0xbe, 0x03, 0xd1,
    0x79, 0x21, 0x70, 0xa0, 0xf3, 0x00, 0x9c, 0xee
};


/*
 * The data is the same for all tests, only the used length changes
 */
static unsigned char ccm_key[16] =
{
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
};

static unsigned char ccm_iv[12] =
{
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b
};

static unsigned char ccm_ad[20] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13
};

static unsigned char ccm_msg[24] =
{
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
};

static unsigned char ccm_res[32] =
{
    0xe3, 0xb2, 0x01, 0xa9, 0xf5, 0xb7, 0x1a, 0x7a,
    0x9b, 0x1c, 0xea, 0xec, 0xcd, 0x97, 0xe7, 0x0b,
    0x61, 0x76, 0xaa, 0xd9, 0xa4, 0x42, 0x8a, 0xa5,
    0x48, 0x43, 0x92, 0xfb, 0xc1, 0xb0, 0x99, 0x51
};


static u32_t cap_flags;
static u8_t op_done;

static void print_buffer_comparison(u8_t *wanted_result,
                                    u8_t *result, size_t length)
{
    int i, j;

    printk("Was waiting for: \n");

    for (i = 0, j = 1; i < length; i++, j++)
    {
        printk("0x%02x ", wanted_result[i]);

        if (j == 10)
        {
            printk("\n");
            j = 0;
        }
    }

    printk("\n But got:\n");

    for (i = 0, j = 1; i < length; i++, j++)
    {
        printk("0x%02x ", result[i]);

        if (j == 10)
        {
            printk("\n");
            j = 0;
        }
    }

    printk("\n");
}

int validate_hw_compatibility(struct device *dev)
{
    u32_t flags = 0;

    flags = cipher_query_hwcaps(dev);
    if ((flags & CAP_RAW_KEY) == 0)
    {
        printk(" Please provision the key separately "
               "as the module doesnt support a raw key");
        return -1;
    }

    if ((flags & CAP_ASYNC_OPS) == 0)
    {
        printk("The app assumes sync semantics. "
               "Please rewrite the app accordingly before proceeding");
        return -1;
    }

    if ((flags & CAP_SEPARATE_IO_BUFS) == 0)
    {
        printk("The app assumes distinct IO buffers. "
               "Please rewrite the app accordingly before proceeding");
        return -1;
    }

    cap_flags = CAP_RAW_KEY | CAP_ASYNC_OPS | CAP_SEPARATE_IO_BUFS;

    return 0;

}

void completion_cb(struct cipher_pkt *completed, int status)
{
    op_done = TRUE;
}


void aes_ctr_mode_demo(struct device *dev)
{
    u8_t encrypted[64] = {0};
    u8_t decrypted[64] = {0};

    struct cipher_ctx ini =
    {
        .keylen = sizeof(aes_test_ctr_key),
        .keyfrom = CRYPTO_CIPHER_KEYFROM_REG,
        .key.bit_stream = aes_test_ctr_key,
        .data_type = CRYP_CIPHER_DATATYPE_8B,
        .enable_dma = FALSE,
        .flags = cap_flags,
        /*  ivlen + ctrlen = keylen , so ctrlen is 128 - 96 = 32 bits */
        .mode_params.ctr_info.ctr_len = 32,
    };

    struct cipher_pkt encrypt =
    {
        .in_buf = aes_test_ctr_plaintext,
        .in_len = sizeof(aes_test_ctr_plaintext),
        .out_buf = encrypted,
        .out_buf_max = sizeof(encrypted),
    };

    struct cipher_pkt decrypt =
    {
        .in_buf = aes_test_ctr_dec,
        .in_len = sizeof(aes_test_ctr_dec),
        .out_buf = decrypted,
        .out_buf_max = sizeof(decrypted),
    };


    op_done = FALSE;
    if (cipher_begin_session(dev, &ini, CRYPTO_CIPHER_ALGO_AES, CRYPTO_CIPHER_MODE_CTR, CRYPTO_CIPHER_OP_ENCRYPT))
    {
        printk("AES CTR mode begin session fail.\n");
        return;
    }
    cipher_callback_set(dev, completion_cb);

    if (cipher_ctr_op(&ini, &encrypt, aes_test_ctr_nonce_counter))
    {
        printk("AES CTR mode encrypt fail.\n");
        return;
    }

    while (op_done == FALSE)
    {
        k_sleep(1);
    }
    if (memcmp(encrypt.out_buf, aes_test_ctr_enc, sizeof(aes_test_ctr_enc)))
    {
        printk("CTR mode ENCRYPT - Mismatch between expected "
               "and returned cipher text");
        print_buffer_comparison(aes_test_ctr_enc, encrypt.out_buf,
                                sizeof(aes_test_ctr_enc));
        goto out;
    }
    printk("AES CTR mode ENCRYPT - Match\n");
    cipher_free_session(dev, &ini);

    op_done = FALSE;
    if (cipher_begin_session(dev, &ini, CRYPTO_CIPHER_ALGO_AES, CRYPTO_CIPHER_MODE_CTR, CRYPTO_CIPHER_OP_DECRYPT))
    {
        return;
    }
    cipher_callback_set(dev, completion_cb);

    if (cipher_ctr_op(&ini, &decrypt, aes_test_ctr_nonce_counter))
    {
        printk("AES CTR mode decrypt fail.\n");
        goto out;
    }

    while (op_done == FALSE)
    {
        k_sleep(1);
    }
    if (memcmp(decrypt.out_buf, aes_test_ctr_plaintext, sizeof(aes_test_ctr_plaintext)))
    {
        printk("CTR mode DECRYPT - Mismatch between plaintext "
               "and decypted cipher text");
        print_buffer_comparison(aes_test_ctr_plaintext,
                                decrypt.out_buf, sizeof(aes_test_ctr_plaintext));
        goto out;
    }

    printk("CTR mode DECRYPT - Match\n");

out:
    cipher_free_session(dev, &ini);
    return;
}


void aes_ccm_mode_demo(struct device *dev)
{
    u8_t encrypted[32] = {0};
    u8_t decrypted[32] = {0};

    struct cipher_ctx ini =
    {
        .keylen = sizeof(ccm_key),
        .keyfrom = CRYPTO_CIPHER_KEYFROM_REG,
        .key.bit_stream = ccm_key,
        .data_type = CRYP_CIPHER_DATATYPE_8B,
        .enable_dma = FALSE,
        .flags = cap_flags,
        .mode_params.ccm_info = {
            .tag_len = 8,
            .nonce_len = sizeof(ccm_iv),
        }
    };

    struct cipher_pkt encrypt =
    {
        .in_buf = ccm_msg,
        .in_len = sizeof(ccm_msg),
        .out_buf = encrypted,
        .out_buf_max = sizeof(encrypted),
    };

    struct cipher_aead_pkt ccm_op =
    {
        .pkt = &encrypt,
        .ad = ccm_ad,
        .ad_len = sizeof(ccm_ad),
        .tag = encrypted + sizeof(ccm_msg),
    };

    struct cipher_pkt decrypt =
    {
        .in_buf = ccm_res,
        .in_len = sizeof(ccm_res) - ini.mode_params.ccm_info.tag_len,
        .out_buf = decrypted,
        .out_buf_max = sizeof(decrypted),
    };


    op_done = FALSE;
    if (cipher_begin_session(dev, &ini, CRYPTO_CIPHER_ALGO_AES, CRYPTO_CIPHER_MODE_CCM, CRYPTO_CIPHER_OP_ENCRYPT))
    {
        printk("AES CCM mode begin session fail.\n");
        return;
    }
    cipher_callback_set(dev, completion_cb);

    ccm_op.pkt = &encrypt;
    if (cipher_ccm_op(&ini, &ccm_op, ccm_iv))
    {
        printk("AES CCM mode encrypt fail.\n");
        return;
    }

    while (op_done == FALSE)
    {
        k_sleep(1);
    }
    if (memcmp(encrypt.out_buf, ccm_res, encrypt.in_len + ini.mode_params.ccm_info.tag_len))
    {
        printk("CCM mode ENCRYPT - Mismatch between expected "
               "and returned cipher text");
        print_buffer_comparison(ccm_res, encrypt.out_buf,
                                encrypt.in_len + ini.mode_params.ccm_info.tag_len);
        goto out;
    }
    printk("AES CCM mode ENCRYPT - Match\n");
    cipher_free_session(dev, &ini);

    op_done = FALSE;
    if (cipher_begin_session(dev, &ini, CRYPTO_CIPHER_ALGO_AES, CRYPTO_CIPHER_MODE_CCM, CRYPTO_CIPHER_OP_DECRYPT))
    {
        return;
    }
    cipher_callback_set(dev, completion_cb);

    ccm_op.pkt = &decrypt;
    if (cipher_ccm_op(&ini, &ccm_op, ccm_iv))
    {
        printk("AES CCM mode decrypt fail.\n");
        goto out;
    }

    while (op_done == FALSE)
    {
        k_sleep(1);
    }
    if (memcmp(decrypt.out_buf, ccm_msg, sizeof(ccm_msg)))
    {
        printk("CCM mode DECRYPT - Mismatch between plaintext "
               "and decypted cipher text\n");
        print_buffer_comparison(ccm_msg,
                                decrypt.out_buf, sizeof(ccm_msg));
        goto out;
    }
    if (memcmp(decrypt.out_buf + decrypt.in_len, ccm_res + decrypt.in_len, ini.mode_params.ccm_info.tag_len))
    {
        printk("CCM digest fail.\n");
        goto out;
    }

    printk("CCM mode DECRYPT - Match\n");

out:
    cipher_free_session(dev, &ini);
    return;
}


struct mode_test
{
    const char *mode;
    void (*mode_func)(struct device *dev);
};

void main(void)
{
    struct device *dev = device_get_binding(CONFIG_CRYPTO_GM_DEV_NAME);
    const struct mode_test modes[] =
    {
        { .mode = "CTR Mode", .mode_func = aes_ctr_mode_demo },
        { .mode = "CCM Mode", .mode_func = aes_ccm_mode_demo },
        { },
    };
    int i;

    if (!dev)
    {
        printk("%s pseudo device not found\n", CONFIG_CRYPTO_GM_DEV_NAME);
        return;
    }

    if (validate_hw_compatibility(dev))
    {
        printk("Incompatible h/w");
        return;
    }

    printk("Cipher Sample:\n");

    for (i = 0; modes[i].mode; i++)
    {
        printk("%s\n", modes[i].mode);
        modes[i].mode_func(dev);
    }
}
