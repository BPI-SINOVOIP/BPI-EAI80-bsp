
#include <gm_hal_dma.h>
#include "../../driver/nandflash/gm_ll_nf.h"








#if 1


    #define HW32_REG(ADDRESS)   (*((volatile uint32_t *)(ADDRESS)))
    #define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
    #define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))



    //damon add define begin
    #define     SRAM_BASE_ADDR          0x20000000

    #define     NF_VER                  0x70000000
    #define     NF_MR                   0x70000010
    #define     NF_CR                   0x70000014
    #define     NF_ECC_CR               0x70000018
    #define     NF_DMA_CR               0x7000001C
    #define     NF_DR                   0x70000020
    #define     NF_DMA_DR               0x70000024
    #define     NF_RD_TIM               0x70000030
    #define     NF_WR_TIM               0x70000034
    #define     NF_EDO_RB_TIM           0x70000038
    #define     NF_DQS_TIM              0x7000003C
    #define     FW_WR_REDU_W0           0x70000040
    #define     FW_WR_REDU_W1           0x70000044
    #define     NF_IE                   0x7000004C
    #define     NF_SR                   0x70000050
    #define     NF_ECC_SR               0x70000054
    #define     NF_SEC_ERR_INFO         0x70000058
    #define     FW_RD_REDU_W0           0x70000060
    #define     FW_RD_REDU_W1           0x70000064
    #define     NF_LP_CR                0x70000070

    #define     NF_SRAM_STR             0x70002000


    #define odwNF_W_REDU0_REG       (HW32_REG(FW_WR_REDU_W0))
    #define odwNF_W_REDU1_REG       (HW32_REG(FW_WR_REDU_W1))
    #define obNF_RD_CYCLE_REG       (HW8_REG(NF_RD_TIM))
    #define obNF_WR_CYCLE_REG       (HW8_REG(NF_WR_TIM))
    #define obNF_EDO_CYCLE_REG      (HW32_REG(NF_EDO_RB_TIM))
    #define NF_IP_EN                (HW8_REG(NF_MR) = 0x01)
    #define owNF_MODE_REG           (HW32_REG(NF_MR))
    #define obNF_STD_RD_TIM         (HW8_REG(NF_RD_TIM))
    #define obNF_STD_WR_TIM         (HW8_REG(NF_WR_TIM))
    #define obNF_EDO_TIM            (HW8_REG(NF_EDO_RB_TIM))
    #define obNF_REJ_DLY            (HW8_REG(NF_EDO_RB_TIM+1))
    #define obNF_DMA_INIT           (HW8_REG(NF_DMA_CR))
    #define obNF_DMA_CTRL           (HW8_REG(NF_DMA_CR+1))
    #define obNF_DMA_CFG            (HW8_REG(NF_DMA_CR+2))
    #define obNF_PIO_DR             (HW8_REG(NF_DR))
    #define odwNF_DMA_DR            (HW32_REG(NF_DMA_DR))
    #define odwNF_SR                (HW32_REG(NF_SR))
    #define obNF_IE                 (HW8_REG(NF_IE))
    #define obNF_SR0                (HW8_REG(NF_SR))
    #define obNF_SR1                (HW8_REG(NF_SR+1))
    #define obNF_SR2                (HW8_REG(NF_SR+2))
    #define odwNF_CR                (HW32_REG(NF_CR))
    #define obNF_CEJ                (HW8_REG(NF_CR))
    #define obNF_ALE                (HW8_REG(NF_CR+1))
    #define obNF_CLE                (HW8_REG(NF_CR+2))
    #define obNF_WP                 (HW8_REG(NF_CR+3))
    #define obNF_ECC_RST            (HW8_REG(NF_ECC_CR))
    #define obNF_ECC_MODE           (HW8_REG(NF_ECC_CR+1))
    #define obNF_BCH_MODE           (HW8_REG(NF_ECC_CR+2))
    #define obNF_ECC_SR             (HW8_REG(NF_ECC_SR))
    #define odwNF_SEC_ERR_INFO      (HW32_REG(NF_SEC_ERR_INFO))



    #define CRYP_TEST_SEG0_ADDR   0x20008000
    #define CRYP_TEST_SEG1_ADDR   0x2000a000
    //damon add define end


#endif













#ifndef PAGE_SIZE
    //#define PAGE_SIZE 8192//except Spare size
    #define PAGE_SIZE 2048//except Spare size

#endif
#define SECT_SIZE_ECC_MODE      1024    //NF CTRL SPEC :1 sector is 1024B for 16/24/40/48/60 bit ECC modes 
#define SECT_SIZE_UNECC_MODE    512     //NF CTRL SPEC :1 sector is 512B for others mode

#define TIMEOUT_FOR_WAIT        100  //100ms




#define TIMEOUT                 -1    //timeout err


/* Return Message */
typedef enum
{
    Flash_Success,
    Flash_Error,
    Flash_Busy,
    Flash_OperationTimeOut,
    Flash_ProgramFailed,
    Flash_EraseFailed,
    Flash_ReadIDFailed,
    Flash_CmdInvalid,
    Flash_DataInvalid,
    Flash_AddrInvalid
} FlashReturnMsg;

#define BITS8_MASK  0x000000FF
#define BITS16_MASK 0x0000FF00
#define BITS24_MASK 0x00FF0000
#define BITS32_MASK 0xFF000000

#define BYTE0_OFFSET  0
#define BYTE1_MASK   0xFF
#define BYTE1_OFFSET  8
#define BYTE2_OFFSET 16
#define BYTE3_OFFSET 24
#define BYTE4_OFFSET 32
#define PAGE_MASK    0xFFFF
#define BYTE_MASK 0xFF


typedef  unsigned char  BOOL;

/* Flash Parameter Structure */
struct fps
{
    uint8_t *Array;
    uint32_t Address;
    uint32_t Length;
};

typedef enum
{
    NFlash_STATUS_READY               = 0,
    NFlash_STATUS_BUSY,
} NFlash_Status_T;

typedef enum
{
    ECC_ODD_MODE            = 0,
    ECC_BCH_MODE,
} NFlash_ECC_MODE_T;

typedef enum
{
    DO_ERR_CORRECT_DMA_ATSTOP            = 0x00,
    DO_ERR_CORRECT_DMA_ATSTOP_ALLSECT,
    DO_ERR_CORRECT,
    NOT_DO_ERR_CORRECT,
} NFlash_ECC_BYPASS_MODE_T;

typedef struct
{
    uint8_t                         trpj;                           /**<Read cycle low pulse**/
    uint8_t                         trp;                            /**<Time of read cycle high pulse**/
    uint8_t                         twpj;                           /**<Write cycle low pulse**/
    uint8_t                         twp;                            /**<Time of write cycle high pulse**/
    uint8_t                         trb;                            /**<Delay to start ECC engine decode,For EDO mode, this value should more than 0x06**/
} NF_Config_T;




typedef struct fps FlashParameter;

FlashReturnMsg HAL_NFlash_Init(NFlash_Device_T *pNFlashDev, NF_Config_T config);
FlashReturnMsg HAL_NFlash_Reset(NFlash_Device_T *pNFlashDev);
FlashReturnMsg HAL_NFlash_Read_ID(NFlash_Device_T *pNFlashDev, uint8_t *id_buf);
FlashReturnMsg HAL_NFlash_Page_Read(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length);
FlashReturnMsg HAL_NFlash_Page_Read_DMA_PIO(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length);
FlashReturnMsg HAL_NFlash_Page_Read_DMA_PIO_ODD(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length);
FlashReturnMsg HAL_NFlash_Page_Read_DMA_DMA(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length);
FlashReturnMsg HAL_NFlash_Cache_Seq_Rand_Read_Begin(NFlash_Device_T *pNFlashDev, uint32_t addr);
FlashReturnMsg HAL_NFlash_Cache_Seq_Read_Another_OP(NFlash_Device_T *pNFlashDev, uint8_t *data_buf, uint32_t length, BOOL last_page);
FlashReturnMsg HAL_NFlash_Page_Program(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length);
FlashReturnMsg HAL_NFlash_Page_Program_DMA_PIO(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length);
FlashReturnMsg HAL_NFlash_Page_Program_DMA_PIO_ODD(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length);
FlashReturnMsg HAL_NFlash_Page_Program_DMA_DMA(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length);
FlashReturnMsg HAL_NFlash_Cache_Program(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length, BOOL last_page);
FlashReturnMsg HAL_NFlash_Page_Random_Program_Begin(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length);
FlashReturnMsg HAL_NFlash_Page_Random_Program_Input(NFlash_Device_T *pNFlashDev, uint32_t addr, uint8_t *data_buf, uint32_t length);
FlashReturnMsg HAL_NFlash_Page_Random_Program_End(NFlash_Device_T *pNFlashDev);
FlashReturnMsg HAL_NFlash_Block_Erase(NFlash_Device_T *pNFlashDev, uint32_t addr);

