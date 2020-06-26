
#include "spi_drv.h"



//   Read/Write Array Commands
#define MX25R_READ_CMD                    0x03        //normal read
#define MX25R_FAST_READ_CMD               0x0B         //fast read data
#define MX25R_2READ_CMD                   0xBB        //  2 x I/O read command
#define MX25R_DREAD_CMD                   0x3B         //1I / 2O read command
#define MX25R_4READ_CMD                   0xEB         //4xI/O read
#define MX25R_QREAD_CMD                   0x6B         //1I / 4O read command

#define MX25R_PP_CMD                      0x02        //page program
#define MX25R_4PP_CMD                     0x38         //quad page program 
#define MX25R_SE_CMD                      0x20         //sector erase
#define MX25R_BE_32K_CMD                  0x52         //block erase 32KB 
#define MX25R_BE_CMD                      0xD8        //block erase 64KB 

#define MX25R_CE_CMD                      0x60//0xC7   //chip erase
#define MX25R_RDSFDP_CMD                  0x5A         //Read SFDP mode

//  Register/Setting Commands
#define MX25R_WREN_CMD                    0x06        //write enable
#define MX25R_WRDI_CMD                    0x04        //write disable
#define MX25R_RDSR_CMD                    0x05        //read status register
#define MX25R_RDCR_CMD                    0x15        //read configuration register
#define MX25R_WRSR_CMD                    0x01        //write status register
#define MX25R_PGM_ERS_SPD_CMD             0x75        // 0xB0  //Suspends Program/Erase
#define MX25R_PGM_ERS_RSM_CMD             0x7A// 0x30  //ResumesProgram/Erase
#define MX25R_DP_CMD                      0xB9        //Deep power down
#define MX25R_SBL_CMD                     0xC0        //Set Burst Length
#define MX25R_EN4B_CMD                    0xB7        //Deep power down
#define MX25R_EX4B_CMD                    0xE9        //Set Burst Length

//Define Dummy Byte
#define MX25R_DUMMY_BYTE 0xA5

// ID/Reset Commands
#define MX25R_RDID_CMD                    0x9F        //read identification
#define MX25R_RES_CMD                     0xAB        //read electronic ID
#define MX25R_REMS_CMD                    0x90        //read electronic manufacturer& device ID
#define MX25R_ENSO_CMD                    0xB1        //enter secured OTP
#define MX25R_EXSO_CMD                    0xC1        //exit secured OTP
#define MX25R_RDSCUR_CMD                  0x2B        //read security register
#define MX25R_WRSCUR_CMD                  0x2F        //write security register
#define MX25R_NOP_CMD                     0x00        //No Operation
#define MX25R_RSTEN_CMD                   0x66        //Reset Enable
#define MX25R_RST_CMD                     0x99        //Reset Memory

/* relevant status register bits */
#define MX25RXXDV_WIP_ENABLE              (0x1 << 0)
#define MX25RXXDV_WRITE_LAUTCH_ENABLE     (0x1 << 1)
#define MX25RXXDV_QUAD_ENABLE             (0x1 << 6)
#define MX25RXXDV_SRWD_ENABLE             (0x1 << 7)

/*Used for flash Erase*/
#define MX25RXXDV_SECTOR_LEN              0x1000     //4KB
#define MX25RXXDV_BLOCK32_LEN             0x8000     //32KB
#define MX25RXXDV_BLOCK_LEN               0x10000    //64KB
#define MX25RXXDV_PAGE_LEN                0x100      //256B
#define MX25RXXDV_ALL_FLASH_LEN           0x400000   //4MB
#define MX25RXXDV_SECTOR_MASK             (0xFFF)
#define MX25RXXDV_PAGE_MASK               (0xFF)

typedef enum
{
    MOD0 = 0,
    MOD1 = 1,
    MOD2 = 2,
    MOD3 = 3,
    MOD4 = 4,
    MOD5 = 5,
    MOD6 = 6
} SQI_RW_IO_MODE_T;


/*config register*/


typedef unsigned char       uint8_t;
typedef unsigned short int  uint16_t;
typedef unsigned int        uint32_t;


/** FLASH Controller- SQI controller register structure */
typedef struct FLASH_MemMap
{
    volatile uint32_t VS;                        /* SQI Version Register: 0x0 */
    volatile uint32_t BR;                        /* SQI Baud Rate Register: 0x4 */
    volatile uint32_t CFG;                       /* SQI Configuration Register 1: 0x8 */
    volatile uint32_t DMY;                       /* SQI Dummy Register: 0xc */
    volatile uint32_t CNT;                       /* SQI Counter: 0x10 */
    volatile uint32_t ADDRCFG;                   /* SQI Address Configuration Register: 0x14 */
    volatile uint32_t TCR;                       /* SQI Timing Control Register: 0x18 */
    volatile uint32_t PREFCTRL;                  /* SQI Prefetch Control Register: 0x1C */
    volatile uint32_t CRCCTRL;                   /* SQI CRC Control Register: 0x20 */
    volatile uint32_t CRCIV;                     /* SQI CRC Initial Value Register: 0x24 */
    volatile uint32_t CRCRES;                    /* SQI CRC Result Register: 0x28 */
} *FLASH_MemMapPtr;

/** SQI controller Map **/
#define FLASH                           ((FLASH_MemMapPtr)0x40001000u)
/** Add Watchdog in the future **/

#define M8(adr)  (*((volatile unsigned char *)(adr)))
//#define M16(adr) (*((volatile unsigned short *)(adr)))



static void *memcpy(void *dest, void *src, unsigned count)
{
#if 0
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }
#endif

    char *pdest = (char *) dest;
    char *psrc = (char *)src;
    while (count--)
    {
        *pdest++ = *psrc++;
    }
    return dest;
}

static uint8_t     spi_flash_read_reg(void)
{
    uint8_t reg = 0;

    FLASH->CFG = 0x905;
    //memcpy(&reg,(uint32_t *)FLASH_BASE_ADDR,1);
    reg = *(uint8_t *)FLASH_BASE_ADDR;
    return reg;
}

static uint8_t     spi_flash_read_cfg_reg(void)
{
    uint8_t reg = 0;

    FLASH->CFG = 0x915;
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



static uint8_t     spi_flash_write_reg(uint8_t regv)
{
    uint8_t reg = 0;

    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();

    FLASH->CFG = 0x901;
    (*(uint8_t *)FLASH_BASE_ADDR) = regv; //trig to send out the command and value

    return reg;
}

static  void enter_to_4BMode()
{
    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();

    FLASH->CFG = 0x8B7;
    (*(uint32_t *)FLASH_BASE_ADDR) = 0x0;//Just used to trig out the write enable command
    wait_for_flash_idle();
}

static  void exit_from_4BMode()
{
    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();

    FLASH->CFG = 0x8E9;
    (*(uint32_t *)FLASH_BASE_ADDR) = 0x0;//Just used to trig out the write enable command
    wait_for_flash_idle();
}

static void spi_flash_read_enable(void)
{
#ifdef SUPPORT_4BYTE_ADDR
    //enter_to_4BMode();
    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();

    FLASH->CFG = 0x80d13;
    (*(uint32_t *)FLASH_BASE_ADDR) = 0x0;//Just used to trig out the write enable command(need?)

    //exit_from_4BMode();
#else
    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();

    FLASH->CFG = 0xd03;
    (*(uint32_t *)FLASH_BASE_ADDR) = 0x0;//Just used to trig out the write enable command(need?)
#endif
}


static void system_clock_init()
{
#if 0
    if (SYS_CLOCK == 8000000)
    {
        *(volatile uint32_t *)0x40000170 |= 0x01;  //Enable HIRC
        while (!(*(volatile uint32_t *)0x400001a4 & (0x1 << 0))); //Wait HIRC stable
        *(volatile uint32_t *)0x40000090 = 0x0;
    }
    else if (SYS_CLOCK == 133000000)
#endif
    {
        *(volatile uint32_t *)0x400001a0 &= ~0x3;
        *(volatile uint32_t *)0x400001a0 |= 0x0;     //PLL reference clock HIRC

        *(volatile uint32_t *)0x40000188 |= 0x01;  //Enable PLLKDP
        while (!(*(volatile uint32_t *)0x400001a4 & (0x1 << 5))); //Wait PLLKDP stable

        *(volatile uint32_t *)0x40000090 &= ~(0x7 << 0);
        *(volatile uint32_t *)0x40000090 |= (0x4 << 0);     //switch CPU clock to PLLKDP   133MHZ

        *(volatile uint32_t *)0x2001F000 = 15;      //test
    }
}

static void pinmux_init()
{
    /* set flash PINMUX */
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
    /* init system clock */
    system_clock_init();

    /* set flash PINMUX */
    pinmux_init();

    FLASH->CFG  = 0xd03;

    enter_to_4BMode();

    spi_flash_read_enable();//change back to read status

    return (0);
}

/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit(unsigned long fnc)
{
    /* init system clock */
    system_clock_init();

    exit_from_4BMode();

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

    /* init system clock */
    system_clock_init();


#ifdef SUPPORT_4BYTE_ADDR
    //enter_to_4BMode();
    for (i = 0; i < sz; i++)
    {
        FLASH->CFG  = 0x80d13;
        if (M8(b_adr) != (*(volatile unsigned char *)buf))
        {
            return (0);
        }

        buf   += 1;
        b_adr += 1;
    }
    //exit_to_4BMode();
#else
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
#endif

    return (adr + sz);                                // Finished without Errors
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip(void)
{
    /* init system clock */
    system_clock_init();

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
    /* init system clock */
    system_clock_init();

#ifdef SUPPORT_4BYTE_ADDR
    //enter_to_4BMode();
    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();
    FLASH->CFG = 0x80c21;
    (*(uint32_t *)(FLASH_BASE_ADDR | adr)) = 0x0;
    wait_for_flash_idle();
    //exit_to_4BMode();
#else
    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();
    FLASH->CFG = 0xc20;
    (*(uint32_t *)(FLASH_BASE_ADDR | adr)) = 0x0;
    wait_for_flash_idle();
#endif

    spi_flash_read_enable();//change back to read status
    return (0);
}

int Slow_ProgramPage(unsigned long addr, unsigned long size, unsigned char *buffer)
{
    int isize = 0;

#ifdef SUPPORT_4BYTE_ADDR
    //enter_to_4BMode();
    for (isize = 0; isize < size; ++isize)
    {
        wait_for_flash_idle();
        spi_flash_write_enable();
        wait_for_flash_idle();
        FLASH->CFG = 0x80d12;

        (*(uint8_t *)((FLASH_BASE_ADDR | addr) + isize)) = buffer[isize];

        wait_for_flash_idle();
    }
    //exit_to_4BMode();
#else
    for (isize = 0; isize < size; ++isize)
    {
        wait_for_flash_idle();
        spi_flash_write_enable();
        wait_for_flash_idle();
        FLASH->CFG = 0x80d02;

        (*(uint8_t *)((FLASH_BASE_ADDR | addr) + isize)) = buffer[isize];

        wait_for_flash_idle();
    }
#endif

    return (0);                                  // Finished without Errors
}

int Fast_ProgramPage(unsigned long addr, unsigned long size, unsigned char *buffer)
{
    int inum = 0;
    int page_num = 0;
    int page_off = 0;
    int addr_offset;
    int buffer_offset = 0;

    if ((addr & MX25RXXDV_PAGE_MASK) != 0)
    {
        addr = addr & (0xFFFFFF00);//Page align
    }

#ifdef SUPPORT_4BYTE_ADDR
    //enter_to_4BMode();
    spi_flash_read_cfg_reg();

    addr_offset = addr;

    page_num = size / MX25RXXDV_PAGE_LEN;
    page_off = size % MX25RXXDV_PAGE_LEN;

    for (inum = 0; inum < page_num; inum++)
    {
        spi_flash_write_reg(MX25RXXDV_QUAD_ENABLE);

        wait_for_flash_idle();
        spi_flash_write_enable();
        wait_for_flash_idle();

        FLASH->CFG = 0x02bcd3e;
        FLASH->CNT = MX25RXXDV_PAGE_LEN;

        memcpy((unsigned char *)(FLASH_BASE_ADDR | addr_offset), buffer + buffer_offset, MX25RXXDV_PAGE_LEN);
        //for(isize = 0;isize < size;++isize)
        //{
        //  (*(uint8_t *)((FLASH_BASE_ADDR | addr) + isize)) = buffer[isize];
        //}

        wait_for_flash_idle();

        addr_offset     += MX25RXXDV_PAGE_LEN;
        buffer_offset   += MX25RXXDV_PAGE_LEN;
    }

    spi_flash_write_reg(MX25RXXDV_QUAD_ENABLE);

    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();

    FLASH->CFG = 0x02bcd3e;
    FLASH->CNT = page_off;

    memcpy((unsigned char *)(FLASH_BASE_ADDR | addr_offset), buffer + buffer_offset, page_off);
    //for(isize = 0;isize < size;++isize)
    //{
    //  (*(uint8_t *)((FLASH_BASE_ADDR | addr) + isize)) = buffer[isize];
    //}

    wait_for_flash_idle();

    //exit_to_4BMode();
#else
    addr_offset = addr;

    page_num = size / MX25RXXDV_PAGE_LEN;
    page_off = size % MX25RXXDV_PAGE_LEN;

    for (inum = 0; inum < page_num; inum++)
    {
        spi_flash_write_reg(MX25RXXDV_QUAD_ENABLE);

        wait_for_flash_idle();
        spi_flash_write_enable();
        wait_for_flash_idle();

        FLASH->CFG = 0x023cd38;
        FLASH->CNT = MX25RXXDV_PAGE_LEN;

        memcpy((unsigned char *)(FLASH_BASE_ADDR | addr_offset), buffer + buffer_offset, MX25RXXDV_PAGE_LEN);
        //for(isize = 0;isize < size;++isize)
        //{
        //  (*(uint8_t *)((FLASH_BASE_ADDR | addr) + isize)) = buffer[isize];
        //}

        wait_for_flash_idle();

        addr_offset     += MX25RXXDV_PAGE_LEN;
        buffer_offset   += MX25RXXDV_PAGE_LEN;
    }

    spi_flash_write_reg(MX25RXXDV_QUAD_ENABLE);

    wait_for_flash_idle();
    spi_flash_write_enable();
    wait_for_flash_idle();

    FLASH->CFG = 0x02bcd38;
    FLASH->CNT = page_off;

    memcpy((unsigned char *)(FLASH_BASE_ADDR | addr_offset), buffer + buffer_offset, page_off);
    //for(isize = 0;isize < size;++isize)
    //{
    //  (*(uint8_t *)((FLASH_BASE_ADDR | addr) + isize)) = buffer[isize];
    //}

    wait_for_flash_idle();
#endif
    return 0;
}


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage(unsigned long addr, unsigned long size, unsigned char *buffer)
{
    int ret = 0;

    /* init system clock */
    system_clock_init();

#if 0
    if (((FLASH_BASE_ADDR | addr) &  0xFFF != 0) || ((size % SECTOR_LENGTH) != 0))
    {
        return (1);
    }
#endif

#ifdef SUPPORT_FAST_OP
    ret = Fast_ProgramPage(addr, size, buffer);
#else
    ret = Slow_ProgramPage(addr, size, buffer);
#endif

    spi_flash_read_enable();//change back to read status
    return ret;                                  // Finished without Errors
}


/**************************************************/
unsigned long ReadPage(unsigned long adr, unsigned long sz, unsigned char *buf)
{
    int i;
    unsigned long b_adr = adr | FLASH_BASE_ADDR;

    for (i = 0; i < sz; i++)
    {
#ifdef SUPPORT_4BYTE_ADDR
        FLASH->CFG  = 0x80d13;
#else
        FLASH->CFG  = 0xd03;
#endif
        *(volatile unsigned char *)buf = M8(b_adr);

        buf   += 1;
        b_adr += 1;
    }

    return (sz);                                  // Finished without Errors
}
