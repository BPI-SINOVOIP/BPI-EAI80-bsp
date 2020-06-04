
#ifndef __SOC_GM6721_H__
#define __SOC_GM6721_H__
#ifdef __cplusplus
extern "C" {
#endif

#include<gm_common.h>

#if 0

#define __CM4_REV                          1U /**< Core revision r0p1 */
#define __MPU_PRESENT                      1U /**< GM6721X provides an MPU */
#define __NVIC_PRIO_BITS                   4U /**< GM6721X uses 4 bits for the piority levels */
#define __Vendor_SysTickConfig             0U /**< Set to 1 if different sysTick config is used */
#define __FPU_PRESENT                      1U /**< FPU present */


/* Cortex-M4 processor and core peripherals */
#include "core_cm4.h"

/**
 *for specil user want to operate device reg direct
 *for example for fpga test.maybe user want to rw
 *reg directly for qucikly test.
 */
#if defined(USE_DEVICE_LL_DIRECT)
    #include<driver/uart/gm_ll_uart.h>
    /*add other modulis gm_ll_xx.h*/
#endif


void SystemInit(void);
#else

/* GM6721 Interrupt Number Definitions */
typedef enum
{
    /******  cortex-m4 processor exceptions numbers ****************************************************************/
    NonMaskableInt_IRQn                   = -14, /**< 2 Non Maskable Interrupt */
    MemoryManagement_IRQn                 = -12, /**< 4 Cortex-M4 Memory Management Interrupt */
    BusFault_IRQn                         = -11, /**< 5 Cortex-M4 Bus Fault Interrupt */
    UsageFault_IRQn                       = -10, /**< 6 Cortex-M4 Usage Fault Interrupt */
    SVCall_IRQn                           = -5,  /**< 11 Cortex-M4 SV Call Interrupt */
    DebugMonitor_IRQn                     = -4,  /**< 12 Cortex-M4 Debug Monitor Interrupt */
    PendSV_IRQn                           = -2,  /**< 14 Cortex-M4 Pend SV Interrupt */
    SysTick_IRQn                          = -1,  /**< 15 Cortex-M4 System Tick Interrupt */
    
    /******  GM6721X specific interrupt numbers **********************************************************************/
    WDOG_IRQn                             = 0,   /**< WDOG interrupt */
    LVD_IRQn                              = 1,   /**< LVD interrupt */
    RTC_IRQn                              = 2,   /**< RTC interrupt */
    Flash_IRQn                            = 3,   /**< Flash interrupt */
    DMA1_IRQn                             = 4,   /**< DMA1 interrupt */
    GPIOA_IRQn                            = 5,   /**< GPIOA interrupt */
    GPIOB_IRQn                            = 6 ,  /**< GPIOB interrupt */
    GPIOC_IRQn                            = 7,   /**< GPIOC interrupt */
    GPIOD_IRQn                            = 8,   /**< GPIOD interrupt */
    ACMP_IRQn                             = 9,   /**< ACMP interrupt */
    ADC1_IRQn                             = 10,  /**< ADC0 interrupt */
    ETM2_IRQn                             = 11,  /**< ETM2 interrupt */
    ETM21_IRQn                            = 12,  /**< ETM21 interrupt */
    ETM22_IRQn                            = 13,  /**< ETM22 interrupt */
    LPETM_IRQn                            = 14,  /**< LPETM interrupt */
    I2C0_IRQn                             = 15,  /**< I2C0 interrupt */
    I2C1_IRQn                             = 16,  /**< I2C1 interrupt */
    I2C2_IRQn                             = 17,  /**< I2C2 interrupt */
    UART0_IRQn                            = 18,  /**< UART0 interrupt */
    UART1_IRQn                            = 19,  /**< UART1 interrupt */
    UART2_IRQn                            = 20,  /**< UART2 interrupt */
    SPI0_IRQn                             = 21,  /**< SPI0 interrupt */
    DMA2_IRQn                             = 22,  /**< DMA2 interrupt */
    CRC_IRQn                              = 23,  /**< CRC interrupt */
    USB_IRQn                              = 24,  /**< USB interrupt */
    BASETIMER_IRQn                        = 25,  /**< BASETIMER interrupt */
    KBI_IRQn                              = 26,  /**< KBI interrupt */
    SPIFLASH_IRQn                         = 27,  /**< SPIFLASH interrupt */
    KDP_IRQn                              = 28,  /**< KDP interrupt */
    LCD_IRQn                              = 29,  /**< LCD interrupt */
    SPI1_IRQn                             = 30,  /**< SPI1 interrupt */
    SPI2_IRQn                             = 31,  /**< SPI2 interrupt */
    SPI3_IRQn                             = 32,  /**< SPI3 interrupt */
    SPI4_IRQn                             = 33,  /**< SPI4 interrupt */
    SPI5_IRQn                             = 34,  /**< SPI5 interrupt */
    SAIA_IRQn                             = 35,  /**< SAIA interrupt */
    SAIB_IRQn                             = 36,  /**< SAIB interrupt */
    IWDG_IRQn                             = 37,  /**< IWDG interrupt */
    TRNG_IRQn                             = 38,  /**< TRNG interrupt */
    GPIOE_IRQn                            = 39,  /**< GPIOE interrupt */
    GPIOF_IRQn                            = 40,  /**< GPIOF interrupt */
    GPIGG_IRQn                            = 41,  /**< GPIOG interrupt */
    GPIGH_IRQn                            = 42,  /**< GPIOH interrupt */
    GPIOI_IRQn                            = 43,  /**< GPIOI interrupt */
    GPIGJ_IRQn                            = 44,  /**< GPIOJ interrupt */
    GPIGK_IRQn                            = 45,  /**< GPIOK interrupt */
    IPM_IRQn                              = 46,  /**< IPM interrupt */
    VIN_IRQn                              = 47,  /**< VIN interrupt */
    VOUT_IRQn                             = 48,  /**< VOUT interrupt */
    ADC2_IRQn                             = 49,  /**< ADC2 interrupt */
    ADC3_IRQn                              = 50,  /**< ADC3 interrupt */
    CAN0_IRQn                             = 51,  /**< CAN0 interrupt */
    CAN1_IRQn                             = 52,  /**< CAN1 interrupt */
    FMC_IRQn                              = 53,  /**< FMC interrupt */
    SLAVE_IRQn                            = 70,  /**< SLAVE CPU interrupt */
    MASTER_IRQn                           = 71,  /**< MASTER CPU interrupt */
} IRQn_Type;



/** Interrupt Vector Macro */
#ifndef NonMaskableInt_IRQn_INTVEC
#define NonMaskableInt_IRQn_INTVEC     02
#else
#define INTVEC_02   NonMaskableInt_IRQn_INTVEC
#endif

#ifndef HardFault_IRQn_INTVEC
#define HardFault_IRQn_INTVEC          03
#else
#define INTVEC_03   HardFault_IRQn_INTVEC
#endif

#ifndef SVCall_IRQn_INTVEC
#define SVCall_IRQn_INTVEC             11
#else
#define INTVEC_11   SVCall_IRQn_INTVEC
#endif

#ifndef PendSV_IRQn_INTVEC
#define PendSV_IRQn_INTVEC             14
#else
#define INTVEC_14   PendSV_IRQn_INTVEC
#endif

#ifndef SysTick_IRQn_INTVEC
#define SysTick_IRQn_INTVEC             15
#else
#define INTVEC_15   SysTick_IRQn_INTVEC
#endif

#ifndef WDOG_IRQn_INTVEC
#define WDOG_IRQn_INTVEC                16
#else
#define INTVEC_16   WDOG_IRQn_INTVEC
#endif
#ifndef LVD_IRQn_INTVEC
#define LVD_IRQn_INTVEC                 17
#else
#define INTVEC_17   LVD_IRQn_INTVEC
#endif
#ifndef RTC_IRQn_INTVEC
#define RTC_IRQn_INTVEC                 18
#else
#define INTVEC_18   RTC_IRQn_INTVEC
#endif
#ifndef Flash_IRQn_INTVEC
#define Flash_IRQn_INTVEC               19
#else
#define INTVEC_19   Flash_IRQn_INTVEC
#endif
#ifndef DMA1_IRQn_INTVEC
#define DMA1_IRQn_INTVEC                 20
#else
#define INTVEC_20   DMA1_IRQn_INTVEC
#endif
#ifndef GPIO0_IRQn_INTVEC
#define GPIO0_IRQn_INTVEC               21
#else
#define INTVEC_21   GPIO0_IRQn_INTVEC
#endif
#ifndef GPIO1_IRQn_INTVEC
#define GPIO1_IRQn_INTVEC               22
#else
#define INTVEC_22   GPIO1_IRQn_INTVEC
#endif
#ifndef GPIO2_IRQn_INTVEC
#define GPIO2_IRQn_INTVEC               23
#else
#define INTVEC_23   GPIO2_IRQn_INTVEC
#endif
#ifndef GPIO3_IRQn_INTVEC
#define GPIO3_IRQn_INTVEC               24
#else
#define INTVEC_24   GPIO3_IRQn_INTVEC
#endif
#ifndef ACMP_IRQn_INTVEC
#define ACMP_IRQn_INTVEC                25
#else
#define INTVEC_25   ACMP_IRQn_INTVEC
#endif
#ifndef ADC1_IRQn_INTVEC
#define ADC1_IRQn_INTVEC                 26
#else
#define INTVEC_26   ADC1_IRQn_INTVEC
#endif
#ifndef ETM0_IRQn_INTVEC
#define ETM0_IRQn_INTVEC                27
#else
#define INTVEC_27   ETM0_IRQn_INTVEC
#endif
#ifndef ETM1_IRQn_INTVEC
#define ETM1_IRQn_INTVEC                28
#else
#define INTVEC_28   ETM1_IRQn_INTVEC
#endif
#ifndef ETM2_IRQn_INTVEC
#define ETM2_IRQn_INTVEC                29
#else
#define INTVEC_29   ETM2_IRQn_INTVEC
#endif
#ifndef ETM3_IRQn_INTVEC
#define ETM3_IRQn_INTVEC                30
#else
#define INTVEC_30   ETM3_IRQn_INTVEC
#endif
#ifndef I2C0_IRQn_INTVEC
#define I2C0_IRQn_INTVEC                31
#else
#define INTVEC_31   I2C0_IRQn_INTVEC
#endif
#ifndef I2C1_IRQn_INTVEC
#define I2C1_IRQn_INTVEC                32
#else
#define INTVEC_32   I2C1_IRQn_INTVEC
#endif
#ifndef I2C2_IRQn_INTVEC
#define I2C2_IRQn_INTVEC                33
#else
#define INTVEC_33   I2C2_IRQn_INTVEC
#endif
#ifndef UART0_IRQn_INTVEC
#define UART0_IRQn_INTVEC               34
#else
#define INTVEC_34   UART0_IRQn_INTVEC
#endif
#ifndef UART1_IRQn_INTVEC
#define UART1_IRQn_INTVEC               35
#else
#define INTVEC_35   UART1_IRQn_INTVEC
#endif
#ifndef UART2_IRQn_INTVEC
#define UART2_IRQn_INTVEC               36
#else
#define INTVEC_36   UART2_IRQn_INTVEC
#endif
#ifndef SPI0_IRQn_INTVEC
#define SPI0_IRQn_INTVEC               37
#else
#define INTVEC_37   SPI0_IRQn_INTVEC
#endif
#ifndef DMA2_IRQn_INTVEC
#define DMA2_IRQn_INTVEC                38
#else
#define INTVEC_38   DMA2_IRQn_INTVEC
#endif
#ifndef CRC_IRQn_INTVEC
#define CRC_IRQn_INTVEC                39
#else
#define INTVEC_39   CRC_IRQn_INTVEC
#endif
#ifndef ETM4_IRQn_INTVEC
#define ETM4_IRQn_INTVEC               40
#else
#define INTVEC_40   ETM4_IRQn_INTVEC
#endif
#ifndef ETM5_IRQn_INTVEC
#define ETM5_IRQn_INTVEC               41
#else
#define INTVEC_41   ETM5_IRQn_INTVEC
#endif
#ifndef GPIO4_IRQn_INTVEC
#define GPIO4_IRQn_INTVEC               42
#else
#define INTVEC_42   GPIO4_IRQn_INTVEC
#endif
#ifndef GPIO5_IRQn_INTVEC
#define GPIO5_IRQn_INTVEC               43
#else
#define INTVEC_43   GPIO5_IRQn_INTVEC
#endif
#ifndef CAN_IRQn_INTVEC
#define CAN_IRQn_INTVEC                 44
#else
#define INTVEC_44   CAN_IRQn_INTVEC
#endif
#ifndef LCD_IRQn_INTVEC
#define LCD_IRQn_INTVEC                 45
#else
#define INTVEC_45   LCD_IRQn_INTVEC
#endif
#ifndef SPI1_IRQn_INTVEC
#define SPI1_IRQn_INTVEC                46
#else
#define INTVEC_46  SPI1_IRQn_INTVEC
#endif

#ifndef ADC2_IRQn_INTVEC
#define ADC2_IRQn_INTVEC                 65
#else
#define INTVEC_65   ADC2_IRQn_INTVEC
#endif


#ifndef ADC3_IRQn_INTVEC
#define ADC3_IRQn_INTVEC                 66
#else
#define INTVEC_66   ADC3_IRQn_INTVEC
#endif

/* defautlt irq priority */
#define WDOG_IRQ_PRIORITY                  0 /**< WDOG interrupt */
#define LVD_IRQ_PRIORITY                   0 /**< LVD nterrupt */
#define RTC_IRQ_PRIORITY                   0 /**< RTC    interrupt */
#define Flash_IRQ_PRIORITY                 0 /**< Flash  interrupt */
#define DMA1_IRQ_PRIORITY                  0 /**< DMA0   interrupt */
#define GPIOA_IRQ_PRIORITY                 0 /**< GPIOA  interrupt */
#define GPIOB_IRQ_PRIORITY                 0 /**< GPIOB  interrupt */
#define GPIOC_IRQ_PRIORITY                 0 /**< GPIOC  interrupt */
#define GPIOD_IRQ_PRIORITY                 0 /**< GPIOD  interrupt */
#define ACMP_IRQ_PRIORITY                  0 /**< ACMP   interrupt */
#define ADC0_IRQ_PRIORITY                  0 /**< ADC0   interrupt */
#define ETM2_IRQ_PRIORITY                  0 /**< ETM2   interrupt */
#define ETM21_IRQ_PRIORITY                 0 /**< ETM21  interrupt */
#define ETM22_IRQ_PRIORITY                 0 /**< ETM22  interrupt */
#define LPETM_IRQ_PRIORITY                 0 /**< LPETM  interrupt */
#define I2C0_IRQ_PRIORITY                  0 /**< I2C0   interrupt */
#define I2C1_IRQ_PRIORITY                  0 /**< I2C1   interrupt */
#define I2C2_IRQ_PRIORITY                  0 /**< I2C2     interrupt */
#define UART0_IRQ_PRIORITY                 0 /**< UART0     interrupt */
#define UART1_IRQ_PRIORITY                 0 /**< UART1     interrupt */
#define UART2_IRQ_PRIORITY                 0 /**< UART2     interrupt */
#define UART3_IRQ_PRIORITY                 0 /**< UART2     interrupt */
#define SPI0_IRQ_PRIORITY                  0 /**< SPI0      interrupt */
#define DMA2_IRQ_PRIORITY                  0 /**< DAM1      interrupt */
#define CRC_IRQ_PRIORITY                   0 /**< CRC       interrupt */
#define USB_IRQ_PRIORITY                   0 /**< USB       interrupt */
#define BASETIMER_IRQ_PRIORITY             0 /**< BASETIMER interrupt */
#define KBI_IRQ_PRIORITY                   0 /**< KBI interrupt */
#define SPIFLASH_IRQ_PRIORITY              0 /**< SPIFLASH interrupt */
#define KDP_IRQ_PRIORITY                   0 /**< KDP interrupt */
#define LCD_IRQ_PRIORITY                   0 /**< LCD interrupt */
#define SPI1_IRQ_PRIORITY                  0 /**< SPI1 interrupt */
#define SPI2_IRQ_PRIORITY                  0 /**< SPI2 interrupt */
#define SPI3_IRQ_PRIORITY                  0 /**< SPI3 interrupt */
#define SPI4_IRQ_PRIORITY                  0 /**< SPI4 interrupt */
#define SPI5_IRQ_PRIORITY                  0 /**< SPI5 interrupt */
#define SAIA_IRQ_PRIORITY                  0 /**< SAIA interrupt */
#define SAIB_IRQ_PRIORITY                  0 /**< SAIB interrupt */
#define IWDG_IRQ_PRIORITY                  0 /**< IWDG interrupt */
#define TRNG_IRQ_PRIORITY                  0 /**< TRNG interrupt */
#define GPIOE_IRQ_PRIORITY                 0 /**< GPIOE interrupt */
#define GPIOF_IRQ_PRIORITY                 0 /**< GPIOF interrupt */
#define GPIGG_IRQ_PRIORITY                 0 /**< GPIOG interrupt */
#define GPIGH_IRQ_PRIORITY                 0 /**< GPIOH interrupt */
#define GPIOI_IRQ_PRIORITY                 0 /**< GPIOI interrupt */
#define GPIGJ_IRQ_PRIORITY                 0 /**< GPIOJ interrupt */
#define GPIGK_IRQ_PRIORITY                 0 /**< GPIOK interrupt */
#define IPM_IRQ_PRIORITY                   0 /**< IPM interrupt */
#define VIN_IRQ_PRIORITY                   0 /**< VIN interrupt */
#define VOUT_IRQ_PRIORITY                  0 /**< VOUT interrupt */
#define ADC1_IRQ_PRIORITY                  0 /**< ADC1 interrupt */
#define ADC2IRQ_PRIORITY                   0 /**< ADC2 interrupt */
#define CAN0_IRQ_PRIORITY                  0 /**< CAN0 interrupt */
#define CAN1_IRQ_PRIORITY                  0 /**< CAN1 interrupt */
#define FMC_IRQ_PRIORITY                   0 /**< FMC interrupt */
#define SLAVE_IRQ_PRIORITY                 0 /**< SLAVE interrupt */
#define MASTER_IRQ_PRIORITY                0 /**< MASTER interrupt */

#define SPI_FLASH_BASE                     (0x10000000U)
#define INFORMATION_PAGE_BASE              (0x1FFF0000U)

/*!< sram  memory map */
#define  SRAM0_BASE                        (0x20000000U)
#define  SRAM1_BASE                        (0x20018000U)
#define  SRAM2_BASE                        (0x2001C000U)
#define  KDP310_SRAM_BASE                  (0x20020000U)
#define  CCM_SRAM_BASE                     (0x21000000U)


/*!< peripheral memory map */
#define APB1_PERIPHERAL_BASE               (0x40000000U)
#define APB2_PERIPHERAL_BASE               (0x40030000U)
#define APB3_PERIPHERAL_BASE               (0x40040000U)
#define AHB1_PERIPHERAL_BASE               (0x40010000U)
#define AHB2_PERIPHERAL_BASE               (0x50000000U)
#define FMC_BASE                           (0x60000000U)
#define CPU_BASE                           (0xE0042020U)
/*!< AHB1 peripheral memory map */
#define GPIOA_BASE                         (AHB1_PERIPHERAL_BASE + 0x00000000U)
#define GPIOB_BASE                         (AHB1_PERIPHERAL_BASE + 0x00001000U)
#define GPIOC_BASE                         (AHB1_PERIPHERAL_BASE + 0x00002000U)
#define GPIOD_BASE                         (AHB1_PERIPHERAL_BASE + 0x00003000U)
#define GPIOE_BASE                         (AHB1_PERIPHERAL_BASE + 0x00004000U)
#define GPIOF_BASE                         (AHB1_PERIPHERAL_BASE + 0x00005000U)
#define GPIOG_BASE                         (AHB1_PERIPHERAL_BASE + 0x00006000U)
#define GPIOH_BASE                         (AHB1_PERIPHERAL_BASE + 0x00007000U)
#define GPIOI_BASE                         (AHB1_PERIPHERAL_BASE + 0x00008000U)
#define GPIOJ_BASE                         (AHB1_PERIPHERAL_BASE + 0x00009000U)
#define GPIOK_BASE                         (AHB1_PERIPHERAL_BASE + 0x0000A000U)
#define DMA1_BASE                          (AHB1_PERIPHERAL_BASE + 0x00010000U)
#define DMA2_BASE                          (AHB1_PERIPHERAL_BASE + 0x00011000U)
#define IPM_BASE                           (AHB1_PERIPHERAL_BASE + 0x00014000U)
#define VIN_BASE                           (AHB1_PERIPHERAL_BASE + 0x00015000U)
#define VOUT_BASE                          (AHB1_PERIPHERAL_BASE + 0x00016000U)
#define KDP310_BASE                        (AHB1_PERIPHERAL_BASE + 0x05FF0000U)

/*!< APB1 peripheral memory map */
#define SYS_BASE                           (APB1_PERIPHERAL_BASE)
#define EFLASH_CTRL_BASE                   (APB1_PERIPHERAL_BASE + 0x00001000U)
#define I2C0_BASE                          (APB1_PERIPHERAL_BASE + 0x0000E000U)
#define I2C1_BASE                          (APB1_PERIPHERAL_BASE + 0x0000F000U)
#define I2C2_BASE                          (APB1_PERIPHERAL_BASE + 0x00002000U)

#define SPI0_BASE                          (APB1_PERIPHERAL_BASE + 0x00003000U)
#define UART0_BASE                         (APB1_PERIPHERAL_BASE + 0x00004000U)
#define UART1_BASE                         (APB1_PERIPHERAL_BASE + 0x00005000U)
#define UART2_BASE                         (APB1_PERIPHERAL_BASE + 0x00006000U)

#define WWDG_BASE                          (APB1_PERIPHERAL_BASE + 0x00008000U)



//#define ETM0_BASE                        (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM1_BASE                        (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM2_BASE                        (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM3_BASE                        (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM4_BASE                        (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM5_BASE                        (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM6_BASE                        (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM7_BASE                        (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM8_BASE                        (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM9_BASE                        (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM10_BASE                       (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM11_BASE                       (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM12_BASE                       (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM13_BASE                       (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM14_BASE                       (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define ETM15_BASE                       (APB1_PERIPHERAL_BASE + ) /*todo by ic spec*/

#define RTC_TOP_BASE                       (APB1_PERIPHERAL_BASE + 0x00009000U)
#define RTC0_BASE                          (APB1_PERIPHERAL_BASE + 0x00009000U)
#define RTC1_BASE                          (APB1_PERIPHERAL_BASE + 0x00009400U)
#define RTC2_BASE                          (APB1_PERIPHERAL_BASE + 0x00009800U)
#define CRC_BASE                           (APB1_PERIPHERAL_BASE + 0x0000A000U)
#define HIRX_BASE                          (APB1_PERIPHERAL_BASE + 0x0000B000U)


#define ADC1_BASE                          (APB1_PERIPHERAL_BASE + 0x0000C000) /*todo by ic spec*/
#define ADC2_BASE                          (APB1_PERIPHERAL_BASE + 0x0000C100) /*todo by ic spec*/
#define ADC3_BASE                          (APB1_PERIPHERAL_BASE + 0x0000C200) /*todo by ic spec*/
#define ADC_COM_BASE                       (APB1_PERIPHERAL_BASE + 0x0000C900u)/* Base addresses of ADC common registers */

#define ACMP_BASE                          (APB1_PERIPHERAL_BASE + 0x0000D000U)

/*!< APB2 peripheral memory map */
//#define CAN0_BASE                        (APB2_PERIPHERAL_BASE + ) /*todo by ic spec*/
//#define CAN1_BASE                        (APB2_PERIPHERAL_BASE + ) /*todo by ic spec*/
#define SPI1_BASE                          (APB2_PERIPHERAL_BASE + 0x00001000U)
#define SPI2_BASE                          (APB2_PERIPHERAL_BASE + 0x00003000U)
#define SPI3_BASE                          (APB2_PERIPHERAL_BASE + 0x00004000U)
#define SPI4_BASE                          (APB2_PERIPHERAL_BASE + 0x0000B000U)
#define SPI5_BASE                          (APB2_PERIPHERAL_BASE + 0x0000C000U)
#define SAI_BASE                           (APB2_PERIPHERAL_BASE + 0x00002000U)
#define KBI_BASE                           (APB2_PERIPHERAL_BASE + 0x00005000U)
#define BASETIMER_BASE                     (APB2_PERIPHERAL_BASE + 0x00006000U)
#define PES_BASE                           (APB2_PERIPHERAL_BASE + 0x00007000U)
#define ALWAYS_ON_BASE                     (APB2_PERIPHERAL_BASE + 0x00009000U)
#define LP_STM_BASE                        (APB2_PERIPHERAL_BASE + 0x0000A000U)
#define UART3_BASE                         (APB2_PERIPHERAL_BASE + 0x0000D000U)
#define IWDG_BASE                          (APB2_PERIPHERAL_BASE + 0x0000E000U)
#define BKSRAM_BASE                        (APB2_PERIPHERAL_BASE + 0x0000F000U)

/*!< AHB2 peripheral memory map */
#define USB_BASE                           (AHB2_PERIPHERAL_BASE)

/*!< AHB3 peripheral memory map */
#define TRNG_BASE                          (APB3_PERIPHERAL_BASE + 0x00005000U)

/*!< FMC memory map */
#define PNOR_FLASH_BASE                    (FMC_BASE)
#define PSRAM_BASE                         (FMC_BASE)
#define NAND_FLASH_BASE                    (FMC_BASE+ 0x10000000U)
#define SDRAM_BASE                         (FMC_BASE+ 0x60000000U)

/*!< slave boot map address */
#define SLAVE_SRAM_VECTOR_ADDR             (0x2001c000U)

#if 0
/**
 * Cortex M0 Core Configuration
 */
#define __CM0_REV                      0         /**< Core Revision r0p0 */
#define __MPU_PRESENT                  0         /**< Not provide MPU */
#define __NVIC_PRIO_BITS               2         /**< Uses 2 Bits for the Priority Levels */
#define __Vendor_SysTickConfig         0         /**< Standard ARM SysTick is used  */

#include "core_cm0.h"                            /**< Core Peripheral Access Layer */
#else
#define __CM4_REV                          1U /**< Core revision r0p1 */
#define __MPU_PRESENT                      1U /**< GM6721X provides an MPU */
#define __NVIC_PRIO_BITS                   4U /**< GM6721X uses 4 bits for the piority levels */
#define __Vendor_SysTickConfig             0U /**< Set to 1 if different sysTick config is used */
#define __FPU_PRESENT                      1U /**< FPU present */

#include "core_cm4.h"

#endif

//#include <arch/arm_m0.h>
#include <arch/interrupt.h>
#include <arch/systick.h>
#include <driver/sys.h>
#include <../driver/uart/gm_ll_uart.h>
#include <../driver/nvm/flash_dev.h>
#include <../driver/gpio/gpio_dev.h>
#include <../driver/dma/gm_ll_dma_dev.h>
#include <../driver/adc/gm_ll_adc.h>
#include <driver/clock.h>

#if 0
/*!< peripheral memory map */
#define APB1_PERIPHERAL_BASE               (0x40000000U)
#define APB2_PERIPHERAL_BASE               (0x40030000U)
#define APB3_PERIPHERAL_BASE               (0x40040000U)
#define AHB1_PERIPHERAL_BASE               (0x40010000U)
#define AHB2_PERIPHERAL_BASE               (0x50000000U)
//#define FMC_BASE                           (0x60000000U)
//#define CPU_BASE                           (0xE0042020U)

//#define ADC1_BASE                          (APB1_PERIPHERAL_BASE + 0x0000C000) /*todo by ic spec*/
//#define ADC2_BASE                          (APB1_PERIPHERAL_BASE + 0x0000C100) /*todo by ic spec*/
//#define ADC3_BASE                          (APB1_PERIPHERAL_BASE + 0x0000C200) /*todo by ic spec*/
//#define ADC_COM_BASE                       (APB1_PERIPHERAL_BASE + 0x0000C900u)/* Base addresses of ADC common registers */

void SystemInit(void);
#endif

#endif

#ifdef __cplusplus
}
#endif
#endif /* #ifndef __SOC_GM6721_H__ */










































