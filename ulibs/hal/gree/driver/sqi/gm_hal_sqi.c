/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_sqi.c
 *
 * @author        Colin.Yang
 *
 * @version       1.0.0
 *
 * @date          2018/03/24
 *
 * @brief
 *
 * @author        steven.lee
 *
 * @version       2.0.0
 *
 * @date          2019/11/28
 * @note
 *    2018/03/24, Colin.Yang, V1.0.0
 *        Initial version.
  *    2019/11/28, Steven.lee, V2.0.0
 *        enhance version.
 */
//#include <gm_soc.h>
#include <gm_hal_sqi.h>
#include "gm_hal_sqi_host.h"
#include <gm_hal_dma.h>
#ifdef RABBIT_NO_OS_SYSTEM
    #include <systick.h>
#else
    #include <ugelis.h>
#endif


static uint32_t sqi_mt_size[6] =
{
    SQI_8M_SIZE,
    SQI_16M_SIZE,
    SQI_32M_SIZE,
    SQI_64M_SIZE,
    SQI_128M_SIZE,
    SQI_256M_SIZE,
};


static uint32_t sqi_mx_size[7] =
{
    SQI_1M_SIZE,
    SQI_2M_SIZE,
    SQI_4M_SIZE,
    SQI_8M_SIZE,
    SQI_16M_SIZE,
    SQI_32M_SIZE,
    SQI_64M_SIZE,
};

static uint32_t sqi_gd_size[2] =
{
    SQI_16M_SIZE,
    SQI_32M_SIZE,
};

static uint32_t sqi_wb_size[2] =
{
    SQI_16M_SIZE,
    SQI_32M_SIZE,
};

static char *model[] =
{
    "MX25L1605D",
    "MX25L3205D",
    "MX25L6433F",
    "MX25L12835F",
    "MX25L25633F ",
    "MX25L51245G",
    "MT25QL128ABA",
    "MT25QL256ABA",
    "GD25Q127C",
    "GD25Q257D",
    "W25Q128JV",
    "W25Q256JV",
    "KH25V8035F"
};


static uint8_t g_tmpbuf[SFLASH_SECTOR_LEN];

/*DMA*/
static DMA_Handle_T s_dmaHandle;
static volatile  uint32_t s_dmaComplete = 0;
static volatile  uint32_t s_dmaError = 0;

static void dmaCallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        /* DMA transaction completes */
        s_dmaComplete = 1;
    }
    else if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        /* Error occurs during DMA transaction */
        s_dmaError = 1;
    }
}

static RET_CODE HAL_SQI_DMA_Config(void)
{
    RET_CODE ret = RET_ERROR;
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_M2M, &reqResult);
    if (ret != RET_OK)
    {
        return ret;
    }
    memset(&dmaConfig, 0, sizeof(dmaConfig));
    dmaConfig.channelIdx = reqResult.channelIdx;
    dmaConfig.deviceIdx = reqResult.deviceIdx;
    dmaConfig.direction = DMA_MEMORY_TO_MEMORY;
    dmaConfig.increment    = DMA_SRC_INC_DST_INC;
    dmaConfig.width        = DMA_WIDTH_32BIT;
    dmaConfig.priority     = DMA_PRIORITY_LOW;//DMA_PRIORITY_HIGH;
    dmaConfig.burst        = DMA_BURST_1;
    dmaConfig.alter        = DMA_PRIMARY;//DMA_ALTER;
    dmaConfig.callback = dmaCallback;
    ret = HAL_DMA_Init(DMA_DEV, &dmaConfig, &s_dmaHandle);
    return ret;
}






/*============================================MX25 Function=========================================================*/


/**
 * @brief     Read data by DMA mode
 * @param  hSQI        SQI device handle
 * @param  offset        address offset,address = 0x10000000 + offset
 * @param  buf            The data memory
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR   read data failed
 * @retval RET_TIMEOUT   read data is timeout
 * @retval RET_OK         Read data  successfully
 */

static RET_CODE HAL_SQI_MX25_DMA_4LineRead(SQI_Handle_T *hSQI, uint32_t offset, void *buff, uint32_t length)
{
    SQI_Device_T *pSQIDev;
    RET_CODE ret = 0;
    uint32_t no_4bit_num_end = 0;
    uint32_t no_4bit_num_start = 0;
    uint32_t buffend;
    uint32_t buffdma;
    int timeout = SQI_TIMEOUT;
    s_dmaComplete = 0;
    s_dmaError = 0 ;


    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }

    if ((offset % 4) || ((uint32_t)buff % 4))
    {
        if (((uint32_t)buff % 4) != (offset % 4))
        {
            printf("error: 1.offset address or buff address is not 4byte alignment,please input a 4byte alignment address\n  \
					           2.buff address and offset is not continue\n");
            return RET_ERROR;
        }
        else
        {
            no_4bit_num_start = 4 - (offset % 4);
            no_4bit_num_end   = (length - no_4bit_num_start) % 4;
        }

    }
    else
    {
        no_4bit_num_end = length % 4;
    }

    ret = HAL_SQI_MX25_4LineRead_Enable(pSQIDev, length, hSQI->flashsize);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    if (no_4bit_num_start)
    {
        memcpy((buff), (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), no_4bit_num_start);
    }
    buffdma = (uint32_t)(buff) + (uint32_t)(no_4bit_num_start);
    HAL_DMA_Start(&s_dmaHandle, (uint32_t)(CONFIG_SQI_SFLASH_MMAP_BASE + offset + no_4bit_num_start), buffdma, (length  - no_4bit_num_end - no_4bit_num_start) / 4);
    while ((timeout > 0) && (s_dmaComplete == 0) && (s_dmaError == 0))
    {
        HAL_SQI_Time_Delayms(10);
        timeout--;
    }
    if (s_dmaComplete == 1)
    {
        ret = RET_OK;
    }
    else if (s_dmaError == 1)
    {
        ret =  RET_ERROR;
    }
    else if (timeout < 1)
    {
        ret =  RET_TIMEOUT;
    }
    if (no_4bit_num_end)
    {
        buffend = (uint32_t)(buff) + (uint32_t)(length - no_4bit_num_end - no_4bit_num_start);
        memcpy((void *)(buffend), (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + length - no_4bit_num_end - no_4bit_num_start), no_4bit_num_end);
    }
    HAL_SQI_Wait_Idle(pSQIDev);
    return ret;


}



/**
 * @brief  SQI Flash  MX25 read
 * @param  hSQI:    SQI Device Handle.
 * @param  offset:   addres offset
 * @param  buffer:   read data buff
 * @param  size:     read size
 * @retval read statues
 */
static RET_CODE HAL_SQI_MX25_4Line_Read(SQI_Handle_T *hSQI, uint32_t offset, uint8_t *buffer, uint32_t size)
{

    SQI_Device_T *pSQIDev;
    RET_CODE      ret;


    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }

    if (hSQI->pConfig->read_mode == SQI_READ_BY_CPU)
    {

        ret = HAL_SQI_MX25_4LineRead_Enable(pSQIDev, size, hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        memcpy(buffer, (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), size);
        HAL_SQI_Wait_Idle(pSQIDev);
    }
    else
    {
        ret = HAL_SQI_MX25_DMA_4LineRead(hSQI, offset, buffer, size);
        if (ret != RET_OK)
        {
            return ret;
        }
    }

    return RET_OK;
}



/**
 * @brief  SQI Flash write on 4Line mode.
 * @param  pSQIDev:  SQI Device.
 * @param  offset:  the address of flash.
 * @param  buffer:  the data will write to flash.
 * @param  len:  write size.
 * @retval HAL status
 */
static RET_CODE HAL_SQI_MX25_4Line_Write(SQI_Handle_T *hSQI, uint32_t offset, const void *buffer, uint32_t len)
{
    RET_CODE        ret;
    uint32_t            *tmpbuf;
    uint32_t            *tmpaddr ;
    int             pagecnt;
    uint32_t        endpagebyte;
    uint32_t        startpagebyte;
    SQI_Device_T *pSQIDev;

    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }


    if (offset > 0)
    {
        startpagebyte = hSQI->pagesize - (offset % hSQI->pagesize);
    }
    else
    {
        startpagebyte = 0;
    }

    if ((len < startpagebyte))
    {
        ret = HAL_SQI_MX25_4LineWrite_Enable(pSQIDev, len, hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), buffer, len);
        HAL_SQI_Wait_Idle(pSQIDev);
        return RET_OK;
    }

    if ((len - startpagebyte) < hSQI->pagesize)
    {

        if (startpagebyte)
        {
            ret = HAL_SQI_MX25_4LineWrite_Enable(pSQIDev, startpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), buffer, startpagebyte);
        }
        ret = HAL_SQI_MX25_4LineWrite_Enable(pSQIDev, (len - startpagebyte), hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        tmpaddr = (uint32_t *)(CONFIG_SQI_SFLASH_MMAP_BASE + (uint32_t)(offset + startpagebyte));
        tmpbuf = (uint32_t *)((uint32_t)buffer + (uint32_t)startpagebyte);
        memcpy((void *)tmpaddr, (void *)tmpbuf, (len - startpagebyte));
        HAL_SQI_Wait_Idle(pSQIDev);
    }
    else
    {
        pagecnt = (len - startpagebyte) / hSQI->pagesize;
        endpagebyte = (len - startpagebyte) % hSQI->pagesize;


        if (startpagebyte)
        {
            ret = HAL_SQI_MX25_4LineWrite_Enable(pSQIDev, startpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), (void *)buffer, startpagebyte);
        }
        tmpaddr = (uint32_t *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset + startpagebyte);
        tmpbuf = (uint32_t *)((uint32_t)buffer + (uint32_t)startpagebyte);
        while (pagecnt > 0)
        {
            ret = HAL_SQI_MX25_4LineWrite_Enable(pSQIDev, hSQI->pagesize, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)tmpaddr, (void *)tmpbuf, hSQI->pagesize);
            pagecnt--;
            tmpaddr += hSQI->pagesize / 4;
            tmpbuf += hSQI->pagesize / 4;
        }

        if (endpagebyte)
        {
            ret = HAL_SQI_MX25_4LineWrite_Enable(pSQIDev, endpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)tmpaddr, (void *)tmpbuf, endpagebyte);
        }

        HAL_SQI_Wait_Idle(pSQIDev);
    }


    return RET_OK;

}





/*============================================MT25 Function=========================================================*/

/**
 * @brief  SQI Flash write on 4Line mode.
 * @param  pSQIDev:  SQI Device.
 * @param  offset:  the address of flash.
 * @param  buffer:  the data will write to flash.
 * @param  len:  write size.
 * @retval HAL status
 */
static RET_CODE HAL_SQI_MT25_4Line_Write(SQI_Handle_T *hSQI, uint32_t offset, const void *buffer, size_t len)
{
    RET_CODE        ret;
    uint32_t            *tmpbuf;
    uint32_t            *tmpaddr ;
    int             pagecnt;
    uint32_t        endpagebyte;
    uint32_t        startpagebyte;
    SQI_Device_T *pSQIDev;

    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }


    if (offset > 0)
    {
        startpagebyte = hSQI->pagesize - (offset % hSQI->pagesize);
    }
    else
    {
        startpagebyte = 0;
    }

    if ((len < startpagebyte))
    {
        ret = HAL_SQI_MT25_4LineWrite_Enable(pSQIDev, len, hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), buffer, len);
        HAL_SQI_Wait_Idle(pSQIDev);
        return RET_OK;
    }

    if ((len - startpagebyte) < hSQI->pagesize)
    {

        if (startpagebyte)
        {
            ret = HAL_SQI_MT25_4LineWrite_Enable(pSQIDev, startpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), buffer, startpagebyte);
        }
        ret = HAL_SQI_MT25_4LineWrite_Enable(pSQIDev, (len - startpagebyte), hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        tmpaddr = (uint32_t *)(CONFIG_SQI_SFLASH_MMAP_BASE + (uint32_t)(offset + startpagebyte));
        tmpbuf = (uint32_t *)((uint32_t)buffer + (uint32_t)startpagebyte);
        memcpy((void *)tmpaddr, (void *)tmpbuf, (len - startpagebyte));
        HAL_SQI_Wait_Idle(pSQIDev);
    }
    else
    {
        pagecnt = (len - startpagebyte) / hSQI->pagesize;
        endpagebyte = (len - startpagebyte) % hSQI->pagesize;


        if (startpagebyte)
        {
            ret = HAL_SQI_MT25_4LineWrite_Enable(pSQIDev, startpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), (void *)buffer, startpagebyte);
        }
        tmpaddr = (uint32_t *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset + startpagebyte);
        tmpbuf = (uint32_t *)((uint32_t)buffer + (uint32_t)startpagebyte);
        while (pagecnt > 0)
        {
            ret = HAL_SQI_MT25_4LineWrite_Enable(pSQIDev, hSQI->pagesize, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)tmpaddr, (void *)tmpbuf, hSQI->pagesize);
            pagecnt--;
            tmpaddr += hSQI->pagesize / 4;
            tmpbuf += hSQI->pagesize / 4;
        }

        if (endpagebyte)
        {
            ret = HAL_SQI_MT25_4LineWrite_Enable(pSQIDev, endpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)tmpaddr, (void *)tmpbuf, endpagebyte);
        }

        HAL_SQI_Wait_Idle(pSQIDev);
    }


    return RET_OK;

}


/**
 * @brief     Read data by DMA mode
 * @param  hSQI        SQI device handle
 * @param  offset        address offset,address = 0x10000000 + offset
 * @param  buf            The data memory
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR   read data failed
 * @retval RET_TIMEOUT   read data is timeout
 * @retval RET_OK         Read data  successfully
 */

static RET_CODE HAL_SQI_MT25_DMA_4LineRead(SQI_Handle_T *hSQI, uint32_t offset, void *buff, uint32_t length)
{
    SQI_Device_T *pSQIDev;
    RET_CODE ret = 0;
    uint32_t no_4bit_num_end = 0;
    uint32_t no_4bit_num_start = 0;
    uint32_t buffend;
    uint32_t buffdma;
    int timeout = SQI_TIMEOUT;
    s_dmaComplete = 0;
    s_dmaError = 0 ;


    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }


    if ((offset % 4) || ((uint32_t)buff % 4))
    {
        if (((uint32_t)buff % 4) != (offset % 4))
        {
            printf("error: 1.offset address or buff address is not 4byte alignment,please input a 4byte alignment address\n  \
					           2.buff address and offset is not continue\n");
            return RET_ERROR;
        }
        else
        {
            no_4bit_num_start = 4 - (offset % 4);
            no_4bit_num_end   = (length - no_4bit_num_start) % 4;
        }

    }
    else
    {
        no_4bit_num_end = length % 4;
    }

    ret = HAL_SQI_MT25_4LineRead_Enable(pSQIDev, length, hSQI->flashsize);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    if (no_4bit_num_start)
    {
        memcpy((buff), (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), no_4bit_num_start);
    }
    buffdma = (uint32_t)(buff) + (uint32_t)(no_4bit_num_start);
    HAL_DMA_Start(&s_dmaHandle, (uint32_t)(CONFIG_SQI_SFLASH_MMAP_BASE + offset + no_4bit_num_start), buffdma, (length  - no_4bit_num_end - no_4bit_num_start) / 4);
    while ((timeout > 0) && (s_dmaComplete == 0) && (s_dmaError == 0))
    {
        HAL_SQI_Time_Delayms(10);
        timeout--;
    }
    if (s_dmaComplete == 1)
    {
        ret = RET_OK;
    }
    else if (s_dmaError == 1)
    {
        ret =  RET_ERROR;
    }
    else if (timeout < 1)
    {
        ret =  RET_TIMEOUT;
    }
    if (no_4bit_num_end)
    {
        buffend = (uint32_t)(buff) + (uint32_t)(length - no_4bit_num_end - no_4bit_num_start);
        memcpy((void *)(buffend), (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + length - no_4bit_num_end - no_4bit_num_start), no_4bit_num_end);
    }
    HAL_SQI_Wait_Idle(pSQIDev);
    return ret;


}




static RET_CODE HAL_SQI_MT25_4Line_Read(SQI_Handle_T *hSQI, uint32_t offset, uint8_t *buffer, uint32_t size)
{
    SQI_Device_T *pSQIDev;
    RET_CODE      ret;

    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }

    if (hSQI->pConfig->read_mode == SQI_READ_BY_CPU)
    {

        ret = HAL_SQI_MT25_4LineRead_Enable(pSQIDev, size, hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        memcpy(buffer, (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), size);
        HAL_SQI_Wait_Idle(pSQIDev);
    }
    else
    {
        ret = HAL_SQI_MT25_DMA_4LineRead(hSQI, offset, buffer, size);
        if (ret != RET_OK)
        {
            return ret;
        }
    }

    return RET_OK;
}




/*============================================GD25 Function=========================================================*/

/**
 * @brief  SQI Flash write on 4Line mode.
 * @param  pSQIDev:  SQI Device.
 * @param  offset:  the address of flash.
 * @param  buffer:  the data will write to flash.
 * @param  len:  write size.
 * @retval HAL status
 */
static RET_CODE HAL_SQI_GD25_4Line_Write(SQI_Handle_T *hSQI, uint32_t offset, const void *buffer, size_t len)
{
    RET_CODE        ret;
    uint32_t            *tmpbuf;
    uint32_t            *tmpaddr ;
    int             pagecnt;
    uint32_t        endpagebyte;
    uint32_t        startpagebyte;
    SQI_Device_T *pSQIDev;

    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }


    if (offset > 0)
    {
        startpagebyte = hSQI->pagesize - (offset % hSQI->pagesize);
    }
    else
    {
        startpagebyte = 0;
    }

    if ((len < startpagebyte))
    {
        ret = HAL_SQI_GD25_4LineWrite_Enable(pSQIDev, len, hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), buffer, len);
        HAL_SQI_Wait_Idle(pSQIDev);
        return RET_OK;
    }

    if ((len - startpagebyte) < hSQI->pagesize)
    {

        if (startpagebyte)
        {
            ret = HAL_SQI_GD25_4LineWrite_Enable(pSQIDev, startpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), buffer, startpagebyte);
        }
        ret = HAL_SQI_GD25_4LineWrite_Enable(pSQIDev, (len - startpagebyte), hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        tmpaddr = (uint32_t *)(CONFIG_SQI_SFLASH_MMAP_BASE + (uint32_t)(offset + startpagebyte));
        tmpbuf = (uint32_t *)((uint32_t)buffer + (uint32_t)startpagebyte);
        memcpy((void *)tmpaddr, (void *)tmpbuf, (len - startpagebyte));
        HAL_SQI_Wait_Idle(pSQIDev);
    }
    else
    {
        pagecnt = (len - startpagebyte) / hSQI->pagesize;
        endpagebyte = (len - startpagebyte) % hSQI->pagesize;


        if (startpagebyte)
        {
            ret = HAL_SQI_GD25_4LineWrite_Enable(pSQIDev, startpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), (void *)buffer, startpagebyte);
        }
        tmpaddr = (uint32_t *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset + startpagebyte);
        tmpbuf = (uint32_t *)((uint32_t)buffer + (uint32_t)startpagebyte);
        while (pagecnt > 0)
        {
            ret = HAL_SQI_GD25_4LineWrite_Enable(pSQIDev, hSQI->pagesize, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)tmpaddr, (void *)tmpbuf, hSQI->pagesize);
            pagecnt--;
            tmpaddr += hSQI->pagesize / 4;
            tmpbuf += hSQI->pagesize / 4;
        }

        if (endpagebyte)
        {
            ret = HAL_SQI_GD25_4LineWrite_Enable(pSQIDev, endpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)tmpaddr, (void *)tmpbuf, endpagebyte);
        }

        HAL_SQI_Wait_Idle(pSQIDev);
    }


    return RET_OK;

}







/**
 * @brief     Read data by DMA mode
 * @param  hSQI        SQI device handle
 * @param  offset        address offset,address = 0x10000000 + offset
 * @param  buf            The data memory
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR   read data failed
 * @retval RET_TIMEOUT   read data is timeout
 * @retval RET_OK         Read data  successfully
 */

static RET_CODE HAL_SQI_GD25_DMA_4LineRead(SQI_Handle_T *hSQI, uint32_t offset, void *buff, uint32_t length)
{
    SQI_Device_T *pSQIDev;
    RET_CODE ret = 0;
    uint32_t no_4bit_num_end = 0;
    uint32_t no_4bit_num_start = 0;
    uint32_t buffend;
    uint32_t buffdma;
    int timeout = SQI_TIMEOUT;
    s_dmaComplete = 0;
    s_dmaError = 0 ;


    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }


    if ((offset % 4) || ((uint32_t)buff % 4))
    {
        if (((uint32_t)buff % 4) != (offset % 4))
        {
            printf("error: 1.offset address or buff address is not 4byte alignment,please input a 4byte alignment address\n  \
					           2.buff address and offset is not continue\n");
            return RET_ERROR;
        }
        else
        {
            no_4bit_num_start = 4 - (offset % 4);
            no_4bit_num_end   = (length - no_4bit_num_start) % 4;
        }

    }
    else
    {
        no_4bit_num_end = length % 4;
    }

    ret = HAL_SQI_GD25_4LineRead_Enable(pSQIDev, length, hSQI->flashsize);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    if (no_4bit_num_start)
    {
        memcpy((buff), (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), no_4bit_num_start);
    }
    buffdma = (uint32_t)(buff) + (uint32_t)(no_4bit_num_start);
    HAL_DMA_Start(&s_dmaHandle, (uint32_t)(CONFIG_SQI_SFLASH_MMAP_BASE + offset + no_4bit_num_start), buffdma, (length  - no_4bit_num_end - no_4bit_num_start) / 4);
    while ((timeout > 0) && (s_dmaComplete == 0) && (s_dmaError == 0))
    {
        HAL_SQI_Time_Delayms(10);
        timeout--;
    }
    if (s_dmaComplete == 1)
    {
        ret = RET_OK;
    }
    else if (s_dmaError == 1)
    {
        ret =  RET_ERROR;
    }
    else if (timeout < 1)
    {
        ret =  RET_TIMEOUT;
    }
    if (no_4bit_num_end)
    {
        buffend = (uint32_t)(buff) + (uint32_t)(length - no_4bit_num_end - no_4bit_num_start);
        memcpy((void *)(buffend), (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + length - no_4bit_num_end - no_4bit_num_start), no_4bit_num_end);
    }
    HAL_SQI_Wait_Idle(pSQIDev);
    return ret;


}




static RET_CODE HAL_SQI_GD25_4Line_Read(SQI_Handle_T *hSQI, uint32_t offset, uint8_t *buffer, uint32_t size)
{
    SQI_Device_T *pSQIDev;
    RET_CODE      ret;


    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }


    if (hSQI->pConfig->read_mode == SQI_READ_BY_CPU)
    {

        ret = HAL_SQI_GD25_4LineRead_Enable(pSQIDev, size, hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        memcpy(buffer, (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), size);
        HAL_SQI_Wait_Idle(pSQIDev);
    }
    else
    {
        ret = HAL_SQI_GD25_DMA_4LineRead(hSQI, offset, buffer, size);
        if (ret != RET_OK)
        {
            return ret;
        }
    }

    return RET_OK;
}


/*============================================W25 Function=========================================================*/

/**
 * @brief  SQI Flash write on 4Line mode.
 * @param  pSQIDev:  SQI Device.
 * @param  offset:  the address of flash.
 * @param  buffer:  the data will write to flash.
 * @param  len:  write size.
 * @retval HAL status
 */
static RET_CODE HAL_SQI_W25_4Line_Write(SQI_Handle_T *hSQI, uint32_t offset, const void *buffer, size_t len)
{
    RET_CODE        ret;
    uint32_t            *tmpbuf;
    uint32_t            *tmpaddr ;
    int             pagecnt;
    uint32_t        endpagebyte;
    uint32_t        startpagebyte;
    SQI_Device_T *pSQIDev;

    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }


    if (offset > 0)
    {
        startpagebyte = hSQI->pagesize - (offset % hSQI->pagesize);
    }
    else
    {
        startpagebyte = 0;
    }

    if ((len < startpagebyte))
    {
        ret = HAL_SQI_W25_4LineWrite_Enable(pSQIDev, len, hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), buffer, len);
        HAL_SQI_Wait_Idle(pSQIDev);
        return RET_OK;
    }

    if ((len - startpagebyte) < hSQI->pagesize)
    {

        if (startpagebyte)
        {
            ret = HAL_SQI_W25_4LineWrite_Enable(pSQIDev, startpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), buffer, startpagebyte);
        }
        ret = HAL_SQI_W25_4LineWrite_Enable(pSQIDev, (len - startpagebyte), hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        tmpaddr = (uint32_t *)(CONFIG_SQI_SFLASH_MMAP_BASE + (uint32_t)(offset + startpagebyte));
        tmpbuf = (uint32_t *)((uint32_t)buffer + (uint32_t)startpagebyte);
        memcpy((void *)tmpaddr, (void *)tmpbuf, (len - startpagebyte));
        HAL_SQI_Wait_Idle(pSQIDev);
    }
    else
    {
        pagecnt = (len - startpagebyte) / hSQI->pagesize;
        endpagebyte = (len - startpagebyte) % hSQI->pagesize;


        if (startpagebyte)
        {
            ret = HAL_SQI_W25_4LineWrite_Enable(pSQIDev, startpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), (void *)buffer, startpagebyte);
        }
        tmpaddr = (uint32_t *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset + startpagebyte);
        tmpbuf = (uint32_t *)((uint32_t)buffer + (uint32_t)startpagebyte);
        while (pagecnt > 0)
        {
            ret = HAL_SQI_W25_4LineWrite_Enable(pSQIDev, hSQI->pagesize, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)tmpaddr, (void *)tmpbuf, hSQI->pagesize);
            pagecnt--;
            tmpaddr += hSQI->pagesize / 4;
            tmpbuf += hSQI->pagesize / 4;
        }

        if (endpagebyte)
        {
            ret = HAL_SQI_W25_4LineWrite_Enable(pSQIDev, endpagebyte, hSQI->flashsize);
            if (ret != RET_OK)
            {
                return ret;
            }
            memcpy((void *)tmpaddr, (void *)tmpbuf, endpagebyte);
        }

        HAL_SQI_Wait_Idle(pSQIDev);
    }


    return RET_OK;

}







/**
 * @brief     Read data by DMA mode
 * @param  hSQI        SQI device handle
 * @param  offset        address offset,address = 0x10000000 + offset
 * @param  buf            The data memory
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR   read data failed
 * @retval RET_TIMEOUT   read data is timeout
 * @retval RET_OK         Read data  successfully
 */

static RET_CODE HAL_SQI_W25_DMA_4LineRead(SQI_Handle_T *hSQI, uint32_t offset, void *buff, uint32_t length)
{
    SQI_Device_T *pSQIDev;
    RET_CODE ret = 0;
    uint32_t no_4bit_num_end = 0;
    uint32_t no_4bit_num_start = 0;
    uint32_t buffend;
    uint32_t buffdma;
    int timeout = SQI_TIMEOUT;
    s_dmaComplete = 0;
    s_dmaError = 0 ;

    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }


    if ((offset % 4) || ((uint32_t)buff % 4))
    {
        if (((uint32_t)buff % 4) != (offset % 4))
        {
            printf("error: 1.offset address or buff address is not 4byte alignment,please input a 4byte alignment address\n  \
					           2.buff address and offset is not continue\n");
            return RET_ERROR;
        }
        else
        {
            no_4bit_num_start = 4 - (offset % 4);
            no_4bit_num_end   = (length - no_4bit_num_start) % 4;
        }

    }
    else
    {
        no_4bit_num_end = length % 4;
    }

    ret = HAL_SQI_W25_4LineRead_Enable(pSQIDev, length, hSQI->flashsize);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    if (no_4bit_num_start)
    {
        memcpy((buff), (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), no_4bit_num_start);
    }
    buffdma = (uint32_t)(buff) + (uint32_t)(no_4bit_num_start);
    HAL_DMA_Start(&s_dmaHandle, (uint32_t)(CONFIG_SQI_SFLASH_MMAP_BASE + offset + no_4bit_num_start), buffdma, (length  - no_4bit_num_end - no_4bit_num_start) / 4);
    while ((timeout > 0) && (s_dmaComplete == 0) && (s_dmaError == 0))
    {
        HAL_SQI_Time_Delayms(10);
        timeout--;
    }
    if (s_dmaComplete == 1)
    {
        ret = RET_OK;
    }
    else if (s_dmaError == 1)
    {
        ret =  RET_ERROR;
    }
    else if (timeout < 1)
    {
        ret =  RET_TIMEOUT;
    }
    if (no_4bit_num_end)
    {
        buffend = (uint32_t)(buff) + (uint32_t)(length - no_4bit_num_end - no_4bit_num_start);
        memcpy((void *)(buffend), (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + length - no_4bit_num_end - no_4bit_num_start), no_4bit_num_end);
    }
    HAL_SQI_Wait_Idle(pSQIDev);
    return ret;


}




static RET_CODE HAL_SQI_W25_4Line_Read(SQI_Handle_T *hSQI, uint32_t offset, uint8_t *buffer, uint32_t size)
{
    SQI_Device_T *pSQIDev;
    RET_CODE      ret;
    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }

    if (hSQI->pConfig->read_mode == SQI_READ_BY_CPU)
    {

        ret = HAL_SQI_W25_4LineRead_Enable(pSQIDev, size, hSQI->flashsize);
        if (ret != RET_OK)
        {
            return ret;
        }
        memcpy(buffer, (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), size);
        HAL_SQI_Wait_Idle(pSQIDev);
    }
    else
    {
        ret = HAL_SQI_W25_DMA_4LineRead(hSQI, offset, buffer, size);
        if (ret != RET_OK)
        {
            return ret;
        }
    }

    return RET_OK;
}








/*============================================Public Function=========================================================*/
static RET_CODE HAL_SQI_Standard_Write(SQI_Handle_T *hSQI, uint32_t offset, const void *buffer, uint32_t len)
{
    uint32_t            *tmpbuf;
    uint32_t            *tmpaddr ;
    int             pagecnt;
    uint32_t        endpagebyte;
    uint32_t        startpagebyte;
    SQI_Device_T *pSQIDev;

    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }

    if (offset > 0)
    {
        startpagebyte = hSQI->pagesize - (offset % hSQI->pagesize);
    }
    else
    {
        startpagebyte = 0;
    }

    if ((len < startpagebyte))
    {
        HAL_SQI_NormalWrite_Enable(pSQIDev, len, hSQI->flashsize);

        memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), buffer, len);
        HAL_SQI_Wait_Idle(pSQIDev);
        return RET_OK;
    }

    if ((len - startpagebyte) < hSQI->pagesize)
    {

        if (startpagebyte)
        {
            HAL_SQI_NormalWrite_Enable(pSQIDev, startpagebyte, hSQI->flashsize);
            memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), buffer, startpagebyte);
        }
        HAL_SQI_NormalWrite_Enable(pSQIDev, (len - startpagebyte), hSQI->flashsize);
        tmpaddr = (uint32_t *)(CONFIG_SQI_SFLASH_MMAP_BASE + (uint32_t)(offset + startpagebyte));
        tmpbuf = (uint32_t *)((uint32_t)buffer + (uint32_t)startpagebyte);
        memcpy((void *)tmpaddr, (void *)tmpbuf, (len - startpagebyte));
        HAL_SQI_Wait_Idle(pSQIDev);
    }
    else
    {
        pagecnt = (len - startpagebyte) / hSQI->pagesize;
        endpagebyte = (len - startpagebyte) % hSQI->pagesize;


        if (startpagebyte)
        {
            HAL_SQI_NormalWrite_Enable(pSQIDev, startpagebyte, hSQI->flashsize);

            memcpy((void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), (void *)buffer, startpagebyte);
        }
        tmpaddr = (uint32_t *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset + startpagebyte);
        tmpbuf = (uint32_t *)((uint32_t)buffer + (uint32_t)startpagebyte);
        while (pagecnt > 0)
        {
            HAL_SQI_NormalWrite_Enable(pSQIDev, hSQI->pagesize, hSQI->flashsize);
            memcpy((void *)tmpaddr, (void *)tmpbuf, hSQI->pagesize);
            pagecnt--;
            tmpaddr += hSQI->pagesize / 4;
            tmpbuf += hSQI->pagesize / 4;
        }

        if (endpagebyte)
        {
            HAL_SQI_NormalWrite_Enable(pSQIDev, endpagebyte, hSQI->flashsize);

            memcpy((void *)tmpaddr, (void *)tmpbuf, endpagebyte);
        }

        HAL_SQI_Wait_Idle(pSQIDev);
    }


    return RET_OK;

}


static RET_CODE HAL_SQI_Standard_Read(SQI_Handle_T *hSQI, uint32_t offset, void *buffer, uint32_t len)
{
    SQI_Device_T *pSQIDev;

    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }

    HAL_SQI_NormalRead_Enable(pSQIDev, hSQI->flashsize);
    memcpy((void *)buffer, (void *)(CONFIG_SQI_SFLASH_MMAP_BASE + offset), len);
    HAL_SQI_Wait_Idle(pSQIDev);

    return RET_OK;

}




/**
 * @brief  device  modele Identification
 * @param  hSQI:  SQI Handle.
  * @retval result
 */

static RET_CODE HAL_SQI_Model_Identification(SQI_Handle_T *hSQI, uint8_t mt_id)
{
    if (hSQI->Manufacturer == SQI_MT)
    {
        if (mt_id == MT_MT_ID)
        {
            if (hSQI->flashsize == SQI_32M_SIZE)
            {
                hSQI->model = MT25QL256ABA;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
        }
        else
        {
            printf("#############Sorry,The flash model is don't support!#############\n");
            return RET_ERROR;
        }
    }
    else if (hSQI->Manufacturer == SQI_MX)
    {
        if (mt_id == MX_MT_ID)
        {
            if (hSQI->flashsize == SQI_2M_SIZE)
            {
                hSQI->model = MX25L1605D;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
            else if (hSQI->flashsize == SQI_4M_SIZE)
            {
                hSQI->model = MX25L3205D;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
            else if (hSQI->flashsize == SQI_8M_SIZE)
            {
                hSQI->model = MX25L6433F;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
            else if (hSQI->flashsize == SQI_16M_SIZE)
            {
                hSQI->model = MX25L12835F;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
            else if (hSQI->flashsize == SQI_32M_SIZE)
            {
                hSQI->model = MX25L25633F;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
            else if (hSQI->flashsize == SQI_64M_SIZE)
            {
                hSQI->model = MX25L51245G;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
            else
            {
                printf("#############Sorry,The flash model is don't support!#############\n");
                return RET_ERROR;
            }

        }
        else if (mt_id == KH_MT_ID)
        {
            if (hSQI->flashsize == SQI_1M_SIZE)
            {
                hSQI->model = KH25V8035F;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
            else
            {
                printf("#############Sorry,The flash model is don't support!#############\n");
                return RET_ERROR;
            }
        }
        else
        {
            printf("#############Sorry,The flash model is don't support!#############\n");
            return RET_ERROR;
        }
    }
    else if (hSQI->Manufacturer == SQI_GD)
    {
        if (mt_id == GD_MT_ID)
        {
            if (hSQI->flashsize == SQI_32M_SIZE)
            {
                hSQI->model = GD25Q257D;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
            else if (hSQI->flashsize == SQI_16M_SIZE)
            {
                hSQI->model = GD25Q127C;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
        }
        else
        {
            printf("#############Sorry,The flash model is don't support!#############\n");
            return RET_ERROR;
        }
    }
    else if (hSQI->Manufacturer == SQI_WB)
    {
        if (mt_id == WB_MT_ID)
        {
            if (hSQI->flashsize == SQI_16M_SIZE)
            {
                hSQI->model = W25Q128JV;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
            else if (hSQI->flashsize == SQI_32M_SIZE)
            {
                hSQI->model = W25Q256JV;
                hSQI->pagesize = SQI_256B_PAGE_SIZE;
            }
        }
        else
        {
            printf("#############Sorry,The flash model is don't support!#############\n");
            return RET_ERROR;
        }

    }
    else
    {
        printf("#############Sorry,The flash model is don't support!#############\n");
        return RET_ERROR;
    }

    return RET_OK;

}


/**
 * @brief  device init
 * @param  hSQI:  SQI Handle.
  * @retval result
 */
static RET_CODE HAL_SQI_Device_Init(SQI_Handle_T *hSQI)
{
    SQI_Device_T *pSQIDev;
    uint32_t device_id;
    uint8_t  mf_id, mt_id, md_id;
    RET_CODE ret;

    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }

    device_id = HAL_SQI_ReadID(pSQIDev);
    md_id = (uint8_t)(device_id >> 8);
    mt_id = (uint8_t)(device_id >> 16);
    mf_id = (uint8_t)(device_id >> 24);

    if (mf_id == MT_MF_ID)
    {
        hSQI->Manufacturer = SQI_MT;
        if ((md_id >= 0x17) && (md_id <= 0x22))
        {
            hSQI->flashsize    = sqi_mt_size[md_id - 0x17];
        }
        else
        {
            printf("#############Sorry,The flash model is don't support!#############\n");
            return RET_ERROR;
        }
        ret = HAL_SQI_Model_Identification(hSQI, mt_id);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }
    else if (mf_id == MX_MF_ID)
    {
        hSQI->Manufacturer = SQI_MX;
        if ((md_id >= 0x14) && (md_id <= 0x20))
        {
            hSQI->flashsize    = sqi_mx_size[md_id - 0x14];
        }
        else
        {
            printf("#############Sorry,The flash model is don't support!#############\n");
            return RET_ERROR;
        }

        ret = HAL_SQI_Model_Identification(hSQI, mt_id);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

    }
    else if (mf_id == GD_MF_ID)
    {
        hSQI->Manufacturer = SQI_GD;
        if ((md_id >= 0x18) && (md_id <= 0x19))
        {
            hSQI->flashsize    = sqi_gd_size[md_id - 0x18];
        }
        else
        {
            printf("#############Sorry,The flash model is don't support!#############\n");
            return RET_ERROR;
        }

        ret = HAL_SQI_Model_Identification(hSQI, mt_id);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

    }
    else if (mf_id == WB_MF_ID)
    {
        hSQI->Manufacturer = SQI_WB;
        if ((md_id >= 0x18) && (md_id <= 0x19))
        {
            hSQI->flashsize    = sqi_wb_size[md_id - 0x18];
        }
        else
        {
            printf("#############Sorry,The flash model is don't support!#############\n");
            return RET_ERROR;
        }

        ret = HAL_SQI_Model_Identification(hSQI, mt_id);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }
    else
    {
        printf("#############Sorry,The flash model is don't support!#############\n");
        return RET_ERROR;
    }

    return RET_OK;

}

/**
 * @brief  when flash size >16M need to entery 4address mode
 * @param  hSQI:  SQI Handle.
  * @retval result
 */
static RET_CODE HAL_SQI_4Address_Enable(SQI_Handle_T *hSQI)
{
    SQI_Device_T *pSQIDev;
    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
    }
    else
    {
        return RET_ERROR;
    }

    if (hSQI->Manufacturer == SQI_MX)
    {
        return HAL_SQI_MX25_4Address_Enable(pSQIDev);
    }
    else if (hSQI->Manufacturer == SQI_MT)
    {
        return HAL_SQI_MT25_4Address_Enable(pSQIDev);
    }
    else if (hSQI->Manufacturer == SQI_GD)
    {
        return HAL_SQI_GD25_4Address_Enable(pSQIDev);
    }
    else if (hSQI->Manufacturer == SQI_WB)
    {
        return HAL_SQI_W25_4Address_Enable(pSQIDev);
    }
    return RET_OK;
}



/*============================================User API=========================================================*/

/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */

RET_CODE   HAL_SQI_Init(SQI_Handle_T *hSQI, SQI_Device_T *pSQIDev, SQI_Config_T *pConfig)
{
    RET_CODE ret;
    if (hSQI)
    {
        hSQI->pSQIDev = pSQIDev;
        hSQI->pConfig = pConfig;
        ret = HAL_SQI_Device_Init(hSQI);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        if (hSQI->pConfig->clk_div > 1)
        {
            ret = HAL_SQI_Div_Config(pSQIDev, hSQI->pConfig->clk_div);
            if (ret != RET_OK)
            {
                return RET_ERROR;
            }
        }
        else
        {
            ret = HAL_SQI_Div_Config(pSQIDev, SCLK_DIV);
            if (ret != RET_OK)
            {
                return RET_ERROR;
            }
        }
        if (hSQI->pConfig->read_mode == SQI_READ_BY_DMA)
        {
            if (HAL_SQI_DMA_Config() != RET_OK)
            {
                return RET_ERROR;
            }
        }
    }
    else
    {
        return RET_ERROR;
    }

    return RET_OK;
}

/**
 * @brief  printf sflash device
 * @param  hSQI:  SQI handle
  * @retval NO
 */

RET_CODE HAL_SQI_Print_Device(SQI_Handle_T *hSQI)
{
    if (!hSQI)
    {
        printf("Error: SQI handle is NULL!\n");
        return RET_ERROR;
    }

    printf("Flash information :\nModel: %s\nflash size:%dM\nVoltage:2.7V~3.6V\npage size:%dB\n", model[hSQI->model], hSQI->flashsize / 0x100000, hSQI->pagesize);
    return RET_OK;
}


/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */

RET_CODE HAL_SQI_Read(SQI_Handle_T *hSQI, uint32_t offset, void *buff, uint32_t length)
{
    RET_CODE ret;
    if (hSQI)
    {
        if (length + offset > hSQI->flashsize)
        {
            printf("Error:read area is over flash!\n");
            return RET_ERROR;
        }

        if (hSQI->flashsize > SQI_16M_SIZE)
        {
            ret = HAL_SQI_4Address_Enable(hSQI);
            if (ret != RET_OK)
            {
                return RET_ERROR;
            }
        }
        if (hSQI->pConfig->bit_mode == SQI_4BIT_MODE)
        {
            if (hSQI->Manufacturer == SQI_MX)
            {
                return HAL_SQI_MX25_4Line_Read(hSQI, offset, buff, length);
            }
            else if (hSQI->Manufacturer == SQI_MT)
            {
                return HAL_SQI_MT25_4Line_Read(hSQI, offset, buff, length);
            }
            else if (hSQI->Manufacturer == SQI_GD)
            {
                return HAL_SQI_GD25_4Line_Read(hSQI, offset, buff, length);
            }
            else if (hSQI->Manufacturer == SQI_WB)
            {
                return HAL_SQI_W25_4Line_Read(hSQI, offset, buff, length);
            }
        }
        else
        {
            return HAL_SQI_Standard_Read(hSQI, offset, buff, length);
        }
    }
    else
    {
        return RET_ERROR;
    }
    return RET_OK;

}


/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
RET_CODE HAL_SQI_Write(SQI_Handle_T *hSQI, uint32_t offset, const void *buff, uint32_t length)
{
    RET_CODE ret;
    if (hSQI)
    {
        if (length + offset > hSQI->flashsize)
        {
            printf("Error:write area is over flash!\n");
            return RET_ERROR;
        }

        if (hSQI->flashsize > SQI_16M_SIZE)
        {
            ret = HAL_SQI_4Address_Enable(hSQI);
            if (ret != RET_OK)
            {
                return RET_ERROR;
            }
        }

        if (hSQI->pConfig->bit_mode == SQI_4BIT_MODE)
        {
            if (hSQI->Manufacturer == SQI_MX)
            {
                return HAL_SQI_MX25_4Line_Write(hSQI, offset, buff, length);
            }
            else if (hSQI->Manufacturer == SQI_MT)
            {
                return HAL_SQI_MT25_4Line_Write(hSQI, offset, buff, length);
            }
            else if (hSQI->Manufacturer == SQI_GD)
            {
                return HAL_SQI_GD25_4Line_Write(hSQI, offset, buff, length);
            }
            else if (hSQI->Manufacturer == SQI_WB)
            {
                return HAL_SQI_W25_4Line_Write(hSQI, offset, buff, length);
            }
        }
        else
        {
            return HAL_SQI_Standard_Write(hSQI, offset, buff, length);
        }

    }
    else
    {
        return RET_ERROR;
    }
    return RET_OK;
}



/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */

RET_CODE  HAL_SQI_Erase(SQI_Handle_T *hSQI, uint32_t offset, uint32_t length)
{
    RET_CODE ret;
    SQI_Device_T *pSQIDev;

    if (hSQI)
    {
        pSQIDev = hSQI->pSQIDev;
        if ((offset + length) > hSQI->flashsize)
        {
            printf("Error:erase area is over flash!\n");
            return RET_ERROR;
        }

        if (hSQI->flashsize > SQI_16M_SIZE)
        {
            ret = HAL_SQI_4Address_Enable(hSQI);
            if (ret != RET_OK)
            {
                return RET_ERROR;
            }
        }

        return HAL_SQI_Erase_Internal(pSQIDev, offset, length, hSQI->flashsize);
    }

    return RET_ERROR;
}




/**
 * @brief  SQI Flash write read and erase operation.
 * @param  pSQIDev:  SQI Device.
 * @param  offset:  the address of flash.
 * @param  size:  erase size.
 * @retval HAL status
 */

RET_CODE HAL_SQI_WriteAll(SQI_Handle_T *hSQI, uint32_t offset, const void *buffer, uint32_t len)
{
    uint32_t    wsector_pos;
    uint16_t    wsector_off;
    uint16_t    sector_remain;
    uint16_t    i;
    uint8_t    *rxbuf  = g_tmpbuf;
    uint8_t    *writebuf    = (uint8_t *)buffer;
    uint32_t      wtotallen   = 0;
    uint32_t    writelen;
    uint32_t    write_remain = len;
    RET_CODE    ret;

    if (!hSQI)
    {
        return RET_ERROR;
    }

    wsector_pos     = offset / SFLASH_SECTOR_LEN;
    wsector_off     = offset % SFLASH_SECTOR_LEN;
    sector_remain   = SFLASH_SECTOR_LEN - wsector_off;

    writelen = sector_remain;

    if (write_remain <= sector_remain)
    {
        writelen = len;
    }

    while (1)
    {
        ret = HAL_SQI_Read(hSQI, wsector_pos * SFLASH_SECTOR_LEN, rxbuf, SFLASH_SECTOR_LEN);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
        for (i = 0; i < writelen; i++)
        {
            if (rxbuf[wsector_off + i] != 0XFF)
            {
                break;
            }
        }
        if (i < writelen - 1)
        {
            ret = HAL_SQI_Erase(hSQI, wsector_pos * SFLASH_SECTOR_LEN, SFLASH_SECTOR_LEN);
            if (ret != RET_OK)
            {
                return RET_ERROR;
            }
            for (i = 0; i < writelen; i++)
            {
                rxbuf[i + wsector_off] = writebuf[i];
            }
            ret = HAL_SQI_Write(hSQI, wsector_pos * SFLASH_SECTOR_LEN, rxbuf, SFLASH_SECTOR_LEN);
            if (ret != RET_OK)
            {
                return RET_ERROR;
            }
        }
        else
        {
            /*printk("tmpaddr=0x%x,sector_remain=%d,rxbufaddr=0x%x,rxbuf[0] =0x%x\n",tmpaddr,sector_remain,tmpbuf,rxbuf[0]);*/
            ret = HAL_SQI_Write(hSQI, (wsector_pos * SFLASH_SECTOR_LEN) + wsector_off, writebuf, writelen);
            if (ret != RET_OK)
            {
                return RET_ERROR;
            }

        }

        wtotallen += writelen;

        if (wtotallen == len)
        {
            break;
        }
        else
        {
            wsector_pos++;
            wsector_off = 0;

            writebuf    += writelen;
            write_remain -= writelen;
            if (write_remain > SFLASH_SECTOR_LEN)
            {
                writelen = SFLASH_SECTOR_LEN;
            }
            else
            {
                writelen = write_remain;
            }
        }
    }

    return RET_OK;
}


/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
uint32_t       HAL_SQI_GetFlashSize(SQI_Handle_T *hSQI)
{
    if (hSQI)
    {
        return hSQI->flashsize;
    }
    else
    {
        return 1;
    }
}


/**
 * @brief                 Read cmd/addr/dummy/data from SQI Controller
 * @param  pSQIDev        SQI device handle
 * @param  buf            The value of cmd/addr/dummy/data
 * @param  length         The byte num will be read cmd/addr/dummy/data
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
uint32_t       HAL_SQI_GetPageSize(SQI_Handle_T *hSQI)
{
    if (hSQI)
    {
        return hSQI->pagesize;
    }
    else
    {
        return 1;
    }
}









