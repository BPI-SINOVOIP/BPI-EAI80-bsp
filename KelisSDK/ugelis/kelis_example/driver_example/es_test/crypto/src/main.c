#include <ugelis.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <gm_hal_crypto.h>
#include "crypto_test.h"


static CRYP_Handle_T crypHandle;
static CRYP_Config_T config;


#define     OTP_VER                 0x1FFFF000
#define     OTP_WM                  0x1FFFF010
#define     OTP_EN                  0x1FFFF014
#define     OTP_CELL_SEL            0x1FFFF018
#define     OTP_ADDR                0x1FFFF01C
#define     OTP_CTRL                0x1FFFF020
#define     OTP_ST                  0x1FFFF024
#define     OTP_PDIN                0x1FFFF028
#define     OTP_TIM0                0x1FFFF030
#define     OTP_TIM1                0x1FFFF034
#define     OTP_BR_ST               0x1FFFF040

#define     OTP_MEM_IO              0x1FFF8000

#define     NORMAL_MOD              0x00
#define     INIT_MARGIN_RD_MOD      0x01
#define     PROG_MOD                0x02
#define     PGM_MARGIN_RD_MOD       0x04

uint32_t aes_key[8] =
{
    0x603deb10, 0x15ca71be, 0x2b73aef0, 0x857d7781,
    0x1f352c07, 0x3b6108d7, 0x2d9810a3, 0x914dff4
};

uint32_t tdes_key[6] =
{
    0x1234567, 0x89abcdef, 0x23456789, 0xabcdef01,
    0x456789ab, 0xcdef0123
};

void OTP_Program(uint32_t addr, uint32_t *key, uint32_t len)
{
    uint32_t i = 0;
    uint32_t tmp = 0;

    *((volatile uint32_t *)0x1FFFF014) &= ~(0x01 << 0); //disable OTP
    *((volatile uint32_t *)0x1FFFF010) = 0x02;   //program mode
    *((volatile uint32_t *)0x1FFFF014) |= (0x01 << 0); //enable OTP

    for (i = 0; i < len; i++)
    {
        *((volatile uint32_t *)0x1FFFF01C) = ((addr >> 2));
        *((volatile uint32_t *)0x1FFFF028) = key[i];
        addr += 4;

        *((volatile uint32_t *)0x1FFFF018) &= ~(0x01 << 1);
        *((volatile uint32_t *)0x1FFFF020) |= 0x01;
        tmp = *((volatile uint32_t *)0x1FFFF024);
        while (tmp & 0x01)
        {
            ;
        }

        *((volatile uint32_t *)0x1FFFF018) |= (0x01 << 1);
        *((volatile uint32_t *)0x1FFFF020) |= 0x01;
        while (tmp & 0x01)
        {
            ;
        }
    }

}

uint32_t OTP_Read(uint32_t addr)
{
    uint32_t data = 0;

    *((volatile uint32_t *)0x1FFFF014) &= ~(0x01 << 0); //disable OTP
    *((volatile uint32_t *)0x1FFFF010) = 0;   //program mode
    *((volatile uint32_t *)0x1FFFF014) |= (0x01 << 0); //enable OTP

    data = *((volatile uint32_t *)(0x1FFF8000 + addr));

    return data;
}

int main(void)
{
    uint32_t addr = 0x50;
    uint32_t data;
#if 0
    //OTP_Program(addr, aes_key, 8);
    data = OTP_Read(addr);

    addr = 0x80;
    //OTP_Program(addr, tdes_key, 6);
    data = OTP_Read(addr);

    addr = 0xB0;//0x90;
    //OTP_Program(addr, tdes_key, 6);
    data = OTP_Read(addr);

    addr = 0x48;
    data = 0xFFFFFFF0;
    //OTP_Program(addr, &data, 1);
    data = OTP_Read(addr);

    /* CRYP_DBG_EN shadow register */
    //addr = 0x1ffff0c8;
    //*((volatile uint32_t *)addr) = 0xFFFFFFF0;

    CRYP_AesEcbTest(&crypHandle, &config);
    CRYP_DESEcbTest(&crypHandle, &config);
#else

    CRYP_AesEcbTest(&crypHandle, &config);
    CRYP_AesCbcTest(&crypHandle, &config);
    CRYP_AesCTRTest(&crypHandle, &config);
    CRYP_AesGCMTest(&crypHandle, &config);
    CRYP_AesCCMTest(&crypHandle, &config);
    CRYP_DESEcbTest(&crypHandle, &config);
    CRYP_DESCbcTest(&crypHandle, &config);

    CRYP_AesEcbSwapTest(&crypHandle, &config);
#endif
    return 0;
}
