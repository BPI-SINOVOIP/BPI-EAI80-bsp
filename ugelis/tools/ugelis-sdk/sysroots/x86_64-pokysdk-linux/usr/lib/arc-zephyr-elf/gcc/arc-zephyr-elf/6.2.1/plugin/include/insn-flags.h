/* Generated automatically by the program `genflags'
   from the machine description file `md'.  */

#ifndef GCC_INSN_FLAGS_H
#define GCC_INSN_FLAGS_H

#define HAVE_store_direct 1
#define HAVE_unary_comparison 1
#define HAVE_addsi_compare_2 1
#define HAVE_abssi2 1
#define HAVE_smaxsi3 1
#define HAVE_sminsi3 1
#define HAVE_mulhisi3_imm (TARGET_MPYW)
#define HAVE_mulhisi3_reg (TARGET_MPYW)
#define HAVE_umulhisi3_imm (TARGET_MPYW)
#define HAVE_umulhisi3_reg (TARGET_MPYW)
#define HAVE_umul_600 (TARGET_MULMAC_32BY16_SET)
#define HAVE_mac_600 (TARGET_MULMAC_32BY16_SET)
#define HAVE_mulsi_600 (TARGET_MUL64_SET)
#define HAVE_mulsi3_600_lib (!TARGET_ANY_MPY \
   && SFUNC_CHECK_PREDICABLE)
#define HAVE_mulsidi_600 (TARGET_MUL64_SET)
#define HAVE_mul64 (TARGET_MUL64_SET)
#define HAVE_umulsidi_600 (TARGET_MUL64_SET)
#define HAVE_mulu64 (TARGET_MUL64_SET)
#define HAVE_mulsi3_700 (TARGET_ARC700_MPY)
#define HAVE_mulsi3_v2 (TARGET_MULTI)
#define HAVE_mul64_600 (TARGET_MULMAC_32BY16_SET)
#define HAVE_mac64_600 (TARGET_MULMAC_32BY16_SET)
#define HAVE_mulsidi3_700 (TARGET_MPY)
#define HAVE_mulsi3_highpart (TARGET_MPY)
#define HAVE_umulsi3_highpart_600_lib_le (!TARGET_BIG_ENDIAN \
   && !TARGET_ANY_MPY \
   && SFUNC_CHECK_PREDICABLE)
#define HAVE_umulsi3_highpart_600_lib_be (TARGET_BIG_ENDIAN \
   && !TARGET_ANY_MPY \
   && SFUNC_CHECK_PREDICABLE)
#define HAVE_umulsi3_highpart_int (TARGET_MPY)
#define HAVE_umul64_600 (TARGET_MULMAC_32BY16_SET)
#define HAVE_umac64_600 (TARGET_MULMAC_32BY16_SET)
#define HAVE_umulsidi3_700 (TARGET_MPY)
#define HAVE_umulsidi3_600_lib (!TARGET_ANY_MPY \
   && SFUNC_CHECK_PREDICABLE)
#define HAVE_add_f 1
#define HAVE_adc (register_operand (operands[1], SImode) \
   || register_operand (operands[2], SImode))
#define HAVE_subsi3_insn (register_operand (operands[1], SImode) \
   || register_operand (operands[2], SImode))
#define HAVE_subdi3_i (register_operand (operands[1], DImode) \
   || register_operand (operands[2], DImode))
#define HAVE_sbc (register_operand (operands[1], SImode) \
   || register_operand (operands[2], SImode))
#define HAVE_sub_f (register_operand (operands[1], SImode) \
   || register_operand (operands[2], SImode))
#define HAVE_andsi3_i ((register_operand (operands[1], SImode) \
    && nonmemory_operand (operands[2], SImode)) \
   || (memory_operand (operands[1], SImode) \
       && satisfies_constraint_Cux (operands[2])))
#define HAVE_iorsi3 1
#define HAVE_xorsi3 1
#define HAVE_negsi2 1
#define HAVE_one_cmplsi2 1
#define HAVE_one_cmpldi2 1
#define HAVE_shift_si3 (!TARGET_BARREL_SHIFTER)
#define HAVE_shift_si3_loop (!TARGET_BARREL_SHIFTER)
#define HAVE_rotrsi3 (TARGET_BARREL_SHIFTER)
#define HAVE_cmpsi_cc_insn_mixed 1
#define HAVE_jump_i (!TARGET_LONG_CALLS_SET || !CROSSING_JUMP_P (insn))
#define HAVE_indirect_jump 1
#define HAVE_casesi_load 1
#define HAVE_casesi_jump 1
#define HAVE_casesi_compact_jump (TARGET_COMPACT_CASESI)
#define HAVE_nop 1
#define HAVE_nopv 1
#define HAVE_flush_icache 1
#define HAVE_clrsbsi2 (TARGET_NORM)
#define HAVE_norm_f (TARGET_NORM)
#define HAVE_clrsbhi2 (TARGET_NORM)
#define HAVE_normw (TARGET_NORM)
#define HAVE_swap (TARGET_SWAP)
#define HAVE_divaw (TARGET_ARC700 || TARGET_EA_SET)
#define HAVE_flag 1
#define HAVE_brk 1
#define HAVE_rtie 1
#define HAVE_sync 1
#define HAVE_swi 1
#define HAVE_sleep (check_if_valid_sleep_operand(operands,0))
#define HAVE_core_read 1
#define HAVE_core_write 1
#define HAVE_lr 1
#define HAVE_sr 1
#define HAVE_trap_s (!TARGET_ARC600_FAMILY)
#define HAVE_unimp_s (!TARGET_ARC600_FAMILY)
#define HAVE_simple_return (reload_completed)
#define HAVE_p_return_i (reload_completed \
   && !(TARGET_V2 \
     && ARC_INTERRUPT_P (arc_compute_function_type (cfun))))
#define HAVE_cbranchsi4_scratch ((reload_completed \
     || (TARGET_EARLY_CBRANCHSI \
	 && brcc_nolimm_operator (operands[0], VOIDmode))) \
    && !CROSSING_JUMP_P (insn))
#define HAVE_doloop_begin_i 1
#define HAVE_doloop_end_i 1
#define HAVE_doloop_fallback_m (reload_completed)
#define HAVE_abssf2 1
#define HAVE_negsf2 1
#define HAVE_tls_load_tp_soft 1
#define HAVE_tls_gd_get_addr 1
#define HAVE_bswapsi2 (TARGET_V2 && TARGET_SWAP)
#define HAVE_prefetch_1 (TARGET_HS)
#define HAVE_prefetch_2 (TARGET_HS)
#define HAVE_prefetch_3 (TARGET_HS)
#define HAVE_divsi3 (TARGET_DIVREM)
#define HAVE_udivsi3 (TARGET_DIVREM)
#define HAVE_modsi3 (TARGET_DIVREM)
#define HAVE_umodsi3 (TARGET_DIVREM)
#define HAVE_arcseteq (TARGET_V2 && TARGET_CODE_DENSITY)
#define HAVE_arcsetne (TARGET_V2 && TARGET_CODE_DENSITY)
#define HAVE_arcsetgt (TARGET_V2 && TARGET_CODE_DENSITY)
#define HAVE_arcsetlt (TARGET_V2 && TARGET_CODE_DENSITY)
#define HAVE_arcsetge (TARGET_V2 && TARGET_CODE_DENSITY)
#define HAVE_arcsetle (TARGET_V2 && TARGET_CODE_DENSITY)
#define HAVE_arcsetltu (TARGET_V2 && TARGET_CODE_DENSITY)
#define HAVE_arcsetgeu (TARGET_V2 && TARGET_CODE_DENSITY)
#define HAVE_arcsethi (TARGET_V2 && TARGET_CODE_DENSITY)
#define HAVE_arcsetls (TARGET_V2 && TARGET_CODE_DENSITY)
#define HAVE_extzvsi (TARGET_HS && TARGET_BARREL_SHIFTER)
#define HAVE_kflag (TARGET_V2)
#define HAVE_clri (TARGET_V2)
#define HAVE_ffs (TARGET_NORM && TARGET_V2)
#define HAVE_ffs_f (TARGET_NORM && TARGET_V2)
#define HAVE_fls (TARGET_NORM && TARGET_V2)
#define HAVE_seti (TARGET_V2)
#define HAVE_stack_irq_dwarf 1
#define HAVE_stack_tie 1
#define HAVE_cmpsfpx_raw (TARGET_ARGONAUT_SET && TARGET_SPFP)
#define HAVE_cmpdfpx_raw (TARGET_ARGONAUT_SET && TARGET_DPFP)
#define HAVE_dexcl_2op (TARGET_DPFP)
#define HAVE_adddf3_insn (TARGET_DPFP && \
   !(GET_CODE(operands[2]) == CONST_DOUBLE && GET_CODE(operands[3]) == CONST_INT))
#define HAVE_muldf3_insn (TARGET_DPFP && \
   !(GET_CODE(operands[2]) == CONST_DOUBLE && GET_CODE(operands[3]) == CONST_INT))
#define HAVE_subdf3_insn (TARGET_DPFP && \
   !(GET_CODE(operands[2]) == CONST_DOUBLE && GET_CODE(operands[3]) == CONST_INT) && \
   !(GET_CODE(operands[1]) == CONST_DOUBLE && GET_CODE(operands[3]) == CONST_INT))
#define HAVE_fmasf4_fpu (TARGET_FP_SP_FUSED \
   && (register_operand (operands[1], SFmode) \
       || register_operand (operands[2], SFmode)))
#define HAVE_fnmasf4_fpu (TARGET_FP_SP_FUSED \
   && (register_operand (operands[1], SFmode) \
       || register_operand (operands[2], SFmode)))
#define HAVE_fmadf4_split (TARGET_FP_DP_FUSED)
#define HAVE_fnmadf4_split (TARGET_FP_DP_FUSED)
#define HAVE_fmadf4_fpu (TARGET_FP_DP_FUSED)
#define HAVE_fnmadf4_fpu (TARGET_FP_DP_FUSED)
#define HAVE_divsf3_fpu (TARGET_FP_SP_SQRT \
   && (register_operand (operands[1], SFmode) \
       || register_operand (operands[2], SFmode)))
#define HAVE_sqrtsf2_fpu (TARGET_FP_SP_SQRT)
#define HAVE_divdf3 (TARGET_FP_DP_SQRT)
#define HAVE_sqrtdf2 (TARGET_FP_DP_SQRT)
#define HAVE_extendsfdf2 (TARGET_FP_DP_CONV)
#define HAVE_floatsidf2 (TARGET_FP_DP_CONV)
#define HAVE_floatunssidf2 (TARGET_FP_DP_CONV)
#define HAVE_fixuns_truncsfdi2 (TARGET_FP_DP_CONV)
#define HAVE_fix_truncsfdi2 (TARGET_FP_DP_CONV)
#define HAVE_floatsisf2_fpu (TARGET_FP_SP_CONV)
#define HAVE_floatunssisf2 (TARGET_FP_SP_CONV)
#define HAVE_fixuns_truncsfsi2 (TARGET_FP_SP_CONV)
#define HAVE_fix_truncsfsi2_fpu (TARGET_FP_SP_CONV)
#define HAVE_floatdidf2 (TARGET_FP_DP_CONV)
#define HAVE_floatunsdidf2 (TARGET_FP_DP_CONV)
#define HAVE_fixuns_truncdfdi2 (TARGET_FP_DP_CONV)
#define HAVE_fix_truncdfdi2 (TARGET_FP_DP_CONV)
#define HAVE_truncdfsf2 (TARGET_FP_DP_CONV)
#define HAVE_floatdisf2 (TARGET_FP_DP_CONV)
#define HAVE_floatunsdisf2 (TARGET_FP_DP_CONV)
#define HAVE_fixuns_truncdfsi2 (TARGET_FP_DP_CONV)
#define HAVE_fix_truncdfsi2 (TARGET_FP_DP_CONV)
#define HAVE_vld128_insn (TARGET_SIMD_SET)
#define HAVE_vst128_insn (TARGET_SIMD_SET)
#define HAVE_vst64_insn (TARGET_SIMD_SET)
#define HAVE_movv8hi_insn (TARGET_SIMD_SET && !(GET_CODE (operands[0]) == MEM && GET_CODE(operands[1]) == MEM))
#define HAVE_movti_insn 1
#define HAVE_vaddaw_insn (TARGET_SIMD_SET)
#define HAVE_vaddw_insn (TARGET_SIMD_SET)
#define HAVE_vavb_insn (TARGET_SIMD_SET)
#define HAVE_vavrb_insn (TARGET_SIMD_SET)
#define HAVE_vdifaw_insn (TARGET_SIMD_SET)
#define HAVE_vdifw_insn (TARGET_SIMD_SET)
#define HAVE_vmaxaw_insn (TARGET_SIMD_SET)
#define HAVE_vmaxw_insn (TARGET_SIMD_SET)
#define HAVE_vminaw_insn (TARGET_SIMD_SET)
#define HAVE_vminw_insn (TARGET_SIMD_SET)
#define HAVE_vmulaw_insn (TARGET_SIMD_SET)
#define HAVE_vmulfaw_insn (TARGET_SIMD_SET)
#define HAVE_vmulfw_insn (TARGET_SIMD_SET)
#define HAVE_vmulw_insn (TARGET_SIMD_SET)
#define HAVE_vsubaw_insn (TARGET_SIMD_SET)
#define HAVE_vsubw_insn (TARGET_SIMD_SET)
#define HAVE_vsummw_insn (TARGET_SIMD_SET)
#define HAVE_vand_insn (TARGET_SIMD_SET)
#define HAVE_vandaw_insn (TARGET_SIMD_SET)
#define HAVE_vbic_insn (TARGET_SIMD_SET)
#define HAVE_vbicaw_insn (TARGET_SIMD_SET)
#define HAVE_vor_insn (TARGET_SIMD_SET)
#define HAVE_vxor_insn (TARGET_SIMD_SET)
#define HAVE_vxoraw_insn (TARGET_SIMD_SET)
#define HAVE_veqw_insn (TARGET_SIMD_SET)
#define HAVE_vlew_insn (TARGET_SIMD_SET)
#define HAVE_vltw_insn (TARGET_SIMD_SET)
#define HAVE_vnew_insn (TARGET_SIMD_SET)
#define HAVE_vmr1aw_insn (TARGET_SIMD_SET)
#define HAVE_vmr1w_insn (TARGET_SIMD_SET)
#define HAVE_vmr2aw_insn (TARGET_SIMD_SET)
#define HAVE_vmr2w_insn (TARGET_SIMD_SET)
#define HAVE_vmr3aw_insn (TARGET_SIMD_SET)
#define HAVE_vmr3w_insn (TARGET_SIMD_SET)
#define HAVE_vmr4aw_insn (TARGET_SIMD_SET)
#define HAVE_vmr4w_insn (TARGET_SIMD_SET)
#define HAVE_vmr5aw_insn (TARGET_SIMD_SET)
#define HAVE_vmr5w_insn (TARGET_SIMD_SET)
#define HAVE_vmr6aw_insn (TARGET_SIMD_SET)
#define HAVE_vmr6w_insn (TARGET_SIMD_SET)
#define HAVE_vmr7aw_insn (TARGET_SIMD_SET)
#define HAVE_vmr7w_insn (TARGET_SIMD_SET)
#define HAVE_vmrb_insn (TARGET_SIMD_SET)
#define HAVE_vh264f_insn (TARGET_SIMD_SET)
#define HAVE_vh264ft_insn (TARGET_SIMD_SET)
#define HAVE_vh264fw_insn (TARGET_SIMD_SET)
#define HAVE_vvc1f_insn (TARGET_SIMD_SET)
#define HAVE_vvc1ft_insn (TARGET_SIMD_SET)
#define HAVE_vbaddw_insn (TARGET_SIMD_SET)
#define HAVE_vbmaxw_insn (TARGET_SIMD_SET)
#define HAVE_vbminw_insn (TARGET_SIMD_SET)
#define HAVE_vbmulaw_insn (TARGET_SIMD_SET)
#define HAVE_vbmulfw_insn (TARGET_SIMD_SET)
#define HAVE_vbmulw_insn (TARGET_SIMD_SET)
#define HAVE_vbrsubw_insn (TARGET_SIMD_SET)
#define HAVE_vbsubw_insn (TARGET_SIMD_SET)
#define HAVE_vasrrwi_insn (TARGET_SIMD_SET)
#define HAVE_vasrsrwi_insn (TARGET_SIMD_SET)
#define HAVE_vasrwi_insn (TARGET_SIMD_SET)
#define HAVE_vasrpwbi_insn (TARGET_SIMD_SET)
#define HAVE_vasrrpwbi_insn (TARGET_SIMD_SET)
#define HAVE_vsr8awi_insn (TARGET_SIMD_SET)
#define HAVE_vsr8i_insn (TARGET_SIMD_SET)
#define HAVE_vmvaw_insn (TARGET_SIMD_SET)
#define HAVE_vmvw_insn (TARGET_SIMD_SET)
#define HAVE_vmvzw_insn (TARGET_SIMD_SET)
#define HAVE_vd6tapf_insn (TARGET_SIMD_SET)
#define HAVE_vmovaw_insn (TARGET_SIMD_SET)
#define HAVE_vmovw_insn (TARGET_SIMD_SET)
#define HAVE_vmovzw_insn (TARGET_SIMD_SET)
#define HAVE_vsr8_insn (TARGET_SIMD_SET)
#define HAVE_vasrw_insn (TARGET_SIMD_SET)
#define HAVE_vsr8aw_insn (TARGET_SIMD_SET)
#define HAVE_vabsaw_insn (TARGET_SIMD_SET)
#define HAVE_vabsw_insn (TARGET_SIMD_SET)
#define HAVE_vaddsuw_insn (TARGET_SIMD_SET)
#define HAVE_vsignw_insn (TARGET_SIMD_SET)
#define HAVE_vexch1_insn (TARGET_SIMD_SET)
#define HAVE_vexch2_insn (TARGET_SIMD_SET)
#define HAVE_vexch4_insn (TARGET_SIMD_SET)
#define HAVE_vupbaw_insn (TARGET_SIMD_SET)
#define HAVE_vupbw_insn (TARGET_SIMD_SET)
#define HAVE_vupsbaw_insn (TARGET_SIMD_SET)
#define HAVE_vupsbw_insn (TARGET_SIMD_SET)
#define HAVE_vdirun_insn (TARGET_SIMD_SET)
#define HAVE_vdorun_insn (TARGET_SIMD_SET)
#define HAVE_vdiwr_insn (TARGET_SIMD_SET)
#define HAVE_vdowr_insn (TARGET_SIMD_SET)
#define HAVE_vrec_insn (TARGET_SIMD_SET)
#define HAVE_vrun_insn (TARGET_SIMD_SET)
#define HAVE_vrecrun_insn (TARGET_SIMD_SET)
#define HAVE_vendrec_insn (TARGET_SIMD_SET)
#define HAVE_vld32wh_insn (TARGET_SIMD_SET)
#define HAVE_vld32wl_insn (TARGET_SIMD_SET)
#define HAVE_vld64w_insn (TARGET_SIMD_SET)
#define HAVE_vld64_insn (TARGET_SIMD_SET)
#define HAVE_vld32_insn (TARGET_SIMD_SET)
#define HAVE_vst16_n_insn (TARGET_SIMD_SET)
#define HAVE_vst32_n_insn (TARGET_SIMD_SET)
#define HAVE_vinti_insn (TARGET_SIMD_SET)
#define HAVE_bswapv2hi2 (TARGET_V2 && TARGET_SWAP)
#define HAVE_addv2hi3 (TARGET_PLUS_DMPY)
#define HAVE_addv4hi3 (TARGET_PLUS_DMPY)
#define HAVE_addv2si3 (TARGET_PLUS_DMPY)
#define HAVE_subv2hi3 (TARGET_PLUS_DMPY)
#define HAVE_subv4hi3 (TARGET_PLUS_DMPY)
#define HAVE_subv2si3 (TARGET_PLUS_DMPY)
#define HAVE_addsubv2hi3 (TARGET_PLUS_DMPY)
#define HAVE_addsubv2si3 (TARGET_PLUS_DMPY)
#define HAVE_subaddv2hi3 (TARGET_PLUS_DMPY)
#define HAVE_subaddv2si3 (TARGET_PLUS_DMPY)
#define HAVE_addsubv4hi3 (TARGET_PLUS_QMACW)
#define HAVE_subaddv4hi3 (TARGET_PLUS_QMACW)
#define HAVE_dmpyh (TARGET_PLUS_DMPY)
#define HAVE_dmpyhu (TARGET_PLUS_DMPY)
#define HAVE_arc_vec_smult_lo_v4hi (TARGET_PLUS_MACD)
#define HAVE_arc_vec_umult_lo_v4hi (TARGET_PLUS_MACD)
#define HAVE_arc_vec_smultacc_lo_v4hi (TARGET_PLUS_MACD)
#define HAVE_arc_vec_umultacc_lo_v4hi (TARGET_PLUS_MACD)
#define HAVE_arc_vec_smult_hi_v4hi (TARGET_PLUS_QMACW)
#define HAVE_arc_vec_umult_hi_v4hi (TARGET_PLUS_QMACW)
#define HAVE_arc_vec_smac_hi_v4hi (TARGET_PLUS_MACD)
#define HAVE_arc_vec_umac_hi_v4hi (TARGET_PLUS_MACD)
#define HAVE_dmach (TARGET_PLUS_DMPY)
#define HAVE_dmachu (TARGET_PLUS_DMPY)
#define HAVE_dmacwh (TARGET_PLUS_QMACW)
#define HAVE_dmacwhu (TARGET_PLUS_QMACW)
#define HAVE_vmac2h (TARGET_PLUS_MACD)
#define HAVE_vmac2hu (TARGET_PLUS_MACD)
#define HAVE_vmpy2h (TARGET_PLUS_MACD)
#define HAVE_vmpy2hu (TARGET_PLUS_MACD)
#define HAVE_qmach (TARGET_PLUS_QMACW)
#define HAVE_qmachu (TARGET_PLUS_QMACW)
#define HAVE_qmpyh (TARGET_PLUS_QMACW)
#define HAVE_qmpyhu (TARGET_PLUS_QMACW)
#define HAVE_atomic_compare_and_swapsi_1 (TARGET_ATOMIC)
#define HAVE_arc_load_exclusivesi (TARGET_ATOMIC)
#define HAVE_arc_store_exclusivesi (TARGET_ATOMIC)
#define HAVE_exchangesi 1
#define HAVE_movqi 1
#define HAVE_movhi 1
#define HAVE_movsi 1
#define HAVE_bic_f_zn 1
#define HAVE_movdi 1
#define HAVE_movsf 1
#define HAVE_movdf 1
#define HAVE_movsicc 1
#define HAVE_movdicc 1
#define HAVE_movsfcc 1
#define HAVE_movdfcc 1
#define HAVE_zero_extendqihi2 1
#define HAVE_zero_extendqisi2 1
#define HAVE_zero_extendhisi2 1
#define HAVE_extendqihi2 1
#define HAVE_extendqisi2 1
#define HAVE_extendhisi2 1
#define HAVE_mulhisi3 (TARGET_MPYW)
#define HAVE_umulhisi3 (TARGET_MPYW)
#define HAVE_mulsi3 1
#define HAVE_mulsidi3 (TARGET_ANY_MPY)
#define HAVE_umulsi3_highpart (!TARGET_MUL64_SET && !TARGET_MULMAC_32BY16_SET)
#define HAVE_umulsidi3 1
#define HAVE_addsi3 1
#define HAVE_adddi3 1
#define HAVE_subsi3 1
#define HAVE_subdi3 1
#define HAVE_andsi3 1
#define HAVE_ashlsi3 1
#define HAVE_ashrsi3 1
#define HAVE_lshrsi3 1
#define HAVE_cbranchsi4 1
#define HAVE_cstoresi4 1
#define HAVE_cstoresf4 (TARGET_FP_SP_BASE || TARGET_OPTFPE)
#define HAVE_cstoredf4 ((TARGET_FP_SP_BASE || TARGET_OPTFPE) && (TARGET_OPTFPE))
#define HAVE_scc_insn 1
#define HAVE_branch_insn 1
#define HAVE_jump 1
#define HAVE_casesi 1
#define HAVE_call 1
#define HAVE_call_value 1
#define HAVE_clzsi2 (TARGET_NORM)
#define HAVE_ctzsi2 (TARGET_NORM)
#define HAVE_sibcall 1
#define HAVE_sibcall_value 1
#define HAVE_prologue 1
#define HAVE_epilogue 1
#define HAVE_sibcall_epilogue 1
#define HAVE_return (optimize < 0)
#define HAVE_doloop_begin 1
#define HAVE_doloop_end (!TARGET_ARC601)
#define HAVE_movmemsi 1
#define HAVE_cbranchsf4 (TARGET_FP_SP_BASE || TARGET_OPTFPE)
#define HAVE_cbranchdf4 ((TARGET_FP_SP_BASE || TARGET_OPTFPE) && (TARGET_OPTFPE))
#define HAVE_cmp_float 1
#define HAVE_get_thread_pointersi 1
#define HAVE_set_thread_pointersi 1
#define HAVE_prefetch (TARGET_HS)
#define HAVE_reload_qi_load 1
#define HAVE_reload_hi_load 1
#define HAVE_reload_qi_store 1
#define HAVE_reload_hi_store 1
#define HAVE_ffssi2 (TARGET_NORM && TARGET_V2)
#define HAVE_addsf3 (TARGET_FP_SP_BASE || TARGET_SPFP)
#define HAVE_subsf3 (TARGET_FP_SP_BASE || TARGET_SPFP)
#define HAVE_mulsf3 (TARGET_FP_SP_BASE || TARGET_SPFP)
#define HAVE_adddf3 (TARGET_FP_DP_BASE || TARGET_DPFP)
#define HAVE_subdf3 (TARGET_FP_DP_BASE || TARGET_DPFP)
#define HAVE_muldf3 (TARGET_FP_DP_BASE || TARGET_DPFP)
#define HAVE_divsf3 (TARGET_FPX_QUARK || TARGET_FP_SP_SQRT || TARGET_FPU_QUARK2)
#define HAVE_sqrtsf2 (TARGET_FPX_QUARK || TARGET_FP_SP_SQRT || TARGET_FPU_QUARK2)
#define HAVE_fix_truncsfsi2 (TARGET_FPX_QUARK || TARGET_FP_SP_CONV)
#define HAVE_floatsisf2 (TARGET_FPX_QUARK || TARGET_FP_SP_CONV)
#define HAVE_extzv (TARGET_NPS_BITOPS)
#define HAVE_insv (TARGET_NPS_BITOPS)
#define HAVE_cmp_quark 1
#define HAVE_fmasf4 (TARGET_FP_SP_FUSED)
#define HAVE_fnmasf4 (TARGET_FP_SP_FUSED)
#define HAVE_fmadf4 (TARGET_FP_DP_FUSED)
#define HAVE_fnmadf4 (TARGET_FP_DP_FUSED)
#define HAVE_movv8hi 1
#define HAVE_movv2hi 1
#define HAVE_movmisalignv2hi 1
#define HAVE_movv4hi 1
#define HAVE_movv2si 1
#define HAVE_movmisalignv4hi 1
#define HAVE_movmisalignv2si 1
#define HAVE_sdot_prodv2hi (TARGET_PLUS_DMPY)
#define HAVE_udot_prodv2hi (TARGET_PLUS_DMPY)
#define HAVE_vec_widen_smult_lo_v4hi (TARGET_PLUS_QMACW)
#define HAVE_vec_widen_umult_lo_v4hi (TARGET_PLUS_QMACW)
#define HAVE_vec_widen_smult_hi_v4hi (TARGET_PLUS_MACD)
#define HAVE_vec_widen_umult_hi_v4hi (TARGET_PLUS_MACD)
#define HAVE_memory_barrier 1
#define HAVE_atomic_compare_and_swapqi (TARGET_ATOMIC)
#define HAVE_atomic_compare_and_swaphi (TARGET_ATOMIC)
#define HAVE_atomic_compare_and_swapsi (TARGET_ATOMIC)
#define HAVE_atomic_exchangesi (TARGET_ATOMIC)
#define HAVE_atomic_addsi (TARGET_ATOMIC)
#define HAVE_atomic_subsi (TARGET_ATOMIC)
#define HAVE_atomic_orsi (TARGET_ATOMIC)
#define HAVE_atomic_xorsi (TARGET_ATOMIC)
#define HAVE_atomic_andsi (TARGET_ATOMIC)
#define HAVE_atomic_nandsi (TARGET_ATOMIC)
#define HAVE_atomic_fetch_addsi (TARGET_ATOMIC)
#define HAVE_atomic_fetch_subsi (TARGET_ATOMIC)
#define HAVE_atomic_fetch_orsi (TARGET_ATOMIC)
#define HAVE_atomic_fetch_xorsi (TARGET_ATOMIC)
#define HAVE_atomic_fetch_andsi (TARGET_ATOMIC)
#define HAVE_atomic_fetch_nandsi (TARGET_ATOMIC)
#define HAVE_atomic_add_fetchsi (TARGET_ATOMIC)
#define HAVE_atomic_sub_fetchsi (TARGET_ATOMIC)
#define HAVE_atomic_or_fetchsi (TARGET_ATOMIC)
#define HAVE_atomic_xor_fetchsi (TARGET_ATOMIC)
#define HAVE_atomic_and_fetchsi (TARGET_ATOMIC)
#define HAVE_atomic_nand_fetchsi (TARGET_ATOMIC)
extern rtx        gen_store_direct                (rtx, rtx);
extern rtx        gen_unary_comparison            (rtx, rtx, rtx, rtx);
extern rtx        gen_addsi_compare_2             (rtx, rtx);
extern rtx        gen_abssi2                      (rtx, rtx);
extern rtx        gen_smaxsi3                     (rtx, rtx, rtx);
extern rtx        gen_sminsi3                     (rtx, rtx, rtx);
extern rtx        gen_mulhisi3_imm                (rtx, rtx, rtx);
extern rtx        gen_mulhisi3_reg                (rtx, rtx, rtx);
extern rtx        gen_umulhisi3_imm               (rtx, rtx, rtx);
extern rtx        gen_umulhisi3_reg               (rtx, rtx, rtx);
extern rtx        gen_umul_600                    (rtx, rtx, rtx, rtx);
extern rtx        gen_mac_600                     (rtx, rtx, rtx, rtx);
extern rtx        gen_mulsi_600                   (rtx, rtx, rtx, rtx);
extern rtx        gen_mulsi3_600_lib              (void);
extern rtx        gen_mulsidi_600                 (rtx, rtx, rtx);
extern rtx        gen_mul64                       (rtx, rtx);
extern rtx        gen_umulsidi_600                (rtx, rtx, rtx);
extern rtx        gen_mulu64                      (rtx, rtx);
extern rtx        gen_mulsi3_700                  (rtx, rtx, rtx);
extern rtx        gen_mulsi3_v2                   (rtx, rtx, rtx);
extern rtx        gen_mul64_600                   (rtx, rtx);
extern rtx        gen_mac64_600                   (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_700                (rtx, rtx, rtx);
extern rtx        gen_mulsi3_highpart             (rtx, rtx, rtx);
extern rtx        gen_umulsi3_highpart_600_lib_le (void);
extern rtx        gen_umulsi3_highpart_600_lib_be (void);
extern rtx        gen_umulsi3_highpart_int        (rtx, rtx, rtx);
extern rtx        gen_umul64_600                  (rtx, rtx);
extern rtx        gen_umac64_600                  (rtx, rtx, rtx);
extern rtx        gen_umulsidi3_700               (rtx, rtx, rtx);
extern rtx        gen_umulsidi3_600_lib           (void);
extern rtx        gen_add_f                       (rtx, rtx, rtx);
extern rtx        gen_adc                         (rtx, rtx, rtx);
extern rtx        gen_subsi3_insn                 (rtx, rtx, rtx);
extern rtx        gen_subdi3_i                    (rtx, rtx, rtx);
extern rtx        gen_sbc                         (rtx, rtx, rtx, rtx);
extern rtx        gen_sub_f                       (rtx, rtx, rtx);
extern rtx        gen_andsi3_i                    (rtx, rtx, rtx);
extern rtx        gen_iorsi3                      (rtx, rtx, rtx);
extern rtx        gen_xorsi3                      (rtx, rtx, rtx);
extern rtx        gen_negsi2                      (rtx, rtx);
extern rtx        gen_one_cmplsi2                 (rtx, rtx);
extern rtx        gen_one_cmpldi2                 (rtx, rtx);
extern rtx        gen_shift_si3                   (rtx, rtx, rtx, rtx);
extern rtx        gen_shift_si3_loop              (rtx, rtx, rtx, rtx);
extern rtx        gen_rotrsi3                     (rtx, rtx, rtx);
extern rtx        gen_cmpsi_cc_insn_mixed         (rtx, rtx);
extern rtx        gen_jump_i                      (rtx);
extern rtx        gen_indirect_jump               (rtx);
extern rtx        gen_casesi_load                 (rtx, rtx, rtx, rtx);
extern rtx        gen_casesi_jump                 (rtx, rtx);
extern rtx        gen_casesi_compact_jump         (rtx, rtx);
extern rtx        gen_nop                         (void);
extern rtx        gen_nopv                        (void);
extern rtx        gen_flush_icache                (rtx);
extern rtx        gen_clrsbsi2                    (rtx, rtx);
extern rtx        gen_norm_f                      (rtx, rtx);
extern rtx        gen_clrsbhi2                    (rtx, rtx);
extern rtx        gen_normw                       (rtx, rtx);
extern rtx        gen_swap                        (rtx, rtx);
extern rtx        gen_divaw                       (rtx, rtx, rtx);
extern rtx        gen_flag                        (rtx);
extern rtx        gen_brk                         (rtx);
extern rtx        gen_rtie                        (rtx);
extern rtx        gen_sync                        (rtx);
extern rtx        gen_swi                         (rtx);
extern rtx        gen_sleep                       (rtx);
extern rtx        gen_core_read                   (rtx, rtx);
extern rtx        gen_core_write                  (rtx, rtx);
extern rtx        gen_lr                          (rtx, rtx);
extern rtx        gen_sr                          (rtx, rtx);
extern rtx        gen_trap_s                      (rtx);
extern rtx        gen_unimp_s                     (rtx);
extern rtx        gen_simple_return               (void);
extern rtx        gen_p_return_i                  (rtx);
extern rtx        gen_cbranchsi4_scratch          (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_doloop_begin_i              (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_doloop_end_i                (rtx, rtx, rtx);
extern rtx        gen_doloop_fallback_m           (rtx, rtx, rtx);
extern rtx        gen_abssf2                      (rtx, rtx);
extern rtx        gen_negsf2                      (rtx, rtx);
extern rtx        gen_tls_load_tp_soft            (void);
extern rtx        gen_tls_gd_get_addr             (rtx);
extern rtx        gen_bswapsi2                    (rtx, rtx);
extern rtx        gen_prefetch_1                  (rtx, rtx, rtx);
extern rtx        gen_prefetch_2                  (rtx, rtx, rtx, rtx);
extern rtx        gen_prefetch_3                  (rtx, rtx, rtx);
extern rtx        gen_divsi3                      (rtx, rtx, rtx);
extern rtx        gen_udivsi3                     (rtx, rtx, rtx);
extern rtx        gen_modsi3                      (rtx, rtx, rtx);
extern rtx        gen_umodsi3                     (rtx, rtx, rtx);
extern rtx        gen_arcseteq                    (rtx, rtx, rtx);
extern rtx        gen_arcsetne                    (rtx, rtx, rtx);
extern rtx        gen_arcsetgt                    (rtx, rtx, rtx);
extern rtx        gen_arcsetlt                    (rtx, rtx, rtx);
extern rtx        gen_arcsetge                    (rtx, rtx, rtx);
extern rtx        gen_arcsetle                    (rtx, rtx, rtx);
extern rtx        gen_arcsetltu                   (rtx, rtx, rtx);
extern rtx        gen_arcsetgeu                   (rtx, rtx, rtx);
extern rtx        gen_arcsethi                    (rtx, rtx, rtx);
extern rtx        gen_arcsetls                    (rtx, rtx, rtx);
extern rtx        gen_extzvsi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_kflag                       (rtx);
extern rtx        gen_clri                        (rtx, rtx);
extern rtx        gen_ffs                         (rtx, rtx);
extern rtx        gen_ffs_f                       (rtx, rtx);
extern rtx        gen_fls                         (rtx, rtx);
extern rtx        gen_seti                        (rtx);
extern rtx        gen_stack_irq_dwarf             (void);
extern rtx        gen_stack_tie                   (rtx, rtx);
extern rtx        gen_cmpsfpx_raw                 (rtx, rtx);
extern rtx        gen_cmpdfpx_raw                 (rtx, rtx);
extern rtx        gen_dexcl_2op                   (rtx, rtx, rtx);
extern rtx        gen_adddf3_insn                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_muldf3_insn                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_subdf3_insn                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_fmasf4_fpu                  (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmasf4_fpu                 (rtx, rtx, rtx, rtx);
extern rtx        gen_fmadf4_split                (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmadf4_split               (rtx, rtx, rtx, rtx);
extern rtx        gen_fmadf4_fpu                  (rtx, rtx, rtx);
extern rtx        gen_fnmadf4_fpu                 (rtx, rtx, rtx);
extern rtx        gen_divsf3_fpu                  (rtx, rtx, rtx);
extern rtx        gen_sqrtsf2_fpu                 (rtx, rtx);
extern rtx        gen_divdf3                      (rtx, rtx, rtx);
extern rtx        gen_sqrtdf2                     (rtx, rtx);
extern rtx        gen_extendsfdf2                 (rtx, rtx);
extern rtx        gen_floatsidf2                  (rtx, rtx);
extern rtx        gen_floatunssidf2               (rtx, rtx);
extern rtx        gen_fixuns_truncsfdi2           (rtx, rtx);
extern rtx        gen_fix_truncsfdi2              (rtx, rtx);
extern rtx        gen_floatsisf2_fpu              (rtx, rtx);
extern rtx        gen_floatunssisf2               (rtx, rtx);
extern rtx        gen_fixuns_truncsfsi2           (rtx, rtx);
extern rtx        gen_fix_truncsfsi2_fpu          (rtx, rtx);
extern rtx        gen_floatdidf2                  (rtx, rtx);
extern rtx        gen_floatunsdidf2               (rtx, rtx);
extern rtx        gen_fixuns_truncdfdi2           (rtx, rtx);
extern rtx        gen_fix_truncdfdi2              (rtx, rtx);
extern rtx        gen_truncdfsf2                  (rtx, rtx);
extern rtx        gen_floatdisf2                  (rtx, rtx);
extern rtx        gen_floatunsdisf2               (rtx, rtx);
extern rtx        gen_fixuns_truncdfsi2           (rtx, rtx);
extern rtx        gen_fix_truncdfsi2              (rtx, rtx);
extern rtx        gen_vld128_insn                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vst128_insn                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vst64_insn                  (rtx, rtx, rtx, rtx);
extern rtx        gen_movv8hi_insn                (rtx, rtx);
extern rtx        gen_movti_insn                  (rtx, rtx);
extern rtx        gen_vaddaw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vaddw_insn                  (rtx, rtx, rtx);
extern rtx        gen_vavb_insn                   (rtx, rtx, rtx);
extern rtx        gen_vavrb_insn                  (rtx, rtx, rtx);
extern rtx        gen_vdifaw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vdifw_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmaxaw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmaxw_insn                  (rtx, rtx, rtx);
extern rtx        gen_vminaw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vminw_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmulaw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmulfaw_insn                (rtx, rtx, rtx);
extern rtx        gen_vmulfw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmulw_insn                  (rtx, rtx, rtx);
extern rtx        gen_vsubaw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vsubw_insn                  (rtx, rtx, rtx);
extern rtx        gen_vsummw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vand_insn                   (rtx, rtx, rtx);
extern rtx        gen_vandaw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vbic_insn                   (rtx, rtx, rtx);
extern rtx        gen_vbicaw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vor_insn                    (rtx, rtx, rtx);
extern rtx        gen_vxor_insn                   (rtx, rtx, rtx);
extern rtx        gen_vxoraw_insn                 (rtx, rtx, rtx);
extern rtx        gen_veqw_insn                   (rtx, rtx, rtx);
extern rtx        gen_vlew_insn                   (rtx, rtx, rtx);
extern rtx        gen_vltw_insn                   (rtx, rtx, rtx);
extern rtx        gen_vnew_insn                   (rtx, rtx, rtx);
extern rtx        gen_vmr1aw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmr1w_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmr2aw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmr2w_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmr3aw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmr3w_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmr4aw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmr4w_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmr5aw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmr5w_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmr6aw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmr6w_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmr7aw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmr7w_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmrb_insn                   (rtx, rtx, rtx);
extern rtx        gen_vh264f_insn                 (rtx, rtx, rtx);
extern rtx        gen_vh264ft_insn                (rtx, rtx, rtx);
extern rtx        gen_vh264fw_insn                (rtx, rtx, rtx);
extern rtx        gen_vvc1f_insn                  (rtx, rtx, rtx);
extern rtx        gen_vvc1ft_insn                 (rtx, rtx, rtx);
extern rtx        gen_vbaddw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vbmaxw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vbminw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vbmulaw_insn                (rtx, rtx, rtx);
extern rtx        gen_vbmulfw_insn                (rtx, rtx, rtx);
extern rtx        gen_vbmulw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vbrsubw_insn                (rtx, rtx, rtx);
extern rtx        gen_vbsubw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vasrrwi_insn                (rtx, rtx, rtx);
extern rtx        gen_vasrsrwi_insn               (rtx, rtx, rtx);
extern rtx        gen_vasrwi_insn                 (rtx, rtx, rtx);
extern rtx        gen_vasrpwbi_insn               (rtx, rtx, rtx);
extern rtx        gen_vasrrpwbi_insn              (rtx, rtx, rtx);
extern rtx        gen_vsr8awi_insn                (rtx, rtx, rtx);
extern rtx        gen_vsr8i_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmvaw_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmvw_insn                   (rtx, rtx, rtx);
extern rtx        gen_vmvzw_insn                  (rtx, rtx, rtx);
extern rtx        gen_vd6tapf_insn                (rtx, rtx, rtx);
extern rtx        gen_vmovaw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vmovw_insn                  (rtx, rtx, rtx);
extern rtx        gen_vmovzw_insn                 (rtx, rtx, rtx);
extern rtx        gen_vsr8_insn                   (rtx, rtx, rtx, rtx);
extern rtx        gen_vasrw_insn                  (rtx, rtx, rtx, rtx);
extern rtx        gen_vsr8aw_insn                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vabsaw_insn                 (rtx, rtx);
extern rtx        gen_vabsw_insn                  (rtx, rtx);
extern rtx        gen_vaddsuw_insn                (rtx, rtx);
extern rtx        gen_vsignw_insn                 (rtx, rtx);
extern rtx        gen_vexch1_insn                 (rtx, rtx);
extern rtx        gen_vexch2_insn                 (rtx, rtx);
extern rtx        gen_vexch4_insn                 (rtx, rtx);
extern rtx        gen_vupbaw_insn                 (rtx, rtx);
extern rtx        gen_vupbw_insn                  (rtx, rtx);
extern rtx        gen_vupsbaw_insn                (rtx, rtx);
extern rtx        gen_vupsbw_insn                 (rtx, rtx);
extern rtx        gen_vdirun_insn                 (rtx, rtx, rtx);
extern rtx        gen_vdorun_insn                 (rtx, rtx, rtx);
extern rtx        gen_vdiwr_insn                  (rtx, rtx);
extern rtx        gen_vdowr_insn                  (rtx, rtx);
extern rtx        gen_vrec_insn                   (rtx);
extern rtx        gen_vrun_insn                   (rtx);
extern rtx        gen_vrecrun_insn                (rtx);
extern rtx        gen_vendrec_insn                (rtx);
extern rtx        gen_vld32wh_insn                (rtx, rtx, rtx, rtx);
extern rtx        gen_vld32wl_insn                (rtx, rtx, rtx, rtx);
extern rtx        gen_vld64w_insn                 (rtx, rtx, rtx, rtx);
extern rtx        gen_vld64_insn                  (rtx, rtx, rtx, rtx);
extern rtx        gen_vld32_insn                  (rtx, rtx, rtx, rtx);
extern rtx        gen_vst16_n_insn                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vst32_n_insn                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vinti_insn                  (rtx);
extern rtx        gen_bswapv2hi2                  (rtx, rtx);
extern rtx        gen_addv2hi3                    (rtx, rtx, rtx);
extern rtx        gen_addv4hi3                    (rtx, rtx, rtx);
extern rtx        gen_addv2si3                    (rtx, rtx, rtx);
extern rtx        gen_subv2hi3                    (rtx, rtx, rtx);
extern rtx        gen_subv4hi3                    (rtx, rtx, rtx);
extern rtx        gen_subv2si3                    (rtx, rtx, rtx);
extern rtx        gen_addsubv2hi3                 (rtx, rtx, rtx);
extern rtx        gen_addsubv2si3                 (rtx, rtx, rtx);
extern rtx        gen_subaddv2hi3                 (rtx, rtx, rtx);
extern rtx        gen_subaddv2si3                 (rtx, rtx, rtx);
extern rtx        gen_addsubv4hi3                 (rtx, rtx, rtx);
extern rtx        gen_subaddv4hi3                 (rtx, rtx, rtx);
extern rtx        gen_dmpyh                       (rtx, rtx, rtx);
extern rtx        gen_dmpyhu                      (rtx, rtx, rtx);
extern rtx        gen_arc_vec_smult_lo_v4hi       (rtx, rtx, rtx);
extern rtx        gen_arc_vec_umult_lo_v4hi       (rtx, rtx, rtx);
extern rtx        gen_arc_vec_smultacc_lo_v4hi    (rtx, rtx);
extern rtx        gen_arc_vec_umultacc_lo_v4hi    (rtx, rtx);
extern rtx        gen_arc_vec_smult_hi_v4hi       (rtx, rtx, rtx);
extern rtx        gen_arc_vec_umult_hi_v4hi       (rtx, rtx, rtx);
extern rtx        gen_arc_vec_smac_hi_v4hi        (rtx, rtx, rtx);
extern rtx        gen_arc_vec_umac_hi_v4hi        (rtx, rtx, rtx);
extern rtx        gen_dmach                       (rtx, rtx, rtx);
extern rtx        gen_dmachu                      (rtx, rtx, rtx);
extern rtx        gen_dmacwh                      (rtx, rtx, rtx);
extern rtx        gen_dmacwhu                     (rtx, rtx, rtx);
extern rtx        gen_vmac2h                      (rtx, rtx, rtx);
extern rtx        gen_vmac2hu                     (rtx, rtx, rtx);
extern rtx        gen_vmpy2h                      (rtx, rtx, rtx);
extern rtx        gen_vmpy2hu                     (rtx, rtx, rtx);
extern rtx        gen_qmach                       (rtx, rtx, rtx);
extern rtx        gen_qmachu                      (rtx, rtx, rtx);
extern rtx        gen_qmpyh                       (rtx, rtx, rtx);
extern rtx        gen_qmpyhu                      (rtx, rtx, rtx);
extern rtx        gen_atomic_compare_and_swapsi_1 (rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_arc_load_exclusivesi        (rtx, rtx);
extern rtx        gen_arc_store_exclusivesi       (rtx, rtx);
extern rtx        gen_exchangesi                  (rtx, rtx, rtx);
extern rtx        gen_movqi                       (rtx, rtx);
extern rtx        gen_movhi                       (rtx, rtx);
extern rtx        gen_movsi                       (rtx, rtx);
extern rtx        gen_bic_f_zn                    (rtx, rtx, rtx);
extern rtx        gen_movdi                       (rtx, rtx);
extern rtx        gen_movsf                       (rtx, rtx);
extern rtx        gen_movdf                       (rtx, rtx);
extern rtx        gen_movsicc                     (rtx, rtx, rtx, rtx);
extern rtx        gen_movdicc                     (rtx, rtx, rtx, rtx);
extern rtx        gen_movsfcc                     (rtx, rtx, rtx, rtx);
extern rtx        gen_movdfcc                     (rtx, rtx, rtx, rtx);
extern rtx        gen_zero_extendqihi2            (rtx, rtx);
extern rtx        gen_zero_extendqisi2            (rtx, rtx);
extern rtx        gen_zero_extendhisi2            (rtx, rtx);
extern rtx        gen_extendqihi2                 (rtx, rtx);
extern rtx        gen_extendqisi2                 (rtx, rtx);
extern rtx        gen_extendhisi2                 (rtx, rtx);
extern rtx        gen_mulhisi3                    (rtx, rtx, rtx);
extern rtx        gen_umulhisi3                   (rtx, rtx, rtx);
extern rtx        gen_mulsi3                      (rtx, rtx, rtx);
extern rtx        gen_mulsidi3                    (rtx, rtx, rtx);
extern rtx        gen_umulsi3_highpart            (rtx, rtx, rtx);
extern rtx        gen_umulsidi3                   (rtx, rtx, rtx);
extern rtx        gen_addsi3                      (rtx, rtx, rtx);
extern rtx        gen_adddi3                      (rtx, rtx, rtx);
extern rtx        gen_subsi3                      (rtx, rtx, rtx);
extern rtx        gen_subdi3                      (rtx, rtx, rtx);
extern rtx        gen_andsi3                      (rtx, rtx, rtx);
extern rtx        gen_ashlsi3                     (rtx, rtx, rtx);
extern rtx        gen_ashrsi3                     (rtx, rtx, rtx);
extern rtx        gen_lshrsi3                     (rtx, rtx, rtx);
extern rtx        gen_cbranchsi4                  (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoresi4                   (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoresf4                   (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoredf4                   (rtx, rtx, rtx, rtx);
extern rtx        gen_scc_insn                    (rtx, rtx);
extern rtx        gen_branch_insn                 (rtx, rtx);
extern rtx        gen_jump                        (rtx);
extern rtx        gen_casesi                      (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_call                        (rtx, rtx);
extern rtx        gen_call_value                  (rtx, rtx, rtx);
extern rtx        gen_clzsi2                      (rtx, rtx);
extern rtx        gen_ctzsi2                      (rtx, rtx);
extern rtx        gen_sibcall                     (rtx, rtx, rtx);
extern rtx        gen_sibcall_value               (rtx, rtx, rtx, rtx);
extern rtx        gen_prologue                    (void);
extern rtx        gen_epilogue                    (void);
extern rtx        gen_sibcall_epilogue            (void);
extern rtx        gen_return                      (void);
extern rtx        gen_doloop_begin                (rtx, rtx);
extern rtx        gen_doloop_end                  (rtx, rtx);
extern rtx        gen_movmemsi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchsf4                  (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchdf4                  (rtx, rtx, rtx, rtx);
extern rtx        gen_cmp_float                   (rtx, rtx);
extern rtx        gen_get_thread_pointersi        (rtx);
extern rtx        gen_set_thread_pointersi        (rtx);
extern rtx        gen_prefetch                    (rtx, rtx, rtx);
extern rtx        gen_reload_qi_load              (rtx, rtx, rtx);
extern rtx        gen_reload_hi_load              (rtx, rtx, rtx);
extern rtx        gen_reload_qi_store             (rtx, rtx, rtx);
extern rtx        gen_reload_hi_store             (rtx, rtx, rtx);
extern rtx        gen_ffssi2                      (rtx, rtx);
extern rtx        gen_addsf3                      (rtx, rtx, rtx);
extern rtx        gen_subsf3                      (rtx, rtx, rtx);
extern rtx        gen_mulsf3                      (rtx, rtx, rtx);
extern rtx        gen_adddf3                      (rtx, rtx, rtx);
extern rtx        gen_subdf3                      (rtx, rtx, rtx);
extern rtx        gen_muldf3                      (rtx, rtx, rtx);
extern rtx        gen_divsf3                      (rtx, rtx, rtx);
extern rtx        gen_sqrtsf2                     (rtx, rtx);
extern rtx        gen_fix_truncsfsi2              (rtx, rtx);
extern rtx        gen_floatsisf2                  (rtx, rtx);
extern rtx        gen_extzv                       (rtx, rtx, rtx, rtx);
extern rtx        gen_insv                        (rtx, rtx, rtx, rtx);
extern rtx        gen_cmp_quark                   (rtx, rtx);
extern rtx        gen_fmasf4                      (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmasf4                     (rtx, rtx, rtx, rtx);
extern rtx        gen_fmadf4                      (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmadf4                     (rtx, rtx, rtx, rtx);
extern rtx        gen_movv8hi                     (rtx, rtx);
extern rtx        gen_movv2hi                     (rtx, rtx);
extern rtx        gen_movmisalignv2hi             (rtx, rtx);
extern rtx        gen_movv4hi                     (rtx, rtx);
extern rtx        gen_movv2si                     (rtx, rtx);
extern rtx        gen_movmisalignv4hi             (rtx, rtx);
extern rtx        gen_movmisalignv2si             (rtx, rtx);
extern rtx        gen_sdot_prodv2hi               (rtx, rtx, rtx, rtx);
extern rtx        gen_udot_prodv2hi               (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_lo_v4hi     (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_lo_v4hi     (rtx, rtx, rtx);
extern rtx        gen_vec_widen_smult_hi_v4hi     (rtx, rtx, rtx);
extern rtx        gen_vec_widen_umult_hi_v4hi     (rtx, rtx, rtx);
extern rtx        gen_memory_barrier              (void);
extern rtx        gen_atomic_compare_and_swapqi   (rtx, rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_compare_and_swaphi   (rtx, rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_compare_and_swapsi   (rtx, rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_exchangesi           (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_addsi                (rtx, rtx, rtx);
extern rtx        gen_atomic_subsi                (rtx, rtx, rtx);
extern rtx        gen_atomic_orsi                 (rtx, rtx, rtx);
extern rtx        gen_atomic_xorsi                (rtx, rtx, rtx);
extern rtx        gen_atomic_andsi                (rtx, rtx, rtx);
extern rtx        gen_atomic_nandsi               (rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_addsi          (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_subsi          (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_orsi           (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_xorsi          (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_andsi          (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_nandsi         (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_add_fetchsi          (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_sub_fetchsi          (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_or_fetchsi           (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_xor_fetchsi          (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_and_fetchsi          (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_nand_fetchsi         (rtx, rtx, rtx, rtx);

#endif /* GCC_INSN_FLAGS_H */
