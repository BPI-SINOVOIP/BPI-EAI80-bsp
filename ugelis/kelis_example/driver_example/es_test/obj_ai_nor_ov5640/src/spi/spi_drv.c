
#include "spi_drv.h"



typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;

/** FLASH Controller- SQI controller register structure */
typedef struct FLASH_MemMap
{
    volatile uint32_t       VS;                        /* SQI Version Register: 0x0 */
    volatile uint32_t       BR;                        /* SQI Baud Rate Register: 0x4 */
    volatile uint32_t       CFG;                       /* SQI Configuration Register 1: 0x8 */
    volatile uint32_t       DMY;                       /* SQI Dummy Register: 0xc */
    volatile uint32_t       CNT;                       /* SQI Counter: 0x10 */
    volatile uint32_t       ADDRCFG;                   /* SQI Address Configuration Register: 0x14 */
    volatile uint32_t       TCR;                       /* SQI Timing Control Register: 0x18 */
    volatile uint32_t       PREFCTRL;                  /* SQI Prefetch Control Register: 0x1C */
    volatile uint32_t       CRCCTRL;                   /* SQI CRC Control Register: 0x20 */
    volatile uint32_t       CRCIV;                     /* SQI CRC Initial Value Register: 0x24 */
    volatile uint32_t       CRCRES;                    /* SQI CRC Result Register: 0x28 */
} *FLASH_MemMapPtr;

/** SQI controller Map **/
#define FLASH                           ((FLASH_MemMapPtr)0x40001000u)
/** Add Watchdog in the future **/

#define M8(adr)                         (*((volatile unsigned char *)(adr)))
//#define M16(adr) (*((volatile unsigned short *)(adr)))

/*
MX25L12835F
Size:   16MB (0x01000000)
Sector: 4KB  (0x1000)
Page:   256B (0x100)
*/

static uint8_t     spi_flash_read_reg(void)
{
    uint8_t     reg = 0;
    FLASH->CFG = 0x905;
    //memcpy(&reg,(uint32_t *)FLASH_BASE_ADDR,1);
    reg = *(uint8_t *)FLASH_BASE_ADDR;
    return reg;
}

static  void wait_for_flash_idle()
{
    uint8_t rbuf;

    rbuf   = spi_flash_read_reg();
    while (rbuf & (0x1 << 0))
    {
        rbuf = spi_flash_read_reg();
    }
}

static void spi_flash_write_enable(void)
{
    FLASH->CFG = 0x806;
    (*(uint32_t *)FLASH_BASE_ADDR) = 0x0;//Just used to trig out the write enable command
}

static void spi_flash_read_enable(void)
{
    FLASH->CFG = 0xd03;
    (*(uint32_t *)FLASH_BASE_ADDR) = 0x0;//Just used to trig out the write enable command(need?)
}

static void spi_flash_io_setting(void)
{
    (*(uint32_t *)0x40000298) = 0xffffffff;
    (*(uint32_t *)0x4000029c) = 0xffffffff;
    (*(uint32_t *)0x400002a0) = 0xffffffff;
    (*(uint32_t *)0x400002a4) = 0xffffffff;
    (*(uint32_t *)0x400002a8) = 0xffffffff;
    (*(uint32_t *)0x400002ac) = 0xffffffff;
    (*(uint32_t *)0x400002b0) = 0xffffffff;
    (*(uint32_t *)0x400002b4) = 0xffffffff;
    (*(uint32_t *)0x400002b8) = 0xffffffff;
    (*(uint32_t *)0x400002bc) = 0xffffffff;
    (*(uint32_t *)0x400002c0) = 0xffffffff;
}




/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int Init(unsigned long adr, unsigned long clk, unsigned long fnc)
{
    *(volatile uint32_t *)(0x400002D8) &= ~((uint32_t)0xF << 16);
    *(volatile uint32_t *)(0x400002D8) |= ((uint32_t)0x5 << 16);

    *(volatile uint32_t *)(0x400002DC) &= ~((uint32_t)0x3F << 26);
    *(volatile uint32_t *)(0x400002DC) |= ((uint32_t)0x15 << 26);

    *(volatile uint32_t *)(0x400002E0) &= ~((uint32_t)0x3 << 0);
    *(volatile uint32_t *)(0x400002E0) |= ((uint32_t)0x1 << 0);

    *(volatile uint32_t *)(0x4000031C) &= ~((uint32_t)0xFF << 0);
    *(volatile uint32_t *)(0x4000031C) |= ((uint32_t)0xcc << 0);

    *(volatile uint32_t *)(0x40000324) &= ~((uint32_t)0xFFF << 20);
    *(volatile uint32_t *)(0x40000324) |= ((uint32_t)0xccc << 20);

    *(volatile uint32_t *)(0x40000328) &= ~((uint32_t)0xF << 0);
    *(volatile uint32_t *)(0x40000328) |= ((uint32_t)0xc << 0);
    FLASH->CFG  = 0xd03;

    spi_flash_io_setting();

    return (0);
}

/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit(unsigned long fnc)
{
    /* Add your Code */
    return (0);                                  // Finished without Errors
}

/*
 *  Verify Flash Contents
 *    Parameter:      adr:  Start Address
 *                    sz:   Size (in bytes)
 *                    buf:  Data
 *    Return Value:   (adr+sz) - OK, Failed Address
 */
unsigned long Verify(unsigned long adr, unsigned long sz, unsigned char *buf)
{
    int i;
    unsigned long b_adr = adr | FLASH_BASE_ADDR;

    for (i = 0; i < sz; i++)
    {
        FLASH->CFG  = 0xd03;
        if (M8(b_adr) != (*(volatile unsigned char *)buf))
        {
            return (0);
        }

        buf   += 1;
        b_adr += 1;
    }

    return (adr + sz);                                // Finished without Errors
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip(void)
{
    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();
    FLASH->CFG = 0x860;
    (*(uint32_t *)FLASH_BASE_ADDR) = 0x0;
    wait_for_flash_idle();

    spi_flash_read_enable();//change back to read status
    return (0);
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseSector(unsigned long adr)
{
    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();
    FLASH->CFG = 0xc20;
    (*(uint32_t *)(FLASH_BASE_ADDR | adr)) = 0x0;
    wait_for_flash_idle();

    spi_flash_read_enable();//change back to read status
    return (0);
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage(unsigned long adr, unsigned long sz, unsigned char *buf)
{
    int isz = 0;

#if 0
    if (((FLASH_BASE_ADDR | adr) &  0xFFF != 0) || ((sz % SECTOR_LENGTH) != 0))
    {
        return (1);
    }
#endif

    for (isz = 0; isz < sz; ++isz)
    {
        wait_for_flash_idle();
        spi_flash_write_enable();
        wait_for_flash_idle();
        FLASH->CFG = 0xd02;

        (*(uint8_t *)((FLASH_BASE_ADDR | adr) + isz)) = buf[isz];

        wait_for_flash_idle();
    }

    spi_flash_read_enable();//change back to read status
    return (0);                                  // Finished without Errors
}


/*
 *  Read Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

unsigned long ReadPage(unsigned long adr, unsigned long sz, unsigned char *buf)
{
    int i;
    unsigned long b_adr = adr | FLASH_BASE_ADDR;

    for (i = 0; i < sz; i++)
    {
        FLASH->CFG  = 0xd03;
        *(volatile unsigned char *)buf = M8(b_adr);

        buf   += 1;
        b_adr += 1;
    }

    return (sz);                                  // Finished without Errors
}

