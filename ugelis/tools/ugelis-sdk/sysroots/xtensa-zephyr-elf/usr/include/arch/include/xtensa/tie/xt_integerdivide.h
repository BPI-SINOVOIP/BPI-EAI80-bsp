/* Definitions for the xt_integerdivide TIE package */

/*
 * Customer ID=4313; Build=0x64d47; Copyright (c) 2004-2010 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of Tensilica Inc.
 * They may not be modified, copied, reproduced, distributed, or disclosed to
 * third parties in any manner, medium, or form, in whole or in part, without
 * the prior written consent of Tensilica Inc.
 */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_integerdivide_HEADER
#define _XTENSA_xt_integerdivide_HEADER

#ifdef __XTENSA__
#ifdef __XCC__

#ifndef _ASMLANGAGE
#ifndef _NOCLANGUAGE
#ifndef __ASSEMBLER__

#include <xtensa/tie/xt_core.h>

/*
 * The following prototypes describe intrinsic functions
 * corresponding to TIE instructions.  Some TIE instructions
 * may produce multiple results (designated as "out" operands
 * in the iclass section) or may have operands used as both
 * inputs and outputs (designated as "inout").  However, the C
 * and C++ languages do not provide syntax that can express
 * the in/out/inout constraints of TIE intrinsics.
 * Nevertheless, the compiler understands these constraints
 * and will check that the intrinsic functions are used
 * correctly.  To improve the readability of these prototypes,
 * the "out" and "inout" parameters are marked accordingly
 * with comments.
 */

extern unsigned _TIE_xt_integerdivide_REMS(unsigned ars, unsigned art);
extern unsigned _TIE_xt_integerdivide_REMU(unsigned ars, unsigned art);
extern unsigned _TIE_xt_integerdivide_QUOS(unsigned ars, unsigned art);
extern unsigned _TIE_xt_integerdivide_QUOU(unsigned ars, unsigned art);

#endif /*_ASMLANGUAGE*/
#endif /*_NOCLANGUAGE*/
#endif /*__ASSEMBLER__*/

#define XT_REMS _TIE_xt_integerdivide_REMS
#define XT_REMU _TIE_xt_integerdivide_REMU
#define XT_QUOS _TIE_xt_integerdivide_QUOS
#define XT_QUOU _TIE_xt_integerdivide_QUOU

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_integerdivide_HEADER */
