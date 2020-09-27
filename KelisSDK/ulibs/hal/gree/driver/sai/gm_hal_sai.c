/* Includes ------------------------------------------------------------------*/
#include <gm_soc.h>
#include <sai/gm_ll_sai.h>
#include <gm_hal_sai.h>
#include <adc/gm_ll_adc.h>
#include <gm_irq.h>

#define MAX_SAI_DEVICE                      2


#if 1//def CONFIG_GM_HAL_SAI
#define HAL_MAX_DELAY      0xFFFFFFFFU

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))
//#define assert_param(expr)    ((void)0U)
uint32_t s_mstTransCmplt = 0;
uint32_t s_slvTransCmplt = 0;
uint8_t s_dmaError    = 0;

void HAL_SAIA_IRQHandler(void);
void HAL_SAIB_IRQHandler(void);
void HAL_SAI_IRQHandler(SAI_HandleTypeDef *hsai);

#define DEFAULT_SYSTEM_CLOCK           12000000u           /* Default System clock value */
//static DMA_Config_T s_dmaHandle;
static DMA_Handle_T s_dmaHandle;

uint32_t counter_RX_SAI = 0;

/* Pointers to SAI bases for each instance */
static SAI_Block_TypeDef *const s_SaiBases[MAX_SAI_DEVICE] = {SAI1_Block_A, SAI1_Block_B};

/* Pointers to SAI handles for each instance */
static SAI_HandleTypeDef *s_SaiHandle[MAX_SAI_DEVICE] = {NULL, NULL};

#if (!defined(require_action_quiet))
#define require_action_quiet( X, LABEL, ACTION )    \
    do                                              \
    {                                               \
        if( ( !(X) ) )                              \
        {                                           \
            { ACTION; }                             \
            goto LABEL;                             \
        }                                           \
        \
    }   while( 1==0 )
#endif

//#define SAI_DMA_SUPPORT

#define SAI_FIFO_SIZE         8U
#define SAI_DEFAULT_TIMEOUT   4U /* 4ms */
#define SAI_xCR2_MUTECNT_OFFSET POSITION_VAL(SAI_xCR2_MUTECNT)

static __inline uint32_t SAI_GetIndex(SAI_Block_TypeDef *pSAI)
{
    uint32_t i;

    /* Find the index from base address mappings. */
    for (i = 0; i < MAX_SAI_DEVICE; i++)
    {
        if (s_SaiBases[i] == pSAI)
        {
            break;
        }
    }

    ASSERT(i < MAX_SAI_DEVICE);

    return i;
}

#if 1
void HAL_SAIA_IRQHandler(void)
{
    HAL_SAI_IRQHandler(s_SaiHandle[SAIDEV_0]);
}

void HAL_SAIB_IRQHandler(void)
{
    HAL_SAI_IRQHandler(s_SaiHandle[SAIDEV_1]);
}
#endif

static void SAI_FillFifo(SAI_HandleTypeDef *hsai);
static uint32_t SAI_InterruptFlag(SAI_HandleTypeDef *hsai, uint32_t mode);
static HAL_SAI_StatusTypeDef SAI_InitI2S(SAI_Block_TypeDef *pSAI, SAI_HandleTypeDef *hsai);
static HAL_SAI_StatusTypeDef SAI_InitPCM(SAI_Block_TypeDef *pSAI, SAI_HandleTypeDef *hsai);

static HAL_SAI_StatusTypeDef SAI_Disable(SAI_HandleTypeDef *hsai);
static void SAI_Transmit_IT8Bit(SAI_HandleTypeDef *hsai);
static void SAI_Transmit_IT16Bit(SAI_HandleTypeDef *hsai);
static void SAI_Transmit_IT32Bit(SAI_HandleTypeDef *hsai);
static void SAI_Receive_IT8Bit(SAI_HandleTypeDef *hsai);
static void SAI_Receive_IT16Bit(SAI_HandleTypeDef *hsai);
static void SAI_Receive_IT32Bit(SAI_HandleTypeDef *hsai);

#if 1//def SAI_DMA_SUPPORT

    //static void SAI_DMAError(DMA_Handle_T *hDMA);
    //static void SAI_DMAAbort(DMA_Handle_T *hDMA);

#endif

/**
  * @brief  Initialize the structure FrameInit, SlotInit and the low part of
  *         Init according to the specified parameters and call the function
  *         HAL_SAI_Init to initialize the SAI block.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  protocol: one of the supported protocol @ref SAI_Protocol
  * @param  datasize: one of the supported datasize @ref SAI_Protocol_DataSize
  *                   the configuration information for SAI module.
  * @param  nbslot: Number of slot.
  * @retval HAL status
  */
//HAL_SAI_StatusTypeDef HAL_SAI_InitProtocol(SAI_HandleTypeDef *hsai, uint32_t protocol, uint32_t datasize, uint32_t nbslot)
HAL_SAI_StatusTypeDef HAL_SAI_InitProtocol(SAI_Block_TypeDef *pSAI, SAI_HandleTypeDef *hsai)
{
    HAL_SAI_StatusTypeDef status = HAL_SAI_OK;
    hsai->Instance = pSAI;
    s_SaiHandle[SAI_GetIndex(pSAI)] = hsai;

    /* Check the parameters */
    assert_param(IS_SAI_SUPPORTED_PROTOCOL(hsai->Init.Protocol));
    assert_param(IS_SAI_PROTOCOL_DATASIZE(hsai->Init.DataSize));

    switch (hsai->Init.Protocol)
    {
        case SAI_I2S_STANDARD :
        case SAI_I2S_MSBJUSTIFIED :
        case SAI_I2S_LSBJUSTIFIED :
            status = SAI_InitI2S(pSAI, hsai);
            break;
        case SAI_PCM_LONG :
        case SAI_PCM_SHORT :
            status = SAI_InitPCM(pSAI, hsai);
            break;
        default :
            status = HAL_SAI_ERROR;
            break;
    }

    if (status == HAL_SAI_OK)
    {
        status = HAL_SAI_Init(pSAI, hsai);
    }

    return status;
}

/**
  * @brief  Initialize the SAI according to the specified parameters.
  *         in the SAI_InitTypeDef structure and initialize the associated handle.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval HAL status
  */



//todo, set clock in gm6721
HAL_SAI_StatusTypeDef HAL_SAI_Init(SAI_Block_TypeDef *pSAI, SAI_HandleTypeDef *hsai)
{
    uint32_t mckdiv = 0U;
    uint32_t bckdiv = 0U;

    /* This variable is used to compute CKSTR bits of SAI CR1 according to
       ClockStrobing and AudioMode fields */

    uint32_t ckstr_bits = 0U;
    uint32_t syncen_bits = 0U;
    uint32_t dmaen_bits = 0U;

    /* Check the SAI handle allocation */
    if (hsai == NULL)
    {
        return HAL_SAI_ERROR;
    }

    /* Check the SAI Block parameters */


    if (hsai->State == HAL_SAI_STATE_RESET)
    {
        /* Allocate lock resource and initialize it */
        hsai->Lock = HAL_SAI_UNLOCKED;

        /* Init the low level hardware : GPIO, CLOCK, NVIC and DMA */
        HAL_SAI_MspInit(hsai);
    }

    hsai->State = HAL_SAI_STATE_BUSY;

    /* Disable the selected SAI peripheral */
    SAI_Disable(hsai);

    /* SAI Block Synchro Configuration -----------------------------------------*/


    /*set sai clock, todo, hongze*/
    if ((hsai->Init.AudioFrequency == SAI_AUDIO_FREQUENCY_8K)   || \
            (hsai->Init.AudioFrequency == SAI_AUDIO_FREQUENCY_500)  || \
            (hsai->Init.AudioFrequency == SAI_AUDIO_FREQUENCY_16K)  || \
            (hsai->Init.AudioFrequency == SAI_AUDIO_FREQUENCY_32K)  || \
            (hsai->Init.AudioFrequency == SAI_AUDIO_FREQUENCY_48K)  || \
            (hsai->Init.AudioFrequency == SAI_AUDIO_FREQUENCY_96K)  || \
            (hsai->Init.AudioFrequency == SAI_AUDIO_FREQUENCY_192K))
    {
        hsai->Init.ClockSource = SAI_PLL_49;
        HW32_REG(0x400000ac) |= (1 << 0); //SAIA_CLK_SEL  49.1428M
        HW32_REG(0x400000ac) |= (1 << 8); //SAIB_CLK_SEL  49.1428M
        #ifdef  Recoder_SAI_Mst_RX
        {
            mckdiv = 5;
            bckdiv = hsai->Init.ClockSource / ((mckdiv + 1) * \
                                               (hsai->FrameInit.FrameLength + 1) * hsai->Init.AudioFrequency);
        }
        #else
        {
            mckdiv = hsai->Init.ClockSource / (512 * hsai->Init.AudioFrequency) - 1;
            bckdiv = 512 / (hsai->FrameInit.FrameLength) - 1;
        }
		#endif
    }
    else if ((hsai->Init.AudioFrequency == SAI_AUDIO_FREQUENCY_11K) || \
             (hsai->Init.AudioFrequency == SAI_AUDIO_FREQUENCY_22K)  || \
             (hsai->Init.AudioFrequency == SAI_AUDIO_FREQUENCY_44K))
    {
        hsai->Init.ClockSource = SAI_PLL_11;
        HW32_REG(0x400000ac) &= ~(3 << 0); //SAIA_CLK_SEL  11.2941M
        HW32_REG(0x400000ac) &= ~(3 << 8); //SAIB_CLK_SEL  11.2941M
        #ifdef Recoder_SAI_Mst_RX
        {
            mckdiv = 5;
            bckdiv = hsai->Init.ClockSource / ((mckdiv + 1) * \
                                               (hsai->FrameInit.FrameLength + 1) * hsai->Init.AudioFrequency);
        }
        #else
        {
            mckdiv = hsai->Init.ClockSource / (512 * hsai->Init.AudioFrequency) - 1;
            bckdiv = 512 / (hsai->FrameInit.FrameLength) - 1;
        }
		#endif
    }
    else
    {
        //fix the bug of 6721 double interrupt can work in high fs level
        mckdiv = 3;
        bckdiv = 0x1f;
    }

    hsai->Instance->CLKCR &= ~(SAI_xCKCTL_BCKDIV | SAI_xCKCTL_MCKDIV | SAI_xCKCTL_MCKNODIV);
    LL_SAI_SET_MCKDIV_BIT(hsai->Instance, mckdiv);
    LL_SAI_SET_BCKDIV_BIT(hsai->Instance, bckdiv);
    LL_SAI_SET_MCKNODIV_BIT(hsai->Instance, hsai->Init.MCKNODIV);

    /*if hsai->Init.MCKNODIV is 1, need another way to set clock ???*/
#if 0
    /* Compute CKSTR bits of SAI CR1 according to ClockStrobing and AudioMode */
    if ((hsai->Init.AudioMode == SAI_MODEMASTER_TX) || (hsai->Init.AudioMode == SAI_MODESLAVE_TX))
    {
        ckstr_bits = (hsai->Init.ClockStrobing == SAI_CLOCKSTROBING_RISINGEDGE) ? 0U : SAI_xCR1_CKSTR;
    }
    else
    {
        ckstr_bits = (hsai->Init.ClockStrobing == SAI_CLOCKSTROBING_RISINGEDGE) ? SAI_xCR1_CKSTR : 0U;
    }
#endif
    /* SAI Block Configuration -------------------------------------------------*/
    switch (hsai->Init.Synchro)
    {
        case SAI_ASYNCHRONOUS :
        {
            syncen_bits = 0U;
        }
        break;
        case SAI_SYNCHRONOUS :
        {
            syncen_bits = SAI_xCR1_SYNCEN_0;
        }
        break;
        case SAI_SYNCHRONOUS_EXT_SAI1 :
        case SAI_SYNCHRONOUS_EXT_SAI2 :
        {
            syncen_bits = SAI_xCR1_SYNCEN_1;
        }
        break;
        default:
            break;
    }

    dmaen_bits = SAI_xCR1_DMAEN;
    /* SAI CR1 Configuration */

    /* todo, hongze, add gm6721 clock setting*/

    /*clear bits val*/
    hsai->Instance->CR1 &= ~(SAI_xCR1_MODE | SAI_xCR1_PRTCFG |  SAI_xCR1_DS |      \
                             SAI_xCR1_LSBFIRST | SAI_xCR1_CKSTR | SAI_xCR1_SYNCEN | \
                             SAI_xCR1_MONO | SAI_xCR1_OUTDRIV  | SAI_xCR1_DMAEN |  \
                             SAI_xCR1_FSSOEN | SAI_xCR1_BCKSOEN | SAI_xCR1_MCKSOEN);

    /*set bits val*/
    hsai->Instance->CR1 |= (hsai->Init.AudioMode | hsai->Init.Protocol |
                            hsai->Init.DataSize | hsai->Init.FirstBit  |
                            ckstr_bits | syncen_bits |   dmaen_bits    |
                            hsai->Init.MonoStereoMode | hsai->Init.OutputDrive |
                            hsai->Init.FSOEn | hsai->Init.BCKOEn | hsai->Init.MCKOEn);
    /* SAI CR2 Configuration */
    hsai->Instance->CR2 &= ~(SAI_xCR2_FTH | SAI_xCR2_FFLUSH | SAI_xCR2_COMP | SAI_xCR2_CPL);
    hsai->Instance->CR2 |= (hsai->Init.FIFOThreshold | hsai->Init.CompandingMode | hsai->Init.TriState);

    /* SAI Frame Configuration -----------------------------------------*/
    hsai->Instance->FRCR &= (~(SAI_xFRCR_FRL | SAI_xFRCR_FSALL | SAI_xFRCR_FSDEF | \
                               SAI_xFRCR_FSPOL | SAI_xFRCR_FSOFF));
    hsai->Instance->FRCR |= ((hsai->FrameInit.FrameLength - 1U) | \
                             hsai->FrameInit.FSOffset |
                             hsai->FrameInit.FSPolarity   |
                             ((hsai->FrameInit.ActiveFrameLength - 1U) << 8U));

    /* SAI Block_x SLOT Configuration ------------------------------------------*/
    /* This register has no meaning in AC 97 and SPDIF audio protocol */
    hsai->Instance->SLOTR &= ~(SAI_xSLOTR_FBOFF | SAI_xSLOTR_SLOTSZ | \
                               SAI_xSLOTR_NBSLOT | SAI_xSLOTR_SLOTEN);

    hsai->Instance->SLOTR |=  hsai->SlotInit.FirstBitOffset |  hsai->SlotInit.SlotSize  | \
                              (hsai->SlotInit.SlotActive << 16U) | ((hsai->SlotInit.SlotNumber - 1U) <<  8U);

    /* Initialize the error code */
    hsai->ErrorCode = HAL_SAI_ERROR_NONE;
    #ifdef Recoder_SAI_Mst_RX
    {
        LL_SAI_CPL_2S(pSAI);
        LL_SAI_FSPOL_Falling(pSAI);
        //LL_SAI_FSOFF_CLR(pSAI);
        //LL_SAI_FBOFF_4(pSAI);
    }
	#endif
    /* Initialize the SAI state */
    hsai->State = HAL_SAI_STATE_READY;

    /* Release Lock */
    __HAL_SAI_UNLOCK(hsai);

    return HAL_SAI_OK;
}

/**
  * @brief  DeInitialize the SAI peripheral.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_DeInit(SAI_HandleTypeDef *hsai)
{
    /* Check the SAI handle allocation */
    if (hsai == NULL)
    {
        return HAL_SAI_ERROR;
    }

    hsai->State = HAL_SAI_STATE_BUSY;

    /* Disabled All interrupt and clear all the flag */
    hsai->Instance->IMR = 0U;
    hsai->Instance->CLRFR = 0xFFFFFFFFU;

    /* Disable the SAI */
    SAI_Disable(hsai);

    /* Flush the fifo */
    SET_BIT(hsai->Instance->CR2, SAI_xCR2_FFLUSH);

    /* DeInit the low level hardware: GPIO, CLOCK, NVIC and DMA */
    HAL_SAI_MspDeInit(hsai);

    /* Initialize the error code */
    hsai->ErrorCode = HAL_SAI_ERROR_NONE;

    /* Initialize the SAI state */
    hsai->State = HAL_SAI_STATE_RESET;


    /* Release Lock */
    __HAL_SAI_UNLOCK(hsai);

    return HAL_SAI_OK;
}

/**
  * @brief Initialize the SAI MSP.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hsai);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_SAI_MspInit could be implemented in the user file
     */
}

/**
  * @brief DeInitialize the SAI MSP.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_MspDeInit(SAI_HandleTypeDef *hsai)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hsai);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_SAI_MspDeInit could be implemented in the user file
     */
}

/**
  * @brief  Transmit an amount of data in blocking mode.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_Transmit(SAI_HandleTypeDef *hsai, uint32_t *pData, uint16_t Size, uint32_t Timeout)
{
    uint32_t tickstart = HAL_GetTick();

    if ((pData == NULL) || (Size == 0))
    {
        return  HAL_SAI_ERROR;
    }

    if (hsai->State == HAL_SAI_STATE_READY)
    {
        /* Process Locked */
        __HAL_SAI_LOCK(hsai);

        hsai->XferSize = Size;
        hsai->XferCount = Size;
        hsai->pBuffPtr = pData;
        hsai->State = HAL_SAI_STATE_BUSY_TX;
        hsai->ErrorCode = HAL_SAI_ERROR_NONE;

        /* Check if the SAI is already enabled */
        if ((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
        {
            /* fill the fifo with data before to enabled the SAI */
            SAI_FillFifo(hsai);
            /* Enable SAI peripheral */
            __HAL_SAI_ENABLE(hsai);
        }

        while (hsai->XferCount > 0U)
        {
            /* Write data if the FIFO is not full */
            if ((hsai->Instance->SR & SAI_xSR_FLVL) != SAI_FIFOSTATUS_FULL)
            {
                if ((hsai->Init.DataSize == SAI_DATASIZE_8) && (hsai->Init.CompandingMode == SAI_NOCOMPANDING))
                {
                    hsai->Instance->DR = (*hsai->pBuffPtr++);
                }
                else if (hsai->Init.DataSize <= SAI_DATASIZE_16)
                {
                    hsai->Instance->DR = *((uint16_t *)hsai->pBuffPtr);
                    hsai->pBuffPtr += 4U;
                }
                else
                {
                    hsai->Instance->DR = *((uint32_t *)hsai->pBuffPtr);
                    //printf("\nhsai->Instance->DR = %x",hsai->Instance->DR);
                    hsai->pBuffPtr += 1U;
                }
                hsai->XferCount--;
            }
            else
            {
                /* Check for the Timeout */
                if ((Timeout != HAL_MAX_DELAY) && ((Timeout == 0U) || ((HAL_GetTick() - tickstart) > Timeout)))
                {
                    /* Update error code */
                    hsai->ErrorCode |= HAL_SAI_ERROR_TIMEOUT;

                    /* Clear all the flags */
                    hsai->Instance->CLRFR = 0xFFFFFFFFU;

                    /* Disable SAI peripheral */
                    SAI_Disable(hsai);

                    /* Flush the fifo */
                    SET_BIT(hsai->Instance->CR2, SAI_xCR2_FFLUSH);

                    /* Change the SAI state */
                    hsai->State = HAL_SAI_STATE_READY;

                    /* Process Unlocked */
                    __HAL_SAI_UNLOCK(hsai);

                    return HAL_SAI_ERROR;
                }
            }
        }

        hsai->State = HAL_SAI_STATE_READY;

        /* Process Unlocked */
        __HAL_SAI_UNLOCK(hsai);

        return HAL_SAI_OK;
    }
    else
    {
        return HAL_SAI_BUSY;
    }
}

/**
  * @brief  Receive an amount of data in blocking mode.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_Receive(SAI_HandleTypeDef *hsai, uint32_t *pData, uint16_t Size, uint32_t Timeout)
{
    uint32_t tickstart = HAL_GetTick();

    if ((pData == NULL) || (Size == 0))
    {
        return  HAL_SAI_ERROR;
    }

    if (hsai->State == HAL_SAI_STATE_READY)
    {
        /* Process Locked */
        __HAL_SAI_LOCK(hsai);

        hsai->pBuffPtr = pData;
        hsai->XferSize = Size;
        hsai->XferCount = Size;
        hsai->State = HAL_SAI_STATE_BUSY_RX;
        hsai->ErrorCode = HAL_SAI_ERROR_NONE;

        /* Check if the SAI is already enabled */
        if ((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
        {
            /* Enable SAI peripheral */
            __HAL_SAI_ENABLE(hsai);
        }

        /* Receive data */
        while (hsai->XferCount > 0U)
        {
            if ((hsai->Instance->SR & SAI_xSR_FLVL) != SAI_FIFOSTATUS_EMPTY)
            {
                if ((hsai->Init.DataSize == SAI_DATASIZE_8) && (hsai->Init.CompandingMode == SAI_NOCOMPANDING))
                {
                    (*hsai->pBuffPtr++) = hsai->Instance->DR;
                }
                else if (hsai->Init.DataSize <= SAI_DATASIZE_16)
                {
                    *((uint16_t *)hsai->pBuffPtr) = hsai->Instance->DR;
                    hsai->pBuffPtr += 4U;
                }
                else
                {
                    *((uint32_t *)hsai->pBuffPtr) = hsai->Instance->DR;
                    hsai->pBuffPtr += 1U;
                }
                hsai->XferCount--;
            }
            else
            {
                /* Check for the Timeout */
                if ((Timeout != HAL_MAX_DELAY) && ((Timeout == 0U) || ((HAL_GetTick() - tickstart) > Timeout)))
                {
                    /* Update error code */
                    hsai->ErrorCode |= HAL_SAI_ERROR_TIMEOUT;

                    /* Clear all the flags */
                    hsai->Instance->CLRFR = 0xFFFFFFFFU;

                    /* Disable SAI peripheral */
                    SAI_Disable(hsai);

                    /* Flush the fifo */
                    SET_BIT(hsai->Instance->CR2, SAI_xCR2_FFLUSH);

                    /* Change the SAI state */
                    hsai->State = HAL_SAI_STATE_READY;

                    /* Process Unlocked */
                    __HAL_SAI_UNLOCK(hsai);

                    return HAL_SAI_ERROR;
                }
            }
        }

        hsai->State = HAL_SAI_STATE_READY;

        /* Process Unlocked */
        __HAL_SAI_UNLOCK(hsai);

        return HAL_SAI_OK;
    }
    else
    {
        return HAL_SAI_BUSY;
    }
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with Interrupt.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_Transmit_IT(SAI_HandleTypeDef *hsai, uint32_t *pData, uint16_t Size)
{
    if ((pData == NULL) || (Size == 0))
    {
        return  HAL_SAI_ERROR;
    }

    if (hsai->State == HAL_SAI_STATE_READY)
    {
        /* Process Locked */
        __HAL_SAI_LOCK(hsai);

        hsai->pBuffPtr = pData;
        hsai->XferSize = Size;
        hsai->XferCount = Size;
        hsai->ErrorCode = HAL_SAI_ERROR_NONE;
        hsai->State = HAL_SAI_STATE_BUSY_TX;

        /*request interrupt enable */
        hsai->Instance->IMR |= SAI_xIMR_FREQIE;
        if ((hsai->Init.DataSize == SAI_DATASIZE_8) && (hsai->Init.CompandingMode == SAI_NOCOMPANDING))
        {
            hsai->InterruptServiceRoutine = SAI_Transmit_IT8Bit;
        }
        else if (hsai->Init.DataSize <= SAI_DATASIZE_16)
        {
            hsai->InterruptServiceRoutine = SAI_Transmit_IT16Bit;
        }
        else
        {
            hsai->InterruptServiceRoutine = SAI_Transmit_IT32Bit;
        }

        /* Fill the fifo before starting the communication */
        //SAI_FillFifo(hsai);

        /* Enable FRQ and OVRUDR interrupts */
        __HAL_SAI_ENABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));

        /* Check if the SAI is already enabled */
        if ((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
        {
            /* Enable SAI peripheral */
            __HAL_SAI_ENABLE(hsai);
        }
        if (hsai->Instance == SAI1_Block_A)
        {
            HAL_RequestIrq(SAIA_IRQn, SAIA_IRQ_PRIORITY, HAL_SAIA_IRQHandler, 0);
            HAL_IRQ_ENABLE(SAIA_IRQn);
        }
        if (hsai->Instance == SAI1_Block_B)
        {
            HAL_RequestIrq(SAIB_IRQn, SAIB_IRQ_PRIORITY, HAL_SAIB_IRQHandler, 0);
            HAL_IRQ_ENABLE(SAIB_IRQn);
        }
        /* Process Unlocked */
        __HAL_SAI_UNLOCK(hsai);

        return HAL_SAI_OK;
    }
    else
    {
        return HAL_SAI_BUSY;
    }
}

/**
  * @brief  Receive an amount of data in non-blocking mode with Interrupt.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_Receive_IT(SAI_HandleTypeDef *hsai, uint32_t *pData, uint16_t Size)
{
    if ((pData == NULL) || (Size == 0))
    {
        return  HAL_SAI_ERROR;
    }

    if (hsai->State == HAL_SAI_STATE_READY)
    {
        /* Process Locked */
        __HAL_SAI_LOCK(hsai);

        hsai->pBuffRx = pData;
        hsai->XferSize = Size;
        hsai->XferCount = Size;
        hsai->ErrorCode = HAL_SAI_ERROR_NONE;
        hsai->State = HAL_SAI_STATE_BUSY_RX;
        /*request interrupt enable */
        hsai->Instance->IMR |= SAI_xIMR_FREQIE;

        if ((hsai->Init.DataSize == SAI_DATASIZE_8) && (hsai->Init.CompandingMode == SAI_NOCOMPANDING))
        {
            hsai->InterruptServiceRoutine = SAI_Receive_IT8Bit;
        }
        else if (hsai->Init.DataSize <= SAI_DATASIZE_16)
        {
            hsai->InterruptServiceRoutine = SAI_Receive_IT16Bit;
        }
        else
        {
            hsai->InterruptServiceRoutine = SAI_Receive_IT32Bit;
        }

        /* Enable TXE and OVRUDR interrupts */
        __HAL_SAI_ENABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));

        /* Check if the SAI is already enabled */
        if ((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
        {
            /* Enable SAI peripheral */
            __HAL_SAI_ENABLE(hsai);
        }
        if (hsai->Instance == SAI1_Block_A)
        {
            HAL_RequestIrq(SAIA_IRQn, SAIA_IRQ_PRIORITY, HAL_SAIA_IRQHandler, 1);
            HAL_IRQ_ENABLE(SAIA_IRQn);
        }
        if (hsai->Instance == SAI1_Block_B)
        {
            HAL_RequestIrq(SAIB_IRQn, SAIB_IRQ_PRIORITY, HAL_SAIB_IRQHandler, 1);
            HAL_IRQ_ENABLE(SAIB_IRQn);
        }

        /* Process Unlocked */
        __HAL_SAI_UNLOCK(hsai);

        return HAL_SAI_OK;
    }
    else
    {
        return HAL_SAI_BUSY;
    }
}

#if 0
static HAL_SAI_StatusTypeDef HAL_SAI_DMA_Abort(DMA_HandleTypeDef *hdma);
{
    DMA_ChannelIdx_T chIdx = hdma->chIdx;

    ASSERT(hdma != NULL);

    if (hdma->State == DMA_STATE_BUSY)
    {
        return HAL_SAI_ERROR;
    }

    DMA_CHANNEL_DISABLE(hdma->Instance, chIdx);
    DMA_INTERRUPT_DISABLE(hdma->Instance, chIdx);
    DMA_CLEAR_TRCMPLT(hdma->Instance, chIdx);
    DMA_CLEAR_ERR(hdma->Instance, chIdx);

    memset(s_dmaHandle[chIdx], 0, sizeof(DMA_HandleTypeDef));
    s_dmaHandle[chIdx] = NULL;

    /*Change DMA state back to DMA_STATE_INITIAL*/
    hdma->State = DMA_STATE_INITIAL;

    return HAL_SAI_OK;

}

/**
  * @brief DMA SAI Abort callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SAI_DMAAbort(DMA_Handle_T *hDMA)
{
    SAI_HandleTypeDef *hsai = (SAI_HandleTypeDef *)((DMA_Handle_T *)hDMA)->pParent;

    /* Disable DMA request */
    hsai->Instance->CR1 &= ~SAI_xCR1_DMAEN;

    /* Disable all interrupts and clear all flags */
    hsai->Instance->IMR = 0U;
    hsai->Instance->CLRFR = 0xFFFFFFFFU;

    if (hsai->ErrorCode != HAL_SAI_ERROR_WCKCFG)
    {
        /* Disable SAI peripheral */
        SAI_Disable(hsai);

        /* Flush the fifo */
        SET_BIT(hsai->Instance->CR2, SAI_xCR2_FFLUSH);
    }
    /* Set the SAI state to ready to be able to start again the process */
    hsai->State = HAL_SAI_STATE_READY;

    /* Initialize XferCount */
    hsai->XferCount = 0U;

    /* SAI error Callback */
    HAL_SAI_ErrorCallback(hsai);
}
#endif
#if 1//def SAI_DMA_SUPPORT
/**
  * @brief Pause the audio stream playing from the Media.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval HAL status
  */

HAL_SAI_StatusTypeDef HAL_SAI_DMAPause(SAI_HandleTypeDef *hsai)
{
    /* Process Locked */
    __HAL_SAI_LOCK(hsai);

    /* Pause the audio file playing by disabling the SAI DMA requests */
    hsai->Instance->CR1 &= ~SAI_xCR1_DMAEN;

    /* Process Unlocked */
    __HAL_SAI_UNLOCK(hsai);

    return HAL_SAI_OK;
}

/**
  * @brief Resume the audio stream playing from the Media.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_DMAResume(SAI_HandleTypeDef *hsai)
{
    /* Process Locked */
    __HAL_SAI_LOCK(hsai);

    /* Enable the SAI DMA requests */
    hsai->Instance->CR1 |= SAI_xCR1_DMAEN;

    /* If the SAI peripheral is still not enabled, enable it */
    if ((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == RESET)
    {
        /* Enable SAI peripheral */
        __HAL_SAI_ENABLE(hsai);
    }

    /* Process Unlocked */
    __HAL_SAI_UNLOCK(hsai);

    return HAL_SAI_OK;
}

/**
  * @brief Stop the audio stream playing from the Media.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_DMAStop(SAI_HandleTypeDef *hsai)
{
    /* Process Locked */
    __HAL_SAI_LOCK(hsai);

    /* Disable the SAI DMA request */
    hsai->Instance->CR1 &= ~SAI_xCR1_DMAEN;

    /* Abort the SAI DMA Streams */
    if (hsai->hDMA != NULL)
    {
        if (HAL_DMA_Abort(hsai->hDMA) != HAL_SAI_OK)
        {
            return HAL_SAI_ERROR;
        }
    }

    /* Disable SAI peripheral */
    SAI_Disable(hsai);

    hsai->State = HAL_SAI_STATE_READY;

    /* Process Unlocked */
    __HAL_SAI_UNLOCK(hsai);

    return HAL_SAI_OK;
}
#endif

/**
  * @brief Abort the current transfer and disable the SAI.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_Abort(SAI_HandleTypeDef *hsai)
{
    /* Process Locked */
    __HAL_SAI_LOCK(hsai);

    /* Check SAI DMA is enabled or not */
    if ((hsai->Instance->CR1 & SAI_xCR1_DMAEN) == SAI_xCR1_DMAEN)
    {
        /* Disable the SAI DMA request */
        hsai->Instance->CR1 &= ~SAI_xCR1_DMAEN;

        /* Abort the SAI DMA Streams */
        if (hsai->hDMA != NULL)
        {
            if (HAL_DMA_Abort(hsai->hDMA) != HAL_SAI_OK)
            {
                return HAL_SAI_ERROR;
            }
        }
    }

    /* Disabled All interrupt and clear all the flag */
    hsai->Instance->IMR = 0U;
    hsai->Instance->CLRFR = 0xFFFFFFFFU;

    /* Disable SAI peripheral */
    SAI_Disable(hsai);

    /* Flush the fifo */
    SET_BIT(hsai->Instance->CR2, SAI_xCR2_FFLUSH);

    hsai->State = HAL_SAI_STATE_READY;

    /* Process Unlocked */
    __HAL_SAI_UNLOCK(hsai);

    return HAL_SAI_OK;
}

static void SAI_TxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    SAI_HandleTypeDef *pSAIHandle = GET_PARENTHDL_FROM_DMAHDL(hDMA, struct __SAI_HandleTypeDef);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        /* DMA transaction completes */
        s_mstTransCmplt = 1;
        //HAL_DMA_Abort(hDMA);
        pSAIHandle->State = HAL_SAI_STATE_RESET;
    }

    if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        /* Error occurs during DMA transaction */
        s_dmaError = 1;
        HAL_DMA_Abort(hDMA);
        pSAIHandle->ErrorCode = HAL_SAI_ERROR_DMA;
    }
    //HAL_DMA_Abort(hDMA);
}

static void SAI_RxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    SAI_HandleTypeDef *pSAIHandle = GET_PARENTHDL_FROM_DMAHDL(hDMA, struct __SAI_HandleTypeDef);

    if (event == DMA_EVENT_PINGPONG_PRIM_DONE)
    {
        /* DMA transaction completes */
        if (pSAIHandle->sai_callback)
        {
            /* Notify usr app that I2C data-transcation is completed */
            pSAIHandle->sai_callback(pSAIHandle, SAI_DMAMST_RECEIVE_EVENT, 0);
        }
    }
    if (event == DMA_EVENT_PINGPONG_ALTER_DONE)
    {
        /* DMA transaction completes */
        if (pSAIHandle->sai_callback)
        {
            /* Notify usr app that I2C data-transcation is completed */
            pSAIHandle->sai_callback(pSAIHandle, SAI_DMASLV_RECEIVE_EVENT, 0);
        }
    }

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        /* DMA transaction completes */
        s_slvTransCmplt = 1;
        //HAL_DMA_Abort(hDMA);
        pSAIHandle->State = HAL_SAI_STATE_RESET;
        pSAIHandle->ErrorCode = HAL_SAI_ERROR_NONE;
    }

    if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        /* Error occurs during DMA transaction */
        s_dmaError = 1;
        HAL_DMA_Abort(hDMA);
        pSAIHandle->ErrorCode = HAL_SAI_ERROR_DMA;
    }

}
#if 0
/**
 * @brief DMA callback function for master data-transferring completion.
 *
 * @param hDMA A pointer to DMA device handler.
 * @para event Indicate a DMA determined event.
 * @para param Private information for callback.
 */

static void DMA_MasterTxComplete_Callback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
    SAI_TxDMACallback(hDMA, event, param);
}


/**
 * @brief DMA callback function for master data-receiving completion.
 *
 * @param hDMA A pointer to DMA device handler.
 * @para event Indicate a DMA determined event.
 * @para param Private information for callback.
 */
static void DMA_MasterRxComplete_Callback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
    SAI_RxDMACallback(hDMA, event, param);
}

/**
 * @brief DMA callback function for slave data-transferring completion.
 *
 * @param hDMA A pointer to DMA device handler.
 * @para event Indicate a DMA determined event.
 * @para param Private information for callback.
 */
static void DMA_SlaveTxComplete_Callback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
    SAI_RxDMACallback(hDMA, event, param);
}

/**
 * @brief DMA callback function for slave data-receiving completion.
 *
 * @param hDMA A pointer to DMA device handler.
 * @para event Indicate a DMA determined event.
 * @para param Private information for callback.
 */
static void DMA_SlaveRxComplete_Callback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
    SAI_RxDMACallback(hDMA, event, param);
}
#endif

static RET_CODE SAI_DMAConfig(SAI_Block_TypeDef *pSAI, uint8_t dmaWrite, uint8_t saiMode, DMA_Handle_T *hDMA)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = HAL_SAI_ERROR;

    ASSERT(pSAI != NULL && hDMA != NULL);
    if (dmaWrite)
    {
        if (pSAI == SAI1_Block_A)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_SAI1_A, &reqResult);

        }
        else if (pSAI == SAI1_Block_B)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_SAI1_B, &reqResult);
        }

        if (ret != HAL_SAI_OK)
        {
            return HAL_SAI_ERROR;
        }
        memset(&dmaConfig, 0, sizeof(dmaConfig));

        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
        dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
        dmaConfig.width = DMA_WIDTH_32BIT;
        dmaConfig.burst = DMA_BURST_1;
#if DMA_PINGPANG_MODE
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PINGPONG;
#endif
#if !DMA_PINGPANG_MODE
        dmaConfig.priority = DMA_PRIORITY_LOW;
        dmaConfig.alter = DMA_PRIMARY;
#endif
        dmaConfig.callback = SAI_TxDMACallback;
    }
    else
    {
        if (pSAI == SAI1_Block_A)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_SAI1_A, &reqResult);
        }
        else if (pSAI == SAI1_Block_B)
        {
            ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_SAI1_B, &reqResult);
        }

        if (ret != HAL_SAI_OK)
        {
            return HAL_SAI_ERROR;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));

        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
        dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
#if DMA_PINGPANG_MODE
        dmaConfig.width = DMA_WIDTH_16BIT;
#else
        dmaConfig.width = DMA_WIDTH_32BIT;
#endif
        dmaConfig.burst = DMA_BURST_1;
#if DMA_PINGPANG_MODE
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PINGPONG;
#endif
#if !DMA_PINGPANG_MODE
        dmaConfig.priority = DMA_PRIORITY_LOW;
        dmaConfig.alter = DMA_PRIMARY;
#endif

        dmaConfig.callback = SAI_RxDMACallback;
    }

    HAL_DMA_Init(DMA_DEV, &dmaConfig, hDMA);
    return  HAL_SAI_OK;
}

#if 1//def SAI_DMA_SUPPORT
/**
  * @brief  Transmit an amount of data in non-blocking mode with DMA.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be sent
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_Transmit_DMA(SAI_HandleTypeDef *hsai, SAI_WorkMode_T mode, uint32_t *pData, uint32_t Size)
{
    HAL_SAI_StatusTypeDef error = HAL_SAI_OK;
    uint32_t destAddress = 0x0;
    uint32_t *pBuffer = hsai->pdmaBuffPtr;

    uint32_t  i = 0;
    uint32_t tickstart = HAL_GetTick();
    __HAL_SAI_ENABLE(hsai);
    if ((pData == NULL) || (Size == 0))
    {
        return  HAL_SAI_ERROR;
    }
    if ((uint32_t)(hsai->pBuffPtr) & 0x01 ||
            (hsai->dmaBufferLength > MAX_BLOCK_SIZE_LENGTH))
    {
        return HAL_SAI_INVPARAM;
    }

    /* Copy data in pData to DMA buffer, it's 16Bit aligned by usr code */

    for (i = 0; i < Size; i++)
    {

        *pBuffer = *pData;
        pBuffer ++;
        pData ++;
    }

    require_action_quiet(SAI_DMAConfig(hsai->Instance, TRUE, mode, &s_dmaHandle) == HAL_SAI_OK, \
                         exit, error = HAL_SAI_BUSY; hsai->ErrorCode = HAL_SAI_ERROR_DMA;);
    /* Associate DMA Handle with SAI handle */
    LINK_DMAHDL_TO_PARENTHDL(hsai, hDMA, s_dmaHandle);

    /* Configure DMA destination address */
    destAddress = (uint32_t)(&(hsai->Instance->DR));

    if ((s_dmaHandle.devIdx == DMA_DEV_0 && s_dmaHandle.chIdx == DMA_CHANNEL_9) || \
            (s_dmaHandle.devIdx == DMA_DEV_0 && s_dmaHandle.chIdx == DMA_CHANNEL_11))
    {
        SAIA_DMA_ENABLE(hsai->Instance);
    }


    if ((s_dmaHandle.devIdx == DMA_DEV_1 && s_dmaHandle.chIdx == DMA_CHANNEL_12) || \
            (s_dmaHandle.devIdx == DMA_DEV_0 && s_dmaHandle.chIdx == DMA_CHANNEL_13))
    {
        SAIB_DMA_ENABLE(hsai->Instance);
    }

    /* Start DMA transcation */
    require_action_quiet(HAL_DMA_Start(hsai->hDMA, (uint32_t)hsai->pdmaBuffPtr, (uint32_t)destAddress, Size) == HAL_SAI_OK, \
                         exit, error = HAL_SAI_ERROR; hsai->ErrorCode = HAL_SAI_ERROR_DMA;);

exit:
    return error;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with DMA.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  pData: Pointer to data buffer
  * @param  Size: Amount of data to be received
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_Receive_DMA(SAI_HandleTypeDef *hsai, SAI_WorkMode_T mode, uint16_t *pData, uint32_t Size)
{
    HAL_SAI_StatusTypeDef error = HAL_SAI_OK;
    uint32_t srcAddress = 0x0;

    __HAL_SAI_ENABLE(hsai);

    require_action_quiet(SAI_DMAConfig(hsai->Instance, FALSE, mode, &s_dmaHandle) == HAL_SAI_OK, \
                         exit, error = HAL_SAI_BUSY; hsai->ErrorCode = HAL_SAI_ERROR_DMA;);

    /* Associate DMA Handle with I2C handle */
    LINK_DMAHDL_TO_PARENTHDL(hsai, hDMA, s_dmaHandle);
    /* Configure DMA source address */
    srcAddress = (uint32_t)(&(hsai->Instance->DR));
    if ((s_dmaHandle.devIdx == DMA_DEV_0 && s_dmaHandle.chIdx == DMA_CHANNEL_9) || \
            (s_dmaHandle.devIdx == DMA_DEV_0 && s_dmaHandle.chIdx == DMA_CHANNEL_11))
    {
        SAIA_DMA_ENABLE(hsai->Instance);
    }


    if ((s_dmaHandle.devIdx == DMA_DEV_1 && s_dmaHandle.chIdx == DMA_CHANNEL_12) || \
            (s_dmaHandle.devIdx == DMA_DEV_0 && s_dmaHandle.chIdx == DMA_CHANNEL_13))
    {
        SAIB_DMA_ENABLE(hsai->Instance);
    }

    /* Start DMA transcation */
#if !DMA_PINGPANG_MODE
    require_action_quiet(HAL_DMA_Start(hsai->hDMA, srcAddress, (uint32_t)pData, Size) == HAL_SAI_OK, \
                         exit, error = HAL_SAI_ERROR; hsai->ErrorCode = HAL_SAI_ERROR_DMA;);
#endif
#if DMA_PINGPANG_MODE
    HAL_DMA_MultiBuffer_Start(hsai->hDMA, srcAddress, (uint32_t)pData, (uint32_t)(&pData[Size / 2]), Size / 2);
#endif
exit:
#endif
    return error;

}
#endif
#if 0
static RET_CODE Wait_tillDMACompletes(SAI_HandleTypeDef *hsai, WaitTimer_T *waitTimer)
{
    RET_CODE error = HAL_SAI_OK;

    /* Wait till DMA completes */
    while (1)
    {
        if (s_dmaComplete == 1 || s_dmaError == 1)
        {
            break;
        }

        if (WaitTillTimeout(waitTimer) == HAL_SAI_TIMEOUT)
        {
            hsai->ErrorCode = HAL_SAI_ERROR_TIMEOUT;
            return HAL_SAI_TIMEOUT;
        }
    }

    /* DMA error occures */
    if (s_dmaError == 1)
    {
        require_action_quiet(0, exit, error = HAL_SAI_ERROR; hsai->ErrorCode = HAL_SAI_ERROR_DMA;);
    }

    /* De-intialize DMA */
    require_action_quiet(HAL_DMA_Abort(&s_dmaHandle) == HAL_SAI_OK, exit, error = HAL_SAI_ERROR; hsai->ErrorCode = HAL_SAI_ERROR_DMA;);
    /* Change back to IDLE state */
    hsai->State = HAL_SAI_STATE_RESET;

exit:
    return error;

}
#endif
/**
  * @brief  Enable the Tx mute mode.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  val:  value sent during the mute @ref SAI_Block_Mute_Value
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_EnableTxMuteMode(SAI_HandleTypeDef *hsai, uint16_t val)
{
    assert_param(IS_SAI_BLOCK_MUTE_VALUE(val));

    if (hsai->State != HAL_SAI_STATE_RESET)
    {
        CLEAR_BIT(hsai->Instance->CR2, SAI_xCR2_MUTEVAL | SAI_xCR2_MUTE);
        SET_BIT(hsai->Instance->CR2, SAI_xCR2_MUTE | val);
        return HAL_SAI_OK;
    }
    return HAL_SAI_ERROR;
}

/**
  * @brief  Disable the Tx mute mode.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_DisableTxMuteMode(SAI_HandleTypeDef *hsai)
{
    if (hsai->State != HAL_SAI_STATE_RESET)
    {
        CLEAR_BIT(hsai->Instance->CR2, SAI_xCR2_MUTEVAL | SAI_xCR2_MUTE);
        return HAL_SAI_OK;
    }
    return HAL_SAI_ERROR;
}

/**
  * @brief  Enable the Rx mute detection.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  callback: function called when the mute is detected.
  * @param  counter: number a data before mute detection max 63.
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_EnableRxMuteMode(SAI_HandleTypeDef *hsai, SAIcallback callback, uint16_t counter)
{
    assert_param(IS_SAI_BLOCK_MUTE_COUNTER(counter));

    if (hsai->State != HAL_SAI_STATE_RESET)
    {
        /* set the mute counter */
        CLEAR_BIT(hsai->Instance->CR2, SAI_xCR2_MUTECNT);
        SET_BIT(hsai->Instance->CR2, (uint32_t)((uint32_t)counter << SAI_xCR2_MUTECNT_OFFSET));
        hsai->mutecallback = callback;
        /* enable the IT interrupt */
        __HAL_SAI_ENABLE_IT(hsai, SAI_IT_MUTEDET);
        return HAL_SAI_OK;
    }
    return HAL_SAI_ERROR;
}

/**
  * @brief  Disable the Rx mute detection.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval HAL status
  */
HAL_SAI_StatusTypeDef HAL_SAI_DisableRxMuteMode(SAI_HandleTypeDef *hsai)
{
    if (hsai->State != HAL_SAI_STATE_RESET)
    {
        /* set the mutecallback to NULL */
        hsai->mutecallback = (SAIcallback)NULL;
        /* enable the IT interrupt */
        __HAL_SAI_DISABLE_IT(hsai, SAI_IT_MUTEDET);
        return HAL_SAI_OK;
    }
    return HAL_SAI_ERROR;
}

/**
  * @brief  Handle SAI interrupt request.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
void HAL_SAI_IRQHandler(SAI_HandleTypeDef *hsai)
{
    if (hsai->State != HAL_SAI_STATE_RESET)
    {
        uint32_t itflags = hsai->Instance->SR;
        uint32_t itsources = hsai->Instance->IMR;
        uint32_t cr1config = hsai->Instance->CR1;
        uint32_t tmperror;

        /* SAI Fifo request interrupt occured ------------------------------------*/
        if (((itflags & SAI_xSR_FREQ) == SAI_xSR_FREQ) && ((itsources & SAI_IT_FREQ) == SAI_IT_FREQ))
        {
            hsai->InterruptServiceRoutine(hsai);
        }
        /* SAI Overrun error interrupt occurred ----------------------------------*/
        else if (((itflags & SAI_FLAG_OVRUDR) == SAI_FLAG_OVRUDR) && ((itsources & SAI_IT_OVRUDR) == SAI_IT_OVRUDR))
        {
            /* Clear the SAI Overrun flag */
            __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_OVRUDR);

            /* Get the SAI error code */
            tmperror = ((hsai->State == HAL_SAI_STATE_BUSY_RX) ? HAL_SAI_ERROR_OVR : HAL_SAI_ERROR_UDR);

            /* Change the SAI error code */
            hsai->ErrorCode |= tmperror;

            /* the transfer is not stopped, we will forward the information to the user and we let the user decide what needs to be done */
            HAL_SAI_ErrorCallback(hsai);
        }
        /* SAI mutedet interrupt occurred ----------------------------------*/
        else if (((itflags & SAI_FLAG_MUTEDET) == SAI_FLAG_MUTEDET) && ((itsources & SAI_IT_MUTEDET) == SAI_IT_MUTEDET))
        {
            /* Clear the SAI mutedet flag */
            __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_MUTEDET);

            /* call the call back function */
            if (hsai->mutecallback != (SAIcallback)NULL)
            {
                /* inform the user that an RX mute event has been detected */
                hsai->mutecallback();
            }
        }
        /* SAI AFSDET interrupt occurred ----------------------------------*/
        else if (((itflags & SAI_FLAG_AFSDET) == SAI_FLAG_AFSDET) && ((itsources & SAI_IT_AFSDET) == SAI_IT_AFSDET))
        {
            /* Change the SAI error code */
            hsai->ErrorCode |= HAL_SAI_ERROR_AFSDET;

            /* Check SAI DMA is enabled or not */
            if ((cr1config & SAI_xCR1_DMAEN) == SAI_xCR1_DMAEN)
            {
                /* Abort the SAI DMA Streams */
                if (hsai->hDMA != NULL)
                {
                    /* Set the DMA Tx abort callback */
                    // hsai->hDMA->callback = SAI_DMAAbort;

                    /* Abort DMA in IT mode */
                    //HAL_DMA_Abort_IT(hsai->hdmatx);
                    HAL_DMA_Abort(hsai->hDMA);
                }
            }
            else
            {
                /* Abort SAI */
                HAL_SAI_Abort(hsai);

                /* Set error callback */
                HAL_SAI_ErrorCallback(hsai);
            }
        }
        /* SAI LFSDET interrupt occurred ----------------------------------*/
        else if (((itflags & SAI_FLAG_LFSDET) == SAI_FLAG_LFSDET) && ((itsources & SAI_IT_LFSDET) == SAI_IT_LFSDET))
        {
            /* Change the SAI error code */
            hsai->ErrorCode |= HAL_SAI_ERROR_LFSDET;

            /* Check SAI DMA is enabled or not */
            if ((cr1config & SAI_xCR1_DMAEN) == SAI_xCR1_DMAEN)
            {
                /* Abort the SAI DMA Streams */
                if (hsai->hDMA != NULL)
                {
                    /* Set the DMA Tx abort callback */
                    // hsai->hDMA->callback = SAI_DMAAbort;

                    /* Abort DMA in IT mode */
                    //HAL_DMA_Abort_IT(hsai->hdmatx);
                    HAL_DMA_Abort(hsai->hDMA);
                }
            }
            else
            {
                /* Abort SAI */
                HAL_SAI_Abort(hsai);

                /* Set error callback */
                HAL_SAI_ErrorCallback(hsai);
            }
        }
        /* SAI WCKCFG interrupt occurred ----------------------------------*/
        else if (((itflags & SAI_FLAG_WCKCFG) == SAI_FLAG_WCKCFG) && ((itsources & SAI_IT_WCKCFG) == SAI_IT_WCKCFG))
        {
            /* Change the SAI error code */
            hsai->ErrorCode |= HAL_SAI_ERROR_WCKCFG;

            /* Check SAI DMA is enabled or not */
            if ((cr1config & SAI_xCR1_DMAEN) == SAI_xCR1_DMAEN)
            {
                /* Abort the SAI DMA Streams */
                if (hsai->hDMA != NULL)
                {
                    /* Set the DMA Tx abort callback */
                    // hsai->hDMA->callback = SAI_DMAAbort;

                    /* Abort DMA in IT mode */
                    //HAL_DMA_Abort_IT(hsai->hdmatx);
                    HAL_DMA_Abort(hsai->hDMA);
                }
            }
            else
            {
                /* If WCKCFG occurs, SAI audio block is automatically disabled */
                /* Disable all interrupts and clear all flags */
                hsai->Instance->IMR = 0U;
                hsai->Instance->CLRFR = 0xFFFFFFFFU;

                /* Set the SAI state to ready to be able to start again the process */
                hsai->State = HAL_SAI_STATE_READY;

                /* Initialize XferCount */
                hsai->XferCount = 0U;

                /* SAI error Callback */
                HAL_SAI_ErrorCallback(hsai);
            }
        }
        /* SAI CNRDY interrupt occurred ----------------------------------*/
        else if (((itflags & SAI_FLAG_CNRDY) == SAI_FLAG_CNRDY) && ((itsources & SAI_IT_CNRDY) == SAI_IT_CNRDY))
        {
            /* Clear the SAI CNRDY flag */
            __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_CNRDY);

            /* Change the SAI error code */
            hsai->ErrorCode |= HAL_SAI_ERROR_CNREADY;

            /* the transfer is not stopped, we will forward the information to the user and we let the user decide what needs to be done */
            HAL_SAI_ErrorCallback(hsai);
        }
        else
        {
            /* Nothing to do */
        }
    }
}

/**
  * @brief Tx Transfer completed callback.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *                the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hsai);
    s_mstTransCmplt = 1;

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_SAI_TxCpltCallback could be implemented in the user file
     */
}

/**
  * @brief Tx Transfer Half completed callback.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hsai);
    s_mstTransCmplt = 1;

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_SAI_TxHalfCpltCallback could be implemented in the user file
     */
}

/**
  * @brief Rx Transfer completed callback.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hsai);
    s_slvTransCmplt = 1;
    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_SAI_RxCpltCallback could be implemented in the user file
     */
}

/**
  * @brief Rx Transfer half completed callback.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hsai);
    s_slvTransCmplt = 1;

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_SAI_RxHalfCpltCallback could be implemented in the user file
     */
}

/**
  * @brief SAI error callback.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
__weak void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hsai);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_SAI_ErrorCallback could be implemented in the user file
     */
}


/**
  * @brief  Return the SAI handle state.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval HAL state
  */
HAL_SAI_StateTypeDef HAL_SAI_GetState(SAI_HandleTypeDef *hsai)
{
    return hsai->State;
}

/**
* @brief  Return the SAI error code.
* @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *             the configuration information for the specified SAI Block.
* @retval SAI Error Code
*/
uint32_t HAL_SAI_GetError(SAI_HandleTypeDef *hsai)
{
    return hsai->ErrorCode;
}
/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup SAI_Private_Functions
 *  @brief      Private functions
  * @{
  */

/**
  * @brief  Initialize the SAI I2S protocol according to the specified parameters
  *         in the SAI_InitTypeDef and create the associated handle.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  protocol: one of the supported protocol.
  * @param  datasize: one of the supported datasize @ref SAI_Protocol_DataSize
  *                the configuration information for SAI module.
  * @param  nbslot: number of slot minimum value is 2 and max is 16.
  *                    the value must be a multiple of 2.
  * @retval HAL status
  */
static HAL_SAI_StatusTypeDef SAI_InitI2S(SAI_Block_TypeDef *pSAI, SAI_HandleTypeDef *hsai)
{
    uint32_t nbslot;
    hsai->Init.Protocol = SAI_FREE_PROTOCOL;
    hsai->Init.FirstBit = SAI_FIRSTBIT_MSB;
    /* Compute ClockStrobing according AudioMode */
    if ((hsai->Init.AudioMode == SAI_MODEMASTER_TX) || (hsai->Init.AudioMode == SAI_MODESLAVE_TX))
    {
        /* Transmit */

        hsai->Init.ClockStrobing     = SAI_CLOCKSTROBING_FALLINGEDGE;
    }
    else
    {
        /* Receive */
        hsai->Init.ClockStrobing     = SAI_CLOCKSTROBING_RISINGEDGE;
    }
    hsai->FrameInit.FSDefinition   = SAI_FS_CHANNEL_IDENTIFICATION;
    hsai->SlotInit.SlotActive      = SAI_SLOTACTIVE_ALL;

    hsai->SlotInit.FirstBitOffset  = 0U;
    nbslot = hsai->SlotInit.SlotNumber;

    /* in IS2 the number of slot must be even */
    if ((nbslot & 0x1U) != 0U)
    {
        return HAL_SAI_ERROR;
    }

    switch (hsai->Init.Protocol)
    {
        case SAI_I2S_STANDARD :
            hsai->FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
            hsai->FrameInit.FSOffset   = SAI_FS_BEFOREFIRSTBIT;
            break;
        case SAI_I2S_MSBJUSTIFIED :
        case SAI_I2S_LSBJUSTIFIED :
            hsai->FrameInit.FSPolarity = SAI_FS_ACTIVE_HIGH;
            hsai->FrameInit.FSOffset   = SAI_FS_FIRSTBIT;
            break;
        default :
            return HAL_SAI_ERROR;
    }
#if 1
    if (hsai->Init.DataSize == SAI_xCR1_DS)
    {
        hsai->Init.DataSize =  SAI_PROTOCOL_DATASIZE_32BIT;
    }
    if (hsai->Init.DataSize == SAI_xCR1_DS_2)
    {
        hsai->Init.DataSize =  SAI_PROTOCOL_DATASIZE_16BIT;
    }
    if (hsai->Init.DataSize == SAI_xCR1_DS_3)
    {
        hsai->Init.DataSize =  SAI_PROTOCOL_DATASIZE_16BITEXTENDED;
    }
    if (hsai->Init.DataSize == SAI_xCR1_DS_4)
    {
        hsai->Init.DataSize =  SAI_PROTOCOL_DATASIZE_24BIT;
    }
#endif
    /* Frame definition */
#if 1
    switch (hsai->Init.DataSize)
    {
        case SAI_PROTOCOL_DATASIZE_16BIT:
            hsai->Init.DataSize = SAI_DATASIZE_16;
            hsai->FrameInit.FrameLength = 64U * (nbslot / 2U);
            hsai->FrameInit.ActiveFrameLength = 32U * (nbslot / 2U);
            hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
            break;
        case SAI_PROTOCOL_DATASIZE_16BITEXTENDED :
            hsai->Init.DataSize = SAI_DATASIZE_16;
            hsai->FrameInit.FrameLength = 64U * (nbslot / 2U);
            hsai->FrameInit.ActiveFrameLength = 32U * (nbslot / 2U);
            hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
            break;
        case SAI_PROTOCOL_DATASIZE_24BIT:
            hsai->Init.DataSize = SAI_DATASIZE_24;
            hsai->FrameInit.FrameLength = 64U * (nbslot / 2U);
            hsai->FrameInit.ActiveFrameLength = 32U * (nbslot / 2U);
            hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
            break;
        case SAI_PROTOCOL_DATASIZE_32BIT:
            hsai->Init.DataSize = SAI_DATASIZE_32;
            hsai->FrameInit.FrameLength = 64U * (nbslot / 2U);
            hsai->FrameInit.ActiveFrameLength = 32U * (nbslot / 2U);
            hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
            break;
        default :
            return HAL_SAI_ERROR;
    }
#endif
    if (hsai->Init.Protocol == SAI_I2S_LSBJUSTIFIED)
    {
        if (hsai->Init.DataSize == SAI_PROTOCOL_DATASIZE_16BITEXTENDED)
        {
            hsai->SlotInit.FirstBitOffset = 16U;
        }
        if (hsai->Init.DataSize == SAI_PROTOCOL_DATASIZE_24BIT)
        {
            hsai->SlotInit.FirstBitOffset = 8U;
        }
    }
    return HAL_SAI_OK;
}

/**
  * @brief  Initialize the SAI PCM protocol according to the specified parameters
  *         in the SAI_InitTypeDef and create the associated handle.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  protocol: one of the supported protocol
  * @param  datasize: one of the supported datasize @ref SAI_Protocol_DataSize
  * @param  nbslot: number of slot minimum value is 1 and the max is 16.
  * @retval HAL status
  */
static HAL_SAI_StatusTypeDef SAI_InitPCM(SAI_Block_TypeDef *pSAI, SAI_HandleTypeDef *hsai)
{
    uint32_t nbslot;
    hsai->Init.Protocol            = SAI_FREE_PROTOCOL;
    hsai->Init.FirstBit            = SAI_FIRSTBIT_MSB;
    /* Compute ClockStrobing according AudioMode */
    if ((hsai->Init.AudioMode == SAI_MODEMASTER_TX) || (hsai->Init.AudioMode == SAI_MODESLAVE_TX))
    {
        /* Transmit */
        hsai->Init.ClockStrobing     = SAI_CLOCKSTROBING_RISINGEDGE;
    }
    else
    {
        /* Receive */
        hsai->Init.ClockStrobing     = SAI_CLOCKSTROBING_FALLINGEDGE;
    }
    hsai->FrameInit.FSDefinition   = SAI_FS_STARTFRAME;
    hsai->FrameInit.FSPolarity     = SAI_FS_ACTIVE_HIGH;
    hsai->FrameInit.FSOffset       = SAI_FS_BEFOREFIRSTBIT;
    hsai->SlotInit.FirstBitOffset  = 0U;
    nbslot = hsai->SlotInit.SlotNumber ;
    hsai->SlotInit.SlotActive      = SAI_SLOTACTIVE_ALL;

    switch (hsai->Init.Protocol)
    {
        case SAI_PCM_SHORT :
            hsai->FrameInit.ActiveFrameLength = 1U;
            break;
        case SAI_PCM_LONG :
            hsai->FrameInit.ActiveFrameLength = 13U;
            break;
        default :
            return HAL_SAI_ERROR;
    }

    switch (hsai->Init.DataSize)
    {
        case SAI_PROTOCOL_DATASIZE_16BIT:
            hsai->Init.DataSize = SAI_DATASIZE_16;
            hsai->FrameInit.FrameLength = 16U * nbslot;
            hsai->SlotInit.SlotSize = SAI_SLOTSIZE_16B;
            break;
        case SAI_PROTOCOL_DATASIZE_16BITEXTENDED :
            hsai->Init.DataSize = SAI_DATASIZE_16;
            hsai->FrameInit.FrameLength = 32U * nbslot;
            hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
            break;
        case SAI_PROTOCOL_DATASIZE_24BIT :
            hsai->Init.DataSize = SAI_DATASIZE_24;
            hsai->FrameInit.FrameLength = 32U * nbslot;
            hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
            break;
        case SAI_PROTOCOL_DATASIZE_32BIT:
            hsai->Init.DataSize = SAI_DATASIZE_32;
            hsai->FrameInit.FrameLength = 32U * nbslot;
            hsai->SlotInit.SlotSize = SAI_SLOTSIZE_32B;
            break;
        default :
            return HAL_SAI_ERROR;
    }

    return HAL_SAI_OK;
}

/**
  * @brief  Fill the fifo.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
static void SAI_FillFifo(SAI_HandleTypeDef *hsai)
{
    /* fill the fifo with data before to enabled the SAI */
    while (((hsai->Instance->SR & SAI_xSR_FLVL) != SAI_FIFOSTATUS_FULL) && (hsai->XferCount > 0U))
    {
        if ((hsai->Init.DataSize == SAI_DATASIZE_8) && (hsai->Init.CompandingMode == SAI_NOCOMPANDING))
        {
            hsai->Instance->DR = (*hsai->pBuffPtr++);
        }
        else if (hsai->Init.DataSize <= SAI_DATASIZE_16)
        {
            hsai->Instance->DR = *((uint16_t *)hsai->pBuffPtr);
            hsai->pBuffPtr += 4U;
        }
        else
        {
            hsai->Instance->DR = *((uint32_t *)hsai->pBuffPtr);
            hsai->pBuffPtr += 1U;
            //printf("\nSAIA****************hsai->Instance->DR = %x*************",hsai->Instance->DR);
        }
        hsai->XferCount--;
    }
}

/**
  * @brief  Return the interrupt flag to set according the SAI setup.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @param  mode: SAI_MODE_DMA or SAI_MODE_IT
  * @retval the list of the IT flag to enable
 */
static uint32_t SAI_InterruptFlag(SAI_HandleTypeDef *hsai, uint32_t mode)
{
    uint32_t tmpIT = SAI_IT_OVRUDR;

    if (mode == SAI_MODE_IT)
    {
        tmpIT |= SAI_IT_FREQ;
    }

    if ((hsai->Init.Protocol == SAI_AC97_PROTOCOL) &&
            ((hsai->Init.AudioMode == SAI_MODESLAVE_RX) || (hsai->Init.AudioMode == SAI_MODEMASTER_RX)))
    {
        tmpIT |= SAI_IT_CNRDY;
    }

    if ((hsai->Init.AudioMode == SAI_MODESLAVE_RX) || (hsai->Init.AudioMode == SAI_MODESLAVE_TX))
    {
        tmpIT |= SAI_IT_AFSDET | SAI_IT_LFSDET;
    }
    else
    {
        /* hsai has been configured in master mode */
        tmpIT |= SAI_IT_WCKCFG;
    }
    return tmpIT;
}

/**
  * @brief  Disable the SAI and wait for the disabling.
  * @param  hsai : pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
static HAL_SAI_StatusTypeDef SAI_Disable(SAI_HandleTypeDef *hsai)
{
    uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

    register uint32_t count = SAI_DEFAULT_TIMEOUT * (SystemCoreClock / 7U / 1000U);
    HAL_SAI_StatusTypeDef status = HAL_SAI_OK;

    /* Disable the SAI instance */
    __HAL_SAI_DISABLE(hsai);

    do
    {
        /* Check for the Timeout */
        if (count-- == 0U)
        {
            /* Update error code */
            hsai->ErrorCode |= HAL_SAI_ERROR_TIMEOUT;
            status = HAL_SAI_TIMEOUT;
            break;
        }
    } while ((hsai->Instance->CR1 & SAI_xCR1_SAIEN) != RESET);

    return status;
}

/**
  * @brief  Tx Handler for Transmit in Interrupt mode 8-Bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
static void SAI_Transmit_IT8Bit(SAI_HandleTypeDef *hsai)
{
    if (hsai->XferCount == 0U)
    {
        /* Handle the end of the transmission */
        /* Disable FREQ and OVRUDR interrupts */
        __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));
        hsai->State = HAL_SAI_STATE_READY;
        HAL_SAI_TxCpltCallback(hsai);
    }
    else
    {
        /* Write data on DR register */
        hsai->Instance->DR = (*hsai->pBuffPtr++);
        hsai->XferCount--;
    }
}

/**
  * @brief  Tx Handler for Transmit in Interrupt mode for 16-Bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
static void SAI_Transmit_IT16Bit(SAI_HandleTypeDef *hsai)
{
    if (hsai->XferCount == 0U)
    {
        /* Handle the end of the transmission */
        /* Disable FREQ and OVRUDR interrupts */
        __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));
        hsai->State = HAL_SAI_STATE_READY;
        HAL_SAI_TxCpltCallback(hsai);
    }
    else
    {
        /* Write data on DR register */
        hsai->Instance->DR = *(uint16_t *)hsai->pBuffPtr;
        hsai->pBuffPtr += 2U;
        hsai->XferCount--;
    }
}

/**
  * @brief  Tx Handler for Transmit in Interrupt mode for 32-Bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
static void SAI_Transmit_IT32Bit(SAI_HandleTypeDef *hsai)
{
    if (hsai->XferCount == 0U)
    {
        /* Handle the end of the transmission */
        /* Disable FREQ and OVRUDR interrupts */
        __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));
        hsai->State = HAL_SAI_STATE_READY;
        HAL_SAI_TxCpltCallback(hsai);
    }
    else
    {
        /* Write data on DR register */
        hsai->Instance->DR = *(uint32_t *)hsai->pBuffPtr;
        hsai->pBuffPtr += 1U;
        hsai->XferCount--;
        hsai->Instance->CLRFR |= (1 << 3);
        NVIC_ClearPendingIRQ(SAIA_IRQn);
    }
}

/**
  * @brief  Rx Handler for Receive in Interrupt mode 8-Bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
static void SAI_Receive_IT8Bit(SAI_HandleTypeDef *hsai)
{
    /* Receive data */
    (*hsai->pBuffPtr++) = hsai->Instance->DR;
    hsai->XferCount--;

    /* Check end of the transfer */
    if (hsai->XferCount == 0U)
    {
        /* Disable TXE and OVRUDR interrupts */
        __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));

        /* Clear the SAI Overrun flag */
        __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_OVRUDR);

        hsai->State = HAL_SAI_STATE_READY;
        HAL_SAI_RxCpltCallback(hsai);
    }
}

/**
  * @brief  Rx Handler for Receive in Interrupt mode for 16-Bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
static void SAI_Receive_IT16Bit(SAI_HandleTypeDef *hsai)
{
    /* Receive data */
    *(uint16_t *)hsai->pBuffPtr = hsai->Instance->DR;
    hsai->pBuffPtr += 2U;
    hsai->XferCount--;

    /* Check end of the transfer */
    if (hsai->XferCount == 0U)
    {
        /* Disable TXE and OVRUDR interrupts */
        __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));

        /* Clear the SAI Overrun flag */
        __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_OVRUDR);

        hsai->State = HAL_SAI_STATE_READY;
        HAL_SAI_RxCpltCallback(hsai);
    }
}

/**
  * @brief  Rx Handler for Receive in Interrupt mode for 32-Bit transfer.
  * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
  *               the configuration information for SAI module.
  * @retval None
  */
static void SAI_Receive_IT32Bit(SAI_HandleTypeDef *hsai)
{
    /* Receive data */
    //printf("\nSAIB_RX  hsai->Instance->DR = %x",hsai->Instance->DR);
    *(uint32_t *)hsai->pBuffRx = hsai->Instance->DR;
    hsai->pBuffRx += 1U;
    hsai->XferCount--;
    hsai->Instance->CLRFR |= (1 << 3);
    NVIC_ClearPendingIRQ(SAIB_IRQn);
    /* Check end of the transfer */
    if (hsai->XferCount == 0U)
    {
        /* Disable TXE and OVRUDR interrupts */
        __HAL_SAI_DISABLE_IT(hsai, SAI_InterruptFlag(hsai, SAI_MODE_IT));

        /* Clear the SAI Overrun flag */
        __HAL_SAI_CLEAR_FLAG(hsai, SAI_FLAG_OVRUDR);

        hsai->State = HAL_SAI_STATE_READY;
        HAL_SAI_RxCpltCallback(hsai);
    }
}
/* HAL_SAI_MODULE_ENABLED */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
