/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_hal_lvd.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.2
 *
 * @date          2016/08/17
 *
 * @brief         lvd device header file
 *
 * @note
 *    2016/08/17, mark.lee,     V1.0.0  Initial version.
 *    2017/05/19, Angrad.Yang,  V1.0.1  Modified LVD_Init to enable that VD1 and VD2 can work together.
 *    2019/02/23, Angrad.Yang,  V1.0.2  Modification for GM6721.
 *
 */
#ifndef _GM_HAL_LVD_H_
#define _GM_HAL_LVD_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************************************
 * VD1 Control
 ************************************************/
#define HAL_LVD_VD1_LOCK			(0x01000000U)

#define HAL_LVD_VD1_LEVEL			(0x00000700U)
#define HAL_LVD_VD1_LEVEL_POS       (8U)
#define HAL_LVD_VD1_LEVEL_FIELD(x)	(((uint32_t)(((uint32_t)(x)) << HAL_LVD_VD1_LEVEL_POS)) & HAL_LVD_VD1_LEVEL)

#define HAL_LVD_VD1_RSTEN			(0x00000010U)
#define HAL_LVD_VD1_EN				(0x00000001U)

/************************************************
 * VD1 Status
 ************************************************/
#define HAL_LVD_VD1_OUT				(0x00000001U)


/************************************************
 * VD2 Control
 ************************************************/
#define HAL_LVD_VD2_LOCK			(0x01000000U)

#define HAL_LVD_VD2_THR				(0x00FF0000U)	/* Timing threshold for filter the glitch of voltage detector 2 */
#define HAL_LVD_VD2_THR_POS			(16U)
#define HAL_LVD_VD2_THR_FIELD(x)	(((uint32_t)(((uint32_t)(x)) << HAL_LVD_VD2_THR_POS)) & HAL_LVD_VD2_THR)

#define HAL_LVD_VD2_FLT_CLK_PSC		(0x00070000U)	/*  Prescaler of the voltage detector 2's filter clock */
#define HAL_LVD_VD2_FLT_CLK_PSC_POS	(12U)
#define HAL_LVD_VD2_FLT_FIELD(x)	(((uint32_t)(((uint32_t)(x)) << HAL_LVD_VD2_FLT_CLK_PSC_POS)) & HAL_LVD_VD2_FLT_CLK_PSC)

#define HAL_LVD_VD2_LEVEL			(0x00000700U)
#define HAL_LVD_VD2_LEVEL_POS       (8U)
#define HAL_LVD_VD2_LEVEL_FIELD(x)	(((uint32_t)(((uint32_t)(x)) << HAL_LVD_VD1_LEVEL_POS)) & HAL_LVD_VD1_LEVEL)

#define HAL_LVD_VD2_EN				(0x00000001U)

/************************************************
 * VD2 Status
 ************************************************/
#define HAL_LVD_VD2_FLAG			(0x00000002U)	/* VDDCORE < VD2_SEL is detected */
#define HAL_LVD_VD2_OUT				(0x00000001U)

/************************************************
 * VD3 Control
 ************************************************/
#define HAL_LVD_VD3_LOCK			(0x01000000U)

#define HAL_LVD_VD3_LEVEL			(0x00000300U)
#define HAL_LVD_VD3_LEVEL_POS       (8U)
#define HAL_LVD_VD3_LEVEL_FIELD(x)	(((uint32_t)(((uint32_t)(x)) << HAL_LVD_VD1_LEVEL_POS)) & HAL_LVD_VD1_LEVEL)

#define HAL_LVD_VD3_RSTEN			(0x00000010U)

#define HAL_LVD_VD3_EN				(0x00000001U)

/************************************************
 * VD3 Status
 ************************************************/
#define HAL_LVD_VD3_OUT				(0x00000001U)


/************************************************
 * Reset Status
 ************************************************/
#define HAL_LVD_RST_STATUS_LVD		(0x00000001U)


/**
 * List of low voltage detection modules.
 */
typedef enum
{
    LVD_VOLDT_1 = 1U,		/**< Voltage detection 1 */
    LVD_VOLDT_2 = 2U,		/**< Voltage detection 2*/
    LVD_VOLDT_3 = 3U		/**< Voltage detection 2*/
}
LVD_VoltageDetection_T;

/** @defgroup Detection voltage level
  * @{
  */
#define LVD_VD1_Level_0        0x0    /* 1.70+0.05V/-0.05V  */ 
#define LVD_VD1_Level_1        0x1    /* 1.80+0.05V/-0.05V  */ 
#define LVD_VD1_Level_2        0x2    /* 2.00+0.05V/-0.05V  */ 
#define LVD_VD1_Level_3        0x3    /* 2.20+0.05V/-0.05V  */ 
#define LVD_VD1_Level_4        0x4    /* 2.40+0.05V/-0.05V  */ 
#define LVD_VD1_Level_5        0x5    /* 2.60+0.05V/-0.05V  */ 
#define LVD_VD1_Level_6        0x6    /* 2.80+0.05V/-0.05V  */ 
#define LVD_VD1_Level_7        0x7    /* 3.00+0.05V/-0.05V  */ //POR Reset at level 2.4v
                                                                  
#define LVD_VD2_Level_0        0x0    /* 1.80+0.05V/-0.05V  */ 
#define LVD_VD2_Level_1        0x1    /* 1.90+0.05V/-0.05V  */ 
#define LVD_VD2_Level_2        0x2    /* 2.00+0.05V/-0.05V  */ 
#define LVD_VD2_Level_3        0x3    /* 2.20+0.05V/-0.05V  */ 
#define LVD_VD2_Level_4        0x4    /* 2.40+0.05V/-0.05V  */ 
#define LVD_VD2_Level_5        0x5    /* 2.60+0.05V/-0.05V  */ 
#define LVD_VD2_Level_6        0x6    /* 2.80+0.05V/-0.05V  */ 
#define LVD_VD2_Level_7        0x7    /* 3.00+0.05V/-0.05V  */ 
                                                                  
#define LVD_VD3_Level_0        0x0    /* 0.90+0.03V/-0.03V  */ 
#define LVD_VD3_Level_1        0x1    /* 0.95+0.03V/-0.03V  */ 
#define LVD_VD3_Level_2        0x2    /* 1.00+0.03V/-0.03V  */ 
#define LVD_VD3_Level_3        0x3    /* 1.05+0.03V/-0.03V  */ 
/**
  * @}
  */

/**
 * LVD configuration structure
 *
 * This structure holds the configuration settings for Voltage detection.
 *
 */
typedef void (*LVD_Int_Callback) (int);

typedef struct
{
    int threshold;         /**< Configures timing threshold for filter the glitch of voltage detector */
    int detectionLevel;    /**< Configures the voltage detection level */
    int reset;             /**< reset chip when VDD is less than detectionLevel */
    int enable;            /**< enable the operation of voltage detection */
	LVD_Int_Callback int_call;
} LVD_Config_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/

 /**
 * @brief Default LVD Irq Handler
 *
 * @param               None.
 * @retval              None.
 */
void HAL_LVD_IRQHandler(void);

/**
 * @brief Initializes the LVD module according to the specified parameters.
 *
 * @param pConfig1  Pointer to the configuration structure of VD1
 * @param pConfig2  Pointer to the configuration structure of VD2
 * @param pConfig2  Pointer to the configuration structure of VD3
 * @retval None.
 */
void HAL_LVD_Init(const LVD_Config_T *pConfig1, const LVD_Config_T *pConfig2,
                  const LVD_Config_T *pConfig3);


/**
 * @brief Enables voltage detection interrupt.
 *
 * @param voltage Selects low voltage detection module.
 * @retval None.
 */
void HAL_LVD_EnableInterrupt(LVD_VoltageDetection_T voltage);

/**
 * @brief Disables voltage detection interrupt.
 *
 * @param voltage Selects low voltage detection module.
 * @retval None.
 */
void HAL_LVD_DisableInterrupt(LVD_VoltageDetection_T voltage);

/**
 * @brief  Clear interrupt flag.
 *
 * @param  voltage  Selects low voltage detection module.
 * @retval None.
 */
void HAL_LVD_ClearInterruptFlag(LVD_VoltageDetection_T voltage);

/**
 * @brief  Get LVD status.
 *
 * @param  LVD module source
 * @retval Flag whether lower voltage status is detected.
 */
int HAL_LVD_GetVDXFlag(int source);

/**
 * @brief  Lock LVD configuration.
 *
 * @param  voltage  Selects low voltage detection module.
 * @retval None.
 */
void HAL_LVD_Lock(LVD_VoltageDetection_T voltage);

/**
 * @brief  Reset detection voltage level.
 *
 * @param  voltage  Selects low voltage detection module.
 *		   voltage level
 * @retval None.
 */
void HAL_LVD_ResetLevel(LVD_VoltageDetection_T voltage, int level);

/**
 * @brief  POR or Voltage detect reset status.
 *
 * @param  None.
 * @retval 0: Reset not caused by VOL_DET or POR.
 *         1: Reset by VOL_DET or POR. 
 */
int HAL_LVD_CheckResetStatus(void);

/**
 * @brief  Clear the caused by LVD reset.
 *
 * @param  None.
 * @retval None.
 */
void HAL_LVD_ClearResetStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_HAL_LVD_H_ */

