#include <ugelis.h>
#include <device.h>
#include <ztest.h>
#include <uart.h>
#include <board.h>


//#define UART_DEVICE_NAME "UART_0"
#define UART_DEVICE_NAME "UART_1"
//#define UART_DEVICE_NAME "UART_2"
static const char *poll_data = "This is a POLL test.\r\n";


#define UART_USERBUFFER_SIZE 32           /**< Specify user buffer size */
static char g_tx_data[UART_USERBUFFER_SIZE];
static char g_rx_data[UART_USERBUFFER_SIZE];
int g_tx_num = 0;
int g_rx_num = 0;

static void SetUserBuffer(void)
{
    uint32_t i;

    g_tx_num = 0;
    g_rx_num = 0;
    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        g_rx_data[i] = 0xFF;
        g_tx_data[i] = (i & 0x1FF);
    }
}

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
}

void uart_receive_callback(struct device *dev)
{
    unsigned char recv_char;
    uart_poll_in(dev, &recv_char);
    g_rx_data[g_rx_num++] = recv_char;
}

static int test_uart_irq_and_poll(void)
{
    int i = 0;
    int sent_char;

    SetUserBuffer();

    struct device *uart_dev = device_get_binding("UART_3");
    uart_irq_callback_set(uart_dev, uart_receive_callback);
    uart_irq_rx_disable(uart_dev);
    uart_irq_rx_enable(uart_dev);

    struct device *uart_dev2 = device_get_binding("UART_6");
    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        sent_char = uart_poll_out(uart_dev2, g_tx_data[i]);
        if (sent_char != g_tx_data[i])
        {
            TC_PRINT("expect send %c, actaul send %c\n", g_tx_data[i], sent_char);
            return TC_FAIL;
        }
    }

    k_sleep(2000);
    uart_irq_rx_disable(uart_dev);
    if (g_rx_num != UART_USERBUFFER_SIZE)
    {
        return TC_FAIL;
    }

    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        if (g_rx_data[i] != g_tx_data[i])
        {
            return TC_FAIL;
        }
    }

    TC_PRINT("test_uart_irq_and_poll ok.\n");
    return TC_PASS;
}

static int test_uart_irq_and_irq(void)
{
    int i = 0;
    int sent_char;

    SetUserBuffer();

    struct device *uart_dev = device_get_binding("UART_3");
    uart_irq_callback_set(uart_dev, uart_receive_callback);
    uart_irq_rx_disable(uart_dev);
    uart_irq_rx_enable(uart_dev);

    struct device *uart_dev2 = device_get_binding("UART_6");
    uart_irq_callback_set(uart_dev2, uart_transmit_callback);
    uart_irq_tx_disable(uart_dev2);
    uart_irq_tx_enable(uart_dev2);

    //start send data
    if (uart_irq_tx_ready(uart_dev2))
    {
        uart_poll_out(uart_dev2, g_tx_data[g_tx_num++]);
    }

    k_sleep(3000);

    uart_irq_rx_disable(uart_dev);
    if (g_rx_num != UART_USERBUFFER_SIZE)
    {
        return TC_FAIL;
    }

    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        if (g_rx_data[i] != g_tx_data[i])
        {
            return TC_FAIL;
        }
    }

    TC_PRINT("test_uart_irq_and_irq ok.\n");
    return TC_PASS;
}

static int test_poll_in(void)
{
    unsigned char recv_char;
    struct device *uart_dev = device_get_binding(UART_DEVICE_NAME);
    if (!uart_dev)
    {
        TC_PRINT("Cannot get UART device\n");
        return TC_FAIL;
    }
    TC_PRINT("Please send characters to serial console:\n");

    while (1)
    {
        uart_poll_in(uart_dev, &recv_char);
        TC_PRINT("%c", recv_char);
        g_rx_data[g_rx_num++] = recv_char;
        if ((recv_char == '\n') || (recv_char == '\r'))
        {
            break;
        }
    }

    TC_PRINT("\ntest_poll_in ok.\n");
    return TC_PASS;
}

static int test_poll_out(void)
{
    int i;
    unsigned char sent_char;
    struct device *uart_dev = device_get_binding(UART_DEVICE_NAME);
    if (!uart_dev)
    {
        TC_PRINT("Cannot get UART device\n");
        return TC_FAIL;
    }

    for (i = 0; i < strlen(poll_data); i++)
    {
        sent_char = uart_poll_out(uart_dev, poll_data[i]);
        if (sent_char != poll_data[i])
        {
            TC_PRINT("expect send %c, actaul send %c\n", poll_data[i], sent_char);
            return TC_FAIL;
        }
    }

    TC_PRINT("test_poll_out ok.\n");
    return TC_PASS;
}

void gm_test_uart(void)
{
    //these 2 testcases can use in UART0, UART1 and UART2
    zassert_true(test_poll_out() == TC_PASS, NULL);
    zassert_true(test_poll_in() == TC_PASS, NULL);

    //need to connect UART0 and UART1
    zassert_true(test_uart_irq_and_poll() == TC_PASS, NULL);
    zassert_true(test_uart_irq_and_irq() == TC_PASS, NULL);
}
