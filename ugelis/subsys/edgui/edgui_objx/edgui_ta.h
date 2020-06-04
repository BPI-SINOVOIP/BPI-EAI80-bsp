/**
 * @file edgui_ta.h
 *
 */

#ifndef EDGUI_TA_H
#define EDGUI_TA_H

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

#if EDGUI_USE_TA != 0

/*Testing of dependencies*/
#if EDGUI_USE_PAGE == 0
#error "edgui_ta: edgui_page is required. Enable it in edgui_conf.h (EDGUI_USE_PAGE  1) "
#endif

#if EDGUI_USE_LABEL == 0
#error "edgui_ta: edgui_label is required. Enable it in edgui_conf.h (EDGUI_USE_LABEL  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_page.h"
#include "edgui_label.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_TA_CURSOR_LAST (0x7FFF) /*Put the cursor after the last character*/

/**********************
 *      TYPEDEFS
 **********************/

/** Style of text area's cursor. */
enum {
    EDGUI_CURSOR_NONE, /**< No cursor */
    EDGUI_CURSOR_LINE, /**< Vertical line */
    EDGUI_CURSOR_BLOCK, /**< Rectangle */
    EDGUI_CURSOR_OUTLINE, /**< Outline around character */
    EDGUI_CURSOR_UNDERLINE, /**< Horizontal line under character */
    EDGUI_CURSOR_HIDDEN = 0x08, /**< This flag can be ORed to any of the other values to temporarily hide the cursor */
};
typedef uint8_t edgui_cursor_type_t;

/*Data of text area*/
typedef struct
{
    edgui_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    edgui_obj_t * label;            /*Label of the text area*/
    edgui_obj_t * placeholder;      /*Place holder label. only visible if text is an empty string*/
    char * pwd_tmp;              /*Used to store the original text in password mode*/
    const char * accapted_chars; /*Only these characters will be accepted. NULL: accept all*/
    uint16_t max_length;         /*The max. number of characters. 0: no limit*/
    uint16_t pwd_show_time;      /*Time to show characters in password mode before change them to '*' */
    struct
    {
        const edgui_style_t * style;  /* Style of the cursor (NULL to use label's style)*/
        edgui_coord_t valid_x;        /* Used when stepping up/down to a shorter line.
                                    * (Used by the library)*/
        uint16_t pos;              /* The current cursor position
                                    * (0: before 1st letter; 1: before 2nd letter ...)*/
        uint16_t blink_time;       /*Blink period*/
        edgui_area_t area;            /* Cursor area relative to the Text Area*/
        uint16_t txt_byte_pos;     /* Byte index of the letter after (on) the cursor*/
        edgui_cursor_type_t type : 4; /* Shape of the cursor*/
        uint8_t state : 1;         /*Cursor is visible now or not (Handled by the library)*/
        uint8_t click_pos : 1;     /*1: Enable positioning the cursor by clicking the text area*/
    } cursor;
#if EDGUI_LABEL_TEXT_SEL
    uint16_t tmp_sel_start;       /*Temporary value*/
    uint16_t tmp_sel_end;         /*Temporary value*/
    uint8_t text_sel_in_prog : 1; /*User is in process of selecting */
    uint8_t text_sel_en : 1;      /*Text can be selected on this text area*/
#endif
    uint8_t pwd_mode : 1; /*Replace characters with '*' */
    uint8_t one_line : 1; /*One line mode (ignore line breaks)*/
} edgui_ta_ext_t;

/** Possible text areas tyles. */
enum {
    EDGUI_TA_STYLE_BG, /**< Text area background style */
    EDGUI_TA_STYLE_SB, /**< Scrollbar style */
    EDGUI_TA_STYLE_CURSOR, /**< Cursor style */
    EDGUI_TA_STYLE_EDGE_FLASH, /**< Edge flash style */
    EDGUI_TA_STYLE_PLACEHOLDER, /**< Placeholder style */
};
typedef uint8_t edgui_ta_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a text area objects
 * @param par pointer to an object, it will be the parent of the new text area
 * @param copy pointer to a text area object, if not NULL then the new object will be copied from it
 * @return pointer to the created text area
 */
edgui_obj_t * edgui_ta_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Insert a character to the current cursor position.
 * To add a wide char, e.g. 'Á' use `edgui_txt_encoded_conv_wc('Á')`
 * @param ta pointer to a text area object
 * @param c a character (e.g. 'a')
 */
void edgui_ta_add_char(edgui_obj_t * ta, uint32_t c);

/**
 * Insert a text to the current cursor position
 * @param ta pointer to a text area object
 * @param txt a '\0' terminated string to insert
 */
void edgui_ta_add_text(edgui_obj_t * ta, const char * txt);

/**
 * Delete a the left character from the current cursor position
 * @param ta pointer to a text area object
 */
void edgui_ta_del_char(edgui_obj_t * ta);

/**
 * Delete the right character from the current cursor position
 * @param ta pointer to a text area object
 */
void edgui_ta_del_char_forward(edgui_obj_t * ta);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void edgui_ta_set_text(edgui_obj_t * ta, const char * txt);

/**
 * Set the placeholder text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void edgui_ta_set_placeholder_text(edgui_obj_t * ta, const char * txt);

/**
 * Set the cursor position
 * @param obj pointer to a text area object
 * @param pos the new cursor position in character index
 *             < 0 : index from the end of the text
 *             EDGUI_TA_CURSOR_LAST: go after the last character
 */
void edgui_ta_set_cursor_pos(edgui_obj_t * ta, int16_t pos);

/**
 * Set the cursor type.
 * @param ta pointer to a text area object
 * @param cur_type: element of 'edgui_cursor_type_t'
 */
void edgui_ta_set_cursor_type(edgui_obj_t * ta, edgui_cursor_type_t cur_type);

/**
 * Enable/Disable the positioning of the the cursor by clicking the text on the text area.
 * @param ta pointer to a text area object
 * @param en true: enable click positions; false: disable
 */
void edgui_ta_set_cursor_click_pos(edgui_obj_t * ta, bool en);

/**
 * Enable/Disable password mode
 * @param ta pointer to a text area object
 * @param en true: enable, false: disable
 */
void edgui_ta_set_pwd_mode(edgui_obj_t * ta, bool en);

/**
 * Configure the text area to one line or back to normal
 * @param ta pointer to a Text area object
 * @param en true: one line, false: normal
 */
void edgui_ta_set_one_line(edgui_obj_t * ta, bool en);

/**
 * Set the alignment of the text area.
 * In one line mode the text can be scrolled only with `EDGUI_LABEL_ALIGN_LEFT`.
 * This function should be called if the size of text area changes.
 * @param ta pointer to a text are object
 * @param align the desired alignment from `edgui_label_align_t`. (EDGUI_LABEL_ALIGN_LEFT/CENTER/RIGHT)
 */
void edgui_ta_set_text_align(edgui_obj_t * ta, edgui_label_align_t align);

/**
 * Set a list of characters. Only these characters will be accepted by the text area
 * @param ta pointer to  Text Area
 * @param list list of characters. Only the pointer is saved. E.g. "+-.,0123456789"
 */
void edgui_ta_set_accepted_chars(edgui_obj_t * ta, const char * list);

/**
 * Set max length of a Text Area.
 * @param ta pointer to  Text Area
 * @param num the maximal number of characters can be added (`edgui_ta_set_text` ignores it)
 */
void edgui_ta_set_max_length(edgui_obj_t * ta, uint16_t num);

/**
 * In `EDGUI_EVENT_INSERT` the text which planned to be inserted can be replaced by an other text.
 * It can be used to add automatic formatting to the text area.
 * @param ta pointer to a text area.
 * @param txt pointer to a new string to insert. If `""` no text will be added.
 *            The variable must be live after the `event_cb` exists. (Should be `global` or
 * `static`)
 */
void edgui_ta_set_insert_replace(edgui_obj_t * ta, const char * txt);

/**
 * Set the scroll bar mode of a text area
 * @param ta pointer to a text area object
 * @param sb_mode the new mode from 'edgui_page_sb_mode_t' enum
 */
static __inline void edgui_ta_set_sb_mode(edgui_obj_t * ta, edgui_sb_mode_t mode)
{
    edgui_page_set_sb_mode(ta, mode);
}

/**
 * Enable the scroll propagation feature. If enabled then the Text area will move its parent if
 * there is no more space to scroll.
 * @param ta pointer to a Text area
 * @param en true or false to enable/disable scroll propagation
 */
static __inline void edgui_ta_set_scroll_propagation(edgui_obj_t * ta, bool en)
{
    edgui_page_set_scroll_propagation(ta, en);
}

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param page pointer to a Text Area
 * @param en true or false to enable/disable end flash
 */
static __inline void edgui_ta_set_edge_flash(edgui_obj_t * ta, bool en)
{
    edgui_page_set_edge_flash(ta, en);
}

/**
 * Set a style of a text area
 * @param ta pointer to a text area object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_ta_set_style(edgui_obj_t * ta, edgui_ta_style_t type, const edgui_style_t * style);

/**
 * Enable/disable selection mode.
 * @param ta pointer to a text area object
 * @param en true or false to enable/disable selection mode
 */
void edgui_ta_set_text_sel(edgui_obj_t * ta, bool en);

/**
 * Set how long show the password before changing it to '*'
 * @param ta pointer to Text area
 * @param time show time in milliseconds. 0: hide immediately.
 */
void edgui_ta_set_pwd_show_time(edgui_obj_t * ta, uint16_t time);

/**
 * Set cursor blink animation time
 * @param ta pointer to Text area
 * @param time blink period. 0: disable blinking
 */
void edgui_ta_set_cursor_blink_time(edgui_obj_t * ta, uint16_t time);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a text area. In password mode it gives the real text (not '*'s).
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * edgui_ta_get_text(const edgui_obj_t * ta);

/**
 * Get the placeholder text of a text area
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * edgui_ta_get_placeholder_text(edgui_obj_t * ta);

/**
 * Get the label of a text area
 * @param ta pointer to a text area object
 * @return pointer to the label object
 */
edgui_obj_t * edgui_ta_get_label(const edgui_obj_t * ta);

/**
 * Get the current cursor position in character index
 * @param ta pointer to a text area object
 * @return the cursor position
 */
uint16_t edgui_ta_get_cursor_pos(const edgui_obj_t * ta);

/**
 * Get the current cursor type.
 * @param ta pointer to a text area object
 * @return element of 'edgui_cursor_type_t'
 */
edgui_cursor_type_t edgui_ta_get_cursor_type(const edgui_obj_t * ta);

/**
 * Get whether the cursor click positioning is enabled or not.
 * @param ta pointer to a text area object
 * @return true: enable click positions; false: disable
 */
bool edgui_ta_get_cursor_click_pos(edgui_obj_t * ta);

/**
 * Get the password mode attribute
 * @param ta pointer to a text area object
 * @return true: password mode is enabled, false: disabled
 */
bool edgui_ta_get_pwd_mode(const edgui_obj_t * ta);

/**
 * Get the one line configuration attribute
 * @param ta pointer to a text area object
 * @return true: one line configuration is enabled, false: disabled
 */
bool edgui_ta_get_one_line(const edgui_obj_t * ta);

/**
 * Get a list of accepted characters.
 * @param ta pointer to  Text Area
 * @return list of accented characters.
 */
const char * edgui_ta_get_accepted_chars(edgui_obj_t * ta);

/**
 * Set max length of a Text Area.
 * @param ta pointer to  Text Area
 * @return the maximal number of characters to be add
 */
uint16_t edgui_ta_get_max_length(edgui_obj_t * ta);

/**
 * Get the scroll bar mode of a text area
 * @param ta pointer to a text area object
 * @return scrollbar mode from 'edgui_page_sb_mode_t' enum
 */
static __inline edgui_sb_mode_t edgui_ta_get_sb_mode(const edgui_obj_t * ta)
{
    return edgui_page_get_sb_mode(ta);
}

/**
 * Get the scroll propagation property
 * @param ta pointer to a Text area
 * @return true or false
 */
static __inline bool edgui_ta_get_scroll_propagation(edgui_obj_t * ta)
{
    return edgui_page_get_scroll_propagation(ta);
}

/**
 * Get the scroll propagation property
 * @param ta pointer to a Text area
 * @return true or false
 */
static __inline bool edgui_ta_get_edge_flash(edgui_obj_t * ta)
{
    return edgui_page_get_edge_flash(ta);
}

/**
 * Get a style of a text area
 * @param ta pointer to a text area object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_ta_get_style(const edgui_obj_t * ta, edgui_ta_style_t type);

/**
 * Find whether text is selected or not.
 * @param ta Text area object
 * @return whether text is selected or not
 */
bool edgui_ta_text_is_selected(const edgui_obj_t * ta);

/**
 * Find whether selection mode is enabled.
 * @param ta pointer to a text area object
 * @return true: selection mode is enabled, false: disabled
 */
bool edgui_ta_get_text_sel_en(edgui_obj_t * ta);

/**
 * Set how long show the password before changing it to '*'
 * @param ta pointer to Text area
 * @return show time in milliseconds. 0: hide immediately.
 */
uint16_t edgui_ta_get_pwd_show_time(edgui_obj_t * ta);

/**
 * Set cursor blink animation time
 * @param ta pointer to Text area
 * @return time blink period. 0: disable blinking
 */
uint16_t edgui_ta_get_cursor_blink_time(edgui_obj_t * ta);

/*=====================
 * Other functions
 *====================*/

/**
 * Clear the selection on the text area.
 * @param ta Text area object
 */
void edgui_ta_clear_selection(edgui_obj_t * ta);

/**
 * Move the cursor one character right
 * @param ta pointer to a text area object
 */
void edgui_ta_cursor_right(edgui_obj_t * ta);

/**
 * Move the cursor one character left
 * @param ta pointer to a text area object
 */
void edgui_ta_cursor_left(edgui_obj_t * ta);

/**
 * Move the cursor one line down
 * @param ta pointer to a text area object
 */
void edgui_ta_cursor_down(edgui_obj_t * ta);

/**
 * Move the cursor one line up
 * @param ta pointer to a text area object
 */
void edgui_ta_cursor_up(edgui_obj_t * ta);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_TA_H*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TA_H*/
