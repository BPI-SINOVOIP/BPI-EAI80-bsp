/**
 * @file edgui_fs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_fs.h"
#if EDGUI_USE_FILESYSTEM

#include "edgui_ll.h"
#include <string.h>
#include "edgui_gc.h"

#if defined(EDGUI_GC_INCLUDE)
#include EDGUI_GC_INCLUDE
#endif /* EDGUI_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/

/* "free" is used as a function pointer (in edgui_fs_drv_t).
 * We must make sure "free" was not defined to a platform specific
 * free function, otherwise compilation would fail.
 */
#ifdef free
#undef free
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static const char * edgui_fs_get_real_path(const char * path);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the File system interface
 */
void edgui_fs_init(void)
{
    edgui_ll_init(&EDGUI_GC_ROOT(_edgui_drv_ll), sizeof(edgui_fs_drv_t));
}

/**
 * Test if a drive is rady or not. If the `ready` function was not initialized `true` will be
 * returned.
 * @param letter letter of the drive
 * @return true: drive is ready; false: drive is not ready
 */
bool edgui_fs_is_ready(char letter)
{
    edgui_fs_drv_t * drv = edgui_fs_get_drv(letter);

    if(drv == NULL) return false; /*An unknown driver in not ready*/

    if(drv->ready_cb == NULL) return true; /*Assume the driver is always ready if no handler provided*/

    return drv->ready_cb(drv);
}

/**
 * Open a file
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_open(edgui_fs_file_t * file_p, const char * path, edgui_fs_mode_t mode)
{
    file_p->drv    = NULL;
    file_p->file_d = NULL;

    if(path == NULL) return EDGUI_FS_RES_INV_PARAM;

    char letter = path[0];

    file_p->drv = edgui_fs_get_drv(letter);

    if(file_p->drv == NULL) {
        file_p->file_d = NULL;
        return EDGUI_FS_RES_NOT_EX;
    }

    if(file_p->drv->ready_cb != NULL) {
        if(file_p->drv->ready_cb(file_p->drv) == false) {
            file_p->drv    = NULL;
            file_p->file_d = NULL;
            return EDGUI_FS_RES_HW_ERR;
        }
    }

    file_p->file_d = edgui_mem_alloc(file_p->drv->file_size);
    edgui_mem_assert(file_p->file_d);
    if(file_p->file_d == NULL) {
        file_p->drv = NULL;
        return EDGUI_FS_RES_OUT_OF_MEM; /* Out of memory */
    }

    if(file_p->drv->open_cb == NULL) {
        return EDGUI_FS_RES_NOT_IMP;
    }

    const char * real_path = edgui_fs_get_real_path(path);
    edgui_fs_res_t res        = file_p->drv->open_cb(file_p->drv, file_p->file_d, real_path, mode);

    if(res != EDGUI_FS_RES_OK) {
        edgui_mem_free(file_p->file_d);
        file_p->file_d = NULL;
        file_p->drv    = NULL;
    }

    return res;
}

/**
 * Close an already opened file
 * @param file_p pointer to a edgui_fs_file_t variable
 * @return  EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_close(edgui_fs_file_t * file_p)
{
    if(file_p->drv == NULL) {
        return EDGUI_FS_RES_INV_PARAM;
    }

    if(file_p->drv->close_cb == NULL) {
        return EDGUI_FS_RES_NOT_IMP;
    }

    edgui_fs_res_t res = file_p->drv->close_cb(file_p->drv, file_p->file_d);

    edgui_mem_free(file_p->file_d); /*Clean up*/
    file_p->file_d = NULL;
    file_p->drv    = NULL;
    file_p->file_d = NULL;

    return res;
}

/**
 * Delete a file
 * @param path path of the file to delete
 * @return  EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_remove(const char * path)
{
    if(path == NULL) return EDGUI_FS_RES_INV_PARAM;
    edgui_fs_drv_t * drv = NULL;

    char letter = path[0];

    drv = edgui_fs_get_drv(letter);
    if(drv == NULL) return EDGUI_FS_RES_NOT_EX;
    if(drv->ready_cb != NULL) {
        if(drv->ready_cb(drv) == false) return EDGUI_FS_RES_HW_ERR;
    }

    if(drv->remove_cb == NULL) return EDGUI_FS_RES_NOT_IMP;

    const char * real_path = edgui_fs_get_real_path(path);
    edgui_fs_res_t res        = drv->remove_cb(drv, real_path);

    return res;
}

/**
 * Read from a file
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param buf pointer to a buffer where the read bytes are stored
 * @param btr Bytes To Read
 * @param br the number of real read bytes (Bytes Read). NULL if unused.
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_read(edgui_fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    if(br != NULL) *br = 0;
    if(file_p->drv == NULL) return EDGUI_FS_RES_INV_PARAM;
    if(file_p->drv->read_cb == NULL) return EDGUI_FS_RES_NOT_IMP;

    uint32_t br_tmp = 0;
    edgui_fs_res_t res = file_p->drv->read_cb(file_p->drv, file_p->file_d, buf, btr, &br_tmp);
    if(br != NULL) *br = br_tmp;

    return res;
}

/**
 * Write into a file
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btr Bytes To Write
 * @param br the number of real written bytes (Bytes Written). NULL if unused.
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_write(edgui_fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    if(bw != NULL) *bw = 0;

    if(file_p->drv == NULL) {
        return EDGUI_FS_RES_INV_PARAM;
    }

    if(file_p->drv->write_cb == NULL) {
        return EDGUI_FS_RES_NOT_IMP;
    }

    uint32_t bw_tmp = 0;
    edgui_fs_res_t res = file_p->drv->write_cb(file_p->drv, file_p->file_d, buf, btw, &bw_tmp);
    if(bw != NULL) *bw = bw_tmp;

    return res;
}

/**
 * Set the position of the 'cursor' (read write pointer) in a file
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param pos the new position expressed in bytes index (0: start of file)
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_seek(edgui_fs_file_t * file_p, uint32_t pos)
{
    if(file_p->drv == NULL) {
        return EDGUI_FS_RES_INV_PARAM;
    }

    if(file_p->drv->seek_cb == NULL) {
        return EDGUI_FS_RES_NOT_IMP;
    }

    edgui_fs_res_t res = file_p->drv->seek_cb(file_p->drv, file_p->file_d, pos);

    return res;
}

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param pos_p pointer to store the position of the read write pointer
 * @return EDGUI_FS_RES_OK or any error from 'fs_res_t'
 */
edgui_fs_res_t edgui_fs_tell(edgui_fs_file_t * file_p, uint32_t * pos)
{
    if(file_p->drv == NULL) {
        pos = 0;
        return EDGUI_FS_RES_INV_PARAM;
    }

    if(file_p->drv->tell_cb == NULL) {
        pos = 0;
        return EDGUI_FS_RES_NOT_IMP;
    }

    edgui_fs_res_t res = file_p->drv->tell_cb(file_p->drv, file_p->file_d, pos);

    return res;
}

/**
 * Truncate the file size to the current position of the read write pointer
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with edgui_fs_open )
 * @return EDGUI_FS_RES_OK: no error, the file is read
 *         any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_trunc(edgui_fs_file_t * file_p)
{
    if(file_p->drv == NULL) {
        return EDGUI_FS_RES_INV_PARAM;
    }

    if(file_p->drv->tell_cb == NULL) {
        return EDGUI_FS_RES_NOT_IMP;
    }

    edgui_fs_res_t res = file_p->drv->trunc_cb(file_p->drv, file_p->file_d);

    return res;
}
/**
 * Give the size of a file bytes
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param size pointer to a variable to store the size
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_size(edgui_fs_file_t * file_p, uint32_t * size)
{
    if(file_p->drv == NULL) {
        return EDGUI_FS_RES_INV_PARAM;
    }

    if(file_p->drv->size_cb == NULL) return EDGUI_FS_RES_NOT_IMP;

    if(size == NULL) return EDGUI_FS_RES_INV_PARAM;

    edgui_fs_res_t res = file_p->drv->size_cb(file_p->drv, file_p->file_d, size);

    return res;
}

/**
 * Rename a file
 * @param oldname path to the file
 * @param newname path with the new name
 * @return EDGUI_FS_RES_OK or any error from 'fs_res_t'
 */
edgui_fs_res_t edgui_fs_rename(const char * oldname, const char * newname)
{
    if(!oldname || !newname) return EDGUI_FS_RES_INV_PARAM;

    char letter = oldname[0];

    edgui_fs_drv_t * drv = edgui_fs_get_drv(letter);

    if(!drv) {
        return EDGUI_FS_RES_NOT_EX;
    }

    if(drv->ready_cb != NULL) {
        if(drv->ready_cb(drv) == false) {
            return EDGUI_FS_RES_HW_ERR;
        }
    }

    if(drv->rename_cb == NULL) return EDGUI_FS_RES_NOT_IMP;

    const char * old_real = edgui_fs_get_real_path(oldname);
    const char * new_real = edgui_fs_get_real_path(newname);

    edgui_fs_res_t res = drv->rename_cb(drv, old_real, new_real);

    return res;
}

/**
 * Initialize a 'fs_read_dir_t' variable for directory reading
 * @param rddir_p pointer to a 'fs_read_dir_t' variable
 * @param path path to a directory
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_dir_open(edgui_fs_dir_t * rddir_p, const char * path)
{
    if(path == NULL) return EDGUI_FS_RES_INV_PARAM;

    char letter = path[0];

    rddir_p->drv = edgui_fs_get_drv(letter);

    if(rddir_p->drv == NULL) {
        rddir_p->dir_d = NULL;
        return EDGUI_FS_RES_NOT_EX;
    }

    rddir_p->dir_d = edgui_mem_alloc(rddir_p->drv->rddir_size);
    edgui_mem_assert(rddir_p->dir_d);
    if(rddir_p->dir_d == NULL) {
        rddir_p->dir_d = NULL;
        return EDGUI_FS_RES_OUT_OF_MEM; /* Out of memory */
    }

    if(rddir_p->drv->dir_open_cb == NULL) {
        return EDGUI_FS_RES_NOT_IMP;
    }

    const char * real_path = edgui_fs_get_real_path(path);

    edgui_fs_res_t res = rddir_p->drv->dir_open_cb(rddir_p->drv, rddir_p->dir_d, real_path);

    return res;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param rddir_p pointer to an initialized 'fs_read_dir_t' variable
 * @param fn pointer to a buffer to store the filename
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_dir_read(edgui_fs_dir_t * rddir_p, char * fn)
{
    if(rddir_p->drv == NULL || rddir_p->dir_d == NULL) {
        fn[0] = '\0';
        return EDGUI_FS_RES_INV_PARAM;
    }

    if(rddir_p->drv->dir_read_cb == NULL) {
        return EDGUI_FS_RES_NOT_IMP;
    }

    edgui_fs_res_t res = rddir_p->drv->dir_read_cb(rddir_p->drv, rddir_p->dir_d, fn);

    return res;
}

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'fs_read_dir_t' variable
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_dir_close(edgui_fs_dir_t * rddir_p)
{
    if(rddir_p->drv == NULL || rddir_p->dir_d == NULL) {
        return EDGUI_FS_RES_INV_PARAM;
    }

    edgui_fs_res_t res;

    if(rddir_p->drv->dir_close_cb == NULL) {
        res = EDGUI_FS_RES_NOT_IMP;
    } else {
        res = rddir_p->drv->dir_close_cb(rddir_p->drv, rddir_p->dir_d);
    }

    edgui_mem_free(rddir_p->dir_d); /*Clean up*/
    rddir_p->dir_d = NULL;
    rddir_p->drv   = NULL;
    rddir_p->dir_d = NULL;

    return res;
}

/**
 * Get the free and total size of a driver in kB
 * @param letter the driver letter
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free size_cb [kB]
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_free_space(char letter, uint32_t * total_p, uint32_t * free_p)
{
    edgui_fs_drv_t * drv = edgui_fs_get_drv(letter);

    if(drv == NULL) {
        return EDGUI_FS_RES_INV_PARAM;
    }

    edgui_fs_res_t res;

    if(drv->free_space_cb == NULL) {
        res = EDGUI_FS_RES_NOT_IMP;
    } else {
        uint32_t total_tmp = 0;
        uint32_t free_tmp  = 0;
        res                = drv->free_space_cb(drv, &total_tmp, &free_tmp);

        if(total_p != NULL) *total_p = total_tmp;
        if(free_p != NULL) *free_p = free_tmp;
    }

    return res;
}

/**
 * Initialize a file system driver with default values.
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param drv pointer to driver variable to initialize
 */
void edgui_fs_drv_init(edgui_fs_drv_t * drv)
{
    memset(drv, 0, sizeof(edgui_fs_drv_t));
}

/**
 * Add a new drive
 * @param drv_p pointer to an edgui_fs_drv_t structure which is inited with the
 * corresponding function pointers. The data will be copied so the variable can be local.
 */
void edgui_fs_drv_register(edgui_fs_drv_t * drv_p)
{
    /*Save the new driver*/
    edgui_fs_drv_t * new_drv;
    new_drv = edgui_ll_ins_head(&EDGUI_GC_ROOT(_edgui_drv_ll));
    edgui_mem_assert(new_drv);
    if(new_drv == NULL) return;

    memcpy(new_drv, drv_p, sizeof(edgui_fs_drv_t));
}

/**
 * Give a pointer to a driver from its letter
 * @param letter the driver letter
 * @return pointer to a driver or NULL if not found
 */
edgui_fs_drv_t * edgui_fs_get_drv(char letter)
{
    edgui_fs_drv_t * drv;

    EDGUI_LL_READ(EDGUI_GC_ROOT(_edgui_drv_ll), drv)
    {
        if(drv->letter == letter) {
            return drv;
        }
    }

    return NULL;
}
/**
 * Fill a buffer with the letters of existing drivers
 * @param buf buffer to store the letters ('\0' added after the last letter)
 * @return the buffer
 */
char * edgui_fs_get_letters(char * buf)
{
    edgui_fs_drv_t * drv;
    uint8_t i = 0;

    EDGUI_LL_READ(EDGUI_GC_ROOT(_edgui_drv_ll), drv)
    {
        buf[i] = drv->letter;
        i++;
    }

    buf[i] = '\0';

    return buf;
}

/**
 * Return with the extension of the filename
 * @param fn string with a filename
 * @return pointer to the beginning extension or empty string if no extension
 */
const char * edgui_fs_get_ext(const char * fn)
{
    uint16_t i;
    for(i = strlen(fn); i > 0; i--) {
        if(fn[i] == '.') {
            return &fn[i + 1];
        } else if(fn[i] == '/' || fn[i] == '\\') {
            return ""; /*No extension if a '\' or '/' found*/
        }
    }

    return ""; /*Empty string if no '.' in the file name. */
}

/**
 * Step up one level
 * @param path pointer to a file name
 * @return the truncated file name
 */
char * edgui_fs_up(char * path)
{
    uint16_t len = strlen(path);
    if(len == 0) return path;

    len--; /*Go before the trailing '\0'*/

    /*Ignore trailing '/' or '\'*/
    while(path[len] == '/' || path[len] == '\\') {
        path[len] = '\0';
        if(len > 0)
            len--;
        else
            return path;
    }

    uint16_t i;
    for(i = len; i > 0; i--) {
        if(path[i] == '/' || path[i] == '\\') break;
    }

    if(i > 0) path[i] = '\0';

    return path;
}

/**
 * Get the last element of a path (e.g. U:/folder/file -> file)
 * @param path a character sting with the path to search in
 * @return pointer to the beginning of the last element in the path
 */
const char * edgui_fs_get_last(const char * path)
{
    uint16_t len = strlen(path);
    if(len == 0) return path;

    len--; /*Go before the trailing '\0'*/

    /*Ignore trailing '/' or '\'*/
    while(path[len] == '/' || path[len] == '\\') {
        if(len > 0)
            len--;
        else
            return path;
    }

    uint16_t i;
    for(i = len; i > 0; i--) {
        if(path[i] == '/' || path[i] == '\\') break;
    }

    /*No '/' or '\' in the path so return with path itself*/
    if(i == 0) return path;

    return &path[i + 1];
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Leave the driver letters and / or \ letters from beginning of the path
 * @param path path string (E.g. S:/folder/file.txt)
 * @return pointer to the beginning of the real path (E.g. folder/file.txt)
 */
static const char * edgui_fs_get_real_path(const char * path)
{
    /* Example path: "S:/folder/file.txt"
     * Leave the letter and the : / \ characters*/

    path++; /*Ignore the driver letter*/

    while(*path != '\0') {
        if(*path == ':' || *path == '\\' || *path == '/') {
            path++;
        } else {
            break;
        }
    }

    return path;
}

#endif /*EDGUI_USE_FILESYSTEM*/
