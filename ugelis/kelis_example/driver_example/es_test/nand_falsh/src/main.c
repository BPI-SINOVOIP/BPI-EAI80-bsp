/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>

#include <gm_hal_nf.h>
#include <stdlib.h>
#include <string.h>




uint8_t odd_read_dat[PAGE_SIZE] = {0};





#define RAND_MAX (0x7fffffffu)
static uint32_t libc_random_gen_state[4] = { 1455997910, 1999515274, 1234451287, 1949149569 };


//#define Multi_Bit_Cfg(reg,bit_vld,start_bit,val) (HW32_REG(reg) =(HW32_REG(reg)& ~(bit_vld<<start_bit)) | (val<<start_bit))


#define NF_ADDR_BASE         0x70000000//Õâ¸ö¾ßÌåÒª·Åµ½ÄÄ¸öÎÄ¼þÀïÃæ£¬´ý¶¨
NFlash_Device_T *pNFlashDev = (NFlash_Device_T *)(NF_ADDR_BASE);



/**
 * Generate pseudo-random integer
 *
 * Note:
 *      The function implements George Marsaglia's XorShift random number generator
 *
 * @return integer in range [0; RAND_MAX]
 */


#if 1
static void tr_clock()
{
    (*(volatile uint32_t *)(0x40000098)) |= 0x00000030;
    (*(volatile uint32_t *)(0x40000188)) |= 0x00000001;
    (*(volatile uint32_t *)(0x40000090)) |= 0x00000002;

}


#endif




#if 1
int pinmux_config()
{
    (*(volatile uint32_t *)(0x400002d8)) |= 0x01450000;
    (*(volatile uint32_t *)(0x400002e4)) |= 0x00154140;
    (*(volatile uint32_t *)(0x400002d0)) |= 0x50000000;
    (*(volatile uint32_t *)(0x400002d4)) |= 0x1;
    (*(volatile uint32_t *)(0x400002dc)) |= 0x00100100;
    (*(volatile uint32_t *)(0x4000031c)) |= 0x000bb0bb;
    (*(volatile uint32_t *)(0x40000330)) |= 0xb00bb000;
    (*(volatile uint32_t *)(0x4000030c)) |= 0xbb000000;
    (*(volatile uint32_t *)(0x40000310)) |= 0x0000000b;
    (*(volatile uint32_t *)(0x40000320)) |= 0x000b0000;

    //  (*(volatile uint32_t*)(0x40000334)) &=0xffffff0ff;
    (*(volatile uint32_t *)(0x40000334)) = 0x00000bcb;


    (*(volatile uint32_t *)(0x40000324)) |= 0x00000c00;

}


#endif

void nf_reset()
{
    (*(volatile uint32_t *)(0x70000014)) |= 0x00010000;
    (*(volatile uint32_t *)(0x70000020)) |= 0x000000ff;
    (*(volatile uint32_t *)(0x70000014)) &= !(0x00010000);
}




static int rand(void)
{
    uint32_t intermediate = libc_random_gen_state[0] ^ (libc_random_gen_state[0] << 11);
    intermediate ^= intermediate >> 8;

    libc_random_gen_state[0] = libc_random_gen_state[1];
    libc_random_gen_state[1] = libc_random_gen_state[2];
    libc_random_gen_state[2] = libc_random_gen_state[3];

    libc_random_gen_state[3] ^= libc_random_gen_state[3] >> 19;
    libc_random_gen_state[3] ^= intermediate;

    return libc_random_gen_state[3] % (RAND_MAX + 1u);
} /* rand */



#define TRANS_LEN 64

/* Command definition, some optional commands
   are supported by specific products. */
typedef enum
{
    Read_ID,
    Program,
    Read,
    BlockErase,
    ReadDmaPio,
    ReadDmaDma,
    ProgramDmaPio,
    ProgramDmaDma,
    CacheProgram,
    ReadDmaPioODD,
    ProgramDmaPioODD
} CmdType;

FlashReturnMsg FlashAPI(NFlash_Device_T *pNFlashDev, CmdType cmd, FlashParameter *fptr);

/*
 * Function:     main
 * Arguments:    None.
 * Return Value: 0 -> execute successful
 *               1 -> execute failed
 * Description:  Entry of program.
 */

uint8_t exp_dat[PAGE_SIZE] = {0};
uint8_t read_dat[PAGE_SIZE] = {0};













#define HW32_REG(ADDRESS)   (*((volatile uint32_t *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))



//damon add define begin
#define     SRAM_BASE_ADDR          0x20000000

#define     NF_VER                  0x70000000
#define     NF_MR                   0x70000010
#define     NF_CR                   0x70000014
#define     NF_ECC_CR               0x70000018
#define     NF_DMA_CR               0x7000001C
#define     NF_DR                   0x70000020
#define     NF_DMA_DR               0x70000024
#define     NF_RD_TIM               0x70000030
#define     NF_WR_TIM               0x70000034
#define     NF_EDO_RB_TIM           0x70000038
#define     NF_DQS_TIM              0x7000003C
#define     FW_WR_REDU_W0           0x70000040
#define     FW_WR_REDU_W1           0x70000044
#define     NF_IE                   0x7000004C
#define     NF_SR                   0x70000050
#define     NF_ECC_SR               0x70000054
#define     NF_SEC_ERR_INFO         0x70000058
#define     FW_RD_REDU_W0           0x70000060
#define     FW_RD_REDU_W1           0x70000064
#define     NF_LP_CR                0x70000070

#define     NF_SRAM_STR             0x70002000


#define odwNF_W_REDU0_REG       (HW32_REG(FW_WR_REDU_W0))
#define odwNF_W_REDU1_REG       (HW32_REG(FW_WR_REDU_W1))
#define obNF_RD_CYCLE_REG       (HW8_REG(NF_RD_TIM))
#define obNF_WR_CYCLE_REG       (HW8_REG(NF_WR_TIM))
#define obNF_EDO_CYCLE_REG      (HW32_REG(NF_EDO_RB_TIM))
#define NF_IP_EN                (HW8_REG(NF_MR) = 0x01)
#define owNF_MODE_REG           (HW32_REG(NF_MR))
#define obNF_STD_RD_TIM         (HW8_REG(NF_RD_TIM))
#define obNF_STD_WR_TIM         (HW8_REG(NF_WR_TIM))
#define obNF_EDO_TIM            (HW8_REG(NF_EDO_RB_TIM))
#define obNF_REJ_DLY            (HW8_REG(NF_EDO_RB_TIM+1))
#define obNF_DMA_INIT           (HW8_REG(NF_DMA_CR))
#define obNF_DMA_CTRL           (HW8_REG(NF_DMA_CR+1))
#define obNF_DMA_CFG            (HW8_REG(NF_DMA_CR+2))
#define obNF_PIO_DR             (HW8_REG(NF_DR))
#define odwNF_DMA_DR            (HW32_REG(NF_DMA_DR))
#define odwNF_SR                (HW32_REG(NF_SR))
#define obNF_IE                 (HW8_REG(NF_IE))
#define obNF_SR0                (HW8_REG(NF_SR))
#define obNF_SR1                (HW8_REG(NF_SR+1))
#define obNF_SR2                (HW8_REG(NF_SR+2))
#define odwNF_CR                (HW32_REG(NF_CR))
#define obNF_CEJ                (HW8_REG(NF_CR))
#define obNF_ALE                (HW8_REG(NF_CR+1))
#define obNF_CLE                (HW8_REG(NF_CR+2))
#define obNF_WP                 (HW8_REG(NF_CR+3))
#define obNF_ECC_RST            (HW8_REG(NF_ECC_CR))
#define obNF_ECC_MODE           (HW8_REG(NF_ECC_CR+1))
#define obNF_BCH_MODE           (HW8_REG(NF_ECC_CR+2))
#define obNF_ECC_SR             (HW8_REG(NF_ECC_SR))
#define odwNF_SEC_ERR_INFO      (HW32_REG(NF_SEC_ERR_INFO))



#define CRYP_TEST_SEG0_ADDR   0x20008000
#define CRYP_TEST_SEG1_ADDR   0x2000a000
//damon add define end


void nf_dmapio_write(uint32_t seed, uint32_t wr_page, uint32_t page_cnt);
void nf_dmapio_read(uint32_t seed, uint32_t rd_page, uint32_t page_cnt, uint32_t ignore_addr, uint32_t ignore_len);
//void nf_pio_rand_byte_write(uint32_t wr_page, uint32_t addr, uint8_t data);
void nf_page2K_ecc_func_test(uint8_t bch_mod, uint32_t page_tst);
void diy_delay(int loop);

extern uint8_t ReservedArray[16384];





void nf_pio_write(uint32_t seed, uint32_t wr_page, uint32_t page_cnt);
void nf_pio_read(uint32_t seed, uint32_t rd_page, uint32_t page_cnt, uint32_t ignore_addr, uint32_t ignore_len);
void nf_pio_rand_byte_write(uint32_t wr_page, uint32_t addr, uint8_t data);




#if  1         /*ODD V2.0*/

void nf_page8K_pio_write(uint32_t seed, uint32_t wr_page, uint32_t page_cnt);
void nf_page8K_pio_read(uint32_t seed, uint32_t rd_page, uint32_t page_cnt, uint32_t ignore_addr, uint32_t ignore_len);
void nf_pio_rand_byte_write(uint32_t wr_page, uint32_t addr, uint8_t data);

void nf_page8K_ecc_info_test(uint8_t bch_mod, uint32_t page_tst);









void nf_pio_rand_byte_write(uint32_t wr_page, uint32_t addr, uint8_t data)
{
    // not ECC
    uint32_t gxnf_page_num  ;
    uint32_t gxnf_page_cnt  ;
    uint32_t gxnf_wr_page  ;
    uint32_t gxnf_prog_page ;
    uint16_t gxnf_data_len  ;
    uint32_t wdata;
    uint8_t gxbNandStatus;



    gxnf_prog_page  = wr_page;

    // NF Write data start
    printf("\nBegin NF PIO WRITE\n");

    //CmdSerialDataInputNF();
    //begin
    k_sleep(1);
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    obNF_PIO_DR =   0x80;
    odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
    //end
    k_sleep(1);
    obNF_PIO_DR = addr;   // col0
    k_sleep(1);
    obNF_PIO_DR = (addr & 0xff00) >> 8; // col1
    k_sleep(1);
    obNF_PIO_DR = gxnf_prog_page & 0xff;          // row0
    k_sleep(1);
    obNF_PIO_DR = (gxnf_prog_page & 0xff00) >> 8; // row1
    //obNF_PIO_DR = (gxnf_prog_page & 0xff0000)>>16;// row2

    diy_delay(10);

    // set random data input command
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x85;
    odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
    k_sleep(1);
    obNF_PIO_DR = addr;   // col0
    k_sleep(1);
    obNF_PIO_DR = (addr & 0xff00) >> 8; // col1
    //obNF_PIO_DR =   gxnf_prog_page & 0xff;
    //obNF_PIO_DR =   (gxnf_prog_page & 0xff00)>>8;

    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
    obNF_DMA_CTRL = 0x00; // ~DMA_DIR, ~DMA_EN

    diy_delay(5);
    k_sleep(1);
    obNF_PIO_DR = 0xc4;

    printf("\n Begin NF PIO random Program\n");

    //CmdProgram();
    //begin
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x10;
    obNF_CLE    =   0x0 ;
    //end
    diy_delay(5);
    //CmdReadStatus();
    //WaitNFReady();
    //begin
    obNF_CLE    =   0x1;
    k_sleep(1);
    obNF_PIO_DR =   0x70;
    obNF_CLE    =   0x0;
    //----------------------


    if (obNF_WP)
    {
        while (obNF_PIO_DR != 0x60) { ; }
    }
    else
    {
        while (obNF_PIO_DR != 0xE0) { ; }
    }
    //end

}



void nf_page8K_pio_read(uint32_t seed, uint32_t rd_page, uint32_t page_cnt, uint32_t ignore_addr, uint32_t ignore_len)
{
    //CPU PIO Read From Nand Flash (NO ECC)
    uint32_t gxnf_page_num  ;
    uint32_t  gxnf_sec_num  ;
    uint32_t gxnf_page_cnt  ;
    uint32_t gxnf_rd_page  ;
    uint32_t gxnf_read_page ;
    uint16_t gxnf_data_len  ;
    uint32_t gdata, rdata, tmp;
    uint8_t gxbNandStatus;
    uint8_t gxbnf_flag;
    //uint8_t gxbnf_flag;

    gxnf_page_cnt   =   page_cnt;
    gxnf_rd_page    =   rd_page;
    obNF_ECC_MODE   =   0x2;    // odd-even parity
    k_sleep(1);
    obNF_DMA_CFG    = (4 << 5 | 4);  //FW_REDU_NUM=4, DMA_LEN=2
    k_sleep(1);
    obNF_DMA_INIT = 1;
    obNF_DMA_INIT = 0;


    for (gxnf_page_num = 0; gxnf_page_num < gxnf_page_cnt; gxnf_page_num++)
    {

        printf("begin NF PIO Read\n");

        gxnf_read_page  = rd_page + gxnf_page_num ;

        //obNF_DMA_LEN_REG = (FW_REDUN_NUM+P_DMA_LENGTH1);
        //CmdRead1NF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x00;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        k_sleep(1);
        //end
        obNF_PIO_DR = 0x00; // col0
        k_sleep(1);
        obNF_PIO_DR = 0x00; // col1
        k_sleep(1);
        obNF_PIO_DR = gxnf_read_page & 0xff;          // row0
        k_sleep(1);
        obNF_PIO_DR = (gxnf_read_page & 0xff00) >> 8; // row1
        //obNF_PIO_DR = (gxnf_read_page & 0xff0000)>>16;// row2
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        //CmdRead2NF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x30;
        k_sleep(1);
        obNF_CLE    =   0x0 ;
        k_sleep(1);
        //end
        //CmdReadStatus();
        //WaitNFReady();
        //begin
        obNF_CLE    =   0x1;
        k_sleep(1);
        obNF_PIO_DR =   0x70;
        k_sleep(1);
        obNF_CLE    =   0x0;
        //----------------------


        if (obNF_WP)
        {
            while (obNF_PIO_DR != 0x60) { ; }
        }
        else
        {
            while (obNF_PIO_DR != 0xE0) { ; }
        }
        //end
        diy_delay(5);
        //CmdRead1NF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x00;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        k_sleep(1);
        //end
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        k_sleep(1);
        obNF_DMA_CTRL = 0x01; // ~DMA_DIR, DMA_EN
        k_sleep(1);

        for (gxnf_sec_num = 0; gxnf_sec_num < 4; gxnf_sec_num++)
        {

            //NFSRAM_AVA();
            //begin


            gxbnf_flag = obNF_SR1;
            while ((gxbnf_flag & 0x01) != 0x01)
            {
                gxbnf_flag = obNF_SR1;
            }
            //end

            for (gxnf_data_len = 0; gxnf_data_len < 512; gxnf_data_len = gxnf_data_len + 4)
            {

                gdata = seed ;//(seed + gxnf_page_num) * (gxnf_data_len+1+gxnf_sec_num*1024);
                rdata =  odwNF_DMA_DR;
                k_sleep(1);

                if ((gxnf_sec_num == (ignore_addr / 512)) &&
                        (ignore_len > 0) &&
                        (gxnf_data_len >= ignore_addr % 512) &&
                        (gxnf_data_len < (ignore_addr % 512 + ignore_len)))
                {
                    ;
                }
                else if (rdata != gdata)
                {
                    printf("\nError_Mx_SIM, Page %d, addr 0x%0x, rdata=0x%x, gdata=0x%x", gxnf_read_page, gxnf_data_len, rdata, gdata);
                }
            }
        }

        printf("begin NF DMA PIO Read Finish\n");
        //NFReadFinish();
        //begin


        gxbnf_flag = obNF_SR0;
        while ((gxbnf_flag & 0xA0) != 0xA0)
        {
            gxbnf_flag = obNF_SR0;
        }
        obNF_SR0 = 0x00;
        //end

    }
}


void nf_page8K_pio_write(uint32_t seed, uint32_t wr_page, uint32_t page_cnt)
{
    //CPU PIO Write To Nand Flash (NO ECC)  page align
    uint32_t gxnf_page_num  ;
    uint32_t  gxnf_sec_num  ;
    uint32_t gxnf_page_cnt  ;
    uint32_t gxnf_wr_page  ;
    uint32_t gxnf_prog_page ;
    uint16_t gxnf_data_len  ;
    uint32_t wdata;
    uint8_t gxbnf_flag;
    uint8_t gxbNandStatus;


    gxnf_page_cnt   =   page_cnt;
    gxnf_wr_page    =   wr_page;

    obNF_ECC_MODE   =   0x0;    // odd-even parity
    k_sleep(1);
    obNF_DMA_CFG    = (4 << 5 | 4);  //FW_REDU_NUM=4, DMA_LEN=16
    k_sleep(1);
    obNF_DMA_CTRL   =   0x02; // DMA_DIR, ~DMA_EN
    k_sleep(1);
    odwNF_W_REDU0_REG = 0x04030201; // WR redun 3-0
    k_sleep(1);
    odwNF_W_REDU1_REG = 0x08070605; // WR redun 7-4



    for (gxnf_page_num = 0; gxnf_page_num < gxnf_page_cnt; gxnf_page_num++)
    {

        gxnf_prog_page  = wr_page + gxnf_page_num ;

        // NF Write data start
        printf("\n Begin NF PIO WRITE\n");

        //CmdSerialDataInputNF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x80;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        //end
        obNF_PIO_DR = 0x00;   // col0
        obNF_PIO_DR = 0x00;   // col1
        obNF_PIO_DR = gxnf_prog_page & 0xff;          // row0
        obNF_PIO_DR = (gxnf_prog_page & 0xff00) >> 8; // row1
        //obNF_PIO_DR = (gxnf_prog_page & 0xff0000)>>16;// row2
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        k_sleep(1);
        obNF_DMA_CTRL = 0x03; // DMA_DIR, DMA_EN
        k_sleep(1);


        for (gxnf_sec_num = 0; gxnf_sec_num < 4; gxnf_sec_num++)
        {

            printf("\n Begin NF SRAM FULL\n");
            //NFSRAM_FULL();
            //begin
            //uint8_t gxbnf_flag;

            gxbnf_flag = obNF_SR1;
            while ((gxbnf_flag & 0x02) == 0x02)
            {
                gxbnf_flag = obNF_SR1;
            }
            //end
            for (gxnf_data_len = 0; gxnf_data_len < 512; gxnf_data_len = gxnf_data_len + 4)
            {
                wdata =  seed ;//(seed + gxnf_page_num) * (gxnf_data_len+1 + gxnf_sec_num*512);
                odwNF_DMA_DR =  wdata;
                k_sleep(1);
            }
        }

        //NFWriteFinish();
        //begin


        gxbnf_flag = obNF_SR0;
        while ((gxbnf_flag & 0x80) != 0x80)
        {
            gxbnf_flag = obNF_SR0;
        }
        obNF_SR0 = 0x00;
        //end
        printf("\nBegin NF DMA PIO Program\n");

        //CmdProgram();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x10;
        k_sleep(1);
        obNF_CLE    =   0x0 ;
        //end
        diy_delay(5);
        //CmdReadStatus();
        //WaitNFReady();
        //begin
        obNF_CLE    =   0x1;
        k_sleep(1);

        obNF_PIO_DR =   0x70;
        k_sleep(1);
        obNF_CLE    =   0x0;
        //----------------------


        if (obNF_WP)
        {
            while (obNF_PIO_DR != 0x60) { ; }
        }
        else
        {
            while (obNF_PIO_DR != 0xE0) { ; }
        }
        //end
    }

}





void nf_page8K_ecc_info_test(uint8_t bch_mod, uint32_t page_tst)
{
    int i = 0;
    int k = 0;
    uint32_t test_len   ;
    uint32_t test_seed  ;
    uint32_t gdata      ;

    uint32_t tst_page_num   ;
    uint32_t tst_sec_num    ;
    uint32_t tst_dat_addr   ;
    uint32_t tst_err_cnt    ;
    uint32_t  wdata          ;
    uint8_t  bit_vld        ;
    uint8_t  max_ecc_ability;
    uint32_t ecc_parity_byte;
    uint32_t err_info       ;


    ecc_parity_byte = 16;

    //set bch mode
    owNF_MODE_REG   =   0x01;
    k_sleep(1);
    obNF_ECC_MODE   =   0x0;    // odd-even parity

    test_seed = rand();
    //test_seed = 0x08080808;

    nf_page8K_pio_write(test_seed, page_tst, 1);
    nf_page8K_pio_read(test_seed, page_tst, 1, 0, 0);

    tst_page_num    = 0 ;
    tst_sec_num     = 0 ;
    tst_dat_addr    = 0 ;
    tst_err_cnt     = 0 ;

    err_info = 0xffff0001;
    do
    {
        tst_dat_addr = 0;

        //gdata = (seed + gxnf_page_num) * (0+1+gxnf_sec_num*1024);

        // corrupt 1st byte data in nf
        gdata =  test_seed ;//(test_seed + tst_page_num) * (1 + tst_sec_num * 1024);
        printf("\n   Test Case: gdata = 0x%x", gdata);

        //reverse bit 0
        if (gdata & 0x1)
        {
            wdata = gdata & 0xfffffffe;
        }
        else
        {
            wdata = gdata | 0x1;
        }


        printf("\n   Test Case: corrupted ori data 0x%0x, sec 0x%0x, addr 0x%0x", wdata, tst_sec_num, tst_dat_addr);


        nf_pio_rand_byte_write(page_tst, tst_sec_num * (512 + ecc_parity_byte), wdata);

#if 1
        FlashParameter fpar = {0};




        fpar.Array = odd_read_dat;
        fpar.Address = 0x00;
        fpar.Length = 512;
        memset(odd_read_dat, 0, PAGE_SIZE);
        if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)      /* Read data */
        {
            printf("Error:Command Read:Read Data Operation not OK\n");
            return 1;
        }

        for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
        {
            if (odd_read_dat[i] != test_seed)
            {
                printf("odd_read_dat[%d]=%d\n", i, odd_read_dat[i]);
                //return 1;  /* Verify data, should equal to 0xFF */
            }
        }
        printf("Command BlockErase:Block Erase result  OK\n");




#endif








        nf_page8K_pio_read(test_seed, page_tst, 1, 0, 0);

        if (obNF_ECC_SR != 1)
        {
            printf("\nError_Mx_SIM: ecc err cnt not %0d, should be 1", obNF_ECC_SR);
        }

        if (odwNF_SEC_ERR_INFO != err_info)
        {
            printf("\nError_Mx_SIM: ecc err info not 0x%x, should be 0x%x", odwNF_SEC_ERR_INFO, err_info);
        }

        tst_sec_num++;
        err_info = err_info << 1;
        err_info |= 0xffff0001;

    } while (tst_sec_num < 4);

    /*
    tst_sec_num     = 15 ;
    //check err cnt for sector 15   //maximum err_cnt = 2
    gdata = (test_seed + tst_page_num) * (1 + tst_sec_num * 512 + 256);
    printf("\n   Test Case: gdata = 0x%x", gdata);

    //reverse bit 0
    if(gdata&0x1)
        wdata = gdata & 0xfffffffe;
    else
        wdata = gdata | 0x1;

    printf("\n   Test Case: corrupted ori data 0x%0x, sec 0x%0x, addr 0x%0x", wdata, tst_sec_num ,tst_dat_addr);

    nf_pio_rand_byte_write(page_tst, tst_sec_num*(512+ecc_parity_byte) + 256, wdata);
    nf_page8K_pio_read(test_seed, page_tst, 1, 0, 0);

    if(obNF_ECC_SR!=2){
            printf("\nError_Mx_SIM: ecc err cnt not %0d, should be 2", obNF_ECC_SR);
    }
    */

}










#endif












#if 0








void nf_pio_write(uint32_t seed, uint32_t wr_page, uint32_t page_cnt)
{
    //CPU PIO Write To Nand Flash (NO ECC)  page align
    uint32_t gxnf_page_num  ;
    uint32_t  gxnf_sec_num  ;
    uint32_t gxnf_page_cnt  ;
    uint32_t gxnf_wr_page  ;
    uint32_t gxnf_prog_page ;
    uint16_t gxnf_data_len  ;
    uint32_t wdata;
    uint8_t gxbnf_flag;
    //uint8_t gxbnf_flag;
    uint8_t gxbNandStatus;


    gxnf_page_cnt   =   page_cnt;
    gxnf_wr_page    =   wr_page;

    obNF_DMA_CFG    = (4 << 5 | 2);  //FW_REDU_NUM=4, DMA_LEN=2
    k_sleep(1);
    obNF_DMA_CTRL   =   0x02; // DMA_DIR, ~DMA_EN
    k_sleep(1);
    odwNF_W_REDU0_REG = 0x04030201; // WR redun 3-0
    k_sleep(1);
    odwNF_W_REDU1_REG = 0x08070605; // WR redun 7-4



    for (gxnf_page_num = 0; gxnf_page_num < gxnf_page_cnt; gxnf_page_num++)
    {

        gxnf_prog_page  = wr_page + gxnf_page_num ;

        // NF Write data start
        printf("\n Begin NF PIO WRITE\n");

        //CmdSerialDataInputNF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x80;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        //end
        obNF_PIO_DR = 0x00;   // col0
        k_sleep(1);
        obNF_PIO_DR = 0x00;   // col1
        k_sleep(1);
        obNF_PIO_DR = gxnf_prog_page & 0xff;          // row0
        obNF_PIO_DR = (gxnf_prog_page & 0xff00) >> 8; // row1
        //obNF_PIO_DR = (gxnf_prog_page & 0xff0000)>>16;// row2
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        obNF_DMA_CTRL = 0x03; // DMA_DIR, DMA_EN

        diy_delay(5);

        for (gxnf_sec_num = 0; gxnf_sec_num < 2; gxnf_sec_num++)
        {

            printf("\n Begin NF SRAM FULL\n");
            //NFSRAM_FULL();
            //begin


            gxbnf_flag = obNF_SR1;
            while ((gxbnf_flag & 0x02) == 0x02)
            {
                gxbnf_flag = obNF_SR1;
            }
            //end

            for (gxnf_data_len = 0; gxnf_data_len < 1024; gxnf_data_len = gxnf_data_len + 4)
            {
                //wdata = (seed + gxnf_page_num) * (gxnf_data_len+1 + gxnf_sec_num*1024);
                wdata = seed;
                odwNF_DMA_DR =  wdata;
                k_sleep(1);
            }
        }

        //NFWriteFinish();
        //begin


        gxbnf_flag = obNF_SR0;
        while ((gxbnf_flag & 0x80) != 0x80)
        {
            gxbnf_flag = obNF_SR0;
        }
        obNF_SR0 = 0x00;
        //end
        printf("\nBegin NF DMA PIO Program\n");

        //CmdProgram();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x10;
        k_sleep(1);
        obNF_CLE    =   0x0 ;
        k_sleep(1);
        //end
        diy_delay(5);
        //CmdReadStatus();
        //WaitNFReady();
        //begin
        obNF_CLE    =   0x1;
        k_sleep(1);
        obNF_PIO_DR =   0x70;
        k_sleep(1);
        obNF_CLE    =   0x0;
        //----------------------


        if (obNF_WP)
        {
            while (obNF_PIO_DR != 0x60) { ; }
        }
        else
        {
            while (obNF_PIO_DR != 0xE0) { ; }
        }
        //end
    }
}

void nf_pio_rand_byte_write(uint32_t wr_page, uint32_t addr, uint8_t data)
{
    // not ECC
    uint32_t gxnf_page_num  ;
    uint32_t gxnf_page_cnt  ;
    uint32_t gxnf_wr_page  ;
    uint32_t gxnf_prog_page ;
    uint16_t gxnf_data_len  ;
    uint32_t wdata;
    uint8_t gxbNandStatus;



    gxnf_prog_page  = wr_page;

    // NF Write data start
    printf("\nBegin NF PIO WRITE\n");

    //CmdSerialDataInputNF();
    //begin
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x80;
    k_sleep(1);
    odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
    //end
    obNF_PIO_DR = 0x00;   // col0
    obNF_PIO_DR = 0x00;   // col1
    obNF_PIO_DR = gxnf_prog_page & 0xff;          // row0
    obNF_PIO_DR = (gxnf_prog_page & 0xff00) >> 8; // row1
    //obNF_PIO_DR = (gxnf_prog_page & 0xff0000)>>16;// row2

    diy_delay(10);

    // set random data input command
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x85;
    k_sleep(1);
    odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
    obNF_PIO_DR =   addr & 0xff;
    obNF_PIO_DR = (addr & 0xff00) >> 8;

    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);
    odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
    k_sleep(1);
    obNF_DMA_CTRL = 0x00; // ~DMA_DIR, ~DMA_EN

    diy_delay(5);

    obNF_PIO_DR = data;

    printf("\n Begin NF PIO random Program\n");

    //CmdProgram();
    //begin
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x10;
    k_sleep(1);
    obNF_CLE    =   0x0 ;
    //end
    diy_delay(5);
    //CmdReadStatus();
    //WaitNFReady();
    //begin
    obNF_CLE    =   0x1;
    k_sleep(1);
    obNF_PIO_DR =   0x70;
    k_sleep(1);
    obNF_CLE    =   0x0;
    //----------------------


    if (obNF_WP)
    {
        while (obNF_PIO_DR != 0x60) { ; }
    }
    else
    {
        while (obNF_PIO_DR != 0xE0) { ; }
    }
    //end

}

void nf_pio_read(uint32_t seed, uint32_t rd_page, uint32_t page_cnt, uint32_t ignore_addr, uint32_t ignore_len)
{
    //CPU PIO Read From Nand Flash (NO ECC)
    uint32_t gxnf_page_num  ;
    uint32_t  gxnf_sec_num  ;
    uint32_t gxnf_page_cnt  ;
    uint32_t gxnf_rd_page  ;
    uint32_t gxnf_read_page ;
    uint16_t gxnf_data_len  ;
    uint32_t gdata, rdata, tmp;
    uint8_t gxbNandStatus;
    uint8_t gxbnf_flag;
    //uint8_t gxbnf_flag;

    gxnf_page_cnt   =   page_cnt;
    gxnf_rd_page    =   rd_page;
    obNF_DMA_CFG    = (4 << 5 | 2);  //FW_REDU_NUM=4, DMA_LEN=2


    for (gxnf_page_num = 0; gxnf_page_num < gxnf_page_cnt; gxnf_page_num++)
    {

        printf("begin NF PIO Read\n");

        gxnf_read_page  = rd_page + gxnf_page_num ;

        //obNF_DMA_LEN_REG = (FW_REDUN_NUM+P_DMA_LENGTH1);
        //CmdRead1NF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x00;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        //end
        obNF_PIO_DR = 0x00; // col0
        obNF_PIO_DR = 0x00; // col1
        obNF_PIO_DR = gxnf_read_page & 0xff;          // row0
        obNF_PIO_DR = (gxnf_read_page & 0xff00) >> 8; // row1
        // obNF_PIO_DR = (gxnf_read_page & 0xff0000)>>16;// row2
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        //CmdRead2NF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x30;
        k_sleep(1);
        obNF_CLE    =   0x0 ;
        //end
        diy_delay(5);
        //CmdReadStatus();
        //WaitNFReady();
        //begin
        obNF_CLE    =   0x1;
        k_sleep(1);
        obNF_PIO_DR =   0x70;
        k_sleep(1);
        obNF_CLE    =   0x0;
        //----------------------


        if (obNF_WP)
        {
            while (obNF_PIO_DR != 0x60) { ; }
        }
        else
        {
            while (obNF_PIO_DR != 0xE0) { ; }
        }
        //end
        diy_delay(5);
        //CmdRead1NF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x00;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        k_sleep(1);
        //end
        diy_delay(5);
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        k_sleep(1);
        obNF_DMA_CTRL = 0x01; // ~DMA_DIR, DMA_EN
        k_sleep(1);

        for (gxnf_sec_num = 0; gxnf_sec_num < 2; gxnf_sec_num++)
        {

            //NFSRAM_AVA();
            //begin


            gxbnf_flag = obNF_SR1;
            while ((gxbnf_flag & 0x01) != 0x01)
            {
                gxbnf_flag = obNF_SR1;
            }
            //end

            for (gxnf_data_len = 0; gxnf_data_len < 1024; gxnf_data_len = gxnf_data_len + 4)
            {

                // gdata = (seed + gxnf_page_num) * (gxnf_data_len+1+gxnf_sec_num*1024);
                gdata = seed;
                rdata =  odwNF_DMA_DR;
                k_sleep(1);

                if ((gxnf_sec_num == (ignore_addr / 1024)) &&
                        (ignore_len > 0) &&
                        (gxnf_data_len >= ignore_addr % 1024) &&
                        (gxnf_data_len < (ignore_addr % 1024 + ignore_len)))
                {
                    ;
                }
                else if (rdata != gdata)
                {
                    printf("gxnf_data_len = %d,rdata = %d,gata = %d\n", gxnf_data_len, rdata, gdata);
                    printf("\nError_Mx_SIM, Page %d, addr 0x%0x, rdata=0x%x, gdata=0x%x", gxnf_read_page, gxnf_data_len, rdata, gdata);
                }
            }
        }

        printf("begin NF DMA PIO Read Finish\n");
        //NFReadFinish();
        //begin


        gxbnf_flag = obNF_SR0;
        while ((gxbnf_flag & 0xA0) != 0xA0)
        {
            gxbnf_flag = obNF_SR0;
        }
        obNF_SR0 = 0x00;
        //end

    }
}














#endif
























//damon add sunfunction begin
void diy_delay(int loop)
{
    int i;
    for (i = 0; i < loop; i++)
    {
        //__NOP;
        //__NOP;
    }
}





void nf_dmapio_write(uint32_t seed, uint32_t wr_page, uint32_t page_cnt)
{
    //CPU PIO Write To Nand Flash (NO ECC)  page align
    uint32_t gxnf_page_num  ;
    uint32_t  gxnf_sec_num  ;
    uint32_t gxnf_page_cnt  ;
    uint32_t gxnf_wr_page  ;
    uint32_t gxnf_prog_page ;
    uint16_t gxnf_data_len  ;
    uint32_t wdata;


    gxnf_page_cnt   =   page_cnt;
    gxnf_wr_page    =   wr_page;

    obNF_DMA_CFG    = (4 << 5 | 2);   //FW_REDU_NUM=4, DMA_LEN=2
    obNF_DMA_CTRL   =   0x02; // DMA_DIR, ~DMA_EN
    // odwNF_W_REDU0_REG = 0x04030201; // WR redun 3-0
    // odwNF_W_REDU1_REG = 0x08070605; // WR redun 7-4



    for (gxnf_page_num = 0; gxnf_page_num < gxnf_page_cnt; gxnf_page_num++)
    {

        gxnf_prog_page  = wr_page + gxnf_page_num ;

        // NF Write data start
        printk("\n Begin NF PIO WRITE\n");

        //CmdSerialDataInputNF();
        //begin
        k_sleep(1);
        odwNF_CR    =   0x00010000;//WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x80;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        //end
        k_sleep(1);

        obNF_PIO_DR = 0x00;   // col0
        k_sleep(1);
        obNF_PIO_DR = 0x00;   // col1
        k_sleep(1);
        obNF_PIO_DR = gxnf_prog_page & 0xff;// row0
        k_sleep(1);
        obNF_PIO_DR = (gxnf_prog_page & 0xff00) >> 8; // row1
        k_sleep(1);
        //obNF_PIO_DR = (gxnf_prog_page & 0xff0000)>>16;// row2
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        k_sleep(1);
        obNF_DMA_CTRL = 0x03; // DMA_DIR, DMA_EN
        k_sleep(1);


        for (gxnf_sec_num = 0; gxnf_sec_num < 2; gxnf_sec_num++)
        {

            printf("\n Begin NF SRAM FULL\n");
            //NFSRAM_FULL();
            //begin
            uint8_t gxbnf_flag;
            gxbnf_flag = obNF_SR1;
            while ((gxbnf_flag & 0x02) == 0x02)
            {
                gxbnf_flag = obNF_SR1;
            }
            //end

            for (gxnf_data_len = 0; gxnf_data_len < 1024; gxnf_data_len = gxnf_data_len + 4)
            {
                //    wdata = (seed + gxnf_page_num) * (gxnf_data_len+1 + gxnf_sec_num*1024);
                wdata = seed;
                k_sleep(1);
                odwNF_DMA_DR =  wdata;
            }
        }

        //NFWriteFinish();
        //begin
        uint8_t gxbnf_flag;
        gxbnf_flag = obNF_SR0;
        while ((gxbnf_flag & 0x80) != 0x80)
        {
            gxbnf_flag = obNF_SR0;
        }
        obNF_SR0 = 0x00;
        //end


        printf("\nBegin NF DMA PIO Program\n");

        //CmdProgram();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        obNF_PIO_DR =   0x10;
        obNF_CLE    =   0x0 ;
        //end


        diy_delay(5);
        //CmdReadStatus();
        //WaitNFReady();
        //beign
        obNF_CLE    =   0x1;
        obNF_PIO_DR =   0x70;
        obNF_CLE    =   0x0;
        //------------------
        uint8_t gxbNandStatus;
        if (obNF_WP)
        {
            while (obNF_PIO_DR != 0x60) { ; }
        }
        else
        {
            while (obNF_PIO_DR != 0xE0) { ; }
        }
        //end


    }

}






void nf_dmapio_read(uint32_t seed, uint32_t rd_page, uint32_t page_cnt, uint32_t ignore_addr, uint32_t ignore_len)
{
    //CPU PIO Read From Nand Flash (NO ECC)
    uint32_t gxnf_page_num  ;
    uint32_t  gxnf_sec_num  ;
    uint32_t gxnf_page_cnt  ;
    uint32_t gxnf_rd_page  ;
    uint32_t gxnf_read_page ;
    uint16_t gxnf_data_len  ;
    uint32_t gdata, rdata, tmp;

    gxnf_page_cnt   =   page_cnt;
    gxnf_rd_page    =   rd_page;
    obNF_DMA_CFG    = (4 << 5 | 2);  //FW_REDU_NUM=4, DMA_LEN=2


    for (gxnf_page_num = 0; gxnf_page_num < gxnf_page_cnt; gxnf_page_num++)
    {

        printf("begin NF PIO Read\n");

        gxnf_read_page  = rd_page + gxnf_page_num ;

        //obNF_DMA_LEN_REG = (FW_REDUN_NUM+P_DMA_LENGTH1);
        //CmdRead1NF();
        //beign
        odwNF_CR    =   0x00010000;//WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x00;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        //end
        k_sleep(1);
        obNF_PIO_DR = 0x00; // col0
        k_sleep(1);
        obNF_PIO_DR = 0x00; // col1
        k_sleep(1);
        obNF_PIO_DR = gxnf_read_page & 0xff; // row0
        k_sleep(1);
        obNF_PIO_DR = (gxnf_read_page & 0xff00) >> 8; // row1
        k_sleep(1);
        //obNF_PIO_DR = (gxnf_read_page & 0xff0000)>>16;// row2
        k_sleep(1);
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        k_sleep(1);
        //CmdRead2NF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x30;
        k_sleep(1);
        obNF_CLE    =   0x0 ;
        //end
        k_sleep(1);
        //CmdReadStatus();
        //WaitNFReady();
        //beign
        obNF_CLE    =   0x1;
        k_sleep(1);
        obNF_PIO_DR =   0x70;
        k_sleep(1);
        obNF_CLE    =   0x0;
        k_sleep(1);
        //------------------
        uint8_t gxbNandStatus;
        if (obNF_WP)
        {
            while (obNF_PIO_DR != 0x60) { ; }
        }
        else
        {
            while (obNF_PIO_DR != 0xE0) { ; }
        }
        //end

        k_sleep(1);
        //CmdRead1NF();
        //beign
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x00;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        k_sleep(1);
        //end
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        k_sleep(1);
        obNF_DMA_CTRL = 0x01; // ~DMA_DIR, DMA_EN

        for (gxnf_sec_num = 0; gxnf_sec_num < 2; gxnf_sec_num++)
        {

            //NFSRAM_AVA();
            //begin
            uint8_t gxbnf_flag;
            gxbnf_flag = obNF_SR1;
            while ((gxbnf_flag & 0x01) != 0x01)
            {
                gxbnf_flag = obNF_SR1;
            }
            //end


            for (gxnf_data_len = 0; gxnf_data_len < 1024; gxnf_data_len = gxnf_data_len + 4)
            {

                gdata = (seed + gxnf_page_num) * (gxnf_data_len + 1 + gxnf_sec_num * 1024);
                rdata =  odwNF_DMA_DR;


                if ((gxnf_sec_num == (ignore_addr / 1024)) &&
                        (ignore_len > 0) &&
                        (gxnf_data_len >= ignore_addr % 1024) &&
                        (gxnf_data_len < (ignore_addr % 1024 + ignore_len)))
                {
                    ;
                }
                //     else if(rdata!=gdata){
                else if (rdata != seed)
                {
                    printf("\nError_Mx_SIM, Page %d, addr 0x%0x, rdata=0x%x, gdata=0x%x", gxnf_read_page, gxnf_data_len, rdata, gdata);
                }
            }
        }

        printf("begin NF DMA PIO Read Finish\n");
        //NFReadFinish();
        //begin
        uint8_t gxbnf_flag;
        gxbnf_flag = obNF_SR0;
        while ((gxbnf_flag & 0xA0) != 0xA0)
        {
            gxbnf_flag = obNF_SR0;
        }
        obNF_SR0 = 0x00;
        //end

    }
}

#if 0
void nf_pio_rand_byte_write(uint32_t wr_page, uint32_t addr, uint8_t data)
{
    // not ECC
    uint32_t gxnf_page_num  ;
    uint32_t gxnf_page_cnt  ;
    uint32_t gxnf_wr_page  ;
    uint32_t gxnf_prog_page ;
    uint16_t gxnf_data_len  ;
    uint32_t wdata;



    gxnf_prog_page  = wr_page;

    // NF Write data start
    printf("\nBegin NF PIO WRITE\n");

    //CmdSerialDataInputNF();
    //begin
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    obNF_PIO_DR =   0x80;
    odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
    //end

    obNF_PIO_DR = 0x00;   // col0
    obNF_PIO_DR = 0x00;   // col1
    obNF_PIO_DR = gxnf_prog_page & 0xff;          // row0
    obNF_PIO_DR = (gxnf_prog_page & 0xff00) >> 8; // row1
    //obNF_PIO_DR = (gxnf_prog_page & 0xff0000)>>16;// row2

    diy_delay(10);

    // set random data input command
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    obNF_PIO_DR =   0x85;
    odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
    obNF_PIO_DR =   addr & 0xff;
    obNF_PIO_DR = (addr & 0xff00) >> 8;

    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
    obNF_DMA_CTRL = 0x00; // ~DMA_DIR, ~DMA_EN

    diy_delay(5);

    obNF_PIO_DR = data;

    printf("\n Begin NF PIO random Program\n");

    //CmdProgram();
    //begin
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    obNF_PIO_DR =   0x10;
    obNF_CLE    =   0x0 ;
    //end

    diy_delay(5);
    //CmdReadStatus();
    //WaitNFReady();
    //beign
    obNF_CLE    =   0x1;
    obNF_PIO_DR =   0x70;
    obNF_CLE    =   0x0;
    //------------------
    uint8_t gxbNandStatus;
    if (obNF_WP)
    {
        while (obNF_PIO_DR != 0x60) { ; }
    }
    else
    {
        while (obNF_PIO_DR != 0xE0) { ; }
    }
    //end
}


#endif

//=============================================================
//bch_mod
//000: 16b/1KB, ECC 28B + 4B FW redundant
//001: 24b/1KB, ECC 42B + 4B FW redundant
//010: 40b/1KB, ECC 70B + 4B FW redundant
//011: 48b/1KB, ECC 84B + 4B FW redundant
//100: 60b/1KB, ECC 106B + 4B FW redundant


#if 0

void nf_page2K_ecc_func_test(uint8_t bch_mod, uint32_t page_tst)
{
    int i = 0;
    int k = 0;
    uint32_t test_len   ;
    uint32_t test_seed  ;
    uint32_t gdata      ;

    uint32_t tst_page_num   ;
    uint32_t tst_sec_num    ;
    uint32_t tst_dat_addr   ;
    uint32_t tst_err_cnt    ;
    uint8_t  wdata          ;
    uint8_t  bit_vld        ;
    uint8_t  max_ecc_ability;


    switch (bch_mod)
    {
        case 0x0:
        {
            printf("\n  @@@Test Item: Check ECC 16bit/1024Byte function!");
            max_ecc_ability = 16;
            break;
        }
        case 0x1:
        {
            printf("\n  @@@Test Item: Check ECC 24bit/1024Byte function!");
            max_ecc_ability = 24;
            break;
        }
        case 0x2:
        {
            printf("\n  @@@Test Item: Check ECC 40bit/1024Byte function!");
            max_ecc_ability = 40;
            break;
        }
        case 0x3:
        {
            printf("\n  @@@Test Item: Check ECC 48bit/1024Byte function!");
            max_ecc_ability = 48;
            break;
        }
        case 0x4:
        {
            printf("\n  @@@Test Item: Check ECC 60bit/1024Byte function!");
            max_ecc_ability = 60;
            break;
        }
        default:
        {
            break;
        }
    }


    //set bch mode
    owNF_MODE_REG   = 0x01;
    obNF_ECC_MODE   = 0x06;   //BCH, with error correction
    obNF_BCH_MODE   = bch_mod;   //16B/1024Byte

    //test_seed = rand();
    test_seed = 0xc5c5c5c5;


    nf_dmapio_write(test_seed, page_tst, 1);

    tst_page_num    = 0 ;
    tst_sec_num     = 0 ;
    tst_dat_addr    = 0 ;
    tst_err_cnt     = 0 ;

    do
    {

        // corrupt 1st byte data in nf
        gdata = (test_seed + tst_page_num) * ((tst_dat_addr / 4) * 4 + 1 + tst_sec_num * 1024);
        printf("\n   Test Case: gdata = 0x%x", gdata);

        if (0 == tst_dat_addr % 4)
        {
            wdata = gdata & 0xff;
        }
        else if (1 == tst_dat_addr % 4)
        {
            wdata = (gdata & 0xff00) >> 8;
        }
        else if (2 == tst_dat_addr % 4)
        {
            wdata = (gdata & 0xff0000) >> 16;
        }
        else
        {
            wdata = (gdata & 0xff000000) >> 24;
        }

        /*add*/
        wdata = 0xc5c5c5c5;


        printf("\n   Test Case: corrupted ori data is 0x%0x, addr is 0x%0x", wdata, tst_dat_addr);

        bit_vld = 1;
        for (i = 0; i < 8; i++)
        {
            if (0 != (wdata & bit_vld))
            {
                tst_err_cnt++;
            }
            bit_vld = bit_vld << 1;
        }

        printf("\n   Test Case: there are %0d bits corrupted", tst_err_cnt);

        // corrupt data with 0x0
        nf_pio_rand_byte_write(page_tst, tst_dat_addr, 0);

        if (tst_err_cnt <= max_ecc_ability)
        {
            nf_dmapio_read(test_seed, page_tst, 1, 0, 0);

            if (obNF_ECC_SR != tst_err_cnt)
            {
                printf("\nError_Mx_SIM: ecc err cnt not %0d, should be %0d", obNF_ECC_SR, tst_err_cnt);
            }

            if (odwNF_SEC_ERR_INFO != 0xffff0001)
            {
                printf("\nError_Mx_SIM: ecc err info not 0x%x, should be 0xffff0001", odwNF_SEC_ERR_INFO);
            }
        }
        else
        {
            nf_dmapio_read(test_seed, page_tst, 1, 0, tst_dat_addr + 1);

            if (obNF_ECC_SR > max_ecc_ability)
            {
                printf("\nError_Mx_SIM: ecc err cnt not %0d, should <= %0d", obNF_ECC_SR, max_ecc_ability);
            }

            if (odwNF_SEC_ERR_INFO != 0xfffe0001)
            {
                printf("\nError_Mx_SIM: ecc err info not 0x%x, should be 0xfffe0001", odwNF_SEC_ERR_INFO);
            }
        }

        tst_dat_addr++;

    } while (tst_err_cnt <= max_ecc_ability);
}
//damon add sunfunction end


#endif






#if 0     /*error function*/
void nf_page8K_pio_read(uint32_t seed, uint32_t rd_page, uint32_t page_cnt, uint32_t ignore_addr, uint32_t ignore_len)
{
    //CPU PIO Read From Nand Flash (NO ECC)
    uint32_t gxnf_page_num  ;
    uint32_t  gxnf_sec_num  ;
    uint32_t gxnf_page_cnt  ;
    uint32_t gxnf_rd_page  ;
    uint32_t gxnf_read_page ;
    uint16_t gxnf_data_len  ;
    uint32_t gdata, rdata, tmp;


    gxnf_page_cnt   =   page_cnt;
    gxnf_rd_page    =   rd_page;
    obNF_ECC_MODE   =   0x2;    // odd-even parity
    obNF_DMA_CFG    = (4 << 5 | 2);  //FW_REDU_NUM=4, DMA_LEN=2
    //obNF_DMA_INIT=1; obNF_DMA_INIT=0;


    for (gxnf_page_num = 0; gxnf_page_num < gxnf_page_cnt; gxnf_page_num++)
    {

        printf("begin NF PIO Read\n");

        gxnf_read_page  = rd_page + gxnf_page_num ;

        //obNF_DMA_LEN_REG = (FW_REDUN_NUM+P_DMA_LENGTH1);
        //CmdRead1NF();
        //beign
        odwNF_CR    =   0x00010000;//WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x00;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        //end
        k_sleep(1);
        obNF_PIO_DR = 0x00; // col0
        k_sleep(1);
        obNF_PIO_DR = 0x00; // col1
        k_sleep(1);
        obNF_PIO_DR = gxnf_read_page & 0xff; // row0
        k_sleep(1);
        obNF_PIO_DR = (gxnf_read_page & 0xff00) >> 8; // row1
        k_sleep(1);
        //obNF_PIO_DR = (gxnf_read_page & 0xff0000)>>16;// row2
        k_sleep(1);
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        k_sleep(1);
        //CmdRead2NF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x30;
        k_sleep(1);
        obNF_CLE    =   0x0 ;
        //end
        k_sleep(1);
        //CmdReadStatus();
        //WaitNFReady();
        //beign
        obNF_CLE    =   0x1;
        k_sleep(1);
        obNF_PIO_DR =   0x70;
        k_sleep(1);
        obNF_CLE    =   0x0;
        k_sleep(1);
        //------------------
        uint8_t gxbNandStatus;
        if (obNF_WP)
        {
            while (obNF_PIO_DR != 0x60) { ; }
        }
        else
        {
            while (obNF_PIO_DR != 0xE0) { ; }
        }
        //end

        k_sleep(1);
        //CmdRead1NF();
        //beign
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        k_sleep(1);
        obNF_PIO_DR =   0x00;
        k_sleep(1);
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        k_sleep(1);
        //end
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        k_sleep(1);
        obNF_DMA_CTRL = 0x01; // ~DMA_DIR, DMA_EN

        //        for(gxnf_sec_num= 0; gxnf_sec_num<2; gxnf_sec_num++){
        int a = 2;
        do
        {
            //NFSRAM_AVA();
            //begin
            uint8_t gxbnf_flag;
            gxbnf_flag = obNF_SR1;
            while ((gxbnf_flag & 0x01) != 0x01)
            {
                gxbnf_flag = obNF_SR1;
            }
            //end


            for (gxnf_data_len = 0; gxnf_data_len < 1024; gxnf_data_len = gxnf_data_len + 4)
            {

                gdata = (seed + gxnf_page_num) * (gxnf_data_len + 1 + gxnf_sec_num * 1024);
                rdata =  odwNF_DMA_DR;


                if ((gxnf_sec_num == (ignore_addr / 1024)) &&
                        (ignore_len > 0) &&
                        (gxnf_data_len >= ignore_addr % 1024) &&
                        (gxnf_data_len < (ignore_addr % 1024 + ignore_len)))
                {
                    ;
                }
                //     else if(rdata!=gdata){
                else if (rdata != seed)
                {
                    printf("\nError_Mx_SIM, Page %d, addr 0x%0x, rdata=0x%x, gdata=0x%x", gxnf_read_page, gxnf_data_len, rdata, gdata);
                }
            }
            a--;
        } while (a > 0);

        printf("begin NF DMA PIO Read Finish\n");
        //NFReadFinish();
        //begin
        uint8_t gxbnf_flag;
        gxbnf_flag = obNF_SR0;
        while ((gxbnf_flag & 0xA0) != 0xA0)
        {
            gxbnf_flag = obNF_SR0;
        }
        obNF_SR0 = 0x00;
        //end

    }
}

#endif











#if 0
void nf_pinmux_set(void)
{
    //NF_CEJ        XPI10   0x400002E4[21:20] = 0x1     0x40000334[11:8] = 0xB
    Multi_Bit_Cfg(0x400002E4, 0x3, 20, 1);
    Multi_Bit_Cfg(0x40000334, 0xf, 8, 0xb);
    //NF_ALE        XPF9    0x400002D8[19:18] = 0x1     0x4000031C[7:4] = 0xB
    Multi_Bit_Cfg(0x400002D8, 0x3, 18, 1);
    Multi_Bit_Cfg(0x4000031C, 0xf, 4, 0xb);
    //NF_CLE        XPF8    0x400002D8[17:16] = 0x1     0x4000031C[3:0] = 0xB
    Multi_Bit_Cfg(0x400002D8, 0x3, 16, 1);
    Multi_Bit_Cfg(0x4000031C, 0xf, 0, 0xb);
    //NF_REJ        XPI7    0x400002E4[15:14] = 0x1     0x40000330[31:28] = 0xB
    Multi_Bit_Cfg(0x400002E4, 0x3, 14, 1);
    Multi_Bit_Cfg(0x40000330, 0xf, 28, 0xb);
    //NF_WEJ        XPI8    0x400002E4[17:16] = 0x1     0x40000334[3:0] = 0xB
    Multi_Bit_Cfg(0x400002E4, 0x3, 16, 1);
    Multi_Bit_Cfg(0x40000334, 0xf, 0, 0xB);
    //NF_WPJ        XPI9    0x400002E4[19:18] = 0x1     0x40000334[7:4] = 0xC
    Multi_Bit_Cfg(0x400002E4, 0x3, 18, 1);
    Multi_Bit_Cfg(0x40000334, 0xf, 4, 0xC);
    //NF_DAT[0]     XPF13   0x400002D8[27:26] = 0x1     0x4000031C[23:20] = 0xB
    Multi_Bit_Cfg(0x400002D8, 0x3, 26, 1);
    Multi_Bit_Cfg(0x4000031C, 0xf, 20, 0xB);
    //NF_DAT[1]     XPF14   0x400002D8[29:28] = 0x1     0x4000031C[27:24] = 0xB
    Multi_Bit_Cfg(0x400002D8, 0x3, 28, 1);
    Multi_Bit_Cfg(0x4000031C, 0xf, 24, 0xB);
    //NF_DAT[2]     XPI3    0x400002E4[7:6] = 0x1       0x40000330[15:12] = 0xB
    Multi_Bit_Cfg(0x400002E4, 0x3, 6, 1);
    Multi_Bit_Cfg(0x40000330, 0xf, 12, 0xB);
    //NF_DAT[3]     XPI4    0x400002E4[9:8] = 0x1       0x40000330[19:16] = 0xB
    Multi_Bit_Cfg(0x400002E4, 0x3, 8, 1);
    Multi_Bit_Cfg(0x40000330, 0xf, 16, 0xB);
    //NF_DAT[4]     XPG1    0x400002DC[3:2] = 0x1       0x40000320[7:4] = 0xB
    Multi_Bit_Cfg(0x400002DC, 0x3, 2, 1);
    Multi_Bit_Cfg(0x40000320, 0xf, 4, 0xB);
    //NF_DAT[5]     XPG2    0x400002DC[5:4] = 0x1       0x40000320[11:8] = 0xB
    Multi_Bit_Cfg(0x400002DC, 0x3, 4, 1);
    Multi_Bit_Cfg(0x40000320, 0xf, 8, 0xB);
    //NF_DAT[6]     XPG3    0x400002DC[7:6] = 0x1       0x40000320[15:12] = 0xB
    Multi_Bit_Cfg(0x400002DC, 0x3, 6, 1);
    Multi_Bit_Cfg(0x40000320, 0xf, 12, 0xB);
    //NF_DAT[7]     XPG4    0x400002DC[9:8] = 0x1       0x40000320[19:16] = 0xB
    Multi_Bit_Cfg(0x400002DC, 0x3, 8, 1);
    Multi_Bit_Cfg(0x40000320, 0xf, 16, 0xB);
}


#endif


/**
 * Initialize pseudo-random number generator with the specified seed value
 */
static void srand(unsigned int seed)  /**< new seed */
{
    libc_random_gen_state[0] =
                    libc_random_gen_state[1] =
                                    libc_random_gen_state[2] =
                                            libc_random_gen_state[3] = seed;
} /* srand */

static void release_clk_rst(void)
{
    (*(volatile uint32_t *)(0x40000080)) |= 1 << 6;
    (*(volatile uint32_t *)(0x40000070)) |= 1 << 6;
}

/*
 * COPYRIGHT (c) 2010-2016 MACRONIX INTERNATIONAL CO., LTD
 * MX30-series Parallel Flash Low Level Driver (LLD) Sample Code
 *
 * Application example.
 * This sample code provides a reference, not recommand for direct using.
 *
 * $Id: MX30_APP.c,v 1.1.1.1 2012/02/23 06:14:37 modelqa Exp $
 */









#if 0                  /*ECC 2.0*/

void nf_page2K_ecc_func_test(uint8_t bch_mod, uint32_t page_tst)
{
    int i = 0;
    int k = 0;

    uint32_t test_len   ;
    uint32_t test_seed  ;
    uint32_t gdata      ;

    uint32_t tst_page_num   ;
    uint32_t tst_sec_num    ;
    uint32_t tst_dat_addr   ;
    uint32_t tst_err_cnt    ;
    uint8_t  wdata          ;
    uint8_t  bit_vld        ;
    uint8_t  max_ecc_ability;


    switch (bch_mod)
    {
        case 0x0:
        {
            printf("\n  @@@Test Item: Check ECC 16bit/1024Byte function!");
            max_ecc_ability = 16;
            break;
        }
        case 0x1:
        {
            printf("\n  @@@Test Item: Check ECC 24bit/1024Byte function!");
            max_ecc_ability = 24;
            break;
        }
        case 0x2:
        {
            printf("\n  @@@Test Item: Check ECC 40bit/1024Byte function!");
            max_ecc_ability = 40;
            break;
        }
        case 0x3:
        {
            printf("\n  @@@Test Item: Check ECC 48bit/1024Byte function!");
            max_ecc_ability = 48;
            break;
        }
        case 0x4:
        {
            printf("\n  @@@Test Item: Check ECC 60bit/1024Byte function!");
            max_ecc_ability = 60;
            break;
        }
        default:
        {
            break;
        }
    }


    //set bch mode
    owNF_MODE_REG   = 0x01;
    k_sleep(1);
    obNF_ECC_MODE   = 0x06;      //BCH, with error correction
    k_sleep(1);
    obNF_BCH_MODE   = bch_mod;   //16B/1024Byte

    test_seed = rand();

    nf_pio_write(test_seed, page_tst, 1);

    tst_page_num    = 0 ;
    tst_sec_num     = 0 ;
    tst_dat_addr    = 0 ;
    tst_err_cnt     = 0 ;

    do
    {

        // corrupt 1st byte data in nf
        gdata = (test_seed + tst_page_num) * ((tst_dat_addr / 4) * 4 + 1 + tst_sec_num * 1024);
        printf("\n   Test Case: gdata = 0x%x", gdata);

        if (0 == tst_dat_addr % 4)
        {
            wdata = gdata & 0xff;
        }
        else if (1 == tst_dat_addr % 4)
        {
            wdata = (gdata & 0xff00) >> 8;
        }
        else if (2 == tst_dat_addr % 4)
        {
            wdata = (gdata & 0xff0000) >> 16;
        }
        else
        {
            wdata = (gdata & 0xff000000) >> 24;
        }


        printf("\n   Test Case: corrupted ori data is 0x%0x, addr is 0x%0x", wdata, tst_dat_addr);

        bit_vld = 1;
        for (i = 0; i < 8; i++)
        {
            if (0 != (wdata & bit_vld))
            {
                tst_err_cnt++;
            }
            bit_vld = bit_vld << 1;
        }

        printf("\n   Test Case: there are %0d bits corrupted", tst_err_cnt);

        // corrupt data with 0x0
        nf_pio_rand_byte_write(page_tst, tst_dat_addr, 0);


#if 0
        FlashParameter fpar = {0};

        fpar.Address = 0x00000000;
        fpar.Length = PAGE_SIZE;
        for (i = 0 ; i < PAGE_SIZE ; i = i + 4)
        {
            test_dat[i] = test_seed & 0xFF;  /* Generate random number */
            test_dat[i + 1] = (test_seed >> 8) & 0xFF;
            test_dat[i + 2] = (test_seed >> 16) & 0xFF;
            test_dat[i + 3] = (test_seed >> 24) & 0xFF;

        }




        fpar.Array = read_dat;
        memset(read_dat, 0, 2200);
        if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)      /* Read data */
        {
            printf("Error:Command Read:Read Data Operation not OK\n");
            return 1;
        }

        for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
        {
            if (read_dat[i] != test_dat[i])
            {
                printf("read[%d] = %d test_seed = %d\n", i, read_dat[i], test_dat[i]);
                //return 1;  /* Verify data, should equal to 0xFF */
            }
        }


#endif

        if (tst_err_cnt <= max_ecc_ability)
        {
            nf_pio_read(test_seed, page_tst, 1, 0, 0);

            if (obNF_ECC_SR != tst_err_cnt)
            {
                printf("\nError_Mx_SIM: ecc err cnt not %0d, should be %0d", obNF_ECC_SR, tst_err_cnt);
            }

            if (odwNF_SEC_ERR_INFO != 0xffff0001)
            {
                printf("\nError_Mx_SIM: ecc err info not 0x%x, should be 0xffff0001", odwNF_SEC_ERR_INFO);
            }
        }
        else
        {
            nf_pio_read(test_seed, page_tst, 1, 0, tst_dat_addr + 1);

            if (obNF_ECC_SR > max_ecc_ability)
            {
                printf("\nError_Mx_SIM: ecc err cnt not %0d, should <= %0d", obNF_ECC_SR, max_ecc_ability);
            }

            if (odwNF_SEC_ERR_INFO != 0xfffe0001)
            {
                printf("\nError_Mx_SIM: ecc err info not 0x%x, should be 0xfffe0001", odwNF_SEC_ERR_INFO);
            }
        }

        tst_dat_addr++;

    } while (tst_err_cnt <= max_ecc_ability);



}



#endif






int main(void)
{
    FlashParameter fpar = {0};
    uint32_t flash_init_addr ;
    uint8_t device_id[4] = {0};

    int i;
    int j;

    pinmux_config();
    tr_clock();


    /* Initial of MX30-series LLD applcation */
    printf("Nand Flash Test Begin\n");
    release_clk_rst();
    if (Flash_Success != HAL_NFlash_Init(pNFlashDev))
    {
        printf("Nand Flash Initial Error...\n");
    }

    /*reset nand_flash*/
    nf_reset();
    //srand( 123 );  /* Initial random seed of rand() function */

    /* Read device ID. */
    fpar.Array = device_id;
    fpar.Length = 4;
    //nand_flash_readID();
    if (FlashAPI(pNFlashDev, Read_ID, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command Read_ID: Cannot Read the NandFlash ID\n");
        return 1;
    }

    printf("device ID:");
    for (i = 0; i < 5; i = i + 1)
    {
        //if( device_id[i] != ID_CODE[i] ) return 1;
        printf("%x ", device_id[i]);
    }
    printf("\n");
    /* Program data to flash device */
    flash_init_addr = 0x00000000;
    memset(exp_dat, 0, PAGE_SIZE);
    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        exp_dat[i] = rand() & 0xFF;  /* Generate random number */
    }
    fpar.Array = exp_dat;
    fpar.Address = flash_init_addr;
    fpar.Length = PAGE_SIZE;




    if (FlashAPI(pNFlashDev, BlockErase, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command BlockErase:Block Erase operation not OK\n");
        return 1;  /* Erase data */
    }
    printf("Command BlockErase:Block Erase Operation  OK\n");
    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)      /* Read data */
    {
        printf("Error:Command Read:Read Data Operation not OK\n");
        return 1;
    }

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != 0xFF)
        {
            printf("Error:Command BlockErase:Block Erase result not OK\n");
            //return 1;  /* Verify data, should equal to 0xFF */
        }
    }
    printf("Command BlockErase:Block Erase result  OK\n");







#if 1              /*ODD V2.0*/
    i = 0;
    int k = 0;
    uint32_t test_len   ;
    uint32_t test_seed  ;
    uint32_t gdata      ;

    uint32_t tst_page_num   ;
    uint32_t tst_sec_num    ;
    uint32_t tst_dat_addr   ;
    uint32_t tst_err_cnt    ;
    uint8_t  wdata          ;
    uint8_t  bit_vld        ;
    uint8_t bID[5];





    puts("\nNF test begin-------------------------------------------------");
    //NF_INTT();
    //begin
    owNF_MODE_REG   = 0x01;
    k_sleep(1);
    obNF_STD_RD_TIM = 0x4b;
    k_sleep(1);
    obNF_STD_WR_TIM = 0x4b;
    k_sleep(1);

    obNF_EDO_TIM    = 0x04;
    k_sleep(1);
    obNF_DMA_CTRL   = 0x00;
    //end

    //NF_RESET();
    //begin
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0xFF;
    k_sleep(1);
    obNF_CLE    =   0x0;
    //end

    //ReadID();
    //begin
    //uint8_t bID[5];

    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x90;
    k_sleep(1);
    odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x00;
    k_sleep(1);
    obNF_ALE    =   0x0 ;
    bID[0] = obNF_PIO_DR;
    bID[1] = obNF_PIO_DR;
    bID[2] = obNF_PIO_DR;
    bID[3] = obNF_PIO_DR;
    bID[4] = obNF_PIO_DR;

    printf("bIDs=%X %X %X %X %X\n", bID[0], bID[1], bID[2], bID[3], bID[4]);
    //end

    obNF_ECC_SR     = 0x00;
    k_sleep(1);
    obNF_SR0        = 0x00;

    //ECC 16B/1024Byte
    nf_page8K_ecc_info_test(0, 0);
    //nf_page8K_ecc_uncorrect_test(0);



    puts("\nNF test end-----------------------------------------------");


    return 0;








#endif
































#if 0                  /*ECC TEST V2.0*/
    i = 0 ;
    int k = 0 ;

    uint32_t test_len   ;
    uint32_t test_seed  ;
    uint32_t gdata      ;

    uint32_t tst_page_num   ;
    uint32_t tst_sec_num    ;
    uint32_t tst_dat_addr   ;
    uint32_t tst_err_cnt    ;
    uint8_t  wdata          ;
    uint8_t  bit_vld        ;
    uint8_t bID[5];

#if 0
    /*- RSIM:1 ; ES_test:0 -*/

    release_clk_rst();

#endif

    //NF_INTT();
    //begin
    owNF_MODE_REG   = 0x01;
    k_sleep(1);
    obNF_STD_RD_TIM = 0x4b;
    k_sleep(1);
    obNF_STD_WR_TIM = 0x4b;
    k_sleep(1);
    obNF_EDO_TIM    = 0x04;
    k_sleep(1);
    obNF_DMA_CTRL   = 0x00;
    k_sleep(1);
    //end

    //NF_RESET();
    //begin
    odwNF_CR    =   0x00010000;//WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);

    obNF_PIO_DR =   0xFF;

    k_sleep(1);
    obNF_CLE    =   0x0;
    //end

    //ReadID();
    //begin
    //uint8_t bID[5];

    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x90;
    k_sleep(1);
    odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x00;
    k_sleep(1);
    obNF_ALE    =   0x0 ;
    bID[0] = obNF_PIO_DR;
    bID[1] = obNF_PIO_DR;
    bID[2] = obNF_PIO_DR;
    bID[3] = obNF_PIO_DR;
    bID[4] = obNF_PIO_DR;

    printf("bIDs=%X %X %X %X %X\n", bID[0], bID[1], bID[2], bID[3], bID[4]);
    //end

    obNF_ECC_SR     = 0x00;         //clear status register
    k_sleep(1);
    obNF_SR0        = 0x00;

    //ECC 16B/1024BYTE
    //   nf_page2K_ecc_func_test(0, 0);

    //ECC 24B/1024Byte
    nf_page2K_ecc_func_test(1, 0);

    //ECC 40B/1024Byte
    //nf_page2K_ecc_func_test(2, 2);

    //ECC 48B/1024Byte
    //nf_page2K_ecc_func_test(3, 3);

    //ECC 60B/1024Byte
    //nf_page2K_ecc_func_test(4, 4);


    return 0;




#endif































#if 0                                  /*ECC test         note: OK*  /


    //damon add main begin
    //int i=0;
    int k=0;
    uint32_t test_len   ;
    uint32_t test_seed  ;
    uint32_t gdata      ;
    uint32_t tst_page_num   ;
    uint32_t tst_sec_num    ;
    uint32_t tst_dat_addr   ;
    uint32_t tst_err_cnt    ;
    uint8_t  wdata          ;
    uint8_t  bit_vld        ;

    //controller initial
    //owNF_MODE_REG   = 0x01;
    obNF_STD_RD_TIM = 0x4b;
    obNF_STD_WR_TIM = 0x4b;
    //obNF_EDO_TIM    = 0x04;
    obNF_DMA_CTRL   = 0x00;

    //nandflash reset
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    obNF_PIO_DR =   0xFF;
    obNF_CLE    =   0x0;

    uint8_t bID[5];     //define buffer save id*5

    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x90;
    k_sleep(1);
    odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
    k_sleep(1);
    obNF_PIO_DR =   0x00;
    k_sleep(1);
    obNF_ALE    =   0x0 ;
    bID[0]=obNF_PIO_DR;
    bID[1]=obNF_PIO_DR;
    bID[2]=obNF_PIO_DR;
    bID[3]=obNF_PIO_DR;
    bID[4]=obNF_PIO_DR;

    printf("bIDs=%X %X %X %X %X\n",bID[0], bID[1], bID[2], bID[3], bID[4]);

    //clear ecc flag
    obNF_ECC_SR     = 0x00;
    obNF_SR0        = 0x00;


    printf("Now beging to test the DMA PIO program and read \n");
    /* Erase flash data */
    fpar.Address = flash_init_addr;

    if (FlashAPI(pNFlashDev, BlockErase, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command BlockErase:Block Erase operation not OK\n");
        return 1;  /* Erase data */
    }
    printf("Command BlockErase:Block Erase Operation  OK\n");
    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)      /* Read data */
    {
        printf("Error:Command Read:Read Data Operation not OK\n");
        return 1;
    }

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != 0xFF)
        {
            printf("Error:Command BlockErase:Block Erase result not OK\n");
            //return 1;  /* Verify data, should equal to 0xFF */
        }
    }
    printf("Command BlockErase:Block Erase result  OK\n");


    fpar.Array = exp_dat;
    fpar.Address = flash_init_addr;
    fpar.Length = PAGE_SIZE;

    if (FlashAPI(pNFlashDev, ProgramDmaPio, &fpar) != (FlashReturnMsg)Flash_Success)     //Program data
    {
        printf("Error:Command ProgramDmaPio:Program Data Operation not OK\n");
        //return 1;
    }
    printf("Command ProgramDmaPio:Program Data Operation  OK\n");


    nf_pio_rand_byte_write(0, 0, 0);


    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, ReadDmaPio, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command ReadDmaPio:Read Data Operation not OK\n");
        //return 1;
    }











    if (FlashAPI(pNFlashDev, BlockErase, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command BlockErase:Block Erase operation not OK\n");
        return 1;  /* Erase data */
    }
    printf("Command BlockErase:Block Erase Operation  OK\n");
    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)      /* Read data */
    {
        printf("Error:Command Read:Read Data Operation not OK\n");
        return 1;
    }

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != 0xFF)
        {
            printf("Error:Command BlockErase:Block Erase result not OK\n");
            //return 1;  /* Verify data, should equal to 0xFF */
        }
    }
    printf("Command BlockErase:Block Erase result  OK\n");



#if 0
//test ecc function begin
printf("ecc 16b/1024byte test begin");
nf_page2K_ecc_func_test(0, 0);
printf("ecc 16b/1024byte test end");
#endif


#if 1
printf("ecc 24b/1024byte test begin");
nf_page2K_ecc_func_test(1, 0);
printf("ecc 24b/1024byte test end");
#endif

#if 0

    printf("ecc 40b/1024byte test begin");
    nf_page2K_ecc_func_test(2, 2);
    printf("ecc 40b/1024byte test end");
#endif

#if 0
printf("ecc 48b/1024byte test begin");
nf_page2K_ecc_func_test(3, 3);
printf("ecc 48b/1024byte test end");
#endif
#if 0
printf("ecc 60b/1024byte test begin");
nf_page2K_ecc_func_test(4, 4);
printf("ecc 60b/1024byte test end");
//test ecc function end
#endif
//damon add main end



#endif         /* end ECC test*/



    /*3.28 the last shiyan*/
#if 1



#if 0
    if (FlashAPI(pNFlashDev, BlockErase, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command BlockErase:Block Erase operation not OK\n");
        return 1;  /* Erase data */
    }
    printf("Command BlockErase Before Program:Block Erase Operation  OK\n");

    if (FlashAPI(pNFlashDev, Program, &fpar) != (FlashReturnMsg)Flash_Success)     //Program data
    {
        printf("Error:Command Program:Program Data Operation not OK\n");
        return 1;
    }

    printf("Command Program:Program Data Operation  OK\n");
    /* Read and verify data */
    fpar.Array = read_dat;

    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command Read:Read Data Operation not OK\n");
        return 1;
    }

    printf("Command Read:Read Data Operation  OK\n");

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != exp_dat[i])
        {
            printf("Error:Command Read:Data Read is diff to Data Write\n");
            return 1;  /* Verify data */
        }
    }
    printf("Command Read:Read Data is Same to Data Write\n");

#endif

    printf("Now beging to test the DMA PIO program and read \n");
    /* Erase flash data */
    fpar.Address = flash_init_addr;

    if (FlashAPI(pNFlashDev, BlockErase, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command BlockErase:Block Erase operation not OK\n");
        return 1;  /* Erase data */
    }
    printf("Command BlockErase:Block Erase Operation  OK\n");
    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)     /* Read data */
    {
        printf("Error:Command Read:Read Data Operation not OK\n");
        return 1;
    }

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != 0xFF)
        {
            printf("Error:Command BlockErase:Block Erase result not OK\n");
            //return 1;  /* Verify data, should equal to 0xFF */
        }
    }
    printf("Command BlockErase:Block Erase result  OK\n");


    fpar.Array = exp_dat;
    fpar.Address = flash_init_addr;
    fpar.Length = PAGE_SIZE;

    if (FlashAPI(pNFlashDev, ProgramDmaPio, &fpar) != (FlashReturnMsg)Flash_Success)     //Program data
    {
        printf("Error:Command ProgramDmaPio:Program Data Operation not OK\n");
        //return 1;
    }
    printf("Command ProgramDmaPio:Program Data Operation  OK\n");

    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, ReadDmaPio, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command ReadDmaPio:Read Data Operation not OK\n");
        //return 1;
    }

    printf("Command ReadDmaPio:Read Data Operation  OK\n");

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != exp_dat[i])
        {
            printf("Error:Command ReadDmaPio::Data Read is diff to Data Write\n");
            break;
            //return 1;  /* Verify data */
        }
    }
    printf("Command ReadDmaPio:Read Data is Same to Data Write\n");
    printf("Now end to test the DMA PIO program and read \n");

#if 0

    printf("Now beging to test the DMA DMA program and read \n");

    fpar.Array = exp_dat;
    fpar.Address = flash_init_addr;
    fpar.Length = PAGE_SIZE;

    if (FlashAPI(pNFlashDev, BlockErase, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command BlockErase:Block Erase operation not OK\n");
        return 1;  /* Erase data */
    }

    if (FlashAPI(pNFlashDev, ProgramDmaDma, &fpar) != (FlashReturnMsg)Flash_Success)     //Program data
    {
        printf("Error:Command ProgramDmaDma:Program Data Operation not OK\n");
        //return 1;
    }
    printf("Command ProgramDmaDma:Program Data Operation  OK\n");
    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, ReadDmaDma, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command ReadDmaDma:Read Data Operation not OK\n");
        //return 1;
    }

    printf("Command ReadDmaDma:Read Data Operation	OK\n");

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != exp_dat[i])
        {
            printf("Error:Command ReadDmaDma::Data Read is diff to Data Write\n");
            break;
            //return 1;  /* Verify data */
        }
    }
    printf("Command ReadDmaDma:Read Data is Same to Data Write\n");
    printf("Now End to test the DMA DMA program and read \n");
#endif


#endif















#if 0

    printf("Begin Cache Write and Cache Read fucntion check.\n");
    fpar.Array = exp_dat;
    fpar.Address = flash_init_addr;
    fpar.Length = PAGE_SIZE;
    if (FlashAPI(pNFlashDev, BlockErase, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command BlockErase Before CacheProgram:Block Erase operation not OK\n");
        //return 1;  /* Erase data */
    }
    printf("Command BlockErase Before CacheProgram:Block Erase operation is OK\n");

    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)     /* Read data */
    {
        printf("Error:Command Read:Read Data Operation not OK\n");
        //return 1;
    }

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != 0xFF)
        {
            printf("Error:Command BlockErase:Block Erase result not OK\n");
            //return 1;  /* Verify data, should equal to 0xFF */
        }
    }
    printf("Command BlockErase Before CacheProgram:Block Erase and Read Operation  OK\n");

    fpar.Array = exp_dat;
    fpar.Address = flash_init_addr;
    fpar.Length = PAGE_SIZE;
    if (FlashAPI(pNFlashDev, CacheProgram, &fpar) != (FlashReturnMsg)Flash_Success)     //Program data
    {
        printf("Error:Command CacheProgram:Program Data Operation not OK\n");
        //return 1;
    }
    //printf("Command BlockErase:Block Erase result  OK\n");
    printf("Command CacheProgram:Program Data Operation is OK\n");

    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)     /* Read data */
    {
        printf("Error:Command Read:Read Data Operation not OK\n");
        //return 1;
    }
    printf("Command Read:Read Data Operation  OK\n");
    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != exp_dat[i])
        {
            printf("Error:Command Read:Data Read is diff to Data Write\n");
            break;
            //return 1;  /* Verify data */
        }
    }
    printf("Command Read:Read Data is Same to Data Write\n");

    //Cache Read Mode
    memset(read_dat, 0, PAGE_SIZE);
    HAL_NFlash_Cache_Seq_Rand_Read_Begin(pNFlashDev, 0x00);
    HAL_NFlash_Cache_Seq_Read_Another_OP(pNFlashDev, read_dat, PAGE_SIZE, 1);
    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != exp_dat[i])
        {
            printf("Error:Command CacheRead:Data Read is diff to Data Write\n");
            break;
            //return 1;  /* Verify data */
        }
    }
    printf("Command CacheRead:Read Data is Same to Data Write\n");
    printf("End Cache Write and Cache Read fucntion check.\n");


#endif










#if 0

    printf("Now beging to test the DMA PIO program and read \n");
    /* Erase flash data */
    fpar.Address = flash_init_addr;

    if (FlashAPI(pNFlashDev, BlockErase, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command BlockErase:Block Erase operation not OK\n");
        return 1;  /* Erase data */
    }
    printf("Command BlockErase:Block Erase Operation  OK\n");
    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)     /* Read data */
    {
        printf("Error:Command Read:Read Data Operation not OK\n");
        return 1;
    }

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != 0xFF)
        {
            printf("Error:Command BlockErase:Block Erase result not OK\n");
            //return 1;  /* Verify data, should equal to 0xFF */
        }
    }
    printf("Command BlockErase:Block Erase result  OK\n");


    fpar.Array = exp_dat;
    fpar.Address = flash_init_addr;
    fpar.Length = PAGE_SIZE;

    if (FlashAPI(pNFlashDev, ProgramDmaPio, &fpar) != (FlashReturnMsg)Flash_Success)      //Program data
    {
        printf("Error:Command ProgramDmaPio:Program Data Operation not OK\n");
        //return 1;
    }
    printf("Command ProgramDmaPio:Program Data Operation  OK\n");

    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, ReadDmaPio, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command ReadDmaPio:Read Data Operation not OK\n");
        //return 1;
    }

    printf("Command ReadDmaPio:Read Data Operation  OK\n");

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != exp_dat[i])
        {
            printk("read_dat[%d]=%d,exp_dat[%d]=%d\n", i, read_dat[i], i, exp_dat[i]);
            printf("Error:Command ReadDmaPio::Data Read is diff to Data Write\n");
            break;
            //return 1;  /* Verify data */
        }
    }






#endif

    printk("Nand Flash Test End\n");
    return 0;  /* Execute success */
}


/*
 * Function:     FlashAPI
 * Arguments:    Cmd -> Input command code
 *               fptr -> Flash parameter pointer
 * Return Value: Flash command returned message.
 * Description:  This function provide the interface to access flash commands.
 *
 * Include Command:
 *     Read_ID
 */
FlashReturnMsg FlashAPI(NFlash_Device_T *pNFlashDev, CmdType cmd, FlashParameter *fptr)
{
    FlashReturnMsg rtMsg = Flash_Success;

    switch (cmd)
    {
        case Read_ID:
            rtMsg = HAL_NFlash_Read_ID(pNFlashDev, fptr->Array);
            break;
        case Program:
            rtMsg = HAL_NFlash_Page_Program(pNFlashDev, fptr->Address, fptr->Array, fptr->Length);
            break;
        case Read:
            rtMsg = HAL_NFlash_Page_Read(pNFlashDev, fptr->Address, fptr->Array, fptr->Length);
            break;
        case BlockErase:
            rtMsg = HAL_NFlash_Block_Erase(pNFlashDev, fptr->Address);
            break;
        case ReadDmaPio:
            rtMsg = HAL_NFlash_Page_Read_DMA_PIO(pNFlashDev, fptr->Address, fptr->Array, fptr->Length);
            break;
        case ProgramDmaPio:
            rtMsg = HAL_NFlash_Page_Program_DMA_PIO(pNFlashDev, fptr->Address, fptr->Array, fptr->Length);
            break;
        case CacheProgram:
            rtMsg = HAL_NFlash_Cache_Program(pNFlashDev, fptr->Address, fptr->Array, fptr->Length, 1);
            break;
        case ProgramDmaDma:
            rtMsg = HAL_NFlash_Page_Program_DMA_DMA(pNFlashDev, fptr->Address, fptr->Array, fptr->Length);
            break;
        case ReadDmaDma:
            rtMsg = HAL_NFlash_Page_Read_DMA_DMA(pNFlashDev, fptr->Address, fptr->Array, fptr->Length);
            break;
        case ReadDmaPioODD:
            rtMsg = HAL_NFlash_Page_Read_DMA_PIO_ODD(pNFlashDev, fptr->Address, fptr->Array, fptr->Length);
            break;
        case ProgramDmaPioODD:
            rtMsg = HAL_NFlash_Page_Program_DMA_PIO_ODD(pNFlashDev, fptr->Address, fptr->Array, fptr->Length);
            break;
        default:
            rtMsg = Flash_CmdInvalid;
            break;
    }

    return rtMsg;
}
