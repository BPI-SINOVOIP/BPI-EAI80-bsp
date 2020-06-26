/* Generated automatically by the program 'build/genpreds'
   from the machine description file '../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/xtensa.md'.  */

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
satisfies_constraint_I (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 71 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(xtensa_simm12b (ival)));
}
static inline bool
satisfies_constraint_J (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 76 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(xtensa_simm8 (ival)));
}
static inline bool
satisfies_constraint_K (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 83 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(xtensa_b4const_or_zero (ival)));
}
static inline bool
satisfies_constraint_L (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 89 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(xtensa_b4constu (ival)));
}
static inline bool
satisfies_constraint_M (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 95 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(ival >= -32 && ival <= 95));
}
static inline bool
satisfies_constraint_N (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 101 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(xtensa_simm8x256 (ival)));
}
static inline bool
satisfies_constraint_O (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 106 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(ival == -1 || (ival >= 1 && ival <= 15)));
}
static inline bool
satisfies_constraint_P (rtx op)
{
  HOST_WIDE_INT ival = 0;
  if (CONST_INT_P (op))
    ival = INTVAL (op);
  return (GET_CODE (op) == CONST_INT) && (
#line 112 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(xtensa_mask_immediate (ival)));
}
static inline bool
satisfies_constraint_Y (rtx op)
{
  switch (GET_CODE (op))
    {
    case CONST_INT:
    case CONST_DOUBLE:
    case CONST:
    case SYMBOL_REF:
    case LABEL_REF:
      break;
    default:
      return false;
    }
  return 
#line 117 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(TARGET_AUTO_LITPOOLS);
}
static inline bool
satisfies_constraint_R (rtx op)
{
  return ((GET_CODE (op) == MEM) && (
#line 128 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(smalloffset_mem_p (op)))) || ((GET_CODE (op) == REG) && (
#line 130 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(reload_in_progress
			&& REGNO (op) >= FIRST_PSEUDO_REGISTER)));
}
static inline bool
satisfies_constraint_T (rtx op)
{
  return (GET_CODE (op) == MEM) && (
#line 136 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(!TARGET_CONST16 && constantpool_mem_p (op)));
}
static inline bool
satisfies_constraint_U (rtx op)
{
  return ((GET_CODE (op) == MEM) && (
#line 141 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(! constantpool_mem_p (op)))) || ((GET_CODE (op) == REG) && (
#line 143 "../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/xtensa/constraints.md"
(reload_in_progress
			&& REGNO (op) >= FIRST_PSEUDO_REGISTER)));
}
#endif /* tm-constrs.h */
