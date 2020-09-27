#include "storage.h"
#include "ai_common.h"
#include <gm_hal_sqi.h>
#include <ugelis.h>
#include <device.h>
#include <board.h>
#include <misc/util.h>


static SQI_Device_T *pSQIDev = (SQI_Device_T *)EFLASH_CTRL_BASE;
static struct SQI_Handle  gSQIHandle;
static struct SQI_Config  gSQIConfig;

int ai_init_flash(void)
{
    RET_CODE    ret;
    ret  = HAL_SQI_Init(&gSQIHandle, pSQIDev, &gSQIConfig);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

void ai_init_storage(void)
{
    ai_init_flash();
}

int ai_flash_read_ops(unsigned int ops_offset, unsigned int ops_len, unsigned int img_offset, unsigned int img_len,  unsigned char *ops_data, unsigned char *image_pic, int flag)
{
    RET_CODE    ret;
    int i = 0;

    ret  = HAL_SQI_Read(&gSQIHandle, ops_offset, (void *)ops_data, ops_len);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }


    ret  = HAL_SQI_Read(&gSQIHandle, img_offset, (void *)image_pic, img_len);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

int ai_load_ops_data(unsigned int ops_offset, unsigned int ops_len, unsigned int img_offset, unsigned int img_len, unsigned char *ops_data, unsigned char *image)
{
    return ai_flash_read_ops(ops_offset, ops_len, img_offset, img_len, ops_data, image, 1);
}
