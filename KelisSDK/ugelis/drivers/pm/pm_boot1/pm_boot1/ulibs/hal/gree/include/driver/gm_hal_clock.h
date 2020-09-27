#ifndef _GM_HAL_CLOCK_H_
#define _GM_HAL_CLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <clock/gm_ll_clock.h>

/**
  * @defgroup clock mask
  */
typedef enum
{
    CLOCK_HIRC_MASK         = 0x01,
    CLOCK_HOSC_MASK         = 0x02,
    CLOCK_LIRC_MASK         = 0x04,
    CLOCK_LOSC_MASK         = 0x08,
    CLOCK_PLLKDP_MASK       = 0x10,
    CLOCK_PLL_MASK          = 0x20,
    CLOCK_PLLI2S_MASK       = 0x40,
    CLOCK_PLLSAI_MASK       = 0x80,
} CLOCK_Sel_T;


/**
  * @defgroup peripheral mask
  */
typedef enum
{
    PERIPHERAL_SDRAM_MASK       = 0x00000001,
    PERIPHERAL_WWDT_MASK        = 0x00000002,
    PERIPHERAL_LPSTM_MASK       = 0x00000004,
    PERIPHERAL_MCO1_MASK        = 0x00000008,
    PERIPHERAL_MCO2_MASK        = 0x00000010,
    PERIPHERAL_RTC_MASK         = 0x00000020,
    PERIPHERAL_KDP_MASK         = 0x00000040,
    PERIPHERAL_ADC1_MASK        = 0x00000080,
    PERIPHERAL_ADC2_MASK        = 0x00000100,
    PERIPHERAL_ADC3_MASK        = 0x00000200,
    PERIPHERAL_DAC_MASK         = 0x00000400,
    PERIPHERAL_SDMMC_MASK       = 0x00000800,
    PERIPHERAL_VOUT_MASK        = 0x00001000,
    PERIPHERAL_I2C1_MASK        = 0x00002000,
    PERIPHERAL_I2C2_MASK        = 0x00004000,
    PERIPHERAL_I2C3_MASK        = 0x00008000,
    PERIPHERAL_BTM1_MASK        = 0x00010000,
    PERIPHERAL_BTM2_MASK        = 0x00020000,
    PERIPHERAL_SAIA_MASK        = 0x00040000,
    PERIPHERAL_SAIB_MASK        = 0x00080000,
    PERIPHERAL_UART1_MASK       = 0x10000001,
    PERIPHERAL_UART2_MASK       = 0x10000002,
    PERIPHERAL_UART3_MASK       = 0x10000004,
    PERIPHERAL_UART4_MASK       = 0x10000008,
    PERIPHERAL_UART5_MASK       = 0x10000010,
    PERIPHERAL_UART6_MASK       = 0x10000020,
    PERIPHERAL_UART7_MASK       = 0x10000040,
    PERIPHERAL_UART8_MASK       = 0x10000080,
    PERIPHERAL_TIM1_MASK        = 0x10000100,
    PERIPHERAL_TIM2_MASK        = 0x10000200,
    PERIPHERAL_TIM3_MASK        = 0x10000400,
    PERIPHERAL_TIM4_MASK        = 0x10000800,
    PERIPHERAL_TIM5_MASK        = 0x10001000,
    PERIPHERAL_TIM8_MASK        = 0x10002000,
    PERIPHERAL_TIM9_MASK        = 0x10004000,
    PERIPHERAL_TIM10_MASK       = 0x10008000,
    PERIPHERAL_TIM11_MASK       = 0x10010000,
    PERIPHERAL_TIM12_MASK       = 0x10020000,
    PERIPHERAL_TIM13_MASK       = 0x10040000,
    PERIPHERAL_TIM14_MASK       = 0x10080000,
} CLOCK_PeripheMask_T;

/**
  * CPU clock prescaler
  */
typedef enum
{
    CLOCK_DIVIDED_1             = 0x000,    /**<CPU clock(FCLK) divided by 1 */
    CLOCK_DIVIDED_2             = 0x001,    /**<CPU clock(FCLK) divided by 2 */
    CLOCK_DIVIDED_4             = 0x003,    /**<CPU clock(FCLK) divided by 4 */
    CLOCK_DIVIDED_8             = 0x007,    /**<CPU clock(FCLK) divided by 8 */
    CLOCK_DIVIDED_16            = 0x00F,    /**<CPU clock(FCLK) divided by 16 */
    CLOCK_DIVIDED_32            = 0x01F,    /**<CPU clock(FCLK) divided by 32 */
    CLOCK_DIVIDED_64            = 0x03F,    /**<CPU clock(FCLK) divided by 64 */
    CLOCK_DIVIDED_128           = 0x07F,    /**<CPU clock(FCLK) divided by 128 */
    CLOCK_DIVIDED_256           = 0x0FF,    /**<CPU clock(FCLK) divided by 256 */
    CLOCK_DIVIDED_512           = 0x1FF,    /**<CPU clock(FCLK) divided by 512 */
} CLOCK_SysClockPrescaler_T;

/**
  * CPU source clock
  *
  */
typedef enum
{
    SYSCLOCK_SOURCE_HIRC8M      = 0x00,
    SYSCLOCK_SOURCE_HOSC8M      = 0x01,
    SYSCLOCK_SOURCE_PLLKDP150   = 0x02,
    SYSCLOCK_SOURCE_PLL         = 0x03,
    SYSCLOCK_SOURCE_PLLKDP100   = 0x04,
#if defined(CHIP_F6721B)
    SYSCLOCK_SOURCE_PLLSAI      = 0x05,
#endif
} CLOCK_SysClockSource_T;

/**
  * PLL reference clock select
  *
  */
typedef enum
{
    PLL_IRC_HSCLK = 0x00,       /**< PLL reference clock is derived from HIRC 8MHz */
    PLL_OSC_HSCLK = 0x01,       /**< PLL reference clock is derived from HOSC 8MHz */
} CLOCK_PLLRefClk_T;

#define IS_PLL_REFCLK(VALUE)    ((VALUE == PLL_IRC_HSCLK) || (VALUE == PLL_OSC_HSCLK))

/**
  * Clock Configuration
  */
typedef struct
{
    CLOCK_SysClockSource_T      sysClkSource;
    CLOCK_SysClockPrescaler_T   sysClkPrescaler;
} CLOCK_SysClockConfig_T;

/**
  * Peripheral source clock select and prescaler set
  */
/******************************SDRAM clock only sourced by system clock***************************/
typedef enum
{
    CLOCK_SDRAM_DIV_1           = 0x00,
    CLOCK_SDRAM_DIV_2           = 0x01,
    CLOCK_SDRAM_DIV_3           = 0x02,
    CLOCK_SDRAM_DIV_4           = 0x03,
    CLOCK_SDRAM_DIV_5           = 0x04,
    CLOCK_SDRAM_DIV_6           = 0x05,
    CLOCK_SDRAM_DIV_7           = 0x06,
    CLOCK_SDRAM_DIV_8           = 0x07,
} PeripheClk_SDRAM_Config_T;

/******************************WWDT clock only sourced by system clock****************************/
typedef enum
{
    CLOCK_WWDT_DIV_1            = 0x000,
    CLOCK_WWDT_DIV_2            = 0x001,
    CLOCK_WWDT_DIV_4            = 0x003,
    CLOCK_WWDT_DIV_8            = 0x007,
    CLOCK_WWDT_DIV_16           = 0x00F,
    CLOCK_WWDT_DIV_32           = 0x01F,
    CLOCK_WWDT_DIV_64           = 0x03F,
    CLOCK_WWDT_DIV_128          = 0x07F,
    CLOCK_WWDT_DIV_256          = 0x0FF,
    CLOCK_WWDT_DIV_512          = 0x1FF,
} PeripheClk_WWDT_Config_T;

/******************************LPSTM has no prescaler*******************************************/
typedef enum
{
    CLOCK_LPSTM_SEL_SYSCLK      = 0x00,
    CLOCK_LPSTM_SEL_LIRC        = 0x01,
    CLOCK_LPSTM_SEL_LOSC        = 0x02,
    CLOCK_LPSTM_SEL_HIRC        = 0x03,
} PeripheClk_LPSTM_Config_T;

/******************************MCO2 clock configuration****************************************/
typedef enum
{
    CLOCK_MCO2_DIV_1            = 0x00,
    CLOCK_MCO2_DIV_2            = 0x01,
    CLOCK_MCO2_DIV_3            = 0x02,
    CLOCK_MCO2_DIV_4            = 0x03,
    CLOCK_MCO2_DIV_5            = 0x04,
    CLOCK_MCO2_DIV_6            = 0x05,
    CLOCK_MCO2_DIV_7            = 0x06,
    CLOCK_MCO2_DIV_8            = 0x07,
} PeripheClk_MCO2_prescaler_T;

typedef enum
{
    CLOCK_MCO2_SEL_PLLKDPP      = 0x00,
    CLOCK_MCO2_SEL_PLLKDPQ      = 0x01,
    CLOCK_MCO2_SEL_PLLKDPR      = 0x02,
    CLOCK_MCO2_SEL_PLLI2SP      = 0x03,
    CLOCK_MCO2_SEL_PLLI2SQ      = 0x04,
    CLOCK_MCO2_SEL_PLLI2SR      = 0x05,
    CLOCK_MCO2_SEL_PLLSAIP      = 0x06,
    CLOCK_MCO2_SEL_HCLK         = 0x07,
} PeripheClk_MCO2_sel_T;

typedef struct
{
    PeripheClk_MCO2_prescaler_T  mco2Prescaler;
    PeripheClk_MCO2_sel_T        mco2SourceClk;
} PeripheClk_MCO2_Config_T;


/******************************MCO1 clock configuration****************************************/
typedef enum
{
    CLOCK_MCO1_DIV_1            = 0x00,
    CLOCK_MCO1_DIV_2            = 0x01,
    CLOCK_MCO1_DIV_3            = 0x02,
    CLOCK_MCO1_DIV_4            = 0x03,
    CLOCK_MCO1_DIV_5            = 0x04,
    CLOCK_MCO1_DIV_6            = 0x05,
    CLOCK_MCO1_DIV_7            = 0x06,
    CLOCK_MCO1_DIV_8            = 0x07,
} PeripheClk_MCO1_prescaler_T;

typedef enum
{
    CLOCK_MCO1_SEL_FCLK         = 0x00,
    CLOCK_MCO1_SEL_HIRC         = 0x01,
    CLOCK_MCO1_SEL_LIRC         = 0x02,
    CLOCK_MCO1_SEL_HOSC         = 0x03,
    CLOCK_MCO1_SEL_LOSC         = 0x04,
    CLOCK_MCO1_SEL_PLLP         = 0x05,
    CLOCK_MCO1_SEL_PLLQ         = 0x06,
    CLOCK_MCO1_SEL_PLLSAIP      = 0x07,
} PeripheClk_MCO1_sel_T;

typedef struct
{
    PeripheClk_MCO1_prescaler_T  mco1Prescaler;
    PeripheClk_MCO1_sel_T        mco1SourceClk;
} PeripheClk_MCO1_Config_T;


/******************************RTC clock configuration*****************************************/
typedef enum
{
    CLOCK_RTC_HOSCDIV_1         = 0x00,
    CLOCK_RTC_HOSCDIV_2         = 0x01,
    CLOCK_RTC_HOSCDIV_4         = 0x03,
    CLOCK_RTC_HOSCDIV_8         = 0x07,
    CLOCK_RTC_HOSCDIV_16        = 0x0F,
    CLOCK_RTC_HOSCDIV_32        = 0x1F,
} PeripheClk_RTC_hoscdiv_T;

typedef enum
{
    CLOCK_RTC_SEL_LIRC          = 0x00,
    CLOCK_RTC_SEL_LOSC          = 0x01,
    CLOCK_RTC_SEL_HOSC          = 0x02,
} PeripheClk_RTC_sel_T;

typedef struct
{
    PeripheClk_RTC_hoscdiv_T    rtcHOSCDiv;
    PeripheClk_RTC_sel_T        rtcSourceClk;
} PeripheClk_RTC_Config_T;


/******************************KDP has no prescaler********************************************/
typedef enum
{
    CLOCK_KDP_SEL_SYSCLK        = 0x00,
    CLOCK_KDP_SEL_PLLKDP300     = 0x01,
} PeripheClk_KDP_Config_T;


/******************************ADC clock configuration*****************************************/
typedef enum
{
    CLOCK_ADC_DIV_1             = 0x00,
    CLOCK_ADC_DIV_2             = 0x01,
    CLOCK_ADC_DIV_3             = 0x02,
    CLOCK_ADC_DIV_4             = 0x03,
    CLOCK_ADC_DIV_5             = 0x04,
    CLOCK_ADC_DIV_6             = 0x05,
    CLOCK_ADC_DIV_7             = 0x06,
    CLOCK_ADC_DIV_8             = 0x07,
    CLOCK_ADC_DIV_9             = 0x08,
    CLOCK_ADC_DIV_10            = 0x09,
    CLOCK_ADC_DIV_11            = 0x0A,
    CLOCK_ADC_DIV_12            = 0x0B,
    CLOCK_ADC_DIV_13            = 0x0C,
    CLOCK_ADC_DIV_14            = 0x0D,
    CLOCK_ADC_DIV_15            = 0x0E,
    CLOCK_ADC_DIV_16            = 0x0F,
} PeripheClk_ADC_prescaler_T;

typedef enum
{
    CLOCK_ADC_SEL_HIRC          = 0x00,
    CLOCK_ADC_SEL_PLLI2S        = 0x01,
    CLOCK_ADC_SEL_PLL           = 0x02,
} PeripheClk_ADC_sel_T;

typedef struct
{
    PeripheClk_ADC_prescaler_T  adcPrescaler;
    PeripheClk_ADC_sel_T        adcSourceClk;
} PeripheClk_ADC_Config_T;

/******************************DAC has no prescaler*******************************************/
typedef enum
{
    CLOCK_DAC_SEL_LOSC          = 0x00,
    CLOCK_DAC_SEL_LIRC          = 0x01,
} PeripheClk_DAC_Config_T;

/******************************SDMMC has no prescaler*****************************************/
typedef enum
{
    CLOCK_SDMMC_SEL_SYSCLK      = 0x00,
    CLOCK_SDMMC_SEL_PLLI2S      = 0x01,
} PeripheClk_SDMMC_Config_T;


/******************************VOUT clock is only sourced by PLL 50~54MHz*********************/
typedef enum
{
    CLOCK_VOUT_DIV_1            = 0x00,
    CLOCK_VOUT_DIV_2            = 0x01,
    CLOCK_VOUT_DIV_3            = 0x02,
    CLOCK_VOUT_DIV_4            = 0x03,
    CLOCK_VOUT_DIV_5            = 0x04,
    CLOCK_VOUT_DIV_6            = 0x05,
    CLOCK_VOUT_DIV_7            = 0x06,
    CLOCK_VOUT_DIV_8            = 0x07,
} PeripheClk_VOUT_Config_T;

/******************************I2C is only sourced by system clock****************************/
typedef enum
{
    CLOCK_I2C_DIV_1             = 0x00,
    CLOCK_I2C_DIV_2             = 0x01,
    CLOCK_I2C_DIV_4             = 0x03,
    CLOCK_I2C_DIV_8             = 0x07,
    CLOCK_I2C_DIV_16            = 0x0F,
    CLOCK_I2C_DIV_32            = 0x1F,
    CLOCK_I2C_DIV_64            = 0x3F,
    CLOCK_I2C_DIV_128           = 0x7F,
    CLOCK_I2C_DIV_256           = 0xFF,
} PeripheClk_I2C_Config_T;

/******************************BTM is only sourced by system clock****************************/
typedef enum
{
    CLOCK_BTM_DIV_1             = 0x000,
    CLOCK_BTM_DIV_2             = 0x001,
    CLOCK_BTM_DIV_4             = 0x003,
    CLOCK_BTM_DIV_8             = 0x007,
    CLOCK_BTM_DIV_16            = 0x00F,
    CLOCK_BTM_DIV_32            = 0x01F,
    CLOCK_BTM_DIV_64            = 0x03F,
    CLOCK_BTM_DIV_128           = 0x07F,
    CLOCK_BTM_DIV_256           = 0x0FF,
    CLOCK_BTM_DIV_512           = 0x1FF,
} PeripheClk_BTM_Config_T;

/******************************SAI has no prescaler*******************************************/
typedef enum
{
    CLOCK_SAI_SEL_PLLSAI        = 0x00,
    CLOCK_SAI_SEL_PLLI2S        = 0x01,
    CLOCK_SAI_SEL_IO            = 0x02,
} PeripheClk_SAI_Config_T;

/******************************UART clock configuration***************************************/
typedef enum
{
    CLOCK_UART_DIV_1            = 0x00,
    CLOCK_UART_DIV_2            = 0x01,
    CLOCK_UART_DIV_4            = 0x03,
    CLOCK_UART_DIV_8            = 0x07,
    CLOCK_UART_DIV_16           = 0x0F,
    CLOCK_UART_DIV_32           = 0x1F,
} PeripheClk_UART_prescaler_T;

typedef enum
{
    CLOCK_UART_SEL_HIRC         = 0x0,
    CLOCK_UART_SEL_PLLI2S       = 0x1,
} PeripheClk_UART_sel_T;

typedef struct
{
    PeripheClk_UART_prescaler_T  uartPrescaler;
    PeripheClk_UART_sel_T        uartSourceClk;
} PeripheClk_UART_Config_T;

/******************************TIM is only sourced by system clock****************************/
typedef enum
{
    CLOCK_TIM_DIV_1             = 0x0,
    CLOCK_TIM_DIV_2             = 0x1,
    CLOCK_TIM_DIV_4             = 0x2,
    CLOCK_TIM_DIV_8             = 0x3,
} PeripheClk_TIM_Config_T;


/**
  * typedef union of peripheral
  *
  */
typedef union
{
    PeripheClk_SDRAM_Config_T       sdramClkConfig;
    PeripheClk_WWDT_Config_T        wwdtClkConfig;
    PeripheClk_LPSTM_Config_T       lpstmClkConfig;
    PeripheClk_MCO1_Config_T        mco1ClkConfig;
    PeripheClk_MCO2_Config_T        mco2ClkConfig;
    PeripheClk_RTC_Config_T         rtcClkConfig;
    PeripheClk_KDP_Config_T         kdpClkConfig;
    PeripheClk_ADC_Config_T         adcClkConfig;
    PeripheClk_DAC_Config_T         dacClkConfig;
    PeripheClk_SDMMC_Config_T       sdmmcClkConfig;
    PeripheClk_VOUT_Config_T        voutClkConfig;
    PeripheClk_I2C_Config_T         i2cClkConfig;
    PeripheClk_BTM_Config_T         btmClkConfig;
    PeripheClk_SAI_Config_T         saiClkConfig;
    PeripheClk_UART_Config_T        uartClkConfig;
    PeripheClk_TIM_Config_T         timClkConfig;
} PeripheClk_Config_T;



typedef struct
{
    CLOCK_PeripheMask_T     peripheMask;     //Detetmine which peripheral clock will be set by this param
    PeripheClk_Config_T     peripheClkConfig;    //The param which is used to set peripheral
} Peripheral_Config_T;

/**
  * Peripheral source clock select and prescaler set end
  */



/**
  * @brief  Reset the clock configuration to the default state.
  * @Note   The default reset state of the clock configuration is given below:
  *             - HIRC ON and used as system clock source, so system clock is set to 8MHz
  *             - HOSC, PLL, PLLKDP, PLLI2S, PLLSAI OFF
  *             - System clock prescaler set to 1
  *             - Disable HOSC monitor
  *             - Clear HOSC loss event flag
  *         This function doesn't modify the configuration of the
  *             - Peripheral clocks
  *             - LIRC, LOSC clock
  * @param  None
  * @retval None
  */
void        HAL_CLOCK_DeInit(void);

/**
  * @brief  This function is used to enable or disable clock
  * @param  clockMask: the specified clock to be enabled or disabled
  *         This param can be the following values:
  *             @arg CLOCK_HIRC_MASK
  *             @arg CLOCK_HOSC_MASK
  *             @arg CLOCK_LIRC_MASK
  *             @arg CLOCK_LOSC_MASK
  *             @arg CLOCK_PLLKDP_MASK
  *             @arg CLOCK_PLL_MASK
  *             @arg CLOCK_PLLI2S_MASK
  *             @arg CLOCK_PLLSAI_MASK
  *         newState: new state of the clock.
  *         This parameter can be: ENBALE or DISABLE.
  *
  * @retval None
  */
void        HAL_CLOCK_ClockCmd(CLOCK_Sel_T clockMask, uint8_t newState);


/**
  * @brief  Adjust PLL input clock divider M.
  * @note   Be careful when calling this function. In general, should not modify this param
  *
  * @param  divm: PLL input divider M selection, default value: 4b'0100
  *         this param can be 2~15
  *
  * @retval None
  */
void        HAL_CLOCK_AdjustPLLsDefaultDivM(uint8_t divm);


/**
  * @brief  Adjust PLL output clock divider selection P, Q, R, N.
  * @note   Be careful when calling this function. In general, should not modify these four params
  *
  * @param  divp: PLL output divider P selection, default value: 8b'00000010
  *         divq: PLL output divider Q selection, default value: 8b'00000010
  *         divr: PLL output divider R selection, default value: 8b'00000010
  *         divn: PLL output divider N selection, default value: 8b'00000010
  *         these four param can be 2~255
  *
  * @Note   if you just want to change values of some parameters, you should pass the values you
  *         desired and pass 0 to the other parameters, so the other parameters will remain the
  *         original value.
  *
  *         For Example: you want to change the value of divq and divn, you should pass the value
  *         of divq and divn, setting divp and divr to 0. You can call this function as following:
  *
  *         HAL_CLOCK_AdjustPLLKDPDefaultDiv(0, divq_value, 0, divn_value);
  *
  * @retval None
  */

void        HAL_CLOCK_AdjustPLLDefaultDiv(uint8_t divp, uint8_t divq, uint8_t divr, uint8_t divn);


/**
  * @brief  Adjust PLLKDP output clock divider selection P, Q, R, N.
  * @note   Be careful when calling this function. In general, should not modify these four params
  *
  * @param  divp: PLLKDP output divider P selection, default value: 8b'00000010
  *         divq: PLLKDP output divider Q selection, default value: 8b'00000100
  *         divr: PLLKDP output divider R selection, default value: 8b'00000110
  *         divn: PLLKDP output divider N selection, default value: 9b'110010000
  *         param divp, divq, divr can be 2~255
  *         param divn can be 2~511
  *
  * @Note   if you just want to change values of some parameters, you should pass the values you
  *         desired and pass 0 to the other parameters, so the other parameters will remain the
  *         original value.
  *
  *         For Example: you want to change the value of divq and divn, you should pass the value
  *         of divq and divn, setting divp and divr to 0. You can call this function as following:
  *
  *         HAL_CLOCK_AdjustPLLKDPDefaultDiv(0, divq_value, 0, divn_value);
  *
  * @retval None
  */

void        HAL_CLOCK_AdjustPLLKDPDefaultDiv(uint8_t divp, uint8_t divq, uint8_t divr, uint32_t divn);


/**
  * @brief  Adjust PLLI2S output clock divider selection P, Q, R, N.
  * @note   Be careful when calling this function. In general, should not modify these four params
  *
  * @param  divp: PLLI2S output divider P selection, default value: 8b'00000010
  *         divq: PLLI2S output divider Q selection, default value: 8b'00000010
  *         divr: PLLI2S output divider R selection, default value: 8b'00001000
  *         divn: PLLI2S output divider N selection, default value: 8b'11000000
  *         these four params can be 2~255
  *
  * @Note   if you just want to change values of some parameters, you should pass the values you
  *         desired and pass 0 to the other parameters, so the other parameters will remain the
  *         original value.
  *
  *         For Example: you want to change the value of divq and divn, you should pass the value
  *         of divq and divn, setting divp and divr to 0. You can call this function as following:
  *
  *         HAL_CLOCK_AdjustPLLI2SDefaultDiv(0, divq_value, 0, divn_value);
  *
  * @retval None
  */

void        HAL_CLOCK_AdjustPLLI2SDefaultDiv(uint8_t divp, uint8_t divq, uint8_t divr, uint8_t divn);

/**
  * @brief  Adjust PLLSAI output clock divider selection P, Q, R, N.
  * @note   Be careful when calling this function. In general, should not modify these four params
  *
  * @param  divp: PLLSAI output divider P selection, default value: 8b'00000111
  *         divq: PLLSAI output divider Q selection, default value: 8b'00001000
  *         divr: PLLSAI output divider R selection, default value: 8b'00001001
  *         divn: PLLSAI output divider N selection, default value: 8b'10101100
  *         these four params can be 2~255
  *
  * @Note   if you just want to change values of some parameters, you should pass the values you
  *         desired and pass 0 to the other parameters, so the other parameters will remain the
  *         original value.
  *
  *         For Example: you want to change the value of divq and divn, you should pass the value
  *         of divq and divn, setting divp and divr to 0. You can call this function as following:
  *
  *         HAL_CLOCK_AdjustPLLSAIDefaultDiv(0, divq_value, 0, divn_value);
  *
  * @retval None
  */

void        HAL_CLOCK_AdjustPLLSAIDefaultDiv(uint8_t divp, uint8_t divq, uint8_t divr, uint8_t divn);


/**
  * @brief  This function is used to check whether the specified clock is stable or not
  * @param  clkStableFlag: the specified clock stable flag
  *         this param can be:
  *             @arg SYS_CMUST_HIRC_STABLE
  *             @arg SYS_CMUST_LIRC_STABLE
  *             @arg SYS_CMUST_HOSC_STABLE
  *             @arg SYS_CMUST_LOSC_STABLE
  *             @arg SYS_CMUST_PLL_STABLE
  *             @arg SYS_CMUST_PLLKDP_STABLE
  *             @arg SYS_CMUST_PLLI2S_STABLE
  *             @arg SYS_CMUST_PLLSAI_STABLE
  *
  * @retval 0: the specified clock is not stable
  *         non-zero: the specified is stable;
  */
uint32_t    HAL_CLOCK_WaitForStable(uint32_t clkStableFlag);

/**
  * @brief  This function is used to select PLL reference clock
  * @note   the reference clock must be stable at first, then can be chosen by PLL
  *         if the reference clock is disabled, should enable it.
  * @param  refClk: the reference clock that chosen by PLL
  *         this param can be:
  *             @arg PLL_IRC_HSCLK: PLL reference clock is drived from HIRC
  *             @arg PLL_OSC_HSCLK: PLL reference clock is drived from HOSC
  * @retval None
  */
void        HAL_CLOCK_PLLsReferenceClock(CLOCK_PLLRefClk_T refClk);

/**
  * @brief  This function is used to configure system clock
  * @note   system clock can be drived from:
  *             -SYSCLOCK_SOURCE_HIRC8M
  *             -SYSCLOCK_SOURCE_HOSC8M
  *             -SYSCLOCK_SOURCE_PLLKDP150
  *             -SYSCLOCK_SOURCE_PLL
  *             -SYSCLOCK_SOURCE_PLLKDP100
  *
  * @retval None
  */
void        HAL_CLOCK_SYSCLKConfig(CLOCK_SysClockConfig_T *SysClockConfigStructure);

/**
  * @brief  Returns the clock source used as system clock.
  *
  * @param  None
  *
  * @retval The clock source used as system clock. The returned value can be
  *         one of the following:
  *             -0x00:  HIRC used as system clock
  *             -0x01:  HOSC used as system clock
  *             -0x02:  PLLKDP150 used as system clock
  *             -0x03:  PLL used as system clock
  *             -0x04:  PLLKDP100 used as system clock
  */
void        HAL_CLOCK_GetSYSCLKConfig(CLOCK_SysClockConfig_T *getSysClock);

/**
  * @brief  Returns the system clock frequency.
  *
  * @param  None
  *
  * @retval The system clock frequency
  *
  */
uint32_t    HAL_CLOCK_GetSYSCLKFreq(void);

/**
  * @brief  The following two functions are used to enable/disable peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application
  *         software has to enable clock before using peripheral
  * @param  ENABLECLK0Peripheral: specifies the peripheral in enable clock register0
  *         This parameter can be any combination of the following values:
  *         @arg SYS_ENCLK0_SM4
  *         @arg SYS_ENCLK0_GPIO
  *         @arg SYS_ENCLK0_DMA
  *         @arg SYS_ENCLK0_CRC
  *         @arg SYS_ENCLK0_TRNG
  *         @arg SYS_ENCLK0_CRYPTO
  *         @arg SYS_ENCLK0_FMC
  *         @arg SYS_ENCLK0_SDR
  *         @arg SYS_ENCLK0_RTC
  *         @arg SYS_ENCLK0_LPSTM
  *         @arg SYS_ENCLK0_ACMP
  *         @arg SYS_ENCLK0_DAC
  *         @arg SYS_ENCLK0_ADC
  *         @arg SYS_ENCLK0_STF
  *         @arg SYS_ENCLK0_KDPSRAM
  *         @arg SYS_ENCLK0_KDP
  *         @arg SYS_ENCLK0_IMP
  *         @arg SYS_ENCLK0_VIN
  *         @arg SYS_ENCLK0_VOUT
  *         @arg SYS_ENCLK0_USB
  *         @arg SYS_ENCLK0_SDMMC
  *         @arg SYS_ENCLK0_CAN1
  *         @arg SYS_ENCLK0_CAN2
  *
  * @retval None
  *
  */
void        HAL_CLOCK_PeripheralClockEnable0(uint32_t ENABLECLK0Peripheral);
void        HAL_CLOCK_PeripheralClockDisable0(uint32_t ENABLECLK0Peripheral);

/**
  * @brief  The following two functions are used to enable/disable peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application software has to
  *         enable clock before using the peripheral
  * @param  priClkGate: specifies the peripheral in enable clock register1
  *         This parameter can be any combination of the following values:
  *         @arg SYS_ENCLK1_SPI1
  *         @arg SYS_ENCLK1_SPI2
  *         @arg SYS_ENCLK1_SPI3
  *         @arg SYS_ENCLK1_SPI4
  *         @arg SYS_ENCLK1_SPI5
  *         @arg SYS_ENCLK1_SPI6
  *         @arg SYS_ENCLK1_I2C1
  *         @arg SYS_ENCLK1_I2C2
  *         @arg SYS_ENCLK1_I2C3
  *         @arg SYS_ENCLK1_BTM1
  *         @arg SYS_ENCLK1_BTM2
  *         @arg SYS_ENCLK1_SAIA
  *         @arg SYS_ENCLK1_SAIB
  *         @arg SYS_ENCLK1_PMEM
  *
  * @param  NewState: new state of the specified peripheral clock
  *         This parameter can be : ENABLE or DISABLE
  *
  * @retval None
  *
  */
void        HAL_CLOCK_PeripheralClockEnable1(uint32_t ENABLECLK1Peripheral);
void        HAL_CLOCK_PeripheralClockDisable1(uint32_t ENABLECLK1Peripheral);

/**
  * @brief  The following two functions are used to enable/disable peripheral clock.
  * @note   After reset, the peripheral clock is disabled and the application software has to
  *         enable clock before using it
  * @param  priClkGate: specifies the peripheral in enable clock register2
  *         This parameter can be any combination of the following values:
  *         @arg SYS_ENCLK2_UART1:
  *         @arg SYS_ENCLK2_UART2:
  *         @arg SYS_ENCLK2_UART3:
  *         @arg SYS_ENCLK2_UART4:
  *         @arg SYS_ENCLK2_UART5:
  *         @arg SYS_ENCLK2_UART6:
  *         @arg SYS_ENCLK2_UART7
  *         @arg SYS_ENCLK2_UART8
  *         @arg SYS_ENCLK2_ETM1
  *         @arg SYS_ENCLK2_ETM2
  *         @arg SYS_ENCLK2_ETM3
  *         @arg SYS_ENCLK2_ETM4
  *         @arg SYS_ENCLK2_ETM5
  *         @arg SYS_ENCLK2_ETM6
  *         @arg SYS_ENCLK2_ETM7
  *         @arg SYS_ENCLK2_ETM8
  *         @arg SYS_ENCLK2_ETM9
  *         @arg SYS_ENCLK2_ETM10
  *         @arg SYS_ENCLK2_ETM11
  *         @arg SYS_ENCLK2_ETM12
  *         @arg SYS_ENCLK2_WWDT
  *
  * @param  NewState: new state of the specified peripheral clock
  *         This parameter can be : ENABLE or DISABLE
  *
  * @retval None
  *
  */
void        HAL_CLOCK_PeripheralClockEnable2(uint32_t ENABLECLK2Peripheral);
void        HAL_CLOCK_PeripheralClockDisable2(uint32_t ENABLECLK2Peripheral);

/**
  * @brief  This function is used to select peripheral source clock and prescaler
  * @note   Peripheral_Config_T is struct type, it has two member:
  *             -peripheMask is a flag to indicate which peripheral is to be set.
  *              this param can be:
  *                     -PERIPHERAL_SDRAM_MASK
  *                     -PERIPHERAL_WWDT_MASK
  *                     -PERIPHERAL_LPSTM_MASK
  *                     -PERIPHERAL_MCO1_MASK
  *                     -PERIPHERAL_MCO2_MASK
  *                     -PERIPHERAL_RTC_MASK
  *                     -PERIPHERAL_KDP_MASK
  *                     -PERIPHERAL_ADC_MASK
  *                     -PERIPHERAL_DAC_MASK
  *                     -PERIPHERAL_SDMMC_MASK
  *                     -PERIPHERAL_VOUT_MASK
  *                     -PERIPHERAL_I2C1_MASK
  *                     -PERIPHERAL_I2C2_MASK
  *                     -PERIPHERAL_I2C3_MASK
  *                     -PERIPHERAL_BTM1_MASK
  *                     -PERIPHERAL_BTM2_MASK
  *                     -PERIPHERAL_SAIA_MASK
  *                     -PERIPHERAL_SAIB_MASK
  *                     -PERIPHERAL_UART1_MASK
  *                     -PERIPHERAL_UART2_MASK
  *                     -PERIPHERAL_UART3_MASK
  *                     -PERIPHERAL_UART4_MASK
  *                     -PERIPHERAL_UART5_MASK
  *                     -PERIPHERAL_UART6_MASK
  *                     -PERIPHERAL_UART7_MASK
  *                     -PERIPHERAL_UART8_MASK
  *                     -PERIPHERAL_TIM1_MASK
  *                     -PERIPHERAL_TIM2_MASK
  *                     -PERIPHERAL_TIM3_MASK
  *                     -PERIPHERAL_TIM4_MASK
  *                     -PERIPHERAL_TIM5_MASK
  *                     -PERIPHERAL_TIM8_MASK
  *                     -PERIPHERAL_TIM9_MASK
  *                     -PERIPHERAL_TIM10_MASK
  *                     -PERIPHERAL_TIM11_MASK
  *                     -PERIPHERAL_TIM12_MASK
  *                     -PERIPHERAL_TIM13_MASK
  *                     -PERIPHERAL_TIM14_MASK
  *             -peripheClockSet is a union type variable, you can configure the
  *              specified peripheral clock through this param as followings:
  *                     -SDRAM: can only configure prescaler
  *                     -WWDT:  can only configure prescaler
  *                     -LPSTM: can only configure source clock
  *                     -MCO1:  can configure source clock and prescaler
  *                     -MCO2:  can configure source clock and prescaler
  *                     -RTC:   can configure source clock and prescaler
  *                     -KDP:   can only configure source clock
  *                     -ADC:   can configure source clock and prescaler
  *                     -DAC:   can only configure source clock
  *                     -SDMMC: can only configure source clock
  *                     -VOUT:  can only configure prescaler
  *                     -I2C:   can only configure prescaler
  *                     -BTM:   can only configure prescaler
  *                     -SAI:   can only configure source clock
  *                     -UART:  can configure source clock and prescaler
  *                     -ETM:   can only configure prescaler
  * @retval  None
  *
  */
void        HAL_CLOCK_PeripheralClkSetConfig(Peripheral_Config_T *peripheClockSet);

#ifdef __cplusplus
}
#endif

#endif
