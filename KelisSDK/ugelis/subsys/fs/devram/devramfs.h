#ifndef _DEVRAMFS_H_
#define _DEVRAMFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel.h>
#include <device.h>
#include <fs.h>
#include <fs/fs_interface.h>


#define DEVRAMFS_MOUNT_POINT                            CONFIG_DEVRAMFS_MOUNT_POINT
#define DEVRAMFS_ROOT_DETRY_NAME                        CONFIG_DEVRAMFS_ROOT_DETRY_NAME
#define DEVRAMFS_MAX_PER_DENTRY_NAME_SIZE               CONFIG_DEVRAMFS_MAX_PER_DENTRY_NAME_SIZE
#define DEVRAMFS_MAX_INODE_NUM                          CONFIG_DEVRAMFS_MAX_INODE_NUM
#define DEVRAMFS_MAX_DETRY_NUM                          CONFIG_DEVRAMFS_MAX_DETRY_NUM
#define DEVRAMFS_MAX_FILE_PRIVATE_DATA                  CONFIG_DEVRAMFS_MAX_DETRY_NUM
#define DEVRAMFS_MAX_SECTION_PER_PATH                   CONFIG_DEVRAMFS_MAX_SECTION_PER_PATH



#define devramfs_malloc(size)                           malloc_ext(0, size)


struct devramfs_inode
{
    uint32_t size;                              /*file or dentry size*/
    struct file_operations_t *f_op;
};

struct devramfs_dentry
{
    sys_dlist_t as_parent_node;
    sys_dnode_t as_child_node;
    enum fs_dir_entry_type type;
    char name[DEVRAMFS_MAX_PER_DENTRY_NAME_SIZE + 1];
    uint8_t dentry_depth;
    uint8_t name_length;
    struct devramfs_inode *inode;
};

struct qstr
{
    const char *name;
    uint8_t length;
};

struct devramfs_section_dentry
{
    uint8_t depth ;
    struct qstr qstr_table[DEVRAMFS_MAX_SECTION_PER_PATH];
    /**
     * cache the last dentry that exited in the abspath dentry section
     * to see the detail usage for
     * function:devramfs_lookup
     */
    struct devramfs_dentry *devramfs_dentry_cache;
};


#ifdef __cplusplus
}
#endif

#endif /* _DEVRAMFS_H_ */


