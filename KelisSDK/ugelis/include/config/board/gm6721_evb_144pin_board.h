/**************************************************
 * Generated include file for gree,gm6721evb-144pin
 *               DO NOT MODIFY
 */

#ifndef _DEVICE_TREE_BOARD_H
    #define _DEVICE_TREE_BOARD_H

    /* memory@20000000 */
    #define CONFIG_SRAM_BASE_ADDRESS    0x20000000
    #define CONFIG_SRAM_SIZE        128

    /* sdram@c0000000 */
    #define CONFIG_SDRAM_BASE_ADDRESS   0xc0000000
    #define CONFIG_SDRAM_SIZE       16384

    /* camera_ov5640@0 */
    #define GREE_CAMERA_OV5640_0_GPIO_PWDN_PIN  15
    #define GREE_CAMERA_OV5640_0_GPIO_PWDN_PORT "GPIOB"
    #define GREE_CAMERA_OV5640_0_GPIO_RST_PIN   1
    #define GREE_CAMERA_OV5640_0_GPIO_RST_PORT  "GPIOC"
    #define GREE_CAMERA_OV5640_0_HSYNC_POLARITY 0
    #define GREE_CAMERA_OV5640_0_LABEL      "CAMERA_OV5640_0"
    #define GREE_CAMERA_OV5640_0_PCLK_POLARITY  1
    #define GREE_CAMERA_OV5640_0_VSYNC_POLARITY 0
    #define CAMERA0_GPIO_PWDN_PIN           GREE_CAMERA_OV5640_0_GPIO_PWDN_PIN
    #define CAMERA0_GPIO_PWDN_PORT          GREE_CAMERA_OV5640_0_GPIO_PWDN_PORT
    #define CAMERA0_GPIO_RST_PIN            GREE_CAMERA_OV5640_0_GPIO_RST_PIN
    #define CAMERA0_GPIO_RST_PORT           GREE_CAMERA_OV5640_0_GPIO_RST_PORT
    #define CAMERA0_HSYNC_POLARITY          GREE_CAMERA_OV5640_0_HSYNC_POLARITY
    #define CAMERA0_LABEL               GREE_CAMERA_OV5640_0_LABEL
    #define CAMERA0_PCLK_POLARITY           GREE_CAMERA_OV5640_0_PCLK_POLARITY
    #define CAMERA0_VSYNC_POLARITY          GREE_CAMERA_OV5640_0_VSYNC_POLARITY

    /* camera_ov5640@1 */
    #define GREE_CAMERA_OV5640_1_GPIO_PWDN_PIN  7
    #define GREE_CAMERA_OV5640_1_GPIO_PWDN_PORT "GPIOG"
    #define GREE_CAMERA_OV5640_1_GPIO_RST_PIN   11
    #define GREE_CAMERA_OV5640_1_GPIO_RST_PORT  "GPIOG"
    #define GREE_CAMERA_OV5640_1_HSYNC_POLARITY 0
    #define GREE_CAMERA_OV5640_1_LABEL      "CAMERA_OV5640_1"
    #define GREE_CAMERA_OV5640_1_PCLK_POLARITY  1
    #define GREE_CAMERA_OV5640_1_VSYNC_POLARITY 0
    #define CAMERA1_GPIO_PWDN_PIN           GREE_CAMERA_OV5640_1_GPIO_PWDN_PIN
    #define CAMERA1_GPIO_PWDN_PORT          GREE_CAMERA_OV5640_1_GPIO_PWDN_PORT
    #define CAMERA1_GPIO_RST_PIN            GREE_CAMERA_OV5640_1_GPIO_RST_PIN
    #define CAMERA1_GPIO_RST_PORT           GREE_CAMERA_OV5640_1_GPIO_RST_PORT
    #define CAMERA1_HSYNC_POLARITY          GREE_CAMERA_OV5640_1_HSYNC_POLARITY
    #define CAMERA1_LABEL               GREE_CAMERA_OV5640_1_LABEL
    #define CAMERA1_PCLK_POLARITY           GREE_CAMERA_OV5640_1_PCLK_POLARITY
    #define CAMERA1_VSYNC_POLARITY          GREE_CAMERA_OV5640_1_VSYNC_POLARITY

    /* can@40030000 */
    #define GREE_GM_CAN_40030000_BASE_ADDRESS   0x40030000
    #define GREE_GM_CAN_40030000_BAUDRATE       20000
    #define GREE_GM_CAN_40030000_BS1        6
    #define GREE_GM_CAN_40030000_BS2        3
    #define GREE_GM_CAN_40030000_ERRWARNTHRESHOLD   120
    #define GREE_GM_CAN_40030000_IRQ_0      51
    #define GREE_GM_CAN_40030000_IRQ_0_PRIORITY 3
    #define GREE_GM_CAN_40030000_LABEL      "CAN_0"
    #define GREE_GM_CAN_40030000_SAMPLE     1
    #define GREE_GM_CAN_40030000_SIZE       4096
    #define GREE_GM_CAN_40030000_SJW        0
    #define GREE_GM_CAN_40030000_WORKMODE       1
    #define CAN0_BASE_ADDRESS           GREE_GM_CAN_40030000_BASE_ADDRESS
    #define CAN0_BAUDRATE               GREE_GM_CAN_40030000_BAUDRATE
    #define CAN0_BS1                GREE_GM_CAN_40030000_BS1
    #define CAN0_BS2                GREE_GM_CAN_40030000_BS2
    #define CAN0_ERRWARNTHRESHOLD           GREE_GM_CAN_40030000_ERRWARNTHRESHOLD
    #define CAN0_IRQ                GREE_GM_CAN_40030000_IRQ_0
    #define CAN0_IRQ_PRIORITY           GREE_GM_CAN_40030000_IRQ_0_PRIORITY
    #define CAN0_LABEL              GREE_GM_CAN_40030000_LABEL
    #define CAN0_SAMPLE             GREE_GM_CAN_40030000_SAMPLE
    #define CAN0_SIZE               GREE_GM_CAN_40030000_SIZE
    #define CAN0_SJW                GREE_GM_CAN_40030000_SJW
    #define CAN0_WORKMODE               GREE_GM_CAN_40030000_WORKMODE

    /* flash@10000000 */
    #define CONFIG_FLASH_BASE_ADDRESS       0x10000000
    #define CONFIG_FLASH_LOAD_OFFSET        0
    #define CONFIG_FLASH_LOAD_SIZE          0
    #define CONFIG_FLASH_SIZE           1024
    #define FLASH_FLASH_GM_LABEL            "FLASH_GM"
    #define FLASH_FLASH_GM_WRITE_BLOCK_SIZE     1
    #define FLASH_LABEL             "FLASH_GM"
    #define FLASH_WRITE_BLOCK_SIZE          1

    /* imp@40024000 */
    #define GREE_GM_IMP_40024000_BASE_ADDRESS   0x40024000
    #define GREE_GM_IMP_40024000_IRQ_0      46
    #define GREE_GM_IMP_40024000_IRQ_0_PRIORITY 3
    #define GREE_GM_IMP_40024000_SIZE       3072

    /* interrupt-controller@e000e100 */
    #define ARM_V7M_NVIC_E000E100_ARM_NUM_IRQ_PRIORITY_BITS     3
    #define ARM_V7M_NVIC_E000E100_BASE_ADDRESS          0xe000e100
    #define ARM_V7M_NVIC_E000E100_SIZE              3072

    /* iwdg@40008000 */
    #define GREE_GM_IWDG_40008000_BASE_ADDRESS  0x40008000
    #define GREE_GM_IWDG_40008000_LABEL     "IWDG"
    #define GREE_GM_IWDG_40008000_SIZE      4096
    #define IWDG_BASE_ADDRESS           GREE_GM_IWDG_40008000_BASE_ADDRESS
    #define IWDG_LABEL              GREE_GM_IWDG_40008000_LABEL
    #define IWDG_SIZE               GREE_GM_IWDG_40008000_SIZE

    /* lvd@40000138 */
    #define GREE_GM_LVD_40000138_BASE_ADDRESS   0x40000138
    #define GREE_GM_LVD_40000138_IRQ_0      1
    #define GREE_GM_LVD_40000138_IRQ_0_PRIORITY 3
    #define GREE_GM_LVD_40000138_LABEL      "GMLVD"
    #define GREE_GM_LVD_40000138_SIZE       256
    #define LVD_BASE_ADDRESS            GREE_GM_LVD_40000138_BASE_ADDRESS
    #define LVD_IRQ                 GREE_GM_LVD_40000138_IRQ_0
    #define LVD_IRQ_PRIORITY            GREE_GM_LVD_40000138_IRQ_0_PRIORITY
    #define LVD_LABEL               GREE_GM_LVD_40000138_LABEL
    #define LVD_SIZE                GREE_GM_LVD_40000138_SIZE

    /* mmc@4003b000 */
    #define GREE_GM_MMC_4003B000_BASE_ADDRESS       0x4003b000
    #define GREE_GM_MMC_4003B000_BUSWITCH           1
    #define GREE_GM_MMC_4003B000_IRQ_0          79
    #define GREE_GM_MMC_4003B000_IRQ_0_PRIORITY     3
    #define GREE_GM_MMC_4003B000_LABEL          "MMC_0"
    #define GREE_GM_MMC_4003B000_POLL_DETECT_CARD_HOTPLUG   True
    #define GREE_GM_MMC_4003B000_REMOVABLE          1
    #define GREE_GM_MMC_4003B000_SIZE           4096
    #define GREE_GM_MMC_4003B000_TYPE           0
    #define MMC0_BASE_ADDRESS               GREE_GM_MMC_4003B000_BASE_ADDRESS
    #define MMC0_BUSWITCH                   GREE_GM_MMC_4003B000_BUSWITCH
    #define MMC0_IRQ                    GREE_GM_MMC_4003B000_IRQ_0
    #define MMC0_IRQ_PRIORITY               GREE_GM_MMC_4003B000_IRQ_0_PRIORITY
    #define MMC0_LABEL                  GREE_GM_MMC_4003B000_LABEL
    #define MMC0_POLL_DETECT_CARD_HOTPLUG           GREE_GM_MMC_4003B000_POLL_DETECT_CARD_HOTPLUG
    #define MMC0_REMOVABLE                  GREE_GM_MMC_4003B000_REMOVABLE
    #define MMC0_SIZE                   GREE_GM_MMC_4003B000_SIZE
    #define MMC0_TYPE                   GREE_GM_MMC_4003B000_TYPE

    /* tim@40007000 */
    #define GREE_GM_TIM_40007000_BASE_ADDRESS       0x40007000
    #define GREE_GM_TIM_40007000_IRQ_0              11
    #define GREE_GM_TIM_40007000_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007000_LABEL              "TIM_1"
    #define GREE_GM_TIM_40007000_SIZE               4096
    #define TIM1_BASE_ADDRESS                       GREE_GM_TIM_40007000_BASE_ADDRESS
    #define TIM1_IRQ                                GREE_GM_TIM_40007000_IRQ_0
    #define TIM1_IRQ_PRIORITY                       GREE_GM_TIM_40007000_IRQ_0_PRIORITY
    #define TIM1_LABEL                              GREE_GM_TIM_40007000_LABEL
    #define TIM1_SIZE                               GREE_GM_TIM_40007000_SIZE

    /* tim@40007100 */
    #define GREE_GM_TIM_40007100_BASE_ADDRESS       0x40007100
    #define GREE_GM_TIM_40007100_IRQ_0              12
    #define GREE_GM_TIM_40007100_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007100_LABEL              "TIM_2"
    #define GREE_GM_TIM_40007100_SIZE               4096
    #define TIM2_BASE_ADDRESS                       GREE_GM_TIM_40007100_BASE_ADDRESS
    #define TIM2_IRQ                                GREE_GM_TIM_40007100_IRQ_0
    #define TIM2_IRQ_PRIORITY                       GREE_GM_TIM_40007100_IRQ_0_PRIORITY
    #define TIM2_LABEL                              GREE_GM_TIM_40007100_LABEL
    #define TIM2_SIZE                               GREE_GM_TIM_40007100_SIZE

    /* tim@40007200 */
    #define GREE_GM_TIM_40007200_BASE_ADDRESS       0x40007200
    #define GREE_GM_TIM_40007200_IRQ_0              13
    #define GREE_GM_TIM_40007200_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007200_LABEL              "TIM_3"
    #define GREE_GM_TIM_40007200_SIZE               4096
    #define TIM3_BASE_ADDRESS                       GREE_GM_TIM_40007200_BASE_ADDRESS
    #define TIM3_IRQ                                GREE_GM_TIM_40007200_IRQ_0
    #define TIM3_IRQ_PRIORITY                       GREE_GM_TIM_40007200_IRQ_0_PRIORITY
    #define TIM3_LABEL                              GREE_GM_TIM_40007200_LABEL
    #define TIM3_SIZE                               GREE_GM_TIM_40007200_SIZE

    /* tim@40007300 */
    #define GREE_GM_TIM_40007300_BASE_ADDRESS       0x40007300
    #define GREE_GM_TIM_40007300_IRQ_0              54
    #define GREE_GM_TIM_40007300_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007300_LABEL              "TIM_4"
    #define GREE_GM_TIM_40007300_SIZE               4096
    #define TIM4_BASE_ADDRESS                       GREE_GM_TIM_40007300_BASE_ADDRESS
    #define TIM4_IRQ                                GREE_GM_TIM_40007300_IRQ_0
    #define TIM4_IRQ_PRIORITY                       GREE_GM_TIM_40007300_IRQ_0_PRIORITY
    #define TIM4_LABEL                              GREE_GM_TIM_40007300_LABEL
    #define TIM4_SIZE                               GREE_GM_TIM_40007300_SIZE

    /* tim@40007400 */
    #define GREE_GM_TIM_40007400_BASE_ADDRESS       0x40007400
    #define GREE_GM_TIM_40007400_IRQ_0              66
    #define GREE_GM_TIM_40007400_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007400_LABEL              "TIM_5"
    #define GREE_GM_TIM_40007400_SIZE               4096
    #define TIM5_BASE_ADDRESS                       GREE_GM_TIM_40007400_BASE_ADDRESS
    #define TIM5_IRQ                                GREE_GM_TIM_40007400_IRQ_0
    #define TIM5_IRQ_PRIORITY                       GREE_GM_TIM_40007400_IRQ_0_PRIORITY
    #define TIM5_LABEL                              GREE_GM_TIM_40007400_LABEL
    #define TIM5_SIZE                               GREE_GM_TIM_40007400_SIZE

    /* tim@40007500 */
    #define GREE_GM_TIM_40007500_BASE_ADDRESS       0x40007500
    #define GREE_GM_TIM_40007500_IRQ_0              67
    #define GREE_GM_TIM_40007500_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007500_LABEL              "TIM_8"
    #define GREE_GM_TIM_40007500_SIZE               4096
    #define TIM8_BASE_ADDRESS                       GREE_GM_TIM_40007500_BASE_ADDRESS
    #define TIM8_IRQ                                GREE_GM_TIM_40007500_IRQ_0
    #define TIM8_IRQ_PRIORITY                       GREE_GM_TIM_40007500_IRQ_0_PRIORITY
    #define TIM8_LABEL                              GREE_GM_TIM_40007500_LABEL
    #define TIM8_SIZE                               GREE_GM_TIM_40007500_SIZE

    /* tim@40007600 */
    #define GREE_GM_TIM_40007600_BASE_ADDRESS       0x40007600
    #define GREE_GM_TIM_40007600_IRQ_0              72
    #define GREE_GM_TIM_40007600_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007600_LABEL              "TIM_9"
    #define GREE_GM_TIM_40007600_SIZE               4096
    #define TIM9_BASE_ADDRESS                       GREE_GM_TIM_40007600_BASE_ADDRESS
    #define TIM9_IRQ                                GREE_GM_TIM_40007600_IRQ_0
    #define TIM9_IRQ_PRIORITY                       GREE_GM_TIM_40007600_IRQ_0_PRIORITY
    #define TIM9_LABEL                              GREE_GM_TIM_40007600_LABEL
    #define TIM9_SIZE                               GREE_GM_TIM_40007600_SIZE

    /* tim@40007700 */
    #define GREE_GM_TIM_40007700_BASE_ADDRESS       0x40007700
    #define GREE_GM_TIM_40007700_IRQ_0              73
    #define GREE_GM_TIM_40007700_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007700_LABEL              "TIM_10"
    #define GREE_GM_TIM_40007700_SIZE               4096
    #define TIM10_BASE_ADDRESS                      GREE_GM_TIM_40007700_BASE_ADDRESS
    #define TIM10_IRQ                               GREE_GM_TIM_40007700_IRQ_0
    #define TIM10_IRQ_PRIORITY                      GREE_GM_TIM_40007700_IRQ_0_PRIORITY
    #define TIM10_LABEL                             GREE_GM_TIM_40007700_LABEL
    #define TIM10_SIZE                              GREE_GM_TIM_40007700_SIZE

    /* tim@40007800 */
    #define GREE_GM_TIM_40007800_BASE_ADDRESS       0x40007800
    #define GREE_GM_TIM_40007800_IRQ_0              74
    #define GREE_GM_TIM_40007800_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007800_LABEL              "TIM_11"
    #define GREE_GM_TIM_40007800_SIZE               4096
    #define TIM11_BASE_ADDRESS                      GREE_GM_TIM_40007800_BASE_ADDRESS
    #define TIM11_IRQ                               GREE_GM_TIM_40007800_IRQ_0
    #define TIM11_IRQ_PRIORITY                      GREE_GM_TIM_40007800_IRQ_0_PRIORITY
    #define TIM11_LABEL                             GREE_GM_TIM_40007800_LABEL
    #define TIM11_SIZE                              GREE_GM_TIM_40007800_SIZE

    /* tim@40007900 */
    #define GREE_GM_TIM_40007900_BASE_ADDRESS       0x40007900
    #define GREE_GM_TIM_40007900_IRQ_0              75
    #define GREE_GM_TIM_40007900_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007900_LABEL              "TIM_12"
    #define GREE_GM_TIM_40007900_SIZE               4096
    #define TIM12_BASE_ADDRESS                      GREE_GM_TIM_40007900_BASE_ADDRESS
    #define TIM12_IRQ                               GREE_GM_TIM_40007900_IRQ_0
    #define TIM12_IRQ_PRIORITY                      GREE_GM_TIM_40007900_IRQ_0_PRIORITY
    #define TIM12_LABEL                             GREE_GM_TIM_40007900_LABEL
    #define TIM12_SIZE                              GREE_GM_TIM_40007900_SIZE

    /* tim@40007A00 */
    #define GREE_GM_TIM_40007A00_BASE_ADDRESS       0x40007A00
    #define GREE_GM_TIM_40007A00_IRQ_0              76
    #define GREE_GM_TIM_40007A00_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007A00_LABEL              "TIM_13"
    #define GREE_GM_TIM_40007A00_SIZE               4096
    #define TIM13_BASE_ADDRESS                      GREE_GM_TIM_40007A00_BASE_ADDRESS
    #define TIM13_IRQ                               GREE_GM_TIM_40007A00_IRQ_0
    #define TIM13_IRQ_PRIORITY                      GREE_GM_TIM_40007A00_IRQ_0_PRIORITY
    #define TIM13_LABEL                             GREE_GM_TIM_40007A00_LABEL
    #define TIM13_SIZE                              GREE_GM_TIM_40007A00_SIZE

    /* tim@40007B00 */
    #define GREE_GM_TIM_40007B00_BASE_ADDRESS       0x40007B00
    #define GREE_GM_TIM_40007B00_IRQ_0              77
    #define GREE_GM_TIM_40007B00_IRQ_0_PRIORITY     3
    #define GREE_GM_TIM_40007B00_LABEL              "TIM_14"
    #define GREE_GM_TIM_40007B00_SIZE               4096
    #define TIM14_BASE_ADDRESS                      GREE_GM_TIM_40007B00_BASE_ADDRESS
    #define TIM14_IRQ                               GREE_GM_TIM_40007B00_IRQ_0
    #define TIM14_IRQ_PRIORITY                      GREE_GM_TIM_40007B00_IRQ_0_PRIORITY
    #define TIM14_LABEL                             GREE_GM_TIM_40007B00_LABEL
    #define TIM14_SIZE                              GREE_GM_TIM_40007B00_SIZE

    /* power@40000004 */
    #define GREE_GM_POWER_40000004_BASE_ADDRESS     0x2625a04
    #define GREE_GM_POWER_40000004_IRQ_0            53
    #define GREE_GM_POWER_40000004_IRQ_0_PRIORITY       3
    #define GREE_GM_POWER_40000004_LABEL            "POWER"
    #define GREE_GM_POWER_40000004_SIZE         4
    #define GREE_GM_POWER_40000004_WKPIN_XPA0_VAILD_EDGE    0
    #define GREE_GM_POWER_40000004_WKPIN_XPA7_VAILD_EDGE    0
    #define GREE_GM_POWER_40000004_WKPIN_XPA8_VAILD_EDGE    0
    #define POWER_BASE_ADDRESS              GREE_GM_POWER_40000004_BASE_ADDRESS
    #define POWER_IRQ                   GREE_GM_POWER_40000004_IRQ_0
    #define POWER_IRQ_PRIORITY              GREE_GM_POWER_40000004_IRQ_0_PRIORITY
    #define POWER_LABEL                 GREE_GM_POWER_40000004_LABEL
    #define POWER_SIZE                  GREE_GM_POWER_40000004_SIZE
    #define POWER_WKPIN_XPA0_VAILD_EDGE         GREE_GM_POWER_40000004_WKPIN_XPA0_VAILD_EDGE
    #define POWER_WKPIN_XPA7_VAILD_EDGE         GREE_GM_POWER_40000004_WKPIN_XPA7_VAILD_EDGE
    #define POWER_WKPIN_XPA8_VAILD_EDGE         GREE_GM_POWER_40000004_WKPIN_XPA8_VAILD_EDGE

    /* rtc@40009000 */
    #define GREE_GM_RTC_40009000_BASE_ADDRESS   0x40009000
    #define GREE_GM_RTC_40009000_CLK_SRC        0
    #define GREE_GM_RTC_40009000_IRQ_0      2
    #define GREE_GM_RTC_40009000_IRQ_0_PRIORITY 3
    #define GREE_GM_RTC_40009000_LABEL      "GMRTC"
    #define GREE_GM_RTC_40009000_SIZE       256
    #define RTC_BASE_ADDRESS            GREE_GM_RTC_40009000_BASE_ADDRESS
    #define RTC_CLK_SRC             GREE_GM_RTC_40009000_CLK_SRC
    #define RTC_IRQ                 GREE_GM_RTC_40009000_IRQ_0
    #define RTC_IRQ_PRIORITY            GREE_GM_RTC_40009000_IRQ_0_PRIORITY
    #define RTC_LABEL               GREE_GM_RTC_40009000_LABEL
    #define RTC_SIZE                GREE_GM_RTC_40009000_SIZE

    /* uart@40004000 */
    #define GREE_GM_UART_40004000_BASE_ADDRESS  0x40004000
    #define GREE_GM_UART_40004000_CLOCKSRC      8000000
    #define GREE_GM_UART_40004000_CURRENT_SPEED 19200
    #define GREE_GM_UART_40004000_IRQ_0     18
    #define GREE_GM_UART_40004000_IRQ_0_PRIORITY    3
    #define GREE_GM_UART_40004000_LABEL     "UART_0"
    #define GREE_GM_UART_40004000_PARITY        1
    #define GREE_GM_UART_40004000_SIZE      4096
    #define GREE_GM_UART_40004000_STOPBITS      0
    #define GREE_GM_UART_40004000_WORDLENGTH    3
    #define UART0_BASE_ADDRESS          GREE_GM_UART_40004000_BASE_ADDRESS
    #define UART0_CLOCKSRC              GREE_GM_UART_40004000_CLOCKSRC
    #define UART0_CURRENT_SPEED         GREE_GM_UART_40004000_CURRENT_SPEED
    #define UART0_IRQ               GREE_GM_UART_40004000_IRQ_0
    #define UART0_IRQ_PRIORITY          GREE_GM_UART_40004000_IRQ_0_PRIORITY
    #define UART0_LABEL             GREE_GM_UART_40004000_LABEL
    #define UART0_PARITY                GREE_GM_UART_40004000_PARITY
    #define UART0_SIZE              GREE_GM_UART_40004000_SIZE
    #define UART0_STOPBITS              GREE_GM_UART_40004000_STOPBITS
    #define UART0_WORDLENGTH            GREE_GM_UART_40004000_WORDLENGTH

    /* i2c@4000e000 */
    #define GREE_GM_I2C_4000E000_BASE_ADDRESS   0x4000e000
    //#define GREE_GM_I2C_4000E000_CLOCKSRC     16000000
    #define GREE_GM_I2C_4000E000_CURRENT_SPEED  10
    #define GREE_GM_I2C_4000E000_IRQ_0          15
    #define GREE_GM_I2C_4000E000_IRQ_0_PRIORITY 2
    #define GREE_GM_I2C_4000E000_LABEL          "I2C_0"
    #define GREE_GM_I2C_4000E000_ADDRMODE       0
    #define GREE_GM_I2C_4000E000_MASTERTXMODE   2
    #define GREE_GM_I2C_4000E000_SPEEDMODE      0
    #define GREE_GM_I2C_4000E000_WORKMODE       1
    #define GREE_GM_I2C_4000E000_ENABLEDMA      0
    #define GREE_GM_I2C_4000E000_I2C0_LOCALADDR 0x56
    #define GREE_GM_I2C_4000E000_SIZE           4096
    #define I2C0_BASE_ADDRESS           GREE_GM_I2C_4000E000_BASE_ADDRESS
    #define I2C0_CLOCKSRC               GREE_GM_I2C_4000E000_CLOCKSRC
    #define I2C0_CURRENT_SPEED          GREE_GM_I2C_4000E000_CURRENT_SPEED
    #define I2C0_IRQ                    GREE_GM_I2C_4000E000_IRQ_0
    #define I2C0_IRQ_PRIORITY           GREE_GM_I2C_4000E000_IRQ_0_PRIORITY
    #define I2C0_LABEL                  GREE_GM_I2C_4000E000_LABEL
    #define I2C0_SIZE                   GREE_GM_I2C_4000E000_SIZE
    #define I2C0_ADDRMODE               GREE_GM_I2C_4000E000_ADDRMODE
    #define I2C0_MASTERTXMODE           GREE_GM_I2C_4000E000_MASTERTXMODE
    #define I2C0_SPEEDMODE              GREE_GM_I2C_4000E000_SPEEDMODE
    #define I2C0_WORKMODE               GREE_GM_I2C_4000E000_WORKMODE
    #define I2C0_ENABLEDMA              GREE_GM_I2C_4000E000_ENABLEDMA
    #define I2C0_LOCALADDR              GREE_GM_I2C_4000E000_I2C0_LOCALADDR

    /* i2c@4000f000 */
    #define GREE_GM_I2C_4000F000_BASE_ADDRESS   0x4000f000
    //#define GREE_GM_I2C_4000F000_CLOCKSRC     16000000
    #define GREE_GM_I2C_4000F000_CURRENT_SPEED  100
    #define GREE_GM_I2C_4000F000_IRQ_0          16
    #define GREE_GM_I2C_4000F000_IRQ_0_PRIORITY 3
    #define GREE_GM_I2C_4000F000_LABEL          "I2C_1"
    #define GREE_GM_I2C_4000F000_ADDRMODE       0
    #define GREE_GM_I2C_4000F000_MASTERTXMODE   2
    #define GREE_GM_I2C_4000F000_SPEEDMODE      0
    #define GREE_GM_I2C_4000F000_WORKMODE       1
    #define GREE_GM_I2C_4000F000_ENABLEDMA      0
    #define GREE_GM_I2C_4000F000_I2C1_LOCALADDR 0x56
    #define GREE_GM_I2C_4000F000_SIZE           4096
    #define I2C1_BASE_ADDRESS           GREE_GM_I2C_4000F000_BASE_ADDRESS
    #define I2C1_CLOCKSRC               GREE_GM_I2C_4000F000_CLOCKSRC
    #define I2C1_CURRENT_SPEED          GREE_GM_I2C_4000F000_CURRENT_SPEED
    #define I2C1_IRQ                    GREE_GM_I2C_4000F000_IRQ_0
    #define I2C1_IRQ_PRIORITY           GREE_GM_I2C_4000F000_IRQ_0_PRIORITY
    #define I2C1_LABEL                  GREE_GM_I2C_4000F000_LABEL
    #define I2C1_SIZE                   GREE_GM_I2C_4000F000_SIZE
    #define I2C1_ADDRMODE               GREE_GM_I2C_4000F000_ADDRMODE
    #define I2C1_MASTERTXMODE           GREE_GM_I2C_4000F000_MASTERTXMODE
    #define I2C1_SPEEDMODE              GREE_GM_I2C_4000F000_SPEEDMODE
    #define I2C1_WORKMODE               GREE_GM_I2C_4000F000_WORKMODE
    #define I2C1_ENABLEDMA              GREE_GM_I2C_4000F000_ENABLEDMA
    #define I2C1_LOCALADDR              GREE_GM_I2C_4000F000_I2C1_LOCALADDR

    #define GREE_GM_I2C_40002000_BASE_ADDRESS   0x40002000
    //#define GREE_GM_I2C_40002000_CLOCKSRC     16000000
    #define GREE_GM_I2C_40002000_CURRENT_SPEED  10
    #define GREE_GM_I2C_40002000_IRQ_0          17
    #define GREE_GM_I2C_40002000_IRQ_0_PRIORITY 3
    #define GREE_GM_I2C_40002000_LABEL          "I2C_2"
    #define GREE_GM_I2C_40002000_ADDRMODE       0
    #define GREE_GM_I2C_40002000_MASTERTXMODE   2
    #define GREE_GM_I2C_40002000_SPEEDMODE      0
    #define GREE_GM_I2C_40002000_WORKMODE       0
    #define GREE_GM_I2C_40002000_ENABLEDMA      0
    #define GREE_GM_I2C_40002000_I2C2_LOCALADDR 0x56
    #define GREE_GM_I2C_40002000_SIZE           4096
    #define GREE_GM_I2C_40002000_I2C2_s_dmabuffer   s_dmabuffer
    #define I2C2_BASE_ADDRESS           GREE_GM_I2C_40002000_BASE_ADDRESS
    #define I2C2_CLOCKSRC               GREE_GM_I2C_40002000_CLOCKSRC
    #define I2C2_CURRENT_SPEED          GREE_GM_I2C_40002000_CURRENT_SPEED
    #define I2C2_IRQ                    GREE_GM_I2C_40002000_IRQ_0
    #define I2C2_IRQ_PRIORITY           GREE_GM_I2C_40002000_IRQ_0_PRIORITY
    #define I2C2_LABEL                  GREE_GM_I2C_40002000_LABEL
    #define I2C2_SIZE                   GREE_GM_I2C_40002000_SIZE
    #define I2C2_ADDRMODE               GREE_GM_I2C_40002000_ADDRMODE
    #define I2C2_MASTERTXMODE           GREE_GM_I2C_40002000_MASTERTXMODE
    #define I2C2_SPEEDMODE              GREE_GM_I2C_40002000_SPEEDMODE
    #define I2C2_WORKMODE               GREE_GM_I2C_40002000_WORKMODE
    #define I2C2_ENABLEDMA              GREE_GM_I2C_40002000_ENABLEDMA
    #define I2C2_LOCALADDR              GREE_GM_I2C_40002000_I2C2_LOCALADDR
    #define I2C2_s_dmabuffer            GREE_GM_I2C_40002000_I2C2_s_dmabuffer
    /* uart@40005000 */
    #define GREE_GM_UART_40005000_BASE_ADDRESS  0x40005000
    #define GREE_GM_UART_40005000_CLOCKSRC      8000000
    #define GREE_GM_UART_40005000_CURRENT_SPEED 19200
    #define GREE_GM_UART_40005000_IRQ_0     19
    #define GREE_GM_UART_40005000_IRQ_0_PRIORITY    3
    #define GREE_GM_UART_40005000_LABEL     "UART_1"
    #define GREE_GM_UART_40005000_PARITY        1
    #define GREE_GM_UART_40005000_SIZE      4096
    #define GREE_GM_UART_40005000_STOPBITS      0
    #define GREE_GM_UART_40005000_WORDLENGTH    3
    #define UART1_BASE_ADDRESS          GREE_GM_UART_40005000_BASE_ADDRESS
    #define UART1_CLOCKSRC              GREE_GM_UART_40005000_CLOCKSRC
    #define UART1_CURRENT_SPEED         GREE_GM_UART_40005000_CURRENT_SPEED
    #define UART1_IRQ               GREE_GM_UART_40005000_IRQ_0
    #define UART1_IRQ_PRIORITY          GREE_GM_UART_40005000_IRQ_0_PRIORITY
    #define UART1_LABEL             GREE_GM_UART_40005000_LABEL
    #define UART1_PARITY                GREE_GM_UART_40005000_PARITY
    #define UART1_SIZE              GREE_GM_UART_40005000_SIZE
    #define UART1_STOPBITS              GREE_GM_UART_40005000_STOPBITS
    #define UART1_WORDLENGTH            GREE_GM_UART_40005000_WORDLENGTH

    /* uart@40006000 */
    #define CONFIG_UART_CONSOLE_ON_DEV_NAME     "UART_2"
    #define GREE_GM_UART_40006000_BASE_ADDRESS  0x40006000
    #define GREE_GM_UART_40006000_CLOCKSRC      8000000
    #define GREE_GM_UART_40006000_CURRENT_SPEED 19200
    #define GREE_GM_UART_40006000_IRQ_0     20
    #define GREE_GM_UART_40006000_IRQ_0_PRIORITY    3
    #define GREE_GM_UART_40006000_LABEL     "UART_2"
    #define GREE_GM_UART_40006000_PARITY        1
    #define GREE_GM_UART_40006000_SIZE      4096
    #define GREE_GM_UART_40006000_STOPBITS      0
    #define GREE_GM_UART_40006000_WORDLENGTH    3
    #define UART2_BASE_ADDRESS          GREE_GM_UART_40006000_BASE_ADDRESS
    #define UART2_CLOCKSRC              GREE_GM_UART_40006000_CLOCKSRC
    #define UART2_CURRENT_SPEED         GREE_GM_UART_40006000_CURRENT_SPEED
    #define UART2_IRQ               GREE_GM_UART_40006000_IRQ_0
    #define UART2_IRQ_PRIORITY          GREE_GM_UART_40006000_IRQ_0_PRIORITY
    #define UART2_LABEL             GREE_GM_UART_40006000_LABEL
    #define UART2_PARITY                GREE_GM_UART_40006000_PARITY
    #define UART2_SIZE              GREE_GM_UART_40006000_SIZE
    #define UART2_STOPBITS              GREE_GM_UART_40006000_STOPBITS
    #define UART2_WORDLENGTH            GREE_GM_UART_40006000_WORDLENGTH

    /* vin@40025000 */
    #define GREE_GM_VIN_40025000_BASE_ADDRESS   0x40025000
    #define GREE_GM_VIN_40025000_IRQ_0      47
    #define GREE_GM_VIN_40025000_IRQ_0_PRIORITY 3
    #define GREE_GM_VIN_40025000_LABEL      "VIN"
    #define GREE_GM_VIN_40025000_SIZE       44

    /* vout@40026000 */
    #define GREE_GM_VOUT_40026000_BASE_ADDRESS  0x40026000
    #define GREE_GM_VOUT_40026000_HBP       40
    #define GREE_GM_VOUT_40026000_HEIGHT        272
    #define GREE_GM_VOUT_40026000_HFP       5
    #define GREE_GM_VOUT_40026000_HSW       1
    #define GREE_GM_VOUT_40026000_IRQ_0     48
    #define GREE_GM_VOUT_40026000_IRQ_0_PRIORITY    3
    #define GREE_GM_VOUT_40026000_LABEL     "VOUT"
    #define GREE_GM_VOUT_40026000_PLLSAIDIVR    4
    #define GREE_GM_VOUT_40026000_PLLSAIN       16
    #define GREE_GM_VOUT_40026000_PLLSAIR       8
    #define GREE_GM_VOUT_40026000_SIZE      328
    #define GREE_GM_VOUT_40026000_VBP       8
    #define GREE_GM_VOUT_40026000_VFP       8
    #define GREE_GM_VOUT_40026000_VSW       1
    #define GREE_GM_VOUT_40026000_WIDTH     480

    /* wwdg@4003E000 */
    #define GREE_GM_WWDG_4003E000_BASE_ADDRESS  0x4003e000
    #define GREE_GM_WWDG_4003E000_IRQ_0     0
    #define GREE_GM_WWDG_4003E000_IRQ_0_PRIORITY    3
    #define GREE_GM_WWDG_4003E000_LABEL     "WWDG"
    #define GREE_GM_WWDG_4003E000_SIZE      4096
    #define WWDG_BASE_ADDRESS           GREE_GM_WWDG_4003E000_BASE_ADDRESS
    #define WWDG_IRQ                GREE_GM_WWDG_4003E000_IRQ_0
    #define WWDG_IRQ_PRIORITY           GREE_GM_WWDG_4003E000_IRQ_0_PRIORITY
    #define WWDG_LABEL              GREE_GM_WWDG_4003E000_LABEL
    #define WWDG_SIZE               GREE_GM_WWDG_4003E000_SIZE





    /* Following definitions fixup the generated include */
    /* This file is a temporary workaround for mapping of the generated information
    * to the current driver definitions.  This will be removed when the drivers
    * are modified to handle the generated information, or the mapping of
    * generated data matches the driver definitions.
    */
    /* SoC level DTS fixup file */

    #define CONFIG_NUM_IRQ_PRIO_BITS            ARM_V7M_NVIC_E000E100_ARM_NUM_IRQ_PRIORITY_BITS

    #ifdef UART0_LABEL
        /*rename uart0 dts generate marco*/
        #define CONFIG_UART0                UART0_LABEL
        #define CONFIG_UART0_BASE           UART0_BASE_ADDRESS
        #define CONFIG_UART0_REG_LENGTH     UART0_WORDLENGTH
        #define CONFIG_UART0_IRQ_NUM        UART0_IRQ
        #define CONFIG_UART0_IRQ_PRIORITY   UART0_IRQ_PRIORITY
    #endif

    #ifdef UART1_LABEL
        /*rename uart1 dts generate marco*/
        #define CONFIG_UART1                UART1_LABEL
        #define CONFIG_UART1_BASE           UART1_BASE_ADDRESS
        #define CONFIG_UART1_REG_LENGTH     UART1_WORDLENGTH
        #define CONFIG_UART1_IRQ_NUM        UART1_IRQ
        #define CONFIG_UART1_IRQ_PRIORITY   UART1_IRQ_PRIORITY
    #endif

    #ifdef UART2_LABEL
        /*rename uart2 dts generate marco*/
        #define CONFIG_UART2                UART2_LABEL
        #define CONFIG_UART2_BASE           UART2_BASE_ADDRESS
        #define CONFIG_UART2_REG_LENGTH     UART2_WORDLENGTH
        #define CONFIG_UART2_IRQ_NUM        UART2_IRQ
        #define CONFIG_UART2_IRQ_PRIORITY   UART2_IRQ_PRIORITY
    #endif

    #ifdef UART3_LABEL
        /*rename uart3 dts generate marco*/
        #define CONFIG_UART3                UART3_LABEL
        #define CONFIG_UART3_BASE           UART3_BASE_ADDRESS
        #define CONFIG_UART3_REG_LENGTH     UART3_WORDLENGTH
        #define CONFIG_UART3_IRQ_NUM        UART3_IRQ
        #define CONFIG_UART3_IRQ_PRIORITY   UART3_IRQ_PRIORITY
    #endif

    #ifdef I2C0_LABEL
        /*rename i2c0 dts generate marco*/
        #define CONFIG_I2C0                 I2C0_LABEL
        #define CONFIG_I2C0_BASE            I2C0_BASE_ADDRESS
        #define CONFIG_I2C0_REG_LENGTH      I2C0_WORDLENGTH
        #define CONFIG_I2C0_IRQ_NUM         I2C0_IRQ
        #define CONFIG_I2C0_IRQ_PRIORITY    I2C0_IRQ_PRIORITY
    #endif

    #ifdef I2C1_LABEL
        /*rename i2c1 dts generate marco*/
        #define CONFIG_I2C1                 I2C1_LABEL
        #define CONFIG_I2C1_BASE            I2C1_BASE_ADDRESS
        #define CONFIG_I2C1_REG_LENGTH      I2C1_WORDLENGTH
        #define CONFIG_I2C1_IRQ_NUM         I2C1_IRQ
        #define CONFIG_I2C1_IRQ_PRIORITY    I2C1_IRQ_PRIORITY
    #endif

    #ifdef I2C0_LABEL
        /*rename i2c0 dts generate marco*/
        #define CONFIG_I2C0_BAUDRATE        I2C0_CURRENT_SPEED
        #define CONFIG_I2C0_ADDRMODE        I2C0_ADDRMODE
        #define CONFIG_I2C0_MASTERTXMODE    I2C0_MASTERTXMODE
        #define CONFIG_I2C0_SPEEDMODE       I2C0_SPEEDMODE
        #define CONFIG_I2C0_CLOCKSRC        I2C0_CLOCKSRC
        #define CONFIG_I2C0_MASTER_MODE     I2C0_WORKMODE
        #define CONFIG_I2C0_LOCAL_ADDRESS   I2C0_LOCALADDR
    #endif

    #ifdef I2C1_LABEL
        /*rename i2c1 dts generate marco*/
        #define CONFIG_I2C1_BAUDRATE        I2C1_CURRENT_SPEED
        #define CONFIG_I2C1_ADDRMODE        I2C1_ADDRMODE
        #define CONFIG_I2C1_MASTERTXMODE    I2C1_MASTERTXMODE
        #define CONFIG_I2C1_SPEEDMODE       I2C1_SPEEDMODE
        #define CONFIG_I2C1_CLOCKSRC        I2C1_CLOCKSRC
        #define CONFIG_I2C1_MASTER_MODE     I2C1_WORKMODE
        #define CONFIG_I2C1_LOCAL_ADDRESS   I2C1_LOCALADDR
    #endif

    #ifdef I2C2_LABEL
        /*rename i2c2 dts generate marco*/
        #define CONFIG_I2C2                 I2C2_LABEL
        #define CONFIG_I2C2_BASE            I2C2_BASE_ADDRESS
        #define CONFIG_I2C2_REG_LENGTH      I2C2_WORDLENGTH
        #define CONFIG_I2C2_IRQ_NUM         I2C2_IRQ
        #define CONFIG_I2C2_IRQ_PRIORITY    I2C2_IRQ_PRIORITY
    #endif


    #ifdef I2C2_LABEL
        /*rename i2c2 dts generate marco*/
        #define CONFIG_I2C2_BAUDRATE        I2C2_CURRENT_SPEED
        #define CONFIG_I2C2_ADDRMODE        I2C2_ADDRMODE
        #define CONFIG_I2C2_MASTERTXMODE    I2C2_MASTERTXMODE
        #define CONFIG_I2C2_SPEEDMODE       I2C2_SPEEDMODE
        #define CONFIG_I2C2_CLOCKSRC        I2C2_CLOCKSRC
        #define CONFIG_I2C2_MASTER_MODE     I2C2_WORKMODE
        #define CONFIG_I2C2_LOCAL_ADDRESS   I2C2_LOCALADDR
        #define CONFIG_I2C2_s_dmabuffer     I2C2_s_dmabuffer
    #endif



    #ifdef UART4_LABEL
        /*rename uart4 dts generate marco*/
        #define CONFIG_UART4                UART4_LABEL
        #define CONFIG_UART4_BASE           UART4_BASE_ADDRESS
        #define CONFIG_UART4_REG_LENGTH     UART4_WORDLENGTH
        #define CONFIG_UART4_IRQ_NUM        UART4_IRQ
        #define CONFIG_UART4_IRQ_PRIORITY   UART4_IRQ_PRIORITY
    #endif

    #ifdef UART5_LABEL
        /*rename uart5 dts generate marco*/
        #define CONFIG_UART5                UART5_LABEL
        #define CONFIG_UART5_BASE           UART5_BASE_ADDRESS
        #define CONFIG_UART5_REG_LENGTH     UART5_WORDLENGTH
        #define CONFIG_UART5_IRQ_NUM        UART5_IRQ
        #define CONFIG_UART5_IRQ_PRIORITY   UART5_IRQ_PRIORITY
    #endif

    #ifdef UART6_LABEL
        /*rename uart6 dts generate marco*/
        #define CONFIG_UART6                UART6_LABEL
        #define CONFIG_UART6_BASE           UART6_BASE_ADDRESS
        #define CONFIG_UART6_REG_LENGTH     UART6_WORDLENGTH
        #define CONFIG_UART6_IRQ_NUM        UART6_IRQ
        #define CONFIG_UART6_IRQ_PRIORITY   UART6_IRQ_PRIORITY
    #endif

    #ifdef UART7_LABEL
        /*rename uart7 dts generate marco*/
        #define CONFIG_UART7                UART7_LABEL
        #define CONFIG_UART7_BASE           UART7_BASE_ADDRESS
        #define CONFIG_UART7_REG_LENGTH     UART7_WORDLENGTH
        #define CONFIG_UART7_IRQ_NUM        UART7_IRQ
        #define CONFIG_UART7_IRQ_PRIORITY   UART7_IRQ_PRIORITY
    #endif


    #ifdef IWDG_LABEL
        /*rename IWDG dts generate marco*/
        #define CONFIG_IWDG_NAME             IWDG_LABEL
        #define CONFIG_IWDG_BASE_ADDRESS     IWDG_BASE_ADDRESS
    #endif

    #ifdef WWDG_LABEL
        /*rename IWDG dts generate marco*/
        #define CONFIG_WWDG_NAME             WWDG_LABEL
        #define CONFIG_WWDG_BASE_ADDRESS     WWDG_BASE_ADDRESS
    #endif

    #ifdef RTC_LABEL
        /*rename RCT dts generate marco*/
        #define CONFIG_RTC_NAME             RTC_LABEL
        #define CONFIG_RTC_BASE_ADDRESS     RTC_BASE_ADDRESS
    #endif

    #ifdef LVD_LABEL
        /*rename LVD dts generate marco*/
        #define CONFIG_LVD_NAME             LVD_LABEL
        #define CONFIG_LVD_BASE_ADDRESS     LVD_BASE_ADDRESS
    #endif

    #ifdef TIM1_LABEL
        /*rename tim1 dts generate marco*/
        #define CONFIG_TIM1_NAME                     TIM1_LABEL
        #define CONFIG_TIM1_BASE                     TIM1_BASE_ADDRESS
        #define CONFIG_TIM1_IRQ_NUM                  TIM1_IRQ
        #define CONFIG_TIM1_IRQ_PRIORITY             TIM1_IRQ_PRIORITY
    #endif

    #ifdef TIM2_LABEL
        /*rename tim2 dts generate marco*/
        #define CONFIG_TIM2_NAME                     TIM2_LABEL
        #define CONFIG_TIM2_BASE                     TIM2_BASE_ADDRESS
        #define CONFIG_TIM2_IRQ_NUM                  TIM2_IRQ
        #define CONFIG_TIM2_IRQ_PRIORITY             TIM2_IRQ_PRIORITY
    #endif

    #ifdef TIM3_LABEL
        /*rename tim3 dts generate marco*/
        #define CONFIG_TIM3_NAME                     TIM3_LABEL
        #define CONFIG_TIM3_BASE                     TIM3_BASE_ADDRESS
        #define CONFIG_TIM3_IRQ_NUM                  TIM3_IRQ
        #define CONFIG_TIM3_IRQ_PRIORITY             TIM3_IRQ_PRIORITY
    #endif

    #ifdef TIM4_LABEL
        /*rename tim4 dts generate marco*/
        #define CONFIG_TIM4_NAME                     TIM4_LABEL
        #define CONFIG_TIM4_BASE                     TIM4_BASE_ADDRESS
        #define CONFIG_TIM4_IRQ_NUM                  TIM4_IRQ
        #define CONFIG_TIM4_IRQ_PRIORITY             TIM4_IRQ_PRIORITY
    #endif

    #ifdef TIM5_LABEL
        /*rename tim5 dts generate marco*/
        #define CONFIG_TIM5_NAME                     TIM5_LABEL
        #define CONFIG_TIM5_BASE                     TIM5_BASE_ADDRESS
        #define CONFIG_TIM5_IRQ_NUM                  TIM5_IRQ
        #define CONFIG_TIM5_IRQ_PRIORITY             TIM5_IRQ_PRIORITY
    #endif

    #ifdef TIM8_LABEL
        /*rename tim8 dts generate marco*/
        #define CONFIG_TIM8_NAME                     TIM8_LABEL
        #define CONFIG_TIM8_BASE                     TIM8_BASE_ADDRESS
        #define CONFIG_TIM8_IRQ_NUM                  TIM8_IRQ
        #define CONFIG_TIM8_IRQ_PRIORITY             TIM8_IRQ_PRIORITY
    #endif

    #ifdef TIM9_LABEL
        /*rename tim9 dts generate marco*/
        #define CONFIG_TIM9_NAME                     TIM9_LABEL
        #define CONFIG_TIM9_BASE                     TIM9_BASE_ADDRESS
        #define CONFIG_TIM9_IRQ_NUM                  TIM9_IRQ
        #define CONFIG_TIM9_IRQ_PRIORITY             TIM9_IRQ_PRIORITY
    #endif

    #ifdef TIM10_LABEL
        /*rename tim10 dts generate marco*/
        #define CONFIG_TIM10_NAME                     TIM10_LABEL
        #define CONFIG_TIM10_BASE                     TIM10_BASE_ADDRESS
        #define CONFIG_TIM10_IRQ_NUM                  TIM10_IRQ
        #define CONFIG_TIM10_IRQ_PRIORITY             TIM10_IRQ_PRIORITY
    #endif

    #ifdef TIM11_LABEL
        /*rename tim11 dts generate marco*/
        #define CONFIG_TIM11_NAME                     TIM11_LABEL
        #define CONFIG_TIM11_BASE                     TIM11_BASE_ADDRESS
        #define CONFIG_TIM11_IRQ_NUM                  TIM11_IRQ
        #define CONFIG_TIM11_IRQ_PRIORITY             TIM11_IRQ_PRIORITY
    #endif

    #ifdef TIM12_LABEL
        /*rename tim12 dts generate marco*/
        #define CONFIG_TIM12_NAME                     TIM12_LABEL
        #define CONFIG_TIM12_BASE                     TIM12_BASE_ADDRESS
        #define CONFIG_TIM12_IRQ_NUM                  TIM12_IRQ
        #define CONFIG_TIM12_IRQ_PRIORITY             TIM12_IRQ_PRIORITY
    #endif

    #ifdef TIM13_LABEL
        /*rename tim13 dts generate marco*/
        #define CONFIG_TIM13_NAME                     TIM13_LABEL
        #define CONFIG_TIM13_BASE                     TIM13_BASE_ADDRESS
        #define CONFIG_TIM13_IRQ_NUM                  TIM13_IRQ
        #define CONFIG_TIM13_IRQ_PRIORITY             TIM13_IRQ_PRIORITY
    #endif

    #ifdef TIM14_LABEL
        /*rename tim14 dts generate marco*/
        #define CONFIG_TIM14_NAME                     TIM14_LABEL
        #define CONFIG_TIM14_BASE                     TIM14_BASE_ADDRESS
        #define CONFIG_TIM14_IRQ_NUM                  TIM14_IRQ
        #define CONFIG_TIM14_IRQ_PRIORITY             TIM14_IRQ_PRIORITY
    #endif

    #ifdef POWER_LABEL
        #define CONFIG_PMU_IRQn             POWER_IRQ
        #define CONFIG_PMU_IRQ_PRIORITY     POWER_IRQ_PRIORITY
    #endif
    /* End of SoC Level DTS fixup file */


    /* Following definitions fixup the generated include */
    /* board level DTS fixup file */

    #ifdef UART0_LABEL
        /*rename uart0 dts generate marco*/
        #define CONFIG_UART0_BAUDRATE       UART0_CURRENT_SPEED
        #define CONFIG_UART0_PARITY         UART0_PARITY
        #define CONFIG_UART0_STOPBITS       UART0_STOPBITS
        #define CONFIG_UART0_DATABITS       UART0_WORDLENGTH
        #define CONFIG_UART0_CLOCKSRC       UART0_CLOCKSRC
    #endif

    #ifdef UART1_LABEL
        /*rename uart1 dts generate marco*/
        #define CONFIG_UART1_BAUDRATE       UART1_CURRENT_SPEED
        #define CONFIG_UART1_PARITY         UART1_PARITY
        #define CONFIG_UART1_STOPBITS       UART1_STOPBITS
        #define CONFIG_UART1_DATABITS       UART1_WORDLENGTH
        #define CONFIG_UART1_CLOCKSRC       UART1_CLOCKSRC
    #endif

    #ifdef UART2_LABEL
        /*rename uart2 dts generate marco*/
        #define CONFIG_UART2_BAUDRATE       UART2_CURRENT_SPEED
        #define CONFIG_UART2_PARITY         UART2_PARITY
        #define CONFIG_UART2_STOPBITS       UART2_STOPBITS
        #define CONFIG_UART2_DATABITS       UART2_WORDLENGTH
        #define CONFIG_UART2_CLOCKSRC       UART2_CLOCKSRC
    #endif

    #ifdef IWDG_LABEL
        /*rename IWDG dts generate marco*/
        #define CONFIG_IWDG_NAME             IWDG_LABEL
        #define CONFIG_IWDG_BASE_ADDRESS     IWDG_BASE_ADDRESS
    #endif

    #ifdef WWDG_LABEL
        /*rename IWDG dts generate marco*/
        #define CONFIG_WWDG_NAME             WWDG_LABEL
        #define CONFIG_WWDG_BASE_ADDRESS     WWDG_BASE_ADDRESS
    #endif

    /*rename VOUT dts generate marco*/
    #define CONFIG_VOUT_BASE_ADDRESS       GREE_GM_VOUT_40026000_BASE_ADDRESS
    #define CONFIG_VOUT_DEFAULT_VBP        GREE_GM_VOUT_40026000_VBP
    #define CONFIG_VOUT_DEFAULT_VFP        GREE_GM_VOUT_40026000_VFP
    #define CONFIG_VOUT_DEFAULT_HBP        GREE_GM_VOUT_40026000_HBP
    #define CONFIG_VOUT_DEFAULT_HFP        GREE_GM_VOUT_40026000_HFP
    #define CONFIG_VOUT_DEFAULT_VSW        GREE_GM_VOUT_40026000_VSW
    #define CONFIG_VOUT_DEFAULT_HSW        GREE_GM_VOUT_40026000_HSW
    #define CONFIG_VOUT_DEFAULT_PLLSAIN    GREE_GM_VOUT_40026000_PLLSAIN
    #define CONFIG_VOUT_DEFAULT_PLLSAIR    GREE_GM_VOUT_40026000_PLLSAIR
    #define CONFIG_VOUT_DEFAULT_PLLSAIDIVR GREE_GM_VOUT_40026000_PLLSAIDIVR
    #define CONFIG_VOUT_DEFAULT_WIDTH      GREE_GM_VOUT_40026000_WIDTH
    #define CONFIG_VOUT_DEFAULT_HEIGHT     GREE_GM_VOUT_40026000_HEIGHT
    #define CONFIG_VOUT_NAME               GREE_GM_VOUT_40026000_LABEL
    #define CONFIG_VOUT_IRQ_PRI            GREE_GM_VOUT_40026000_IRQ_0_PRIORITY
    #define VOUT_IRQ                       GREE_GM_VOUT_40026000_IRQ_0

    /*rename some marco about ov5640 camera sensor*/
    #define OV5640_0_GPIO_RESET_PIN          GREE_CAMERA_OV5640_0_GPIO_RST_PIN
    #define OV5640_0_GPIO_RESET_PORT         GREE_CAMERA_OV5640_0_GPIO_RST_PORT
    #define OV5640_0_GPIO_PWDN_PIN           GREE_CAMERA_OV5640_0_GPIO_PWDN_PIN
    #define OV5640_0_GPIO_PWDN_PORT          GREE_CAMERA_OV5640_0_GPIO_PWDN_PORT
    #define OV5640_0_DEV_NAME                  GREE_CAMERA_OV5640_0_LABEL

    #define OV5640_1_GPIO_RESET_PIN          GREE_CAMERA_OV5640_1_GPIO_RST_PIN
    #define OV5640_1_GPIO_RESET_PORT         GREE_CAMERA_OV5640_1_GPIO_RST_PORT
    #define OV5640_1_GPIO_PWDN_PIN           GREE_CAMERA_OV5640_1_GPIO_PWDN_PIN
    #define OV5640_1_GPIO_PWDN_PORT          GREE_CAMERA_OV5640_1_GPIO_PWDN_PORT
    #define OV5640_1_DEV_NAME                  GREE_CAMERA_OV5640_1_LABEL

    #define OV5640_PCLK_POLARITY             GREE_CAMERA_OV5640_0_PCLK_POLARITY
    #define OV5640_HSYNC_POLARITY            GREE_CAMERA_OV5640_0_HSYNC_POLARITY
    #define OV5640_VSYNC_POLARITY            GREE_CAMERA_OV5640_0_VSYNC_POLARITY

    /*rename some marco about VIN modules*/
    #define CONFIG_VIN_BASE_ADDRESS        GREE_GM_VIN_40025000_BASE_ADDRESS
    #define CONFIG_VIN_NAME                GREE_GM_VIN_40025000_LABEL
    #define CONFIG_VIN_IRQ_PRI             GREE_GM_VIN_40025000_IRQ_0_PRIORITY
    #define VIN_IRQ                        GREE_GM_VIN_40025000_IRQ_0

    #define CONFIG_IMP_BASE_ADDRESS    GREE_GM_IMP_40024000_BASE_ADDRESS

    #ifdef MMC0_LABEL
        /*rename mmc0 dts generate marco*/
        #define CONFIG_MMC0_BUSWITCH        MMC0_BUSWITCH
        #define CONFIG_MMC0_CARD_REMOVABLE  MMC0_REMOVABLE
        #define CONFIG_MMC0_CARD_TYPE       MMC0_TYPE
        #ifdef  MMC0_POLL_DETECT_CARD_HOTPLUG
            #define CONFIG_HOTPLUG_POLL_DETECT
        #endif

        #ifdef CAN0_LABEL
            /*rename can0 dts generate marco*/
            #define CONFIG_CAN0_BAUDRATE            CAN0_BAUDRATE
            #define CONFIG_CAN0_BS1                 CAN0_BS1
            #define CONFIG_CAN0_BS2                 CAN0_BS2
            #define CONFIG_CAN0_ERRWARNTHRESHOLD    CAN0_ERRWARNTHRESHOLD
            #define CONFIG_CAN0_SAMPLE              CAN0_SAMPLE
            #define CONFIG_CAN0_SJW                 CAN0_SJW
            #define CONFIG_CAN0_WORKMODE            CAN0_WORKMODE
        #endif

        #ifdef RTC_LABEL
            /*rename RTC dts generate marco*/
            #define CONFIG_RTC_NAME             RTC_LABEL
            #define CONFIG_RTC_BASE_ADDRESS     RTC_BASE_ADDRESS
        #endif

        #ifdef LVD_LABEL
            /*rename LVD dts generate marco*/
            #define CONFIG_LVD_NAME             LVD_LABEL
            #define CONFIG_LVD_BASE_ADDRESS     LVD_BASE_ADDRESS
        #endif


        #ifdef POWER_LABEL
            /*rename power dts generate marco for power*/
            #define WKPIN_XPA0_VAILD_EDGE       GREE_GM_POWER_40000004_WKPIN_XPA0_VAILD_EDGE
            #define WKPIN_XPA7_VAILD_EDGE       GREE_GM_POWER_40000004_WKPIN_XPA7_VAILD_EDGE
            #define WKPIN_XPA8_VAILD_EDGE       GREE_GM_POWER_40000004_WKPIN_XPA8_VAILD_EDGE
        #endif

    #endif




    /* End of SoC Level DTS fixup file */


#endif
                                                                                                                        