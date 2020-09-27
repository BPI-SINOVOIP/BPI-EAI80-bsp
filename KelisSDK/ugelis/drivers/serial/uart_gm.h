/*
 * Copyright (c) 2016, Gree Instruments Incorporated
 *
 */
#ifndef __UART_GM_H__
#define __UART_GM_H__

#ifdef __cplusplus
     extern "C"
     {
#endif

/*gm include files*/
#include <gm_soc.h>
#include <gm_hal_uart.h>

#define  UART0_DEV_NAME     uart0_dev
#define  UART1_DEV_NAME     uart1_dev
#define  UART2_DEV_NAME     uart2_dev
#define  UART3_DEV_NAME     uart3_dev
#define  UART4_DEV_NAME     uart4_dev
#define  UART5_DEV_NAME     uart5_dev
#define  UART6_DEV_NAME     uart6_dev
#define  UART7_DEV_NAME     uart7_dev

#ifdef CONFIG_UART0
#define  UART0_DRV_NAME     CONFIG_UART0
#endif
#ifdef CONFIG_UART1
#define  UART1_DRV_NAME     CONFIG_UART1
#endif
#ifdef CONFIG_UART2
#define  UART2_DRV_NAME     CONFIG_UART2
#endif
#ifdef CONFIG_UART3
#define  UART3_DRV_NAME     CONFIG_UART3
#endif
#ifdef CONFIG_UART4
#define  UART4_DRV_NAME     CONFIG_UART4
#endif
#ifdef CONFIG_UART5
#define  UART5_DRV_NAME     CONFIG_UART5
#endif
#ifdef CONFIG_UART6
#define  UART6_DRV_NAME     CONFIG_UART6
#endif
#ifdef CONFIG_UART7
#define  UART7_DRV_NAME     CONFIG_UART7
#endif

struct uart_gm_dev_data_t {
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    uart_irq_callback_t cb; /* Callback function pointer */
#endif /* CONFIG_UART_INTERRUPT_DRIVEN */
    uint8_t uart_id;
    UART_Handle_T huart;
};

#define DEV_CFG(dev) \
    ((const struct uart_device_config * const)((dev)->config->config_info))
#define DEV_DATA(dev) \
    ((struct uart_gm_dev_data_t *)((dev)->driver_data))
#define DEV_STRUCT(dev) \
    ((UART_Device_T *)((DEV_CFG(dev))->config_u.base))


#ifdef __cplusplus
    }
#endif

#endif /*__UART_GM_H__*/


