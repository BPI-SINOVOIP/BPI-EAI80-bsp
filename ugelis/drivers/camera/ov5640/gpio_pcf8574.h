#ifndef __GPIO_PCF8574_H
#define __GPIO_PCF8574_H

#ifdef __cplusplus
extern "C" {
#endif

uint8_t pcf8574_init(void);
void pcf8574_write(uint8_t bit, uint8_t status);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_PCF8574_H */
