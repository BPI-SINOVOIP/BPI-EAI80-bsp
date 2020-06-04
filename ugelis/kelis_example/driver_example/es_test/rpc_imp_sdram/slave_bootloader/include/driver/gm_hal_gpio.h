/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart.h
 *
 * @author        wen.liu
 *
 * @version       1.1.0
 *
 * @date          2016/06/13
 *
 * @brief         Header file of uart driver.
 *
 * @note
 *    2015/08/21, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _GM_HAL_GPIO_H_
#define _GM_HAL_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gm_common.h>
#include <gpio/gm_ll_gpio_dev.h>
#include <gpio/gm_ll_gpio.h>

/**
 * @brief  Initializes GPIO single pin with user specified attribute.
 *
 * @param pGPIO   GPIO base address.
 * @param pConfig Pointer to user-defined configuration.
 * @return RET_OK indicates success; Else indicates failure.
 */
int HAL_GPIO_Init(GPIO_Device_T *pGPIO, GPIO_PinConfig_T *pConfig);

/**
 * @brief  Deinitializes specific GPIO single pin to default state. Gpio interrupt enabled for this pin is also disabled.
 *
 * @param pGPIO      GPIO base address.
 * @param pin        Specifies the GPIO pins to be configured.This parameter can be any value of @ref GPIO_pins_define.
 * @param alternate  Peripheral to be connected to the selected pins.
 *                   This parameter can be a value of @ref GPIOEx_alternate_function_selection.
 * @return RET_OK indicates success; Else indicates failure.
 */
int HAL_GPIO_DeInit(GPIO_Device_T *pGPIO, uint16_t pin, uint32_t alternate);

/**
 * @brief  Set or clear specified pin.
 *
 * This function allows atomic modification of GPIO level.
 *
 * @param  pin    Specifies the pin bit mask address. This parameter can be a value of @ref GPIO_bit_addr.
 * @param  state  Specifies expected level of the selected bit. It is low level if this parameter is zero.
 * @retval None
 */
void HAL_GPIO_PinWrite(GPIO_Pin_T pin, GPIO_PinState_T state);

/**
 * @brief  Read the specified port pin.
 *
 * @param  pin  Specifies the pin bit mask address. This parameter can be a value of @ref GPIO_bit_addr.
 * @retval      Level of the specified port pin. It is zero if the pin is at low level.
 */
GPIO_PinState_T HAL_GPIO_PinRead(GPIO_Pin_T pin);

/**
 * @brief  Toggle the specified port pin.
 *
 * @param  pin  Specifies the pin bit mask address. This parameter can be a value of @ref GPIO_bit_addr.
 * @retval None
 */
void HAL_GPIO_PinToggle(GPIO_Pin_T pin);

/**
 * @brief  Sets the specified port pin direction.
 *
 * @param  pin  Specifies the pin bit mask address. This parameter can be a value of @ref GPIO_bit_addr.
 * @param  direction  Specifies the pin direction.
 * @retval None
 */
void HAL_GPIO_SetPinDirection(GPIO_Pin_T pin, GPIO_PinDir_T direction);

/**
 * @brief  Sets the specified port pin glitch filter.
 *
 * @param  pin  Specifies the pin bit mask address. This parameter can be a value of @ref GPIO_bit_addr.
 * @param  pFilter  config input port pin filter.
 *
 * @return RET_OK indicates success; Else indicates failure.
 */
int HAL_GPIO_SetPinFilter(GPIO_Pin_T pin, GPIO_InputGlitchFilter_T *pFilter);

/**
  * @brief  Checks whether the specified EXTI line is asserted or not.
  * @param pGPIO   GPIO base address.
  * @param  __EXTI_LINE__: specifies the EXTI line to check.
  *          This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval The new state of __EXTI_LINE__ (SET or RESET).
  */
#define HAL_GPIO_EXTI_GET_IT(pGPIO, __EXTI_LINE__) (pGPIO->ISFR & (__EXTI_LINE__))

/**
  * @brief  Clears the EXTI's line pending bits.
  * @param pGPIO   GPIO base address.
  * @param  __EXTI_LINE__: specifies the EXTI lines to clear.
  *          This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#define HAL_GPIO_EXTI_CLEAR_IT(pGPIO, __EXTI_LINE__) (pGPIO->ISFR = (__EXTI_LINE__))

/**
  * @brief  Enable the gpio's irq line  bits.
  * @param pGPIO   GPIO base address.
  * @param  _PIN_ENABLE__: specifies the gpio lines to enable.
  *          This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#define HAL_GPIO_IRQ_ENABLE(pGPIO, _PIN_ENABLE__) (pGPIO->IER |= (_PIN_ENABLE__))

/**
  * @brief  Disable the gpio's irq line  bits.
  * @param pGPIO   GPIO base address.
  * @param  _PIN_DISABLE__: specifies the gpio irq lines to disable.
  *          This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#define HAL_GPIO_IRQ_DISABLE(pGPIO, _PIN_DISABLE__) (pGPIO->IER &= ~(_PIN_DISABLE__))


#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GM_HAL_GPIO_H_ */



