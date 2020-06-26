/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2016 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "BlinkLed.h"

// ----------------------------------------------------------------------------

void
blink_led_init()
{
  // Turn on clock for port module
  SIM->SCGC5 |= BLINK_SCGC5_MASKx(BLINK_PORT_NUMBER);

  // Set the pin multiplexer to GPIO mode
  BLINK_PORTx(BLINK_PORT_NUMBER)->PCR[BLINK_PIN_NUMBER] = PORT_PCR_MUX(1);

  // Set the pin as output
  BLINK_GPIOx(BLINK_PORT_NUMBER)->PDDR |= BLINK_PIN_MASK(BLINK_PIN_NUMBER);

  // Start with led turned off
  blink_led_off();
}

// ----------------------------------------------------------------------------
