/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file pdm_example.c
 *
 * @version 1.0.0
 *
 * @date 2019/11/02
 *
 * @contents Verification and example code of PDM.
 *
 * @history
 *  Date               By             Comments          Ver.
 *  =========       =========        =========        =========
 *  2019/11/02    Robert_hu       Create           1.0.0
 **********************************************************************/
#include <gm_common.h>
//#include "stdio.h"
#include <gm_hal_pdm.h>
#include <pinmux.h>
#include <gm_hal_clock.h>
#include <gm_hal_reset.h>
#include <gm_hal_rtc.h>
#include <gm_hal_dma.h>
#include <gm_hal_gpio.h>
#include <ugelis.h>
#include <device.h>
#include <init.h>
#include "DSpotterSDKApi.h"
#include "DSpotterSDKApi_Const.h"
#include "RingBuffer.h"
#include "./Model/CYBase.h"
#include "./Model/Group_1.h"
#include "./Model/Group_2.h"
#include "./Model/IndexToCommand.h"

/******************************************************************************
* Constants and macros
******************************************************************************/
#define PDM_MASTER_DEVICE       (PDMA_DEV)

#define SKIP_LEADING_FRAME_COUNT 2       // DMic record data is unstable at leading time.
#define DMIC_DC_OFFSET           11000   // The DC offset of DMic record data.
#define VOLUME_SCALE_PERCENTAGE  300

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

#define DSPOTTER_FRAME_SIZE		 960				  // Must be multiple times of 4, 960 is equal to 30 ms data
#define DMA_BUFFER_SIZE          (DSPOTTER_FRAME_SIZE*2)
#define RBUF_SIZE				 (DSPOTTER_FRAME_SIZE*100)
#define DSPOTTER_MEM_SIZE		 105000				  // Please modify this number by the return value of DSpotter_GetMemoryUsage_XXX.
#define MAX_COMMAND_TIME		 (3000 / 10)		  // Command must be spoke in 3000 ms.

#define RECOGNITION_MODE_TRIGGER  0
#define RECOGNITION_MODE_COMMAND  1

#define TIMEOUT_TIME_IN_MS        4000*1000

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
HANDLE DSpotterReInit(HANDLE hDSpotter, int nRecognitionMode);

/******************************************************************************
* Global variables
******************************************************************************/
static PDM_Handle_T    g_PDMSlvHandle;
static int16_t g_rdbuf[2][DSPOTTER_FRAME_SIZE/sizeof(int16_t)] = {0};
static int32_t g_dmabuffer[DMA_BUFFER_SIZE/sizeof(int32_t)] = {0};

HANDLE g_hRingBuffer = NULL;
BYTE g_byaRingBuffer[RING_BUFFER_GET_MEM_USAGE(RBUF_SIZE)];
BYTE g_byDSpotterMem[DSPOTTER_MEM_SIZE];		  // The memory for DSpotter engine
int g_nMode = RECOGNITION_MODE_TRIGGER;
int g_nVolumeScalePercentage = VOLUME_SCALE_PERCENTAGE;

void pdmReceiveCallback(struct PDM_Handle_T *pPDM, uint8_t event, unsigned long param)
{
    if (event == PDM_DMAMST_RECEIVE_EVENT)
    {
		if (RingBufferGetFreeSize(g_hRingBuffer) < DSPOTTER_FRAME_SIZE)
		{
			printf("ERROR : No free ring buffer, data lost!\r\n");
		}
		else
		{
			// Put DMic record data to ring buffer
			RingBufferPutData(g_hRingBuffer, g_rdbuf[0], DSPOTTER_FRAME_SIZE);
		}
    }
    else if (event == PDM_DMASLV_RECEIVE_EVENT)
    {
		if (RingBufferGetFreeSize(g_hRingBuffer) < DSPOTTER_FRAME_SIZE)
		{
			printf("ERROR : No free ring buffer, data lost!\r\n");
		}
		else
		{
			// Put DMic record data to ring buffer
			RingBufferPutData(g_hRingBuffer, g_rdbuf[1], DSPOTTER_FRAME_SIZE);
		}
    }
}

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
    struct device *portd = device_get_binding(CONFIG_PINMUX_GM_PORTD_NAME);
    if (portd == NULL)
    {
        return -1;
    }
    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_pdmPinMux[2] =
    {
        {PINMUX_GPIO_PIN_2, GPIOD, GPIOD2_AF3_PDM_BCK_B},
        {PINMUX_GPIO_PIN_0, GPIOD, GPIOD0_AF3_PDM_SD_B},
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
    HW32_REG(0x40000198) |= (1 << 0); //D_PLLPDM_EN
    HW32_REG(0x40000190) |= (1 << 0); //D_PLLI2S_EN
    HW32_REG(0x400000ac) |= (1 << 4); //PDMA_CLK_SEL  48M
    HW32_REG(0x400000ac) |= (1 << 6); //PDMB_CLK_SEL
    HW32_REG(0x400000ac) |= (1 << 12); //PDMC_CLK_SEL
    HW32_REG(0x400000ac) |= (1 << 14); //PDMD_CLK_SEL
}

extern void set_ui_layer(int x, int y);

int main()
{
    int ret = 0;
    PDM_Config_T pdm_config;
	short *lpsaSample;
	int nDSpotterMemSize, nDSpotterMemSize1, nDSpotterMemSize2;
	HANDLE hDSpotter = NULL;
	BYTE *lppbyModel[1];
    RET_CODE retval;
	int i, n;
	int nRealSize;
	int nRet;
	int nCommandRecordSample;
	int nRecordFrameCount = 0;
	bool bOverLimit;

	printf("test_pdm_dma Start\n");

	// Initial ring buffer
	RingBufferInit(g_byaRingBuffer, RING_BUFFER_GET_MEM_USAGE(RBUF_SIZE), RBUF_SIZE, &g_hRingBuffer);
	if (g_hRingBuffer == NULL)
	{
		printf("RingBufferInit() fail!\r\n");
		goto PROGRAM_END;
	}
    
	// Check DSpotter memory
	lppbyModel[0] = (BYTE *)g_lpdwGroup_1;
	nDSpotterMemSize1 = DSpotter_GetMemoryUsage_Multi((BYTE *)g_lpdwCYBase, lppbyModel, 1, MAX_COMMAND_TIME);
	lppbyModel[0] = (BYTE *)g_lpdwGroup_2;
	nDSpotterMemSize2 = DSpotter_GetMemoryUsage_Multi((BYTE *)g_lpdwCYBase, lppbyModel, 1, MAX_COMMAND_TIME);
	nDSpotterMemSize = max(nDSpotterMemSize1, nDSpotterMemSize2);
	printf("The needed working memory size = %d, %d\r\n", nDSpotterMemSize1, nDSpotterMemSize2);
	printf("The declare memory buffer size = %d\r\n", sizeof(g_byDSpotterMem));
	if (sizeof(g_byDSpotterMem) < nDSpotterMemSize)
	{
		printf("Please define DSPOTTER_MEM_SIZE more large!\r\n");
		goto PROGRAM_END;
	}

	printf("Trigger word list: \r\n");
	for (i = 0; i < GetTriggerCount(); i++)
		printf("    %s\r\n", GetTriggerStringByIndex(i) );

	printf("\r\n");
	printf("Command word list: \r\n");
	for (i = 0; i < GetCommandCount(); i++)
		printf("    %s\r\n", GetCommandStringByIndex(i) );
	
	// Initial DSpotter
	hDSpotter = DSpotterReInit(hDSpotter, RECOGNITION_MODE_TRIGGER);
	if (hDSpotter == NULL)
		goto PROGRAM_END;
	
	pdm_clk_choose();
	PDMAPinMuxConfig();
    
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
	pdm_config.dmaBufferLength     = sizeof(g_dmabuffer) / sizeof(int32_t);
	pdm_config.pdmaBuffer          = g_dmabuffer;
	pdm_config.pdm_callback        = pdmReceiveCallback;

	retval = HAL_PDM_Init(&g_PDMSlvHandle, PDM_MASTER_DEVICE, &pdm_config);
	if (retval != HAL_PDM_OK)
	{
		printf("HAL_PDM_Init() fail %d!\n", retval);
		goto PROGRAM_END;
	}
    
	// Start DMA record
	retval = HAL_PDM_Receive_DMA(&g_PDMSlvHandle, PDM_MASTER_MODE, (int32_t *)g_rdbuf[0], 2*DSPOTTER_FRAME_SIZE/sizeof(int16_t));
	if (retval != HAL_PDM_OK)
	{
		printf("HAL_PDM_Receive_DMA() fail %d!\n", retval);
		goto PROGRAM_END;
	}
    set_ui_layer(0, 0);
    lcd_showString(2, 0);

	while (1)
	{
		if (RingBufferGetDataSize(g_hRingBuffer) < DSPOTTER_FRAME_SIZE)
			continue;
		
		nRecordFrameCount++;
		
#ifdef SKIP_LEADING_FRAME_COUNT
		// Drop the unstable leading data.
		if (nRecordFrameCount <= SKIP_LEADING_FRAME_COUNT)
			continue;
#endif

		// DSpotter AddSample
		RingBufferGetDataBuffer(g_hRingBuffer, DSPOTTER_FRAME_SIZE, (void**)&lpsaSample, &nRealSize, NULL, NULL);
		if (nRealSize != DSPOTTER_FRAME_SIZE)
			printf("RingBufferGetDataBuffer() problem \r\n");
		bOverLimit = false;
		for (i = 0; i < nRealSize/2; i++)
		{
			n = ((int)lpsaSample[i] - DMIC_DC_OFFSET) * g_nVolumeScalePercentage / 100;
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
		if (bOverLimit && g_nVolumeScalePercentage > 100)
		{
			g_nVolumeScalePercentage--;
			printf("g_nVolumeScalePercentage = %d\n", g_nVolumeScalePercentage);
		}
		nRet = DSpotter_AddSample(hDSpotter, lpsaSample, nRealSize/2);
		RingBufferDequeueData(g_hRingBuffer, nRealSize);
		if (nRet == DSPOTTER_ERR_NeedMoreSample)
		{
			if (g_nMode == RECOGNITION_MODE_COMMAND)
			{
				// Check timeout for command recognition mode
				nCommandRecordSample += nRealSize/sizeof(short);
				if (nCommandRecordSample > 16000 / 1000 * TIMEOUT_TIME_IN_MS)	// Check timeout
				{
					printf("Timeout!\r\n");
					printf("\r\n");
					
					hDSpotter = DSpotterReInit(hDSpotter, RECOGNITION_MODE_TRIGGER);
					if (hDSpotter == NULL)
						break;
				}
			}
		}
		else if (nRet == DSPOTTER_SUCCESS)
		{
			int nCmdIndex = DSpotter_GetResult(hDSpotter);
			if (g_nMode == RECOGNITION_MODE_TRIGGER)
			{
				if (GetTriggerStringByIndex(nCmdIndex) != NULL)
				{
					printf("Get trigger : %s(%d)\r\n", GetTriggerStringByIndex(nCmdIndex), nCmdIndex);
					printf("\r\n");

                    // show string on lcd
                    lcd_showString(RECOGNITION_MODE_TRIGGER, nCmdIndex);

					nCommandRecordSample = 0;

					// Go to the command mode
					hDSpotter = DSpotterReInit(hDSpotter, RECOGNITION_MODE_COMMAND);
					if (hDSpotter == NULL)
						break;
				}
				else
				{
					printf("Reject trigger index %d\r\n", nCmdIndex);
				}
			}
			else if (g_nMode == RECOGNITION_MODE_COMMAND)
			{
				if (GetCommandStringByIndex(nCmdIndex) != NULL)
				{
					printf("Get command : %s(%d)\r\n", GetCommandStringByIndex(nCmdIndex), nCmdIndex);
					printf("\r\n");

                    // show string on lcd
                    lcd_showString(RECOGNITION_MODE_COMMAND, nCmdIndex);

					// Return to trigger mode
					//hDSpotter = DSpotterReInit(hDSpotter, RECOGNITION_MODE_TRIGGER);
                    hDSpotter = DSpotterReInit(hDSpotter, RECOGNITION_MODE_COMMAND);
					if (hDSpotter == NULL)
						break;
				}
			}
			DSpotter_Reset(hDSpotter);
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

PROGRAM_END:

	HAL_DMA_Abort(g_PDMSlvHandle.hDMA);
	RingBufferRelease(g_hRingBuffer);
	g_hRingBuffer = NULL;

	if (hDSpotter != NULL)
	{
		DSpotter_Release(hDSpotter);
		hDSpotter = NULL;
	}
	
	printf("test_pdm_dma Stop\n");

	return HAL_PDM_OK;
}

HANDLE DSpotterReInit(HANDLE hDSpotter, int nRecognitionMode)
{
	int nRet = DSPOTTER_ERR_IllegalParam;
	BYTE *lppbyModel[1];
	int i;

	if (hDSpotter != NULL)
	{
		DSpotter_Release(hDSpotter);
		hDSpotter = NULL;
	}

	g_nMode = nRecognitionMode;

	switch (nRecognitionMode)
	{
	case RECOGNITION_MODE_TRIGGER:
		lppbyModel[0] = (BYTE *)g_lpdwGroup_1;
		hDSpotter = DSpotter_Init_Multi((BYTE *)g_lpdwCYBase, lppbyModel, 1, MAX_COMMAND_TIME,
						g_byDSpotterMem, sizeof(g_byDSpotterMem), NULL, 0, &nRet);
		printf("Enter trigger mode\r\n");
		break;

	case RECOGNITION_MODE_COMMAND:
		lppbyModel[0] = (BYTE *)g_lpdwGroup_2;
		hDSpotter = DSpotter_Init_Multi((BYTE *)g_lpdwCYBase, lppbyModel, 1, MAX_COMMAND_TIME,
						g_byDSpotterMem, sizeof(g_byDSpotterMem), NULL, 0, &nRet);
		printf("Enter command mode, please speak command in %d seconds.\r\n", TIMEOUT_TIME_IN_MS/1000);
		//for (i = 0; i < GetCommandCount(); i++)
		//	printf("    %s\r\n", GetCommandStringByIndex(i) );
		break;
	}

	if (hDSpotter == NULL)
		printf("DSpotter_Init() fail, error = %d!\r\n", nRet);

	return hDSpotter;
}
