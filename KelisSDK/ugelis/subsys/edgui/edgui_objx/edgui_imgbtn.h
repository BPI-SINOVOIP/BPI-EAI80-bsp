/**
 * @file edgui_imgbtn.h
 *
 */

#ifndef EDGUI_IMGBTN_H
#define EDGUI_IMGBTN_H

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

#if EDGUI_USE_IMGBTN != 0

/*Testing of dependencies*/
#if EDGUI_USE_BTN == 0
#error "edgui_imgbtn: edgui_btn is required. Enable it in edgui_conf.h (EDGUI_USE_BTN  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_btn.h"
#include "../edgui_draw/edgui_draw_img.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of image button*/
typedef struct
{
    edgui_btn_ext_t btn; /*Ext. of ancestor*/
    /*New data for this type */
#if EDGUI_IMGBTN_TILED == 0
    const void * img_src[_EDGUI_BTN_STATE_NUM]; /*Store images to each state*/
#else
    const void * img_src_left[_EDGUI_BTN_STATE_NUM];  /*Store left side images to each state*/
    const void * img_src_mid[_EDGUI_BTN_STATE_NUM];   /*Store center images to each state*/
    const void * img_src_right[_EDGUI_BTN_STATE_NUM]; /*Store right side images to each state*/
#endif
    edgui_img_cf_t act_cf; /*Color format of the currently active image*/
} edgui_imgbtn_ext_t;

/*Styles*/
enum {
    EDGUI_IMGBTN_STYLE_REL, /**< Same meaning as ordinary button styles. */
    EDGUI_IMGBTN_STYLE_PR,
    EDGUI_IMGBTN_STYLE_TGL_REL,
    EDGUI_IMGBTN_STYLE_TGL_PR,
    EDGUI_IMGBTN_STYLE_INA,
};
typedef uint8_t edgui_imgbtn_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a image button objects
 * @param par pointer to an object, it will be the parent of the new image button
 * @param copy pointer to a image button object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created image button
 */
edgui_obj_t * edgui_imgbtn_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

#if EDGUI_IMGBTN_TILED == 0
/**
 * Set images for a state of the image button
 * @param imgbtn pointer to an image button object
 * @param state for which state set the new image (from `edgui_btn_state_t`) `
 * @param src pointer to an image source (a C array or path to a file)
 */
void edgui_imgbtn_set_src(edgui_obj_t * imgbtn, edgui_btn_state_t state, const void * src);
#else
/**
 * Set images for a state of the image button
 * @param imgbtn pointer to an image button object
 * @param state for which state set the new image (from `edgui_btn_state_t`) `
 * @param src_left pointer to an image source for the left side of the button (a C array or path to
 * a file)
 * @param src_mid pointer to an image source for the middle of the button (ideally 1px wide) (a C
 * array or path to a file)
 * @param src_right pointer to an image source for the right side of the button (a C array or path
 * to a file)
 */
void edgui_imgbtn_set_src(edgui_obj_t * imgbtn, edgui_btn_state_t state, const void * src_left, const void * src_mid,
                       const void * src_right);

#endif

/**
 * Enable the toggled states. On release the button will change from/to toggled state.
 * @param imgbtn pointer to an image button object
 * @param tgl true: enable toggled states, false: disable
 */
static __inline void edgui_imgbtn_set_toggle(edgui_obj_t * imgbtn, bool tgl)
{
    edgui_btn_set_toggle(imgbtn, tgl);
}

/**
 * Set the state of the image button
 * @param imgbtn pointer to an image button object
 * @param state the new state of the button (from edgui_btn_state_t enum)
 */
static __inline void edgui_imgbtn_set_state(edgui_obj_t * imgbtn, edgui_btn_state_t state)
{
    edgui_btn_set_state(imgbtn, state);
}

/**
 * Toggle the state of the image button (ON->OFF, OFF->ON)
 * @param imgbtn pointer to a image button object
 */
static __inline void edgui_imgbtn_toggle(edgui_obj_t * imgbtn)
{
    edgui_btn_toggle(imgbtn);
}

/**
 * Set a style of a image button.
 * @param imgbtn pointer to image button object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_imgbtn_set_style(edgui_obj_t * imgbtn, edgui_imgbtn_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/

#if EDGUI_IMGBTN_TILED == 0
/**
 * Get the images in a  given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `edgui_btn_state_t`) `
 * @return pointer to an image source (a C array or path to a file)
 */
const void * edgui_imgbtn_get_src(edgui_obj_t * imgbtn, edgui_btn_state_t state);

#else

/**
 * Get the left image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `edgui_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * edgui_imgbtn_get_src_left(edgui_obj_t * imgbtn, edgui_btn_state_t state);

/**
 * Get the middle image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `edgui_btn_state_t`) `
 * @return pointer to the middle image source (a C array or path to a file)
 */
const void * edgui_imgbtn_get_src_middle(edgui_obj_t * imgbtn, edgui_btn_state_t state);

/**
 * Get the right image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `edgui_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * edgui_imgbtn_get_src_right(edgui_obj_t * imgbtn, edgui_btn_state_t state);

#endif
/**
 * Get the current state of the image button
 * @param imgbtn pointer to a image button object
 * @return the state of the button (from edgui_btn_state_t enum)
 */
static __inline edgui_btn_state_t edgui_imgbtn_get_state(const edgui_obj_t * imgbtn)
{
    return edgui_btn_get_state(imgbtn);
}

/**
 * Get the toggle enable attribute of the image button
 * @param imgbtn pointer to a image button object
 * @return ture: toggle enabled, false: disabled
 */
static __inline bool edgui_imgbtn_get_toggle(const edgui_obj_t * imgbtn)
{
    return edgui_btn_get_toggle(imgbtn);
}

/**
 * Get style of a image button.
 * @param imgbtn pointer to image button object
 * @param type which style should be get
 * @return style pointer to the style
 */
const edgui_style_t * edgui_imgbtn_get_style(const edgui_obj_t * imgbtn, edgui_imgbtn_style_t type);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_IMGBTN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_IMGBTN_H*/
