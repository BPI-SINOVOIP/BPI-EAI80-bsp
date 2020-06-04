#include <gm_soc.h>
#include <gm_irq.h>


#include <gm_hal_mmc.h>
#include "../core/hal_mmc_core.h"
#include "../host/gm_ll_mmc_host.h"

#ifdef MMC_DEBUG
    #define mmc_pr_debug       printf
#else
    #define mmc_pr_debug(...)  do{}while(0);
#endif

#ifdef MMC_RET
    #define mmc_pr_ret         printf
#else
    #define mmc_pr_ret(...)    do{}while(0);
#endif


extern RET_CODE HAL_MMC_HostInit(MMC_Handle_T *hMMC,  MMC_Device_T *pMMC, MMC_Config_T *pConfig);
static int HAL_MMCReadBlocks(struct mmc *mmc, uint8_t *buf, uint32_t start,uint32_t blkcnt);
static uint32_t HAL_MMC_WriteBlocks(struct mmc *mmc, uint32_t start,uint32_t blkcnt, const uint8_t *buf);
static uint32_t HAL_MMC_Eraseblocks(struct mmc *mmc, uint32_t start, uint32_t blkcnt);
/*all the host init must keey this name*/
RET_CODE HAL_MMC_HostInit(MMC_Handle_T *hMMC,  MMC_Device_T *pMMC, MMC_Config_T *pConfig);


RET_CODE HAL_MMC_Init(MMC_Handle_T *hMMC,  MMC_Device_T *pMMC, MMC_Config_T *pConfig)
{
    if ((!hMMC) || (!pMMC) || (!pConfig))
    {
        return -RET_INVPARAM;
    }

    return HAL_MMC_HostInit(hMMC, pMMC, pConfig);
}

static int HAL_MMCReadBlocks(struct mmc *mmc, uint8_t *buf, uint32_t start,uint32_t blkcnt)
{
    struct mmc_cmd cmd;
    struct mmc_data data;

    if (blkcnt > 1)
    {
        cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
    }
    else
    {
        cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;
    }

    if (mmc->high_capacity)
    {
        cmd.cmdarg = start;
    }
    else
    {
        cmd.cmdarg = start * mmc->read_bl_len;
    }

    cmd.resp_type = MMC_RSP_R1;
    cmd.cmd_type  = MMC_CMD_ADTC_RD;

    data.dest = buf;
    data.blocks = blkcnt;
    data.blocksize = mmc->read_bl_len;
    data.flags = MMC_DATA_READ;

    if (mmc_send_cmd(mmc, &cmd, &data)){
        return 0;
    }

    if (blkcnt > 1)
    {
        cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
        cmd.cmdarg = 0;
        cmd.resp_type = MMC_RSP_R1b;
        cmd.cmd_type  = MMC_CMD_AC;
        if (mmc_send_cmd(mmc, &cmd, NULL))
        {
            mmc_pr_ret("mmc fail to send stop cmd\n");
            return 0;
        }
    }

    return blkcnt;
}

uint32_t HAL_MMC_Read(MMC_Handle_T *hMMC, uint32_t start, uint32_t blkcnt, uint8_t *buf)
{
    uint32_t cur, blocks_todo = blkcnt;
    struct mmc *mmc = hMMC->mmc;


    if (blkcnt == 0)
    {
        return 0;
    }

    if (!mmc)
    {
        return 0;
    }

    if ((start + blkcnt) > mmc->block_dev.lba)
    {
        mmc_pr_ret("MMC: block number %ul exceeds max(%ul)\n",
                                start + blkcnt, mmc->block_dev.lba);
        return 0;
    }

    if (mmc_set_blocklen(mmc, mmc->read_bl_len))
    {
        return 0;
    }


    do {
        cur = (blocks_todo > mmc->cfg->b_max) ?
        mmc->cfg->b_max : blocks_todo;
        if(HAL_MMCReadBlocks(mmc, buf, start, cur) != cur)
        {
            return 0;
        }

        blocks_todo -= cur;
        start += cur;
        buf += cur * mmc->read_bl_len;
    } while (blocks_todo > 0);


    return blkcnt;
}

static uint32_t HAL_MMC_WriteBlocks(struct mmc *mmc, uint32_t start,uint32_t blkcnt, const uint8_t *buf)
{
    struct mmc_cmd cmd;
    struct mmc_data data;

    int timeout = 1000;

    if ((start + blkcnt) > mmc->block_dev.lba)
    {
        mmc_pr_ret("MMC: block number %ul exceeds max %ul\n",
                                start + blkcnt, mmc->block_dev.lba);
        goto out;
    }

    if (blkcnt == 0)
    {
        goto out;
    }
    else if (blkcnt == 1){
        cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;
    }
    else{
        cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;
    }

    if (mmc->high_capacity)
    {
        cmd.cmdarg = start;
    }
    else
    {
        cmd.cmdarg = start * mmc->write_bl_len;
    }

    cmd.resp_type = MMC_RSP_R1;
    cmd.cmd_type  = MMC_CMD_ADTC_WR;

    data.src = buf;
    data.blocks = blkcnt;
    data.blocksize = mmc->write_bl_len;
    data.flags = MMC_DATA_WRITE;

    if (mmc_send_cmd(mmc, &cmd, &data))
    {
        blkcnt = 0;
        mmc_pr_ret("mmc write failed\n");
        goto out;
    }

    if (blkcnt > 1) {
        cmd.cmdidx     = MMC_CMD_STOP_TRANSMISSION;
        cmd.cmdarg     = 0;
        cmd.resp_type  = MMC_RSP_R1b;
        cmd.cmd_type   = MMC_CMD_AC;

        if (mmc_send_cmd(mmc, &cmd, NULL))
        {
            blkcnt = 0;
            mmc_pr_ret("mmc fail to send stop cmd\n");
            goto out;
        }
    }

    /* Waiting for the ready status */
    if (mmc_send_status(mmc, timeout))
    {
        blkcnt = 0;
        goto out;
    }

out:

    return blkcnt;
}

uint32_t HAL_MMC_Write(MMC_Handle_T *hMMC, uint32_t start, uint32_t blkcnt, const uint8_t *buf)
{
    uint32_t cur, blocks_todo = blkcnt;
    struct mmc *mmc =(struct mmc *)(hMMC->mmc);

    if (!mmc){
        return 0;
    }

    if (mmc_set_blocklen(mmc, mmc->write_bl_len)){
        return 0;
    }

    do {
        cur = (blocks_todo > mmc->cfg->b_max) ?
        mmc->cfg->b_max : blocks_todo;
        if (HAL_MMC_WriteBlocks(mmc, start, cur, buf) != cur)
        {
            return 0;
        }

        blocks_todo -= cur;
        start += cur;
        buf += cur * mmc->write_bl_len;
    } while (blocks_todo > 0);

    return blkcnt;
}


static uint32_t HAL_MMC_Eraseblocks(struct mmc *mmc, uint32_t start, uint32_t blkcnt)
{
    struct mmc_cmd cmd;
    uint32_t end;
    int ret, start_cmd, end_cmd;

    if (mmc->high_capacity)
    {
        end = start + blkcnt - 1;
    }
    else
    {
        end = (start + blkcnt - 1) * mmc->write_bl_len;
        start *= mmc->write_bl_len;
    }

    if (IS_SD(mmc))
    {
        start_cmd = SD_CMD_ERASE_WR_BLK_START;
        end_cmd = SD_CMD_ERASE_WR_BLK_END;
    }
    else
    {
        start_cmd = MMC_CMD_ERASE_GROUP_START;
        end_cmd = MMC_CMD_ERASE_GROUP_END;
    }

    cmd.cmdidx = start_cmd;
    cmd.cmdarg = start;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmd_type  = MMC_CMD_AC;

    ret = mmc_send_cmd(mmc, &cmd, NULL);
    if (ret)
    {
        goto ret_out;
    }

    cmd.cmdidx = end_cmd;
    cmd.cmdarg = end;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmd_type  = MMC_CMD_AC;

    ret = mmc_send_cmd(mmc, &cmd, NULL);
    if (ret)
    {
         goto ret_out;
    }

    cmd.cmdidx = MMC_CMD_ERASE;
    cmd.cmdarg = SECURE_ERASE;
    cmd.resp_type = MMC_RSP_R1b;
    cmd.cmd_type  = MMC_CMD_AC;

    ret = mmc_send_cmd(mmc, &cmd, NULL);
    if (ret)
    {
        goto ret_out;
    }

    return 0;

ret_out:
    mmc_pr_ret("mmc erase failed\n");
    return ret;
}

uint32_t HAL_MMC_Erase(MMC_Handle_T *hMMC, uint32_t start, uint32_t blkcnt)
{
    RET_CODE ret = RET_OK;
    struct mmc *mmc = hMMC->mmc;
    uint32_t blk = 0, blk_r = 0;
    int timeout = 1000;

    if (!mmc)
    {
        return RET_INVPARAM;
    }

    while (blk < blkcnt)
    {
        blk_r = ((blkcnt - blk) > mmc->erase_grp_size) ?
        mmc->erase_grp_size : (blkcnt - blk);
        ret = HAL_MMC_Eraseblocks(mmc, start + blk, blk_r);
        if (ret)
        {
            break;
        }
        blk += blk_r;
        /* Waiting for the ready status */
        if (mmc_send_status(mmc, timeout)){
            return 0;
        }
    }

    return blk;
}


RET_CODE HAL_MMC_Indentifier(MMC_Handle_T *hMMC)
{
    RET_CODE ret = RET_OK;

    if( !((hMMC) && (hMMC->mmc)))
    {
        return RET_INVPARAM;
    }
    ret = mmc_init(hMMC->mmc,(mmc_typde_t)hMMC->type);
    if(ret)
    {
        mmc_pr_ret(" mmc Indentifie fail\n");
    }
    else
    {
        mmc_pr_debug("mmc Indentifie OK\n");
    }

    return ret;
}

RET_CODE HAL_MMC_IsActiveStatus(MMC_Handle_T *hMMC)
{
    RET_CODE ret = RET_OK;
    uint8_t  status = FALSE;
    int32_t  tryCnt = 1;

    if( !((hMMC) && (hMMC->mmc)))
    {
        return RET_INVPARAM;
    }

    ret = mmc_send_status(hMMC->mmc, tryCnt);
    if(ret == RET_OK)
    {
        status = TRUE;
    }
    return status;
}


/*return sectors*/
uint64_t HAL_MMC_GetCapcity(MMC_Handle_T *hMMC)
{
    struct mmc *mmc =(struct mmc *)(hMMC->mmc);

    mmc->block_dev.lba = mmc->capacity/mmc->read_bl_len;

    return mmc->block_dev.lba;
}

MMC_SLOT_STATUS_T HAL_MMC_GetCd(MMC_Handle_T *hMMC)
{
    struct mmc *mmc =(struct mmc *)(hMMC->mmc);
	if(mmc_getcd(mmc))
    {
        return MMC_IN_SLOT;
    }
	else
	{
	    return MMC_OFF_SLOT;
	}
}

void HAL_MMC_SetCd (MMC_Handle_T *hMMC, MMC_SLOT_STATUS_T status)
{
    struct mmc *mmc =(struct mmc *)(hMMC->mmc);

    mmc_setcd(mmc,(uint8_t)status);
}

