/* Generated automatically by the program 'build/genpreds'
   from the machine description file '../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/mips.md'.  */

#ifndef GCC_TM_CONSTRS_H
#define GCC_TM_CONSTRS_H

static inline bool
satisfies_constraint_m (rtx op)
{
  return (GET_CODE (op) == MEM) && (
#line 26 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(memory_address_addr_space_p (GET_MODE (op), XEXP (op, 0),
						 MEM_ADDR_SPACE (op))));
}
static inline bool
satisfies_constraint_o (rtx op)
{
  return (GET_CODE (op) == MEM) && (
#line 32 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(offsettable_nonstrict_memref_p (op)));
}
static inline bool
satisfies_constraint_V (rtx op)
{
  return (GET_CODE (op) == MEM) && ((
#line 41 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(memory_address_addr_space_p (GET_MODE (op), XEXP (op, 0),
						 MEM_ADDR_SPACE (op)))) && (!(
#line 43 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(offsettable_nonstrict_memref_p (op)))));
}
static inline bool
satisfies_constraint__l (rtx op)
{
  return (GET_CODE (op) == MEM) && ((
#line 50 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(GET_CODE (XEXP (op, 0)) == PRE_DEC)) || (
#line 51 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(GET_CODE (XEXP (op, 0)) == POST_DEC)));
}
static inline bool
satisfies_constraint__g (rtx op)
{
  return (GET_CODE (op) == MEM) && ((
#line 57 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(GET_CODE (XEXP (op, 0)) == PRE_INC)) || (
#line 58 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(GET_CODE (XEXP (op, 0)) == POST_INC)));
}
static inline bool
satisfies_constraint_p (rtx ARG_UNUSED (op))
{
  return 
#line 62 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(address_operand (op, VOIDmode));
}
static inline bool
satisfies_constraint_i (rtx op)
{
  return (
#line 66 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(CONSTANT_P (op))) && (
#line 67 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(!flag_pic || LEGITIMATE_PIC_OPERAND_P (op)));
}
static inline bool
satisfies_constraint_s (rtx op)
{
  return (
#line 71 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(CONSTANT_P (op))) && ((
#line 72 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(!CONST_SCALAR_INT_P (op))) && (
#line 73 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(!flag_pic || LEGITIMATE_PIC_OPERAND_P (op))));
}
static inline bool
satisfies_constraint_n (rtx op)
{
  return (
#line 77 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(CONST_SCALAR_INT_P (op))) && (
#line 78 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(!flag_pic || LEGITIMATE_PIC_OPERAND_P (op)));
}
static inline bool
satisfies_constraint_E (rtx op)
{
  return (
#line 82 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(CONST_DOUBLE_AS_FLOAT_P (op))) || (
#line 83 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(GET_CODE (op) == CONST_VECTOR
		    && GET_MODE_CLASS (GET_MODE (op)) == MODE_VECTOR_FLOAT));
}
static inline bool
satisfies_constraint_F (rtx op)
{
  return (
#line 89 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(CONST_DOUBLE_AS_FLOAT_P (op))) || (
#line 90 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(GET_CODE (op) == CONST_VECTOR
		    && GET_MODE_CLASS (GET_MODE (op)) == MODE_VECTOR_FLOAT));
}
static inline bool
satisfies_constraint_X (rtx ARG_UNUSED (op))
{
  return 
#line 95 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/common.md"
(true);
}
static inline bool
satisfies_constraint_kf (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return force_to_mem_operand (op, mode);
}
static inline bool
satisfies_constraint_ks (rtx op)
{
  return (GET_CODE (op) == REG) && (
#line 107 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(REGNO (op) == STACK_POINTER_REGNUM));
}
static inline bool
satisfies_constraint_I (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 114 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(SMALL_OPERAND (ival)));
}
static inline bool
satisfies_constraint_J (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 119 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(ival == 0));
}
static inline bool
satisfies_constraint_K (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 124 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(SMALL_OPERAND_UNSIGNED (ival)));
}
static inline bool
satisfies_constraint_L (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 130 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(LUI_OPERAND (ival)));
}
static inline bool
satisfies_constraint_M (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && ((!(
#line 136 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(SMALL_OPERAND (ival)))) && ((!(
#line 137 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(SMALL_OPERAND_UNSIGNED (ival)))) && (!(
#line 138 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(LUI_OPERAND (ival))))));
}
static inline bool
satisfies_constraint_N (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 143 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(ival >= -0xffff && ival < 0));
}
static inline bool
satisfies_constraint_O (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 148 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(ival >= -0x4000 && ival < 0x4000));
}
static inline bool
satisfies_constraint_P (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 153 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(ival > 0 && ival < 0x10000));
}
static inline bool
satisfies_constraint_G (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (GET_CODE (op) == CONST_DOUBLE) && (
#line 160 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(op == CONST0_RTX (mode)));
}
static inline bool
satisfies_constraint_Q (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return const_arith_operand (op, mode);
}
static inline bool
satisfies_constraint_R (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (GET_CODE (op) == MEM) && (
#line 171 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(mips_address_insns (XEXP (op, 0), mode, false) == 1));
}
static inline bool
satisfies_constraint_S (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (call_insn_operand (op, mode)) && (
#line 177 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(CONSTANT_P (op)));
}
static inline bool
satisfies_constraint_Udb7 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return db7_operand (op, mode);
}
static inline bool
satisfies_constraint_Udb8 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return db8_operand (op, mode);
}
static inline bool
satisfies_constraint_Uead (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return addiur2_operand (op, mode);
}
static inline bool
satisfies_constraint_Uean (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return andi16_operand (op, mode);
}
static inline bool
satisfies_constraint_Uesp (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return addiusp_operand (op, mode);
}
static inline bool
satisfies_constraint_Uib3 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return ib3_operand (op, mode);
}
static inline bool
satisfies_constraint_Usb4 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return sb4_operand (op, mode);
}
static inline bool
satisfies_constraint_Usb5 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return sb5_operand (op, mode);
}
static inline bool
satisfies_constraint_Usb8 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return sb8_operand (op, mode);
}
static inline bool
satisfies_constraint_Usd8 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return sd8_operand (op, mode);
}
static inline bool
satisfies_constraint_Uub8 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return ub8_operand (op, mode);
}
static inline bool
satisfies_constraint_Uuw5 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return uw5_operand (op, mode);
}
static inline bool
satisfies_constraint_Uuw6 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return uw6_operand (op, mode);
}
static inline bool
satisfies_constraint_Uuw8 (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return uw8_operand (op, mode);
}
static inline bool
satisfies_constraint_W (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (GET_CODE (op) == MEM) && ((memory_operand (op, mode)) && ((!(
#line 257 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(TARGET_MIPS16))) || ((!(stack_operand (op, mode))) && (!(
#line 259 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(CONSTANT_P (XEXP (op, 0))))))));
}
static inline bool
satisfies_constraint_YG (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (GET_CODE (op) == CONST_VECTOR) && (
#line 265 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(op == CONST0_RTX (mode)));
}
static inline bool
satisfies_constraint_YA (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 271 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(UIMM6_OPERAND (ival)));
}
static inline bool
satisfies_constraint_YB (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 277 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(IMM10_OPERAND (ival)));
}
static inline bool
satisfies_constraint_Yb (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return qi_mask_operand (op, mode);
}
static inline bool
satisfies_constraint_Yd (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (move_operand (op, mode)) && ((
#line 288 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(CONSTANT_P (op))) && (!(
#line 289 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(mips_dangerous_for_la25_p (op)))));
}
static inline bool
satisfies_constraint_Yf (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (move_operand (op, mode)) && ((
#line 296 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(CONSTANT_P (op))) && (
#line 297 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(mips_dangerous_for_la25_p (op))));
}
static inline bool
satisfies_constraint_Yh (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return hi_mask_operand (op, mode);
}
static inline bool
satisfies_constraint_Yw (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return si_mask_operand (op, mode);
}
static inline bool
satisfies_constraint_Yx (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return low_bitmask_operand (op, mode);
}
static inline bool
satisfies_constraint_ZC (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (GET_CODE (op) == MEM) && ((
#line 317 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(TARGET_MICROMIPS)) ? (
#line 318 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(umips_12bit_offset_address_p (XEXP (op, 0), mode))) : ((
#line 319 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(ISA_HAS_9BIT_DISPLACEMENT)) ? (
#line 320 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(mips_9bit_offset_address_p (XEXP (op, 0), mode))) : (
#line 321 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(mips_address_insns (XEXP (op, 0), mode, false)))));
}
static inline bool
satisfies_constraint_ZD (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (
#line 326 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(TARGET_MICROMIPS)) ? (
#line 327 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(umips_12bit_offset_address_p (op, mode))) : ((
#line 328 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(ISA_HAS_9BIT_DISPLACEMENT)) ? (
#line 329 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(mips_9bit_offset_address_p (op, mode))) : (
#line 330 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/mips/constraints.md"
(mips_address_insns (op, mode, false))));
}
static inline bool
satisfies_constraint_ZR (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return mem_noofs_operand (op, mode);
}
static inline bool
satisfies_constraint_ZS (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (GET_CODE (op) == MEM) && (lwsp_swsp_operand (op, mode));
}
static inline bool
satisfies_constraint_ZT (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (GET_CODE (op) == MEM) && (lw16_sw16_operand (op, mode));
}
static inline bool
satisfies_constraint_ZU (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (GET_CODE (op) == MEM) && (lhu16_sh16_operand (op, mode));
}
static inline bool
satisfies_constraint_ZV (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (GET_CODE (op) == MEM) && (sb16_operand (op, mode));
}
static inline bool
satisfies_constraint_ZW (rtx op)
{
  machine_mode mode = GET_MODE (op);
  return (GET_CODE (op) == MEM) && (lbu16_operand (op, mode));
}
#endif /* tm-constrs.h */
