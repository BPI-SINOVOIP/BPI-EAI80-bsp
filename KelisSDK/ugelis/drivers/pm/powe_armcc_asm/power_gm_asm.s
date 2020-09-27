	
    GET		..\..\..\arch\arm\core-armcc\inc\config.inc
	GET		..\..\..\arch\arm\core-armcc\inc\board.inc
     
    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

pm_save_current_cpu_contex_asm    PROC
    EXPORT pm_save_current_cpu_contex_asm
        
    push {r0}
    ;/*save r1~r12,s0~s31,fpscr*/
    ldr r0,=0xc0000014
    stmea r0!,{r1-r12}
    vstmia r0!, {s0-s31}
    vmrs r1,fpscr
    str r1,[r0]

    ;/*save psr*/
    ldr r0,=0xc0000000
    mrs r1,psr
    mov r2,#0x01
    lsl r2,r2,#24
    orr r1,r2
    stmea r0!,{r1}

    ;/* save boot1 callback func address
    ; * address = 0xc0000004
    ; * alread be done in function:pm_gm_set_sys_resume_entry
    ; */

    ;//ldr r1,=boot_rusume_func
    ;//ldr r2,[r1]
    ;//stmea r0!,{r2}
    add r0,#4

    ;/*save psp*/
    mov r1,r13
    add r1,#4
    stmea r0!,{r1}

    ;/*save enterstandby address+4*/
    mov r1,lr   ;//restore resume from standby ugelis back addr
    add r1,#6
    stmea r0!,{r1}

   ; /*save r0*/
    mov r2,r0
    pop {r0}
    str r0,[r2]

    bx lr

    ENDP
    
pm_gm_resume_cpu_contex_asm    PROC
    EXPORT pm_gm_resume_cpu_contex_asm
    IMPORT _interrupt_stack
    IMPORT  _interrupt_stack
    ;IMPORT CONFIG_ISR_STACK_SIZE
    ldr r0,=_interrupt_stack
    ldr r1,=CONFIG_ISR_STACK_SIZE
    add r0,r1
    msr msp, r0

    ;/*switch psp*/
    msr PSP, r0
    movs.n r0, #2   ;/* switch to using PSP (bit1 of CONTROL reg) */
    msr CONTROL, r0
    isb

    ;/*resume psp*/
    ldr r0, = 0xc0000008
    ldmia r0!,{r1}
    msr PSP, r1

   ; /*resume lr*/
   ; //ldmia r0!,{r1}
    ldmia r0!,{r1}
    mov lr,r1
    add r0,#4
   ; /*resume r1~r12,s0~s31,fpsr*/
    ldmia r0!,{r1-r12}
    vldmia r0!, {s0-s31}
    push {r1-r2}
    ldmia r0!,{r1}
    vmsr fpscr,r1

    ;/*resume r0*/
    ldr r1,=0xc0000010
    ldr r0,[r1]

    ldr r1,=0xc0000000
    ldr r2,[r1]
    msr psr,r2

    pop {r1-r2}
    bx lr

    ENDP
        
	ALIGN   4

    END

