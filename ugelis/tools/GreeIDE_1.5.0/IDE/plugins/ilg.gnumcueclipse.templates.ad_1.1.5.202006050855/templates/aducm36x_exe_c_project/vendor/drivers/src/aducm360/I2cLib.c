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
   @addtogroup i2c
   @{
   @file     I2cLib.c
   @brief    Set of I2C peripheral functions.

   @version  V0.5
   @author   ADI
   @date     October 2015

   @par Revision History:
   - V0.1, April 2012: initial version.
   - V0.2, May 2013:   corrected comments in I2cBaud().
   - V0.3, July 2013:  corrected I2cMRdCfg().
   - V0.4, October 2015: Coding style cleanup - no functional changes.
   - V0.5, October 2015: Use Standard Integer Types, prefer unsigned types, add include and C++ guards.

**/

#include "I2cLib.h"

/**
   @brief uint32_t I2cMCfg(uint32_t iDMACfg, uint32_t iIntSources, uint32_t iConfig)
         ========== Configure the I2C master channel.
   @param iDMACfg :{0|I2CMCON_TXDMA|I2CMCON_RXDMA} \n
      - 0 by default to disable all DMA operation.
      - I2CMCON_TXDMA to enable I2C Master Tx DMA requests.
      - I2CMCON_RXDMA to enable I2C Master Rx DMA requests.
   @param iIntSources :{0|I2CMCON_IENCMP_EN|I2CMCON_IENNACK_EN|I2CMCON_IENALOST_EN|I2CMCON_IENTX_EN|I2CMCON_IENRX_EN} \n
      - 0 by default to disable all interrupt sources.
      - I2CMCON_IENCMP_EN to enable interrupts when a Stop condition is detected. I2CMCON_IENCMP_DIS by default.
      - I2CMCON_IENNACK_EN to enable interrupts when a NACK is detected. I2CMCON_IENNACK_DIS by default.
      - I2CMCON_IENALOST_EN to enable interrupts when bus arbitration is lost. I2CMCON_IENALOST_DIS by default.
      - I2CMCON_IENTX_EN to enable Transmit interrupts. I2CMCON_IENTX_DIS by default.
      - I2CMCON_IENRX_EN to enable Receive interrupts. I2CMCON_IENRX_DIS by default.
   @param iConfig :{0|I2CMCON_LOOPBACK_EN|I2CMCON_COMPETE_EN|I2CMCON_MAS_EN}  \n
      - 0 by default.
      - I2CMCON_LOOPBACK_EN to enable I2C loopback mode. I2CMCON_LOOPBACK_DIS by default.
      - I2CMCON_COMPETE_EN to enable I2C Master to compete for control of bus. I2CMCON_COMPETE_DIS by default.
      - I2CMCON_MAS_EN to enable I2C Master mode. I2CMCON_MAS_DIS by default.
**/

uint32_t I2cMCfg(uint32_t iDMACfg, uint32_t iIntSources, uint32_t iConfig)
{
   uint32_t i1;

   i1 = iDMACfg;
   i1 |= iIntSources;
   i1 |= iConfig;
   pADI_I2C->I2CMCON = i1;

   return 1;
}
/**
   @brief uint32_t I2cStretch(uint32_t iMode, uint32_t iStretch)
         ========== Configure the I2C Clock stretching.
   @param iMode :{MASTER, SLAVE} \n
      - 0 or MASTER  to control I2C Master clock stretch.
      - 1 or SLAVE  to control I2C Slave clock stretch.
   @param iStretch :{STRETCH_DIS,STRETCH_EN} \n
      - 0 or STRETCH_DIS to disable clock stretching.
      - 1 or STRETCH_EN to enable clock stretching.
**/
uint32_t I2cStretch(uint32_t iMode, uint32_t iStretch)
{
   if (iMode == MASTER) {
      if (iStretch == STRETCH_EN) {  // Enable master clock stretching
         pADI_I2C->I2CMCON |= I2CMCON_STRETCH;

      } else {                          // Disable master clock stretching
         pADI_I2C->I2CMCON &= 0xFFF7;
      }

   } else { // Slave clock stretch control
      if (iStretch == STRETCH_EN) {    // Enable Slave clock stretching
         pADI_I2C->I2CSCON |= I2CSCON_STRETCH;

      } else {                          // Disable Slave clock stretching
         pADI_I2C->I2CSCON &= 0xFFBF;
      }
   }

   return 1;
}

/**
   @brief uint32_t I2cFifoFlush(uint32_t iMode, uint32_t iFlush)
         ========== Flush Master or Slave Tx FIFO
   @param iMode :{MASTER, SLAVE}
      - 0 or MASTER for I2C Master operation.
      - 1 or SLAVE for I2C Slave operation.
   @param iFlush :{DISABLE, ENABLE}
      - 0 or DISABLE to disable flush of FIFO.
      - 1 or ENABLE to enable flush of FIFO.
   @return 1 if successful
**/

uint32_t I2cFifoFlush(uint32_t iMode, uint32_t iFlush)
{
   if (iMode == 1) { // slave
      if (iFlush == 1) {
         pADI_I2C->I2CFSTA |= 0x100;

      } else {
         pADI_I2C->I2CFSTA &= 0xEFF;
      }

   } else {       // master
      if (iFlush == 1) {
         pADI_I2C->I2CFSTA |= 0x200;

      } else {
         pADI_I2C->I2CFSTA &= 0xDFF;
      }
   }

   return 1;
}

/**
   @brief uint32_t I2cSCfg(uint32_t iDMACfg, uint32_t iIntSources, uint32_t iConfig)
         ========== Configure the SPI channel.
   @param iDMACfg :{0|I2CSCON_TXDMA|I2CSCON_RXDMA} \n
      - 0 by default to disable all DMA operation.
      - I2CSCON_TXDMA to enable I2C Master Tx DMA requests.
      - I2CSCON_RXDMA to enable I2C Master Rx DMA requests.
   @param iIntSources :{0|I2CSCON_IENREPST_EN|I2CSCON_IENTX_EN|I2CSCON_IENRX_EN|I2CSCON_IENSTOP_EN} \n
      - 0 by default to disable all interrupt sources.
      - I2CSCON_IENREPST_EN to enable interrupts when a repeated Start condition is detected. I2CSCON_IENREPST_DIS by default.
      - I2CSCON_IENTX_EN to enable Transmit interrupts. I2CSCON_IENTX_DIS by default.
      - I2CSCON_IENRX_EN to enable Receive interrupts. I2CSCON_IENRX_DIS by default.
      - I2CSCON_IENSTOP_EN to enable interrupts when Stop condition is detected. I2CSCON_IENSTOP_DIS by default.
   @param iConfig :{0|I2CSCON_NACK_EN|I2CSCON_EARLYTXR_EN|I2CSCON_GCSB_CLR|I2CSCON_HGC_EN|I2CSCON_GC_EN|I2CSCON_ADR10_EN|I2CSCON_SLV_EN}  \n
      - 0 by default.
      - I2CSCON_NACK_EN to force NACK after next byte. I2CSCON_NACK_DIS by default.
      - I2CSCON_EARLYTXR_EN to enable early transmit request. I2CSCON_EARLYTXR_DIS by default.
      - I2CSCON_GCSB_CLR to clear general call status register.
      - I2CSCON_HGC_EN to enable Hardware general calls. I2CSCON_HGCEN_DIS by default.
      - I2CSCON_GC_EN to enable general calls. I2CSCON_GCEN_DIS by default.
      - I2CSCON_ADR10_EN to enable 10-bit addresses. I2CSCON_ADR10EN_DIS by default.
      - I2CSCON_SLV_EN to enable Slave mode. I2CSCON_SLVEN_DIS by default.
**/

uint32_t I2cSCfg(uint32_t iDMACfg, uint32_t iIntSources, uint32_t iConfig)
{
   uint32_t i1;

   i1 = iDMACfg;
   i1 |= iIntSources;
   i1 |= iConfig;
   pADI_I2C->I2CSCON = i1;

   return 1;
}


/**
   @brief uint32_t I2cRx(uint32_t iMode)
         ========== Reads 8 bits of I2CMRX or I2CSRX
   @param iMode :{MASTER, SLAVE}
      - 0 or MASTER for I2C Master operation.
      - 1 or SLAVE for I2C Slave operation.
   @return I2CSRX or I2CMRX
**/

uint32_t I2cRx(uint32_t iMode)
{
   uint32_t i1;

   i1 = iMode;

   if (i1 == 1) {
      return pADI_I2C->I2CSRX;

   } else {
      return pADI_I2C->I2CMRX;
   }
}



/**
   @brief uint32_t I2cTx(uint32_t iMode, uint32_t iTx)
         ========== Write 8 bits of iTx to I2CMTX ro I2CSTX
   @param iMode :{MASTER, SLAVE}
      - 0 or MASTER for I2C Master operation.
      - 1 or SLAVE for I2C Slave operation.
   @param iTx :{0-255}  \n
      Byte to transmit.
   @return 1 if successful
**/

uint32_t I2cTx(uint32_t iMode, uint32_t iTx)
{
   uint32_t i1;

   i1 = iTx;

   if (iMode == 1) {
      pADI_I2C->I2CSTX = i1;

   } else {
      pADI_I2C->I2CMTX = i1;
   }

   return 1;
}


/**
   @brief uint32_t I2cBaud(uint32_t iHighPeriod, uint32_t iLowPeriod)
   ========== Set the I2C clock rate in Master mode - CLKCON1 setting not accounted for

   @brief iHighPeriod configures the Master Clock high period               \n
   @brief iLowPeriod configures the Master Clock low period            \n
   @param iHighPeriod :{0-255}                                       \n
      - 0x12 for 400kHz operation.
      - 0x4E for 100kHz operation.
   @param iLowPeriod :{0-255}                              \n
      - 0x13 for 400kHz operation.
      - 0x4F for 100kHz operation.
   @return 1
**/

uint32_t I2cBaud(uint32_t iHighPeriod, uint32_t iLowPeriod)
{
   uint32_t i1;

   i1 =  iLowPeriod;
   i1 |= (iHighPeriod << 8);
   pADI_I2C->I2CDIV = i1;
   return 1;
}


/**
   @brief uint32_t I2cMWrCfg(uint32_t uiDevAddr)
         ========== Configure I2CADR0/I2CADR1 - Device address register.

   @param uiDevAddr :{0-1023}     \n
   @ This function write uiDevAddr to I2CADR0 address, lsb =0
   @return 1
**/

uint32_t I2cMWrCfg(uint32_t uiDevAddr)
{
   uiDevAddr &= 0xFFFE;     // Ensure write bit is configured

   if (uiDevAddr > 0xFF) {  // Check for 10-bit address
      pADI_I2C->I2CADR1 = (uiDevAddr & 0xFF);
      pADI_I2C->I2CADR0   = ((uiDevAddr >> 7) & 0x6) | 0xF0;

   } else {
      pADI_I2C->I2CADR0 = uiDevAddr & 0xFF;
      pADI_I2C->I2CADR1 = 0x00;
   }

   return 1;
}


/**
   @brief uint32_t I2cMRdCfg(uint32_t uiDevAddr, uint32_t iNumBytes, uint32_t iExt)
         ========== Configure I2CMRXCNT - Master read control register.
   @param uiDevAddr :{0-1023}     \n
   @param iNumBytes :{0-256}     \n
   @param iExt :{DISABLE,ENABLE}
      - DISABLE for Master to Read 1 to 256 bytes
      - ENABLE for Master to read more than 256 bytes
   @return 1
**/
uint32_t I2cMRdCfg(uint32_t uiDevAddr, uint32_t iNumBytes, uint32_t iExt)
{
   pADI_I2C->I2CMRXCNT = (iNumBytes - 1) | (iExt << 8);

   if (uiDevAddr > 0xFF) {                          // 10-bit address
      pADI_I2C->I2CADR1   = (uiDevAddr & 0xFF);                // 8 LSB
      pADI_I2C->I2CADR0   = ((uiDevAddr >> 7) & 0x6) | 0xF1;   // Format: 1111 0 bit9 bit8 read bit

   } else {                                                   // 7-bit address
      pADI_I2C->I2CADR0 = (uiDevAddr & 0xFF) | 1;              // Ensure read bit is set to 1
      pADI_I2C->I2CADR1 = 0x00;
   }

   return 1;
}

/**
   @brief uint32_t I2cSta(uint32_t iMode)
         ========== Read the status register for the I2C Master or Slave.
@param iMode :{MASTER, SLAVE}
      - 0 or MASTER for I2C Master operation.
      - 1 or SLAVE for I2C Slave operation.
   @return value of I2CSSTA:
      - I2CSSTA.0 = Tx FIFO status
      - I2CSSTA.1 = I2C Tx FIFO under-run
      - I2CSSTA.2 = I2C Tx Request
      - I2CSSTA.3 = I2C Rx Request
      - I2CSSTA.4 = I2C Rx FIFO overflow
      - I2CSSTA.5 = I2C Slave NACK of address byte
      - I2CSSTA.6 = I2C Slave busy
      - I2CSSTA.7 = I2C general call interrupt
      - I2CSSTA.9:8 = I2C general call ID
      - I2CSSTA.10 = I2C Stop condition detected
      - I2CSSTA.12:11 = I2C ID Match status
      - I2CSSTA.13 = I2C repeated Start condition detected
      - I2CSSTA.14 = I2C Start + matching address condition detected
   @return value of I2CMSTA:
      - I2CMSTA.1:0 = Tx FIFO status
      - I2CMSTA.2 = I2C Tx Request
      - I2CMSTA.3 = I2C Rx Request
      - I2CMSTA.4 = I2C NACK of address byte from Slave
      - I2CMSTA.5 = I2C Arbitration lost
      - I2CMSTA.6 = I2C Master busy
      - I2CMSTA.7 = I2C NACK of data byte from Slave
      - I2CMSTA.8 = I2C Stop condition detected
      - I2CMSTA.9 = Rx overflow
      - I2CMSTA.10 = I2C Bus busy
      - I2CMSTA.11 = I2C Stop detected
      - I2CMSTA.12 = I2C Tx Under-run condition
**/

uint32_t I2cSta(uint32_t iMode)
{
   if (iMode == 1) {
      return pADI_I2C->I2CSSTA;

   } else {
      return pADI_I2C->I2CMSTA;
   }
}


/**
   @brief uint32_t I2cMRdCnt(void)
         ========== Master Read count register - read by master to keep track of number of bytes received.

   @return value of I2CMCRXCNT.
**/

uint32_t I2cMRdCnt(void)
{
   return pADI_I2C->I2CMCRXCNT;
}


/**
   @brief uint32_t I2cSGCallCfg(uint32_t iHWGCallAddr)
         ========== Configure ID value for Hardware General Calls.

   @param iHWGCallAddr :{0-255}
   @return 1
**/

uint32_t I2cSGCallCfg(uint32_t iHWGCallAddr)
{
   pADI_I2C->I2CALT = iHWGCallAddr;
   return 1;
}


/**
   @brief uint32_t I2cSIDCfg(uint32_t iSlaveID0, uint32_t iSlaveID1,uint32_t iSlaveID2,uint32_t iSlaveID3)
         ========== Configure ID value for Slave address - value betweeen 0-0xFF.

   @param iSlaveID0 :{0-255}
   @param iSlaveID1 :{0-255}
   @param iSlaveID2 :{0-255}
   @param iSlaveID3 :{0-255}
   @return 1
**/

uint32_t I2cSIDCfg(uint32_t iSlaveID0, uint32_t iSlaveID1, uint32_t iSlaveID2, uint32_t iSlaveID3)
{
   pADI_I2C->I2CID0 = iSlaveID0;
   pADI_I2C->I2CID1 = iSlaveID1;
   pADI_I2C->I2CID2 = iSlaveID2;
   pADI_I2C->I2CID3 = iSlaveID3;
   return 1;
}

/**@}*/


