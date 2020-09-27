/**
 * @file
 *
 * @brief Public APIs for the uncompress drivers.
 */

/*
 * Copyright (c) 2018 Alexander Wachter
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __LOAD_COMPRESS_IMAGE_H
#define __LOAD_COMPRESS_IMAGE_H

/**
 * @brief Uncompress Interface
 * @defgroup Uncompress CAN Interface
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <ugelis/types.h>
#include <device.h>
#include <string.h>


int ldimg_flash_compress_image(uint32_t image_flash_offset, uint8_t *uncompress);

int ldimg_uncompress_image(uint8_t *compress, uint32_t compress_len,
                           uint8_t *uncompress, uint32_t *uncompress_len);



#ifdef __cplusplus
}
#endif
/**
 * @}
 */


#endif /*__LOAD_COMPRESS_IMAGE_H*/
