/**
 * @file edgui_led.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_led.h"
#if EDGUI_USE_LED != 0

#include "../edgui_themes/edgui_theme.h"
#include "../edgui_draw/edgui_draw.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_LED_WIDTH_DEF (EDGUI_DPI / 3)
#define EDGUI_LED_HEIGHT_DEF (EDGUI_DPI / 3)
#define EDGUI_LED_BRIGHT_OFF 100
#define EDGUI_LED_BRIGHT_ON 255

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_led_design(edgui_obj_t * led, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_led_signal(edgui_obj_t * led, edgui_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_design_cb_t ancestor_design_f;
static edgui_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a led objects
 * @param par pointer to an object, it will be the parent of the new led
 * @param copy pointer to a led object, if not NULL then the new object will be copied from it
 * @return pointer to the created led
 */
edgui_obj_t * edgui_led_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("led create started");

    /*Create the ancestor basic object*/
    edgui_obj_t * new_led = edgui_obj_create(par, copy);
    edgui_mem_assert(new_led);
    if(new_led == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_led);
    if(ancestor_design_f == NULL) ancestor_design_f = edgui_obj_get_design_cb(new_led);

    /*Allocate the object type specific extended data*/
    edgui_led_ext_t * ext = edgui_obj_allocate_ext_attr(new_led, sizeof(edgui_led_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->bright = EDGUI_LED_BRIGHT_ON;

    edgui_obj_set_signal_cb(new_led, edgui_led_signal);
    edgui_obj_set_design_cb(new_led, edgui_led_design);

    /*Init the new led object*/
    if(copy == NULL) {
        edgui_obj_set_size(new_led, EDGUI_LED_WIDTH_DEF, EDGUI_LED_HEIGHT_DEF);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_led_set_style(new_led, EDGUI_LED_STYLE_MAIN, th->style.led);
        } else {
            edgui_led_set_style(new_led, EDGUI_LED_STYLE_MAIN, &edgui_style_pretty_color);
        }
    }
    /*Copy an existing object*/
    else {
        edgui_led_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->bright             = copy_ext->bright;

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_led);
    }

    EDGUI_LOG_INFO("led created");

    return new_led;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the brightness of a LED object
 * @param led pointer to a LED object
 * @param bright 0 (max. dark) ... 255 (max. light)
 */
void edgui_led_set_bright(edgui_obj_t * led, uint8_t bright)
{
    /*Set the brightness*/
    edgui_led_ext_t * ext = edgui_obj_get_ext_attr(led);
    if(ext->bright == bright) return;

    ext->bright = bright;

    /*Invalidate the object there fore it will be redrawn*/
    edgui_obj_invalidate(led);
}

/**
 * Light on a LED
 * @param led pointer to a LED object
 */
void edgui_led_on(edgui_obj_t * led)
{
    edgui_led_set_bright(led, EDGUI_LED_BRIGHT_ON);
}

/**
 * Light off a LED
 * @param led pointer to a LED object
 */
void edgui_led_off(edgui_obj_t * led)
{
    edgui_led_set_bright(led, EDGUI_LED_BRIGHT_OFF);
}

/**
 * Toggle the state of a LED
 * @param led pointer to a LED object
 */
void edgui_led_toggle(edgui_obj_t * led)
{
    uint8_t bright = edgui_led_get_bright(led);
    if(bright > (EDGUI_LED_BRIGHT_OFF + EDGUI_LED_BRIGHT_ON) >> 1)
        edgui_led_off(led);
    else
        edgui_led_on(led);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the brightness of a LEd object
 * @param led pointer to LED object
 * @return bright 0 (max. dark) ... 255 (max. light)
 */
uint8_t edgui_led_get_bright(const edgui_obj_t * led)
{
    edgui_led_ext_t * ext = edgui_obj_get_ext_attr(led);
    return ext->bright;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the leds
 * @param led pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_led_design(edgui_obj_t * led, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask area*/
        return ancestor_design_f(led, mask, mode);
    } else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        /*Make darker colors in a temporary style according to the brightness*/
        edgui_led_ext_t * ext       = edgui_obj_get_ext_attr(led);
        const edgui_style_t * style = edgui_obj_get_style(led);

        /* Store the real pointer because of 'edgui_group'
         * If the object is in focus 'edgui_obj_get_style()' will give a pointer to tmp style
         * and to the real object style. It is important because of style change tricks below*/
        const edgui_style_t * style_ori_p = led->style_p;

        /*Create a temporal style*/
        edgui_style_t leds_tmp;
        memcpy(&leds_tmp, style, sizeof(leds_tmp));

        /*Mix. the color with black proportionally with brightness*/
        leds_tmp.body.main_color   = edgui_color_mix(leds_tmp.body.main_color, EDGUI_COLOR_BLACK, ext->bright);
        leds_tmp.body.grad_color   = edgui_color_mix(leds_tmp.body.grad_color, EDGUI_COLOR_BLACK, ext->bright);
        leds_tmp.body.border.color = edgui_color_mix(leds_tmp.body.border.color, EDGUI_COLOR_BLACK, ext->bright);

        /*Set the current swidth according to brightness proportionally between EDGUI_LED_BRIGHT_OFF
         * and EDGUI_LED_BRIGHT_ON*/
        uint16_t bright_tmp = ext->bright;
        leds_tmp.body.shadow.width =
            ((bright_tmp - EDGUI_LED_BRIGHT_OFF) * style->body.shadow.width) / (EDGUI_LED_BRIGHT_ON - EDGUI_LED_BRIGHT_OFF);

        led->style_p = &leds_tmp;
        ancestor_design_f(led, mask, mode);
        led->style_p = style_ori_p; /*Restore the ORIGINAL style pointer*/
    }
    return true;
}

/**
 * Signal function of the led
 * @param led pointer to a led object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_led_signal(edgui_obj_t * led, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(led, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_led";
    }

    return res;
}
#endif
