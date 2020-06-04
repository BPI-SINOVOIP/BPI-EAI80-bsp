;
; @file
; @brief Default basic NMI handler before the kernel is up
;
; Provide a default handler for NMI before the system is up. The default action
; is to hard hang, sleeping.
;
; This might be preferable than rebooting to help debugging, or because
; rebooting might trigger the exact same problem over and over.
;

;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************
	EXPORT _SysNmiOnReset[CODE]

;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
	AREA |.text|, CODE, READONLY, ALIGN=4
		THUMB
		REQUIRE8
		PRESERVE8

;******************************************************************************
; Functions:
;     size_t _SysNmiOnReset(void);
;     void _SysNmiOnReset(size_t cpsr);
;******************************************************************************
_SysNmiOnReset
    WFI
    B _SysNmiOnReset
	
	ALIGN
    END
