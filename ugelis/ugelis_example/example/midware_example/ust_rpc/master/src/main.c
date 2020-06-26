/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <misc/printk.h>

#include <imlib.h>
#include <display/fb_display.h>
#include <camera/dcmi.h>
#include <camera/camera_sensor.h>
#include <gm_hal_vin.h>
#include <gm_hal_vout.h>
#include <gm_hal_imp.h>
#include <gm_irq.h>

#include <ust_core.h>
#include <ust_camera.h>
#include <ust_filter.h>
#include <ust_edge.h>
#include <ust_lcd.h>



#define IMP_DEBUG 1
#ifdef IMP_DEBUG
    #define imp_pr_debug       printk
#else
    #define imp_pr_debug(...)  do{}while(0)
#endif


typedef enum
{
    /* camera */
#ifdef CONFIG_UST_PLUGIN_CAMERA_QUEUE
    ELEMENT_CAMERA_QUEUE_PAD_SRC_INDEX,
#else
    ELEMENT_CAMERA_PAD_SRC_INDEX,
#endif

    /* filter */
    ELEMENT_FILTER_PAD_SINK_INDEX,
#ifdef CONFIG_UST_PLUGIN_FILTER_QUEUE
    ELEMENT_FILTER_QUEUE_PAD_SRC_INDEX,
#else
    ELEMENT_FILTER_PAD_SRC_INDEX,
#endif

    /* edge */
    ELEMENT_EDGE_PAD_SINK_INDEX,
#ifdef CONFIG_UST_PLUGIN_EDGE_QUEUE
    ELEMENT_EDGE_QUEUE_PAD_SRC_INDEX,
#else
    ELEMENT_EDGE_PAD_SRC_INDEX,
#endif

    /* lcd */
    ELEMENT_LCD_PAD_SINK_INDEX,

    ELEMENT_MAX,
} IMP_PAD_INDEX_T;


int imp_pipe_line_config()
{
    int ret = 0;
    UST_Element_T *elt_camera = NULL;
    UST_Element_T *elt_filter = NULL;
    UST_Element_T *elt_edge = NULL;
    UST_Element_T *elt_lcd = NULL;
    UST_PipeLine_T *imp_pipe_line = NULL;
    UST_Pad_T *imp_pad_table[ELEMENT_MAX];

    /*alloc pileline*/
    imp_pipe_line = UST_AllocPileLine();
    if (!imp_pipe_line)
    {
        imp_pr_debug("[%s]:Err!!!! UST_AllocPileLine NULL\n", __FUNCTION__);
        ret = -ENOMEM;
        goto out;
    }

    /**
     *connent element connet to pileLine  link as bellow
     *pileline-->elt_camera-->elt_filter--->elt_lcd
     */
    elt_camera = UST_GetElement(ELEMENT_CAMERA);
    if (elt_camera == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }

#ifdef CONFIG_UST_PLUGIN_CAMERA_QUEUE
    UST_Element_T *elt_camera_queue = NULL;
    elt_camera_queue = UST_GetElement(ELEMENT_CAMERA_QUEUE);
    if (elt_camera_queue == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }
#endif

    elt_filter = UST_GetElement(ELEMENT_FILTER);
    if (elt_filter == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }

#ifdef CONFIG_UST_PLUGIN_FILTER_QUEUE
    UST_Element_T *elt_filter_queue = NULL;
    elt_filter_queue = UST_GetElement(ELEMENT_FILTER_QUEUE);
    if (elt_filter_queue == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }
#endif

    elt_edge = UST_GetElement(ELEMENT_EDGE);
    if (elt_edge == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }

#ifdef CONFIG_UST_PLUGIN_EDGE_QUEUE
    UST_Element_T *elt_edge_queue = NULL;
    elt_edge_queue = UST_GetElement(ELEMENT_EDGE_QUEUE);
    if (elt_edge_queue == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }
#endif

    elt_lcd = UST_GetElement(ELEMENT_LCD);
    if (elt_lcd == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }

    UST_ElementAddToPipeLine(imp_pipe_line, elt_camera);
#ifdef CONFIG_UST_PLUGIN_CAMERA_QUEUE
    UST_ElementAddToPipeLine(imp_pipe_line, elt_camera_queue);
#endif
    UST_ElementAddToPipeLine(imp_pipe_line, elt_filter);
#ifdef CONFIG_UST_PLUGIN_FILTER_QUEUE
    UST_ElementAddToPipeLine(imp_pipe_line, elt_filter_queue);
#endif
    UST_ElementAddToPipeLine(imp_pipe_line, elt_edge);
#ifdef CONFIG_UST_PLUGIN_EDGE_QUEUE
    UST_ElementAddToPipeLine(imp_pipe_line, elt_edge_queue);
#endif
    UST_ElementAddToPipeLine(imp_pipe_line, elt_lcd);

    /**
     * connet imp_pipe_line pad sequence
     * ELEMENT_CAMERA_PAD_SRC<--link-->ELEMENT_FILTER_PAD_SINK   ELEMENT_FILTER_PAD_SRC<---link---->ELEMENT_LCD_PAD_SINK
     */
#ifdef CONFIG_UST_PLUGIN_CAMERA_QUEUE
    imp_pad_table[ELEMENT_CAMERA_QUEUE_PAD_SRC_INDEX] = UST_GetPadFromElement(elt_camera_queue, UST_SRC_PAD, ELEMENT_CAMERA_QUEUE_PAD_SRC);
    if (imp_pad_table[ELEMENT_CAMERA_QUEUE_PAD_SRC_INDEX] == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetPadFromElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }
#else
    imp_pad_table[ELEMENT_CAMERA_PAD_SRC_INDEX] = UST_GetPadFromElement(elt_camera, UST_SRC_PAD, ELEMENT_CAMERA_PAD_SRC);
    if (imp_pad_table[ELEMENT_CAMERA_PAD_SRC_INDEX] == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetPadFromElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }
#endif

    imp_pad_table[ELEMENT_FILTER_PAD_SINK_INDEX] = UST_GetPadFromElement(elt_filter, UST_SINK_PAD, ELEMENT_FILTER_PAD_SINK);
    if (imp_pad_table[ELEMENT_FILTER_PAD_SINK_INDEX] == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetPadFromElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }

#ifdef CONFIG_UST_PLUGIN_FILTER_QUEUE
    imp_pad_table[ELEMENT_FILTER_QUEUE_PAD_SRC_INDEX] = UST_GetPadFromElement(elt_filter_queue, UST_SRC_PAD, ELEMENT_FILTER_QUEUE_PAD_SRC);
    if (imp_pad_table[ELEMENT_FILTER_QUEUE_PAD_SRC_INDEX] == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetPadFromElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }
#else
    imp_pad_table[ELEMENT_FILTER_PAD_SRC_INDEX] = UST_GetPadFromElement(elt_filter, UST_SRC_PAD, ELEMENT_FILTER_PAD_SRC);
    if (imp_pad_table[ELEMENT_FILTER_PAD_SRC_INDEX] == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetPadFromElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }
#endif

    imp_pad_table[ELEMENT_EDGE_PAD_SINK_INDEX] = UST_GetPadFromElement(elt_edge, UST_SINK_PAD, ELEMENT_EDGE_PAD_SINK);
    if (imp_pad_table[ELEMENT_EDGE_PAD_SINK_INDEX] == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetPadFromElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }

#ifdef CONFIG_UST_PLUGIN_EDGE_QUEUE
    imp_pad_table[ELEMENT_EDGE_QUEUE_PAD_SRC_INDEX] = UST_GetPadFromElement(elt_edge_queue, UST_SRC_PAD, ELEMENT_EDGE_QUEUE_PAD_SRC);
    if (imp_pad_table[ELEMENT_EDGE_QUEUE_PAD_SRC_INDEX] == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetPadFromElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }
#else
    imp_pad_table[ELEMENT_EDGE_PAD_SRC_INDEX] = UST_GetPadFromElement(elt_edge, UST_SRC_PAD, ELEMENT_EDGE_PAD_SRC);
    if (imp_pad_table[ELEMENT_EDGE_PAD_SRC_INDEX] == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetPadFromElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }
#endif

    imp_pad_table[ELEMENT_LCD_PAD_SINK_INDEX] = UST_GetPadFromElement(elt_lcd, UST_SINK_PAD, ELEMENT_LCD_PAD_SINK);
    if (imp_pad_table[ELEMENT_LCD_PAD_SINK_INDEX] == NULL)
    {
        imp_pr_debug("[%s]:Err!!!! UST_GetPadFromElement NULL\n", __FUNCTION__);
        ret = -EINVAL;
        goto out;
    }

#ifdef CONFIG_UST_PLUGIN_CAMERA_QUEUE
    UST_PadConnect(imp_pad_table[ELEMENT_CAMERA_QUEUE_PAD_SRC_INDEX], imp_pad_table[ELEMENT_FILTER_PAD_SINK_INDEX]);
#else
    UST_PadConnect(imp_pad_table[ELEMENT_CAMERA_PAD_SRC_INDEX], imp_pad_table[ELEMENT_FILTER_PAD_SINK_INDEX]);
#endif

#ifdef CONFIG_UST_PLUGIN_FILTER_QUEUE
    UST_PadConnect(imp_pad_table[ELEMENT_FILTER_QUEUE_PAD_SRC_INDEX], imp_pad_table[ELEMENT_EDGE_PAD_SINK_INDEX]);
#else
    UST_PadConnect(imp_pad_table[ELEMENT_FILTER_PAD_SRC_INDEX], imp_pad_table[ELEMENT_EDGE_PAD_SINK_INDEX]);
#endif

#ifdef CONFIG_UST_PLUGIN_EDGE_QUEUE
    UST_PadConnect(imp_pad_table[ELEMENT_EDGE_QUEUE_PAD_SRC_INDEX], imp_pad_table[ELEMENT_LCD_PAD_SINK_INDEX]);
#else
    UST_PadConnect(imp_pad_table[ELEMENT_EDGE_PAD_SRC_INDEX], imp_pad_table[ELEMENT_LCD_PAD_SINK_INDEX]);
#endif


    /*pileline start runing*/
    ret =  UST_SetPipeLineStatus(imp_pipe_line, UST_PIPE_READY);
    if (ret)
    {
        imp_pr_debug("[%s]:Err!!!! UST_SetPipeLineStatus ready fail\n", __FUNCTION__);
        goto out;
    }

    ret =  UST_SetPipeLineStatus(imp_pipe_line, UST_PIPE_PALY);
    if (ret)
    {
        imp_pr_debug("[%s]:Err!!!! UST_SetPipeLineStatus play fail\n", __FUNCTION__);
        goto out;
    }

out:
    return ret ;
}

void main(void)
{
    int ret = 0;

    *(volatile uint32_t *)0x40000090 |= (0x2 << 0);


    ret = imp_pipe_line_config();
    if (ret != 0)
    {
        printk("imp_pipe_line_config fail\n");
    }

    while (1)
    {
        k_sleep(1000);
    }
}

