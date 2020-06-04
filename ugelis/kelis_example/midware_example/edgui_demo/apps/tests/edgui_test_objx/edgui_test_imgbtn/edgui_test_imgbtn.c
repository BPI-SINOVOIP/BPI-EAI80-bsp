/**
 * @file edgui_test_imgbtn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_imgbtn.h"
#if EDGUI_USE_IMGBTN && EDGUI_USE_TESTS

#if EDGUI_EX_PRINTF
#include <stdio.h>
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void imgbtn_event_handler(edgui_obj_t * imgbtn, edgui_event_t event);

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
 * Create imgbtns to test their functionalities
 */
void edgui_test_imgbtn_1(void)
{
    /* Create an image button and set images for it*/
    edgui_obj_t * imgbtn1 = edgui_imgbtn_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(imgbtn1, 10, 10);
    edgui_imgbtn_set_toggle(imgbtn1, true);

    EDGUI_IMG_DECLARE(imgbtn_img_1);
    EDGUI_IMG_DECLARE(imgbtn_img_2);
    EDGUI_IMG_DECLARE(imgbtn_img_3);
    EDGUI_IMG_DECLARE(imgbtn_img_4);

    edgui_imgbtn_set_src(imgbtn1, EDGUI_BTN_STATE_REL, &imgbtn_img_1);
    edgui_imgbtn_set_src(imgbtn1, EDGUI_BTN_STATE_PR, &imgbtn_img_2);
    edgui_imgbtn_set_src(imgbtn1, EDGUI_BTN_STATE_TGL_REL, &imgbtn_img_3);
    edgui_imgbtn_set_src(imgbtn1, EDGUI_BTN_STATE_TGL_PR, &imgbtn_img_4);

    edgui_obj_set_event_cb(imgbtn1, imgbtn_event_handler);

    /*Add a label*/
    edgui_obj_t * label = edgui_label_create(imgbtn1, NULL);
    edgui_label_set_text(label, "Button 1");

    /*Copy the image button*/
    edgui_obj_t * imgbtn2 = edgui_imgbtn_create(edgui_disp_get_scr_act(NULL), imgbtn1);
    edgui_imgbtn_set_state(imgbtn2, EDGUI_BTN_STATE_TGL_REL);
    edgui_obj_align(imgbtn2, imgbtn1, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);

    label = edgui_label_create(imgbtn2, NULL);
    edgui_label_set_text(label, "Button 2");
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void imgbtn_event_handler(edgui_obj_t * imgbtn, edgui_event_t event)
{
    (void) imgbtn; /*Unused*/

    if(event == EDGUI_EVENT_SHORT_CLICKED) {
    #if EDGUI_EX_PRINTF
        printf("Clicked\n");
    #endif
    }

}

#endif /*EDGUI_USE_IMGBTN && EDGUI_USE_TESTS*/
