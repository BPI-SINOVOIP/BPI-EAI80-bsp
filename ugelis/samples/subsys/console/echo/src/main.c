/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <ugelis.h>
#include <console.h>

void main(void)
{
	console_init();

	printk("Start typing characters to see them echoed back\n");

	while (1) {
		u8_t c = console_getchar();

		console_putchar(c);
		if (c == '\r') {
			console_putchar('\n');
		}
	}
}
