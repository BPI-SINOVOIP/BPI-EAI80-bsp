#ifndef _STREAMFIFO_H_
#define _STREAMFIFO_H_

#define     __IO    volatile             /*!< Defines 'read / write' permissions              */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

typedef struct
{
    __IO uint32_t CTRL     ;
    __IO uint32_t STATUE   ;
    __IO uint32_t SRC      ;
    __IO uint32_t DATACNT  ;
    __IO uint32_t DATD     ;
    __IO uint32_t VERSION  ;
}STRFIFO_Device_T;

//* Base addresses of STRFIFO Peripheral Control Registers */
#define STRFIFO_BASE                               (0x46001000U)

/* I2C Device Pointer */
#define STRFIFO                                    ((STRFIFO_Device_T *)STRFIFO_BASE)


#define   AHB_SINGLE    0  //3'b000
#define   AHB_INCR      1  //3'b001
#define   AHB_WRAP4     2  //3'b010
#define   AHB_WRAP8     4  //3'b100
#define   AHB_WRAP16    6  //3'b110


#define   UNLEN4        0  //3'b000
#define   UNLEN8        1  //3'b001
#define   UNLEN16       2  //3'b010
#define   UNLEN32       3  //3'b011
#define   UNLEN64       4  //3'b100
#define   UNLEN128      5  //3'b101
#define   UNLEN256      6  //3'b110
#define   UNLEN512      7  //3'b111

#define   NORMAL_MODE   0
#define   CONTINUE_MODE 1


void streamfifo_init(uint32_t *src, uint32_t len, uint8_t burst_type, uint8_t burst_unlen, uint8_t continue_mode);
#endif
