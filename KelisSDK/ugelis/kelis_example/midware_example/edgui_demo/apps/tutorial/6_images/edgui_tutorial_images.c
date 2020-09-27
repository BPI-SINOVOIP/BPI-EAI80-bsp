/**
 * @file edgui_tutorial_images.h
 *
 */

/*
 * -------------------------------------------------------------------------------------------
 * Learn how to use images stored internally (in a variable) or externally (e.g. on an SD card)
 *-------------------------------------------------------------------------------------------
 *
 *
 * The basic object to display images is 'edgui_img'. The displayed image is called 'image source'.
 *
 * IMAGE SOURCES
 * -----------------
 *
 * 1. IMAGE CONVETED TO C ARRAY
 *  With the online image converter tool you can convert your images into C arrays:
 *  https://littlevgl.com/image-to-c-array
 *
 *  If you have the converted file:
 *    - Copy the result C file into your project
 *    - Declare the image variable with 'EDGUI_IMG_DECLARE(image_name);'
 *    - Set it for an image object: 'edgui_img_set_src(img1, &image_name);'
 *
 *  In this case you don't need to think about color format because
 *  all color formats are included in the C file and the currently active
 *  (according to 'EDGUI_COLOR_DEPTH' in 'edgui_conf.h') will be enabled.
 *
 * 2. IMAGE FROM FILE
 *  With the above mentioned online image converter tool you can convert images to binary files too.
 *  Now you should choose the right color format.
 *  The result of the conversion should be a *.bin file which can be copied to any external device (e.g. SD card)
 *
 *  To read this file you need to provide some functions for edgui. You will see it in the example below.
 *
 *  To set a file for an image object use: 'edgui_img_set_src(img, "S:path/to/image.bin")'
 *
 * 3. IMAGE FROM SYMBOL FONT
 *  The symbol fonts are letters however they look like small images.
 *  To set symbols for an image object use: 'edgui_img_set_src(img, EDGUI_SYMBOL_CLOSE)'
 *
 * TRANSPARENCY
 * ---------------
 *
 * The images have 2 features related to pixel level transparency:
 *
 * 1. CHROMA KEYING
 *  The EDGUI_COLOR_TRANSP (edgui_conf.h) pixels will be transparent.
 *  This feature can be enabled individually in the images in the online image converter tool.
 *  Because Chroma keying can only show/hide a pixel edges on the image might be jagged.
 *  On the other hand it dosn't mean extra memory usage.
 *
 * 2. ALHPA BYTE
 *  It will add an extra byte to every pixel to show its opacity.
 *  This feature also can be enabled in the online converter tool.
 *  In case of 8 and 16 bit images it means extra 8 bit for every pixel.
 *  The 24 bit images are stored on 32 bit independently from Alpha byte settings.
 *  Alpha byte results very smooth edges and high quality images.
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_tutorial_images.h"
#if EDGUI_USE_TUTORIALS

#include "edgui/edgui.h"
#include <stdio.h>
#include <errno.h>

/*********************
 *      DEFINES
 *********************/
#define PC_FILES    1       /*If you are on PC you can add PC file function to 'edgui_fs'*/

/**********************
 *      TYPEDEFS
 **********************/
typedef  FILE * pc_file_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if PC_FILES && EDGUI_USE_FILESYSTEM
/*Interface functions to standard C file functions (only the required ones to image handling)*/
static edgui_fs_res_t pcfs_open(edgui_fs_drv_t * drv, void * file_p, const char * fn, edgui_fs_mode_t mode);
static edgui_fs_res_t pcfs_close(edgui_fs_drv_t * drv, void * file_p);
static edgui_fs_res_t pcfs_read(edgui_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static edgui_fs_res_t pcfs_seek(edgui_fs_drv_t * drv, void * file_p, uint32_t pos);
static edgui_fs_res_t pcfs_tell(edgui_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
/*Declare the "source code image" which is stored in the flash*/
EDGUI_IMG_DECLARE(red_flower)
EDGUI_IMG_DECLARE(red_rose_16);
EDGUI_IMG_DECLARE(flower_icon_alpha);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create images from variable and file
 */
void edgui_tutorial_image(void)
{
    edgui_obj_t * scr = edgui_disp_get_scr_act(NULL);     /*Get the current screen*/

    /*************************
     * IMAGE FROM SOURCE CODE
     *************************/

    edgui_obj_t * img_var = edgui_img_create(scr, NULL); /*Crate an image object*/
    edgui_img_set_src(img_var, &red_flower);  /*Set the created file as image (a red flower)*/
    edgui_obj_set_pos(img_var, 10, 10);      /*Set the positions*/
    edgui_obj_set_drag(img_var, true);

    img_var = edgui_img_create(scr, NULL); /*Crate an image object*/
    edgui_img_set_src(img_var, &red_rose_16);  /*Set the created file as image (a red rose)*/
    edgui_obj_set_pos(img_var, 10, 100);      /*Set the positions*/
    edgui_obj_set_drag(img_var, true);

    static edgui_style_t style_red;
    edgui_style_copy(&style_red, &edgui_style_plain);
    style_red.image.color = EDGUI_COLOR_RED;

    img_var = edgui_img_create(scr, NULL); /*Crate an image object*/
    edgui_img_set_src(img_var, &flower_icon_alpha);  /*Set the created file as image (a red flower icon)*/
    edgui_img_set_style(img_var, EDGUI_IMG_STYLE_MAIN, &style_red);
    edgui_obj_set_pos(img_var, 10, 200);      /*Set the positions*/
    edgui_obj_set_drag(img_var, true);

#if PC_FILES && EDGUI_USE_FILESYSTEM
    /**************************
     * IMAGE FROM BINARY FILE
     **************************/

    /* Add a simple drive to open images from PC*/
    edgui_fs_drv_t pcfs_drv;                         /*A driver descriptor*/
    memset(&pcfs_drv, 0, sizeof(edgui_fs_drv_t));    /*Initialization*/

    pcfs_drv.file_size = sizeof(pc_file_t);       /*Set up fields...*/
    pcfs_drv.letter = 'P';
    pcfs_drv.open_cb = pcfs_open;
    pcfs_drv.close_cb = pcfs_close;
    pcfs_drv.read_cb = pcfs_read;
    pcfs_drv.seek_cb = pcfs_seek;
    pcfs_drv.tell_cb = pcfs_tell;
    edgui_fs_drv_register(&pcfs_drv);


    edgui_obj_t * img_bin = edgui_img_create(scr, NULL); /*Create an image object*/
    /* Set the image's file according to the current color depth
     * a blue flower picture*/
#if EDGUI_COLOR_DEPTH == 8
    edgui_img_set_src(img_bin, "P:/edgui_examples/edgui_tutorial/6_images/blue_flower_8.bin");
#elif EDGUI_COLOR_DEPTH == 16 && EDGUI_COLOR_16_SWAP == 0
    edgui_img_set_src(img_bin, "P:/edgui_examples/edgui_tutorial/6_images/blue_flower_16.bin");
#elif EDGUI_COLOR_DEPTH == 16 && EDGUI_COLOR_16_SWAP == 0
    edgui_img_set_src(img_bin, "P:/edgui_examples/edgui_tutorial/6_images/blue_flower_16_swap.bin");
#elif EDGUI_COLOR_DEPTH == 32
    edgui_img_set_src(img_bin, "P:/edgui_examples/edgui_tutorial/6_images/blue_flower_32.bin");
#endif

    edgui_obj_set_pos(img_bin, 150, 10);     /*Align next to the source image*/
    edgui_obj_set_drag(img_bin, true);

    img_bin = edgui_img_create(scr, NULL); /*Crate an image object*/
    edgui_img_set_src(img_bin, "P:/edgui_examples/edgui_tutorial/6_images/blue_rose_16.bin");  /*Set the created file as image (a red rose)*/
    edgui_obj_set_pos(img_bin, 150, 100);      /*Set the positions*/
    edgui_obj_set_drag(img_bin, true);

    static edgui_style_t style_blue;
    edgui_style_copy(&style_blue, &edgui_style_plain);
    style_blue.image.color = EDGUI_COLOR_BLUE;

    img_bin = edgui_img_create(scr, NULL); /*Crate an image object*/
    edgui_img_set_src(img_bin, "P:/edgui_examples/edgui_tutorial/6_images/flower_icon_alpha.bin");  /*Set the created file as image (a red flower icon)*/
    edgui_img_set_style(img_bin, EDGUI_IMG_STYLE_MAIN, &style_blue);
    edgui_obj_set_pos(img_bin, 150, 200);      /*Set the positions*/
    edgui_obj_set_drag(img_bin, true);
#endif

    edgui_obj_t * img_symbol = edgui_img_create(scr, NULL);
    edgui_img_set_src(img_symbol, EDGUI_SYMBOL_OK);
    edgui_obj_set_drag(img_symbol, true);
    edgui_obj_set_pos(img_symbol, 300, 10);      /*Set the positions*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if PC_FILES && EDGUI_USE_FILESYSTEM
/**
 * Open a file from the PC
 * @param drv pointer to the current driver
 * @param file_p pointer to a FILE* variable
 * @param fn name of the file.
 * @param mode element of 'fs_mode_t' enum or its 'OR' connection (e.g. FS_MODE_WR | FS_MODE_RD)
 * @return EDGUI_FS_RES_OK: no error, the file is opened
 *         any error from edgui_fs_res_t enum
 */
static edgui_fs_res_t pcfs_open(edgui_fs_drv_t * drv, void * file_p, const char * fn, edgui_fs_mode_t mode)
{
    (void) drv; /*Unused*/

    errno = 0;

    const char * flags = "";

    if(mode == EDGUI_FS_MODE_WR) flags = "wb";
    else if(mode == EDGUI_FS_MODE_RD) flags = "rb";
    else if(mode == (EDGUI_FS_MODE_WR | EDGUI_FS_MODE_RD)) flags = "a+";

    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256];
    sprintf(buf, "./%s", fn);

    pc_file_t f = fopen(buf, flags);
    if(f == NULL) return EDGUI_FS_RES_UNKNOWN;
    else {
        fseek(f, 0, SEEK_SET);

        /* 'file_p' is pointer to a file descriptor and
         * we need to store our file descriptor here*/
        pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
        *fp = f;
    }

    return EDGUI_FS_RES_OK;
}


/**
 * Close an opened file
 * @param drv pointer to the current driver
 * @param file_p pointer to a FILE* variable. (opened with edgui_ufs_open)
 * @return EDGUI_FS_RES_OK: no error, the file is read
 *         any error from edgui__fs_res_t enum
 */
static edgui_fs_res_t pcfs_close(edgui_fs_drv_t * drv, void * file_p)
{
    (void) drv; /*Unused*/

    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    fclose(*fp);
    return EDGUI_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv pointer to the current driver
 * @param file_p pointer to a FILE variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return EDGUI_FS_RES_OK: no error, the file is read
 *         any error from edgui__fs_res_t enum
 */
static edgui_fs_res_t pcfs_read(edgui_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    (void) drv; /*Unused*/

    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    *br = fread(buf, 1, btr, *fp);
    return EDGUI_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv pointer to the current driver
 * @param file_p pointer to a FILE* variable. (opened with edgui_ufs_open )
 * @param pos the new position of read write pointer
 * @return EDGUI_FS_RES_OK: no error, the file is read
 *         any error from edgui__fs_res_t enum
 */
static edgui_fs_res_t pcfs_seek(edgui_fs_drv_t * drv, void * file_p, uint32_t pos)
{
    (void) drv; /*Unused*/

    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    fseek(*fp, pos, SEEK_SET);
    return EDGUI_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv pointer to the current driver
 * @param file_p pointer to a FILE* variable.
 * @param pos_p pointer to to store the result
 * @return EDGUI_FS_RES_OK: no error, the file is read
 *         any error from edgui__fs_res_t enum
 */
static edgui_fs_res_t pcfs_tell(edgui_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    (void) drv; /*Unused*/
    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    *pos_p = ftell(*fp);
    return EDGUI_FS_RES_OK;
}

#endif

#endif /*EDGUI_USE_TUTORIALS*/
