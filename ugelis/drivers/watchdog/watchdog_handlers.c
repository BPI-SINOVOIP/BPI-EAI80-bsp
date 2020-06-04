/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <watchdog.h>
#include <syscall_handler.h>

Z_SYSCALL_HANDLER(wdt_enable, dev)
{
	Z_OOPS(Z_SYSCALL_DRIVER_WDG(dev, enable));
	_impl_wdt_enable((struct device *)dev);
	return 0;
}

Z_SYSCALL_HANDLER(wdt_disable, dev)
{
	Z_OOPS(Z_SYSCALL_DRIVER_WDG(dev, disable));
	_impl_wdt_disable((struct device *)dev);
	return 0;
}

Z_SYSCALL_HANDLER(wdt_get_config, dev, config_p)
{
	const struct wdt_driver_api *config = (const struct wdt_driver_api *)config_p;

	Z_OOPS(Z_SYSCALL_MEMORY_READ(config, sizeof(*config)));
	Z_OOPS(Z_SYSCALL_DRIVER_WDG(dev, get_config));
	return _impl_wdt_get_config((struct device *)dev, config);
}

Z_SYSCALL_HANDLER(wdt_set_config, dev, config_p)
{
	const struct wdt_driver_api *config = (const struct wdt_driver_api *)config_p;

	Z_OOPS(Z_SYSCALL_MEMORY_WRITE(config, sizeof(*config)));
	Z_OOPS(Z_SYSCALL_DRIVER_WDG(dev, set_config));
	return _impl_wdt_set_config((struct device *)dev, config);
}

Z_SYSCALL_HANDLER(wdt_reload, dev)
{
	Z_OOPS(Z_SYSCALL_DRIVER_WDG(dev, reload));
	_impl_wdt_reload((struct device *)dev);
	return 0;
}