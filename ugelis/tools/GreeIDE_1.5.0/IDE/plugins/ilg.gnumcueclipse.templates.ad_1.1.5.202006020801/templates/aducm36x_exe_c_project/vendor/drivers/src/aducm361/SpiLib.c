/*******************************************************************************
* Copyright 2015(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/

/**
 *****************************************************************************
   @addtogroup spi
   @{
   @file     SpiLib.c
   @brief    Set of SPI peripheral functions.
   @version  V0.5
   @author   ADI
   @date     October 2015
   @par Revision History:
   - V0.1, May 2012: initial version.
   - V0.2, October 2012: Added SPI DMA support
   - V0.3, November 2012: Moved SPI DMA functionality to DmaLib
   - V0.4, October 2015: Coding style cleanup - no functional changes.
   - V0.5, October 2015: Use Standard Integer Types, prefer unsigned types, add include and C++ guards.

**/

#include "SpiLib.h"
#include "DmaLib.h"

/**
      @brief uint32_t SpiCfg(ADI_SPI_TypeDef *pSPI, uint32_t iFifoSize, uint32_t iMasterEn, uint32_t iConfig);
         ========== Configure the SPI channel.
      @param pSPI :{pADI_SPI0 , pADI_SPI1}
      - pADI_SPI0 for SPI0.
      - pADI_SPI1 for SPI1.
      @param iFifoSize :{SPICON_MOD_TX1RX1,SPICON_MOD_TX2RX2,SPICON_MOD_TX3RX3,SPICON_MOD_TX4RX4}
                - SPICON_MOD_TX1RX1 for single byte Rx and Tx FIFO sizes.
      - SPICON_MOD_TX2RX2 for two bytes Rx and Tx FIFO sizes.
      - SPICON_MOD_TX3RX3 for three bytes Rx and Tx FIFO sizes.
      - SPICON_MOD_TX4RX4 for four bytes Rx and Tx FIFO sizes.
      @param iMasterEn :{SPICON_MASEN_DIS,SPICON_MASEN_EN}
      - SPICON_MASEN_DIS to enable SPI in Slave mode.
      - SPICON_MASEN_EN to enable SPI in Master mode.
      @param iConfig :{SPICON_CON_EN|SPICON_LOOPBACK_EN|SPICON_SOEN_EN|SPICON_RXOF_EN|SPICON_ZEN_EN|SPICON_TIM_TXWR|
                       SPICON_LSB_EN|SPICON_WOM_EN|SPICON_CPOL_HIGH|SPICON_CPHA_SAMPLETRAILING|SPICON_ENABLE_EN}
                - SPICON_CON_EN to enable continuous transfer. SPICON_CON_DIS by default.
                - SPICON_LOOPBACK_EN to enable loopback mode (connect MISO to MOSI). SPICON_LOOPBACK_DIS by default.
                - SPICON_SOEN_EN for MISO to operate as normal.
                  SPICON_SOEN_DIS disable the output driver on the MISO pin, by default.
                - SPICON_RXOF_EN to overwrite the valid data in the Rx register with the new serial byte received.
                  SPICON_RXOF_DIS discard the new serial byte received, by default.
                - SPICON_ZEN_EN to transmit 0x00 when there is no valid data in the Tx FIFO.
                  SPICON_ZEN_DIS transmit the last transmitted value, by default.
                - SPICON_TIM_TXWR for Initiate transfer with a write to the SPI TX register. SPICON_TIM_RXRD by default.
                - SPICON_LSB_EN for LSB transmitted first. SPICON_LSB_DIS by default.
                - SPICON_WOM_EN for Open Circuit Data Output Enable. SPICON_WOM_DIS by default.
                - SPICON_CPOL_HIGH for Serial clock idles high. SPICON_CPOL_LOW by default.
                - SPICON_CPHA_SAMPLETRAILING for Serial clock pulses at the  start of the first data bit transfer.
                  SPICON_CPHA_SAMPLELEADING by default.
                - SPICON_ENABLE_EN to enable of the SPI peripheral. SPICON_ENABLE_DIS by default.
   @return 1.
**/

uint32_t SpiCfg(ADI_SPI_TypeDef *pSPI, uint32_t iFifoSize, uint32_t iMasterEn, uint32_t iConfig)
{
   uint32_t i1;
   i1  = iFifoSize;
   i1 |= iMasterEn;
   i1 |= iConfig;
   pSPI->SPICON = i1;
   return 1;
}

/**
   @brief uint32_t SpiBaud(ADI_SPI_TypeDef *pSPI, uint32_t iClkDiv, uint32_t iCserr)
         ========== Set the SPI clock rate in Master mode.
   @param pSPI :{pADI_SPI0 , pADI_SPI1}
      - pADI_SPI0 for SPI0.
      - pADI_SPI1 for SPI1.
   @param iClkDiv :{0 - 63}
      -  iClkDiv to set the SPI baud rate - baud rate is SPI UCLK/2x(iCLKDiv+1).
   @param iCserr :{SPIDIV_BCRST_DIS,SPIDIV_BCRST_EN}
      - SPIDIV_BCRST_DIS to disable CS error detection.
      - SPIDIV_BCRST_EN to Enable CS error detection.
   @return 1.
**/
uint32_t SpiBaud(ADI_SPI_TypeDef *pSPI, uint32_t iClkDiv, uint32_t iCserr)
{
   uint32_t i1;
   i1 = iCserr;
   i1 |= iClkDiv;
   pSPI->SPIDIV = i1;
   return 1;
}

/**
   @brief uint32_t SpiRx(ADI_SPI_TypeDef *pSPI)
         ========== Write 8 bits of iRx to SPIxRX.
   @param pSPI :{pADI_SPI0 , pADI_SPI1}
      - pADI_SPI0 for SPI0.
      - pADI_SPI1 for SPI1.
   @return SPIRX value.
**/
uint32_t SpiRx(ADI_SPI_TypeDef *pSPI)
{
   return pSPI->SPIRX;
}

/**
   @brief uint32_t SpiTx(ADI_SPI_TypeDef *pSPI, uint32_t iTx);
         ========== Write 8 bits of iTx to SPIxTX.
   @param pSPI :{pADI_SPI0 , pADI_SPI1}
      - pADI_SPI0 for SPI0.
      - pADI_SPI1 for SPI1.
        @param iTx :{0-255}
      - Byte to transmit.
   @return 1.
**/
uint32_t SpiTx(ADI_SPI_TypeDef *pSPI, uint32_t iTx)
{
   pSPI->SPITX = iTx;
   return 1;
}

/**
   @brief uint32_t SpiSta(ADI_SPI_TypeDef *pSPI)
         ========== Read the status register for the SPI.
   @param pSPI :{pADI_SPI0, pADI_SPI1}
      - pADI_SPI0 for SPI0.
      - pADI_SPI1 for SPI1.
        @return value of SPIxSTA:
                - SPISTA_IRQ, Global SPI IRQ.
                - SPISTA_TXFSTA_EMPTY, SPISTA_TXFSTA_ONEBYTE, SPISTA_TXFSTA_TWOBYTES, SPISTA_TXFSTA_THREEBYTES or
                  SPISTA_TXFSTA_FOURBYTES, SPI Tx FIFO status, number of bytes in FIFO.
                - SPISTA_TXUR, SPI Tx FIFO under-run.
                - SPISTA_TX, SPI Tx IRQ.
                - SPISTA_RX, SPI Rx IRQ.
                - SPISTA_RXOF, SPI Rx Overflow IRQ.
                - SPISTA_RXFSTA_EMPTY, SPISTA_RXFSTA_ONEBYTE, SPISTA_RXFSTA_TWOBYTES, SPISTA_RXFSTA_THREEBYTES or
                  SPISTA_RXFSTA_FOURBYTES, SPI Rx FIFO status, number of bytes in FIFO.
                - SPISTA_RXS, SPI Rx FIFO excess bytes in FIFO.
                - SPISTA_CSERR, Chip select SPI error.
**/
uint32_t SpiSta(ADI_SPI_TypeDef *pSPI)
{
   return pSPI->SPISTA;
}


/**
   @brief uint32_t SpiFifoFlush(ADI_SPI_TypeDef *pSPI, uint32_t iTxFlush, uint32_t iRxFlush);
         ========== Function to flush Rx or Tx FIFOs.
   @param pSPI :{pADI_SPI0 , pADI_SPI1}
      - pADI_SPI0 for SPI0.
      - pADI_SPI1 for SPI1.
   @param iTxFlush :{SPICON_TFLUSH_DIS,SPICON_TFLUSH_EN}
      - Set to Flush the Tx FIFO.
      - SPICON_TFLUSH_DIS to do nothing.
      - SPICON_TFLUSH_EN to flush Tx FIFO.
   @param iRxFlush :{SPICON_RFLUSH_DIS,SPICON_RFLUSH_EN}
      - Set to Flush the Tx FIFO.
      - SPICON_RFLUSH_DIS to do nothing.
      - SPICON_RFLUSH_EN to flush Rx FIFO.
   @return 1.
**/
uint32_t SpiFifoFlush(ADI_SPI_TypeDef *pSPI, uint32_t iTxFlush, uint32_t iRxFlush)
{
   if (iTxFlush == SPICON_TFLUSH_EN) {
      pSPI->SPICON  |= 0x2000;
      pSPI->SPICON  &= 0xDFFF;
   }

   if (iRxFlush == SPICON_RFLUSH_EN) {
      pSPI->SPICON  |= 0x1000;
      pSPI->SPICON  &= 0xEFFF;
   }

   return 1;
}

/**
   @brief uint32_t SpiTxFifoFlush(ADI_SPI_TypeDef *pSPI, uint32_t iTxFlush)
         ========== Function to flush Tx FIFO.
   @param pSPI :{pADI_SPI0 , pADI_SPI1}
      - pADI_SPI0 for SPI0.
      - pADI_SPI1 for SPI1.
   @param iTxFlush :{SPICON_TFLUSH_DIS,SPICON_TFLUSH_EN}
      - Set to Flush the Tx FIFO.
      - SPICON_TFLUSH_DIS to do nothing.
      - SPICON_TFLUSH_EN to flush Tx FIFO.
   @warning
                The flush bit stays set or cleared.
   @return 1.
**/
uint32_t SpiTxFifoFlush(ADI_SPI_TypeDef *pSPI, uint32_t iTxFlush)
{
   if (iTxFlush == SPICON_TFLUSH_EN) {
      pSPI->SPICON  |= 0x2000;

   } else {
      pSPI->SPICON  &= 0xDFFF;
   }

   return 1;
}

/**
   @brief uint32_t SpiRxFifoFlush(ADI_SPI_TypeDef *pSPI, uint32_t iRxFlush)
         ========== Function to flush Rx FIFO.
   @param pSPI :{pADI_SPI0 , pADI_SPI1}
      - pADI_SPI0 for SPI0.
      - pADI_SPI1 for SPI1.
   @param iRxFlush :{SPICON_RFLUSH_DIS,SPICON_RFLUSH_EN}
      - Set to Flush the Tx FIFO.
      - SPICON_RFLUSH_DIS to do nothing.
      - SPICON_RFLUSH_EN to flush Rx FIFO
   @warning
                The flush bit stays set or cleared.
   @return 1.
**/
uint32_t SpiRxFifoFlush(ADI_SPI_TypeDef *pSPI, uint32_t iRxFlush)
{
   if (iRxFlush == SPICON_RFLUSH_EN) {
      pSPI->SPICON  |= 0x1000;

   } else {
      pSPI->SPICON  &= 0xEFFF;
   }

   return 1;
}

/**
   @brief uint32_t SpiDma(ADI_SPI_TypeDef *pSPI, uint32_t iDmaRxSel, uint32_t iDmaTxSel, uint32_t iDmaEn);
         ========== Enables/Disables DMA channel.
   @param pSPI :{0,pADI_SPI1}
      - pADI_SPI1 for SPI1.
   @param iDmaRxSel :{SPIDMA_IENRXDMA_DIS,SPIDMA_IENRXDMA_EN}
      - SPIDMA_IENRXDMA_DIS to disable SPI Rx DMA channel.
      - SPIDMA_IENRXDMA_EN to enable SPI Rx DMA channel.
   @param iDmaTxSel :{SPIDMA_IENTXDMA_DIS,SPIDMA_IENTXDMA_EN}
      - SPIDMA_IENTXDMA_DIS to disable SPI Tx DMA channel.
      - SPIDMA_IENTXDMA_EN to enable SPI Tx DMA channel.
   @param iDmaEn :{SPIDMA_ENABLE_DIS,SPIDMA_ENABLE_EN}
      - SPIDMA_ENABLE_DIS to disable SPI DMA mode.
      - SPIDMA_ENABLE_EN to enable SPI DMA mode.
   @return 1.
*/
uint32_t SpiDma(ADI_SPI_TypeDef *pSPI, uint32_t iDmaRxSel, uint32_t iDmaTxSel, uint32_t iDmaEn)
{
   uint32_t i1;
   i1 = iDmaRxSel;
   i1 |= iDmaTxSel;
   i1 |= iDmaEn;
   pSPI->SPIDMA = i1;
   return 1;
}

/**
   @brief uint32_t SpiCountRd(ADI_SPI_TypeDef *pSPI);
         ========== Read the SPIxCNT register for the SPI - number of bytes received.
   @param pSPI :{pADI_SPI0 , pADI_SPI1}
      - pADI_SPI0 for SPI0.
      - pADI_SPI1 for SPI1.
   @return
      SPICNT, Number of bytes received.
**/

uint32_t SpiCountRd(ADI_SPI_TypeDef *pSPI)
{
   return pSPI->SPICNT;
}

/**@}*/
