#include <ugelis.h>
#include <device.h>
#include <uart.h>
#include <board.h>
#include <serial/uart_gm.h>


#define PRINT_UART_NAME "UART_2" //index start from 0
#define UART_A "UART_0"
#define UART_B "UART_1"

static const char *poll_data = "This is a POLL test.\r\n";


#define UART_USERBUFFER_SIZE     32           /**< Specify user buffer size */

static char g_tx_data[UART_USERBUFFER_SIZE];
static char g_rx_data[UART_USERBUFFER_SIZE];
int g_tx_num = 0;
int g_rx_num = 0;

unsigned char recv_char;
unsigned char sent_char;


static void SetUserBuffer(void)
{
    int i;

    g_tx_num = 0;
    g_rx_num = 0;
    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        g_rx_data[i] = 0xFF;
        g_tx_data[i] = (i & 0x1FF);
    }
}

static int t_irq_count = 0;
static int r_irq_count = 0;
void uart_transmit_callback(struct device *dev)
{
    if (g_tx_num < UART_USERBUFFER_SIZE)
    {
        uart_poll_out(dev, g_tx_data[g_tx_num++]);
    }
    else
    {
        uart_irq_tx_disable(dev);
    }
    t_irq_count++;
}

void uart_receive_callback(struct device *dev)
{
    uart_poll_in(dev, &recv_char);
    g_rx_data[g_rx_num++] = recv_char;
    r_irq_count++;
}

static int test_poll_in(void)
{
    struct device *uart_dev = device_get_binding(PRINT_UART_NAME);
    if (!uart_dev)
    {
        printk("Cannot get UART device\n");
        return RET_ERROR;
    }
    printk("Please send characters to serial console:\n");

    while (1)
    {
        uart_poll_in(uart_dev, &recv_char);
        printk("%c", recv_char);
        if ((recv_char == '\n') || (recv_char == '\r'))
        {
            break;
        }
    }

    printk("\ntest_poll_in ok.\n");
    return RET_OK;
}

static int test_poll_out(void)
{
    int i;
    struct device *uart_dev = device_get_binding(PRINT_UART_NAME);
    if (!uart_dev)
    {
        printk("Cannot get UART device\n");
        return RET_ERROR;
    }

    for (i = 0; i < strlen(poll_data); i++)
    {
        sent_char = uart_poll_out(uart_dev, poll_data[i]);
        if (sent_char != poll_data[i])
        {
            printk("expect send %c, actaul send %c\n", poll_data[i], sent_char);
            return RET_ERROR;
        }
    }

    printk("test_poll_out ok.\n");
    return RET_OK;
}

static int test_uart_irq_and_poll(void)
{
    int i = 0;
    SetUserBuffer();

    struct device *uart_dev = device_get_binding(UART_B);
    uart_irq_callback_set(uart_dev, uart_receive_callback);
    uart_irq_rx_enable(uart_dev);

    struct device *uart_dev2 = device_get_binding(UART_A);
    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        sent_char = uart_poll_out(uart_dev2, g_tx_data[i]);
        if (sent_char != g_tx_data[i])
        {
            printk("expect send %c, actaul send %c\n", g_tx_data[i], sent_char);
            return RET_ERROR;
        }

        //fixed: test failed in FLASH exported to GreeIDE, becoz it run very slowly in FLASH
        //without this line, it will occur overrun error!
        k_sleep(1);
    }

    k_sleep(1000);

    uart_irq_rx_disable(uart_dev);
    uart_irq_callback_set(uart_dev, NULL);

    int tag = 1;
    if (g_rx_num != UART_USERBUFFER_SIZE)
    {
        tag = 0;
    }

    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        if (g_rx_data[i] != g_tx_data[i])
        {
            tag = 0;
            break;
        }
    }

    if (tag)
    {
        printk("test_uart_irq_and_poll ok.\n");
        return RET_OK;
    }
    else
    {
        printk("test_uart_irq_and_poll failed.\n");
        return RET_ERROR;
    }
}

static int test_uart_irq_and_irq(void)
{
    int i = 0;
    SetUserBuffer();

    struct device *uart_dev = device_get_binding(UART_A);
    uart_irq_callback_set(uart_dev, uart_receive_callback);
    uart_irq_rx_enable(uart_dev);

    struct device *uart_dev2 = device_get_binding(UART_B);
    uart_irq_callback_set(uart_dev2, uart_transmit_callback);
    uart_irq_tx_enable(uart_dev2);

    //Let's start the mission: start to transmite data
    uart_poll_out(uart_dev2, g_tx_data[g_tx_num++]);

    k_sleep(1000);

    uart_irq_rx_disable(uart_dev);
    uart_irq_tx_disable(uart_dev2);
    uart_irq_callback_set(uart_dev, NULL);
    uart_irq_callback_set(uart_dev2, NULL);

    int tag = 1;
    if (g_rx_num != UART_USERBUFFER_SIZE)
    {
        tag = 0;
    }

    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        if (g_rx_data[i] != g_tx_data[i])
        {
            tag = 0;
            break;
        }
    }

    if (tag)
    {
        printk("test_uart_irq_and_irq ok.\n");
        return RET_OK;
    }
    else
    {
        printk("test_uart_irq_and_irq failed.\n");
        return RET_ERROR;
    }
}


void main(void)
{
    //choose UART to print in board/arm/gm6721_evb_*/gm6721_evb_*.dts
    printk("test UARTx printk.\n");

    //these 2 testcases can use in UART0, UART1 and UART2
    test_poll_out();
    //test_poll_in();

    //need to connect UART_A and UART_B
    //test_uart_irq_and_poll();
    //test_uart_irq_and_irq();
}

