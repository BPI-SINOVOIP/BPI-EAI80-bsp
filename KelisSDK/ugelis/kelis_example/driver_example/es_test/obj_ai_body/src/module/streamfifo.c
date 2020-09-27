
//#include <common.h>
//#include "stdio.h"
//#include "uart_stdout.h"
#include "streamfifo.h"


//#include "../../../software/common/retarget/dma_660x/dma.h"
//#include "../../../software/common/retarget/dma_660x/dma_dev.h"



int done;
int len;

__IO uint32_t txmem[256];
__IO uint32_t rxmem[256];
//#include "dma_gm6721.c"

int dma_send(unsigned int pkt_length, unsigned int base_address)
{
    streamfifo_init((uint32_t *)base_address, pkt_length, AHB_WRAP4, UNLEN16, NORMAL_MODE);
    return 0;
}

void streamfifo_init(uint32_t *src, uint32_t len, uint8_t burst_type, uint8_t burst_unlen, uint8_t continue_mode)
{

    uint32_t level;
    STRFIFO->CTRL = 0;

    if (burst_type == AHB_WRAP4)
    {
        level = 4 - 1;
    }
    else if (burst_type == AHB_WRAP8)
    {
        level = 8 - 1;
    }
    else if (burst_type == AHB_WRAP16)
    {
        level = 16 - 1;
    }
    else if (burst_type == AHB_SINGLE)
    {
        level = 1 - 1;
    }
    else if (burst_type == AHB_INCR)
    {
        switch (burst_unlen)
        {
            case UNLEN4    :
                level = 4   - 1;
                break;
            case UNLEN8    :
                level = 8   - 1;
                break;
            case UNLEN16   :
                level = 16  - 1;
                break;
            case UNLEN32   :
                level = 32  - 1;
                break;
            case UNLEN64   :
                level = 64  - 1;
                break;
            case UNLEN128  :
                level = 128 - 1;
                break;
            case UNLEN256  :
                level = 256 - 1;
                break;
            case UNLEN512  :
                level = 512 - 1;
                break;
            default :
                break;
        }
    }

    STRFIFO->SRC  = (uint32_t) src;
    if (continue_mode == CONTINUE_MODE)
    {
        STRFIFO->DATACNT = 0x00ffffff;
    }
    else
    {
        STRFIFO->DATACNT = len;
    }
    STRFIFO->CTRL = (1 << 31)             | //[31]   done_ie
                    (1 << 30)             | //[30]   over_ie
                    (1 << 29)             | //[29]   under_ie
                    (0 << 28)             | //[28]   full_ie
                    ((1024 - level) << 8) | //[17:8] dma_level 1024 4096
                    (burst_unlen << 5)    | //[7:5]  burst unlen : 2->len 16
                    (burst_type << 2)     | //[4:2]  burst type : 1->INCR
                    (1 << 1)              | //[1]    internal fifo enable
                    (1 << 0)              ; //[0]    STRFIFO enable


}


#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))

int imp_trans(int src_addr, int src_offset, int drc_addr, int drc_offset, int image_w, int image_h, int rd16_wr32_en, int rd32_wr16_en)
{
    unsigned int  tmp, i, b, c;
    unsigned int  imp_base;
    unsigned int  imp_cr_addr;
    unsigned int  imp_bpc_addr      ;
    unsigned int  imp_isr_addr      ;
    unsigned int  imp_fgmar_addr        ;
    unsigned int  imp_fgor_addr ;
    unsigned int  imp_bgor_addr     ;
    unsigned int  imp_bgmar_addr    ;
    unsigned int  imp_fgpfccr_addr      ;
    unsigned int  imp_fgclor_addr   ;
    unsigned int  imp_bgpfccr_addr;
    unsigned int  imp_bgclor_addr       ;
    unsigned int  imp_opfccr_addr    ;
    unsigned int  imp_ocolr_addr  ;
    unsigned int  imp_omar_addr   ;
    unsigned int  imp_oor_addr   ;
    unsigned int  imp_nlr_addr   ;
    unsigned int  imp_lwr_addr   ;
    unsigned int  imp_amtcr_addr   ;
    unsigned int  imp_ip_ver_addr  ;

    imp_base = 0x40024000;
    imp_cr_addr         = imp_base + 0x00;
    imp_isr_addr            = imp_base + 0x04;
    imp_fgmar_addr          = imp_base + 0x0C;
    imp_fgor_addr           = imp_base + 0x10;
    imp_bgor_addr           = imp_base + 0x18;
    imp_bgmar_addr          = imp_base + 0x14;
    imp_fgpfccr_addr            = imp_base + 0x1C;
    imp_fgclor_addr         = imp_base + 0x20;
    imp_bgpfccr_addr            = imp_base + 0x24;
    imp_bgclor_addr         = imp_base + 0x28;
    imp_opfccr_addr          = imp_base + 0x34;
    imp_ocolr_addr        = imp_base + 0x38;
    imp_omar_addr         = imp_base + 0x3C;
    imp_oor_addr         = imp_base + 0x40;
    imp_nlr_addr         = imp_base + 0x44;
    imp_lwr_addr         = imp_base + 0x48;
    imp_amtcr_addr         = imp_base + 0x4c;
    //release clock and reset
    HW32_REG(0x40000080) = 0xFFFFFFFF;
    HW32_REG(0x40000070) = 0xFFFFFFFF;
    //config fg memory addr
    HW32_REG(imp_fgmar_addr) = src_addr;
    //config fg offset = 0
    HW32_REG(imp_fgor_addr) = src_offset;
    //config fg color mode = argb8888
    //    HW32_REG(imp_fgpfccr_addr)= 0x0;

    if (rd16_wr32_en)
    {
        HW32_REG(imp_fgpfccr_addr) = 0x2;
    }
    else if (rd32_wr16_en)
    {
        HW32_REG(imp_fgpfccr_addr) = 0x0;
    }
    //config out memory addr

    if (rd16_wr32_en)
    {
        HW32_REG(imp_opfccr_addr) = 0;
    }
    else if (rd32_wr16_en)
    {
        HW32_REG(imp_opfccr_addr) = 2;
    }

    //config out memory addr
    HW32_REG(imp_omar_addr) = drc_addr;
    //config out memory offset 0
    HW32_REG(imp_oor_addr) = drc_offset;
    //config out pl/nl
    HW32_REG(imp_nlr_addr) = (image_w << 16) + image_h;
    //    HW32_REG(imp_cr_addr)=0x00001;
    HW32_REG(imp_cr_addr) = (rd16_wr32_en << 20) + (rd32_wr16_en << 21) + (1 << 16) + 1;
    while (HW32_REG(imp_cr_addr) & 1 != 0);
    return 1;
}



/*
int main(void)
{
    int i;

    for(i=0;i<256;i++) {
      txmem[i] = 0x12345678 + i;
    }


    streamfifo_init(txmem, 256, AHB_INCR, UNLEN16, NORMAL_MODE);


    //while((STRFIFO->STATUE)>>8);
    while(!((STRFIFO->STATUE) & (1<<4)));

#if 0
    STRFIFO->SRC  = (uint32_t) txmem;
    STRFIFO->DATACNT = 256;
    STRFIFO->CTRL = (1<<31)          |   //[31]   done_ie
                    (1<<30)          |   //[30]   over_ie
                    (1<<29)          |   //[29]   under_ie
                    (0<<28)          |   //[28]   full_ie
                    ((1024 - 15)<<8) |   //[17:8] dma_level
                    (2<<5)           |   //[7:5]  burst type : 2->len 16
                    (1<<2)           |   //[4:2]  burst type : 1->INCR
                    (1<<1)           |   //[1]    internal fifo enable
                    (1<<0)           ;   //[0]    STRFIFO enable


    len = 256;

    //huge_data_trans(txmem, rxmem, 1256, DMA_WORD_MODE, DMA_WORD_MODE);
    huge_data_trans_int(txmem, rxmem, DMA_WORD_MODE, DMA_WORD_MODE);

    //  DMA_Init_streamfifo_ch1(txmem, &(STRFIFO->DATD), 128, DMA_WORD_MODE, DMA_WORD_MODE);

    //  dma_ch1_enable();

    //  wait_dmac_ch1_done();

    //  clear_ch1_pending();
    //  dma_ch1_disable();

    while(done!=1);
#endif

}
*/
/*
void DMA_COMBINED_Handler(void)
{
  puts("\nDMA1_Handler\n");

  clear_ch1_pending();
  dma_ch1_disable();

  if(len==0) {
     NVIC_DisableIRQ(DMA2_IRQn);
     NVIC_ClearPendingIRQ(DMA2_IRQn);
     done = 1;
  }
  else if(len<=1023) {
    DMA0->CH1_BLK_TS = len;
    len = 0;
    dma_ch1_enable();
  } else {
    DMA0->CH1_BLK_TS = 1023;
    len = len - 1023;
    dma_ch1_enable();
  }
}
*/

