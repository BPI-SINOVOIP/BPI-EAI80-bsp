#include "at_esp8266.h"
#include <pinmux.h>

#include <systick.h>



#define ERROR_TRACE(fmt, ...)  \
    do { \
        printk("%s|%03d :: ", __func__, __LINE__); \
        printk(fmt, ##__VA_ARGS__); \
        printk("%s", "\r\n"); \
    } while(0)


#define DBG_MODE

#ifdef DBG_MODE
    #define DEBUG(...)       do{printk(__VA_ARGS__);}while(0)
#else
    #define DEBUG(...)
#endif


UART_Handle_T g_ATuartHandle;

uint8_t uart1_txbuf[UART1_MAX_SEND_LEN] = {0};
uint8_t uart1_rxbuf[UART1_MAX_RECV_LEN] = {0};

const uint8_t *pWifiSta_SSID = "HUAWEI";          // route ssid
const uint8_t *pWifiSta_PWD  = "9876543210";        // connect password
const uint8_t *pWifi_IP      = "192.168.43.1";    // connect ip
const uint8_t *pNetworkMode  = "TCP";             // network work mode
const uint16_t pPort         = 10500 ;             // network port
#define SEND_N                  24u               //UART1_MAX_SEND_LEN 
#define NETWORK_DATA_HEAD_LEN   8u                // the length of network data header \r\n+IPD,n 

uint8_t databuf[30] = {0}; /*发送的一定是字符串，且发送数目一定大于等于AT+CIPSEND=n中n,以'\r\n'结束  */
char key[21] = "12a1MZxOdcBnO";
uint8_t *tbuf[2];

const struct at_ap_command at_cmds_table[] =
{
    { .id = AT_CMD_AT, .pre_cmd = "AT", .help = "test AT start"},
    { .id = AT_CMD_AT_GETATVERSION, .pre_cmd = "AT+GMR", .help = "Get version information" },
    { .id = AT_CMD_AT_RST, .pre_cmd = "AT+RST", .help = "reboot wifi module"},
    { .id = AT_CMD_AT_CWMODE, .pre_cmd = "AT+CWMODE", .help = "set Ap or Station mode"},
    { .id = AT_CMD_AT_CWJAP, .pre_cmd = "AT+CWJAP", .help = "AT+CWJAP=\"ssid\",\"pwd\""},
    { .id = AT_CMD_AT_CWLAP, .pre_cmd = "AT+CWLAP", .help = "list the IP of used"},
    { .id = AT_CMD_CIPSTART, .pre_cmd = "AT+CIPSTART", .help = "creat tcp/udp connected"},
    { .id = AT_CMD_CIPSEND, .pre_cmd = "AT+CIPSEND", .help = "AT+CIPSEND=4 send 4 byte data"},
    { .id = AT_CMD_CIPCLOSE, .pre_cmd = "AT+CIPCLOSE", .help = "turn off connected"},

    { .id = AT_CMD_AT_RESTORE, .pre_cmd = "AT+RESTORE", .help = " Restore the factory setting"},
    { .id = AT_CMD_AT_UART_CUR, .pre_cmd = "AT+UART_CUR", .help = "AT+UART_CUR=baudrate,databits,stop bits,parity,flow control set uart current temporary config"},
    { .id = AT_CMD_UART_DEF, .pre_cmd = "AT+UART_DEF", .help = "AT+UART_DEF=baudrate,databits,stop bits,parity,flow control set uart config"},
    { .id = AT_CMD_SLEEP, .pre_cmd = "AT+SLEEP", .help = "AT+SLEEP=sleep mode set sleep mode"},
    { .id = AT_CMD_CWQAP, .pre_cmd = "AT+CWQAP", .help = "disconnected AP"},

    { .id = AT_CMD_AT_CWLIF, .pre_cmd = "AT+CWLIF", .help = "Query connect to ESP32 SoftAP's Station information"},
    { .id = AT_CMD_CIPSSLCCONF, .pre_cmd = "AT+CIPSSLCCONF", .help = "AT+CIPSSLCCONF=type,cert_key_ID,CA_ID config SSL client"},
    { .id = AT_CMD_CIPSENDEX, .pre_cmd = "AT+CIPSENDEX", .help = "AT+CIPSENDEX=length send length byte data"},
    { .id = AT_CMD_CIPSERVER, .pre_cmd = "AT+CIPSERVER", .help = "AT+CIPSERVER=mode,port, set up/shut down TCP/SSL server"},
    { .id = AT_CMD_MAX, .help = "end" },
};


static uint32_t ESP8266_AT_Cwmode(void);
static uint32_t ESP8266_AT_Reset(void);
static uint32_t ESP8266_AT_CWJap(void);
static uint32_t ESP8266_AT_CIPStart(void);
static uint32_t ESP8266_AT_CIPSend(void);
static uint32_t ESP8266_AT_Gmr(void);
static uint32_t ESP8266_AT_CIPClose(void);


/**
  * @brief  connect uart,test uart function.
  *
  * @param  data: pointer to data.
  * @param  len: the data of length.
  *
  * @retval None.
  */
static void AT_CONN_Send(uint8_t  *data, uint32_t len)
{

    printk("%s-\r\n", __FUNCTION__);
    printk("%s\r\n", data);
    memcpy(tbuf, data, len);
    printk("%s\r\n", (char *)tbuf); /*print tbuf[0] context*/
    printk("%x\r\n", &tbuf[0]); /*print tbuf[0] address*/
    printk("%s\r\n", (char *)&tbuf[0]); /*print tbuf context*/
    memcpy(&tbuf[1], data, len);
    printk("%s\r\n", (char *)&tbuf[1]); /*print tbuf[1] context*/
}

static void conn_send(const void *data, uint32_t size)
{
    printk("%s-%s\r\n", __FUNCTION__, (const char *)data);
    AT_CONN_Send((uint8_t *)data, size);
}


/**
  * @brief  Send AT instruction.
  * @param  None.
  * @retval Status_Success：Return value is ok.
  * @retval Status_InvalidArgument：Return value is invalid.
  */
static uint32_t ESP8266_AT(void)
{
    uint32_t res = Status_Success;
    memset(uart1_txbuf, 0, UART1_MAX_SEND_LEN);
    memset(uart1_rxbuf, 0, UART1_MAX_RECV_LEN);
    strcpy((char *)&uart1_txbuf, at_cmds_table[AT_CMD_AT].pre_cmd);
    strcat((char *)&uart1_txbuf, "\r\n");

    at_send("%s", &uart1_txbuf);

    res = AT_recv_cmdResp(AT_UART, (char *)&uart1_txbuf, uart1_rxbuf, UART1_MAX_RECV_LEN, 30);
    if (res != 0)
    {
        return res;
    }
    else
    {
        printk("%s ", (const char *)&uart1_rxbuf);
        printk("\r\n");
    }
    return res;
}

/**
  * @brief  Send AT+GMR instruction.
  * @param  None.
  * @retval Status_Success：Return value is ok.
  * @retval Status_InvalidArgument：Return value is invalid.
  */
static uint32_t ESP8266_AT_Gmr(void)
{
    uint16_t i;
    uint32_t res = Status_Success;
    uint8_t cmdbuf[10];

    memset(uart1_txbuf, 0, UART1_MAX_SEND_LEN);
    memset(uart1_rxbuf, 0, UART1_MAX_RECV_LEN);
    strcpy((char *)&uart1_txbuf, at_cmds_table[AT_CMD_AT_GETATVERSION].pre_cmd);
    strcat((char *)&uart1_txbuf, "\r\n");

    sprintf((char *)&uart1_txbuf, "%s\r\n", at_cmds_table[AT_CMD_AT_GETATVERSION].pre_cmd);
    at_send("%s", &uart1_txbuf);
    res = AT_recv_cmdResp(AT_UART, (char *)&cmdbuf, uart1_rxbuf, UART1_MAX_RECV_LEN, 30);
    if (res != 0)
    {
        return res;
    }
    else
    {
        for (i = 0 ; i < 1; i++)
        {
            printk("%s ", (const char *)&uart1_rxbuf[i]);
        }
        printk("\r\n");
    }
    return res;
}

/**
  * @brief  Send AT+CWMODE=3 instruction.
  * @param  None.
  * @retval Status_Success：Return value is ok.
  * @retval Status_InvalidArgument：Return value is invalid.
  */
static uint32_t ESP8266_AT_Cwmode(void)
{
    uint32_t res = Status_Success;

    memset(uart1_txbuf, 0, UART1_MAX_SEND_LEN);
    memset(uart1_rxbuf, 0, UART1_MAX_RECV_LEN);
    strcpy((char *)&uart1_txbuf, at_cmds_table[AT_CMD_AT_CWMODE].pre_cmd);
    strcat((char *)&uart1_txbuf, "=3\r\n");
    at_send("%s", &uart1_txbuf);
    res = AT_recv_cmdResp(AT_UART, (char *)&uart1_txbuf, uart1_rxbuf, UART1_MAX_RECV_LEN, 30);
    if (res != 0)
    {
        return res;
    }
    else
    {

        printk("%s ", (const char *)&uart1_rxbuf);

        printk("\r\n");
    }

    return res;
}

/**
  * @brief  Send AT+RESET instruction.
  * @param  None.
  * @retval Status_Success：Return value is ok.
  * @retval Status_InvalidArgument：Return value is invalid.
  */
static uint32_t ESP8266_AT_Reset(void)
{
    uint32_t res = Status_Success;

    memset(uart1_txbuf, 0, UART1_MAX_SEND_LEN);
    memset(uart1_rxbuf, 0, UART1_MAX_RECV_LEN);
    strcpy((char *)&uart1_txbuf, at_cmds_table[AT_CMD_AT_RST].pre_cmd);
    strcat((char *)&uart1_txbuf, "\r\n");
    at_send("%s", &uart1_txbuf);
    res = AT_recv_cmdResp(AT_UART, (char *)&uart1_txbuf, uart1_rxbuf, UART1_MAX_RECV_LEN, 30);
    if (res != 0)
    {
        return res;
    }
    else
    {
        printk("%s ", (const char *)&uart1_rxbuf);
        printk("\r\n");
    }
    return res;
}

/**
  * @brief  Send AT+CWJAP instruction.
  * @param  None.
  * @retval Status_Success：Return value is ok
  * @retval Status_InvalidArgument：Return value is invalid
  */
static uint32_t ESP8266_AT_CWJap(void)
{
    uint32_t res = Status_Success;
    uint8_t cmdbuf[10];
    memset(uart1_txbuf, 0, UART1_MAX_SEND_LEN);
    memset(uart1_rxbuf, 0, UART1_MAX_RECV_LEN);
    strcpy((char *)&cmdbuf, at_cmds_table[AT_CMD_AT_CWJAP].pre_cmd);
    strcat((char *)&cmdbuf, "\r\n");

    sprintf((char *)&uart1_txbuf, "%s=\"%s\",\"%s\"\r\n", at_cmds_table[AT_CMD_AT_CWJAP].pre_cmd, pWifiSta_SSID, pWifiSta_PWD);
    at_send("%s", &uart1_txbuf);

    res = AT_recv_cmdCWJapResp(AT_UART, (char *)&cmdbuf, uart1_rxbuf, UART1_MAX_RECV_LEN, 30);
    if (res != 0)
    {
        return res;
    }
    else
    {
        printk("%s ", (const char *)&uart1_rxbuf);
        printk("\r\n");
    }
    return res;
}

/**
  * @brief  Send AT instruction.
  * @param  None.
  * @retval Status_Success：Return value is ok
  * @retval Status_InvalidArgument：Return value is invalid
  */
static uint32_t ESP8266_AT_CIPStart(void)
{

    uint32_t res = Status_Success;
    uint8_t cmdbuf[10];

    memset(uart1_txbuf, 0, UART1_MAX_SEND_LEN);
    memset(uart1_rxbuf, 0, UART1_MAX_RECV_LEN);
    strcpy((char *)&cmdbuf, at_cmds_table[AT_CMD_CIPSTART].pre_cmd);
    strcat((char *)&cmdbuf, "\r\n");

    sprintf((char *)&uart1_txbuf, "%s=\"%s\",\"%s\",%d\r\n", at_cmds_table[AT_CMD_CIPSTART].pre_cmd, pNetworkMode, pWifi_IP, pPort);
    at_send("%s", &uart1_txbuf);

    res = AT_recv_cmdResp(AT_UART, (char *)&cmdbuf, uart1_rxbuf, UART1_MAX_RECV_LEN, 30);
    if (res != 0)
    {
        return res;
    }
    else
    {
        printk("%s ", (const char *)&uart1_rxbuf);
        printk("\r\n");
    }
    return res;
}

/**
  * @brief  Send AT instruction.
  * @param  None.
  * @retval Status_Success：Return value is ok
  * @retval Status_InvalidArgument：Return value is invalid
  */
static uint32_t ESP8266_AT_CIPSend(void)
{
    uint32_t res = Status_Success;
    uint8_t cmdbuf[10];

    memset(uart1_txbuf, 0, UART1_MAX_SEND_LEN);
    memset(uart1_rxbuf, 0, UART1_MAX_RECV_LEN);
    strcpy((char *)&cmdbuf, at_cmds_table[AT_CMD_CIPSEND].pre_cmd);
    strcat((char *)&cmdbuf, "\r\n");

    sprintf((char *)&uart1_txbuf, "%s=%d\r\n", at_cmds_table[AT_CMD_CIPSEND].pre_cmd, SEND_N);
    at_send("%s", &uart1_txbuf);
    res = AT_recv_cmdResp(AT_UART, (char *)&cmdbuf, uart1_rxbuf, UART1_MAX_RECV_LEN, 30);
    if (res != 0)
    {
        return res;
    }
    else
    {

        printk("%s ", (const char *)&uart1_rxbuf);
        printk("\r\n");
    }
    return res;
}


/**
  * @brief  Send AT+CIPCLOSE instruction.
  * @param  None.
  * @retval Status_Success：Return value is ok.
  * @retval Status_InvalidArgument：Return value is invalid.
  */
static uint32_t ESP8266_AT_CIPClose(void)
{
    uint32_t res = Status_Success;
    uint8_t cmdbuf[10];

    memset(uart1_txbuf, 0, UART1_MAX_SEND_LEN);
    memset(uart1_rxbuf, 0, UART1_MAX_RECV_LEN);
    strcpy((char *)&uart1_txbuf, at_cmds_table[AT_CMD_CIPCLOSE].pre_cmd);
    strcat((char *)&uart1_txbuf, "\r\n");

    sprintf((char *)&uart1_txbuf, "%s\r\n", at_cmds_table[AT_CMD_CIPCLOSE].pre_cmd);
    at_send("%s", &uart1_txbuf);
    res = AT_recv_cmdResp(AT_UART, (char *)&cmdbuf, uart1_rxbuf, UART1_MAX_RECV_LEN, 30);
    if (res != 0)
    {
        return res;
    }
    else
    {
        printk("%s ", (const char *)&uart1_rxbuf);
        printk("\r\n");
    }
    return res;
}

/**
  * @brief  the process of esp8266 tramsmision and receive.
  * @param  None.
  * @retval None.
  */
void ESP8266_UserProcess(void)
{
    uint16_t i;
    uint32_t res = Status_Success;
    uint32_t recvlen;

    conn_send(key, 21);

    while (1)
    {
        res = ESP8266_AT();
        if (res != 0)
        {
            printk("AT_GMR Failed!\r\n");
            goto out;
        }

        res = ESP8266_AT_Gmr();
        if (res != 0)
        {
            printk("AT_GMR Failed!\r\n");
            goto out;
        }
        res = ESP8266_AT_Cwmode();
        if (res != 0)
        {
            printk("AT_CWMODE Failed!\r\n");
            goto out;
        }

        res = ESP8266_AT_CWJap();
        if (res != 0)
        {
            printk("AT_CWJAP Failed!\r\n");
            goto out;
        }

        res = ESP8266_AT_CIPStart();
        if (res != 0)
        {
            printk("AT_CIPSTART Failed!\r\n");
            goto out;
        }

        res = ESP8266_AT_CIPSend();
        if (res != 0)
        {
            printk("AT_CIPSEND Failed!\r\n");
            goto out;
        }
        /*发送的一定是字符串，且发送数目一定大于等于AT+CIPSEND=n中n,以'\r\n'结束  */
        at_send("%s", "send data to tcp server!");
        res = AT_recv_dataResp(AT_UART, uart1_rxbuf, UART1_MAX_RECV_LEN, 30);
        if (res != 0)
        {
            printk("Failed:receive the result from sending data!\r\n");
            goto out;
        }
        else
        {

            printk("%s ", (const char *)&uart1_rxbuf);
            printk("\r\n");
        }
        while (1)
        {
            memset(uart1_txbuf, 0, UART1_MAX_SEND_LEN);
            memset(uart1_rxbuf, 0, UART1_MAX_RECV_LEN);
            res = AT_recv_NetWorkdata(AT_UART, uart1_rxbuf, NETWORK_DATA_HEAD_LEN, 30, recvlen);
            if (res == 0)
            {
                for (i = 0 ; i < 1; i++)
                {
                    printk("%s ", (const char *)&uart1_rxbuf[i]);
                }
                printk("\r\n");
            }
            else
            {
                printk("Failed:Receive from network data !\r\n");
            }
        }
out:
        if (res != 0)
        {
            res = ESP8266_AT_Reset();
            if (res != 0)
            {
                printk("AT_RST Failed!\r\n");

            }
            printk("there is a problem!waiting for check!\r\n");
        }


        while (1);

    }
}

//m^n函数
//返回值:m^n次方
uint32_t usmart_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
    {
        result *= m;
    }
    return result;
}
//把字符串转为数字
//支持16进制转换,但是16进制字母必须是大写的,且格式为以0X开头的.
//支持负数
//*str:数字字符串指针
//*res:转换完的结果存放地址.
//返回值:0，成功转换完成.其他,错误代码.
//1,数据格式错误.2,16进制位数为0.3,起始格式错误.4,十进制位数为0.
uint8_t usmart_str2num(uint8_t *str, uint32_t *res)
{
    uint32_t t;
    int tnum;
    uint8_t bnum = 0;   //数字的位数
    uint8_t *p;
    uint8_t hexdec = 10; //默认为十进制数据
    uint8_t flag = 0;   //0,没有符号标记;1,表示正数;2,表示负数.
    p = str;
    *res = 0; //清零.
    while (1)
    {
        if ((*p <= '9' && *p >= '0') || ((*str == '-' || *str == '+') && bnum == 0) || (*p <= 'F' && *p >= 'A') || (*p == 'X' && bnum == 1)) //参数合法
        {
            if (*p >= 'A')
            {
                hexdec = 16;    //字符串中存在字母,为16进制格式.
            }
            if (*str == '-')
            {
                flag = 2;    //偏移掉符号
                str += 1;
            }
            else if (*str == '+')
            {
                flag = 1;    //偏移掉符号
                str += 1;
            }
            else
            {
                bnum++;    //位数增加.
            }
        }
        else if (*p == '\0')
        {
            break;    //碰到结束符,退出.
        }
        else
        {
            return 1;    //不全是十进制或者16进制数据.
        }
        p++;
    }
    p = str;            //重新定位到字符串开始的地址.
    if (hexdec == 16)   //16进制数据
    {
        if (bnum < 3)
        {
            return 2;    //位数小于3，直接退出.因为0X就占了2个,如果0X后面不跟数据,则该数据非法.
        }
        if (*p == '0' && (*(p + 1) == 'X')) //必须以'0X'开头.
        {
            p += 2; //偏移到数据起始地址.
            bnum -= 2; //减去偏移量
        }
        else
        {
            return 3;    //起始头的格式不对
        }
    }
    else if (bnum == 0)
    {
        return 4;    //位数为0，直接退出.
    }
    while (1)
    {
        if (bnum)
        {
            bnum--;
        }
        if (*p <= '9' && *p >= '0')
        {
            t = *p - '0';    //得到数字的值
        }
        else
        {
            t = *p - 'A' + 10;    //得到A~F对应的值
        }
        *res += t * usmart_pow(hexdec, bnum);
        p++;
        if (*p == '\0')
        {
            break;    //数据都查完了.
        }
    }
    if (flag == 2) //是负数?
    {
        tnum = -*res;
        *res = tnum;
    }
    return 0;//成功转换
}

/**
  * @brief  esp8266 response instruction,for cmd AT+CWJAP response.
  *
  * @param  pUart   Pointer to UART device handler.
  * @param  cmd     Pointer to cmd,AT instructions.
  * @param  data    Pointer to recevice data.
  * @param  length  the data of length.
  *
  * @retval Status_Success：Return value is ok.
  * @retval Status_InvalidArgument：Return value is fail.
  */
status_t link_ReadBlocking_Esp8266_cmdCWJapResp(UART_Device_T *pUart, char *cmd, uint8_t *data, size_t length)
{
    ASSERT(data != NULL);
    ASSERT(pUart != NULL);

    uint32_t statusFlag = Status_Success;
    uint32_t recv_len = 0;
    uint32_t offset = 0;


    while (length--)
    {
        while (!HAL_UART_DataReady(pUart))
        {

        }
        *(data++) = (uint8_t)HAL_UART_GetChar(pUart);
        recv_len++;

        if (uart1_rxbuf[recv_len - 2] == 0x0D && uart1_rxbuf[recv_len - 1] == 0x0A)
        {

            /*if receive the data is the last, compare to result*/
            if ((recv_len - offset) <= 6 && (recv_len - offset > 2))
            {
                if (strstr((const char *)&uart1_rxbuf, (const char *)"FAIL\r\n"))
                {
                    statusFlag = Status_InvalidArgument;
                    break;
                }
                else
                {
                    if (strstr((const char *)&uart1_rxbuf, (const char *)"OK\r\n"))
                    {
                        statusFlag = Status_Success;
                        break;
                    }
                }
            }
            offset = recv_len;
        }

    }
    return statusFlag;
}


/**
  * @brief  esp8266 response instruction.
  *
  * @param  pUart   Pointer to UART device handler.
  * @param  cmd     Pointer to cmd,AT instructions.
  * @param  data    Pointer to recevice data.
  * @param  length  the data of length.
  *
  * @retval Status_Success：Return value is ok.
  * @retval Status_InvalidArgument：Return value is fail.
  */
status_t link_ReadBlocking_Esp8266_cmdResp(UART_Device_T *pUart, char *cmd, uint8_t *data, size_t length)
{
    ASSERT(data != NULL);
    ASSERT(pUart != NULL);

    uint32_t statusFlag = Status_Success;
    uint32_t recv_len = 0;
    uint32_t offset = 0;


    while (length--)
    {
        while (!HAL_UART_DataReady(pUart))
        {

        }
        *(data++) = (uint8_t)HAL_UART_GetChar(pUart);
        recv_len++;

        if (uart1_rxbuf[recv_len - 2] == 0x0D && uart1_rxbuf[recv_len - 1] == 0x0A)
        {

            /*if receive the data is the last, compare to result*/
            if ((recv_len - offset) <= 7 && (recv_len - offset > 2))
            {
                if (strstr((const char *)&uart1_rxbuf, (const char *)"ERROR\r\n"))
                {
                    statusFlag = Status_InvalidArgument;
                    break;
                }
                else
                {
                    if (strstr((const char *)&uart1_rxbuf, (const char *)"OK\r\n"))
                    {
                        statusFlag = Status_Success;
                        break;
                    }
                }
            }
            offset = recv_len;
        }

    }

    return statusFlag;
}




/*!
 * @brief Read RX cmd response data register using a blocking method.
 *
 * This function polls the RX register, waits for the RX register to be full or for RX FIFO to
 * have data, and reads data from the TX register.
 *
 * @param pUart UART device handle
 * @param outData Start address of the buffer to store the received data.
 * @param cmd     AT CMD
 * @param length Size of the buffer.
 * @param timeout waiting for receive time ,unit ms
 *
 * @retval Status_Fail Receiver overrun occurred while receiving data.
 * @retval Status_Success Successfully received all data.
 */
status_t AT_recv_cmdResp(UART_Device_T *pUart, char *cmd, uint8_t *pOutData, uint32_t outLen, uint32_t timeout)
{
    ASSERT(pOutData);
    ASSERT(cmd);

    status_t ret = Status_Success;

    /*waiting for receive one byte from wifi module*/
    ret = link_ReadBlocking_Esp8266_cmdResp(pUart, cmd, pOutData, outLen);
    if (ret != Status_Success)
    {
        ERROR_TRACE("err code:%d", ret);
    }

    return ret;

}

/*!
 * @brief Read RX cmd response data register using a blocking method.
 *
 * This function polls the RX register, waits for the RX register to be full or for RX FIFO to
 * have data, and reads data from the TX register.
 *
 * @param pUart UART device handle
 * @param cmd     AT CMD
 * @param pOutData Start address of the buffer to store the received data.
 * @param outLen Size of the buffer.
 * @param timeout waiting for receive time ,unit ms
 *
 * @retval Status_Fail Receiver overrun occurred while receiving data.
 * @retval Status_Success Successfully received all data.
 */
status_t AT_recv_cmdCWJapResp(UART_Device_T *pUart, char *cmd, uint8_t *pOutData, uint32_t outLen, uint32_t timeout)
{
    ASSERT(pOutData);
    ASSERT(cmd);

    status_t ret = Status_Success;

    /*waiting for receive one byte from wifi module*/
    ret = link_ReadBlocking_Esp8266_cmdCWJapResp(pUart, cmd, pOutData, outLen);
    if (ret != Status_Success)
    {
        ERROR_TRACE("err code:%d", ret);
    }

    return ret;

}

/*!
 * @brief Read RX cmd response data register using a blocking method.
 *
 * This function polls the RX register, waits for the RX register to be full or for RX FIFO to
 * have data, and reads data from the TX register.
 *
 * @param data Start address of the buffer to store the received data.
 * @param length Size of the buffer.
 * @param timeout waiting for receive time ,unit ms
 *
 * @retval Status_Fail Receiver overrun occurred while receiving data.
 * @retval Status_Success Successfully received all data.
 */
status_t link_ReadBlocking_Esp8266NetWorkData(UART_Device_T *pUart, uint8_t *data, size_t length, uint32_t recvLen)
{
    ASSERT(data);

    uint32_t statusFlag = Status_Success;
    uint32_t recv_len = 0;
    uint8_t  restart = 0;
    uint8_t  offset = 0;    //record ':' offset
    uint8_t  i = 0 ;
    uint32_t tmpLen = 0;
    uint8_t  tmpRxBuf = 0;

    while (length--)
    {
        while (!HAL_UART_DataReady(pUart))
        {
        }
        *(data++) = (uint8_t)HAL_UART_GetChar(pUart);
        recv_len++;
        if (recv_len >= 8)
        {
            /*data format \r\n+IPD,<LEN>:<DATA> or +IPD,<id>,<LEN>:<DATA>*/
            if (strstr((const char *)&uart1_rxbuf, (const char *)"\r\n+IPD,") && (restart == 0))
            {
                restart = 1;
            }
            if (restart)
            {
                if (strstr((const char *)&uart1_rxbuf, (const char *)":"))
                {
                    offset = recv_len - 8;
                    for (i = 0 ; i < offset ; i++)
                    {
                        // it must copy to tmpRxBuf, if not ,usmart_str2num  will be error
                        tmpRxBuf = uart1_rxbuf[6 + offset - i];
                        statusFlag = usmart_str2num(&tmpRxBuf, &recvLen);
                        if (statusFlag != Status_Success)
                        {
                            return statusFlag;
                        }
                        tmpLen += (recvLen * usmart_pow(10, i));
                    }
                    statusFlag = Status_Success;
                    recvLen = tmpLen;
                    break;
                }
            }
            length++; // you didn't know recv_len,  it stoped until find for ':',so lenght++
        }

    }
    recv_len = recvLen;
    while (recv_len--)
    {
        while (!HAL_UART_DataReady(pUart))
        {
        }
        *(data++) = (uint8_t)HAL_UART_GetChar(pUart);

    }
    recvLen = recvLen + 1 + 7 + 1; /*data format \r\n+IPD,<LEN>:<DATA> */
    statusFlag = Status_Success;
    return statusFlag;
}

/*!
 * brief Read RX network data register using a blocking method.
 *
 * This function polls the RX register, waits for the RX register to be full or for RX FIFO to
 * have data, and reads data from the TX register.
 *
 * @param outData Start address of the buffer to store the received data.
 * @param outLen Size of the buffer.
 * @param timeout waiting for receive time ,unit ms
 *
 * @retval Status_Fail Receiver overrun occurred while receiving data.
 * @retval Status_Success Successfully received all data.
 */
status_t AT_recv_NetWorkdata(UART_Device_T *pUart, uint8_t *pOutData, uint32_t outLen, uint32_t timeout, uint32_t recvlen)
{
    ASSERT(pOutData);

    status_t ret = Status_Success;
    //  uint32_t recvLen;

    /*waiting for receive one byte from wifi module*/
    ret = link_ReadBlocking_Esp8266NetWorkData(pUart, pOutData, outLen, recvlen);
    if (ret != Status_Success)
    {
        ERROR_TRACE("err code:%d", ret);
    }

    return ret;

}

/*!
 * @brief Read RX cmd response data register using a blocking method.
 *
 * This function polls the RX register, waits for the RX register to be full or for RX FIFO to
 * have data, and reads data from the TX register.
 *
 * @param pUart UART device handle
 * @param data Start address of the buffer to store the received data.
 * @param length Size of the buffer.
 *
 * @retval Status_Fail Receiver overrun occurred while receiving data.
 * @retval Status_Success Successfully received all data.
 */
status_t link_ReadBlocking_Esp8266DataResp(UART_Device_T *pUart, uint8_t *data, size_t length)
{
    ASSERT(data);

    uint32_t statusFlag = Status_Success;
    uint32_t recv_len = 0;

    while (length--)
    {
        while (!HAL_UART_DataReady(pUart))
        {
        }
        *(data++) = (uint8_t)HAL_UART_GetChar(pUart);
        recv_len++;
        if (recv_len >= 11)
        {
            if (strstr((const char *)&uart1_rxbuf, (const char *)"\r\nSEND OK\r\n"))
            {
                statusFlag = Status_Success;
                break;
            }

        }

    }

    return statusFlag;
}

/*!
 * brief receive from the response to sent data to AP
 *
 * This function polls the RX register, waits for the RX register to be full or for RX FIFO to
 * have data, and reads data from the TX register.
 *
 * @param pUart UART device handle
 * @param outData Start address of the buffer to store the received data.
 * @param length Size of the buffer.
 * @param timeout waiting for receive time ,unit ms
 * @retval Status_Fail Receiver overrun occurred while receiving data.
 * @retval Status_Success Successfully received all data.
 */
status_t AT_recv_dataResp(UART_Device_T *pUart, uint8_t *pOutData, uint32_t outLen, uint32_t timeout)
{
    ASSERT(pOutData);

    status_t ret = Status_Success;

    /*waiting for receive one byte from wifi module*/
    ret = link_ReadBlocking_Esp8266DataResp(pUart, pOutData, outLen);
    if (ret != Status_Success)
    {
        ERROR_TRACE("err code:%d", ret);
    }

    return ret;

}

/** Configure ESP8266 enable */
RET_CODE ESP8266_EnConfig(void)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;

    HAL_GPIO_PinWrite(GPIOB15, GPIO_PIN_SET);

    GPIO_InitStruct.pin = GPIO_PIN_15;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;

    ret = HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        ret = RET_ERROR;
    }
    HAL_GPIO_PinWrite(GPIOB15, GPIO_PIN_SET);
    return ret;
}

/** Configure ESP8266 enable */
RET_CODE ESP8266_ResetConfig(void)
{
    RET_CODE ret = RET_OK;

    HAL_GPIO_SetPinDirection(GPIOC5, GPIO_PIN_OUTPUT);
    HAL_GPIO_PinWrite(GPIOC5, GPIO_PIN_RESET);
    SysTick_DelayMS(500);
    HAL_GPIO_PinWrite(GPIOC5, GPIO_PIN_SET);

    return ret;
}

/** Configure pinmux for an ESP8266 transmission UART device */
RET_CODE ESP8266_UartPinMuxConfig(UART_Device_T *pUART)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;

    GPIO_InitStruct.pin = GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 2;

    ret = HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /** UART3 GPIO Configuration */
    GPIO_InitStruct.pin = GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 2;
    ret = HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
    return RET_OK;
}

/** Init uart port */
void at_uart_init(void)
{
    UART_Config_T config;
    UART_TransferConfig_T transConfig;

    /* Initialize uart device for printk port */
    config.baudRate = 115200;  //8M: 19200 ; 48M: 115200
    config.parity = UART_PARITY_NONE;
    config.wordLength = UART_DATABITS_8;
    config.stopBits = UART_STOPBITS_1;
    config.clockSrc = 48000000;
    config.callback = NULL;
    HAL_UART_Init(&g_ATuartHandle, AT_UART, &config);

    /* Data transfer configuration(enable tx fifo mode) */
    transConfig.enableFifo = FALSE;
    transConfig.enableRxDMA = FALSE;
    transConfig.enableTxDMA = FALSE;
    transConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    transConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    HAL_UART_TransferConfig(&g_ATuartHandle, &transConfig);

}


/**
* brief Writes to the UART1 TX register using a blocking method.
*
* note This function does not check whether all data is sent out to the bus.
*
*/
void at_send(char *fmt, ...)
{
    uint16_t length, i;
    va_list args;

    va_start(args, fmt);
    vsprintf((char *)uart1_txbuf, fmt, args);
    va_end(args);
    length = strlen((const char *)uart1_txbuf);
    /* This API can only ensure that the data is written into the data buffer but can't
    ensure all data in the data buffer are sent into the transmit shift buffer. */
    for (i = 0 ; i < length; i++)
    {
        HAL_UART_PutChar(AT_UART, uart1_txbuf[i]);
    }
}

