/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <arch/cpu.h>
#include <board.h>
#include <init.h>
#include <clock_control.h>
#include <dma.h>
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>

#include <camera/dcmi.h>
#include "vin_gm6721x.h"

/* convenience defines */
#define DEV_CFG(dev)                            \
    ((const struct vin_gm6721x_config * const)(dev)->config->config_info)
#define DEV_DATA(dev)                           \
    ((struct vin_gm6721x_data * const)(dev)->driver_data)
#define DEV_STRUCT(dev)                 \
    ((VIN_Device_T *)(DEV_CFG(dev))->base)


static bool vin_gm6721x_request_buf(struct vb_manager *pfrm_mgr, uint8_t *pfrm_idx)
{
    uint8_t i = 0;
    uint8_t frm_idx = 0xff;
    bool free_buf_get = false;
    uint32_t kdp310_buf_addr = 0xffffffff;
    uint32_t de_hw_addr = 0xffffffff;

    for (i = 0; i < pfrm_mgr->frm_number; i++)
    {
        if (pfrm_mgr->frm_info[i].state != VB_MEMORY_FREE
                || pfrm_mgr->frm_info[i].fb_start == 0
                || pfrm_mgr->frm_info[i].buf_size == 0
                || pfrm_mgr->frm_info[i].fb_start == kdp310_buf_addr
                || pfrm_mgr->frm_info[i].fb_start == de_hw_addr)
        {
            continue;
        }

        frm_idx = i;
        free_buf_get = true;
        break;
    }

    if (free_buf_get == false)
    {
        return false;
    }

    pfrm_mgr->frm_info[frm_idx].state = VB_MEMORY_BUSY;

    *pfrm_idx = frm_idx;

    return true;
}

/**
 * @brief Allocate frame buffers.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vin_gm6721x_create_bufs(struct device *dev, struct dcmi_requestbuffers *reqbufs)
{
    struct vin_gm6721x_data *data = DEV_DATA(dev);
    struct vb_manager *pfrm_mgr = &data->vin_frm_mgr;
    uint8_t i = 0;
    uint8_t bpp = 0;
    uint32_t bot_addr = 0;
    uint32_t buf_size = 0;

    if (reqbufs->mem_start == 0 || reqbufs->length == 0)
    {
        return 1;
    }

    bot_addr = (reqbufs->mem_start + 15) & 0xfffffff0;

    if (reqbufs->count > MAX_FRAME_SIZE)
    {
        pfrm_mgr->frm_number = MAX_FRAME_SIZE;
        reqbufs->count = MAX_FRAME_SIZE;
    }
    else
    {
        pfrm_mgr->frm_number = reqbufs->count;
    }

    bpp = (data->csc_en == true) ? 3 : data->bpp;
    if (data->pix_fmt == PIX_FMT_JPEG)
    {
        buf_size = reqbufs->length / reqbufs->count;
    }
    else
    {
        buf_size = data->width * data->height * bpp;
    }

    for (i = 0; i < pfrm_mgr->frm_number; i++)
    {
        pfrm_mgr->frm_info[i].buf_size = buf_size;
        pfrm_mgr->frm_info[i].fb_start = bot_addr + i * (buf_size);
        pfrm_mgr->frm_info[i].state = VB_MEMORY_FREE;
    }

    while ((pfrm_mgr->frm_info[pfrm_mgr->frm_number - 1].fb_start
            + pfrm_mgr->frm_info[pfrm_mgr->frm_number - 1].buf_size) > (reqbufs->mem_start + reqbufs->length))
    {
        pfrm_mgr->frm_number--;
        reqbufs->count = pfrm_mgr->frm_number;
        if (pfrm_mgr->frm_number == 0)
        {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief query frame buffer information.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vin_gm6721x_query_buf(struct device *dev, struct dcmi_buf *buf)
{
    struct vin_gm6721x_data *data = DEV_DATA(dev);
    struct vb_manager *pfrm_mgr = &data->vin_frm_mgr;

    if (buf->index < MAX_FRAME_SIZE && pfrm_mgr->frm_info[buf->index].fb_start != NULL
            && pfrm_mgr->frm_info[buf->index].buf_size != 0)
    {
        buf->addr = pfrm_mgr->frm_info[buf->index].fb_start;
        buf->size = pfrm_mgr->frm_info[buf->index].buf_size;
        return 0;
    }

    return 1;
}

/**
 * @brief Dequeue a buffer to the userspace.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vin_gm6721x_dequeue_buf(struct device *dev, struct dcmi_buf *buf)
{
    struct vin_gm6721x_data *data = DEV_DATA(dev);
    struct vb_manager *pfrm_mgr = &data->vin_frm_mgr;
    struct dcmi_queue *q = &data->q;
    uint8_t frm_idx = 0xff;
    bool ret = false;

    ret = vb_reqbuf(q, &frm_idx);

    if (ret != false)
    {
        buf->index = frm_idx;
        buf->addr = pfrm_mgr->frm_info[frm_idx].fb_start;
        buf->size = pfrm_mgr->frm_info[frm_idx].buf_size;
        buf->timestamp = pfrm_mgr->frm_info[frm_idx].timestamp;
        vb_dqbuf(q, frm_idx);

        return 0;
    }

    return 1;
}

/**
 * @brief Queue a buffer from userspace.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vin_gm6721x_queue_buf(struct device *dev, struct dcmi_buf *buf)
{
    struct vin_gm6721x_data *data = DEV_DATA(dev);
    struct vb_manager *pfrm_mgr = &data->vin_frm_mgr;

    if (buf->index < MAX_FRAME_SIZE)
    {
        pfrm_mgr->frm_info[buf->index].state = VB_MEMORY_FREE;
        return 0;
    }

    return 1;
}

/**
 * @brief Start camera capture.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vin_gm6721x_start_streaming(struct device *dev, uint32_t mode)
{
    VIN_Device_T *pVIN = DEV_STRUCT(dev);
    struct vin_gm6721x_data *data = DEV_DATA(dev);
    struct vb_manager *pfrm_mgr = &data->vin_frm_mgr;
    struct dcmi_queue *q = &data->q;
    uint8_t i = 0;
    uint8_t frm_idx = 0xff;
    bool ret = false;

    k_mutex_lock(&data->mutex, K_FOREVER);

    for (i = 0; i < pfrm_mgr->frm_number; i++)
    {
        pfrm_mgr->frm_info[i].state = VB_MEMORY_FREE;
    }

    ret = vin_gm6721x_request_buf(pfrm_mgr, &frm_idx);
    if (ret == false)
    {
        k_mutex_unlock(&data->mutex);
        return -ENOTSUP;
    }

    pfrm_mgr->active_idx = frm_idx;

    vb_queue_init(q);

    /* Clear the interrupt flag */
    __HAL_VIN_CLEAR_FLAG(pVIN, VIN_FLAG_FRAMERI | VIN_FLAG_OVRRI | VIN_FLAG_ERRRI);

    HAL_VIN_Start(pVIN, mode, pfrm_mgr->frm_info[frm_idx].fb_start, pfrm_mgr->frm_info[frm_idx].buf_size);

    HAL_VIN_EnableInterrupts(pVIN, VIN_IT_FRAME | VIN_IT_OVR | VIN_IT_ERR);

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief Stop camera capture.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vin_gm6721x_stop_streaming(struct device *dev)
{
    VIN_Device_T *pVIN = DEV_STRUCT(dev);
    struct vin_gm6721x_data *data = DEV_DATA(dev);
    struct vb_manager *pfrm_mgr = &data->vin_frm_mgr;
    struct dcmi_queue *q = &data->q;

    k_mutex_lock(&data->mutex, K_FOREVER);

    HAL_VIN_Stop(pVIN);

    HAL_VIN_DisableInterrupts(pVIN, VIN_IT_FRAME | VIN_IT_OVR | VIN_IT_ERR);

    vb_queue_init(q);
    pfrm_mgr->active_idx = 0xff;

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief Setting camera input format.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fmt .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vin_gm6721x_set_fmt_vid_cap(struct device *dev, const struct dcmi_format *f)
{
    struct vin_gm6721x_data *data = DEV_DATA(dev);
    VIN_Device_T *pVIN = DEV_STRUCT(dev);

    k_mutex_lock(&data->mutex, K_FOREVER);

    switch (f->pix_fmt)
    {
        case PIX_FMT_RAW:
        case PIX_FMT_JPEG:
            data->bpp = 1;
            break;
        case PIX_FMT_YUV422:
        case PIX_FMT_RGB565:
            data->bpp = 2;
            break;
        default:
            break;
    }

    if (f->pix_fmt == PIX_FMT_JPEG)
    {
        HAL_VIN_EnableJPEGMode(pVIN);
    }
    else
    {
        HAL_VIN_DisableJPEGMode(pVIN);
    }

    data->pix_fmt = f->pix_fmt;
    data->width = f->width;
    data->height = f->height;

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief Get camera input format.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fmt .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vin_gm6721x_get_fmt_vid_cap(struct device *dev, struct dcmi_format *f)
{
    struct vin_gm6721x_data *data = DEV_DATA(dev);

    k_mutex_lock(&data->mutex, K_FOREVER);

    f->pix_fmt = data->pix_fmt;
    f->width = data->width;
    f->height = data->height;

    k_mutex_unlock(&data->mutex);

    return 0;
}

/**
 * @brief camera capture io command operation.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param cmd  io command.
 * @param arg  .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
static inline int vin_gm6721x_ioctl(struct device *dev, uint32_t cmd, void *arg)
{
    VIN_Device_T *pVIN = DEV_STRUCT(dev);
    struct vin_gm6721x_data *data = DEV_DATA(dev);
    struct dcmi_crop_rect *rect;
    struct dcmi_requestbuffers *reqbufs;
    struct dcmi_buf *buf;
    uint32_t csc_onoff;
    uint32_t path_index;
    int ret = 0;

    switch (cmd)
    {
        case VIDIOC_CONV_COLORSPACE:
            csc_onoff = *(uint32_t *)arg;
            if (data->pix_fmt != PIX_FMT_YUV422)
            {
                return -ENOTSUP;
            }
            if (csc_onoff != 0)
            {
                HAL_VIN_EnableColorSpaceConv(pVIN);
                data->csc_en = true;
            }
            else
            {
                HAL_VIN_DisableColorSpaceConv(pVIN);
                data->csc_en = false;
            }

            break;
        case VIDIOC_S_CROP:
            rect = (struct dcmi_crop_rect *)arg;
            HAL_VIN_DisableCrop(pVIN);
            HAL_VIN_ConfigCrop(pVIN, rect->left, rect->top, rect->width, rect->height);
            data->width = (rect->width + 1) / data->bpp;
            data->height = rect->height + 1;
            HAL_VIN_EnableCrop(pVIN);
            break;
        case VIDIOC_CREATE_BUFS:
            reqbufs = (struct dcmi_requestbuffers *)arg;
            ret = vin_gm6721x_create_bufs(dev, reqbufs);
            if (ret != 0)
            {
                return -ENOTSUP;
            }
            break;
        case VIDIOC_QUERYBUF:
            buf = (struct dcmi_buf *)arg;
            ret = vin_gm6721x_query_buf(dev, buf);
            if (ret != 0)
            {
                return -ENOTSUP;
            }
            break;
        case VIDIOC_QBUF:
            buf = (struct dcmi_buf *)arg;
            ret = vin_gm6721x_queue_buf(dev, buf);
            if (ret != 0)
            {
                return -ENOTSUP;
            }
            break;
        case VIDIOC_DQBUF:
            buf = (struct dcmi_buf *)arg;
            ret = vin_gm6721x_dequeue_buf(dev, buf);
            if (ret != 0)
            {
                return -ENOTSUP;
            }
            break;
        case VIDIOC_SWITCH_PATH:
            path_index = *(uint32_t *)arg;
            ret = HAL_VIN_SwitchPath(pVIN, path_index);
            if (ret != 0)
            {
                return -ENOTSUP;
            }
            break;
        default:
            break;
    }

    return 0;
}

static void vin_gm6721x_irq_callback_set(struct device *dev,
        dcmi_frame_irq_callback_t cb)
{
    struct vin_gm6721x_data *data = DEV_DATA(dev);

    data->user_cb = cb;
}

/** fixed hardware issue: if vin overrun, software can't clear vin fifo
**  when closing capture enable bit. Then hardwae will write some bytes
**  on fifo to next frame buffer
**/
static void vin_backup_data(VIN_Device_T *pVIN)
{
    HAL_VIN_Disable(pVIN);
    /* Disable Write memory */
    HAL_VIN_DisableMemoryWrite(pVIN);
    HAL_VIN_BackupData(pVIN);

    if (__HAL_VIN_FIFO_FLAG(pVIN, VIN_FLAG_FIFO))
    {
        printk("there are something wrong \n");
    }

    HAL_VIN_EnableMemoryWrite(pVIN);
    HAL_VIN_Enable(pVIN);
}

static void vin_gm6721x_isr(void *arg)
{
    struct device *dev = (struct device *)arg;
    struct vin_gm6721x_data *data = DEV_DATA(dev);
    struct vb_manager *pfrm_mgr = &data->vin_frm_mgr;
    struct dcmi_queue *q = &data->q;
    VIN_Device_T *pVIN = DEV_STRUCT(dev);
    uint8_t frm_idx = 0xff;
    bool ret = false;

    if (__HAL_VIN_GET_FLAG(pVIN, VIN_FLAG_OVRRI) || __HAL_VIN_GET_FLAG(pVIN, VIN_FLAG_ERRRI))
    {
        /* Clear the interrupt flag */
        __HAL_VIN_CLEAR_FLAG(pVIN, VIN_FLAG_FRAMERI | VIN_FLAG_OVRRI | VIN_FLAG_ERRRI);

        vin_backup_data(pVIN);
    }

    if (__HAL_VIN_GET_IT_SOURCE(pVIN, VIN_IT_FRAME) && __HAL_VIN_GET_FLAG(pVIN, VIN_FLAG_FRAMERI))
    {
        /* Clear the Line interrupt flag */
        __HAL_VIN_CLEAR_FLAG(pVIN, VIN_FLAG_FRAMERI);

        if (pfrm_mgr->active_idx != 0xff)
        {
            if (data->pix_fmt == PIX_FMT_JPEG)
            {
                pfrm_mgr->frm_info[pfrm_mgr->active_idx].buf_size = HAL_VIN_GetJPEGLength(pVIN);
            }
            vb_qbuf(q, pfrm_mgr->active_idx);
            pfrm_mgr->frm_info[pfrm_mgr->active_idx].timestamp = k_uptime_get_32();
        }

        ret = vin_gm6721x_request_buf(pfrm_mgr, &frm_idx);

        if (ret == true)
        {
            pfrm_mgr->active_idx = frm_idx;
            HAL_VIN_EnableMemoryWrite(pVIN);
            HAL_VIN_SetAddress(pVIN, pfrm_mgr->frm_info[frm_idx].fb_start, pfrm_mgr->frm_info[frm_idx].buf_size);
        }
        else
        {
            /* Disable Write memory */
            HAL_VIN_DisableMemoryWrite(pVIN);

            pfrm_mgr->active_idx = 0xff;
        }

        if (data->user_cb)
        {
            data->user_cb(dev, data->cur_line);
        }
    }
}

static const struct dcmi_driver_api vin_gm6721x_drv_api_funcs =
{
    .start_streaming = vin_gm6721x_start_streaming,
    .stop_streaming = vin_gm6721x_stop_streaming,
    .set_fmt_vid_cap = vin_gm6721x_set_fmt_vid_cap,
    .get_fmt_vid_cap = vin_gm6721x_get_fmt_vid_cap,
    .ioctl = vin_gm6721x_ioctl,
    .irq_callback_set = vin_gm6721x_irq_callback_set,
};

static int vin_gm6721x_init(struct device *dev)
{
    const struct vin_gm6721x_config *config = DEV_CFG(dev);
    struct vin_gm6721x_data *data = DEV_DATA(dev);
    struct vb_manager *pfrm_mgr = &data->vin_frm_mgr;
    struct dcmi_queue *q = &data->q;
    VIN_Device_T *pVIN = DEV_STRUCT(dev);
    uint8_t i = 0;

    /* mutex for mul task acess hardware*/
    k_mutex_init(&data->mutex);

    /* VIN clock and reset */

    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_VIN);
    HAL_Reset_Module(RESET_VIN_SW_RSTJ);

    data->pConfig.synchroMode = VIN_SYNCHRO_HARDWARE;
    data->pConfig.PCKPolarity = (config->pclk_polarity == 1) ? VIN_PCKPOLARITY_RISING : VIN_PCKPOLARITY_FALLING;
    data->pConfig.VSPolarity = (config->vsync_polarity == 1) ? VIN_VSPOLARITY_HIGH : VIN_VSPOLARITY_LOW;
    data->pConfig.HSPolarity = (config->hsync_polarity == 1) ? VIN_HSPOLARITY_HIGH : VIN_HSPOLARITY_LOW;
    data->pConfig.captureRate = VIN_CR_ALL_FRAME;
    data->pConfig.extendedDataMode = VIN_EXTEND_DATA_8B;
    HAL_VIN_Init(pVIN, &data->pConfig);

    data->cur_line = 0;
    data->csc_en = false;

    for (i = 0; i < MAX_FRAME_SIZE; i++)
    {
        pfrm_mgr->frm_info[i].buf_size = 0;
        pfrm_mgr->frm_info[i].fb_start = 0;
    }

    pfrm_mgr->active_idx = 0xff;
    vb_queue_init(q);

    config->irq_config_func(dev);

    return 0;
}

static struct vin_gm6721x_data vin_gm6721x_dev_data =
{
    .cur_line = 0,
    .pix_fmt = PIX_FMT_RGB565,
    .bpp = 2,
};

static void vin_gm6721x_config_func(struct device *dev);

static const struct vin_gm6721x_config vin_gm6721x_dev_cfg =
{
    .base = CONFIG_VIN_BASE_ADDRESS,
    .pclk_polarity  = CAMERA_PCLK_POLARITY,
    .hsync_polarity = CAMERA_HSYNC_POLARITY,
    .vsync_polarity  = CAMERA_VSYNC_POLARITY,
    .irq_config_func = vin_gm6721x_config_func,
};

DEVICE_AND_API_INIT(vin_gm6721x, CONFIG_VIN_NAME,
                    vin_gm6721x_init,
                    &vin_gm6721x_dev_data, &vin_gm6721x_dev_cfg,
                    POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
                    &vin_gm6721x_drv_api_funcs);

static void vin_gm6721x_config_func(struct device *dev)
{
    ARG_UNUSED(dev);

    IRQ_CONNECT(VIN_IRQ, CONFIG_VIN_IRQ_PRI,
                vin_gm6721x_isr, DEVICE_GET(vin_gm6721x), 0);

    irq_enable(VIN_IRQ);
}

