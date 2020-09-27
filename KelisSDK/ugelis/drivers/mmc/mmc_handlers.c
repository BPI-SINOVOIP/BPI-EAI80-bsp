/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <mmc.h>
#include <syscall_handler.h>

Z_SYSCALL_HANDLER(mmc_in_slot, dev)
{
	return _impl_mmc_in_slot((struct device *)dev);
}

Z_SYSCALL_HANDLER(mmc_set_callback, dev,cb)
{
	return _impl_mmc_set_callback((struct device *)dev, cb);
}

Z_SYSCALL_HANDLER(mmc_read, dev,sector_addr,sector_nr,buf)
{
	return _impl_mmc_read((struct device *)dev, sector_addr, sector_nr, (uint8_t *)buf);
}

Z_SYSCALL_HANDLER(mmc_write, dev,sector_addr,sector_nr,buf)
{
	return _impl_mmc_write((struct device *)dev,sector_addr, sector_nr, (uint8_t *)buf);
}


Z_SYSCALL_HANDLER(mmc_get_capacity, dev)
{
	return _impl_mmc_get_capacity((struct device *)dev);
}




