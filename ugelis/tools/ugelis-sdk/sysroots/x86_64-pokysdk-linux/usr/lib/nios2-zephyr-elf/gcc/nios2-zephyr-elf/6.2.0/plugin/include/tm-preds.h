/* Generated automatically by the program 'build/genpreds'
   from the machine description file '../../../../../../../work-shared/gcc-6.2.0-r0/gcc-6.2.0/gcc/config/nios2/nios2.md'.  */

#ifndef GCC_TM_PREDS_H
#define GCC_TM_PREDS_H

#ifdef HAVE_MACHINE_MODES
extern int general_operand (rtx, machine_mode);
extern int address_operand (rtx, machine_mode);
extern int register_operand (rtx, machine_mode);
extern int pmode_register_operand (rtx, machine_mode);
extern int scratch_operand (rtx, machine_mode);
extern int immediate_operand (rtx, machine_mode);
extern int const_int_operand (rtx, machine_mode);
extern int const_double_operand (rtx, machine_mode);
extern int nonimmediate_operand (rtx, machine_mode);
extern int nonmemory_operand (rtx, machine_mode);
extern int push_operand (rtx, machine_mode);
extern int pop_operand (rtx, machine_mode);
extern int memory_operand (rtx, machine_mode);
extern int indirect_operand (rtx, machine_mode);
extern int ordered_comparison_operator (rtx, machine_mode);
extern int comparison_operator (rtx, machine_mode);
extern int const_0_operand (rtx, machine_mode);
extern int reg_or_0_operand (rtx, machine_mode);
extern int const_uns_arith_operand (rtx, machine_mode);
extern int uns_arith_operand (rtx, machine_mode);
extern int const_arith_operand (rtx, machine_mode);
extern int arith_operand (rtx, machine_mode);
extern int add_regimm_operand (rtx, machine_mode);
extern int const_logical_operand (rtx, machine_mode);
extern int logical_operand (rtx, machine_mode);
extern int const_and_operand (rtx, machine_mode);
extern int and_operand (rtx, machine_mode);
extern int const_shift_operand (rtx, machine_mode);
extern int shift_operand (rtx, machine_mode);
extern int call_operand (rtx, machine_mode);
extern int rdwrctl_operand (rtx, machine_mode);
extern int rdprs_dcache_operand (rtx, machine_mode);
extern int custom_insn_opcode (rtx, machine_mode);
extern int expandable_comparison_operator (rtx, machine_mode);
extern int pop_operation (rtx, machine_mode);
extern int ldwm_operation (rtx, machine_mode);
extern int stwm_operation (rtx, machine_mode);
extern int nios2_hard_register_operand (rtx, machine_mode);
extern int stack_memory_operand (rtx, machine_mode);
extern int ldstio_memory_operand (rtx, machine_mode);
extern int ldstex_memory_operand (rtx, machine_mode);
#endif /* HAVE_MACHINE_MODES */

#define CONSTRAINT_NUM_DEFINED_P 1
enum constraint_num
{
  CONSTRAINT__UNKNOWN = 0,
  CONSTRAINT_r,
  CONSTRAINT_c,
  CONSTRAINT_j,
  CONSTRAINT_I,
  CONSTRAINT_J,
  CONSTRAINT_K,
  CONSTRAINT_L,
  CONSTRAINT_M,
  CONSTRAINT_N,
  CONSTRAINT_O,
  CONSTRAINT_P,
  CONSTRAINT_m,
  CONSTRAINT_o,
  CONSTRAINT_v,
  CONSTRAINT_w,
  CONSTRAINT_p,
  CONSTRAINT_U,
  CONSTRAINT_V,
  CONSTRAINT__l,
  CONSTRAINT__g,
  CONSTRAINT_i,
  CONSTRAINT_s,
  CONSTRAINT_n,
  CONSTRAINT_E,
  CONSTRAINT_F,
  CONSTRAINT_X,
  CONSTRAINT_S,
  CONSTRAINT_T,
  CONSTRAINT__LIMIT
};

extern enum constraint_num lookup_constraint_1 (const char *);
extern const unsigned char lookup_constraint_array[];

/* Return the constraint at the beginning of P, or CONSTRAINT__UNKNOWN if it
   isn't recognized.  */

static inline enum constraint_num
lookup_constraint (const char *p)
{
  unsigned int index = lookup_constraint_array[(unsigned char) *p];
  return (index == UCHAR_MAX
          ? lookup_constraint_1 (p)
          : (enum constraint_num) index);
}

extern bool (*constraint_satisfied_p_array[]) (rtx);

/* Return true if X satisfies constraint C.  */

static inline bool
constraint_satisfied_p (rtx x, enum constraint_num c)
{
  int i = (int) c - (int) CONSTRAINT_I;
  return i >= 0 && constraint_satisfied_p_array[i] (x);
}

static inline bool
insn_extra_register_constraint (enum constraint_num c)
{
  return c >= CONSTRAINT_r && c <= CONSTRAINT_j;
}

static inline bool
insn_extra_memory_constraint (enum constraint_num c)
{
  return c >= CONSTRAINT_m && c <= CONSTRAINT_w;
}

static inline bool
insn_extra_special_memory_constraint (enum constraint_num)
{
  return false;
}

static inline bool
insn_extra_address_constraint (enum constraint_num c)
{
  return c >= CONSTRAINT_p && c <= CONSTRAINT_p;
}

static inline void
insn_extra_constraint_allows_reg_mem (enum constraint_num c,
				      bool *allows_reg, bool *allows_mem)
{
  if (c >= CONSTRAINT_U && c <= CONSTRAINT_U)
    return;
  if (c >= CONSTRAINT_V && c <= CONSTRAINT__g)
    {
      *allows_mem = true;
      return;
    }
  (void) c;
  *allows_reg = true;
  *allows_mem = true;
}

#define CONSTRAINT_LEN(c_,s_) 1

extern enum reg_class reg_class_for_constraint_1 (enum constraint_num);

static inline enum reg_class
reg_class_for_constraint (enum constraint_num c)
{
  if (insn_extra_register_constraint (c))
    return reg_class_for_constraint_1 (c);
  return NO_REGS;
}

extern bool insn_const_int_ok_for_constraint (HOST_WIDE_INT, enum constraint_num);
#define CONST_OK_FOR_CONSTRAINT_P(v_,c_,s_) \
    insn_const_int_ok_for_constraint (v_, lookup_constraint (s_))

enum constraint_type
{
  CT_REGISTER,
  CT_CONST_INT,
  CT_MEMORY,
  CT_SPECIAL_MEMORY,
  CT_ADDRESS,
  CT_FIXED_FORM
};

static inline enum constraint_type
get_constraint_type (enum constraint_num c)
{
  if (c >= CONSTRAINT_p)
    {
      if (c >= CONSTRAINT_U)
        return CT_FIXED_FORM;
      return CT_ADDRESS;
    }
  if (c >= CONSTRAINT_m)
    return CT_MEMORY;
  if (c >= CONSTRAINT_I)
    return CT_CONST_INT;
  return CT_REGISTER;
}
#endif /* tm-preds.h */
