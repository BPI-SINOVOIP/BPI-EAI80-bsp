/**
 * @file edgui_cb.h
 *
 */

#ifndef EDGUI_CB_H
#define EDGUI_CB_H

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

#if EDGUI_USE_CB != 0

/*Testing of dependencies*/
#if EDGUI_USE_BTN == 0
#error "edgui_cb: edgui_btn is required. Enable it in edgui_conf.h (EDGUI_USE_BTN  1) "
#endif

#if EDGUI_USE_LABEL == 0
#error "edgui_cb: edgui_label is required. Enable it in edgui_conf.h (EDGUI_USE_LABEL  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_btn.h"
#include "edgui_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of check box*/
typedef struct
{
    edgui_btn_ext_t bg_btn; /*Ext. of ancestor*/
    /*New data for this type */
    edgui_obj_t * bullet; /*Pointer to button*/
    edgui_obj_t * label;  /*Pointer to label*/
} edgui_cb_ext_t;

/** Checkbox styles. */
enum {
    EDGUI_CB_STYLE_BG, /**< Style of object background. */
    EDGUI_CB_STYLE_BOX_REL, /**< Style of box (released). */
    EDGUI_CB_STYLE_BOX_PR, /**< Style of box (pressed). */
    EDGUI_CB_STYLE_BOX_TGL_REL, /**< Style of box (released but checked). */
    EDGUI_CB_STYLE_BOX_TGL_PR, /**< Style of box (pressed and checked). */
    EDGUI_CB_STYLE_BOX_INA, /**< Style of disabled box */
};
typedef uint8_t edgui_cb_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a check box objects
 * @param par pointer to an object, it will be the parent of the new check box
 * @param copy pointer to a check box object, if not NULL then the new object will be copied from it
 * @return pointer to the created check box
 */
edgui_obj_t * edgui_cb_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a check box. `txt` will be copied and may be deallocated
 * after this function returns.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void edgui_cb_set_text(edgui_obj_t * cb, const char * txt);

/**
 * Set the text of a check box. `txt` must not be deallocated during the life
 * of this checkbox.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void edgui_cb_set_static_text(edgui_obj_t * cb, const char * txt);

/**
 * Set the state of the check box
 * @param cb pointer to a check box object
 * @param checked true: make the check box checked; false: make it unchecked
 */
static __inline void edgui_cb_set_checked(edgui_obj_t * cb, bool checked)
{
    edgui_btn_set_state(cb, checked ? EDGUI_BTN_STATE_TGL_REL : EDGUI_BTN_STATE_REL);
}

/**
 * Make the check box inactive (disabled)
 * @param cb pointer to a check box object
 */
static __inline void edgui_cb_set_inactive(edgui_obj_t * cb)
{
    edgui_btn_set_state(cb, EDGUI_BTN_STATE_INA);
}

/**
 * Set a style of a check box
 * @param cb pointer to check box object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void edgui_cb_set_style(edgui_obj_t * cb, edgui_cb_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param cb pointer to check box object
 * @return pointer to the text of the check box
 */
const char * edgui_cb_get_text(const edgui_obj_t * cb);

/**
 * Get the current state of the check box
 * @param cb pointer to a check box object
 * @return true: checked; false: not checked
 */
static __inline bool edgui_cb_is_checked(const edgui_obj_t * cb)
{
    return edgui_btn_get_state(cb) == EDGUI_BTN_STATE_REL ? false : true;
}

/**
 * Get whether the check box is inactive or not.
 * @param cb pointer to a check box object
 * @return true: inactive; false: not inactive
 */
static __inline bool edgui_cb_is_inactive(const edgui_obj_t * cb)
{
    return edgui_btn_get_state(cb) == EDGUI_BTN_STATE_INA ? false : true;
}

/**
 * Get a style of a button
 * @param cb pointer to check box object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const edgui_style_t * edgui_cb_get_style(const edgui_obj_t * cb, edgui_cb_style_t type);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_CB*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_CB_H*/
