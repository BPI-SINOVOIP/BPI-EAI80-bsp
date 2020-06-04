/*
 * Copyright 2008, Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Based vaguely on the Linux code
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#include "hal_mmc_core.h"
#include<stdlib.h>

#ifdef MMC_CORE_DEBUG
#define mmc_core_pr_debug       printk
#else
#define mmc_core_pr_debug(...)  do{}while(0);
#endif

#ifdef MMC_CORE_ERR
#define mmc_core_pr_err         printk
#else
#define mmc_core_pr_err(...)    do{}while(0);
#endif

static int mmc_set_capacity(struct mmc *mmc, int part_num);

static struct mmc gmmcInstance[3];
static struct mmc_config  gcfgInstance[3];

static uint32_t  __be32_to_cpu(uint32_t val)
{
    uint32_t temp;

    temp  =  (val&0xff000000)>>24;
    temp  |= ((val&0x00ff0000)>>8);
    temp  |= ((val&0x0000ff00)<<8);
    temp  |= ((val&0x000000ff)<<24);

    return temp;
}


#define CACHE_ENBALE
    #ifdef CACHE_ENBALE
    #define DCACHE_ALIGN	    16
#else
    #define DCACHE_ALIGN	    1
#endif


int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    int ret;

    #ifdef CONFIG_MMC_TRACE
    int i;
    uint8_t *ptr;

    mmc_core_pr_debug("CMD_SEND:%d\n", cmd->cmdidx);
    mmc_core_pr_debug("\t\tARG\t\t\t 0x%08X\n", cmd->cmdarg);
    ret = mmc->cfg->ops->send_cmd(mmc, cmd, data);
    switch (cmd->resp_type) {
        case MMC_RSP_NONE:
            mmc_core_pr_debug("\t\tMMC_RSP_NONE\n");
            break;

        case MMC_RSP_R1:
            mmc_core_pr_debug("\t\tMMC_RSP_R1,5,6,7 \t 0x%08X \n",
            cmd->response[0]);
            break;

        case MMC_RSP_R1b:
            mmc_core_pr_debug("\t\tMMC_RSP_R1b\t\t 0x%08X \n",
            cmd->response[0]);
            break;

        case MMC_RSP_R2:
            mmc_core_pr_debug("\t\tMMC_RSP_R2\t\t 0x%08X \n",
                                                 cmd->response[0]);
            mmc_core_pr_debug("\t\t          \t\t 0x%08X \n",
                                                 cmd->response[1]);
            mmc_core_pr_debug("\t\t          \t\t 0x%08X \n",
                                                cmd->response[2]);
           mmc_core_pr_debug("\t\t          \t\t 0x%08X \n",
                                                cmd->response[3]);
            mmc_core_pr_debug("\n");
                                                mmc_core_pr_debug("\t\t\t\t\tDUMPING DATA\n");
            for (i = 0; i < 4; i++) {
                int j;
                mmc_core_pr_debug("\t\t\t\t\t%03d - ", i*4);
                ptr = (uint8_t *)&cmd->response[i];
                ptr += 3;
                for (j = 0; j < 4; j++){
                    mmc_core_pr_debug("%02X ", *ptr--);
                }

                mmc_core_pr_debug("\n");
            }
            break;

        case MMC_RSP_R3:
            mmc_core_pr_debug("\t\tMMC_RSP_R3,4\t\t 0x%08X \n",
                                                 cmd->response[0]);
        break;

        default:
            mmc_core_pr_debug("\t\tERROR MMC rsp not supported\n");
            break;

    }
    #else
        ret = mmc->cfg->ops->send_cmd(mmc, cmd, data);
    #endif

    return ret;
}

int mmc_send_status(struct mmc *mmc, int timeout)
{
    struct mmc_cmd cmd;
    int err, retries = 5;

    #ifdef CONFIG_MMC_TRACE
        int status;
    #endif

    cmd.cmdidx = MMC_CMD_SEND_STATUS;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmdarg = mmc->rca << 16;
    cmd.cmd_type  = MMC_CMD_AC;

    do {
        err = mmc_send_cmd(mmc, &cmd, NULL);
        if (!err){
            if ((cmd.response[0] & MMC_STATUS_RDY_FOR_DATA) &&
                (cmd.response[0] & MMC_STATUS_CURR_STATE) != MMC_STATE_PRG) {
                break;
            }else if (cmd.response[0] & MMC_STATUS_MASK) {
                mmc_core_pr_err("Status Error: 0x%08X\n",cmd.response[0]);
                return COMM_ERR;
            }
        } else if (--retries < 0){
            return err;
        }


        HAL_DelayMs(1);

    } while (timeout--);

    #ifdef CONFIG_MMC_TRACE
        status = (cmd.response[0] & MMC_STATUS_CURR_STATE) >> 9;
        mmc_core_pr_debug("CURR STATE:%d\n", status);
    #endif

    if (timeout <= 0) {
        mmc_core_pr_err("Timeout waiting card ready\n");
        return TIMEOUT;
    }

    return 0;
}

int mmc_set_blocklen(struct mmc *mmc, int len)
{
    struct mmc_cmd cmd;

    cmd.cmdidx      = MMC_CMD_SET_BLOCKLEN;
    cmd.resp_type   = MMC_RSP_R1;
    cmd.cmd_type    = MMC_CMD_AC;
    cmd.cmdarg      = len;

    return mmc_send_cmd(mmc, &cmd, NULL);
}

static int mmc_go_idle(struct mmc *mmc)
{
    struct mmc_cmd cmd;
    int err;

    HAL_DelayMs(1);

    cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
    cmd.cmdarg = 0;
    cmd.resp_type = MMC_RSP_NONE;
    cmd.cmd_type  = MMC_CMD_BC;

    err = mmc_send_cmd(mmc, &cmd, NULL);
    if (err)
        return err;

    HAL_DelayMs(2);

    return 0;
}

static int sd_send_op_cond(struct mmc *mmc)
{
    int timeout = 1000;
    int err;
    struct mmc_cmd cmd;

    do {
        cmd.cmdidx = MMC_CMD_APP_CMD;
        cmd.resp_type = MMC_RSP_R1;
        cmd.cmd_type  = MMC_CMD_AC;
        cmd.cmdarg = 0;

        err = mmc_send_cmd(mmc, &cmd, NULL);

        if (err)
            return err;

        cmd.cmdidx    = SD_CMD_APP_SEND_OP_COND;
        cmd.resp_type = MMC_RSP_R3;
        cmd.cmd_type  = MMC_CMD_BCR;


        /*
        * Most cards do not answer if some reserved bits
        * in the ocr are set. However, Some controller
        * can set bit 7 (reserved for low voltages), but
        * how to manage low voltages SD card is not yet
        * specified.
        */

        cmd.cmdarg = mmc->cfg->voltages & 0xff8000;

        if (mmc->version == SD_VERSION_2)
            cmd.cmdarg |= OCR_HCS;

        err = mmc_send_cmd(mmc, &cmd, NULL);

        if (err)
            return err;

        HAL_DelayMs(1);
    } while ((!(cmd.response[0] & OCR_BUSY)) && timeout--);


    if (timeout <= 0)
        return UNUSABLE_ERR;

    if (mmc->version != SD_VERSION_2)
        mmc->version = SD_VERSION_1_0;

    mmc->ocr = cmd.response[0];

    mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
    mmc->rca = 0;

    return 0;
}

/* We pass in the cmd since otherwise the init seems to fail */
static int mmc_send_op_cond_iter(struct mmc *mmc, struct mmc_cmd *cmd, int use_arg)
{
    int err;

    cmd->cmdidx = MMC_CMD_SEND_OP_COND;
    cmd->resp_type = MMC_RSP_R3;
    cmd->cmd_type  = MMC_CMD_BCR;
    cmd->cmdarg = 0;

    if (use_arg) {
        cmd->cmdarg = (mmc->cfg->voltages &
                                   (mmc->op_cond_response & OCR_VOLTAGE_MASK)) |
                                   (mmc->op_cond_response & OCR_ACCESS_MODE);

    if (mmc->cfg->host_caps & MMC_MODE_HC)
        cmd->cmdarg |= OCR_HCS;
    }

    err = mmc_send_cmd(mmc, cmd, NULL);
    if (err)
        return err;

    mmc->op_cond_response = cmd->response[0];

    return 0;
}

int mmc_send_op_cond(struct mmc *mmc)
{
    struct mmc_cmd cmd;
    int err, i;

    /* Some cards seem to need this */
    mmc_go_idle(mmc);

    /* Asking to the card its capabilities */
    mmc->op_cond_pending = 1;
    for (i = 0; i < 2; i++) {
        err = mmc_send_op_cond_iter(mmc, &cmd, i != 0);
    if (err)
        return err;

    /* exit if not busy (flag seems to be inverted) */
    if (mmc->op_cond_response & OCR_BUSY)
        return 0;
    }

    return IN_PROGRESS;
}

int mmc_complete_op_cond(struct mmc *mmc)
{
    struct mmc_cmd cmd;
    int timeout = 1000;

    int err;

    mmc->op_cond_pending = 0;

    do {
        err = mmc_send_op_cond_iter(mmc, &cmd, 1);
        if (err)
            return err;

        if(--timeout<0){
            mmc_core_pr_err("mmc_complete_op_cond timeout\n");
            return UNUSABLE_ERR;
        }

        HAL_DelayUs(100);
    } while (!(mmc->op_cond_response & OCR_BUSY));

    mmc->version = MMC_VERSION_UNKNOWN;
    mmc->ocr = cmd.response[0];

    mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
    mmc->rca = 1;

    return 0;
}


static int mmc_send_ext_csd(struct mmc *mmc, uint8_t *ext_csd)
{
    struct mmc_cmd cmd;
    struct mmc_data data;
    int err;

    /* Get the Card Status Register */
    cmd.cmdidx = MMC_CMD_SEND_EXT_CSD;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmd_type  = MMC_CMD_ADTC_RD;
    cmd.cmdarg = 0;

    data.dest = ext_csd;
    data.blocks = 1;
    data.blocksize = MMC_MAX_BLOCK_LEN;
    data.flags = MMC_DATA_READ;

    err = mmc_send_cmd(mmc, &cmd, &data);

    return err;
}


static int mmc_switch(struct mmc *mmc, uint8_t set, uint8_t index, uint8_t value)
{
    struct mmc_cmd cmd;
    int timeout = 1000;
    int ret;

    cmd.cmdidx = MMC_CMD_SWITCH;
    cmd.resp_type = MMC_RSP_R1b;
    cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |(index << 16) |(value << 8);
    cmd.cmd_type  = MMC_CMD_AC;

    ret = mmc_send_cmd(mmc, &cmd, NULL);

    /* Waiting for the ready status */
    if (!ret)
        ret = mmc_send_status(mmc, timeout);

    return ret;

}

static int mmc_change_freq(struct mmc *mmc)
{

	  static uint8_t __attribute__ ((aligned (DCACHE_ALIGN))) ext_csd[MMC_MAX_BLOCK_LEN];

    char cardtype;
    int err;

    mmc->card_caps = 0;

    /* Only version 4 supports high-speed */
    if (mmc->version < MMC_VERSION_4)
        return 0;

    err = mmc_send_ext_csd(mmc, ext_csd);

    if (err)
        return err;

    cardtype = ext_csd[EXT_CSD_CARD_TYPE] & 0xf;

    err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 1);

    if (err)
        return err;

    /* Now check to see that it worked */
    err = mmc_send_ext_csd(mmc, ext_csd);

    if (err)
        return err;

    /* No high-speed support */
    if (!ext_csd[EXT_CSD_HS_TIMING])
        return 0;

    /* High Speed is set, there are two types: 52MHz and 26MHz */
    if (cardtype & MMC_HS_52MHZ)
        mmc->card_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;
    else
        mmc->card_caps |= MMC_MODE_HS;

    return 0;
}

static int sd_switch(struct mmc *mmc, int mode, int group, uint8_t value, uint8_t *resp)
{
    struct mmc_cmd cmd;
    struct mmc_data data;

    /* Switch the frequency */
    cmd.cmdidx = SD_CMD_SWITCH_FUNC;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmd_type  = MMC_CMD_ADTC_RD;
    cmd.cmdarg = (mode << 31) | 0xffffff;
    cmd.cmdarg &= ~(0xf << (group * 4));
    cmd.cmdarg |= value << (group * 4);

    data.dest = resp;
    data.blocksize = 64;
    data.blocks = 1;
    data.flags = MMC_DATA_READ;

    return mmc_send_cmd(mmc, &cmd, &data);
}


static int sd_change_freq(struct mmc *mmc)
{
    int err = 0;
    struct mmc_cmd cmd;
    static uint32_t __attribute__ ((aligned (DCACHE_ALIGN))) scr[2];
    static uint32_t __attribute__ ((aligned (DCACHE_ALIGN))) switch_status[16];
    struct mmc_data data;
    int timeout;

    mmc->card_caps = 0;

    /* Read the SCR to find out if this card supports higher speeds */
    cmd.cmdidx = MMC_CMD_APP_CMD;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmd_type  = MMC_CMD_AC;
    cmd.cmdarg = mmc->rca << 16;

    err = mmc_send_cmd(mmc, &cmd, NULL);

    if (err)
        return err;

    cmd.cmdidx = SD_CMD_APP_SEND_SCR;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmd_type  = MMC_CMD_ADTC_RD;
    cmd.cmdarg = 0;

    timeout = 3;

retry_scr:
    data.dest = (uint8_t *)scr;
    data.blocksize = 8;
    data.blocks = 1;
    data.flags = MMC_DATA_READ;

    err = mmc_send_cmd(mmc, &cmd, &data);

    if (err) {
        if (timeout--)
            goto retry_scr;

        return err;
    }

    mmc->scr[0] = __be32_to_cpu(scr[0]);
    mmc->scr[1] = __be32_to_cpu(scr[1]);

    switch ((mmc->scr[0] >> 24) & 0xf)
    {

    case 0:
        mmc->version = SD_VERSION_1_0;
        break;

    case 1:
        mmc->version = SD_VERSION_1_10;
        break;

    case 2:
        mmc->version = SD_VERSION_2;
        if ((mmc->scr[0] >> 15) & 0x1)
            mmc->version = SD_VERSION_3;
        break;

   default:
        mmc->version = SD_VERSION_1_0;
        break;
    }

    if ((mmc->scr[0] & SD_DATA_4BIT) &&(mmc->cfg->host_caps & MMC_MODE_4BIT)){
        mmc->card_caps |= MMC_MODE_4BIT;
    }

    /* Version 1.0 doesn't support switching */
    if (mmc->version == SD_VERSION_1_0)
        return 0;

    timeout = 4;

    while (timeout--) {
        err = sd_switch(mmc, SD_SWITCH_CHECK, 0, 1,(uint8_t *)switch_status);
        if (err)
            return err;

        /* The high-speed function is busy.  Try again */
        if (!(__be32_to_cpu(switch_status[7]) & SD_HIGHSPEED_BUSY))
            break;
    }

    /* If high-speed isn't supported, we return */
    if (!(__be32_to_cpu(switch_status[3]) & SD_HIGHSPEED_SUPPORTED))
        return 0;

    /*
    * If the host doesn't support SD_HIGHSPEED, do not switch card to
    * HIGHSPEED mode even if the card support SD_HIGHSPPED.
    * This can avoid furthur problem when the card runs in different
    * mode between the host.
    */
    if (!((mmc->cfg->host_caps & MMC_MODE_HS_52MHz) &&
        (mmc->cfg->host_caps & MMC_MODE_HS)))
        return 0;

    err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (uint8_t *)switch_status);

    if (err)
        return err;

    if ((__be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000)
        mmc->card_caps |= MMC_MODE_HS;

    return 0;
}

/* frequency bases */
/* divided by 10 to be nice to platforms without floating point */
static const int fbase[] = {
10000,
100000,
1000000,
10000000,
};

/* Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
* to platforms without floating point.
*/
static const int multipliers[] = {
0,	/* reserved */
10,
12,
13,
15,
20,
25,
30,
35,
40,
45,
50,
55,
60,
70,
80,
};

static void mmc_set_ios(struct mmc *mmc)
{
    if (mmc->cfg->ops->set_ios)
        mmc->cfg->ops->set_ios(mmc);
}

void mmc_set_clock(struct mmc *mmc, uint32_t clock)
{
    if (clock > mmc->cfg->f_max)
        clock = mmc->cfg->f_max;

    if (clock < mmc->cfg->f_min)
        clock = mmc->cfg->f_min;

    mmc->clock = clock;

    mmc_set_ios(mmc);
}

static void mmc_set_bus_width(struct mmc *mmc, uint32_t width)
{
    mmc->bus_width = width;

    mmc_set_ios(mmc);
}

static int mmc_set_capacity(struct mmc *mmc, int part_num)
{
    switch (part_num)
    {
     case 0:
        mmc->capacity = mmc->capacity_user;
        break;

    case 1:
    case 2:
        mmc->capacity = mmc->capacity_boot;
        break;

    case 3:
        mmc->capacity = mmc->capacity_rpmb;
        break;

    case 4:
    case 5:
    case 6:
    case 7:
        mmc->capacity = mmc->capacity_gp[part_num - 4];
        break;

    default:
        return -1;
    }

    mmc->block_dev.lba = mmc->capacity/mmc->read_bl_len;
    mmc_core_pr_debug("block_dev.lba:%ul sector capacity:%ul byte read_bl_len:%d\n",
    mmc->block_dev.lba,mmc->capacity,mmc->read_bl_len);

    return 0;
}

static int mmc_startup(struct mmc *mmc)
{
    int err, i;
    uint32_t mult, freq;
    uint32_t cmult, csize, capacity;
    struct mmc_cmd cmd;


	static unsigned ext_csd_bits[] = {
        EXT_CSD_BUS_WIDTH_8,
        EXT_CSD_BUS_WIDTH_4,
        EXT_CSD_BUS_WIDTH_1,
    };

    /* An array to map CSD bus widths to host cap bits */
    static unsigned ext_to_hostcaps[] = {
    MMC_MODE_1BIT,
	MMC_MODE_4BIT,
    MMC_MODE_8BIT,
    };

    static uint8_t __attribute__ ((aligned (DCACHE_ALIGN))) ext_csd[MMC_MAX_BLOCK_LEN];
    static uint8_t __attribute__ ((aligned (DCACHE_ALIGN))) test_csd[MMC_MAX_BLOCK_LEN];
    int timeout = 1000;

    cmd.cmdidx = MMC_CMD_ALL_SEND_CID;
    cmd.resp_type = MMC_RSP_R2;
    cmd.cmd_type  = MMC_CMD_BCR;
    cmd.cmdarg = 0;

    err = mmc_send_cmd(mmc, &cmd, NULL);
    if (err)
        return err;

    memcpy(mmc->cid, cmd.response, 16);

    /*
    * For MMC cards, set the Relative Address.
    * For SD cards, get the Relatvie Address.
    * This also puts the cards into Standby State
    */
    cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
    cmd.cmdarg = mmc->rca << 16;
    cmd.resp_type = MMC_RSP_R6;
    cmd.cmd_type  = MMC_CMD_BCR;

    err = mmc_send_cmd(mmc, &cmd, NULL);
    if (err)
        return err;

    if (IS_SD(mmc))
        mmc->rca = (cmd.response[0] >> 16) & 0xffff;

    /* Get the Card-Specific Data */
    cmd.cmdidx = MMC_CMD_SEND_CSD;
    cmd.resp_type = MMC_RSP_R2;
    cmd.cmd_type = MMC_CMD_AC;
    cmd.cmdarg = mmc->rca << 16;

    err = mmc_send_cmd(mmc, &cmd, NULL);

    mmc->csd[0] = cmd.response[0];
    mmc->csd[1] = cmd.response[1];
    mmc->csd[2] = cmd.response[2];
    mmc->csd[3] = cmd.response[3];

    /* Waiting for the ready status */
    mmc_send_status(mmc, timeout);

    if (err)
        return err;

    if (mmc->version == MMC_VERSION_UNKNOWN) {
        int version = (cmd.response[0] >> 26) & 0xf;

        switch (version)
        {
        case 0:
            mmc->version = MMC_VERSION_1_2;
            break;

        case 1:
            mmc->version = MMC_VERSION_1_4;
            break;

        case 2:
            mmc->version = MMC_VERSION_2_2;
            break;

        case 3:
            mmc->version = MMC_VERSION_3;
            break;

        case 4:
            mmc->version = MMC_VERSION_4;
            break;

        default:
            mmc->version = MMC_VERSION_1_2;
            break;
        }
    }

    /* divide frequency by 10, since the mults are 10x bigger */
    freq = fbase[(cmd.response[0] & 0x7)];
    mult = multipliers[((cmd.response[0] >> 3) & 0xf)];

    mmc->tran_speed = freq * mult;
    mmc->dsr_imp = ((cmd.response[1] >> 12) & 0x1);
    mmc->read_bl_len = 1 << ((cmd.response[1] >> 16) & 0xf);

    if (IS_SD(mmc))
        mmc->write_bl_len = mmc->read_bl_len;
    else
        mmc->write_bl_len = 1 << ((cmd.response[3] >> 22) & 0xf);


    if (mmc->high_capacity) {
        csize = (mmc->csd[1] & 0x3f) << 16 | (mmc->csd[2] & 0xffff0000) >> 16;
        cmult = 8;
    } else {
        csize = (mmc->csd[1] & 0x3ff) << 2| (mmc->csd[2] & 0xc0000000) >> 30;
        cmult = (mmc->csd[2] & 0x00038000) >> 15;
    }

    mmc->capacity_user = (csize + 1) << (cmult + 2);
    mmc->capacity_user *= mmc->read_bl_len;
    mmc->capacity_boot = 0;
    mmc->capacity_rpmb = 0;
    for (i = 0; i < 4; i++)
        mmc->capacity_gp[i] = 0;

    if (mmc->read_bl_len > MMC_MAX_BLOCK_LEN)
        mmc->read_bl_len = MMC_MAX_BLOCK_LEN;

    if (mmc->write_bl_len > MMC_MAX_BLOCK_LEN)
        mmc->write_bl_len = MMC_MAX_BLOCK_LEN;

    if ((mmc->dsr_imp) && (0xffffffff != mmc->dsr))
   {
        cmd.cmdidx = MMC_CMD_SET_DSR;
        cmd.cmdarg = (mmc->dsr & 0xffff) << 16;
        cmd.resp_type = MMC_RSP_NONE;
        cmd.cmd_type  = MMC_CMD_BC;

         if (mmc_send_cmd(mmc, &cmd, NULL))
            mmc_core_pr_err("MMC: SET_DSR failed\n");
    }

    /* Select the card, and put it into Transfer Mode */

    cmd.cmdidx    = MMC_CMD_SELECT_CARD;
    cmd.resp_type = MMC_RSP_R1;
    cmd.cmd_type  = MMC_CMD_AC;
    cmd.cmdarg    = mmc->rca << 16;
    err = mmc_send_cmd(mmc, &cmd, NULL);

    if (err)
        return err;
    /*
    * For SD, its erase group is always one sector
    */
    mmc->erase_grp_size = 1;
    mmc->part_config = MMCPART_NOAVAILABLE;
    if (!IS_SD(mmc) && (mmc->version >= MMC_VERSION_4)) {
        /* check  ext_csd version and capacity */
        err = mmc_send_ext_csd(mmc, ext_csd);
        if (!err && (ext_csd[EXT_CSD_REV] >= 2)) {
            /*
            * According to the JEDEC Standard, the value of
            * ext_csd's capacity is valid if the value is more
            * than 2GB
            */
            capacity = ext_csd[EXT_CSD_SEC_CNT] << 0
                            | ext_csd[EXT_CSD_SEC_CNT + 1] << 8
                            | ext_csd[EXT_CSD_SEC_CNT + 2] << 16
                            | ext_csd[EXT_CSD_SEC_CNT + 3] << 24;

            capacity *= MMC_MAX_BLOCK_LEN;

            if ((capacity >> 20) > 2 * 1024)
                mmc->capacity_user = capacity;
        }

        switch (ext_csd[EXT_CSD_REV]) {
        case 1:
            mmc->version = MMC_VERSION_4_1;
            break;

        case 2:
            mmc->version = MMC_VERSION_4_2;
            break;

        case 3:
            mmc->version = MMC_VERSION_4_3;
            break;

        case 5:
            mmc->version = MMC_VERSION_4_41;
            break;

        case 6:
            mmc->version = MMC_VERSION_4_5;
            break;
        }

    /*
    * Host needs to enable ERASE_GRP_DEF bit if device is
    * partitioned. This bit will be lost every time after a reset
    * or power off. This will affect erase size.
    */
    if ((ext_csd[EXT_CSD_PARTITIONING_SUPPORT] & PART_SUPPORT) &&
        (ext_csd[EXT_CSD_PARTITIONS_ATTRIBUTE] & PART_ENH_ATTRIB)) {
            err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
                                          EXT_CSD_ERASE_GROUP_DEF, 1);

        if (err)
            return err;

        /* Read out group size from ext_csd */
        mmc->erase_grp_size =ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] *
                                                        MMC_MAX_BLOCK_LEN * 1024;
    } else {
        /* Calculate the group size from the csd value. */
        int erase_gsz, erase_gmul;
        erase_gsz = (mmc->csd[2] & 0x00007c00) >> 10;
        erase_gmul = (mmc->csd[2] & 0x000003e0) >> 5;
        mmc->erase_grp_size = (erase_gsz + 1) * (erase_gmul + 1);
    }

    /* store the partition info of emmc */
    if ((ext_csd[EXT_CSD_PARTITIONING_SUPPORT] & PART_SUPPORT) ||
          ext_csd[EXT_CSD_BOOT_MULT])
        mmc->part_config = ext_csd[EXT_CSD_PART_CONF];

        mmc->capacity_boot = ext_csd[EXT_CSD_BOOT_MULT] << 17;
        mmc->capacity_rpmb = ext_csd[EXT_CSD_RPMB_MULT] << 17;

        for (i = 0; i < 4; i++) {
            int idx = EXT_CSD_GP_SIZE_MULT + i * 3;
            mmc->capacity_gp[i] = (ext_csd[idx + 2] << 16) +
                                                (ext_csd[idx + 1] << 8) + ext_csd[idx];
            mmc->capacity_gp[i] *=ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE];
            mmc->capacity_gp[i] *= ext_csd[EXT_CSD_HC_WP_GRP_SIZE];
        }
    }

    err = mmc_set_capacity(mmc, mmc->part_num);
    if (err)
        return err;

    if (IS_SD(mmc))
        err = sd_change_freq(mmc);
    else
        err = mmc_change_freq(mmc);

    if (err)
        return err;

    /* Restrict card's capabilities by what the host can do */
    mmc->card_caps &= mmc->cfg->host_caps;

    if (IS_SD(mmc)) {
        if (mmc->card_caps & MMC_MODE_4BIT) {
            cmd.cmdidx = MMC_CMD_APP_CMD;
            cmd.resp_type = MMC_RSP_R1;
            cmd.cmd_type  = MMC_CMD_AC;
            cmd.cmdarg = mmc->rca << 16;

            err = mmc_send_cmd(mmc, &cmd, NULL);
            if (err)
                return err;

            cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
            cmd.resp_type = MMC_RSP_R1;
            cmd.cmd_type  = MMC_CMD_AC;
            cmd.cmdarg = 2;
            err = mmc_send_cmd(mmc, &cmd, NULL);
            if (err)
                return err;

            mmc_set_bus_width(mmc, 4);
    }

    if (mmc->card_caps & MMC_MODE_HS)
        mmc->tran_speed = 50000000;
    else
        mmc->tran_speed = 25000000;
    } else {
        int idx;

        /* An array of possible bus widths in order of preference */


        /* An array to map chosen bus width to an integer */
        static unsigned widths[] = {
            8, 4, 1,
        };

        for (idx=0; idx < 3; idx++) {
            unsigned int extw = ext_csd_bits[idx];

            /*
            * Check to make sure the controller supports
            * this bus width, if it's more than 1
            */
            if (extw != EXT_CSD_BUS_WIDTH_1 &&
                !(mmc->cfg->host_caps & ext_to_hostcaps[extw]))
                continue;

            err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL,
                                            EXT_CSD_BUS_WIDTH, extw);

            if (err)
                continue;

            mmc_set_bus_width(mmc, widths[idx]);

            err = mmc_send_ext_csd(mmc, test_csd);
            if (!err && ext_csd[EXT_CSD_PARTITIONING_SUPPORT] \
                == test_csd[EXT_CSD_PARTITIONING_SUPPORT]
                && ext_csd[EXT_CSD_ERASE_GROUP_DEF] \
                == test_csd[EXT_CSD_ERASE_GROUP_DEF] \
                && ext_csd[EXT_CSD_REV] \
                == test_csd[EXT_CSD_REV]
                && ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] \
                == test_csd[EXT_CSD_HC_ERASE_GRP_SIZE]
                && memcmp(&ext_csd[EXT_CSD_SEC_CNT], \
                &test_csd[EXT_CSD_SEC_CNT], 4) == 0) {

                mmc->card_caps |= ext_to_hostcaps[extw];
                break;
            }
        }

        if (mmc->card_caps & MMC_MODE_HS) {
            if (mmc->card_caps & MMC_MODE_HS_52MHz)
                mmc->tran_speed = 52000000;
            else
                mmc->tran_speed = 26000000;
        }
    }

    mmc_set_clock(mmc, mmc->tran_speed);

    mmc->block_dev.blksz = mmc->read_bl_len;
    mmc->block_dev.lba = mmc->capacity/mmc->read_bl_len;

    return 0;
}


static int mmc_send_if_cond(struct mmc *mmc)
{
    struct mmc_cmd cmd;
    int err;

    cmd.cmdidx = SD_CMD_SEND_IF_COND;
    /* We set the bit if the host supports voltages between 2.7 and 3.6 V */
    cmd.cmdarg = ((mmc->cfg->voltages & 0xff8000) != 0) << 8 | 0xaa;
    cmd.resp_type = MMC_RSP_R7;
    cmd.cmd_type  = MMC_CMD_BCR;

    err = mmc_send_cmd(mmc, &cmd, NULL);

    if (err)
        return err;

    if ((cmd.response[0] & 0xff) != 0xaa)
        return UNUSABLE_ERR;
    else
        mmc->version = SD_VERSION_2;

    return 0;
}

struct mmc *mmc_create(struct mmc_config *cfg, void *priv, mmc_typde_t mmc_typde)
{
    struct mmc *mmc = NULL;
    struct mmc_config *cfg_instance = NULL;;

    /* quick validation */
    if (cfg == NULL || cfg->ops == NULL || cfg->ops->send_cmd == NULL ||
        cfg->f_min == 0 || cfg->f_max == 0 || cfg->b_max == 0){
        return NULL;
    }

    if(mmc_typde > mmc_emmc){
        return NULL;
    }

    /*alloc for struct mmc instance*/
    mmc = &(gmmcInstance[mmc_typde]);
    memset(mmc, 0, sizeof(*mmc));

    /*alloc for struct mmc_config instance*/
    cfg_instance = &(gcfgInstance[mmc_typde]);
    memcpy(cfg_instance, cfg, sizeof(*cfg_instance));

    mmc->cfg = cfg_instance;
    mmc->priv = priv;
    /* Setup dsr related values */
    mmc->dsr_imp = 0;
    mmc->dsr = 0xffffffff;

    return mmc;
}


int mmc_start_init(struct mmc *mmc)
{
    int err = 0;

    if(mmc->cfg->ops->setup){
        err = mmc->cfg->ops->setup(mmc);
        if(err){
            return err;
        }
    }

    mmc_set_bus_width(mmc, 1);
    mmc_set_clock(mmc, 1);

    err = mmc_go_idle(mmc);

    if (err)
        return err;

        /* The internal partition reset to user partition(0) at every CMD0*/
        mmc->part_num = 0;

        /* Test for SD version 2 */
        err = mmc_send_if_cond(mmc);

        /* Now try to get the SD card's operating condition */
        err = sd_send_op_cond(mmc);

    return err;
}

static int mmc_complete_init(struct mmc *mmc)
{
    int err = 0;

    if (mmc->op_cond_pending)
        err = mmc_complete_op_cond(mmc);

        if (!err)
            err = mmc_startup(mmc);

    return err;
}


static int mmc_sd_init(struct mmc *mmc)
{
    int err = IN_PROGRESS;

    mmc_setcd(mmc,TRUE);

    err = mmc_start_init(mmc);
    if(err){
        goto out;
    }
    err = mmc_complete_init(mmc);
    if(err){
        goto out;
    }

out:
    if((err)&&(mmc->cfg->ops->unsetup)){
        mmc->cfg->ops->unsetup(mmc);
        mmc_setcd(mmc,FALSE);
    }

    return err;
}


int mmc_init(struct mmc *mmc, mmc_typde_t mmc_typde)
{
    int err = 0;

    if(mmc_typde == mmc_sd){
        err = mmc_sd_init(mmc);
    }

    return err;
}


void mmc_setcd(struct mmc *mmc,uint8_t status)
{
    if(mmc->cfg->ops->setcd){
        mmc->cfg->ops->setcd(mmc,status) ;
    }
}

int mmc_getcd(struct mmc *mmc)
{
    if(mmc->cfg->ops->getcd){
        return mmc->cfg->ops->getcd(mmc);
    }

    return FALSE;
}

