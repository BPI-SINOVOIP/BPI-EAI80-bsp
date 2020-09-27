/**
 * @file edgui_test_tileview.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_tileview.h"
#if EDGUI_USE_TILEVIEW && EDGUI_USE_BTN && EDGUI_USE_LABEL && EDGUI_USE_LIST && EDGUI_USE_TESTS

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
 * Create a tileview to test their functionalities
 */
void edgui_test_tileview_1(void)
{
    edgui_coord_t hres = 240;//edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = 240;//edgui_disp_get_ver_res(NULL);

    static const edgui_point_t vp[] = {
                   {1,0},          /*First row: only the middle tile*/
            {0,1}, {1,1}, {1,2},   /*Second row: all tree tiles */
                   {2,1}, {2,2},   /*Third row: middle and right tile*/
            };

    edgui_obj_t * t;
    t = edgui_tileview_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_tileview_set_valid_positions(t, vp, 6);
    edgui_tileview_set_edge_flash(t, true);
    edgui_obj_set_size(t, hres, vres);
    edgui_obj_t * label;

    /*x0, y1 container*/
    edgui_obj_t * p01 = edgui_obj_create(t, NULL);
    edgui_obj_set_click(p01, true);
    edgui_obj_set_style(p01, &edgui_style_pretty_color);
    edgui_obj_set_size(p01, edgui_obj_get_width(t), edgui_obj_get_height(t));
    edgui_tileview_add_element(t, p01);

    /*Add a  button at x0, y1*/
    edgui_obj_t * b01 = edgui_btn_create(p01, NULL);
    edgui_tileview_add_element(t, b01);
    edgui_obj_align(b01, NULL, EDGUI_ALIGN_CENTER, 0, 50);
    label = edgui_label_create(b01, NULL);
    edgui_label_set_text(label, "Button");

    /*Add a label to indicate the position*/
    label = edgui_label_create(p01, NULL);
    edgui_label_set_text(label, "x0, y1");
    edgui_obj_align(label, NULL, EDGUI_ALIGN_CENTER, 0, 0);

    /*x1, y1 container*/
    edgui_obj_t * p11 = edgui_obj_create(t, p01);
    edgui_obj_align(p11, p01, EDGUI_ALIGN_OUT_RIGHT_MID, 0, 0);
    edgui_tileview_add_element(t, p11);

    /*Add a label to indicate the position*/
    label = edgui_label_create(p11, NULL);
    edgui_label_set_text(label, "x1, y1");
    edgui_obj_align(label, NULL, EDGUI_ALIGN_CENTER, 0, 0);

    /*x1, y2 list*/
    edgui_obj_t * list12 = edgui_list_create(t, NULL);
    edgui_obj_set_size(list12, hres, vres);
    edgui_obj_align(list12, p11, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 0);
    edgui_list_set_scroll_propagation(list12, true);
    edgui_tileview_add_element(t, list12);

    edgui_obj_t * list_btn;
    list_btn = edgui_list_add_btn(list12, NULL, "One");
    edgui_tileview_add_element(t, list_btn);

    list_btn = edgui_list_add_btn(list12, NULL, "Two");
    edgui_tileview_add_element(t, list_btn);

    list_btn = edgui_list_add_btn(list12, NULL, "Three");
    edgui_tileview_add_element(t, list_btn);

    list_btn = edgui_list_add_btn(list12, NULL, "Four");
    edgui_tileview_add_element(t, list_btn);

    list_btn = edgui_list_add_btn(list12, NULL, "Five");
    edgui_tileview_add_element(t, list_btn);

    list_btn = edgui_list_add_btn(list12, NULL, "Six");
    edgui_tileview_add_element(t, list_btn);

    list_btn = edgui_list_add_btn(list12, NULL, "Seven");
    edgui_tileview_add_element(t, list_btn);

    /*x1, y0 container*/
    edgui_obj_t * p10 = edgui_obj_create(t, p01);
    edgui_tileview_add_element(t, p10);

    /*Add a label to indicate the position*/
    label = edgui_label_create(p10, NULL);
    edgui_label_set_text(label, "x1, y0");
    edgui_obj_align(label, NULL, EDGUI_ALIGN_CENTER, 0, 0);
    edgui_obj_align(p10, p11, EDGUI_ALIGN_OUT_TOP_MID, 0, 0);

    /*x2, y1 container*/
    edgui_obj_t * p21 = edgui_obj_create(t, p01);
    edgui_tileview_add_element(t, p21);
    edgui_obj_align(p21, p11, EDGUI_ALIGN_OUT_RIGHT_MID, 0, 0);

    /*Add a label to indicate the position*/
    label = edgui_label_create(p21, NULL);
    edgui_label_set_text(label, "x2, y1");
    edgui_obj_align(label, NULL, EDGUI_ALIGN_CENTER, 0, 0);

    /*x2, y1 container*/
    edgui_obj_t * p22 = edgui_obj_create(t, p01);
    edgui_tileview_add_element(t, p22);
    edgui_obj_align(p22, p21, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, 0);

    /*Add a label to indicate the position*/
    label = edgui_label_create(p22, NULL);
    edgui_label_set_text(label, "x2, y2");
    edgui_obj_align(label, NULL, EDGUI_ALIGN_CENTER, 0, 0);

    /*Focus on a tile (the list)*/
    edgui_tileview_set_tile_act(t, 1, 2, true);

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TILEVIEW && EDGUI_USE_TESTS*/
