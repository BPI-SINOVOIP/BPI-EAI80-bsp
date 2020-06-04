/**
 * @file edgui_mbox.h
 *
 */

#ifndef EDGUI_MBOX_H
#define EDGUI_MBOX_H

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

#if EDGUI_USE_MBOX != 0

/*Testing of dependencies*/
#if EDGUI_USE_CONT == 0
#error "edgui_mbox: edgui_cont is required. Enable it in edgui_conf.h (EDGUI_USE_CONT  1) "
#endif

#if EDGUI_USE_BTNM == 0
#error "edgui_mbox: edgui_btnm is required. Enable it in edgui_conf.h (EDGUI_USE_BTNM  1) "
#endif

#if EDGUI_USE_LABEL == 0
#error "edgui_mbox: edgui_label is required. Enable it in edgui_conf.h (EDGUI_USE_LABEL  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_cont.h"
#include "edgui_btnm.h"
#include "edgui_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of message box*/
typedef struct
{
    edgui_cont_ext_t bg; /*Ext. of ancestor*/
    /*New data for this type */
    edgui_obj_t * text; /*Text of the message box*/
    edgui_obj_t * btnm; /*Button matrix for the buttons*/
#if EDGUI_USE_ANIMATION
    uint16_t anim_time; /*Duration of close animation [ms] (0: no animation)*/
#endif
} edgui_mbox_ext_t;

/** Message box styles. */
enum {
    EDGUI_MBOX_STYLE_BG,
    EDGUI_MBOX_STYLE_BTN_BG, /**< Same meaning as ordinary button styles. */
    EDGUI_MBOX_STYLE_BTN_REL,
    EDGUI_MBOX_STYLE_BTN_PR,
    EDGUI_MBOX_STYLE_BTN_TGL_REL,
    EDGUI_MBOX_STYLE_BTN_TGL_PR,
    EDGUI_MBOX_STYLE_BTN_INA,
};
typedef uint8_t edgui_mbox_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a message box objects
 * @param par pointer to an object, it will be the parent of the new message box
 * @param copy pointer to a message box object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created message box
 */
edgui_obj_t * edgui_mbox_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add button to the message box
 * @param mbox pointer to message box object
 * @param btn_map button descriptor (button matrix map).
 *                E.g.  a const char *txt[] = {"ok", "close", ""} (Can not be local variable)
 */
void edgui_mbox_add_btns(edgui_obj_t * mbox, const char ** btn_mapaction);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of the message box
 * @param mbox pointer to a message box
 * @param txt a '\0' terminated character string which will be the message box text
 */
void edgui_mbox_set_text(edgui_obj_t * mbox, const char * txt);

/**
 * Set animation duration
 * @param mbox pointer to a message box object
 * @param anim_time animation length in  milliseconds (0: no animation)
 */
void edgui_mbox_set_anim_time(edgui_obj_t * mbox, uint16_t anim_time);

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param delay a time (in milliseconds) to wait before delete the message box
 */
void edgui_mbox_start_auto_close(edgui_obj_t * mbox, uint16_t delay);

/**
 * Stop the auto. closing of message box
 * @param mbox pointer to a message box object
 */
void edgui_mbox_stop_auto_close(edgui_obj_t * mbox);

/**
 * Set a style of a message box
 * @param mbox pointer to a message box object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_mbox_set_style(edgui_obj_t * mbox, edgui_mbox_style_t type, const edgui_style_t * style);

/**
 * Set whether recoloring is enabled. Must be called after `edgui_mbox_add_btns`.
 * @param btnm pointer to button matrix object
 * @param en whether recoloring is enabled
 */
void edgui_mbox_set_recolor(edgui_obj_t * mbox, bool en);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the text of the message box
 */
const char * edgui_mbox_get_text(const edgui_obj_t * mbox);

/**
 * Get the index of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`.
 * @param btnm pointer to button matrix object
 * @return  index of the last released button (EDGUI_BTNM_BTN_NONE: if unset)
 */
uint16_t edgui_mbox_get_active_btn(edgui_obj_t * mbox);

/**
 * Get the text of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`.
 * @param btnm pointer to button matrix object
 * @return text of the last released button (NULL: if unset)
 */
const char * edgui_mbox_get_active_btn_text(edgui_obj_t * mbox);

/**
 * Get the animation duration (close animation time)
 * @param mbox pointer to a message box object
 * @return animation length in  milliseconds (0: no animation)
 */
uint16_t edgui_mbox_get_anim_time(const edgui_obj_t * mbox);

/**
 * Get a style of a message box
 * @param mbox pointer to a message box object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_mbox_get_style(const edgui_obj_t * mbox, edgui_mbox_style_t type);

/**
 * Get whether recoloring is enabled
 * @param mbox pointer to a message box object
 * @return whether recoloring is enabled
 */
bool edgui_mbox_get_recolor(const edgui_obj_t * mbox);

/**
 * Get message box button matrix
 * @param mbox pointer to a message box object
 * @return pointer to button matrix object
 * @remarks return value will be NULL unless `edgui_mbox_add_btns` has been already called
 */
edgui_obj_t * edgui_mbox_get_btnm(edgui_obj_t * mbox);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_MBOX*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_MBOX_H*/
