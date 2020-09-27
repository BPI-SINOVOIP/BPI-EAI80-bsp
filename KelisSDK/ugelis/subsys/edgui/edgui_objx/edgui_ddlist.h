/**
 * @file edgui_ddlist.h
 *
 */

#ifndef EDGUI_DDLIST_H
#define EDGUI_DDLIST_H

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

#if EDGUI_USE_DDLIST != 0

/*Testing of dependencies*/
#if EDGUI_USE_PAGE == 0
#error "edgui_ddlist: edgui_page is required. Enable it in edgui_conf.h (EDGUI_USE_PAGE  1) "
#endif

#if EDGUI_USE_LABEL == 0
#error "edgui_ddlist: edgui_label is required. Enable it in edgui_conf.h (EDGUI_USE_LABEL  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "../edgui_objx/edgui_page.h"
#include "../edgui_objx/edgui_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of drop down list*/
typedef struct
{
    edgui_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    edgui_obj_t * label;             /*Label for the options*/
    const edgui_style_t * sel_style; /*Style of the selected option*/
    uint16_t option_cnt;          /*Number of options*/
    uint16_t sel_opt_id;          /*Index of the current option*/
    uint16_t sel_opt_id_ori;      /*Store the original index on focus*/
    uint8_t opened : 1;           /*1: The list is opened (handled by the library)*/
    uint8_t force_sel : 1;        /*1: Keep the selection highlight even if the list is closed*/
    uint8_t draw_arrow : 1;       /*1: Draw arrow*/
    uint8_t stay_open : 1;        /*1: Don't close the list when a new item is selected*/
    edgui_coord_t fix_height;        /*Height of the ddlist when opened. (0: auto-size)*/
} edgui_ddlist_ext_t;

enum {
    EDGUI_DDLIST_STYLE_BG,
    EDGUI_DDLIST_STYLE_SEL,
    EDGUI_DDLIST_STYLE_SB,
};
typedef uint8_t edgui_ddlist_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 * Create a drop down list objects
 * @param par pointer to an object, it will be the parent of the new drop down list
 * @param copy pointer to a drop down list object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created drop down list
 */
edgui_obj_t * edgui_ddlist_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void edgui_ddlist_set_options(edgui_obj_t * ddlist, const char * options);

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void edgui_ddlist_set_selected(edgui_obj_t * ddlist, uint16_t sel_opt);

/**
 * Set a fix height for the drop down list
 * If 0 then the opened ddlist will be auto. sized else the set height will be applied.
 * @param ddlist pointer to a drop down list
 * @param h the height when the list is opened (0: auto size)
 */
void edgui_ddlist_set_fix_height(edgui_obj_t * ddlist, edgui_coord_t h);

/**
 * Set a fix width for the drop down list
 * @param ddlist pointer to a drop down list
 * @param w the width when the list is opened (0: auto size)
 */
void edgui_ddlist_set_fix_width(edgui_obj_t * ddlist, edgui_coord_t w);

/**
 * Set arrow draw in a drop down list
 * @param ddlist pointer to drop down list object
 * @param en enable/disable a arrow draw. E.g. "true" for draw.
 */
void edgui_ddlist_set_draw_arrow(edgui_obj_t * ddlist, bool en);

/**
 * Leave the list opened when a new value is selected
 * @param ddlist pointer to drop down list object
 * @param en enable/disable "stay open" feature
 */
void edgui_ddlist_set_stay_open(edgui_obj_t * ddlist, bool en);

/**
 * Set the scroll bar mode of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param sb_mode the new mode from 'edgui_page_sb_mode_t' enum
 */
static __inline void edgui_ddlist_set_sb_mode(edgui_obj_t * ddlist, edgui_sb_mode_t mode)
{
    edgui_page_set_sb_mode(ddlist, mode);
}
/**
 * Set the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @param anim_time: open/close animation time [ms]
 */
static __inline void edgui_ddlist_set_anim_time(edgui_obj_t * ddlist, uint16_t anim_time)
{
    edgui_page_set_anim_time(ddlist, anim_time);
}

/**
 * Set a style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void edgui_ddlist_set_style(edgui_obj_t * ddlist, edgui_ddlist_style_t type, const edgui_style_t * style);

/**
 * Set the alignment of the labels in a drop down list
 * @param ddlist pointer to a drop down list object
 * @param align alignment of labels
 */
void edgui_ddlist_set_align(edgui_obj_t * ddlist, edgui_label_align_t align);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the options of a drop down list
 * @param ddlist pointer to drop down list object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * edgui_ddlist_get_options(const edgui_obj_t * ddlist);

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t edgui_ddlist_get_selected(const edgui_obj_t * ddlist);

/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
void edgui_ddlist_get_selected_str(const edgui_obj_t * ddlist, char * buf, uint16_t buf_size);

/**
 * Get the fix height value.
 * @param ddlist pointer to a drop down list object
 * @return the height if the ddlist is opened (0: auto size)
 */
edgui_coord_t edgui_ddlist_get_fix_height(const edgui_obj_t * ddlist);

/**
 * Get arrow draw in a drop down list
 * @param ddlist pointer to drop down list object
 */
bool edgui_ddlist_get_draw_arrow(edgui_obj_t * ddlist);

/**
 * Get whether the drop down list stay open after selecting a  value or not
 * @param ddlist pointer to drop down list object
 */
bool edgui_ddlist_get_stay_open(edgui_obj_t * ddlist);

/**
 * Get the scroll bar mode of a drop down list
 * @param ddlist pointer to a  drop down list object
 * @return scrollbar mode from 'edgui_page_sb_mode_t' enum
 */
static __inline edgui_sb_mode_t edgui_ddlist_get_sb_mode(const edgui_obj_t * ddlist)
{
    return edgui_page_get_sb_mode(ddlist);
}

/**
 * Get the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @return open/close animation time [ms]
 */
static __inline uint16_t edgui_ddlist_get_anim_time(const edgui_obj_t * ddlist)
{
    return edgui_page_get_anim_time(ddlist);
}

/**
 * Get a style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_ddlist_get_style(const edgui_obj_t * ddlist, edgui_ddlist_style_t type);

/**
 * Get the alignment of the labels in a drop down list
 * @param ddlist pointer to a drop down list object
 * @return alignment of labels
 */
edgui_label_align_t edgui_ddlist_get_align(const edgui_obj_t * ddlist);

/*=====================
 * Other functions
 *====================*/

/**
 * Open the drop down list with or without animation
 * @param ddlist pointer to drop down list object
 * @param anim_en EDGUI_ANIM_ON: use animation; EDGUI_ANOM_OFF: not use animations
 */
void edgui_ddlist_open(edgui_obj_t * ddlist, edgui_anim_enable_t anim);

/**
 * Close (Collapse) the drop down list
 * @param ddlist pointer to drop down list object
 * @param anim_en EDGUI_ANIM_ON: use animation; EDGUI_ANOM_OFF: not use animations
 */
void edgui_ddlist_close(edgui_obj_t * ddlist, edgui_anim_enable_t anim);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_DDLIST*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_DDLIST_H*/
