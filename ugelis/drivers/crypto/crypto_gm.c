/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file            crypto_gm.c
 *
 * @author        terry.wu 
 *
 * @version       1.0.0
 * 
 * @date           2019/03/19 
 *    
 * @brief           Implementation of trng Driver.
 * 
 * @note
 *    2019/03/19, terry.wu, V1.0.0
 *        Initial version.
 *    
 */


#include <kernel.h>
#include <crypto/cipher.h>
#include <gm_hal_crypto.h>
#include <misc/__assert.h>
#include <misc/util.h>
#include <string.h>

#if !defined(CONFIG_SOC_SERIES_GM6721X)
#error CRYPTO only available on GM6721x series
#else
static CRYP_Handle_T crypHandle;
static CRYP_Config_T crypConfig;

struct crypto_device_data {
	CRYP_Handle_T * handle;
	CRYP_Config_T * config;
	u8_t cipher_op;
	crypto_completion_cb callback;
};

#define DEV_DATA(dev) \
	((struct crypto_device_data *)((dev)->driver_data))


static u32_t crypto_map_algo(u32_t app_algo)
{
	u32_t hal_algo = 0xFFFFFFFF;

	if (app_algo == CRYPTO_CIPHER_ALGO_AES)
	{
		hal_algo = CRYP_ALGO_AES;
	}
	else if (app_algo == CRYPTO_CIPHER_ALGO_DES)
	{
		hal_algo = CRYP_ALGO_DES;
	}
	else if (app_algo == CRYPTO_CIPHER_ALGO_TDES)
	{
		hal_algo = CRYP_ALGO_TDES;
	}
	else
	{
		printk("Unsupport algo : %d.\n", app_algo);
	}

	return hal_algo;
}


static u32_t crypto_map_cipher_mode(u32_t app_mode)
{
	u32_t hal_mode = 0xFFFFFFFF;

	if (app_mode == CRYPTO_CIPHER_MODE_ECB)
	{
		hal_mode = CRYP_ALGOMODE_ECB;
	}
	else if (app_mode == CRYPTO_CIPHER_MODE_CBC)
	{
		hal_mode = CRYP_ALGOMODE_CBC;
	}
	else if (app_mode == CRYPTO_CIPHER_MODE_CTR)
	{
		hal_mode = CRYP_ALGOMODE_CTR;
	}
	else if (app_mode == CRYPTO_CIPHER_MODE_CCM)
	{
		hal_mode = CRYP_ALGOMODE_CCM;
	}
	else if (app_mode == CRYPTO_CIPHER_MODE_GCM)
	{
		hal_mode = CRYP_ALGOMODE_GCM;
	}
	else
	{
		printk("Unsupport cipher mode : %d.\n", app_mode);
	}

	return hal_mode;
}


static u32_t crypto_map_keytype(u32_t app_keytype)
{
	u32_t hal_keytype = 0xFFFFFFFF;

	if (app_keytype == CRYPTO_CIPHER_KEYFROM_REG)
	{
		hal_keytype = CRYP_KEYFROM_REG;
	}
	else if (app_keytype == CRYPTO_CIPHER_KEYFROM_OTP)
	{
		hal_keytype = CRYP_KEYFROM_OTP;
	}
	else if (app_keytype == CRYPTO_CIPHER_KEYFROM_KEYBUF)
	{
		hal_keytype = CRYP_KEYFROM_KEYBUF;
	}
	else
	{
		printk("Unsupport key type : %d.\n", app_keytype);
	}

	return hal_keytype;
}


static u32_t crypto_map_datatype(u32_t app_datatype)
{
	u32_t hal_datatype = 0xFFFFFFFF;

	if (app_datatype == CRYP_CIPHER_DATATYPE_32B)
	{
		hal_datatype = CRYP_DATATYPE_32B;
	}
	else if (app_datatype == CRYP_CIPHER_DATATYPE_16B)
	{
		hal_datatype = CRYP_DATATYPE_16B;
	}
	else if (app_datatype == CRYP_CIPHER_DATATYPE_8B)
	{
		hal_datatype = CRYP_DATATYPE_8B;
	}
	else if (app_datatype == CRYP_CIPHER_DATATYPE_1B)
	{
		hal_datatype = CRYP_DATATYPE_1B;
	}
	else
	{
		printk("Unsupport data type : %d.\n", app_datatype);
	}

	return hal_datatype;
}


static u32_t crypto_map_keylen(u32_t app_keylen)
{
	u32_t hal_keylen;

	if (app_keylen == 16)
	{
		hal_keylen = CRYP_KEYSIZE_128B;
	}
	else if (app_keylen == 24)
	{
		hal_keylen = CRYP_KEYSIZE_192B;
	}
	else if (app_keylen == 32)
	{
		hal_keylen = CRYP_KEYSIZE_256B;
	}
	else
	{
		printk("Unsupport key size : %d.\n", app_keylen);
	}

	return hal_keylen;
}


static int crypt_op(struct cipher_ctx *ctx, struct cipher_pkt *pkt)
{
	u8_t ret = 0;
	struct crypto_device_data * dev_data = DEV_DATA(ctx->device);
	CRYP_Handle_T * hCryp = (CRYP_Handle_T *)(dev_data->handle);
	CRYP_Config_T * config = (CRYP_Config_T *)(dev_data->config);
	u8_t cipher_op = dev_data->cipher_op;
	hCryp->param = (void *)pkt;  /* backup cipher packer for aysn ops */

	if (cipher_op == CRYPTO_CIPHER_OP_DECRYPT)
	{
		switch (config->algo)
		{
			case CRYP_ALGO_AES:
				ret = HAL_CRYP_AES_Decrypt(hCryp, pkt->in_buf, pkt->in_len, pkt->out_buf);
				break;
			case CRYP_ALGO_DES:
				ret = HAL_CRYP_DES_Decrypt(hCryp, pkt->in_buf, pkt->in_len, pkt->out_buf);
				break;
			case CRYP_ALGO_TDES:
				ret = HAL_CRYP_TDES_Decrypt(hCryp, pkt->in_buf, pkt->in_len, pkt->out_buf);
				break;
			default:
				printk("%s-%d :unknown algo.\n", __FUNCTION__, __LINE__);
				break;
		}
	}
	else
	{
		switch (config->algo)
		{
			case CRYP_ALGO_AES:
				ret = HAL_CRYP_AES_Encrypt(hCryp, pkt->in_buf, pkt->in_len, pkt->out_buf);
				break;
			case CRYP_ALGO_DES:
				ret = HAL_CRYP_DES_Encrypt(hCryp, pkt->in_buf, pkt->in_len, pkt->out_buf);
				break;
			case CRYP_ALGO_TDES:
				ret = HAL_CRYP_TDES_Encrypt(hCryp, pkt->in_buf, pkt->in_len, pkt->out_buf);
				break;
			default:
				printk("%s-%d :unknown algo.\n", __FUNCTION__, __LINE__);
				break;
		}
	}

	return ret;
}


static int crypto_ecb_op(struct cipher_ctx *ctx, struct cipher_pkt *pkt)
{
	if (crypt_op(ctx, pkt) != 0)
	{
		return -1;
	}

	return 0;
}


static int crypto_cbc_op(struct cipher_ctx *ctx, struct cipher_pkt *pkt,
			u8_t *iv)
{
	struct crypto_device_data * dev_data = DEV_DATA(ctx->device);
	CRYP_Config_T * config = dev_data->config;

	config->pInitVect = iv;
	if (config->algo == CRYP_ALGO_AES)
	{
		config->IVSize = CRYP_VECTSIZE_128B;
	}
	else
	{
		config->IVSize = CRYP_VECTSIZE_64B;
	}
	if (crypt_op(ctx, pkt) != 0)
	{
		return -1;
	}

	return 0;
}


static int crypto_ctr_op(struct cipher_ctx *ctx, struct cipher_pkt *pkt,
			u8_t *ctr)
{
	struct crypto_device_data * dev_data = DEV_DATA(ctx->device);
	CRYP_Config_T * config = dev_data->config;

	if ((config->algo == CRYP_ALGO_DES) || (config->algo == CRYP_ALGO_TDES))
	{
		printk("%s-%d : DES/TDES not support CTR mode.\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	config->pInitVect = ctr;
	config->IVSize = CRYP_VECTSIZE_128B;
	if (crypt_op(ctx, pkt) != 0)
	{
		return -1;
	}

	return 0;
}


static int crypto_ccm_op(struct cipher_ctx *ctx, struct cipher_aead_pkt *pkt,
			 u8_t *nonce)
{
	struct crypto_device_data * dev_data = DEV_DATA(ctx->device);
	CRYP_Config_T * config = dev_data->config;
	struct CRYP_Handle * hCryp = dev_data->handle;

	if ((config->algo == CRYP_ALGO_DES) || (config->algo == CRYP_ALGO_TDES))
	{
		printk("%s-%d : DES/TDES not support CCM mode.\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	config->pInitVect = nonce;
	config->IVSize = (u8_t)(ctx->mode_params.ccm_info.nonce_len);
	config->header = pkt->ad;
	config->headerSize = pkt->ad_len;
	config->tagSize = (u8_t)(ctx->mode_params.ccm_info.tag_len);
	config->pScratch = (u8_t *)k_malloc(config->headerSize + 21);
	if (crypt_op(ctx, pkt->pkt) != 0)
	{
		return -1;
	}

	if (pkt->pkt->in_len == 0)
	{
		HAL_CRYP_AESCCM_Final(hCryp, hCryp->pCrypOutBuffPtr + hCryp->crypOutCount, 6);
	}

	return 0;
}


static int crypto_gcm_op(struct cipher_ctx *ctx, struct cipher_aead_pkt *pkt,
			u8_t *iv)
{
	struct crypto_device_data * dev_data = DEV_DATA(ctx->device);
	CRYP_Handle_T * hCryp = dev_data->handle;
	CRYP_Config_T * config = dev_data->config;

	if ((config->algo == CRYP_ALGO_DES) || (config->algo == CRYP_ALGO_TDES))
	{
		printk("%s-%d : DES/TDES not support GCM mode.\n", __FUNCTION__, __LINE__);
		return -1;
	}

	config->pInitVect = iv;
	config->IVSize = ctx->mode_params.gcm_info.iv_len;
	config->header = pkt->ad;
	config->headerSize = pkt->ad_len;
	config->tagSize = ctx->mode_params.gcm_info.tag_len;
	config->pScratch = (u8_t *)k_malloc(config->tagSize);
	if (crypt_op(ctx, pkt->pkt) != 0)
	{
		return -1;
	}

	if (pkt->pkt->in_len == 0)
	{
		HAL_CRYP_AESGCM_Final(hCryp, hCryp->crypInCount, hCryp->pCrypOutBuffPtr + hCryp->crypOutCount, 6);
	}

	return 0;
}


void crypto_callback(struct CRYP_Handle * hCryp, CRYP_Event_T event, unsigned long param)
{
	u32_t cipher_mode = hCryp->init->algoMode;
	struct cipher_pkt * pkt = (struct cipher_pkt *)hCryp->param;
	struct device * dev = pkt->ctx->device;	
	struct crypto_device_data * dev_data = DEV_DATA(dev);
	
	switch (event)
	{
		case CRYP_EVENT_INFIFO_TRANS_DONE:
			
			break;
		case CRYP_EVENT_OUTFIFO_TRANS_DONE:
			if (cipher_mode == CRYP_ALGOMODE_CCM)
			{
				HAL_CRYP_AESCCM_Final(hCryp, hCryp->pCrypOutBuffPtr + hCryp->crypOutCount, 6);
			}
			else if (cipher_mode == CRYP_ALGOMODE_GCM)
			{
				HAL_CRYP_AESGCM_Final(hCryp, hCryp->crypInCount, hCryp->pCrypOutBuffPtr + hCryp->crypOutCount, 6);
			}
			dev_data->callback(pkt, 0);
			break;
		default:
			printk("unknown event.\n");
			break;
	}
}


static int crypto_gm_init(struct device * dev)
{
	/* nothing here */
	return 0;
}


static int crypto_gm_query_caps(struct device * dev)
{
	return (CAP_OPAQUE_KEY_HNDL | CAP_RAW_KEY | CAP_SEPARATE_IO_BUFS | CAP_ASYNC_OPS);
}

static void gm_crypto_isr(void *arg)
{
	ARG_UNUSED(arg);

    HAL_CRYP_IRQHandler();
}

int crypto_gm_session_setup(struct device *dev, struct cipher_ctx *ctx,
		enum cipher_algo algo, enum cipher_mode mode, enum cipher_op op_type)
{
	struct crypto_device_data * dev_data = DEV_DATA(dev);
	CRYP_Handle_T * hCryp = (CRYP_Handle_T *)(dev_data->handle);
	CRYP_Config_T * config = (CRYP_Config_T *)(dev_data->config);
	
	memset(config, 0, sizeof(CRYP_Config_T));
	config->algo = crypto_map_algo(algo);
	config->algoMode = crypto_map_cipher_mode(mode);
	config->dataType = crypto_map_datatype(ctx->data_type);
	config->keySel = crypto_map_keytype(ctx->keyfrom);
	config->keySize = crypto_map_keylen(ctx->keylen);
	if (config->keySel == CRYP_KEYFROM_REG)
	{
		config->pKey = ctx->key.bit_stream;
	}
	else
	{
		config->keyPos = *((u32_t *)(ctx->key.handle));
	}
	config->enableDMA = ctx->enable_dma;
	config->callback = crypto_callback;
	dev_data->cipher_op = op_type;
	HAL_CRYP_Init(hCryp, config);
    IRQ_CONNECT(CRYP_IRQn, 0, gm_crypto_isr, 0, 0);
    irq_enable(CRYP_IRQn);
	
	
	switch (mode)
	{
		case CRYPTO_CIPHER_MODE_ECB:
			ctx->ops.block_crypt_hndlr = crypto_ecb_op;
			break;
		case CRYPTO_CIPHER_MODE_CBC:
			ctx->ops.cbc_crypt_hndlr = crypto_cbc_op;
			break;
		case CRYPTO_CIPHER_MODE_CTR:
			ctx->ops.ctr_crypt_hndlr = crypto_ctr_op;
			break;
		case CRYPTO_CIPHER_MODE_CCM:
			ctx->ops.ccm_crypt_hndlr = crypto_ccm_op;
			break;
		case CRYPTO_CIPHER_MODE_GCM:
			ctx->ops.gcm_crypt_hndlr = crypto_gcm_op;
			break;
		default:
			printk("%s-%d : not support this cipher mode.\n", __FUNCTION__, __LINE__);
			break;
	}
	ctx->ops.cipher_mode = mode;

	return 0;
}


int crypto_gm_session_free(struct device *dev, struct cipher_ctx *ctx)
{
	struct crypto_device_data * dev_data = DEV_DATA(dev);
	CRYP_Handle_T * hCryp = dev_data->handle;
	CRYP_Config_T * config = dev_data->config;
	
	HAL_CRYP_DeInit(hCryp);
	
	return 0;
}


int crypto_gm_set_async_callback(struct device *dev, crypto_completion_cb cb)
{
	struct crypto_device_data * dev_data = DEV_DATA(dev);

	dev_data->callback = cb;

	return 0;
}


static const struct crypto_driver_api crypto_gm_funcs = {
	.query_hw_caps = crypto_gm_query_caps,
	.begin_session = crypto_gm_session_setup,
	.free_session = crypto_gm_session_free,
	.crypto_async_callback_set = crypto_gm_set_async_callback,
};

struct crypto_device_data crypto_gm_dev_data = {
	.handle = &crypHandle,
	.config = &crypConfig,
};

DEVICE_AND_API_INIT(crypto_gm, CONFIG_CRYPTO_GM_DEV_NAME, 
	crypto_gm_init, &crypto_gm_dev_data, NULL,
	POST_KERNEL, CONFIG_CRYPTO_INIT_PRIORITY,
	(void *)&crypto_gm_funcs);

#endif
