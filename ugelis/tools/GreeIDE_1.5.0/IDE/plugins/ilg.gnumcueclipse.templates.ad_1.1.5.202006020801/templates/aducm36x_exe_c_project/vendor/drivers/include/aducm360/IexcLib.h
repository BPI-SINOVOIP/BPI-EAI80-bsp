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
   @file     IexcLib.h
   @brief    Set of Excitation Current Source functions.
   - Configure Excitation Currents with IexcCfg().
   - Select output current with IexcDat()

   @version  V0.4
   @author   ADI
   @date     October 2015
   @par Revision History:
   - V0.1, March 2011: initial version.
   - V0.2, January 2013: Fixed IexcDat() - current outputs are all correct.
   - V0.3, October 2015: Coding style cleanup - no functional changes.
   - V0.4, October 2015: Use Standard Integer Types, prefer unsigned types, add include and C++ guards.

**/
#ifndef __ADUCM36X_IEXCLIB_H
#define __ADUCM36X_IEXCLIB_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <ADuCM360.h>

uint32_t IexcCfg(uint32_t iPd, uint32_t iRefsel, uint32_t iPinsel1, uint32_t iPinsel0);
uint32_t IexcDat(uint32_t iIDAT, uint32_t iIDAT0);

#define IDAT0En   1
#define IDAT0Dis  0

#ifdef __cplusplus
}
#endif

#endif /* __ADUCM36X_IEXCLIB_H */

