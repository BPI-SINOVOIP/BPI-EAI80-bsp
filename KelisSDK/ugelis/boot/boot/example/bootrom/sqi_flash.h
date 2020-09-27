#ifndef _SQI_FLASH_H_
#define _SQI_FLASH_H_

#define FLASH_BASE_ADDR 0x10000000

typedef unsigned char 	uint8_t;
typedef unsigned short	uint16_t;
typedef unsigned int	uint32_t;

#define MX25R32		1
#define W25Q64		1

#define MX25R32_W25Q64_PAGE_LEN 		0x100



#if defined(MX25R32) || defined(W25Q64)

/* nor flash common instructions*/
#define MX25R32_W25Q64_READ_CMD			0x03
#define MX25R32_W25Q64_FAST_READ_CMD	0x0B


#define MX25R32_W25Q64_RDSR_CMD			0x05
#define MX25R32_W25Q64_WREN_CMD			0x06
#define MX25R32_W25Q64_SE_CMD			0x20
#define MX25R32_W25Q64_BE32K_CMD		0x52
#define MX25R32_W25Q64_BE_CMD			0xD8
#define MX25R32_W25Q64_CE_CMD			0x60
#define MX25R32_W25Q64_PP_CMD			0x02

/* nor flash Status register flag bit */
#define MX25R32_W25Q64_BUSY				(0x1 << 0)


#endif


/*SPI nor flash controller register bit filed */
/**************************** SQI Configuration register bitfiled *********************************/
#define SQI_HOLDJ_EN_POS                                  	((uint32_t)29)
#define SQI_HOLDJ_EN                                        ((uint32_t)((uint32_t)1 << SQI_HOLDJ_EN_POS))
#define SQI_WJ_EN_POS                                       ((uint32_t)28) 
#define SQI_WJ_EN                                           ((uint32_t)((uint32_t)1<<SQI_WJ_EN_POS))
#define SQI_FAST_RW_CNT_EN_POS                             	((uint32_t)21) 
#define SQI_FAST_RW_CNT_EN                                 	((uint32_t)((uint32_t)1<<SQI_FAST_RW_CNT_EN_POS))
#define SQI_ADDR_BYTE_NUM_POS                               ((uint32_t)19) 
#define SQI_ADDR_BYTE_NUM                                   ((uint32_t)((uint32_t)3<<SQI_ADDR_BYTE_NUM_POS))
#define SQI_RW_IO_MODE_POS                                  ((uint32_t)16) 
#define SQI_RW_IO_MODE                                      ((uint32_t)((uint32_t)7<<SQI_RW_IO_MODE_POS))
#define SQI_FAST_WRITE_EN_POS                           	((uint32_t)15) 
#define SQI_FAST_WRITE_EN                               	((uint32_t)((uint32_t)1<<SQI_FAST_WRITE_EN_POS))
#define SQI_FAST_READ_EN_POS                            	((uint32_t)14) 
#define SQI_FAST_READ_EN                                	((uint32_t)((uint32_t)1<<SQI_FAST_READ_EN_POS))
#define SQI_DMY_NUM_POS                                     ((uint32_t)12) 
#define SQI_DMY_NUM                                         ((uint32_t)((uint32_t)3<<SQI_DMY_NUM_POS))
#define SQI_COMMAND_SELECTED_EN_POS                         ((uint32_t)11) 
#define SQI_COMMAND_SELECTED_EN                             ((uint32_t)((uint32_t)1<<SQI_COMMAND_SELECTED_EN_POS))
#define SQI_ADDR_SELECTED_EN_POS                            ((uint32_t)10) 
#define SQI_ADDR_SELECTED_EN                                ((uint32_t)((uint32_t)1<<SQI_ADDR_SELECTED_EN_POS))
#define SQI_DMY_SELECTED_EN_POS                             ((uint32_t)9) 
#define SQI_DMY_SELECTED_EN                                 ((uint32_t)((uint32_t)1<<SQI_DMY_SELECTED_EN_POS))
#define SQI_DATA_SELECTED_EN_POS                            ((uint32_t)8) 
#define SQI_DATA_SELECTED_EN                                ((uint32_t)((uint32_t)1<<SQI_DATA_SELECTED_EN_POS))
#define SQI_INS_SET_POS                                     ((uint32_t)0)
#define SQI_INS_SET                                         ((uint32_t)((uint32_t)0xFF<<SQI_INS_SET_POS))
/*******************************************************************************************************/

/* FLASH controller: SQI controller register structure */
typedef struct FLASH_MemMap
{
	volatile uint32_t VS;                        /* SQI Version Register: 0x0 */
    volatile uint32_t BR;                        /* SQI Baud Rate Register: 0x4 */
    volatile uint32_t CFG;                       /* SQI Configuration Register 1: 0x8 */
    volatile uint32_t DMY;                       /* SQI Dummy Register: 0xc */
    volatile uint32_t CNT;                       /* SQI Counter: 0x10 */
    volatile uint32_t ADDRCFG;                   /* SQI Address Configuration Register: 0x14 */
    volatile uint32_t TCR;                       /* SQI Timing Control Register: 0x18 */
    volatile uint32_t PREFCTRL;                  /* SQI Prefetch Control Register: 0x1C */
    volatile uint32_t CRCCTRL;                   /* SQI CRC Control Register: 0x20 */
    volatile uint32_t CRCIV;                     /* SQI CRC Initial Value Register: 0x24 */
    volatile uint32_t CRCRES;                    /* SQI CRC Result Register: 0x28 */
}*FLASH_MemMapPtr;

#define FLASH	((FLASH_MemMapPtr)0x40001000u)

/* @brief: 	Initialize nor flash
 * @retval:	None
 */

void SPI_Init(void);

/* @brief: 	De-Initialize nor flash
 * @retval:	None
 */

void SPI_UnInit(void);

/* @brief: 	erase block in flash memory
 * @param:	
 *			addr: the block address to be erased
 * @retval:	None
 */

void SPI_EraseBlock64K(uint32_t addr);

/* @brief: 	program page in flash memory
 * @param:	
 *			addr: page start address
 *			sz:	  page size
 *			buf:  the buffer data should be written
 * @retval:	None
 */

void SPI_Program(uint32_t addr, uint32_t sz, uint8_t * buf);

/* @brief: 	enable SPI nor flash work in 4 line mode
 * @param:	None
 * @retval:	None
 */

void SPI_Enable4LineMode(void);

#endif
