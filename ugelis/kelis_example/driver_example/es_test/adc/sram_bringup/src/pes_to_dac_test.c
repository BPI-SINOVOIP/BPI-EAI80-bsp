#include "CMSDK_CM4.h"
#include <stdio.h>
#include "uart_stdout.h"
#include "./pes_initial.c"

int dac_basic_test(CMSDK_DAC_TypeDef *CMSDK_DAC);
void small_delay();
int dac_result_check(int data_out, int expect_data_out, int convert_mode);

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

//#define small_delay()   __ISB

int main(void)
{
    int err = 0;

    UartStdOutInit();
    //Enable DAC clock
    HW32_REG(0x40000080) |= 1 << 11;
    //Release DAC reset
    HW32_REG(0x40000070) |= 1 << 11;


    puts("----------------DAC basic test begin  -----------------\n");
    err = dac_basic_test(CMSDK_DAC1);
    if (err != 0)
    {
        puts("**Error : DAC1 basic test failed !!! ** \n");
    }
    else
    {
        puts("**Success : DAC1 basic test pass !!! **\n");
    }

    err = dac_basic_test(CMSDK_DAC2);
    if (err != 0)
    {
        puts("**Error : DAC2 basic test failed !!! ** \n");
    }
    else
    {
        puts("**Success : DAC2 basic test pass !!! **\n");
    }

    puts("----------------DAC basic test end  -----------------\n");

    UartEndSimulation();
    return 0;
}

int dac_basic_test(CMSDK_DAC_TypeDef *CMSDK_DAC)
{
    int data_out   = 0;
    int error   = 0;
    int i   = 0;
    int k   = 0;
    int data_mask   = 0xFFF, data_mask_10 = 0x3FF, data_mask_8 = 0xFF;
    float vref  = 4096, vref_10 = 1024, vref_8 = 256;
    float convert_result  = 0;
    float expect_data   = 0;
    //int expect_convert_data = 0;
    int convert_data[4] = {0x00000000, 0xFFFFFFFF, 0x5A5A5A5A, 0xA5A5A5A5};

    //DAC 12-bit mode test
    CMSDK_DAC->DAC_CTRL = 0x1;
    if (CMSDK_DAC->DAC_DO != 0x0)
    {
        printf("Error: Data out register is not empty ,read = 0x%x\n", CMSDK_DAC->DAC_DO);
    }

    for (k = 0; k < 12; k ++)
    {
        for (i = 0; i < 4; i ++)
        {
            CMSDK_DAC->DAC_DH   = convert_data[i];
            //CMSDK_DAC->DAC_STRIG    = 0x1;
            CMSDK_DAC->DAC_HTRIG &= 0x0;
            CMSDK_DAC->DAC_HTRIG |= (k << 4 | 1 << 0);
            pes_send_initial(k, 0);
            small_delay();

            data_out  = CMSDK_DAC->DAC_DO;
            if (data_out != (convert_data[i] & data_mask))
            {
                printf("**Error : data_out != convert_data \n data_out = 0x%x, convert_data = 0x%x\n", data_out, convert_data[i]);
                error   |= 1 << 0;
            }

            error += dac_result_check(data_out, convert_data[i], 12);
        }
    }
    return  error;
}


void small_delay()
{
    int i = 0;
    int a = 0;

    for (i = 0; i < 5; i++)
    {
        a ++;
    }

    return;
}

int dac_result_check(int data_out, int expect_data_out, int convert_mode)
{
    float   convert_result;
    float   expect_result;
    int error = 0;
    int data_mask   = 0xFFF, data_mask_10 = 0x3FF, data_mask_8 = 0xFF;
    float vref  = 4096, vref_10 = 1024, vref_8 = 256;


    convert_result  = ((data_out + 1) / vref) * 5;

    if (convert_mode == 12)
    {
        expect_result   = (((expect_data_out & data_mask) + 1) / vref) * 5;
    }
    else if (convert_mode == 10)
    {
        expect_result   = (((expect_data_out & data_mask_10) + 1) / vref_10) * 5;
    }
    else if (convert_mode == 8)
    {
        expect_result   = (((expect_data_out & data_mask_8) + 1) / vref_8) * 5;
    }

    if (convert_result != expect_result)
    {
        puts("**Error : convert result != expect result\n");
        error += 1;
    }
    else
    {
        puts("**Success : convert result = expect result\n");
    }

    printf("======expect_result = %f ========\n", expect_result);
    printf("======convert_result = %f =======\n\n", convert_result);

    return  error;
}
















