/**
 * @file edgui_obj.h
 *
 */

#ifndef EDGUI_OBJ_H
#define EDGUI_OBJ_H

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

#include <stddef.h>
#include <stdbool.h>
#include "edgui_style.h"
#include "../edgui_misc/edgui_types.h"
#include "../edgui_misc/edgui_area.h"
#include "../edgui_misc/edgui_mem.h"
#include "../edgui_misc/edgui_ll.h"
#include "../edgui_misc/edgui_color.h"
#include "../edgui_misc/edgui_log.h"
#include "../edgui_hal/edgui_hal.h"

/*********************
 *      DEFINES
 *********************/

/*Error check of edgui_conf.h*/
#if EDGUI_HOR_RES_MAX == 0 || EDGUI_VER_RES_MAX == 0
#error "edgui: EDGUI_HOR_RES_MAX and EDGUI_VER_RES_MAX must be greater than 0"
#endif

#if EDGUI_ANTIALIAS > 1
#error "edgui: EDGUI_ANTIALIAS can be only 0 or 1"
#endif

#define EDGUI_MAX_ANCESTOR_NUM 8

#define EDGUI_EXT_CLICK_AREA_OFF 0
#define EDGUI_EXT_CLICK_AREA_TINY 1
#define EDGUI_EXT_CLICK_AREA_FULL 2

/**********************
 *      TYPEDEFS
 **********************/

struct _edgui_obj_t;


/** Design modes */
enum {
    EDGUI_DESIGN_DRAW_MAIN, /**< Draw the main portion of the object */
    EDGUI_DESIGN_DRAW_POST, /**< Draw extras on the object */
    EDGUI_DESIGN_COVER_CHK, /**< Check if the object fully covers the 'mask_p' area */
};
typedef uint8_t edgui_design_mode_t;

/**
 * The design callback is used to draw the object on the screen.
 * It accepts the object, a mask area, and the mode in which to draw the object.
 */
typedef bool (*edgui_design_cb_t)(struct _edgui_obj_t * obj, const edgui_area_t * mask_p, edgui_design_mode_t mode);

enum {
    EDGUI_EVENT_PRESSED,             /**< The object has been pressed*/
    EDGUI_EVENT_PRESSING,            /**< The object is being pressed (called continuously while pressing)*/
    EDGUI_EVENT_PRESS_LOST,          /**< User is still pressing but slid cursor/finger off of the object */
    EDGUI_EVENT_SHORT_CLICKED,       /**< User pressed object for a short period of time, then released it. Not called if dragged. */
    EDGUI_EVENT_LONG_PRESSED,        /**< Object has been pressed for at least `EDGUI_INDEV_LONG_PRESS_TIME`.  Not called if dragged.*/
    EDGUI_EVENT_LONG_PRESSED_REPEAT, /**< Called after `EDGUI_INDEV_LONG_PRESS_TIME` in every
                                       `EDGUI_INDEV_LONG_PRESS_REP_TIME` ms.  Not called if dragged.*/
    EDGUI_EVENT_CLICKED,             /**< Called on release if not dragged (regardless to long press)*/
    EDGUI_EVENT_RELEASED,            /**< Called in every cases when the object has been released*/                                    
    EDGUI_EVENT_DRAG_BEGIN,		  
    EDGUI_EVENT_DRAG_END,
    EDGUI_EVENT_DRAG_THROW_BEGIN,
    EDGUI_EVENT_KEY,
    EDGUI_EVENT_FOCUSED,
    EDGUI_EVENT_DEFOCUSED,
    EDGUI_EVENT_VALUE_CHANGED,		 /**< The object's value has changed (i.e. slider moved) */
    EDGUI_EVENT_INSERT,
    EDGUI_EVENT_REFRESH,
    EDGUI_EVENT_APPLY,  /**< "Ok", "Apply" or similar specific button has clicked*/
    EDGUI_EVENT_CANCEL, /**< "Close", "Cancel" or similar specific button has clicked*/
    EDGUI_EVENT_DELETE, /**< Object is being deleted */
};
typedef uint8_t edgui_event_t; /**< Type of event being sent to the object. */

/**
 * @brief Event callback.
 * Events are used to notify the user of some action being taken on the object.
 * For details, see ::edgui_event_t.
 */
typedef void (*edgui_event_cb_t)(struct _edgui_obj_t * obj, edgui_event_t event);

/** Signals are for use by the object itself or to extend the object's functionality.
  * Applications should use ::edgui_obj_set_event_cb to be notified of events that occur
  * on the object. */
enum {
    /*General signals*/
    EDGUI_SIGNAL_CLEANUP, /**< Object is being deleted */
    EDGUI_SIGNAL_CHILD_CHG, /**< Child was removed/added */
    EDGUI_SIGNAL_CORD_CHG, /**< Object coordinates/size have changed */
    EDGUI_SIGNAL_PARENT_SIZE_CHG, /**< Parent's size has changed */
    EDGUI_SIGNAL_STYLE_CHG, /**< Object's style has changed */
    EDGUI_SIGNAL_REFR_EXT_DRAW_PAD, /**< Object's extra padding has changed */
    EDGUI_SIGNAL_GET_TYPE, /**< edgui needs to retrieve the object's type */

    /*Input device related*/
    EDGUI_SIGNAL_PRESSED,           /**< The object has been pressed*/
    EDGUI_SIGNAL_PRESSING,          /**< The object is being pressed (called continuously while pressing)*/
    EDGUI_SIGNAL_PRESS_LOST,        /**< User is still pressing but slid cursor/finger off of the object */
    EDGUI_SIGNAL_RELEASED,          /**< User pressed object for a short period of time, then released it. Not called if dragged. */
    EDGUI_SIGNAL_LONG_PRESS,        /**< Object has been pressed for at least `EDGUI_INDEV_LONG_PRESS_TIME`.  Not called if dragged.*/
    EDGUI_SIGNAL_LONG_PRESS_REP,    /**< Called after `EDGUI_INDEV_LONG_PRESS_TIME` in every `EDGUI_INDEV_LONG_PRESS_REP_TIME` ms.  Not called if dragged.*/
    EDGUI_SIGNAL_DRAG_BEGIN,	
    EDGUI_SIGNAL_DRAG_END,                                   
    /*Group related*/
    EDGUI_SIGNAL_FOCUS,
    EDGUI_SIGNAL_DEFOCUS,
    EDGUI_SIGNAL_CONTROL,
    EDGUI_SIGNAL_GET_EDITABLE,
};
typedef uint8_t edgui_signal_t;

typedef edgui_res_t (*edgui_signal_cb_t)(struct _edgui_obj_t * obj, edgui_signal_t sign, void * param);

/** Object alignment. */
enum {
    EDGUI_ALIGN_CENTER = 0,
    EDGUI_ALIGN_IN_TOP_LEFT,
    EDGUI_ALIGN_IN_TOP_MID,
    EDGUI_ALIGN_IN_TOP_RIGHT,
    EDGUI_ALIGN_IN_BOTTOM_LEFT,
    EDGUI_ALIGN_IN_BOTTOM_MID,
    EDGUI_ALIGN_IN_BOTTOM_RIGHT,
    EDGUI_ALIGN_IN_LEFT_MID,
    EDGUI_ALIGN_IN_RIGHT_MID,
    EDGUI_ALIGN_OUT_TOP_LEFT,
    EDGUI_ALIGN_OUT_TOP_MID,
    EDGUI_ALIGN_OUT_TOP_RIGHT,
    EDGUI_ALIGN_OUT_BOTTOM_LEFT,
    EDGUI_ALIGN_OUT_BOTTOM_MID,
    EDGUI_ALIGN_OUT_BOTTOM_RIGHT,
    EDGUI_ALIGN_OUT_LEFT_TOP,
    EDGUI_ALIGN_OUT_LEFT_MID,
    EDGUI_ALIGN_OUT_LEFT_BOTTOM,
    EDGUI_ALIGN_OUT_RIGHT_TOP,
    EDGUI_ALIGN_OUT_RIGHT_MID,
    EDGUI_ALIGN_OUT_RIGHT_BOTTOM,
};
typedef uint8_t edgui_align_t;

#if EDGUI_USE_OBJ_REALIGN
typedef struct
{
    const struct _edgui_obj_t * base;
    edgui_coord_t xofs;
    edgui_coord_t yofs;
    edgui_align_t align;
    uint8_t auto_realign : 1;
    uint8_t origo_align : 1; /**< 1: the origo (center of the object) was aligned with
                                `edgui_obj_align_origo`*/
} edgui_reailgn_t;
#endif

enum {
    EDGUI_DRAG_DIR_HOR = 0x1, /**< Object can be dragged horizontally. */
    EDGUI_DRAG_DIR_VER = 0x2, /**< Object can be dragged vertically. */
    EDGUI_DRAG_DIR_ALL = 0x3, /**< Object can be dragged in all directions. */
};

typedef uint8_t edgui_drag_dir_t;

typedef struct _edgui_obj_t
{
    struct _edgui_obj_t * par; /**< Pointer to the parent object*/
    edgui_ll_t child_ll;       /**< Linked list to store the children objects*/

    edgui_area_t coords; /**< Coordinates of the object (x1, y1, x2, y2)*/

    edgui_event_cb_t event_cb; /**< Event callback function */
    edgui_signal_cb_t signal_cb; /**< Object type specific signal function*/
    edgui_design_cb_t design_cb; /**< Object type specific design function*/

    void * ext_attr;            /**< Object type specific extended data*/
    const edgui_style_t * style_p; /**< Pointer to the object's style*/

#if EDGUI_USE_GROUP != 0
    void * group_p; /**< Pointer to the group of the object*/
#endif

#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_TINY
    uint8_t ext_click_pad_hor; /**< Extra click padding in horizontal direction */
    uint8_t ext_click_pad_ver; /**< Extra click padding in vertical direction */
#endif

#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_FULL
    edgui_area_t ext_click_pad;   /**< Extra click padding area. */
#endif

    /*Attributes and states*/
    uint8_t click : 1;          /**< 1: Can be pressed by an input device*/
    uint8_t drag : 1;           /**< 1: Enable the dragging*/
    uint8_t drag_throw : 1;     /**< 1: Enable throwing with drag*/
    uint8_t drag_parent : 1;    /**< 1: Parent will be dragged instead*/
    uint8_t hidden : 1;         /**< 1: Object is hidden*/
    uint8_t top : 1;            /**< 1: If the object or its children is clicked it goes to the foreground*/
    uint8_t opa_scale_en : 1;   /**< 1: opa_scale is set*/
    uint8_t parent_event : 1;   /**< 1: Send the object's events to the parent too. */
    edgui_drag_dir_t drag_dir : 2; /**<  Which directions the object can be dragged in */
    uint8_t reserved : 6;       /**<  Reserved for future use*/
    uint8_t protect;            /**< Automatically happening actions can be prevented. 'OR'ed values from
                                   `edgui_protect_t`*/
    edgui_opa_t opa_scale;         /**< Scale down the opacity by this factor. Effects all children as well*/

    edgui_coord_t ext_draw_pad; /**< EXTtend the size in every direction for drawing. */

#if EDGUI_USE_OBJ_REALIGN
    edgui_reailgn_t realign;       /**< Information about the last call to ::edgui_obj_align. */
#endif

#if EDGUI_USE_USER_DATA
    edgui_obj_user_data_t user_data; /**< Custom user data for object. */
#endif

} edgui_obj_t;

/*Protect some attributes (max. 8 bit)*/
enum {
    EDGUI_PROTECT_NONE      = 0x00,
    EDGUI_PROTECT_CHILD_CHG = 0x01,   /**< Disable the child change signal. Used by the library*/
    EDGUI_PROTECT_PARENT    = 0x02,   /**< Prevent automatic parent change (e.g. in edgui_page)*/
    EDGUI_PROTECT_POS       = 0x04,   /**< Prevent automatic positioning (e.g. in edgui_cont layout)*/
    EDGUI_PROTECT_FOLLOW    = 0x08,   /**< Prevent the object be followed in automatic ordering (e.g. in
                                      edgui_cont PRETTY layout)*/
    EDGUI_PROTECT_PRESS_LOST = 0x10,  /**< If the `indev` was pressing this object but swiped out while
                                      pressing do not search other object.*/
    EDGUI_PROTECT_CLICK_FOCUS = 0x20, /**< Prevent focusing the object by clicking on it*/
};
typedef uint8_t edgui_protect_t;

/** Used by `edgui_obj_get_type()`. The object's and its ancestor types are stored here*/
typedef struct
{
    const char * type[EDGUI_MAX_ANCESTOR_NUM]; /**< [0]: the actual type, [1]: ancestor, [2] #1's ancestor
                                               ... [x]: "edgui_obj" */
} edgui_obj_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init. the 'lv' library.
 */
void edgui_init(void);

/*--------------------
 * Create and delete
 *-------------------*/

/**
 * Create a basic object
 * @param parent pointer to a parent object.
 *                  If NULL then a screen will be created
 * @param copy pointer to a base object, if not NULL then the new object will be copied from it
 * @return pointer to the new object
 */
edgui_obj_t * edgui_obj_create(edgui_obj_t * parent, const edgui_obj_t * copy);

/**
 * Delete 'obj' and all of its children
 * @param obj pointer to an object to delete
 * @return EDGUI_RES_INV because the object is deleted
 */
edgui_res_t edgui_obj_del(edgui_obj_t * obj);

/**
 * Helper function for asynchronously deleting objects.
 * Useful for cases where you can't delete an object directly in an `EDGUI_EVENT_DELETE` handler (i.e. parent).
 * @param obj object to delete
 * @see edgui_async_call
 */
void edgui_obj_del_async(struct _edgui_obj_t *obj);

/**
 * Delete all children of an object
 * @param obj pointer to an object
 */
void edgui_obj_clean(edgui_obj_t * obj);

/**
 * Mark the object as invalid therefore its current position will be redrawn by 'edgui_refr_task'
 * @param obj pointer to an object
 */
void edgui_obj_invalidate(const edgui_obj_t * obj);

/*=====================
 * Setter functions
 *====================*/

/*--------------------
 * Parent/children set
 *--------------------*/

/**
 * Set a new parent for an object. Its relative position will be the same.
 * @param obj pointer to an object. Can't be a screen.
 * @param parent pointer to the new parent object. (Can't be NULL)
 */
void edgui_obj_set_parent(edgui_obj_t * obj, edgui_obj_t * parent);

/**
 * Move and object to the foreground
 * @param obj pointer to an object
 */
void edgui_obj_move_foreground(edgui_obj_t * obj);

/**
 * Move and object to the background
 * @param obj pointer to an object
 */
void edgui_obj_move_background(edgui_obj_t * obj);

/*--------------------
 * Coordinate set
 * ------------------*/

/**
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent
 * @param y new distance from the top of the parent
 */
void edgui_obj_set_pos(edgui_obj_t * obj, edgui_coord_t x, edgui_coord_t y);

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent
 */
void edgui_obj_set_x(edgui_obj_t * obj, edgui_coord_t x);

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent
 */
void edgui_obj_set_y(edgui_obj_t * obj, edgui_coord_t y);

/**
 * Set the size of an object
 * @param obj pointer to an object
 * @param w new width
 * @param h new height
 */
void edgui_obj_set_size(edgui_obj_t * obj, edgui_coord_t w, edgui_coord_t h);

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width
 */
void edgui_obj_set_width(edgui_obj_t * obj, edgui_coord_t w);

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height
 */
void edgui_obj_set_height(edgui_obj_t * obj, edgui_coord_t h);

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'edgui_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void edgui_obj_align(edgui_obj_t * obj, const edgui_obj_t * base, edgui_align_t align, edgui_coord_t x_mod, edgui_coord_t y_mod);

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'edgui_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void edgui_obj_align_origo(edgui_obj_t * obj, const edgui_obj_t * base, edgui_align_t align, edgui_coord_t x_mod, edgui_coord_t y_mod);

/**
 * Realign the object based on the last `edgui_obj_align` parameters.
 * @param obj pointer to an object
 */
void edgui_obj_realign(edgui_obj_t * obj);

/**
 * Enable the automatic realign of the object when its size has changed based on the last
 * `edgui_obj_align` parameters.
 * @param obj pointer to an object
 * @param en true: enable auto realign; false: disable auto realign
 */
void edgui_obj_set_auto_realign(edgui_obj_t * obj, bool en);

/**
 * Set the size of an extended clickable area
 * @param obj pointer to an object
 * @param left extended clickable are on the left [px]
 * @param right extended clickable are on the right [px]
 * @param top extended clickable are on the top [px]
 * @param bottom extended clickable are on the bottom [px]
 */
void edgui_obj_set_ext_click_area(edgui_obj_t * obj, edgui_coord_t left, edgui_coord_t right, edgui_coord_t top, edgui_coord_t bottom);

/*---------------------
 * Appearance set
 *--------------------*/

/**
 * Set a new style for an object
 * @param obj pointer to an object
 * @param style_p pointer to the new style
 */
void edgui_obj_set_style(edgui_obj_t * obj, const edgui_style_t * style);

/**
 * Notify an object about its style is modified
 * @param obj pointer to an object
 */
void edgui_obj_refresh_style(edgui_obj_t * obj);

/**
 * Notify all object if a style is modified
 * @param style pointer to a style. Only the objects with this style will be notified
 *               (NULL to notify all objects)
 */
void edgui_obj_report_style_mod(edgui_style_t * style);

/*-----------------
 * Attribute set
 *----------------*/

/**
 * Hide an object. It won't be visible and clickable.
 * @param obj pointer to an object
 * @param en true: hide the object
 */
void edgui_obj_set_hidden(edgui_obj_t * obj, bool en);

/**
 * Enable or disable the clicking of an object
 * @param obj pointer to an object
 * @param en true: make the object clickable
 */
void edgui_obj_set_click(edgui_obj_t * obj, bool en);

/**
 * Enable to bring this object to the foreground if it
 * or any of its children is clicked
 * @param obj pointer to an object
 * @param en true: enable the auto top feature
 */
void edgui_obj_set_top(edgui_obj_t * obj, bool en);

/**
 * Enable the dragging of an object
 * @param obj pointer to an object
 * @param en true: make the object dragable
 */
void edgui_obj_set_drag(edgui_obj_t * obj, bool en);

/**
 * Set the directions an object can be dragged in
 * @param obj pointer to an object
 * @param drag_dir bitwise OR of allowed drag directions
 */
void edgui_obj_set_drag_dir(edgui_obj_t * obj, edgui_drag_dir_t drag_dir);

/**
 * Enable the throwing of an object after is is dragged
 * @param obj pointer to an object
 * @param en true: enable the drag throw
 */
void edgui_obj_set_drag_throw(edgui_obj_t * obj, bool en);

/**
 * Enable to use parent for drag related operations.
 * If trying to drag the object the parent will be moved instead
 * @param obj pointer to an object
 * @param en true: enable the 'drag parent' for the object
 */
void edgui_obj_set_drag_parent(edgui_obj_t * obj, bool en);

/**
 * Propagate the events to the parent too
 * @param obj pointer to an object
 * @param en true: enable the event propagation
 */
void edgui_obj_set_parent_event(edgui_obj_t * obj, bool en);

/**
 * Set the opa scale enable parameter (required to set opa_scale with `edgui_obj_set_opa_scale()`)
 * @param obj pointer to an object
 * @param en true: opa scaling is enabled for this object and all children; false: no opa scaling
 */
void edgui_obj_set_opa_scale_enable(edgui_obj_t * obj, bool en);

/**
 * Set the opa scale of an object.
 * The opacity of this object and all it's children will be scaled down with this factor.
 * `edgui_obj_set_opa_scale_enable(obj, true)` needs to be called to enable it.
 * (not for all children just for the parent where to start the opa scaling)
 * @param obj pointer to an object
 * @param opa_scale a factor to scale down opacity [0..255]
 */
void edgui_obj_set_opa_scale(edgui_obj_t * obj, edgui_opa_t opa_scale);

/**
 * Set a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from `edgui_protect_t`
 */
void edgui_obj_set_protect(edgui_obj_t * obj, uint8_t prot);

/**
 * Clear a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from `edgui_protect_t`
 */
void edgui_obj_clear_protect(edgui_obj_t * obj, uint8_t prot);

/**
 * Set a an event handler function for an object.
 * Used by the user to react on event which happens with the object.
 * @param obj pointer to an object
 * @param event_cb the new event function
 */
void edgui_obj_set_event_cb(edgui_obj_t * obj, edgui_event_cb_t event_cb);

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `edgui_event_t`.
 * @param data arbitrary data depending on the object type and the event. (Usually `NULL`)
 * @return EDGUI_RES_OK: `obj` was not deleted in the event; EDGUI_RES_INV: `obj` was deleted in the event
 */
edgui_res_t edgui_event_send(edgui_obj_t * obj, edgui_event_t event, const void * data);

/**
 * Call an event function with an object, event, and data.
 * @param event_xcb an event callback function. If `NULL` `EDGUI_RES_OK` will return without any actions.
 *        (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *         the `func_name(object, callback, ...)` convention)
 * @param obj pointer to an object to associate with the event (can be `NULL` to simply call the `event_cb`)
 * @param event an event
 * @param data pointer to a custom data
 * @return EDGUI_RES_OK: `obj` was not deleted in the event; EDGUI_RES_INV: `obj` was deleted in the event
 */
edgui_res_t edgui_event_send_func(edgui_event_cb_t event_xcb, edgui_obj_t * obj, edgui_event_t event, const void * data);

/**
 * Get the `data` parameter of the current event
 * @return the `data` parameter
 */
const void * edgui_event_get_data(void);

/**
 * Set the a signal function of an object. Used internally by the library.
 * Always call the previous signal function in the new.
 * @param obj pointer to an object
 * @param signal_cb the new signal function
 */
void edgui_obj_set_signal_cb(edgui_obj_t * obj, edgui_signal_cb_t signal_cb);

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `edgui_event_t`.
 */
void edgui_signal_send(edgui_obj_t * obj, edgui_signal_t signal, void * param);

/**
 * Set a new design function for an object
 * @param obj pointer to an object
 * @param design_cb the new design function
 */
void edgui_obj_set_design_cb(edgui_obj_t * obj, edgui_design_cb_t design_cb);

/*----------------
 * Other set
 *--------------*/

/**
 * Allocate a new ext. data for an object
 * @param obj pointer to an object
 * @param ext_size the size of the new ext. data
 * @return pointer to the allocated ext
 */
void * edgui_obj_allocate_ext_attr(edgui_obj_t * obj, uint16_t ext_size);

/**
 * Send a 'EDGUI_SIGNAL_REFR_EXT_SIZE' signal to the object
 * @param obj pointer to an object
 */
void edgui_obj_refresh_ext_draw_pad(edgui_obj_t * obj);

/*=======================
 * Getter functions
 *======================*/

/**
 * Return with the screen of an object
 * @param obj pointer to an object
 * @return pointer to a screen
 */
edgui_obj_t * edgui_obj_get_screen(const edgui_obj_t * obj);

/**
 * Get the display of an object
 * @param scr pointer to an object
 * @return pointer the object's display
 */
edgui_disp_t * edgui_obj_get_disp(const edgui_obj_t * obj);

/*---------------------
 * Parent/children get
 *--------------------*/

/**
 * Returns with the parent of an object
 * @param obj pointer to an object
 * @return pointer to the parent of  'obj'
 */
edgui_obj_t * edgui_obj_get_parent(const edgui_obj_t * obj);

/**
 * Iterate through the children of an object (start from the "youngest, lastly created")
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
edgui_obj_t * edgui_obj_get_child(const edgui_obj_t * obj, const edgui_obj_t * child);

/**
 * Iterate through the children of an object (start from the "oldest", firstly created)
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
edgui_obj_t * edgui_obj_get_child_back(const edgui_obj_t * obj, const edgui_obj_t * child);

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t edgui_obj_count_children(const edgui_obj_t * obj);

/** Recursively count the children of an object
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t edgui_obj_count_children_recursive(const edgui_obj_t * obj);

/*---------------------
 * Coordinate get
 *--------------------*/

/**
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param cords_p pointer to an area to store the coordinates
 */
void edgui_obj_get_coords(const edgui_obj_t * obj, edgui_area_t * cords_p);

/**
 * Reduce area retried by `edgui_obj_get_coords()` the get graphically usable area of an object.
 * (Without the size of the border or other extra graphical elements)
 * @param coords_p store the result area here
 */
void edgui_obj_get_inner_coords(const edgui_obj_t * obj, edgui_area_t * coords_p);

/**
 * Get the x coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent
 */
edgui_coord_t edgui_obj_get_x(const edgui_obj_t * obj);

/**
 * Get the y coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the top of its parent
 */
edgui_coord_t edgui_obj_get_y(const edgui_obj_t * obj);

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width
 */
edgui_coord_t edgui_obj_get_width(const edgui_obj_t * obj);

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height
 */
edgui_coord_t edgui_obj_get_height(const edgui_obj_t * obj);

/**
 * Get that width reduced by the left and right padding.
 * @param obj pointer to an object
 * @return the width which still fits into the container
 */
edgui_coord_t edgui_obj_get_width_fit(edgui_obj_t * obj);

/**
 * Get that height reduced by the top an bottom padding.
 * @param obj pointer to an object
 * @return the height which still fits into the container
 */
edgui_coord_t edgui_obj_get_height_fit(edgui_obj_t * obj);

/**
 * Get the automatic realign property of the object.
 * @param obj pointer to an object
 * @return  true: auto realign is enabled; false: auto realign is disabled
 */
bool edgui_obj_get_auto_realign(edgui_obj_t * obj);

/**
 * Get the left padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended left padding
 */
edgui_coord_t edgui_obj_get_ext_click_pad_left(const edgui_obj_t * obj);

/**
 * Get the right padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended right padding
 */
edgui_coord_t edgui_obj_get_ext_click_pad_right(const edgui_obj_t * obj);

/**
 * Get the top padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended top padding
 */
edgui_coord_t edgui_obj_get_ext_click_pad_top(const edgui_obj_t * obj);

/**
 * Get the bottom padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended bottom padding
 */
edgui_coord_t edgui_obj_get_ext_click_pad_bottom(const edgui_obj_t * obj);

/**
 * Get the extended size attribute of an object
 * @param obj pointer to an object
 * @return the extended size attribute
 */
edgui_coord_t edgui_obj_get_ext_draw_pad(const edgui_obj_t * obj);

/*-----------------
 * Appearance get
 *---------------*/

/**
 * Get the style pointer of an object (if NULL get style of the parent)
 * @param obj pointer to an object
 * @return pointer to a style
 */
const edgui_style_t * edgui_obj_get_style(const edgui_obj_t * obj);

/*-----------------
 * Attribute get
 *----------------*/

/**
 * Get the hidden attribute of an object
 * @param obj pointer to an object
 * @return true: the object is hidden
 */
bool edgui_obj_get_hidden(const edgui_obj_t * obj);

/**
 * Get the click enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is clickable
 */
bool edgui_obj_get_click(const edgui_obj_t * obj);

/**
 * Get the top enable attribute of an object
 * @param obj pointer to an object
 * @return true: the auto top feature is enabled
 */
bool edgui_obj_get_top(const edgui_obj_t * obj);

/**
 * Get the drag enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is dragable
 */
bool edgui_obj_get_drag(const edgui_obj_t * obj);

/**
 * Get the directions an object can be dragged
 * @param obj pointer to an object
 * @return bitwise OR of allowed directions an object can be dragged in
 */
edgui_drag_dir_t edgui_obj_get_drag_dir(const edgui_obj_t * obj);

/**
 * Get the drag throw enable attribute of an object
 * @param obj pointer to an object
 * @return true: drag throw is enabled
 */
bool edgui_obj_get_drag_throw(const edgui_obj_t * obj);

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool edgui_obj_get_drag_parent(const edgui_obj_t * obj);

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool edgui_obj_get_parent_event(const edgui_obj_t * obj);

/**
 * Get the opa scale enable parameter
 * @param obj pointer to an object
 * @return true: opa scaling is enabled for this object and all children; false: no opa scaling
 */
edgui_opa_t edgui_obj_get_opa_scale_enable(const edgui_obj_t * obj);

/**
 * Get the opa scale parameter of an object
 * @param obj pointer to an object
 * @return opa scale [0..255]
 */
edgui_opa_t edgui_obj_get_opa_scale(const edgui_obj_t * obj);

/**
 * Get the protect field of an object
 * @param obj pointer to an object
 * @return protect field ('OR'ed values of `edgui_protect_t`)
 */
uint8_t edgui_obj_get_protect(const edgui_obj_t * obj);

/**
 * Check at least one bit of a given protect bitfield is set
 * @param obj pointer to an object
 * @param prot protect bits to test ('OR'ed values of `edgui_protect_t`)
 * @return false: none of the given bits are set, true: at least one bit is set
 */
bool edgui_obj_is_protected(const edgui_obj_t * obj, uint8_t prot);

/**
 * Get the signal function of an object
 * @param obj pointer to an object
 * @return the signal function
 */
edgui_signal_cb_t edgui_obj_get_signal_cb(const edgui_obj_t * obj);

/**
 * Get the design function of an object
 * @param obj pointer to an object
 * @return the design function
 */
edgui_design_cb_t edgui_obj_get_design_cb(const edgui_obj_t * obj);

/**
 * Get the event function of an object
 * @param obj pointer to an object
 * @return the event function
 */
edgui_event_cb_t edgui_obj_get_event_cb(const edgui_obj_t * obj);

/*------------------
 * Other get
 *-----------------*/

/**
 * Get the ext pointer
 * @param obj pointer to an object
 * @return the ext pointer but not the dynamic version
 *         Use it as ext->data1, and NOT da(ext)->data1
 */
void * edgui_obj_get_ext_attr(const edgui_obj_t * obj);

/**
 * Get object's and its ancestors type. Put their name in `type_buf` starting with the current type.
 * E.g. buf.type[0]="edgui_btn", buf.type[1]="edgui_cont", buf.type[2]="edgui_obj"
 * @param obj pointer to an object which type should be get
 * @param buf pointer to an `edgui_obj_type_t` buffer to store the types
 */
void edgui_obj_get_type(edgui_obj_t * obj, edgui_obj_type_t * buf);

#if EDGUI_USE_USER_DATA
/**
 * Get the object's user data
 * @param obj pointer to an object
 * @return user data
 */
edgui_obj_user_data_t edgui_obj_get_user_data(edgui_obj_t * obj);

/**
 * Get a pointer to the object's user data
 * @param obj pointer to an object
 * @return pointer to the user data
 */
edgui_obj_user_data_t * edgui_obj_get_user_data_ptr(edgui_obj_t * obj);

/**
 * Set the object's user data. The data will be copied.
 * @param obj pointer to an object
 * @param data user data
 */
void edgui_obj_set_user_data(edgui_obj_t * obj, edgui_obj_user_data_t data);

#endif

#if EDGUI_USE_GROUP
/**
 * Get the group of the object
 * @param obj pointer to an object
 * @return the pointer to group of the object
 */
void * edgui_obj_get_group(const edgui_obj_t * obj);

/**
 * Tell whether the object is the focused object of a group or not.
 * @param obj pointer to an object
 * @return true: the object is focused, false: the object is not focused or not in a group
 */
bool edgui_obj_is_focused(const edgui_obj_t * obj);

#endif

/**********************
 *      MACROS
 **********************/

/**
 * Helps to quickly declare an event callback function.
 * Will be expanded to: `void <name> (edgui_obj_t * obj, edgui_event_t e)`
 *
 * Examples:
 * static EDGUI_EVENT_CB_DECLARE(my_event1);  //Protoype declaration
 *
 * static EDGUI_EVENT_CB_DECLARE(my_event1)
 * {
 *   if(e == EDGUI_EVENT_CLICKED) {
 *      edgui_obj_set_hidden(obj ,true);
 *   }
 * }
 */
#define EDGUI_EVENT_CB_DECLARE(name) void name(edgui_obj_t * obj, edgui_event_t e)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_OBJ_H*/
