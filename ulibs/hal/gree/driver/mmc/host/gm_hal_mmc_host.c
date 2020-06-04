#include <gm_soc.h>
#include <gm_irq.h>

#include <gm_hal_gpio.h>
#include <gm_hal_dma.h>
#include <gm_hal_mmc.h>
#include "gm_ll_mmc_dev.h"

#include "../core/hal_mmc_core.h"
#include "gm_ll_mmc_host.h"
#include <gm_hal_clock.h>

//#include<gm_list.h>


//#define MMC_HOST_DEBUG 1
//#define MMC_HOST_ERR   1

#ifdef MMC_HOST_DEBUG
    #define mmc_host_pr_debug       printk
#else
    #define mmc_host_pr_debug(...)  do{}while(0);
#endif

#ifdef MMC_HOST_ERR
    #define mmc_host_pr_err         printk
#else
    #define mmc_host_pr_err(...)    do{}while(0);
#endif

#ifdef USE_HAL_TEST
static void HAL_MMC_IRQHandler(uint32_t param);
 #else
 void HAL_MMC_IRQHandler(uint32_t param);
#endif

static void MMC_HostDMACallback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param);
static RET_CODE HAL_MMC_DmaChannelInit(MMC_Handle_T *hMMC);
static void HAL_MMC_HostSetClk(MMC_Handle_T *hMMC ,uint32_t clk);
static void HAL_MMC_HostSetIos(struct mmc *mmc);
static void HAL_MMC_HostCardPowerSupply(uint8_t on);
static RET_CODE HAL_MMC_HostSetup(struct mmc *mmc);
static RET_CODE HAL_MMC_HostUnSetup(struct mmc *mmc);
static RET_CODE HAL_MMC_HostConfigMMCResType(MMC_Device_T *pDev, uint32_t resType);
static MMC_TransferStatus_T HAL_MMC_HostWaitRes1Busy(MMC_Handle_T *hMMC,struct mmc_cmd *cmd);
static void HAL_MMC_HostSetIos(struct mmc *mmc);
static void HAL_MMC_HostIrqConfig( MMC_Handle_T *hMMC);
static RET_CODE HAL_MMC_HostSendCmd(struct mmc *mmc, struct mmc_cmd *cmd,
			struct mmc_data *data);
static RET_CODE HAL_MMC_HostConfigDataTransfer(MMC_Handle_T *hMMC, struct mmc_data *data);
static MMC_TransferStatus_T HAL_MMC_HostWaitCmdFinish(MMC_Handle_T *hMMC,struct mmc_cmd *cmd);
static MMC_TransferStatus_T HAL_MMC_HostWaitDataFinish(MMC_Handle_T *hMMC, struct mmc_cmd *cmd, uint32_t dir);
static RET_CODE HAL_MMC_HostSendCmd(struct mmc *mmc, struct mmc_cmd *cmd,
			struct mmc_data *data);
RET_CODE HAL_MMC_HostInit(MMC_Handle_T *hMMC,  MMC_Device_T *pMMC, MMC_Config_T *pConfig);

static RET_CODE HAL_MMC_HostDMAConfig(MMC_Handle_T *hMMC, uint8_t dmaDir, DMA_Handle_T *pDMAHandle);

//static void HAL_MMC_HostIrqConfig(MMC_Handle_T *hMMC);
static int HAL_MMC_HostClkAlwaysOn(struct mmc *mmc, uint8_t en);
static int32_t HAL_MMC_HostGetCd(struct mmc *mmc);
static void HAL_MMC_HostSetCd (struct mmc *mmc, uint8_t status);


static MMC_Handle_T *g_handlers[MAX_MMC_HOST_DEVICE];

static struct mmc_ops g_hostOps;



static void HAL_MMC_RegisetOps()
{
	g_hostOps.send_cmd = HAL_MMC_HostSendCmd;
	g_hostOps.set_ios = HAL_MMC_HostSetIos;
	g_hostOps.setup = HAL_MMC_HostSetup;
	g_hostOps.unsetup  = HAL_MMC_HostUnSetup;
	g_hostOps.getcd = HAL_MMC_HostGetCd;
	g_hostOps.setcd = HAL_MMC_HostSetCd;
	g_hostOps.forceclk = HAL_MMC_HostClkAlwaysOn;
}





static RET_CODE HAL_MMC_HostClkAlwaysOn(struct mmc *mmc, uint8_t en)
{
    RET_CODE ret = RET_OK;
    MMC_Handle_T *hMMC = (MMC_Handle_T *)(mmc->priv);

    if ((!mmc)||(!(mmc->priv)))
    {
        goto out;
    }

    if(en)
    {
        LL_MMC_CLK_ALWAYS_EN(hMMC->device);
    }
    else
    {
        LL_MMC_CLK_ALWAYS_DIS(hMMC->device);
    }

out:
    return ret;

}


static void HAL_MMC_HostIrqConfig(MMC_Handle_T *hMMC)
{

     if (hMMC->device == MMC0)    
     {
	     hMMC->IRQn  = MMC0_IRQn;
         HAL_RequestIrq(MMC0_IRQn, MMC0_IRQ_PRIORITY, HAL_MMC_IRQHandler, 0);                      
     }
     HAL_IRQ_ENABLE(hMMC->IRQn);
}

static RET_CODE HAL_MMCInstallHandlers(MMC_Handle_T *hMMC)
{
    RET_CODE ret = RET_OK;

    if (hMMC->device == MMC0)
    {
        g_handlers[0] = hMMC;
    }else{
        ret = RET_INVPARAM;
    }

    return ret;
}


void HAL_MMC0_IRQHandler()
{
	HAL_MMC_IRQHandler(0);
}




#ifdef USE_HAL_TEST
static void HAL_MMC_IRQHandler(uint32_t param)
#else
void HAL_MMC_IRQHandler(uint32_t param)
#endif
{
    int index = (int)param;
    MMC_Handle_T *hMMC = g_handlers[index];
    MMC_Device_T *pMMC = hMMC->device;

    if (index < 0 || index >= MAX_MMC_HOST_DEVICE)
    {
        mmc_host_pr_err("Err:%s: Invaild Paramer\n",__FUNCTION__);
        return;
    }

    if(!LL_MMC_IRQ_PEDING(pMMC))
    {
        mmc_host_pr_err("Err:%s: err irq coming\n",__FUNCTION__);
        return;
    }

    if(LL_MMC_SDIO_IRQ_PEDNING(pMMC) && LL_MMC_SDIO_IRQ_ENABLE(pMMC))
    {
        LL_MMC_CLEAR_SDIO_IRQ(pMMC);
    }
    else if(LL_MMC_CRC7_ERR_PEDING(pMMC) && LL_MMC_CRC7_ERR_IRQ_ENABLE(pMMC))
    {
        LL_MMC_CLEAR_CRC7_ERR_IRQ(pMMC);
        hMMC->transferStatus = MMC_STATUS_CRC7_ERR;
    }
    else if(LL_MMC_CRC16_ERR_PEDING(pMMC) && LL_MMC_CRC16_ERR_IRQ_ENABLE(pMMC))
    {
        LL_MMC_CLEAR_CRC16_ERR_IRQ(pMMC);
        hMMC->transferStatus = MMC_STATUS_CRC16_ERR;
    }
    else if(LL_MMC_CMD_END_PEDING(pMMC) && LL_MMC_CMD_END_IRQ_ENABLE(pMMC))
    {
        LL_MMC_CLEAR_CMD_END_IRQ(pMMC);
        hMMC->transferStatus = MMC_STATUS_CMD_FINISH;

    }
    else if(LL_MMC_DATA_END_PEDING(pMMC) && LL_MMC_DATA_END_IRQ_ENABLE(pMMC))
    {
        LL_MMC_CLEAR_DATA_END_IRQ(pMMC);
        if(hMMC->waitDmaFinish == FALSE){
            /*
             * if need wait dma finish,we should not change transferStatus,keep it idle
             * to wait dma done event
             */
            hMMC->transferStatus = MMC_STATUS_DATA_FINISH;
        }

    }
}


static void MMC_HostDMACallback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
	  MMC_Handle_T *hMMC = hDMA->pParent;
	
    if((!hDMA)||(!(hDMA->pParent)))
    {
        mmc_host_pr_err("Err:%s: hDMA NULL or hDMA->pParent\n",__FUNCTION__);
        return;
    }


    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        mmc_host_pr_debug("%s:DMA_EVENT_TRANSFER_DONE\n",__FUNCTION__);
        hMMC->transferStatus = MMC_STATUS_DAM_FINISH;
    }
    else if(event == DMA_EVENT_TRANSFER_ERROR)
    {
        mmc_host_pr_err("%s:DMA_EVENT_TRANSFER_ERROR\n",__FUNCTION__);
        hMMC->transferStatus = MMC_STATUS_DAM_ERR;
    }
}

static void HAL_MMC_HostSetClk(MMC_Handle_T *hMMC ,uint32_t clk)
{
    uint32_t div;
    unsigned MMC_CLK_SRC;

    /*select clk 48Mhz*/
    HAL_CLOCK_PeripheralClockEnable0(SYS_ENCLK0_SDMMC);
#ifdef CHIP_GM6721
    MMC_CLK_SRC = IIS_CLK_48MHZ;
    /*config clk*/
    div = ((MMC_CLK_SRC/(2*clk))+1);
#endif
#ifdef CHIP_F6721B
    div = 1;
    MMC_CLK_SRC = 0;//to clear wraning
#endif
    LL_MMC_SET_BUS_CLK(hMMC->device,div);
}

static void HAL_MMC_HostSetIos(struct mmc *mmc)
{
    static uint32_t clk = 0;
    static uint8_t busWidthTemp = 0;
    MMC_Handle_T *hMMC = (MMC_Handle_T *)(mmc->priv);
    MMC_Device_T *pMMC = hMMC->device;
    MMC_BusWidth_T busWidth;

    /*set bus set clk*/
    if(clk != mmc->clock)
   {
        clk = mmc->clock;
        HAL_MMC_HostSetClk(hMMC,clk);
    }
    /*set bus with*/
    if(busWidthTemp != mmc->bus_width){
        /*backup last seting val*/
        busWidthTemp = mmc->bus_width;
        if(busWidthTemp == 8){
            LL_MMC_SDIOCTRL_8BIT_EN(pMMC,TRUE);
        }else{
            if(busWidthTemp == 4){
                busWidth = MMC_HOST_BUS_4BIT;
            }else{
                busWidth = MMC_HOST_BUS_1BIT;
            }
            LL_MMC_SDIOCTRL_8BIT_EN(pMMC,FALSE);
            LL_MMC_SET_BUS_WIDTH(hMMC->device,busWidth);
        }
    }
}

static RET_CODE HAL_MMC_HostSetup(struct mmc *mmc)
{
    MMC_Handle_T *hMMC = (MMC_Handle_T *)(mmc->priv);
    MMC_Device_T *pMMC = hMMC->device;

    /*give sdcard power*/
    if(hMMC->type == MMC_SD){
        HAL_MMC_HostCardPowerSupply(TRUE);
    }
    /*clk enable */
    LL_MMC_CLK_ENABLE(pMMC);
    /*ip reset*/
    LL_MMC_RESET(pMMC,2000);
    /*enable module clk*/
    LL_MMC_BUS_CLK_START(pMMC);

    /*host irq config*/
    LL_MMC_IE_ENABLE_ALL(pMMC);

    /*set 1 bit mode*/
    LL_MMC_SDIOCTRL_8BIT_EN(pMMC,FALSE);
    LL_MMC_SET_BUS_WIDTH(pMMC,MMC_HOST_BUS_1BIT);
    /*clk awalys on disable*/
    LL_MMC_CLK_ALWAYS_DIS(pMMC);
    hMMC->forceClk = FALSE;

    return RET_OK;

}

static RET_CODE HAL_MMC_HostUnSetup(struct mmc *mmc)
{
    MMC_Handle_T *hMMC = (MMC_Handle_T *)(mmc->priv);
    MMC_Device_T *pMMC = hMMC->device;

    /*clk bus stop*/
    LL_MMC_BUS_CLK_STOP(pMMC);
    /*power offer sd card*/
    if(hMMC->type == MMC_SD)
   {
        /**/
        HAL_MMC_HostCardPowerSupply(FALSE);
    }

    /*clk disable*/
    LL_MMC_CLK_DISABLE(pMMC);


    return RET_OK;

}

static int32_t HAL_MMC_HostGetCd(struct mmc *mmc)
{
    MMC_Handle_T *hMMC = (MMC_Handle_T *)(mmc->priv);
    return hMMC->cardInSlot;
}


static void HAL_MMC_HostSetCd (struct mmc *mmc, uint8_t status)
{
    MMC_Handle_T *hMMC = (MMC_Handle_T *)(mmc->priv);
	if(status)
	{
	    hMMC->cardInSlot = MMC_IN_SLOT;
	}
	else
	{
	    hMMC->cardInSlot = MMC_OFF_SLOT;
	}
}



static RET_CODE HAL_MMC_HostConfigMMCResType(MMC_Device_T *pDev, uint32_t resType)
{
    RET_CODE ret = RET_OK;

    MMC_ResType_T type;

    switch (resType)
    {
    case MMC_RSP_NONE:
        type = MMC_RES_R0;
        break;

    case MMC_RSP_R1:
    case MMC_RSP_R1b:
        type = MMC_RES_R1_R1B;
        break;

    case MMC_RSP_R2:
        type = MMC_RES_R2;
        break;

    case MMC_RSP_R3 :
    case MMC_RSP_R4 :
        type = MMC_RES_R3_R4;
        break;

    case MMC_RSP_R5:
    case MMC_RSP_R6:
    case MMC_RSP_R7:
        type = MMC_RES_R5_R6_R7;
        break;

    default:
        mmc_host_pr_err("Err:%s: not match\n",__FUNCTION__);
        return RET_ERROR;
    }

    LL_MMC_SET_RESP_TYPE(pDev,type);

    return ret;
}


static MMC_TransferStatus_T HAL_MMC_HostWaitRes1Busy(MMC_Handle_T *hMMC,struct mmc_cmd *cmd)
{
    int32_t timeout = 10000; /*10s*/
    MMC_TransferStatus_T ret = MMC_STATUS_SUCCESS;
    /*for data not busy::: 0:busy 1:ready*/

    while((LL_MMC_GET_ST_DA0(hMMC->device) == FALSE)&&((--timeout)>0))
    {
        HAL_DelayMs(1);
    }
    if(timeout<=0)
    {
        return MMC_STATUS_TIMEOUT;
    }

    return ret;
}



static RET_CODE HAL_MMC_DmaChannelInit(MMC_Handle_T *hMMC)
{
    RET_CODE ret = RET_OK;

    ret = HAL_MMC_HostDMAConfig(hMMC,MMC_DATA_WRITE,&(hMMC->TxDMAHandle));
    if(ret)
    {
        mmc_host_pr_err("Err:%s: TxDMAHandle Fail\n",__FUNCTION__);
        goto out;
    }
    ret = HAL_MMC_HostDMAConfig(hMMC,MMC_DATA_READ,&(hMMC->RxDMAHandle));
    if(ret)
    {
        mmc_host_pr_err("Err:%s: TxDMAHandle Fail\n",__FUNCTION__);
        goto out;
    }
out:
    return ret;
}


static void HAL_MMC_HostDmaTriger(MMC_Device_T *pDev,uint32_t dir)
{

    if(dir == MMC_DATA_WRITE)
   {
         LL_MMC_SET_DATA_DIR_WR(pDev) ;
         LL_MMC_DMACTRL_SET_WR_DIR(pDev);
    }else{
         LL_MMC_SET_DATA_DIR_RD(pDev) ;
         LL_MMC_DMACTRL_SET_RD_DIR(pDev);
    }
        /*config dma start*/
    LL_MMC_DMACTRL_DMA_START(pDev);
}


static RET_CODE HAL_MMC_HostConfigDataTransfer(MMC_Handle_T *hMMC, struct mmc_data *data)
{
    MMC_Device_T *pDev= hMMC->device;
    DMA_Handle_T * pDMAHandle = NULL;
    uint32_t srcAddress ;
    uint32_t destAddress;
    uint32_t size = (data->blocksize * data->blocks);
    /*dam 32bit transfer*/
    uint32_t tranSize = (size % 4) ? ((size + 3) / 4) : (size / 4);
    if((size % 4)){
        mmc_host_pr_err("############waing waing %s:tranSize:%d###################\n",__FUNCTION__,size);
    }

    /*config blksize and blkcnt*/
    LL_MMC_SET_BLK_LENGTH(pDev,data->blocksize);
    LL_MMC_SET_BLK_NUM(pDev,data->blocks);

    /*only read need wait dma finfish flag*/
    if(data->flags == MMC_DATA_READ)
    {
        hMMC->waitDmaFinish  = TRUE;
    }
    else
    {
        hMMC->waitDmaFinish  = FALSE;
    }

    if(data->flags == MMC_DATA_WRITE)
    {
        pDMAHandle = &(hMMC->TxDMAHandle);
        srcAddress  = (uint32_t)(data->src);
        destAddress = MMC_HOST_DAM_ADDRESS;
    }
    else
    {
        pDMAHandle = &(hMMC->RxDMAHandle);
        srcAddress  = MMC_HOST_DAM_ADDRESS;
        destAddress = (uint32_t)(data->dest);
    }

    /*dma mod triger dma start*/
    HAL_DMA_Start(pDMAHandle,srcAddress, destAddress, tranSize);
    /*host triger dma start*/
    HAL_MMC_HostDmaTriger(pDev,data->flags);

    return RET_OK;

}

static RET_CODE HAL_MMC_HostDMAConfig(MMC_Handle_T *hMMC, uint8_t dmaDir, DMA_Handle_T *pDMAHandle)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_ERROR;
    MMC_Device_T *pMMC = hMMC->device;

    if (dmaDir == MMC_DATA_READ)
    {

        if (pMMC == MMC0)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_SDIO_RX, &reqResult);
        }

        if (ret != RET_OK)
        {
            mmc_host_pr_err("Err:%s: DMA_REQUEST_SDIO_RX Fail\n",__FUNCTION__);
            return ret;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));
        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;

        dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
        dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
        dmaConfig.width     = DMA_WIDTH_32BIT;
        dmaConfig.burst     = DMA_BURST_1;
        dmaConfig.priority  = DMA_PRIORITY_HIGH;
        dmaConfig.alter     = DMA_PRIMARY;

        dmaConfig.callback = MMC_HostDMACallback;

    }
    else
    {
        if (pMMC == MMC0)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_SDIO_TX, &reqResult);
        }

        if (ret != RET_OK)
        {
            mmc_host_pr_err("Err:DMA_REQUEST_SD_TX Fail\n");
            return ret;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));
        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
        dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
        dmaConfig.width     = DMA_WIDTH_32BIT;
        dmaConfig.burst     = DMA_BURST_1;
        dmaConfig.priority  = DMA_PRIORITY_HIGH;
        dmaConfig.alter     = DMA_PRIMARY;

        dmaConfig.callback = MMC_HostDMACallback;
    }

    HAL_DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);
    pDMAHandle->pParent = hMMC;

    return ret;
}


static  void HAL_DumpErr(MMC_Device_T *pDev,struct mmc_cmd *cmd)
{
    mmc_host_pr_err("!!!cmdIndex:%d arg:0x%08x\n",cmd->cmdidx,cmd->cmdarg);
    mmc_host_pr_err("!!!MMCCTRL:0x%08x\n",pDev->MMCCTRL);
    mmc_host_pr_err("!!!ARG:0x%08x\n",pDev->ARG);
    mmc_host_pr_err("!!!DBL:0x%08x\n",pDev->DBL);
    mmc_host_pr_err("!!!SDIOCTRL0x%08x\n",pDev->SDIOCTRL);
}

#define MMC_CMD_GO_IDLE_STATE		0
#define MMC_CMD_SEND_OP_COND		1
#define MMC_CMD_ALL_SEND_CID		2
#define MMC_CMD_SET_RELATIVE_ADDR	3
#define MMC_CMD_SET_DSR		    	4
#define MMC_CMD_SWITCH			    6
#define MMC_CMD_SELECT_CARD		    7
#define MMC_CMD_SEND_EXT_CSD		8
#define MMC_CMD_SEND_CSD	    	9
#define MMC_CMD_SEND_CID	    	10
#define MMC_CMD_STOP_TRANSMISSION	12
#define MMC_CMD_SEND_STATUS		    13
#define MMC_CMD_GO_INACTIVE_STATE   15
#define MMC_CMD_SET_BLOCKLEN		16
#define MMC_CMD_READ_SINGLE_BLOCK	17
#define MMC_CMD_READ_MULTIPLE_BLOCK	18
#define MMC_CMD_WRITE_SINGLE_BLOCK	24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK	25
#define MMC_CMD_ERASE_GROUP_START	35
#define MMC_CMD_ERASE_GROUP_END		36
#define MMC_CMD_ERASE			    38
#define MMC_CMD_APP_CMD			    55
#define MMC_CMD_SPI_READ_OCR		58
#define MMC_CMD_SPI_CRC_ON_OFF		59
#define MMC_CMD_RES_MAN			    62


static MMC_TransferStatus_T HAL_MMC_HostWaitCmdFinish(MMC_Handle_T *hMMC,struct mmc_cmd *cmd)
{
    #define TIMEOUT_CMD  80 /*500ms*/
    int32_t timeoutMs = TIMEOUT_CMD;
    uint32_t typde = cmd->resp_type;
    MMC_Device_T *pDev = hMMC->device;
    MMC_TransferStatus_T ret = MMC_STATUS_SUCCESS;
    uint8_t debuginfor = TRUE;


    uint16_t cmdidx = cmd->cmdidx;
    if((cmdidx == MMC_CMD_APP_CMD)       ||
        (cmdidx == SD_CMD_SEND_IF_COND)  ||
        (cmdidx == MMC_CMD_SEND_STATUS))
    {
        /*
         *those cmd will cost too much time
         * when card is not in slot,so
         * when set timeout time very small
         */
        timeoutMs =  10;
        debuginfor = FALSE;
    }

    /*write wait host irq or err*/
    while(!(((hMMC->transferStatus == MMC_STATUS_CMD_FINISH)      ||\
            (hMMC->transferStatus == MMC_STATUS_CRC16_ERR)         ||\
            (hMMC->transferStatus == MMC_STATUS_CRC7_ERR))))
    {

        if((hMMC->cardInSlot == MMC_OFF_SLOT)&&(hMMC->removable))
        {
           ret =  MMC_STATUS_CARD_ERR;
            mmc_host_pr_err("Err %s: Card is Remove\n",__FUNCTION__);
            goto out;
        }

        if(--timeoutMs <= 0)
        {
            ret = MMC_STATUS_TIMEOUT;
            if(debuginfor == TRUE){
                mmc_host_pr_err("Err %s: MMC_STATUS_TIMEOUT\n",__FUNCTION__);
                HAL_DumpErr(pDev,cmd);
            }
            goto out;
        }
        HAL_DelayMs(1);
    }

    if((typde & MMC_RSP_CRC)&&(LL_MMC_CRC7_ERR(pDev)))
    {
        ret = MMC_STATUS_CRC7_ERR;
        mmc_host_pr_err("Err %s: MMC_STATUS_CRC7_ERR\n",__FUNCTION__);
        HAL_DumpErr(pDev,cmd);
        goto out;
    }


    if((typde & MMC_RSP_CRC)&&(LL_MMC_CRC16_ERR(pDev)))
    {
         ret = MMC_STATUS_CRC16_ERR;
         mmc_host_pr_err("Err %s: MMC_STATUS_CRC16_ERR\n",__FUNCTION__);
         HAL_DumpErr(pDev,cmd);
         goto out;
     }

    if(typde&MMC_RSP_BUSY)
    {
        /*wait cmd busy finish*/
        ret = HAL_MMC_HostWaitRes1Busy(hMMC,cmd);
        if(ret){
            mmc_host_pr_err("Err %s: HAL_MMC_HostWaitRes1Busy Timeout\n",__FUNCTION__);
            HAL_DumpErr(pDev,cmd);
            goto out ;
        }
    }

    if(typde&MMC_RSP_OPCODE)
    {
        /*todo check*/
    }

    if(typde&MMC_RSP_PRESENT)
    {

        if(typde&MMC_RSP_136)
        {
            cmd->response[3] = LL_MMC_GET_CMD_RESP0(pDev);
            cmd->response[2] = LL_MMC_GET_CMD_RESP1(pDev);
            cmd->response[1] = LL_MMC_GET_CMD_RESP2(pDev);
            cmd->response[0] = LL_MMC_GET_CMD_RESP3(pDev);
        }
        else
        {
            cmd->response[0] = LL_MMC_GET_CMD_RESP0(pDev);
        }

    }
out:

    hMMC->transferStatus = ret;


    return ret;
}

static MMC_TransferStatus_T HAL_MMC_HostWaitDataFinish(MMC_Handle_T *hMMC, struct mmc_cmd *cmd, uint32_t dir)
{
    #define TIMEOUT_DATA  10000 /*10s*/

    MMC_TransferStatus_T ret = MMC_STATUS_SUCCESS;
    MMC_Device_T *pDev = hMMC->device;
    int32_t timeoutMs = TIMEOUT_DATA;

    if(dir == MMC_DATA_WRITE)
    {
        /*write wait host irq or err*/
        while(!(((hMMC->transferStatus == MMC_STATUS_DATA_FINISH)      ||\
                 (hMMC->transferStatus == MMC_STATUS_DAM_ERR)          ||\
                 (hMMC->transferStatus == MMC_STATUS_CRC7_ERR)         ||\
                 (hMMC->transferStatus == MMC_STATUS_CRC16_ERR))))
        {
            if((HAL_MMC_HostGetCd(hMMC->mmc) == FALSE)&&(hMMC->removable == TRUE))
            {
                ret =  MMC_STATUS_CARD_ERR;
                mmc_host_pr_err("Err %s: Card is Remove\n",__FUNCTION__);
                goto out;
            }

            if(--timeoutMs <= 0)
            {
                ret = MMC_STATUS_TIMEOUT;
                mmc_host_pr_err("Err %s: MMC_STATUS_TIMEOUT\n",__FUNCTION__);
                HAL_DumpErr(pDev,cmd);
                goto out;
            }
            HAL_DelayMs(3);
        }
    }
    else
    {
        /*read wait dma or err*/
        while(!(((hMMC->transferStatus == MMC_STATUS_DAM_FINISH)      ||\
                 (hMMC->transferStatus == MMC_STATUS_DAM_ERR)         ||\
                 (hMMC->transferStatus == MMC_STATUS_CRC7_ERR)        ||\
                 (hMMC->transferStatus == MMC_STATUS_CRC16_ERR))))
        {
            if(--timeoutMs <= 0)
            {
                ret = MMC_STATUS_TIMEOUT;
                mmc_host_pr_err("Err %s: MMC_STATUS_TIMEOUT\n",__FUNCTION__);
                HAL_DumpErr(pDev,cmd);
                goto out;
            }
            if((HAL_MMC_HostGetCd(hMMC->mmc) == FALSE)&&(hMMC->removable == TRUE))
            {
                ret =  MMC_STATUS_CARD_ERR;
                mmc_host_pr_err("Err %s: Card is Remove\n",__FUNCTION__);
                goto out;
            }
            HAL_DelayMs(3);
        }
    }
    /*check host err status*/
    if(LL_MMC_CRC7_ERR(pDev))
    {
        ret = MMC_STATUS_CRC7_ERR;
        mmc_host_pr_err("Err %s: MMC_STATUS_CRC7_ERR\n",__FUNCTION__);
        HAL_DumpErr(pDev,cmd);
        goto out;
    }
    else if(LL_MMC_CRC16_ERR(pDev))
    {
        ret = MMC_STATUS_CRC16_ERR;
        mmc_host_pr_err("Err %s: MMC_STATUS_CRC16_ERR\n",__FUNCTION__);
        HAL_DumpErr(pDev,cmd);
        goto out;
    }
    else if(hMMC->transferStatus == MMC_STATUS_DAM_ERR)
    {
        ret = MMC_STATUS_DAM_ERR;
        mmc_host_pr_err("Err %s: MMC_STATUS_DAM_ERR\n",__FUNCTION__);
        HAL_DumpErr(pDev,cmd);
        goto out;
    }
    else
    {
        ret = MMC_STATUS_SUCCESS;
    }


out:
    return ret;
}

static RET_CODE HAL_MMC_HostSetCmdType(MMC_Device_T *pDev,struct mmc_cmd *cmd)
{
    MMC_CmdType_T type;

    switch (cmd->cmd_type) {
    case MMC_CMD_BC:
        type = MMC_HOST_CMD_BC;
        break;

    case MMC_CMD_BCR:
        type = MMC_HOST_CMD_BCR;
        break;

    case MMC_CMD_AC:
        type = MMC_HOST_CMD_AC;
        break;

    case MMC_CMD_ADTC_RD :
        type = MMC_HOST_CMD_RD;
        break;

    case MMC_CMD_ADTC_WR:
        type = MMC_HOST_CMD_WR;
        break;

    default:
        mmc_host_pr_err("Err:%s: not match\n",__FUNCTION__);
        return RET_ERROR;
    }

    LL_MMC_SET_CMD_TYPE(pDev,type);

    return RET_OK;

}

static RET_CODE HAL_MMC_HostSendCmd(struct mmc *mmc, struct mmc_cmd *cmd,
			                                        struct mmc_data *data)
{
    MMC_TransferStatus_T status = MMC_STATUS_SUCCESS;
    RET_CODE ret = RET_OK;
    MMC_Handle_T *hMMC = (MMC_Handle_T *)(mmc->priv);
    MMC_Device_T *pDev = hMMC->device;

    mmc_host_pr_debug("\ncmdindex:%d cmdtype:%d restype:%d cmdarg:0x%x resp_type:0x%x\n",
                  		 cmd->cmdidx,cmd->cmd_type,cmd->resp_type,cmd->cmdarg,cmd->resp_type);



    hMMC->transferStatus = MMC_STATUS_IDEL;

    /*config cmd type*/
    ret = HAL_MMC_HostSetCmdType(pDev,cmd);
    if(ret){
        mmc_host_pr_err("Err:%s: HAL_MMC_HostSetCmdType\n",__FUNCTION__);
        return ret;
    }
    ret = HAL_MMC_HostConfigMMCResType(pDev,cmd->resp_type);
    if(ret){
        mmc_host_pr_err("Err:%s: HAL_MMC_HostConfigMMCResType\n",__FUNCTION__);
        goto out;
    }

    /*config cmd index*/
    LL_MMC_SET_CMDINDEX(pDev,cmd->cmdidx);
    /*config cmd arg*/
    LL_MMC_SET_ARG(pDev,cmd->cmdarg);

    if(data)
    {
        ret = HAL_MMC_HostConfigDataTransfer(hMMC, data);
        if(ret)
        {
            mmc_host_pr_err("Err:%s: HAL_MMC_HostConfigDataTransfer\n",__FUNCTION__);
            goto out;
        }
    }

    /*
     *  host specil config:
     *  1:when check data0 busy must send
     *    clk all the time until data0 not busy
     *  2: host specil desingn,cmd0 must aslo send clk
     */
    if((cmd->resp_type == MMC_RSP_R1b)||(cmd->cmdidx == MMC_CMD_GO_IDLE_STATE))
    {
        LL_MMC_CLK_ALWAYS_EN(pDev);
        hMMC->forceClk = TRUE;
    }

    /*trige to realy send cmd*/
    LL_MMC_CMD_START(pDev);

    if(data)
    {
        /*data cmd transter and wait and check data finish*/
        status = HAL_MMC_HostWaitDataFinish(hMMC,cmd,data->flags);
        if(status != MMC_STATUS_SUCCESS)
        {
            ret = RET_ERROR;
            mmc_host_pr_err("Err:%s: HAL_MMC_HostWaitDataFinish\n",__FUNCTION__);
            goto out;
        }
    }
    else
    {
        /*pure cmd transter and wait and check cmd finish*/
        status = HAL_MMC_HostWaitCmdFinish(hMMC,cmd);
        if(status != MMC_STATUS_SUCCESS)
        {
            ret = RET_ERROR;
            goto out;
        }
    }
out:
    /*
     *  host specil config:
     *  1: data0 not busy should not send close clk
     *  2: host specil desingn,cmd0 finish should close clk
     */
    if(hMMC->forceClk == TRUE)
    {

        LL_MMC_CLK_ALWAYS_DIS(pDev);
        hMMC->forceClk = FALSE;
    }

    return ret;
}

RET_CODE HAL_MMC_HostInit(MMC_Handle_T *hMMC,  MMC_Device_T *pMMC, MMC_Config_T *pConfig)
{
    RET_CODE ret = RET_OK;

    struct mmc * mmc = NULL;
    struct mmc_config cfg;

    /*hander fill config*/
    hMMC->device     = pMMC;
    hMMC->type       = pConfig->type;
    hMMC->removable  = pConfig->removable;

	HAL_MMC_RegisetOps();

    ret = HAL_MMCInstallHandlers(hMMC);
    if(ret){
        mmc_host_pr_err("Err: HAL_MMCInstallHandlers\n");
        goto out;
    }

    /*dam config*/
    ret = HAL_MMC_DmaChannelInit(hMMC);
    if(ret)
    {
        goto out;
    }
    /*irq config*/
    HAL_MMC_HostIrqConfig(hMMC);

    memset(&cfg,0,sizeof(struct mmc_config));

    /*fill subclass function*/
    cfg.ops = &g_hostOps;

    /*config subclass cap*/
    if(pConfig->busWitch == MMC_BUS_WIDTH_1)
    {
        cfg.host_caps = MMC_MODE_1BIT;
    }
    else if(pConfig->busWitch == MMC_BUS_WIDTH_4)
    {
        cfg.host_caps = MMC_MODE_4BIT;
    }
    else if(pConfig->busWitch == MMC_BUS_WIDTH_8)
    {
        cfg.host_caps = MMC_MODE_8BIT;
    }

    cfg.host_caps |= (MMC_MODE_HS  | MMC_MODE_HC);

    /*fill subclass voltages*/
    cfg.voltages = MMC_VDD_32_33 | MMC_VDD_33_34 ;
    cfg.b_max = MMC_HOST_MAX_BLK_COUNT;

    /*fill subclass fre*/
    cfg.f_min = MMC_HOST_FRE_MIN;//todo makesure
    if (cfg.host_caps & MMC_MODE_HS)
    {
        if (cfg.host_caps & MMC_MODE_HS_52MHz)
        {
            cfg.f_max = 52000000;
        }
        else
        {
            cfg.f_max = 24000000;
        }
    }
    else
    {
        cfg.f_max = 20000000;
    }

    /*fpga force to 8Mhz*/
  //  cfg.f_max = MMC_HOST_FRE_MAX;

    /*regist subclass mmc to absturct system*/
    mmc = mmc_create(&cfg,hMMC, (mmc_typde_t)hMMC->type);
    if(!mmc)
    {
        mmc_host_pr_err("Err: mmc_create NULL\n");
        return NULL;
    }

    hMMC->mmc = mmc;
    ret = mmc_init(mmc,(mmc_typde_t)hMMC->type);
out:
    return ret;
}


/*todo for sdk*/
static void HAL_MMC_HostCardPowerSupply(uint8_t on)
{
    if(on == TRUE){

    }else{

    }
}


