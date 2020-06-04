#include "storage.h"
#include "ai_common.h"
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

StorageCallback storage_callback;

int ai_flash_read(unsigned char *wei_addr, unsigned char *cmd_addr, unsigned char *base_addr)
{
    RET_CODE    ret;
    int i = 0;

    storage_callback("get weight");

    /*load weight*/
    ret  = HAL_SQI_Read(&gSQIHandle, WEI_BIN_FLASH_START, (void *)wei_addr, WEI_BIN_SIZE);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    storage_callback("get cmd");
    ret  = HAL_SQI_Read(&gSQIHandle, CMD_BIN_FLASH_START, (void *)cmd_addr, CMD_BIN_SIZE);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    storage_callback("get base");

    ret  = HAL_SQI_Read(&gSQIHandle, BAK_BIN_FLASH_START, (void *)base_addr, BAK_BIN_SIZE);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}


int ai_data_read(unsigned char *wei_addr, unsigned char *cmd_addr, unsigned char *base_addr)
{
    return ai_flash_read(wei_addr, cmd_addr, base_addr);
}

void ai_init_storage(StorageCallback sc)
{
    storage_callback = sc;

    ai_init_flash();

}
