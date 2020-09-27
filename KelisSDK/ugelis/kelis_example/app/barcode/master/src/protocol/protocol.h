#ifndef _UART_PROTOCOL_H__
#define _UART_PROTOCOL_H__

void protocol_init(void);
RET_CODE protocol_send_heartbeat(uint8_t wait_feedback);
RET_CODE protocol_send_barcode(uint8_t *data, uint32_t data_len);

#endif