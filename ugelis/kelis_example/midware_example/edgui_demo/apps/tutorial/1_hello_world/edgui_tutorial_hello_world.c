/**
 * @file edgui_tutorial_hello_world
 *
 */

/*
 *-------------------------------------------------------------------------------
 * Create your first object: a "Hello world" label
 * ------------------------------------------------------------------------------
 *
 * If you have ported the edgui you are ready to create content on your display.
 *
 * Now you will create a "Hello world!" label and align it to the middle.
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_tutorial_hello_world.h"
#if EDGUI_USE_TUTORIALS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

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
 * Create a simple 'Hello world!' label
 */
void edgui_tutorial_hello_world(void)
{
    edgui_obj_t * scr = edgui_disp_get_scr_act(NULL);     /*Get the current screen*/

    /*Create a Label on the currently active screen*/
    edgui_obj_t * label1 =  edgui_label_create(scr, NULL);

    /*Modify the Label's text*/
    edgui_label_set_text(label1, "Hello world!");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    edgui_obj_align(label1, NULL, EDGUI_ALIGN_CENTER, 0, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/
