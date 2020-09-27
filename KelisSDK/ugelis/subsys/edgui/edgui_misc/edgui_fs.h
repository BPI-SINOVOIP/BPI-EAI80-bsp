/**
 * @file edgui_fs.h
 *
 */

#ifndef EDGUI_FS_H
#define EDGUI_FS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui_conf.h"
#else
#include "../edgui/edgui_conf.h"
#endif

#if EDGUI_USE_FILESYSTEM

#include <stdint.h>
#include <stdbool.h>
#include "edgui_mem.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_FS_MAX_FN_LENGTH 64

/**********************
 *      TYPEDEFS
 **********************/
/**
 * Errors in the filesystem module.
 */
enum {
    EDGUI_FS_RES_OK = 0,
    EDGUI_FS_RES_HW_ERR,     /*Low level hardware error*/
    EDGUI_FS_RES_FS_ERR,     /*Error in the file system structure */
    EDGUI_FS_RES_NOT_EX,     /*Driver, file or directory is not exists*/
    EDGUI_FS_RES_FULL,       /*Disk full*/
    EDGUI_FS_RES_LOCKED,     /*The file is already opened*/
    EDGUI_FS_RES_DENIED,     /*Access denied. Check 'fs_open' modes and write protect*/
    EDGUI_FS_RES_BUSY,       /*The file system now can't handle it, try later*/
    EDGUI_FS_RES_TOUT,       /*Process time outed*/
    EDGUI_FS_RES_NOT_IMP,    /*Requested function is not implemented*/
    EDGUI_FS_RES_OUT_OF_MEM, /*Not enough memory for an internal operation*/
    EDGUI_FS_RES_INV_PARAM,  /*Invalid parameter among arguments*/
    EDGUI_FS_RES_UNKNOWN,    /*Other unknown error*/
};
typedef uint8_t edgui_fs_res_t;

/**
 * Filesystem mode.
 */
enum {
    EDGUI_FS_MODE_WR = 0x01,
    EDGUI_FS_MODE_RD = 0x02,
};
typedef uint8_t edgui_fs_mode_t;

typedef struct _edgui_fs_drv_t
{
    char letter;
    uint16_t file_size;
    uint16_t rddir_size;
    bool (*ready_cb)(struct _edgui_fs_drv_t * drv);

    edgui_fs_res_t (*open_cb)(struct _edgui_fs_drv_t * drv, void * file_p, const char * path, edgui_fs_mode_t mode);
    edgui_fs_res_t (*close_cb)(struct _edgui_fs_drv_t * drv, void * file_p);
    edgui_fs_res_t (*remove_cb)(struct _edgui_fs_drv_t * drv, const char * fn);
    edgui_fs_res_t (*read_cb)(struct _edgui_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
    edgui_fs_res_t (*write_cb)(struct _edgui_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
    edgui_fs_res_t (*seek_cb)(struct _edgui_fs_drv_t * drv, void * file_p, uint32_t pos);
    edgui_fs_res_t (*tell_cb)(struct _edgui_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
    edgui_fs_res_t (*trunc_cb)(struct _edgui_fs_drv_t * drv, void * file_p);
    edgui_fs_res_t (*size_cb)(struct _edgui_fs_drv_t * drv, void * file_p, uint32_t * size_p);
    edgui_fs_res_t (*rename_cb)(struct _edgui_fs_drv_t * drv, const char * oldname, const char * newname);
    edgui_fs_res_t (*free_space_cb)(struct _edgui_fs_drv_t * drv, uint32_t * total_p, uint32_t * free_p);

    edgui_fs_res_t (*dir_open_cb)(struct _edgui_fs_drv_t * drv, void * rddir_p, const char * path);
    edgui_fs_res_t (*dir_read_cb)(struct _edgui_fs_drv_t * drv, void * rddir_p, char * fn);
    edgui_fs_res_t (*dir_close_cb)(struct _edgui_fs_drv_t * drv, void * rddir_p);

#if EDGUI_USE_USER_DATA
    edgui_fs_drv_user_data_t user_data; /**< Custom file user data */
#endif
} edgui_fs_drv_t;

typedef struct
{
    void * file_d;
    edgui_fs_drv_t * drv;
} edgui_fs_file_t;

typedef struct
{
    void * dir_d;
    edgui_fs_drv_t * drv;
} edgui_fs_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the File system interface
 */
void edgui_fs_init(void);

/**
 * Initialize a file system driver with default values.
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param drv pointer to driver variable to initialize
 */
void edgui_fs_drv_init(edgui_fs_drv_t * drv);

/**
 * Add a new drive
 * @param drv_p pointer to an edgui_fs_drv_t structure which is inited with the
 * corresponding function pointers. The data will be copied so the variable can be local.
 */
void edgui_fs_drv_register(edgui_fs_drv_t * drv_p);

/**
 * Give a pointer to a driver from its letter
 * @param letter the driver letter
 * @return pointer to a driver or NULL if not found
 */
edgui_fs_drv_t * edgui_fs_get_drv(char letter);

/**
 * Test if a drive is rady or not. If the `ready` function was not initialized `true` will be
 * returned.
 * @param letter letter of the drive
 * @return true: drive is ready; false: drive is not ready
 */
bool edgui_fs_is_ready(char letter);

/**
 * Open a file
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_open(edgui_fs_file_t * file_p, const char * path, edgui_fs_mode_t mode);

/**
 * Close an already opened file
 * @param file_p pointer to a edgui_fs_file_t variable
 * @return  EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_close(edgui_fs_file_t * file_p);

/**
 * Delete a file
 * @param path path of the file to delete
 * @return  EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_remove(const char * path);

/**
 * Read from a file
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param buf pointer to a buffer where the read bytes are stored
 * @param btr Bytes To Read
 * @param br the number of real read bytes (Bytes Read). NULL if unused.
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_read(edgui_fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br);

/**
 * Write into a file
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btr Bytes To Write
 * @param br the number of real written bytes (Bytes Written). NULL if unused.
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_write(edgui_fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw);

/**
 * Set the position of the 'cursor' (read write pointer) in a file
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param pos the new position expressed in bytes index (0: start of file)
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_seek(edgui_fs_file_t * file_p, uint32_t pos);

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param pos_p pointer to store the position of the read write pointer
 * @return EDGUI_FS_RES_OK or any error from 'fs_res_t'
 */
edgui_fs_res_t edgui_fs_tell(edgui_fs_file_t * file_p, uint32_t * pos);

/**
 * Truncate the file size to the current position of the read write pointer
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with edgui_fs_open )
 * @return EDGUI_FS_RES_OK: no error, the file is read
 *         any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_trunc(edgui_fs_file_t * file_p);

/**
 * Give the size of a file bytes
 * @param file_p pointer to a edgui_fs_file_t variable
 * @param size pointer to a variable to store the size
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_size(edgui_fs_file_t * file_p, uint32_t * size);

/**
 * Rename a file
 * @param oldname path to the file
 * @param newname path with the new name
 * @return EDGUI_FS_RES_OK or any error from 'fs_res_t'
 */
edgui_fs_res_t edgui_fs_rename(const char * oldname, const char * newname);

/**
 * Initialize a 'fs_dir_t' variable for directory reading
 * @param rddir_p pointer to a 'fs_read_dir_t' variable
 * @param path path to a directory
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_dir_open(edgui_fs_dir_t * rddir_p, const char * path);

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param rddir_p pointer to an initialized 'fs_rdir_t' variable
 * @param fn pointer to a buffer to store the filename
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_dir_read(edgui_fs_dir_t * rddir_p, char * fn);

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'fs_dir_t' variable
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_dir_close(edgui_fs_dir_t * rddir_p);

/**
 * Get the free and total size of a driver in kB
 * @param letter the driver letter
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free size [kB]
 * @return EDGUI_FS_RES_OK or any error from edgui_fs_res_t enum
 */
edgui_fs_res_t edgui_fs_free_space(char letter, uint32_t * total_p, uint32_t * free_p);

/**
 * Fill a buffer with the letters of existing drivers
 * @param buf buffer to store the letters ('\0' added after the last letter)
 * @return the buffer
 */
char * edgui_fs_get_letters(char * buf);

/**
 * Return with the extension of the filename
 * @param fn string with a filename
 * @return pointer to the beginning extension or empty string if no extension
 */
const char * edgui_fs_get_ext(const char * fn);

/**
 * Step up one level
 * @param path pointer to a file name
 * @return the truncated file name
 */
char * edgui_fs_up(char * path);

/**
 * Get the last element of a path (e.g. U:/folder/file -> file)
 * @param buf buffer to store the letters ('\0' added after the last letter)
 * @return pointer to the beginning of the last element in the path
 */
const char * edgui_fs_get_last(const char * path);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_FILESYSTEM*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_FS_H*/
