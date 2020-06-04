/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file            bootrom.c
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date           2016/07/16 
 *    
 * @brief           This file implements IAP, IO access/modify in bootrom through UART0.
 * 
 * @note
 *    2016/07/16, terry.wu, V1.0.0
 *        Initial version.
 */
 

#include "bootrom.h"
#include "ymodem.h"
#include "sqi_flash.h"
#include "MX30_CMD.h"



#define EROM_CMD_SYNC1           (0x90)
#define EROM_CMD_SYNC2           (0x08)
#define EROM_CMD_MASS_ERASE      (0x80)
#define EROM_CMD_SETPC           (0x70)
#define EROM_CMD_READ_REG        (0x60)
#define EROM_CMD_WRITE_REG       (0x50)
#define EROM_CMD_READ_MEM        (0x40)
#define EROM_CMD_WRITE_MEM       (0x30)
#define EROM_CMD_UPG_FIRMWARE    (0x20)
#define EROM_CMD_ACK             (0x10)
#define EROM_CMD_INVALID         (0xFF)
#define EROM_PARA_INVALID        (0xEE)

#define BYTE_TIMEOUT            (1)  /* 1ms */
#define ADDR_MIN                (0x20000000)
#define ADDR_MAX                (0x60000000)
#define MAX_LEN                 (0x20000000)

uint32_t flashWriteAddr = 0x0;

static volatile uint32_t s_sysTime = 0;


typedef void (*pFunction)(void);
pFunction JumpToApplication;

extern void Ymodem_Receive(void);


/*****************************************************************************
*
* SysTick drivers
*
******************************************************************************/

void SysTick_Init(void)
{
    /*Disable systick*/
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    s_sysTime = 0;
    SysTick->VAL  = 0x0;       /* Clear current timer value */
    SysTick->LOAD = SYSTICK_RELOAD_VALUE;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

static uint32_t SysTick_GetTime(void)
{
	return s_sysTime;
}


static uint32_t SysTick_GetTick(void)
{
	return SysTick->VAL;
}

volatile unsigned long ms_array[100]; 
volatile unsigned long ms_array_idx = 0;

void SysTick_DelayUS(uint16_t us)
{
    uint32_t distance, cur_tick;
    uint32_t tick = SysTick->VAL;
    uint32_t ms = s_sysTime;
    /* Wait ticks = (ticks not waited at the beginning) + (left ticks)  */
    uint32_t ticks = tick + SYSTICK_US_TICKS*us;

    if (us <= 500)
    {
        do
        {
            cur_tick = SysTick->VAL;
            distance = (cur_tick <= tick)?(tick - cur_tick):(((SYSTICK_RELOAD_VALUE + 1) - cur_tick) + tick);
            if (distance >= SYSTICK_US_TICKS*us)
                break;
        } while (1);
        return;
    }

    /* Wait for some miniseconds */
    while (ticks >= SYSTICK_MS_TICKS)
    {
        while (s_sysTime == ms) {};
        ms = s_sysTime;
        ticks -= SYSTICK_MS_TICKS;
    }
    do
    {
        if (SysTick->VAL > ticks)
        {
            break;
        }
        /* If ticks is almost equal to SYSTICK_MS_TICKS, it is not sure by only polling "SysTick->VAL" */
        if (s_sysTime != ms)
        {
            break;
        }
    } while (1);/* ((tick - SysTick->VAL) & 0x00FFFFFF) < ticks); */
}

void SysTick_DelayMS(uint32_t ms)
{
    int i;

    for (i = 0; i < ms; i++)
    {
        SysTick_DelayUS(1000);
    }

    return;
}

/* SYSTICK interrupt handler */
void INT15_Handler(void)
{
    s_sysTime++;
}

void SysTick_Disable(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}


/*****************************************************************************
*
* Uart drivers
*
******************************************************************************/
void UART_Init()
{
    /* Set UART0 pinmux */
    *((volatile uint32_t *)(0x40000140)) = ((*((volatile uint32_t *)(0x40000140)) & (~(0x3 << 24))) | (0x1 << 24));
    *((volatile uint32_t *)(0x40000140)) = ((*((volatile uint32_t *)(0x40000140)) & (~(0x3 << 26))) | (0x1 << 26));
    *((volatile uint32_t *)(0x40000154)) = ((*((volatile uint32_t *)(0x40000154)) & (~(0xF << 16))) | (0x1 << 16));
    *((volatile uint32_t *)(0x40000154)) = ((*((volatile uint32_t *)(0x40000154)) & (~(0xF << 20))) | (0x1 << 20));
    *((volatile unsigned long *)(0x40004008)) &= ~(0x1 << 15);

	/* clear reset&clock */
    SET_BIT(UART_RESET_BASE, 1 << 0);
    SET_BIT(UART_CLOCK_BASE, 1 << 0);
    
    /* UART0 initialize */
    SET_WORD(UART0_BASE + UART_IE, 0x00);
    SET_BIT(UART0_BASE + UART_BARD_DIV, 0x1A);//bardrate(115200 for 48MHz, 19200 for 8MHz)
	//SET_WORD(UART0_BASE + UART_C3, 0x03);//8 data bit, 1 stop bit, no parity 
	SET_WORD(UART0_BASE + UART_C3, 0x3B);//8 data bit, 1 stop bit, even parity 

}

void UART_PutChar(uint8_t data)
{
	while (!((GET_WORD(UART0_BASE + UART_LS)) & UART_LS_THRE))
	{
		;
	}
	*((volatile uint8_t *)(UART0_BASE) + UART_DR) = data;
}

uint8_t UART_GetChar(uint32_t tmoMs)
{
	uint8_t ret = 0xFF;
	uint32_t status = 0xFFFFFFFF;
	uint32_t time_old = 0;
	uint32_t time_new = 0;

	status = GET_WORD(UART0_BASE + UART_LS);
	time_old = SysTick_GetTime();
	while (!(status & UART_LS_DR))
	{
		status = GET_WORD(UART0_BASE + UART_LS);
		if (status & UART_LS_DR)
		{
			break;
		}

        time_new = SysTick_GetTime();
		if ((time_new - time_old) > tmoMs)
		{
			ret = 0xFF;
			return ret;
		}
	}
	
	ret = (*((volatile uint8_t *)(UART0_BASE) + UART_DR));

	return ret;
}

static uint32_t UART_ReadInteger(void)
{
	int i = 0;
	uint32_t tmp = 0;

	for (i = 3; i >= 0; i--)
	{
		tmp |= (UART_GetChar(0) << (i * 8));
	}
	

	return tmp;
}

static void UART_WriteInteger(uint32_t data)
{
	int i = 0;

	for (i = 3; i >= 0; i--)
	{
		UART_PutChar((uint8_t)((data >> (i * 8)) & 0xFF));
	}
}


/*****************************************************************************
*
* CRC drivers
*
******************************************************************************/
void CRC_Init()
{
#if 0
    /* CRC initialize,  CRC-16-CCIT */
	SET_WORD(CRC_BASE + CRC_CTRL, (0x1 << 4) | (0x1 << 12)); /* polysize = 16bit, enable xor out */
	SET_WORD(CRC_BASE + CRC_POL, 0x1021);
	SET_WORD(CRC_BASE + CRC_INIT, 0x00);
	SET_WORD(CRC_BASE + CRC_XOR, 0x00);
#endif
}

uint32_t CRC_Calculate8Bit(uint8_t * buf, uint32_t len)
{
#if 0
	uint32_t i = 0;

	SET_BIT(CRC_BASE + CRC_CTRL, 0x01);  /* calc start */

	for(i = 0; i < len; i++)
	{
		SET_WORD(CRC_BASE + CRC_DATA, buf[i]);
	}

	/* wait CRC calculate finish */
	while (!((GET_WORD(CRC_BASE + CRC_STATUS)) & 0x01))
	{
		;
	}

	return (GET_WORD(CRC_BASE + CRC_RESULT));
#else
    return 0;
#endif
}

static uint8_t CRC_Calculate32bit(uint32_t data)
{
#if 0
	uint8_t res = 0;
	uint8_t i = 0;

	SET_BIT(CRC_BASE + CRC_CTRL, 0x01);  /* calc start */
	SET_WORD(CRC_BASE + CRC_DATA, data);

	/* wait CRC calculate finish */
	while (!((GET_WORD(CRC_BASE + CRC_STATUS)) & 0x01))
	{
		;
	}

	res = (uint8_t)(GET_WORD(CRC_BASE + CRC_RESULT) & 0xFF);
	
	return res;
#else
    return 0;
#endif
}


/*****************************************************************************
*
* SQI Flash drivers
*
******************************************************************************/
void FLASH_MassErase(void)
{
#if 0
	SET_WORD(FLASH_BASE + FLASH_CMD, (2 << 4));
	SET_BIT(FLASH_BASE + FLASH_CMD, 0x01);
	while (!((GET_WORD(FLASH_BASE + FLASH_STATUS)) & (1 << 12)))
	{
		;
	}
#endif
}

void FLASH_Program(uint32_t addr, uint32_t *pData, uint32_t len)
{
#if 0
	uint32_t i = 0;
	uint32_t phyAddr = 0;

	phyAddr = addr;
	for (i = 0; i < len; i++)
	{
		while ((GET_WORD(FLASH_BASE + FLASH_STATUS) & 0x01))
		{
			;
		}
		SET_WORD(FLASH_BASE + FLASH_ADDR, phyAddr);
		SET_WORD(FLASH_BASE + FLASH_DATA, pData[i]);
		SET_WORD(FLASH_BASE + FLASH_CMD, 0x01);
		phyAddr += 4;
	}
#endif
}

static uint8_t FLASH_IsSecurity(void)
{
#if 0
	return ((GET_WORD(FLASH_BASE + FLASH_OPT) & 0x02) >> 1);
#else
    return 0;
#endif
}


/*****************************************************************************
*
* Others
*
******************************************************************************/
static uint8_t AddrIsValid(uint32_t addr)
{
	uint8_t ret = 1;
	
	if ((addr < ADDR_MIN) || (addr >= ADDR_MAX))
	{
		ret = 0;
	}

	return ret;
}

static void SetMSP(uint32_t topOfMainStack)
{
	register uint32_t __regMainStackPointer __asm("msp");
	__regMainStackPointer = topOfMainStack;
}

static int SetPC(uint32_t addr)
{
	uint32_t jumpAddr = 0;

	if ((addr < ADDR_MIN) || (addr > ADDR_MAX))
	{
		return -1;
	}
	else
	{
		/* jump to SRAM to run */
		jumpAddr = (*((volatile uint32_t *)(addr + 4)));
	}
	/* Jump to user application */
	JumpToApplication = (pFunction)jumpAddr;
	/* Initialize user application's Stack Pointer */
	SetMSP(*((volatile uint32_t *)addr));
    SysTick_Disable();
    UART_PutChar('.');
	JumpToApplication();

    return 0;
}

static void DispatchCommand(void)
{
	uint8_t cmd = 0;
	uint32_t len = 0;
	uint32_t i = 0;
	uint32_t regAddr = 0;
	uint32_t regValue = 0;
	uint32_t memAddr = 0;
	unsigned char * addr;

	do
	{
		cmd = UART_GetChar(0);
		if ((FLASH_IsSecurity() == 1) && (cmd != EROM_CMD_MASS_ERASE))
		{
			/* tell remote in security mode, only mass erase command is accepted */
			UART_PutChar(EROM_CMD_MASS_ERASE);
			continue;
		}
		switch (cmd)
		{
			case EROM_CMD_SYNC1:
			case EROM_CMD_SYNC2:
				UART_PutChar(EROM_CMD_ACK);
				break;
			case EROM_CMD_MASS_ERASE:
				/* mass erase and reboot */
				FLASH_MassErase();

				UART_PutChar(EROM_CMD_ACK);				
				break;
			case EROM_CMD_READ_REG:
				regAddr = UART_ReadInteger();
				if (AddrIsValid(regAddr) == 0)
				{
					UART_PutChar(EROM_PARA_INVALID);
					break;
				}
				regValue = GET_WORD(regAddr);
				UART_WriteInteger(regValue);
				break;
			case EROM_CMD_WRITE_REG:
				regAddr = UART_ReadInteger();
				if (AddrIsValid(regAddr) == 0)
				{
					UART_PutChar(EROM_PARA_INVALID);
					break;
				}
				regValue = UART_ReadInteger();
				SET_WORD(regAddr, regValue);
				UART_PutChar(EROM_CMD_ACK);   /* tell remote already set register */
				break;
			case EROM_CMD_READ_MEM:
				memAddr = UART_ReadInteger();
				len = UART_ReadInteger();
				if ((AddrIsValid(memAddr) == 0) || (len > MAX_LEN))
				{
					UART_PutChar(EROM_PARA_INVALID);
					break;
				}
				addr = (unsigned char *)memAddr;
				for (i = len; i > 0; i--, addr++)
				{
					UART_PutChar(*addr);
				}
				break;
			case EROM_CMD_WRITE_MEM:
				memAddr = UART_ReadInteger();
				len = UART_ReadInteger();
				if ((AddrIsValid(memAddr) == 0) || (len > MAX_LEN))
				{
					UART_PutChar(EROM_PARA_INVALID);
					break;
				}
				addr = (unsigned char *)memAddr;
				for (i = len; i > 0; i--, addr++)
				{
					*addr = UART_GetChar(0);
				}
				UART_PutChar(EROM_CMD_ACK);
				break;
			case EROM_CMD_UPG_FIRMWARE:
				flashWriteAddr = UART_ReadInteger();
				if (flashWriteAddr >= ADDR_MIN)
				{
					UART_PutChar(EROM_PARA_INVALID);
					break;
				}
				UART_PutChar(EROM_CMD_ACK);
				Ymodem_Receive();
				UART_PutChar(EROM_CMD_ACK);   /* tell remote already write flash */
				break;
			case EROM_CMD_SETPC:
				memAddr = UART_ReadInteger();
				if (memAddr < 0x40000000)     /* currently no external RAM and flash */
				{
					SetPC(memAddr);
				}				
				break;
			default:
				UART_PutChar(EROM_CMD_INVALID);  /* tell remote that received an invalid command */
				break;
		}
	} while (1);
}

//extern uint8_t s_ymodemBuffer[];
extern uint8_t s_ymodemBuffer[];
static void SqiFlashTest()
{
    int i = 0;
    uint8_t *sqiBuffer = s_ymodemBuffer;
    uint32_t size = PACKET_1K_SIZE;
    
    SPI_EraseChip();

    for(i=0; i<size; i++)
    {
        sqiBuffer[i] = i;
    }
    
    SPI_ProgramPage(0x10000000, size, sqiBuffer);
}

uint8_t s_nandPageBuffer[PAGE_SIZE];

static void NandFlashTest()
{
    int i = 0;
    uint8_t *nandBuffer = s_nandPageBuffer;
    uint32_t size = PAGE_SIZE;
    uint32_t id = 0;
    uint32_t address = 0;

    if(Read_ID_OP((uint8_t*)(&id)) != Flash_Success)
    {
        UART_PutChar('E');
        return;
    }
    if(Block_Erase_OP(address) != Flash_Success)
    {
        UART_PutChar('E');
        return;
    }  
    
    for(i=0; i<size; i++)
    {
        nandBuffer[i] = i;
    }
    if(Page_Program_OP(address, nandBuffer, size) != Flash_Success)
    {
        UART_PutChar('E');
        return;
    }
    
	memset(nandBuffer, 0, size);
    if(Page_Read_OP(address, nandBuffer, size) != Flash_Success)
    {
        UART_PutChar('E');
        return;
    }

    return;
}

/* release clk rst for nand */
void release_clk_rst(void){
    (*(volatile uint32_t*)(0x40000080)) |= 1<<6;
    (*(volatile uint32_t*)(0x40000070)) |= 1<<6;
}


static int BootromInit(void)
{
    int ret = 0;

    SysTick_Init();	
    /* test systick */
    //SysTick_DelayUS(5000000);
    //while(1)
    //SysTick_DelayMS(5000);

    UART_Init();
    UART_PutChar('B');
    //UART_PutChar(0x4);

    CRC_Init();

#if 0
    SPI_Init(0, 0, 0);
    SPI_Enalbe4LineMode();
    /* test sqi flash */
    //SqiFlashTest();
#else
    release_clk_rst();
    InitFlash();
	//NandFlashTest();
#endif

    return ret;
}

static void CommandLoop(void)
{
    uint32_t tick = 0;
	uint8_t data1 = 0;
	uint8_t data2 = 0;
	uint32_t syncTmo = 0;
	uint32_t time_old = 0;
    uint32_t time_new = 0;
    uint32_t timeoutMs = 200; /* 200ms */

    /* uart test */
    while (0)
	{
	#if 0
		data1 = UART_GetChar(BYTE_TIMEOUT);
        data2 = UART_GetChar(BYTE_TIMEOUT);
        if (((data1 == EROM_CMD_SYNC1) && (data2 == EROM_CMD_SYNC2)) ||
			((data1 == EROM_CMD_SYNC2) && (data2 == EROM_CMD_SYNC1)))
		{
			UART_PutChar(EROM_CMD_ACK);
		}
    #else
        data1 = UART_GetChar(BYTE_TIMEOUT);
        if (data1 != 0xFF)
        {
            UART_PutChar(data1);
        }
    #endif
	}
    
	while (1)
	{
		data1 = UART_GetChar(BYTE_TIMEOUT);
		data2 = UART_GetChar(BYTE_TIMEOUT);
		if (((data1 == EROM_CMD_SYNC1) && (data2 == EROM_CMD_SYNC2)) ||
			((data1 == EROM_CMD_SYNC2) && (data2 == EROM_CMD_SYNC1)))
		{
			UART_PutChar(EROM_CMD_ACK);
			DispatchCommand();
		}

        time_new = SysTick_GetTime();
		if ((time_new - time_old) > timeoutMs)
		{
			break;
		}
	}

    return;
}

#if 0//debug
#define BOOT1_ADDRESS_IN_FLASH  0x10004000
#define BOOT1_ADDRESS_IN_SRAM   0x20004000
#define BOOT1_SIZE              (50*1024)

static void JumpToLoader(void)
{
    memcpy((uint32_t *)BOOT1_ADDRESS_IN_SRAM, (uint32_t *)BOOT1_ADDRESS_IN_FLASH, BOOT1_SIZE); 
    SetPC(BOOT1_ADDRESS_IN_SRAM);

    return;
}
#else
#define INFO_ADDRESS_IN_FLASH   0x10000000

typedef struct
{
    uint32_t part_num;
    uint32_t info_addr;
    uint32_t info_size;
    uint32_t info_load_addr;
    uint32_t boot1_addr;
    uint32_t boot1_size;
    uint32_t boot1_load_addr;
    uint32_t system_addr;
    uint32_t system_size;
    uint32_t system_load_addr;
}info_t;

static void JumpToLoader(void)
{
    info_t info;
    uint32_t i = 0;
    uint32_t left = 0;
    uint32_t offset = 0;

    /* get info */
#if 0
    if(Page_Read_OP(0, page_data, PAGE_SIZE) != Flash_Success)
    {
        printk("read info fail!\n");
        return -1;
    }
    memcpy(&info, page_data, sizeof(info));
#else
    /*
    nand mapping:
    0x00000000 -> boot1
    0x00010000 -> system
    */
    info.boot1_load_addr = 0x20001000;
    info.boot1_addr = 0;
    info.boot1_size = 0x10000;
#endif

    /* copy loader to sram */
    left = info.boot1_size;
    while(left > 0)
    {
        offset = i*PAGE_SIZE;
		memset(s_nandPageBuffer, 0, PAGE_SIZE);
        if(Page_Read_OP(info.boot1_addr+offset, s_nandPageBuffer, PAGE_SIZE) != Flash_Success)
        {
            UART_PutChar('E');
            return;
        }
        
        if(left >= PAGE_SIZE)
        {
            memcpy((uint32_t *)(info.boot1_load_addr+offset), s_nandPageBuffer, PAGE_SIZE);
            left -= PAGE_SIZE;
        }
        else
        {
            memcpy((uint32_t *)(info.boot1_load_addr+offset), s_nandPageBuffer, left);
            left = 0;
        }
        i++;
    }

    /* run */
    SetPC(info.boot1_load_addr);

    return;
}
#endif


uint8_t infopage_read[PAGE_SIZE] = {0};
uint8_t page_copy[PAGE_SIZE] = {0};
uint8_t copy_systemSection_toSram[PAGE_SIZE * 32] = {0};

int main(void)
{   
    BootromInit();

    CommandLoop();

    JumpToLoader();

    return 0;
}


