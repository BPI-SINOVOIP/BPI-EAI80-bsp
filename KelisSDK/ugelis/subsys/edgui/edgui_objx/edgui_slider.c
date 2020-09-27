
/**
 * @file edgui_slider.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_slider.h"
#if EDGUI_USE_SLIDER != 0

#include "../edgui_core/edgui_group.h"
#include "../edgui_core/edgui_indev.h"
#include "../edgui_draw/edgui_draw.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_math.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_SLIDER_SIZE_MIN 4 /*hor. pad and ver. pad cannot make the bar or indicator smaller then this [px]*/
#define EDGUI_SLIDER_NOT_PRESSED INT16_MIN

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_slider_design(edgui_obj_t * slider, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_slider_signal(edgui_obj_t * slider, edgui_signal_t sign, void * param);

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
 * Create a slider objects
 * @param par pointer to an object, it will be the parent of the new slider
 * @param copy pointer to a slider object, if not NULL then the new object will be copied from it
 * @return pointer to the created slider
 */
edgui_obj_t * edgui_slider_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("slider create started");

    /*Create the ancestor slider*/
    edgui_obj_t * new_slider = edgui_bar_create(par, copy);
    edgui_mem_assert(new_slider);
    if(new_slider == NULL) return NULL;

    if(ancestor_design_f == NULL) ancestor_design_f = edgui_obj_get_design_cb(new_slider);
    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_slider);

    /*Allocate the slider type specific extended data*/
    edgui_slider_ext_t * ext = edgui_obj_allocate_ext_attr(new_slider, sizeof(edgui_slider_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */
    ext->drag_value = EDGUI_SLIDER_NOT_PRESSED;
    ext->style_knob = &edgui_style_pretty;
    ext->knob_in    = 0;

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_slider, edgui_slider_signal);
    edgui_obj_set_design_cb(new_slider, edgui_slider_design);

    /*Init the new slider slider*/
    if(copy == NULL) {
        edgui_obj_set_click(new_slider, true);
        edgui_obj_set_protect(new_slider, EDGUI_PROTECT_PRESS_LOST);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_slider_set_style(new_slider, EDGUI_SLIDER_STYLE_BG, th->style.slider.bg);
            edgui_slider_set_style(new_slider, EDGUI_SLIDER_STYLE_INDIC, th->style.slider.indic);
            edgui_slider_set_style(new_slider, EDGUI_SLIDER_STYLE_KNOB, th->style.slider.knob);
        } else {
            edgui_slider_set_style(new_slider, EDGUI_SLIDER_STYLE_KNOB, ext->style_knob);
        }
    }
    /*Copy an existing slider*/
    else {
        edgui_slider_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->style_knob            = copy_ext->style_knob;
        ext->knob_in               = copy_ext->knob_in;
        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_slider);
    }

    EDGUI_LOG_INFO("slider created");

    return new_slider;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the 'knob in' attribute of a slider
 * @param slider pointer to slider object
 * @param in true: the knob is drawn always in the slider;
 *           false: the knob can be out on the edges
 */
void edgui_slider_set_knob_in(edgui_obj_t * slider, bool in)
{
    edgui_slider_ext_t * ext = edgui_obj_get_ext_attr(slider);
    if(ext->knob_in == in) return;

    ext->knob_in = in == false ? 0 : 1;
    edgui_obj_invalidate(slider);
}

/**
 * Set a style of a slider
 * @param slider pointer to a slider object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_slider_set_style(edgui_obj_t * slider, edgui_slider_style_t type, const edgui_style_t * style)
{
    edgui_slider_ext_t * ext = edgui_obj_get_ext_attr(slider);

    switch(type) {
        case EDGUI_SLIDER_STYLE_BG: edgui_bar_set_style(slider, EDGUI_BAR_STYLE_BG, style); break;
        case EDGUI_SLIDER_STYLE_INDIC: edgui_bar_set_style(slider, EDGUI_BAR_STYLE_INDIC, style); break;
        case EDGUI_SLIDER_STYLE_KNOB:
            ext->style_knob = style;
            edgui_obj_refresh_ext_draw_pad(slider);
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a slider
 * @param slider pointer to a slider object
 * @return the value of the slider
 */
int16_t edgui_slider_get_value(const edgui_obj_t * slider)
{
    edgui_slider_ext_t * ext = edgui_obj_get_ext_attr(slider);

    if(ext->drag_value != EDGUI_SLIDER_NOT_PRESSED)
        return ext->drag_value;
    else
        return edgui_bar_get_value(slider);
}

/**
 * Give the slider is being dragged or not
 * @param slider pointer to a slider object
 * @return true: drag in progress false: not dragged
 */
bool edgui_slider_is_dragged(const edgui_obj_t * slider)
{
    edgui_slider_ext_t * ext = edgui_obj_get_ext_attr(slider);
    return ext->drag_value == EDGUI_SLIDER_NOT_PRESSED ? false : true;
}

/**
 * Get the 'knob in' attribute of a slider
 * @param slider pointer to slider object
 * @return true: the knob is drawn always in the slider;
 *         false: the knob can be out on the edges
 */
bool edgui_slider_get_knob_in(const edgui_obj_t * slider)
{
    edgui_slider_ext_t * ext = edgui_obj_get_ext_attr(slider);
    return ext->knob_in == 0 ? false : true;
}

/**
 * Get a style of a slider
 * @param slider pointer to a slider object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_slider_get_style(const edgui_obj_t * slider, edgui_slider_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_slider_ext_t * ext    = edgui_obj_get_ext_attr(slider);

    switch(type) {
        case EDGUI_SLIDER_STYLE_BG: style = edgui_bar_get_style(slider, EDGUI_BAR_STYLE_BG); break;
        case EDGUI_SLIDER_STYLE_INDIC: style = edgui_bar_get_style(slider, EDGUI_BAR_STYLE_INDIC); break;
        case EDGUI_SLIDER_STYLE_KNOB: style = ext->style_knob; break;
        default: style = NULL; break;
    }

    return style;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the sliders
 * @param slider pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_slider_design(edgui_obj_t * slider, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        edgui_slider_ext_t * ext = edgui_obj_get_ext_attr(slider);

        const edgui_style_t * style_bg    = edgui_slider_get_style(slider, EDGUI_SLIDER_STYLE_BG);
        const edgui_style_t * style_knob  = edgui_slider_get_style(slider, EDGUI_SLIDER_STYLE_KNOB);
        const edgui_style_t * style_indic = edgui_slider_get_style(slider, EDGUI_SLIDER_STYLE_INDIC);

        edgui_opa_t opa_scale = edgui_obj_get_opa_scale(slider);

        edgui_coord_t slider_w = edgui_area_get_width(&slider->coords);
        edgui_coord_t slider_h = edgui_area_get_height(&slider->coords);

        /*Draw the bar*/
        edgui_area_t area_bg;
        edgui_area_copy(&area_bg, &slider->coords);

        /*Be sure at least EDGUI_SLIDER_SIZE_MIN  size will remain*/
        edgui_coord_t pad_top_bg    = style_bg->body.padding.top;
        edgui_coord_t pad_bottom_bg = style_bg->body.padding.bottom;
        edgui_coord_t pad_left_bg   = style_bg->body.padding.left;
        edgui_coord_t pad_right_bg  = style_bg->body.padding.right;
        if(pad_top_bg + pad_bottom_bg + EDGUI_SLIDER_SIZE_MIN > edgui_area_get_height(&area_bg)) {
            pad_top_bg    = (edgui_area_get_height(&area_bg) - EDGUI_SLIDER_SIZE_MIN) >> 1;
            pad_bottom_bg = pad_top_bg;
        }
        if(pad_left_bg + pad_right_bg + EDGUI_SLIDER_SIZE_MIN > edgui_area_get_width(&area_bg)) {
            pad_left_bg  = (edgui_area_get_width(&area_bg) - EDGUI_SLIDER_SIZE_MIN) >> 1;
            pad_right_bg = (edgui_area_get_width(&area_bg) - EDGUI_SLIDER_SIZE_MIN) >> 1;
        }

        if(ext->knob_in) { /*Enable extra size if the knob is inside */
            area_bg.x1 += pad_left_bg;
            area_bg.x2 -= pad_right_bg;
            area_bg.y1 += pad_top_bg;
            area_bg.y2 -= pad_bottom_bg;
        } else {                                                   /*Let space only in the perpendicular directions*/
            area_bg.x1 += slider_w < slider_h ? pad_left_bg : 0;   /*Pad only for vertical slider*/
            area_bg.x2 -= slider_w < slider_h ? pad_right_bg : 0;  /*Pad only for vertical slider*/
            area_bg.y1 += slider_w > slider_h ? pad_top_bg : 0;    /*Pad only for horizontal slider*/
            area_bg.y2 -= slider_w > slider_h ? pad_bottom_bg : 0; /*Pad only for horizontal slider*/
        }

#if EDGUI_USE_GROUP == 0
        edgui_draw_rect(&area_bg, mask, style_bg, edgui_obj_get_opa_scale(slider));
#else
        /* Draw the borders later if the slider is focused.
         * At value = 100% the indicator can cover to whole background and the focused style won't
         * be visible*/
        if(edgui_obj_is_focused(slider)) {
            edgui_style_t style_tmp;
            edgui_style_copy(&style_tmp, style_bg);
            style_tmp.body.border.width = 0;
            edgui_draw_rect(&area_bg, mask, &style_tmp, opa_scale);
        } else {
            edgui_draw_rect(&area_bg, mask, style_bg, opa_scale);
        }
#endif

        /*Draw the indicator*/
        edgui_area_t area_indic;
        edgui_area_copy(&area_indic, &area_bg);

        /*Be sure at least ver pad/hor pad width indicator will remain*/
        edgui_coord_t pad_top_indic    = style_indic->body.padding.top;
        edgui_coord_t pad_bottom_indic = style_indic->body.padding.bottom;
        edgui_coord_t pad_left_indic   = style_indic->body.padding.left;
        edgui_coord_t pad_right_indic  = style_indic->body.padding.right;
        if(pad_top_indic + pad_bottom_indic + EDGUI_SLIDER_SIZE_MIN > edgui_area_get_height(&area_bg)) {
            pad_top_indic    = (edgui_area_get_height(&area_bg) - EDGUI_SLIDER_SIZE_MIN) >> 1;
            pad_bottom_indic = pad_top_indic;
        }
        if(pad_left_indic + pad_right_indic + EDGUI_SLIDER_SIZE_MIN > edgui_area_get_width(&area_bg)) {
            pad_left_indic  = (edgui_area_get_width(&area_bg) - EDGUI_SLIDER_SIZE_MIN) >> 1;
            pad_right_indic = pad_left_indic;
        }

        area_indic.x1 += pad_left_indic;
        area_indic.x2 -= pad_right_indic;
        area_indic.y1 += pad_top_indic;
        area_indic.y2 -= pad_bottom_indic;

        edgui_coord_t cur_value = edgui_slider_get_value(slider);
        edgui_coord_t min_value = edgui_slider_get_min_value(slider);
        edgui_coord_t max_value = edgui_slider_get_max_value(slider);

        /*If dragged draw to the drag position*/
        if(ext->drag_value != EDGUI_SLIDER_NOT_PRESSED) cur_value = ext->drag_value;

        if(slider_w >= slider_h) {
            edgui_coord_t indic_w = edgui_area_get_width(&area_indic);
#if EDGUI_USE_ANIMATION
            if(ext->bar.anim_state != EDGUI_BAR_ANIM_STATE_INV) {
                /*Calculate the coordinates of anim. start and end*/
                edgui_coord_t anim_start_x =
                    (int32_t)((int32_t)indic_w * (ext->bar.anim_start - min_value)) / (max_value - min_value);
                edgui_coord_t anim_end_x =
                    (int32_t)((int32_t)indic_w * (ext->bar.anim_end - min_value)) / (max_value - min_value);

                /*Calculate the real position based on `anim_state` (between `anim_start` and
                 * `anim_end`)*/
                area_indic.x2 = anim_start_x + (((anim_end_x - anim_start_x) * ext->bar.anim_state) >> 8);
            } else
#endif
            {
                area_indic.x2 = (int32_t)((int32_t)indic_w * (cur_value - min_value)) / (max_value - min_value);
            }
            area_indic.x2 = area_indic.x1 + area_indic.x2 - 1;

            /*Draw the indicator but don't draw an ugly 1px wide rectangle on the left on min.
             * value*/
            if(area_indic.x1 != area_indic.x2) edgui_draw_rect(&area_indic, mask, style_indic, opa_scale);

        } else {
            edgui_coord_t indic_h = edgui_area_get_height(&area_indic);
#if EDGUI_USE_ANIMATION
            if(ext->bar.anim_state != EDGUI_BAR_ANIM_STATE_INV) {
                /*Calculate the coordinates of anim. start and end*/
                edgui_coord_t anim_start_y =
                    (int32_t)((int32_t)indic_h * (ext->bar.anim_start - min_value)) / (max_value - min_value);
                edgui_coord_t anim_end_y =
                    (int32_t)((int32_t)indic_h * (ext->bar.anim_end - min_value)) / (max_value - min_value);

                /*Calculate the real position based on `anim_state` (between `anim_start` and
                 * `anim_end`)*/
                area_indic.y1 = anim_start_y + (((anim_end_y - anim_start_y) * ext->bar.anim_state) >> 8);
            } else
#endif
            {
                area_indic.y1 = (int32_t)((int32_t)indic_h * (cur_value - min_value)) / (max_value - min_value);
            }
            area_indic.y1 = area_indic.y2 - area_indic.y1 + 1;

            /*Draw the indicator but don't draw an ugly 1px height rectangle on the bottom on min.
             * value*/
            if(area_indic.x1 != area_indic.x2) edgui_draw_rect(&area_indic, mask, style_indic, opa_scale);
        }

        /*Before the knob add the border if required*/
#if EDGUI_USE_GROUP
        /* Draw the borders later if the bar is focused.
         * At value = 100% the indicator can cover to whole background and the focused style won't
         * be visible*/
        if(edgui_obj_is_focused(slider)) {
            edgui_style_t style_tmp;
            edgui_style_copy(&style_tmp, style_bg);
            style_tmp.body.opa          = EDGUI_OPA_TRANSP;
            style_tmp.body.shadow.width = 0;
            edgui_draw_rect(&area_bg, mask, &style_tmp, opa_scale);
        }
#endif

        /*Draw the knob*/
        edgui_area_t knob_area;
        edgui_area_copy(&knob_area, &slider->coords);

        if(slider_w >= slider_h) {
            if(ext->knob_in == 0) {
                knob_area.x1 = area_indic.x2 - slider_h / 2;
                knob_area.x2 = knob_area.x1 + slider_h - 1;
            } else {
#if EDGUI_USE_ANIMATION
                if(ext->bar.anim_state != EDGUI_BAR_ANIM_STATE_INV) {
                    edgui_coord_t w = slider_w - slider_h - 1;
                    edgui_coord_t anim_start_x =
                        (int32_t)((int32_t)w * (ext->bar.anim_start - min_value)) / (max_value - min_value);
                    edgui_coord_t anim_end_x =
                        (int32_t)((int32_t)w * (ext->bar.anim_end - min_value)) / (max_value - min_value);

                    /*Calculate the real position based on `anim_state` (between `anim_start` and
                     * `anim_end`)*/
                    knob_area.x1 = anim_start_x + (((anim_end_x - anim_start_x) * ext->bar.anim_state) >> 8);
                } else
#endif
                {
                    knob_area.x1 = (int32_t)((int32_t)(slider_w - slider_h - 1) * (cur_value - min_value)) /
                                   (max_value - min_value);
                }

                knob_area.x1 += slider->coords.x1;
                knob_area.x2 = knob_area.x1 + slider_h - 1;
            }

            knob_area.y1 = slider->coords.y1;
            knob_area.y2 = slider->coords.y2;
        } else {
            if(ext->knob_in == 0) {
                knob_area.y1 = area_indic.y1 - slider_w / 2;
                knob_area.y2 = knob_area.y1 + slider_w - 1;
            } else {
#if EDGUI_USE_ANIMATION
                if(ext->bar.anim_state != EDGUI_BAR_ANIM_STATE_INV) {
                    edgui_coord_t h = slider_h - slider_w - 1;
                    edgui_coord_t anim_start_x =
                        (int32_t)((int32_t)h * (ext->bar.anim_start - min_value)) / (max_value - min_value);
                    edgui_coord_t anim_end_x =
                        (int32_t)((int32_t)h * (ext->bar.anim_end - min_value)) / (max_value - min_value);

                    /*Calculate the real position based on `anim_state` (between `anim_start` and
                     * `anim_end`)*/
                    knob_area.y2 = anim_start_x + (((anim_end_x - anim_start_x) * ext->bar.anim_state) >> 8);
                } else
#endif
                {
                    knob_area.y2 = (int32_t)((int32_t)(slider_h - slider_w - 1) * (cur_value - min_value)) /
                                   (max_value - min_value);
                }

                knob_area.y2 = slider->coords.y2 - knob_area.y2;
                knob_area.y1 = knob_area.y2 - slider_w - 1;
            }
            knob_area.x1 = slider->coords.x1;
            knob_area.x2 = slider->coords.x2;
        }
        edgui_draw_rect(&knob_area, mask, style_knob, opa_scale);
    }
    /*Post draw when the children are drawn*/
    else if(mode == EDGUI_DESIGN_DRAW_POST) {
    }

    return true;
}

/**
 * Signal function of the slider
 * @param slider pointer to a slider object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_slider_signal(edgui_obj_t * slider, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(slider, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_slider_ext_t * ext = edgui_obj_get_ext_attr(slider);
    edgui_point_t p;
    edgui_coord_t w = edgui_obj_get_width(slider);
    edgui_coord_t h = edgui_obj_get_height(slider);

    if(sign == EDGUI_SIGNAL_PRESSED) {
        ext->drag_value = edgui_slider_get_value(slider);
    } else if(sign == EDGUI_SIGNAL_PRESSING) {
        edgui_indev_get_point(param, &p);
        int16_t tmp = 0;
        if(w > h) {
            edgui_coord_t knob_w = h;
            p.x -=
                slider->coords.x1 + h / 2; /*Modify the point to shift with half knob (important on the start and end)*/
            tmp = (int32_t)((int32_t)p.x * (ext->bar.max_value - ext->bar.min_value + 1)) / (w - knob_w);
            tmp += ext->bar.min_value;
        } else {
            edgui_coord_t knob_h = w;
            p.y -=
                slider->coords.y1 + w / 2; /*Modify the point to shift with half knob (important on the start and end)*/
            tmp = (int32_t)((int32_t)p.y * (ext->bar.max_value - ext->bar.min_value + 1)) / (h - knob_h);
            tmp = ext->bar.max_value - tmp; /*Invert the value: smaller value means higher y*/
        }

        if(tmp < ext->bar.min_value)
            tmp = ext->bar.min_value;
        else if(tmp > ext->bar.max_value)
            tmp = ext->bar.max_value;

        if(tmp != ext->drag_value) {
            ext->drag_value = tmp;
            edgui_obj_invalidate(slider);
            res = edgui_event_send(slider, EDGUI_EVENT_VALUE_CHANGED, NULL);
            if(res != EDGUI_RES_OK) return res;
        }
    } else if(sign == EDGUI_SIGNAL_RELEASED || sign == EDGUI_SIGNAL_PRESS_LOST) {
        if(ext->drag_value != EDGUI_SLIDER_NOT_PRESSED) edgui_slider_set_value(slider, ext->drag_value, false);
        ext->drag_value = EDGUI_SLIDER_NOT_PRESSED;

#if EDGUI_USE_GROUP
        /*Leave edit mode if released. (No need to wait for LONG_PRESS) */
        edgui_group_t * g             = edgui_obj_get_group(slider);
        bool editing               = edgui_group_get_editing(g);
        edgui_indev_type_t indev_type = edgui_indev_get_type(edgui_indev_get_act());
        if(indev_type == EDGUI_INDEV_TYPE_ENCODER) {
            if(editing) edgui_group_set_editing(g, false);
        }
#endif

    } else if(sign == EDGUI_SIGNAL_CORD_CHG) {
        /* The knob size depends on slider size.
         * During the drawing method the ext. size is used by the knob so refresh the ext. size.*/
        if(edgui_obj_get_width(slider) != edgui_area_get_width(param) ||
           edgui_obj_get_height(slider) != edgui_area_get_height(param)) {
            slider->signal_cb(slider, EDGUI_SIGNAL_REFR_EXT_DRAW_PAD, NULL);
        }
    } else if(sign == EDGUI_SIGNAL_REFR_EXT_DRAW_PAD) {
        const edgui_style_t * style      = edgui_slider_get_style(slider, EDGUI_SLIDER_STYLE_BG);
        const edgui_style_t * knob_style = edgui_slider_get_style(slider, EDGUI_SLIDER_STYLE_KNOB);

        edgui_coord_t shadow_w = knob_style->body.shadow.width;
        if(ext->knob_in == 0) {
            /* The smaller size is the knob diameter*/
            edgui_coord_t x = EDGUI_MATH_MIN(w / 2 + 1 + shadow_w, h / 2 + 1 + shadow_w);
            if(slider->ext_draw_pad < x) slider->ext_draw_pad = x;
        } else {
            edgui_coord_t pad = 0;
            pad            = EDGUI_MATH_MIN(pad, style->body.padding.top);
            pad            = EDGUI_MATH_MIN(pad, style->body.padding.bottom);
            pad            = EDGUI_MATH_MIN(pad, style->body.padding.left);
            pad            = EDGUI_MATH_MIN(pad, style->body.padding.right);
            if(pad < 0) pad = -pad;
            if(slider->ext_draw_pad < pad) slider->ext_draw_pad = pad;

            if(slider->ext_draw_pad < shadow_w) slider->ext_draw_pad = shadow_w;
        }
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        char c = *((char *)param);

        ext->drag_value = EDGUI_SLIDER_NOT_PRESSED;

        if(c == EDGUI_KEY_RIGHT || c == EDGUI_KEY_UP) {
            edgui_slider_set_value(slider, edgui_slider_get_value(slider) + 1, true);
            res = edgui_event_send(slider, EDGUI_EVENT_VALUE_CHANGED, NULL);
            if(res != EDGUI_RES_OK) return res;
        } else if(c == EDGUI_KEY_LEFT || c == EDGUI_KEY_DOWN) {
            edgui_slider_set_value(slider, edgui_slider_get_value(slider) - 1, true);
            res = edgui_event_send(slider, EDGUI_EVENT_VALUE_CHANGED, NULL);
            if(res != EDGUI_RES_OK) return res;
        }
    } else if(sign == EDGUI_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = true;
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_slider";
    }

    return res;
}
#endif
