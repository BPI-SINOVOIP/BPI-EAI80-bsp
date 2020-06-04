/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <misc/util.h>
#include <misc/printk.h>

#include "vb_queue.h"

void vb_queue_init(struct dcmi_queue *q)
{
	q->queue_wr = 0;
    q->queue_rd = 0;
    q->queue_cnt = 0;
}

uint8_t vb_qbuf(struct dcmi_queue *q, uint8_t frm_idx)
{
    uint8_t ret = q->queue_wr;
    
	if(q->queue_cnt == REAY_BUF_MAX)
	{
        return -1;
	}

	q->frm_idx[q->queue_wr] = frm_idx;
    q->queue_wr = (q->queue_wr==(REAY_BUF_MAX-1))?0:q->queue_wr+1;
    q->queue_cnt++;
    return ret;
}

bool vb_reqbuf(struct dcmi_queue *q, uint8_t *pfrm_idx)
{
	if(q->queue_cnt == 0)
	{
		return false;
	}	

	*pfrm_idx = q->frm_idx[q->queue_rd];
	return true;	
}

bool vb_dqbuf(struct dcmi_queue *q, uint8_t frm_idx)
{
	if(q->queue_cnt == 0)
	{
		return false;
	}	

	q->queue_rd = (q->queue_rd == (REAY_BUF_MAX-1))?0:q->queue_rd+1;
	q->queue_cnt--;
	return true;
}


