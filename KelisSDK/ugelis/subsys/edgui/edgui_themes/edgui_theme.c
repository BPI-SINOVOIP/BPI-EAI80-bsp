/**
 * @file edgui_theme.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_theme.h"
#include "../edgui_core/edgui_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

#if EDGUI_THEME_LIVE_UPDATE == 0
static edgui_theme_t * current_theme;
#else
/* If live update is used then a big `edgui_style_t` array is used to store the real styles of the
 * theme not only pointers. On `edgui_theme_set_current` the styles of the theme are copied to this
 * array. The pointers in `current_theme` are initialized to point to the styles in the array. This
 * way the theme styles will always point to the same memory address even after theme is change.
 * (The pointers in the theme points to the styles declared by the theme itself) */

/* Store the styles in this array. */
static edgui_style_t th_styles[EDGUI_THEME_STYLE_COUNT];
static bool inited = false;
static edgui_theme_t current_theme;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Set a theme for the system.
 * From now, all the created objects will use styles from this theme by default
 * @param th pointer to theme (return value of: 'edgui_theme_init_xxx()')
 */
void edgui_theme_set_current(edgui_theme_t * th)
{
#if EDGUI_THEME_LIVE_UPDATE == 0
    current_theme = th;

#if EDGUI_USE_GROUP
    /*Copy group style modification callback functions*/
    memcpy(&current_theme->group, &th->group, sizeof(th->group));
#endif

    /*Let the object know their style might change*/
    edgui_obj_report_style_mod(NULL);

#else
    uint32_t style_num = sizeof(th->style) / sizeof(edgui_style_t *); /*Number of styles in a theme*/

    if(!inited) {
        /*Initialize the style pointers `current_theme` to point to the `th_styles` style array */
        uint16_t i;
        edgui_style_t ** cur_th_style_p = (edgui_style_t **)&current_theme.style;
        for(i = 0; i < style_num; i++) {
            uintptr_t adr = (uintptr_t)&th_styles[i];
            memcpy(&cur_th_style_p[i], &adr, sizeof(edgui_style_t *));
        }
        inited = true;
    }

    /*Copy the styles pointed by the new theme to the `th_styles` style array*/
    uint16_t i;
    edgui_style_t ** th_style = (edgui_style_t **)&th->style;
    for(i = 0; i < style_num; i++) {
        uintptr_t s = (uintptr_t)th_style[i];
        if(s) memcpy(&th_styles[i], (void *)s, sizeof(edgui_style_t));
    }

#if EDGUI_USE_GROUP
    /*Copy group style modification callback functions*/
    memcpy(&current_theme.group, &th->group, sizeof(th->group));
#endif

    /*Let the object know their style might change*/
    edgui_obj_report_style_mod(NULL);

#endif

#if EDGUI_USE_GROUP
    edgui_group_report_style_mod(NULL);
#endif
}

/**
 * Get the current system theme.
 * @return pointer to the current system theme. NULL if not set.
 */
edgui_theme_t * edgui_theme_get_current(void)
{
#if EDGUI_THEME_LIVE_UPDATE == 0
    return current_theme;
#else
    if(!inited)
        return NULL;
    else
        return &current_theme;
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
