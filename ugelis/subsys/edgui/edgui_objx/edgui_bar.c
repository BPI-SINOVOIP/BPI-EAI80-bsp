

/**
 * @file edgui_bar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_bar.h"
#if EDGUI_USE_BAR != 0

#include "../edgui_draw/edgui_draw.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_anim.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_bar_design(edgui_obj_t * bar, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_bar_signal(edgui_obj_t * bar, edgui_signal_t sign, void * param);

#if EDGUI_USE_ANIMATION
static void edgui_bar_anim(void * bar, edgui_anim_value_t value);
static void edgui_bar_anim_ready(edgui_anim_t * a);
#endif

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
 * Create a bar objects
 * @param par pointer to an object, it will be the parent of the new bar
 * @param copy pointer to a bar object, if not NULL then the new object will be copied from it
 * @return pointer to the created bar
 */
edgui_obj_t * edgui_bar_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("edgui_bar create started");

    /*Create the ancestor basic object*/
    edgui_obj_t * new_bar = edgui_obj_create(par, copy);
    edgui_mem_assert(new_bar);
    if(new_bar == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_bar);
    if(ancestor_design_f == NULL) ancestor_design_f = edgui_obj_get_design_cb(new_bar);

    /*Allocate the object type specific extended data*/
    edgui_bar_ext_t * ext = edgui_obj_allocate_ext_attr(new_bar, sizeof(edgui_bar_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->min_value = 0;
    ext->max_value = 100;
    ext->cur_value = 0;
#if EDGUI_USE_ANIMATION
    ext->anim_time  = 200;
    ext->anim_start = 0;
    ext->anim_end   = 0;
    ext->anim_state = EDGUI_BAR_ANIM_STATE_INV;
#endif
    ext->sym         = 0;
    ext->style_indic = &edgui_style_pretty_color;

    edgui_obj_set_signal_cb(new_bar, edgui_bar_signal);
    edgui_obj_set_design_cb(new_bar, edgui_bar_design);

    /*Init the new  bar object*/
    if(copy == NULL) {
        edgui_obj_set_click(new_bar, false);
        edgui_obj_set_size(new_bar, EDGUI_DPI * 2, EDGUI_DPI / 3);
        edgui_bar_set_value(new_bar, ext->cur_value, false);

        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_bar_set_style(new_bar, EDGUI_BAR_STYLE_BG, th->style.bar.bg);
            edgui_bar_set_style(new_bar, EDGUI_BAR_STYLE_INDIC, th->style.bar.indic);
        } else {
            edgui_obj_set_style(new_bar, &edgui_style_pretty);
        }
    } else {
        edgui_bar_ext_t * ext_copy = edgui_obj_get_ext_attr(copy);
        ext->min_value          = ext_copy->min_value;
        ext->max_value          = ext_copy->max_value;
        ext->cur_value          = ext_copy->cur_value;
        ext->style_indic        = ext_copy->style_indic;
        ext->sym                = ext_copy->sym;
        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_bar);

        edgui_bar_set_value(new_bar, ext->cur_value, false);
    }

    EDGUI_LOG_INFO("bar created");

    return new_bar;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the bar
 * @param bar pointer to a bar object
 * @param value new value
 * @param anim EDGUI_ANIM_ON: set the value with an animation; EDGUI_ANIM_OFF: change the value immediatelly
 */
void edgui_bar_set_value(edgui_obj_t * bar, int16_t value, edgui_anim_enable_t anim)
{
#if EDGUI_USE_ANIMATION == 0
    anim = false;
#endif
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);
    if(ext->cur_value == value) return;

    int16_t new_value;
    new_value = value > ext->max_value ? ext->max_value : value;
    new_value = new_value < ext->min_value ? ext->min_value : new_value;

    if(ext->cur_value == new_value) return;

    if(anim == EDGUI_ANIM_OFF) {
        ext->cur_value = new_value;
        edgui_obj_invalidate(bar);
    } else {
#if EDGUI_USE_ANIMATION
        /*No animation in progress -> simply set the values*/
        if(ext->anim_state == EDGUI_BAR_ANIM_STATE_INV) {
            ext->anim_start = ext->cur_value;
            ext->anim_end   = new_value;
        }
        /*Animation in progress. Start from the animation end value*/
        else {
            ext->anim_start = ext->anim_end;
            ext->anim_end   = new_value;
        }

        edgui_anim_t a;
        a.var            = bar;
        a.start          = EDGUI_BAR_ANIM_STATE_START;
        a.end            = EDGUI_BAR_ANIM_STATE_END;
        a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_bar_anim;
        a.path_cb        = edgui_anim_path_linear;
        a.ready_cb       = edgui_bar_anim_ready;
        a.act_time       = 0;
        a.time           = ext->anim_time;
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;

        edgui_anim_create(&a);
#endif
    }
}

/**
 * Set minimum and the maximum values of a bar
 * @param bar pointer to the bar object
 * @param min minimum value
 * @param max maximum value
 */
void edgui_bar_set_range(edgui_obj_t * bar, int16_t min, int16_t max)
{
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);
    if(ext->min_value == min && ext->max_value == max) return;

    ext->max_value = max;
    ext->min_value = min;
    if(ext->cur_value > max) {
        ext->cur_value = max;
        edgui_bar_set_value(bar, ext->cur_value, false);
    }
    if(ext->cur_value < min) {
        ext->cur_value = min;
        edgui_bar_set_value(bar, ext->cur_value, false);
    }
    edgui_obj_invalidate(bar);
}

/**
 * Make the bar symmetric to zero. The indicator will grow from zero instead of the minimum
 * position.
 * @param bar pointer to a bar object
 * @param en true: enable disable symmetric behavior; false: disable
 */
void edgui_bar_set_sym(edgui_obj_t * bar, bool en)
{
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);
    ext->sym           = en ? 1 : 0;
}

/**
 * Set the animation time of the bar
 * @param bar pointer to a bar object
 * @param anim_time the animation time in milliseconds.
 */
void edgui_bar_set_anim_time(edgui_obj_t * bar, uint16_t anim_time)
{
#if EDGUI_USE_ANIMATION
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);
    ext->anim_time     = anim_time;
#else
    (void)bar;       /*Unused*/
    (void)anim_time; /*Unused*/
#endif
}

/**
 * Set a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_bar_set_style(edgui_obj_t * bar, edgui_bar_style_t type, const edgui_style_t * style)
{
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);

    switch(type) {
        case EDGUI_BAR_STYLE_BG: edgui_obj_set_style(bar, style); break;
        case EDGUI_BAR_STYLE_INDIC:
            ext->style_indic = style;
            edgui_obj_refresh_ext_draw_pad(bar);
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a bar
 * @param bar pointer to a bar object
 * @return the value of the bar
 */
int16_t edgui_bar_get_value(const edgui_obj_t * bar)
{
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);
    /*If animated tell that it's already at the end value*/
#if EDGUI_USE_ANIMATION
    if(ext->anim_state != EDGUI_BAR_ANIM_STATE_INV) return ext->anim_end;
#endif
    /*No animation, simple return the current value*/
    return ext->cur_value;
}

/**
 * Get the minimum value of a bar
 * @param bar pointer to a bar object
 * @return the minimum value of the bar
 */
int16_t edgui_bar_get_min_value(const edgui_obj_t * bar)
{
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);
    return ext->min_value;
}

/**
 * Get the maximum value of a bar
 * @param bar pointer to a bar object
 * @return the maximum value of the bar
 */
int16_t edgui_bar_get_max_value(const edgui_obj_t * bar)
{
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);
    return ext->max_value;
}

/**
 * Get whether the bar is symmetric or not.
 * @param bar pointer to a bar object
 * @return true: symmetric is enabled; false: disable
 */
bool edgui_bar_get_sym(edgui_obj_t * bar)
{
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);
    return ext->sym ? true : false;
}

/**
 * Get the animation time of the bar
 * @param bar pointer to a bar object
 * @return the animation time in milliseconds.
 */
uint16_t edgui_bar_get_anim_time(edgui_obj_t * bar)
{
#if EDGUI_USE_ANIMATION
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);
    return ext->anim_time;
#else
    (void)bar;       /*Unused*/
    return 0;
#endif
}

/**
 * Get a style of a bar
 * @param bar pointer to a bar object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_bar_get_style(const edgui_obj_t * bar, edgui_bar_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_bar_ext_t * ext       = edgui_obj_get_ext_attr(bar);

    switch(type) {
        case EDGUI_BAR_STYLE_BG: style = edgui_obj_get_style(bar); break;
        case EDGUI_BAR_STYLE_INDIC: style = ext->style_indic; break;
        default: style = NULL; break;
    }

    return style;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the bars
 * @param bar pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_bar_design(edgui_obj_t * bar, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask area*/
        return ancestor_design_f(bar, mask, mode);
    } else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        edgui_opa_t opa_scale = edgui_obj_get_opa_scale(bar);

#if EDGUI_USE_GROUP == 0
        ancestor_design_f(bar, mask, mode);
#else
        /* Draw the borders later if the bar is focused.
         * At value = 100% the indicator can cover to whole background and the focused style won't
         * be visible*/
        if(edgui_obj_is_focused(bar)) {
            const edgui_style_t * style_bg = edgui_bar_get_style(bar, EDGUI_BAR_STYLE_BG);
            edgui_style_t style_tmp;
            edgui_style_copy(&style_tmp, style_bg);
            style_tmp.body.border.width = 0;
            edgui_draw_rect(&bar->coords, mask, &style_tmp, opa_scale);
        } else {
            ancestor_design_f(bar, mask, mode);
        }
#endif
        edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);

        if(ext->cur_value != ext->min_value || ext->sym
#if EDGUI_USE_ANIMATION
           || ext->anim_start != EDGUI_BAR_ANIM_STATE_INV
#endif
        ) {
            const edgui_style_t * style_indic = edgui_bar_get_style(bar, EDGUI_BAR_STYLE_INDIC);
            edgui_area_t indic_area;
            edgui_area_copy(&indic_area, &bar->coords);
            indic_area.x1 += style_indic->body.padding.left;
            indic_area.x2 -= style_indic->body.padding.right;
            indic_area.y1 += style_indic->body.padding.top;
            indic_area.y2 -= style_indic->body.padding.bottom;

            edgui_coord_t w = edgui_area_get_width(&indic_area);
            edgui_coord_t h = edgui_area_get_height(&indic_area);

            if(w >= h) {
                /*Horizontal*/
#if EDGUI_USE_ANIMATION
                if(ext->anim_state != EDGUI_BAR_ANIM_STATE_INV) {
                    /*Calculate the coordinates of anim. start and end*/
                    edgui_coord_t anim_start_x =
                        (int32_t)((int32_t)w * (ext->anim_start - ext->min_value)) / (ext->max_value - ext->min_value);
                    edgui_coord_t anim_end_x =
                        (int32_t)((int32_t)w * (ext->anim_end - ext->min_value)) / (ext->max_value - ext->min_value);

                    /*Calculate the real position based on `anim_state` (between `anim_start` and
                     * `anim_end`)*/
                    indic_area.x2 =
                        anim_start_x + (((anim_end_x - anim_start_x) * ext->anim_state) >> EDGUI_BAR_ANIM_STATE_NORM);
                } else
#endif
                {
                    indic_area.x2 =
                        (int32_t)((int32_t)w * (ext->cur_value - ext->min_value)) / (ext->max_value - ext->min_value);
                }

                indic_area.x2 = indic_area.x1 + indic_area.x2 - 1;
                if(ext->sym && ext->min_value < 0 && ext->max_value > 0) {
                    /*Calculate the coordinate of the zero point*/
                    edgui_coord_t zero;
                    zero = indic_area.x1 + (-ext->min_value * w) / (ext->max_value - ext->min_value);
                    if(indic_area.x2 > zero)
                        indic_area.x1 = zero;
                    else {
                        indic_area.x1 = indic_area.x2;
                        indic_area.x2 = zero;
                    }
                }
            } else {
#if EDGUI_USE_ANIMATION
                if(ext->anim_state != EDGUI_BAR_ANIM_STATE_INV) {
                    /*Calculate the coordinates of anim. start and end*/
                    edgui_coord_t anim_start_y =
                        (int32_t)((int32_t)h * (ext->anim_start - ext->min_value)) / (ext->max_value - ext->min_value);
                    edgui_coord_t anim_end_y =
                        (int32_t)((int32_t)h * (ext->anim_end - ext->min_value)) / (ext->max_value - ext->min_value);

                    /*Calculate the real position based on `anim_state` (between `anim_start` and
                     * `anim_end`)*/
                    indic_area.y1 =
                        anim_start_y + (((anim_end_y - anim_start_y) * ext->anim_state) >> EDGUI_BAR_ANIM_STATE_NORM);
                } else
#endif
                {
                    indic_area.y1 =
                        (int32_t)((int32_t)h * (ext->cur_value - ext->min_value)) / (ext->max_value - ext->min_value);
                }

                indic_area.y1 = indic_area.y2 - indic_area.y1 + 1;

                if(ext->sym && ext->min_value < 0 && ext->max_value > 0) {
                    /*Calculate the coordinate of the zero point*/
                    edgui_coord_t zero;
                    zero = indic_area.y2 - (-ext->min_value * h) / (ext->max_value - ext->min_value);
                    if(indic_area.y1 < zero)
                        indic_area.y2 = zero;
                    else {
                        indic_area.y2 = indic_area.y1;
                        indic_area.y1 = zero;
                    }
                }
            }

            /*Draw the indicator*/
            edgui_draw_rect(&indic_area, mask, style_indic, opa_scale);
        }
    } else if(mode == EDGUI_DESIGN_DRAW_POST) {
#if EDGUI_USE_GROUP
        /*Draw the border*/
        if(edgui_obj_is_focused(bar)) {
            edgui_opa_t opa_scale          = edgui_obj_get_opa_scale(bar);
            const edgui_style_t * style_bg = edgui_bar_get_style(bar, EDGUI_BAR_STYLE_BG);
            edgui_style_t style_tmp;
            edgui_style_copy(&style_tmp, style_bg);
            style_tmp.body.opa          = EDGUI_OPA_TRANSP;
            style_tmp.body.shadow.width = 0;
            edgui_draw_rect(&bar->coords, mask, &style_tmp, opa_scale);
        }
#endif
    }
    return true;
}

/**
 * Signal function of the bar
 * @param bar pointer to a bar object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_bar_signal(edgui_obj_t * bar, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(bar, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_REFR_EXT_DRAW_PAD) {
        const edgui_style_t * style_indic = edgui_bar_get_style(bar, EDGUI_BAR_STYLE_INDIC);
        if(style_indic->body.shadow.width > bar->ext_draw_pad) bar->ext_draw_pad = style_indic->body.shadow.width;
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_bar";
    }

    return res;
}

#if EDGUI_USE_ANIMATION
static void edgui_bar_anim(void * bar, edgui_anim_value_t value)
{
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(bar);
    ext->anim_state    = value;
    edgui_obj_invalidate(bar);
}

static void edgui_bar_anim_ready(edgui_anim_t * a)
{
    edgui_bar_ext_t * ext = edgui_obj_get_ext_attr(a->var);
    ext->anim_state    = EDGUI_BAR_ANIM_STATE_INV;
    edgui_bar_set_value(a->var, ext->anim_end, false);
}
#endif

#endif
