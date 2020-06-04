/**
 * @file edgui_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_sw.h"

#if EDGUI_USE_SW != 0

/*Testing of dependencies*/
#if EDGUI_USE_SLIDER == 0
#error "edgui_sw: edgui_slider is required. Enable it in edgui_conf.h (EDGUI_USE_SLIDER  1) "
#endif

#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_math.h"
#include "../edgui_core/edgui_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_sw_signal(edgui_obj_t * sw, edgui_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a switch objects
 * @param par pointer to an object, it will be the parent of the new switch
 * @param copy pointer to a switch object, if not NULL then the new object will be copied from it
 * @return pointer to the created switch
 */
edgui_obj_t * edgui_sw_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("switch create started");

    /*Create the ancestor of switch*/
    edgui_obj_t * new_sw = edgui_slider_create(par, copy);
    edgui_mem_assert(new_sw);
    if(new_sw == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_sw);

    /*Allocate the switch type specific extended data*/
    edgui_sw_ext_t * ext = edgui_obj_allocate_ext_attr(new_sw, sizeof(edgui_sw_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */
    ext->changed = 0;
#if EDGUI_USE_ANIMATION
    ext->anim_time = 0;
#endif
    ext->style_knob_off = ext->slider.style_knob;
    ext->style_knob_on  = ext->slider.style_knob;

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_sw, edgui_sw_signal);

    /*Init the new switch switch*/
    if(copy == NULL) {
        edgui_obj_set_size(new_sw, 2 * EDGUI_DPI / 3, EDGUI_DPI / 3);
        edgui_slider_set_knob_in(new_sw, true);
        edgui_slider_set_range(new_sw, 0, EDGUI_SW_MAX_VALUE);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_sw_set_style(new_sw, EDGUI_SW_STYLE_BG, th->style.sw.bg);
            edgui_sw_set_style(new_sw, EDGUI_SW_STYLE_INDIC, th->style.sw.indic);
            edgui_sw_set_style(new_sw, EDGUI_SW_STYLE_KNOB_OFF, th->style.sw.knob_off);
            edgui_sw_set_style(new_sw, EDGUI_SW_STYLE_KNOB_ON, th->style.sw.knob_on);
        } else {
            /*Let the slider' style*/
        }

    }
    /*Copy an existing switch*/
    else {
        edgui_sw_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->style_knob_off    = copy_ext->style_knob_off;
        ext->style_knob_on     = copy_ext->style_knob_on;
#if EDGUI_USE_ANIMATION
        ext->anim_time = copy_ext->anim_time;
#endif

        if(edgui_sw_get_state(new_sw))
            edgui_slider_set_style(new_sw, EDGUI_SLIDER_STYLE_KNOB, ext->style_knob_on);
        else
            edgui_slider_set_style(new_sw, EDGUI_SLIDER_STYLE_KNOB, ext->style_knob_off);

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_sw);
    }

    EDGUI_LOG_INFO("switch created");

    return new_sw;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Turn ON the switch
 * @param sw pointer to a switch objec
 * @param anim EDGUI_ANOM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 */
void edgui_sw_on(edgui_obj_t * sw, edgui_anim_enable_t anim)
{
#if EDGUI_USE_ANIMATION == 0
    anim = EDGUI_ANIM_OFF;
#endif
    edgui_sw_ext_t * ext = edgui_obj_get_ext_attr(sw);
    edgui_slider_set_value(sw, EDGUI_SW_MAX_VALUE, anim);
    edgui_slider_set_style(sw, EDGUI_SLIDER_STYLE_KNOB, ext->style_knob_on);
}

/**
 * Turn OFF the switch
 * @param sw pointer to a switch object
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 */
void edgui_sw_off(edgui_obj_t * sw, edgui_anim_enable_t anim)
{
#if EDGUI_USE_ANIMATION == 0
    anim = EDGUI_ANIM_OFF;
#endif
    edgui_sw_ext_t * ext = edgui_obj_get_ext_attr(sw);
    edgui_slider_set_value(sw, 0, anim);
    edgui_slider_set_style(sw, EDGUI_SLIDER_STYLE_KNOB, ext->style_knob_off);
}

/**
 * Toggle the position of the switch
 * @param sw pointer to a switch object
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediately
 * @return resulting state of the switch.
 */
bool edgui_sw_toggle(edgui_obj_t * sw, edgui_anim_enable_t anim)
{
#if EDGUI_USE_ANIMATION == 0
    anim = EDGUI_ANIM_OFF;
#endif

    bool state = edgui_sw_get_state(sw);
    if(state)
        edgui_sw_off(sw, anim);
    else
        edgui_sw_on(sw, anim);

    return !state;
}

/**
 * Set a style of a switch
 * @param sw pointer to a switch object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_sw_set_style(edgui_obj_t * sw, edgui_sw_style_t type, const edgui_style_t * style)
{
    edgui_sw_ext_t * ext = edgui_obj_get_ext_attr(sw);

    switch(type) {
        case EDGUI_SLIDER_STYLE_BG: edgui_slider_set_style(sw, EDGUI_SLIDER_STYLE_BG, style); break;
        case EDGUI_SLIDER_STYLE_INDIC: edgui_bar_set_style(sw, EDGUI_SLIDER_STYLE_INDIC, style); break;
        case EDGUI_SW_STYLE_KNOB_OFF:
            ext->style_knob_off = style;
            if(edgui_sw_get_state(sw) == 0) edgui_slider_set_style(sw, EDGUI_SLIDER_STYLE_KNOB, style);
            break;
        case EDGUI_SW_STYLE_KNOB_ON:
            ext->style_knob_on = style;
            if(edgui_sw_get_state(sw) != 0) edgui_slider_set_style(sw, EDGUI_SLIDER_STYLE_KNOB, style);
            break;
    }
}

void edgui_sw_set_anim_time(edgui_obj_t * sw, uint16_t anim_time)
{
#if EDGUI_USE_ANIMATION
    edgui_sw_ext_t * ext = edgui_obj_get_ext_attr(sw);
    ext->anim_time    = anim_time;
#else
    (void)sw;
    (void)anim_time;
#endif
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get a style of a switch
 * @param sw pointer to a  switch object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_sw_get_style(const edgui_obj_t * sw, edgui_sw_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_sw_ext_t * ext        = edgui_obj_get_ext_attr(sw);

    switch(type) {
        case EDGUI_SW_STYLE_BG: style = edgui_slider_get_style(sw, EDGUI_SLIDER_STYLE_BG); break;
        case EDGUI_SW_STYLE_INDIC: style = edgui_slider_get_style(sw, EDGUI_SLIDER_STYLE_INDIC); break;
        case EDGUI_SW_STYLE_KNOB_OFF: style = ext->style_knob_off; break;
        case EDGUI_SW_STYLE_KNOB_ON: style = ext->style_knob_on; break;
        default: style = NULL; break;
    }

    return style;
}

uint16_t edgui_sw_get_anim_time(const edgui_obj_t * sw)
{

#if EDGUI_USE_ANIMATION
    edgui_sw_ext_t * ext = edgui_obj_get_ext_attr(sw);
    return ext->anim_time;
#else
    (void)sw; /*Unused*/
    return 0;
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the switch
 * @param sw pointer to a switch object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_sw_signal(edgui_obj_t * sw, edgui_signal_t sign, void * param)
{
    edgui_sw_ext_t * ext = edgui_obj_get_ext_attr(sw);

    /*Save the current (old) value before slider signal modifies it. It will be required in the
     * later calculations*/
    int16_t old_val;
    if(sign == EDGUI_SIGNAL_PRESSING)
        old_val = ext->slider.drag_value;
    else
        old_val = edgui_slider_get_value(sw);

    /*Don't let the slider to call the action. Switch handles it differently*/
    edgui_event_cb_t event_cb = sw->event_cb;
    sw->event_cb           = NULL;

    edgui_res_t res;
    /* Include the ancient signal function */

    res = ancestor_signal(sw, sign, param);
    if(res != EDGUI_RES_OK) return res;

    sw->event_cb = event_cb;

    if(sign == EDGUI_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == EDGUI_SIGNAL_PRESSED) {

        /*Save the x coordinate of the pressed point to see if the switch was slid*/
        edgui_indev_t * indev = edgui_indev_get_act();
        if(indev) {
            edgui_point_t p;
            edgui_indev_get_point(indev, &p);
            ext->start_x = p.x;
        }
        ext->slided  = 0;
        ext->changed = 0;
    } else if(sign == EDGUI_SIGNAL_PRESSING) {
        /*See if the switch was slid (moved at least a little)*/
        edgui_indev_t * indev = edgui_indev_get_act();
        if(indev) {
            edgui_point_t p = {0, 0};
            edgui_indev_get_point(indev, &p);
            if(EDGUI_MATH_ABS(p.x - ext->start_x) > EDGUI_INDEV_DEF_DRAG_LIMIT) ext->slided = 1;
        }

        /*If didn't slide then revert the min/max value. So click without slide won't move the
         * switch as a slider*/
        if(ext->slided == 0) {
            if(edgui_sw_get_state(sw))
                ext->slider.drag_value = EDGUI_SW_MAX_VALUE;
            else
                ext->slider.drag_value = 0;
        }

        /*If explicitly changed (by slide) don't need to be toggled on release*/
        int16_t threshold = EDGUI_SW_MAX_VALUE / 2;
        if((old_val < threshold && ext->slider.drag_value > threshold) ||
           (old_val > threshold && ext->slider.drag_value < threshold)) {
            ext->changed = 1;
        }
    } else if(sign == EDGUI_SIGNAL_PRESS_LOST) {
        if(edgui_sw_get_state(sw)) {
            edgui_slider_set_style(sw, EDGUI_SLIDER_STYLE_KNOB, ext->style_knob_on);
            edgui_slider_set_value(sw, EDGUI_SW_MAX_VALUE, EDGUI_ANIM_ON);
            if(res != EDGUI_RES_OK) return res;
        } else {
            edgui_slider_set_style(sw, EDGUI_SLIDER_STYLE_KNOB, ext->style_knob_off);
            edgui_slider_set_value(sw, 0, EDGUI_ANIM_ON);
            if(res != EDGUI_RES_OK) return res;
        }
    } else if(sign == EDGUI_SIGNAL_RELEASED) {
        /*If not dragged then toggle the switch*/
        if(ext->changed == 0) {
            int32_t state;
            if(edgui_sw_get_state(sw)) {
                edgui_sw_off(sw, EDGUI_ANIM_ON);
                state = 0;
            } else {
                edgui_sw_on(sw, EDGUI_ANIM_ON);
                state = 1;
            }

            res = edgui_event_send(sw, EDGUI_EVENT_VALUE_CHANGED, &state);
            if(res != EDGUI_RES_OK) return res;
        }
        /*If the switch was dragged then calculate the new state based on the current position*/
        else {
            int16_t v = edgui_slider_get_value(sw);
            int32_t state;
            if(v > EDGUI_SW_MAX_VALUE / 2) {
                edgui_sw_on(sw, EDGUI_ANIM_ON);
                state = 1;
            } else {
                edgui_sw_off(sw, EDGUI_ANIM_ON);
                state = 0;
            }
            res = edgui_event_send(sw, EDGUI_EVENT_VALUE_CHANGED, &state);
            if(res != EDGUI_RES_OK) return res;
        }
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        char c = *((char *)param);
        uint32_t state;
        if(c == EDGUI_KEY_RIGHT || c == EDGUI_KEY_UP) {
            edgui_slider_set_value(sw, EDGUI_SW_MAX_VALUE, true);
            state = 1;
            res   = edgui_event_send(sw, EDGUI_EVENT_VALUE_CHANGED, &state);
            if(res != EDGUI_RES_OK) return res;
        } else if(c == EDGUI_KEY_LEFT || c == EDGUI_KEY_DOWN) {
            edgui_slider_set_value(sw, 0, true);
            state = 0;
            res   = edgui_event_send(sw, EDGUI_EVENT_VALUE_CHANGED, &state);
            if(res != EDGUI_RES_OK) return res;
        }
    } else if(sign == EDGUI_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = false; /*The ancestor slider is editable the switch is not*/
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_sw";
    }

    return res;
}

#endif
