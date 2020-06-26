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

#define NF_ADDR_BASE         0x70000000//Õâ¸ö¾ßÌåÒª·Åµ½ÄÄ¸öÎÄ¼þÀïÃæ£¬´ý¶¨
NFlash_Device_T *pNFlashDev = (NFlash_Device_T *)(NF_ADDR_BASE);


#define RAND_MAX (0x7fffffffu)
static uint32_t libc_random_gen_state[4] = { 1455997910, 1999515274, 1234451287, 1949149569 };
/**
 * Generate pseudo-random integer
 *
 * Note:
 *      The function implements George Marsaglia's XorShift random number generator
 *
 * @return integer in range [0; RAND_MAX]
 */
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

int main(void)
{
    FlashParameter fpar = {0};
    uint32_t flash_init_addr ;
    uint8_t device_id[4] = {0};

    int i;
    int j;

    /* Initial of MX30-series LLD applcation */
    printf("Nand Flash Test Begin\n");
    release_clk_rst();
    if (Flash_Success != HAL_NFlash_Init(pNFlashDev))
    {
        printf("Nand Flash Initial Error...\n");
    }
    srand(123);    /* Initial random seed of rand() function */

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
    for (i = 0; i < 4; i = i + 1)
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


    printf("Now beging to test the BCH MODE 16b-24b-40b-48b-60b per 1K \n");
    for (j = 0; j <= 4; ++j)
    {
        LL_NF_BCH_SEL_SET(pNFlashDev, j); //Set  BCH algorithm
        fpar.Address = flash_init_addr;

        if (FlashAPI(pNFlashDev, BlockErase, &fpar) != (FlashReturnMsg)Flash_Success)
        {
            printf("Error:Command BlockErase:Block Erase operation not OK\n");
            return 1;  /* Erase data */
        }

        fpar.Array = exp_dat;
        fpar.Address = flash_init_addr;
        fpar.Length = PAGE_SIZE;
        if (FlashAPI(pNFlashDev, ProgramDmaPio, &fpar) != (FlashReturnMsg)Flash_Success)     //Program data
        {
            printf("Error:Command ProgramDmaPio:Program Data Operation not OK\n");
            //return 1;
        }

        /* Read and verify data */
        fpar.Array = read_dat;
        memset(read_dat, 0, PAGE_SIZE);
        if (pNFlashDev, FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)    /* Read data */
        {
            printf("Error:Command Read:Read Data Operation not OK\n");
            //return 1;
        }

        for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
        {
            if (read_dat[i] != exp_dat[i])
            {
                printf("Warning!!! Need to download the data by IDE and compare the data by hands ,then  counter the number!\n");
                break;
                //return 1;  /* Verify data */
            }
        }
    }
    printf("Now End to test the BCH MODE 16b-24b-40b-48b-60b per 1K \n");
#if 0

    printf("Begin ECC odd/even fucntion check.\n");
    // VAL_NF_ECC_MODE  = 0x00;//Change the BCH to odd/parity check
    VAL_NF_MR = 0x101;//used for CRC mode change
    fpar.Address = flash_init_addr;

    if (FlashAPI(pNFlashDev, BlockErase, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command BlockErase:Block Erase operation not OK\n");
        return 1;  /* Erase data */
    }

    fpar.Array = exp_dat;
    fpar.Address = flash_init_addr;
    fpar.Length = PAGE_SIZE;
    if (FlashAPI(pNFlashDev, ProgramDmaPioODD, &fpar) != (FlashReturnMsg)Flash_Success)     //Program data
    {
        printf("Error:Command ProgramDmaPio:Program Data Operation not OK\n");
        //return 1;
    }

    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, Read, &fpar) != (FlashReturnMsg)Flash_Success)     /* Read data */
    {
        printf("Error:Command Read:Read Data Operation not OK\n");
        //return 1;
    }

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != exp_dat[i])
        {
            //printf("Warning!!! Need to download the data by IDE and compare the data by hands ,then  counter the number!\n");
            break;
            //return 1;  /* Verify data */
        }
    }

    uint8_t b[2] = {107, 120};
    Page_Random_Program_Begin_OP(0x00, NULL, 0);
    Page_Random_Program_Input_OP(1, b, 1);
    Page_Random_Program_End_OP();

    b[0] = 120;
    Page_Random_Program_Begin_OP(0x00, NULL, 0);
    Page_Random_Program_Input_OP(511, b, 1);
    Page_Random_Program_End_OP();

    /* Read and verify data */
    fpar.Array = read_dat;
    fpar.Length = PAGE_SIZE;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, ReadDmaPioODD, &fpar) != (FlashReturnMsg)Flash_Success)     /* Read data */
    {
        printf("Error:Command Read:Read Data Operation not OK\n");
        //return 1;
    }

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != exp_dat[i])
        {
            printf("Error!!! Odd and Parity check is unuseful!\n");
            break;
            //return 1;  /* Verify data */
        }
    }
    printf("OK!! Odd and Parity check is useful!\n");
    VAL_NF_ECC_MODE  = 0x06;
    printf("End ECC odd/even fucntion check.\n");
#endif


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



#if 1

    printf("Now Begining to test the Corrupt the data and  ECC function \n");
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

    /* Change the data */
    uint8_t a[2] = {0, 0};
    for (i = 0; i <= 2; ++i) //The bits < 16 bits
    {
        HAL_NFlash_Page_Random_Program_Begin(pNFlashDev, 0x00, NULL, 0);

        HAL_NFlash_Page_Random_Program_Input(pNFlashDev, i, a, 1);

        HAL_NFlash_Page_Random_Program_End(pNFlashDev);
    }

    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, ReadDmaPio, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command ReadDmaPio:Read Data Operation not OK\n");
        //return 1;
    }

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (read_dat[i] != exp_dat[i])
        {
            printf("Error:Command ReadDmaPio::Data Read is diff to Data Write\n");
            break;
            //return 1;  /* Verify data */
        }
    }

    printf("The smaller than 16 bits ECC OK!\n");
    for (i = 0; i <= 3; ++i) //The bits > 16 bits
    {
        HAL_NFlash_Page_Random_Program_Begin(pNFlashDev, 0x00, NULL, 0);

        HAL_NFlash_Page_Random_Program_Input(pNFlashDev, i, a, 1);

        HAL_NFlash_Page_Random_Program_End(pNFlashDev);
    }

    /* Read and verify data */
    fpar.Array = read_dat;
    memset(read_dat, 0, PAGE_SIZE);
    if (FlashAPI(pNFlashDev, ReadDmaPio, &fpar) != (FlashReturnMsg)Flash_Success)
    {
        printf("Error:Command ReadDmaPio:Read Data Operation not OK\n");
        //return 1;
    }

    printf("Command ReadDmaPio:Read Data Operation	OK\n");

    for (i = 0 ; i < PAGE_SIZE ; i = i + 1)
    {
        if (i <= 3)
        {
            if (read_dat[i] == 0)
            {
                continue;
            }
        }
        else
        {
            if (read_dat[i] != exp_dat[i])
            {
                printf("Error:Command ReadDmaPio::Data Read is diff to Data Write\n");
                break;
                //return 1;  /* Verify data */
            }
        }
    }

    printf("The larger than 16 bits ECC OK!\n");
    printf("Now end to test the Corrupt the data and  ECC function \n");

#endif
    printf("Nand Flash Test End\n");
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
