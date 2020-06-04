/**
 * @file edgui_group.h
 *
 */

#ifndef EDGUI_GROUP_H
#define EDGUI_GROUP_H

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

#include "edgui_obj.h"

/*********************
 *      DEFINES
 *********************/
/*Predefined keys to control the focused object via edgui_group_send(group, c)*/
/*For compatibility in signal function define the keys regardless to `EDGUI_USE_GROUP`*/

enum {
    EDGUI_KEY_UP        = 17,  /*0x11*/
    EDGUI_KEY_DOWN      = 18,  /*0x12*/
    EDGUI_KEY_RIGHT     = 19,  /*0x13*/
    EDGUI_KEY_LEFT      = 20,  /*0x14*/
    EDGUI_KEY_ESC       = 27,  /*0x1B*/
    EDGUI_KEY_DEL       = 127, /*0x7F*/
    EDGUI_KEY_BACKSPACE = 8,   /*0x08*/
    EDGUI_KEY_ENTER     = 10,  /*0x0A, '\n'*/
    EDGUI_KEY_NEXT      = 9,   /*0x09, '\t'*/
    EDGUI_KEY_PREV      = 11,  /*0x0B, '*/
    EDGUI_KEY_HOME      = 2,   /*0x02, STX*/
    EDGUI_KEY_END       = 3,   /*0x03, ETX*/
};
typedef uint8_t edgui_key_t;

#if EDGUI_USE_GROUP != 0
/**********************
 *      TYPEDEFS
 **********************/
struct _edgui_group_t;

typedef void (*edgui_group_style_mod_cb_t)(struct _edgui_group_t *, edgui_style_t *);
typedef void (*edgui_group_focus_cb_t)(struct _edgui_group_t *);

/**
 * Groups can be used to logically hold objects so that they can be individually focused.
 * They are NOT for laying out objects on a screen (try `edgui_cont` for that).
 */
typedef struct _edgui_group_t
{
    edgui_ll_t obj_ll;        /**< Linked list to store the objects in the group */
    edgui_obj_t ** obj_focus; /**< The object in focus*/

    edgui_group_style_mod_cb_t style_mod_cb;      /**< A function to modifies the style of the focused object*/
    edgui_group_style_mod_cb_t style_mod_edit_cb; /**< A function which modifies the style of the edited object*/
    edgui_group_focus_cb_t focus_cb;              /**< A function to call when a new object is focused (optional)*/
    edgui_style_t style_tmp;                      /**< Stores the modified style of the focused object */
#if EDGUI_USE_USER_DATA
    edgui_group_user_data_t user_data;
#endif

    uint8_t frozen : 1;         /**< 1: can't focus to new object*/
    uint8_t editing : 1;        /**< 1: Edit mode, 0: Navigate mode*/
    uint8_t click_focus : 1;    /**< 1: If an object in a group is clicked by an indev then it will be
                                   focused */
    uint8_t refocus_policy : 1; /**< 1: Focus prev if focused on deletion. 0: Focus next if focused on
                                   deletion.*/
    uint8_t wrap : 1;           /**< 1: Focus next/prev can wrap at end of list. 0: Focus next/prev stops at end
                                   of list.*/
} edgui_group_t;

enum { EDGUI_GROUP_REFOCUS_POLICY_NEXT = 0, EDGUI_GROUP_REFOCUS_POLICY_PREV = 1 };
typedef uint8_t edgui_group_refocus_policy_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init. the group module
 * @remarks Internal function, do not call directly.
 */
void edgui_group_init(void);

/**
 * Create a new object group
 * @return pointer to the new object group
 */
edgui_group_t * edgui_group_create(void);

/**
 * Delete a group object
 * @param group pointer to a group
 */
void edgui_group_del(edgui_group_t * group);

/**
 * Add an object to a group
 * @param group pointer to a group
 * @param obj pointer to an object to add
 */
void edgui_group_add_obj(edgui_group_t * group, edgui_obj_t * obj);

/**
 * Remove an object from its group
 * @param obj pointer to an object to remove
 */
void edgui_group_remove_obj(edgui_obj_t * obj);

/**
 * Remove all objects from a group
 * @param group pointer to a group
 */
void edgui_group_remove_all_objs(edgui_group_t * group);

/**
 * Focus on an object (defocus the current)
 * @param obj pointer to an object to focus on
 */
void edgui_group_focus_obj(edgui_obj_t * obj);

/**
 * Focus the next object in a group (defocus the current)
 * @param group pointer to a group
 */
void edgui_group_focus_next(edgui_group_t * group);

/**
 * Focus the previous object in a group (defocus the current)
 * @param group pointer to a group
 */
void edgui_group_focus_prev(edgui_group_t * group);

/**
 * Do not let to change the focus from the current object
 * @param group pointer to a group
 * @param en true: freeze, false: release freezing (normal mode)
 */
void edgui_group_focus_freeze(edgui_group_t * group, bool en);

/**
 * Send a control character to the focuses object of a group
 * @param group pointer to a group
 * @param c a character (use EDGUI_KEY_.. to navigate)
 * @return result of focused object in group.
 */
edgui_res_t edgui_group_send_data(edgui_group_t * group, uint32_t c);

/**
 * Set a function for a group which will modify the object's style if it is in focus
 * @param group pointer to a group
 * @param style_mod_cb the style modifier function pointer
 */
void edgui_group_set_style_mod_cb(edgui_group_t * group, edgui_group_style_mod_cb_t style_mod_cb);

/**
 * Set a function for a group which will modify the object's style if it is in focus in edit mode
 * @param group pointer to a group
 * @param style_mod_edit_cb the style modifier function pointer
 */
void edgui_group_set_style_mod_edit_cb(edgui_group_t * group, edgui_group_style_mod_cb_t style_mod_edit_cb);

/**
 * Set a function for a group which will be called when a new object is focused
 * @param group pointer to a group
 * @param focus_cb the call back function or NULL if unused
 */
void edgui_group_set_focus_cb(edgui_group_t * group, edgui_group_focus_cb_t focus_cb);

/**
 * Set whether the next or previous item in a group is focused if the currently focussed obj is
 * deleted.
 * @param group pointer to a group
 * @param new refocus policy enum
 */
void edgui_group_set_refocus_policy(edgui_group_t * group, edgui_group_refocus_policy_t policy);

/**
 * Manually set the current mode (edit or navigate).
 * @param group pointer to group
 * @param edit: true: edit mode; false: navigate mode
 */
void edgui_group_set_editing(edgui_group_t * group, bool edit);

/**
 * Set the `click_focus` attribute. If enabled then the object will be focused then it is clicked.
 * @param group pointer to group
 * @param en: true: enable `click_focus`
 */
void edgui_group_set_click_focus(edgui_group_t * group, bool en);

/**
 * Set whether focus next/prev will allow wrapping from first->last or last->first object.
 * @param group pointer to group
 * @param en: true: wrapping enabled; false: wrapping disabled
 */
void edgui_group_set_wrap(edgui_group_t * group, bool en);

/**
 * Modify a style with the set 'style_mod' function. The input style remains unchanged.
 * @param group pointer to group
 * @param style pointer to a style to modify
 * @return a copy of the input style but modified with the 'style_mod' function
 */
edgui_style_t * edgui_group_mod_style(edgui_group_t * group, const edgui_style_t * style);

/**
 * Get the focused object or NULL if there isn't one
 * @param group pointer to a group
 * @return pointer to the focused object
 */
edgui_obj_t * edgui_group_get_focused(const edgui_group_t * group);

#if EDGUI_USE_USER_DATA
/**
 * Get a pointer to the group's user data
 * @param group pointer to an group
 * @return pointer to the user data
 */
edgui_group_user_data_t * edgui_group_get_user_data(edgui_group_t * group);

#endif

/**
 * Get a the style modifier function of a group
 * @param group pointer to a group
 * @return pointer to the style modifier function
 */
edgui_group_style_mod_cb_t edgui_group_get_style_mod_cb(const edgui_group_t * group);

/**
 * Get a the style modifier function of a group in edit mode
 * @param group pointer to a group
 * @return pointer to the style modifier function
 */
edgui_group_style_mod_cb_t edgui_group_get_style_mod_edit_cb(const edgui_group_t * group);

/**
 * Get the focus callback function of a group
 * @param group pointer to a group
 * @return the call back function or NULL if not set
 */
edgui_group_focus_cb_t edgui_group_get_focus_cb(const edgui_group_t * group);

/**
 * Get the current mode (edit or navigate).
 * @param group pointer to group
 * @return true: edit mode; false: navigate mode
 */
bool edgui_group_get_editing(const edgui_group_t * group);

/**
 * Get the `click_focus` attribute.
 * @param group pointer to group
 * @return true: `click_focus` is enabled; false: disabled
 */
bool edgui_group_get_click_focus(const edgui_group_t * group);

/**
 * Get whether focus next/prev will allow wrapping from first->last or last->first object.
 * @param group pointer to group
 * @param en: true: wrapping enabled; false: wrapping disabled
 */
bool edgui_group_get_wrap(edgui_group_t * group);

/**
 * Notify the group that current theme changed and style modification callbacks need to be
 * refreshed.
 * @param group pointer to group. If NULL then all groups are notified.
 */
void edgui_group_report_style_mod(edgui_group_t * group);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_GROUP != 0*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_GROUP_H*/
