/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GM_HAL_FMC_H
#define __GM_HAL_FMC_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef struct
{
  __IO uint32_t BTCR[8];    /*!< NOR/PSRAM chip-select control register(BCR) and chip-select timing register(BTR), Address offset: 0x00-1C */   
} FMC_Bank1_TypeDef; 

/** 
  * @brief Flexible Memory Controller Bank1E
  */
  
typedef struct
{
  __IO uint32_t BWTR[7];    /*!< NOR/PSRAM write timing registers, Address offset: 0x104-0x11C */
} FMC_Bank1E_TypeDef;

#define FMC_R_BASE            ((uint32_t)0xA0000000) /*!< todo, FMC registers base address */

#define FMC_Bank1_R_BASE      (FMC_R_BASE + 0x0000) //todo
#define FMC_Bank1E_R_BASE     (FMC_R_BASE + 0x0104) //todo

/** 
  * @brief  FMC NORSRAM Configuration Structure definition
  */ 
typedef struct
{
  uint32_t NSBank;                       /*!< Specifies the NORSRAM memory device that will be used.
                                              This parameter can be a value of @ref FMC_NORSRAM_Bank                     */

  uint32_t DataAddressMux;               /*!< Specifies whether the address and data values are
                                              multiplexed on the data bus or not. 
                                              This parameter can be a value of @ref FMC_Data_Address_Bus_Multiplexing    */

  uint32_t MemoryType;                   /*!< Specifies the type of external memory attached to
                                              the corresponding memory device.
                                              This parameter can be a value of @ref FMC_Memory_Type                      */

  uint32_t MemoryDataWidth;              /*!< Specifies the external memory device width.
                                              This parameter can be a value of @ref FMC_NORSRAM_Data_Width               */

  uint32_t BurstAccessMode;              /*!< Enables or disables the burst access mode for Flash memory,
                                              valid only with synchronous burst Flash memories.
                                              This parameter can be a value of @ref FMC_Burst_Access_Mode                */

  uint32_t WaitSignalPolarity;           /*!< Specifies the wait signal polarity, valid only when accessing
                                              the Flash memory in burst mode.
                                              This parameter can be a value of @ref FMC_Wait_Signal_Polarity             */

  uint32_t WrapMode;                     /*!< Enables or disables the Wrapped burst access mode for Flash
                                              memory, valid only when accessing Flash memories in burst mode.
                                              This parameter can be a value of @ref FMC_Wrap_Mode
                                              This mode is not available for the STM32F446/467/479xx devices                    */

  uint32_t WaitSignalActive;             /*!< Specifies if the wait signal is asserted by the memory one
                                              clock cycle before the wait state or during the wait state,
                                              valid only when accessing memories in burst mode. 
                                              This parameter can be a value of @ref FMC_Wait_Timing                      */

  uint32_t WriteOperation;               /*!< Enables or disables the write operation in the selected device by the FMC. 
                                              This parameter can be a value of @ref FMC_Write_Operation                  */

  uint32_t WaitSignal;                   /*!< Enables or disables the wait state insertion via wait
                                              signal, valid for Flash memory access in burst mode. 
                                              This parameter can be a value of @ref FMC_Wait_Signal                      */

  uint32_t ExtendedMode;                 /*!< Enables or disables the extended mode.
                                              This parameter can be a value of @ref FMC_Extended_Mode                    */

  uint32_t AsynchronousWait;             /*!< Enables or disables wait signal during asynchronous transfers,
                                              valid only with asynchronous Flash memories.
                                              This parameter can be a value of @ref FMC_AsynchronousWait                 */

  uint32_t WriteBurst;                   /*!< Enables or disables the write burst operation.
                                              This parameter can be a value of @ref FMC_Write_Burst                      */

  uint32_t ContinuousClock;              /*!< Enables or disables the FMC clock output to external memory devices.
                                              This parameter is only enabled through the FMC_BCR1 register, and don't care 
                                              through FMC_BCR2..4 registers.
                                              This parameter can be a value of @ref FMC_Continous_Clock                  */

  uint32_t WriteFifo;                    /*!< Enables or disables the write FIFO used by the FMC controller.
                                              This parameter is only enabled through the FMC_BCR1 register, and don't care 
                                              through FMC_BCR2..4 registers.
                                              This parameter can be a value of @ref FMC_Write_FIFO
                                              This mode is available only for the STM32F446/469/479xx devices            */

  uint32_t PageSize;                     /*!< Specifies the memory page size.
                                              This parameter can be a value of @ref FMC_Page_Size                        */
}FMC_NORSRAM_InitTypeDef;

/** 
  * @brief  FMC NORSRAM Timing parameters structure definition  
  */
typedef struct
{
  uint32_t AddressSetupTime;             /*!< Defines the number of HCLK cycles to configure
                                              the duration of the address setup time. 
                                              This parameter can be a value between Min_Data = 0 and Max_Data = 15.
                                              @note This parameter is not used with synchronous NOR Flash memories.      */

  uint32_t AddressHoldTime;              /*!< Defines the number of HCLK cycles to configure
                                              the duration of the address hold time.
                                              This parameter can be a value between Min_Data = 1 and Max_Data = 15. 
                                              @note This parameter is not used with synchronous NOR Flash memories.      */

  uint32_t DataSetupTime;                /*!< Defines the number of HCLK cycles to configure
                                              the duration of the data setup time.
                                              This parameter can be a value between Min_Data = 1 and Max_Data = 255.
                                              @note This parameter is used for SRAMs, ROMs and asynchronous multiplexed 
                                              NOR Flash memories.                                                        */

  uint32_t BusTurnAroundDuration;        /*!< Defines the number of HCLK cycles to configure
                                              the duration of the bus turnaround.
                                              This parameter can be a value between Min_Data = 0 and Max_Data = 15.
                                              @note This parameter is only used for multiplexed NOR Flash memories.      */

  uint32_t CLKDivision;                  /*!< Defines the period of CLK clock output signal, expressed in number of 
                                              HCLK cycles. This parameter can be a value between Min_Data = 2 and Max_Data = 16.
                                              @note This parameter is not used for asynchronous NOR Flash, SRAM or ROM 
                                              accesses.                                                                  */

  uint32_t DataLatency;                  /*!< Defines the number of memory clock cycles to issue
                                              to the memory before getting the first data.
                                              The parameter value depends on the memory type as shown below:
                                              - It must be set to 0 in case of a CRAM
                                              - It is don't care in asynchronous NOR, SRAM or ROM accesses
                                              - It may assume a value between Min_Data = 2 and Max_Data = 17 in NOR Flash memories
                                                with synchronous burst mode enable                                       */

  uint32_t AccessMode;                   /*!< Specifies the asynchronous access mode. 
                                              This parameter can be a value of @ref FMC_Access_Mode                      */
}FMC_NORSRAM_TimingTypeDef;

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup FMC_LL_Private_Constants FMC Private Constants
  * @{
  */

/** @defgroup FMC_LL_NOR_SRAM_Controller FMC NOR/SRAM Controller 
  * @{
  */ 
/** @defgroup FMC_NORSRAM_Bank FMC NOR/SRAM Bank
  * @{
  */
#define FMC_NORSRAM_BANK1                       0x00000000U
#define FMC_NORSRAM_BANK2                       0x00000002U
#define FMC_NORSRAM_BANK3                       0x00000004U
#define FMC_NORSRAM_BANK4                       0x00000006U
/**
  * @}
  */

/** @defgroup FMC_Data_Address_Bus_Multiplexing FMC Data Address Bus Multiplexing 
  * @{
  */
#define FMC_DATA_ADDRESS_MUX_DISABLE            0x00000000U
#define FMC_DATA_ADDRESS_MUX_ENABLE             0x00000002U
/**
  * @}
  */

/** @defgroup FMC_Memory_Type FMC Memory Type 
  * @{
  */
#define FMC_MEMORY_TYPE_SRAM                    0x00000000U
#define FMC_MEMORY_TYPE_PSRAM                   0x00000004U
#define FMC_MEMORY_TYPE_NOR                     0x00000008U
/**
  * @}
  */

/** @defgroup FMC_NORSRAM_Data_Width FMC NORSRAM Data Width
  * @{
  */
#define FMC_NORSRAM_MEM_BUS_WIDTH_8             0x00000000U
#define FMC_NORSRAM_MEM_BUS_WIDTH_16            0x00000010U
#define FMC_NORSRAM_MEM_BUS_WIDTH_32            0x00000020U
/**
  * @}
  */

/** @defgroup FMC_NORSRAM_Flash_Access FMC NOR/SRAM Flash Access
  * @{
  */
#define FMC_NORSRAM_FLASH_ACCESS_ENABLE         0x00000040U
#define FMC_NORSRAM_FLASH_ACCESS_DISABLE        0x00000000U
/**
  * @}
  */

/** @defgroup FMC_Burst_Access_Mode FMC Burst Access Mode 
  * @{
  */
#define FMC_BURST_ACCESS_MODE_DISABLE           0x00000000U 
#define FMC_BURST_ACCESS_MODE_ENABLE            0x00000100U
/**
  * @}
  */

/** @defgroup FMC_Wait_Signal_Polarity FMC Wait Signal Polarity 
  * @{
  */
#define FMC_WAIT_SIGNAL_POLARITY_LOW            0x00000000U
#define FMC_WAIT_SIGNAL_POLARITY_HIGH           0x00000200U
/**
  * @}
  */

/** @defgroup FMC_Wrap_Mode FMC Wrap Mode 
  * @{
  */
/** @note This mode is not available for the STM32F446/469/479xx devices
  */
#define FMC_WRAP_MODE_DISABLE                   0x00000000U
#define FMC_WRAP_MODE_ENABLE                    0x00000400U 
/**
  * @}
  */

/** @defgroup FMC_Wait_Timing FMC Wait Timing 
  * @{
  */
#define FMC_WAIT_TIMING_BEFORE_WS               0x00000000U
#define FMC_WAIT_TIMING_DURING_WS               0x00000800U
/**
  * @}
  */

/** @defgroup FMC_Write_Operation FMC Write Operation 
  * @{
  */
#define FMC_WRITE_OPERATION_DISABLE             0x00000000U
#define FMC_WRITE_OPERATION_ENABLE              0x00001000U
/**
  * @}
  */

/** @defgroup FMC_Wait_Signal FMC Wait Signal 
  * @{
  */
#define FMC_WAIT_SIGNAL_DISABLE                 0x00000000U
#define FMC_WAIT_SIGNAL_ENABLE                  0x00002000U
/**
  * @}
  */

/** @defgroup FMC_Extended_Mode FMC Extended Mode
  * @{
  */
#define FMC_EXTENDED_MODE_DISABLE               0x00000000U
#define FMC_EXTENDED_MODE_ENABLE                0x00004000U
/**
  * @}
  */

/** @defgroup FMC_AsynchronousWait FMC Asynchronous Wait 
  * @{
  */
#define FMC_ASYNCHRONOUS_WAIT_DISABLE           0x00000000U
#define FMC_ASYNCHRONOUS_WAIT_ENABLE            0x00008000U
/**
  * @}
  */  

/** @defgroup FMC_Page_Size FMC Page Size
  * @{
  */
#define FMC_PAGE_SIZE_NONE           0x00000000U
#define FMC_PAGE_SIZE_128            ((uint32_t)FMC_BCR1_CPSIZE_0)
#define FMC_PAGE_SIZE_256            ((uint32_t)FMC_BCR1_CPSIZE_1)
#define FMC_PAGE_SIZE_512            ((uint32_t)(FMC_BCR1_CPSIZE_0 | FMC_BCR1_CPSIZE_1))
#define FMC_PAGE_SIZE_1024           ((uint32_t)FMC_BCR1_CPSIZE_2)
/**
  * @}
  */


/** @defgroup FMC_Write_Burst FMC Write Burst 
  * @{
  */
#define FMC_WRITE_BURST_DISABLE                 0x00000000U
#define FMC_WRITE_BURST_ENABLE                  0x00080000U 
/**
  * @}
  */
  
/** @defgroup FMC_Continous_Clock FMC Continuous Clock 
  * @{
  */
#define FMC_CONTINUOUS_CLOCK_SYNC_ONLY          0x00000000U
#define FMC_CONTINUOUS_CLOCK_SYNC_ASYNC         0x00100000U
/**
  * @}
  */
	
/** @defgroup FMC_Access_Mode FMC Access Mode 
  * @{
  */
#define FMC_ACCESS_MODE_A                        0x00000000U
#define FMC_ACCESS_MODE_B                        0x10000000U 
#define FMC_ACCESS_MODE_C                        0x20000000U
#define FMC_ACCESS_MODE_D                        0x30000000U


/** @defgroup FMC_LL_Interrupt_definition FMC Interrupt definition  
  * @{
  */  
#define FMC_IT_RISING_EDGE                0x00000008U
#define FMC_IT_LEVEL                      0x00000010U
#define FMC_IT_FALLING_EDGE               0x00000020U
#define FMC_IT_REFRESH_ERROR              0x00004000U
/**
  * @}
  */
    
/** @defgroup FMC_LL_Flag_definition FMC Flag definition 
  * @{
  */ 
#define FMC_FLAG_RISING_EDGE                    0x00000001U
#define FMC_FLAG_LEVEL                          0x00000002U
#define FMC_FLAG_FALLING_EDGE                   0x00000004U
#define FMC_FLAG_FEMPT                          0x00000040U

#define FMC_NORSRAM_TypeDef            FMC_Bank1_TypeDef
#define FMC_NORSRAM_EXTENDED_TypeDef   FMC_Bank1E_TypeDef



//#define FMC_NORSRAM_DEVICE             FMC_Bank1
//#define FMC_NORSRAM_EXTENDED_DEVICE    FMC_Bank1E


#define __FMC_NORSRAM_ENABLE(__INSTANCE__, __BANK__)  ((__INSTANCE__)->BTCR[(__BANK__)] |= FMC_BCR1_MBKEN)

/**
  * @brief  Disable the NORSRAM device access.
  * @param  __INSTANCE__: FMC_NORSRAM Instance
  * @param  __BANK__: FMC_NORSRAM Bank   
  * @retval None
  */ 
#define __FMC_NORSRAM_DISABLE(__INSTANCE__, __BANK__) ((__INSTANCE__)->BTCR[(__BANK__)] &= ~FMC_BCR1_MBKEN)  
 
/** @defgroup FSMC_LL_Assert_Macros FSMC Assert Macros
  * @{
  */
#define IS_FMC_NORSRAM_BANK(BANK) (((BANK) == FMC_NORSRAM_BANK1) || \
                                   ((BANK) == FMC_NORSRAM_BANK2) || \
                                   ((BANK) == FMC_NORSRAM_BANK3) || \
                                   ((BANK) == FMC_NORSRAM_BANK4))

#define IS_FMC_MUX(__MUX__) (((__MUX__) == FMC_DATA_ADDRESS_MUX_DISABLE) || \
                              ((__MUX__) == FMC_DATA_ADDRESS_MUX_ENABLE))

#define IS_FMC_MEMORY(__MEMORY__) (((__MEMORY__) == FMC_MEMORY_TYPE_SRAM) || \
                                    ((__MEMORY__) == FMC_MEMORY_TYPE_PSRAM)|| \
                                    ((__MEMORY__) == FMC_MEMORY_TYPE_NOR))

#define IS_FMC_NORSRAM_MEMORY_WIDTH(__WIDTH__) (((__WIDTH__) == FMC_NORSRAM_MEM_BUS_WIDTH_8)  || \
                                                 ((__WIDTH__) == FMC_NORSRAM_MEM_BUS_WIDTH_16) || \
                                                 ((__WIDTH__) == FMC_NORSRAM_MEM_BUS_WIDTH_32))

#define IS_FMC_ACCESS_MODE(__MODE__) (((__MODE__) == FMC_ACCESS_MODE_A) || \
                                       ((__MODE__) == FMC_ACCESS_MODE_B) || \
                                       ((__MODE__) == FMC_ACCESS_MODE_C) || \
                                       ((__MODE__) == FMC_ACCESS_MODE_D))


#define IS_FMC_WAIT_FEATURE(FEATURE) (((FEATURE) == FMC_NAND_PCC_WAIT_FEATURE_DISABLE) || \
                                      ((FEATURE) == FMC_NAND_PCC_WAIT_FEATURE_ENABLE))


#define IS_FMC_TCLR_TIME(TIME) ((TIME) <= 255U)

#define IS_FMC_TAR_TIME(TIME) ((TIME) <= 255U)

#define IS_FMC_SETUP_TIME(TIME) ((TIME) <= 255U)

#define IS_FMC_WAIT_TIME(TIME) ((TIME) <= 255U)

#define IS_FMC_HOLD_TIME(TIME) ((TIME) <= 255U)

#define IS_FMC_HIZ_TIME(TIME) ((TIME) <= 255U)

//#define IS_FMC_NORSRAM_DEVICE(__INSTANCE__) ((__INSTANCE__) == FMC_NORSRAM_DEVICE)

//#define IS_FMC_NORSRAM_EXTENDED_DEVICE(__INSTANCE__) ((__INSTANCE__) == FMC_NORSRAM_EXTENDED_DEVICE)


#define IS_FMC_BURSTMODE(__STATE__) (((__STATE__) == FMC_BURST_ACCESS_MODE_DISABLE) || \
                                     ((__STATE__) == FMC_BURST_ACCESS_MODE_ENABLE))

#define IS_FMC_WAIT_POLARITY(__POLARITY__) (((__POLARITY__) == FMC_WAIT_SIGNAL_POLARITY_LOW) || \
                                            ((__POLARITY__) == FMC_WAIT_SIGNAL_POLARITY_HIGH))


#define IS_FMC_WRAP_MODE(__MODE__) (((__MODE__) == FMC_WRAP_MODE_DISABLE) || \
                                    ((__MODE__) == FMC_WRAP_MODE_ENABLE))

#define IS_FMC_WAIT_SIGNAL_ACTIVE(__ACTIVE__) (((__ACTIVE__) == FMC_WAIT_TIMING_BEFORE_WS) || \
                                                ((__ACTIVE__) == FMC_WAIT_TIMING_DURING_WS)) 

#define IS_FMC_WRITE_OPERATION(__OPERATION__) (((__OPERATION__) == FMC_WRITE_OPERATION_DISABLE) || \
                                                ((__OPERATION__) == FMC_WRITE_OPERATION_ENABLE))

#define IS_FMC_WAITE_SIGNAL(__SIGNAL__) (((__SIGNAL__) == FMC_WAIT_SIGNAL_DISABLE) || \
                                          ((__SIGNAL__) == FMC_WAIT_SIGNAL_ENABLE))

#define IS_FMC_EXTENDED_MODE(__MODE__) (((__MODE__) == FMC_EXTENDED_MODE_DISABLE) || \
                                         ((__MODE__) == FMC_EXTENDED_MODE_ENABLE))

#define IS_FMC_ASYNWAIT(__STATE__) (((__STATE__) == FMC_ASYNCHRONOUS_WAIT_DISABLE) || \
                                     ((__STATE__) == FMC_ASYNCHRONOUS_WAIT_ENABLE))

#define IS_FMC_WRITE_BURST(__BURST__) (((__BURST__) == FMC_WRITE_BURST_DISABLE) || \
                                        ((__BURST__) == FMC_WRITE_BURST_ENABLE))

#define IS_FMC_CONTINOUS_CLOCK(CCLOCK) (((CCLOCK) == FMC_CONTINUOUS_CLOCK_SYNC_ONLY) || \
                                        ((CCLOCK) == FMC_CONTINUOUS_CLOCK_SYNC_ASYNC))

#define IS_FMC_ADDRESS_SETUP_TIME(__TIME__) ((__TIME__) <= 15U)

#define IS_FMC_ADDRESS_HOLD_TIME(__TIME__) (((__TIME__) > 0U) && ((__TIME__) <= 15U))

#define IS_FMC_DATASETUP_TIME(__TIME__) (((__TIME__) > 0U) && ((__TIME__) <= 255U))

#define IS_FMC_TURNAROUND_TIME(__TIME__) ((__TIME__) <= 15U)

#define IS_FMC_DATA_LATENCY(__LATENCY__) (((__LATENCY__) > 1U) && ((__LATENCY__) <= 17U))

#define IS_FMC_CLK_DIV(DIV) (((DIV) > 1U) && ((DIV) <= 16U))


#define IS_FMC_LOADTOACTIVE_DELAY(DELAY) (((DELAY) > 0U) && ((DELAY) <= 16U))

#define IS_FMC_EXITSELFREFRESH_DELAY(DELAY) (((DELAY) > 0U) && ((DELAY) <= 16U))
 
#define IS_FMC_SELFREFRESH_TIME(TIME) (((TIME) > 0U) && ((TIME) <= 16U))
 
#define IS_FMC_ROWCYCLE_DELAY(DELAY) (((DELAY) > 0U) && ((DELAY) <= 16U))
  
#define IS_FMC_WRITE_RECOVERY_TIME(TIME) (((TIME) > 0U) && ((TIME) <= 16U))
 
#define IS_FMC_RP_DELAY(DELAY) (((DELAY) > 0U) && ((DELAY) <= 16U))

#define IS_FMC_RCD_DELAY(DELAY) (((DELAY) > 0U) && ((DELAY) <= 16U))


#define IS_FMC_AUTOREFRESH_NUMBER(NUMBER) (((NUMBER) > 0U) && ((NUMBER) <= 16U))

#define IS_FMC_MODE_REGISTER(CONTENT) ((CONTENT) <= 8191U)

#define IS_FMC_REFRESH_RATE(RATE) ((RATE) <= 8191U)

#define IS_FMC_PAGESIZE(SIZE) (((SIZE) == FMC_PAGE_SIZE_NONE) || \
                               ((SIZE) == FMC_PAGE_SIZE_128)  || \
                               ((SIZE) == FMC_PAGE_SIZE_256)  || \
                               ((SIZE) == FMC_PAGE_SIZE_512)  || \
                               ((SIZE) == FMC_PAGE_SIZE_1024))

HAL_StatusTypeDef  FMC_NORSRAM_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_InitTypeDef *Init);
HAL_StatusTypeDef  FMC_NORSRAM_Timing_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank);
HAL_StatusTypeDef  FMC_NORSRAM_Extended_Timing_Init(FMC_NORSRAM_EXTENDED_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank, uint32_t ExtendedMode);
HAL_StatusTypeDef  FMC_NORSRAM_DeInit(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_EXTENDED_TypeDef *ExDevice, uint32_t Bank);
HAL_StatusTypeDef  FMC_NORSRAM_WriteOperation_Enable(FMC_NORSRAM_TypeDef *Device, uint32_t Bank);
HAL_StatusTypeDef  FMC_NORSRAM_WriteOperation_Disable(FMC_NORSRAM_TypeDef *Device, uint32_t Bank);

/**
  * @}
  */

/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* __GM_HAL_FMC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
