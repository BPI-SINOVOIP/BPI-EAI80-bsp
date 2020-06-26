/* Generated automatically by the program 'build/genpreds'
   from the machine description file '../../../../../../../work-shared/gcc-6.x.arc-r0/git/gcc/config/arc/arc.md'.  */

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
extern int dest_reg_operand (rtx, machine_mode);
extern int mpy_dest_reg_operand (rtx, machine_mode);
extern int symbolic_operand (rtx, machine_mode);
extern int call_address_operand (rtx, machine_mode);
extern int call_operand (rtx, machine_mode);
extern int u6_immediate_operand (rtx, machine_mode);
extern int short_immediate_operand (rtx, machine_mode);
extern int p2_immediate_operand (rtx, machine_mode);
extern int long_immediate_operand (rtx, machine_mode);
extern int long_immediate_loadstore_operand (rtx, machine_mode);
extern int compact_register_operand (rtx, machine_mode);
extern int compact_store_memory_operand (rtx, machine_mode);
extern int move_src_operand (rtx, machine_mode);
extern int move_double_src_operand (rtx, machine_mode);
extern int move_dest_operand (rtx, machine_mode);
extern int nonvol_nonimm_operand (rtx, machine_mode);
extern int proper_comparison_operator (rtx, machine_mode);
extern int equality_comparison_operator (rtx, machine_mode);
extern int brcc_nolimm_operator (rtx, machine_mode);
extern int cc_register (rtx, machine_mode);
extern int cc_set_register (rtx, machine_mode);
extern int cc_use_register (rtx, machine_mode);
extern int zn_compare_operator (rtx, machine_mode);
extern int shift_operator (rtx, machine_mode);
extern int shiftl4_operator (rtx, machine_mode);
extern int shiftr4_operator (rtx, machine_mode);
extern int shift4_operator (rtx, machine_mode);
extern int mult_operator (rtx, machine_mode);
extern int commutative_operator (rtx, machine_mode);
extern int commutative_operator_sans_mult (rtx, machine_mode);
extern int noncommutative_operator (rtx, machine_mode);
extern int unary_operator (rtx, machine_mode);
extern int _2_4_8_operand (rtx, machine_mode);
extern int arc_double_register_operand (rtx, machine_mode);
extern int shouldbe_register_operand (rtx, machine_mode);
extern int vector_register_operand (rtx, machine_mode);
extern int vector_register_or_memory_operand (rtx, machine_mode);
extern int arc_dpfp_operator (rtx, machine_mode);
extern int arc_simd_dma_register_operand (rtx, machine_mode);
extern int acc1_operand (rtx, machine_mode);
extern int acc2_operand (rtx, machine_mode);
extern int mlo_operand (rtx, machine_mode);
extern int mhi_operand (rtx, machine_mode);
extern int extend_operand (rtx, machine_mode);
extern int millicode_store_operation (rtx, machine_mode);
extern int millicode_load_operation (rtx, machine_mode);
extern int millicode_load_clob_operation (rtx, machine_mode);
extern int immediate_usidi_operand (rtx, machine_mode);
extern int short_const_int_operand (rtx, machine_mode);
extern int mem_noofs_operand (rtx, machine_mode);
extern int any_mem_operand (rtx, machine_mode);
extern int even_register_operand (rtx, machine_mode);
extern int double_register_operand (rtx, machine_mode);
extern int cmem_address_0 (rtx, machine_mode);
extern int cmem_address_1 (rtx, machine_mode);
extern int cmem_address_2 (rtx, machine_mode);
extern int cmem_address (rtx, machine_mode);
extern int short_unsigned_const_operand (rtx, machine_mode);
extern int arc_short_operand (rtx, machine_mode);
#endif /* HAVE_MACHINE_MODES */

#define CONSTRAINT_NUM_DEFINED_P 1
enum constraint_num
{
  CONSTRAINT__UNKNOWN = 0,
  CONSTRAINT_r,
  CONSTRAINT_c,
  CONSTRAINT_Rac,
  CONSTRAINT_w,
  CONSTRAINT_W,
  CONSTRAINT_l,
  CONSTRAINT_x,
  CONSTRAINT_Rgp,
  CONSTRAINT_f,
  CONSTRAINT_b,
  CONSTRAINT_k,
  CONSTRAINT_q,
  CONSTRAINT_Rrq,
  CONSTRAINT_e,
  CONSTRAINT_D,
  CONSTRAINT_d,
  CONSTRAINT_v,
  CONSTRAINT_Rsc,
  CONSTRAINT_h,
  CONSTRAINT_Rcd,
  CONSTRAINT_Rsd,
  CONSTRAINT_Rzd,
  CONSTRAINT_I,
  CONSTRAINT_K,
  CONSTRAINT_L,
  CONSTRAINT_M,
  CONSTRAINT_N,
  CONSTRAINT_O,
  CONSTRAINT_P,
  CONSTRAINT_J12,
  CONSTRAINT_J16,
  CONSTRAINT_m,
  CONSTRAINT_o,
  CONSTRAINT_T,
  CONSTRAINT_Uts,
  CONSTRAINT_S,
  CONSTRAINT_Uex,
  CONSTRAINT_Usc,
  CONSTRAINT_ATO,
  CONSTRAINT_Ucd,
  CONSTRAINT_p,
  CONSTRAINT_CnL,
  CONSTRAINT_CmL,
  CONSTRAINT_C16,
  CONSTRAINT_C__0,
  CONSTRAINT_Cn0,
  CONSTRAINT_Cca,
  CONSTRAINT_CL2,
  CONSTRAINT_CM4,
  CONSTRAINT_Csp,
  CONSTRAINT_C2a,
  CONSTRAINT_C0p,
  CONSTRAINT_C1p,
  CONSTRAINT_C2p,
  CONSTRAINT_C3p,
  CONSTRAINT_Ccp,
  CONSTRAINT_Cux,
  CONSTRAINT_Chs,
  CONSTRAINT_Clo,
  CONSTRAINT_Chi,
  CONSTRAINT_Cbf,
  CONSTRAINT_Cbn,
  CONSTRAINT_C18,
  CONSTRAINT_Crr,
  CONSTRAINT_Cbi,
  CONSTRAINT_G,
  CONSTRAINT_H,
  CONSTRAINT_Cbr,
  CONSTRAINT_Cbp,
  CONSTRAINT_Clb,
  CONSTRAINT_Q,
  CONSTRAINT_Cm1,
  CONSTRAINT_Cm2,
  CONSTRAINT_Cm3,
  CONSTRAINT_C62,
  CONSTRAINT_Rcq,
  CONSTRAINT_Rcw,
  CONSTRAINT_Rcr,
  CONSTRAINT_Rcb,
  CONSTRAINT_Rck,
  CONSTRAINT_Rs5,
  CONSTRAINT_Rcc,
  CONSTRAINT_V,
  CONSTRAINT__l,
  CONSTRAINT__g,
  CONSTRAINT_Usd,
  CONSTRAINT_Us_l,
  CONSTRAINT_Us_g,
  CONSTRAINT_Ucm,
  CONSTRAINT_i,
  CONSTRAINT_s,
  CONSTRAINT_n,
  CONSTRAINT_E,
  CONSTRAINT_F,
  CONSTRAINT_X,
  CONSTRAINT_Cpc,
  CONSTRAINT_Cal,
  CONSTRAINT_C32,
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
  return c >= CONSTRAINT_r && c <= CONSTRAINT_Rzd;
}

static inline bool
insn_extra_memory_constraint (enum constraint_num c)
{
  return c >= CONSTRAINT_m && c <= CONSTRAINT_Ucd;
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
  if (c >= CONSTRAINT_CnL && c <= CONSTRAINT_C62)
    return;
  if (c >= CONSTRAINT_Rcq && c <= CONSTRAINT_Rcc)
    {
      *allows_reg = true;
      return;
    }
  if (c >= CONSTRAINT_V && c <= CONSTRAINT_Ucm)
    {
      *allows_mem = true;
      return;
    }
  (void) c;
  *allows_reg = true;
  *allows_mem = true;
}

static inline size_t
insn_constraint_len (char fc, const char *str ATTRIBUTE_UNUSED)
{
  switch (fc)
    {
    case 'A': return 3;
    case 'C': return 3;
    case 'J': return 3;
    case 'R': return 3;
    case 'U': return 3;
    default: break;
    }
  return 1;
}

#define CONSTRAINT_LEN(c_,s_) insn_constraint_len (c_,s_)

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
      if (c >= CONSTRAINT_CnL)
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
