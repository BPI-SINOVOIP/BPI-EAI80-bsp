void nf_dmapio_write(uint32_t seed, uint32_t wr_page, uint32_t page_cnt);
void nf_dmapio_read(uint32_t seed, uint32_t rd_page, uint32_t page_cnt, uint32_t ignore_addr, uint32_t ignore_len);
void nf_pio_rand_byte_write(uint32_t wr_page, uint32_t addr, uint8_t data);
void nf_page2K_ecc_func_test(uint8_t bch_mod, uint32_t page_tst);

int main(void)
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

    //controller initial
    owNF_MODE_REG   = 0x01;
    obNF_STD_RD_TIM = 0x24;
    obNF_STD_WR_TIM = 0x22;
    obNF_EDO_TIM    = 0x04;
    obNF_DMA_CTRL   = 0x00;

    //nandflash reset
    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    obNF_PIO_DR =   0xFF;
    obNF_CLE    =   0x0;

    uint8_t bID[5];     //define buffer save id*5

    odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
    obNF_PIO_DR =   0x90;
    odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
    obNF_PIO_DR =   0x00;
    obNF_ALE    =   0x0 ;
    bID[0] = obNF_PIO_DR;
    bID[1] = obNF_PIO_DR;
    bID[2] = obNF_PIO_DR;
    bID[3] = obNF_PIO_DR;
    bID[4] = obNF_PIO_DR;

    printf("bIDs=%X %X %X %X %X\n", bID[0], bID[1], bID[2], bID[3], bID[4]);

    //clear ecc flag
    obNF_ECC_SR     = 0x00;
    obNF_SR0        = 0x00;

    //test ecc function begin
    printf("ecc 16b/1024byte test begin");
    nf_page2K_ecc_func_test(0, 0);
    printf("ecc 16b/1024byte test end");


    printf("ecc 24b/1024byte test begin");
    nf_page2K_ecc_func_test(1, 1);
    printf("ecc 24b/1024byte test end");


    printf("ecc 40b/1024byte test begin");
    nf_page2K_ecc_func_test(2, 2);
    printf("ecc 40b/1024byte test end");


    printf("ecc 48b/1024byte test begin");
    nf_page2K_ecc_func_test(3, 3);
    printf("ecc 48b/1024byte test end");

    printf("ecc 60b/1024byte test begin");
    nf_page2K_ecc_func_test(4, 4);
    printf("ecc 60b/1024byte test end");
    //test ecc function end

    return 0;
}


void diy_delay(int loop)
{
    int i;
    for (i = 0; i < loop; i++)
    {
        __NOP;
        __NOP;
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



    int length_leave = 2048;
    int sect_num = 0;
    uint32_t temp_data = 0;


    gxnf_page_cnt   =   page_cnt;
    gxnf_wr_page    =   wr_page;

    obNF_DMA_CFG    = (4 << 5 | 2);  //FW_REDU_NUM=4, DMA_LEN=2
    obNF_DMA_CTRL   =   0x02; // DMA_DIR, ~DMA_EN
    odwNF_W_REDU0_REG = 0x04030201; // WR redun 3-0
    odwNF_W_REDU1_REG = 0x08070605; // WR redun 7-4



    for (gxnf_page_num = 0; gxnf_page_num < gxnf_page_cnt; gxnf_page_num++)
    {

        gxnf_prog_page  = wr_page + gxnf_page_num ;

        // NF Write data start
        printf("\n Begin NF PIO WRITE\n");

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
        obNF_PIO_DR = (gxnf_prog_page & 0xff0000) >> 16; // row2
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        obNF_DMA_CTRL = 0x03; // DMA_DIR, DMA_EN

        diy_delay(5);


        while (length_leave != 0)
        {


            do
            {
                nf_state = (uint8_t)((pNFlashDev->SR & BITS16_MASK) >> BYTE1_OFFSET);
            } while ((nf_state & (0x02)) == 0x02);



            /* Send data to program */
            for (i = 0; i < SECT_SIZE_ECC_MODE; i = i + 4)
            {
                temp_data = (uint32_t)data_buf[sect_num * SECT_SIZE_ECC_MODE + i + 3] << BYTE3_OFFSET;
                temp_data = temp_data | ((uint32_t)data_buf[sect_num * SECT_SIZE_ECC_MODE + i + 2] << BYTE2_OFFSET);
                temp_data = temp_data | ((uint32_t)data_buf[sect_num * SECT_SIZE_ECC_MODE + i + 1] << BYTE1_OFFSET);
                temp_data = temp_data | ((uint32_t)data_buf[sect_num * SECT_SIZE_ECC_MODE + i]);
                HAL_NFlash_Write_Dma_PIO(pNFlashDev, temp_data);
            }
            length_leave = length_leave - SECT_SIZE_ECC_MODE;
            ++sect_num;
        }


#if 0
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
                wdata = (seed + gxnf_page_num) * (gxnf_data_len + 1 + gxnf_sec_num * 1024);
                odwNF_DMA_DR =  wdata;
            }
        }

#endif
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
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        obNF_PIO_DR =   0x00;
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        //end
        obNF_PIO_DR = 0x00; // col0
        obNF_PIO_DR = 0x00; // col1
        obNF_PIO_DR = gxnf_read_page & 0xff;          // row0
        obNF_PIO_DR = (gxnf_read_page & 0xff00) >> 8; // row1
        obNF_PIO_DR = (gxnf_read_page & 0xff0000) >> 16; // row2
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
        //CmdRead2NF();
        //begin
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        obNF_PIO_DR =   0x30;
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

        diy_delay(5);
        //CmdRead1NF();
        //beign
        odwNF_CR    =   0x00010000;    //WP:0, CLE:1, ALE:0, CEJ:0
        obNF_PIO_DR =   0x00;
        odwNF_CR    =   0x00000100;    //WP:0, CLE:0, ALE:1, CEJ:0
        //end
        diy_delay(5);
        odwNF_CR    =   0x00000000;    //WP:0, CLE:0, ALE:0, CEJ:0
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
                else if (rdata != gdata)
                {
                    printf("\nError_Mx_SIM, Page %d, addr 0x%0x, rdata=0x%x, gdata=0x%x", gxnf_read_page, gxnf_data_len, rdata, gdata);
                }
            }
        }

        printf("begin NF DMA PIO Read Finish\n");
        NFReadFinish();

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
    obNF_PIO_DR = (gxnf_prog_page & 0xff0000) >> 16; // row2

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

//=============================================================
//bch_mod
//000: 16b/1KB, ECC 28B + 4B FW redundant
//001: 24b/1KB, ECC 42B + 4B FW redundant
//010: 40b/1KB, ECC 70B + 4B FW redundant
//011: 48b/1KB, ECC 84B + 4B FW redundant
//100: 60b/1KB, ECC 106B + 4B FW redundant

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


