/**
 * @file edgui_roller.h
 *
 */

#ifndef EDGUI_ROLLER_H
#define EDGUI_ROLLER_H

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

#if EDGUI_USE_ROLLER != 0

/*Testing of dependencies*/
#if EDGUI_USE_DDLIST == 0
#error "edgui_roller: edgui_ddlist is required. Enable it in edgui_conf.h (EDGUI_USE_DDLIST  1) "
#endif

#include "../edgui_core/edgui_obj.h"
#include "edgui_ddlist.h"
#include "edgui_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Roller mode. */
enum {
    EDGUI_ROLLER_MODE_NORMAL, /**< Normal mode (roller ends at the end of the options). */
    EDGUI_ROLLER_MODE_INIFINITE, /**< Infinite mode (roller can be scrolled forever). */
};

typedef uint8_t edgui_roller_mode_t;



/*Data of roller*/
typedef struct
{
    edgui_ddlist_ext_t ddlist; /*Ext. of ancestor*/
    /*New data for this type */
    edgui_roller_mode_t mode : 1;
} edgui_roller_ext_t;

enum {
    EDGUI_ROLLER_STYLE_BG,
    EDGUI_ROLLER_STYLE_SEL,
};
typedef uint8_t edgui_roller_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a roller object
 * @param par pointer to an object, it will be the parent of the new roller
 * @param copy pointer to a roller object, if not NULL then the new object will be copied from it
 * @return pointer to the created roller
 */
edgui_obj_t * edgui_roller_create(edgui_obj_t * par, const edgui_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the options on a roller
 * @param roller pointer to roller object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 * @param mode `EDGUI_ROLLER_MODE_NORMAL` or `EDGUI_ROLLER_MODE_INFINITE`
 */
void edgui_roller_set_options(edgui_obj_t * roller, const char * options, edgui_roller_mode_t mode);

/**
 * Set the align of the roller's options (left, right or center[default])
 * @param roller - pointer to a roller object
 * @param align - one of edgui_label_align_t values (left, right, center)
 */
void edgui_roller_set_align(edgui_obj_t * roller, edgui_label_align_t align);

/**
 * Set the selected option
 * @param roller pointer to a roller object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 * @param anim EDGUI_ANOM_ON: set with animation; EDGUI_ANIM_OFF set immediately
 */
void edgui_roller_set_selected(edgui_obj_t * roller, uint16_t sel_opt, edgui_anim_enable_t anim);

/**
 * Set the height to show the given number of rows (options)
 * @param roller pointer to a roller object
 * @param row_cnt number of desired visible rows
 */
void edgui_roller_set_visible_row_count(edgui_obj_t * roller, uint8_t row_cnt);

/**
 * Set a fix width for the drop down list
 * @param roller pointer to a roller obejct
 * @param w the width when the list is opened (0: auto size)
 */
static __inline void edgui_roller_set_fix_width(edgui_obj_t * roller, edgui_coord_t w)
{
    edgui_ddlist_set_fix_width(roller, w);
}

/**
 * Set the open/close animation time.
 * @param roller pointer to a roller object
 * @param anim_time: open/close animation time [ms]
 */
static __inline void edgui_roller_set_anim_time(edgui_obj_t * roller, uint16_t anim_time)
{
    edgui_ddlist_set_anim_time(roller, anim_time);
}

/**
 * Set a style of a roller
 * @param roller pointer to a roller object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_roller_set_style(edgui_obj_t * roller, edgui_roller_style_t type, const edgui_style_t * style);

/*=====================
 * Getter functions
 *====================*/
/**
 * Get the id of the selected option
 * @param roller pointer to a roller object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t edgui_roller_get_selected(const edgui_obj_t * roller);

/**
 * Get the current selected option as a string
 * @param roller pointer to roller object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
static __inline void edgui_roller_get_selected_str(const edgui_obj_t * roller, char * buf, uint16_t buf_size)
{
    edgui_ddlist_get_selected_str(roller, buf, buf_size);
}

/**
 * Get the align attribute. Default alignment after _create is EDGUI_LABEL_ALIGN_CENTER
 * @param roller pointer to a roller object
 * @return EDGUI_LABEL_ALIGN_LEFT, EDGUI_LABEL_ALIGN_RIGHT or EDGUI_LABEL_ALIGN_CENTER
 */
edgui_label_align_t edgui_roller_get_align(const edgui_obj_t * roller);

/**
 * Get the options of a roller
 * @param roller pointer to roller object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
static __inline const char * edgui_roller_get_options(const edgui_obj_t * roller)
{
    return edgui_ddlist_get_options(roller);
}

/**
 * Get the open/close animation time.
 * @param roller pointer to a roller
 * @return open/close animation time [ms]
 */
static __inline uint16_t edgui_roller_get_anim_time(const edgui_obj_t * roller)
{
    return edgui_ddlist_get_anim_time(roller);
}

/**
 * Get the auto width set attribute
 * @param roller pointer to a roller object
 * @return true: auto size enabled; false: manual width settings enabled
 */
bool edgui_roller_get_hor_fit(const edgui_obj_t * roller);

/**
 * Get a style of a roller
 * @param roller pointer to a roller object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
const edgui_style_t * edgui_roller_get_style(const edgui_obj_t * roller, edgui_roller_style_t type);

/**********************
 *      MACROS
 **********************/

#endif /*EDGUI_USE_ROLLER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_ROLLER_H*/
