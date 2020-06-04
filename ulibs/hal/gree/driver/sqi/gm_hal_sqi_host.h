/**
 *
 * Copyright (C) 2018 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_sqi_host.h
 *
 * @author      steven.lee
 *
 * @version       1.0.0
 *
 * @date          2019/11/28
 *
 * @brief
 *
 * @note
 *    2019/11/28, steven.lee, V1.0.0
 *        Initial version.
 */
#ifndef _GM_HAL_SQI_HOST_H_
#define _GM_HAL_SQI_HOST_H_

#ifdef __cplusplus
extern "C" {
#endif
#include<gm_soc.h>
#include "gm_ll_sqi.h"




typedef enum
{
    NORR_RDSR   = 0x0,
    MX25R_RDSCUR,
    MX25R_RDCR,
    GD25R_RDSRM,
    GD25R_RDSRH,
    WB25R_RDSRM,
    WB25R_RDSRH,
    MT25R_RDNOVCFGRG

} MAX_READ_STATUS_REG_T;

typedef enum
{
    NORR_WRSR  =  0x0,
    MX25R_WRSCUR,
    GD25R_WRSRM,
    GD25R_WRSRH,
    WB25R_WRSRM,
    WB25R_WRSRH,
    MT25R_WRNOVCFGRG

} MAX_WRITE_STATUS_REG_T;





#define SQI_TIMEOUT        1000

#define SFLASH_SECTOR_LEN              0x1000     //4KB
#define SFLASH_BLOCK32_LEN             0x8000     //32KB
#define SFLASH_BLOCK_LEN               0x10000    //64KB

#define SFLASH_16M_SIZE                0x1000000  //16m




/*=================================public define==================================================*/
#define NOR_READ_CMD                    0x03        //normal read
#define NOR_READ4B_CMD                  0x13        //normal read by 4 byte address
#define NOR_WRITE_CMD                   0x02        //normal write
#define NOR_WRITE4B_CMD                 0x12        //normal write by 4 byte address
#define NOR_RDSR_CMD                    0x05        //read status register 
#define NOR_WRSR_CMD                    0x01        //write status register
#define NOR_WREN_CMD                    0x06        //write enable
#define NOR_SE_CMD                      0x20         //sector erase
#define NOR_SE4B_CMD                    0x21         //sector erase by 4 byte address

#define NOR_BE32K_CMD                   0x52         //block erase 32KB 
#define NOR_BE64K_CMD                   0xD8        //block erase 64KB 
#define NOR_CPE_CMD                     0x60        //block erase 64KB 
#define NOR_READ_ID_CMD                 0x9f        //normal read

#define NOR_EN4B_CMD                    0xB7        //entry 4byte address mode 
#define NOR_EX4B_CMD                    0xE9        //exit 4byte address mode 




#define CONFIG_SQI_SFLASH_MMAP_BASE     0x10000000


#define NOR_WRITE_LAUTCH_ENABLE     (0x1 << 1)
#define NOR_WIP_ENABLE              (0x1 << 0)

/*=================================MX define==================================================*/
#define MX25R_4PP_CMD                     0x38         //quad page program 
#define MX25R_4PP4B_CMD                   0x3E         //quad page program use 4 byte address
#define MX25R_QREAD_CMD                   0x6B         //1I / 4O read command
#define MX25R_QREAD4B_CMD                 0x6C         //1I / 4O read command use 4 byte address
#define MX25R_RDSCUR_CMD                  0x2B        //read security register
#define MX25R_RDCR_CMD                    0x15        //read configuration register
#define MX25R_WRSCUR_CMD                  0x2F        //write security register
#define MX25R_EN4B_CMD                    0xB7        //entry 4byte address mode 
#define MX25R_EX4B_CMD                    0xE9        //exit 4byte address mode 








#define MX25R_QUAD_ENABLE             (0x1 << 6)
#define MX25R_4ADDRESS_ENABLE         (0x1 << 5)



/*=================================MT define==================================================*/
#define MT25R_4PP_CMD                     0x38         //quad page program 
#define MT25R_4PP4B_CMD                   0x3E         //quad page program use 4 byte address
#define MT25R_QREAD_CMD                   0x6B         //1I / 4O read command
#define MT25R_QREAD4B_CMD                 0x6C         //1I / 4O read command use 4 byte address
#define MT25R_RDNOVCFGRG_CMD              0xb5         //read Nonvolatile Configuration Register
#define MT25R_WRNOVCFGRG_CMD              0xb1         //write Nonvolatile Configuration Register

#define MT25_4ADDRESS_ENABLE              (0x1 << 0)



/*=================================GD define==================================================*/
#define GD25R_RDSRM_CMD                   0x35        //read status register 8~15
#define GD25R_RDSRH_CMD                   0x15        //read status register 16~23
#define GD25R_WRSRM_CMD                   0x31        //write status register 8~15
#define GD25R_WRSRH_CMD                   0x11        //write status register 16~23
#define GD25R_4PP_CMD                     0x32         //quad page program 
#define GD25R_4PP4B_CMD                   0x34         //quad page program use 4 byte address
#define GD25R_QREAD_CMD                   0x6B         //1I / 4O read command
#define GD25R_QREAD4B_CMD                 0x6C         //1I / 4O read command use 4 byte address
#define GD25R_EN4B_CMD                    0xB7        //entry 4byte address mode 
#define GD25R_EX4B_CMD                    0xE9        //exit 4byte address mode 



#define GD25_QUAD_ENABLE             (0x1 << 1)
#define GD25_4ADDRESS_ENABLE         (0x1 << 0)


/*=================================WB define==================================================*/
#define WB25R_RDSRM_CMD                   0x35        //read status register 8~15
#define WB25R_RDSRH_CMD                   0x15        //read status register 16~23
#define WB25R_WRSRM_CMD                   0x31        //write status register 8~15
#define WB25R_WRSRH_CMD                   0x11        //write status register 16~23
#define WB25R_4PP_CMD                     0x32         //quad page program 
#define WB25R_4PP4B_CMD                   0x34         //quad page program use 4 byte address
#define WB25R_QREAD_CMD                   0x6B         //1I / 4O read command
#define WB25R_QREAD4B_CMD                 0x6C         //1I / 4O read command use 4 byte address
#define WB25R_EN4B_CMD                    0xB7        //entry 4byte address mode 
#define WB25R_EX4B_CMD                    0xE9        //exit 4byte address mode 



#define WB25_QUAD_ENABLE             (0x1 << 1)
#define WB25_4ADDRESS_ENABLE         (0x1 << 0)








/*=================================Host function================================================*/
void HAL_SQI_Wait_Idle(SQI_Device_T *pSQIDev);

void HAL_SQI_Time_Delayms(uint32_t ms);

void HAL_SQI_Write_Enable(SQI_Device_T *pSQIDev);

void HAL_SQI_NormalWrite_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size);

void HAL_SQI_NormalRead_Enable(SQI_Device_T *pSQIDev, uint32_t flash_size);

RET_CODE HAL_SQI_MX25_4LineWrite_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size);

RET_CODE HAL_SQI_MX25_4LineRead_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size);

RET_CODE HAL_SQI_MX25_Close_4Line(SQI_Device_T *pSQIDev);

RET_CODE HAL_SQI_MX25_4Address_Enable(SQI_Device_T *pSQIDev);

RET_CODE HAL_SQI_MX25_4Address_Enable(SQI_Device_T *pSQIDev);

RET_CODE HAL_SQI_GD25_4LineWrite_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size);

RET_CODE HAL_SQI_GD25_4LineRead_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size);

RET_CODE HAL_SQI_GD25_Close_4Line(SQI_Device_T *pSQIDev);

RET_CODE HAL_SQI_GD25_4Address_Enable(SQI_Device_T *pSQIDev);

RET_CODE HAL_SQI_MT25_4LineWrite_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size);

RET_CODE HAL_SQI_MT25_4LineRead_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size);

RET_CODE HAL_SQI_MT25_4Address_Enable(SQI_Device_T *pSQIDev);

RET_CODE HAL_SQI_W25_4LineWrite_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size);

RET_CODE HAL_SQI_W25_4LineRead_Enable(SQI_Device_T *pSQIDev, uint32_t count, uint32_t flash_size);

RET_CODE HAL_SQI_W25_4Address_Enable(SQI_Device_T *pSQIDev);

RET_CODE HAL_SQI_Div_Config(SQI_Device_T *pSQIDev, uint32_t div);

uint32_t HAL_SQI_ReadID(SQI_Device_T *pSQIDev);

RET_CODE HAL_SQI_Erase_Internal(SQI_Device_T *pSQIDev, uint32_t offset, uint32_t size, uint32_t flash_size);

void HAL_SQI_Clear_SR(SQI_Device_T *pSQIDev);

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_SQI_HOST_H_ */
