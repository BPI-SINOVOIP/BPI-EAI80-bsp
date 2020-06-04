/* Includes ------------------------------------------------------------------*/
#include<gm_soc.h>
#include<gm_hal_fmc.h>
#include"gm_ll_fmc.h"

/**
  * @brief  Initialize the FMC_NORSRAM device according to the specified
  *         control parameters in the FMC_NORSRAM_InitTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Init: Pointer to NORSRAM Initialization structure   
  * @retval HAL status
  */
HAL_StatusTypeDef  FMC_NORSRAM_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_InitTypeDef* Init)
{ 
  uint32_t tmpr = 0U;
    
  /* Check the parameters */
  /* Get the BTCR register value */
  tmpr = Device->BTCR[Init->NSBank];

  /* Clear MBKEN, MUXEN, MTYP, MWID, FACCEN, BURSTEN, WAITPOL, WRAPMOD, WAITCFG, WREN,
           WAITEN, EXTMOD, ASYNCWAIT, CPSIZE, CBURSTRW and CCLKEN bits */
  tmpr &= ((uint32_t)~(FMC_BCR1_MBKEN     | FMC_BCR1_MUXEN    | FMC_BCR1_MTYP     | \
                       FMC_BCR1_MWID      | FMC_BCR1_FACCEN   | FMC_BCR1_BURSTEN  | \
                       FMC_BCR1_WAITPOL   | FMC_BCR1_WRAPMOD  | FMC_BCR1_WAITCFG  | \
                       FMC_BCR1_WREN      | FMC_BCR1_WAITEN   | FMC_BCR1_EXTMOD   | \
                       FMC_BCR1_ASYNCWAIT | FMC_BCR1_CPSIZE   | FMC_BCR1_CBURSTRW | \
                       FMC_BCR1_CCLKEN));
  
  /* Set NORSRAM device control parameters */
  tmpr |= (uint32_t)(Init->DataAddressMux       |\
                    Init->MemoryType           |\
                    Init->MemoryDataWidth      |\
                    Init->BurstAccessMode      |\
                    Init->WaitSignalPolarity   |\
                    Init->WrapMode             |\
                    Init->WaitSignalActive     |\
                    Init->WriteOperation       |\
                    Init->WaitSignal           |\
                    Init->ExtendedMode         |\
                    Init->AsynchronousWait     |\
                    Init->PageSize             |\
                    Init->WriteBurst           |\
                    Init->ContinuousClock);
                    
  if(Init->MemoryType == FMC_MEMORY_TYPE_NOR)
  {
    tmpr |= (uint32_t)FMC_NORSRAM_FLASH_ACCESS_ENABLE;
  }
  
  Device->BTCR[Init->NSBank] = tmpr;

  /* Configure synchronous mode when Continuous clock is enabled for bank2..4 */
  if((Init->ContinuousClock == FMC_CONTINUOUS_CLOCK_SYNC_ASYNC) && (Init->NSBank != FMC_NORSRAM_BANK1))
  { 
    Device->BTCR[FMC_NORSRAM_BANK1] |= (uint32_t)(Init->ContinuousClock);
  }

 
  return HAL_OK;
}

/**
  * @brief  DeInitialize the FMC_NORSRAM peripheral 
  * @param  Device: Pointer to NORSRAM device instance
  * @param  ExDevice: Pointer to NORSRAM extended mode device instance  
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_DeInit(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_EXTENDED_TypeDef *ExDevice, uint32_t Bank)
{
  /* Check the parameters */
  /* Disable the FMC_NORSRAM device */
  __FMC_NORSRAM_DISABLE(Device, Bank);
  
  /* De-initialize the FMC_NORSRAM device */
  /* FMC_NORSRAM_BANK1 */
  if(Bank == FMC_NORSRAM_BANK1)
  {
    Device->BTCR[Bank] = 0x000030DBU;
  }
  /* FMC_NORSRAM_BANK2, FMC_NORSRAM_BANK3 or FMC_NORSRAM_BANK4 */
  else
  {   
    Device->BTCR[Bank] = 0x000030D2U;
  }
  
  Device->BTCR[Bank + 1U] = 0x0FFFFFFFU;
  ExDevice->BWTR[Bank]    = 0x0FFFFFFFU;
   
  return HAL_OK;
}

/**
  * @brief  Initialize the FMC_NORSRAM Timing according to the specified
  *         parameters in the FMC_NORSRAM_TimingTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Timing: Pointer to NORSRAM Timing structure
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_Timing_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank)
{
  uint32_t tmpr = 0U;
  
  /* Check the parameters */  
  /* Get the BTCR register value */
  tmpr = Device->BTCR[Bank + 1U];

  /* Clear ADDSET, ADDHLD, DATAST, BUSTURN, CLKDIV, DATLAT and ACCMOD bits */
  tmpr &= ((uint32_t)~(FMC_BTR1_ADDSET  | FMC_BTR1_ADDHLD | FMC_BTR1_DATAST | \
                       FMC_BTR1_BUSTURN | FMC_BTR1_CLKDIV | FMC_BTR1_DATLAT | \
                       FMC_BTR1_ACCMOD));
  
  /* Set FMC_NORSRAM device timing parameters */  
  tmpr |= (uint32_t)(Timing->AddressSetupTime                   |\
                    ((Timing->AddressHoldTime) << 4U)           |\
                    ((Timing->DataSetupTime) << 8U)             |\
                    ((Timing->BusTurnAroundDuration) << 16U)    |\
                    (((Timing->CLKDivision) - 1U) << 20U)         |\
                    (((Timing->DataLatency) - 2U) << 24U)         |\
                    (Timing->AccessMode));
  
  Device->BTCR[Bank + 1U] = tmpr;
  
  /* Configure Clock division value (in NORSRAM bank 1) when continuous clock is enabled */
  /*
  if(HAL_IS_BIT_SET(Device->BTCR[FMC_NORSRAM_BANK1], FMC_BCR1_CCLKEN))
  {
    tmpr = (uint32_t)(Device->BTCR[FMC_NORSRAM_BANK1 + 1U] & ~(0x0FU << 20U)); 
    tmpr |= (uint32_t)(((Timing->CLKDivision) - 1U) << 20U);
    Device->BTCR[FMC_NORSRAM_BANK1 + 1U] = tmpr;
  }  
  */
  return HAL_OK;   
}

/**
  * @brief  Initialize the FMC_NORSRAM Extended mode Timing according to the specified
  *         parameters in the FMC_NORSRAM_TimingTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Timing: Pointer to NORSRAM Timing structure
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
HAL_StatusTypeDef  FMC_NORSRAM_Extended_Timing_Init(FMC_NORSRAM_EXTENDED_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, uint32_t Bank, uint32_t ExtendedMode)
{  
  uint32_t tmpr = 0U;
 
  /* Check the parameters */  
  /* Set NORSRAM device timing register for write configuration, if extended mode is used */
  if(ExtendedMode == FMC_EXTENDED_MODE_ENABLE)
  {
    /* Check the parameters */    
    /* Get the BWTR register value */
    tmpr = Device->BWTR[Bank];

    /* Clear ADDSET, ADDHLD, DATAST, BUSTURN and ACCMOD bits */
    tmpr &= ((uint32_t)~(FMC_BWTR1_ADDSET  | FMC_BWTR1_ADDHLD | FMC_BWTR1_DATAST | \
                         FMC_BWTR1_BUSTURN | FMC_BWTR1_ACCMOD));
    
    tmpr |= (uint32_t)(Timing->AddressSetupTime                  |\
                      ((Timing->AddressHoldTime) << 4U)          |\
                      ((Timing->DataSetupTime) << 8U)            |\
                      ((Timing->BusTurnAroundDuration) << 16U)   |\
                      (Timing->AccessMode));

    Device->BWTR[Bank] = tmpr;
  }
  else
  {
    Device->BWTR[Bank] = 0x0FFFFFFFU;
  }   
  
  return HAL_OK;  
}
  
/**
  * @brief  Enables dynamically FMC_NORSRAM write operation.
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Bank: NORSRAM bank number   
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_WriteOperation_Enable(FMC_NORSRAM_TypeDef *Device, uint32_t Bank)
{
  /* Check the parameters */  
  /* Enable write operation */
  Device->BTCR[Bank] |= FMC_WRITE_OPERATION_ENABLE; 

  return HAL_OK;  
}

/**
  * @brief  Disables dynamically FMC_NORSRAM write operation.
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Bank: NORSRAM bank number   
  * @retval HAL status
  */
HAL_StatusTypeDef FMC_NORSRAM_WriteOperation_Disable(FMC_NORSRAM_TypeDef *Device, uint32_t Bank)
{ 
  /* Check the parameters */
  /* Disable write operation */
  Device->BTCR[Bank] &= ~FMC_WRITE_OPERATION_ENABLE; 

  return HAL_OK;  
}
