#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <stddef.h>
#ifdef __USE_KEIL_
    #include <errno_ex.h>
#else
    #include <errno.h>
#endif

#include "devramfs.h"


/* Memory pool for devramfs inode objects */
K_MEM_SLAB_DEFINE(devramfs_inode_pool, sizeof(struct devramfs_inode),
                  DEVRAMFS_MAX_INODE_NUM, 4);

/* Memory pool for devramfs dentry objects */
K_MEM_SLAB_DEFINE(devramfs_dentry_pool, sizeof(struct devramfs_dentry),
                  DEVRAMFS_MAX_DETRY_NUM, 4);

/* Memory pool for devramfs private file data objects */
K_MEM_SLAB_DEFINE(devramfs_file_private_data_pool, sizeof(struct devramfs_file_data),
                  DEVRAMFS_MAX_FILE_PRIVATE_DATA, 4);


static struct devramfs_dentry *root_dentry = NULL;
static struct devramfs_section_dentry section_dentry_table;
/* mounting info */
static struct fs_mount_t devramfs_mnt =
{
    .type       = FS_DEVRAMFS,
    .mnt_point  = DEVRAMFS_MOUNT_POINT,
};

/* File system interface */
static inline  struct devramfs_section_dentry *devramfs_get_dentry_section_table()
{
    return &section_dentry_table;
}

static inline struct devramfs_dentry *devramfs_get_root_dentry()
{
    return root_dentry;
}

static inline void devramfs_set_root_dentry(struct devramfs_dentry *dentry)
{
    root_dentry = dentry;
}



static int devramfs_init(struct device *dev);
static int devramfs_creat_root();
static int devramfs_close(struct fs_file_t *filp);
static int devramfs_closedir(struct fs_dir_t *dirp);
static struct devramfs_dentry *devramfs_alloc_dentry();
static struct devramfs_inode *devramfs_alloc_inode();

static int devramfs_ioctl(struct fs_file_t *filp, int cmd, void *data);
static int devramfs_mount(struct fs_mount_t *mountp);
static struct devramfs_file_data *devramfs_alloc_file_private_data();
static int devramfs_mkfile(struct fs_mount_t *mountp, const char *fs_path, void *data);
static int devramfs_opendir(struct fs_dir_t *dirp, const char *fs_path);
static int devramfs_readdir(struct fs_dir_t *dirp, struct fs_dirent *entry);
static int devramfs_mkdir(struct fs_mount_t *mountp, const char *name);
static int devramfs_open(struct fs_file_t *filp, const char *fs_path);
static struct devramfs_dentry *devramfs_lookup(const char *abs_path, uint8_t *vaild_depth);
static int devramfs_stat(struct fs_mount_t *mountp, const char *path, struct fs_dirent *entry);
static int devramfs_statvfs(struct fs_mount_t *mountp, const char *path, struct fs_statvfs *stat);
static struct devramfs_dentry *__devramfs_lookup(struct devramfs_dentry *parentdir, struct qstr *pqstr);
static struct devramfs_section_dentry *devramfs_convert_abspath_to_dentry_section(const char *abs_path);
static int devramfs_creat(const char *fs_path, enum fs_dir_entry_type type, struct file_operations_t *f_op);
static struct devramfs_dentry *__devramfs_creat(struct devramfs_dentry *parent_dentry, struct qstr *pqstr,
        enum fs_dir_entry_type type, struct file_operations_t *f_op);


static struct fs_file_system_t devram_fs =
{
    .open       = devramfs_open,
    .ioctl      = devramfs_ioctl,
    .close      = devramfs_close,
    .opendir    = devramfs_opendir,
    .readdir    = devramfs_readdir,
    .closedir   = devramfs_closedir,
    .mount      = devramfs_mount,
    .mkdir      = devramfs_mkdir,
    .mkfile     = devramfs_mkfile,
    .stat       = devramfs_stat,
    .statvfs    = devramfs_statvfs,
};

static struct devramfs_dentry *__devramfs_lookup(struct devramfs_dentry *parentdir, struct qstr *pqstr)
{
    sys_dlist_t *hlist = (sys_dlist_t *)(&(parentdir->as_parent_node));
    struct devramfs_dentry *child_dentry = NULL;
    struct devramfs_dentry *lookdentry = NULL;
    const char *lookup_name = pqstr->name;
    uint8_t lookup_length = pqstr->length;

    /* Check if mount point already exists */
    SYS_DLIST_FOR_EACH_CONTAINER(hlist, child_dentry, as_child_node)
    {
        /*check dentry name is match*/
        if ((child_dentry->name_length == lookup_length) && \
                (strncmp(child_dentry->name, lookup_name, lookup_length) == 0))
        {
            lookdentry =  child_dentry;
            break;
        }
    }

    return lookdentry;
}

/*
 * input: abspath = /devramfs/abc/def
 * out:   section_dentry_name=devramfs
 *        section_dentry_name=abc
 *        section_dentry_name=def
 *        depth = 3
*/
static struct devramfs_section_dentry *devramfs_convert_abspath_to_dentry_section(const char *abs_path)
{
    uint8_t depth = 0;
    uint8_t index = 1;
    uint8_t pre_slash_index = 0;
    bool    end_with_slash;
    uint8_t total_length = strlen(abs_path);

    struct devramfs_section_dentry  *section_dentry_table = devramfs_get_dentry_section_table();

    if (abs_path[total_length - 1] == '/')
    {
        end_with_slash = true;
    }
    else
    {
        end_with_slash = false;
    }

    /*parse abs_path and fill to dentry_section*/
    while (index < total_length)
    {
        if (abs_path[index] == '/')
        {
            depth++;
            section_dentry_table->qstr_table[depth - 1].name   = &abs_path[pre_slash_index + 1];
            section_dentry_table->qstr_table[depth - 1].length = index - pre_slash_index - 1;
            pre_slash_index = index;
        }
        index++;
    }

    if (end_with_slash == false)
    {
        /*   path=/dev/ab/cd  */
        depth++;
        section_dentry_table->qstr_table[depth - 1].name = &abs_path[pre_slash_index + 1];
        section_dentry_table->qstr_table[depth - 1].length = index - pre_slash_index - 1;
    }

    /*check depth is overflower*/
    if (depth > DEVRAMFS_MAX_SECTION_PER_PATH)
    {
        return NULL;
    }

    section_dentry_table->depth = depth;

    return section_dentry_table;
}


static struct devramfs_dentry *devramfs_lookup(const char *abs_path, uint8_t *vaild_depth)
{
    struct devramfs_dentry *parent_dentry = NULL;
    struct devramfs_dentry *current_dentry = NULL;
    struct devramfs_dentry *devramfs_dentry_cache = NULL;
    struct devramfs_section_dentry *section_dentry_table = NULL;
    struct qstr *pserch_qstr = NULL;
    uint8_t i = 0;

    if (abs_path[0] != '/')
    {
        return NULL;
    }

    parent_dentry = devramfs_get_root_dentry();
    section_dentry_table = devramfs_convert_abspath_to_dentry_section(abs_path);
    if (!section_dentry_table)
    {
        goto out;
    }

    /*check root dentry is exit*/
    pserch_qstr = &(section_dentry_table->qstr_table[0]);

    if (!(parent_dentry->name_length == pserch_qstr->length) && \
            (strncmp(pserch_qstr->name, parent_dentry->name, pserch_qstr->length) == 0))
    {
        goto out;
    }


    current_dentry = parent_dentry;
    devramfs_dentry_cache = current_dentry;
    /*bypass root entry and find the abs path from ramdevfs*/
    for (i = 1; i < section_dentry_table->depth; i++)
    {
        pserch_qstr = &(section_dentry_table->qstr_table[i]);

        current_dentry = __devramfs_lookup(parent_dentry, pserch_qstr);

        if (!current_dentry)
        {
            break;
        }
        parent_dentry = current_dentry;
        devramfs_dentry_cache = current_dentry;
    }
out:

    *vaild_depth = i;
    /*save the last entry for current lookup*/
    section_dentry_table->devramfs_dentry_cache = devramfs_dentry_cache;

    return current_dentry;
}


/* File operations */
static int devramfs_creat(const char *fs_path, enum fs_dir_entry_type type, struct file_operations_t *f_op)
{
    int ret = 0;
    uint8_t length = 0;
    struct devramfs_dentry *dentry = NULL;
    uint8_t abs_total_depth;
    uint8_t vaild_depth;
    uint8_t creat_entry_index = 0;
    struct devramfs_dentry *parent_dentry = NULL;
    struct devramfs_dentry *child_dentry = NULL;
    struct qstr *child_qstr;
    struct devramfs_section_dentry  *section_dentry_table = NULL;
    enum fs_dir_entry_type creat_temp_type;

    length = strlen(fs_path);
    if ((FS_DIR_ENTRY_FILE == type) && (fs_path[length - 1] == '/'))
    {
        return -EINVAL;
    }

    dentry = devramfs_lookup(fs_path, &vaild_depth);
    if (dentry)
    {
        /*file or dir is exist*/
        ret = -EEXIST;
        goto out;
    }

    /*mk for the left entry */

    /*get the last exit entry*/
    section_dentry_table = devramfs_get_dentry_section_table();
    abs_total_depth = section_dentry_table->depth;
    parent_dentry = section_dentry_table->devramfs_dentry_cache;

    /*get the parent of the dentry for start creat */
    for (creat_entry_index = vaild_depth ; creat_entry_index < abs_total_depth; creat_entry_index++)
    {
        /*prepare for child qstr */
        child_qstr = &(section_dentry_table->qstr_table[creat_entry_index]);
        /*set last entry for file attr if we need creat file type*/
        if ((type == FS_DIR_ENTRY_FILE) && (creat_entry_index == abs_total_depth - 1))
        {
            creat_temp_type = FS_DIR_ENTRY_FILE;
        }
        else
        {
            creat_temp_type = FS_DIR_ENTRY_DIR;
        }

        child_dentry = __devramfs_creat(parent_dentry, child_qstr, creat_temp_type, f_op);
        if (!child_dentry)
        {
            ret = -ENOMEM;
            goto out;
        }
        else
        {
            parent_dentry = child_dentry;
        }
    }

out:
    return ret;
}


static struct devramfs_dentry *__devramfs_creat(struct devramfs_dentry *parent_dentry, struct qstr *pqstr,
        enum fs_dir_entry_type type, struct file_operations_t *f_op)
{
    /*alloc dentry*/
    struct devramfs_dentry *dentry = NULL;
    struct devramfs_inode  *inode = NULL;

    dentry = devramfs_alloc_dentry();
    if (!dentry)
    {
        return NULL;
    }

    inode = devramfs_alloc_inode();
    if (!inode)
    {
        return NULL;
    }

    /*init new creat dentry*/
    sys_dlist_init(&dentry->as_parent_node);
    sys_dlist_init(&dentry->as_child_node);
    dentry->type = type;
    dentry->name_length = pqstr->length;
    memcpy(dentry->name, pqstr->name, pqstr->length);
    dentry->inode = inode;

    if (type == FS_DIR_ENTRY_FILE)
    {
        inode->f_op = f_op;
    }

    if (parent_dentry)
    {
        /*add to parent dentry childlist*/
        sys_dlist_append(&(parent_dentry->as_parent_node), &(dentry->as_child_node));
        parent_dentry->dentry_depth++;
        parent_dentry->inode->size = parent_dentry->dentry_depth;
    }

    return dentry;
}

static int devramfs_open(struct fs_file_t *filp, const char *fs_path)
{
    int ret = 0;
    uint8_t vaild_depth = 0;
    struct devramfs_dentry *dentry = NULL;
    struct devramfs_file_data *file_data = NULL;

    dentry = devramfs_lookup(fs_path, &vaild_depth);
    if (!dentry)
    {
        return -ENXIO;
    }

    /*check is open a file*/

    if (dentry->type != FS_DIR_ENTRY_FILE)
    {
        return -EISDIR;
    }

    file_data = devramfs_alloc_file_private_data();
    if (!file_data)
    {
        return -ENOMEM;
    }

    file_data->f_op = dentry->inode->f_op;
    filp->filep = file_data;

    if ((file_data->f_op) && (file_data->f_op->open))
    {
        ret = file_data->f_op->open(filp);
    }

    return ret;

}


static int devramfs_mkfile(struct fs_mount_t *mountp, const char *fs_path, void *data)
{
    ARG_UNUSED(mountp);

    return devramfs_creat(fs_path, FS_DIR_ENTRY_FILE, (struct file_operations_t *)data);

}


static int devramfs_ioctl(struct fs_file_t *filp, int cmd, void *data)
{
    int ret = 0;
    struct file_operations_t *f_op = NULL;
    struct devramfs_file_data  *file_data = NULL;

    file_data = (struct devramfs_file_data *)filp->filep;

    if (!file_data)
    {
        return -ESRCH;
    }

    /*switch to devicephy ctr*/
    f_op = file_data->f_op;
    if (f_op)
    {
        ret = f_op->devctl(filp, cmd, data);
    }
    else
    {
        ret = -ENOTSUP;
    }

    return ret;
}

static int devramfs_close(struct fs_file_t *filp)
{
    int ret = 0;

    return ret;
}

/* Directory operations */
static int devramfs_opendir(struct fs_dir_t *dirp, const char *fs_path)
{
    uint8_t vaild_depth = 0;
    struct devramfs_dentry *dentry = NULL;

    dentry = devramfs_lookup(fs_path, &vaild_depth);
    if (!dentry)
    {
        return -ENXIO;
    }

    /*check is open a file*/
    if (dentry->type != FS_DIR_ENTRY_DIR)
    {
        return -EISDIR;
    }

    /*fill to dirp accord dentry*/
    dirp->dirp = (void *)dentry;


    return 0;

}

static int devramfs_readdir(struct fs_dir_t *dirp, struct fs_dirent *entry)
{
    int ret = 0;
    struct devramfs_dentry *dentry = NULL;

    if ((!dirp) || (!entry))
    {
        return -ENOMEM;
    }
    /*clear name string*/
    memset(entry->name, 0, (MAX_FILE_NAME + 1));

    dentry = (struct devramfs_dentry *)dirp->dirp;
    if (!dentry)
    {
        return -ENOENT;
    }

    memcpy(entry->name, dentry->name, strlen(dentry->name));
    entry->type = dentry->type;
    entry->size = dentry->inode->size;

    return ret;
}

/* File system level operations */
static int devramfs_mount(struct fs_mount_t *mountp)
{
    /*alloc dentry for root entry*/
    return  devramfs_creat_root(DEVRAMFS_ROOT_DETRY_NAME, FS_DIR_ENTRY_DIR);
}


static int devramfs_creat_root()
{
    int ret = 0;
    struct qstr root_qstr;
    struct devramfs_dentry *devramfs_dentry_root = NULL;

    root_qstr.length = strlen(DEVRAMFS_ROOT_DETRY_NAME);
    root_qstr.name = DEVRAMFS_ROOT_DETRY_NAME;

    devramfs_dentry_root = __devramfs_creat(NULL, &root_qstr, FS_DIR_ENTRY_DIR, NULL);
    if (!devramfs_dentry_root)
    {
        ret = -ENOMEM;
        goto out;
    }

    devramfs_set_root_dentry(devramfs_dentry_root);

out:
    return ret;
}

static struct devramfs_dentry *devramfs_alloc_dentry()
{
    struct devramfs_dentry *dentry = NULL;

    if (k_mem_slab_alloc(&devramfs_dentry_pool, (void **)&dentry, K_NO_WAIT) == 0)
    {
        memset(((void *)dentry), 0, sizeof(struct devramfs_dentry));
    }
    else
    {
        return NULL;
    }

    return dentry;
}


static struct devramfs_inode *devramfs_alloc_inode()
{
    struct devramfs_inode *inode = NULL;

    if (k_mem_slab_alloc(&devramfs_inode_pool, (void **)&inode, K_NO_WAIT) == 0)
    {
        memset(((void *)inode), 0, sizeof(struct devramfs_inode));
    }
    else
    {
        return NULL;
    }

    return  inode;
}


static struct devramfs_file_data *devramfs_alloc_file_private_data()
{
    struct devramfs_file_data *file_data = NULL;

    if (k_mem_slab_alloc(&devramfs_file_private_data_pool, (void **)&file_data, K_NO_WAIT) == 0)
    {
        memset((void *)file_data, 0, sizeof(struct devramfs_file_data));
    }
    else
    {
        return NULL;
    }

    return  file_data;
}


static int devramfs_mkdir(struct fs_mount_t *mountp, const char *name)
{
    int ret = devramfs_creat(name, FS_DIR_ENTRY_DIR, NULL);

    return ret;
}

/*todo implement later*/
static int devramfs_stat(struct fs_mount_t *mountp, const char *path, struct fs_dirent *entry)
{
    int ret = 0;

    return ret;
}

/*todo implement later*/
static int devramfs_statvfs(struct fs_mount_t *mountp, const char *path, struct fs_statvfs *stat)
{
    int ret = 0;

    return ret;
}

/*todo implement later*/
static int devramfs_closedir(struct fs_dir_t *dirp)
{
    int ret = 0;

    return ret;
}


static int devramfs_init(struct device *dev)
{
    int ret = 0;

    ARG_UNUSED(dev);

    ret = fs_register(FS_DEVRAMFS, &devram_fs);
    if (ret)
    {
        return ret;
    }

    return fs_mount(&devramfs_mnt);

}

SYS_INIT(devramfs_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

