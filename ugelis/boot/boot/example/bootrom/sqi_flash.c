#include "sqi_flash.h"

static uint8_t spi_flash_read_status_reg(void)
{
	uint8_t reg = 0;
	FLASH->CFG = SQI_COMMAND_SELECTED_EN | SQI_DATA_SELECTED_EN | MX25R32_W25Q64_RDSR_CMD;
	reg = *(uint8_t *)FLASH_BASE_ADDR;
	return reg;
}

static void wait_for_flash_idle(void)
{
	uint8_t rbuf;
	rbuf = spi_flash_read_status_reg();
	while(rbuf & MX25R32_W25Q64_BUSY)
	{
		rbuf = spi_flash_read_status_reg();
	}
}

static void spi_flash_write_enable(void)
{
	FLASH->CFG = SQI_COMMAND_SELECTED_EN | MX25R32_W25Q64_WREN_CMD;
	(*(uint32_t *)FLASH_BASE_ADDR) = 0x0;   //to trigger out the write enable command
}

void SPI_Init(void)
{
  	*(volatile uint32_t *)(0x400002D8) &= ~(0xF << 16);
    *(volatile uint32_t *)(0x400002D8) |=  (0x5 << 16);

	*(volatile uint32_t *)(0x400002DC) &= ~(0x3F << 26);
    *(volatile uint32_t *)(0x400002DC) |=  (0x15 << 26);

	*(volatile uint32_t *)(0x400002E0) &= ~(0x3 << 0);
    *(volatile uint32_t *)(0x400002E0) |=  (0x1 << 0);

	*(volatile uint32_t *)(0x4000031C) &= ~(0xFF << 0);
    *(volatile uint32_t *)(0x4000031C) |=  (0xcc << 0);

	*(volatile uint32_t *)(0x40000324) &= ~(0xFFF << 20);
    *(volatile uint32_t *)(0x40000324) |=  (0xccc << 20);

	*(volatile uint32_t *)(0x40000328) &= ~(0xF << 0);
    *(volatile uint32_t *)(0x40000328) |=  (0xc << 0);
 
/* not need to initialize SPI */ 
	FLASH->CFG = SQI_COMMAND_SELECTED_EN | SQI_ADDR_SELECTED_EN | SQI_DATA_SELECTED_EN | MX25R32_W25Q64_READ_CMD;
}

void SPI_UnInit(void)
{
	/* Add you code if you need */
}

void SPI_EraseBlock64K(uint32_t addr)
{
	wait_for_flash_idle();
	spi_flash_write_enable();
	wait_for_flash_idle();
	FLASH->CFG = SQI_COMMAND_SELECTED_EN | SQI_ADDR_SELECTED_EN | MX25R32_W25Q64_BE_CMD;
	(*(uint32_t *)(FLASH_BASE_ADDR | addr)) = 0x0;
	wait_for_flash_idle();
}

void SPI_Program(uint32_t addr, uint32_t sz, uint8_t * buf)
{
	uint32_t i = 0;
	for(i = 0; i < sz; i++)
	{
		wait_for_flash_idle();
		spi_flash_write_enable();
		wait_for_flash_idle();

		FLASH->CFG = SQI_COMMAND_SELECTED_EN | SQI_ADDR_SELECTED_EN | SQI_DATA_SELECTED_EN | MX25R32_W25Q64_PP_CMD;
		(*(uint8_t *)((FLASH_BASE_ADDR | addr) + i)) = buf[i];
	}
}





