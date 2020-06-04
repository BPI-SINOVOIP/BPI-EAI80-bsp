
#ifndef __SOC_GM6721_H__
#define __SOC_GM6721_H__
#ifdef __cplusplus
extern "C" {
#endif

#include<gm_common.h>


#define __CM4_REV                          1U /**< Core revision r0p1 */
#define __MPU_PRESENT                      1U /**< GM6721X provides an MPU */
#define __NVIC_PRIO_BITS                   3U /**< GM6721X uses 3 bits for the piority levels */
#define __Vendor_SysTickConfig             0U /**< Set to 1 if different sysTick config is used */
#define __FPU_PRESENT                      1U /**< FPU present */
#if !defined(__IO)
#define __IO  volatile
#endif
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
    DMA1_IRQn                             = 4,   /**< DMA0 interrupt */
    GPIOA_IRQn                            = 5,   /**< GPIOA interrupt */
    GPIOB_IRQn                            = 6,   /**< GPIOB interrupt */
    GPIOC_IRQn                            = 7,   /**< GPIOC interrupt */
    GPIOD_IRQn                            = 8,   /**< GPIOD interrupt */
    ACMP_IRQn                             = 9,   /**< ACMP interrupt */
    ADC0_IRQn                             = 10,  /**< ADC0 interrupt */
    TIM1_IRQn                             = 11,  /**< TIM1 interrupt */
    TIM2_IRQn                             = 12,  /**< TIM2 interrupt */
    TIM3_IRQn                             = 13,  /**< TIM3 interrupt */
    BASETIMER1_IRQn                       = 14,  /**< LPETM interrupt */
    I2C0_IRQn                             = 15,  /**< I2C0 interrupt */
    I2C1_IRQn                             = 16,  /**< I2C1 interrupt */
    I2C2_IRQn                             = 17,  /**< I2C2 interrupt */
    UART0_IRQn                            = 18,  /**< UART0 interrupt */
    UART1_IRQn                            = 19,  /**< UART1 interrupt */
    UART2_IRQn                            = 20,  /**< UART2 interrupt */
    SPI0_IRQn                             = 21,  /**< SPI0 interrupt */
    DMA2_IRQn                             = 22,  /**< DAM1 interrupt */
    CRC_IRQn                              = 23,  /**< CRC interrupt */
    USB_IRQn                              = 24,  /**< USB interrupt */
    BASETIMER2_IRQn                       = 25,  /**< BASETIMER interrupt */
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
    GPIOG_IRQn                            = 41,  /**< GPIOG interrupt */
    GPIOH_IRQn                            = 42,  /**< GPIOH interrupt */
    GPIOI_IRQn                            = 43,  /**< GPIOI interrupt */
    GPIOJ_IRQn                            = 44,  /**< GPIOJ interrupt */
    GPIOK_IRQn                            = 45,  /**< GPIOK interrupt */
    IMP_IRQn                              = 46,  /**< IMP interrupt */
    VIN_IRQn                              = 47,  /**< VIN interrupt */
    VOUT_IRQn                             = 48,  /**< VOUT interrupt */
    ADC1_IRQn                             = 49,  /**< ADC1 interrupt */
    ADC2_IRQn                             = 50,  /**< ADC2 interrupt */
    CAN0_IRQn                             = 51,  /**< CAN0 interrupt */
    CAN1_IRQn                             = 52,  /**< CAN1 interrupt */
    PMU_IRQn                              = 53,  /**< PMU  interrupt */
    TIM4_IRQn                             = 54,  /**< TIM4 interrupt*/
    DAC0_IRQn                             = 55,  /**< DAC0 interrupt*/
    DAC1_IRQn                             = 56,  /**< DAC1 interrupt*/
    OTP_IRQn                              = 57,  /**< OTP interrupt */
    CRYP_IRQn                             = 58,  /**< CRYP interrupt */
    HASH_IRQn                             = 59,  /**< HASH interrupt */
    UART3_IRQn                            = 60,  /**< UART3 interrupt */
    UART4_IRQn                            = 61,  /**< UART4 interrupt */
    UART5_IRQn                            = 62,  /**< UART5 interrupt */
    UART6_IRQn                            = 63,  /**< UART6 interrupt */
    UART7_IRQn                            = 64,  /**< UART7 interrupt */
    TIM5_IRQn                             = 66,  /**< TIM5 interrupt*/
    TIM8_IRQn                             = 67,  /**< TIM8 interrupt*/
    S_SYSRESETREQ                         = 68,  /**< SLAVE cpu system rest request interrupt */
    SLOCKUP_IRQn                          = 69,  /**< SLAVE cpu lock interrupt */
    SLAVE_IRQn                            = 70,  /**< SLAVE CPU interrupt */
    MASTER_IRQn                           = 71,  /**< MASTER CPU interrupt */
    TIM9_IRQn                             = 72,  /**< TIM9 interrupt*/
    TIM10_IRQn                            = 73,  /**< TIM10 interrupt*/
    TIM11_IRQn                            = 74,  /**< TIM11 interrupt*/
    TIM12_IRQn                            = 75,  /**< TIM12 interrupt*/
    TIM13_IRQn                            = 76,  /**< TIM13 interrupt*/
    TIM14_IRQn                            = 77,  /**< TIM14 interrupt*/
    PES_IRQn                              = 78,  /**< PES interrupt*/
    MMC0_IRQn                             = 79,  /**< MMC0 interrupt */
} IRQn_Type;

/* defautlt irq priority */
#ifndef WDOG_IRQ_PRIORITY
#define WDOG_IRQ_PRIORITY                  3 /**< WDOG interrupt */
#endif

#ifndef LVD_IRQ_PRIORITY
#define LVD_IRQ_PRIORITY                   3 /**< LVD nterrupt */
#endif

#ifndef RTC_IRQ_PRIORITY
#define RTC_IRQ_PRIORITY                   3 /**< RTC    interrupt */
#endif

#ifndef Flash_IRQ_PRIORITY
#define Flash_IRQ_PRIORITY                 3 /**< Flash  interrupt */
#endif

#ifndef DMA1_IRQ_PRIORITY
#define DMA1_IRQ_PRIORITY                  3 /**< DMA0   interrupt */
#endif

#ifndef GPIOA_IRQ_PRIORITY
#define GPIOA_IRQ_PRIORITY                 3 /**< GPIOA  interrupt */
#endif

#ifndef GPIOB_IRQ_PRIORITY
#define GPIOB_IRQ_PRIORITY                 3 /**< GPIOB  interrupt */
#endif

#ifndef GPIOC_IRQ_PRIORITY
#define GPIOC_IRQ_PRIORITY                 3 /**< GPIOC  interrupt */
#endif

#ifndef GPIOD_IRQ_PRIORITY
#define GPIOD_IRQ_PRIORITY                 3 /**< GPIOD  interrupt */
#endif

#ifndef ACMP_IRQ_PRIORITY
#define ACMP_IRQ_PRIORITY                  3 /**< ACMP   interrupt */
#endif

#ifndef ADC0_IRQ_PRIORITY
#define ADC0_IRQ_PRIORITY                  1 /**< ADC0   interrupt */
#endif

#ifndef TIM1_IRQ_PRIORITY
#define TIM1_IRQ_PRIORITY                  0 /**< TIM1   interrupt */
#endif

#ifndef TIM2_IRQ_PRIORITY
#define TIM2_IRQ_PRIORITY                  0 /**< TIM2   interrupt */
#endif

#ifndef TIM3_IRQ_PRIORITY
#define TIM3_IRQ_PRIORITY                  0 /**< TIM3   interrupt */
#endif

#ifndef TIM4_IRQ_PRIORITY
#define TIM4_IRQ_PRIORITY                  0 /**< TIM4   interrupt */
#endif

#ifndef TIM5_IRQ_PRIORITY
#define TIM5_IRQ_PRIORITY                  0 /**< TIM5   interrupt */
#endif

#ifndef TIM8_IRQ_PRIORITY
#define TIM8_IRQ_PRIORITY                  0 /**< TIM8   interrupt */
#endif

#ifndef TIM9_IRQ_PRIORITY
#define TIM9_IRQ_PRIORITY                  0 /**< TIM9   interrupt */
#endif

#ifndef TIM10_IRQ_PRIORITY
#define TIM10_IRQ_PRIORITY                 0 /**< TIM10  interrupt */
#endif

#ifndef TIM11_IRQ_PRIORITY
#define TIM11_IRQ_PRIORITY                 0 /**< TIM11  interrupt */
#endif

#ifndef TIM12_IRQ_PRIORITY
#define TIM12_IRQ_PRIORITY                 0 /**< TIM12  interrupt */
#endif

#ifndef TIM13_IRQ_PRIORITY
#define TIM13_IRQ_PRIORITY                 0 /**< TIM13  interrupt */
#endif

#ifndef TIM14_IRQ_PRIORITY
#define TIM14_IRQ_PRIORITY                 0 /**< TIM14  interrupt */
#endif

#ifndef LPETM_IRQ_PRIORITY
#define LPETM_IRQ_PRIORITY                 3 /**< LPETM  interrupt */
#endif

#ifndef I2C0_IRQ_PRIORITY
#define I2C0_IRQ_PRIORITY                  3 /**< I2C0   interrupt */
#endif

#ifndef I2C1_IRQ_PRIORITY
#define I2C1_IRQ_PRIORITY                  3 /**< I2C1   interrupt */
#endif

#ifndef I2C2_IRQ_PRIORITY
#define I2C2_IRQ_PRIORITY                  3 /**< I2C2     interrupt */
#endif

#ifndef UART0_IRQ_PRIORITY
#define UART0_IRQ_PRIORITY                 3 /**< UART0     interrupt */
#endif
#ifndef UART1_IRQ_PRIORITY
#define UART1_IRQ_PRIORITY                 3 /**< UART1     interrupt */
#endif
#ifndef UART2_IRQ_PRIORITY
#define UART2_IRQ_PRIORITY                 3 /**< UART2     interrupt */
#endif
#ifndef UART3_IRQ_PRIORITY
#define UART3_IRQ_PRIORITY                 3 /**< UART3     interrupt */
#endif
#ifndef UART4_IRQ_PRIORITY
#define UART4_IRQ_PRIORITY                 3 /**< UART4     interrupt */
#endif
#ifndef UART5_IRQ_PRIORITY
#define UART5_IRQ_PRIORITY                 3 /**< UART5     interrupt */
#endif
#ifndef UART6_IRQ_PRIORITY
#define UART6_IRQ_PRIORITY                 3 /**< UART6     interrupt */
#endif
#ifndef UART7_IRQ_PRIORITY
#define UART7_IRQ_PRIORITY                 3 /**< UART7     interrupt */
#endif

#ifndef SPI0_IRQ_PRIORITY
#define SPI0_IRQ_PRIORITY                  3 /**< SPI0      interrupt */
#endif

#ifndef DMA2_IRQ_PRIORITY
#define DMA2_IRQ_PRIORITY                  3 /**< DAM1      interrupt */
#endif

#ifndef CRC_IRQ_PRIORITY
#define CRC_IRQ_PRIORITY                   3 /**< CRC       interrupt */
#endif

#ifndef USB_IRQ_PRIORITY
#define USB_IRQ_PRIORITY                   3 /**< USB       interrupt */
#endif

#ifndef BASETIMER_IRQ_PRIORITY
#define BASETIMER_IRQ_PRIORITY             3 /**< BASETIMER interrupt */
#endif

#ifndef KBI_IRQ_PRIORITY
#define KBI_IRQ_PRIORITY                   3 /**< KBI interrupt */
#endif

#ifndef SPIFLASH_IRQ_PRIORITY
#define SPIFLASH_IRQ_PRIORITY              3 /**< SPIFLASH interrupt */
#endif

#ifndef KDP_IRQ_PRIORITY
#define KDP_IRQ_PRIORITY                   3 /**< KDP interrupt */
#endif

#ifndef LCD_IRQ_PRIORITY
#define LCD_IRQ_PRIORITY                   3 /**< LCD interrupt */
#endif

#ifndef SPI1_IRQ_PRIORITY
#define SPI1_IRQ_PRIORITY                  3 /**< SPI1 interrupt */
#endif

#ifndef SPI2_IRQ_PRIORITY
#define SPI2_IRQ_PRIORITY                  3 /**< SPI2 interrupt */
#endif

#ifndef SPI3_IRQ_PRIORITY
#define SPI3_IRQ_PRIORITY                  3 /**< SPI3 interrupt */
#endif

#ifndef SPI4_IRQ_PRIORITY
#define SPI4_IRQ_PRIORITY                  3 /**< SPI4 interrupt */
#endif

#ifndef SPI5_IRQ_PRIORITY
#define SPI5_IRQ_PRIORITY                  3 /**< SPI5 interrupt */
#endif

#ifndef SAIA_IRQ_PRIORITY
#define SAIA_IRQ_PRIORITY                  3 /**< SAIA interrupt */
#endif

#ifndef SAIB_IRQ_PRIORITY
#define SAIB_IRQ_PRIORITY                  3 /**< SAIB interrupt */
#endif

#ifndef IWDG_IRQ_PRIORITY
#define IWDG_IRQ_PRIORITY                  3 /**< IWDG interrupt */
#endif

#ifndef TRNG_IRQ_PRIORITY
#define TRNG_IRQ_PRIORITY                  3 /**< TRNG interrupt */
#endif

#ifndef GPIOE_IRQ_PRIORITY
#define GPIOE_IRQ_PRIORITY                 3 /**< GPIOE interrupt */
#endif

#ifndef GPIOF_IRQ_PRIORITY
#define GPIOF_IRQ_PRIORITY                 3 /**< GPIOF interrupt */
#endif

#ifndef GPIGG_IRQ_PRIORITY
#define GPIGG_IRQ_PRIORITY                 3 /**< GPIOG interrupt */
#endif

#ifndef GPIGH_IRQ_PRIORITY
#define GPIGH_IRQ_PRIORITY                 3 /**< GPIOH interrupt */
#endif

#ifndef GPIOI_IRQ_PRIORITY
#define GPIOI_IRQ_PRIORITY                 3 /**< GPIOI interrupt */
#endif

#ifndef GPIGJ_IRQ_PRIORITY
#define GPIGJ_IRQ_PRIORITY                 3 /**< GPIOJ interrupt */
#endif

#ifndef GPIGK_IRQ_PRIORITY
#define GPIGK_IRQ_PRIORITY                 3 /**< GPIOK interrupt */
#endif

#ifndef IMP_IRQ_PRIORITY
#define IMP_IRQ_PRIORITY                   3 /**< IMP interrupt */
#endif

#ifndef VIN_IRQ_PRIORITY
#define VIN_IRQ_PRIORITY                   3 /**< VIN interrupt */
#endif

#ifndef VOUT_IRQ_PRIORITY
#define VOUT_IRQ_PRIORITY                  3 /**< VOUT interrupt */
#endif

#ifndef ADC1_IRQ_PRIORITY
#define ADC1_IRQ_PRIORITY                  3 /**< ADC1 interrupt */
#endif

#ifndef ADC2_IRQ_PRIORITY
#define ADC2_IRQ_PRIORITY                   3 /**< ADC2 interrupt */
#endif

#ifndef CAN0_IRQ_PRIORITY
#define CAN0_IRQ_PRIORITY                  3 /**< CAN0 interrupt */
#endif

#ifndef CAN1_IRQ_PRIORITY
#define CAN1_IRQ_PRIORITY                  3 /**< CAN1 interrupt */
#endif

#ifndef FMC_IRQ_PRIORITY
#define FMC_IRQ_PRIORITY                   3 /**< FMC interrupt */
#endif

#ifndef OTP_IRQ_PRIORITY
#define OTP_IRQ_PRIORITY                   3 /**< OTP interrupt */
#endif

#ifndef CRYP_IRQ_PRIORITY
#define CRYP_IRQ_PRIORITY                  3 /**< crypto interrupt */
#endif

#ifndef HASH_IRQ_PRIORITY
#define HASH_IRQ_PRIORITY                  3 /**< HASH interrupt */
#endif

#ifndef MMC0_IRQ_PRIORITY
#define MMC0_IRQ_PRIORITY                  3 /**< MMC0 interrupt */
#endif

#ifndef SLAVE_LOCKUP_IRQ_PRIORITY
#define SLAVE_LOCKUP_IRQ_PRIORITY          3 /**< SLAVE lockup interrupt */
#endif

#ifndef SLAVE_SYS_RESET_IRQ_PRIORITY
#define SLAVE_SYS_RESET_IRQ_PRIORITY       3 /**< SLAVE lockup interrupt */
#endif

#ifndef SLAVE_IRQ_PRIORITY
#define SLAVE_IRQ_PRIORITY                 0 /**< SLAVE interrupt */
#endif

#ifndef MASTER_IRQ_PRIORITY
#define MASTER_IRQ_PRIORITY                0 /**< MASTER interrupt */
#endif


#ifndef PMU_IRQ_PRIORITY
#define PMU_IRQ_PRIORITY                   3 /**< MASTER interrupt */
#endif

#ifndef BASETIMER1_IRQ_PRIORITY
#define BASETIMER1_IRQ_PRIORITY            0 /**< BASETIMER interrupt */
#endif

#ifndef BASETIMER2_IRQ_PRIORITY
#define BASETIMER2_IRQ_PRIORITY            0 /**< BASETIMER interrupt */
#endif

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
#define IMP_BASE                           (AHB1_PERIPHERAL_BASE + 0x00014000U)
#define VIN_BASE                           (AHB1_PERIPHERAL_BASE + 0x00015000U)
#define VOUT_BASE                          (AHB1_PERIPHERAL_BASE + 0x00016000U)
#define KDP310_BASE                        (AHB1_PERIPHERAL_BASE + 0x05FF0000U)
/** Base address of crypto engine */
#define CRYPTO_BASE                        (AHB1_PERIPHERAL_BASE + 0x00017000U)
#define HASH_BASE                          (AHB1_PERIPHERAL_BASE + 0x00017200U)


/*!< APB1 peripheral memory map */
#define SYS_BASE                           (APB1_PERIPHERAL_BASE)
#define LVD_BASE                           (APB1_PERIPHERAL_BASE + 0x00000138U)
#define EFLASH_CTRL_BASE                   (APB1_PERIPHERAL_BASE + 0x00001000U)
#define I2C0_BASE                          (APB1_PERIPHERAL_BASE + 0x0000E000U)
#define I2C1_BASE                          (APB1_PERIPHERAL_BASE + 0x0000F000U)
#define I2C2_BASE                          (APB1_PERIPHERAL_BASE + 0x00002000U)

#define SPI0_BASE                          (APB1_PERIPHERAL_BASE + 0x00003000U)

#define UART0_BASE                         (APB1_PERIPHERAL_BASE + 0x00004000U)
#define UART1_BASE                         (APB1_PERIPHERAL_BASE + 0x00005000U)
#define UART2_BASE                         (APB1_PERIPHERAL_BASE + 0x00006000U)
#define UART3_BASE                         (APB1_PERIPHERAL_BASE + 0x0003D000U)
#define UART4_BASE                         (APB1_PERIPHERAL_BASE + 0x00041000U)
#define UART5_BASE                         (APB1_PERIPHERAL_BASE + 0x00042000U)
#define UART6_BASE                         (APB1_PERIPHERAL_BASE + 0x00043000U)
#define UART7_BASE                         (APB1_PERIPHERAL_BASE + 0x00044000U)

#define IWDG_BASE                          (APB1_PERIPHERAL_BASE + 0x00008000U)
#define MMC0_BASE                          (APB2_PERIPHERAL_BASE + 0x0000b000U)


#define TIM1_BASE                          (APB1_PERIPHERAL_BASE + 0x00007000)
#define TIM2_BASE                          (APB1_PERIPHERAL_BASE + 0x00007100)
#define TIM3_BASE                          (APB1_PERIPHERAL_BASE + 0x00007200)
#define TIM4_BASE                          (APB1_PERIPHERAL_BASE + 0x00007300)
#define TIM5_BASE                          (APB1_PERIPHERAL_BASE + 0x00007400)
#define TIM8_BASE                          (APB1_PERIPHERAL_BASE + 0x00007500)
#define TIM9_BASE                          (APB1_PERIPHERAL_BASE + 0x00007600)
#define TIM10_BASE                         (APB1_PERIPHERAL_BASE + 0x00007700)
#define TIM11_BASE                         (APB1_PERIPHERAL_BASE + 0x00007800)
#define TIM12_BASE                         (APB1_PERIPHERAL_BASE + 0x00007900)
#define TIM13_BASE                         (APB1_PERIPHERAL_BASE + 0x00007A00)
#define TIM14_BASE                         (APB1_PERIPHERAL_BASE + 0x00007B00)

#define BASETIMER1_BASE                    (APB2_PERIPHERAL_BASE + 0x00006000)
#define BASETIMER2_BASE                    (APB2_PERIPHERAL_BASE + 0x00006400)


#define RTC_TOP_BASE                       (APB1_PERIPHERAL_BASE + 0x00009000U)
#define RTC0_BASE                          (APB1_PERIPHERAL_BASE + 0x00009000U)
#define RTC1_BASE                          (APB1_PERIPHERAL_BASE + 0x00009400U)
#define RTC2_BASE                          (APB1_PERIPHERAL_BASE + 0x00009800U)
#define CRC_BASE                           (APB1_PERIPHERAL_BASE + 0x0000A000U)
#define HIRX_BASE                          (APB1_PERIPHERAL_BASE + 0x0000B000U)


#define ADC0_BASE                          (APB1_PERIPHERAL_BASE + 0x0000C000) /*todo by ic spec*/
#define ADC1_BASE                          (APB1_PERIPHERAL_BASE + 0x0000C100) /*todo by ic spec*/
#define ADC2_BASE                          (APB1_PERIPHERAL_BASE + 0x0000C200) /*todo by ic spec*/
#define ADC_COM_BASE                       (APB1_PERIPHERAL_BASE + 0x0000C900u)/* Base addresses of ADC common registers */

#define ACMP_BASE                          (APB1_PERIPHERAL_BASE + 0x0000D004U) /* 0x4000D004 */

#define DAC0_BASE                          (APB1_PERIPHERAL_BASE + 0x0000B000U)
#define DAC1_BASE                          (APB1_PERIPHERAL_BASE + 0x0000B100U) /*0x4000B100*/
#define DAC_COM_BASE                       (APB1_PERIPHERAL_BASE + 0x0000B900U)


/*!< APB2 peripheral memory map */
#define CAN0_BASE                          (APB2_PERIPHERAL_BASE + 0x0) /*todo by ic spec*/
#define CAN1_BASE                          (APB2_PERIPHERAL_BASE + 0x00000800U)/*0x40030800u*/
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
#define WWDG_BASE                          (APB2_PERIPHERAL_BASE + 0x0000E000U)
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


/*!< sys reg*/
typedef struct
{
    __IO uint32_t CID;                          /**< Chip ID Register: 0x00*/
    __IO uint32_t SRS;                          /**< System Reset Status Register: 0x04*/
    __IO uint32_t CFGTRG;                       /**< System configuration Trigger Register: 0x8 */
    __IO uint32_t CFGVAL;                       /**< System Configuration Value Register: 0xc */
    __IO uint32_t CFGSTS;                       /**< System Configuration Status Register: 0x10 */
    __IO uint32_t CFGMISC;                      /**< System Miscellaneous Configuration Register: 0x14 */
    __IO uint32_t CFGPM;                        /**< Power Mode Configuration register: 0x18 */
    __IO uint32_t RESERVED0[2];                 /**< Reserved: 0x1c~0x20 */
    __IO uint32_t TICKCALIB;                    /**< SystemTick Calibration register: 0x24 */
    __IO uint32_t RESERVED1[6];                 /**< Reserved: 0x28~0x3c */
    __IO uint32_t PWRDBG;                       /**< Power Mode FSM Debug Register: 0x40 */
    __IO uint32_t DBGCPU[4];                    /**< CPU access record register: 0x44~0x50 */
    __IO uint32_t WDTRCD;                       /**< WatchDog Record Register: 0x54 */
    __IO uint32_t WDTDBG;                       /**< WatchDog Debug Register: 0x58 */
    __IO uint32_t RESERVED2[5];                 /**< Reserved: 0x5c~0x6c */
    __IO uint32_t IPSWRST[3];                   /**< IP SW Reset: 0x70~0x78 */
    __IO uint32_t RESERVED3;                    /**< Reserved: 0x7c */
    __IO uint32_t ENABLECLK[3];                 /**< Clock Enable Register: 0x80~0x88 */
    __IO uint32_t RESERVED4;                    /**< Reserved: 0x8c */
    __IO uint32_t SYSCLK;                       /**< System Clock Control Register: 0x90 */
    __IO uint32_t CLKSRC[10];                   /**< Clock Source Control Register: 0x94~0xB8 */
    __IO uint32_t DBCS[2];                      /**< Debounce Clock Control Register: 0xBC~0xC0 */
    __IO uint32_t RESERVED5[27];                /**< Reserved: 0xC4~0x12c */
    __IO uint32_t LDOTRIM;                      /**< LDO Trimming control Register: 0x130 */
    __IO uint32_t TEMPSENSOR;                   /**< Temperture Sensor Control Register: 0x134 */
    __IO uint32_t CFGLVD1;                      /**< LVD1 Configuration Register: 0x138 */
    __IO uint32_t STUSLVD1;                     /**< LVD1 Status Register: 0x13c */
    __IO uint32_t CFGLVD2;                      /**< LVD2 Configuration Register: 0x140 */
    __IO uint32_t STUSLVD2;                     /**< LVD2 Status Register: 0x144 */
    __IO uint32_t CFGLVD3;                      /**< LVD3 Configuration Register: 0x148 */
    __IO uint32_t STUSLVD3;                     /**< LVD3 Status Register: 0x14c */
    __IO uint32_t RESERVED6[8];                 /**< Reserved: 0x150~0x16c */
    __IO uint32_t HIRCCR;                       /**< HIRC Control Register: 0x170 */
    __IO uint32_t LIRCCR;                       /**< LIRC Control Register: 0x174 */
    __IO uint32_t HOSCCR;                       /**< HOSC Control Register: 0x178 */
    __IO uint32_t LOSCCR;                       /**< LOSC Control Register: 0x17c */
    __IO uint32_t PLL[2];                       /**< PLL Control Register: 0x180~0x184 */
    __IO uint32_t PLLKDP[2];                    /**< PLLKDP Control Register: 0x188~0x18c */
    __IO uint32_t PLLI2S[2];                    /**< PLLI2S Control Register: 0x190~0x194 */
    __IO uint32_t PLLSAI[2];                    /**< PLLSAI Control Register: 0x198~0x19c */
    __IO uint32_t PLLREF;                       /**< PLL Reference Clock Select: 0x1A0 */
    __IO uint32_t CMUST;                        /**< CMU Clock Status Register: 0x1A4 */
    __IO uint32_t RESERVED7[8];                 /**< Reserved: 0x1A8~0x1C4 */
    __IO uint32_t HOSCM[2];                     /**< HOSC Monitor Control Register: 0x1C8~0x1CC */
    __IO uint32_t RESERVED8[12];                /**< Reserved: 0x1D0~0x1FC */
    __IO uint32_t APBCONT;                      /**< APB Timeout Counter Register: 0x200 */
    __IO uint32_t MCM4EVNT;                     /**< Main CM4 TX Event Register: 0x204 */
    __IO uint32_t SCM4EVNT;                     /**< Slave CM4 TX Event Register: 0x208 */
    __IO uint32_t RESERVED9[5];                 /**< Reserved: 0x20C~0x21C */
    __IO uint32_t IOFLTBP[6];                   /**< IO Debounce bypass control: 0x220~0x234 */
    __IO uint32_t IOODC[6];                     /**< IO Open Drain Control Register: 0x238~0x24c */
    __IO uint32_t IOINPUTE[6];                  /**< IO Pad Input Enable Control Register: 0x250~0x264 */
    __IO uint32_t IOPULLUP[6];                  /**< IO Pull Up Control Register: 0x268~0x27c */
    __IO uint32_t IOPULLDOWN[6];                /**< IO Pull Down Control Register: 0x280~0x294 */
    __IO uint32_t IODS[11];                     /**< IO Driven Strength Control: 0x298~0x2C0 */
    __IO uint32_t IOMODE[11];                   /**< IO Mode Control: 0x2C4~0x2EC */
    __IO uint32_t IOAFR[22];                    /**< IO Alternate Function Control: 0x2F0~0x344 */
} Sys_Device_T;

#define SYS_DEV                               ((Sys_Device_T *)(SYS_BASE))    /* sys reg base */



/**
 * sys reg map
 */


/**************************** sys power register bitfiled *********************************/
#define SYS_POWER_IO_WKEN_POS                  ((uint32_t) 19)                                               /* power wake up event from xpa0 pos */
#define SYS_POWER_IO_WKEN_MASK                 ((uint32_t) ((uint32_t)0x01 << SYS_POWER_IO_WKEN_POS))         /* power wake up event from xpa0 */
#define SYS_POWER_IO_WKEDG_POS                 ((uint32_t) 18)                                    /*  Wake up event from XPA0 polarity selection pos */
#define SYS_POWER_IO_WKEDG_MASK                ((uint32_t) ((uint32_t)0x01 << SYS_POWER_IO_WKEDG_POS))        /* Wake up event from XPA0 polarity selection */

#define SYS_POWER_PES_WKEN_POS                 ((uint32_t) 17)                                        /*  Wake up event from PES channel output enable bit pos */
#define SYS_POWER_PES_WKEN_MASK                ((uint32_t) ((uint32_t)0x01 << SYS_POWER_PES_WKEN_POS))        /*  Wake up event from PES channel output enable bit */

#define SYS_POWER_PES_WKEDG_SEL_POS            ((uint32_t) 16)                                        /*  Wake up event from PES channel output polarity selection pos */
#define SYS_POWER_PES_WKEDG_SEL_MASK           ((uint32_t) ((uint32_t)0x01 << SYS_POWER_PES_WKEDG_SEL_POS))   /*  Wake up event from PES channel output polarity selection */

#define SYS_POWER_BKP_WR_UNLOCK_POS            ((uint32_t) 12)                                        /*  BackUp domain registers are protected against  pos */
#define SYS_POWER_BKP_WR_UNLOCK_MASK           ((uint32_t) ((uint32_t)0x01 << SYS_POWER_BKP_WR_UNLOCK_POS))   /*  BackUp domain registers are protected against  */


#define SYS_POWER_ISO_HOLD_EN_POS              ((uint32_t) 10)                                /*  Isolation latch holding or not when SDP power on finish pos */
#define SYS_POWER_ISO_HOLD_EN_MASK             ((uint32_t) ((uint32_t)0x01 << SYS_POWER_ISO_HOLD_EN_POS))    /*   Isolation latch holding or not when SDP power on finish */

#define SYS_POWER_MEM_RET_EN_POS               ((uint32_t) 9)                                         /*  SRAM retention function enable or not when SDP power on finish pos */
#define SYS_POWER_MEM_RET_EN_MASK              ((uint32_t) ((uint32_t)0x01 << SYS_POWER_MEM_RET_EN_POS))     /*   SRAM retention function enable or not when SDP power on finish */

#define SYS_POWER_LDO_HW_TRIM_POS              ((uint32_t) 8)                                     /*   LDO hardware trim flow enable during lower power mode pos */
#define SYS_POWER_LDO_HW_TRIM_MASK             ((uint32_t) ((uint32_t)0x01 << SYS_POWER_LDO_HW_TRIM_POS))    /*   LDO hardware trim flow enable during lower power mode */

#define SYS_POWER_POWER_MODE_EN_POS            ((uint32_t) 4)                                                 /*   Low power mode setting pos */
#define SYS_POWER_POWER_MODE_EN_MASK           ((uint32_t) ((uint32_t)0x03 << SYS_POWER_POWER_MODE_EN_POS))   /*  Low power mode setting */

#define SYS_POWER_POWER_PMUENABLE_POS          ((uint32_t) 0)                                                  /*  Low power mode setting pos */
#define SYS_POWER_POWER_PMUENABLE_MASK         ((uint32_t) ((uint32_t)1 << SYS_POWER_POWER_PMUENABLE_POS))   /*   Low power mode setting */

/**************************** sys reset register1 bitfiled *********************************/
#define SYS_IP_SW_RESET_SM4_POS                ((uint32_t) 0)
#define SYS_IP_SW_RESET_SM4_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SM4_POS))


#define SYS_IP_SW_RESET_GPIO_POS               ((uint32_t) 1)
#define SYS_IP_SW_RESET_GPIO_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_GPIO_POS))


#define SYS_IP_SW_RESET_DMA_POS                ((uint32_t) 2)
#define SYS_IP_SW_RESET_DMA_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_DMA_POS))


#define SYS_IP_SW_RESET_CRC_POS                ((uint32_t) 3)
#define SYS_IP_SW_RESET_CRC_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_CRC_POS))


#define SYS_IP_SW_RESET_TRNG_POS               ((uint32_t) 4)
#define SYS_IP_SW_RESET_TRNG_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TRNG_POS))


#define SYS_IP_SW_RESET_CRP_POS                ((uint32_t) 5)
#define SYS_IP_SW_RESET_CRP_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_CRP_POS))


#define SYS_IP_SW_RESET_NF_POS                 ((uint32_t) 6)
#define SYS_IP_SW_RESET_NF_SET_MASK            ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_NF_POS))


#define SYS_IP_SW_RESET_SDR_POS                ((uint32_t) 7)
#define SYS_IP_SW_RESET_SDR_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SDR_POS))


#define SYS_IP_SW_RESET_BKD_POS                ((uint32_t) 8)
#define SYS_IP_SW_RESET_BKD_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_BKD_POS))


#define SYS_IP_SW_RESET_LPSTM_POS              ((uint32_t) 9)
#define SYS_IP_SW_RESET_LPSTM_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_LPSTM_POS))


#define SYS_IP_SW_RESET_ACMP_POS               ((uint32_t) 10)
#define SYS_IP_SW_RESET_ACMP_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_ACMP_POS))


#define SYS_IP_SW_RESET_DAC_POS                ((uint32_t) 11)
#define SYS_IP_SW_RESET_DAC_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_DAC_POS))


#define SYS_IP_SW_RESET_ADC_POS                ((uint32_t) 12)
#define SYS_IP_SW_RESET_ADC_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_ADC_POS))


#define SYS_IP_SW_RESET_STF_POS                ((uint32_t) 14)
#define SYS_IP_SW_RESET_STF_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_STF_POS))


#define SYS_IP_SW_RESET_KDPSRAM_POS            ((uint32_t) 15)
#define SYS_IP_SW_RESET_KDPSRAM_SET_MASK       ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_KDPSRAM_POS))


#define SYS_IP_SW_RESET_KDP_POS                ((uint32_t) 16)
#define SYS_IP_SW_RESET_KDP_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_KDP_POS))


#define SYS_IP_SW_RESET_IMP_POS                ((uint32_t) 17)
#define SYS_IP_SW_RESET_IMP_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_IMP_POS))


#define SYS_IP_SW_RESET_VIN_POS                ((uint32_t) 18)
#define SYS_IP_SW_RESET_VIN_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_VIN_POS))


#define SYS_IP_SW_RESET_VOUT_POS               ((uint32_t) 19)
#define SYS_IP_SW_RESET_VOUT_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_VOUT_POS))

#define SYS_IP_SW_RESET_USB_POS                ((uint32_t) 24)
#define SYS_IP_SW_RESET_USB_SET_MASK           ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_USB_POS))



#define SYS_IP_SW_RESET_SDMMC_POS              ((uint32_t) 25)
#define SYS_IP_SW_RESET_SDMMC_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SDMMC_POS))


#define SYS_IP_SW_RESET_CAN1_POS               ((uint32_t) 28)
#define SYS_IP_SW_RESET_CAN1_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_CAN1_POS))



#define SYS_IP_SW_RESET_CAN2_POS               ((uint32_t) 29)
#define SYS_IP_SW_RESET_CAN2_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_CAN2_POS))


/**************************** sys reset register2 bitfiled *********************************/
#define SYS_IP_SW_RESET_SPI1_POS               ((uint32_t) 0)
#define SYS_IP_SW_RESET_SPI1_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SPI1_POS))


#define SYS_IP_SW_RESET_SPI2_POS               ((uint32_t) 1)
#define SYS_IP_SW_RESET_SPI2_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SPI2_POS))


#define SYS_IP_SW_RESET_SPI3_POS               ((uint32_t) 2)
#define SYS_IP_SW_RESET_SPI3_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SPI3_POS))


#define SYS_IP_SW_RESET_SPI4_POS               ((uint32_t) 3)
#define SYS_IP_SW_RESET_SPI4_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SPI4_POS))


#define SYS_IP_SW_RESET_SPI5_POS               ((uint32_t) 4)
#define SYS_IP_SW_RESET_SPI5_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SPI5_POS))


#define SYS_IP_SW_RESET_SPI6_POS               ((uint32_t) 5)
#define SYS_IP_SW_RESET_SPI6_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SPI6_POS))


#define SYS_IP_SW_RESET_I2C1_POS               ((uint32_t) 8)
#define SYS_IP_SW_RESET_I2C1_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_I2C1_POS))


#define SYS_IP_SW_RESET_I2C2_POS               ((uint32_t) 9)
#define SYS_IP_SW_RESET_I2C2_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_I2C2_POS))


#define SYS_IP_SW_RESET_I2C3_POS               ((uint32_t) 10)
#define SYS_IP_SW_RESET_I2C3_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_I2C3_POS))


#define SYS_IP_SW_RESET_BTM1_POS               ((uint32_t) 16)
#define SYS_IP_SW_RESET_BTM1_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_BTM1_POS))


#define SYS_IP_SW_RESET_BTM2_POS               ((uint32_t) 17)
#define SYS_IP_SW_RESET_BTM2_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_BTM2_POS))


#define SYS_IP_SW_RESET_SAIA_POS               ((uint32_t) 24)
#define SYS_IP_SW_RESET_SAIA_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SAIA_POS))


#define SYS_IP_SW_RESET_SAIB_POS               ((uint32_t) 25)
#define SYS_IP_SW_RESET_SAIB_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_SAIB_POS))


#define SYS_IP_SW_RESET_PMEM_POS               ((uint32_t) 28)
#define SYS_IP_SW_RESET_PMEM_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_PMEM_POS))


/**************************** sys reset register3 bitfiled *********************************/
#define SYS_IP_SW_RESET_UART1_POS              ((uint32_t) 0)
#define SYS_IP_SW_RESET_UART1_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_UART1_POS))


#define SYS_IP_SW_RESET_UART2_POS              ((uint32_t) 1)
#define SYS_IP_SW_RESET_UART2_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_UART2_POS))


#define SYS_IP_SW_RESET_UART3_POS              ((uint32_t) 2)
#define SYS_IP_SW_RESET_UART3_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_UART3_POS))


#define SYS_IP_SW_RESET_UART4_POS              ((uint32_t) 3)
#define SYS_IP_SW_RESET_UART4_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_UART4_POS))


#define SYS_IP_SW_RESET_UART5_POS              ((uint32_t) 4)
#define SYS_IP_SW_RESET_UART5_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_UART5_POS))


#define SYS_IP_SW_RESET_UART6_POS              ((uint32_t) 5)
#define SYS_IP_SW_RESET_UART6_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_UART6_POS))


#define SYS_IP_SW_RESET_UART7_POS              ((uint32_t) 6)
#define SYS_IP_SW_RESET_UART7_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_UART7_POS))


#define SYS_IP_SW_RESET_UART8_POS              ((uint32_t) 7)
#define SYS_IP_SW_RESET_UART8_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_UART8_POS))


#define SYS_IP_SW_RESET_TIM1_POS               ((uint32_t) 16)
#define SYS_IP_SW_RESET_TIM1_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM1_POS))


#define SYS_IP_SW_RESET_TIM2_POS               ((uint32_t) 17)
#define SYS_IP_SW_RESET_TIM2_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM2_POS))


#define SYS_IP_SW_RESET_TIM3_POS               ((uint32_t) 18)
#define SYS_IP_SW_RESET_TIM3_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM3_POS))


#define SYS_IP_SW_RESET_TIM4_POS               ((uint32_t) 19)
#define SYS_IP_SW_RESET_TIM4_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM4_POS))


#define SYS_IP_SW_RESET_TIM5_POS               ((uint32_t) 20)
#define SYS_IP_SW_RESET_TIM5_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM5_POS))


#define SYS_IP_SW_RESET_TIM8_POS               ((uint32_t) 21)
#define SYS_IP_SW_RESET_TIM8_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM8_POS))


#define SYS_IP_SW_RESET_TIM9_POS               ((uint32_t) 22)
#define SYS_IP_SW_RESET_TIM9_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM9_POS))


#define SYS_IP_SW_RESET_TIM10_POS              ((uint32_t) 23)
#define SYS_IP_SW_RESET_TIM10_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM10_POS))


#define SYS_IP_SW_RESET_TIM11_POS              ((uint32_t) 24)
#define SYS_IP_SW_RESET_TIM11_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM11_POS))


#define SYS_IP_SW_RESET_TIM12_POS              ((uint32_t) 25)
#define SYS_IP_SW_RESET_TIM12_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM12_POS))


#define SYS_IP_SW_RESET_TIM13_POS              ((uint32_t) 26)
#define SYS_IP_SW_RESET_TIM13_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM13_POS))


#define SYS_IP_SW_RESET_TIM14_POS              ((uint32_t) 27)
#define SYS_IP_SW_RESET_TIM14_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_IP_SW_RESET_TIM14_POS))


/**************************** sys  reset status register bitfiled *********************************/
#define SYS_RESET_STATUS_POR_VO_POS            ((uint32_t)0)
#define SYS_RESET_STATUS_POR_VO_SET_MASK       ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_POR_VO_POS))

#define SYS_RESET_STATUS_WDOG_POS              ((uint32_t)1)
#define SYS_RESET_STATUS_WDOG_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_WDOG_POS))

#define SYS_RESET_STATUS_LOCKUP_POS            ((uint32_t)2)
#define SYS_RESET_STATUS_LOCKUP_SET_MASK       ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_LOCKUP_POS))

#define SYS_RESET_STATUS_SYSRESETREQ_POS       ((uint32_t)3)
#define SYS_RESET_STATUS_SYSRESETREQ_SET_MASK  ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_SYSRESETREQ_POS))

#define SYS_RESET_STATUS_SDP_POS               ((uint32_t)4)
#define SYS_RESET_STATUS_SDP_SET_MASK          ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_SDP_POS))

#define SYS_RESET_STATUS_IWOG_POS              ((uint32_t)5)
#define SYS_RESET_STATUS_IWOG_SET_MASK         ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_IWOG_POS))

#define SYS_RESET_STATUS_BKD_RST_POS           ((uint32_t)6)
#define SYS_RESET_STATUS_BKD_RST_SET_MASK      ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_BKD_RST_POS))

#define SYS_RESET_STATUS_BKD_REG_POS           ((uint32_t)7)
#define SYS_RESET_STATUS_BKD_REG_SET_MASK      ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_BKD_REG_POS))

#define SYS_RESET_STATUS_FROM_STOP_POS         ((uint32_t)8)
#define SYS_RESET_STATUS_FROM_STOP_SET_MASK    ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_FROM_STOP_POS))

#define SYS_RESET_STATUS_FROM_STBY_POS         ((uint32_t)9)
#define SYS_RESET_STATUS_FROM_STBY_SET_MASK    ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_FROM_STBY_POS))

#define SYS_RESET_STATUS_FROM_SHDN_POS         ((uint32_t)10)
#define SYS_RESET_STATUS_FROM_SHDN_SET_MASK    ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_FROM_SHDN_POS))

#define SYS_RESET_STATUS_HOSC_LOSS_POS         ((volatile uint32_t)24)
#define SYS_RESET_STATUS_HOSC_LOSS_SET_MASK    ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_HOSC_LOSS_POS))

#define SYS_RESET_STATUS_PES_WK_POS            ((uint32_t)25)
#define SYS_RESET_STATUS_PES_WK_SET_MASK       ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_PES_WK_POS))

#define SYS_RESET_STATUS_IO_WK_POS             ((volatile uint32_t)26)
#define SYS_RESET_STATUS_IO_WK_SET_MASK        ((uint32_t) ((uint32_t)1 << SYS_RESET_STATUS_IO_WK_POS))


#if defined(CHIP_GM6721)
#define SYS_SRS_HOSC_LSFLAG              ((uint32_t)0x01000000)
#define SYS_IOMODE1_ENABLE_HOSCIN        ((uint32_t)0x00300000)
#define SYS_IOMODE1_ENABLE_HOSCOUT       ((uint32_t)0x00C00000)
#define SYS_IOMODE0_ENABLE_LOSCIN        ((uint32_t)0x0000C000)
#define SYS_IOMODE0_ENABLE_LOSCOUT       ((uint32_t)0x00030000)
#elif defined(CHIP_F6721B)
#define SYS_SRS_HOSC_LSFLAG              ((uint32_t)0x01000000)
#define SYS_IOMODE2_ENABLE_HOSCIN        ((uint32_t)0x00000002)
#define SYS_IOMODE2_ENABLE_HOSCIN_MSK    ((uint32_t)0x00000003)
#define SYS_IOMODE2_ENABLE_HOSCOUT       ((uint32_t)0x00000008)
#define SYS_IOMODE2_ENABLE_HOSCOUT_MSK   ((uint32_t)0x0000000C)
#define SYS_IOMODE1_ENABLE_LOSCIN        ((uint32_t)0x20000000)
#define SYS_IOMODE1_ENABLE_LOSCIN_MSK    ((uint32_t)0x30000000)
#define SYS_IOMODE1_ENABLE_LOSCOUT       ((uint32_t)0x80000000)
#define SYS_IOMODE1_ENABLE_LOSCOUT_MSK   ((uint32_t)0xC0000000)
#endif
/**************************************ENABLECLK[0] Register*******************************************************/
#define SYS_ENCLK0_SM4                   ((uint32_t)0x00000001)     /* Enable Slave CPU clock */
#define SYS_ENCLK0_GPIO                  ((uint32_t)0x00000002)     /* Enable GPIO clock */
#define SYS_ENCLK0_DMA                   ((uint32_t)0x00000004)     /* Enable DMA clock */
#define SYS_ENCLK0_CRC                   ((uint32_t)0x00000008)     /* Enable CRC clock */
#define SYS_ENCLK0_TRNG                  ((uint32_t)0x00000010)     /* Enable TRNG clock */
#define SYS_ENCLK0_CRYPTO                ((uint32_t)0x00000020)     /* Enable CRYPTO clock */
#define SYS_ENCLK0_FMC                   ((uint32_t)0x00000040)     /* Enable FMC clock */
#define SYS_ENCLK0_SDR                   ((uint32_t)0x00000080)     /* Enable SDRAM clock */
#define SYS_ENCLK0_RTC                   ((uint32_t)0x00000100)     /* Enable RTC clock */
#define SYS_ENCLK0_LPSTM                 ((uint32_t)0x00000200)     /* Enable LPSTM clock */
#define SYS_ENCLK0_ACMP                  ((uint32_t)0x00000400)     /* Enable ACMP clock */
#define SYS_ENCLK0_DAC                   ((uint32_t)0x00000800)     /* Enable DAC clock */
#define SYS_ENCLK0_ADC                   ((uint32_t)0x00001000)     /* Enable ADC clock */
#define SYS_ENCLK0_ADC2                  ((uint32_t)0x00002000)     /* Enable ADC2 clock */
#define SYS_ENCLK0_STF                   ((uint32_t)0x00004000)     /* Enable STF clock */
#define SYS_ENCLK0_KDPSRAM               ((uint32_t)0x00008000)     /* Enable KDPSRAM clock */
#define SYS_ENCLK0_KDP                   ((uint32_t)0x00010000)     /* Enable KDP clock */
#define SYS_ENCLK0_IMP                   ((uint32_t)0x00020000)     /* Enable IMP clock */
#define SYS_ENCLK0_VIN                   ((uint32_t)0x00040000)     /* Enable VIN clock */
#define SYS_ENCLK0_VOUT                  ((uint32_t)0x00080000)     /* Enable VOUT clock */
#define SYS_ENCLK0_ADC3                  ((uint32_t)0x00100000)     /* Enable ADC3 clock */
#define SYS_ENCLK0_USB                   ((uint32_t)0x01000000)     /* Enable USB clock */
#define SYS_ENCLK0_SDMMC                 ((uint32_t)0x02000000)     /* Enable SDMMC clock */
#define SYS_ENCLK0_CAN1                  ((uint32_t)0x10000000)     /* Enable CAN1 clock */
#define SYS_ENCLK0_CAN2                  ((uint32_t)0x20000000)     /* Enable CAN2 clock */

#define IS_ENCLK0_PERIPH(PERIPH)         ((((PERIPH) & 0xCCE00000) == 0x00) && ((PERIPH) != 0x00))
/**************************************ENABLECLK[1] Register***********************************************/
#define SYS_ENCLK1_SPI1                  ((uint32_t)0x00000001)     /* Enable SPI1 clock */
#define SYS_ENCLK1_SPI2                  ((uint32_t)0x00000002)     /* Enable SPI2 clock */
#define SYS_ENCLK1_SPI3                  ((uint32_t)0x00000004)     /* Enable SPI3 clock */
#define SYS_ENCLK1_SPI4                  ((uint32_t)0x00000008)     /* Enable SPI4 clock */
#define SYS_ENCLK1_SPI5                  ((uint32_t)0x00000010)     /* Enable SPI5 clock */
#define SYS_ENCLK1_SPI6                  ((uint32_t)0x00000020)     /* Enable SPI6 clock */
#define SYS_ENCLK1_I2C1                  ((uint32_t)0x00000100)     /* Enable I2C1 clock */
#define SYS_ENCLK1_I2C2                  ((uint32_t)0x00000200)     /* Enable I2C2 clock */
#define SYS_ENCLK1_I2C3                  ((uint32_t)0x00000400)     /* Enable I2C3 clock */
#define SYS_ENCLK1_BTM1                  ((uint32_t)0x00010000)     /* Enable BTM1 clock */
#define SYS_ENCLK1_BTM2                  ((uint32_t)0x00020000)     /* Enable BTM2 clock */
#define SYS_ENCLK1_SAIA                  ((uint32_t)0x01000000)     /* Enable SAIA clock */
#define SYS_ENCLK1_SAIB                  ((uint32_t)0x02000000)     /* Enable SAIB clock */
#define SYS_ENCLK1_PMEM                  ((uint32_t)0x10000000)     /* Enable PMEM clock */

#define IS_ENCLK1_PERIPH(PERIPH)         ((((PERIPH) & 0xECFCF8C0) == 0x00) && ((PERIPH) != 0x00))
/**************************************ENABLECLK[2] Register***************************************************/
#define SYS_ENCLK2_UART1                 ((uint32_t)0x00000001)     /* Enable USRT1 clock */
#define SYS_ENCLK2_UART2                 ((uint32_t)0x00000002)     /* Enable USRT2 clock */
#define SYS_ENCLK2_UART3                 ((uint32_t)0x00000004)     /* Enable USRT3 clock */
#define SYS_ENCLK2_UART4                 ((uint32_t)0x00000008)     /* Enable USRT4 clock */
#define SYS_ENCLK2_UART5                 ((uint32_t)0x00000010)     /* Enable USRT5 clock */
#define SYS_ENCLK2_UART6                 ((uint32_t)0x00000020)     /* Enable USRT6 clock */
#define SYS_ENCLK2_UART7                 ((uint32_t)0x00000040)     /* Enable USRT7 clock */
#define SYS_ENCLK2_UART8                 ((uint32_t)0x00000080)     /* Enable USRT8 clock */
#define SYS_ENCLK2_TIM1                  ((uint32_t)0x00010000)     /* Enable TIM1 clock */
#define SYS_ENCLK2_TIM2                  ((uint32_t)0x00020000)     /* Enable TIM2 clock */
#define SYS_ENCLK2_TIM3                  ((uint32_t)0x00040000)     /* Enable TIM3 clock */
#define SYS_ENCLK2_TIM4                  ((uint32_t)0x00080000)     /* Enable TIM4 clock */
#define SYS_ENCLK2_TIM5                  ((uint32_t)0x00100000)     /* Enable TIM5 clock */
#define SYS_ENCLK2_TIM8                  ((uint32_t)0x00200000)     /* Enable TIM10 clock */
#define SYS_ENCLK2_TIM9                  ((uint32_t)0x00400000)     /* Enable TIM11 clock */
#define SYS_ENCLK2_TIM10                 ((uint32_t)0x00800000)     /* Enable TIM10 clock */
#define SYS_ENCLK2_TIM11                 ((uint32_t)0x01000000)     /* Enable TIM11 clock */
#define SYS_ENCLK2_TIM12                 ((uint32_t)0x02000000)     /* Enable TIM12 clock */
#define SYS_ENCLK2_TIM13                 ((uint32_t)0x04000000)     /* Enable TIM13 clock */
#define SYS_ENCLK2_TIM14                 ((uint32_t)0x08000000)     /* Enable TIM14 clock */
#define SYS_ENCLK2_WWDT                  ((uint32_t)0x10000000)     /* Enable WWDT clock */

#define IS_ENCLK2_PERIPH(PERIPH)         ((((PERIPH) & 0xE000FF00) == 0x00) && ((PERIPH) != 0x00))
/****************************PLLREF register*********************************************/
#define SYS_PLLREF_HIRC                  ((uint32_t)0x00000000)
#define SYS_PLLREF_HOSC                  ((uint32_t)0x00000001)
#define SYS_PLLREF_SEL_POS               (0)
#define SYS_PLLREF_SEL_MASK              ((uint32_t)(3 << SYS_PLLREF_SEL_POS))
#define SYS_PLLREF_DIVM_POS              (8)
#define SYS_PLLREF_DIVM_MASK             ((uint32_t)(15 << SYS_PLLREF_DIVM_POS))

#define IS_PLLREF_DIVM_VALUE(VALUE)      ((2 <= (VALUE))&&((VALUE) <= 15))
/****************************CMUST register**********************************************/
#define SYS_CMUST_HIRC_STABLE            ((uint32_t)0x00000001)
#define SYS_CMUST_LIRC_STABLE            ((uint32_t)0x00000002)
#define SYS_CMUST_HOSC_STABLE            ((uint32_t)0x00000004)
#define SYS_CMUST_LOSC_STABLE            ((uint32_t)0x00000008)
#define SYS_CMUST_PLL_STABLE             ((uint32_t)0x00000010)
#define SYS_CMUST_PLLKDP_STABLE          ((uint32_t)0x00000020)
#define SYS_CMUST_PLLI2S_STABLE          ((uint32_t)0x00000040)
#define SYS_CMUST_PLLSAI_STABLE          ((uint32_t)0x00000080)
#define SYS_CMUST_PLLs_ANY_STABLE        ((uint32_t)0x000000F0)

//#define IS_SOURCE_CLOCK(SRCCLOCK)        ((((SRCCLOCK) & 0xFFFFFF00) == 0x00) && ((SRCCLOCK) != 0x00))
/*****************************HIRCCR register*******************************************/
#define SYS_HIRCCR_EN_POS                (0)
#define SYS_HIRCCR_HIRC_MASK             ((uint32_t)(1 << SYS_HIRCCR_EN_POS))
//#define SYS_HIRCCR_HIRC_EN               ((uint32_t)0x00000001)
/*****************************LIRCCR register*******************************************/
#define SYS_LIRCCR_EN_POS                (0)
#define SYS_LIRCCR_LIRC_MASK             ((uint32_t)(1 << SYS_LIRCCR_EN_POS))
//#define SYS_LIRCCR_LIRC_EN               ((uint32_t)0x00000001)
#define SYS_LIRCCR_TMC_POS               (4)
#define SYS_LIRCCR_TMC_MASK              ((uint32_t)(7 << SYS_LIRCCR_TMC_POS))
#define SYS_LIRCCR_TMR_POS               (8)
#define SYS_LIRCCR_TMR_MASK              ((uint32_t)(0x3f << SYS_LIRCCR_TMR_POS))

#define IS_LIRCCR_TMC_VALUE(VALUE)       ((VALUE) <= 63)
#define IS_LIRCCR_TMR_VALUE(VALUE)       ((VALUE) <= 7)
/*****************************HOSCCR register*******************************************/
#define SYS_HOSCCR_EN_POS                (0)
#define SYS_HOSCCR_HOSC_MASK             ((uint32_t)(1 << SYS_HOSCCR_EN_POS))
//#define SYS_HOSCCR_HOSC_EN               ((uint32_t)0x00000001)
#define SYS_HOSCCR_CS_POS                (4)
#define SYS_HOSCCR_CS_MASK               ((uint32_t)(3 << SYS_HOSCCR_CS_POS))
#define SYS_HOSCCR_CAPSEL_POS            (8)
#define SYS_HOSCCR_CAPSEL_MASK           ((uint32_t)(15 << SYS_HOSCCR_CAPSEL_POS))

#define IS_HOSCCR_CS_VALUE(VALUE)        ((VALUE) <= 3)
#define IS_HOSCCR_CAPSEL_VALUE(VALUE)    ((VALUE) <= 15)
/*****************************LOSCCR register*******************************************/
#define SYS_LOSCCR_EN_POS                (0)
#define SYS_LOSCCR_LOSC_MASK             ((uint32_t)(1 << SYS_LOSCCR_EN_POS))
//#define SYS_LOSCCR_LOSC_EN               ((uint32_t)0x00000001)
#define SYS_LOSCCR_ISEL_POS              (4)
#define SYS_LOSCCR_ISEL_MASK             ((uint32_t)(7 << SYS_LOSCCR_ISEL_POS))
#define SYS_LOSCCR_CAPSEL_POS            (8)
#define SYS_LOSCCR_CAPSEL_MASK           ((uint32_t)(15 << SYS_LOSCCR_CAPSEL_POS))

#define IS_LOSCCR_ISEL_VALUE(VALUE)      ((VALUE) <= 7)
#define IS_LOSCCR_CAPSEL_VALUE(VALUE)    ((VALUE) <= 15)
/*****************************PLL[0] register*******************************************/
#define SYS_PLL_EN_POS                   (0)
#define SYS_PLL_EN_MASK                  ((uint32_t)(1 << SYS_PLL_EN_POS))
//#define SYS_PLL_EN                         ((uint32_t)0x00000001)
#define SYS_PLL_CPC_POS                  (4)
#define SYS_PLL_CPC_MASK                 ((uint32_t)(3 << SYS_PLL_CPC_POS))
#define SYS_PLL_VCO_POS                  (8)
#define SYS_PLL_VCO_MASK                 ((uint32_t)(3 << SYS_PLL_VCO_POS))

#define IS_PLL_VCO_VALUE(VALUE)          ((VALUE) <= 3)
#define IS_PLL_CPC_VALUE(VALUE)          ((VALUE) <= 3)
/*****************************PLL[1] register*******************************************/
#define SYS_PLL_DIVP_POS                 (0)
#define SYS_PLL_DIVP_MASK                ((uint32_t)(255 << SYS_PLL_DIVP_POS))
#define SYS_PLL_DIVQ_POS                 (8)
#define SYS_PLL_DIVQ_MASK                ((uint32_t)(255 << SYS_PLL_DIVQ_POS))
#define SYS_PLL_DIVR_POS                 (16)
#define SYS_PLL_DIVR_MASK                ((uint32_t)(255 << SYS_PLL_DIVR_POS))
#define SYS_PLL_DIVN_POS                 (24)
#define SYS_PLL_DIVN_MASK                ((uint32_t)((uint32_t)0xff << SYS_PLL_DIVN_POS))

#define IS_PLL_DIVP_VALUE(VALUE)         (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLL_DIVQ_VALUE(VALUE)         (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLL_DIVR_VALUE(VALUE)         (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLL_DIVN_VALUE(VALUE)         (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
/*****************************PLLKDP[0] register***************************************/
#define SYS_PLLKDP_EN_POS                (0)
#define SYS_PLLKDP_EN_MASK               ((uint32_t)(1 << SYS_PLLKDP_EN_POS))
//#define SYS_PLLKDP_EN                    ((uint32_t)0x00000001)
#define SYS_PLLKDP_CPC_POS               (4)
#define SYS_PLLKDP_CPC_MASK              ((uint32_t)(3 << SYS_PLLKDP_CPC_POS))
#define SYS_PLLKDP_VCO_POS               (8)
#define SYS_PLLKDP_VCO_MASK              ((uint32_t)(3 << SYS_PLLKDP_VCO_POS))
#define SYS_PLLKDP_DIVN_POS              (16)
#define SYS_PLLKDP_DIVN_MASK             ((uint32_t)(511 << SYS_PLLKDP_DIVN_POS))

#define IS_PLLKDP_DIVN_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 511)) || (VALUE == 0))
#define IS_PLLKDP_VCO_VALUE(VALUE)       ((VALUE) <= 3)
#define IS_PLLKDP_CPC_VALUE(VALUE)       ((VALUE) <= 3)
/*****************************PLLKDP[1] register***************************************/
#define SYS_PLLKDP_DIVP_POS              (0)
#define SYS_PLLKDP_DIVP_MASK             ((uint32_t)(255 << SYS_PLLKDP_DIVP_POS))
#define SYS_PLLKDP_DIVQ_POS              (8)
#define SYS_PLLKDP_DIVQ_MASK             ((uint32_t)(255 << SYS_PLLKDP_DIVQ_POS))
#define SYS_PLLKDP_DIVR_POS              (16)
#define SYS_PLLKDP_DIVR_MASK             ((uint32_t)(255 << SYS_PLLKDP_DIVR_POS))

#define IS_PLLKDP_DIVP_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLLKDP_DIVQ_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLLKDP_DIVR_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
/*****************************PLLI2S[0] register***************************************/
#define SYS_PLLI2S_EN_POS                (0)
#define SYS_PLLI2S_EN_MASK               ((uint32_t)(1 << SYS_PLLI2S_EN_POS))
//#define SYS_PLLI2S_EN                    ((uint32_t)0x00000001)
#define SYS_PLLI2S_CPC_POS               (4)
#define SYS_PLLI2S_CPC_MASK              ((uint32_t)(3 << SYS_PLLI2S_CPC_POS))
#define SYS_PLLI2S_VCO_POS               (8)
#define SYS_PLLI2S_VCO_MASK              ((uint32_t)(3 << SYS_PLLI2S_VCO_POS))

#define IS_PLLI2S_CPC_VALUE(VALUE)       ((VALUE) <= 3)
#define IS_PLLI2S_VCO_VALUE(VALUE)       ((VALUE) <= 3)
/*****************************PLLKI2S[1] register*************************************/
#define SYS_PLLI2S_DIVP_POS              (0)
#define SYS_PLLI2S_DIVP_MASK             ((uint32_t)(255 << SYS_PLLI2S_DIVP_POS))
#define SYS_PLLI2S_DIVQ_POS              (8)
#define SYS_PLLI2S_DIVQ_MASK             ((uint32_t)(255 << SYS_PLLI2S_DIVQ_POS))
#define SYS_PLLI2S_DIVR_POS              (16)
#define SYS_PLLI2S_DIVR_MASK             ((uint32_t)(255 << SYS_PLLI2S_DIVR_POS))
#define SYS_PLLI2S_DIVN_POS              (24)
#define SYS_PLLI2S_DIVN_MASK             ((uint32_t)((uint32_t)255 << SYS_PLLI2S_DIVN_POS))

#define IS_PLLI2S_DIVP_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLLI2S_DIVQ_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLLI2S_DIVR_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLLI2S_DIVN_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
/*****************************PLLSAI[0] register*************************************/
#define SYS_PLLSAI_EN_POS                (0)
#define SYS_PLLSAI_EN_MASK               ((uint32_t)(1 << SYS_PLLSAI_EN_POS))
//#define SYS_PLLSAI_EN                    ((uint32_t)0x00000001)
#define SYS_PLLSAI_CPC_POS               (4)
#define SYS_PLLSAI_CPC_MASK              ((uint32_t)(3 << SYS_PLLSAI_CPC_POS))
#define SYS_PLLSAI_VCO_POS               (8)
#define SYS_PLLSAI_VCO_MASK              ((uint32_t)(3 << SYS_PLLSAI_VCO_POS))

#define IS_PLLSAI_CPC_VALUE(VALUE)       ((VALUE) <= 3)
#define IS_PLLSAI_VCO_VALUE(VALUE)       ((VALUE) <= 3)
/*****************************PLLSAI[1] register*************************************/
#define SYS_PLLSAI_DIVP_POS              (0)
#define SYS_PLLSAI_DIVP_MASK             ((uint32_t)(255 << SYS_PLLSAI_DIVP_POS))
#define SYS_PLLSAI_DIVQ_POS              (8)
#define SYS_PLLSAI_DIVQ_MASK             ((uint32_t)(255 << SYS_PLLSAI_DIVQ_POS))
#define SYS_PLLSAI_DIVR_POS              (16)
#define SYS_PLLSAI_DIVR_MASK             ((uint32_t)(255 << SYS_PLLSAI_DIVR_POS))
#define SYS_PLLSAI_DIVN_POS              (24)
#define SYS_PLLSAI_DIVN_MASK             ((uint32_t)((uint32_t)255 << SYS_PLLSAI_DIVN_POS))

#define IS_PLLSAI_DIVP_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLLSAI_DIVQ_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLLSAI_DIVR_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
#define IS_PLLSAI_DIVN_VALUE(VALUE)      (((2 <= (VALUE)) && ((VALUE) <= 255)) || (VALUE == 0))
/*****************************HOSCM[0] register*************************************/
#define SYS_HOSCM_HOSC_EN                ((uint32_t)0x00000001)
#define SYS_HOSCM_LOSS_PEND              ((uint32_t)0x00000002)
#define SYS_HOSCM_HOSC_PRD_POS           (16)
#define SYS_HOSCM_HOSC_PRD_MASK          ((uint32_t)(0xffff << SYS_HOSCM_HOSC_PRD_POS))
/*****************************HOSCM[1] register*************************************/
#define SYS_HOSCM_PRD_LOW_POS            (0)
#define SYS_HOSCM_PRD_LOW_MASK           ((uint32_t)(0xffff << SYS_HOSCM_PRD_LOW_POS))
#define SYS_HOSCM_PRD_HIGH_POS           (16)
#define SYS_HOSCM_PRD_HIGH_MASK          ((uint32_t)(0xffff << SYS_HOSCM_PRD_HIGH_POS))
/*****************************SYSCLK register***************************************/
#define SYS_SYSCLK_FCLK_SEL_POS          (0)
#define SYS_SYSCLK_FCLK_SEL_MASK         ((uint32_t)(7 << SYS_SYSCLK_FCLK_SEL_POS))
#define SYS_SYSCLK_FCLK_DIV_POS          (4)
#define SYS_SYSCLK_FCLK_DIV_MASK         ((uint32_t)(0x1ff << SYS_SYSCLK_FCLK_DIV_POS))
#define SYS_SYSCLK_FCLK_SHD_POS          (16)
#define SYS_SYSCLK_FCLK_SHD_MASK         ((uint32_t)(7 << SYS_SYSCLK_FCLK_SHD_POS))
#define SYS_SYSCLK_FCLK_SWT_ERR          ((uint32_t)0x00100000)
/*****************************CLKSRC[0] register************************************/
#define SYS_CLKSRC0_WWDT_DIV_POS         (4)
#define SYS_CLKSRC0_WWDT_DIV_MASK        ((uint32_t)(0x1FF << SYS_CLKSRC0_WWDT_DIV_POS))
#define SYS_CLKSRC0_SDRAM_DIV_POS        (20)
#define SYS_CLKSRC0_SDRAM_DIV_MASK       ((uint32_t)(7 << SYS_CLKSRC0_SDRAM_DIV_POS))
/*****************************CLKSRC[1] register************************************/
#define SYS_CLKSRC1_RTC_SEL_POS          (0)
#define SYS_CLKSRC1_RTC_SEL_MASK         ((uint32_t)(3 << SYS_CLKSRC1_RTC_SEL_POS))
#define SYS_CLKSRC1_BKD_SEL_POS          (2)
#define SYS_CLKSRC1_BKD_SEL_MASK         ((uint32_t)(1 << SYS_CLKSRC1_BKD_SEL_POS))
#define SYS_CLKSRC1_MCO1_SEL_POS         (4)
#define SYS_CLKSRC1_MCO1_SEL_MASK        ((uint32_t)(7 << SYS_CLKSRC1_MCO1_SEL_POS))
#define SYS_CLKSRC1_MCO1_DIV_POS         (8)
#define SYS_CLKSRC1_MCO1_DIV_MASK        ((uint32_t)(7 << SYS_CLKSRC1_MCO1_DIV_POS))
#define SYS_CLKSRC1_MCO2_SEL_POS         (12)
#define SYS_CLKSRC1_MCO2_SEL_MASK        ((uint32_t)(7 << SYS_CLKSRC1_MCO2_SEL_POS))
#define SYS_CLKSRC1_MCO2_DIV_POS         (16)
#define SYS_CLKSRC1_MCO2_DIV_MASK        ((uint32_t)(7 << SYS_CLKSRC1_MCO2_DIV_POS))
#define SYS_CLKSRC1_LPSTM_SEL_POS        (20)
#define SYS_CLKSRC1_LPSTM_SEL_MASK       ((uint32_t)(3 << SYS_CLKSRC1_LPSTM_SEL_POS))
#define SYS_CLKSRC1_RTC_DIV_POS          (24)
#define SYS_CLKSRC1_RTC_DIV_MASK         ((uint32_t)(31 << SYS_CLKSRC1_RTC_DIV_POS))
/*****************************CLKSRC[2] register************************************/
#define SYS_CLKSRC2_ADC_SEL_POS          (0)
#define SYS_CLKSRC2_ADC_SEL_MASK         ((uint32_t)(3 << SYS_CLKSRC2_ADC_SEL_POS))
#define SYS_CLKSRC2_ADC2_SEL_POS         (2)
#define SYS_CLKSRC2_ADC2_SEL_MASK        ((uint32_t)(3 << SYS_CLKSRC2_ADC2_SEL_POS))
#define SYS_CLKSRC2_ADC3_SEL_POS         (4)
#define SYS_CLKSRC2_ADC3_SEL_MASK        ((uint32_t)(3 << SYS_CLKSRC2_ADC3_SEL_POS))
#define SYS_CLKSRC2_ADC_DIV_POS          (8)
#define SYS_CLKSRC2_ADC_DIV_MASK         ((uint32_t)(15 << SYS_CLKSRC2_ADC_DIV_POS))
#define SYS_CLKSRC2_KDP_SEL_POS          (16)
#define SYS_CLKSRC2_KDP_SEL_MASK         ((uint32_t)(1 << SYS_CLKSRC2_KDP_SEL_POS))
#define SYS_CLKSRC2_ADC2_DIV_POS         (24)
#define SYS_CLKSRC2_ADC2_DIV_MASK        ((uint32_t)(15 << SYS_CLKSRC2_ADC2_DIV_POS))
#define SYS_CLKSRC2_ADC3_DIV_POS         (28)
#define SYS_CLKSRC2_ADC3_DIV_MASK        ((uint32_t)((uint32_t)15 << SYS_CLKSRC2_ADC3_DIV_POS))
/*****************************CLKSRC[3] register************************************/
#define SYS_CLKSRC3_VOUT_DIV_POS         (8)
#define SYS_CLKSRC3_VOUT_DIV_MASK        ((uint32_t)(0x1F << SYS_CLKSRC3_VOUT_DIV_POS))
#define SYS_CLKSRC3_SDMMC_SEL_POS        (16)
#define SYS_CLKSRC3_SDMMC_SEL_MASK       ((uint32_t)(1 << SYS_CLKSRC3_SDMMC_SEL_POS))
#define SYS_CLKSRC3_DAC_SEL_POS          (24)
#define SYS_CLKSRC3_DAC_SEL_MASK         ((uint32_t)(1 << SYS_CLKSRC3_DAC_SEL_POS))
/*****************************CLKSRC[4] register************************************/
#define SYS_CLKSRC4_I2C1_DIV_POS         (8)
#define SYS_CLKSRC4_I2C1_DIV_MASK        ((uint32_t)(0xFF << SYS_CLKSRC4_I2C1_DIV_POS))
#define SYS_CLKSRC4_I2C2_DIV_POS         (24)
#define SYS_CLKSRC4_I2C2_DIV_MASK        ((uint32_t)((uint32_t)0xFF << SYS_CLKSRC4_I2C2_DIV_POS))
/*****************************CLKSRC[5] register************************************/
#define SYS_CLKSRC5_I2C3_DIV_POS         (8)
#define SYS_CLKSRC5_I2C3_DIV_MASK        ((uint32_t)(0xFF << SYS_CLKSRC5_I2C3_DIV_POS))
#define SYS_CLKSRC5_BTM1_DIV_POS         (20)
#define SYS_CLKSRC5_BTM1_DIV_MASK        ((uint32_t)(0x1FF << SYS_CLKSRC5_BTM1_DIV_POS))
/*****************************CLKSRC[6] register************************************/
#define SYS_CLKSRC6_SAIA_SEL_POS         (0)
#define SYS_CLKSRC6_SAIA_SEL_MASK        ((uint32_t)(3 << SYS_CLKSRC6_SAIA_SEL_POS))
#define SYS_CLKSRC6_SAIB_SEL_POS         (8)
#define SYS_CLKSRC6_SAIB_SEL_MASK        ((uint32_t)(3 << SYS_CLKSRC6_SAIB_SEL_POS))
#define SYS_CLKSRC6_BTM2_DIV_POS         (20)
#define SYS_CLKSRC6_BTM2_DIV_MASK        ((uint32_t)(0x1FF << SYS_CLKSRC6_BTM2_DIV_POS))
/*****************************CLKSRC[7] register************************************/
#define SYS_CLKSRC7_UART1_SEL_POS        (0)
#define SYS_CLKSRC7_UART1_SEL_MASK       ((uint32_t)(1 << SYS_CLKSRC7_UART1_SEL_POS))
#define SYS_CLKSRC7_UART1_DIV_POS        (3)
#define SYS_CLKSRC7_UART1_DIV_MASK       ((uint32_t)(0x1F << SYS_CLKSRC7_UART1_DIV_POS))
#define SYS_CLKSRC7_UART2_SEL_POS        (8)
#define SYS_CLKSRC7_UART2_SEL_MASK       ((uint32_t)(1 << SYS_CLKSRC7_UART2_SEL_POS))
#define SYS_CLKSRC7_UART2_DIV_POS        (11)
#define SYS_CLKSRC7_UART2_DIV_MASK       ((uint32_t)(0x1F << SYS_CLKSRC7_UART2_DIV_POS))
#define SYS_CLKSRC7_UART3_SEL_POS        (16)
#define SYS_CLKSRC7_UART3_SEL_MASK       ((uint32_t)(1 << SYS_CLKSRC7_UART3_SEL_POS))
#define SYS_CLKSRC7_UART3_DIV_POS        (19)
#define SYS_CLKSRC7_UART3_DIV_MASK       ((uint32_t)(0x1F << SYS_CLKSRC7_UART3_DIV_POS))
#define SYS_CLKSRC7_UART4_SEL_POS        (24)
#define SYS_CLKSRC7_UART4_SEL_MASK       ((uint32_t)(1 << SYS_CLKSRC7_UART4_SEL_POS))
#define SYS_CLKSRC7_UART4_DIV_POS        (27)
#define SYS_CLKSRC7_UART4_DIV_MASK       ((uint32_t)((uint32_t)15 << SYS_CLKSRC7_UART4_DIV_POS))
/*****************************CLKSRC[8] register************************************/
#define SYS_CLKSRC8_UART5_SEL_POS        (0)
#define SYS_CLKSRC8_UART5_SEL_MASK       ((uint32_t)(1 << SYS_CLKSRC8_UART5_SEL_POS))
#define SYS_CLKSRC8_UART5_DIV_POS        (3)
#define SYS_CLKSRC8_UART5_DIV_MASK       ((uint32_t)(0x1F << SYS_CLKSRC8_UART5_DIV_POS))
#define SYS_CLKSRC8_UART6_SEL_POS        (8)
#define SYS_CLKSRC8_UART6_SEL_MASK       ((uint32_t)(1 << SYS_CLKSRC8_UART6_SEL_POS))
#define SYS_CLKSRC8_UART6_DIV_POS        (11)
#define SYS_CLKSRC8_UART6_DIV_MASK       ((uint32_t)(0x1F << SYS_CLKSRC8_UART6_DIV_POS))
#define SYS_CLKSRC8_UART7_SEL_POS        (16)
#define SYS_CLKSRC8_UART7_SEL_MASK       ((uint32_t)(1 << SYS_CLKSRC8_UART7_SEL_POS))
#define SYS_CLKSRC8_UART7_DIV_POS        (19)
#define SYS_CLKSRC8_UART7_DIV_MASK       ((uint32_t)(0x1F << SYS_CLKSRC8_UART7_DIV_POS))
#define SYS_CLKSRC8_UART8_SEL_POS        (24)
#define SYS_CLKSRC8_UART8_SEL_MASK       ((uint32_t)(1 << SYS_CLKSRC8_UART8_SEL_POS))
#define SYS_CLKSRC8_UART8_DIV_POS        (27)
#define SYS_CLKSRC8_UART8_DIV_MASK       ((uint32_t)((uint32_t)15 << SYS_CLKSRC8_UART8_DIV_POS))
/*****************************CLKSRC[9] register************************************/
#define SYS_CLKSRC9_TIM1_DIV_POS         (0)
#define SYS_CLKSRC9_TIM1_DIV_MASK        ((uint32_t)(3 << SYS_CLKSRC9_TIM1_DIV_POS))
#define SYS_CLKSRC9_TIM2_DIV_POS         (2)
#define SYS_CLKSRC9_TIM2_DIV_MASK        ((uint32_t)(3 << SYS_CLKSRC9_TIM2_DIV_POS))
#define SYS_CLKSRC9_TIM3_DIV_POS         (4)
#define SYS_CLKSRC9_TIM3_DIV_MASK        ((uint32_t)(3 << SYS_CLKSRC9_TIM3_DIV_POS))
#define SYS_CLKSRC9_TIM4_DIV_POS         (6)
#define SYS_CLKSRC9_TIM4_DIV_MASK        ((uint32_t)(3 << SYS_CLKSRC9_TIM4_DIV_POS))
#define SYS_CLKSRC9_TIM5_DIV_POS         (8)
#define SYS_CLKSRC9_TIM5_DIV_MASK        ((uint32_t)(3 << SYS_CLKSRC9_TIM5_DIV_POS))
#define SYS_CLKSRC9_TIM8_DIV_POS         (10)
#define SYS_CLKSRC9_TIM8_DIV_MASK        ((uint32_t)(3 << SYS_CLKSRC9_TIM8_DIV_POS))
#define SYS_CLKSRC9_TIM9_DIV_POS         (12)
#define SYS_CLKSRC9_TIM9_DIV_MASK        ((uint32_t)(3 << SYS_CLKSRC9_TIM9_DIV_POS))
#define SYS_CLKSRC9_TIM10_DIV_POS        (14)
#define SYS_CLKSRC9_TIM10_DIV_MASK       ((uint32_t)(3 << SYS_CLKSRC9_TIM10_DIV_POS))
#define SYS_CLKSRC9_TIM11_DIV_POS        (16)
#define SYS_CLKSRC9_TIM11_DIV_MASK       ((uint32_t)(3 << SYS_CLKSRC9_TIM11_DIV_POS))
#define SYS_CLKSRC9_TIM12_DIV_POS        (18)
#define SYS_CLKSRC9_TIM12_DIV_MASK       ((uint32_t)(3 << SYS_CLKSRC9_TIM12_DIV_POS))
#define SYS_CLKSRC9_TIM13_DIV_POS        (20)
#define SYS_CLKSRC9_TIM13_DIV_MASK       ((uint32_t)(3 << SYS_CLKSRC9_TIM13_DIV_POS))
#define SYS_CLKSRC9_TIM14_DIV_POS        (22)
#define SYS_CLKSRC9_TIM14_DIV_MASK       ((uint32_t)(3 << SYS_CLKSRC9_TIM14_DIV_POS))



/* Cortex-M4 processor and core peripherals */
#include "core_cm4.h"
#ifdef RABBIT_NO_OS_SYSTEM
#else

/**
 *for specil user want to operate device reg direct
 *for example for fpga test.maybe user want to rw
 *reg directly for qucikly test.
 */
#if defined(USE_DEVICE_LL_DIRECT)
#include<driver/uart/gm_ll_uart.h>
#include<driver/mmc/host/gm_ll_mmc_host.h>
#include<driver/rtc/gm_ll_rtc.h>
#include<driver/power/gm_ll_power.h>
/*add other modulis gm_ll_xx.h*/
#endif

#endif

void SystemInit(void);

#ifdef __cplusplus
}
#endif
#endif /* #ifndef __SOC_GM6721_H__ */
