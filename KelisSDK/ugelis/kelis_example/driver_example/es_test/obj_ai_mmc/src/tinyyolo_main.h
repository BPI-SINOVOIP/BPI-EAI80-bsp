#ifndef __TINYYOLO_MAIN_H__
#define __TINYYOLO_MAIN_H__

/* Global R/W tasks */
#define HW32_REG(ADDRESS)  (*((volatile uint32_t  *)(ADDRESS)))
#define HW16_REG(ADDRESS)  (*((volatile uint16_t  *)(ADDRESS)))
#define HW8_REG(ADDRESS)   (*((volatile uint8_t  *)(ADDRESS)))


/*Only for BODY detection*/
//#define TINY_YOLO_FOR_BODY

/*Only for 20 Classes detection*/
//#define TINY_YOLO_FOR_OBJT

/*Only for gesture Classes detection*/
//#define TINY_YOLO_FOR_GESTURES

//#define CNN_TIME_TEST   1
#define IMP_TRANSFER_DATA  1
#define BODY_DETECTION_DEMO  1
#define GESTURE_DETECTION_DEMO  1


/*
 * The following constant controls the length of the buffers for each layer
 */

#define INPUT_WIDTH    224
#define INPUT_HEIGHT   224
#define TEST_BUFFER_SIZE   (INPUT_WIDTH * INPUT_HEIGHT * 3) + 2 /*input_fpga.out.bin, 0x24C02*/
#define PAD_WIDTH  224
#define PAD_HEIGHT 224
#define PAD_UP 0
#define PAD_BOTTOM 0
#define PAD_LEFT   0
#define PAD_RIGHT  0
#define RE_FORMAT_SIZE (PAD_WIDTH * PAD_HEIGHT * 4 )
#define SRAM_BUFFER_SIZE (PAD_WIDTH * PAD_HEIGHT * 4)
#define OUTPUT_WIDTH   7
#define OUTPUT_HEIGHT  7
#define LAST_WIDTH 7
#define LAST_HEIGHT    8

#ifdef TINY_YOLO_FOR_BODY
    //#define FC_CHANNEL 30
    //#define WEIGHT_LENGTH      0x459370       /*full_weights.bin*/
    //#define CMD_BUFFER_SIZE    882            /*generated_cmd_hex.out.bin*/
#endif

#ifdef TINY_YOLO_FOR_OBJT
    #define FC_CHANNEL 125
    #define WEIGHT_LENGTH      0x157ce68       /*full_weights.bin*/
    #define CMD_BUFFER_SIZE    924             /*generated_cmd_hex.out.bin*/
    #define CHANNELIMIT        4
#endif

#ifdef TINY_YOLO_FOR_GESTURES
    //#define FC_CHANNEL         35
    //#define WEIGHT_LENGTH      0x45CF98       /*full_weights.bin*/
    //#define CMD_BUFFER_SIZE    888            /*generated_cmd_hex.out.bin*/
    //#define CHANNELIMIT        4
#endif



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

#define IMG_LEN         TEST_BUFFER_SIZE
#define BASEIMG_LEN     172818 //TEST_BUFFER_SIZE * PIC_CNTS

#if 0
    #define CMD_BASE        0x10010000
    //|32k
    #define IMG_BASE        0x10018000
    //|32k
    #define WEI_BASE        0x10020000
    //|3M
    #define BASEIMG_BASE    0x10320000
    //|3M
    #define CAMERA_BUF_BASE 0x10620000
#else
    //SDRAM SIZE 32M
    //|0xC01F4000 + 22M(0x1600000)
    //|0xC01F4000 +10M(A00000)
    #define SDRAM_COMPILE_BASE  ((unsigned char *)0xC01F4000)//0xC01F4000
    #define MEM_AI_DATA_RESERVED_SIZE   (1<<20)
    #define CONFIG_SDRAM_MALLOC_SIZE    (3*1024*1024)
    #define AI_OFF          (SDRAM_COMPILE_BASE+MEM_AI_DATA_RESERVED_SIZE+CONFIG_SDRAM_MALLOC_SIZE)//  4M:
    //#define CMD_BASE        (AI_OFF+0x00010000) //C05F4000//C0B04000
    //|32k
    //#define IMG_BASE        (AI_OFF+0x00018000) //C05FC000//C0B0C000
    //|32k
    //#define WEI_BASE        (AI_OFF+0x00020000) //C0604000//C0B14000
    //|3M
    //#define BASEIMG_BASE    (AI_OFF+0x00320000) //C0904000//C0E14000
#endif

#define KDP_CTRL        0x46000000
#define KDP_STATUS      0x46000004
#define CORE_BASEADDR   0x46000000
//#define FC_W_BASEADDR   (WEI_BASE + WEIGHT_LENGTH)

/*
 * The following types are used to store user data
 *
 * */
typedef struct node
{
    struct node *next;
    int label;
    int baseCount;
    float *baseVector;
    //might have other variables such as name
} node_t;


int dma_send(unsigned int pkt_length, unsigned int base_address);
void call_cnn(unsigned char *addr);
//void call_layer(int layer_index, int first, int last, int load_input, int load_output, int NumChannelInput, int NumRowInput, int NumColInput, int lengthInput, int start_addrInput, int isAB, int NumChannelOutput, int NumRowOutput, int NumColOutput, int lengthSRAMOutput, int lengthOutput, int start_addrOutput, int startChannel, int lenChannel, int cmdaddr, int cmdlen, int weightaddr, int weightlen, int moveData, int pooling, int directStart, int directEnd);
void call_layer(int layer_index, int first, int last, int load_input, int load_output, int NumChannelInput, int NumRowInput, int NumColInput, int lengthInput, int start_addrInput, int isAB, int NumChannelOutput, int NumRowOutput, int NumColOutput, int lengthSRAMOutput, int lengthOutput, int start_addrOutput, int startChannel, int lenChannel, int cmdaddr, int cmdlen, int weightaddr, int weightlen, int moveData, int pooling, int directStart, int directEnd, int SRAM_start);
void pre_processing(unsigned char *addr);  //Get input data and preprocessing data, void now but  *** or  * when dynamic allocation
void Put_cmd(int start, int length);
//void PutData(int start, int length, int NumCol, int NumChannel, int NumRow); //  *** or  * will be passed into in the future
void PutData(int start, int length, int NumCol, int NumChannel, int NumRow, int source);
void GetData(int start, int lengthSRAMOutput, int length, int NumCol, int NumChannel, int NumRow, int isAB, int startChannel, int lenChannel);
//void AveragePooling(int NumCol, int NumRow, int NumChannel, int validCol, int validRow, float * array);
void AveragePooling(int NumCol, int NumRow, int NumChannel, int validCol, int validRow, float *array, int source);


//void maxPool(int Numrow, int Numcolumn, int Numchannel);
void maxPool(int Numrow, int Numcolumn, int Numchannel, int source);

int nms_comparator(const void *pa, const void *pb);
void PostYolo(void);

void FC_softMax(void);
void FC(float *fcVector);
void startCNN(void);
void waitCNN(void);
int reg_read(int addr);
void reg_write(int addr, int data);
//temp function for this version, will be deleted when use dynamic allocation
void moveToInput(int NumChannel, int NumRow, int NumCol, int NumColPad);


//int record_base(void);

#endif //__TINYYOLO_MAIN_H__
