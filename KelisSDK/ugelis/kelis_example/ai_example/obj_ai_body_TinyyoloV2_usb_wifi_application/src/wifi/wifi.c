#include "wifi.h"
#include "ota.h"
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
static info_t info;

void wifi_transfer(ObjDetData *data)
{
    printf("wifi transfer OK!\n");
}

void wifi_init()
{
    RET_CODE    ret;
    ret  = HAL_SQI_Init(&gSQIHandle, pSQIDev, &gSQIConfig);
    if (ret != RET_OK)
    {
        return ;
    }

    printf("wifi init OK!\n");
}

void wifi_receive(void *data, uint32_t size)
{
    printf("wifi wifi_receive OK!\n");
}

int wifi_ota()
{
    RET_CODE    ret;
    int i = 0;
    void *ota_data;

    ret  = HAL_SQI_Read(&gSQIHandle, 0, (void *)&info, sizeof(info_t));
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    ota_data = (void *)k_malloc(info.system_size);

    wifi_receive(ota_data, info.system_size);

    /* copy loader to sram */
    ret  = HAL_SQI_Write(&gSQIHandle, info.system_addr, (void *)ota_data, info.system_size);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}