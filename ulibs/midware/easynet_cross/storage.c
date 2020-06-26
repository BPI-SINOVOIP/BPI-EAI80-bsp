#include "storage.h"
//#include "ai_common.h"
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

int aimodule_init_device(void)
{
    HAL_Reset_Module(RESET_STF_SW_RSTJ);
    HAL_Reset_Module(RESET_KDPSRAM_SW_RSTJ);//*(volatile uint32_t *)(0x40000090) |= 3 << 0;  /*fast kdp run 300MHZ*/
    HAL_Reset_Module(RESET_KDP_SW_RSTJ);
    HAL_Reset_Module(RESET_IMP_SW_RSTJ);

    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_STF);
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_KDPSRAM);
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_KDP);
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_IMP);

    /*improve system powerformance*/
    *(volatile uint32_t *)(0x40039288) |= 0x3F << 16;

    *(volatile uint32_t *)(0x40000188) |= 1 << 0;

    HAL_CLOCK_AdjustPLLKDPDefaultDiv(2, 0, 0, 0);

    while ((*(volatile uint32_t *)(0x400001A4) & (1 << 5)) != (1 << 5));

    *(volatile uint32_t *)(0x4000009c) |= 2 << 16;

}

void ai_init_storage(void)
{
    ai_init_flash();
}

int ai_flash_read_ops(unsigned int ops_offset, unsigned int ops_len, unsigned int img_offset, unsigned int img_len,  unsigned char *ops_data, unsigned char *image_pic, int flag)
{
    RET_CODE    ret;
    int i = 0;

    /*load weight*/
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
