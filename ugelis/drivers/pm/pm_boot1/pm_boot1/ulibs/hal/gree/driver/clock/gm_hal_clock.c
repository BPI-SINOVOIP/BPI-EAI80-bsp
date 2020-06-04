/**
 *
 * Copyright (C) 2018 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          clock.c
 *
 * @author        bert.Ding
 *
 * @version       1.0.0
 *
 * @date          2018/05/18
 *
 * @brief         Implementation of Clock Driver.
 *
 * @note
 *
 */

#include <gm_hal_clock.h>

#define ASSERT(expr)            do{}while(0)

#define HOSC_VALUE              ((uint32_t)8000000)
#define HIRC_VALUE              ((uint32_t)8000000)

#define IS_CLOCK_STATE(VALUE)   ((VALUE == ENABLE) || (VALUE == DISABLE))


static void HAL_CLOCK_HIRCcmd(uint8_t newState);
static void HAL_CLOCK_LIRCcmd(uint8_t newState);
static void HAL_CLOCK_HOSCcmd(uint8_t newState);
static void HAL_CLOCK_LOSCcmd(uint8_t newState);
static void HAL_CLOCK_PLLcmd(uint8_t newState);
static void HAL_CLOCK_PLLKDPcmd(uint8_t newState);
static void HAL_CLOCK_PLLI2Scmd(uint8_t newState);
static void HAL_CLOCK_PLLSAIcmd(uint8_t newState);

/**
  * @brief  Reset the clock configuration to the default state.
  * @Note   The default reset state of the clock configuration is given below:
  *             - HIRC ON and used as system clock source, so system clock is set to 8MHz
  *             - HOSC, PLL, PLLKDP, PLLI2S, PLLSAI OFF
  *             - System clock prescaler set to 1
  *             - Disable HOSC monitor
  *             - Clear HOSC loss event flag
  * @note   This function doesn't modify the configuration of the
  *             - Peripheral clocks
  *             - LIRC, LOSC clock
  * @param  None
  * @retval None
  */
void HAL_CLOCK_DeInit(void)
{
    /* switch FCLK to HIRC */
    CLOCK_SysClockConfig_T sysClock;
    sysClock.sysClkSource = SYSCLOCK_SOURCE_HIRC8M;
    sysClock.sysClkPrescaler = CLOCK_DIVIDED_1;
    HAL_CLOCK_HIRCcmd(ENABLE);
    while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
    HAL_CLOCK_SYSCLKConfig(&sysClock);
    /* switch off all other clock source */
    HAL_CLOCK_HOSCcmd(DISABLE);
    HAL_CLOCK_LOSCcmd(DISABLE);
    HAL_CLOCK_PLLcmd(DISABLE);
    HAL_CLOCK_PLLKDPcmd(DISABLE);
    HAL_CLOCK_PLLI2Scmd(DISABLE);
    HAL_CLOCK_PLLSAIcmd(DISABLE);
}


/**
  * @brief  Enable or disable the Internal High Speed oscillator(HIRC).
  * @note   The HIRC is used(enabled by hardware) as system clock source after startup
  *         from Reset, or in case of failure of the HOSC used directly or indirectly
  *         as system clock.
  * @note   HIRC can not be stopped if it is used directly or indirectly as system clock
  *         source. In this case, you have to select another source of the system clock
  *         then stop the HIRC.
  * @note   After enabling the HIRC, the application software should wait on HIRC STABLE
  *         flag to be set indicating that HIRC clock is stable and can be used as system
  *         clock source.
  * @param  newState: new state of the HIRC.
  *         This parameter can be: ENBALE or DISABLE.
  * @retval None
  *
  */

static void HAL_CLOCK_HIRCcmd(uint8_t newState)
{
    /* Check the parameter is valid or not */
    ASSERT(IS_CLOCK_STATE(newState));
    /* switch on or off HIRC */
    LL_HIRC_ENABLE_DISABLE(newState);
}

/**
  * @brief  Adjust LIRC trimming value
  * @note   This type of functions ahead of HAL_CLOCK_Adjust** is used to adjust the configuration
  *         value of clock.In generally, the parameters modified by these functions have default
  *         value and should not modify in most of case. So, be careful when calling these functions.
  * @param  tmrValue: accurate trimming value
  *         tmcValue: coarse trimming value
  * @retval None
  *
  */
#if 0
static void HAL_CLOCK_AdjustLIRCTrimValue(uint8_t tmrValue, uint8_t tmcValue)
{
    /* Check the parameters */
    ASSERT(IS_LIRCCR_TMC_VALUE(tmcValue));
    ASSERT(IS_LIRCCR_TMR_VALUE(tmrValue));

    LL_ADJUST_LIRC_TMR(tmrValue);
    LL_ADJUST_LIRC_TMC(tmcValue);
}
#endif

/**
  * @brief  Enable or disable the Internal Low Speed oscillator(LIRC).
  * @note   After enabling the LIRC, the application software should wait on LIRC STABLE
  *         flag to be set indicating that LIRC clock is stable.
  * @param  newState: new state of the LIRC.
  *         This parameter can be: ENBALE or DISABLE.
  * @retval None
  *
  */

static void HAL_CLOCK_LIRCcmd(uint8_t newState)
{
    ASSERT(IS_CLOCK_STATE(newState));
    /* switch on or off LIRC */
    LL_LIRC_ENABLE_DISABLE(newState);
}

/**
  * @brief  Adjust HOSC loading cap and crystal value
  * @note   Be careful when calling this function. In general, should not modify these two param.
  * @param  capSelect: Select the loading cap for HOSC, default value: 4b'1000
  *         crystalCoreSel: Select crystal core, default value: 2b'00
  *         this param can be: 2b'00 4~8MHz
  *                            2b'01 8~14MHz
  *                            2b'10 14~20MHz
  *                            2b'11 20~26MHz
  * @retval None
  *
  */
#if 0
static void HAL_CLOCK_AdjustHOSCDefaultConfig(uint8_t capSelect, uint8_t crystalCoreSel)
{
    /* Check the parameters */
    ASSERT(IS_HOSCCR_CS_VALUE(crystalCoreSel));
    ASSERT(IS_HOSCCR_CAPSEL_VALUE(capSelect));

    LL_ADJUST_HOSC_CAP_SEL(capSelect);
    LL_ADJUST_HOSC_CRYSTAL(crystalCoreSel);
}
#endif

/**
  * @brief  Enable or disable the External High Speed oscillator(HOSC).
  * @note   HOSC can not be stopped if it is used directly or indirectly as system clock
  *         source. In this case, you have to select another source of the system clock
  *         then stop the HOSC.
  * @note   After enabling the HOSC, the application software should wait on HOSC STABLE
  *         flag to be set indicating that HOSC clock is stable and can be used as system
  *         clock source.
  * @param  newState: new state of the HOSC.
  *         This parameter can be: ENBALE or DISABLE.
  * @retval None
  *
  */

static void HAL_CLOCK_HOSCcmd(uint8_t newState)
{
    ASSERT(IS_CLOCK_STATE(newState));

    if (newState == ENABLE)
    {
        LL_HOSC_IN_PINMUX_SET;
        LL_HOSC_OUT_PINMUX_SET;
        *(volatile uint32_t *)0x40000178 |= (0x3 << 4);
        *(volatile uint32_t *)0x40000178 &= ~(0xF << 8);
        LL_HOSC_ENABLE_DISABLE(ENABLE);
    }
    else
    {
        LL_HOSC_IN_PINMUX_CLEAR;
        LL_HOSC_OUT_PINMUX_CLEAR;
        LL_HOSC_ENABLE_DISABLE(DISABLE);
    }
}

/**
  * @brief  Adjust LOSC loading cap and bias current
  * @note   Be careful when calling this function. In general, should not modify these two param.
  * @param  capSelect: Select the loading cap for LOSC, default value: 4b'1000
  *         crystalCoreSel: Select LOSC bias current, default value: 3b'000
  *         this param can be:
  *
  *
  * @retval None
  *
  */
#if 0
static void HAL_CLOCK_AdjustLOSCDefaultConfig(uint8_t capSelect, uint8_t biasCurrentSel)
{
    /* Check the parameters */
    ASSERT(IS_LOSCCR_CAPSEL_VALUE(capSelect));
    ASSERT(IS_LOSCCR_ISEL_VALUE(biasCurrentSel));

    LL_ADJUST_LOSC_CAP_SEL(capSelect);
    LL_ADJUST_LOSC_BIAS_CURRENT(biasCurrentSel);
}
#endif

/**
  * @brief  Enable or disable the External Low Speed oscillator(LOSC).
  * @note   After enabling the LOSC, the application software should wait on LOSC STABLE
  *         flag to be set indicating that LOSC clock is stable.
  * @param  newState: new state of the LOSC.
  *         This parameter can be: ENBALE or DISABLE.
  * @retval None
  *
  */

static void HAL_CLOCK_LOSCcmd(uint8_t newState)
{
    ASSERT(IS_CLOCK_STATE(newState));

    if (newState == ENABLE)
    {
        LL_LOSC_IN_PINMUX_SET;
        LL_LOSC_OUT_PINMUX_SET;
        LL_LOSC_ENABLE_DISABLE(ENABLE);
    }
    else
    {
        LL_LOSC_IN_PINMUX_CLEAR;
        LL_LOSC_OUT_PINMUX_CLEAR;
        LL_LOSC_ENABLE_DISABLE(DISABLE);
    }
}

/**
  * @brief  Adjust PLL vco configuration and charge pump current selection
  * @note   Be careful when calling this function. In general, should not modify these two param
  *
  * @param  vco:    PLL vco control
  *         CPCSel: charge pump current selection
  *
  * @retval None
  */
#if 0
static void HAL_CLOCK_AdjustPLLDefaultConfig(uint8_t vco, uint8_t CPCSel)
{
    /* Check the parameters */
    ASSERT(IS_PLL_VCO_VALUE(vco));
    ASSERT(IS_PLL_CPC_VALUE(CPCSel));

    LL_ADJUST_PLL_VCO(vco);
    LL_ADJUST_PLL_CPC(CPCSel);
}
#endif

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

void HAL_CLOCK_AdjustPLLDefaultDiv(uint8_t divp, uint8_t divq, uint8_t divr, uint8_t divn)
{
    /* Check parametes */
    ASSERT(IS_PLL_DIVP_VALUE(divp));
    ASSERT(IS_PLL_DIVQ_VALUE(divq));
    ASSERT(IS_PLL_DIVR_VALUE(divr));
    ASSERT(IS_PLL_DIVN_VALUE(divn));

    if (divp != 0)
    {
        LL_PLL_CONFIG_DIVP(divp);
    }
    if (divq != 0)
    {
        LL_PLL_CONFIG_DIVQ(divq);
    }
    if (divr != 0)
    {
        LL_PLL_CONFIG_DIVR(divr);
    }
    if (divn != 0)
    {
        LL_PLL_CONFIG_DIVN(divn);
    }
}

/**
  * @brief  Adjust PLL input clock divider M.
  * @note   Be careful when calling this function. In general, should not modify this param
  *
  * @param  divm: PLL input divider M selection, default value: 4b'0100
  *         this param can be 2~15
  *
  * @retval None
  */


void HAL_CLOCK_AdjustPLLsDefaultDivM(uint8_t divm)
{
    ASSERT(IS_PLLREF_DIVM_VALUE(divm));

    LL_PLLs_CONFIG_DIVM(divm);
}

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

void HAL_CLOCK_PLLsReferenceClock(CLOCK_PLLRefClk_T refClk)
{
    /* Check the param is valid or not */
    ASSERT(IS_PLL_REFCLK(refClk));

    if (refClk == PLL_IRC_HSCLK)
    {
        if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
        {
            HAL_CLOCK_HIRCcmd(ENABLE);
            while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
        }
    }
    else
    {
        if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE))
        {
            HAL_CLOCK_HOSCcmd(ENABLE);
            while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE));
        }
    }
    /*Configure PLL reference clock*/
    LL_PLLs_REFERENCE_CLOCK(refClk);
}

/**
  * @brief  Enable or disable the PLL clock.
  * @note   After enabling the PLL, the application software should wait on PLL STABLE
  *         flag to be set indicating that PLL clock is stable.
  * @param  newState: new state of the PLL.
  *         This parameter can be: ENBALE or DISABLE.
  * @retval None
  *
  */


static void HAL_CLOCK_PLLcmd(uint8_t newState)
{
    ASSERT(IS_CLOCK_STATE(newState));
    /* switch on or off PLL */
    LL_PLL_ENABLE_DISABLE(newState);
}

/**
  * @brief  Adjust PLLKDP vco configuration and charge pump current selection
  * @note   Be careful when calling this function. In general, should not modify these two param
  *
  * @param  vco:    PLLKDP vco control
  *         CPCSel: charge pump current selection
  *
  * @retval None
  */


static void HAL_CLOCK_AdjustPLLKDPDefaultConfig(uint8_t vco, uint8_t CPCSel)
{
    ASSERT(IS_PLLKDP_VCO_VALUE(vco));
    ASSERT(IS_PLLKDP_CPC_VALUE(CPCSel));

    LL_ADJUST_PLLKDP_VCO(vco);
    LL_ADJUST_PLLKDP_CPC(CPCSel);
}

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

void HAL_CLOCK_AdjustPLLKDPDefaultDiv(uint8_t divp, uint8_t divq, uint8_t divr, uint32_t divn)
{
    ASSERT(IS_PLLKDP_DIVP_VALUE(divp));
    ASSERT(IS_PLLKDP_DIVQ_VALUE(divq));
    ASSERT(IS_PLLKDP_DIVR_VALUE(divr));
    ASSERT(IS_PLLKDP_DIVN_VALUE(divn));

    if (divp != 0)
    {
        LL_PLLKDP_CONFIG_DIVP(divp);
    }
    if (divq != 0)
    {
        LL_PLLKDP_CONFIG_DIVQ(divq);
    }
    if (divr != 0)
    {
        LL_PLLKDP_CONFIG_DIVR(divr);
    }
    if (divn != 0)
    {
        LL_PLLKDP_CONFIG_DIVN(divn);
    }
}

/**
  * @brief  Enable or disable the PLLKDP clock.
  * @note   After enabling the PLLKDP, the application software should wait on PLLKDP STABLE
  *         flag to be set indicating that PLLKDP clock is stable.
  * @param  newState: new state of the PLLKDP.
  *         This parameter can be: ENBALE or DISABLE.
  * @retval None
  *
  */

static void HAL_CLOCK_PLLKDPcmd(uint8_t newState)
{
    ASSERT(IS_CLOCK_STATE(newState));
    /* switch on or off PLLKDP */
    LL_PLLKDP_ENABLE_DISABLE(newState);
}

/**
  * @brief  Adjust PLLKI2S vco configuration and charge pump current selection
  * @note   Be careful when calling this function. In general, should not modify these two param
  *
  * @param  vco:    PLLI2S vco control
  *         CPCSel: charge pump current selection
  *
  * @retval None
  */
#if 0
static void HAL_CLOCK_AdjustPLLI2SDefaultConfig(uint8_t vco, uint8_t CPCSel)
{
    ASSERT(IS_PLLI2S_VCO_VALUE(vco));
    ASSERT(IS_PLLI2S_CPC_VALUE(CPCSel));

    LL_ADJUST_PLLI2S_VCO(vco);
    LL_ADJUST_PLLI2S_CPC(CPCSel);
}
#endif

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

void HAL_CLOCK_AdjustPLLI2SDefaultDiv(uint8_t divp, uint8_t divq, uint8_t divr, uint8_t divn)
{
    ASSERT(IS_PLLI2S_DIVP_VALUE(divp));
    ASSERT(IS_PLLI2S_DIVQ_VALUE(divq));
    ASSERT(IS_PLLI2S_DIVR_VALUE(divr));
    ASSERT(IS_PLLI2S_DIVN_VALUE(divn));

    SYS_DEV->PLLI2S[1] = (divn << SYS_PLLI2S_DIVN_POS) | (divr << SYS_PLLI2S_DIVR_POS) |
                         (divq << SYS_PLLI2S_DIVQ_POS) | (divp << SYS_PLLI2S_DIVP_POS);
}

/**
  * @brief  Enable or disable the PLLI2S clock.
  * @note   After enabling the PLLI2S, the application software should wait on PLLI2S STABLE
  *         flag to be set indicating that PLLI2S clock is stable.
  * @param  newState: new state of the PLLI2S.
  *         This parameter can be: ENBALE or DISABLE.
  * @retval None
  *
  */

static void HAL_CLOCK_PLLI2Scmd(uint8_t newState)
{
    ASSERT(IS_CLOCK_STATE(newState));
    /* switch on or off PLLI2S */
    LL_PLLI2S_ENABLE_DISABLE(newState);
}

/**
  * @brief  Adjust PLLSAI vco configuration and charge pump current selection
  * @note   Be careful when calling this function. In general, should not modify these two param
  *
  * @param  vco:    PLLSAI vco control
  *         CPCSel: charge pump current selection
  *
  * @retval None
  */
#if 0
static void HAL_CLOCK_AdjustPLLSAIDefaultConfig(uint8_t vco, uint8_t CPCSel)
{
    ASSERT(IS_PLLSAI_VCO_VALUE(vco));
    ASSERT(IS_PLLSAI_CPC_VALUE(CPCSel));

    LL_ADJUST_PLLSAI_VCO(vco);
    LL_ADJUST_PLLSAI_CPC(CPCSel);
}
#endif

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

void HAL_CLOCK_AdjustPLLSAIDefaultDiv(uint8_t divp, uint8_t divq, uint8_t divr, uint8_t divn)
{
    ASSERT(IS_PLLSAI_DIVP_VALUE(divp));
    ASSERT(IS_PLLSAI_DIVQ_VALUE(divq));
    ASSERT(IS_PLLSAI_DIVR_VALUE(divr));
    ASSERT(IS_PLLSAI_DIVN_VALUE(divn));

    if (divp != 0)
    {
        LL_PLLSAI_CONFIG_DIVP(divp);
    }
    if (divq != 0)
    {
        LL_PLLSAI_CONFIG_DIVQ(divq);
    }
    if (divr != 0)
    {
        LL_PLLSAI_CONFIG_DIVR(divr);
    }
    if (divn != 0)
    {
        LL_PLLSAI_CONFIG_DIVN(divn);
    }
}

/**
  * @brief  Enable or disable the PLLSAI clock.
  * @note   After enabling the PLLSAI, the application software should wait on PLLSAI STABLE
  *         flag to be set indicating that PLLSAI clock is stable.
  * @param  newState: new state of the PLLSAI.
  *         This parameter can be: ENBALE or DISABLE.
  * @retval None
  *
  */

static void HAL_CLOCK_PLLSAIcmd(uint8_t newState)
{
    ASSERT(IS_CLOCK_STATE(newState));
    /* switch on or off PLLSAI */
    LL_PLLSAI_ENABLE_DISABLE(newState);
}

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
uint32_t HAL_CLOCK_WaitForStable(uint32_t clkStableFlag)
{
    return (SYS_DEV->CMUST & clkStableFlag);
}

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
void HAL_CLOCK_ClockCmd(CLOCK_Sel_T clockMask, uint8_t newState)
{
    switch (clockMask)
    {
        case CLOCK_HIRC_MASK:
            HAL_CLOCK_HIRCcmd(newState);
            break;
        case CLOCK_HOSC_MASK:
            HAL_CLOCK_HOSCcmd(newState);
            break;
        case CLOCK_LIRC_MASK:
            HAL_CLOCK_LIRCcmd(newState);
            break;
        case CLOCK_LOSC_MASK:
            HAL_CLOCK_LOSCcmd(newState);
            break;
        case CLOCK_PLLKDP_MASK:
            HAL_CLOCK_PLLKDPcmd(newState);
            break;
        case CLOCK_PLL_MASK:
            HAL_CLOCK_PLLcmd(newState);
            break;
        case CLOCK_PLLI2S_MASK:
            HAL_CLOCK_PLLI2Scmd(newState);
            break;
        case CLOCK_PLLSAI_MASK:
            HAL_CLOCK_PLLSAIcmd(newState);
            break;
        default:
            break;
    }
}


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

void HAL_CLOCK_SYSCLKConfig(CLOCK_SysClockConfig_T *SysClockConfigStructure)
{
    /* The system clock is drived from HIRC */
    if (SysClockConfigStructure->sysClkSource == SYSCLOCK_SOURCE_HIRC8M)
    {
        if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
        {
            HAL_CLOCK_HIRCcmd(ENABLE);
            while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
        }
    }
    /* The system clock is drived from HOSC */
    if (SysClockConfigStructure->sysClkSource == SYSCLOCK_SOURCE_HOSC8M)
    {
        if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE))
        {
            HAL_CLOCK_HOSCcmd(ENABLE);
            while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE));
        }
    }
    /* The system clock is drived from PLL */
    if (SysClockConfigStructure->sysClkSource == SYSCLOCK_SOURCE_PLL)
    {
        if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE))
        {
            HAL_CLOCK_PLLcmd(ENABLE);
        }
        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE));
    }
    /* The system clock is drived from PLLKDP100 */
    if (SysClockConfigStructure->sysClkSource == SYSCLOCK_SOURCE_PLLKDP100)
    {

        if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE))
        {
            HAL_CLOCK_AdjustPLLKDPDefaultDiv(2, 4, 6, 0x12C);
            HAL_CLOCK_AdjustPLLKDPDefaultConfig(1, 0);
            HAL_CLOCK_PLLKDPcmd(ENABLE);
        }
        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE));
    }
    /* The system clock is drived from PLLKDP150 */
    if (SysClockConfigStructure->sysClkSource == SYSCLOCK_SOURCE_PLLKDP150)
    {
        if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE))
        {
            HAL_CLOCK_AdjustPLLKDPDefaultDiv(2, 4, 6, 0x12C);
            HAL_CLOCK_AdjustPLLKDPDefaultConfig(1, 0);
            HAL_CLOCK_PLLKDPcmd(ENABLE);
        }
        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE));
    }
#if defined(CHIP_F6721B)
    /* The system clock is derived from PLLSAI */
    if (SysClockConfigStructure->sysClkSource == SYSCLOCK_SOURCE_PLLSAI)
    {
        if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE))
        {
            HAL_CLOCK_PLLSAIcmd(ENABLE);
        }
        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE));
    }
#endif
    /*Configure system clock prescaler*/
    LL_SYSCLOCK_DIV_CONFIG(SysClockConfigStructure->sysClkPrescaler);
    /*Configure system clock source*/
    LL_SYSCLOCK_SEL_CONFIG(SysClockConfigStructure->sysClkSource);
    while (LL_SYSCLOCK_GET_ERR_FLAG)
    {
        LL_SYSCLOCK_CLEAR_ERR_FLAG;
        LL_SYSCLOCK_SEL_CONFIG(SysClockConfigStructure->sysClkSource);
    }
}

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
  *             -0x05:  PLLSAI used as system clock
  */

void HAL_CLOCK_GetSYSCLKConfig(CLOCK_SysClockConfig_T *getSysClock)
{
    getSysClock->sysClkSource = (CLOCK_SysClockSource_T)LL_SYSCLOCK_GET_SYSCLK_SEL;
    getSysClock->sysClkPrescaler = (CLOCK_SysClockPrescaler_T)LL_SYSCLOCK_GET_SYSCLK_DIV;
    getSysClock->sysClkPrescaler >>= SYS_SYSCLK_FCLK_DIV_POS;
}

/**
  * @brief  Returns the system clock frequency.
  *
  * @param  None
  *
  * @retval The system clock frequency
  *
  */

uint32_t HAL_CLOCK_GetSYSCLKFreq(void)
{
    CLOCK_SysClockConfig_T getSysClock;
    uint32_t sysClkFreq;
    uint32_t pll_m;
    uint32_t pllkdp_n;
    uint32_t pllkdp_q;
    uint32_t pllkdp_r;
    uint32_t pll_n;
    uint32_t pll_p;
    HAL_CLOCK_GetSYSCLKConfig(&getSysClock);
    switch (getSysClock.sysClkSource)
    {
        case SYSCLOCK_SOURCE_HIRC8M:
            sysClkFreq = HIRC_VALUE / (getSysClock.sysClkPrescaler + 1);
            break;
        case SYSCLOCK_SOURCE_HOSC8M:
            sysClkFreq = HOSC_VALUE / (getSysClock.sysClkPrescaler + 1);
            break;
        case SYSCLOCK_SOURCE_PLLKDP150:
            pll_m = ((SYS_DEV->PLLREF & (0xF << 8)) >> 8);
            pllkdp_n = ((SYS_DEV->PLLKDP[0] & (0x1FF << 16)) >> 16);
            pllkdp_q = ((SYS_DEV->PLLKDP[1] & (0xFF << 8)) >> 8);
            sysClkFreq = HIRC_VALUE / pll_m * pllkdp_n / pllkdp_q / (getSysClock.sysClkPrescaler + 1);
            break;
        case SYSCLOCK_SOURCE_PLL:
            pll_m = ((SYS_DEV->PLLREF & (0xF << 8)) >> 8);
            pll_n = ((SYS_DEV->PLL[1] & ((uint32_t)0xFF << 24)) >> 24);
            pll_p = ((SYS_DEV->PLL[1] & (0xFF << 0)) >> 0);
            sysClkFreq = HIRC_VALUE / pll_m * pll_n / pll_p / (getSysClock.sysClkPrescaler + 1);
            break;
        case SYSCLOCK_SOURCE_PLLKDP100:
            pll_m = ((SYS_DEV->PLLREF & (0xF << 8)) >> 8);
            pllkdp_n = ((SYS_DEV->PLLKDP[0] & (0x1FF << 16)) >> 16);
            pllkdp_r = ((SYS_DEV->PLLKDP[1] & (0xFF << 16)) >> 16);
            sysClkFreq = HIRC_VALUE / pll_m * pllkdp_n / pllkdp_r / (getSysClock.sysClkPrescaler + 1);
            break;
#if defined(CHIP_F6721B)
        case SYSCLOCK_SOURCE_PLLSAI:
            break;
#endif
        default:
            sysClkFreq = 0;
            break;
    }
    return sysClkFreq;
}





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

void HAL_CLOCK_PeripheralClockEnable0(uint32_t ENABLECLK0Peripheral)
{
    ASSERT(IS_ENCLK0_PERIPH(ENABLECLK0Peripheral));

    LL_SYSCLOCK_ENABLE_PERIPHERAL_CLOCK0(ENABLECLK0Peripheral);
}

void HAL_CLOCK_PeripheralClockDisable0(uint32_t ENABLECLK0Peripheral)
{
    ASSERT(IS_ENCLK0_PERIPH(ENABLECLK0Peripheral));

    LL_SYSCLOCK_DISABLE_PERIPHERAL_CLOCK0(ENABLECLK0Peripheral);
}


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

void HAL_CLOCK_PeripheralClockEnable1(uint32_t ENABLECLK1Peripheral)
{
    ASSERT(IS_ENCLK1_PERIPH(ENABLECLK1Peripheral));

    LL_SYSCLOCK_ENABLE_PERIPHERAL_CLOCK1(ENABLECLK1Peripheral);
}

void HAL_CLOCK_PeripheralClockDisable1(uint32_t ENABLECLK1Peripheral)
{
    ASSERT(IS_ENCLK1_PERIPH(ENABLECLK1Peripheral));

    LL_SYSCLOCK_DISABLE_PERIPHERAL_CLOCK1(ENABLECLK1Peripheral);
}


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

void HAL_CLOCK_PeripheralClockEnable2(uint32_t ENABLECLK2Peripheral)
{
    ASSERT(IS_ENCLK2_PERIPH(ENABLECLK2Peripheral));

    LL_SYSCLOCK_ENABLE_PERIPHERAL_CLOCK2(ENABLECLK2Peripheral);
}

void HAL_CLOCK_PeripheralClockDisable2(uint32_t ENABLECLK2Peripheral)
{
    ASSERT(IS_ENCLK2_PERIPH(ENABLECLK2Peripheral));

    LL_SYSCLOCK_DISABLE_PERIPHERAL_CLOCK2(ENABLECLK2Peripheral);
}


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
void HAL_CLOCK_PeripheralClkSetConfig(Peripheral_Config_T *peripheClockSet)
{
    switch (peripheClockSet->peripheMask)
    {
        case PERIPHERAL_SDRAM_MASK:
            /* Configure clock for SDRAM */
            LL_PERIPHECLK_SDRAM_CONFIG(peripheClockSet->peripheClkConfig.sdramClkConfig);
            break;
        case PERIPHERAL_WWDT_MASK:
            /* Configure clock for WWDT */
            LL_PERIPHECLK_WWDT_CONFIG(peripheClockSet->peripheClkConfig.wwdtClkConfig);
            break;
        case PERIPHERAL_LPSTM_MASK:
            /* Configure clock for LPSTM */
            switch (peripheClockSet->peripheClkConfig.lpstmClkConfig)
            {
                case CLOCK_LPSTM_SEL_LIRC:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_LIRC_STABLE))
                    {
                        HAL_CLOCK_LIRCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_LIRC_STABLE));
                    }
                    break;
                case CLOCK_LPSTM_SEL_LOSC:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_LOSC_STABLE))
                    {
                        HAL_CLOCK_LOSCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_LOSC_STABLE));
                    }
                    break;
                case CLOCK_LPSTM_SEL_HIRC:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                    {
                        HAL_CLOCK_HIRCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                    }
                    break;
                default:
                    break;
            }
            LL_PERIPHECLK_LPSTM_CONFIG(peripheClockSet->peripheClkConfig.lpstmClkConfig);
            break;
        case PERIPHERAL_MCO1_MASK:
            /* Configure source clock for MCO1 */
            LL_PERIPHECLK_MCO1_SEL_CONFIG(peripheClockSet->peripheClkConfig.mco1ClkConfig.mco1SourceClk);
            /* Configure prescaler for MCO1 */
            LL_PERIPHECLK_MCO1_DIV_CONFIG(peripheClockSet->peripheClkConfig.mco1ClkConfig.mco1Prescaler);
            break;
        case PERIPHERAL_MCO2_MASK:
            /* Configure source clock for MCO2 */
            LL_PERIPHECLK_MCO2_SEL_CONFIG(peripheClockSet->peripheClkConfig.mco2ClkConfig.mco2SourceClk);
            /* Configure prescaler for MCO2 */
            LL_PERIPHECLK_MCO2_DIV_CONFIG(peripheClockSet->peripheClkConfig.mco2ClkConfig.mco2Prescaler);
            break;
        case PERIPHERAL_RTC_MASK:
            /* Configure source clock for RTC */
            switch (peripheClockSet->peripheClkConfig.rtcClkConfig.rtcSourceClk)
            {
                case CLOCK_RTC_SEL_LIRC:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_LIRC_STABLE))
                    {
                        HAL_CLOCK_LIRCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_LIRC_STABLE));
                    }
                    break;
                case CLOCK_RTC_SEL_LOSC:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_LOSC_STABLE))
                    {
                        HAL_CLOCK_LOSCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_LOSC_STABLE));
                    }
                    break;
                case CLOCK_RTC_SEL_HOSC:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE))
                    {
                        HAL_CLOCK_HOSCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HOSC_STABLE));
                    }
                    break;
                default:
                    break;
            }
            LL_PERIPHECLK_RTC_SEL_CONFIG(peripheClockSet->peripheClkConfig.rtcClkConfig.rtcSourceClk);
            if (peripheClockSet->peripheClkConfig.rtcClkConfig.rtcSourceClk == CLOCK_RTC_SEL_HOSC)
            {
                /* Configure prescaler for RTC */
                LL_PERIPHECLK_RTC_DIV_CONFIG(peripheClockSet->peripheClkConfig.rtcClkConfig.rtcHOSCDiv);
            }
            break;
        case PERIPHERAL_KDP_MASK:
            /* Configure clock for KDP */
            switch (peripheClockSet->peripheClkConfig.kdpClkConfig)
            {
                case CLOCK_KDP_SEL_PLLKDP300:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE))
                    {
                        /*  PLLKDP is disabled, check if any of PLLs is stable, if the IF condition
                         *  is false, at lease one of the PLLs are enabled and it must has set the
                         *  PLLs reference clock, so just need to enable PLLKDP clock rather than setting
                         *  the reference clock
                         */
                        if (!LL_PLLs_ANY_IS_STABLE)
                        {
                            /* if all PLLs is disabled, set HIRC as PLL reference clock */
                            HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
                            if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                            {
                                HAL_CLOCK_HIRCcmd(ENABLE);
                                while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                            }
                        }
                        /*fix bug: change PLLN to set VCO to 600MHZ, don't change*/
                        HAL_CLOCK_AdjustPLLKDPDefaultDiv(2, 4, 6, 0x12C);
                        HAL_CLOCK_AdjustPLLKDPDefaultConfig(1, 0);
                        /*fix bug end*/
                        HAL_CLOCK_PLLKDPcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLKDP_STABLE));
                    }
                    break;
                default:
                    break;
            }
            LL_PERIPHECLK_KDP_CONFIG(peripheClockSet->peripheClkConfig.kdpClkConfig);
            break;
        case PERIPHERAL_ADC1_MASK:
        case PERIPHERAL_ADC2_MASK:
        case PERIPHERAL_ADC3_MASK:
            /* Configure source clock for ADC */
            switch (peripheClockSet->peripheClkConfig.adcClkConfig.adcSourceClk)
            {
                case CLOCK_ADC_SEL_HIRC:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                    {
                        HAL_CLOCK_HIRCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                    }
                    break;
                case CLOCK_ADC_SEL_PLL:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE))
                    {
                        /*  PLL is disabled, check if any of PLLs is stable, if the IF condition
                         *  is false, at lease one of the PLLs are enabled and it must has set the
                         *  PLLs reference clock, so just need to enable PLL clock rather than setting
                         *  the reference clock
                         */
                        if (!LL_PLLs_ANY_IS_STABLE)
                        {
                            /* if all PLLs is disabled, set HIRC as PLL reference clock */
                            HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
                            if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                            {
                                HAL_CLOCK_HIRCcmd(ENABLE);
                                while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                            }
                        }
                        HAL_CLOCK_PLLcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLL_STABLE));
                    }
                    break;
                case CLOCK_ADC_SEL_PLLI2S:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE))
                    {
                        /*  PLLI2S is disabled, check if any of PLLs is stable, if the IF condition
                         *  is false, at lease one of the PLLs are enabled and it must has set the
                         *  PLLs reference clock, so just need to enable PLLI2S clock rather than
                         *  setting the reference clock
                         */
                        if (!LL_PLLs_ANY_IS_STABLE)
                        {
                            /* if all PLLs is disabled, set HIRC as PLL reference clock */
                            HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
                            if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                            {
                                HAL_CLOCK_HIRCcmd(ENABLE);
                                while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                            }
                        }
                        HAL_CLOCK_PLLI2Scmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE));
                    }
                    break;
                default:
                    break;
            }
            switch (peripheClockSet->peripheMask)
            {
                case PERIPHERAL_ADC1_MASK:
                    LL_PERIPHECLK_ADC1_SEL_CONFIG(peripheClockSet->peripheClkConfig.adcClkConfig.adcSourceClk);
                    /* Configure prescaler for ADC1 */
                    LL_PERIPHECLK_ADC1_DIV_CONFIG(peripheClockSet->peripheClkConfig.adcClkConfig.adcPrescaler);
                    break;
                case PERIPHERAL_ADC2_MASK:
                    LL_PERIPHECLK_ADC2_SEL_CONFIG(peripheClockSet->peripheClkConfig.adcClkConfig.adcSourceClk);
                    /* Configure prescaler for ADC2 */
                    LL_PERIPHECLK_ADC2_DIV_CONFIG(peripheClockSet->peripheClkConfig.adcClkConfig.adcPrescaler);
                    break;
                case PERIPHERAL_ADC3_MASK:
                    LL_PERIPHECLK_ADC3_SEL_CONFIG(peripheClockSet->peripheClkConfig.adcClkConfig.adcSourceClk);
                    /* Configure prescaler for ADC3 */
                    LL_PERIPHECLK_ADC3_DIV_CONFIG(peripheClockSet->peripheClkConfig.adcClkConfig.adcPrescaler);
                    break;
                default:
                    break;
            }
            break;
        case PERIPHERAL_DAC_MASK:
            /* Configure clock for DAC */
            switch (peripheClockSet->peripheClkConfig.dacClkConfig)
            {
                case CLOCK_DAC_SEL_LOSC:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_LOSC_STABLE))
                    {
                        HAL_CLOCK_LOSCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_LOSC_STABLE));
                    }
                    break;
                case CLOCK_DAC_SEL_LIRC:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_LIRC_STABLE))
                    {
                        HAL_CLOCK_LIRCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_LIRC_STABLE));
                    }
                    break;
            }
            LL_PERIPHECLK_DAC_CONFIG(peripheClockSet->peripheClkConfig.dacClkConfig);
            break;
        case PERIPHERAL_SDMMC_MASK:
            /* Configure clock for SDMMC */
            switch (peripheClockSet->peripheClkConfig.sdmmcClkConfig)
            {
                case CLOCK_SDMMC_SEL_PLLI2S:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE))
                    {
                        /*  PLLI2S is disabled, check if any of PLLs is stable, if the IF condition
                         *  is false, at lease one of the PLLs are enabled and it must has set the
                         *  PLLs reference clock, so just need to enable PLLI2S clock rather than
                         *  setting the reference clock
                         */
                        if (!LL_PLLs_ANY_IS_STABLE)
                        {
                            /* if all PLLs is disabled, set HIRC as PLL reference clock */
                            HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
                            if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                            {
                                HAL_CLOCK_HIRCcmd(ENABLE);
                                while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                            }
                        }
                        HAL_CLOCK_PLLI2Scmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE));
                    }
                    break;
                default:
                    break;
            }
            LL_PERIPHECLK_SDMMC_CONFIG(peripheClockSet->peripheClkConfig.sdmmcClkConfig);
            break;
        case PERIPHERAL_VOUT_MASK:
            /* Configure clock for VOUT */
            if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE))
            {
                /*  PLLSAI is disabled, check if any of PLLs is stable, if the IF condition
                 *  is false, at lease one of the PLLs are enabled and it must has set the
                 *  PLLs reference clock, so just need to enable PLLSAI clock rather than
                 *  setting the reference clock
                 */
                if (!LL_PLLs_ANY_IS_STABLE)
                {
                    /* if all PLLs is disabled, set HIRC as PLL reference clock */
                    HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                    {
                        HAL_CLOCK_HIRCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                    }
                }
                HAL_CLOCK_PLLSAIcmd(ENABLE);
                while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE));
            }
            LL_PERIPHECLK_VOUT_CONFIG(peripheClockSet->peripheClkConfig.voutClkConfig);
            break;
        case PERIPHERAL_I2C1_MASK:
            /* Configure clock for I2C1 */
            LL_PERIPHECLK_I2C1_CONFIG(peripheClockSet->peripheClkConfig.i2cClkConfig);
            break;
        case PERIPHERAL_I2C2_MASK:
            /* Configure clock for I2C2 */
            LL_PERIPHECLK_I2C2_CONFIG(peripheClockSet->peripheClkConfig.i2cClkConfig);
            break;
        case PERIPHERAL_I2C3_MASK:
            /* Configure clock for I2C3 */
            LL_PERIPHECLK_I2C3_CONFIG(peripheClockSet->peripheClkConfig.i2cClkConfig);
            break;
        case PERIPHERAL_BTM1_MASK:
            /* Configure clock for BTM1 */
            LL_PERIPHECLK_BTM1_CONFIG(peripheClockSet->peripheClkConfig.btmClkConfig);
            break;
        case PERIPHERAL_BTM2_MASK:
            /* Configure clock for BTM2 */
            LL_PERIPHECLK_BTM2_CONFIG(peripheClockSet->peripheClkConfig.btmClkConfig);
            break;
        case PERIPHERAL_SAIA_MASK:
        case PERIPHERAL_SAIB_MASK:
            /* Configure clock for SAIA */
            switch (peripheClockSet->peripheClkConfig.saiClkConfig)
            {
                case CLOCK_SAI_SEL_PLLSAI:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE))
                    {
                        /*  PLLSAI is disabled, check if any of PLLs is stable, if the IF condition
                         *  is false, at lease one of the PLLs are enabled and it must has set the
                         *  PLLs reference clock, so just need to enable PLLSAI clock rather than
                         *  setting the reference clock
                         */
                        if (!LL_PLLs_ANY_IS_STABLE)
                        {
                            /* if all PLLs is disabled, set HIRC as PLL reference clock */
                            HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
                            if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                            {
                                HAL_CLOCK_HIRCcmd(ENABLE);
                                while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                            }
                        }
                        HAL_CLOCK_PLLSAIcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLSAI_STABLE));
                    }
                    break;
                case CLOCK_SAI_SEL_PLLI2S:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE))
                    {
                        /*  PLLI2S is disabled, check if any of PLLs is stable, if the IF condition
                         *  is false, at lease one of the PLLs are enabled and it must has set the
                         *  PLLs reference clock, so just need to enable PLLI2S clock rather than
                         *  setting the reference clock
                         */
                        if (!LL_PLLs_ANY_IS_STABLE)
                        {
                            /* if all PLLs is disabled, set HIRC as PLL reference clock */
                            HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
                            if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                            {
                                HAL_CLOCK_HIRCcmd(ENABLE);
                                while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                            }
                        }
                        HAL_CLOCK_PLLI2Scmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE));
                    }
                    break;
                case CLOCK_SAI_SEL_IO:
                default:
                    break;
            }
            switch (peripheClockSet->peripheMask)
            {
                case PERIPHERAL_SAIA_MASK:
                    LL_PERIPHECLK_SAIA_CONFIG(peripheClockSet->peripheClkConfig.saiClkConfig);
                    break;
                case PERIPHERAL_SAIB_MASK:
                    LL_PERIPHECLK_SAIB_CONFIG(peripheClockSet->peripheClkConfig.saiClkConfig);
                    break;
                default:
                    break;
            }
            break;
        case PERIPHERAL_UART1_MASK:
        case PERIPHERAL_UART2_MASK:
        case PERIPHERAL_UART3_MASK:
        case PERIPHERAL_UART4_MASK:
        case PERIPHERAL_UART5_MASK:
        case PERIPHERAL_UART6_MASK:
        case PERIPHERAL_UART7_MASK:
        case PERIPHERAL_UART8_MASK:
            /* Configure source clock for UART clock */
            switch (peripheClockSet->peripheClkConfig.uartClkConfig.uartSourceClk)
            {
                case CLOCK_UART_SEL_HIRC:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                    {
                        HAL_CLOCK_HIRCcmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                    }
                    break;
                case CLOCK_UART_SEL_PLLI2S:
                    if (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE))
                    {
                        /*  PLLI2S is disabled, check if any of PLLs is stable, if the IF condition
                         *  is false, at lease one of the PLLs are enabled and it must has set the
                         *  PLLs reference clock, so just need to enable PLLI2S clock rather than
                         *  setting the reference clock
                         */
                        if (!LL_PLLs_ANY_IS_STABLE)
                        {
                            /* if all PLLs is disabled, set HIRC as PLL reference clock */
                            HAL_CLOCK_PLLsReferenceClock(PLL_IRC_HSCLK);
                            if (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE))
                            {
                                HAL_CLOCK_HIRCcmd(ENABLE);
                                while (!HAL_CLOCK_WaitForStable(SYS_CMUST_HIRC_STABLE));
                            }
                        }
                        HAL_CLOCK_PLLI2Scmd(ENABLE);
                        while (!HAL_CLOCK_WaitForStable(SYS_CMUST_PLLI2S_STABLE));
                    }
                    break;
                default:
                    break;
            }
            switch (peripheClockSet->peripheMask)
            {
                case PERIPHERAL_UART1_MASK:
                    LL_PERIPHECLK_UART1_SEL_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartSourceClk);
                    /* Configure prescaler for UART1 */
                    LL_PERIPHECLK_UART1_DIV_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartPrescaler);
                    break;
                case PERIPHERAL_UART2_MASK:
                    LL_PERIPHECLK_UART2_SEL_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartSourceClk);
                    /* Configure prescaler for UART2 */
                    LL_PERIPHECLK_UART2_DIV_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartPrescaler);
                    break;
                case PERIPHERAL_UART3_MASK:
                    LL_PERIPHECLK_UART3_SEL_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartSourceClk);
                    /* Configure prescaler for UART3 */
                    LL_PERIPHECLK_UART3_DIV_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartPrescaler);
                    break;
                case PERIPHERAL_UART4_MASK:
                    LL_PERIPHECLK_UART4_SEL_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartSourceClk);
                    /* Configure prescaler for UART4 */
                    LL_PERIPHECLK_UART4_DIV_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartPrescaler);
                    break;
                case PERIPHERAL_UART5_MASK:
                    LL_PERIPHECLK_UART5_SEL_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartSourceClk);
                    /* Configure prescaler for UART5 */
                    LL_PERIPHECLK_UART5_DIV_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartPrescaler);
                    break;
                case PERIPHERAL_UART6_MASK:
                    LL_PERIPHECLK_UART6_SEL_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartSourceClk);
                    /* Configure prescaler for UART6 */
                    LL_PERIPHECLK_UART6_DIV_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartPrescaler);
                    break;
                case PERIPHERAL_UART7_MASK:
                    LL_PERIPHECLK_UART7_SEL_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartSourceClk);
                    /* Configure prescaler for UART7 */
                    LL_PERIPHECLK_UART7_DIV_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartPrescaler);
                    break;
                case PERIPHERAL_UART8_MASK:
                    LL_PERIPHECLK_UART8_SEL_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartSourceClk);
                    /* Configure prescaler for UART8 */
                    LL_PERIPHECLK_UART8_DIV_CONFIG(peripheClockSet->peripheClkConfig.uartClkConfig.uartPrescaler);
                    break;
                default:
                    break;
            }
            break;
        case PERIPHERAL_TIM1_MASK:
            /* Configure clock for TIM1 */
            LL_PERIPHECLK_TIM1_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM2_MASK:
            /* Configure clock for TIM2 */
            LL_PERIPHECLK_TIM2_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM3_MASK:
            /* Configure clock for TIM3 */
            LL_PERIPHECLK_TIM3_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM4_MASK:
            /* Configure clock for TIM4 */
            LL_PERIPHECLK_TIM4_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM5_MASK:
            /* Configure clock for TIM5 */
            LL_PERIPHECLK_TIM5_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM8_MASK:
            /* Configure clock for TIM8 */
            LL_PERIPHECLK_TIM8_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM9_MASK:
            /* Configure clock for TIM9 */
            LL_PERIPHECLK_TIM9_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM10_MASK:
            /* Configure clock for ETM10 */
            LL_PERIPHECLK_TIM10_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM11_MASK:
            /* Configure clock for ETM11 */
            LL_PERIPHECLK_TIM11_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM12_MASK:
            /* Configure clock for TIM12 */
            LL_PERIPHECLK_TIM12_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM13_MASK:
            /* Configure clock for TIM13 */
            LL_PERIPHECLK_TIM13_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        case PERIPHERAL_TIM14_MASK:
            /* Configure clock for TIM14 */
            LL_PERIPHECLK_TIM14_CONFIG(peripheClockSet->peripheClkConfig.timClkConfig);
            break;
        default:
            break;
    }

}

