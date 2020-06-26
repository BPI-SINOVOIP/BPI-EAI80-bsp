/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <ugelis.h>
#include <misc/printk.h>
#include <fs.h>
#include <fs/fs_interface.h>
#include <device.h>


#define WRITE_ATTR_FILE 0
#define READ_ATTR_FILE 1
#define RAMFS_TEST_FILE "/dev/test/rw_attr"
#define RAMFS_TEST_FILE2 "/dev/test/rw_attr/ttt"


#define RAMFS_ATTR_NODE_DEBUG 1
#define RAMFS_ATTR_NODE_ERR   1


#ifndef RAMFS_ATTR_NODE_DEBUG
    #define ramfs_debug(...)      do {} while (0)
#else
    #define ramfs_debug           printk
#endif

#ifndef RAMFS_ATTR_NODE_ERR
    #define ramfs_err(...)   do {} while (0)
#else
    #define ramfs_err     printk
#endif


static int  test_file_op(struct fs_file_t *filp, int cmd, void *data);
static int  test_file_open(struct fs_file_t *filp);


static struct file_operations_t f_op =
{
    .open   = test_file_open,
    .devctl = test_file_op,
};

static int  test_file_op(struct fs_file_t *filp, int cmd, void *data)
{
    int ret = 0;
    struct devramfs_file_data *pfile_data = NULL;
    uint32_t usr_paramter;

    /*get user private paramter and printf it*/
    pfile_data = (struct devramfs_file_data *)filp->filep;
    usr_paramter = *((uint32_t *)pfile_data->usr_prv);

    if (cmd == READ_ATTR_FILE)
    {
        ramfs_debug("###do read file action,usr private parmeter:0x%08x###\n", usr_paramter);
    }
    else if (cmd == WRITE_ATTR_FILE)
    {
        ramfs_debug("###do write file action,usr private parmeter:0x%08x###\n", usr_paramter);
    }
    else
    {
        ret = -EINVAL;
    }

    return ret;
}


static int  test_file_open(struct fs_file_t *filp)
{
    int ret = 0;
    uint32_t *usr_private_data = NULL;
    struct devramfs_file_data *pfile_data = NULL;

    usr_private_data = k_malloc(sizeof(uint32_t));
    if (!usr_private_data)
    {
        return -ENOMEM;
    }
    *usr_private_data = 0xaabbccdd;


    pfile_data = (struct devramfs_file_data *)filp->filep;
    if (!pfile_data)
    {
        return -ENOMEM;
    }

    pfile_data->usr_prv = (void *)usr_private_data;

    return ret;
}


int main(void)
{
    int ret = 0;
    struct fs_file_t fp;

    ret = fs_mkfile(RAMFS_TEST_FILE, &f_op);
    if (ret)
    {
        ramfs_err("Failed to fs_mkfile err:%d\n", ret);
    }

    ret = fs_open(&fp, RAMFS_TEST_FILE);
    if (ret)
    {
        ramfs_err("Failed to open %s \n", RAMFS_TEST_FILE);
        return ret;
    }

    ret = fs_ioctl(&fp, WRITE_ATTR_FILE, NULL);
    if (ret)
    {
        ramfs_err("Failed to fs_ioctl cmd:WRITE_ATTR_FILE err:(%d)\n", ret);
        return ret;
    }

    ret = fs_ioctl(&fp, READ_ATTR_FILE, NULL);
    if (ret)
    {
        ramfs_err("Failed to fs_ioctl cmd:READ_ATTR_FILE err:(%d)\n", ret);
        return ret;
    }

    ramfs_debug("!!!devramfs test ok\n");
    return 0;

}
