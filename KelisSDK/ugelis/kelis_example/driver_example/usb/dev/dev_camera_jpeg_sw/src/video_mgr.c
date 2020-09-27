/*
 * The Clear BSD License
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
` * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <misc/util.h>
#include <misc/printk.h>
#include "video_mgr.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * API
 ******************************************************************************/
void queue_init(struct queue *q)
{
    q->queue_wr = 0;
    q->queue_rd = 0;
    q->queue_cnt = 0;
}

uint8_t qbuf(struct queue *q, uint8_t frm_idx)
{
    uint8_t ret = q->queue_wr;

    if (q->queue_cnt == REAY_BUF_MAX)
    {
        return -1;
    }

    q->frm_idx[q->queue_wr] = frm_idx;
    q->queue_wr = (q->queue_wr == (REAY_BUF_MAX - 1)) ? 0 : q->queue_wr + 1;
    q->queue_cnt++;
    return ret;
}

bool reqbuf(struct queue *q, uint8_t *pfrm_idx)
{
    if (q->queue_cnt == 0)
    {
        return false;
    }

    *pfrm_idx = q->frm_idx[q->queue_rd];
    return true;
}

bool dqbuf(struct queue *q, uint8_t frm_idx)
{
    if (q->queue_cnt == 0)
    {
        return false;
    }

    q->queue_rd = (q->queue_rd == (REAY_BUF_MAX - 1)) ? 0 : q->queue_rd + 1;
    q->queue_cnt--;
    return true;
}

bool usb_request_buf(struct fb_manager *pfrm_mgr, uint8_t *pfrm_idx)
{
    uint8_t i = 0;
    uint8_t frm_idx = 0xff;
    bool free_buf_get = false;

    for (i = 0; i < pfrm_mgr->frm_number; i++)
    {
        if (pfrm_mgr->frm_info[i].state != FB_MEMORY_FREE
                || pfrm_mgr->frm_info[i].fb_start == 0
                || pfrm_mgr->frm_info[i].buf_size == 0)
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

    pfrm_mgr->frm_info[frm_idx].state = FB_MEMORY_BUSY;

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
int usb_create_bufs(struct fb_manager *pfrm_mgr, struct requestbuffers *reqbufs)
{
    uint8_t i = 0;
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

    buf_size = pfrm_mgr->pic_width * pfrm_mgr->pic_height;

    for (i = 0; i < pfrm_mgr->frm_number; i++)
    {
        pfrm_mgr->frm_info[i].buf_size = buf_size;
        pfrm_mgr->frm_info[i].fb_start = bot_addr + i * (buf_size);
        pfrm_mgr->frm_info[i].state = FB_MEMORY_FREE;
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
int usb_query_buf(struct fb_manager *pfrm_mgr, struct frm_buf *buf)
{
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
int usb_dequeue_buf(struct fb_manager *pfrm_mgr, struct frm_buf *buf)
{
    struct queue *q = &(pfrm_mgr->q);
    uint8_t frm_idx = 0xff;
    bool ret = false;

    ret = reqbuf(q, &frm_idx);

    if (ret != false)
    {
        buf->index = frm_idx;
        buf->addr = pfrm_mgr->frm_info[frm_idx].fb_start;
        buf->size = pfrm_mgr->frm_info[frm_idx].buf_size;
        dqbuf(q, frm_idx);

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
int usb_queue_buf(struct fb_manager *pfrm_mgr, struct frm_buf *buf)
{
    if (buf->index < MAX_FRAME_SIZE)
    {
        pfrm_mgr->frm_info[buf->index].state = FB_MEMORY_FREE;
        return 0;
    }

    return 1;
}

