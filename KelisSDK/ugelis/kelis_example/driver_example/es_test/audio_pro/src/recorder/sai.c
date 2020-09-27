#include "sai.h"
#include "pl230_udma.h"
#include <gm_hal_crypto.h>

#if 1

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

void udma_reset_release(void)
{
    HW32_REG(0x40000070) |= (1 << 2);
}

void udma_clk_enable(void)
{
    HW32_REG(0x40000080) |= (1 << 2);
}



void sai_reset_and_clk_release(void)
{
    //sai_a reset
    HW32_REG(0x40000074) &= ~(1 << 24);

    //sai_b reset
    HW32_REG(0x40000074) &= ~(1 << 25);

    //release the sai_a reset
    HW32_REG(0x40000074) |= (1 << 24);

    //release the sai_b reset
    HW32_REG(0x40000074) |= (1 << 25);

    //release the sai_a clock
    HW32_REG(0x40000084) |= (1 << 24);

    //release the sai_b clock
    HW32_REG(0x40000084) |= (1 << 25);

    HW32_REG(0x40000198) |= (1 << 0); //D_PLLSAI_EN

    HW32_REG(0x40000190) |= (1 << 0); //D_PLLI2S_EN

    HW32_REG(0x400000ac) |= (1 << 0); //SAIA_CLK_SEL
    HW32_REG(0x400000ac) |= (1 << 8); //SAIB_CLK_SEL


    CMSDK_SAI_B->SAI_CR1 &= ~(1 << 16);
    CMSDK_SAI_B->SAI_CR2 |= (1 << 3);
    CMSDK_SAI_B->SAI_CR2 &= ~(1 << 3);
    //CMSDK_SAI_B->SAI_CR1 |= (1<<16);

}


void sai_reset_and_clk_release2(void)
{
    //release the sai_a reset
    HW32_REG(0x40000074) |= (1 << 24);

    //release the sai_b reset
    HW32_REG(0x40000074) |= (1 << 25);

    //release the sai_a clock
    HW32_REG(0x40000084) |= (1 << 24);

    //release the sai_b clock
    HW32_REG(0x40000084) |= (1 << 25);

    HW32_REG(0x40000198) |= (1 << 0); //D_PLLSAI_EN

    HW32_REG(0x40000190) |= (1 << 0); //D_PLLI2S_EN

    HW32_REG(0x400000ac) |= (1 << 0); //SAIA_CLK_SEL
    HW32_REG(0x400000ac) |= (1 << 8); //SAIB_CLK_SEL

}



void XPx_AF_sel(uint8_t pad, uint8_t num, uint8_t sel)
{
    if ((pad >= 'A') && (pad <= 'K'))
    {
        pad -= 'A';
    }
    else
    {
        //printf("Error : pad should be 'A' ~ 'K' !!!!");
        //UartEndSimulation();
        return;
    }

    HW32_REG(0x400002c4 + 4 * pad) &= ~(0x3 << (num * 2)); //clear IO mode
    HW32_REG(0x400002c4 + 4 * pad) |= (0x1 << (num * 2)); //select AF mode

    sel &= 0xf;
    if (num < 8) //0~7
    {
        HW32_REG(0x400002f0 + 8 * pad) &= ~(0xf << (num * 4)); //clear
        HW32_REG(0x400002f0 + 8 * pad) |= (sel << (num * 4)); //set AF
    }
    else        //8~15
    {
        num -= 8;
        HW32_REG(0x400002f4 + 8 * pad) &= ~(0xf << (num * 4)); //clear
        HW32_REG(0x400002f4 + 8 * pad) |= (sel << (num * 4)); //set AF
    }
}


void sai_a_pin_mux_config(void)
{
    XPx_AF_sel('A', 0, 5);   //mclk   XPA0
    XPx_AF_sel('A', 3, 13);  //bck    XPA3
    XPx_AF_sel('A', 2, 13);  //fs     XPA2
    XPx_AF_sel('A', 4, 13);  //sdo/i  XPA4
    //
    // group2
    //
    //XPx_AF_sel('G', 13, 5 );  //mclk   XPG13
    //XPx_AF_sel('E', 7 , 5 );  //bck    XPE7
    //XPx_AF_sel('C', 8 , 6 );  //fs     XPC8
    //XPx_AF_sel('B', 15, 5 );  //sdo/i  XPB15
}

void sai_b_pin_mux_config(void)
{
    XPx_AF_sel('B', 7, 5);    //bck    XPB7
    XPx_AF_sel('B', 8, 5);    //fs     XPB8
    XPx_AF_sel('A', 1, 5);    //sdo/i  XPA1
}



void sai_a_master_tx_config_441k(void)
{
    /* init sai para */
    //    uint32_t FTH_A   = 0; // tx : empty
    //    uint32_t FTH_B   = 1; // rx : FIFO < 1/4 but not empty
    //    uint32_t DMAEN   = 1;
    uint32_t COMP    = 0;
    uint32_t PRTCFG  = 0;  //free mode
    uint32_t FSPOL   = 1;  //0: FS is active low (falling edge)  1: FS is active high (rising edge)
    uint32_t CKSTR   = 1;  //0: data strobing edge is falling edge of SCK  1: data strobing edge is rising edge of SCK
    uint32_t MONO    = 0; // tx : stero mode
    uint32_t FSOFF   = 1;   //FSOFF: Frame synchronization offset
    uint32_t FBOFF   = 0;  //FBOFF[4:0]: First bit offset
    uint32_t FSALL   = 32 - 1;
    uint32_t FRL     = 64 - 1;
    uint32_t TRIS    = 0;
    uint32_t SLOTEN  = 0x0003;
    //uint32_t DS    = 0;    //not used
    //uint32_t DS    = 1;    //not used
    //uint32_t DS    = 2;    //8  bit
    //uint32_t DS    = 3;    //10 bit
    uint32_t DS      = 4;    //16 bit
    //uint32_t DS    = 5;    //20 bit
    //uint32_t DS    = 6;    //24 bit
    //uint32_t DS    = 7;    //32 bit
    uint32_t NBSLOT  = 2 - 1;
    uint32_t SLOTSZ  = 2;    //10: 32-bit
    //uint32_t SLOTSZ  = 1;    //01: 16-bit
    //uint32_t SLOTSZ  = 0;    //00: The slot size is equivalent to the data size (specified in DS[3:0] in the SAI_ACR1 register).
    uint32_t MCKNODIV = 0;
    uint32_t MCKDIV = 12 / 3 - 1 ;
    uint32_t BCKDIV = 4 - 1;
    //uint32_t MCKDIV = 12 - 1 ;
    //uint32_t BCKDIV = 4 - 1;

    uint32_t dat;
    //  int i;

    //CMSDK_SAI_A->SAI_CR1 |= (1<<20) | (1<<19) | (1<<18); //MCK_OE, BCK_OE, FS_OE

    //
    // SAI A clock configure
    //
    dat = CMSDK_SAI_A->SAI_CKCTL;
    dat |= (MCKNODIV << 17)     | //MCKNODIV
           (MCKDIV << 8) |
           (BCKDIV << 0) ;
    CMSDK_SAI_A->SAI_CKCTL = dat;

    //
    // SAI_xCR1
    //
    dat = CMSDK_SAI_A->SAI_CR1;
    dat |= (1 << 20)     | //MCK_OE
           (1 << 19)     | //BCK_OE
           (1 << 18)     | //FS_OE
           (MONO << 12)  | (CKSTR << 9) ; //[9] CKSTR
    CMSDK_SAI_A->SAI_CR1 = dat;


    //
    // SAI_xCR2
    //
    dat = CMSDK_SAI_A->SAI_CR2;
    dat |= (COMP << 14) |
           (TRIS << 4) ; //TRIS  Hi-z control
    CMSDK_SAI_A->SAI_CR2 = dat;

    //
    // SAI xSlot register (SAI_xFRCR)
    //
    dat = CMSDK_SAI_A->SAI_FRCR;
    dat |= (FSOFF << 18) | //FSOFF
           (FSPOL << 17) | //FSPOL
           (FSALL << 8)  | //FSALL
           (FRL << 0)    ; //FRL
    CMSDK_SAI_A->SAI_FRCR = dat;


    //
    // SAI_xSLOTR
    //
    dat = CMSDK_SAI_A->SAI_SLOTR;
    dat |= (SLOTEN << 16)  |  //[31:16]SLOTEN
           (NBSLOT << 8)   |  //[11:8] NBSLOT number of slot
           (SLOTSZ << 6)   |  //[7:6]  SLOTSZ  : 32bit
           (FBOFF << 0)   ;   //[4:0]
    CMSDK_SAI_A->SAI_SLOTR = dat;

    //
    // SAI xConfiguration register 1 (SAI_xCR1)
    //
    dat = CMSDK_SAI_A->SAI_CR1;
    dat &= ~(7 << 5); //[7:5] DS : data size
    dat |= (1     << 16) | //[16]  sai_en
           (DS    << 5)  | //[7:5] DS : data size : 16bit
           (PRTCFG << 2)  ; //[3:2] Protocol
    CMSDK_SAI_A->SAI_CR1 = dat;
}

void sai_a_master_tx_config_16k(void)
{
    /* init sai para */
    //    uint32_t FTH_A   = 0; // tx : empty
    //    uint32_t FTH_B   = 1; // rx : FIFO < 1/4 but not empty
    //    uint32_t DMAEN   = 1;
    uint32_t COMP    = 0;
    uint32_t PRTCFG  = 0;  //free mode
    uint32_t FSPOL   = 1;  //0: FS is active low (falling edge)  1: FS is active high (rising edge)
    uint32_t CKSTR   = 1;  //0: data strobing edge is falling edge of SCK  1: data strobing edge is rising edge of SCK
    uint32_t MONO    = 1; // tx : stero mode
    uint32_t FSOFF   = 1;   //FSOFF: Frame synchronization offset
    uint32_t FBOFF   = 0;  //FBOFF[4:0]: First bit offset
    uint32_t FSALL   = 32 - 1;
    uint32_t FRL     = 64 - 1;
    uint32_t TRIS    = 0;
    uint32_t SLOTEN  = 0x0003;
    //uint32_t DS    = 0;    //not used
    //uint32_t DS    = 1;    //not used
    //uint32_t DS    = 2;    //8  bit
    //uint32_t DS    = 3;    //10 bit
    uint32_t DS      = 4;    //16 bit
    //uint32_t DS    = 5;    //20 bit
    //uint32_t DS    = 6;    //24 bit
    //uint32_t DS    = 7;    //32 bit
    uint32_t NBSLOT  = 2 - 1;
    uint32_t SLOTSZ  = 2;    //10: 32-bit
    //uint32_t SLOTSZ  = 1;    //01: 16-bit
    //uint32_t SLOTSZ  = 0;    //00: The slot size is equivalent to the data size (specified in DS[3:0] in the SAI_ACR1 register).
    uint32_t MCKNODIV = 0;

    //uint32_t MCKDIV = 12/3 - 1 ;
    //uint32_t BCKDIV = 4 - 1;
    uint32_t MCKDIV = 12 - 1 ;
    uint32_t BCKDIV = 4 - 1;

    uint32_t dat;
    //  int i;

    //CMSDK_SAI_A->SAI_CR1 |= (1<<20) | (1<<19) | (1<<18); //MCK_OE, BCK_OE, FS_OE

    //
    // SAI A clock configure
    //
    dat = CMSDK_SAI_A->SAI_CKCTL;
    dat |= (MCKNODIV << 17)     | //MCKNODIV
           (MCKDIV << 8) |
           (BCKDIV << 0) ;
    CMSDK_SAI_A->SAI_CKCTL = dat;

    //
    // SAI_xCR1
    //
    dat = CMSDK_SAI_A->SAI_CR1;
    dat |= (1 << 20)     | //MCK_OE
           (1 << 19)     | //BCK_OE
           (1 << 18)     | //FS_OE
           (MONO << 12)  | (CKSTR << 9) ; //[9] CKSTR
    CMSDK_SAI_A->SAI_CR1 = dat;


    //
    // SAI_xCR2
    //
    dat = CMSDK_SAI_A->SAI_CR2;
    dat |= (COMP << 14) |
           (TRIS << 4) ; //TRIS  Hi-z control
    CMSDK_SAI_A->SAI_CR2 = dat;

    //
    // SAI xSlot register (SAI_xFRCR)
    //
    dat = CMSDK_SAI_A->SAI_FRCR;
    dat |= (FSOFF << 18) | //FSOFF
           (FSPOL << 17) | //FSPOL
           (FSALL << 8)  | //FSALL
           (FRL << 0)    ; //FRL
    CMSDK_SAI_A->SAI_FRCR = dat;


    //
    // SAI_xSLOTR
    //
    dat = CMSDK_SAI_A->SAI_SLOTR;
    dat |= (SLOTEN << 16)  |  //[31:16]SLOTEN
           (NBSLOT << 8)   |  //[11:8] NBSLOT number of slot
           (SLOTSZ << 6)   |  //[7:6]  SLOTSZ  : 32bit
           (FBOFF << 0)   ;   //[4:0]
    CMSDK_SAI_A->SAI_SLOTR = dat;

    //
    // SAI xConfiguration register 1 (SAI_xCR1)
    //
    dat = CMSDK_SAI_A->SAI_CR1;
    dat &= ~(7 << 5); //[7:5] DS : data size
    dat |= (1     << 16) | //[16]  sai_en
           (DS    << 5)  | //[7:5] DS : data size : 16bit
           (PRTCFG << 2)  ; //[3:2] Protocol
    CMSDK_SAI_A->SAI_CR1 = dat;
}


void sai_b_slave_rx_config(void)
{
    /* init sai para*/
    //    uint32_t FTH_A   = 0; // tx : empty
    uint32_t FTH_B   = 1; // rx : FIFO < 1/4 but not empty
    uint32_t DMAEN   = 1;
    uint32_t COMP    = 0;
    uint32_t PRTCFG  = 0;  //free mode
    uint32_t FSPOL   = 1;  //0: FS is active low (falling edge)  1: FS is active high (rising edge)
    uint32_t CKSTR   = 1;  //0: data strobing edge is falling edge of SCK  1: data strobing edge is rising edge of SCK
    uint32_t MONO    = 1; // rx, mono mode, 1 channel, xunfei 6 mic board, or 1 mems mic
    uint32_t FSOFF   = 1;   //FSOFF: Frame synchronization offset
    uint32_t FBOFF   = 0;  //FBOFF[4:0]: First bit offset
    uint32_t FSALL   = 32 - 1;
    uint32_t FRL     = 64 - 1;
    uint32_t TRIS    = 0;
    uint32_t SLOTEN  = 0x0003;
    //uint32_t DS    = 0;    //not used
    //uint32_t DS    = 1;    //not used
    //uint32_t DS    = 2;    //8  bit
    //uint32_t DS    = 3;    //10 bit
    uint32_t DS      = 4;    //16 bit
    //uint32_t DS    = 5;    //20 bit
    //uint32_t DS    = 6;    //24 bit
    //uint32_t DS    = 7;    //32 bit

    uint32_t NBSLOT  = 2 - 1;
    uint32_t SLOTSZ  = 2;    //10: 32-bit
    //uint32_t SLOTSZ  = 1;    //01: 16-bit
    //uint32_t SLOTSZ  = 0;    //00: The slot size is equivalent to the data size (specified in DS[3:0] in the SAI_ACR1 register).
    //    uint32_t MCKNODIV = 0;

    uint32_t MCKDIV = 12 / 3 - 1 ;
    uint32_t BCKDIV = 4 - 1;

    //uint32_t MCKDIV = 12 - 1 ;
    //uint32_t BCKDIV = 4 - 1;

    uint32_t dat;
    //  int i;

    //CMSDK_SAI_B->SAI_CR1 |= (1<<20) | (1<<19) | (1<<18); //MCK_OE, BCK_OE, FS_OE

    //
    // SAI A clock configure
    //
    dat = CMSDK_SAI_B->SAI_CKCTL;
    dat |= (1 << 17)     | //MCKNODIV
           (MCKDIV << 8) |
           (BCKDIV << 0) ;
    CMSDK_SAI_B->SAI_CKCTL = dat;

    //
    // SAI_xCR1
    //
    dat = CMSDK_SAI_B->SAI_CR1;
    dat |= (CKSTR << 9) | (MONO << 12); //[9] CKSTR
    CMSDK_SAI_B->SAI_CR1 = dat;


    //
    // SAI_xCR2
    //
    dat = CMSDK_SAI_B->SAI_CR2;
    dat |= (COMP << 14) |
           (TRIS << 4) | //TRIS  Hi-z control
           (FTH_B << 0) ; //[2:0] FTH
    CMSDK_SAI_B->SAI_CR2 = dat;

    //
    // SAI xSlot register (SAI_xFRCR)
    //
    dat = CMSDK_SAI_B->SAI_FRCR;
    dat |= (FSOFF << 18) | //FSOFF
           (FSPOL << 17) | //FSPOL
           (FSALL << 8)  | //FSALL
           (FRL << 0)    ; //FRL
    CMSDK_SAI_B->SAI_FRCR = dat;


    //
    // SAI_xSLOTR
    //
    dat = CMSDK_SAI_B->SAI_SLOTR;
    dat |= (SLOTEN << 16)  |  //[31:16]SLOTEN
           (NBSLOT << 8)   |  //[11:8] NBSLOT number of slot
           (SLOTSZ << 6)   |  //[7:6]  SLOTSZ  : 32bit
           (FBOFF << 0)   ;   //[4:0]
    CMSDK_SAI_B->SAI_SLOTR = dat;

    //
    // SAI xConfiguration register 1 (SAI_xCR1)
    //
    dat = CMSDK_SAI_B->SAI_CR1;
    dat &= ~(7 << 5); //[7:5] DS : data size
    dat |= (DMAEN << 17) | //DMAEN
           (1     << 16) | //[16]  sai_en
           (DS    << 5)  | //[7:5] DS : data size : 16bit
           (PRTCFG << 2)  | //[3:2] Protocol
           (3 << 0)       ; //MODE : slave rx mode

    CMSDK_SAI_B->SAI_CR1 = dat;
}



/*for mems mic*/
void sai_b_master_rx_config(void)
{
    uint32_t FTH_A   = 0; // tx : empty
    //    uint32_t FTH_B   = 1; // rx : FIFO < 1/4 but not empty
    uint32_t DMAEN   = 1;
    uint32_t COMP    = 0;
    uint32_t PRTCFG  = 0;  //free mode
    uint32_t FSPOL   = 0;  //0: FS is active low (falling edge)  1: FS is active high (rising edge)
    uint32_t CKSTR   = 1;  //0: data strobing edge is falling edge of SCK  1: data strobing edge is rising edge of SCK
    uint32_t MONO    = 1; // tx : empty
    uint32_t FSOFF   = 1;   //FSOFF: Frame synchronization offset
    uint32_t FBOFF   = 0;  //FBOFF[4:0]: First bit offset
    uint32_t FSALL   = 32 - 1;
    uint32_t FRL     = 64 - 1;
    uint32_t TRIS    = 0  ;
    uint32_t SLOTEN  = 0x0003;
    //uint32_t DS    = 0;    //not used
    //uint32_t DS    = 1;    //not used
    //uint32_t DS    = 2;    //8  bit
    //uint32_t DS    = 3;    //10 bit
    uint32_t DS      = 4;    //16 bit
    //uint32_t DS    = 5;    //20 bit
    //uint32_t DS    = 6;    //24 bit
    //uint32_t DS    = 7;    //32 bit
    uint32_t NBSLOT  = 2 - 1;
    uint32_t SLOTSZ  = 2;    //10: 32-bit
    //uint32_t SLOTSZ  = 1;    //01: 16-bit
    //uint32_t SLOTSZ  = 0;    //00: The slot size is equivalent to the data size (specified in DS[3:0] in the SAI_ACR1 register).
    uint32_t MCKNODIV = 0;
    uint32_t MCKDIV = 12 - 1 ;   //256fs
    uint32_t BCKDIV =  4 - 1;    //64fs
    uint32_t dat;
    //    int i;

    CMSDK_SAI_B->SAI_CR1 |= (1 << 20) | (1 << 19) | (1 << 18); //MCK_OE, BCK_OE, FS_OE

    //
    // SAI A clock configure
    //
    dat = CMSDK_SAI_B->SAI_CKCTL;
    dat |= (MCKNODIV << 17)     | //MCKNODIV
           (MCKDIV << 8) |
           (BCKDIV << 0) ;
    CMSDK_SAI_B->SAI_CKCTL = dat;

    //
    // SAI_xCR1
    //
    dat = CMSDK_SAI_B->SAI_CR1;
    dat |= (1 << 20)     | //MCK_OE
           (1 << 19)     | //BCK_OE
           (1 << 18)     | //FS_OE
           (CKSTR << 9) | (MONO << 12);; //[9] CKSTR
    CMSDK_SAI_B->SAI_CR1 = dat;


    //
    // SAI_xCR2
    //
    dat = CMSDK_SAI_B->SAI_CR2;
    dat |= (COMP << 14) |
           (TRIS << 4)  | //TRIS  Hi-z control
           (FTH_A << 0) ; //[2:0] FTH
    CMSDK_SAI_B->SAI_CR2 = dat;

    //
    // SAI xSlot register (SAI_xFRCR)
    //
    dat = CMSDK_SAI_B->SAI_FRCR;
    dat |= (FSOFF << 18) | //FSOFF
           (FSPOL << 17) | //FSPOL
           (FSALL << 8)  | //FSALL
           (FRL << 0)    ; //FRL
    CMSDK_SAI_B->SAI_FRCR = dat;


    //
    // SAI_xSLOTR
    //
    dat = CMSDK_SAI_B->SAI_SLOTR;
    dat |= (SLOTEN << 16)  |  //[31:16]SLOTEN
           (NBSLOT << 8)   |  //[11:8] NBSLOT number of slot
           (SLOTSZ << 6)   |  //[7:6]  SLOTSZ  : 32bit
           (FBOFF << 0)   ;   //[4:0]
    CMSDK_SAI_B->SAI_SLOTR = dat;


    //
    // write data to fifo
    //
#if 0

#endif

    //
    // SAI xConfiguration register 1 (SAI_xCR1)
    //
    dat = CMSDK_SAI_B->SAI_CR1;
    dat &= ~(7 << 5); //[7:5] DS : data size
    dat |= (DMAEN << 17) | //DMAEN
           (1     << 16) | //[16]  sai_en
           (DS    << 5)  | //[7:5] DS : data size : 16bit
           (1    << 0)  |
           (PRTCFG << 2)  ; //[3:2] Protocol
    CMSDK_SAI_B->SAI_CR1 = dat;
}

#endif


DMA_Handle_T saiRxDMAHandle;
DMA_Handle_T saiTxDMAHandle;

extern int read_complete;
extern struct k_sem rec_data_lock;
static void SAI_RxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
#if 1
    //UART_Handle_T *hUART = NULL;

    //__ASSERT(hDMA != NULL, "");

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        read_complete = 1;
        //k_sem_give(&rec_data_lock);
        //SAI_Read_Start(buf_wav, wav_data_len);
        //HAL_DMA_Abort(hDMA);// todo
    }
#endif
}

static void UART_TxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
#if 0
    UART_Handle_T *hUART = NULL;

    __ASSERT(hDMA != NULL, "");

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        hUART = CONTAINER_OF(hDMA, UART_Handle_T, TxDMAHandle);
        if (hUART == NULL)
        {
            return;
        }

        hUART->txDMAStatus = UART_DMA_FINISHED;
        hUART->txSize += hUART->tcount;
        hUART->tcount = 0;
        if (hUART->txExpectedSize == hUART->txSize)
        {
            hUART->status &= ~UART_STATUS_TX_BUSY;
            if (NULL != hUART->callback)
            {
                hUART->callback(hUART, UART_EVENT_TX_OVER, (ulong_t)0);
            }
        }

        HAL_DMA_Abort(hDMA);
    }
#endif
}


void SAI_DMAConfig(CMSDK_SAI_TypeDef *pSAI, uint8_t dmaRead, DMA_Handle_T *pDMAHandle)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_ERROR;


    //__ASSERT((pSAI != NULL) && (pDMAHandle != NULL), "");

    if (dmaRead)
    {
        // todo
        //ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_SAI1_B, &reqResult);

        if (ret != RET_OK)
        {
            //fail, need return fail status
            return;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));
        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
        dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
        dmaConfig.width = DMA_WIDTH_16BIT;
        dmaConfig.burst = 0;
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PRIMARY;
        dmaConfig.callback = SAI_RxDMACallback;
#if 0//SAI DMA EN    
        LL_SAI_RX_DMA_RDL(pSAI, 0);
        LL_SAI_RX_DMA_ENABLE(pSAI);
        LL_SAI_TX_DMA_DISABLE(pSAI);
#endif
    }
    else
    {
        // todo
        //ret = HAL_DMA_Request_Channel(DMA_DEV, DMA_REQUEST_SAI1_A, &reqResult);

        if (ret != RET_OK)
        {
            //fail, need return fail status
            return;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));
        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
        dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
        dmaConfig.width = DMA_WIDTH_16BIT;
        dmaConfig.burst = 0;
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PRIMARY;
        dmaConfig.callback = UART_TxDMACallback;
#if 0//SAI DMA EN
        LL_SAI_TX_DMA_TDL(pSAI, MAX_UART_TX_FIFO_SIZE / 2);
        LL_SAI_TX_DMA_ENABLE(pSAI);
        LL_SAI_RX_DMA_DISABLE(pSAI);
#endif
    }

    // todo
    //HAL_DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);
}

void SAI_Read_Start(uint8_t *data_buf, uint32_t data_len)
{
    // todo
    //HAL_DMA_Start(&saiRxDMAHandle, (uint32_t)&(CMSDK_SAI_B->SAI_DR), (uint32_t)(data_buf), data_len);
}

void SAI_Write_Start(uint8_t *data_buf, uint32_t data_len)
{
    // todo
    HAL_DMA_Start(&saiTxDMAHandle, (uint32_t) & (CMSDK_SAI_A->SAI_DR), (uint32_t)(data_buf), data_len);
}



//SAI Block B初始化,I2S,飞利浦标准
//mode:工作模式,可以设置:SAI_MODEMASTER_TX/SAI_MODEMASTER_RX/SAI_MODESLAVE_TX/SAI_MODESLAVE_RX
//cpol:数据在时钟的上升/下降沿选通，可以设置：SAI_CLOCKSTROBING_FALLINGEDGE/SAI_CLOCKSTROBING_RISINGEDGE
//datalen:数据大小,可以设置：SAI_DATASIZE_8/10/16/20/24/32

#define MASTER_RX 0
void SAIB_Init()
{
    sai_b_pin_mux_config();
    sai_reset_and_clk_release();

#if MASTER_RX
    sai_b_master_rx_config();
#else
    sai_b_slave_rx_config();
#endif
}

//SAI Block B初始化,I2S,飞利浦标准
//mode:工作模式,可以设置:SAI_MODEMASTER_TX/SAI_MODEMASTER_RX/SAI_MODESLAVE_TX/SAI_MODESLAVE_RX
//cpol:数据在时钟的上升/下降沿选通，可以设置：SAI_CLOCKSTROBING_FALLINGEDGE/SAI_CLOCKSTROBING_RISINGEDGE
//datalen:数据大小,可以设置：SAI_DATASIZE_8/10/16/20/24/32
void SAIA_Init()
{

    sai_a_pin_mux_config();
    sai_reset_and_clk_release();
    sai_a_master_tx_config_16k();

    //    SAI_DMAConfig(CMSDK_SAI_A, FALSE, &saiTxDMAHandle);
}


//开启SAIB的DMA功能,HAL库没有提供此函数
//因此我们需要自己操作寄存器编写一个
void SAIA_DMA_Enable(void)
{

}

//开启SAIB的DMA功能,HAL库没有提供此函数
//因此我们需要自己操作寄存器编写一个
void SAIB_DMA_Enable(void)
{

}
//设置SAIA的采样率(@MCKEN)
//samplerate:采样率,单位:Hz
//返回值:0,设置成功;1,无法设置.
u8 SAIA_SampleRate_Set(u32 samplerate)
{
    /*rx don't need set sample rate*/

    /*tx need set sample rate*/
}
//SAIA TX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
//width:位宽(存储器和外设,同时设置),0,8位;1,16位;2,32位;
void SAIA_TX_DMA_Init(u8 *buf0, u8 *buf1, u16 num, u8 width)
{


}

//SAI DMA回调函数指针
void (*sai_tx_callback)(void);  //TX回调函数
void (*sai_rx_callback)(void);  //RX回调函数

//SAIA TX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
//width:位宽(存储器和外设,同时设置),0,8位;1,16位;2,32位;


void SAIB_RX_DMA_Init(u8 *buf0, u8 *buf1, u16 num, u8 width)
{
}

//SAI开始播放
void SAI_Play_Start(void)
{
    //todo __HAL_DMA_ENABLE(&SAI1_TXDMA_Handler);//开启DMA TX传输
}
//关闭I2S播放
void SAI_Play_Stop(void)
{
    //todo __HAL_DMA_DISABLE(&SAI1_TXDMA_Handler);  //结束播放
}
//SAI开始录音
void SAI_Rec_Start(void)
{
    //HAL_IRQ_ENABLE(4);//开启 DMA RX 传输
}
//关闭SAI录音
void SAI_Rec_Stop(void)
{
    //HAL_IRQ_DISABLE(4);//停止 DMA RX 传输
}

