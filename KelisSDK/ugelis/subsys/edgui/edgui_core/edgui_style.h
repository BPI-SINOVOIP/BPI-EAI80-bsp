/**
 * @file edgui_style.h
 *
 */

#ifndef EDGUI_STYLE_H
#define EDGUI_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "../edgui_font/edgui_font.h"
#include "../edgui_misc/edgui_color.h"
#include "../edgui_misc/edgui_area.h"
#include "../edgui_misc/edgui_anim.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_RADIUS_CIRCLE (EDGUI_COORD_MAX) /**< A very big radius to always draw as circle*/

/**********************
 *      TYPEDEFS
 **********************/

/*Border types (Use 'OR'ed values)*/
enum {
    EDGUI_BORDER_NONE     = 0x00,
    EDGUI_BORDER_BOTTOM   = 0x01,
    EDGUI_BORDER_TOP      = 0x02,
    EDGUI_BORDER_LEFT     = 0x04,
    EDGUI_BORDER_RIGHT    = 0x08,
    EDGUI_BORDER_FULL     = 0x0F,
    EDGUI_BORDER_INTERNAL = 0x10, /**< FOR matrix-like objects (e.g. Button matrix)*/
};
typedef uint8_t edgui_border_part_t;

/*Shadow types*/
enum {
    EDGUI_SHADOW_BOTTOM = 0, /**< Only draw bottom shadow */
    EDGUI_SHADOW_FULL,       /**< Draw shadow on all sides */
};
typedef uint8_t edgui_shadow_type_t;

/**
 * Objects in edgui can be assigned a style - which holds information about
 * how the object should be drawn.
 * 
 * This allows for easy customization without having to modify the object's design
 * function.
 */
typedef struct
{
    uint8_t glass : 1; /**< 1: Do not inherit this style*/

    /** Object background. */
    struct
    {
        edgui_color_t main_color; /**< Object's main background color. */
        edgui_color_t grad_color; /**< Second color. If not equal to `main_color` a gradient will be drawn for the background. */
        edgui_coord_t radius; /**< Object's corner radius. You can use #EDGUI_RADIUS_CIRCLE if you want to draw a circle. */
        edgui_opa_t opa; /**< Object's opacity (0-255). */

        struct
        {
            edgui_color_t color; /**< Border color */
            edgui_coord_t width; /**< Border width */
            edgui_border_part_t part; /**< Which borders to draw */
            edgui_opa_t opa; /**< Border opacity. */
        } border;

        
        struct
        {
            edgui_color_t color;
            edgui_coord_t width;
            edgui_shadow_type_t type; /**< Which parts of the shadow to draw */
        } shadow;

        struct
        {
            edgui_coord_t top;
            edgui_coord_t bottom;
            edgui_coord_t left;
            edgui_coord_t right;
            edgui_coord_t inner;
        } padding;
    } body;

    /** Style for text drawn by this object. */
    struct
    {
        edgui_color_t color; /**< Text color */
        edgui_color_t sel_color; /**< Text selection background color. */
        const edgui_font_t * font;
        edgui_coord_t letter_space; /**< Space between letters */
        edgui_coord_t line_space; /**< Space between lines (vertical) */
        edgui_opa_t opa; /**< Text opacity */
    } text;

    /**< Style of images. */
    struct
    {
        edgui_color_t color; /**< Color to recolor the image with */
        edgui_opa_t intense; /**< Opacity of recoloring (0 means no recoloring) */
        edgui_opa_t opa; /**< Opacity of whole image */
    } image;

    /**< Style of lines (not borders). */
    struct
    {
        edgui_color_t color;
        edgui_coord_t width;
        edgui_opa_t opa;
        uint8_t rounded : 1; /**< 1: rounded line endings*/
    } line;
} edgui_style_t;

#if EDGUI_USE_ANIMATION
/** Data structure for style animations. */
typedef struct
{
    edgui_style_t style_start; /*Save not only pointers because can be same as 'style_anim' then it
                               will be modified too*/
    edgui_style_t style_end;
    edgui_style_t * style_anim;
    edgui_anim_ready_cb_t ready_cb;
} edgui_style_anim_dsc_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 *  Init the basic styles
 */
void edgui_style_init(void);

/**
 * Copy a style to an other
 * @param dest pointer to the destination style
 * @param src pointer to the source style
 */
void edgui_style_copy(edgui_style_t * dest, const edgui_style_t * src);

/**
 * Mix two styles according to a given ratio
 * @param start start style
 * @param end end style
 * @param res store the result style here
 * @param ratio the ratio of mix [0..256]; 0: `start` style; 256: `end` style
 */
void edgui_style_mix(const edgui_style_t * start, const edgui_style_t * end, edgui_style_t * res, uint16_t ratio);

#if EDGUI_USE_ANIMATION

/**
 * Initialize an animation variable.
 * E.g.:
 * edgui_anim_t a;
 * edgui_style_anim__init(&a);
 * edgui_style_anim_set_...(&a);
 * edgui_style_anim_create(&a);
 * @param a pointer to an `edgui_anim_t` variable to initialize
 */
void edgui_style_anim_init(edgui_anim_t * a);

/**
 *
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param to_anim pointer to the style to animate
 * @param start pointer to a style to animate from (start value)
 * @param end pointer to a style to animate to (end value)
 */
void edgui_style_anim_set_styles(edgui_anim_t * a, edgui_style_t * to_anim, const edgui_style_t * start, const edgui_style_t * end);

/**
 * Set the duration and delay of an animation
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param duration duration of the animation in milliseconds
 * @param delay delay before the animation in milliseconds
 */
static __inline void edgui_style_anim_set_time(edgui_anim_t * a, uint16_t duration, uint16_t delay)
{
    edgui_anim_set_time(a, duration, delay);
}

/**
 * Set a function call when the animation is ready
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param ready_cb a function call when the animation is ready
 */
static __inline void edgui_style_anim_set_ready_cb(edgui_anim_t * a, edgui_anim_ready_cb_t ready_cb)
{
    edgui_style_anim_dsc_t * dsc = (edgui_style_anim_dsc_t *)a->var;
    dsc->ready_cb             = ready_cb;
}

/**
 * Make the animation to play back to when the forward direction is ready
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param wait_time time in milliseconds to wait before starting the back direction
 */
static __inline void edgui_style_anim_set_playback(edgui_anim_t * a, uint16_t wait_time)
{
    edgui_anim_set_playback(a, wait_time);
}

/**
 * Disable playback. (Disabled after `edgui_anim_init()`)
 * @param a pointer to an initialized `edgui_anim_t` variable
 */
static __inline void edgui_style_anim_clear_playback(edgui_anim_t * a)
{
    edgui_anim_clear_playback(a);
}

/**
 * Make the animation to start again when ready.
 * @param a pointer to an initialized `edgui_anim_t` variable
 * @param wait_time time in milliseconds to wait before starting the animation again
 */
static __inline void edgui_style_anim_set_repeat(edgui_anim_t * a, uint16_t wait_time)
{
    edgui_anim_set_repeat(a, wait_time);
}

/**
 * Disable repeat. (Disabled after `edgui_anim_init()`)
 * @param a pointer to an initialized `edgui_anim_t` variable
 */
static __inline void edgui_style_anim_clear_repeat(edgui_anim_t * a)
{
    edgui_anim_clear_repeat(a);
}

/**
 * Create an animation
 * @param a an initialized 'anim_t' variable. Not required after call.
 */
static __inline void edgui_style_anim_create(edgui_anim_t * a)
{
    edgui_anim_create(a);
}

#endif

/*************************
 *    GLOBAL VARIABLES
 *************************/
extern edgui_style_t edgui_style_scr;
extern edgui_style_t edgui_style_transp;
extern edgui_style_t edgui_style_transp_fit;
extern edgui_style_t edgui_style_transp_tight;
extern edgui_style_t edgui_style_plain;
extern edgui_style_t edgui_style_plain_color;
extern edgui_style_t edgui_style_pretty;
extern edgui_style_t edgui_style_pretty_color;
extern edgui_style_t edgui_style_btn_rel;
extern edgui_style_t edgui_style_btn_pr;
extern edgui_style_t edgui_style_btn_tgl_rel;
extern edgui_style_t edgui_style_btn_tgl_pr;
extern edgui_style_t edgui_style_btn_ina;

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_STYLE_H*/
