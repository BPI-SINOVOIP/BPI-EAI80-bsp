#ifndef _AT_ESP8266_H_
#define _AT_ESP8266_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <gm_hal_uart.h>

#define ESP8266_RECV_DATA_HEADER_LEN    8u              // the length of network data header \r\n+IPD,n 
#define AT_UART                         UART5
#define UART1_MAX_RECV_LEN              400u            //receive the max number byte from uart4
#define UART1_MAX_SEND_LEN              400u            //send the max number byte from uart4   


extern uint8_t uart1_txbuf[UART1_MAX_SEND_LEN];
extern uint8_t uart1_rxbuf[UART1_MAX_RECV_LEN];

enum _generic_status
{
    Status_Success              = 0,
    Status_Fail                 = 1,
    Status_ReadOnly             = 2,
    Status_OutOfRange           = 3,
    Status_InvalidArgument      = 4,
    Status_Timeout              = 5,
    Status_NoTransferInProgress = 6,
    Status_Network_connect_err  = 7,
    Status_CertVerifyFail       = 8,
    Status_Network_err          = 9,
    Status_ERROR_MALLOC         = 10,
};
/*! @brief Type used for all status and error return values. */
typedef uint8_t status_t;

enum at_cmd_e
{
    /*base at command*/
    AT_CMD_AT = 0,
    AT_CMD_AT_GETATVERSION,
    AT_CMD_AT_RST,
    /*wifi function command*/
    AT_CMD_AT_CWMODE,
    AT_CMD_AT_CWJAP,
    AT_CMD_AT_CWLAP,
    /*tcp/ip command*/
    AT_CMD_CIPSTART,
    AT_CMD_CIPSEND,
    AT_CMD_CIPCLOSE,

    AT_CMD_AT_RESTORE,
    AT_CMD_AT_UART_CUR,
    AT_CMD_UART_DEF,
    AT_CMD_SLEEP,
    AT_CMD_CWQAP,
    AT_CMD_AT_CWLIF,
    AT_CMD_CIPSSLCCONF,
    AT_CMD_CIPSENDEX,
    AT_CMD_CIPSERVER,

    AT_CMD_MAX = 0xff
};

typedef enum
{
    /* WiFi */
    TCP_SERVER,
    TCP_CLIENT,
    SSL_CLIENT,
    UDP_BROADCAST,
    UDP_UNICAST,
    /*WiFi end */
    /* Add others hereafter */
} CONN_TYPE;

/* Fill necessary fileds according to the socket type. */
typedef struct
{
    int fd; /* fd that are used in socket level */
    CONN_TYPE type;
    char *addr; /* remote ip or domain */
    int32_t r_port; /* remote port (set to -1 if not used) */
    int32_t l_port; /* local port (set to -1 if not used) */
    uint32_t tcp_keep_alive; /* tcp keep alive value (set to 0 if not used) */
} at_conn_t;


struct at_ap_command
{
    enum at_cmd_e id;
    const char *pre_cmd;
    const char *help;
};
extern const struct at_ap_command at_cmds_table[] ;


void ESP8266_UserProcess(void);

RET_CODE ESP8266_EnConfig(void);

RET_CODE ESP8266_ResetConfig(void);

RET_CODE ESP8266_UartPinMuxConfig(UART_Device_T *pUART);

/**
 * @brief Initializes with user configuration.
 *
 * @retval                None
 */
void at_uart_init(void);
/**
* brief Writes to the UART1 TX register using a blocking method.
*
* note This function does not check whether all data is sent out to the bus.
*
*/
void at_send(char *fmt, ...);


/*!
 * brief Read RX cmd response data register using a blocking method.
 *
 * This function polls the RX register, waits for the RX register to be full or for RX FIFO to
 * have data, and reads data from the TX register.
 *
 * param outData Start address of the buffer to store the received data.
 * param cmd     AT CMD
 * param length Size of the buffer.
 * param timeout waiting for receive time ,unit ms
 * retval Status_UART_RxHardwareOverrun Receiver overrun occurred while receiving data.
 * retval Status_UART_NoiseError A noise error occurred while receiving data.
 * retval Status_UART_FramingError A framing error occurred while receiving data.
 * retval Status_UART_ParityError A parity error occurred while receiving data.
 * retval Status_Success Successfully received all data.
 */
status_t AT_recv_cmdResp(UART_Device_T *pUart, char *cmd, uint8_t *pOutData, uint32_t outLen, uint32_t timeout);
status_t AT_recv_cmdCWJapResp(UART_Device_T *pUart, char *cmd, uint8_t *pOutData, uint32_t outLen, uint32_t timeout);

/*!
 * brief Read RX network data register using a blocking method.
 *
 * This function polls the RX register, waits for the RX register to be full or for RX FIFO to
 * have data, and reads data from the TX register.
 *
 * param outData Start address of the buffer to store the received data.
 * param length Size of the buffer.
 * param timeout waiting for receive time ,unit ms
 * retval Status_UART_RxHardwareOverrun Receiver overrun occurred while receiving data.
 * retval Status_UART_NoiseError A noise error occurred while receiving data.
 * retval Status_UART_FramingError A framing error occurred while receiving data.
 * retval Status_UART_ParityError A parity error occurred while receiving data.
 * retval Status_Success Successfully received all data.
 */
status_t AT_recv_NetWorkdata(UART_Device_T *pUart, uint8_t *pOutData, uint32_t outLen, uint32_t timeout, uint32_t recvlen);


/*!
 * brief receive from the response to sent data to AP
 *
 * This function polls the RX register, waits for the RX register to be full or for RX FIFO to
 * have data, and reads data from the TX register.
 *
 * param outData Start address of the buffer to store the received data.
 * param length Size of the buffer.
 * param timeout waiting for receive time ,unit ms
 * retval Status_UART_RxHardwareOverrun Receiver overrun occurred while receiving data.
 * retval Status_UART_NoiseError A noise error occurred while receiving data.
 * retval Status_UART_FramingError A framing error occurred while receiving data.
 * retval Status_UART_ParityError A parity error occurred while receiving data.
 * retval Status_Success Successfully received all data.
 */
status_t AT_recv_dataResp(UART_Device_T *pUart, uint8_t *pOutData, uint32_t outLen, uint32_t timeout);


#ifdef __cplusplus
}
#endif

#endif
