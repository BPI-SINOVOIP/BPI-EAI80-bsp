

    
    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8



pm_enter_soc_low_power_asm    PROC
    EXPORT pm_enter_soc_low_power_asm
    mov r1 , lr
    IMPORT pm_enter_soc_low_power
    bl pm_enter_soc_low_power
    ;/*cpu must not run here*/
    b .
    ENDP


	ALIGN   4

    END