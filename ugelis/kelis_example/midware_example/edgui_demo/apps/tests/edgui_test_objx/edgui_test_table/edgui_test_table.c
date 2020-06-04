/**
 * @file edgui_test_table.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_table.h"
#if EDGUI_USE_TABLE && EDGUI_USE_TESTS

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
 * Create tables to test their functionalities
 */
void edgui_test_table_1(void)
{
    static edgui_style_t cell1_style;
    edgui_style_copy(&cell1_style, &edgui_style_plain);
    cell1_style.body.border.width = 1;
    cell1_style.line.width = 1;

    static edgui_style_t cell_head_style;
    edgui_style_copy(&cell_head_style, &edgui_style_plain);
    cell_head_style.body.border.width = 1;
    cell_head_style.body.padding.top = 20;
    cell_head_style.body.padding.bottom = 20;
    cell_head_style.line.width = 1;
    cell_head_style.text.color = EDGUI_COLOR_RED;
    cell_head_style.text.line_space = 0;
    cell_head_style.text.letter_space = 5;
    cell_head_style.text.letter_space = 3;

    /* Create a default object*/
    edgui_obj_t * table1 = edgui_table_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_pos(table1, 10, 10);
    edgui_table_set_style(table1, EDGUI_TABLE_STYLE_CELL1, &cell1_style);
    edgui_table_set_style(table1, EDGUI_TABLE_STYLE_CELL2, &cell_head_style);
    edgui_table_set_col_cnt(table1, 4);
    edgui_table_set_row_cnt(table1, 5);

    /*Set the type 1 for the first row. Thye will use */
    edgui_table_set_cell_type(table1, 0, 0, 2);
    edgui_table_set_cell_type(table1, 0, 1, 2);
    edgui_table_set_cell_type(table1, 0, 2, 2);
    edgui_table_set_cell_type(table1, 0, 3, 2);

    edgui_table_set_cell_value(table1, 0, 0, "First\nnew");

    edgui_table_set_cell_value(table1, 0, 1, "Very long second");

    edgui_table_set_cell_value(table1, 0, 2, "Center aligned third");
    edgui_table_set_cell_align(table1, 0, 2, EDGUI_LABEL_ALIGN_CENTER);

    edgui_table_set_cell_value(table1, 0, 3, "Right aligned fourth ");
    edgui_table_set_cell_align(table1, 0, 3, EDGUI_LABEL_ALIGN_RIGHT);

    edgui_table_set_cell_value(table1, 2, 2, "Merge ");
    edgui_table_set_cell_merge_right(table1, 2, 2, true);


    edgui_table_set_cell_value(table1, 3, 1, "Vert. center");

    edgui_table_set_cell_value(table1, 3, 2, "Merge center\nin three\nrows");
    edgui_table_set_cell_merge_right(table1, 3, 2, true);
    edgui_table_set_cell_align(table1, 3, 2, EDGUI_LABEL_ALIGN_CENTER);

    edgui_table_set_cell_value(table1, 4, 2, "Merge right");
    edgui_table_set_cell_merge_right(table1, 4, 2, true);
    edgui_table_set_cell_align(table1, 4, 2, EDGUI_LABEL_ALIGN_RIGHT);


    /*Add some extra rows*/
    edgui_table_set_row_cnt(table1, edgui_table_get_row_cnt(table1) + 2);
    edgui_table_set_cell_value(table1, 6, 0, "Multiple merge");
    edgui_table_set_cell_merge_right(table1, 6, 0, true);
    edgui_table_set_cell_merge_right(table1, 6, 1, true);
    edgui_table_set_cell_merge_right(table1, 6, 2, true);
}

/**
 * Create tables to test their functionalities
 */
void edgui_test_table_2(void)
{
    edgui_obj_t * page = edgui_page_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_page_set_style(page, EDGUI_PAGE_STYLE_BG, &edgui_style_transp_fit);
    edgui_page_set_style(page, EDGUI_PAGE_STYLE_SCRL, &edgui_style_transp_fit);
    edgui_page_set_scrl_fit(page, EDGUI_FIT_TIGHT);
    edgui_obj_set_size(page, 200, 200);

    static edgui_style_t cell_style;
    edgui_style_copy(&cell_style, &edgui_style_plain);
    cell_style.body.border.width = 1;

    /* Create a default object*/
    edgui_obj_t * table1 = edgui_table_create(page, NULL);
    edgui_obj_set_pos(table1, 10, 10);
    edgui_page_glue_obj(table1, true);
    edgui_table_set_style(table1, EDGUI_TABLE_STYLE_CELL1, &cell_style);
    edgui_table_set_col_cnt(table1, 2);
    edgui_table_set_row_cnt(table1, 8);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TABLE && EDGUI_USE_TESTS*/
