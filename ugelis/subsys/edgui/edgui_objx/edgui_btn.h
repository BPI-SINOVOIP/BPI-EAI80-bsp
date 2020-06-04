/**
 * @file edgui_btn.h
 *
 */

#ifndef EDGUI_BTN_H
#define EDGUI_BTN_H

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

#if EDGUI_USE_BTN != 0

/*Testing of dependencies*/
#if EDGUI_USE_CONT == 0
#error "edgui_btn: edgui_cont is required. Enable it in edgui_conf.h (EDGUI_USE_CONT  1) "
#endif

#include "edgui_cont.h"
#include "../edgui_core/edgui_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Possible states of a button.
 * It can be used not only by buttons but other button-like objects too*/
enum {
    /**Released*/
    EDGUI_BTN_STATE_REL,

    /**Pressed*/
    EDGUI_BTN_STATE_PR,

    /**Toggled released*/
    EDGUI_BTN_STATE_TGL_REL,

    /**Toggled pressed*/
    EDGUI_BTN_STATE_TGL_PR,

    /**Inactive*/
    EDGUI_BTN_STATE_INA,

    /**Number of states*/
    _EDGUI_BTN_STATE_NUM,
};
typedef uint8_t edgui_btn_state_t;

/** Extended data of button*/
typedef struct
{
    /** Ext. of ancestor*/
    edgui_cont_ext_t cont;

    /*New data for this type */

    /**Styles in each state*/
    const edgui_style_t * styles[_EDGUI_BTN_STATE_NUM];
#if EDGUI_BTN_INK_EFFECT
    /** [ms] Time of ink fill effect (0: disable ink effect)*/
    uint16_t ink_in_time;

    /** [ms] Wait before the ink disappears */
    uint16_t ink_wait_time;

    /** [ms] Time of ink disappearing*/
    uint16_t ink_out_time;
#endif

    /** Current state of the button from 'edgui_btn_state_t' enum*/
    edgui_btn_state_t state : 3;

    /** 1: Toggle enabled*/
    uint8_t toggle : 1;
} edgui_btn_ext_t;

/**Styles*/
enum {
    /** Release style */
    EDGUI_BTN_STYLE_REL,

    /**Pressed style*/
    EDGUI_BTN_STYLE_PR,

    /** Toggle released style*/
    EDGUI_BTN_STYLE_TGL_REL,

    /** Toggle pressed style */
    EDGUI_BTN_STYLE_TGL_PR,

    /** Inactive style*/
    EDGUI_BTN_STYLE_INA,
};
typedef uint8_t edgui_btn_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a button object
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
edgui_obj_t * edgui_btn_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Enable the toggled states. On release the button will change from/to toggled state.
 * @param btn pointer to a button object
 * @param tgl true: enable toggled states, false: disable
 */
void edgui_btn_set_toggle(edgui_obj_t * btn, bool tgl);

/**
 * Set the state of the button
 * @param btn pointer to a button object
 * @param state the new state of the button (from edgui_btn_state_t enum)
 */
void edgui_btn_set_state(edgui_obj_t * btn, edgui_btn_state_t state);

/**
 * Toggle the state of the button (ON->OFF, OFF->ON)
 * @param btn pointer to a button object
 */
void edgui_btn_toggle(edgui_obj_t * btn);

/**
 * Set the layout on a button
 * @param btn pointer to a button object
 * @param layout a layout from 'edgui_cont_layout_t'
 */
static __inline void edgui_btn_set_layout(edgui_obj_t * btn, edgui_layout_t layout)
{
    edgui_cont_set_layout(btn, layout);
}

/**
 * Set the fit policy in all 4 directions separately.
 * It tells how to change the button size automatically.
 * @param btn pointer to a button object
 * @param left left fit policy from `edgui_fit_t`
 * @param right right fit policy from `edgui_fit_t`
 * @param top top fit policy from `edgui_fit_t`
 * @param bottom bottom fit policy from `edgui_fit_t`
 */
static __inline void edgui_btn_set_fit4(edgui_obj_t * btn, edgui_fit_t left, edgui_fit_t right, edgui_fit_t top, edgui_fit_t bottom)
{
    edgui_cont_set_fit4(btn, left, right, top, bottom);
}

/**
 * Set the fit policy horizontally and vertically separately.
 * It tells how to change the button size automatically.
 * @param btn pointer to a button object
 * @param hor horizontal fit policy from `edgui_fit_t`
 * @param ver vertical fit policy from `edgui_fit_t`
 */
static __inline void edgui_btn_set_fit2(edgui_obj_t * btn, edgui_fit_t hor, edgui_fit_t ver)
{
    edgui_cont_set_fit2(btn, hor, ver);
}

/**
 * Set the fit policy in all 4 direction at once.
 * It tells how to change the button size automatically.
 * @param btn pointer to a button object
 * @param fit fit policy from `edgui_fit_t`
 */
static __inline void edgui_btn_set_fit(edgui_obj_t * btn, edgui_fit_t fit)
{
    edgui_cont_set_fit(btn, fit);
}

/**
 * Set time of the ink effect (draw a circle on click to animate in the new state)
 * @param btn pointer to a button object
 * @param time the time of the ink animation
 */
void edgui_btn_set_ink_in_time(edgui_obj_t * btn, uint16_t time);

/**
 * Set the wait time before the ink disappears
 * @param btn pointer to a button object
 * @param time the time of the ink animation
 */
void edgui_btn_set_ink_wait_time(edgui_obj_t * btn, uint16_t time);

/**
 * Set time of the ink out effect (animate to the released state)
 * @param btn pointer to a button object
 * @param time the time of the ink animation
 */
void edgui_btn_set_ink_out_time(edgui_obj_t * btn, uint16_t time);

/**
 * Set a style of a button.
 * @param btn pointer to button object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void edgui_btn_set_style(edgui_obj_t * btn, edgui_btn_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current state of the button
 * @param btn pointer to a button object
 * @return the state of the button (from edgui_btn_state_t enum)
 */
edgui_btn_state_t edgui_btn_get_state(const edgui_obj_t * btn);

/**
 * Get the toggle enable attribute of the button
 * @param btn pointer to a button object
 * @return true: toggle enabled, false: disabled
 */
bool edgui_btn_get_toggle(const edgui_obj_t * btn);

/**
 * Get the layout of a button
 * @param btn pointer to button object
 * @return the layout from 'edgui_cont_layout_t'
 */
static __inline edgui_layout_t edgui_btn_get_layout(const edgui_obj_t * btn)
{
    return edgui_cont_get_layout(btn);
}

/**
 * Get the left fit mode
 * @param btn pointer to a button object
 * @return an element of `edgui_fit_t`
 */
static __inline edgui_fit_t edgui_btn_get_fit_left(const edgui_obj_t * btn)
{
    return edgui_cont_get_fit_left(btn);
}

/**
 * Get the right fit mode
 * @param btn pointer to a button object
 * @return an element of `edgui_fit_t`
 */
static __inline edgui_fit_t edgui_btn_get_fit_right(const edgui_obj_t * btn)
{
    return edgui_cont_get_fit_right(btn);
}

/**
 * Get the top fit mode
 * @param btn pointer to a button object
 * @return an element of `edgui_fit_t`
 */
static __inline edgui_fit_t edgui_btn_get_fit_top(const edgui_obj_t * btn)
{
    return edgui_cont_get_fit_top(btn);
}

/**
 * Get the bottom fit mode
 * @param btn pointer to a button object
 * @return an element of `edgui_fit_t`
 */
static __inline edgui_fit_t edgui_btn_get_fit_bottom(const edgui_obj_t * btn)
{
    return edgui_cont_get_fit_bottom(btn);
}

/**
 * Get time of the ink in effect (draw a circle on click to animate in the new state)
 * @param btn pointer to a button object
 * @return the time of the ink animation
 */
uint16_t edgui_btn_get_ink_in_time(const edgui_obj_t * btn);

/**
 * Get the wait time before the ink disappears
 * @param btn pointer to a button object
 * @return the time of the ink animation
 */
uint16_t edgui_btn_get_ink_wait_time(const edgui_obj_t * btn);

/**
 * Get time of the ink out effect (animate to the releases state)
 * @param btn pointer to a button object
 * @return the time of the ink animation
 */
uint16_t edgui_btn_get_ink_out_time(const edgui_obj_t * btn);

/**
 * Get style of a button.
 * @param btn pointer to button object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const edgui_style_t * edgui_btn_get_style(const edgui_obj_t * btn, edgui_btn_style_t type);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_BUTTON*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_BTN_H*/
