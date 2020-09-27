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

_CODE_BASE_ADDR           	EQU  0x00000000
_CODE_END_ADDR            	EQU  0x1FFFFFFF
_SRAM_BASE_ADDR           	EQU  0x20000000
_SRAM_BIT_BAND_REGION     	EQU  0x20000000
_SRAM_BIT_BAND_REGION_END 	EQU  0x200FFFFF
_SRAM_BIT_BAND_ALIAS      	EQU  0x22000000
_SRAM_BIT_BAND_ALIAS_END  	EQU  0x23FFFFFF
_SRAM_END_ADDR            	EQU  0x3FFFFFFF

;0x40000000 -> 0x5fffffff: Peripherals [0.5GB]
_PERI_BASE_ADDR           	EQU 0x40000000
_PERI_BIT_BAND_REGION     	EQU 0x40000000
_PERI_BIT_BAND_REGION_END 	EQU 0x400FFFFF
_PERI_BIT_BAND_ALIAS      	EQU 0x42000000
_PERI_BIT_BAND_ALIAS_END  	EQU 0x43FFFFFF
_PERI_END_ADDR            	EQU 0x5FFFFFFF

;0x60000000 -> 0x9fffffff: external RAM [1GB] 
_ERAM_BASE_ADDR            EQU 0x60000000
_ERAM_END_ADDR             EQU 0x9FFFFFFF

; 0xa0000000 -> 0xdfffffff: external devices [1GB]
_EDEV_BASE_ADDR            EQU 0xA0000000
_EDEV_END_ADDR             EQU 0xDFFFFFFF
_PPB_INT_ITM               EQU 0xE0000000
_PPB_INT_DWT               EQU 0xE0001000
_PPB_INT_FPB               EQU 0xE0002000
_PPB_INT_RSVD_1            EQU 0xE0003000
_PPB_INT_SCS               EQU 0xE000E000
_PPB_INT_RSVD_2            EQU 0xE000F000   


_PPB_INT_END_ADDR         EQU 0xE003FFFF          
                                                      
; 0xe0000000 -> 0xe00fffff: private peripheral bus
; 0xe0040000 -> 0xe00fffff: external [768K]
_PPB_EXT_BASE_ADDR        EQU 0xE0040000  
_PPB_EXT_TPIU             EQU 0xE0040000        
_PPB_EXT_ETM              EQU 0xE0041000
_PPB_EXT_PPB              EQU 0xE0042000
_PPB_EXT_ROM_TABLE        EQU 0xE00FF000
_PPB_EXT_END_ADDR         EQU 0xE00FFFFF                     
                                                                     
; 0xe0100000 -> 0xffffffff: vendor-specific [0.5GB-1MB or 511MB]
_VENDOR_BASE_ADDR         EQU 0xE0100000                         
_VENDOR_END_ADDR          EQU 0xFFFFFFFF  

_SCS_BASE_ADDR 			  EQU  _PPB_INT_SCS
_SCS_ICSR 				  EQU  (_SCS_BASE_ADDR + 0xd04)
_SCS_ICSR_PENDSV 		  EQU  (1 << 28)
_SCS_ICSR_UNPENDSV 		  EQU  (1 << 27)
_SCS_ICSR_RETTOBASE       EQU  (1 << 11)  

	END