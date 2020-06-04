/*
 * Copyright 2008,2010 Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Based (loosely) on the Linux code
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _HAL_MMC_CORE_H_
#define _HAL_MMC_CORE_H_


#include<gm_common.h>
#ifdef RABBIT_NO_OS_SYSTEM
    #pragma anon_unions
#endif

#define SD_VERSION_SD	0x20000
#define SD_VERSION_3	(SD_VERSION_SD | 0x300)
#define SD_VERSION_2	(SD_VERSION_SD | 0x200)
#define SD_VERSION_1_0	(SD_VERSION_SD | 0x100)
#define SD_VERSION_1_10	(SD_VERSION_SD | 0x10a)
#define MMC_VERSION_MMC		0x10000
#define MMC_VERSION_UNKNOWN	(MMC_VERSION_MMC)
#define MMC_VERSION_1_2		(MMC_VERSION_MMC | 0x102)
#define MMC_VERSION_1_4		(MMC_VERSION_MMC | 0x104)
#define MMC_VERSION_2_2		(MMC_VERSION_MMC | 0x202)
#define MMC_VERSION_3		(MMC_VERSION_MMC | 0x300)
#define MMC_VERSION_4		(MMC_VERSION_MMC | 0x400)
#define MMC_VERSION_4_1		(MMC_VERSION_MMC | 0x401)
#define MMC_VERSION_4_2		(MMC_VERSION_MMC | 0x402)
#define MMC_VERSION_4_3		(MMC_VERSION_MMC | 0x403)
#define MMC_VERSION_4_41	(MMC_VERSION_MMC | 0x429)
#define MMC_VERSION_4_5		(MMC_VERSION_MMC | 0x405)

#define MMC_MODE_HS		    0x001
#define MMC_MODE_HS_52MHz	0x010

#define MMC_MODE_1BIT		0x100
#define MMC_MODE_4BIT		0x200
#define MMC_MODE_8BIT		0x400


#define MMC_MODE_HC		    0x800

#define MMC_MODE_MASK_WIDTH_BITS (MMC_MODE_1BIT|MMC_MODE_4BIT | MMC_MODE_8BIT|)
#define MMC_MODE_WIDTH_BITS_SHIFT 8

#define SD_DATA_4BIT	0x00040000

#define IS_SD(x) (x->version & SD_VERSION_SD)

#define MMC_DATA_READ		1
#define MMC_DATA_WRITE		2

#define NO_CARD_ERR		    -16 /* No SD/MMC card inserted */
#define UNUSABLE_ERR		-17 /* Unusable Card */
#define COMM_ERR		    -18 /* Communications Error */
#define TIMEOUT			    -19
#define IN_PROGRESS		    -20 /* operation is in progress */

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

#define MMC_CMD62_ARG1			    0xefac62ec
#define MMC_CMD62_ARG2			    0xcbaea7


#define SD_CMD_SEND_RELATIVE_ADDR	3
#define SD_CMD_SWITCH_FUNC		    6
#define SD_CMD_SEND_IF_COND		    8

#define SD_CMD_APP_SET_BUS_WIDTH	6
#define SD_CMD_ERASE_WR_BLK_START	32
#define SD_CMD_ERASE_WR_BLK_END		33
#define SD_CMD_APP_SEND_OP_COND		41
#define SD_CMD_APP_SEND_SCR		    51

/* SCR definitions in different words */
#define SD_HIGHSPEED_BUSY	        0x00020000
#define SD_HIGHSPEED_SUPPORTED	    0x00020000

#define MMC_HS_TIMING		        0x00000100
#define MMC_HS_52MHZ		        0x2

#define OCR_BUSY		            0x80000000
#define OCR_HCS			            0x40000000
#define OCR_VOLTAGE_MASK	        0x007FFF80
#define OCR_ACCESS_MODE		        0x60000000

#define SECURE_ERASE		        0x80000000

#define MMC_STATUS_MASK		        (~0x0206BF7F)
#define MMC_STATUS_RDY_FOR_DATA     (1 << 8)
#define MMC_STATUS_CURR_STATE	    (0xf << 9)
#define MMC_STATUS_ERROR	        (1 << 19)

#define MMC_STATE_PRG		        (7 << 9)

#define MMC_VDD_165_195		        0x00000080	/* VDD voltage 1.65 - 1.95 */
#define MMC_VDD_20_21		        0x00000100	/* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22		        0x00000200	/* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23		        0x00000400	/* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24		        0x00000800	/* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25		        0x00001000	/* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26		        0x00002000	/* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27		        0x00004000	/* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28		        0x00008000	/* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29		        0x00010000	/* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30		0x00020000	/* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31		0x00040000	/* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32		0x00080000	/* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33		0x00100000	/* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34		0x00200000	/* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35		0x00400000	/* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36		0x00800000	/* VDD voltage 3.5 ~ 3.6 */

#define MMC_SWITCH_MODE_CMD_SET		0x00 /* Change the command set */
#define MMC_SWITCH_MODE_SET_BITS	0x01 /* Set bits in EXT_CSD byte
						addressed by index which are
						1 in value field */
#define MMC_SWITCH_MODE_CLEAR_BITS	0x02 /* Clear bits in EXT_CSD byte
						addressed by index, which are
						1 in value field */
#define MMC_SWITCH_MODE_WRITE_BYTE	0x03 /* Set target byte to value */

#define SD_SWITCH_CHECK		0
#define SD_SWITCH_SWITCH	1

/*
 * EXT_CSD fields
 */
#define EXT_CSD_GP_SIZE_MULT		143	/* R/W */
#define EXT_CSD_PARTITIONS_ATTRIBUTE	156	/* R/W */
#define EXT_CSD_PARTITIONING_SUPPORT	160	/* RO */
#define EXT_CSD_RST_N_FUNCTION		162	/* R/W */
#define EXT_CSD_RPMB_MULT		168	/* RO */
#define EXT_CSD_ERASE_GROUP_DEF		175	/* R/W */
#define EXT_CSD_BOOT_BUS_WIDTH		177
#define EXT_CSD_PART_CONF		179	/* R/W */
#define EXT_CSD_BUS_WIDTH		183	/* R/W */
#define EXT_CSD_HS_TIMING		185	/* R/W */
#define EXT_CSD_REV			192	/* RO */
#define EXT_CSD_CARD_TYPE		196	/* RO */
#define EXT_CSD_SEC_CNT			212	/* RO, 4 bytes */
#define EXT_CSD_HC_WP_GRP_SIZE		221	/* RO */
#define EXT_CSD_HC_ERASE_GRP_SIZE	224	/* RO */
#define EXT_CSD_BOOT_MULT		226	/* RO */

/*
 * EXT_CSD field definitions
 */

#define EXT_CSD_CMD_SET_NORMAL		(1 << 0)
#define EXT_CSD_CMD_SET_SECURE		(1 << 1)
#define EXT_CSD_CMD_SET_CPSECURE	(1 << 2)

#define EXT_CSD_CARD_TYPE_26	(1 << 0)	/* Card can run at 26MHz */
#define EXT_CSD_CARD_TYPE_52	(1 << 1)	/* Card can run at 52MHz */

#define EXT_CSD_BUS_WIDTH_1	0	/* Card is in 1 bit mode */
#define EXT_CSD_BUS_WIDTH_4	1	/* Card is in 4 bit mode */
#define EXT_CSD_BUS_WIDTH_8	2	/* Card is in 8 bit mode */

#define EXT_CSD_BOOT_ACK_ENABLE			(1 << 6)
#define EXT_CSD_BOOT_PARTITION_ENABLE		(1 << 3)
#define EXT_CSD_PARTITION_ACCESS_ENABLE		(1 << 0)
#define EXT_CSD_PARTITION_ACCESS_DISABLE	(0 << 0)

#define EXT_CSD_BOOT_ACK(x)		(x << 6)
#define EXT_CSD_BOOT_PART_NUM(x)	(x << 3)
#define EXT_CSD_PARTITION_ACCESS(x)	(x << 0)

#define EXT_CSD_BOOT_BUS_WIDTH_MODE(x)	(x << 3)
#define EXT_CSD_BOOT_BUS_WIDTH_RESET(x)	(x << 2)
#define EXT_CSD_BOOT_BUS_WIDTH_WIDTH(x)	(x)

#define R1_ILLEGAL_COMMAND		(1 << 22)
#define R1_APP_CMD			(1 << 5)



#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136	    (1 << 1)		/* 136 bit response */
#define MMC_RSP_CRC	    (1 << 2)		/* expect valid crc */
#define MMC_RSP_BUSY	(1 << 3)		/* card may send busy */
#define MMC_RSP_OPCODE	(1 << 4)		/* response contains opcode */

#define MMC_RSP_R1_CODE	    (1 << 6)
#define MMC_RSP_R1b_CODE	(1 << 7)
#define MMC_RSP_R2_CODE	    (1 << 8)
#define MMC_RSP_R3_CODE	    (1 << 9)
#define MMC_RSP_R4_CODE	    (1 << 10)
#define MMC_RSP_R5_CODE	    (1 << 11)
#define MMC_RSP_R6_CODE	    (1 << 12)
#define MMC_RSP_R7_CODE	    (1 << 13)


#define MMC_RSP_NONE	(0)

#define MMC_RSP_R1	    (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_R1_CODE)
#define MMC_RSP_R1b	    (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY|MMC_RSP_R1b_CODE)
#define MMC_RSP_R2	    (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC|MMC_RSP_R2_CODE)
#define MMC_RSP_R3	    (MMC_RSP_PRESENT|MMC_RSP_R3_CODE)
#define MMC_RSP_R4	    (MMC_RSP_PRESENT|MMC_RSP_R4_CODE)
#define MMC_RSP_R5	    (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_R5_CODE)
#define MMC_RSP_R6	    (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_R6_CODE)
#define MMC_RSP_R7	    (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_R7_CODE)



#define MMC_CMD_BC          0
#define MMC_CMD_BCR         1
#define MMC_CMD_AC          2
#define MMC_CMD_ADTC_RD     3
#define MMC_CMD_ADTC_WR     4


#define MMCPART_NOAVAILABLE	(0xff)
#define PART_ACCESS_MASK	(0x7)
#define PART_SUPPORT		(0x1)
#define PART_ENH_ATTRIB		(0x1f)

/* Maximum block size for MMC */
#define MMC_MAX_BLOCK_LEN	512

/* The number of MMC physical partitions.  These consist of:
 * boot partitions (2), general purpose partitions (4) in MMC v4.4.
 */
#define MMC_NUM_BOOT_PARTITION	2


typedef enum
{
    mmc_sd                = 0x0,
    mmc_sdio              = 0x1,
    mmc_emmc
} mmc_typde_t;

struct mmc_cid {
	uint32_t psn;
	unsigned short oid;
	unsigned char mid;
	unsigned char prv;
	unsigned char mdt;
	char pnm[7];
};

struct mmc_cmd {
    uint8_t        cmd_type;
	uint16_t       cmdidx;
	uint32_t       resp_type;
	uint32_t       cmdarg;
	uint32_t       response[4];
};

struct mmc_data {
	union {
		uint8_t *dest;
		const uint8_t *src; /* src buffers don't get written to */
	};
	uint32_t flags;
	uint32_t blocks;
	uint32_t blocksize;
};

/* forward decl. */
struct mmc;




struct mmc_ops {
    int (*send_cmd)(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data);
    void (*set_ios)(struct mmc *mmc);
    int (*setup)(struct mmc *mmc);
    int (*unsetup)(struct mmc *mmc);
    int (*getcd)(struct mmc *mmc);
    void (*setcd)(struct mmc *mmc, uint8_t status);
    int (*forceclk)(struct mmc *mmc, uint8_t en);
};

struct mmc_config {
	const struct mmc_ops *ops;
	uint32_t        host_caps;
	uint32_t        voltages;
	uint32_t        f_min;
	uint32_t        f_max;
	uint32_t        b_max;
};

 typedef struct block_dev_desc {
     uint8_t         devtype;       /* device type */
     uint8_t         removable;  /* removable device */
     uint32_t        lba;           /* number of blocks */
     uint32_t        blksz;      /* block size */
     void            *priv;      /* driver private struct pointer */
 }block_dev_desc_t;


/* TODO struct mmc should be in mmc_private but it's hard to fix right now */
struct mmc {
	const struct mmc_config *cfg;	/* provided configuration */
	uint32_t version;
	void *priv;
	int high_capacity;
	uint32_t bus_width;
	uint32_t clock;
	uint32_t card_caps;
	uint32_t ocr;
	uint32_t dsr;
	uint32_t dsr_imp;
	uint32_t scr[2];
	uint32_t csd[4];
	uint32_t cid[4];
	uint16_t rca;
	char part_config;
	char part_num;
	uint32_t tran_speed;
	uint32_t read_bl_len;
	uint32_t write_bl_len;
	uint32_t erase_grp_size;

	uint64_t capacity;
	uint64_t capacity_user;
	uint64_t capacity_boot;
	uint64_t capacity_rpmb;
	uint64_t capacity_gp[4];

	block_dev_desc_t block_dev;
	char op_cond_pending;	/* 1 if we are waiting on an op_cond command */
	char init_in_progress;	/* 1 if we have done mmc_start_init() */
	char preinit;		/* start init as early as possible */
	uint32_t op_cond_response;	/* the response byte from the last op_cond */
};


struct mmc *mmc_create(struct mmc_config *cfg, void *priv, mmc_typde_t mmc_typde);
int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data);
int mmc_send_status(struct mmc *mmc, int timeout);
int mmc_set_blocklen(struct mmc *mmc, int len);
int mmc_init(struct mmc *mmc, mmc_typde_t mmc_typde);
void mmc_set_clock(struct mmc *mmc, uint32_t clock);
int mmc_getcd(struct mmc *mmc);
void mmc_setcd(struct mmc *mmc,uint8_t status);

/**
 * Start device initialization and return immediately; it does not block on
 * polling OCR (operation condition register) status.  Then you should call
 * mmc_init, which would block on polling OCR status and complete the device
 * initializatin.
 *
 * @param mmc	Pointer to a MMC device struct
 * @return 0 on success, IN_PROGRESS on waiting for OCR status, <0 on error.
 */
int mmc_start_init(struct mmc *mmc);

/**
 * Set preinit flag of mmc device.
 *
 * This will cause the device to be pre-inited during mmc_initialize(),
 * which may save boot time if the device is not accessed until later.
 * Some eMMC devices take 200-300ms to init, but unfortunately they
 * must be sent a series of commands to even get them to start preparing
 * for operation.
 *
 * @param mmc		Pointer to a MMC device struct
 * @param preinit	preinit flag value
 */




#endif /* _HAL_MMC_CORE_H_ */
