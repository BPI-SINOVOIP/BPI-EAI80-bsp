/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _FS_INTERFACE_H_
#define _FS_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_DEVRAM_FILESYSTEM
#include <device.h>
#endif



#define CONFIG_FILE_SYSTEM_DEVRAM 1



#if defined(CONFIG_FILE_SYSTEM_NFFS)
#define MAX_FILE_NAME 256
#elif defined(CONFIG_FILE_SYSTEM_DEVRAM)
#define MAX_FILE_NAME 256
#else
#define MAX_FILE_NAME 12
#endif




struct fs_mount_t;

/**
 * @brief File object representing an open file
 *
 * @param Pointer to FATFS file object structure
 * @param mp Pointer to mount point structure
 */
struct fs_file_t
{
    void *filep;
    const struct fs_mount_t *mp;
};

/**
 * @brief Directory object representing an open directory
 *
 * @param dirp Pointer to directory object structure
 * @param mp Pointer to mount point structure
 */
struct fs_dir_t
{
    void *dirp;
    const struct fs_mount_t *mp;
};


#ifdef CONFIG_DEVRAM_FILESYSTEM
struct file_operations_t
{
    int (*devctl)(struct fs_file_t *filp, int cmd, void *data);
    int (*open)(struct fs_file_t *filp);
};

struct devramfs_file_data
{
    void *usr_prv;
    struct file_operations_t *f_op;
    uint32_t offset;
};

#endif


#ifdef __cplusplus
}
#endif

#endif /* _FS_INTERFACE_H_ */
