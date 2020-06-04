

#include "pes_init.h"

char text_buf[20];
void diy_delay(int loop);

void pes_init(uint8_t chn);
void acmp_ac0_src_low_init(void);
void acmp_ac0_src_high_init(void);
void get_ac0_out(uint8_t val);
void sam_ac0_out(uint8_t val);
void acmp_ac1_src_high_init(void);
void acmp_ac1_src_low_init(void);
void get_ac1_out(uint8_t val);
void sam_ac1_out(uint8_t val);

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))
#define Multi_Bit_Cfg(reg,bit_vld,start_bit,val) (HW32_REG(reg) =(HW32_REG(reg)& ~(bit_vld<<start_bit)) | (val<<start_bit))

void acmp_ac_init(void)
{

    HW32_REG(ACMP_BASE_ADDR + 0x20) |= (1 << 21)   ; //Enable AC0 event trigger
    HW32_REG(ACMP_BASE_ADDR + 0x30) |= (1 << 21)   ; //Enable AC1 event trigger

}
int main_demo(void)
{
    int i = 0;
    int read_value;
    uint32_t    prog_val       = 0;
    uint32_t    new_prog_val   = 0;
    uint32_t    reg_addr        ;
    uint32_t    error   = 0;

    printf("\n **** ACMP event to PES test begin ****\n");

    //ACMP clock enable
    HW32_REG(0x40000080)    |= 1 << 10;
    //ACMP reset enable
    HW32_REG(0x40000070)    |= 1 << 10;

    HW32_REG(ACMP_BASE_ADDR + 0x04) |= 1 << 0 ;
    AC0_EVNT_OUT_SET;

    for (i = 0; i < 12; i++)
    {
        pes_recieve_initial(i, MASTER_ACMP, 0, PES_ASNY_EN, 0, DETECT_EN);
        acmp_ac0_src_low_init();
        sam_ac0_out(0);
        acmp_ac0_src_high_init();
        sam_ac0_out(1);
        error += pes_recieve_detect(i);
        pes_recieve_initial(i, 0, 0, 0, 0, 0);
    }

    if (error != 0)
    {
        printf("\n*** ERROR !!!! ACMP event to PES test failed ***\n");
    }
    else
    {
        printf("\n*** SUCCESS !!!! ACMP event to PES test passed ***\n");
    }

    return 0;

}


void pes_init(uint8_t chn)
{

    Multi_Bit_Cfg(PES_BASE_ADDR + 0x20, 0x3, 24, 3); //Event output mode
    Multi_Bit_Cfg(PES_BASE_ADDR + 0x20, 0xf, 0, 5);  //set acmp as source
    Multi_Bit_Cfg(PES_BASE_ADDR + 0x20, 0xf, 8, chn); //acmp selection


    HW32_REG(ACMP_BASE_ADDR + 0x64) = (1 << 0); //local channel selection
}


void ACMP_Handler()
{
    printf("\n ACMP interrupt is issuing!");

    printf("\nClear AC_INT");
    HW32_REG(ACMP_BASE_ADDR + 0x20) |= (1 << 25); //clear interrupt

    if (0 == (HW32_REG(ACMP_BASE_ADDR + 0x20) & (1 << 25)))
    {
        printf("\n AC_INT = 0");
    }
    else
    {
        printf("\nError_Mx_SIM : AC_INT is not correct! 0x%x", (HW32_REG(ACMP_BASE_ADDR + 4)));
    }

    HW32_REG(ACMP_BASE_ADDR + 0x20) &= ~(1 << 19) ; //Disable AC interrupt

    //__NOP;

}

void diy_delay(int loop)
{
    int i;

    for (i = 0; i < loop; i++)
    {
        // __NOP;
        // __NOP;
    }
}


void acmp_ac0_src_low_init(void)
{

    printf("\n Init ACMP source LOW");
    printf("\n   @@ACMP_PSEL:DAC2, AC0_NSEL:ACMP[3]");
    Multi_Bit_Cfg(ACMP_BASE_ADDR + 0x04, 0x3, 7, 3); //DAC2
    Multi_Bit_Cfg(ACMP_BASE_ADDR + 0x04, 0x7, 9, 1); //ACMP[3]
}

void acmp_ac0_src_high_init(void)
{

    printf("\n Init ACMP source HIGH");
    printf("\n   @@ACMP_PSEL:ACMP[1], AC0_NSEL:VREFIN");
    Multi_Bit_Cfg(ACMP_BASE_ADDR + 0x04, 0x3, 7, 1); //ACMP[1]
    Multi_Bit_Cfg(ACMP_BASE_ADDR + 0x04, 0x7, 9, 4); //VREFIN
}

void get_ac0_out(uint8_t val)
{
    uint32_t    tmp ;

    printf("\n   @@Till AC0 output stable!");
    //    do{;}while((1<<26)==(HW32_REG(ACMP_BASE_ADDR+0x20)&(1<<26)));
    if (val > 0)
    {
        tmp = (1 << 24);
    }
    else
    {
        tmp = 0 ;
    }
    do {;}
    while (0 == (HW32_REG(ACMP_BASE_ADDR + 0x20) & (1 << 26)));
    if (tmp != (HW32_REG(ACMP_BASE_ADDR + 0x20) & (1 << 24)))
    {
        printf("\nError_Mx_SIM: AC0 output is not correct, should be %0x!\n", val);
    }
    else
    {
        printf("\nCongralution: AC0 output is correct, %0x!\n", val);
    }

}

void sam_ac0_out(uint8_t val)
{
    uint32_t    tmp ;

    //HW32_REG(ACMP_BASE_ADDR+0x04) |= 1<<0 ;
    printf("\n   @@Till ACMP output stable!");
    if (val > 0)
    {
        tmp = (1 << 24);
    }
    else
    {
        tmp = 0 ;
    }
    //do{;}while(0==(HW32_REG(ACMP_BASE_ADDR+0x04)&(1<<26)));
    if (tmp != (HW32_REG(ACMP_BASE_ADDR + 0x04) & (1 << 24)))
    {
        printf("\nError_Mx_SIM: ACMP output is not correct, should be %0x!\n", val);
    }
    else
    {
        printf("\nCongralution: ACMP output is correct, %0x!\n", val);
    }

    //HW32_REG(ACMP_BASE_ADDR+0x04) &= ~(1<<0) ;
}

void acmp_ac1_src_high_init(void)
{

    printf("\n Init AC1 source HIGH");
    printf("\n   @@AC1_PSEL:ACMP[3], AC0_NSEL:ACMP[2]");
    Multi_Bit_Cfg(ACMP_BASE_ADDR + 0x30, 0x7, 4, 2); //ACMP[3]
    Multi_Bit_Cfg(ACMP_BASE_ADDR + 0x30, 0x7, 8, 0); //ACMP[2]

}

void acmp_ac1_src_low_init(void)
{

    printf("\n Init AC1 source LOW");
    printf("\n   @@AC1_PSEL:ACMP[3], AC0_NSEL:ACMP[5]");
    Multi_Bit_Cfg(ACMP_BASE_ADDR + 0x30, 0x7, 4, 2); //ACMP[3]
    Multi_Bit_Cfg(ACMP_BASE_ADDR + 0x30, 0x7, 8, 2); //ACMP[5]
}


void get_ac1_out(uint8_t val)
{
    uint32_t    tmp;

    printf("\n   @@Till AC1 output stable!");
    //    do{;}while((1<<26)==(HW32_REG(ACMP_BASE_ADDR+0x30)&(1<<26)));
    if (val > 0)
    {
        tmp = (1 << 24);
    }
    else
    {
        tmp = 0 ;
    }
    do {;}
    while (0 == (HW32_REG(ACMP_BASE_ADDR + 0x30) & (1 << 26)));
    if (tmp != (HW32_REG(ACMP_BASE_ADDR + 0x30) & (1 << 24)))
    {
        printf("\nError_Mx_SIM: AC1 output is not correct, should be %0x!", val);
    }
    else
    {
        printf("\nCongralution: AC1 output is correct, %0x!", val);
    }

}

void sam_ac1_out(uint8_t val)
{
    uint32_t    tmp;

    HW32_REG(ACMP_BASE_ADDR + 0x30) |= 1 << 0 ;
    printf("\n   @@Till AC1 output stable!");
    if (val > 0)
    {
        tmp = (1 << 24);
    }
    else
    {
        tmp = 0 ;
    }
    do {;}
    while (0 == (HW32_REG(ACMP_BASE_ADDR + 0x30) & (1 << 26)));
    if (tmp != (HW32_REG(ACMP_BASE_ADDR + 0x30) & (1 << 24)))
    {
        printf("\nError_Mx_SIM: AC1 output is not correct, should be %0x!", val);
    }
    else
    {
        printf("\nCongralution: AC1 output is correct, %0x!", val);
    }

    HW32_REG(ACMP_BASE_ADDR + 0x30) &= ~(1 << 0) ;
}
