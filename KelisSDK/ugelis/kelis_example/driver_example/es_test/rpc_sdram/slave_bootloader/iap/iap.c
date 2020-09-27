#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"

iapfun jump2app;
unsigned int iapbuf[512];   //2K byte buffer
//appxaddr:Ӧ�ó������ʼ��ַ
//appbuf:Ӧ�ó���CODE.
//appsize:Ӧ�ó����С(�ֽ�).
void iap_write_appbin(u32 appxaddr, u8 *appbuf, u32 appsize)
{
#if 0
    u32 t;
    u16 i = 0;
    u32 temp;
    u32 fwaddr = appxaddr; //��ǰд��ĵ�ַ
    u8 *dfu = appbuf;
    for (t = 0; t < appsize; t += 4)
    {
        temp = (u32)dfu[3] << 24;
        temp |= (u32)dfu[2] << 16;
        temp |= (u32)dfu[1] << 8;
        temp |= (u32)dfu[0];
        dfu += 4; //ƫ��4���ֽ�
        iapbuf[i++] = temp;
        if (i == 512)
        {
            i = 0;
            STMFLASH_Write(fwaddr, iapbuf, 512);
            fwaddr += 2048; //ƫ��2048  512*4=2048
        }
    }
    if (i)
    {
        STMFLASH_Write(fwaddr, iapbuf, i);    //������һЩ�����ֽ�д��ȥ.
    }
#endif
}

//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0             //set Main Stack value
    BX r14
}


//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(u32 appxaddr)
{
#if 0
    if (((*(vu32 *)appxaddr) & 0x2FF00000) == 0x20000000) //���ջ����ַ�Ƿ�Ϸ�.
    {
        jump2app = (iapfun) * (vu32 *)(appxaddr + 4); //�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)
        MSR_MSP(*(vu32 *)appxaddr);                 //��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
        jump2app();                                 //��ת��APP.
    }
#endif
}
