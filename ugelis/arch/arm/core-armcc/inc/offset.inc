;
; @file
; @brief Definitions for the boot vector table
;
;
; Definitions for the boot vector table.
;
; System exception handler names all have the same format:
;
;   __<exception name with underscores>
;
; No other symbol has the same format, so they are easy to spot.
;

___kernel_t_current_OFFSET 					EQU 	0x8
___kernel_t_nested_OFFSET 					EQU 	0x0
___kernel_t_irq_stack_OFFSET 				EQU 	0x4
___kernel_t_idle_OFFSET						EQU		0xc
___cpu_t_current_OFFSET 					EQU 	0x8
___cpu_t_nested_OFFSET 						EQU 	0x0
___cpu_t_irq_stack_OFFSET 					EQU 	0x4
___kernel_t_ready_q_OFFSET 					EQU 	0x1c
___kernel_t_arch_OFFSET 					EQU 	0x128
___ready_q_t_cache_OFFSET 					EQU 	0x0
___kernel_t_current_fp_OFFSET 				EQU 	0x124
___thread_base_t_user_options_OFFSET 		EQU 	0x8
___thread_base_t_thread_state_OFFSET 		EQU 	0x9
___thread_base_t_prio_OFFSET 				EQU 	0xa
___thread_base_t_sched_locked_OFFSET 		EQU 	0xb
___thread_base_t_preempt_OFFSET 			EQU 	0xa
___thread_base_t_swap_data_OFFSET 			EQU 	0x10
___thread_t_base_OFFSET 					EQU 	0x0
___thread_t_caller_saved_OFFSET 			EQU 	0x2c
___thread_t_callee_saved_OFFSET 			EQU 	0x2c
___thread_t_arch_OFFSET 					EQU 	0x60
K_THREAD_SIZEOF 							EQU 	0xa8
___thread_arch_t_basepri_OFFSET 			EQU 	0x0
___thread_arch_t_swap_return_value_OFFSET 	EQU 	0x4
___thread_arch_t_preempt_float_OFFSET 		EQU 	0x8
___esf_t_a1_OFFSET 							EQU 	0x0
___esf_t_a2_OFFSET 							EQU 	0x4
___esf_t_a3_OFFSET 							EQU 	0x8
___esf_t_a4_OFFSET 							EQU 	0xc
___esf_t_ip_OFFSET 							EQU 	0x10
___esf_t_lr_OFFSET 							EQU 	0x14
___esf_t_pc_OFFSET 							EQU 	0x18
___esf_t_xpsr_OFFSET 						EQU 	0x1c
___esf_t_s_OFFSET 							EQU 	0x20
___esf_t_fpscr_OFFSET 						EQU 	0x60
___esf_t_SIZEOF 							EQU 	0x68
___callee_saved_t_v1_OFFSET	 				EQU 	0x0
___callee_saved_t_v2_OFFSET 				EQU 	0x4
___callee_saved_t_v3_OFFSET 				EQU 	0x8
___callee_saved_t_v4_OFFSET 				EQU 	0xc
___callee_saved_t_v5_OFFSET 				EQU 	0x10
___callee_saved_t_v6_OFFSET 				EQU 	0x14
___callee_saved_t_v7_OFFSET 				EQU 	0x18
___callee_saved_t_v8_OFFSET 				EQU 	0x1c
___callee_saved_t_psp_OFFSET 				EQU 	0x20
___callee_saved_t_SIZEOF 					EQU 	0x24
_K_THREAD_NO_FLOAT_SIZEOF 					EQU 	0x68
_kernel_offset_to_nested 					EQU 	(___kernel_t_nested_OFFSET)
_kernel_offset_to_irq_stack 				EQU 	(___kernel_t_irq_stack_OFFSET)
_kernel_offset_to_current 					EQU 	(___kernel_t_current_OFFSET)
_kernel_offset_to_idle 						EQU 	(___kernel_t_idle_OFFSET)
_kernel_offset_to_current_fp 				EQU 	(___kernel_t_current_fp_OFFSET)
_kernel_offset_to_ready_q_cache 			EQU 	(___kernel_t_ready_q_OFFSET + ___ready_q_t_cache_OFFSET)
_thread_offset_to_basepri                   EQU     (___thread_t_arch_OFFSET + ___thread_arch_t_basepri_OFFSET)
_thread_offset_to_swap_return_value         EQU     (___thread_t_arch_OFFSET + ___thread_arch_t_swap_return_value_OFFSET)
_thread_offset_to_preempt_float             EQU     (___thread_t_arch_OFFSET + ___thread_arch_t_preempt_float_OFFSET)
_thread_offset_to_callee_saved              EQU     (___thread_t_callee_saved_OFFSET)
_thread_offset_to_thread_state 				EQU     (___thread_t_base_OFFSET + ___thread_base_t_thread_state_OFFSET)
_thread_offset_to_user_options 				EQU     (___thread_t_base_OFFSET + ___thread_base_t_user_options_OFFSET)
_thread_offset_to_prio 						EQU     (___thread_t_base_OFFSET + ___thread_base_t_prio_OFFSET)
_thread_offset_to_sched_locked 				EQU     (___thread_t_base_OFFSET + ___thread_base_t_sched_locked_OFFSET)
_thread_offset_to_preempt 					EQU     (___thread_t_base_OFFSET + ___thread_base_t_preempt_OFFSET)
	
	END