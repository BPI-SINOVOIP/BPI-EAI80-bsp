#ifndef __GM_IO_H__
#define __GM_IO_H__
#ifdef __cplusplus
extern "C" {
#endif

#include<stdint.h>
#include<stdio.h>
#include<gm_basic_types.h>


#define IO_BIT_GET(reg, bit)    ((reg) & (bit))
#define IO_BIT_SET(reg, bit)    (reg)|=(bit)
#define IO_BIT_CLEAR(reg, bit)  (reg)&=~(bit)
#define IO_BIT_TOGGLE(reg, bit) (reg)^=(bit)
#define IO_BITS_GET(reg, bits)  ((reg)&(bits))
#define IO_BITS_SET(reg, bits)  (reg)|=(bits)
#define IO_WRITE_REG(reg, val)  ((reg) = (val))
#define IO_READ_REG(reg)        ((reg))
#define IO_BITMASK_SET(reg, bitmask, setmask)    (reg) = (((reg) & (~(bitmask))) | (setmask))

#define IO_READB(addr)          *((const volatile uint8_t *) (addr))
#define IO_READW(addr)          *((const volatile uint16_t *) (addr))
#define IO_READL(addr)          *((const volatile uint32_t *) (addr))
#define IO_WRITEB(val,addr)     *(volatile uint8_t *) (addr) = val
#define IO_WRITEW(val,addr)     *(volatile uint16_t *) (addr) = val
#define IO_WRITEL(val,addr)     *(volatile uint32_t *) (addr) = val

#ifdef __cplusplus
}
#endif
#endif /* __GM_IO_H__ */

