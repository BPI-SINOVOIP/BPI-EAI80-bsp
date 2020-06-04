/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        gm_ll_dac.h
 *
 * @author      Richard.Liu
 *
 * @version     v1.0.0
 *
 * @date        2019/01/24
 *
 * @brief       DAC source file
 *
 * @note
 *              2019/01/24, Richard.Liu, v1.0.0
 *                  Initial version.
 *
 **/

#ifndef _GM_LL_DAC_H_
#define _GM_LL_DAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include "gm_ll_dac_dev.h"
#include "gm_hal_dma.h"
#include <gm_soc.h>
#include <gm_basic_types.h>
#include <dac/gm_ll_dac_dev.h>

/**
 * @defgroup RET_CODE
 * Specify general return value
 * @{
 */
typedef int     RET_CODE; /* 32 bits */
#define RET_OK       0x00  /**< Function call returns succesfully  */
#define RET_ERROR    0x01  /**< Function fails because of some error condition  */
#define RET_BUSY     0x02  /**< Function fails because device is busy */
#define RET_TIMEOUT  0x03  /**< Function fails because transaction is not over within specified time */
#define RET_INVPARAM 0x04  /**< Function fails because parameter is invalid */
#define RET_ONGOING  0x05  /**< Non-blocking function returns if transaction is on progress and not finished */
/**
 * @}
 */


#define DAC0_DEV                                    ((DAC_Device_T *)DAC0_BASE) /* DAC device pointer  0x4000B000 */
#if defined(CHIP_GM6721)
#define DAC1_DEV                                    ((DAC_Device_T *)DAC1_BASE) /* DAC device pointer  0x4000B100 */
#define DAC_COM_DEV                                 ((DAC_ComDevice_T *)DAC_COM_BASE) /* DAC device pointer  0x4000B900 */
#endif

#if (defined(CHIP_GM6721) || defined(CHIP_F6721B))

#define DAC_SW_RSTJ                   (1 << 11)
#define DAC_CLK_EN                                  (1 << 11)

/* ************** DACx Controller Register bitfiled ***************** */

#define DAC_DATA_MOD_POS                            ((uint32_t) 20) /* DAC converted data mode pos  */
#define DAC_DATA_MOD                                ((uint32_t) ((uint32_t)0x3 << DAC_DATA_MOD_POS))    /* DAC converted data mode  filed */

#define DAC_DMA_EN_POS                              ((uint32_t) 16) /* DAC_DMA_EN:  DMA enable pos  */
#define DAC_DMA_EN                                  ((uint32_t) ((uint32_t)0x01 << DAC_DMA_EN_POS))    /* DAC  DMA enable enable  filed */

#define DAC_MS_AM_POS                               ((uint32_t) 12) /*  Mask bit of LSFR and amplitude value in wave generation pos */
#define DAC_MS_AM                                   ((uint32_t) ((uint32_t)0x0F << DAC_MS_AM_POS))    /* DAC  Mask bit of LSFR and amplitude value in wave generation */

#define DAC_WAVE_POS                                ((uint32_t) 8) /* DAC noise/triangle wave generation enable pos */
#define DAC_WAVE                                    ((uint32_t) ((uint32_t)0x03 << DAC_WAVE_POS))  /* DAC noise/triangle wave generation enable */

#define DAC_SHEN_POS                                ((uint32_t) 4)  /* DAC sample and hold enable bit pos */
#define DAC_SHEN                                    ((uint32_t) ((uint32_t)0x01 << DAC_SHEN_POS))  /* DAC sample and hold enable bit */

#define DAC_EN_POS                                  ((uint32_t) 0) /* DAC modules enable pos */
#define DAC_EN                                      ((uint32_t) ((uint32_t)0x01 << DAC_EN_POS)) /* DAC modules enable */


#define DAC_EN_POS                                  ((uint32_t) 0) /* DAC modules enable pos */
#define DAC_EN                                      ((uint32_t) ((uint32_t)0x01 << DAC_EN_POS)) /* DAC modules enable */

#define DAC_DMA_UDRIE_POS                           ((uint32_t) 0) /* DAC DMA underrun interrupt enable pos */
#define DAC_DMA_UDRIE_EN                            ((uint32_t) ((uint32_t)0x01 << DAC_DMA_UDRIE_POS)) /* DAC DMA underrun interrupt enable */


/* ************** DACx Status Register  bitfiled ***************** */

#define DAC_BW_ST_RO_POS                       ((uint32_t) 0) /* DACx Status Register pos */
#define DAC_BW_ST                              ((uint32_t) ((uint32_t)0x01 << DAC_BW_ST_RO_POS)) /* DACx Status Register */

#define DAC_DMA_UDRINT_ST_POS                       ((uint32_t) 0) /* DACx Status Register pos */
#define DAC_DMA_UDRINT_ST                           ((uint32_t) ((uint32_t)0x01 << DAC_DMA_UDRINT_ST_POS)) /* DACx Status Register */


/* ************** DACx Data Hold Register   bitfiled ***************** */
#define DAC_DH_POS                            ((uint32_t) 0) /* DAC noise/triangle wave generation  pos */
#define DAC_DATAHOLD                                ((uint32_t) ((uint32_t)0xFFF << DAC_DH_POS))  /* DAC noise/triangle wave generation  */


/* ************** DACx Data Hold Register   bitfiled ***************** */
#define DAC0_DH_POS                            ((uint32_t) 0) /* DAC Dual Data Hold Register  pos */
#define DAC0_DATA_DOUAL_HOLD                                ((uint32_t) ((uint32_t)0xFFF << DAC0_DH_POS))  /* DAC Dual Data Hold Register  */

#define DAC1_DH_POS                            ((uint32_t) 16) /* DAC Dual Data Hold Register  pos */
#define DAC1_DATA_DOUAL_HOLD                                ((uint32_t) ((uint32_t)0xFFF << DAC1_DH_POS))  /* DAC Dual Data Hold Register  */


/* **************DACx Software Trigger Register bitfiled ***************** */
#define DAC_TRIG_POS                            ((uint32_t) 0) /* DAC noise/triangle wave generation enable pos */
#define DAC_TRIG_EN                             ((uint32_t) ((uint32_t)1 << DAC_TRIG_POS))  /* DAC noise/triangle wave generation enable */


/* **************DACx Hardware Trigger Register bitfiled ***************** */
#define DAC_HTRIG_SEL_POS                            ((uint32_t) 0) /* DAC DAC hardware trigger source selection pos */
#define DAC_HTRIG_SEL                                ((uint32_t) ((uint32_t)1 << DAC_HTRIG_SEL_POS))  /* DAC hardware trigger source selection */

#define DAC_HTRIG_EN_POS                            ((uint32_t) 0) /* DAC hardware trigger enable pos */
#define DAC_HTRIG_EN                                ((uint32_t) ((uint32_t)1 << DAC_HTRIG_EN_POS))  /* DAC hardware trigger enable enable */

/* **************DACx Sample and Hold Sample Time Register bitfiled ***************** */
#define DAC_SHST_POS                            ((uint32_t) 0) /* DAC Sample and Hold Sample Time Register  pos */
#define DAC_SHST_TIME                           ((uint32_t) ((uint32_t)0x3FF << DAC_SHST_POS))  /* DAC Sample and Hold Sample Time Register Register  */


/* **************DACx Sample and Hold Hold Time Register  bitfiled ***************** */
#define DAC_SHHT_POS                            ((uint32_t) 0) /* DAC Sample and Hold Hold Sample Time Register  pos */
#define DAC_SHHT_TIME                           ((uint32_t) ((uint32_t)0x3FF << DAC_SHHT_POS))  /* DAC Sample and Hold Hold Sample Time Register Register  */


/* **************DACx Sample and Hold Refresh Time Register   bitfiled ***************** */
#define DAC_SHRF_POS                            ((uint32_t) 0) /* DAC Sample and Hold Hold Sample Time Register  pos */
#define DAC_SHRF_TIME                           ((uint32_t) ((uint32_t)0x3FF << DAC_SHRF_POS))  /* DAC Sample and Hold Hold Sample Time Register Register  */



/* **************DAC Dual Software Trigger Register bitfiled ***************** */
#define DAC_DUAL_TRIG_POS                            ((uint32_t) 0) /* DAC Dual Software Trigger Register enable pos */
#define DAC_DUAL_TRIG_EN                             ((uint32_t) ((uint32_t)1 << DAC_DUAL_TRIG_POS))  /* DAC DAC Dual Software Trigger Register enable */


/**
 * @brief DAC function enable
 * @param pDAC                DAC devive point
 *
 * @retval                     None
 */
#define DAC_DMA_ENABLE(pDAC)                          IO_BITS_SET(pDAC->DAC_CTRL, DAC_DMA_EN)

/**
 * @brief DAC function disable
 *
 * @param pDAC                DAC devive point
 *
 * @retval                     None
 */
#define DAC_DMA_DISABLE(pDAC)                         IO_BIT_CLEAR(pDAC->DAC_CTRL, DAC_DMA_EN)


/**
 * @brief Mask bit of LSFR and amplitude value in wave generation
 *
 * @param pDAC                DAC devive point
 * @param val                  val
 *
 * @retval                     None
 */

#define DAC_SET_DATA_MODE(pDAC, val)                IO_BITMASK_SET(pDAC->DAC_CTRL, DAC_DATA_MOD,            \
        (val<<DAC_DATA_MOD_POS))



/**
 * @brief Mask bit of LSFR and amplitude value in wave generation
 *
 * @param pDAC                DAC devive point
 * @param val                  val
 *
 * @retval                     None
 */

#define DAC_SET_MS_AM(pDAC, val)                IO_BITMASK_SET(pDAC->DAC_CTRL, DAC_MS_AM,            \
        (val<<DAC_MS_AM_POS))

/**
 * @brief DAC noise/triangle wave generation enable
 *
 * @param pDAC                DAC devive point
 * @param val                  val
 *
 * @retval                     None
 */

#define DAC_SET_WAVE_TYPE(pDAC, val)                IO_BITMASK_SET(pDAC->DAC_CTRL, DAC_WAVE,            \
        (val<<DAC_WAVE_POS))

/**
 * @brief DAC interrupt enable
 *
 * @param pDAC                DAC devive point
 *
 * @retval                     None
 */
#define DAC_INTERRUPT_ENABLE(pDAC)                IO_BITS_SET(pDAC->DAC_IE, DAC_DMA_UDRIE_EN)

/**
 * @brief DAC interrupt disable
 *
 * @param pDAC                DAC devive point
 *
 * @retval                     None
 */
#define DAC_INTERRUPT_DISABLE(pDAC)               IO_BIT_CLEAR(pDAC->DAC_IE, DAC_DMA_UDRIE_EN)


/**
 * @brief DAC function enable
 * @param pDAC                DAC devive point
 *
 * @retval                     None
 */
#define DAC_SHEN_ENABLE(pDAC)                          IO_BITS_SET(pDAC->DAC_CTRL, DAC_SHEN)

/**
 * @brief DAC function disable
 *
 * @param pDAC                DAC devive point
 *
 * @retval                     None
 */
#define DAC_SHEN_DISABLE(pDAC)                         IO_BIT_CLEAR(pDAC->DAC_CTRL, DAC_SHEN)


/**
 * @brief DAC function enable
 * @param pDAC                DAC devive point
 *
 * @retval                     None
 */
#define DAC_ENABLE(pDAC)                          IO_BITS_SET(pDAC->DAC_CTRL, DAC_EN)

/**
 * @brief DAC function disable
 *
 * @param pDAC                DAC devive point
 *
 * @retval                     None
 */
#define DAC_DISABLE(pDAC)                         IO_BIT_CLEAR(pDAC->DAC_CTRL, DAC_EN)

/**
 * @brief Clear DAC interrupt
 *
 * @param pDAC               DAC devive point
 * @retval                     None
 */
#define DAC_CLEAR_INTERRUPT(pDAC)                 IO_BITS_SET(pDAC->DAC_STATUS, DAC_DMA_UDRINT_ST)


#define DAC_SET_DATAHOLD_REG(pDAC, val)                IO_BITMASK_SET(pDAC->DAC_DH, DAC_DATAHOLD,            \
        (val<<DAC_DH_POS))

#define DAC0_SET_DOUL_HOLD_DATA(pDAC_COM, val)                IO_BITMASK_SET(pDAC_COM->DAC_DDH, DAC0_DATA_DOUAL_HOLD,            \
        (val<<DAC0_DH_POS))
#define DAC1_SET_DOUL_HOLD_DATA(pDAC_COM, val)                IO_BITMASK_SET(pDAC_COM->DAC_DDH, DAC1_DATA_DOUAL_HOLD,            \
        (val<<DAC1_DH_POS))


#define DAC_SET_SAMPLE_HOLD_SAME_TIME(pDAC, val)                IO_BITMASK_SET(pDAC->DAC_SHST, DAC_SHHT_TIME,            \
        (val<<DAC_SHHT_POS))

#define DAC_SET_SAMPLE_HOLD_HOLD_TIME(pDAC, val)                IO_BITMASK_SET(pDAC->DAC_SHHT, DAC_SHHT_TIME,            \
        (val<<DAC_SHHT_POS))


#define DAC_SET_SAMPLE_HOLD_REFRESH_TIME(pDAC, val)                IO_BITMASK_SET(pDAC->DAC_SHRF, DAC_SHRF_TIME,            \
        (val<<DAC_SHRF_POS))

#define DAC_SOFT_TRIG(pDAC)                    IO_BITS_SET(pDAC->DAC_STRIG, DAC_TRIG_EN)

#define DAC_DUAL_SOFT_TRIG(pDAC_COM)            IO_BITS_SET(pDAC_COM->DAC_DTRIG, DAC_DUAL_TRIG_EN) /* DAC Dual Software Trigger Register*/


#define DAC_HARD_TRIG(pDAC)                    IO_BITS_SET(pDAC->DAC_HTRIG, DAC_HTRIG_EN)

/**
 * @brief Resert ACMP IP
 *
 * @retval                     None
 */
#define DAC_RESET(void)                           IO_BITS_SET(SYSRSTCLKREG->IP_RST_CTRL[0], DAC_SW_RSTJ)

/**
 * @brief Enable ACMP clock
 *
 * @retval                     None
 */
#define DAC_CLKEN(void)                          IO_BITS_SET(SYSRSTCLKREG->CLK_EN_CTRL[0], DAC_CLK_EN)


#define DAC_CLK_SEL(val)                        IO_BITMASK_SET(SYSRSTCLKREG->CLK_SRC_CTRL[4], 0X1<<24, (val<<24))


#endif


/**
 *  DAC events
 */
typedef enum
{
    DAC_MATCH_EVENT              = 0x00,             /**< When positivepin and negativepin match voltage mode which is
                                                     *   by CompareMatchTrigger mode,it will send event to usr
                                                     */
} DAC_Event_T;

typedef enum
{
    LOSC_CLOCK = 0,
    LIRC_CLOCK = 1,
} DAC_LS_CLK_SEL_T;


/**
 *  Compare match trigger mode
 */
typedef enum
{
    DAC_SINGLE_SOFT_TRIG        = 0x00,
    DAC_HARDWARE_TRIG           = 0x01,
#if defined(CHIP_GM6721)
    DAC_DUAL_SOFTWARE_TRIG      = 0x02,
#endif
} DAC_TriggerType_T;

/**
 *  DAC Select
 */
typedef enum
{
    DAC0_SEL        = 0x00, /*Select DAC0*/
    DAC1_SEL        = 0x01, /*Select DAC1*/
    DAC_DUAL        = 0x02, /*Select DAC0 and DAC1*/
} DAC_SELECT_T;

/**
 *  Compare match trigger mode
 */
typedef enum
{
    DAC_WAVE_DISABLE        = 0x00,
    DAS_WAVE_NOISE_EN       = 0x01,
    DAS_WAVE_TRIANGLE_EN    = 0x03,
} DAC_WAVE_TYPE_T;

/**
 *  Compare match trigger mode
 */
typedef enum
{
    DAC_DATA_MODE_12BIT       = 0x00,
    DAC_DATA_MODE_10BIT       = 0x01,
    DAC_DATA_MODE_8BIT        = 0x02,
} DAC_DATA_MODE_T;


/**
 *  Usr DAC parameter config
 */
typedef struct
{
    uint16_t                 dacHoldData;
    uint16_t                 dacHoldSampTime; /**< Sample and Hold Sample Time */
    uint16_t                 dacHoldTime;     /**< DACx Sample and Hold Hold Time Register */
    uint16_t                 dacHoldRefTime;  /**< DACx Sample and Hold Refresh Time Register  */
    uint16_t                 dacDAC1DualHoldData; /**<  DAC Dual Data Hold Register */
    uint16_t                 dacDAC0DualHoldData; /**<  DAC Dual Data Hold Register */

} DAC_ComConfig_T;

typedef struct
{
    uint8_t                     dacEn;
    uint8_t                     samholdEn;
    uint8_t                     waveType;
    uint8_t                     msAM; /**< Mask bit of LSFR and amplitude value in wave generation */
    uint8_t                     dacDMAEn;
    uint8_t                     resolution;
    uint8_t                     dacSoftTrigleEn;

    DAC_ComConfig_T            dacComConfig;         /**< Config dac convert parameter */
    DAC_TriggerType_T          triggerMode;       /**< Inputpin voltage match trigger mode */
    DAC_SELECT_T               dacSel;
    //    DAC_Callback_T             callback;          /**< Driver send usr event to call this function */

} DAC_Config_T;

/**
 * DAC user handle structure.
 */
typedef struct
{
    DAC_Device_T   *device;                       /**< Pointer to dac device */
    DAC_Config_T   config;
    DMA_Handle_T             *pDMAHandle;                 /*!< Pointer DMA Handler */
    uint32_t rst_data[16];
    uint32_t dma_rst_len;

} DAC_Handle_T;

//typedef void (*DAC_Callback_T) (DAC_Handle_T *hDAC, DAC_Event_T event, unsigned long Param);

/**
  * @brief HAL State structures definition
  */
typedef enum
{
    HAL_DAC_STATE_DMA_FINISH,

} HAL_DAC_StateTypeDef;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_LL_DAC_H_ */

