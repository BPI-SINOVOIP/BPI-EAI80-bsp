#include "gm_hal_kdp.h"
#include "processor_common.h"
#include "bank_operation.h"

extern int cntt;

int64_t fix32_round_truncate(int64_t a)
{
    int64_t result;

    /* rounding */
    if (a & 0x80)
    {
        result = (a + 0x100) & ~(0xff);
    }
    else
    {
        result = a;
    }
    result = result >> 8;
    return result;
}

static inline int simu_relu_activate(int x)
{
    return (x > 0) ? x : 0;
}

#define RECHARD_DEBUG  1

/*
 * fix32_round_truncate(fix32_mul(fix32_from_float(.1), x));
 */
static inline int simu_leaky_activate(int x, int relu_num)
{
    if (x > 0)
    {
        return x;
    }
    else
    {

#ifdef RECHARD_DEBUG
        return ((x * relu_num * 2) >> 8);
#else
        return fix32_round_truncate(x * relu_num * 2);
#endif
    }
}

/*****************************************************************************/
/**
*
* This function puts the command into command BRAM
*
* @param    start of index, end of index
*
* @return   none
*
* @note     None.
*
******************************************************************************/
void put_cmd(unsigned char *cmd_addr_start, int start, int length)
{
    int data = 0;
    int tmp, i, index;

    for (index = 0; index < length; index++)
    {
        if (index % 2 == 0)
        {
            tmp = cmd_addr_start[index + start + 1] << 8 | cmd_addr_start[index + start];
            data = tmp;
        }
        else if (index % 2 == 1)
        {
            i = index / 2;
            HW32_REG(BRAM_CMD_BASEARDDR + i * 4) = data;
            data = 0;
        }
    }

}

void kdp_print_op(int code)
{
#ifdef DEBUG_KDP_LEVEL_0
    char *opers[] = {"Hw", "Wait", "MaxPool", "AvgPool", "Connected", "WriteOut", "DataFetch", "Input", "Switch",
                     "Layer", "Buffer", "Shortcut", "Route", "Activation", "Output", "OP_CROSS_WRITEOUT",
                     "CROSS_DATAFETCH", "OP_BANK_MAXPOOL", "Yolo", "Upsample", "BANK_GENERAL", "OP_BANK_ROUTE",
                     "OP_DEPTHWISE_CONV", "OP_GAUSSIAN_YOLO", "OP_FINAL_INPUT_CHANNEL_CUT",
                    };
    if (code < sizeof(opers) / sizeof(char *))
    {
        printf("(%d)%s\n", code, opers[code]);
    }
    else
    {
        printf("(%d)%s\n", code, "UNKOWN");
    }
#endif
}

#define ARMC4_OPT

#ifdef ARMC4_OPT
/*
 *R0: mem_addr   Memory source address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: dst_mem_addr    Memory destination address
 *R4-R7:         Data from Memory(4 cols, 2 rows)
 *R8-R9:         Data from Bank(2 cols, 2 rows)
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 *R12: tmp
 */
__asm void *AsmAddToMemRelu(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int16_t *dst_mem_addr, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr, int32_t dst_last_mem_addr)
{
    PUSH   {R4 - R12, LR}

    MOV    R14, #0                 //Start channel is 0
    channel_loop_add_to_mem_relu
    LDR    R4,  [sp, #0x38]        //Get  len_channel
    CMP    R14, R4
    BGE.W  exit_add_to_mem_relu
    STRD   R0,  R1,  [sp, #0x44]   //Save mem_addr and bank addr
    STR    R3,  [sp, #0x4c]        //Save dst_mem_addr
    LDR    R11, [sp, #0x30]  //Get  valid_len_row
    row_loop_add_to_mem_relu
    CMP    R11, #1           //Check bank row end
    BLT.W  exit_row_loop_add_to_mem_relu
    LDR    R10, [sp, #0x34]  //Get valid_row_off(0x30)
    LDR    R8,  [sp, #0x30]  //Get  valid_len_row
    SUB    R8,  R11
    CMP    R8,  R10          //Check bank row start
    BLT.W  exit_col_loop_add_to_mem_relu
    LDR    R10, [sp, #0x2c]  //Get  valid_len_col
    ANDS   R8,  R0, #3       //Check source and dest alignment
    BNE    dword1_start_loop_add_to_mem_relu
    ANDS   R8,  R3, #3
    BEQ    dword4_col_loop_add_to_mem_relu
    dword1_start_loop_add_to_mem_relu
    LDR    R4,  [R1], #4          //Read dword from bank addr, then increase addr
    MOV    R9,  #0
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4                //Add and relu activation
    SELGE  R6,  R6,  R9
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R6,  [R0], #2          //Get half word at even row from sdram
    SADD16 R6,  R4                //Add and relu activation
    SELGE  R6,  R6,  R9
    STRH   R6,  [R3], #2          //Store half word at even row to sdram, then increase addr
    SUBS   R10, #0x1
    ANDS   R8,  R0, #3            //Check source and dest alignment again
    BNE    dword1_col_loop_add_to_mem_relu
    ANDS   R8,  R3, #3
    BNE    dword1_col_loop_add_to_mem_relu
    dword4_col_loop_add_to_mem_relu
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop_add_to_mem_relu
    ADD    R4,  R0, R2, LSL #1
    LDRD   R5,  R7, [R4]              //Get odd row (mem_addr + col_num) R5-R7
    LDMIA  R0!, {R4, R6}              //Get even row (mem_addr) R4-R6
    LDMIA  R1!, {R8, R9}
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R4,  R8                   //Add and Relu activation
    MOV    R9, #0
    SELGE  R4,  R4,  R9
    SADD16 R5,  R12                  //Add and Relu activation
    SELGE  R5,  R5,  R9
    LDMIA  R1!, {R8, R9}
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R6,  R8
    MOV    R9,  #0
    SELGE  R6,  R6,  R9
    SADD16 R7,  R12
    SELGE  R7,  R7,  R9
    ADD    R12, R3, R2, LSL #1
    CMP    R11, #0x1                  //Check remain valid_len_row > 1
    STRDGT R5,  R7, [R12]             //Store odd row
    STMIA  R3!, {R4, R6}              //Store even row
    B      dword4_col_loop_add_to_mem_relu
    exit_dword4_col_loop_add_to_mem_relu
    ADD    R10, #0x4
    dword1_col_loop_add_to_mem_relu
    SUBS   R10, #0x1
    BLT    exit_col_loop_add_to_mem_relu
    LDR    R4,  [R1], #4          //Read dword from bank addr, then increase addr
    MOV    R9,  #0
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4                //Add and Relu activation
    SELGE  R6,  R6,   R9
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R6,  [R0], #2          //Get half word at even row from sdram
    SADD16 R6,  R4                //Add and Relu activation
    SELGE  R6,  R6,   R9
    STRH   R6,  [R3], #2          //Store half word at even row to sdram, then increase addr
    B      dword1_col_loop_add_to_mem_relu
    exit_col_loop_add_to_mem_relu
    LDRD   R9,  R12, [sp, #0x28]  //Get  len_col valid_len_col
    SUB    R0,  R0, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R3,  R3, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R3,  R3, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R1,  R1, R12, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R9,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R11, #0x2              //Decrease 2 rows
    B      row_loop_add_to_mem_relu
    exit_row_loop_add_to_mem_relu
    ADD    R14, #1
    LDR    R1,  [sp, #0x48]        //Get  bank_addr of last channel
    ANDS   R4,  R14, #1            //Check Odd channel or even channel
    BNE    oddch_addrinc_add_to_mem_relu
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x40]
    ADD    R1,  R4
    oddch_addrinc_add_to_mem_relu
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x3c]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x44]        //Get  mem_addr of last channel
    ADD    R0,  R4
    LDR    R3,  [sp, #0x4c]        //Get  dst_mem_addr of last channel
    ADD    R3,  R4
    B      channel_loop_add_to_mem_relu

    exit_add_to_mem_relu
    POP     {R4 - R12, LR}
    BX      lr
}

/*
 *R0: mem_addr   Memory source address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: dst_mem_addr    Memory destination address
 *R4-R7:         Data from Memory(4 cols, 2 rows)
 *R8-R9:         Data from Bank(2 cols, 2 rows)
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 *R12: tmp
 */
__asm void *AsmAddToBothRelu(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int16_t *dst_mem_addr, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr, int32_t dst_last_mem_addr)
{
    PUSH   {R4 - R12, LR}

    MOV    R14, #0                 //Start channel is 0
    channel_loop_add_to_both_relu
    LDR    R4,  [sp, #0x38]        //Get  len_channel
    CMP    R14, R4
    BGE.W  exit_add_to_both_relu
    STRD   R0,  R1,  [sp, #0x44]   //Save mem_addr and bank addr
    STR    R3,  [sp, #0x4c]        //Save dst_mem_addr
    LDR    R11, [sp, #0x30]  //Get  valid_len_row
    row_loop_add_to_both_relu
    CMP    R11, #1           //Check bank row end
    BLT.W  exit_row_loop_add_to_both_relu
    LDR    R10, [sp, #0x34]  //Get valid_row_off(0x30)
    LDR    R8,  [sp, #0x30]  //Get  valid_len_row
    SUB    R8,  R11
    CMP    R8,  R10          //Check bank row start
    BLT.W  exit_col_loop_add_to_both_relu
    LDR    R10, [sp, #0x2c]  //Get  valid_len_col
    ANDS   R8,  R0, #3       //Check source and dest alignment
    BNE    dword1_start_loop_add_to_both_relu
    ANDS   R8,  R3, #3
    BEQ    dword4_col_loop_add_to_both_relu
    dword1_start_loop_add_to_both_relu
    LDR    R4,  [R1]              //Read dword from bank addr
    MOV    R9,  #0
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4                //Add and relu activation
    SELGE  R6,  R6,   R9
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram
    SADD16 R7,  R4                //Add and relu activation
    SELGE  R7,  R7,   R9
    STRH   R7,  [R3], #2          //Store half word at even row to sdram, then increase addr
    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    SUBS   R10, #0x1
    ANDS   R8,  R0, #3            //Check source and dest alignment again
    BNE    dword1_col_loop_add_to_both_relu
    ANDS   R8,  R3, #3
    BNE    dword1_col_loop_add_to_both_relu
    dword4_col_loop_add_to_both_relu
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop_add_to_both_relu
    ADD    R4,  R0, R2, LSL #1
    LDRD   R5,  R7, [R4]              //Get odd row (mem_addr + col_num) R5-R7
    LDMIA  R0!, {R4, R6}              //Get even row (mem_addr) R4-R6
    LDRD   R8,  R9, [R1, #0]
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R4,  R8                   //Add and relu activation
    MOV    R9,  #0
    SELGE  R4,  R4,  R9
    SADD16 R5,  R12                  //Add and relu activation
    SELGE  R5,  R5,  R9
    LDRD   R8,  R9, [R1, #8]
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R6,  R8                   //Add and relu activation
    MOV    R9,  #0
    SELGE  R6,  R6,  R9
    SADD16 R7,  R12                  //Add and relu activation
    SELGE  R7,  R7,  R9
    ADD    R12, R3, R2, LSL #1
    CMP    R11, #0x1                  //Check remain valid_len_row > 1
    STRDGT R5,  R7, [R12]             //Store odd row
    STMIA  R3!, {R4, R6}              //Store even row
    PKHBT  R8,  R5, R4, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R9,  R4, R5, ASR #16      //Even row, upper 16bits of R4 and R5
    STMIA  R1!, {R8, R9}
    PKHBT  R8,  R7, R6, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R9,  R6, R7, ASR #16      //Even row, upper 16bits of R4 and R5
    STMIA  R1!, {R8, R9}
    B      dword4_col_loop_add_to_both_relu
    exit_dword4_col_loop_add_to_both_relu
    ADD    R10, #0x4
    dword1_col_loop_add_to_both_relu
    SUBS   R10, #0x1
    BLT    exit_col_loop_add_to_both_relu
    LDR    R4,  [R1]              //Read dword from bank addr, then increase addr
    MOV    R9,  #0
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4                //Add and relu activation
    SELGE  R6,  R6,  R9
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram
    SADD16 R7,  R4                //Add and relu activation
    SELGE  R7,  R7,  R9
    STRH   R7,  [R3], #2          //Store half word at even row to sdram, then increase addr
    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    B      dword1_col_loop_add_to_both_relu
    exit_col_loop_add_to_both_relu
    LDRD   R9,  R12, [sp, #0x28]  //Get  len_col valid_len_col
    SUB    R0,  R0, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R3,  R3, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R3,  R3, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R1,  R1, R12, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R9,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R11, #0x2              //Decrease 2 rows
    B      row_loop_add_to_both_relu
    exit_row_loop_add_to_both_relu
    ADD    R14, #1
    LDR    R1,  [sp, #0x48]        //Get  bank_addr of last channel
    ANDS   R4,  R14, #1            //Check Odd channel or even channel
    BNE    oddch_addrinc_add_to_both_relu
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x40]        //Increase bank addr for even channel
    ADD    R1,  R4
    oddch_addrinc_add_to_both_relu
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x3c]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x44]        //Get  mem_addr of last channel
    ADD    R0,  R4
    LDR    R3,  [sp, #0x4c]        //Get  dst_mem_addr of last channel
    ADD    R3,  R4
    B      channel_loop_add_to_both_relu
    exit_add_to_both_relu
    POP     {R4 - R12, LR}
    BX      lr
}
/*
 *R0: mem_addr   Memory source address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: dst_mem_addr    Memory destination address
 *R4-R7:         Data from Memory(4 cols, 2 rows)
 *R8-R9:         Data from Bank(2 cols, 2 rows)
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 *R12: tmp
 */
__asm void *AsmAddToBankRelu(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int16_t *dst_mem_addr, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr)
{
    PUSH   {R4 - R12, LR}

    MOV    R14, #0                 //Start channel is 0
    channel_loop_add_to_bank_relu
    LDR    R4,  [sp, #0x38]        //Get  len_channel
    CMP    R14, R4
    BGE.W  exit_add_to_bank_relu
    STRD   R0,  R1,  [sp, #0x44]   //Save mem_addr and bank addr
    LDR    R11, [sp, #0x30]  //Get  valid_len_row
    row_loop_add_to_bank_relu
    CMP    R11, #1           //Check bank row end
    BLT.W  exit_row_loop_add_to_bank_relu
    LDR    R10, [sp, #0x34]  //Get valid_row_off(0x30)
    LDR    R8,  [sp, #0x30]  //Get  valid_len_row
    SUB    R8,  R11
    CMP    R8,  R10          //Check bank row start
    BLT    exit_col_loop_add_to_bank_relu
    LDR    R10, [sp, #0x2c]  //Get  valid_len_col
    ANDS   R8,  R0, #3       //Check source and dest alignment
    BEQ    dword4_col_loop_add_to_bank_relu
    LDR    R4,  [R1]              //Read dword from bank addr
    MOV    R9,  #0
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row from sdram
    SADD16 R6,  R4                //Add and relu activation
    SELGE  R6,  R6,  R9
    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram, then increase addr
    SADD16 R7,  R4                //Add and relu activation
    SELGE  R7,  R7,  R9
    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    SUBS   R10, #0x1
    dword4_col_loop_add_to_bank_relu
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop_add_to_bank_relu
    ADD    R4,  R0, R2, LSL #1
    LDRD   R5,  R7, [R4]              //Get odd row (mem_addr + col_num) R5-R7
    LDMIA  R0!, {R4, R6}              //Get even row (mem_addr) R4-R6
    LDM    R1,  {R8, R9}
    PKHBT  R12, R5, R4, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R4,  R4, R5, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R8,  R12                  //Add and relu activation
    MOV    R5,  #0
    SELGE  R8,  R8,  R5
    SADD16 R9,  R4
    SELGE  R9,  R9,  R5
    STMIA  R1!, {R8, R9}
    LDM    R1,  {R8, R9}
    PKHBT  R12, R7, R6, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R4,  R6, R7, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R8,  R12                  //Add and relu activation
    MOV    R5,  #0
    SELGE  R8,  R8,  R5
    SADD16 R9,  R4
    SELGE  R9,  R9,  R5
    STMIA  R1!, {R8, R9}
    B      dword4_col_loop_add_to_bank_relu
    exit_dword4_col_loop_add_to_bank_relu
    ADD    R10, #0x4
    dword1_col_loop_add_to_bank_relu
    SUBS   R10, #0x1
    BLT    exit_col_loop_add_to_bank_relu
    LDR    R4,  [R1]              //Read dword from bank addr
    MOV    R9,  #0
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4                //Add and relu activation
    SELGE  R6,  R6,  R9
    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram
    SADD16 R7,  R4                //Add and relu activation
    SELGE  R7,  R7,  R9
    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    B      dword1_col_loop_add_to_bank_relu
    exit_col_loop_add_to_bank_relu
    LDRD   R9,  R12, [sp, #0x28] //Get  len_col valid_len_col
    SUB    R0,  R0, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R1,  R1, R12, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R9,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R11, #0x2              //Decrease 2 rows
    B      row_loop_add_to_bank_relu
    exit_row_loop_add_to_bank_relu
    ADD    R14, #1
    LDR    R1,  [sp, #0x48]        //Get  bank_addr of last channel
    ANDS   R4,  R14, #1            //Check Odd channel or even channel
    BNE    oddch_addrinc_add_to_bank_relu
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x40]        //Increase bank addr for even channel
    ADD    R1,  R4
    oddch_addrinc_add_to_bank_relu
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x3c]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x44]        //Get  mem_addr of last channel
    ADD    R0,  R4
    B      channel_loop_add_to_bank_relu
    exit_add_to_bank_relu
    POP     {R4 - R12, LR}
    BX      lr
}
/*
 *R0: mem_addr   Memory source address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: dst_mem_addr    Memory destination address
 *R4-R7:         Data from Memory(4 cols, 2 rows)
 *R8-R9:         Data from Bank(2 cols, 2 rows)
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 *R12: tmp
 */
__asm void *AsmAddToMemLeaky(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int16_t *dst_mem_addr, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr, int32_t dst_last_mem_addr)
{
    PUSH   {R4 - R12, LR}

    MOV    R14, #0                 //Start channel is 0
    channel_loop_add_to_mem_leaky
    LDR    R4,  [sp, #0x38]        //Get  len_channel
    CMP    R14, R4
    BGE.W  exit_add_to_mem_leaky
    STRD   R0,  R1,  [sp, #0x44]   //Save mem_addr and bank addr
    STR    R3,  [sp, #0x4c]        //Save dst_mem_addr
    LDR    R11, [sp, #0x30]  //Get  valid_len_row
    row_loop_add_to_mem_leaky
    CMP    R11, #1           //Check bank row end
    BLT.W  exit_row_loop_add_to_mem_leaky
    LDR    R10, [sp, #0x34]  //Get valid_row_off(0x30)
    LDR    R8,  [sp, #0x30]  //Get  valid_len_row
    SUB    R8,  R11
    CMP    R8,  R10          //Check bank row start
    BLT.W  exit_col_loop_add_to_mem_leaky
    LDR    R10, [sp, #0x2c]  //Get  valid_len_col
    ANDS   R8,  R0, #3       //Check source and dest alignment
    BNE    dword1_start_loop_add_to_mem_leaky
    ANDS   R8,  R3, #3
    BEQ    dword4_col_loop_add_to_mem_leaky
    dword1_start_loop_add_to_mem_leaky
    LDR    R4,  [R1], #4          //Read dword from bank addr, then increase addr
    MOV    R9,  #0x1A
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4                //Add and relu leaky activation
    SMULBB R8,  R6,  R9
    ASR    R8,  #8
    SELGE  R6,  R6,  R8

    CMP    R11, #0x1              //Check remain valid_len_row > 1
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R6,  [R0], #2          //Get half word at even row from sdram
    SADD16 R6,  R4                //Add and relu leaky activation
    SMULBB R8,  R6,  R9
    ASR    R8,  #8
    SELGE  R6,  R6,  R8

    STRH   R6,  [R3], #2          //Store half word at even row to sdram, then increase addr
    SUBS   R10, #0x1
    ANDS   R8,  R0, #3            //Check source and dest alignment again
    BNE    dword1_col_loop_add_to_mem_leaky
    ANDS   R8,  R3, #3
    BNE    dword1_col_loop_add_to_mem_leaky
    dword4_col_loop_add_to_mem_leaky
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop_add_to_mem_leaky
    ADD    R4,  R0, R2, LSL #1
    LDRD   R5,  R7, [R4]              //Get odd row (mem_addr + col_num) R5-R7
    LDMIA  R0!, {R4, R6}              //Get even row (mem_addr) R4-R6
    LDMIA  R1!, {R8, R9}
    PKHBT  R12, R8, R9, LSL #16       //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16       //Even row, upper 16bits of R4 and R5
    SADD16 R4,  R8                    //Add and relu leaky activation
    MOV    R9,  #0x1A
    SMULTB R8,  R4,  R9
    ASR    R8,  #8
    SMULBB R9,  R4,  R9
    ASR    R9,  #8
    PKHBT  R8,  R9,  R8, LSL #16
    SELGE  R4,  R4,  R8

    SADD16 R5,  R12                   //Add and relu leaky activation
    MOV    R9,  #0x1A
    SMULTB R8,  R5,  R9
    ASR    R8,  #8
    SMULBB R9,  R5,  R9
    ASR    R9,  #8
    PKHBT  R8,  R9,  R8, LSL #16
    SELGE  R5,  R5,  R8

    LDMIA  R1!, {R8, R9}
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R6,  R8                   //Add and relu leaky activation
    MOV    R9,  #0x1A
    SMULTB R8,  R6,  R9
    ASR    R8,  #8
    SMULBB R9,  R6,  R9
    ASR    R9,  #8
    PKHBT  R8,  R9,  R8, LSL #16
    SELGE  R6,  R6,  R8

    SADD16 R7,  R12                //Add and relu leaky activation
    MOV    R9,  #0x1A
    SMULTB R8,  R7,  R9
    ASR    R8,  #8
    SMULBB R9,  R7,  R9
    ASR    R9,  #8
    PKHBT  R8,  R9,  R8, LSL #16
    SELGE  R7,  R7,  R8

    ADD    R12, R3, R2, LSL #1
    CMP    R11, #0x1                  //Check remain valid_len_row > 1
    STRDGT R5,  R7, [R12]             //Store odd row
    STMIA  R3!, {R4, R6}              //Store even row
    B      dword4_col_loop_add_to_mem_leaky
    exit_dword4_col_loop_add_to_mem_leaky
    ADD    R10, #0x4
    dword1_col_loop_add_to_mem_leaky
    SUBS   R10, #0x1
    BLT    exit_col_loop_add_to_mem_leaky
    LDR    R4,  [R1], #4          //Read dword from bank addr, then increase addr
    MOV    R9,  #0x1A
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4                //Add and relu leaky activation
    SMULBB R8,  R6,  R9
    ASR    R8,  #8
    SELGE  R6,  R6,  R8

    CMP    R11, #0x1              //Check remain valid_len_row > 1
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R6,  [R0], #2          //Get half word at even row from sdram
    SADD16 R6,  R4                //Add and relu leaky activation
    SMULBB R8,  R6,  R9
    ASR    R8,  #8
    SELGE  R6,  R6,  R8

    STRH   R6,  [R3], #2          //Store half word at even row to sdram, then increase addr
    B      dword1_col_loop_add_to_mem_leaky
    exit_col_loop_add_to_mem_leaky
    LDRD   R9,  R12, [sp, #0x28]  //Get  len_col valid_len_col
    SUB    R0,  R0, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R3,  R3, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R3,  R3, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R1,  R1, R12, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R9,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R11, #0x2              //Decrease 2 rows
    B      row_loop_add_to_mem_leaky
    exit_row_loop_add_to_mem_leaky
    ADD    R14, #1
    LDR    R1,  [sp, #0x48]        //Get  bank_addr of last channel
    ANDS   R4,  R14, #1            //Check Odd channel or even channel
    BNE    oddch_addrinc_add_to_mem_leaky
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x40]        //Increase bank addr for even channel
    ADD    R1,  R4
    oddch_addrinc_add_to_mem_leaky
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x3c]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x44]        //Get  mem_addr of last channel
    ADD    R0,  R4
    LDR    R3,  [sp, #0x4c]        //Get  dst_mem_addr of last channel
    ADD    R3,  R4
    B      channel_loop_add_to_mem_leaky
    exit_add_to_mem_leaky
    POP     {R4 - R12, LR}
    BX      lr
}

/*
 *R0: mem_addr   Memory source address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: dst_mem_addr    Memory destination address
 *R4-R7:         Data from Memory(4 cols, 2 rows)
 *R8-R9:         Data from Bank(2 cols, 2 rows)
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 *R12: tmp
 */
__asm void *AsmAddToBothLeaky(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int16_t *dst_mem_addr, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr, int32_t dst_last_mem_addr)
{
    PUSH   {R4 - R12, LR}
    MOV    R14, #0                 //Start channel is 0
    channel_loop_add_to_both_leaky
    LDR    R4,  [sp, #0x38]        //Get  len_channel
    CMP    R14, R4
    BGE.W  exit_add_to_both_leaky
    STRD   R0,  R1,  [sp, #0x44]   //Save mem_addr and bank addr
    STR    R3,  [sp, #0x4c]        //Save dst_mem_addr
    LDR    R11, [sp, #0x30]  //Get  valid_len_row
    row_loop_add_to_both_leaky
    CMP    R11, #1           //Check bank row end
    BLT.W  exit_row_loop_add_to_both_leaky
    LDR    R10, [sp, #0x34]  //Get valid_row_off(0x30)
    LDR    R8,  [sp, #0x30]  //Get  valid_len_row
    SUB    R8,  R11
    CMP    R8,  R10          //Check bank row start
    BLT.W  exit_col_loop_add_to_both_leaky
    LDR    R10, [sp, #0x2c]  //Get  valid_len_col
    ANDS   R8,  R0, #3       //Check source and dest alignment
    BNE    dword1_start_loop_add_to_both_leaky
    ANDS   R8,  R3, #3
    BEQ    dword4_col_loop_add_to_both_leaky
    dword1_start_loop_add_to_both_leaky
    LDR    R4,  [R1]              //Read dword from bank addr
    MOV    R9,  #0x1A
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4                //Add and relu leaky activation
    SMULBB R8,  R6,  R9
    ASR    R8,  #8
    SELGE  R6,  R6,  R8

    CMP    R11, #0x1              //Check remain valid_len_row > 1
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram
    SADD16 R7,  R4                //Add and relu leaky activation
    SMULBB R8,  R7,  R9
    ASR    R8,  #8
    SELGE  R7,  R7,  R8

    STRH   R7,  [R3], #2          //Store half word at even row to sdram, then increase addr
    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    SUBS   R10, #0x1
    ANDS   R8,  R0, #3            //Check source and dest alignment again
    BNE    dword1_col_loop_add_to_both_leaky
    ANDS   R8,  R3, #3
    BNE    dword1_col_loop_add_to_both_leaky
    dword4_col_loop_add_to_both_leaky
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop_add_to_both_leaky
    ADD    R4,  R0, R2, LSL #1
    LDRD   R5,  R7, [R4]              //Get odd row (mem_addr + col_num) R5-R7
    LDMIA  R0!, {R4, R6}              //Get even row (mem_addr) R4-R6
    LDRD   R8,  R9, [R1, #0]
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R4,  R8                   //Add and relu leaky activation
    MOV    R9,  #0x1A
    SMULTB R8,  R4,  R9
    ASR    R8,  #8
    SMULBB R9,  R4,  R9
    ASR    R9,  #8
    PKHBT  R8,  R9,  R8, LSL #16
    SELGE  R4,  R4,  R8

    SADD16 R5,  R12                 //Add and relu leaky activation
    MOV    R9,  #0x1A
    SMULTB R8,  R5,  R9
    ASR    R8,  #8
    SMULBB R9,  R5,  R9
    ASR    R9,  #8
    PKHBT  R8,  R9,  R8, LSL #16
    SELGE  R5,  R5,  R8

    LDRD   R8,  R9, [R1, #8]
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R6,  R8                   //Add and relu leaky activation
    MOV    R9,  #0x1A
    SMULTB R8,  R6,  R9
    ASR    R8,  #8
    SMULBB R9,  R6,  R9
    ASR    R9,  #8
    PKHBT  R8,  R9,  R8, LSL #16
    SELGE  R6,  R6,  R8

    SADD16 R7,  R12                 //Add and relu leaky activation
    MOV    R9,  #0x1A
    SMULTB R8,  R7,  R9
    ASR    R8,  #8
    SMULBB R9,  R7,  R9
    ASR    R9,  #8
    PKHBT  R8,  R9,  R8, LSL #16
    SELGE  R7,  R7,  R8

    ADD    R12, R3, R2, LSL #1
    CMP    R11, #0x1                  //Check remain valid_len_row > 1
    STRDGT R5,  R7, [R12]             //Store odd row
    STMIA  R3!, {R4, R6}              //Store even row
    PKHBT  R8,  R5, R4, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R9,  R4, R5, ASR #16      //Even row, upper 16bits of R4 and R5
    STMIA  R1!, {R8, R9}
    PKHBT  R8,  R7, R6, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R9,  R6, R7, ASR #16      //Even row, upper 16bits of R4 and R5
    STMIA  R1!, {R8, R9}
    B      dword4_col_loop_add_to_both_leaky
    exit_dword4_col_loop_add_to_both_leaky
    ADD    R10, #0x4
    dword1_col_loop_add_to_both_leaky
    SUBS   R10, #0x1
    BLT    exit_col_loop_add_to_both_leaky
    LDR    R4,  [R1]              //Read dword from bank addr, then increase addr
    MOV    R9,  #0x1A
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4                //Add and relu leaky activation
    SMULBB R8,  R6,  R9
    ASR    R8,  #8
    SELGE  R6,  R6,  R8

    CMP    R11, #0x1              //Check remain valid_len_row > 1
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram
    SADD16 R7,  R4                //Add and relu leaky activation
    SMULBB R8,  R7,  R9
    ASR    R8,  #8
    SELGE  R7,  R7,  R8

    STRH   R7,  [R3], #2          //Store half word at even row to sdram, then increase addr
    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    B      dword1_col_loop_add_to_both_leaky
    exit_col_loop_add_to_both_leaky
    LDRD   R9,  R12, [sp, #0x28]  //Get  len_col valid_len_col
    SUB    R0,  R0, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R3,  R3, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R3,  R3, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R1,  R1, R12, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R9,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R11, #0x2              //Decrease 2 rows
    B      row_loop_add_to_both_leaky
    exit_row_loop_add_to_both_leaky
    ADD    R14, #1
    LDR    R1,  [sp, #0x48]        //Get  bank_addr of last channel
    ANDS   R4,  R14, #1            //Check Odd channel or even channel
    BNE    oddch_addrinc_add_to_both_leaky
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x40]        //Increase bank addr for even channel
    ADD    R1,  R4
    oddch_addrinc_add_to_both_leaky
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x3c]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x44]        //Get  mem_addr of last channel
    ADD    R0,  R4
    LDR    R3,  [sp, #0x4c]        //Get  dst_mem_addr of last channel
    ADD    R3,  R4
    B      channel_loop_add_to_both_leaky
    exit_add_to_both_leaky
    POP     {R4 - R12, LR}
    BX      lr
}
/*
 *R0: mem_addr   Memory source address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: dst_mem_addr    Memory destination address
 *R4-R7:         Data from Memory(4 cols, 2 rows)
 *R8-R9:         Data from Bank(2 cols, 2 rows)
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 *R12: tmp
 */
__asm void *AsmAddToBankLeaky(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int16_t *dst_mem_addr, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr)
{
    PUSH   {R4 - R12, LR}

    MOV    R14, #0                 //Start channel is 0
    channel_loop_add_to_bank_leaky
    LDR    R4,  [sp, #0x38]        //Get  len_channel
    CMP    R14, R4
    BGE.W  exit_add_to_bank_leaky
    STRD   R0,  R1,  [sp, #0x44]   //Save mem_addr and bank addr
    LDR    R11, [sp, #0x30]  //Get  valid_len_row
    row_loop_add_to_bank_leaky
    CMP    R11, #1           //Check bank row end
    BLT.W  exit_row_loop_add_to_bank_leaky
    LDR    R10, [sp, #0x34]  //Get valid_row_off(0x30)
    LDR    R8,  [sp, #0x30]  //Get  valid_len_row
    SUB    R8,  R11
    CMP    R8,  R10          //Check bank row start
    BLT.W  exit_col_loop_add_to_bank_leaky
    LDR    R10, [sp, #0x2c]  //Get  valid_len_col
    ANDS   R8,  R0, #3       //Check source and dest alignment
    BEQ    dword4_col_loop_add_to_bank_leaky
    LDR    R4,  [R1]              //Read dword from bank addr
    MOV    R9,  #0x1A
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row from sdram
    SADD16 R6,  R4                //Add and relu leaky activation
    SMULBB R8,  R6,  R9
    ASR    R8,  #8
    SELGE  R6,  R6,  R8

    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram, then increase addr
    SADD16 R7,  R4                //Add and relu leaky activation
    SMULBB R8,  R7,  R9
    ASR    R8,  #8
    SELGE  R7,  R7,  R8

    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    SUBS   R10, #0x1
    dword4_col_loop_add_to_bank_leaky
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop_add_to_bank_leaky
    ADD    R4,  R0, R2, LSL #1
    LDRD   R5,  R7, [R4]              //Get odd row (mem_addr + col_num) R5-R7
    LDMIA  R0!, {R4, R6}              //Get even row (mem_addr) R4-R6
    LDM    R1,  {R8, R9}
    PKHBT  R12, R5, R4, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R4,  R4, R5, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R9,  R4                   //Add and relu leaky activation
    MOV    R5,  #0x1A
    SMULTB R4,  R9,  R5
    ASR    R4,  #8
    SMULBB R5,  R9,  R5
    ASR    R5,  #8
    PKHBT  R4,  R5,  R4, LSL #16
    SELGE  R9,  R9,  R4

    SADD16 R8,  R12                  //Add and relu leaky activation
    MOV    R5,  #0x1A
    SMULTB R4,  R8,  R5
    ASR    R4,  #8
    SMULBB R5,  R8,  R5
    ASR    R5,  #8
    PKHBT  R4,  R5,  R4, LSL #16
    SELGE  R8,  R8,  R4

    STMIA  R1!, {R8, R9}
    LDM    R1,  {R8, R9}
    PKHBT  R12, R7, R6, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R4,  R6, R7, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R9,  R4                   //Add and relu leaky activation
    MOV    R5,  #0x1A
    SMULTB R4,  R9,  R5
    ASR    R4,  #8
    SMULBB R5,  R9,  R5
    ASR    R5,  #8
    PKHBT  R4,  R5,  R4, LSL #16
    SELGE  R9,  R9,  R4

    SADD16 R8,  R12                 //Add and relu leaky activation
    MOV    R5,  #0x1A
    SMULTB R4,  R8,  R5
    ASR    R4,  #8
    SMULBB R5,  R8,  R5
    ASR    R5,  #8
    PKHBT  R4,  R5,  R4, LSL #16
    SELGE  R8,  R8,  R4

    STMIA  R1!, {R8, R9}
    B      dword4_col_loop_add_to_bank_leaky
    exit_dword4_col_loop_add_to_bank_leaky
    ADD    R10, #0x4
    dword1_col_loop_add_to_bank_leaky
    SUBS   R10, #0x1
    BLT    exit_col_loop_add_to_bank_leaky
    LDR    R4,  [R1]              //Read dword from bank addr
    MOV    R9,  #0x1A
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4                //Add and relu leaky activation
    SMULBB R8,  R6,  R9
    ASR    R8,  #8
    SELGE  R6,  R6,  R8

    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram
    SADD16 R7,  R4                //Add and relu leaky activation
    SMULBB R8,  R7,  R9
    ASR    R8,  #8
    SELGE  R7,  R7,  R8

    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    B      dword1_col_loop_add_to_bank_leaky
    exit_col_loop_add_to_bank_leaky
    LDRD   R9,  R12, [sp, #0x28] //Get  len_col valid_len_col
    SUB    R0,  R0, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R1,  R1, R12, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R9,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R11, #0x2              //Decrease 2 rows
    B      row_loop_add_to_bank_leaky
    exit_row_loop_add_to_bank_leaky
    ADD    R14, #1
    LDR    R1,  [sp, #0x48]        //Get  bank_addr of last channel
    ANDS   R4,  R14, #1            //Check Odd channel or even channel
    BNE    oddch_addrinc_add_to_bank_leaky
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x40]        //Increase bank addr for even channel
    ADD    R1,  R4
    oddch_addrinc_add_to_bank_leaky
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x3c]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x44]        //Get  mem_addr of last channel
    ADD    R0,  R4
    B      channel_loop_add_to_bank_leaky
    exit_add_to_bank_leaky
    POP     {R4 - R12, LR}
    BX      lr
}

/*
 *R0: mem_addr   Memory source address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: dst_mem_addr    Memory destination address
 *R4-R7:         Data from Memory(4 cols, 2 rows)
 *R8-R9:         Data from Bank(2 cols, 2 rows)
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 *R12: tmp
 */
__asm void *AsmAddToMem(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int16_t *dst_mem_addr, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr, int32_t dst_last_mem_addr)
{
    PUSH   {R4 - R12, LR}

    MOV    R14, #0                 //Start channel is 0
    channel_loop_add_to_mem
    LDR    R4,  [sp, #0x38]        //Get  len_channel
    CMP    R14, R4
    BGE.W  exit_add_to_mem
    STRD   R0,  R1,  [sp, #0x44]   //Save mem_addr and bank addr
    STR    R3,  [sp, #0x4c]        //Save dst_mem_addr
    LDR    R11, [sp, #0x30]  //Get  valid_len_row
    row_loop_add_to_mem
    CMP    R11, #1           //Check bank row end
    BLT.W  exit_row_loop_add_to_mem
    LDR    R10, [sp, #0x34]  //Get valid_row_off(0x30)
    LDR    R8,  [sp, #0x30]  //Get  valid_len_row
    SUB    R8,  R11
    CMP    R8,  R10          //Check bank row start
    BLT    exit_col_loop_add_to_mem
    LDR    R10, [sp, #0x2c]  //Get  valid_len_col
    ANDS   R8,  R0, #3       //Check source and dest alignment
    BNE    dword1_start_loop_add_to_mem
    ANDS   R8,  R3, #3
    BEQ    dword4_col_loop_add_to_mem
    dword1_start_loop_add_to_mem
    LDR    R4,  [R1], #4          //Read dword from bank addr, then increase addr
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R6,  [R0], #2          //Get half word at even row from sdram
    SADD16 R6,  R4
    STRH   R6,  [R3], #2          //Store half word at even row to sdram, then increase addr
    SUBS   R10, #0x1
    ANDS   R8,  R0, #3            //Check source and dest alignment again
    BNE    dword1_col_loop_add_to_mem
    ANDS   R8,  R3, #3
    BNE    dword1_col_loop_add_to_mem
    dword4_col_loop_add_to_mem
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop_add_to_mem
    ADD    R4,  R0, R2, LSL #1
    LDRD   R5,  R7, [R4]              //Get odd row (mem_addr + col_num) R5-R7
    LDMIA  R0!, {R4, R6}              //Get even row (mem_addr) R4-R6
    LDMIA  R1!, {R8, R9}
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R4,  R8
    SADD16 R5,  R12
    LDMIA  R1!, {R8, R9}
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R6,  R8
    SADD16 R7,  R12
    ADD    R12, R3, R2, LSL #1
    CMP    R11, #0x1                  //Check remain valid_len_row > 1
    STRDGT R5,  R7, [R12]             //Store odd row
    STMIA  R3!, {R4, R6}              //Store even row
    B      dword4_col_loop_add_to_mem
    exit_dword4_col_loop_add_to_mem
    ADD    R10, #0x4
    dword1_col_loop_add_to_mem
    SUBS   R10, #0x1
    BLT    exit_col_loop_add_to_mem
    LDR    R4,  [R1], #4          //Read dword from bank addr, then increase addr
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R6,  [R0], #2          //Get half word at even row from sdram
    SADD16 R6,  R4
    STRH   R6,  [R3], #2          //Store half word at even row to sdram, then increase addr
    B      dword1_col_loop_add_to_mem
    exit_col_loop_add_to_mem
    LDRD   R9,  R12, [sp, #0x28]  //Get  len_col valid_len_col
    SUB    R0,  R0, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R3,  R3, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R3,  R3, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R1,  R1, R12, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R9,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R11, #0x2              //Decrease 2 rows
    B      row_loop_add_to_mem
    exit_row_loop_add_to_mem
    ADD    R14, #1
    LDR    R1,  [sp, #0x48]        //Get bank_addr of last channel
    ANDS   R4,  R14, #1            //Check Odd channel or even channel
    BNE    oddch_addrinc_add_to_mem
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x40]        //Increase bank addr for even channel
    ADD    R1,  R4
    oddch_addrinc_add_to_mem
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x3c]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x44]        //Get  mem_addr of last channel
    ADD    R0,  R4
    LDR    R3,  [sp, #0x4c]        //Get  dst_mem_addr of last channel
    ADD    R3,  R4
    B      channel_loop_add_to_mem
    exit_add_to_mem
    POP     {R4 - R12, LR}
    BX      lr
}

/*
 *R0: mem_addr   Memory source address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: dst_mem_addr    Memory destination address
 *R4-R7:         Data from Memory(4 cols, 2 rows)
 *R8-R9:         Data from Bank(2 cols, 2 rows)
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 *R12: tmp
 */
__asm void *AsmAddToBoth(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int16_t *dst_mem_addr, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr, int32_t dst_last_mem_addr)
{
    PUSH   {R4 - R12, LR}

    MOV    R14, #0                 //Start channel is 0
    channel_loop_add_to_both
    LDR    R4,  [sp, #0x38]        //Get  len_channel
    CMP    R14, R4
    BGE.W  exit_add_to_both
    STRD   R0,  R1,  [sp, #0x44]   //Save mem_addr and bank addr
    STR    R3,  [sp, #0x4c]        //Save dst_mem_addr
    LDR    R11, [sp, #0x30]  //Get  valid_len_row
    row_loop_add_to_both
    CMP    R11, #1           //Check bank row end
    BLT.W  exit_row_loop_add_to_both
    LDR    R10, [sp, #0x34]  //Get valid_row_off(0x30)
    LDR    R8,  [sp, #0x30]  //Get  valid_len_row
    SUB    R8,  R11
    CMP    R8,  R10          //Check bank row start
    BLT    exit_col_loop_add_to_both
    LDR    R10, [sp, #0x2c]  //Get  valid_len_col
    ANDS   R8,  R0, #3       //Check source and dest alignment
    BNE    dword1_start_loop_add_to_both
    ANDS   R8,  R3, #3
    BEQ    dword4_col_loop_add_to_both
    dword1_start_loop_add_to_both
    LDR    R4,  [R1]              //Read dword from bank addr
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram
    SADD16 R7,  R4
    STRH   R7,  [R3], #2          //Store half word at even row to sdram, then increase addr
    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    SUBS   R10, #0x1
    ANDS   R8,  R0, #3            //Check source and dest alignment again
    BNE    dword1_col_loop_add_to_both
    ANDS   R8,  R3, #3
    BNE    dword1_col_loop_add_to_both
    dword4_col_loop_add_to_both
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop_add_to_both
    ADD    R4,  R0, R2, LSL #1
    LDRD   R5,  R7, [R4]              //Get odd row (mem_addr + col_num) R5-R7
    LDMIA  R0!, {R4, R6}              //Get even row (mem_addr) R4-R6
    LDRD   R8,  R9, [R1, #0]
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R4,  R8
    SADD16 R5,  R12
    LDRD   R8,  R9, [R1, #8]
    PKHBT  R12, R8, R9, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R8,  R9, R8, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R6,  R8
    SADD16 R7,  R12
    ADD    R12, R3, R2, LSL #1
    CMP    R11, #0x1                  //Check remain valid_len_row > 1
    STRDGT R5,  R7, [R12]             //Store odd row
    STMIA  R3!, {R4, R6}              //Store even row
    PKHBT  R8,  R5, R4, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R9,  R4, R5, ASR #16      //Even row, upper 16bits of R4 and R5
    STMIA  R1!, {R8, R9}
    PKHBT  R8,  R7, R6, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R9,  R6, R7, ASR #16      //Even row, upper 16bits of R4 and R5
    STMIA  R1!, {R8, R9}
    B      dword4_col_loop_add_to_both
    exit_dword4_col_loop_add_to_both
    ADD    R10, #0x4
    dword1_col_loop_add_to_both
    SUBS   R10, #0x1
    BLT    exit_col_loop_add_to_both
    LDR    R4,  [R1]              //Read dword from bank addr, then increase addr
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4
    STRHGT R6,  [R3,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram
    SADD16 R7,  R4
    STRH   R7,  [R3], #2          //Store half word at even row to sdram, then increase addr
    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    B      dword1_col_loop_add_to_both
    exit_col_loop_add_to_both
    LDRD   R9,  R12, [sp, #0x28]  //Get  len_col valid_len_col
    SUB    R0,  R0, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R3,  R3, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R3,  R3, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R1,  R1, R12, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R9,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R11, #0x2              //Decrease 2 rows
    B      row_loop_add_to_both
    exit_row_loop_add_to_both
    ADD    R14, #1
    LDR    R1,  [sp, #0x48]        //Get  bank_addr of last channel
    ANDS   R4,  R14, #1            //Check Odd channel or even channel
    BNE    oddch_addrinc_add_to_both
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x40]        //Increase bank addr for even channel
    ADD    R1,  R4
    oddch_addrinc_add_to_both
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x3c]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x44]        //Get  mem_addr of last channel
    ADD    R0,  R4
    LDR    R3,  [sp, #0x4c]        //Get  dst_mem_addr of last channel
    ADD    R3,  R4
    B      channel_loop_add_to_both
    exit_add_to_both
    POP     {R4 - R12, LR}
    BX      lr
}
/*
 *R0: mem_addr   Memory source address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: dst_mem_addr    Memory destination address
 *R4-R7:         Data from Memory(4 cols, 2 rows)
 *R8-R9:         Data from Bank(2 cols, 2 rows)
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 *R12: tmp
 */
__asm void *AsmAddToBank(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int16_t *dst_mem_addr, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr)
{
    PUSH   {R4 - R12, LR}

    MOV    R14, #0                 //Start channel is 0
    channel_loop_add_to_bank
    LDR    R4,  [sp, #0x38]        //Get  len_channel
    CMP    R14, R4
    BGE.W  exit_add_to_bank
    STRD   R0,  R1,  [sp, #0x44]   //Save mem_addr and bank addr
    LDR    R11, [sp, #0x30]  //Get  valid_len_row
    row_loop_add_to_bank
    CMP    R11, #1           //Check bank row end
    BLT.W  exit_row_loop_add_to_bank
    LDR    R10, [sp, #0x34]  //Get valid_row_off(0x30)
    LDR    R8,  [sp, #0x30]  //Get  valid_len_row
    SUB    R8,  R11
    CMP    R8,  R10          //Check bank row start
    BLT    exit_col_loop_add_to_bank
    LDR    R10, [sp, #0x2c]  //Get  valid_len_col
    ANDS   R8,  R0, #3       //Check source and dest alignment
    BEQ    dword4_col_loop_add_to_bank
    LDR    R4,  [R1]              //Read dword from bank addr
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row from sdram
    SADD16 R6,  R4
    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram, then increase addr
    SADD16 R7,  R4
    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    SUBS   R10, #0x1
    dword4_col_loop_add_to_bank
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop_add_to_bank
    ADD    R4,  R0, R2, LSL #1
    LDRD   R5,  R7, [R4]              //Get odd row (mem_addr + col_num) R5-R7
    LDMIA  R0!, {R4, R6}              //Get even row (mem_addr) R4-R6
    LDM    R1,  {R8, R9}
    PKHBT  R12, R5, R4, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R4,  R4, R5, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R8,  R12
    SADD16 R9,  R4
    STMIA  R1!, {R8, R9}
    LDM    R1,  {R8, R9}
    PKHBT  R12, R7, R6, LSL #16      //Odd row,  lower 16bits of R4 and R5
    PKHTB  R4,  R6, R7, ASR #16      //Even row, upper 16bits of R4 and R5
    SADD16 R8,  R12
    SADD16 R9,  R4
    STMIA  R1!, {R8, R9}
    B      dword4_col_loop_add_to_bank
    exit_dword4_col_loop_add_to_bank
    ADD    R10, #0x4
    dword1_col_loop_add_to_bank
    SUBS   R10, #0x1
    BLT    exit_col_loop_add_to_bank
    LDR    R4,  [R1]              //Read dword from bank addr
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    LDRH   R6,  [R0,  R2, LSL #1] //Get half word at odd row to sdram
    SADD16 R6,  R4
    ASR    R4,  R4,   #16
    LDRH   R7,  [R0], #2          //Get half word at even row from sdram
    SADD16 R7,  R4
    PKHBT  R12, R6, R7, LSL #16   //Lower 16bits of R6 and R7
    STR    R12, [R1], #4          //Write dword into bank addr, then increase addr
    B      dword1_col_loop_add_to_bank
    exit_col_loop_add_to_bank
    LDRD   R9,  R12, [sp, #0x28] //Get  len_col valid_len_col
    SUB    R0,  R0, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R1,  R1, R12, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R9,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R11, #0x2              //Decrease 2 rows
    B      row_loop_add_to_bank
    exit_row_loop_add_to_bank
    ADD    R14, #1
    LDR    R1,  [sp, #0x48]        //Get  bank_addr of last channel
    ANDS   R4,  R14, #1            //Check Odd channel or even channel
    BNE    oddch_addrinc_add_to_bank
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x40]        //Increase bank addr for even channel
    ADD    R1,  R4
    oddch_addrinc_add_to_bank
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x3c]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x44]        //Get  mem_addr of last channel
    ADD    R0,  R4
    B      channel_loop_add_to_bank
    exit_add_to_bank
    POP     {R4 - R12, LR}
    BX      lr
}

/*
 *R0: mem_addr   Memory destination address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: len_col    Column number in bank
 *R4-R7:         Data from Bank(4 cols, 2 rows)
 *R8-R9:         Temp Data
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 *R12: valid_len_col(backup value)
 *R14: ch        channel loop counter, start from 0
 */
__asm void *AsmCopyToMem(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr)
{
    PUSH   {R4 - R12, LR}
    MOV    R14, #0                 //Start channel is 0
    channel_loop
    LDR    R4,  [sp, #0x34]        //Get  len_channel
    CMP    R14, R4
    BGE.W    exit_copy_to_mem          // BGE
    STRD   R0,  R1,  [sp, #0x40]   //Save mem_addr and bank addr
    LDRD   R12, R11, [sp, #0x28]   //Get  valid_len_col, valid_len_row
    row_loop
    CMP    R11, #1                 //Check bank row end
    BLT    exit_row_loop
    LDR    R10, [sp, #0x30]        //Get valid_row_off(0x30)
    LDR    R8, [sp, #0x2c]        //Get valid_len_row(0x2c)
    SUB    R8,  R11
    CMP    R8, R10                //Check bank row start
    BLT    exit_col_loop
    MOV    R10, R12
    ANDS   R8,  R0, #3
    BEQ    check_unaligned_dword4
    LDR    R4,  [R1], #4          //Read dword from bank addr, then increase addr
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    STRHGT R4,  [R0,  R2, LSL #1] //Store half word at odd row to sdram
    ASR    R5,  R4,   #16
    STRH   R5,  [R0], #2          //Store half word at even row to sdram
    SUB    R10, #0x1              //Have procssed one col
    check_unaligned_dword4                    // step1
    ANDS   R8,  R2, #1
    BNE    unaligned_dword4_col_loop
    dword4_col_loop
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop
    LDMIA  R1!, {R4 - R7}
    PKHBT  R8,  R4, R5, LSL #16  //Odd row,  lower 16bits of R4 and R5
    PKHTB  R4,  R5, R4, ASR #16  //Even row, upper 16bits of R4 and R5
    PKHBT  R9,  R6, R7, LSL #16  //Odd row,  lower 16bits of R6 and R7
    PKHTB  R6,  R7, R6, ASR #16  //Even row, upper 16bits of R6 and R7
    ADD    R7,  R0, R2, LSL #1   //Get address of odd row in sdram(+col_num)
    CMP    R11, #0x1             //Check remain valid_len_row > 1
    STMGT  R7,  {R8 - R9}        //Store odd row
    STMIA  R0!, {R4, R6}         //Store even row
    B      dword4_col_loop
    exit_dword4_col_loop
    ADD    R10, #0x4
    B      dword1_col_loop
    unaligned_dword4_col_loop     // step2
    SUBS   R10, #0x4
    BLT    unaligned_exit_dword4_col_loop
    LDMIA  R1!, {R4 - R7}
    ADD    R9,  R0, R2, LSL #1   //Get address of odd row in sdram(+col_num)
    CMP    R11, #0x1             //Check remain valid_len_row > 1
    STRHGT R4,  [R9], #2         //Store half word at odd row to sdram
    PKHBT  R8,  R5, R6, LSL #16  // richard debug
    STRGT  R8,  [R9], #4         //Store half word at odd row to sdram
    STRHGT R7,  [R9], #2         //Store half word at odd row to sdram
    PKHTB  R4,  R5, R4, ASR #16  //Even row, upper 16bits of R4 and R5
    PKHTB  R6,  R7, R6, ASR #16  //Even row, upper 16bits of R6 and R7
    STMIA  R0!, {R4, R6}         //Store even row
    B      unaligned_dword4_col_loop
    unaligned_exit_dword4_col_loop
    ADD    R10, #0x4
    dword1_col_loop
    SUBS   R10, #0x1
    BLT    exit_col_loop
    LDR    R4,  [R1], #4          //Read dword from bank addr, then increase addr
    CMP    R11, #0x1              //Check remain valid_len_row > 1
    STRHGT R4,  [R0,  R2, LSL #1] //Store half word at even row to sdram
    ASR    R5,  R4,   #16
    STRH   R5,  [R0], #2          //Store half word at odd row to sdram
    B      dword1_col_loop
    exit_col_loop
    SUB    R0,  R0, R12, LSL #1   //Mem pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    SUB    R1,  R1, R12, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R3,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R11, #0x2              //Decrease 2 rows
    B      row_loop
    exit_row_loop
    ADD    R14, #1
    LDR    R1,  [sp, #0x44]        //Get  bank_addr of last channel
    ANDS   R4,  R14, #1            //Check Odd channel or even channel
    BNE    odd_channel_addrinc
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x3c]        //Increase bank addr for even channel
    ADD    R1,  R4
    odd_channel_addrinc
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x38]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x40]        //Get  mem_addr of last channel
    ADD    R0,  R4
    B      channel_loop
    exit_copy_to_mem
    POP     {R4 - R12, LR}
    BX      lr
}

/*
 *R0: mem_addr   Memory destination address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: len_col    Column number in bank
 *R4-R7:         Data to Bank(4 cols, 2 rows)
 *R8-R9:         Temp Data
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from 0
 *R12: valid_len_col(backup value)
 *valid_len_col(sp+0x28) valid_len_row(sp+0x2c) start_col(sp+0x30) start_row(sp+0x34) row_num(sp+0x38) valid_col_off(sp+0x3c)
 *valid_row_off(sp+0x40)
 */
__asm void *AsmCopyFromMem(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t start_col, int32_t start_row, int32_t row_num, int32_t valid_col_off, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr, int32_t dst_channel_off, int32_t padding_right_start)
{
    PUSH   {R4 - R12, LR}
    MOV    R14, #0                 //Start channel is 0
    channel_loop_cp_from_mem
    LDR    R4,  [sp, #0x44]        //Get  len_channel
    CMP    R14, R4
    BGE.W  exit_copy_from_mem
    STRD   R0,  R1,  [sp, #0x50]   //Save mem_addr and bank addr

    LDR    R9,  [sp, #0x34]       //Get  start_row
    //Clear padding-up rows R9:row R8: col
    clear_row_loop_cp_from_mem
    CMP    R9,  #0x0
    BGE    exit_clear_loop_cp_from_mem
    MOV    R4,  #0x0
    MOV    R8,  #0x0
    MOV    R7,  R1
    clear_col_loop_cp_from_mem
    SUBS   R5,  R3,  R8
    BLE    exit_clear_col_loop_cp_from_mem
    STRH   R4,  [R7, #2]
    ADDS   R5,  R9,  #0x1
    STRHLT R4,  [R7]
    ADD    R8,  #0x2
    ADD    R7,  #0x4
    B      clear_col_loop_cp_from_mem
    exit_clear_col_loop_cp_from_mem
    ADD    R9,  #0x2
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R3,  LSL #2   //Bank pointer add one col (+ len_col)
    B      clear_row_loop_cp_from_mem
    exit_clear_loop_cp_from_mem                //Clear padding-up rows R9:row R8: col
    MOV    R11,  #0x0                 //Row loop counter start from 0
    row_loop_cp_from_mem
    LDR    R10, [sp, #0x2c]        //Get valid_len_row(0x2c)
    CMP    R11, R10                //Check bank row end
    BGE.W  exit_row_loop_cp_from_mem
    LDR    R9,  [sp, #0x34]        //Get start_row(0x34)
    ADD    R8,  R11, R9            //row + start_row
    LDR    R7,  [sp, #0x38]        //Get row_num
    CMP    R8,  R7                 //Check mem row end
    BGE    clear_bottom_cp_from_mem
    LDR    R10, [sp, #0x40]        //Get valid_row_off(0x2c)
    CMP    R11, R10                //Check bank row end
    BLT    exit_clear_col_loop1_cp_from_mem
    //Clear padding-left cols
    MOV    R4,  #0x0
    LDR    R7,  [sp, #0x30]        //Get start_col(R7)
    MOV    R8,  R7
    MOV    R10, #0x0
    clear_col_loop2_cp_from_mem
    CMP    R8,  #0x0
    BGE    exit_clear_col_loop2_cp_from_mem
    STR    R4,  [R1, R10, LSL #2]
    ADD    R8,  #0x1
    ADD    R10, #0x1
    B      clear_col_loop2_cp_from_mem
    exit_clear_col_loop2_cp_from_mem       //Clear padding-left cols
    //Copy valid cols
    LDR    R12, [sp, #0x3c]       //Get valid_col_off(R12)
    ADD    R0,  R12, LSL #1
    ADD    R1,  R12, LSL #2
    MOV    R10, R3
    ANDS   R8,  R0, #3
    BEQ    dword4_col_loop_cp_from_mem
    LDRH   R4,  [R0,  R2, LSL #1] //Read half word at odd row at sdram
    LDRH   R5,  [R0], #2          //Read half word at even row at sdram
    PKHBT  R4,  R4, R5, LSL #16   //Bank Col 0, lower 16bits of R4 and upper 16bits of R5
    STR    R4,  [R1], #4          //Write dword into bank addr, then increase addr
    SUB    R10, #0x1              //Have procssed one col
    dword4_col_loop_cp_from_mem
    SUBS   R10, #0x4
    BLT    exit_dword4_col_loop_cp_from_mem
    ADD    R7,  R0, R2,  LSL #1
    LDRD   R8, R9, [R7]  //Get odd row (mem_addr + 2*col_num)
    LDMIA  R0!, {R6 - R7}        //Get even row (mem_addr)
    PKHBT  R4,  R8, R6, LSL #16  //Bank Col 0, Lower 16bits of R8 and upper 16bits of R6
    PKHTB  R5,  R6, R8, ASR #16  //Bank Col 1, Lower 16bits of R6 and upper 16bits of R8
    PKHBT  R6,  R9, R7, LSL #16  //Bank Col 2, Lower 16bits of R9 and upper 16bits of R7
    PKHTB  R7,  R7, R9, ASR #16  //Bank Col 3, Lower 16bits of R7 and upper 16bits of R9
    STMIA  R1!, {R4 - R7}        //Store bank rows
    B      dword4_col_loop_cp_from_mem
    exit_dword4_col_loop_cp_from_mem
    ADD    R10, #0x4
    dword1_col_loop_cp_from_mem
    SUBS   R10, #0x1
    BLT    exit_dword1_col_loop_cp_from_mem
    LDRH   R4,  [R0,  R2, LSL #1] //Read half word at odd row at sdram
    LDRH   R5,  [R0], #2          //Store half word at even row at sdram
    PKHBT  R4,  R4, R5, LSL #16   //Bank Col 0, lower 16bits of R4 and upper 16bits of R5
    STR    R4,  [R1], #4          //Write dword into bank addr, then increase addr
    B      dword1_col_loop_cp_from_mem
    exit_dword1_col_loop_cp_from_mem
    SUB    R0,  R3, LSL #1       //Mem pointer go back to valid_col_off (- len_col)
    SUB    R1,  R3, LSL #2       //Bank pointer go back to valid_col_off (- len_col)
    //Clear padding-right cols(col_num - start_col)
    LDR    R8,  [sp, #0x5c]      //Get padding_right_start_col(R7)
    MOV    R4,  #0
    clear_col_loop3_cp_from_mem
    CMP    R8,  R3                //Check col_end
    BGE    exit_clear_col_loop3_cp_from_mem
    STR    R4,  [R1, R8, LSL #2]
    ADD    R8,  #0x1
    B      clear_col_loop3_cp_from_mem
    exit_clear_col_loop3_cp_from_mem      //Clear padding-right cols(col_num - start_col)
    B      exit_clear_col_loop1_cp_from_mem
    //Clear padding-bottom rows R9:row R8: col
    clear_bottom_cp_from_mem
    MOV    R7,  R1
    SUB    R6,  R10, R11
    MOV    R4,  #0x0
    MOV    R8,  #0x0
    clear_col_loop1_cp_from_mem
    SUBS   R5,  R3,  R8
    BLE    exit_clear_col_loop1_cp_from_mem
    STRH   R4,  [R7, #2]
    CMP    R6,  #0x2
    STRHGE R4,  [R7]
    ADD    R8,  #0x2
    ADD    R7,  #0x4
    B      clear_col_loop1_cp_from_mem
    exit_clear_col_loop1_cp_from_mem  //Clear padding-bottom rows R9:row R8: col
    ADD    R0,  R0, R2,  LSL #2   //Mem pointer increase 2*col_num
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R3,  LSL #2   //Bank pointer add one col (+ len_col)
    ADD    R11, #0x2              //Add row loop counter (2 rows)
    B      row_loop_cp_from_mem
    exit_row_loop_cp_from_mem
    ADD    R14, #1
    LDR    R1,  [sp, #0x54]        //Get  bank_addr of last channel
    LDR    R4,  [sp, #0x58]        //Get  dst_bank_channel_off
    ADD    R4,  R14
    ANDS   R4,  R4, #1             //Check Odd channel or even channel
    BNE    oddch_addrinc_copy_from_mem
    SUB    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x4c]        //Increase bank addr for even channel
    ADD    R1,  R4
    oddch_addrinc_copy_from_mem
    ADDNE  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x48]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x50]        //Get  mem_addr of last channel
    ADD    R0,  R4
    B      channel_loop_cp_from_mem
    exit_copy_from_mem
    POP     {R4 - R12, LR}
    BX      lr
}

/*
 *R0: mem_addr   Memory destination address
 *R1: bank_addr  Bank source address
 *R2: col_num    Total column number in sdram
 *R3: len_col    Column number in bank
 *R4-R7:         Data to Bank(4 cols, 2 rows)
 *R8-R9:         Temp Data
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from 0
 *R12: ch_size   row_num*col_num
 *valid_len_col(sp+0x28) valid_len_row(sp+0x2c) start_col(sp+0x30) start_row(sp+0x34) row_num(sp+0x38) valid_col_off(sp+0x3c)
 *valid_row_off(sp+0x40)
 */
__asm void *AsmCopyFromMem1x1(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t start_col, int32_t start_row, int32_t row_num, int32_t valid_col_off, int32_t valid_row_off, int32_t len_channel, int32_t mem_ch_size, int32_t bank_ch_size, int32_t last_mem_addr, int32_t last_bank_addr, int32_t dst_channel_off, int32_t padding_right_start)
{
    PUSH   {R4 - R12, LR}

    LDR    R14, [sp, #0x44]       //Get len_channel
    channel_loop_cp_from_mem1x1
    CMP    R14, #0
    BLT.W  exit_copy_from_mem1x1
    STRD   R0,  R1,  [sp, #0x50]   //Save mem_addr and bank addr

    LDR    R9,  [sp, #0x34]        //Get  start_row
    //Clear padding-up rows R9:row R8: col
    clear_row_loop_cp_from_mem1x1
    CMP    R9,  #0x0
    BGE    exit_clear_loop_cp_from_mem1x1
    MOV    R4,  #0x0
    MOV    R8,  #0x0
    MOV    R7,  R1
    clear_col_loop_cp_from_mem1x1
    SUBS   R5,  R3,  R8
    BLE    exit_clear_col_loop_cp_from_mem1x1
    STR    R4,  [R7], #0x4
    STR    R4,  [R7], #0x4        //Clear 2 rows and 2 col
    ADD    R8,  #0x1
    B      clear_col_loop_cp_from_mem1x1
    exit_clear_col_loop_cp_from_mem1x1
    ADD    R9,  #0x1
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R3,  LSL #3   //Bank pointer add one col (+ 2*len_col)
    B      clear_row_loop_cp_from_mem1x1
    exit_clear_loop_cp_from_mem1x1         //Clear padding-up rows R9:row R8: col

    MOV    R11,  #0x0             //Row loop counter start from 0
    row_loop_cp_from_mem1x1
    LDR    R10, [sp, #0x2c]       //Get valid_len_row(0x2c)
    CMP    R11, R10               //Check bank row end
    BGE.W  exit_row_loop_cp_from_mem1x1
    LDR    R9,  [sp, #0x34]        //Get start_row(0x34)
    ADD    R8,  R11, R9            //row + start_row
    LDR    R7,  [sp, #0x38]        //Get row_num
    CMP    R8,  R7                 //Check mem row end
    BGE.W  clear_bottom_cp_from_mem1x1
    LDR    R10, [sp, #0x40]        //Get valid_row_off(0x2c)
    CMP    R11, R10                //Check bank row end
    BLT.W  exit_clear_col_loop1_cp_from_mem1x1
    //Clear padding-left cols
    MOV    R4,  #0x0
    LDR    R7,  [sp, #0x30]        //Get start_col(R7)
    LSL    R8,  R7, #1             //Clear 2 rows and 2 col
    MOV    R10, #0x0
    clear_col_loop2_cp_from_mem1x1
    CMP    R8,  #0x0
    BGE    exit_clear_col_loop2_cp_from_mem1x1
    STR    R4,  [R1, R10, LSL #2]
    ADD    R8,  #0x1
    ADD    R10, #0x1
    B      clear_col_loop2_cp_from_mem1x1
    exit_clear_col_loop2_cp_from_mem1x1    //Clear padding-left cols
    //Copy valid cols
    LDR    R12, [sp, #0x3c]       //Get valid_col_off(R12)
    ADD    R0,  R12, LSL #1
    ADD    R1,  R12, LSL #3
    MOV    R10, R3
    MOV    R7,  R0
    LDR    R12,  [sp, #0x38]       //Get row_num
    MUL    R12,  R2                //ch_size=row_num*col_num
    ANDS   R8,  R0, #3
    BEQ    dword2_col_loop_cp_from_mem1x1
    LDRH   R4,  [R7]              //Read half word at sdram on channel 0
    LDRH   R5,  [R7, R12, LSL #1] //Read half word at sdram on channel 1
    ADD    R7,  R12, LSL #1
    LDRH   R8,  [R7, R12, LSL #1] //Read half word at sdram on channel 2
    ADD    R7,  R12, LSL #1
    LDRH   R9,  [R7, R12, LSL #1] //Read half word at sdram on channel 3
    PKHBT  R4,  R8, R4, LSL #16   //Bank Col 0, lower 16bits of R8 and upper 16bits of R4
    PKHBT  R5,  R9, R5, LSL #16   //Bank Col 1, lower 16bits of R9 and upper 16bits of R5
    STMIA  R1!, {R4 - R5}         //Write dword into bank addr, then increase addr
    SUB    R10, #0x1              //Have procssed one col
    ADD    R0,  #0x2
    dword2_col_loop_cp_from_mem1x1
    SUBS   R10, #0x2
    BLT    exit_dword2_col_loop_cp_from_mem1x1
    MOV    R7,  R0
    LDR    R4,  [R7]              //Read word at sdram on channel 0
    LDR    R5,  [R7, R12, LSL #1] //Read word at sdram on channel 1
    ADD    R7,  R12, LSL #1
    LDR    R8,  [R7, R12, LSL #1] //Read word at sdram on channel 2
    ADD    R7,  R12, LSL #1
    LDR    R9,  [R7, R12, LSL #1] //Read word at sdram on channel 3
    PKHBT  R6,  R8, R4, LSL #16   //Bank Col 0, Lower 16bits of R8 and lower 16bits of R4
    PKHBT  R7,  R9, R5, LSL #16   //Bank Col 1, Lower 16bits of R9 and lower 16bits of R5
    STMIA  R1!, {R6 - R7}         //Store bank rows
    PKHTB  R6,  R4, R8, ASR #16    //Bank Col 2, Upper 16bits of R4 and upper 16bits of R8
    PKHTB  R7,  R5, R9, ASR #16   //Bank Col 3, Upper 16bits of R5 and upper 16bits of R9
    STMIA  R1!, {R6 - R7}         //Store bank rows
    ADD    R0,  #0x4
    B      dword2_col_loop_cp_from_mem1x1
    exit_dword2_col_loop_cp_from_mem1x1
    ADD    R10, #0x2
    dword1_col_loop_cp_from_mem1x1
    SUBS   R10, #0x1
    BLT    exit_dword1_col_loop_cp_from_mem1x1
    MOV    R7,  R0
    LDRH   R4,  [R7]               //Read half word at sdram on channel 0
    LDRH   R5,  [R7, R12, LSL #1]  //Read half word at sdram on channel 1
    ADD    R7,  R12, LSL #1
    LDRH   R8,  [R7, R12, LSL #1]  //Read half word at sdram on channel 2
    ADD    R7,  R12, LSL #1
    LDRH   R9,  [R7, R12, LSL #1]  //Read half word at sdram on channel 3
    PKHBT  R4,  R8, R4, LSL #16    //Bank Col 0, lower 16bits of R8 and upper 16bits of R4
    PKHBT  R5,  R9, R5, LSL #16    //Bank Col 1, lower 16bits of R9 and upper 16bits of R5
    STMIA  R1!, {R4 - R5}          //Store bank rows
    SUB    R10, #0x1               //Have procssed one col
    ADD    R0,  #0x2
    B      dword1_col_loop_cp_from_mem1x1
    exit_dword1_col_loop_cp_from_mem1x1
    SUB    R0,  R3, LSL #1         //Mem pointer go back to valid_col_off (- len_col)
    SUB    R1,  R3, LSL #3         //Bank pointer go back to valid_col_off (- len_col)//Clear padding-right cols(col_num - start_col)
    //Clear padding-right cols(col_num - start_col)
    LDR    R8,  [sp, #0x5c]      //Get padding_right_start_col(R8)
    MOV    R4,  #0
    //LDR    R7,  [sp, #0x30]        //Get start_col(R7)
    //SUB    R8,  R2, R7
    ADD    R5,  R1, R8, LSL #3
    clear_col_loop3_cp_from_mem1x1
    CMP    R8,  R3                //Check col_end
    BGE    exit_clear_col_loop3_cp_from_mem1x1
    STR    R4,  [R5], #4
    STR    R4,  [R5], #4
    ADD    R8,  #0x1
    B      clear_col_loop3_cp_from_mem1x1 //Clear padding-right cols(col_num - start_col)
    exit_clear_col_loop3_cp_from_mem1x1
    B      exit_clear_col_loop1_cp_from_mem1x1
    //Clear padding-bottom rows R9:row R8: col
    clear_bottom_cp_from_mem1x1
    MOV    R7,  R1
    MOV    R4,  #0x0
    MOV    R8,  #0x0
    clear_col_loop1_cp_from_mem1x1
    SUBS   R5,  R3,  R8
    BLE    exit_clear_col_loop1_cp_from_mem1x1
    STR    R4,  [R7], #4
    STR    R4,  [R7], #4
    ADD    R8,  #0x1
    B      clear_col_loop1_cp_from_mem1x1
    exit_clear_col_loop1_cp_from_mem1x1  //Clear padding-bottom rows R9:row R8: col
    ADD    R0,  R0, R2,  LSL #1   //Mem pointer increase 1*col_num
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R3,  LSL #3   //Bank pointer add one col (+ 2*len_col)
    ADD    R11, #0x1              //Add row loop counter (Actually 2 rows)
    B      row_loop_cp_from_mem1x1
    exit_row_loop_cp_from_mem1x1
    SUBS   R14, #4                //channel -= 4
    BLT    exit_copy_from_mem1x1
    LDR    R1,  [sp, #0x54]        //Get  bank_addr of last channel
    LSR    R4,  R14, #2            //channel /= 4
    ANDS   R4,  R4,  #1            //Check Odd channel or even channel
    BEQ    oddch_addrinc_copy_from_mem1x1
    ADD    R1,  #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x4c]        //Increase bank addr for even channel
    SUB    R1,  R4
    oddch_addrinc_copy_from_mem1x1
    SUBEQ  R1,  #0x10000           //Increase bank addr for odd channel
    LDR    R4,  [sp, #0x48]        //Get  mem_ch_size
    LDR    R0,  [sp, #0x50]        //Get  mem_addr of last channel
    SUB    R0,  R4                 //Decrease mem address
    B      channel_loop_cp_from_mem1x1
    exit_copy_from_mem1x1
    POP     {R4 - R12, LR}
    BX      lr
}

/*
 *R0: bank_addr  Bank source address
 *R1: dst_bank_addr  Bank source address
 *R2: len_row    Row number in bank
 *R3: len_col    Column number in bank
 *R4-R7:         Data from Bank(4 cols, 2 rows)
 *R8-R9:         Temp Data
 *R10: col       col loop counter, start from valid_len_col
 *R11: row       row loop counter, start from valid_len_row
 */
__asm void AsmCopyToBank1x1(uint8_t *bank_addr, uint8_t *dst_bank_addr, int32_t len_row, int32_t len_col, int32_t len_row_bak, int32_t channel, int32_t last_bank_addr, int32_t last_dst_bank_addr)
{
    PUSH   {R4 - R12, LR}
    MOV    R12,  R2               //R12 = len_col * ((len_row + 7)/8)
    ADD    R12,  #0x7
    LSR    R12,  #0x3
    MUL    R12,  R3

    LDR    R14, [sp, #0x2c]       //Get ((len_channel - 1)/4)*4
    channel_loop_copy_to_bank1x1
    LDR    R2,  [sp, #0x28]       //Get len_row_bak
    CMP    R14, #0
    BLT    exit_copy_to_bank1x1
    STRD   R0,  R1,  [sp, #0x30]   //Save bank_addr and dst_bank addr

    row_loop_cp_to_bank1x1
    SUBS   R2,  #2
    BLT    exit_row_loop_cp_to_bank1x1
    MOV    R10, R3
    dword4_col_loop_cp_to_bank1x1
    SUBS   R10, #0x1
    BLT    exit_dword4_col_loop_cp_to_bank1x1
    ADD    R7,  R0, #0x10000       //Next channel
    ADD    R11, R1, #0x4000        //Next bank
    LDR    R8,  [R0, R12, LSL #2]  //Read word at sdram on channel 2
    LDR    R9,  [R7, R12, LSL #2]  //Read word at sdram on channel 3
    LDR    R4,  [R0], #0x4         //Read word at sdram on channel 0, then increase addr
    LDR    R5,  [R7], #0x4         //Read word at sdram on channel 1, then increase addr
    PKHTB  R6,  R4, R8, ASR #16    //Bank Col 0, Upper 16bits of R4 and upper 16bits of R8
    PKHTB  R7,  R5, R9, ASR #16    //Bank Col 1, Upper 16bits of R5 and upper 16bits of R9
    STMIA  R1!, {R6 - R7}          //Store bank rows
    PKHBT  R6,  R8, R4, LSL #16    //Bank Col 2, Lower 16bits of R8 and lower 16bits of R4
    PKHBT  R7,  R9, R5, LSL #16    //Bank Col 3, Lower 16bits of R9 and lower 16bits of R5
    STMIA  R11!, {R6 - R7}         //Store bank rows
    B      dword4_col_loop_cp_to_bank1x1
    exit_dword4_col_loop_cp_to_bank1x1
    SUB    R0,  R3, LSL #2         //Bank pointer go back to valid_col_off (- len_col)
    SUB    R1,  R3, LSL #3         //Bank pointer go back to valid_col_off (- 2*len_col)
    ADD    R0,  R0, #0x4000       //Increase One Bank
    ANDS   R5,  R0, #0xC000
    SUBEQ  R0,  #0x10000          //Go back to Bank 0
    ADDEQ  R0,  R0, R3,  LSL #2   //Bank pointer add one col (+ 2*len_col)
    ADD    R1,  R1, #0x8000       //Increase Two Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R3,  LSL #3   //Bank pointer add one col (+ 4*len_col)
    B      row_loop_cp_to_bank1x1
    exit_row_loop_cp_to_bank1x1
    SUB    R14, #4                 //channel -= 4
    LDR    R0,  [sp, #0x30]        //Get last_bank_addr
    SUB    R0,  R0, R12, LSL #3    //Decrease bank address

    LDR    R1,  [sp, #0x34]        //Get last_dst_bank_addr
    LSR    R4,  R14, #2            //channel/4
    ANDS   R4,  R4, #1             //Check Odd channel or even channel
    BEQ    oddch_addrinc_copy_to_bank1x1
    ADD    R1,  #0x10000           //Increase bank addr for even channel
    SUB    R1,  R1, R12, LSL #4
    oddch_addrinc_copy_to_bank1x1
    SUBEQ  R1,  R1,  #0x10000     //Increase bank addr for odd channel
    B      channel_loop_copy_to_bank1x1
    exit_copy_to_bank1x1
    POP     {R4 - R12, LR}
    BX      lr
}

/*
 *R0: bank_addr  Bank source address
 *R1: dst_bank_addr  Bank source address
 *R2: len_row    Row number in bank
 *R3: len_col    Column number in bank
 *R4-R7:         Data from Bank(4 cols, 2 rows)
 *R8:  col       col loop counter
 *R9: op
 */
__asm void AsmCopyChannel(uint8_t *bank_addr, uint8_t *dst_bank_addr, int32_t len_row, int32_t len_col, int op, int32_t len_channel, int32_t bank_ch_size, int32_t src_channl_off, int32_t dst_channel_off)
{
    PUSH   {R4 - R12, LR}
    LDR    R9,  [sp, #0x28]  //Get op
    MOV    R14, #0
    MOV    R12, R2
    channel_loop_copy_channel
    LDR    R4, [sp, #0x2C]  //Get len_channel
    CMP    R14, R4
    BGE    exit_copy_channel
    MOV    R11, R0          //Save last src channel addr to R11
    MOV    R10, R1          //Save last dst channel addr to R12
    MOV    R2,  R12
    row_loop_copy_channel
    CMP    R2, #0x1
    BLT    exit_row_loop_copy_channel
    CMP    R9, #0x1
    BGE    exchange_channel
    MOV    R8, R3
    dword4_col_loop_copy_channel
    SUBS   R8, #0x4
    BLT    exit_dword4_col_loop_copy_channel
    LDMIA  R0!, {R4 - R7}
    STMIA  R1!, {R4 - R7}        //Store even row
    B      dword4_col_loop_copy_channel
    exit_dword4_col_loop_copy_channel
    ADD    R8, #0x4
    dword1_col_loop_copy_channel
    SUBS   R8, #0x1
    BLT    exit_exchange_channel
    LDR    R4,  [R0], #4          //Read dword from bank addr, then increase addr
    STR    R4,  [R1], #4          //Store half word at odd row to sdram
    B      dword1_col_loop_copy_channel
    exchange_channel
    CMP    R9, #0x2
    BGE    exit_exchange_channel
    MOV    R8, R3
    dword1_col_loop_exchange_channel
    SUBS   R8, #0x1
    BLT    exit_exchange_channel
    LDR    R4,  [R0]              //Read dword from bank addr
    LDR    R5,  [R1]              //Read dword from bank addr
    STR    R4,  [R1], #4          //Store dword to bank addr, then increase addr
    STR    R5,  [R0], #4          //Store dword to bank addr, then increase addr
    B      dword1_col_loop_exchange_channel
    exit_exchange_channel
    SUB    R0,  R0, R3, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    SUB    R1,  R1, R3, LSL #2   //Bank pointer go back to valid_col_off (- valid_len_col)
    ADD    R0,  R0, #0x4000       //Increase One Bank
    ANDS   R5,  R0, #0xC000
    SUBEQ  R0,  #0x10000          //Go back to Bank 0
    ADDEQ  R0,  R0, R3,  LSL #2   //Bank pointer add one col (+ len_col)
    ADD    R1,  R1, #0x4000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R3,  LSL #2   //Bank pointer add one col (+ len_col)
    SUBS   R2,  #0x2              //Decrease 2 rows
    B      row_loop_copy_channel
    exit_row_loop_copy_channel
    ADD    R14, #1
    LDR    R0,  [sp, #0x34]        //Get src_bank_channel_off
    ADD    R4,  R0,  R14
    ANDS   R4,  R4, #1             //Check Odd channel or even channel
    BNE    oddch_addrinc_copy_channel
    SUB    R11, #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x30]        //Get bank_ch_size
    ADD    R0,  R11, R4
    oddch_addrinc_copy_channel
    ADDNE  R0,  R11,  #0x10000     //Increase bank addr for odd channel
    LDR    R1,  [sp, #0x38]        //Get dst_bank_channel_off
    ADD    R4,  R1,  R14           //Get dst_channel
    ANDS   R4,  R4, #1             //Check Odd channel or even channel
    BNE    oddch_addrinc_copy_channel1
    SUB    R10, #0x10000           //Increase bank addr for even channel
    LDR    R4,  [sp, #0x30]        //Get bank_ch_size
    ADD    R1,  R10, R4
    oddch_addrinc_copy_channel1
    ADDNE  R1,  R10,  #0x10000     //Increase bank addr for odd channel
    B      channel_loop_copy_channel
    exit_copy_channel
    POP     {R4 - R12, LR}
    BX      lr
}


/*
 *R0: bank_addr  Bank source address
 *R1: dst_bank_addr  Bank source address
 *R2: len_row    Row number in bank
 *R3: len_col    Column number in bank
 *R4-R5:         Tmp Data
 *R6-R7:         Tmp dst bank addr
 *R8: col        col loop counter
 *R9: off        offset at one channel of extended dst bank
 */
__asm void AsmCopyChannel1x1(uint8_t *bank_addr, uint8_t *dst_bank_addr, int32_t len_row, int32_t len_col, int32_t off)
{
    PUSH   {R4 - R9, LR}
    LDR    R9, [sp, #0x1C]       //Get  offset at one channel
    row_loop_copy_channel1x1
    CMP    R2, #0x1
    BLT    exit_copy_channel1x1
    ADD    R6, R1, R9, LSL #1
    ADD    R7, R6, #0x4000
    MOV    R8, R3
    dword1_col_loop_copy_channel1x1
    SUBS   R8, #0x1
    BLT    exit_col_loop_copy_channel1x1
    LDR    R4,  [R0], #4          //Read dword from bank addr, then increase addr
    ASR    R5,  R4, #16
    STRH   R5,  [R6], #8          //Store half word at even row to extended bank
    STRH   R4,  [R7], #8          //Store half word at odd row to extended bank
    B      dword1_col_loop_copy_channel1x1
    exit_col_loop_copy_channel1x1
    SUB    R0,  R0, R3, LSL #2    //Bank pointer go back to valid_col_off (-len_col)
    ADD    R0,  R0, #0x4000       //Increase One Bank
    ANDS   R5,  R0, #0xC000
    SUBEQ  R0,  #0x10000          //Go back to Bank 0
    ADDEQ  R0,  R0, R3,  LSL #2   //Bank pointer add one col (+ len_col)
    ADD    R1,  R1, #0x8000       //Increase One Bank
    ANDS   R5,  R1, #0xC000
    SUBEQ  R1,  #0x10000          //Go back to Bank 0
    ADDEQ  R1,  R1, R3,  LSL #3   //Bank pointer add one col (+ 2*len_col)
    SUBS   R2,  #0x2              //Decrease 2 rows
    B      row_loop_copy_channel1x1
    exit_copy_channel1x1
    POP     {R4 - R9, LR}
    BX      lr
}
#endif

void FastCopyToMem(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t valid_row_off)
{
    int32_t row, col;

    for (row = 0; row < valid_len_row;)
    {
        if (row >= valid_row_off)
        {
            /* Even row */
            for (col = 0; col < valid_len_col; ++col)
            {
                mem_addr[col] = ((int16_t *)bank_addr)[2 * col + 1];
            }
            if (++row >= valid_len_row)
            {
                break;
            }
            /* Odd row */
            for (col = 0; col < valid_len_col; ++col)
            {
                mem_addr[col_num + col] = ((int16_t *)bank_addr)[2 * col];
            }
            if (++row >= valid_len_row)
            {
                break;
            }
        }
        /* Increase 2 rows to pointer to mem buffer */
        mem_addr += 2 * col_num;
        /* Inrease 2 rows to pointer to bank */
        if ((row & 0x7) == 0)
        {
            bank_addr += 0x4000;
            bank_addr -= 0x10000;
            bank_addr += 4 * len_col;
        }
        else
        {
            bank_addr += 0x4000;
        }
    };

}


void FastCopyFromMem(int16_t *mem_addr, uint8_t *bank_addr, int32_t col_num, int32_t len_col, int32_t valid_len_col, int32_t valid_len_row, int32_t start_col, int32_t start_row, int32_t row_num, int32_t valid_col_off, int32_t valid_row_off, int32_t padding_right_start)
{
    int32_t row, col;

    for (row = start_row; row < 0;)
    {
        for (col = 0; col < len_col; ++col)
        {
            ((int16_t *)bank_addr)[2 * col + 1] = 0;
        }
        row++;
        if (row < 0)
        {
            for (col = 0; col < len_col; ++col)
            {
                ((int16_t *)bank_addr)[2 * col] = 0;
            }
        }
        row++;
        if ((row & 0x7) == 0)
        {
            bank_addr += 0x4000;
            bank_addr -= 0x10000;
            bank_addr += 4 * len_col;
        }
        else
        {
            bank_addr += 0x4000;
        }
    }
    for (row = 0; row < valid_len_row;)
    {
        if (row + start_row >= row_num)
        {
            for (col = 0; col < len_col; ++col)
            {
                ((int16_t *)bank_addr)[2 * col + 1] = 0;
            }
        }
        else if (row >= valid_row_off && row < valid_len_row)
        {
            /* Even row */
            /* Padding left col */
            for (col = 0; col < -start_col; ++col)
            {
                ((int16_t *)bank_addr)[2 * col + 1] = 0;
            }
            /* Copy valid col */
            for (col = valid_col_off; col < len_col; ++col)
            {
                ((int16_t *)bank_addr)[2 * col + 1] = mem_addr[col];
            }
            /* Padding right col */
            for (col = padding_right_start; col < len_col; ++col)
            {
                ((int16_t *)bank_addr)[2 * col + 1] = 0;
            }
        }
        row++;

        if (row + start_row >= row_num)
        {
            for (col = 0; col < len_col; ++col)
            {
                ((int16_t *)bank_addr)[2 * col] = 0;
            }
        }
        else if (row >= valid_row_off && row < valid_len_row)
        {
            /* Odd row */
            /* Padding left col */
            for (col = 0; col < -start_col; ++col)
            {
                ((int16_t *)bank_addr)[2 * col] = 0;
            }
            /* Copy valid col */
            for (col = valid_col_off; col < len_col; ++col)
            {
                ((int16_t *)bank_addr)[2 * col] = mem_addr[col_num + col];
            }
            /* Padding right col */
            for (col = padding_right_start; col < len_col; ++col)
            {
                ((int16_t *)bank_addr)[2 * col] = 0;
            }
        }
        row++;

        /* Increase 2 rows to pointer to mem buffer */
        mem_addr += 2 * col_num;
        /* Inrease 2 rows to pointer to bank */
        if ((row & 0x7) == 0)
        {
            bank_addr += 0x4000;
            bank_addr -= 0x10000;
            bank_addr += 4 * len_col;
        }
        else
        {
            bank_addr += 0x4000;
        }
    };
}


static void FirstPutData(struct easynet_ops_param *param, struct conv_hw_context *cxt, unsigned char *first_imginput)
{
    struct op_cross_datafetch_param *op_param = (struct op_cross_datafetch_param *)(((unsigned char *)param) + sizeof(struct easynet_ops_param));
    uint32_t row, col, channel, row_idx, ch_idx, bank_row_idx, bank_idx;
    uint32_t start_row, len_row, start_col, len_col, end_col, col_num, row_num, ch_num,  row_off, ch_off;
    uint8_t *input = (uint8_t *)first_imginput;
    uint32_t bank_start_addr, bank_addr;

    start_row = op_param->start_row;
    len_row   = op_param->len_row;
    start_col = op_param->start_col;
    len_col   = op_param->len_col;
    col_num   = op_param->col;
    ch_num    = op_param->ch;
    row_num   = op_param->row;

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);

    if (op_param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }

    for (row_idx = 0; row_idx < 2; ++row_idx)
    {

        if (row_idx == 1) /* Odd row , lower half */
        {
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }
        else if (row_idx == 0)  /* Even row , higher half */
        {
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);;
        }

        for (ch_idx = 0; ch_idx < 2 ; ++ch_idx)
        {
            for (bank_row_idx = row_idx; bank_row_idx < 8; bank_row_idx += 2)
            {
                bank_idx = (bank_row_idx / 2) % 4;
                bank_addr = bank_start_addr + ch_idx * 0x10000 + bank_idx * 0x4000;

                for (channel = ch_idx; channel < ch_num; channel += 2)
                {
                    ch_off = col_num * row_num * channel;
                    /* Be careful that there is no padding rows in PutData, so you have to start
                             *  from zero , otherwise bank_adder is nor correct
                             */
                    for (row = 0; row < len_row; row += 8)
                    {
                        row_off = ch_off + (row + bank_row_idx + start_row) * col_num;
                        end_col = start_col + len_col;

                        if (channel == 3 || ((row + bank_row_idx + start_row) >= row_num))
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                HW32_REG(bank_addr) = 0;
                            }
                        }
                        else
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                HW32_REG(bank_addr) = input[col + row_off];
                            }

                            for (; end_col > col_num; end_col--)
                            {
                                ((int16_t *)(bank_addr - 4 * (end_col - col_num)))[0] = 0;
                            }

                        }
                    }
                }
            }
        }
    }
    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
}

static void CrossPutData(struct easynet_ops_param *param, struct conv_hw_context *cxt, unsigned char *first_imginput)
{
    struct op_cross_datafetch_param *op_param = (struct op_cross_datafetch_param *)(((unsigned char *)param) + sizeof(struct easynet_ops_param));
    uint32_t row, col, channel, row_idx, ch_idx, bank_row_idx, bank_idx;
    uint32_t start_row, len_row, start_col, len_col, end_col, col_num, row_num, ch_num, buffer, row_off, ch_off;
    int16_t *input;
    uint32_t bank_start_addr, bank_addr;

    if (op_param->layer_num == 0)
    {
        FirstPutData(param, cxt, first_imginput);
        return;
    }

    HAL_CNN_Bram_hfwd_rw_en(ENABLE);
    start_row = op_param->start_row;
    len_row   = op_param->len_row;
    start_col = op_param->start_col;
    len_col   = op_param->len_col;
    col_num   = op_param->col;
    ch_num    = op_param->ch;
    row_num   = op_param->row;
    buffer    = op_param->buffer;

    input = cxt->layer_buffer[buffer];
    if (op_param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }



    for (row_idx = 0; row_idx < 2; ++row_idx)
    {
        if (row_idx == 1) /* Odd row , lower half */
        {
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        }
        else if (row_idx == 0)  /* Even row , higher half */
        {
            HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);;
        }

        for (ch_idx = 0; ch_idx < 2 ; ++ch_idx)
        {
            for (bank_row_idx = row_idx; bank_row_idx < 8; bank_row_idx += 2)
            {
                bank_idx = (bank_row_idx / 2) % 4;
                bank_addr = bank_start_addr + ch_idx * 0x10000 + bank_idx * 0x4000;

                for (channel = ch_idx; channel < ch_num; channel += 2)
                {
                    ch_off = col_num * row_num * channel;
                    /* Be careful that there is no padding rows in PutData, so you have to start
                     *  from zero , otherwise bank_adder is nor correct
                     */
                    for (row = 0; row < len_row; row += 8)
                    {
                        row_off = ch_off + (row + bank_row_idx + start_row) * col_num;
                        end_col = start_col + len_col;
                        if ((row + bank_row_idx + start_row) < row_num)
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                HW32_REG(bank_addr) = input[col + row_off];
                            }

                            for (; end_col > col_num; end_col--)
                            {
                                ((int16_t *)(bank_addr - 4 * (end_col - col_num)))[0] = 0;
                            }
                        }
                        else
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                HW32_REG(bank_addr) = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    HAL_CNN_Bram_hfwd_rw_en(DISABLE);
}

static void CrossGetData(struct easynet_ops_param *param, struct conv_hw_context *cxt)
{
    struct op_cross_writeout_param *op_param = (struct op_cross_writeout_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    uint32_t start_row, len_row, valid_len_row, valid_start_col, start_col, len_col, end_col, out_col, out_ch, out_row, buffer, start_channel, len_channel;
    uint32_t ch_off, row_off, row_idx, bank_row_idx, ch_idx, bank_idx, channel, row, col;
    int16_t *output;
    void *bank_start_addr, *bank_addr;

    start_row = op_param->start_row;
    len_row   = op_param->len_row;
    valid_len_row = op_param->valid_len_row;
    valid_start_col = op_param->valid_start_col;
    start_col = op_param->start_col;
    len_col   = op_param->len_col;
    out_col = op_param->out_col;
    out_ch  = op_param->out_ch;
    out_row = op_param->out_row;
    buffer = op_param->buffer;
    start_channel = op_param->start_channel;
    len_channel = op_param->len_channel;

    output = cxt->layer_buffer[buffer];
    if (op_param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }
    /*
     * Take data from two banks, and each bank contains channelNum / 2 data
     */
    for (row_idx = 0; row_idx < 2; ++row_idx)
    {
        for (ch_idx = 0; ch_idx < 2 ; ++ch_idx)
        {
            for (bank_row_idx = row_idx; bank_row_idx < 8; bank_row_idx += 2)
            {
                bank_idx = (bank_row_idx / 2) % 4;
                bank_addr = bank_start_addr + ch_idx * 0x10000 + bank_idx * 0x4000;
                if (row_idx == 0)
                {
                    bank_addr += 2;
                }
                for (channel = ch_idx; channel < len_channel; channel += 2)
                {
                    ch_off = out_col * out_row * (channel + start_channel);
                    for (row = bank_row_idx; row < len_row; row += 8)
                    {
                        row_off = ch_off + (row + start_row) * out_col;
                        end_col = start_col + len_col;
                        if (start_col + len_col > out_col)
                        {
                            end_col = out_col;
                        }
                        if (row < valid_len_row)
                        {
                            for (col = start_col; col < valid_start_col; ++col, bank_addr += 4);
                            //if (op_param->layer_num == 0 && channel == 0 && row + start_row == 5)
                            //if (op_param->layer_num == 4 && 2*(col + row_off) == 0x4750)
                            //{
                            //    printf("pp");
                            //}
                            for (col = valid_start_col; col < end_col; ++col, bank_addr += 4)
                            {
                                output[col + row_off] = ((int16_t *)bank_addr)[0];
                                //if (bank_addr == 0x6769b6)
                                //{
                                //  printf("pp");
                                //}
                            }
                            if (start_col + len_col > out_col)
                            {
                                bank_addr += 4 * (start_col + len_col - out_col);
                            }

                        }
                        else
                        {
                            for (col = start_col; col < end_col; ++col, bank_addr += 4);
                            if (start_col + len_col > out_col)
                            {
                                bank_addr += 4 * (start_col + len_col - out_col);
                            }
                        }
                    }
                }
            }
        }
    }

    //if (op_param->layer_num == 0 && op_param->start_col + op_param->len_col >= op_param->out_col)
    //if (op_param->layer_num == 0  && op_param->start_row + op_param->valid_len_row >= op_param->out_row && op_param->start_col + op_param->len_col >= op_param->out_col)
    //if (op_param->layer_num == 8 && op_param->start_col > 0)
    //{
    //  save_layer_bin("./examples/compiler/posenet/layer.bin", output, op_param->out_col*op_param->out_row*op_param->out_ch*2);
    //}
    //if (op_param->layer_num == 8 && op_param->start_row + op_param->valid_len_row >= op_param->out_row && op_param->start_col + op_param->len_col >= op_param->out_col)
    //{
    //result_cmp(output, cxt->layer_buffer[3], op_param->out_col*op_param->out_row*op_param->out_ch);
    //}
}
void kdp_print_bank_op(int code)
{
#ifdef DEBUG_KDP_ALL
    char *opers[] = {"BANK_CP_FROM_MEM", "BANK_CP_TO_MEM", "BANK_CP_TO_BANK", "BANK_ADD_FROM_MEM",
                     "BANK_ADD_TO_MEM", "BANK_PADDING", "BANK_MAXPOOL", "BANK_ADD_BOTH",
                     "BANK_COPY_CHANNEL", "BANK_EXCHANGE_CHANNEL", "BANK_DEMAP_CHANNEL", "BANK_CP_FROM_MEM_WITHBN",
                     "BANK_ACTIVATION_CHANNEL", "BANK_BATCHNORM_CHANNEL",
                    };
    if (code < sizeof(opers) / sizeof(char *))
    {
        printf("(%d)%s\n", code, opers[code]);
    }
    else
    {
        printf("(%d)%s\n", code, "UNKOWN");
    }
#endif
}
/* General bank copy operation, it shall replace all implementation before */
static void do_bank_op(struct bank_op_param *param, struct conv_hw_context *cxt)
{
    unsigned char *bank_addr, *bank_start_addr;
    int16_t val, *mem_addr;
    int32_t row_num, col_num, channel, row, col, mem_off, real_col_num;
    int32_t start_row, valid_len_row, valid_len_col, len_row, start_col, len_col, start_channel, len_channel, valid_row_off, valid_col_off;
    int32_t  activation = param->activation;

    int mem_ch_size;
    int bank_ch_size;

    row_num       = param->row_num;
    real_col_num  = param->col_num;
#ifdef FORCE_COL_NUM_EVEN
    col_num       = ((param->col_num + 1) / 2) * 2;
#else
    col_num       =  real_col_num;
#endif
    len_channel   = param->len_channel;
    len_row       = param->len_row;
    valid_len_row = param->valid_len_row;
    len_col       = param->len_col;
    start_channel = param->start_channel;
    start_row     = param->start_row;
    start_col     = param->start_col;
    valid_row_off = param->valid_start_row - param->start_row;
    valid_col_off = param->valid_start_col - param->start_col;
    valid_len_col = len_col;

    mem_addr      = cxt->layer_buffer[param->buffer];
    if (param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }

    /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
    switch (param->op)
    {
        case BANK_CP_FROM_MEM:
#ifdef ARMC4_OPT
            bank_addr  = bank_start_addr + ((param->dst_channel_off) & 0x1) * 0x10000;
            bank_addr += (param->dst_channel_off / 2) * len_col * ((len_row + 7) / 8) * 4;
            mem_off = col_num * (start_row + row_num * start_channel) + start_col;
            bank_ch_size = len_col * ((len_row + 7) / 8) * 4;
            mem_ch_size  = row_num * col_num * 2;
            AsmCopyFromMem(&mem_addr[mem_off], bank_addr, col_num, len_col, valid_len_col, valid_len_row, start_col, start_row, row_num, valid_col_off, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0, param->dst_channel_off, real_col_num - start_col);
#else
            for (channel = 0; channel < len_channel; channel++)
            {
                bank_addr  = bank_start_addr + ((param->dst_channel_off + channel) & 0x1) * 0x10000;
                bank_addr += ((param->dst_channel_off + channel) / 2) * len_col * ((len_row + 7) / 8) * 4;
                mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;
                FastCopyFromMem(&mem_addr[mem_off], bank_addr, col_num, len_col, valid_len_col, valid_len_row, start_col, start_row, row_num, valid_col_off, valid_row_off, real_col_num - start_col);
            }

#endif
            break;
        case BANK_CP_TO_MEM:
        {
#ifdef ARMC4_OPT
            bank_addr  = bank_start_addr;
            mem_off = col_num * (start_row + row_num * start_channel) + start_col;
            bank_ch_size = len_col * ((len_row + 7) / 8) * 4;
            mem_ch_size  = row_num * col_num * 2;
            valid_len_col = (len_col <= (col_num - start_col)) ? len_col : (col_num - start_col);
            AsmCopyToMem(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, len_col, valid_len_col, valid_len_row, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0);
#else
            for (channel = 0; channel < len_channel; channel++)
            {
                bank_addr  = bank_start_addr + (channel & 0x1) * 0x10000;
                bank_addr += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;
                mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;
                valid_len_col = (len_col <= (col_num - start_col)) ? len_col : (col_num - start_col);
                FastCopyToMem(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, len_col, valid_len_col, valid_len_row, valid_row_off);
            }
#endif
            break;
        }
        case BANK_ADD_TO_MEM: /* Used for input channel cut */
        {
            int16_t *dst_mem_addr;
            int32_t dst_mem_off;
#ifdef ARMC4_OPT
            bank_addr  = bank_start_addr;
            mem_off = col_num * (start_row + row_num * start_channel) + start_col;
            bank_ch_size = len_col * ((len_row + 7) / 8) * 4;
            mem_ch_size  = row_num * col_num * 2;
            /* Never write beyond valid range */
            valid_len_col = (len_col <= (col_num - start_col)) ? len_col : (col_num - start_col);
            dst_mem_addr  = cxt->layer_buffer[param->dst_buffer];
            dst_mem_off = col_num * (start_row + row_num * start_channel) + start_col;

            if (activation == RELU)
            {
                AsmAddToMemRelu(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, &dst_mem_addr[dst_mem_off + valid_col_off], len_col, valid_len_col, valid_len_row, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0, 0);
            }
            else if (activation == LEAKY)
            {
                AsmAddToMemLeaky(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, &dst_mem_addr[dst_mem_off + valid_col_off], len_col, valid_len_col, valid_len_row, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0, 0);
            }
            else
            {
                AsmAddToMem(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, &dst_mem_addr[dst_mem_off + valid_col_off], len_col, valid_len_col, valid_len_row, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0, 0);
            }
#else
            for (channel = 0; channel < len_channel; channel++)
            {
                bank_addr  = bank_start_addr + (channel & 0x1) * 0x10000;
                bank_addr += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;
                mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;
                /* Never write beyond valid range */
                valid_len_col = (len_col <= (col_num - start_col)) ? len_col : (col_num - start_col);
                dst_mem_addr  = cxt->layer_buffer[param->dst_buffer];
                dst_mem_off   = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

                for (row = 0; row < valid_len_row;)
                {
                    if (row >= valid_row_off)
                    {
                        /* Even row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                dst_mem_addr[dst_mem_off + col]  = simu_relu_activate(val);
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                dst_mem_addr[dst_mem_off + col]  = simu_leaky_activate(val, 13);
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                dst_mem_addr[dst_mem_off + col] = val;
                            }
                        }
                    }
                    row++;
                    if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                dst_mem_addr[dst_mem_off + col_num + col]  = simu_relu_activate(val);
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                dst_mem_addr[dst_mem_off + col_num + col]  = simu_leaky_activate(val, 13);
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                dst_mem_addr[dst_mem_off + col_num + col] = val;
                            }
                        }

                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    dst_mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
            }
#endif
            break;
        }
        case BANK_ADD_FROM_MEM: /* Used for shortcut operation */
#ifdef ARMC4_OPT
            bank_addr  = bank_start_addr;
            mem_off = col_num * (start_row + row_num * start_channel) + start_col;
            bank_ch_size = len_col * ((len_row + 7) / 8) * 4;
            mem_ch_size  = row_num * col_num * 2;

            if (activation == RELU)
            {
                AsmAddToBankRelu(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, NULL, len_col, valid_len_col, valid_len_row, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0);
            }
            else if (activation == LEAKY)
            {
                AsmAddToBankLeaky(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, NULL, len_col, valid_len_col, valid_len_row, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0);
            }
            else
            {
                AsmAddToBank(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, NULL, len_col, valid_len_col, valid_len_row, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0);
            }
#else
            for (channel = 0; channel < len_channel; channel++)
            {
                bank_addr  = bank_start_addr + (channel & 0x1) * 0x10000;
                bank_addr += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;
                mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

                /* TODO: Saturation add is necessary */
                for (row = 0; row < valid_len_row;)
                {
                    if (row >= valid_row_off)
                    {
                        /* Even row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                val = simu_relu_activate(val);
                                ((int16_t *)bank_addr)[2 * col + 1]  = val;
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                ((int16_t *)bank_addr)[2 * col + 1]  = simu_leaky_activate(val, 13);
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                ((int16_t *)bank_addr)[2 * col + 1] += mem_addr[mem_off + col];
                            }
                        }
                        for (col = real_col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    row++;
                    if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                ((int16_t *)bank_addr)[2 * col]  = simu_relu_activate(val);
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                ((int16_t *)bank_addr)[2 * col]  = simu_leaky_activate(val, 13);
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < len_col; ++col)
                            {
                                ((int16_t *)bank_addr)[2 * col] += mem_addr[mem_off + col_num + col];
                            }
                        }
                        for (col = real_col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
            }
#endif
            break;
        case BANK_ADD_BOTH: /* Used for shortcut operation */
        {
            int16_t *dst_mem_addr;
            int32_t dst_mem_off;

#ifdef ARMC4_OPT
            bank_addr  = bank_start_addr;
            mem_off = col_num * (start_row + row_num * start_channel) + start_col;
            bank_ch_size = len_col * ((len_row + 7) / 8) * 4;
            mem_ch_size  = row_num * col_num * 2;
            valid_len_col = (len_col <= (col_num - start_col)) ? len_col : (col_num - start_col);
            dst_mem_addr   = cxt->layer_buffer[param->dst_buffer];
            dst_mem_off = col_num * (start_row + row_num * start_channel) + start_col;

            if (activation == RELU)
            {
                AsmAddToBothRelu(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, &dst_mem_addr[dst_mem_off + valid_col_off], len_col, valid_len_col, valid_len_row, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0, 0);
            }
            else if (activation == LEAKY)
            {
                AsmAddToBothLeaky(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, &dst_mem_addr[dst_mem_off + valid_col_off], len_col, valid_len_col, valid_len_row, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0, 0);
            }
            else
            {
                AsmAddToBoth(&mem_addr[mem_off + valid_col_off], (bank_addr + 4 * valid_col_off), col_num, &dst_mem_addr[dst_mem_off + valid_col_off], len_col, valid_len_col, valid_len_row, valid_row_off, len_channel, mem_ch_size, bank_ch_size, 0, 0, 0);
            }
#else
            for (channel = 0; channel < len_channel; channel++)
            {
                bank_addr  = bank_start_addr + (channel & 0x1) * 0x10000;
                bank_addr += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;
                mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;
                valid_len_col = (len_col <= (col_num - start_col)) ? len_col : (col_num - start_col);
                dst_mem_addr   = cxt->layer_buffer[param->dst_buffer];
                dst_mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

                /* TODO: Saturation add is necessary */
                for (row = 0; row < valid_len_row;)
                {
                    if (row >= valid_row_off)
                    {
                        /* Even row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                val = simu_relu_activate(val);
                                ((int16_t *)bank_addr)[2 * col + 1]  = dst_mem_addr[dst_mem_off + col] = val;
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col] + ((int16_t *)bank_addr)[2 * col + 1];
                                val = simu_leaky_activate(val, 13);
                                ((int16_t *)bank_addr)[2 * col + 1]  = dst_mem_addr[dst_mem_off + col] = val;
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = ((int16_t *)bank_addr)[2 * col + 1] + mem_addr[mem_off + col];
                                ((int16_t *)bank_addr)[2 * col + 1]  = dst_mem_addr[dst_mem_off + col] = val;
                            }
                        }
                        for (col = real_col_num - start_col; col < valid_len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    row++;
                    if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        if (activation == RELU)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                val = simu_relu_activate(val);
                                ((int16_t *)bank_addr)[2 * col]  = dst_mem_addr[dst_mem_off + col_num + col] = val;
                            }
                        }
                        else if (activation == LEAKY)
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = mem_addr[mem_off + col_num + col] + ((int16_t *)bank_addr)[2 * col];
                                val = simu_leaky_activate(val, 13);
                                ((int16_t *)bank_addr)[2 * col]  = dst_mem_addr[dst_mem_off + col_num + col] = val;
                            }
                        }
                        else
                        {
                            for (col = valid_col_off; col < valid_len_col; ++col)
                            {
                                val = ((int16_t *)bank_addr)[2 * col] + mem_addr[mem_off + col_num + col];
                                ((int16_t *)bank_addr)[2 * col]  = dst_mem_addr[dst_mem_off + col_num + col] = val;
                            }
                        }
                        for (col = real_col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    dst_mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
            }
#endif
            break;
        }
        case BANK_PADDING:
            for (channel = 0; channel < len_channel; channel++)
            {
                bank_addr  = bank_start_addr + (channel & 0x1) * 0x10000;
                bank_addr += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;
                mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

                for (row = start_row; row < 0;)
                {
                    for (col = 0; col < len_col; ++col)
                    {
                        ((int16_t *)bank_addr)[2 * col + 1] = 0;
                    }
                    row++;
                    if (row < 0)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;
                }
                for (row = 0; row < valid_len_row;)
                {
                    if (row + start_row > row_num)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    else if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Even row */
                        /* Padding left col */
                        for (col = 0; col < -start_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                        /* Padding right col */
                        for (col = real_col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    row++;

                    if (row + start_row > row_num)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    else if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        /* Padding left col */
                        for (col = 0; col < -start_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                        /* Padding right col */
                        for (col = real_col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
            }
            break;
        case BANK_CP_FROM_MEM_WITHBN:
        {
            for (channel = 0; channel < len_channel; channel++)
            {
                bank_addr  = bank_start_addr + ((param->dst_channel_off + channel) & 0x1) * 0x10000;
                bank_addr += ((param->dst_channel_off + channel) / 2) * len_col * ((len_row + 7) / 8) * 4;
                mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

                uint32_t *bn = (uint32_t *)(cxt->weight_start_addr + param->weight_offset);
                int16_t gamma = (int16_t)(bn[channel] & 0xFFFF);
                int16_t beta  = (int16_t)((bn[channel] >> 16) & 0xFFFF);;

                for (row = start_row; row < 0;)
                {
                    for (col = 0; col < len_col; ++col)
                    {
                        ((int16_t *)bank_addr)[2 * col + 1] = 0;
                    }
                    row++;
                    if (row < 0)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;
                }
                for (row = 0; row < valid_len_row;)
                {
                    if (row + start_row > row_num)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    else if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Even row */
                        /* Padding left col */
                        for (col = 0; col < -start_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                        /* Copy valid col */
                        for (col = valid_col_off; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = fix32_round_truncate(mem_addr[mem_off + col] * gamma) + beta;
                        }
                        /* Padding right col */
                        for (col = real_col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col + 1] = 0;
                        }
                    }
                    row++;

                    if (row + start_row > row_num)
                    {
                        for (col = 0; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    else if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Odd row */
                        /* Padding left col */
                        for (col = 0; col < -start_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                        /* Copy valid col */
                        for (col = valid_col_off; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = fix32_round_truncate(mem_addr[mem_off + col_num + col] * gamma) + beta;
                        }
                        /* Padding right col */
                        for (col = real_col_num - start_col; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[2 * col] = 0;
                        }
                    }
                    row++;

                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
            }
            break;
        }
        case BANK_ACTIVATION_CHANNEL:
        {
            for (channel = 0; channel < len_channel; channel++)
            {
                bank_addr  = bank_start_addr + (channel & 0x1) * 0x10000;
                bank_addr += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;
                mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

                int16_t *relu_num_addr = (int16_t *)(cxt->weight_start_addr + param->weight_offset);
                int16_t relu_num = relu_num_addr[param->start_channel + channel];
                uint32_t uint_val_in, uint_val_out;

                for (row = 0; row < len_row;)
                {
                    for (col = 0; col < len_col; ++col)
                    {
                        uint_val_in = ((uint32_t *)bank_addr)[col];

                        val = simu_leaky_activate(((int16_t)(uint_val_in & 0xffff)), relu_num);
                        uint_val_out = ((uint32_t)val) & 0xFFFF;
                        val = simu_leaky_activate(((int16_t)((uint_val_in >> 16) & 0xffff)), relu_num);
                        uint_val_out += ((((uint32_t)val) & 0xFFFF) << 16);
                        ((uint32_t *)bank_addr)[col] = uint_val_out;
                    }
                    row += 2;

                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
            }
            break;
        }
        case BANK_BATCHNORM_CHANNEL:
        {
            for (channel = 0; channel < len_channel; channel++)
            {
                bank_addr  = bank_start_addr + (channel & 0x1) * 0x10000;
                bank_addr += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;
                mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

                uint32_t *bn = (uint32_t *)(cxt->weight_start_addr + param->weight_offset);
                int16_t gamma = (int16_t)(bn[channel] & 0xFFFF);
                int16_t beta  = (int16_t)((bn[channel] >> 16) & 0xFFFF);
                uint32_t uint_val_in, uint_val_out;

                for (row = 0; row < len_row;)
                {
                    for (col = 0; col < len_col; ++col)
                    {
                        uint_val_in = ((uint32_t *)bank_addr)[col];

                        val = fix32_round_truncate(((int16_t)(uint_val_in & 0xffff)) * gamma) + beta;
                        uint_val_out = ((uint32_t)val) & 0xFFFF;
                        val = fix32_round_truncate(((int16_t)((uint_val_in >> 16) & 0xffff)) * gamma) + beta;
                        uint_val_out += ((((uint32_t)val) & 0xFFFF) << 16);
                        ((uint32_t *)bank_addr)[col] = uint_val_out;
                    }
                    row += 2;

                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
            }
            break;
        }
        default:
            break;
    }


    //if (param->op == BANK_CP_TO_MEM && param->layer_num == 77 && param->start_channel + param->len_channel >= cxt->total_out_ch && param->start_row + param->valid_len_row >= param->row_num && param->start_col + param->len_col >= param->col_num)
    //if (param->op == BANK_CP_TO_MEM && param->layer_num == 51 && param->start_channel == 0 && param->start_row + param->valid_len_row >= param->row_num && param->start_col + param->len_col >= param->col_num)
    //{
    //  printf("debug\n");
    //}
}

static void do_channel_cp(struct bank_op_param *param, struct conv_hw_context *cxt)
{
    unsigned char *bank_addr, *bank_start_addr, *dst_bank_addr;
    int32_t row_num, col_num, channel, row, col;
    int32_t len_row, len_col, len_channel;
    uint32_t val;

    row_num       = param->row_num;
    col_num       = param->col_num;
    len_channel   = param->len_channel;
    len_row       = param->len_row;
    len_col       = param->len_col;

    if (param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }

#ifdef ARMC4_OPT
    bank_addr  = bank_start_addr + ((param->src_channel_off) & 0x1) * 0x10000;
    bank_addr += ((param->src_channel_off) / 2) * len_col * ((len_row + 7) / 8) * 4;

    dst_bank_addr  = bank_start_addr + ((param->dst_channel_off) & 0x1) * 0x10000;
    dst_bank_addr += ((param->dst_channel_off) / 2) * len_col * ((len_row + 7) / 8) * 4;

    AsmCopyChannel(bank_addr, dst_bank_addr, len_row, len_col, ((param->op == BANK_EXCHANGE_CHANNEL) ? 1 : 0), len_channel, len_col * ((len_row + 7) / 8) * 4, param->src_channel_off, param->dst_channel_off);
#else
    for (channel = 0; channel < len_channel; channel++)
    {
        /* Set bank pointer to channel start of bank */
        bank_addr  = bank_start_addr + ((channel + param->src_channel_off) & 0x1) * 0x10000;
        bank_addr += ((channel + param->src_channel_off) / 2) * len_col * ((len_row + 7) / 8) * 4;

        dst_bank_addr  = bank_start_addr + ((channel + param->dst_channel_off) & 0x1) * 0x10000;
        dst_bank_addr += ((channel + param->dst_channel_off) / 2) * len_col * ((len_row + 7) / 8) * 4;

        /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
        switch (param->op)
        {
            case BANK_COPY_CHANNEL:
                for (row = 0; row < len_row;)
                {
                    for (col = 0; col < len_col; ++col)
                    {
                        ((uint32_t *)dst_bank_addr)[col] = ((uint32_t *)bank_addr)[col];
                    }

                    row += 2;

                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                        dst_bank_addr += 0x4000;
                        dst_bank_addr -= 0x10000;
                        dst_bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                        dst_bank_addr += 0x4000;
                    }
                };
                break;
            case BANK_EXCHANGE_CHANNEL:
                for (row = 0; row < len_row;)
                {
                    for (col = 0; col < len_col; ++col)
                    {
                        val = ((uint32_t *)dst_bank_addr)[col];
                        ((uint32_t *)dst_bank_addr)[col] = ((uint32_t *)bank_addr)[col];
                        ((uint32_t *)bank_addr)[col] = val;
                    }

                    row += 2;

                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                        dst_bank_addr += 0x4000;
                        dst_bank_addr -= 0x10000;
                        dst_bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                        dst_bank_addr += 0x4000;
                    }
                };
                break;
            default:
                break;
        }
    }
#endif
}

static void do_bank_op_1x1(struct bank_op_param *param, struct conv_hw_context *cxt)
{
    void *bank_addr, *bank_start_addr, *dst_bank_addr;
    int16_t val, *mem_addr;
    int32_t row_num, col_num, channel, row, col, mem_off, ch_size, real_col_num;
    int32_t start_row, valid_len_row, valid_len_col, len_row, start_col, len_col, start_channel, len_channel, valid_row_off, valid_col_off;
    int32_t  activation = param->activation;

    row_num       = param->row_num;
    real_col_num  = param->col_num;
#ifdef FORCE_COL_NUM_EVEN
    col_num       = ((param->col_num + 1) / 2) * 2;
#else
    col_num       = real_col_num;
#endif
    len_channel   = param->len_channel;
    len_row       = param->len_row;
    valid_len_row = param->valid_len_row;
    len_col       = param->len_col;
    start_channel = param->start_channel;
    start_row     = param->start_row;
    start_col     = param->start_col;
    valid_row_off = param->valid_start_row - param->start_row;
    valid_col_off = param->valid_start_col - param->start_col;

    mem_addr      = cxt->layer_buffer[param->buffer];

    if (param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }

    /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
    switch (param->op)
    {
        case BANK_CP_FROM_MEM:
#ifdef ASMC4_OPT
            channel = (((len_channel - 1) / 4) * 4);
            bank_addr  = bank_start_addr + ((channel / 4 + 2) & 0x1) * 0x10000;
            bank_addr += ((channel / 4 + 2) / 2) * len_col * ((len_row + 7) / 8) * 4 * 4;
            mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;
            bank_ch_size = len_col * ((len_row + 7) / 8) * 4 * 4;
            mem_ch_size  = row_num * col_num * 2 * 4;
            AsmCopyFromMem1x1(&mem_addr[mem_off], bank_addr, col_num, len_col, valid_len_col, valid_len_row, start_col, start_row, row_num, valid_col_off, valid_row_off, (((len_channel - 1) / 4) * 4), mem_ch_size, bank_ch_size, 0, 0, 0, real_col_num - start_col);
#else
            for (channel = (((len_channel - 1) / 4) * 4); channel >= 0; channel -= 4)
            {
                /* Set bank pointer to channel start of bank , first 4 channels are all zero */
                bank_addr  = bank_start_addr + ((channel / 4 + 2) & 0x1) * 0x10000;
                bank_addr += ((channel / 4 + 2) / 2) * len_col * ((len_row + 7) / 8) * 4 * 4;
                /* Set row pointer to channel start of shorcut buffer */
                mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

                ch_size  = col_num * row_num;
                for (row = start_row; row < 0;)
                {
                    /* 2 rows and 2 cols */
                    for (col = 0; col < 2 * len_col; ++col)
                    {
                        ((uint32_t *)bank_addr)[col] = 0;
                    }
                    row++;
                }
                for (row = 0; row < valid_len_row;)
                {
                    if (row + start_row > row_num)
                    {
                        for (col = 0; col < 2 * len_col; ++col)
                        {
                            ((uint32_t *)bank_addr)[col] = 0;
                        }
                    }
                    else if (row >= valid_row_off && row < valid_len_row)
                    {
                        /* Even row */
                        /* Padding left col */
                        for (col = 0; col < -2 * start_col; ++col)
                        {
                            ((uint32_t *)bank_addr)[col] = 0;
                        }
                        /* Copy valid col */
                        for (col = valid_col_off; col < len_col; ++col)
                        {
                            ((int16_t *)bank_addr)[4 * col + 1] = mem_addr[mem_off + 0 * ch_size + col];
                            ((int16_t *)bank_addr)[4 * col + 3] = mem_addr[mem_off + 1 * ch_size + col];
                            ((int16_t *)bank_addr)[4 * col + 0] = mem_addr[mem_off + 2 * ch_size + col];
                            ((int16_t *)bank_addr)[4 * col + 2] = mem_addr[mem_off + 3 * ch_size + col];

                        }
                        /* Padding right col */
                        for (col = 2 * (real_col_num - start_col); col < 2 * len_col; ++col)
                        {
                            ((uint32_t *)bank_addr)[col] = 0;
                        }
                    }
                    row++;
                    /* Increase 2 rows to pointer to mem buffer */
                    mem_off += col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if (((2 * row) & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 8 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                };
            }
#endif
            break;
        case BANK_CP_TO_BANK:
#ifdef ASMC4_OPT
            channel = (((len_channel - 1) / 4) * 4);


            dst_bank_addr  = (param->bank == 0) ? cxt->bank_a_addr : cxt->bank_c_addr;
            dst_bank_addr += ((channel / 4 + 2) & 0x1) * 0x10000;
            dst_bank_addr += ((channel / 4 + 2) / 2) * len_col * ((len_row + 7) / 8) * 4 * 4;
            bank_addr      = (param->bank == 0) ? cxt->bank_a_addr : cxt->bank_c_addr;
            bank_addr     += (channel & 0x1) * 0x10000;
            bank_addr     += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;
            AsmCopyToBank1x1(bank_addr, dst_bank_addr, len_row, len_col, len_row, (((len_channel - 1) / 4) * 4), 0, 0);
#else
            for (channel = (((len_channel - 1) / 4) * 4); channel >= 0; channel -= 4)
            {
                uint32_t val0, val2, val4, val6;

                ch_size    = len_col * ((len_row + 7) / 8);
                /* Set bank pointer to channel start of bank */
                /* first 8 channels are all zero */
                dst_bank_addr  = (param->bank == 0) ? cxt->bank_a_addr : cxt->bank_c_addr;
                dst_bank_addr += ((channel / 4 + 2) & 0x1) * 0x10000;
                dst_bank_addr += ((channel / 4 + 2) / 2) * len_col * ((len_row + 7) / 8) * 4 * 4;

                bank_addr      = (param->bank == 0) ? cxt->bank_a_addr : cxt->bank_c_addr;
                bank_addr     += (channel & 0x1) * 0x10000;
                bank_addr     += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;

                /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
                for (row = 0; row < len_row;)
                {
                    for (col = 0; col < len_col; col++)
                    {
                        /* Get data from 2 rows, 1 col, 4 channels */
                        val0 = ((uint32_t *)bank_addr)[col + 0 * ch_size];
                        val2 = ((uint32_t *)(bank_addr + 0x10000))[col + 0 * ch_size];
                        val4 = ((uint32_t *)bank_addr)[col + 1 * ch_size];
                        val6 = ((uint32_t *)(bank_addr + 0x10000))[col + 1 * ch_size];

                        /* Copy data to  4 rows, 2 cols of 1 channel */
                        /* Dst row 0/1 and col0/1 are from src row 0 and col 0 of 4 channels (upper 16 bits of val0/2/4/6) */
                        ((uint32_t *)dst_bank_addr)[2 * col + 0] = (val0 & 0xFFFF0000) | (val4 >> 16);
                        ((uint32_t *)dst_bank_addr)[2 * col + 1] = (val2 & 0xFFFF0000) | (val6 >> 16);
                        /* Dst row 2/3 and col0/1 are from src row 1 and col 0 of 4 channels (lower 16 bits of val0/2/4/6)*/
                        ((uint32_t *)(dst_bank_addr + 0x4000))[2 * col + 0] = ((val0 & 0xFFFF) << 16) | (val4 & 0xFFFF);
                        ((uint32_t *)(dst_bank_addr + 0x4000))[2 * col + 1] = ((val2 & 0xFFFF) << 16) | (val6 & 0xFFFF);
                    }
                    row += 2;

                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }

                    /* Check if next row return to old bank */
                    if (((2 * row) & 0x7) == 0)
                    {
                        dst_bank_addr += 0x8000;
                        dst_bank_addr -= 0x10000;
                        dst_bank_addr += 8 * len_col;
                    }
                    else
                    {
                        dst_bank_addr += 0x8000;
                    }
                }
            }
#endif
            break;
        default:
            break;
    }
}

static void do_channel_cp_1x1(struct bank_op_param *param, struct conv_hw_context *cxt, int32_t len_chanel, int32_t channel_off)
{
    unsigned char *bank_addr, *bank_start_addr, *dst_bank_addr;
    int32_t row, col;
    int32_t len_row, len_col, dst_off, ch_off, col_off, row_off;
    uint32_t val;

    len_row       = param->len_row;
    len_col       = param->len_col;

    if (param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }


    /* Set bank pointer to channel start of bank */
    bank_addr = bank_start_addr;

    /* Get row off and col off in one 3x3 block (combine data from every 9 channels) */
    /* len_channel shall be multiple of 9, channel_off starts from final channel */
    dst_off   = channel_off / 4;
    ch_off    = (channel_off) % 4;
    col_off   = ch_off % 2;
    row_off   = ch_off / 2;

    dst_bank_addr  = (param->bank == 0) ? cxt->bank_a_addr : cxt->bank_c_addr;
    dst_bank_addr += ((dst_off + 2) & 0x1) * 0x10000;
    dst_bank_addr += ((dst_off + 2) / 2) * len_col * ((len_row + 7) / 8) * 4 * 4;

#ifdef ARMC4_OPT
    AsmCopyChannel1x1(bank_addr, dst_bank_addr, len_row, len_col, 2 * col_off + ((row_off == 1) ? 0 : 1));
#else
    /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
    for (row = 0; row < len_row;)
    {
        for (col = 0; col < len_col; col++)
        {
            val = ((uint32_t *)bank_addr)[col];
            ((uint16_t *)dst_bank_addr)[4 * col + 2 * col_off + ((row_off == 1) ? 0 : 1)] = (val >> 16);
            ((uint16_t *)(dst_bank_addr + 0x4000))[4 * col + 2 * col_off + ((row_off == 1) ? 0 : 1)] = (val & 0xFFFF);
        }
        row += 2;

        /* Increase 2 rows to pointer to bank */
        if ((row & 0x7) == 0)
        {
            bank_addr += 0x4000;
            bank_addr -= 0x10000;
            bank_addr += 4 * len_col;
        }
        else
        {
            bank_addr += 0x4000;
        }

        /* Check if next row return to old bank */
        if (((2 * row) & 0x7) == 0)
        {
            dst_bank_addr += 0x8000;
            dst_bank_addr -= 0x10000;
            dst_bank_addr += 8 * len_col;
        }
        else
        {
            dst_bank_addr += 0x8000;
        }
    }
#endif
}

static void do_channel_demap(struct bank_op_param *out_param, struct bank_op_param *in_param, struct conv_hw_context *cxt, int len_channel, int conv_stride)
{
    void *bank_start_addr, *dst_bank_addr;
    int16_t *mem_addr;
    int32_t channel, row, col, out_col, i, mem_off, mem_col_num;
    int32_t len_row, valid_len_row, len_col;
    uint32_t val;

#ifdef FORCE_COL_NUM_EVEN
    mem_col_num = (((out_param->col_num + 1) / 2) * 2);
#else
    mem_col_num = out_param->col_num;
#endif
    len_row       = in_param->len_row;
    valid_len_row = in_param->valid_len_row;
    len_col       = in_param->len_col;

    if (in_param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }

    for (channel = 0; channel < len_channel; channel++)
    {
        /* Set bank pointer to channel start of bank */
        dst_bank_addr  = bank_start_addr + ((channel + in_param->dst_channel_off) & 0x1) * 0x10000;
        dst_bank_addr += ((channel + in_param->dst_channel_off) / 2) * len_col * ((len_row + 7) / 8) * 4;


        mem_addr = cxt->layer_buffer[out_param->buffer];
        mem_off  = mem_col_num * out_param->row_num * (channel + out_param->src_channel_off);

        for (row = 0; row < len_row;)
        {
            if ((row & 0x1) == 0 && row < valid_len_row)
            {
                /* Even row */
                /* Map input col */
                for (col = 0, out_col = 0; col < len_col; col += conv_stride, out_col++)
                {
                    ((int16_t *)dst_bank_addr)[2 * col + 1] = mem_addr[mem_off + out_col];
                }
            }

            if ((row & 0x1) == 1 && row < valid_len_row)
            {
                /* Odd row */
                /* Copy valid col */
                for (col = 0, out_col = 0; col < len_col; col += conv_stride, out_col++)
                {
                    ((int16_t *)dst_bank_addr)[2 * col] = mem_addr[mem_off + out_col];
                }
            }

            /* Increase one row to pointer to mem buffer */
            mem_off += mem_col_num;

            for (i = 0; i < conv_stride; i++)
            {
                if (((++row) & 0x1) == 0)
                {
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        dst_bank_addr += 0x4000;
                        dst_bank_addr -= 0x10000;
                        dst_bank_addr += 4 * len_col;
                    }
                    else
                    {
                        dst_bank_addr += 0x4000;
                    }
                }
            };
        }
    }

}

static void do_channel_demap_compress_1x1(struct bank_op_param *out_param, struct bank_op_param *in_param, struct conv_hw_context *cxt, int len_channel, int conv_stride)
{
    void *bank_start_addr, *dst_bank_addr;
    int16_t *mem_addr;
    int32_t channel, row, col, out_col, i, mem_off, mem_col_num;
    int32_t len_row, valid_len_row, len_col;
    uint32_t dst_channel_off;

#ifdef FORCE_COL_NUM_EVEN
    mem_col_num = (((out_param->col_num + 1) / 2) * 2);
#else
    mem_col_num = out_param->col_num;
#endif
    len_row       = in_param->len_row;
    valid_len_row = in_param->valid_len_row;
    len_col       = in_param->len_col;

    if (in_param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }

    len_row *= 2;
    len_col *= 2;
    valid_len_row *= 2;
    dst_channel_off = in_param->dst_channel_off;
    dst_channel_off = (dst_channel_off + 3) / 4;
    dst_channel_off = ((dst_channel_off + 1) / 2) * 2;
    dst_channel_off += 2;
    conv_stride = 2;

    for (channel = 0; channel < len_channel; channel++)
    {
        /* Set bank pointer to channel start of bank */
        dst_bank_addr  = bank_start_addr + ((channel + dst_channel_off) & 0x1) * 0x10000;
        dst_bank_addr += ((channel + dst_channel_off) / 2) * len_col * ((len_row + 7) / 8) * 4;


        mem_addr = cxt->layer_buffer[out_param->buffer];
        mem_off  = mem_col_num * out_param->row_num * (channel + out_param->src_channel_off);

        for (row = 0; row < len_row;)
        {
            for (col = 0, out_col = 0; col < len_col; col += conv_stride, out_col++)
            {
                ((int16_t *)dst_bank_addr)[2 * col + 1] = mem_addr[mem_off + out_col];
            }

            /* Increase one row to pointer to mem buffer */
            mem_off += mem_col_num;

            for (i = 0; i < conv_stride; i++)
            {
                if (((++row) & 0x1) == 0)
                {
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        dst_bank_addr += 0x4000;
                        dst_bank_addr -= 0x10000;
                        dst_bank_addr += 4 * len_col;
                    }
                    else
                    {
                        dst_bank_addr += 0x4000;
                    }
                }
            };
        }
    }

}

static void do_bank_maxpool(struct bank_op_param *param, struct conv_hw_context *cxt)
{
    void *bank_addr, *bank_start_addr, *dst_bank_addr;
    //int32_t mem_off;
    //int16_t val, *mem_addr;
    int32_t row_num, col_num, channel, row, col;
    int32_t start_row, valid_len_row, valid_len_col, len_row, start_col, len_col, start_channel, len_channel, valid_row_off, valid_col_off;
    int32_t  activation = param->activation;

    row_num       = param->row_num;
    col_num       = param->col_num;
    len_channel   = param->len_channel;
    len_row       = param->len_row;
    valid_len_row = param->valid_len_row;
    len_col       = param->len_col;
    start_channel = param->start_channel;
    start_row     = param->start_row;
    start_col     = param->start_col;
    valid_row_off = param->valid_start_row - param->start_row;
    valid_col_off = param->valid_start_col - param->start_col;

    valid_len_col = len_col;
    if ((start_col + len_col) >  col_num)
    {
        valid_len_col = col_num -  start_col;
    }




    //mem_addr      = cxt->layer_buffer[param->buffer];
    if (param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }

    for (channel = 0; channel < len_channel; channel++)
    {
        /* Set bank pointer to channel start of bank */
        bank_addr  = bank_start_addr + (channel & 0x1) * 0x10000;
        bank_addr += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;
        /* Set row pointer to channel start of shorcut buffer */
        //mem_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

        /* Process all bank data of one channel, you can optimize following code by assembly language on platform */
        /* Used for maxpool operation on bank */
        int32_t pool_stride = param->pool_stride;
        int32_t pool_size   = param->pool_size;
        int32_t pool_pad_left = param->pool_pad_left;
        int32_t pool_pad_up  = param->pool_pad_up;
        int32_t out_col, pool_off, col_pool_off, out_row, i, rowaddr_idx;
        int16_t val, max;
        int32_t len_out_col = param->pool_out_col;
        int32_t len_out_row = param->pool_out_row;
        void   *bank_ch_addr;
        void   *row_bank_addr[4];

        /* Output and input share same memory, it is correct for most of cases, except such as pool_pad_left = 1, pool_size = 2, pool_stride = 1 */
        /* The input can be reused again if out channel is too big, so I can't dump maxpool result back to input bank */
        bank_ch_addr = bank_addr;
        dst_bank_addr = bank_start_addr + (channel & 0x1) * 0x10000;
        dst_bank_addr += (channel / 2) * len_col * ((len_row + 7) / 8) * 4;

        /* Going through all input rows and save results */
        for (row = 0; row < valid_len_row;)
        {
            /* Even row */
            for (col = -pool_pad_left, out_col = 0; out_col < len_out_col; col += pool_stride, out_col++)
            {
                /* Do nothing for all right padding */
                pool_off = pool_size - 1;
                if (col + pool_off >= valid_len_col)
                {
                    pool_off = valid_len_col - col - 1;
                }
                max = -32768;

                for (; (col + pool_off) >= 0 && pool_off >= 0; pool_off--)
                {
                    val = ((int16_t *)bank_ch_addr)[2 * (col + pool_off) + 1];
                    if (val > max)
                    {
                        max = val;
                    }
                }
                ((int16_t *)dst_bank_addr)[2 * out_col + 1] = max;
            }
            row++;

            /* Odd row */
            for (col = -pool_pad_left, out_col = 0; out_col < len_out_col; col += pool_stride, out_col++)
            {
                /* Do nothing for all right padding */
                pool_off = pool_size - 1;
                if (col + pool_off >= valid_len_col)
                {
                    pool_off = valid_len_col - col - 1;
                }
                max = -32768;

                for (; (col + pool_off) >= 0 && pool_off >= 0; pool_off--)
                {
                    val = ((int16_t *)bank_ch_addr)[2 * (col + pool_off)];
                    if (val > max)
                    {
                        max = val;
                    }
                }
                ((int16_t *)dst_bank_addr)[2 * out_col] = max;
            }
            row++;

            /* Inrease  rows to pointer to bank */
            if ((row & 0x7) == 0)
            {
                bank_ch_addr += 0x4000;
                bank_ch_addr -= 0x10000;
                bank_ch_addr += 4 * len_col;
                dst_bank_addr += 0x4000;
                dst_bank_addr -= 0x10000;
                dst_bank_addr += 4 * len_col;
            }
            else
            {
                bank_ch_addr  += 0x4000;
                dst_bank_addr += 0x4000;
            }
        };
        /* Reset bank_ch_addr and dst_bank_addr */
        bank_ch_addr = bank_addr;
        dst_bank_addr = bank_start_addr + (channel & 0x1) * 0x10000;
        dst_bank_addr += (channel / 2) * len_out_col * ((len_out_row + 7) / 8) * 4;
        dst_bank_addr += 2;

        for (row = -pool_pad_up, out_row = 0; out_row < len_out_row; row += pool_stride)
        {
            /* Do nothing for all bottom padding */
            pool_off = pool_size - 1;
            if (row + pool_off >= valid_len_row)
            {
                pool_off = valid_len_row - row - 1;
            }

            /* Set row start addr */
            for (i = 0; (row + i) < 0; i++);
            for (rowaddr_idx = 0; i <= pool_off; rowaddr_idx++, i++)
            {
                row_bank_addr[rowaddr_idx] = bank_ch_addr + (((row + i) >> 1) & 0x3) * 0x4000 + ((row + i) >> 3) * len_col * 4;
                if (0 == ((row + i) & 0x1))
                {
                    row_bank_addr[rowaddr_idx] += 2;
                }
            }
#if 1
            for (col = 0; col < len_out_col; col++)
            {
                max = -32768;
                for (i = 0; i < rowaddr_idx; i++)
                {
                    val = (((int16_t *)row_bank_addr[i]))[2 * col];
                    if (val > max)
                    {
                        max = val;
                    }
                }
                if (max == -32768)
                {
                    ((int16_t *)dst_bank_addr)[2 * col] = 0;
                }
                else
                {
                    ((int16_t *)dst_bank_addr)[2 * col] = max;
                }
            }
#endif
#if 0
            for (col = -pool_pad_left, out_col = 0; out_col < len_out_col; col += pool_stride, out_col++)
            {
                col_pool_off = pool_size - 1;
                if (col + col_pool_off >= len_col)
                {
                    col_pool_off = len_col - col - 1;
                }
                max = -32768;

                for (i = 0; i < rowaddr_idx; i++)
                {
                    for (pool_off = 0; (col + pool_off) < 0; pool_off++);
                    for (; pool_off <= col_pool_off; pool_off++)
                    {
                        val = (((int16_t *)row_bank_addr[i]))[2 * (col + pool_off)];
                        if (val > max)
                        {
                            max = val;
                        }
                    }
                }
                ((int16_t *)dst_bank_addr)[2 * out_col] = max;
            }
#endif
            out_row++;
            if ((out_row & 0x1) == 1)
            {
                dst_bank_addr -= 2;
            }
            else
            {
                /* Inrease  rows to pointer to bank */
                if ((out_row & 0x7) == 0)
                {
                    dst_bank_addr += 0x4000;
                    dst_bank_addr -= 0x10000;
                    dst_bank_addr += 4 * len_out_col;
                }
                else
                {
                    dst_bank_addr += 0x4000;
                }
                dst_bank_addr += 2;
            }
        }
#if 0
        /* Output and input share same memory, it is correct for most of cases, except such as pool_pad_left = 1, pool_size = 2, pool_stride = 1 */
        bank_ch_addr = bank_addr;
        dst_bank_addr = bank_start_addr + (channel & 0x1) * 0x10000;
        dst_bank_addr += (channel / 2) * len_out_col * ((len_out_row + 7) / 8) * 4;

        /* Going through all input rows and save results */
        for (row = 0; row < valid_len_row;)
        {
            /* Even row */
            for (col = -pool_pad_left, out_col = 0; out_col < len_out_col; col += pool_stride, out_col++)
            {
                /* Do nothing for all right padding */
                pool_off = pool_size - 1;
                if (col + pool_off >= len_col)
                {
                    pool_off = len_col - col - 1;
                }
                max = -32768;

                for (; (col + pool_off) >= 0 && pool_off >= 0; pool_off--)
                {
                    val = ((int16_t *)bank_ch_addr)[2 * (col + pool_off) + 1];
                    if (val > max)
                    {
                        max = val;
                    }
                }
                ((int16_t *)dst_bank_addr)[2 * out_col + 1] = max;
            }
            row++;

            /* Odd row */
            for (col = -pool_pad_left, out_col = 0; out_col < len_out_col; col += pool_stride, out_col++)
            {
                /* Do nothing for all right padding */
                pool_off = pool_size - 1;
                if (col + pool_off >= len_col)
                {
                    pool_off = len_col - col - 1;
                }
                max = -32768;

                for (; (col + pool_off) >= 0 && pool_off >= 0; pool_off--)
                {
                    val = ((int16_t *)bank_ch_addr)[2 * (col + pool_off)];
                    if (val > max)
                    {
                        max = val;
                    }
                }
                ((int16_t *)dst_bank_addr)[2 * out_col] = max;
            }
            row++;

            /* Inrease  rows to pointer to bank */
            if ((row & 0x7) == 0)
            {
                bank_ch_addr += 0x4000;
                bank_ch_addr -= 0x10000;
                bank_ch_addr += 4 * len_col;
                dst_bank_addr += 0x4000;
                dst_bank_addr -= 0x10000;
                dst_bank_addr += 4 * len_out_col;
            }
            else
            {
                bank_ch_addr  += 0x4000;
                dst_bank_addr += 0x4000;
            }
        };
        /* Going through all out cols and save results */
        for (col = 0; col < len_out_col; col++)
        {
            for (row = -pool_pad_up, dst_bank_off = 0; row < valid_len_row; row += pool_stride, dst_bank_off++)
            {
                /* Do nothing for all bottom padding */
                pool_off = pool_size - 1;
                if (row + pool_off >= valid_len_row)
                {
                    pool_off = valid_len_row - row - 1;
                }
                max = -32768;

                for (; (row + pool_off) >= 0 && pool_off >= 0; pool_off--)
                {
                    val = ((int16_t *)bank_addr)[2 * (len_out_col * ((row + pool_off) / 2) + col) + (((row + pool_off) & 1) ? 0 : 1)];
                    if (val > max)
                    {
                        max = val;
                    }
                }
                ((int16_t *)dst_bank_addr)[2 * (len_out_col * (dst_bank_off / 2) + col) + ((dst_bank_off & 1) ? 0 : 1)] = max;


            }
        }
#endif
    }
}

static void bank_route(struct easynet_ops_param *param, struct conv_hw_context *cxt)
{
    struct op_bank_route_param *op_param = (struct op_bank_route_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    struct bank_op_param *bank_param = &op_param->bank_param;
    int i;

    int16_t *input;
    uint32_t ch_off, channel, row, col, out_ch;
    uint32_t row_num, col_num, start_row, len_row, valid_len_row, start_col, len_col, start_out_ch, len_out_ch, end_out_ch, bank_ch, start_channel, end_channel, len_channel, real_col_num;
    void *bank_start_addr, *bank_addr;

    row_num   = bank_param->row_num;
    real_col_num   = bank_param->col_num;
#ifdef FORCE_COL_NUM_EVEN
    col_num   = ((bank_param->col_num + 1) / 2) * 2;
#else
    col_num   = real_col_num;
#endif
    start_row = bank_param->start_row;
    len_row   = bank_param->len_row;
    valid_len_row   = bank_param->valid_len_row;
    start_col = bank_param->start_col;
    len_col   = bank_param->len_col;
    start_out_ch = bank_param->start_channel;
    len_out_ch   = bank_param->len_channel;
    end_out_ch   = start_out_ch + len_out_ch;

    /* Select input bank */
    if (bank_param->bank == 0)
    {
        bank_start_addr = cxt->bank_a_addr;
    }
    else
    {
        bank_start_addr = cxt->bank_c_addr;
    }

    /*
     *  Get data from  all source buffers and do route operation, you can get part of source buffer
     */
    for (i = 0, out_ch = 0, bank_ch = 0; i < op_param->num; out_ch += op_param->ch_num[i], i++)
    {
        input = cxt->layer_buffer[op_param->buffers[i]];
        if ((out_ch + op_param->ch_num[i]) <= start_out_ch)
        {
            continue;
        }
        if (out_ch >= (start_out_ch + len_out_ch))
        {
            continue;
        }

        start_channel = (start_out_ch <= out_ch) ? 0 : (start_out_ch - out_ch);

        end_channel   = ((out_ch + op_param->ch_num[i]) < end_out_ch) ? (out_ch + op_param->ch_num[i]) : end_out_ch;
        end_channel  -=  out_ch;
        len_channel   = (end_channel - start_channel);

#if 1
        if (bank_param->weight_offset != 0)
        {
            bank_param->op = BANK_CP_FROM_MEM_WITHBN;
            bank_param->buffer  = op_param->buffers[i];
            bank_param->dst_channel_off = bank_ch;
            bank_param->start_channel = start_channel;
            bank_param->len_channel = len_channel;
            do_bank_op(bank_param, cxt);
        }
        else
        {
            bank_param->op = BANK_CP_FROM_MEM;
            bank_param->buffer  = op_param->buffers[i];
            bank_param->dst_channel_off = bank_ch;
            bank_param->start_channel = start_channel;
            bank_param->len_channel = len_channel;
            do_bank_op(bank_param, cxt);
        }
#else
        for (channel = 0; channel < len_channel; channel++)
        {
            /* Set bank pointer to channel start of bank */
            bank_addr  = bank_start_addr + ((bank_ch + channel) & 0x1) * 0x10000;
            bank_addr += ((bank_ch + channel) / 2) * len_col * ((len_row + 7) / 8) * 4;

            /* Set row pointer to channel start of shorcut buffer */
            ch_off = col_num * (start_row + row_num * (channel + start_channel)) + start_col;

            /* Process all bank data of one channel, you can optimize following code on platform */
            if (bank_param->weight_offset != 0)
            {
                uint32_t *bn = (uint32_t *)(cxt->weight_start_addr + bank_param->weight_offset);
                int16_t gamma = (int16_t)(bn[channel] & 0xFFFF);
                int16_t beta  = (int16_t)((bn[channel] >> 16) & 0xFFFF);
                int16_t val;

                for (row = 0; row < len_row;)
                {
                    /* Even row */
                    for (col = 0; col < len_col; ++col)
                    {
                        val = fix32_round_truncate(input[ch_off + col] * gamma) + beta;
                        ((int16_t *)bank_addr)[2 * col + 1] = val;
                    }
                    for (col = real_col_num - start_col; col < len_col; ++col)
                    {
                        ((int16_t *)bank_addr)[2 * col + 1] = 0;
                    }

                    /* Odd row */
                    for (col = 0; col < len_col; ++col)
                    {
                        val = fix32_round_truncate(input[ch_off + col_num + col] * gamma) + beta;
                        ((int16_t *)bank_addr)[2 * col] = val;
                    }
                    for (col = real_col_num - start_col; col < len_col; ++col)
                    {
                        ((int16_t *)bank_addr)[2 * col] = 0;
                    }

                    /* Increase 2 rows to pointer to shorcut buffer */
                    row += 2;
                    ch_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                }
            }
            else
            {
                for (row = 0; row < len_row;)
                {
                    /* Even row */
                    for (col = 0; col < len_col; ++col)
                    {
                        ((int16_t *)bank_addr)[2 * col + 1] = input[ch_off + col];
                    }
                    for (col = real_col_num - start_col; col < len_col; ++col)
                    {
                        ((int16_t *)bank_addr)[2 * col + 1] = 0;
                    }

                    /* Odd row */
                    for (col = 0; col < len_col; ++col)
                    {
                        ((int16_t *)bank_addr)[2 * col] = input[ch_off + col_num + col];
                    }
                    for (col = real_col_num - start_col; col < len_col; ++col)
                    {
                        ((int16_t *)bank_addr)[2 * col] = 0;
                    }

                    /* Increase 2 rows to pointer to shorcut buffer */
                    row += 2;
                    ch_off += 2 * col_num;
                    /* Inrease 2 rows to pointer to bank */
                    if ((row & 0x7) == 0)
                    {
                        bank_addr += 0x4000;
                        bank_addr -= 0x10000;
                        bank_addr += 4 * len_col;
                    }
                    else
                    {
                        bank_addr += 0x4000;
                    }
                }
            }
        }
#endif
        bank_ch += len_channel;
    }
    //Resume start channel and len_channel
    bank_param->start_channel = start_out_ch;
    bank_param->len_channel =  len_out_ch;
    //save_layer_bin("./examples/compiler/posenet/layer_in.bin", input, bank_param->row_num * bank_param->col_num*128*2);
}

static void bank_depthwise(struct easynet_ops_param *param, struct conv_hw_context *cxt, kdp_processor_t *processor)
{
    struct op_bank_depthwise_param *op_param = (struct op_bank_depthwise_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    struct bank_op_param *in_bank_param = &op_param->in_bank_param;
    struct bank_op_param *bank_param = &op_param->out_bank_param;
    struct op_hardware_param *hw_param = &op_param->hw_param;

    uint32_t op_idx, op;
    uint32_t len_channel, start_channel, in_bank, out_bank, in_len_channel, in_start_channel, recover_in_bank;
    int32_t ch;
    uint32_t weight_offset, one_weight_size;

    uint32_t recover_out_bank_param_start_channel = 0, recover_out_bank_param_len_channel, rcv_out_bank_param_bank;
    uint32_t rcv_in_bank_param_op;

    /* It is not input channel cut, but input channel number = output channel number  */
    in_len_channel = in_bank_param->len_channel;
    in_start_channel = in_bank_param->start_channel;

    len_channel = bank_param->len_channel;
    start_channel = bank_param->start_channel;
    weight_offset = hw_param->weight_offset;
    one_weight_size = hw_param->one_weight_size;
    hw_param->weight_offset = weight_offset + (len_channel - 1) * one_weight_size;
    in_bank  = in_bank_param->bank;
    recover_in_bank = in_bank;
    recover_out_bank_param_len_channel = bank_param->len_channel;
    recover_out_bank_param_start_channel = bank_param->start_channel;

    uint32_t rcv_in_bank_param_start_channel =  in_bank_param->start_channel;
    uint32_t rcv_in_bank_param_len_channel = in_bank_param->len_channel;

    uint32_t recover_out_bank_param_op = bank_param->op;
    uint32_t rcv_in_bank_param_src_channel_off = in_bank_param->src_channel_off;
    uint32_t rcv_in_bank_param_dst_channel_off =  in_bank_param->dst_channel_off;
    uint32_t  rcv_out_bank_param_src_channel_off = bank_param->src_channel_off;
    uint32_t  rcv_out_bank_param_dst_channel_off = bank_param->dst_channel_off;

    rcv_out_bank_param_bank = bank_param->bank;
    rcv_in_bank_param_op = in_bank_param->op;
    out_bank = ((in_bank_param->bank == 0) ? 1 : 0);

    for (ch = len_channel - 1; ch >= 0; ch--)
    {

        /* Copy one channel data to input start */
        in_bank_param->start_channel = start_channel + ch;
        in_bank_param->len_channel   = 1;
        if (in_bank_param->buffer != INVALID_BUFFER_IDX)
        {
            in_bank_param->op = BANK_CP_FROM_MEM;
            in_bank_param->bank = in_bank;
            do_bank_op(in_bank_param, cxt);

        }
        else
        {
            if (ch == len_channel - 1 || ch == 0)
            {
                in_bank_param->op = BANK_EXCHANGE_CHANNEL;
                in_bank_param->src_channel_off = len_channel - 1;
                in_bank_param->dst_channel_off = 0;
            }
            else
            {
                in_bank_param->op = BANK_COPY_CHANNEL;
                in_bank_param->src_channel_off = ch;
                in_bank_param->dst_channel_off = 0;
            }
            do_channel_cp(in_bank_param, cxt);
        }

        /* I do it for simulation debug */
        cxt->start_out_row = bank_param->start_row;
        cxt->start_out_col = bank_param->start_col;
        cxt->simu_start_out_ch = start_channel + ch;


        HAL_CNN_Bram_hfwd_rw_en(ENABLE);
        HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
        HAL_DMA_Send(hw_param->weight_len, (unsigned int)(processor->weight_addr_start + hw_param->weight_offset)); // stream fifo  address must 32 byte aliged
        put_cmd(processor->cmd_addr_start, hw_param->cmd_offset, hw_param->cmd_len);
        HAL_CNN_Start();
        HAL_CNN_Wait();
        HAL_CNN_Bram_hfwd_rw_en(DISABLE);
        HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);

        /* Save shortcut buffer if it is not last one of this combined layer, otherwise we do it in after_bank_op */
        if (op_param->referred_index >= 0 && op_param->referred_layer_num ==  op_param->bank_op.before_layer_number)
        {
            bank_param->op  =  BANK_CP_TO_MEM;
            bank_param->buffer =  op_param->referred_index;
            do_bank_op(bank_param, cxt);
        }

        for (op_idx = 0; op_idx < BANK_OP_MAX_STAGE; op_idx++)
        {
            op = BANK_OP_STAGE(op_param->bank_op.stage, op_idx);
            if (op == 0)
            {
                break;
            }

            switch (op)
            {
                case BANK_OP_MAXPOOL:
                {
                    bank_param->op = BANK_MAXPOOL;
                    bank_param->pool_size   = op_param->bank_op.pool_size;
                    bank_param->pool_stride = op_param->bank_op.pool_stride;
                    bank_param->pool_pad_up = op_param->bank_op.pool_pad_up;
                    bank_param->pool_pad_left = op_param->bank_op.pool_pad_left;
                    bank_param->pool_out_col  = op_param->bank_op.hardware_out_col;
                    bank_param->pool_out_row  = op_param->bank_op.hardware_out_row;
                    do_bank_maxpool(bank_param, cxt);
                    break;
                }
                case BANK_OP_SHORTCUT:
                {
                    bank_param->op = BANK_ADD_TO_MEM;
                    bank_param->dst_buffer = op_param->output_idx;
                    bank_param->buffer = op_param->shortcut_index;
                    bank_param->activation = op_param->bank_op.activation;
                    do_bank_op(bank_param, cxt);
                    break;
                }
                default:
                    break;
            }

            /* Save bank op result if referred by other layer except shortcut layer */
            if (op != BANK_OP_SHORTCUT && op_param->referred_index >= 0 && op_param->referred_layer_num ==  op_param->bank_op.stage_layer_num[op_idx])
            {
                bank_param->op     =  BANK_CP_TO_MEM;
                bank_param->buffer =  op_param->referred_index;
                do_bank_op(bank_param, cxt);
            }
        }

        /* Padding zero to end of cols to go round hardware bug */
        if (op_param->bank_op.pool_size != 1)
        {
            bank_param->start_col  = op_param->bank_op.hardware_start_out_col;    /* Start col in out bank */
            bank_param->len_col    = op_param->bank_op.hardware_out_col;          /* Cols in out bank */
        }

        if (bank_param->start_col + bank_param->len_col > bank_param->col_num)
        {
            bank_param->op     =  BANK_PADDING;
            do_bank_op(bank_param, cxt);
        }

        /* Copy one channel data from output start */
        bank_param->start_channel = start_channel + ch;
        bank_param->len_channel   = 1;
        if (bank_param->buffer != INVALID_BUFFER_IDX)
        {
            bank_param->op = BANK_CP_TO_MEM;
            bank_param->bank = out_bank;
            do_bank_op(bank_param, cxt);
        }
        else
        {
            bank_param->op = BANK_COPY_CHANNEL;
            bank_param->src_channel_off = 0;
            bank_param->dst_channel_off = ch;
            if (op_param->next_1x1 == 1)
            {
                do_channel_cp_1x1(bank_param, cxt, 1, ch);
            }
            else
            {
                do_channel_cp(bank_param, cxt);
            }
        }

        /* Do calculation backwards */
        hw_param->weight_offset -= one_weight_size;
    }




    in_bank_param->len_channel = in_len_channel;
    in_bank_param->start_channel = in_start_channel;
    bank_param->len_channel = len_channel;
    bank_param->start_channel = start_channel;

    hw_param->weight_offset = weight_offset;
}

static void bank_input_channel_cut(struct easynet_ops_param *param, struct conv_hw_context *cxt, kdp_processor_t *processor)
{
    struct op_channel_cut_param *op_param = (struct op_channel_cut_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    struct bank_op_param *bank_param = &op_param->bank_param;
    struct bank_op_param in_bank_param;
    struct op_hardware_param *hw_param = &op_param->hw_param;
    uint32_t op_idx, op;
    uint32_t in_bank, out_bank;
    int32_t ch;
    uint32_t weight_offset, one_weight_size;
    int i;

    weight_offset = hw_param->weight_offset;
    one_weight_size = hw_param->one_weight_size;
    out_bank = bank_param->bank;
    in_bank  = ((out_bank == 0) ? 1 : 0);

    for (ch = 0; ch < op_param->total_out_ch; ch++)
    {
        /* Insert one ouput channel after final input channel  */
        in_bank_param.len_row = op_param->in_len_row;
        in_bank_param.valid_len_row = op_param->in_valid_len_row;
        in_bank_param.len_col = op_param->in_len_col;
        in_bank_param.dst_channel_off = op_param->ch_num;
        in_bank_param.bank = in_bank;

        bank_param->src_channel_off = ch;
        bank_param->buffer = op_param->output_idx;
        if (op_param->compress_1x1 == 1)
        {
            do_channel_demap_compress_1x1(bank_param, &in_bank_param, cxt, 1, op_param->conv_stride);
        }
        else
        {
            do_channel_demap(bank_param, &in_bank_param, cxt, 1, op_param->conv_stride);
        }
        /* I do it for simulation debug */
        cxt->start_out_row = bank_param->start_row;
        cxt->start_out_col = bank_param->start_col;
        cxt->simu_start_out_ch = ch;

        unsigned char *addr_t = (unsigned int)(processor->weight_addr_start + hw_param->weight_offset);
        unsigned char *tmp_t;
        if (((unsigned int)(processor->weight_addr_start + hw_param->weight_offset) & 0xf) != 0)
        {
            tmp_t = (unsigned char *)malloc_ext(0, hw_param->weight_len);
            if (((unsigned int)tmp_t & 0xf) != 0)
            {
                printf("error\n");
                while (1);
            }

            memcpy(tmp_t, addr_t, hw_param->weight_len);
            HAL_CNN_Bram_hfwd_rw_en(ENABLE);
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
            HAL_DMA_Send(hw_param->weight_len, (unsigned int)tmp_t);
            free_ext(0, tmp_t);
        }
        else
        {
            HAL_CNN_Bram_hfwd_rw_en(ENABLE);
            HAL_CNN_Bram_HL_sel(SEL_LOW_16BIT);
            HAL_DMA_Send(hw_param->weight_len, (unsigned int)(processor->weight_addr_start + hw_param->weight_offset));    // stream fifo  address must 32 byte aliged

        }
        put_cmd(processor->cmd_addr_start, hw_param->cmd_offset, hw_param->cmd_len);
        HAL_CNN_Start();
        HAL_CNN_Wait();

        HAL_CNN_Bram_hfwd_rw_en(DISABLE);
        HAL_CNN_Bram_HL_sel(SEL_HIGT_16BIT);

        /* Copy one channel data from output start */
        bank_param->start_channel = ch;
        bank_param->len_channel   = 1;

        /* Save shortcut buffer if it is not last one of this combined layer, otherwise we do it in after_bank_op */
        if (op_param->referred_index >= 0 && op_param->referred_layer_num ==  op_param->bank_op.before_layer_number && !(op_param->referred_layer_num == op_param->end_layer_num))
        {
            bank_param->op  =  BANK_CP_TO_MEM;
            bank_param->buffer =  op_param->referred_index;

            do_bank_op(bank_param, cxt);
        }


        for (op_idx = 0; op_idx < BANK_OP_MAX_STAGE; op_idx++)
        {
            op = BANK_OP_STAGE(op_param->bank_op.stage, op_idx);
            if (op == 0)
            {
                break;
            }

            switch (op)
            {
                case BANK_OP_MAXPOOL:
                {
                    bank_param->op = BANK_MAXPOOL;
                    bank_param->pool_size   = op_param->bank_op.pool_size;
                    bank_param->pool_stride = op_param->bank_op.pool_stride;
                    bank_param->pool_pad_up = op_param->bank_op.pool_pad_up;
                    bank_param->pool_pad_left = op_param->bank_op.pool_pad_left;
                    bank_param->pool_out_col  = op_param->bank_op.hardware_out_col;
                    bank_param->pool_out_row  = op_param->bank_op.hardware_out_row;
                    do_bank_maxpool(bank_param, cxt);
                    break;
                }
                case BANK_OP_SHORTCUT:
                {
                    bank_param->op = BANK_ADD_TO_MEM;
                    bank_param->dst_buffer = op_param->output_idx;
                    bank_param->buffer = op_param->shortcut_index;
                    bank_param->activation = op_param->bank_op.activation;
                    do_bank_op(bank_param, cxt);
                    break;
                }
                default:
                    break;
            }

            /* Save bank op result if referred by other layer except shortcut layer */
            if (op != BANK_OP_SHORTCUT && op_param->referred_index >= 0 && op_param->referred_layer_num ==  op_param->bank_op.stage_layer_num[op_idx])
            {
                bank_param->op     =  BANK_CP_TO_MEM;
                bank_param->buffer =  op_param->referred_index;
                do_bank_op(bank_param, cxt);
            }
        }

        /* Padding zero to end of cols to go round hardware bug */
        if (op_param->bank_op.pool_size != 1)
        {
            bank_param->start_col  = op_param->bank_op.hardware_start_out_col;    /* Start col in out bank */
            bank_param->len_col    = op_param->bank_op.hardware_out_col;          /* Cols in out bank */
        }
        /* No padding is needed, because cross layers is not allowed for input channel cut */
        /* Move data of this channel out to memory */
        //if (op_param->referred_index != op_param->output_idx)
        {
            bank_param->op = BANK_CP_TO_MEM;
            bank_param->buffer = op_param->output_idx;
            do_bank_op(bank_param, cxt);
        }

        /* Do calculation backwards */
        hw_param->weight_offset += one_weight_size;
    }

    hw_param->weight_offset = weight_offset;
}

int bank_ops_process(struct easynet_dev *dev, struct easynet_ops_param *param, kdp_processor_t *processor)
{
    int ret = TRUE;
    struct conv_hw_context *cxt = ((struct conv_hw_context *)dev->context);

#ifdef DEBUG_TIME
    uint32_t time_last = 0, time_now = 0;
    time_last = k_uptime_get_32();
#endif

    switch (param->op)
    {
        case OP_CROSS_WRITEOUT:
            CrossGetData(param, cxt);
            break;
        case OP_CROSS_DATAFETCH:
            CrossPutData(param, cxt, dev->input);
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_CrossPut += time_now - time_last;
#endif
            break;
        case OP_BANK_ROUTE:
            bank_route(param, cxt);

            break;
        case OP_BANK_GENERAL:
        {
            struct bank_op_param *op_param = (struct bank_op_param *)(((unsigned char *)param) + sizeof(struct easynet_ops_param));

            if (op_param->op == BANK_MAXPOOL)
            {
                do_bank_maxpool(op_param, cxt);
            }
            else
            {
                kdp_print_bank_op(op_param->op);
                if (op_param->next_1x1 == 1 && (op_param->op == BANK_CP_FROM_MEM || op_param->op == BANK_CP_TO_BANK))
                {
                    do_bank_op_1x1(op_param, cxt);
                }
                else
                {
                    do_bank_op(op_param, cxt);
                }

            }
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_BankGeneral += time_now - time_last;
#endif
            break;
        }
        case OP_DEPTHWISE_CONV:
            //printf("before OP_DEPTHWISE_CONV\n");
            bank_depthwise(param, cxt, processor);
            //printf("After OP_DEPTHWISE_CONV\n");
#ifdef DEBUG_TIME
            time_now = k_uptime_get_32();
            t_dwcon += time_now - time_last;
#endif
            break;
        case OP_FINAL_INPUT_CHANNEL_CUT:
            bank_input_channel_cut(param, cxt, processor);
            break;

        default:
            break;
    }

    return ret;
}

