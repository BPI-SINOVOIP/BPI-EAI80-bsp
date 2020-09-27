/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __VIDEO_MGR_H__
#define __VIDEO_MGR_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define REAY_BUF_MAX    0xFF

struct queue
{
    uint8_t queue_rd;
    uint8_t queue_wr;
    uint8_t queue_cnt;
    uint8_t frm_idx[REAY_BUF_MAX];
};

#define MAX_FRAME_SIZE  4

enum fb_state
{
    FB_MEMORY_FREE,
    FB_MEMORY_BUSY,
};

struct fb_info
{
    uint32_t fb_start;
    uint32_t buf_size;
    uint32_t timestamp;
    enum fb_state state;
};

struct fb_manager
{

    uint8_t  frm_number;
    uint16_t pic_width;
    uint16_t pic_height;
    struct queue q;
    struct fb_info frm_info[MAX_FRAME_SIZE];
};

struct requestbuffers
{
    uint8_t  count;
    uint32_t mem_start;
    uint32_t length;
};

struct frm_buf
{
    uint8_t     index;
    uint32_t    addr;
    uint32_t    size;
};

/*******************************************************************************
 * API
 ******************************************************************************/
void queue_init(struct queue *q);
uint8_t qbuf(struct queue *q, uint8_t frm_idx);
bool reqbuf(struct queue *q, uint8_t *pfrm_idx);
bool dqbuf(struct queue *q, uint8_t frm_idx);

bool usb_request_buf(struct fb_manager *pfrm_mgr, uint8_t *pfrm_idx);
int usb_create_bufs(struct fb_manager *pfrm_mgr, struct requestbuffers *reqbufs);
int usb_query_buf(struct fb_manager *pfrm_mgr, struct frm_buf *buf);
int usb_dequeue_buf(struct fb_manager *pfrm_mgr, struct frm_buf *buf);
int usb_queue_buf(struct fb_manager *pfrm_mgr, struct frm_buf *buf);

#endif /* __VIDEO_MGR_H__ */
