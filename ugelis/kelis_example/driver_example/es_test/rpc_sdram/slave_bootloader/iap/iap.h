#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"

typedef  void (*iapfun)(void);              //����һ���������͵Ĳ���.
#define FLASH_APP1_ADDR     0x08010000      //��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
//����0X08000000~0X0800FFFF�Ŀռ�ΪBootloaderʹ��(��64KB)
void iap_load_app(u32 appxaddr);            //��ת��APP����ִ��
void iap_write_appbin(u32 appxaddr, u8 *appbuf, u32 applen); //��ָ����ַ��ʼ,д��bin
#endif







































