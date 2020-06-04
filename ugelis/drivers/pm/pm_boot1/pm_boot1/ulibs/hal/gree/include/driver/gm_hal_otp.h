/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_otp.h
 *
 * @author        william.wang
 *
 * @version       1.0.0
 *
 * @date          2018/04/20
 *
 * @brief         Header file of otp Controller module.
 *
 * @note
 *    2018/04/20, william.wang, V1.0.0
 *        Initial version.
 *
 */
#ifndef _GM_HAL_OTP_H_
#define _GM_HAL_OTP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <otp/gm_ll_otp_dev.h>
#include <otp/gm_ll_otp.h>
#include <otp/gm_ll_otp_memio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_OTP_SIZE  (4 * 1024)
typedef struct OTP_Handle OTP_Handle_T;

typedef enum
{
    OTP_EVENT_NONE              = 0,    /**< There is nothing happened. */
    OTP_EVENT_WRITE_FINISHED    = 1,    /**< Write Data Finished */
    OTP_EVENT_WRITE_BUSY        = 2,    /**< In Writing Process. */
    OTP_EVENT_WRITE_PROTECTED   = 4,    /**< This Operation is Write Protection */
    OTP_EVENT_INACTIVE          = 8,   /**< The OTP is in Active Status */
    OTP_EVENT_DEEPSTANBY        = 16,  /**< The OTP is in deep standby mode */
} OTP_Event_T;

/**
 * @brief Callback of uart interrupt handler to notify user an event along with parameters.
 *
 * @param hOTP    Pointer to OTP handle, it maybe NULL if no handle is defined.
 * @param event   OTP event, it can be one of enum @ref  UART_Event_T.
 * @param param   Parameter of OTP event
 *
 * @retval        None.
 */
typedef void (*OTP_Callback_T)(struct OTP_Handle *hOTP, uint8_t event, ulong_t param);

typedef enum
{
    OTP_BOOT_SRAM               = 0,    /**< There is nothing happened. */
    OTP_BOOT_DRAM               = 1,    /**< Write Data Finished */
    OTP_BOOT_OTP                = 2,    /**< In Writing Process. */
    OTP_BOOT_SFLASH             = 4,    /**< This Operation is Write Protection */
} OTP_BOOT_T;
/**
 * @brief OTP Timing Config definition
 */
typedef struct
{
    uint32_t                mTPW;               /**< Configures the OTP transfer mode.*/
    uint32_t                mTPWI;              /**< Configures the color format of the output image.*/
    uint32_t                mTCS;               /**< Configures the color format of the output image. */
    uint32_t                mTPPS;              /**< Configures the color format of the output image. */
    uint32_t                mTPPH;              /**< Configures the color format of the output image. */
    uint32_t                mTPPR;              /**< Configures the color format of the output image. */
} OTP_TimingConfig_T;

/**
 * @brief OTP Init structure definition
 */
typedef struct
{
    OTP_TimingConfig_T      mTimgConfig;       /* otp Timing Config Data*/
    OTP_Callback_T          mOTPCallback;          /**< Callback definition to notify user in interrupt handler */
} OTP_Config_T;



/**
 * OTP user handle structure.
 */
typedef struct OTP_Handle
{
    OTP_Device_T        *mDevice;               /**< Pointer to uart device handle */
    OTP_MEMIO_T         *mMemIO;
    uint32_t            mIRQn;                   /**< uart irq number */
    OTP_Callback_T      mOTPCallback;          /**< Callback definition to notify user in interrupt handler */
} OTP_Handle_T;

/**
 * OTP Chip ID structure.
 */
typedef struct OTP_ARRAY4
{
    uint32_t    array[4];
} OTP_ARRAY4_T;

/**
 * OTP Chip ID structure.
 */
typedef struct OTP_ARRAY8
{
    uint32_t    array[8];
} OTP_ARRAY8_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief  Init OTP Device to buffer.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  pOTPDevice:  OTP Device.
 * @param  pConfig:     OTP Config.
 * @retval HAL status
 */
RET_CODE HAL_OTP_Init(OTP_Handle_T *pOTPHandle, OTP_Device_T *pOTPDevice, OTP_MEMIO_T *pOTPMemIO, OTP_Config_T *pConfig);

/**
 * @brief  Read Buffer from OTP Device to buffer.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  address:     OTP Device Offset address.
 * @param  buffer:      The dst memory Buffer.
 * @param  size:        read data size(DWORD).
 * @retval HAL status
 */
RET_CODE HAL_OTP_ReadBuffer(OTP_Handle_T *pOTPHandle, uint32_t address, void *buffer, uint32_t size);
/**
 * @brief  Write Buffer Data to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  address:     OTP Device Offset address.
 * @param  buffer:      The source memory Buffer.
 * @param  size:        write data size(DWORD).
 * @retval HAL status
 */
RET_CODE HAL_OTP_WriteBuffer(OTP_Handle_T *pOTPHandle, uint32_t address, void *buffer, uint32_t size);
/**
 * @brief  Write data to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  address:     OTP Device Offset address.
 * @param  data:        The data will be writed.
 * @retval HAL status
 */
RET_CODE HAL_OTP_WriteData(OTP_Handle_T *pOTPHandle, uint32_t address, uint32_t data);
/**
 * @brief  Read data from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  address:     OTP Device Offset address.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_ReadData(OTP_Handle_T *pOTPHandle, uint32_t address);
/**
 * @brief  Set Write Protection Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The wp1 will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetWP1(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get Write Protection Mask Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetWP1(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set Write Protection2 Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The wp2 will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetWP2(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get Write Protection2 Mask Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetWP2(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set Read Protection Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The rp mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetRP(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get Read Protection Mask Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetRP(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set Write Protection Selection Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The wp selection will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetWPSel(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get Write Protection Selection Mask Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetWPSel(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set System Config0 Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The system config0 mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg0(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get System Config0 Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg0(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set System Config1(MFC) Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The system config1(MFC) will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg1MFC(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get System Config1(MFC) Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg1MFC(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set System Config1(CSTM) Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The system config1(CSTM) mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg1CSTM(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get System Config1(CSTM) Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg1CSTM(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set System Config1 Selection Status to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The system config1 selection will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg1Sel(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get System Config1 Selection Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg1Sel(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set System Config2 mask to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The system config2 mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg2(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get System Config2 Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg2(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set System Config3 mask to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The system config3 mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg3(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get System Config3 Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg3(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set System Config4 mask to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The system config4 mask will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSysCfg4(OTP_Handle_T *pOTPHandle, uint32_t mask);
/**
 * @brief  Get System Config4 Status from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @retval data from OTP Device
 */
uint32_t HAL_OTP_GetSysCfg4(OTP_Handle_T *pOTPHandle);
/**
 * @brief  Set chipid to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The chipid array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetChipID(OTP_Handle_T *pOTPHandle, OTP_ARRAY8_T *chiparray);
/**
 * @brief  Get CHIPID from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  chiparray:  return chipid array(8 Dword).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetChipID(OTP_Handle_T *pOTPHandle, OTP_ARRAY8_T *chiparray);
/**
 * @brief  Set 128/256-bit hardware unique key to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The huk array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetHUK(OTP_Handle_T *pOTPHandle, OTP_ARRAY8_T *hukarray);
/**
 * @brief  Get 128/256-bit hardware unique key from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  hukarray:  return huk array(8 Dword).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetHUK(OTP_Handle_T *pOTPHandle, OTP_ARRAY8_T *hukarray);
/**
 * @brief  Set 256-bits root of trust public key to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The rotpk array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetROTPK(OTP_Handle_T *pOTPHandle, OTP_ARRAY8_T *rotpkarray);
/**
 * @brief  Get 256-bits root of trust public key from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  rotpkarray:  return rotpk array(8 Dword).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetROTPK(OTP_Handle_T *pOTPHandle, OTP_ARRAY8_T *rotpkarray);
/**
 * @brief  Set A 256-bit Endorsement Key to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The ek array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetEK(OTP_Handle_T *pOTPHandle, OTP_ARRAY8_T *ekarray);
/**
 * @brief  Get A 256-bit Endorsement Key from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  ekarray:  return ekpk array(8 Dword).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetEK(OTP_Handle_T *pOTPHandle, OTP_ARRAY8_T *ekarray);
/**
 * @brief  Set 128/256-bit Secret Symmetric Key to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The ssk array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetSSK(OTP_Handle_T *pOTPHandle, OTP_ARRAY8_T *sskarray);
/**
 * @brief  Get 128/256-bit Secret Symmetric Key from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  sskarray:  return ssk array(8 Dword).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetSSK(OTP_Handle_T *pOTPHandle, OTP_ARRAY8_T *sskarray);
/**
 * @brief  Set Trim data for analog configuration to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The trim array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetTrim(OTP_Handle_T *pOTPHandle, OTP_ARRAY4_T *trimarray);
/**
 * @brief  Get Trim data for analog configuration from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  trimarray:  return trim array(4 Dwords).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetTrim(OTP_Handle_T *pOTPHandle, OTP_ARRAY4_T *trimarray);
/**
 * @brief  Set firmware version to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The fwver array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetFWVer(OTP_Handle_T *pOTPHandle, OTP_ARRAY4_T *fwverarray);
/**
 * @brief  Get firmware version from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  fwverarray:  return fwver array(4 Dwords).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetFWVer(OTP_Handle_T *pOTPHandle, OTP_ARRAY4_T *fwverarray);
/**
 * @brief  Set DebugPort Password to OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  mask:        The dbgpw array will be setted.
 * @retval HAL status
 */
RET_CODE HAL_OTP_SetDBGPW(OTP_Handle_T *pOTPHandle, OTP_ARRAY4_T *dbgpwarray);

/**
 * @brief  Get DebugPort Password from OTP Device.
 * @param  pOTPHandle:  OTP HAL Handle.
 * @param  dbgpwarray:  return dbgpw array(4 Dwords).
 * @retval HAL Status
 */
RET_CODE HAL_OTP_GetDBGPW(OTP_Handle_T *pOTPHandle, OTP_ARRAY4_T *dbgpwarray);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_HAL_OTP_H_ */

