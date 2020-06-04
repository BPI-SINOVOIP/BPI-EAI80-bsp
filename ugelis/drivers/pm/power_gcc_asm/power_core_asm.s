
#include <toolchain.h>
#include <linker/sections.h>
#include <arch/cpu.h>
_ASM_FILE_PROLOGUE
GTEXT(pm_enter_soc_low_power_asm)


SECTION_FUNC(TEXT, pm_enter_soc_low_power_asm)
    /*r1: uint32_t entry*/
    mov r1 , lr
    bl pm_enter_soc_low_power
    /*cpu must not run here*/
bug:
    b bug

