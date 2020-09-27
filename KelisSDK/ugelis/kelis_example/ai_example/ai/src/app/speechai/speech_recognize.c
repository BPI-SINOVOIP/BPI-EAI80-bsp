/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        speech_recognize.c
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2020/03/11
 *
 * @brief       speech recognize
 *
 * @note
 *              2019/11/02, Robert.Hu, v0.8.0
 *                  Initial version.
 *              2020/03/10, Ze.Hong, v0.9.0
 *                  Generate a completed speech demo
 *              2020/03/11, Angrad.Yang, v1.0.0
 *                  Reorganize and Optimize the code
 **/

#include <gm_hal_gpio.h>
#include <pinmux.h>

#include "speechai/speech_recognize.h"
#include "inc/config.h"
#include "worker/painter_worker.h"

#include "RingBuffer.h"
#include "DSpotterSDKApi.h"
#include "DSpotterSDKApi_Const.h"
#include "Group_1.h"
#include "Group_2.h"
#include "CYBase.h"

#include "index2command.h"

#define SKIP_LEADING_FRAME_COUNT    2       // DMic record data is unstable at leading time.
#define DMIC_DC_OFFSET              11000   // The DC offset of DMic record data.
#define VOLUME_SCALE_PERCENTAGE     300
#define MAX_COMMAND_TIME            (3000 / 10)       // Command must be spoke in 3000 ms.
#define RECOGNITION_MODE_TRIGGER    0
#define RECOGNITION_MODE_COMMAND    1
#define TIMEOUT_TIME_IN_MS          4000*1000
#ifdef F6721B_EVB_QFN80PIN
    #define PDM_MASTER_DEVICE       PDMA_DEV
#else
    #define PDM_MASTER_DEVICE       PDMB_DEV
#endif
typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

void *DSpotterReInit(speech_recognized_data_t *sp, void *hDSpotter, int nRecognitionMode);
struct PDM_Handle_T;

void pdmReceiveCallback(struct PDM_Handle_T *pPDM, PDM_Event_T event, unsigned long param)
{
    speech_recognized_data_t *sp = (speech_recognized_data_t *)param;
    if (event == PDM_DMAMST_RECEIVE_EVENT)
    {
        if (RingBufferGetFreeSize(sp->g_hRingBuffer) < DSPOTTER_FRAME_SIZE)
        {
            printf("ERROR : No free ring buffer, data lost!\r\n");
        }
        else
        {
            // Put DMic record data to ring buffer
            RingBufferPutData(sp->g_hRingBuffer, sp->g_rdbuf[0], DSPOTTER_FRAME_SIZE);
        }
    }
    else if (event == PDM_DMASLV_RECEIVE_EVENT)
    {
        if (RingBufferGetFreeSize(sp->g_hRingBuffer) < DSPOTTER_FRAME_SIZE)
        {
            printf("ERROR : No free ring buffer, data lost!\r\n");
        }
        else
        {
            // Put DMic record data to ring buffer
            RingBufferPutData(sp->g_hRingBuffer, sp->g_rdbuf[1], DSPOTTER_FRAME_SIZE);
        }
    }
}


#ifdef BPI
#else
/** Configure B9/B10 enable */
RET_CODE AIoT_Config(void)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;

    HAL_GPIO_PinWrite(GPIOB9, GPIO_PIN_RESET);

    GPIO_InitStruct.pin = GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;

    ret = HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        ret = RET_ERROR;
    }
    HAL_GPIO_PinWrite(GPIOB9, GPIO_PIN_RESET);
		
    HAL_GPIO_PinWrite(GPIOB10, GPIO_PIN_RESET);

    GPIO_InitStruct.pin = GPIO_PIN_10;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;

    ret = HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        ret = RET_ERROR;
    }
    HAL_GPIO_PinWrite(GPIOB10, GPIO_PIN_RESET);		
    return ret;
}

#endif

/** Configure pinmux for an available PDM device */
RET_CODE PDMAPinMuxConfig()
{
    int ret = 0;
    struct device *portj = device_get_binding(CONFIG_PINMUX_GM_PORTJ_NAME);
    if (portj == NULL)
    {
        return -1;
    }
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_pdmPinMux[2] =
    {
        {PINMUX_GPIO_PIN_3, GPIOJ, GPIOJ3_AF4_PDM_BCK_A},
        {PINMUX_GPIO_PIN_4, GPIOJ, GPIOJ4_AF4_PDM_SD_A},
    };
    pin_cfg.pin = BIT(s_pdmPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_pdmPinMux[0].alternate;
    ret = pinmux_pin_set(portj, s_pdmPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_pdmPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_pdmPinMux[1].alternate;
    ret = pinmux_pin_set(portj, s_pdmPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }
    return 0;
}

RET_CODE PDMBPinMuxConfig()
{
    int ret = 0;
#if defined(BANPIBOARD_QFN80PIN)
    struct device *portd = device_get_binding(CONFIG_PINMUX_GM_PORTJ_NAME);
#else
    struct device *portd = device_get_binding(CONFIG_PINMUX_GM_PORTD_NAME);
#endif
    if (portd == NULL)
    {
        return -1;
    }
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_pdmPinMux[2] =
    {
#if defined(BANPIBOARD_QFN80PIN)
        {PINMUX_GPIO_PIN_5, GPIOJ, GPIOJ5_AF4_PDM_SD_B},
        {PINMUX_GPIO_PIN_7, GPIOJ, GPIOJ7_AF4_PDM_BCK_B},
#else
        {PINMUX_GPIO_PIN_10, GPIOD, GPIOD10_AF5_PDM_BCK_B},
        {PINMUX_GPIO_PIN_8, GPIOD,  GPIOD8_AF5_PDM_SD_B},
#endif
    };
    pin_cfg.pin = BIT(s_pdmPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_pdmPinMux[0].alternate;
    ret = pinmux_pin_set(portd, s_pdmPinMux[0].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_pdmPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_pdmPinMux[1].alternate;
    ret = pinmux_pin_set(portd, s_pdmPinMux[1].pin, (unsigned int)&pin_cfg);
    if (ret)
    {
        return ret;
    }
    return 0;
}

void pdm_clk_choose(void)
{
    HW32_REG(0x40000198) |= (1 << 0);  //D_PLLPDM_EN
    HW32_REG(0x40000190) |= (1 << 0);  //D_PLLI2S_EN
    HW32_REG(0x400000ac) |= (1 << 4);  //PDMA_CLK_SEL  48M
    HW32_REG(0x400000ac) |= (1 << 6);  //PDMB_CLK_SEL
    HW32_REG(0x400000ac) |= (1 << 12); //PDMC_CLK_SEL
    HW32_REG(0x400000ac) |= (1 << 14); //PDMD_CLK_SEL
}

static int pdm_init(speech_recognized_data_t *sp)
{
    PDM_Config_T pdm_config;
    RET_CODE retval;

    /* init system */
    pdm_clk_choose();
#ifdef F6721B_EVB_QFN80PIN
    PDMAPinMuxConfig();
#else
    PDMBPinMuxConfig();
#endif
    pdm_config.DataSize            = PDM_DATASIZE_16;
    pdm_config.Fifo_Thr            = PDM_FIFO_THRESHOLD_FULL;
    pdm_config.Cid                 = DISABLE;
    pdm_config.CH_FLAG             = PDM_CH_RIGHT;
    pdm_config.Capt                = ENABLE;
    pdm_config.Shift_Level         = PDM_SHIFT_LEVEL_6;
    pdm_config.bckdiv              = PDM_BCK_1M;
    pdm_config.FS                  = PDM_FS_16K;
    pdm_config.Dual_Ch             = PDM_Dual_R;  //right channel
    pdm_config.NBSlot              = PDM_NB_SLOT_2;
    pdm_config.SlotSize            = PDM_SLOTSZ_32;
    pdm_config.enableDMA           = TRUE;
    pdm_config.dmaBufferLength     = sizeof(sp->g_dmabuffer) / sizeof(int32_t);
    pdm_config.pdmaBuffer          = sp->g_dmabuffer;
    pdm_config.pdm_callback        = (PDM_Callback_INT_T)pdmReceiveCallback;
    pdm_config.param               = (uint32_t)sp;

    retval = HAL_PDM_Init(&sp->g_PDMSlvHandle, PDM_MASTER_DEVICE, &pdm_config);
    if (retval != HAL_PDM_OK)
    {
        printf("HAL_PDM_Init() fail %d!\n", retval);
        return -1;
    }

    // Start DMA record
    retval = HAL_PDM_Receive_DMA(&sp->g_PDMSlvHandle, PDM_MASTER_MODE, (int16_t *)sp->g_rdbuf[0], 2 * DSPOTTER_FRAME_SIZE / sizeof(int16_t));
    if (retval != HAL_PDM_OK)
    {
        printf("HAL_PDM_Receive_DMA() fail %d!\n", retval);
        return -1;
    }
#ifdef BPI
#else
	printf("AIoT: INIT\r\n");	
  AIoT_Config();		
  HAL_GPIO_SetPinDirection(GPIOB9, GPIO_PIN_OUTPUT);
  HAL_GPIO_PinWrite(GPIOB9, GPIO_PIN_RESET);		
  HAL_GPIO_SetPinDirection(GPIOB10, GPIO_PIN_OUTPUT);
  HAL_GPIO_PinWrite(GPIOB10, GPIO_PIN_RESET);		
#endif
    return 0;
}

static int dnn_init(speech_recognized_data_t *sp)
{
    int nDSpotterMemSize, nDSpotterMemSize1, nDSpotterMemSize2;
    uint8_t *lppbyModel[1];

    /* init dnn */
    // Initial ring buffer
    RingBufferInit(sp->g_byaRingBuffer, RING_BUFFER_GET_MEM_USAGE(RBUF_SIZE), RBUF_SIZE, &sp->g_hRingBuffer);
    if (sp->g_hRingBuffer == NULL)
    {
        printf("RingBufferInit() fail!\r\n");
        return -1;
    }

    // Check DSpotter memory
    lppbyModel[0] = (uint8_t *)g_lpdwGroup_1;
    nDSpotterMemSize1 = DSpotter_GetMemoryUsage_Multi((uint8_t *)g_lpdwCYBase, lppbyModel, 1, MAX_COMMAND_TIME);
    lppbyModel[0] = (uint8_t *)g_lpdwGroup_2;
    nDSpotterMemSize2 = DSpotter_GetMemoryUsage_Multi((uint8_t *)g_lpdwCYBase, lppbyModel, 1, MAX_COMMAND_TIME);
    nDSpotterMemSize = max(nDSpotterMemSize1, nDSpotterMemSize2);
    //printf("The needed working memory size = %d, %d\r\n", nDSpotterMemSize1, nDSpotterMemSize2);
    //printf("The declare memory buffer size = %d\r\n", sizeof(sp->g_byDSpotterMem));
    if (sizeof(sp->g_byDSpotterMem) < nDSpotterMemSize)
    {
        printf("Please define DSPOTTER_MEM_SIZE more large!\r\n");
        return -1;
    }
#if 0
    printf("Trigger word list: \r\n");
    for (i = 0; i < GetTriggerCount(); i++)
    {
        printf("    %s\r\n", GetTriggerStringByIndex(i));
    }

    printf("\r\n");
    printf("Command word list: \r\n");
    for (i = 0; i < GetCommandCount(); i++)
    {
        printf("    %s\r\n", GetCommandStringByIndex(i));
    }
#endif
    // Initial DSpotter
    sp->hDSpotter = DSpotterReInit(sp, sp->hDSpotter, RECOGNITION_MODE_TRIGGER);
    if (sp->hDSpotter == NULL)
    {
        return -1;
    }

    return 0;
}

void *DSpotterReInit(speech_recognized_data_t *sp, void *hDSpotter, int nRecognitionMode)
{
    int nRet = DSPOTTER_ERR_IllegalParam;
    uint8_t *lppbyModel[1];

    if (hDSpotter != NULL)
    {
        DSpotter_Release(hDSpotter);
        hDSpotter = NULL;
    }

    sp->g_nMode = nRecognitionMode;

    switch (nRecognitionMode)
    {
        case RECOGNITION_MODE_TRIGGER:
            lppbyModel[0] = (uint8_t *)g_lpdwGroup_1;
            hDSpotter = DSpotter_Init_Multi((uint8_t *)g_lpdwCYBase, lppbyModel, 1, MAX_COMMAND_TIME,
                                            (BYTE *)sp->g_byDSpotterMem, sizeof(sp->g_byDSpotterMem), NULL, 0, &nRet);
            //printf("Enter trigger mode\r\n");
            break;

        case RECOGNITION_MODE_COMMAND:
            lppbyModel[0] = (uint8_t *)g_lpdwGroup_2;
            hDSpotter = DSpotter_Init_Multi((uint8_t *)g_lpdwCYBase, lppbyModel, 1, MAX_COMMAND_TIME,
                                            (BYTE *)sp->g_byDSpotterMem, sizeof(sp->g_byDSpotterMem), NULL, 0, &nRet);
            //printf("Enter command mode, please speak command in %d seconds.\r\n", TIMEOUT_TIME_IN_MS / 1000);
            break;
    }

    if (hDSpotter == NULL)
    {
        printf("DSpotter_Init() fail, error = %d!\r\n", nRet);
    }

    return hDSpotter;
}

void speech_ui_show(worker_t *painter, paint_info_t *info, int index, int color)
{
    int col, line;
    info->w = 100 * DESIGN_W;
    info->h = 16 * DESIGN_W;
    info->color = color;
    line = (index % 10);
    col  = index / 10;
    info->x = CAM_DISP_X + 4 + col * (60 * DESIGN_W + 20);
    info->y = 4 + line * 20 * DESIGN_W;
    info->str = GetCommandText(index);
    painter->run(painter, CMD_PAINT_SHOWCH, info);
}

void speech_ui_notice(worker_t *painter, paint_info_t *info)
{
    info->x = CAM_DISP_X + 4;
    info->y = CAM_DISP_H - 20 * DESIGN_W;
    info->w = CAM_DISP_W - 20 * DESIGN_W;
    info->h = 16 * DESIGN_W;
    info->color = UI_FOCUS_COLOR;
    info->str = GetNoticeText(0);
    painter->run(painter, CMD_PAINT_SHOWCH, info);
}

void speech_ui_init(worker_t *painter, paint_info_t *info, int color)
{
    int index = 0, n;
    paint_info_t info_init;
    info_init.x = CAM_DISP_X;
    info_init.y = 0;
    info_init.w = DEV_LCD_W - CAM_DISP_X;
    info_init.h = DEV_LCD_H;
    info_init.color = 0xFFFFFFFF;
    painter->run(painter, CMD_PAINT_INIT, &info_init);

    n = GetCommandCount();
    while (index < n)
    {
        speech_ui_show(painter, info, index, color);
        index++;
    }
}

void speech_recognize_thread(void *data, void *param, void *not_used)
{
    easynet_proc_param_t *ep = (easynet_proc_param_t *)data;
    speech_recognized_data_t *sp = ep->data;
    short *lpsaSample;
    int i, n;
    int nRealSize;
    int nRet;
    int nCommandRecordSample;
    int nRecordFrameCount = 0;
    bool bOverLimit;

    worker_t *painter = get_worker("painter");
    paint_info_t notice;
    speech_ui_init(painter, &notice, 0xFFEEEEEE);
    speech_ui_notice(painter, &notice);

    dnn_init(sp);
    pdm_init(sp);

    while (ep->running)
    {
        k_yield();
        if (RingBufferGetDataSize(sp->g_hRingBuffer) < DSPOTTER_FRAME_SIZE)
        {
            continue;
        }

        nRecordFrameCount++;

#ifdef SKIP_LEADING_FRAME_COUNT
        // Drop the unstable leading data.
        if (nRecordFrameCount <= SKIP_LEADING_FRAME_COUNT)
        {
            continue;
        }
#endif

        // DSpotter AddSample
        RingBufferGetDataBuffer(sp->g_hRingBuffer, DSPOTTER_FRAME_SIZE, (void **)&lpsaSample, &nRealSize, NULL, NULL);
        if (nRealSize != DSPOTTER_FRAME_SIZE)
        {
            printf("RingBufferGetDataBuffer() problem \r\n");
        }
        bOverLimit = false;
        for (i = 0; i < nRealSize / 2; i++)
        {
            n = ((int)lpsaSample[i] - DMIC_DC_OFFSET) * sp->g_nVolumeScalePercentage / 100;
            //The boundary check for "short" value.
            if (n > 32767)
            {
                n = 32767;
                bOverLimit = true;
            }
            else if (n < -32768)
            {
                n = -32768;
                bOverLimit = true;
            }
            lpsaSample[i] = (short)n;
        }
        if (bOverLimit && sp->g_nVolumeScalePercentage > 100)
        {
            sp->g_nVolumeScalePercentage--;
            printf("g_nVolumeScalePercentage = %d\n", sp->g_nVolumeScalePercentage);
        }
        nRet = DSpotter_AddSample(sp->hDSpotter, lpsaSample, nRealSize / 2);
        RingBufferDequeueData(sp->g_hRingBuffer, nRealSize);
        if (nRet == DSPOTTER_ERR_NeedMoreSample)
        {
            if (sp->g_nMode == RECOGNITION_MODE_COMMAND)
            {
                // Check timeout for command recognition mode
                nCommandRecordSample += nRealSize / sizeof(short);
                if (nCommandRecordSample > 16000 / 1000 * TIMEOUT_TIME_IN_MS)   // Check timeout
                {
                    printf("Timeout!\r\n");
                    printf("\r\n");
#ifdef BPI
#else									
										printf("AIoT: BYE\r\n");										
                    HAL_GPIO_SetPinDirection(GPIOB9, GPIO_PIN_OUTPUT);
                    HAL_GPIO_PinWrite(GPIOB9, GPIO_PIN_RESET);
#endif
                    sp->hDSpotter = DSpotterReInit(sp, sp->hDSpotter, RECOGNITION_MODE_TRIGGER);
                    if (sp->hDSpotter == NULL)
                    {
                        break;
                    }
                }
            }
        }
        else if (nRet == DSPOTTER_SUCCESS)
        {
            int nCmdIndex = DSpotter_GetResult(sp->hDSpotter);
            if (sp->g_nMode == RECOGNITION_MODE_TRIGGER)
            {
                if (GetTriggerStringByIndex(nCmdIndex) != NULL)
                {
                    printf("Get trigger : %s(%d)\r\n", GetTriggerStringByIndex(nCmdIndex), nCmdIndex);
#ifdef BPI
#else
									if(nCmdIndex==0) {
										 printf("AIoT: GO\r\n");
                     HAL_GPIO_SetPinDirection(GPIOB9, GPIO_PIN_OUTPUT);
                     HAL_GPIO_PinWrite(GPIOB9, GPIO_PIN_SET);										
									}
									else {
										 printf("AIoT: BYE\r\n");										
                     HAL_GPIO_SetPinDirection(GPIOB9, GPIO_PIN_OUTPUT);
                     HAL_GPIO_PinWrite(GPIOB9, GPIO_PIN_RESET);	
                  }										
#endif	
									
                    // show string on lcd
                    speech_ui_init(painter, &notice, UI_BKGRD_COLOR);
                    //speech_ui_show(painter, &notice, nCmdIndex, UI_FOCUS_COLOR);
                    //lcd_showString(RECOGNITION_MODE_TRIGGER, nCmdIndex);

                    nCommandRecordSample = 0;

                    // Go to the command mode
                    sp->hDSpotter = DSpotterReInit(sp, sp->hDSpotter, RECOGNITION_MODE_COMMAND);
                    if (sp->hDSpotter == NULL)
                    {
                        break;
                    }
                }
                else
                {
                    printf("Reject trigger index %d\r\n", nCmdIndex);
                }
            }
            else if (sp->g_nMode == RECOGNITION_MODE_COMMAND)
            {
                if (GetCommandStringByIndex(nCmdIndex) != NULL)
                {
                    printf("Get command : %s(%d)\r\n", GetCommandStringByIndex(nCmdIndex), nCmdIndex);
#ifdef BPI
#else
									if(nCmdIndex==0) {
										 printf("AIoT: ON\r\n");
                     HAL_GPIO_SetPinDirection(GPIOB10, GPIO_PIN_OUTPUT);
                     HAL_GPIO_PinWrite(GPIOB10, GPIO_PIN_SET);										
									}
									else {
										 printf("AIoT: OFF\r\n");										
                     HAL_GPIO_SetPinDirection(GPIOB10, GPIO_PIN_OUTPUT);
                     HAL_GPIO_PinWrite(GPIOB10, GPIO_PIN_RESET);	
                  }										
#endif	
									
                    // show string on lcd
                    if (sp->last_cmd_index >= 0)
                    {
                        speech_ui_show(painter, &notice, sp->last_cmd_index, UI_BKGRD_COLOR);
                    }
                    speech_ui_show(painter, &notice, nCmdIndex, UI_FOCUS_COLOR);
                    sp->last_cmd_index = nCmdIndex;
                    //lcd_showString(RECOGNITION_MODE_COMMAND, nCmdIndex);

                    // Return to trigger mode
                    //hDSpotter = DSpotterReInit(hDSpotter, RECOGNITION_MODE_TRIGGER);
                    sp->hDSpotter = DSpotterReInit(sp, sp->hDSpotter, RECOGNITION_MODE_COMMAND);
                    if (sp->hDSpotter == NULL)
                    {
                        break;
                    }
                }
            }
            DSpotter_Reset(sp->hDSpotter);
        }
        else if (nRet == DSPOTTER_ERR_Expired)
        {
            printf("The trail version DSpotter reach the max trial usage count, please reset system.\r\n");
            break;
        }
        else
        {
            printf("DSpotter_AddSample() un-handle nRet = %d\r\n", nRet);
            break;
        }
    }

    HAL_PDM_Exit(&sp->g_PDMSlvHandle);

    /* abort asr, release resource and return*/
    sp->g_PDMSlvHandle.hDMA->state = DMA_STATE_INITIAL;
    nRet = HAL_DMA_Abort(sp->g_PDMSlvHandle.hDMA);

    RingBufferRelease(sp->g_hRingBuffer);
    sp->g_hRingBuffer = NULL;

    if (sp->hDSpotter != NULL)
    {
        DSpotter_Release(sp->hDSpotter);
        sp->hDSpotter = NULL;
    }
    kfree(sp->g_byaRingBuffer);
    kfree(sp);
    painter->run(painter, CMD_PAINT_INIT, 0);
    ep->finish_cb(0, param);
}

int speech_recognize_init(easynet_proc_param_t *ep)
{
    speech_recognized_data_t *sp = (speech_recognized_data_t *)kmalloc(sizeof(speech_recognized_data_t));
    memset(sp, 0, sizeof(speech_recognized_data_t));
    ep->running = 1;
    ep->data = sp;
    sp->asr_status = ASR_STATUS_ENTRY;
    sp->g_nMode = RECOGNITION_MODE_TRIGGER;
    sp->g_nVolumeScalePercentage = VOLUME_SCALE_PERCENTAGE;
    sp->g_byaRingBuffer = (char *)kmalloc(RING_BUFFER_GET_MEM_USAGE(RBUF_SIZE));
    sp->last_cmd_index  = -1;
    return 0;
}

int speech_recognize_exit(easynet_proc_param_t *ep)
{
    ep->running = 0;
    return 0;
}
