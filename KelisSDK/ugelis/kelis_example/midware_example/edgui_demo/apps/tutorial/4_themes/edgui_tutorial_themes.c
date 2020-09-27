/**
 * @file edgui_tutorial_themes.c
 *
 */

/*
 * -------------------------------------------------------------
 * See how the customize with themes much faster and simpler
 * --------------------------------------------------------------
 *
 * To set up styles you need some deeper knowledge about graphics library and
 * requires to be a designer a little bit. In addition it takes quite much time.
 *
 * To address this issue you can use 'themes'.
 * The themes are style collections for every object type with all the required styles.
 *
 * In 'edgui_conf.h' you can enable the themes. E.g.: EDGUI_USE_THEME_ALIEN   1
 *
 * When you initialize a theme you can assign a HUE (from HSV color space) and a font:
 * For example to initialize the 'Alien' theme with a greenish color:
 * edgui_theme_t *th = edgui_theme_alien_init(130, &edgui_font_dejavu_40);
 *
 * You have two options to use the themes:
 * 1. Set the styles in it directly: edgui_bar_set_style(my_bar, EDGUI_BAR_STYLE_BG, th->bar.bg);
 * 2. Set a system theme and let the library to create objects with the theme's styles
 *    E.g. edgui_theme_set_current(th);
 */

/*********************
 *      INCLUDES
 *********************/

#include "edgui_tutorial_themes.h"
#if EDGUI_USE_TUTORIALS && EDGUI_USE_THEME_ALIEN

#include "../2_objects/edgui_tutorial_objects.h"


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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize a theme and create the same objects like "edgui_tutorial_objects'
 */
void edgui_tutorial_themes(void)
{
    /*Initialize the alien theme
     * 210: a green HUE value
     * NULL: use the default font (EDGUI_FONT_DEFAULT)*/
    edgui_theme_t * th = edgui_theme_alien_init(210, NULL);

    /*Set the surent system theme*/
    edgui_theme_set_current(th);

    /*Create the 'edgui_tutorial_objects'*/
    edgui_tutorial_objects();

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/
