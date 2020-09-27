#ifndef _UART_PROTOCOL_H__
#define _UART_PROTOCOL_H__

#define  DEBUG_PRINT

/** add by colin **/
#define FRAME_HEADER1           0x5E
#define FRAME_HEADER2           0x5E
#define MAX_INFO_LEN            78
#define MAX_RX_BUF              256
#define MAX_TX_BUF              256
#define UART_BAUD_RATE          9600L//19200L
#define FRAME_LENGTH_INDEX      4
#define FRAME_COMMAND_INDEX     5
#define FRAME_INFO1_INDEX       6
#define FRAME_INFO2_INDEX       7
#define FRAME_INFO3_INDEX       8

#ifndef  DEBUG_PRINT
    #define PRINTF(...)
    #define PRINTK(...)
#else
    #define PRINTF(...)  printf
    #define PRINTK(...)  printf
#endif

typedef struct
{
    uint8_t         frameHeader1;                  /**<Frame Header 1 */
    uint8_t         frameHeader2;                  /**<Frame Header 2 */
    uint8_t         destAddr;                      /**< Dest Address */
    uint8_t         srcAddr;                       /**< Source Address */
    uint8_t         frameLen;                      /**< Frame Length */
    uint8_t         cmd;                           /**< commander */
    uint8_t         frameInfo[MAX_INFO_LEN];       /**< Frame Information */
    uint8_t         check;                         /**< Verfication Byte */
} Frame_T;

/**
 * Device Address
 */
typedef enum
{
    BROADCAST_ADDR                  = 0x00,  /**< Broadcast Device Address */
    CAMERAL_BOARD_ADDR              = 0xFF,  /**< Cameral   Device Address */
    HOUSE_AIR_COND_ADDR             = 0xF0,  /**<Household Air Conditon Address  */
    MERCHANT_AIR_COND_ADDR          = 0xF1   /**<Merchant Air Conditon Address  */
} Device_Addr_T;

/**
 * Device Address
 */
typedef enum
{
    FRAME_CTRL_STATUS_CMD           = 0x1A,  /**< The Commander Means The Frame Type Is Control Status Frame */
    FRAME_PERSON_INFO_CMD           = 0x1B,  /**< The Commander Means The Frame Type Is Person Information Frame */
    FRAME_SET_CMD                   = 0x1C,  /**< The Commander Means The Frame Type Is Setting Frame */
    FRAME_DEVICE_INFO_CMD           = 0x1D,  /**< The Commander Means The Frame Type Is Device Info Frame */
    FRAME_CTRL_ANSWER_CMD           = 0x8A,  /**< The Commander Means The Frame Type Is Control Answer Frame */
    FRAME_SET_ANSWER_CMD            = 0x8C,  /**< The Commander Means The Frame Type Is Setting Answer Frame */
    FRAME_DEVICE_INFO_ANSWER_CMD    = 0x8D   /**< The Commander Means The Frame Type Is Device Info Answer Frame */
} Frame_Type_Cmd_T;


#endif