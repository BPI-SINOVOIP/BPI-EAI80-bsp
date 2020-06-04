
#include <gm_hal_dma.h>
#include "../../driver/nandflash/gm_ll_nf.h"


#ifndef PAGE_SIZE
    #define PAGE_SIZE 2048//except Spare size
#endif
#define SECT_SIZE_ECC_MODE      1024    //NF CTRL SPEC :1 sector is 1024B for 16/24/40/48/60 bit ECC modes 
#define SECT_SIZE_UNECC_MODE    512     //NF CTRL SPEC :1 sector is 512B for others mode

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

typedef struct fps FlashParameter;

FlashReturnMsg HAL_NFlash_Init(NFlash_Device_T *pNFlashDev);
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

