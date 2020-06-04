/**
 * @file edgui_btn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "edgui_btn.h"
#if EDGUI_USE_BTN != 0

#include <string.h>
#include "../edgui_core/edgui_group.h"
#include "../edgui_draw/edgui_draw.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_area.h"
#include "../edgui_misc/edgui_color.h"
#include "../edgui_misc/edgui_math.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_BTN_INK_VALUE_MAX 256
#define EDGUI_BTN_INK_VALUE_MAX_SHIFT 8

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_btn_design(edgui_obj_t * btn, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_btn_signal(edgui_obj_t * btn, edgui_signal_t sign, void * param);

#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
static void edgui_btn_ink_effect_anim(edgui_obj_t * btn, edgui_anim_value_t val);
static void edgui_btn_ink_effect_anim_ready(edgui_anim_t * a);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;
static edgui_design_cb_t ancestor_design;

#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
static edgui_coord_t ink_act_value;
static edgui_obj_t * ink_obj;
static edgui_btn_state_t ink_bg_state;
static edgui_btn_state_t ink_top_state;
static bool ink_ready;
static bool ink_playback;
static edgui_point_t ink_point;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a button object
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
edgui_obj_t * edgui_btn_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("button create started");

    edgui_obj_t * new_btn;

    new_btn = edgui_cont_create(par, copy);
    edgui_mem_assert(new_btn);
    if(new_btn == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_btn);
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_btn);

    /*Allocate the extended data*/
    edgui_btn_ext_t * ext = edgui_obj_allocate_ext_attr(new_btn, sizeof(edgui_btn_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->state = EDGUI_BTN_STATE_REL;

    ext->styles[EDGUI_BTN_STATE_REL]     = &edgui_style_btn_rel;
    ext->styles[EDGUI_BTN_STATE_PR]      = &edgui_style_btn_pr;
    ext->styles[EDGUI_BTN_STATE_TGL_REL] = &edgui_style_btn_tgl_rel;
    ext->styles[EDGUI_BTN_STATE_TGL_PR]  = &edgui_style_btn_tgl_pr;
    ext->styles[EDGUI_BTN_STATE_INA]     = &edgui_style_btn_ina;

    ext->toggle = 0;
#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
    ext->ink_in_time   = 0;
    ext->ink_wait_time = 0;
    ext->ink_out_time  = 0;
#endif

    edgui_obj_set_signal_cb(new_btn, edgui_btn_signal);
    edgui_obj_set_design_cb(new_btn, edgui_btn_design);

    /*If no copy do the basic initialization*/
    if(copy == NULL) {
        /*Set layout if the button is not a screen*/
        if(par != NULL) {
            edgui_btn_set_layout(new_btn, EDGUI_LAYOUT_CENTER);
        }

        edgui_obj_set_click(new_btn, true); /*Be sure the button is clickable*/

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_btn_set_style(new_btn, EDGUI_BTN_STYLE_REL, th->style.btn.rel);
            edgui_btn_set_style(new_btn, EDGUI_BTN_STYLE_PR, th->style.btn.pr);
            edgui_btn_set_style(new_btn, EDGUI_BTN_STYLE_TGL_REL, th->style.btn.tgl_rel);
            edgui_btn_set_style(new_btn, EDGUI_BTN_STYLE_TGL_PR, th->style.btn.tgl_pr);
            edgui_btn_set_style(new_btn, EDGUI_BTN_STYLE_INA, th->style.btn.ina);
        } else {
            edgui_obj_set_style(new_btn, ext->styles[EDGUI_BTN_STATE_REL]);
        }
    }
    /*Copy 'copy'*/
    else {
        edgui_btn_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->state              = copy_ext->state;
        ext->toggle             = copy_ext->toggle;
#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
        ext->ink_in_time   = copy_ext->ink_in_time;
        ext->ink_wait_time = copy_ext->ink_wait_time;
        ext->ink_out_time  = copy_ext->ink_out_time;
#endif
        memcpy(ext->styles, copy_ext->styles, sizeof(ext->styles));

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_btn);
    }

    EDGUI_LOG_INFO("button created");

    return new_btn;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Enable the toggled states
 * @param btn pointer to a button object
 * @param tgl true: enable toggled states, false: disable
 */
void edgui_btn_set_toggle(edgui_obj_t * btn, bool tgl)
{
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);

    ext->toggle = tgl != false ? 1 : 0;
}

/**
 * Set the state of the button
 * @param btn pointer to a button object
 * @param state the new state of the button (from edgui_btn_state_t enum)
 */
void edgui_btn_set_state(edgui_obj_t * btn, edgui_btn_state_t state)
{
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);
    if(ext->state != state) {
        ext->state = state;
        edgui_obj_set_style(btn, ext->styles[state]);
    }
}

/**
 * Toggle the state of the button (ON->OFF, OFF->ON)
 * @param btn pointer to a button object
 */
void edgui_btn_toggle(edgui_obj_t * btn)
{
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);
    switch(ext->state) {
        case EDGUI_BTN_STATE_REL: edgui_btn_set_state(btn, EDGUI_BTN_STATE_TGL_REL); break;
        case EDGUI_BTN_STATE_PR: edgui_btn_set_state(btn, EDGUI_BTN_STATE_TGL_PR); break;
        case EDGUI_BTN_STATE_TGL_REL: edgui_btn_set_state(btn, EDGUI_BTN_STATE_REL); break;
        case EDGUI_BTN_STATE_TGL_PR: edgui_btn_set_state(btn, EDGUI_BTN_STATE_PR); break;
        default: break;
    }
}

/**
 * Set time of the ink effect (draw a circle on click to animate in the new state)
 * @param btn pointer to a button object
 * @param time the time of the ink animation
 */
void edgui_btn_set_ink_in_time(edgui_obj_t * btn, uint16_t time)
{
#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);
    ext->ink_in_time   = time;
#else
    (void)btn;  /*Unused*/
    (void)time; /*Unused*/
    EDGUI_LOG_WARN("`edgui_btn_set_ink_ink_time` has no effect if EDGUI_BTN_INK_EFEFCT or EDGUI_USE_ANIMATION "
                "is disabled")
#endif
}

/**
 * Set the wait time before the ink disappears
 * @param btn pointer to a button object
 * @param time the time of the ink animation
 */
void edgui_btn_set_ink_wait_time(edgui_obj_t * btn, uint16_t time)
{

#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);
    ext->ink_wait_time = time;
#else
    (void)btn;  /*Unused*/
    (void)time; /*Unused*/
    EDGUI_LOG_WARN("`edgui_btn_set_ink_wait_time` has no effect if EDGUI_BTN_INK_EFEFCT or EDGUI_USE_ANIMATION "
                "is disabled")
#endif
}

/**
 * Set time of the ink out effect (animate to the released state)
 * @param btn pointer to a button object
 * @param time the time of the ink animation
 */
void edgui_btn_set_ink_out_time(edgui_obj_t * btn, uint16_t time)
{
#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);
    ext->ink_out_time  = time;
#else
    (void)btn;  /*Unused*/
    (void)time; /*Unused*/
    EDGUI_LOG_WARN("`edgui_btn_set_ink_out_time` has no effect if EDGUI_BTN_INK_EFEFCT or EDGUI_USE_ANIMATION "
                "is disabled")
#endif
}

/**
 * Set a style of a button
 * @param btn pointer to a button object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_btn_set_style(edgui_obj_t * btn, edgui_btn_style_t type, const edgui_style_t * style)
{
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);

    switch(type) {
        case EDGUI_BTN_STYLE_REL: ext->styles[EDGUI_BTN_STATE_REL] = style; break;
        case EDGUI_BTN_STYLE_PR: ext->styles[EDGUI_BTN_STATE_PR] = style; break;
        case EDGUI_BTN_STYLE_TGL_REL: ext->styles[EDGUI_BTN_STATE_TGL_REL] = style; break;
        case EDGUI_BTN_STYLE_TGL_PR: ext->styles[EDGUI_BTN_STATE_TGL_PR] = style; break;
        case EDGUI_BTN_STYLE_INA: ext->styles[EDGUI_BTN_STATE_INA] = style; break;
    }

    /*Refresh the object with the new style*/
    edgui_obj_set_style(btn, ext->styles[ext->state]);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current state of the button
 * @param btn pointer to a button object
 * @return the state of the button (from edgui_btn_state_t enum)
 */
edgui_btn_state_t edgui_btn_get_state(const edgui_obj_t * btn)
{
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);
    return ext->state;
}

/**
 * Get the toggle enable attribute of the button
 * @param btn pointer to a button object
 * @return true: toggle enabled, false: disabled
 */
bool edgui_btn_get_toggle(const edgui_obj_t * btn)
{
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);

    return ext->toggle != 0 ? true : false;
}

/**
 * Get time of the ink in effect (draw a circle on click to animate in the new state)
 * @param btn pointer to a button object
 * @return the time of the ink animation
 */
uint16_t edgui_btn_get_ink_in_time(const edgui_obj_t * btn)
{
#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);
    return ext->ink_in_time;
#else
    (void)btn; /*Unused*/
    return 0;
#endif
}

/**
 * Get the wait time before the ink disappears
 * @param btn pointer to a button object
 * @return the time of the ink animation
 */
uint16_t edgui_btn_get_ink_wait_time(const edgui_obj_t * btn)
{
#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);
    return ext->ink_wait_time;
#else
    (void)btn; /*Unused*/
    return 0;
#endif
}
/**
 * Get time of the ink out effect (animate to the releases state)
 * @param btn pointer to a button object
 * @return the time of the ink animation
 */
uint16_t edgui_btn_get_ink_out_time(const edgui_obj_t * btn)
{
#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);
    return ext->ink_out_time;
#else
    (void)btn; /*Unused*/
    return 0;
#endif
}

/**
 * Get a style of a button
 * @param btn pointer to a button object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_btn_get_style(const edgui_obj_t * btn, edgui_btn_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_btn_ext_t * ext       = edgui_obj_get_ext_attr(btn);
    edgui_btn_state_t state     = edgui_btn_get_state(btn);

    /* If the style of the current state is asked then return object style.
     * If the button is focused then this style is updated by the group's
     * `style_mod_cb` function */
    if((type == EDGUI_BTN_STYLE_REL && state == EDGUI_BTN_STATE_REL) ||
       (type == EDGUI_BTN_STYLE_PR && state == EDGUI_BTN_STATE_PR) ||
       (type == EDGUI_BTN_STYLE_TGL_REL && state == EDGUI_BTN_STATE_TGL_REL) ||
       (type == EDGUI_BTN_STYLE_TGL_PR && state == EDGUI_BTN_STATE_TGL_PR) ||
       (type == EDGUI_BTN_STYLE_INA && state == EDGUI_BTN_STATE_INA)) {

        style = edgui_obj_get_style(btn);
    } else {
        switch(type) {
            case EDGUI_BTN_STYLE_REL: style = ext->styles[EDGUI_BTN_STATE_REL]; break;
            case EDGUI_BTN_STYLE_PR: style = ext->styles[EDGUI_BTN_STATE_PR]; break;
            case EDGUI_BTN_STYLE_TGL_REL: style = ext->styles[EDGUI_BTN_STATE_TGL_REL]; break;
            case EDGUI_BTN_STYLE_TGL_PR: style = ext->styles[EDGUI_BTN_STATE_TGL_PR]; break;
            case EDGUI_BTN_STYLE_INA: style = ext->styles[EDGUI_BTN_STATE_INA]; break;
            default: style = NULL; break;
        }
    }

    return style;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the drop down lists
 * @param btn pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_btn_design(edgui_obj_t * btn, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return false;
    } else if(mode == EDGUI_DESIGN_DRAW_MAIN) {

#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
        if(btn != ink_obj) {
            ancestor_design(btn, mask, mode);
        } else {
            edgui_opa_t opa_scale = edgui_obj_get_opa_scale(btn);
            edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);

            /*Draw the normal button*/
            if(ink_playback == false) {
                edgui_style_t style_tmp;
                edgui_style_copy(&style_tmp, ext->styles[ink_bg_state]);
                style_tmp.body.shadow.width = ext->styles[ink_top_state]->body.shadow.width;
                edgui_draw_rect(&btn->coords, mask, &style_tmp, opa_scale);

                edgui_coord_t w     = edgui_obj_get_width(btn);
                edgui_coord_t h     = edgui_obj_get_height(btn);
                edgui_coord_t r_max = EDGUI_MATH_MIN(w, h) / 2;

                /*In the first part of the animation increase the size of the circle (ink effect) */
                edgui_area_t cir_area;

                edgui_coord_t coord_state =
                    ink_act_value < EDGUI_BTN_INK_VALUE_MAX / 2 ? ink_act_value : EDGUI_BTN_INK_VALUE_MAX / 2;
                edgui_point_t p_act;
                p_act.x          = ink_point.x;
                p_act.y          = ink_point.y;
                edgui_coord_t x_err = (btn->coords.x1 + w / 2) - p_act.x;
                edgui_coord_t y_err = (btn->coords.y1 + h / 2) - p_act.y;

                p_act.x += (x_err * coord_state) >> (EDGUI_BTN_INK_VALUE_MAX_SHIFT - 1);
                p_act.y += (y_err * coord_state) >> (EDGUI_BTN_INK_VALUE_MAX_SHIFT - 1);

                edgui_coord_t half_side = EDGUI_MATH_MAX(w, h) / 2;
                cir_area.x1          = p_act.x - ((half_side * coord_state) >> (EDGUI_BTN_INK_VALUE_MAX_SHIFT - 1));
                cir_area.y1          = p_act.y - ((half_side * coord_state) >> (EDGUI_BTN_INK_VALUE_MAX_SHIFT - 1));
                cir_area.x2          = p_act.x + ((half_side * coord_state) >> (EDGUI_BTN_INK_VALUE_MAX_SHIFT - 1));
                cir_area.y2          = p_act.y + ((half_side * coord_state) >> (EDGUI_BTN_INK_VALUE_MAX_SHIFT - 1));

                edgui_area_intersect(&cir_area, &btn->coords,
                                  &cir_area); /*Limit the area. (It might be too big on the smaller side)*/

                /*In the second part animate the radius. Circle -> body.radius*/
                edgui_coord_t r_state =
                    ink_act_value > EDGUI_BTN_INK_VALUE_MAX / 2 ? ink_act_value - EDGUI_BTN_INK_VALUE_MAX / 2 : 0;

                edgui_style_copy(&style_tmp, ext->styles[ink_top_state]);
                style_tmp.body.radius       = r_max + (((ext->styles[ink_bg_state]->body.radius - r_max) * r_state) >>
                                                 (EDGUI_BTN_INK_VALUE_MAX_SHIFT - 1));
                style_tmp.body.border.width = 0;

                /*Draw the circle*/
                edgui_draw_rect(&cir_area, mask, &style_tmp, opa_scale);
            } else {
                edgui_style_t res;
                edgui_style_copy(&res, ext->styles[ink_bg_state]);
                edgui_style_mix(ext->styles[ink_bg_state], ext->styles[ink_top_state], &res, ink_act_value);
                edgui_draw_rect(&btn->coords, mask, &res, opa_scale);
            }
        }
#else
        ancestor_design(btn, mask, mode);
#endif
    } else if(mode == EDGUI_DESIGN_DRAW_POST) {
        ancestor_design(btn, mask, mode);
    }

    return true;
}

/**
 * Signal function of the button
 * @param btn pointer to a button object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_btn_signal(edgui_obj_t * btn, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(btn, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_btn_ext_t * ext = edgui_obj_get_ext_attr(btn);
    bool tgl           = edgui_btn_get_toggle(btn);

    if(sign == EDGUI_SIGNAL_PRESSED) {
        /*Refresh the state*/
        if(ext->state == EDGUI_BTN_STATE_REL) {
            edgui_btn_set_state(btn, EDGUI_BTN_STATE_PR);
#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
            ink_bg_state  = EDGUI_BTN_STATE_REL;
            ink_top_state = EDGUI_BTN_STATE_PR;
#endif
        } else if(ext->state == EDGUI_BTN_STATE_TGL_REL) {
            edgui_btn_set_state(btn, EDGUI_BTN_STATE_TGL_PR);
#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
            ink_bg_state  = EDGUI_BTN_STATE_TGL_REL;
            ink_top_state = EDGUI_BTN_STATE_TGL_PR;
#endif
        }

#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
        /*Forget the old inked button*/
        if(ink_obj != NULL && ink_obj != btn) {
            edgui_anim_del(ink_obj, (edgui_anim_exec_xcb_t)edgui_btn_ink_effect_anim);
            edgui_obj_invalidate(ink_obj);
            ink_obj = NULL;
        }
        /*Save the new data for inking and start it's animation if enabled*/
        if(ext->ink_in_time > 0) {
            ink_obj      = btn;
            ink_playback = false;
            ink_ready    = false;
            edgui_indev_get_point(edgui_indev_get_act(), &ink_point);

            edgui_anim_t a;
            a.var            = btn;
            a.start          = 0;
            a.end            = EDGUI_BTN_INK_VALUE_MAX;
            a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_btn_ink_effect_anim;
            a.path_cb        = edgui_anim_path_linear;
            a.ready_cb       = edgui_btn_ink_effect_anim_ready;
            a.act_time       = 0;
            a.time           = ext->ink_in_time;
            a.playback       = 0;
            a.playback_pause = 0;
            a.repeat         = 0;
            a.repeat_pause   = 0;
            edgui_anim_create(&a);
        }
#endif
    } else if(sign == EDGUI_SIGNAL_PRESS_LOST) {
        /*Refresh the state*/
        if(ext->state == EDGUI_BTN_STATE_PR)
            edgui_btn_set_state(btn, EDGUI_BTN_STATE_REL);
        else if(ext->state == EDGUI_BTN_STATE_TGL_PR)
            edgui_btn_set_state(btn, EDGUI_BTN_STATE_TGL_REL);
    } else if(sign == EDGUI_SIGNAL_PRESSING) {
        /*When the button begins to drag revert pressed states to released*/
        if(edgui_indev_is_dragging(param) != false) {
            if(ext->state == EDGUI_BTN_STATE_PR)
                edgui_btn_set_state(btn, EDGUI_BTN_STATE_REL);
            else if(ext->state == EDGUI_BTN_STATE_TGL_PR)
                edgui_btn_set_state(btn, EDGUI_BTN_STATE_TGL_REL);
        }
    } else if(sign == EDGUI_SIGNAL_RELEASED) {
        /*If not dragged and it was not long press action then
         *change state and run the action*/
        if(edgui_indev_is_dragging(param) == false) {
            uint32_t toggled = 0;
            if(ext->state == EDGUI_BTN_STATE_PR && tgl == false) {
                edgui_btn_set_state(btn, EDGUI_BTN_STATE_REL);
                toggled = 0;
            } else if(ext->state == EDGUI_BTN_STATE_TGL_PR && tgl == false) {
                edgui_btn_set_state(btn, EDGUI_BTN_STATE_TGL_REL);
                toggled = 1;
            } else if(ext->state == EDGUI_BTN_STATE_PR && tgl == true) {
                edgui_btn_set_state(btn, EDGUI_BTN_STATE_TGL_REL);
                toggled = 1;
            } else if(ext->state == EDGUI_BTN_STATE_TGL_PR && tgl == true) {
                edgui_btn_set_state(btn, EDGUI_BTN_STATE_REL);
                toggled = 0;
            }

            if(tgl) {
                res = edgui_event_send(btn, EDGUI_EVENT_VALUE_CHANGED, &toggled);
                if(res != EDGUI_RES_OK) return res;
            }

        } else { /*If dragged change back the state*/
            if(ext->state == EDGUI_BTN_STATE_PR) {
                edgui_btn_set_state(btn, EDGUI_BTN_STATE_REL);
            } else if(ext->state == EDGUI_BTN_STATE_TGL_PR) {
                edgui_btn_set_state(btn, EDGUI_BTN_STATE_TGL_REL);
            }
        }

#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
        /*Draw the toggled state in the inking instead*/
        if(ext->toggle) {
            ink_top_state = ext->state;
        }
        /*If not a toggle button and the "IN" inking is ready then start an "OUT" inking*/
        else if(ink_ready && ext->ink_out_time > 0) {
            ink_obj      = btn;
            ink_playback = true; /*It is the playback. If not set `edgui_btn_ink_effect_anim_ready`
                                    will start its own playback*/
            edgui_indev_get_point(edgui_indev_get_act(), &ink_point);

            edgui_anim_t a;
            a.var            = ink_obj;
            a.start          = EDGUI_BTN_INK_VALUE_MAX;
            a.end            = 0;
            a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_btn_ink_effect_anim;
            a.path_cb        = edgui_anim_path_linear;
            a.ready_cb       = edgui_btn_ink_effect_anim_ready;
            a.act_time       = 0;
            a.time           = ext->ink_out_time;
            a.playback       = 0;
            a.playback_pause = 0;
            a.repeat         = 0;
            a.repeat_pause   = 0;
            edgui_anim_create(&a);
        }
#endif
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == EDGUI_KEY_RIGHT || c == EDGUI_KEY_UP) {
            if(edgui_btn_get_toggle(btn)) {
                edgui_btn_set_state(btn, EDGUI_BTN_STATE_TGL_REL);

                uint32_t state = 1;
                res            = edgui_event_send(btn, EDGUI_EVENT_VALUE_CHANGED, &state);
                if(res != EDGUI_RES_OK) return res;
            }

        } else if(c == EDGUI_KEY_LEFT || c == EDGUI_KEY_DOWN) {
            if(edgui_btn_get_toggle(btn)) {
                edgui_btn_set_state(btn, EDGUI_BTN_STATE_REL);

                uint32_t state = 0;
                res            = edgui_event_send(btn, EDGUI_EVENT_VALUE_CHANGED, &state);
                if(res != EDGUI_RES_OK) return res;
            }
        }
    } else if(sign == EDGUI_SIGNAL_CLEANUP) {
#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT
        if(btn == ink_obj) {
            edgui_anim_del(ink_obj, (edgui_anim_exec_xcb_t)edgui_btn_ink_effect_anim);
            ink_obj = NULL;
        }
#endif
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_btn";
    }

    return res;
}

#if EDGUI_USE_ANIMATION && EDGUI_BTN_INK_EFFECT

/**
 * The animator function of inking. CAlled to increase the radius of ink
 * @param btn pointer to the animated button
 * @param val the new radius
 */
static void edgui_btn_ink_effect_anim(edgui_obj_t * btn, edgui_anim_value_t val)
{
    if(btn) {
        ink_act_value = val;
        edgui_obj_invalidate(btn);
    }
}

/**
 * Called to clean up when the ink animation is ready
 * @param a unused
 */
static void edgui_btn_ink_effect_anim_ready(edgui_anim_t * a)
{
    (void)a; /*Unused*/

    edgui_btn_ext_t * ext   = edgui_obj_get_ext_attr(ink_obj);
    edgui_btn_state_t state = edgui_btn_get_state(ink_obj);

    edgui_obj_invalidate(ink_obj);
    ink_ready = true;

    if((state == EDGUI_BTN_STATE_REL || state == EDGUI_BTN_STATE_TGL_REL) && ext->toggle == 0 && ink_playback == false) {
        edgui_anim_t new_a;
        new_a.var            = ink_obj;
        new_a.start          = EDGUI_BTN_INK_VALUE_MAX;
        new_a.end            = 0;
        new_a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_btn_ink_effect_anim;
        new_a.path_cb        = edgui_anim_path_linear;
        new_a.ready_cb       = edgui_btn_ink_effect_anim_ready;
        new_a.act_time       = -ext->ink_wait_time;
        new_a.time           = ext->ink_out_time;
        new_a.playback       = 0;
        new_a.playback_pause = 0;
        new_a.repeat         = 0;
        new_a.repeat_pause   = 0;
        edgui_anim_create(&new_a);

        ink_playback = true;
    } else {
        ink_obj = NULL;
    }
}
#endif /*EDGUI_USE_ANIMATION*/

#endif
