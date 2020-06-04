/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file            ymodem.c
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date           2016/07/19 
 *    
 * @brief           This file implements the Ymodem protocol to send/receive files.
 * 
 * @note
 *    2016/07/19, terry.wu, V1.0.0
 *        Initial version.
 */

#if 1

#include "ymodem.h"
#include "bootrom.h"
#include "sqi_flash.h"
#include <string.h>

#define ERR_MAX     5
#define BYTE_TIMEOUT            1   /* n*1 ms */
#define UPGRADE_ADDR            0x0

uint8_t s_ymodemBuffer[PACKET_1K_SIZE + 6];
//extern uint8_t page_read[PAGE_SIZE];
//extern uint8_t copy_systemSection_toSram[PAGE_SIZE * 32];
//extern uint8_t page_copy[PAGE_SIZE];


extern uint32_t flashWriteAddr;
extern info_t info;

uint8_t tmpData = 0;

static int ReceivePacket(uint8_t * pData, uint32_t * pLength, uint32_t timeout)
{
	int ret = 0;
	uint32_t crc = 0xFF;
	uint32_t calcCrc = 0xFF;
	uint32_t dataLength = 0;
//	uint8_t tmpData = 0;
	uint32_t i = 0;
	uint32_t packetLength = 0;
	uint8_t * pTemp;

	/* if can't get the packet after 3 second, need to send NAK */
	tmpData = UART_GetChar(timeout * 300/*3000*/);
	//tmpData = UART_GetChar(3000);
	switch (tmpData)
	{
		case SOH:
			dataLength = PACKET_SIZE;
			break;
		case STX:
			dataLength = PACKET_1K_SIZE;
			break;
		case EOT:
			*pLength = 1;
			break;
		default:
//			dataLength = PACKET_1K_SIZE;
			*pLength = 0;
			ret = -1;
			break;
	}

	pData[PACKET_START_INDEX] = tmpData;
	if (dataLength >= PACKET_SIZE)
	{
		pTemp = &pData[PACKET_NUMBER_INDEX];
		packetLength = dataLength + PACKET_HEADER_SIZE + PACKET_TRAILER_SIZE;
		for (i = 0; i < (packetLength - 1); i++)  /* 1 for the start char already received */
		{
			*pTemp = UART_GetChar(timeout*10);
			pTemp++;
		}

		/* check the packet valid or not */
//		if (pData[PACKET_NUMBER_INDEX] != (pData[PACKET_CNUMBER_INDEX] ^ NEGATIVE_BYTE))
//		{
//			ret = -1;
//		}
//		else
//		{
//			/* check CRC here */
//			crc = pData[PACKET_DATA_INDEX + dataLength] << 8;
//			crc += pData[PACKET_DATA_INDEX + dataLength + 1];
//			calcCrc = CRC_Calculate8Bit(&pData[PACKET_DATA_INDEX], dataLength);
//			if (calcCrc != crc)
//			{
//				ret = -1;
//			}
//		}

		*pLength = (ret == -1) ? 0 : dataLength;
	}

	return ret;
}

uint32_t pkgReceived = 0;

void Ymodem_Receive(void)
{
	int ret = 0;
	uint8_t sessionBegin = 0;
	uint8_t sessionDone = 0;
	uint32_t pLength = 0;
	uint8_t errCount = 0;
	uint8_t eotCount = 0;
//	uint32_t pkgReceived = 0;
	uint8_t cResendCnt = 0;
	//info_t info;
	uint8_t index = 0;
	uint32_t boot1_size = 0;
	//uint32_t offset = 0; 

	/* delay 20s for operate PC tool temparately */
#if 0	
	SysTick_DelayMS(20000);
#endif

	UART_PutChar(C);
	while (sessionDone == 0)
	{
		pLength = 0;
		memset(&s_ymodemBuffer[0], 0xAA, PACKET_1K_SIZE + 6);
		ret = ReceivePacket(&s_ymodemBuffer[0], &pLength, BYTE_TIMEOUT);
		if (ret >= 0)
		{
			switch (pLength)
			{
				case 1:
					if (s_ymodemBuffer[PACKET_START_INDEX] == EOT)
					{
						eotCount++;
						if (eotCount == 1)
						{
							UART_PutChar(NAK);
						}
						else
						{
							/* already receive the whole file, check whether there is file need to receive */
							UART_PutChar(ACK);
							UART_PutChar(C);
							eotCount = 0;
							pkgReceived = 0;
						}
					}
					break;
				default:
					if (s_ymodemBuffer[PACKET_NUMBER_INDEX] != pkgReceived)
					{
						UART_PutChar(NAK);
					}
					else
					{
						if (pkgReceived == 0)
						{
							if (s_ymodemBuffer[PACKET_DATA_INDEX] != 0)
							{
								/* receive the first packet of the file, here can parse the file name
								    and file length, for bootrom size limited, don't parse now */
								//FLASH_MassErase();
								#if 1  //nor flash
								flashWriteAddr |= 0x10001000;
								//memcpy(&info, (uint32_t *)INFO_ADDRESS_IN_FLASH, sizeof(info_t));
								//SPI_EraseBlock64K(INFO_ADDRESS_IN_FLASH);
								index = PACKET_DATA_INDEX;
								while((uint8_t)s_ymodemBuffer[index++] != 0x00);
//								boot1_size = s_ymodemBuffer[index++];
//								boot1_size = (boot1_size << 8) | s_ymodemBuffer[index++];
//								boot1_size = (boot1_size << 16) | s_ymodemBuffer[index++];
//								boot1_size = (boot1_size << 24) | s_ymodemBuffer[index++];
								boot1_size = (s_ymodemBuffer[index] <<24) | (s_ymodemBuffer[index+1] <<16) | (s_ymodemBuffer[index+2] <<8) | (s_ymodemBuffer[index+3]);
								info.boot1_size = boot1_size;
								if(info.part_num == 0xffffffff)
								{
									info.part_num = 0x03;
									info.boot1_addr = 0x10001000;
									info.boot1_load_addr = 0x20001000;
									info.info_addr = 0x10000000;
									info.info_load_addr = 0x0;
									info.info_size = 0x1000;
									info.system_addr = 0x10010000;
									info.system_load_addr = 0xc0010000;
								}
								SPI_Program(INFO_ADDRESS_IN_FLASH, sizeof(info_t), (uint8_t *)&info);								
								#else  //nand flash
								flashWriteAddr &= 0x0FFFFFFF;
								/**********copy info and system app to sram*****************/
								if(Page_Read_OP(0, page_data, PAGE_SIZE) != Flash_Success)
								{
									//debug_printf("page read error\n");
								}
								while(index < 32)
								{
									offset = index * PAGE_SIZE + 0x10000; //0x10000: system app Base address
									if(Page_Read_OP(offset, page_copy, PAGE_SIZE) != Flash_Success)
									{
										//debug_printf("page copy error");
									}
									else
									{
										memcpy(copy_systemSection_toSram + index * PAGE_SIZE, page_copy, PAGE_SIZE);
									}
									index++;
								}
								/**************Excute block erase*****************************/
								if(Block_Erase_OP(0) != Flash_Success)
								{
									//debug_printf("block erase error\n");
								}
								/************write back info and system to nand flash*********/
								if(Page_Program_OP(0, infopage_read, PAGE_SIZE) != Flash_Success)
								{
									//debug_printf("program nand flash error\n");
								}
								index = 0;
								while(index < 32)
								{
									offset = index * PAGE_SIZE + 0x10000; 
									if(Page_Program_OP(offset, &copy_systemSection_toSram[index*PAGE_SIZE],PAGE_SIZE) != Flash_Success)
									{
										//debug_printf("program nand flash error\n");
									}
									index++;
								}	
								#endif
								////because first frame is the information of file, so not should program it.
								UART_PutChar(ACK);
								UART_PutChar(C);  /* request for the data packet */

								sessionBegin = 1;
							}
							else
							{
								/* there is no more file need to receive */
								UART_PutChar(ACK);
								sessionDone = 1;
							}
						}
						else
						{
							/* data packet here, write to flash */
							////FLASH_Program(flashWriteAddr, (uint32_t *)((uint32_t)&s_ymodemBuffer[PACKET_DATA_INDEX]), (pLength / 4));
							#if 1   //nor flash
							SPI_Program(flashWriteAddr, pLength, &s_ymodemBuffer[PACKET_DATA_INDEX]);
							#else   //nand flash
							Page_Program_OP(flashWriteAddr, &s_ymodemBuffer[PACKET_DATA_INDEX], pLength);///??????
							#endif
							flashWriteAddr += pLength;
							UART_PutChar(ACK);
						}
						pkgReceived++;
					}
					break;
			}
		}
		else
		{
			if (sessionBegin > 0)
			{
				errCount++;
				if (errCount >= ERR_MAX)
				{
					/* stop receive file */
					UART_PutChar(CAN);
					UART_PutChar(CAN);
					sessionDone = 1;
				}
				else
				{
					/* request to resend last packet */
					UART_PutChar(NAK);
				}
			}
			else
			{
				/* after 3 seconds, request for file again */
				SysTick_DelayMS(3000);
				UART_PutChar(C);
				cResendCnt++;
				if (cResendCnt > 4)   /* wait 12 second at most */
				{
					//UART_PutChar(NAK);   /* Sender don't support CRC, change to checksum */
					sessionDone = 1;
				}
			}
			
		}
	}
}

#endif
