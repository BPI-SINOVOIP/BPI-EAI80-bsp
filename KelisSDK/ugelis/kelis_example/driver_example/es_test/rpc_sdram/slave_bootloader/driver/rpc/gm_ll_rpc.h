/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart_dev.h
 *
 * @author        TerryLee
 *
 * @version       1.0.0
 *
 * @date          2018/04/03
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *        porting to ugelis system.
 */
#ifndef _GM_LL_RPC_H_
#define _GM_LL_RPC_H_

#include <gm_common.h>
#include <gm_soc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************************for temp :wait sys common api ready*********************************/

#define RESET_CTR_DEV                               (*((volatile uint32_t *)(0x40000070U)))
#define CLK_CTR_DEV                                 (*((volatile uint32_t *)(0x40000080U)))
#define IRQ_SLAVE_ST_DEV                            (*((volatile uint32_t *)(0x40000204U)))
#define IRQ_MASTER_ST_DEV                           (*((volatile uint32_t *)(0x40000208U)))


#define RESET_SLAVE_POS                             ((uint32_t) 0)
#define RESET_SLAVE_MASK                            ((uint32_t) ((uint32_t)1 << RESET_SLAVE_POS))
#define LL_RPC_MASTER_HOLD_SLAVE()                  IO_BIT_CLEAR(RESET_CTR_DEV, RESET_SLAVE_MASK)
#define LL_RPC_MASTER_START_SLAVE()                 IO_BIT_SET(RESET_CTR_DEV, RESET_SLAVE_MASK)

#define CLK_SLAVE_POS                               ((uint32_t) 0)
#define CLK_SLAVE_MASK                              ((uint32_t) ((uint32_t)1 << CLK_SLAVE_POS))
#define LL_RPC_MASTER_ENABLE_SLAVE_CLK()            IO_BIT_SET(CLK_CTR_DEV, CLK_SLAVE_MASK)
#define LL_RPC_MASTER_DISABLE_SLAVE_CLK()           IO_BIT_CLEAR(CLK_CTR_DEV, CLK_SLAVE_MASK)

#define IRQ_RPC_SLAVE_ST_POS                        ((uint32_t) 0)
#define IRQ_RPC_SLAVE_ST_MASK                       ((uint32_t) ((uint32_t)1 << IRQ_RPC_SLAVE_ST_POS))

#define LL_RPC_GET_SLAVE_IRQ_ST()                   (IRQ_SLAVE_ST_DEV & IRQ_RPC_SLAVE_ST_MASK)
#define LL_RPC_SLAVE_CLEAR_IRQ()                    (IRQ_SLAVE_ST_DEV = IRQ_RPC_SLAVE_ST_MASK)

#define IRQ_RPC_MASTER_ST_POS                       ((uint32_t) 0)
#define IRQ_RPC_MASTER_ST_MASK                      ((uint32_t) ((uint32_t)1 << IRQ_RPC_MASTER_ST_POS))
#define LL_RPC_MASTER_IRQ_ST()                      (IRQ_MASTER_ST_DEV & IRQ_RPC_MASTER_ST_MASK)
#define LL_RPC_MASTER_CLEAR_IRQ()                   (IRQ_MASTER_ST_DEV = IRQ_RPC_MASTER_ST_MASK)


/**************************** RPC register bitfiled *********************************/
#define LL_RPC_GET_DATA(pDev)                       (pDev->MRD)
#define LL_RPC_SET_DATA(pDev,val)                   (pDev->MWR = val)

#define LL_RPC_SLAVE_ID                             (0x0fffabc1)
#define LL_RPC_MASTER_ID                            (0x0fffabc0)
#define LL_RPC_ID_ADDR                              (0xE0042040U)
#define LL_RPC_GET_CPU_ID()                         IO_READL(LL_RPC_ID_ADDR)



#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_LL_UART_H_ */
