#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__
#include "ai_common.h"

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))

//#define CNN_TIME_TEST   1
#define IMP_TRANSFER_DATA  1

/*
 * The following constant controls the length of the buffers for each layer
 */

#define INPUT_WIDTH    224
#define INPUT_HEIGHT   224
#define TEST_BUFFER_SIZE     (INPUT_WIDTH * INPUT_HEIGHT * 3) + 2 /*input_fpga.out.bin, 0x24C02*/    // richard debug
#define PAD_WIDTH  224
#define PAD_HEIGHT 224
#define PAD_UP 0
#define PAD_BOTTOM 0
#define PAD_LEFT   0
#define PAD_RIGHT  0

#define FC_OUT 0
#define WEIGHT_BASEADDR    0x10000000
#define THRESHOLD 1
#define BUFFERSIZE 200704                 /*INPUT_WIDTH * INPUT_HEIGHT * 4*/

/*
 * Address Space for KDP310 IP
 */
#define BRAM_OUTPUT_A_BASEARDDR     0x20020000
#define BRAM_OUTPUT_B_BASEARDDR     0x20030000
#define BRAM_OUTPUT_C_BASEARDDR     0x20040000
#define BRAM_OUTPUT_D_BASEARDDR     0x20050000
#define BRAM_CMD_BASEARDDR          0x20060000
#define BRAM_INPUT_BASEADDR         0x20020000 //Input Bank: BankA, BankB; Output Bank: BankC BankD; Bank Address:0x4000,0000; BankB address: 0x4002,0000;

//SDRAM SIZE 32M

//#define SDRAM_COMPILE_BASE	((unsigned char *)0xC01F4000)//0xC01F4000
//#define MEM_AI_DATA_RESERVED_SIZE	(1<<20)
//#define CONFIG_SDRAM_MALLOC_SIZE    (3*1024*1024)

#define KDP_CTRL        0x46000000
#define KDP_STATUS      0x46000004
#define CORE_BASEADDR   0x46000000

/*
 * The following types are used to store user data
 *
 * */
typedef struct node
{
    struct node * next;
    int label;
    int baseCount;
    float * baseVector;
    //might have other variables such as name
} node_t;

int dma_send(unsigned int pkt_length, unsigned int base_address);
void Put_cmd(int start, int length);
void PutData(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage);
void GetData(struct easynet_ops_param *param, unsigned short *inputImage, unsigned short *outputImage);
void AveragePooling(int NumCol, int NumRow, int NumChannel, int validCol, int validRow, float * array, int source);
void maxPool(struct easynet_ops_param *param, unsigned short int  *outputImage);

int nms_comparator(const void *pa, const void *pb);


void FC_softMax(void);
void FC(float * fcVector);
void startCNN(void);
void waitCNN(void);
int reg_read(int addr);
void reg_write(int addr, int data);
//temp function for this version, will be deleted when use dynamic allocation
void moveToInput(int NumChannel, int NumRow, int NumCol, int NumColPad);

typedef struct kdp_processor
{
    unsigned short int *network_input;
    unsigned short *network_output;
    char *network_output_vector;
    unsigned char *cmd_addr_start;
    unsigned char *weight_addr_start;
}kdp_processor_t;

struct easynet_dev *easynet_process_init(char *seq_buf, unsigned short *pInputImage, unsigned short *pOutputImage);


#endif

