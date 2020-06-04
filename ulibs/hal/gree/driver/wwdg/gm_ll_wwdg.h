/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm_ll_wwdg.h
 *
 * @author        kenny.cmt
 *
 * @version       1.0.0
 *
 * @date          2018/03/20
 *
 * @brief         Header file of wdog driver.
 *
 * @note
 *    2018/03/20    kenny.cmt     V1.0.0
 *        Update For GM6721

 */

#ifndef _GM_LL_WWDG_H_
#define _GM_LL_WWDG_H_
#include <gm_common.h>
#include<gm_soc.h>
#include"gm_ll_wwdg_dev.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup WDOG_control_reg_bit
 * @{
 */
#define WDOG_CTRL_IE_POS                    ((uint32_t) 31)                                               /* WDOG Timer module enable pos */
#define WDOG_CTRL_IE                        ((uint32_t) ((uint32_t)0x01 << WDOG_CTRL_IE_POS))             /* WDOG Timer module enalbe bitmask */
#define WDOG_CNT_OUT_WINDOW_FLAG_POS        ((uint32_t) 2)
#define WDOG_CNT_OUT_WINDOW_OUTSIDE         ((uint32_t) ((uint32_t)0x01 << WDOG_CNT_OUT_WINDOW_FLAG_POS)) /* WDOG counter value outside window */
#define WDOG_CNT_OUT_WINDOW_INSIDE          ((uint32_t) ((uint32_t)0x00 << WDOG_CNT_OUT_WINDOW_FLAG_POS)) /* WDOG counter value inside window */
#define WDOG_WINDOW_EN_POS                  ((uint32_t) 1)                                                /* WDOG enable window mode pos */
#define WDOG_WINDOW_EN                      ((uint32_t) ((uint32_t)0x01 << WDOG_WINDOW_EN_POS))           /* WDOG enable window mode bitmask */
#define WDOG_INT_EN_POS                     ((uint32_t) 0)                                                /* WDOG enable the early warning interrupt pos */
#define WDOG_INT_EN                         ((uint32_t) ((uint32_t)0x01 << WDOG_INT_EN_POS))              /* WDOG enable the early warning interrupt bitmask */
/**
 * @}
 */

/**
* @defgroup WDOG_write_access_bit
* @{
*/
#define WDOG_WRITE_ACCESS_POS               ((uint32_t) 1)                                                /* WDOG access reg lock pos */
#define WDOG_WRITE_ACCESS                   ((uint32_t) ((uint32_t)0xfffffffe << WDOG_WRITE_ACCESS_POS))  /* WDOG access reg lock bitmask */
#define WDOG_WRITE_ENABLE_STATUS_POS        ((uint32_t) 0)                                                /* WDOG read only to check reglock status pos */
#define WDOG_WRITE_ENABLE_STATUS            ((uint32_t) ((uint32_t)0xfffffffe << WDOG_WRITE_ENABLE_STATUS_POS)) /*WDOG read only to check */
/**
 * @}
 */

/**
 * @defgroup WDOG_test_mode_reg_bit
 * @{
 */
#define WDOG_TEST_MODE_EN_POS               ((uint32_t) 0)
#define WDOG_TEST_MODE_EN                   ((uint32_t) ((uint32_t)0X01 << WDOG_TEST_MODE_EN_POS))        /* WDOG test mode register,enable integration*/
/**
 * @}
 */

/**
 * @defgroup WDOG_test_mode_output_reg_bit
 * @{
 */
#define WDOG_TEST_INTVALUE_POS              ((uint32_t) 1)                                                /* WDOG integration test wdtInt pos */
#define WDOG_TEST_INTVALUE                  ((uint32_t) ((uint32_t)0x01 << WDOG_TEST_INTVALUE_POS))       /* WDOG Value output on WDOGINT when in Integration Test Mode */
#define WDOG_TEST_RESVALUE_POS              ((uint32_t) 0)                                                /* WDOG Integration Test WDOGRES value pos */
#define WDOG_TEST_RESVALUE                  ((uint32_t) ((uint32_t)0x01 << WDOG_TEST_RESVALUE_POS))       /* WDOG Value output on WDOGRES when in Integration Test Mode  */
/**
 * @}
 */


#define WDOG_CLEAR_INT_MAGICNUM             0xadcddcba         /* Write This Maigcnum To Tntclr Reg To Clear Irq And Feed WDOG */
#define WDOG_UNLOCK_MAGIC_NUM               0x1acce551         /* Write This Maigcnum To WDOG Lock Reg To Unlock Write WDOG Reg */

#define MS_DIV                              1000               /* WDOG Ms Div Cnt */




/**
 * @defgroup CHIP_GM6721_WDOG_CLK_SRC
 * @{
 */
#define WDOG_SRC_CLK_DIV512                 390625            /* WDOG Srcclk:SYSCLK/(511+1) */

/**
 * @}
 */

#if 0
#define LL_WDOG_WAIT_WRITEDONE()               do{while(SYSDEV->DBGCTRL & SYS_DGB_WDOG_WRITEDONE);}while(0u);         /* Waiting for DBG_CFG_CTRL bit16 wdog write done flag */
#define LL_WDOG_UNLOCK(pWDOG)                  do{WDOG_WAIT_WRITEDONE(); pWDOG->KR = WDOG_UNLOCK_MAGIC_NUM;}while(0u) /* Ulock WDOG Reg,Acess WDOG Reg Is Vaild */
#define LL_WDOG_LOCK(pWDOG)                    do{WDOG_WAIT_WRITEDONE(); pWDOG->KR = 0;}while(0u)                     /* Lock WDOG Reg,Access WDOG Reg Is Not Vaild */
#else
#define SYS_DGB_WDOG_WRITEDONE                 ((uint32_t)0x01 << 17)
#define WDOG_WRITEDONE_REG                     (SYS_DEV->WDTDBG)
//#define LL_WDOG_WAIT_WRITEDONE()               do{while((*(volatile uint32_t *)(0x40000054)) & SYS_DGB_WDOG_WRITEDONE);}while(0u);         /* Waiting for DBG_CFG_CTRL bit16 wdog write done flag */
#define LL_WDOG_WAIT_WRITEDONE()               do{while(WDOG_WRITEDONE_REG & SYS_DGB_WDOG_WRITEDONE); }while(0u);         /* Waiting for DBG_CFG_CTRL bit16 wdog write done flag */
#define LL_WDOG_UNLOCK(pWDOG)                  do{LL_WDOG_WAIT_WRITEDONE();pWDOG->KR = WDOG_UNLOCK_MAGIC_NUM;}while(0u)          /* Ulock WDOG Reg,Acess WDOG Reg Is Vaild */
#define LL_WDOG_LOCK(pWDOG)                    do{LL_WDOG_WAIT_WRITEDONE();pWDOG->KR = 0;}while(0u)          /* Lock WDOG Reg,Access WDOG Reg Is Not Vaild */

#endif

/**
  * @brief  Start the  Watchdog
  * @param  pWDOG WDOG Instance
  * @retval None
  */
__STATIC_INLINE void  LL_WDOG_Enable(WDOG_Device_T *pWDOG)
{
    IO_BIT_SET(pWDOG->CTRL, WDOG_CTRL_IE);
}



/**
  * @brief  Stop the  Watchdog
  * @param  pWDOG WDOG Instance
  * @retval None
  */
__STATIC_INLINE void  LL_WDOG_Disable(WDOG_Device_T *pWDOG)
{
    IO_BIT_CLEAR(pWDOG->CTRL, WDOG_CTRL_IE);
}

/**
  * @brief  Set counter when an interrupt is generated
  * @param  pWDOG WDOG Instance
  * @param  winLen base number
  * @retval None
  */
__STATIC_INLINE void   LL_WDOG_SetWinLen(WDOG_Device_T *pWDOG, uint8_t winLen)
{
    IO_BIT_CLEAR(pWDOG->CTRL, (0x0F << 3));
    IO_BIT_SET(pWDOG->CTRL, (winLen << 3));
}




/**
  * @brief  FeedWdog
  * @param  pWDOG WDOG Instance
  * @retval None
  */
__STATIC_INLINE void  LL_WDOG_FeedWdog(WDOG_Device_T *pWDOG)
{
    pWDOG->KR = 0xAAAA;
}




/**
  * @brief  clear wdog irq which is equal to feedwdog
  * @param  pWDOG WDOG Instance
  * @retval None
  */
__STATIC_INLINE void  LL_WDOG_IntClr(WDOG_Device_T *pWDOG)
{
    pWDOG->IC = WDOG_CLEAR_INT_MAGICNUM;
}


/**
  * @brief  Open wdog interrupt bit
  * @param  pWDOG WDOG Instance
  * @retval None
  */
__STATIC_INLINE void  LL_WDOG_EWI_Enable(WDOG_Device_T *pWDOG)
{
    IO_BIT_SET(pWDOG->CTRL, WDOG_INT_EN);
}



/**
  * @brief  Close wdog interrupt bit
  * @param  pWDOG WDOG Instance
  * @retval None
  */
__STATIC_INLINE void  LL_WDOG_EWI_Disable(WDOG_Device_T *pWDOG)
{
    IO_BIT_CLEAR(pWDOG->CTRL, WDOG_INT_EN);
}

/**
  * @brief  Start window wdog
  * @param  pWDOG WDOG Instance
  * @retval None
  */
__STATIC_INLINE void  LL_WDOG_WIN_Enable(WDOG_Device_T *pWDOG)
{
    IO_BIT_SET(pWDOG->CTRL, WDOG_WINDOW_EN);
}


/**
  * @brief  Stop window wdog
  * @param  pWDOG WDOG Instance
  * @retval None
  */
__STATIC_INLINE void  LL_WDOG_WIN_Disable(WDOG_Device_T *pWDOG)
{
    IO_BIT_CLEAR(pWDOG->CTRL, WDOG_WINDOW_EN);
}


/**
  * @brief  Get wdog count value
  * @param  pWDOG WDOG Instance
  * @retval return count value
  */
__STATIC_INLINE uint32_t  LL_WDOG_CountVal(WDOG_Device_T *pWDOG)
{
    uint32_t res = 0;

    res = pWDOG->VAL;
    return res;
}

/**
  * @brief  Get wdog's flag about out of the window counter
  * @param  pWDOG WDOG Instance
  * @retval return count value
  */
__STATIC_INLINE uint32_t  LL_WDOG_GetWinFlag(WDOG_Device_T *pWDOG)
{
    uint32_t res = 0;

    res = (pWDOG->CTRL & 0X04);
    return res;
}


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_WWDG_H_ */
