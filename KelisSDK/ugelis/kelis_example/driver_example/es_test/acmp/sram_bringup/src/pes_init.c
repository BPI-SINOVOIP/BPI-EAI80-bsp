
#include "pes_init.h"

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

#ifdef PES_BASE_ADDR
#else
    #define  PES_BASE_ADDR      0x40037000
#endif

typedef struct
{
    __IO   uint32_t   RESERVED00     ; /*!< Offset: 0x00  Register (R/W) */
    __IO   uint32_t   RESERVED01     ; /*!< Offset: 0x04  Register (R/W) */
    __IO   uint32_t   RESERVED02     ; /*!< Offset: 0x08  Register (R/W) */
    __IO   uint32_t   RESERVED03     ; /*!< Offset: 0x0C  Register (R/W) */
    __IO   uint32_t   GPIOEVCTL      ; /*!< Offset: 0x10  Register (R/W) */
    __IO   uint32_t   RESERVED05     ; /*!< Offset: 0x14  Register (R/W) */
    __IO   uint32_t   RESERVED06     ; /*!< Offset: 0x18  Register (R/W) */
    __IO   uint32_t   RESERVED07     ; /*!< Offset: 0x1C  Register (R/W) */
    __IO   uint32_t   CH0CTL         ; /*!< Offset: 0x20  Register (R/W) */
    __IO   uint32_t   CH1CTL         ; /*!< Offset: 0x24  Register (R/W) */
    __IO   uint32_t   CH2CTL         ; /*!< Offset: 0x28  Register (R/W) */
    __IO   uint32_t   CH3CTL         ; /*!< Offset: 0x2C  Register (R/W) */
    __IO   uint32_t   CH4CTL         ; /*!< Offset: 0x30  Register (R/W) */
    __IO   uint32_t   CH5CTL         ; /*!< Offset: 0x34  Register (R/W) */
    __IO   uint32_t   CH6CTL         ; /*!< Offset: 0x38  Register (R/W) */
    __IO   uint32_t   CH7CTL         ; /*!< Offset: 0x3C  Register (R/W) */
    __IO   uint32_t   CH8CTL         ; /*!< Offset: 0x40  Register (R/W) */
    __IO   uint32_t   CH9CTL         ; /*!< Offset: 0x44  Register (R/W) */
    __IO   uint32_t   CH10CTL        ; /*!< Offset: 0x48  Register (R/W) */
    __IO   uint32_t   CH11CTL        ; /*!< Offset: 0x4C  Register (R/W) */
    __IO   uint32_t   RESERVED08     ; /*!< Offset: 0x50  Register (R/W) */
    uint32_t   RESERVED0[42]  ;
    __IO   uint32_t   RESERVED09     ; /*!< Offset: 0xFC  Register (R/W) */

} CMSDK_PES_TypeDef;



#define CMSDK_PES             ((CMSDK_PES_TypeDef   *) PES_BASE_ADDR)



void pes_recieve_initial(uint32_t pes_channel, uint32_t master_sel, uint32_t source_sel, uint32_t async_or_sync, uint32_t work_mode, uint32_t detect_en)
{
    uint32_t    PES_CHX_CFG ;
    uint32_t    pes_channel_num ;

    if (pes_channel == 0)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x20;
    }
    else if (pes_channel == 1)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x24;
    }
    else if (pes_channel == 2)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x28;
    }
    else if (pes_channel == 3)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x2C;
    }
    else if (pes_channel == 4)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x30;
    }
    else if (pes_channel == 5)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x34;
    }
    else if (pes_channel == 6)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x38;
    }
    else if (pes_channel == 7)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x3C;
    }
    else if (pes_channel == 8)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x40;
    }
    else if (pes_channel == 9)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x44;
    }
    else if (pes_channel == 10)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x48;
    }
    else if (pes_channel == 11)
    {
        PES_CHX_CFG = PES_BASE_ADDR + 0x4C;
    }
    else
    {
        printf("\n ERROR !!! PES configuration is not supported !!!\n");
    }

    //pes channel configuration
    HW32_REG(PES_CHX_CFG)   = 0x0;
    HW32_REG(PES_CHX_CFG)   |= (master_sel << 0 | source_sel << 8 | detect_en << 12 | detect_en << 15 | work_mode << 24 | async_or_sync << 31);

    HW32_REG(PES_BASE_ADDR + 0x0C)  = 0xffff << 4;

    printf("\n INFO : PES Channel %d configuration finished    \n", pes_channel);

    return;
}

int pes_recieve_detect(uint32_t pes_channel)
{
    uint32_t    pes_status;
    uint32_t    i;
    uint32_t    pes_channel_num ;

    while ((HW32_REG(PES_BASE_ADDR + 0x0c) & (0xfff << 4)) == 0);
    pes_status  = HW32_REG(PES_BASE_ADDR + 0x0c) & (0xfff << 4);
    for (i = 0; i < 12; i++)
    {
        if (pes_status == 1 << (4 + i))
        {
            printf("\n INFO : PES Channel %d has detected a rising edge     \n", i);
            pes_channel_num = i;
            HW32_REG(PES_BASE_ADDR + 0x0c)  = 1 << (i + 4);
        }
    }

    if (pes_channel != pes_channel_num)
    {
        while (1)
        {
            printf("ERROR !!! PES selected channel and detected channel is inconsistent !!!  \n");
        }
        return 1;
    }
    else
    {
        return 0;
    }
}


void pes_send_initial(uint32_t pes_channel, uint32_t pes_evt_type)
{

    if (pes_evt_type == 0)
    {
        HW32_REG(PES_BASE_ADDR + 0x04)  =   0x0;
        printf("\n  INFO : An event is generated on channel %d    \n", pes_channel);
        HW32_REG(PES_BASE_ADDR + 0x04)  |=  1 << pes_channel;
    }
    else if (pes_evt_type == 1)
    {
        HW32_REG(PES_BASE_ADDR + 0x04)  =   0x0;
        printf("\n  INFO : A high level is generated on channel %d    \n", pes_channel);
        HW32_REG(PES_BASE_ADDR + 0x04)  |= (1 << pes_channel + 16);
    }
    else
    {
        printf("\n ERROR !!! PES configuration is ERROR   \n");
    }

    return;
}


