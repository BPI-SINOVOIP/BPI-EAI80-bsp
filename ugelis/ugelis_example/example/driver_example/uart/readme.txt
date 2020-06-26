Overview:
the uart_example application is a simple test demonstration program based on the driver Layer.
It will  the following functions:
    (1). normal print string with uart2;
    (2). communication among multi uarts with polling.
    (3). communication among multi uarts with irq;

Run the example:
1. Download *.bin, which is running gm_test_uart example into the G6721_evb_176pin board.
   The log infomation will be printed by the uart2(0x40006000) in the G6721_evb_176pin.

2. The gm_test_uart need to connect uart0 and uart1 with dupont line like this:
   uart0 tx --- uart1 rx
   uart0 rx --- uart1 tx
