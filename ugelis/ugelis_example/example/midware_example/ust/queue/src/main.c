/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include<ugelis.h>
#include<misc/printk.h>
#include<ust_core.h>
#include<ust_src_element.h>
#include<ust_mid_element.h>
#include<ust_sink_element.h>


//#define UST_DEMO_DEBUG 1
#define UST_DEMO_ERR   1

#ifdef UST_DEMO_DEBUG
    #define ust_demo_pr_debug       printk
#else
    #define ust_demo_pr_debug(...)  do{}while(0)
#endif

#ifdef UST_DEMO_ERR
    #define ust_demo_pr_err         printk
#else
    #define ust_demo_pr_err(...)    do{}while(0)
#endif


#define PAD_TABLE_SIZE              5

#define MAIN_DEAD_LOOP()            do{                         \
        k_sleep(1000);      \
    }while(1);

typedef enum
{
    ELEMENT_SRC_PAD_SRC_INDEX             = 0,
    ELEMENT_SRCQUEUE_PAD_SRC_INDEX,
    ELEMENT_MIDDLE_PAD_SINK_INDEX,
    ELEMENT_MIDDLE_PAD_SRC_INDEX,
    ELEMENT_SINK_PAD_SINK_INDEX,
} PAD_INDEX_T;



static void ust_pad_link(UST_Pad_T **pad_table);
static int  ust_set_pileline_play(UST_PipeLine_T *pipeLine);
static int ust_get_element(UST_Element_T **pEltSrc, UST_Element_T **peltSrcQueue, UST_Element_T **pEltMiddle, UST_Element_T **pEltsink);
static int ust_get_pad(UST_Pad_T **pad_table, UST_Element_T *eltSrc, UST_Element_T *peltSrcQueue,
                       UST_Element_T *eltMiddle, UST_Element_T *eltsink);
static void ust_add_element_to_pipeline(UST_PipeLine_T *line, UST_Element_T *eltSrc, UST_Element_T *peltSrcQueue,
                                        UST_Element_T *eltMiddle, UST_Element_T *eltsink);




static int ust_get_element(UST_Element_T **pEltSrc, UST_Element_T **peltSrcQueue, UST_Element_T **pEltMiddle, UST_Element_T **pEltsink)
{
    int ret = 0;

    *pEltSrc = UST_GetElement(ELEMENT_SRC);
    if (*pEltSrc == NULL)
    {
        ret = -EINVAL;
        goto out;
    }

    *peltSrcQueue = UST_GetElement(ELEMENT_SRC_QUEUE);
    if (*peltSrcQueue == NULL)
    {
        ret = -EINVAL;
        goto out;
    }


    *pEltMiddle = UST_GetElement(ELEMENT_MIDDLE);
    if (*pEltMiddle == NULL)
    {
        ret = -EINVAL;
        goto out;
    }

    *pEltsink = UST_GetElement(ELEMENT_SRINK);
    if (*pEltsink == NULL)
    {
        ret = -EINVAL;
        goto out;
    }

    ust_demo_pr_debug("<--get element: src_element[name:%s]:%p  mid_element[name:%s]:%p sink_element[name:%s]:%p srcqueue_element[name:%s]:%p----->\n",
                      (*pEltSrc)->name,
                      *pEltSrc,
                      (*pEltMiddle)->name,
                      *pEltMiddle,
                      (*pEltsink)->name,
                      *pEltsink,
                      (*peltSrcQueue)->name,
                      *peltSrcQueue);

out:
    return ret;
}


static void ust_add_element_to_pipeline(UST_PipeLine_T *line, UST_Element_T *eltSrc, UST_Element_T *peltSrcQueue,
                                        UST_Element_T *eltMiddle, UST_Element_T *eltsink)
{
    UST_ElementAddToPipeLine(line, eltSrc);
    UST_ElementAddToPipeLine(line, eltMiddle);
    UST_ElementAddToPipeLine(line, eltsink);
    UST_ElementAddToPipeLine(line, peltSrcQueue);
}


static int ust_get_pad(UST_Pad_T **pad_table, UST_Element_T *eltSrc, UST_Element_T *eltSrcQueue,
                       UST_Element_T *eltMiddle, UST_Element_T *eltsink)
{
    int ret = 0;

    pad_table[ELEMENT_SRC_PAD_SRC_INDEX] = UST_GetPadFromElement(eltSrc, UST_SRC_PAD, ELEMENT_SRC_PAD_SRC);
    if (pad_table[ELEMENT_SRC_PAD_SRC_INDEX] == NULL)
    {
        ret = -EINVAL;
        goto out;
    }

    pad_table[ELEMENT_SRCQUEUE_PAD_SRC_INDEX] = UST_GetPadFromElement(eltSrcQueue, UST_SRC_PAD, ELEMENT_SRC_QUEUE_PAD_SRC);
    if (pad_table[ELEMENT_SRCQUEUE_PAD_SRC_INDEX] == NULL)
    {
        ret = -EINVAL;
        goto out;
    }

    pad_table[ELEMENT_MIDDLE_PAD_SINK_INDEX] = UST_GetPadFromElement(eltMiddle, UST_SINK_PAD, ELEMENT_MIDDLE_PAD_SINK);
    if (pad_table[ELEMENT_MIDDLE_PAD_SINK_INDEX] == NULL)
    {
        ret = -EINVAL;
        goto out;
    }

    pad_table[ELEMENT_MIDDLE_PAD_SRC_INDEX] = UST_GetPadFromElement(eltMiddle, UST_SRC_PAD, ELEMENT_MIDDLE_PAD_SRC);
    if (pad_table[ELEMENT_MIDDLE_PAD_SRC_INDEX] == NULL)
    {
        ret = -EINVAL;
        goto out;
    }

    pad_table[ELEMENT_SINK_PAD_SINK_INDEX] = UST_GetPadFromElement(eltsink, UST_SINK_PAD, ELEMENT_SINK_PAD_SINK);
    if (pad_table[ELEMENT_SINK_PAD_SINK_INDEX] == NULL)
    {
        ret = -EINVAL;
        goto out;
    }

    ust_demo_pr_debug("[%s] srcElement:srcpad=%p midElement:sinkpad=%p midElement:srcpad=%p sinkElement:sinkpad=%p  srcQueueElement:srcpad=%p\n\n",
                      __FUNCTION__,
                      pad_table[ELEMENT_SRC_PAD_SRC_INDEX],
                      pad_table[ELEMENT_MIDDLE_PAD_SINK_INDEX],
                      pad_table[ELEMENT_MIDDLE_PAD_SRC_INDEX],
                      pad_table[ELEMENT_SINK_PAD_SINK_INDEX],
                      pad_table[ELEMENT_SRCQUEUE_PAD_SRC_INDEX]
                     );


out:
    return ret;
}

static void ust_pad_link(UST_Pad_T **pad_table)
{
    UST_Pad_T *eltMiddleSinkPad     =  pad_table[ELEMENT_MIDDLE_PAD_SINK_INDEX];
    UST_Pad_T *eltMiddleSrcPad      =  pad_table[ELEMENT_MIDDLE_PAD_SRC_INDEX];
    UST_Pad_T *eltSrcQueuesrcPad    =  pad_table[ELEMENT_SRCQUEUE_PAD_SRC_INDEX];
    UST_Pad_T *sinkPad              =  pad_table[ELEMENT_SINK_PAD_SINK_INDEX];

    UST_PadConnect(eltSrcQueuesrcPad, eltMiddleSinkPad);
    UST_PadConnect(eltMiddleSrcPad, sinkPad);
}


static int  ust_set_pileline_play(UST_PipeLine_T *pipeLine)
{
    int ret = 0;

    ret =  UST_SetPipeLineStatus(pipeLine, UST_PIPE_READY);
    if (ret)
    {
        goto out;
    }

    ret =  UST_SetPipeLineStatus(pipeLine, UST_PIPE_PALY);
    if (ret)
    {
        goto out;
    }

out:
    return ret;
}


int main(void)
{

    int ret = 0;
    UST_Element_T *eltSrc          = NULL;
    UST_Element_T *peltSrcQueue    = NULL;
    UST_Element_T *eltMiddle       = NULL;
    UST_Element_T *eltsink         = NULL;
    UST_PipeLine_T *pipeLine       = NULL;
    UST_Pad_T *pad_table[PAD_TABLE_SIZE];

    /*alloc pileline*/
    pipeLine = UST_AllocPileLine();
    if (!pipeLine)
    {
        ust_demo_pr_err("[%s]:Err!!!! UST_AllocPileLine NULL\n", __FUNCTION__);
        ret = -ENOMEM;
        goto out;
    }

    /**
     *connent element connet to pileLine  link as bellow
     *pileline-->eltSrc-->peltSrcQueue->eltMiddle--->srink
     */
    ret = ust_get_element(&eltSrc, &peltSrcQueue, &eltMiddle, &eltsink);
    if (ret)
    {
        ust_demo_pr_err("[%s]:Err!!!!: ust_get_element NULL\n", __FUNCTION__);
        goto out;
    }

    ust_add_element_to_pipeline(pipeLine, eltSrc, peltSrcQueue, eltMiddle, eltsink);

    /**
     * connet pipeLine pad sequence
     * srcPad<--link-->peltSrcQueueSinkPad (plugin auto link by core, App not care)
     * peltSrcQueueSrcPad<---link---->eltMiddle1SinkPad
     * eltMiddle1SrcPad<---link---->eltsinkPad
     */
    ret = ust_get_pad(pad_table, eltSrc, peltSrcQueue, eltMiddle, eltsink);
    if (ret)
    {
        ust_demo_pr_err("[%s]:Err!!!! ust_get_pad NULL\n", __FUNCTION__);
        goto out;
    }
    ust_pad_link(pad_table);

    /*pileline start runing*/
    ret = ust_set_pileline_play(pipeLine);
    if (ret)
    {
        ust_demo_pr_err("[%s]:Err!!!! ust_set_pileline_play\n", __FUNCTION__);
        goto out;
    }


out:
    MAIN_DEAD_LOOP();
    return ret ;

}






