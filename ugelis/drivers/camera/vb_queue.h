/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _VIDEOBUF_QUEUE_H
#define _VIDEOBUF_QUEUE_H

#define REAY_BUF_MAX    0xFF

struct dcmi_queue {
    uint8_t	queue_rd;
    uint8_t	queue_wr;
    uint8_t	queue_cnt;
    uint8_t frm_idx[REAY_BUF_MAX];
};


void vb_queue_init(struct dcmi_queue *q);
uint8_t vb_qbuf(struct dcmi_queue *q, uint8_t frm_idx);
bool vb_reqbuf(struct dcmi_queue *q, uint8_t *pfrm_idx);
bool vb_dqbuf(struct dcmi_queue *q, uint8_t frm_idx);

#endif /* _VIDEOBUF_QUEUE_H */
