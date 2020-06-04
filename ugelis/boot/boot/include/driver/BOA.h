#ifndef __BOA_H
#define __BOA_H //Bit Operation Accelerator
#ifdef __cplusplus
extern "C" {
#endif

#define IO_BIT_GET(Reg, Bit)    ((Reg)&(Bit))
#define IO_BIT_SET(Reg, Bit)    (Reg)|=(Bit) 
#define IO_BIT_CLEAR(Reg, Bit)  (Reg)&=~(Bit)
#define IO_BIT_TOGGLE(Reg, Bit) (Reg)^=(Bit)
#define IO_BITS_GET(Reg, Bits)  ((Reg)&(Bits))
#define IO_BITS_SET(Reg, Bits)  (Reg)|=(Bits)    
#define IO_BITMASK_SET(Reg, Bitmask, Setmask)    (Reg)=(((Reg) & (~(Bitmask))) | (Setmask))

#if 0
//bitband
#define BYTE_BIT(Byte, BitPos) ((uint32_t)(&Byte) - 0x20000000)*32 + 0x22000000)
#endif
/*
BOA_FIELD_INSERT(Field, Val)

*/


#ifdef __cplusplus
}
#endif
#endif /* __BOA_H */


