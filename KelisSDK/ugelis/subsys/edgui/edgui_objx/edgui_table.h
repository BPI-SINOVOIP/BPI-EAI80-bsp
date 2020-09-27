/**
 * @file edgui_table.h
 *
 */

#ifndef EDGUI_TABLE_H
#define EDGUI_TABLE_H

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

#if EDGUI_USE_TABLE != 0

/*Testing of dependencies*/
#if EDGUI_USE_LABEL == 0
#error "edgui_table: edgui_label is required. Enable it in edgui_conf.h (EDGUI_USE_LABEL  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_label.h"

/*********************
 *      DEFINES
 *********************/
#ifndef EDGUI_TABLE_COL_MAX
#define EDGUI_TABLE_COL_MAX 12
#endif

#define EDGUI_TABLE_CELL_STYLE_CNT 4
/**********************
 *      TYPEDEFS
 **********************/

/**
 * Internal table cell format structure.
 * 
 * Use the `edgui_table` APIs instead.
 */
typedef union
{
    struct
    {
        uint8_t align : 2;
        uint8_t right_merge : 1;
        uint8_t type : 2;
        uint8_t crop : 1;
    } s;
    uint8_t format_byte;
} edgui_table_cell_format_t;

/*Data of table*/
typedef struct
{
    /*New data for this type */
    uint16_t col_cnt;
    uint16_t row_cnt;
    char ** cell_data;
    const edgui_style_t * cell_style[EDGUI_TABLE_CELL_STYLE_CNT];
    edgui_coord_t col_w[EDGUI_TABLE_COL_MAX];
} edgui_table_ext_t;

/*Styles*/
enum {
    EDGUI_TABLE_STYLE_BG,
    EDGUI_TABLE_STYLE_CELL1,
    EDGUI_TABLE_STYLE_CELL2,
    EDGUI_TABLE_STYLE_CELL3,
    EDGUI_TABLE_STYLE_CELL4,
};
typedef uint8_t edgui_table_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a table object
 * @param par pointer to an object, it will be the parent of the new table
 * @param copy pointer to a table object, if not NULL then the new object will be copied from it
 * @return pointer to the created table
 */
edgui_obj_t * edgui_table_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the value of a cell.
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param txt text to display in the cell. It will be copied and saved so this variable is not
 * required after this function call.
 */
void edgui_table_set_cell_value(edgui_obj_t * table, uint16_t row, uint16_t col, const char * txt);

/**
 * Set the number of rows
 * @param table table pointer to a Table object
 * @param row_cnt number of rows
 */
void edgui_table_set_row_cnt(edgui_obj_t * table, uint16_t row_cnt);

/**
 * Set the number of columns
 * @param table table pointer to a Table object
 * @param col_cnt number of columns. Must be < EDGUI_TABLE_COL_MAX
 */
void edgui_table_set_col_cnt(edgui_obj_t * table, uint16_t col_cnt);

/**
 * Set the width of a column
 * @param table table pointer to a Table object
 * @param col_id id of the column [0 .. EDGUI_TABLE_COL_MAX -1]
 * @param w width of the column
 */
void edgui_table_set_col_width(edgui_obj_t * table, uint16_t col_id, edgui_coord_t w);

/**
 * Set the text align in a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param align EDGUI_LABEL_ALIGN_LEFT or EDGUI_LABEL_ALIGN_CENTER or EDGUI_LABEL_ALIGN_RIGHT
 */
void edgui_table_set_cell_align(edgui_obj_t * table, uint16_t row, uint16_t col, edgui_label_align_t align);

/**
 * Set the type of a cell.
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param type 1,2,3 or 4. The cell style will be chosen accordingly.
 */
void edgui_table_set_cell_type(edgui_obj_t * table, uint16_t row, uint16_t col, uint8_t type);

/**
 * Set the cell crop. (Don't adjust the height of the cell according to its content)
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param crop true: crop the cell content; false: set the cell height to the content.
 */
void edgui_table_set_cell_crop(edgui_obj_t * table, uint16_t row, uint16_t col, bool crop);

/**
 * Merge a cell with the right neighbor. The value of the cell to the right won't be displayed.
 * @param table table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @param en true: merge right; false: don't merge right
 */
void edgui_table_set_cell_merge_right(edgui_obj_t * table, uint16_t row, uint16_t col, bool en);

/**
 * Set a style of a table.
 * @param table pointer to table object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_table_set_style(edgui_obj_t * table, edgui_table_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a cell.
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return text in the cell
 */
const char * edgui_table_get_cell_value(edgui_obj_t * table, uint16_t row, uint16_t col);

/**
 * Get the number of rows.
 * @param table table pointer to a Table object
 * @return number of rows.
 */
uint16_t edgui_table_get_row_cnt(edgui_obj_t * table);

/**
 * Get the number of columns.
 * @param table table pointer to a Table object
 * @return number of columns.
 */
uint16_t edgui_table_get_col_cnt(edgui_obj_t * table);

/**
 * Get the width of a column
 * @param table table pointer to a Table object
 * @param col_id id of the column [0 .. EDGUI_TABLE_COL_MAX -1]
 * @return width of the column
 */
edgui_coord_t edgui_table_get_col_width(edgui_obj_t * table, uint16_t col_id);

/**
 * Get the text align of a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return EDGUI_LABEL_ALIGN_LEFT (default in case of error) or EDGUI_LABEL_ALIGN_CENTER or
 * EDGUI_LABEL_ALIGN_RIGHT
 */
edgui_label_align_t edgui_table_get_cell_align(edgui_obj_t * table, uint16_t row, uint16_t col);

/**
 * Get the type of a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return 1,2,3 or 4
 */
edgui_label_align_t edgui_table_get_cell_type(edgui_obj_t * table, uint16_t row, uint16_t col);

/**
 * Get the crop property of a cell
 * @param table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return true: text crop enabled; false: disabled
 */
edgui_label_align_t edgui_table_get_cell_crop(edgui_obj_t * table, uint16_t row, uint16_t col);

/**
 * Get the cell merge attribute.
 * @param table table pointer to a Table object
 * @param row id of the row [0 .. row_cnt -1]
 * @param col id of the column [0 .. col_cnt -1]
 * @return true: merge right; false: don't merge right
 */
bool edgui_table_get_cell_merge_right(edgui_obj_t * table, uint16_t row, uint16_t col);

/**
 * Get style of a table.
 * @param table pointer to table object
 * @param type which style should be get
 * @return style pointer to the style
 */
const edgui_style_t * edgui_table_get_style(const edgui_obj_t * table, edgui_table_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TABLE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TABLE_H*/
