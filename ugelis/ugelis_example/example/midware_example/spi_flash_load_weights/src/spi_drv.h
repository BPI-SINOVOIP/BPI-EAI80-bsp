#ifndef __SPI_DRV_H__
#define __SPI_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_SIZE_32M
//#define FLASH_SIZE_16M


#define FLASH_BASE_ADDR 0x10000000//0x00000000//  //Evan test for pyOCD
#define SUPPORT_FAST_OP
#ifdef FLASH_SIZE_32M
#define SUPPORT_4BYTE_ADDR
#endif

#define PAGE_LENGTH                     0x100
#define SECTOR_LENGTH                   0x1000


int Init(unsigned long adr, unsigned long clk, unsigned long fnc) ;
int UnInit(unsigned long fnc);
unsigned long Verify(unsigned long adr, unsigned long sz, unsigned char *buf);
int EraseChip(void);
int EraseSector(unsigned long adr);
int ProgramPage(unsigned long adr, unsigned long sz, unsigned char *buf);
unsigned long ReadPage(unsigned long adr, unsigned long sz, unsigned char *buf);



#endif


