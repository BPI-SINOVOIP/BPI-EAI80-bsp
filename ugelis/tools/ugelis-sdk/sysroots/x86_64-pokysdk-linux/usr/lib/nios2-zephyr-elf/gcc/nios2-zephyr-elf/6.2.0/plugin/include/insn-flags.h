/* Generated automatically by the program `genflags'
   from the machine description file `md'.  */

#ifndef GCC_INSN_FLAGS_H
#define GCC_INSN_FLAGS_H

#define HAVE_movqi_internal ((register_operand (operands[0], QImode) \
    || reg_or_0_operand (operands[1], QImode)))
#define HAVE_movhi_internal ((register_operand (operands[0], HImode) \
    || reg_or_0_operand (operands[1], HImode)))
#define HAVE_movsi_internal ((register_operand (operands[0], SImode) \
    || reg_or_0_operand (operands[1], SImode)))
#define HAVE_ldbuio 1
#define HAVE_ldhuio 1
#define HAVE_ldwio 1
#define HAVE_ldbio_signed 1
#define HAVE_ldhio_signed 1
#define HAVE_stbio 1
#define HAVE_sthio 1
#define HAVE_stwio 1
#define HAVE_zero_extendhisi2 1
#define HAVE_zero_extendqihi2 1
#define HAVE_zero_extendqisi2 1
#define HAVE_extendhisi2 1
#define HAVE_extendqihi2 1
#define HAVE_extendqisi2 1
#define HAVE_addsi3 1
#define HAVE_subsi3 1
#define HAVE_mulsi3 (TARGET_HAS_MUL)
#define HAVE_divsi3_insn (TARGET_HAS_DIV)
#define HAVE_udivsi3 (TARGET_HAS_DIV)
#define HAVE_smulsi3_highpart (TARGET_HAS_MULX)
#define HAVE_umulsi3_highpart (TARGET_HAS_MULX)
#define HAVE_negsi2 1
#define HAVE_one_cmplsi2 1
#define HAVE_andsi3 1
#define HAVE_iorsi3 1
#define HAVE_xorsi3 1
#define HAVE_ashlsi3 1
#define HAVE_ashrsi3 1
#define HAVE_lshrsi3 1
#define HAVE_rotlsi3 1
#define HAVE_rotrsi3 1
#define HAVE_extzv (TARGET_HAS_BMX)
#define HAVE_insv (TARGET_HAS_BMX)
#define HAVE_addsf3 (nios2_fpu_insn_enabled (n2fpu_fadds))
#define HAVE_subsf3 (nios2_fpu_insn_enabled (n2fpu_fsubs))
#define HAVE_mulsf3 (nios2_fpu_insn_enabled (n2fpu_fmuls))
#define HAVE_divsf3 (nios2_fpu_insn_enabled (n2fpu_fdivs))
#define HAVE_adddf3 (nios2_fpu_insn_enabled (n2fpu_faddd))
#define HAVE_subdf3 (nios2_fpu_insn_enabled (n2fpu_fsubd))
#define HAVE_muldf3 (nios2_fpu_insn_enabled (n2fpu_fmuld))
#define HAVE_divdf3 (nios2_fpu_insn_enabled (n2fpu_fdivd))
#define HAVE_sminsf3 (nios2_fpu_insn_enabled (n2fpu_fmins))
#define HAVE_smaxsf3 (nios2_fpu_insn_enabled (n2fpu_fmaxs))
#define HAVE_smindf3 (nios2_fpu_insn_enabled (n2fpu_fmind))
#define HAVE_smaxdf3 (nios2_fpu_insn_enabled (n2fpu_fmaxd))
#define HAVE_abssf2 (nios2_fpu_insn_enabled (n2fpu_fabss))
#define HAVE_negsf2 (nios2_fpu_insn_enabled (n2fpu_fnegs))
#define HAVE_sqrtsf2 (nios2_fpu_insn_enabled (n2fpu_fsqrts))
#define HAVE_absdf2 (nios2_fpu_insn_enabled (n2fpu_fabsd))
#define HAVE_negdf2 (nios2_fpu_insn_enabled (n2fpu_fnegd))
#define HAVE_sqrtdf2 (nios2_fpu_insn_enabled (n2fpu_fsqrtd))
#define HAVE_nios2_fwrx (nios2_fpu_insn_enabled (n2fpu_fwrx))
#define HAVE_nios2_fwry (nios2_fpu_insn_enabled (n2fpu_fwry))
#define HAVE_nios2_frdxlo (nios2_fpu_insn_enabled (n2fpu_frdxlo))
#define HAVE_nios2_frdxhi (nios2_fpu_insn_enabled (n2fpu_frdxhi))
#define HAVE_nios2_frdy (nios2_fpu_insn_enabled (n2fpu_frdy))
#define HAVE_cossf2 (nios2_fpu_insn_enabled (n2fpu_fcoss))
#define HAVE_sinsf2 (nios2_fpu_insn_enabled (n2fpu_fsins))
#define HAVE_tansf2 (nios2_fpu_insn_enabled (n2fpu_ftans))
#define HAVE_atansf2 (nios2_fpu_insn_enabled (n2fpu_fatans))
#define HAVE_expsf2 (nios2_fpu_insn_enabled (n2fpu_fexps))
#define HAVE_logsf2 (nios2_fpu_insn_enabled (n2fpu_flogs))
#define HAVE_cosdf2 (nios2_fpu_insn_enabled (n2fpu_fcosd))
#define HAVE_sindf2 (nios2_fpu_insn_enabled (n2fpu_fsind))
#define HAVE_tandf2 (nios2_fpu_insn_enabled (n2fpu_ftand))
#define HAVE_atandf2 (nios2_fpu_insn_enabled (n2fpu_fatand))
#define HAVE_expdf2 (nios2_fpu_insn_enabled (n2fpu_fexpd))
#define HAVE_logdf2 (nios2_fpu_insn_enabled (n2fpu_flogd))
#define HAVE_floatsisf2 (nios2_fpu_insn_enabled (n2fpu_floatis))
#define HAVE_floatunssisf2 (nios2_fpu_insn_enabled (n2fpu_floatus))
#define HAVE_floatsidf2 (nios2_fpu_insn_enabled (n2fpu_floatid))
#define HAVE_floatunssidf2 (nios2_fpu_insn_enabled (n2fpu_floatud))
#define HAVE_fix_truncsfsi2 (nios2_fpu_insn_enabled (n2fpu_fixsi))
#define HAVE_fixuns_truncsfsi2 (nios2_fpu_insn_enabled (n2fpu_fixsu))
#define HAVE_fix_truncdfsi2 (nios2_fpu_insn_enabled (n2fpu_fixdi))
#define HAVE_fixuns_truncdfsi2 (nios2_fpu_insn_enabled (n2fpu_fixdu))
#define HAVE_lroundsfsi2 (nios2_fpu_insn_enabled (n2fpu_round))
#define HAVE_extendsfdf2 (nios2_fpu_insn_enabled (n2fpu_fextsd))
#define HAVE_truncdfsf2 (nios2_fpu_insn_enabled (n2fpu_ftruncds))
#define HAVE_simple_return 1
#define HAVE_blockage 1
#define HAVE_eh_set_ra 1
#define HAVE_indirect_jump 1
#define HAVE_jump 1
#define HAVE_sibcall_internal 1
#define HAVE_sibcall_value_internal 1
#define HAVE_nios2_cbranch 1
#define HAVE_nios2_seqsf (nios2_fpu_insn_enabled (n2fpu_fcmpeqs))
#define HAVE_nios2_snesf (nios2_fpu_insn_enabled (n2fpu_fcmpnes))
#define HAVE_nios2_sgtsf (nios2_fpu_insn_enabled (n2fpu_fcmpgts))
#define HAVE_nios2_sgesf (nios2_fpu_insn_enabled (n2fpu_fcmpges))
#define HAVE_nios2_slesf (nios2_fpu_insn_enabled (n2fpu_fcmples))
#define HAVE_nios2_sltsf (nios2_fpu_insn_enabled (n2fpu_fcmplts))
#define HAVE_nios2_seqdf (nios2_fpu_insn_enabled (n2fpu_fcmpeqd))
#define HAVE_nios2_snedf (nios2_fpu_insn_enabled (n2fpu_fcmpned))
#define HAVE_nios2_sgtdf (nios2_fpu_insn_enabled (n2fpu_fcmpgtd))
#define HAVE_nios2_sgedf (nios2_fpu_insn_enabled (n2fpu_fcmpged))
#define HAVE_nios2_sledf (nios2_fpu_insn_enabled (n2fpu_fcmpled))
#define HAVE_nios2_sltdf (nios2_fpu_insn_enabled (n2fpu_fcmpltd))
#define HAVE_nios2_cmpeq 1
#define HAVE_nios2_cmpne 1
#define HAVE_nios2_cmpge 1
#define HAVE_nios2_cmplt 1
#define HAVE_nios2_cmpgeu 1
#define HAVE_nios2_cmpltu 1
#define HAVE_custom_nxx 1
#define HAVE_custom_xnxx 1
#define HAVE_nop 1
#define HAVE_rdctl 1
#define HAVE_wrctl 1
#define HAVE_rdprs 1
#define HAVE_flushd 1
#define HAVE_flushda 1
#define HAVE_wrpie (TARGET_ARCH_R2)
#define HAVE_eni (TARGET_ARCH_R2)
#define HAVE_trap 1
#define HAVE_ctrapsi4 1
#define HAVE_load_got_register 1
#define HAVE_ldex (TARGET_ARCH_R2)
#define HAVE_ldsex (TARGET_ARCH_R2)
#define HAVE_stex (TARGET_ARCH_R2)
#define HAVE_stsex (TARGET_ARCH_R2)
#define HAVE_movqi 1
#define HAVE_movhi 1
#define HAVE_movsi 1
#define HAVE_ldbio 1
#define HAVE_ldhio 1
#define HAVE_divsi3 1
#define HAVE_mulsidi3 (TARGET_HAS_MULX)
#define HAVE_umulsidi3 (TARGET_HAS_MULX)
#define HAVE_prologue 1
#define HAVE_epilogue 1
#define HAVE_sibcall_epilogue 1
#define HAVE_return (nios2_can_use_return_insn ())
#define HAVE_eh_return 1
#define HAVE_call 1
#define HAVE_call_value 1
#define HAVE_sibcall 1
#define HAVE_sibcall_value 1
#define HAVE_tablejump 1
#define HAVE_cstoresi4 1
#define HAVE_cstoresf4 1
#define HAVE_cstoredf4 1
#define HAVE_cbranchsi4 1
#define HAVE_cbranchsf4 1
#define HAVE_cbranchdf4 1
#define HAVE_memory_barrier 1
#define HAVE_sync 1
extern rtx        gen_movqi_internal         (rtx, rtx);
extern rtx        gen_movhi_internal         (rtx, rtx);
extern rtx        gen_movsi_internal         (rtx, rtx);
extern rtx        gen_ldbuio                 (rtx, rtx);
extern rtx        gen_ldhuio                 (rtx, rtx);
extern rtx        gen_ldwio                  (rtx, rtx);
extern rtx        gen_ldbio_signed           (rtx, rtx);
extern rtx        gen_ldhio_signed           (rtx, rtx);
extern rtx        gen_stbio                  (rtx, rtx);
extern rtx        gen_sthio                  (rtx, rtx);
extern rtx        gen_stwio                  (rtx, rtx);
extern rtx        gen_zero_extendhisi2       (rtx, rtx);
extern rtx        gen_zero_extendqihi2       (rtx, rtx);
extern rtx        gen_zero_extendqisi2       (rtx, rtx);
extern rtx        gen_extendhisi2            (rtx, rtx);
extern rtx        gen_extendqihi2            (rtx, rtx);
extern rtx        gen_extendqisi2            (rtx, rtx);
extern rtx        gen_addsi3                 (rtx, rtx, rtx);
extern rtx        gen_subsi3                 (rtx, rtx, rtx);
extern rtx        gen_mulsi3                 (rtx, rtx, rtx);
extern rtx        gen_divsi3_insn            (rtx, rtx, rtx);
extern rtx        gen_udivsi3                (rtx, rtx, rtx);
extern rtx        gen_smulsi3_highpart       (rtx, rtx, rtx);
extern rtx        gen_umulsi3_highpart       (rtx, rtx, rtx);
extern rtx        gen_negsi2                 (rtx, rtx);
extern rtx        gen_one_cmplsi2            (rtx, rtx);
extern rtx        gen_andsi3                 (rtx, rtx, rtx);
extern rtx        gen_iorsi3                 (rtx, rtx, rtx);
extern rtx        gen_xorsi3                 (rtx, rtx, rtx);
extern rtx        gen_ashlsi3                (rtx, rtx, rtx);
extern rtx        gen_ashrsi3                (rtx, rtx, rtx);
extern rtx        gen_lshrsi3                (rtx, rtx, rtx);
extern rtx        gen_rotlsi3                (rtx, rtx, rtx);
extern rtx        gen_rotrsi3                (rtx, rtx, rtx);
extern rtx        gen_extzv                  (rtx, rtx, rtx, rtx);
extern rtx        gen_insv                   (rtx, rtx, rtx, rtx);
extern rtx        gen_addsf3                 (rtx, rtx, rtx);
extern rtx        gen_subsf3                 (rtx, rtx, rtx);
extern rtx        gen_mulsf3                 (rtx, rtx, rtx);
extern rtx        gen_divsf3                 (rtx, rtx, rtx);
extern rtx        gen_adddf3                 (rtx, rtx, rtx);
extern rtx        gen_subdf3                 (rtx, rtx, rtx);
extern rtx        gen_muldf3                 (rtx, rtx, rtx);
extern rtx        gen_divdf3                 (rtx, rtx, rtx);
extern rtx        gen_sminsf3                (rtx, rtx, rtx);
extern rtx        gen_smaxsf3                (rtx, rtx, rtx);
extern rtx        gen_smindf3                (rtx, rtx, rtx);
extern rtx        gen_smaxdf3                (rtx, rtx, rtx);
extern rtx        gen_abssf2                 (rtx, rtx);
extern rtx        gen_negsf2                 (rtx, rtx);
extern rtx        gen_sqrtsf2                (rtx, rtx);
extern rtx        gen_absdf2                 (rtx, rtx);
extern rtx        gen_negdf2                 (rtx, rtx);
extern rtx        gen_sqrtdf2                (rtx, rtx);
extern rtx        gen_nios2_fwrx             (rtx);
extern rtx        gen_nios2_fwry             (rtx);
extern rtx        gen_nios2_frdxlo           (rtx);
extern rtx        gen_nios2_frdxhi           (rtx);
extern rtx        gen_nios2_frdy             (rtx);
extern rtx        gen_cossf2                 (rtx, rtx);
extern rtx        gen_sinsf2                 (rtx, rtx);
extern rtx        gen_tansf2                 (rtx, rtx);
extern rtx        gen_atansf2                (rtx, rtx);
extern rtx        gen_expsf2                 (rtx, rtx);
extern rtx        gen_logsf2                 (rtx, rtx);
extern rtx        gen_cosdf2                 (rtx, rtx);
extern rtx        gen_sindf2                 (rtx, rtx);
extern rtx        gen_tandf2                 (rtx, rtx);
extern rtx        gen_atandf2                (rtx, rtx);
extern rtx        gen_expdf2                 (rtx, rtx);
extern rtx        gen_logdf2                 (rtx, rtx);
extern rtx        gen_floatsisf2             (rtx, rtx);
extern rtx        gen_floatunssisf2          (rtx, rtx);
extern rtx        gen_floatsidf2             (rtx, rtx);
extern rtx        gen_floatunssidf2          (rtx, rtx);
extern rtx        gen_fix_truncsfsi2         (rtx, rtx);
extern rtx        gen_fixuns_truncsfsi2      (rtx, rtx);
extern rtx        gen_fix_truncdfsi2         (rtx, rtx);
extern rtx        gen_fixuns_truncdfsi2      (rtx, rtx);
extern rtx        gen_lroundsfsi2            (rtx, rtx);
extern rtx        gen_extendsfdf2            (rtx, rtx);
extern rtx        gen_truncdfsf2             (rtx, rtx);
extern rtx        gen_simple_return          (void);
extern rtx        gen_blockage               (void);
extern rtx        gen_eh_set_ra              (rtx);
extern rtx        gen_indirect_jump          (rtx);
extern rtx        gen_jump                   (rtx);
extern rtx        gen_sibcall_internal       (rtx, rtx);
extern rtx        gen_sibcall_value_internal (rtx, rtx, rtx);
extern rtx        gen_nios2_cbranch          (rtx, rtx, rtx, rtx);
extern rtx        gen_nios2_seqsf            (rtx, rtx, rtx);
extern rtx        gen_nios2_snesf            (rtx, rtx, rtx);
extern rtx        gen_nios2_sgtsf            (rtx, rtx, rtx);
extern rtx        gen_nios2_sgesf            (rtx, rtx, rtx);
extern rtx        gen_nios2_slesf            (rtx, rtx, rtx);
extern rtx        gen_nios2_sltsf            (rtx, rtx, rtx);
extern rtx        gen_nios2_seqdf            (rtx, rtx, rtx);
extern rtx        gen_nios2_snedf            (rtx, rtx, rtx);
extern rtx        gen_nios2_sgtdf            (rtx, rtx, rtx);
extern rtx        gen_nios2_sgedf            (rtx, rtx, rtx);
extern rtx        gen_nios2_sledf            (rtx, rtx, rtx);
extern rtx        gen_nios2_sltdf            (rtx, rtx, rtx);
extern rtx        gen_nios2_cmpeq            (rtx, rtx, rtx);
extern rtx        gen_nios2_cmpne            (rtx, rtx, rtx);
extern rtx        gen_nios2_cmpge            (rtx, rtx, rtx);
extern rtx        gen_nios2_cmplt            (rtx, rtx, rtx);
extern rtx        gen_nios2_cmpgeu           (rtx, rtx, rtx);
extern rtx        gen_nios2_cmpltu           (rtx, rtx, rtx);
extern rtx        gen_custom_nxx             (rtx, rtx, rtx);
extern rtx        gen_custom_xnxx            (rtx, rtx, rtx, rtx);
extern rtx        gen_nop                    (void);
extern rtx        gen_rdctl                  (rtx, rtx);
extern rtx        gen_wrctl                  (rtx, rtx);
extern rtx        gen_rdprs                  (rtx, rtx, rtx);
extern rtx        gen_flushd                 (rtx);
extern rtx        gen_flushda                (rtx);
extern rtx        gen_wrpie                  (rtx, rtx);
extern rtx        gen_eni                    (rtx);
extern rtx        gen_trap                   (void);
extern rtx        gen_ctrapsi4               (rtx, rtx, rtx, rtx);
extern rtx        gen_load_got_register      (rtx, rtx);
extern rtx        gen_ldex                   (rtx, rtx);
extern rtx        gen_ldsex                  (rtx, rtx);
extern rtx        gen_stex                   (rtx, rtx, rtx);
extern rtx        gen_stsex                  (rtx, rtx, rtx);
extern rtx        gen_movqi                  (rtx, rtx);
extern rtx        gen_movhi                  (rtx, rtx);
extern rtx        gen_movsi                  (rtx, rtx);
extern rtx        gen_ldbio                  (rtx, rtx);
extern rtx        gen_ldhio                  (rtx, rtx);
extern rtx        gen_divsi3                 (rtx, rtx, rtx);
extern rtx        gen_mulsidi3               (rtx, rtx, rtx);
extern rtx        gen_umulsidi3              (rtx, rtx, rtx);
extern rtx        gen_prologue               (void);
extern rtx        gen_epilogue               (void);
extern rtx        gen_sibcall_epilogue       (void);
extern rtx        gen_return                 (void);
extern rtx        gen_eh_return              (rtx);
extern rtx        gen_call                   (rtx, rtx);
extern rtx        gen_call_value             (rtx, rtx, rtx);
extern rtx        gen_sibcall                (rtx, rtx);
extern rtx        gen_sibcall_value          (rtx, rtx, rtx);
extern rtx        gen_tablejump              (rtx, rtx);
extern rtx        gen_cstoresi4              (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoresf4              (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoredf4              (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchsi4             (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchsf4             (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchdf4             (rtx, rtx, rtx, rtx);
extern rtx        gen_memory_barrier         (void);
extern rtx        gen_sync                   (void);
static inline rtx gen_get_thread_pointersi   (rtx);
static inline rtx
gen_get_thread_pointersi(rtx ARG_UNUSED (a))
{
  return 0;
}

#endif /* GCC_INSN_FLAGS_H */
