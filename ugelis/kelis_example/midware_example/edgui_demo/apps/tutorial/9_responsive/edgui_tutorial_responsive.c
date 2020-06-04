/**
 * @file edgui_tutorial_responsive.h
 *
 */

/*
 * -------------------------------------------------
 * See how to create responsive user interfaces
 * ------------------------------------------------
 *
 * Changing the display to different resolution, updating the GUI design
 * or working with dynamic content are much more easier if you use some
 * useful features of the library and follow a few rules.
 *
 * EDGUI_DPI
 * - In edgui_conf.h EDGUI_DPI shows how many pixels are there in 1 inch
 * - You should use it as general unit. For example:
 *     edgui_obj_set_pos(btn1, EDGUI_DPI / 2, EDGUI_DPI);
 * - Built-in styles and themes also use this to set padding and sizes.
 *   So lowering EDGUI_DPI will make paddings smaller.
 * - This way changing to higher pixel density display won't brake your design
 *
 * ALIGN
 * - Use the 'edgui_obj_align()' function to align the object relative to each other
 *     edgui_obj_align(btn1, btn2, EDGUI_ALIGN_OUT_RIGHT_MID, EDGUI_DPI / 2, 0);
 * - It helps to keep an arrangement even is an object is moved
 * - the align happens only once when you call the function.
 *
 * AUTO FIT
 * - The container like objects (edgui_cont, edgui_btn, edgui_page) support auto-fit
 * - It means the object's size will automatically set to include all its children
 * - Can be enabled separately horizontally and vertically
 * - It is useful if you have dynamic content
 * - For example a message box will be as high as its text needs
 * - It uses the style.body.padding.hor/ver to make a padding
 * - Auto-fit runs every time when a children changes (not only once when applied)
 *
 * LAYOUT
 * - You can apply a layout on any container like object
 * - It automatically arranges the children according to a policy
 * - For example `edgui_list` uses it to put elements below each other
 * - Layout runs every time when a children changes (not only once when applied)
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_tutorial_responsive.h"
#if  EDGUI_USE_TUTORIALS

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
 * Create some objects an animate them
 */
void edgui_tutorial_responsive(void)
{
    edgui_obj_t * scr = edgui_disp_get_scr_act(NULL);     /*Get the current screen*/

    edgui_obj_t * label;

    /*EDGUI_DPI*/
    edgui_obj_t * btn1;
    btn1 = edgui_btn_create(scr, NULL);
    edgui_obj_set_pos(btn1, EDGUI_DPI / 10, EDGUI_DPI / 10);     /*Use EDGUI_DPI to set the position*/
    edgui_obj_set_size(btn1, EDGUI_DPI, EDGUI_DPI / 2);          /*Use LVDOI to set the size*/

    label = edgui_label_create(btn1, NULL);
    edgui_label_set_text(label, "EDGUI_DPI");

    /*ALIGN*/
    edgui_obj_t * btn2;
    btn2 = edgui_btn_create(scr, btn1);
    edgui_obj_align(btn2, btn1, EDGUI_ALIGN_OUT_RIGHT_MID, EDGUI_DPI / 4, 0);

    label = edgui_label_create(btn2, NULL);
    edgui_label_set_text(label, "Align");

    /*AUTO FIT*/
    edgui_obj_t * btn3;
    btn3 = edgui_btn_create(scr, btn1);
    edgui_btn_set_fit(btn3, EDGUI_FIT_TIGHT);

    label = edgui_label_create(btn3, NULL);
    edgui_label_set_text(label, "Fit");

    edgui_obj_align(btn3, btn1, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, EDGUI_DPI / 4);   /*Align when already resized because of the label*/

    /*LAYOUT*/
    edgui_obj_t * btn4;
    btn4 = edgui_btn_create(scr, btn1);
    edgui_btn_set_fit(btn4, EDGUI_FIT_TIGHT);           /*Enable fit too*/
    edgui_btn_set_layout(btn4, EDGUI_LAYOUT_COL_R);   /*Right aligned column layout*/

    label = edgui_label_create(btn4, NULL);
    edgui_label_set_text(label, "First");

    label = edgui_label_create(btn4, NULL);
    edgui_label_set_text(label, "Second");

    label = edgui_label_create(btn4, NULL);
    edgui_label_set_text(label, "Third");

    edgui_obj_align(btn4, btn2, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, EDGUI_DPI / 4);   /*Align when already resized because of the label*/

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/
