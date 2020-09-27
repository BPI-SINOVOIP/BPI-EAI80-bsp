//#include "CMSDK_CM4.h"
#include <stdio.h>
#include "pl230_udma.h"
#include "sys_sai.h"



//uint8_t ReservedArray[512] __attribute__((at(0x20000400)));
//uint8_t ReservedArray[768] __attribute__((at(0x200003C0)));

#if 0
/* Pointers to DMA reserved ram area */
char dma_ram_reserve[1024] __attribute__((aligned(512))) = {0};
//DMA_Control_T *p_dmaCtrl = (DMA_Control_T*)__dma_ram_start;
PL230_UDMA_Control_T UDMA_Control = dma_ram_reserve;
#endif

//PL230_UDMA_Control_T   UDMA_Control  __attribute__((at(0x20000400)));
PL230_UDMA_Control_T   UDMA_Control  __attribute__((aligned(512)));


void UDMA_init(uint8_t ch, uint8_t sel, uint8_t high, uint8_t alter, uint32_t *src_end, uint32_t *dst_end, int len, uint8_t src_tr_width, uint8_t mem2mem, uint8_t no_inc, uint8_t inten, uint8_t burst_len)
{
    uint8_t burst_en;

    ch  &= 0xf;
    len &= 0xffffff; //the max len

    if (burst_len == NO_BURST)
    {
        burst_en = 0;
    }
    else
    {
        burst_en = 1;
    }


    if (alter)
    {
        //Alternate control data struct config
        UDMA_Control.Alter_CH[ch].SRC_END = (uint32_t) src_end;

        UDMA_Control.Alter_CH[ch].DES_END = (uint32_t) dst_end;

        UDMA_Control.Alter_CH[ch].CTRL = ((len - 1) << 8)     | //[31:8] n_minus_1
                                         (src_tr_width << 6)  | //[7:6] src size
                                         (burst_len << 4)     | //[5:4] R_power
                                         (burst_en << 3)      | //[3] burst
                                         (0x0 << 0);           //[2:0] trans

        if (mem2mem)
        {
            UDMA_Control.Alter_CH[ch].CTRL |= (2 << 0);    //auto request
        }
        else
        {
            UDMA_Control.Alter_CH[ch].CTRL |= (1 << 0);    //base trans
        }

    }
    else
    {
        //Primary control data struct config
        UDMA_Control.Prim_CH[ch].SRC_END = (uint32_t) src_end;

        UDMA_Control.Prim_CH[ch].DES_END = (uint32_t) dst_end;

        UDMA_Control.Prim_CH[ch].CTRL = ((len - 1) << 8)     | //[31:8] n_minus_1
                                        (src_tr_width << 6)  | //[7:6] src size
                                        (burst_len << 4)     | //[5:4] R_power
                                        (burst_en << 3)      | //[3] burst
                                        (0 << 0);             //[2:0] base trans

        if (mem2mem)
        {
            UDMA_Control.Prim_CH[ch].CTRL |= (2 << 0);    //auto request
        }
        else
        {
            UDMA_Control.Prim_CH[ch].CTRL |= (1 << 0);    //base trans
        }
    }


    //Primary control data struct base address
    UDMA->CTRL_BASE_PTR = (uint32_t)(&(UDMA_Control.Prim_CH[0]));

    //Alternate control data struct base address
    UDMA->ALT_CTRL_BASE_PTR = (uint32_t)(&(UDMA_Control.Alter_CH[0]));

    if (mem2mem)   //memory to memory
    {
        //Channel request mask set : disable the channel req
        UDMA->CHNL_REQ_MASK_SET = (1 << ch);
    }
    else
    {
        //Channel request mask clear : enable the channel req
        UDMA->CHNL_REQ_MASK_CLR = (1 << ch);
    }

    //use the Alter data struct or Primary data struct
    if (alter)
    {
        UDMA->CHNL_PRI_ALT_SET = (1 << ch);
    }
    else
    {
        UDMA->CHNL_PRI_ALT_CLR = (1 << ch);
    }

    //set the high priority of the channel
    if (high)
    {
        UDMA->CHNL_PRIORITY_SET = (1 << ch);
    }
    else
    {
        UDMA->CHNL_PRIORITY_CLR = (1 << ch);
    }

    //Bus error clear
    UDMA->ERR_CLR = (1 << 0);

    //master handshake select
    if (ch < 8)
    {
        sel &= 0xf;
        UDMA->CH_CFG0 &= ~(0xf << (ch * 4));
        UDMA->CH_CFG0 |= (sel << (ch * 4));
    }
    else
    {
        sel &= 0xf;
        UDMA->CH_CFG1 &= ~(0xf << ((ch - 8) * 4));
        UDMA->CH_CFG1 |= (sel << ((ch - 8) * 4));
    }

    //config the src/dst address change or not
    UDMA->NO_INC &= ~(3 << (ch * 2));
    UDMA->NO_INC |= (no_inc << (ch * 2));

    if (inten)
    {
        UDMA->IE |= (1 << ch);
    }
    else
    {
        UDMA->IE &= ~(1 << ch);
    }

    //enable the channel
    UDMA->CHNL_ENABLE_SET = (1 << ch);

    //enable udma
    UDMA->DMA_CFG |= (1 << 0);

    if (mem2mem)   //memory to memory
    {
        UDMA->CHNL_SW_REQUEST |= (1 << ch);
    }
}


void wait_udma_done(uint8_t ch, uint8_t alter)
{
#if 0
    if (alter)
    {
        while ((UDMA_Control.Alter_CH[ch].CTRL & 0x7)); //wait CTRL[2:0] to be 0
    }
    else
    {
        while ((UDMA_Control.Prim_CH[ch].CTRL & 0x7)); //wait CTRL[2:0] to be 0
    }
#else
    while (!(UDMA->INTFLAG & (1 << ch))); //wait done flag
    UDMA->INTFLAG &= ~(1 << ch);        //clear done flag
#endif
    //printf("chnel %d is done\n", ch);
}
