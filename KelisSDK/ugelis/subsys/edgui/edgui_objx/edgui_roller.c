/**
 * @file edgui_roller.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_roller.h"
#if EDGUI_USE_ROLLER != 0

#include "../edgui_draw/edgui_draw.h"
#include "../edgui_core/edgui_group.h"
#include "../edgui_themes/edgui_theme.h"

/*********************
 *      DEFINES
 *********************/
#if EDGUI_USE_ANIMATION == 0
#undef EDGUI_ROLLER_DEF_ANIM_TIME
#define EDGUI_ROLLER_DEF_ANIM_TIME 0 /*No animation*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_roller_design(edgui_obj_t * roller, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_roller_scrl_signal(edgui_obj_t * roller_scrl, edgui_signal_t sign, void * param);
static edgui_res_t edgui_roller_signal(edgui_obj_t * roller, edgui_signal_t sign, void * param);
static void refr_position(edgui_obj_t * roller, edgui_anim_enable_t animen);
static void refr_height(edgui_obj_t * roller);
static void inf_normalize(void * roller_scrl);
#if EDGUI_USE_ANIMATION
static void scroll_anim_ready_cb(edgui_anim_t * a);
#endif
static void draw_bg(edgui_obj_t * roller, const edgui_area_t * mask);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;
static edgui_signal_cb_t ancestor_scrl_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a roller object
 * @param par pointer to an object, it will be the parent of the new roller
 * @param copy pointer to a roller object, if not NULL then the new object will be copied from it
 * @return pointer to the created roller
 */
edgui_obj_t * edgui_roller_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("roller create started");

    /*Create the ancestor of roller*/
    edgui_obj_t * new_roller = edgui_ddlist_create(par, copy);
    edgui_mem_assert(new_roller);
    if(new_roller == NULL) return NULL;

    if(ancestor_scrl_signal == NULL) ancestor_scrl_signal = edgui_obj_get_signal_cb(edgui_page_get_scrl(new_roller));
    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_roller);

    /*Allocate the roller type specific extended data*/
    edgui_roller_ext_t * ext = edgui_obj_allocate_ext_attr(new_roller, sizeof(edgui_roller_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;
    ext->ddlist.draw_arrow = 0; /*Do not draw arrow by default*/

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_roller, edgui_roller_signal);
    edgui_obj_set_design_cb(new_roller, edgui_roller_design);

    /*Init the new roller roller*/
    if(copy == NULL) {
        edgui_obj_t * scrl = edgui_page_get_scrl(new_roller);
        edgui_obj_set_drag(scrl, true);                                  /*In ddlist it might be disabled*/
        edgui_page_set_scrl_fit2(new_roller, EDGUI_FIT_TIGHT, EDGUI_FIT_NONE); /*Height is specified directly*/
        edgui_ddlist_open(new_roller, false);
        edgui_ddlist_set_anim_time(new_roller, EDGUI_ROLLER_DEF_ANIM_TIME);
        edgui_ddlist_set_stay_open(new_roller, true);
        edgui_roller_set_visible_row_count(new_roller, 3);
        edgui_label_set_align(ext->ddlist.label, EDGUI_LABEL_ALIGN_CENTER);

        edgui_obj_set_signal_cb(scrl, edgui_roller_scrl_signal);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_roller_set_style(new_roller, EDGUI_ROLLER_STYLE_BG, th->style.roller.bg);
            edgui_roller_set_style(new_roller, EDGUI_ROLLER_STYLE_SEL, th->style.roller.sel);
        } else {
            /*Refresh the roller's style*/
            edgui_obj_refresh_style(new_roller); /*To set scrollable size automatically*/
        }
    }
    /*Copy an existing roller*/
    else {
        edgui_roller_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->mode                  = copy_ext->mode;

        edgui_obj_t * scrl = edgui_page_get_scrl(new_roller);
        edgui_ddlist_open(new_roller, false);
        edgui_obj_set_signal_cb(scrl, edgui_roller_scrl_signal);

        /*Refresh the roller's style*/
        edgui_obj_refresh_style(new_roller); /*Refresh the style with new signal function*/
    }

    EDGUI_LOG_INFO("roller created");

    return new_roller;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the options on a roller
 * @param roller pointer to roller object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 * @param mode `EDGUI_ROLLER_MODE_NORMAL` or `EDGUI_ROLLER_MODE_INFINITE`
 */
void edgui_roller_set_options(edgui_obj_t * roller, const char * options, edgui_roller_mode_t mode)
{
    edgui_roller_ext_t * ext = edgui_obj_get_ext_attr(roller);

    if(mode == EDGUI_ROLLER_MODE_NORMAL) {
        ext->mode = EDGUI_ROLLER_MODE_NORMAL;
        edgui_ddlist_set_options(roller, options);

        /* Make sure the roller's height and the scrollable's height is refreshed.
         * They are refreshed in `EDGUI_SIGNAL_COORD_CHG` but if the new options has the same width
         * that signal won't be called. (It called because EDGUI_FIT_TIGHT hor fit)*/
        refr_height(roller);
    } else {
        ext->mode = EDGUI_ROLLER_MODE_INIFINITE;

        uint32_t opt_len = strlen(options) + 1; /*+1 to add '\n' after option lists*/
        char * opt_extra = edgui_mem_alloc(opt_len * EDGUI_ROLLER_INF_PAGES);
        uint8_t i;
        for(i = 0; i < EDGUI_ROLLER_INF_PAGES; i++) {
            strcpy(&opt_extra[opt_len * i], options);
            opt_extra[opt_len * (i + 1) - 1] = '\n';
        }
        opt_extra[opt_len * EDGUI_ROLLER_INF_PAGES - 1] = '\0';
        edgui_ddlist_set_options(roller, opt_extra);
        edgui_mem_free(opt_extra);

        /* Make sure the roller's height and the scrollable's height is refreshed.
         * They are refreshed in `EDGUI_SIGNAL_COORD_CHG` but if the new options has the same width
         * that signal won't be called. (It called because EDGUI_FIT_TIGHT hor fit)*/
        refr_height(roller);

        uint16_t real_id_cnt = ext->ddlist.option_cnt / EDGUI_ROLLER_INF_PAGES;
        edgui_roller_set_selected(roller, ((EDGUI_ROLLER_INF_PAGES / 2) + 1) * real_id_cnt, false); /*Select the middle page*/
    }
}

/**
 * Set the align of the roller's options (left or center)
 * @param roller - pointer to a roller object
 * @param align - one of edgui_label_align_t values (left, right, center)
 */
void edgui_roller_set_align(edgui_obj_t * roller, edgui_label_align_t align)
{
    edgui_roller_ext_t * ext = edgui_obj_get_ext_attr(roller);

    edgui_obj_t * label = ext->ddlist.label;

    if(label == NULL) return; /*Probably the roller is being deleted if the label is NULL.*/
    edgui_label_set_align(label, align);

    switch(edgui_label_get_align(label)) {
        case EDGUI_LABEL_ALIGN_LEFT: edgui_obj_align(label, NULL, EDGUI_ALIGN_IN_LEFT_MID, 0, 0); break;
        case EDGUI_LABEL_ALIGN_CENTER: edgui_obj_align(label, NULL, EDGUI_ALIGN_CENTER, 0, 0); break;
        case EDGUI_LABEL_ALIGN_RIGHT: edgui_obj_align(label, NULL, EDGUI_ALIGN_IN_RIGHT_MID, 0, 0); break;
    }
}

/**
 * Set the selected option
 * @param roller pointer to a roller object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 * @param anim_en EDGUI_ANIM_ON: set with animation; EDGUI_ANOM_OFF set immediately
 */
void edgui_roller_set_selected(edgui_obj_t * roller, uint16_t sel_opt, edgui_anim_enable_t anim)
{
#if EDGUI_USE_ANIMATION == 0
    anim = EDGUI_ANIM_OFF;
#endif

    if(edgui_roller_get_selected(roller) == sel_opt) return;

    edgui_ddlist_set_selected(roller, sel_opt);
    refr_position(roller, anim);
}

/**
 * Set the height to show the given number of rows (options)
 * @param roller pointer to a roller object
 * @param row_cnt number of desired visible rows
 */
void edgui_roller_set_visible_row_count(edgui_obj_t * roller, uint8_t row_cnt)
{
    edgui_roller_ext_t * ext          = edgui_obj_get_ext_attr(roller);
    const edgui_style_t * style_label = edgui_obj_get_style(ext->ddlist.label);
    uint8_t n_line_space           = (row_cnt > 1) ? row_cnt - 1 : 1;
    edgui_ddlist_set_fix_height(roller, edgui_font_get_line_height(style_label->text.font) * row_cnt +
                                         style_label->text.line_space * n_line_space);
}

/**
 * Set a style of a roller
 * @param roller pointer to a roller object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_roller_set_style(edgui_obj_t * roller, edgui_roller_style_t type, const edgui_style_t * style)
{
    switch(type) {
        case EDGUI_ROLLER_STYLE_BG: edgui_obj_set_style(roller, style); break;
        case EDGUI_ROLLER_STYLE_SEL: edgui_ddlist_set_style(roller, EDGUI_DDLIST_STYLE_SEL, style); break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the id of the selected option
 * @param roller pointer to a roller object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t edgui_roller_get_selected(const edgui_obj_t * roller)
{
    edgui_roller_ext_t * ext = edgui_obj_get_ext_attr(roller);
    if(ext->mode == EDGUI_ROLLER_MODE_INIFINITE) {
        uint16_t real_id_cnt = ext->ddlist.option_cnt / EDGUI_ROLLER_INF_PAGES;
        return edgui_ddlist_get_selected(roller) % real_id_cnt;
    } else {
        return edgui_ddlist_get_selected(roller);
    }
}

/**
 * Get the align attribute. Default alignment after _create is EDGUI_LABEL_ALIGN_CENTER
 * @param roller pointer to a roller object
 * @return EDGUI_LABEL_ALIGN_LEFT, EDGUI_LABEL_ALIGN_RIGHT or EDGUI_LABEL_ALIGN_CENTER
 */
edgui_label_align_t edgui_roller_get_align(const edgui_obj_t * roller)
{
    edgui_roller_ext_t * ext = edgui_obj_get_ext_attr(roller);
    edgui_mem_assert(ext);
    edgui_mem_assert(ext->ddlist.label);
    return edgui_label_get_align(ext->ddlist.label);
}

/**
 * Get the auto width set attribute
 * @param roller pointer to a roller object
 * @return true: auto size enabled; false: manual width settings enabled
 */
bool edgui_roller_get_hor_fit(const edgui_obj_t * roller)
{
    return edgui_page_get_scrl_fit_left(roller);
}

/**
 * Get a style of a roller
 * @param roller pointer to a roller object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
const edgui_style_t * edgui_roller_get_style(const edgui_obj_t * roller, edgui_roller_style_t type)
{
    switch(type) {
        case EDGUI_ROLLER_STYLE_BG: return edgui_obj_get_style(roller);
        case EDGUI_ROLLER_STYLE_SEL: return edgui_ddlist_get_style(roller, EDGUI_DDLIST_STYLE_SEL);
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the rollers
 * @param roller pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_roller_design(edgui_obj_t * roller, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        draw_bg(roller, mask);

        const edgui_style_t * style = edgui_roller_get_style(roller, EDGUI_ROLLER_STYLE_BG);
        edgui_opa_t opa_scale       = edgui_obj_get_opa_scale(roller);
        const edgui_font_t * font   = style->text.font;
        edgui_roller_ext_t * ext    = edgui_obj_get_ext_attr(roller);
        edgui_coord_t font_h        = edgui_font_get_line_height(font);
        edgui_area_t rect_area;
        rect_area.y1 = roller->coords.y1 + edgui_obj_get_height(roller) / 2 - font_h / 2 - style->text.line_space / 2;
        if((font_h & 0x1) && (style->text.line_space & 0x1)) rect_area.y1--; /*Compensate the two rounding error*/
        rect_area.y2 = rect_area.y1 + font_h + style->text.line_space - 1;
        edgui_area_t roller_coords;
        edgui_obj_get_coords(roller, &roller_coords);
        edgui_obj_get_inner_coords(roller, &roller_coords);

        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;

        edgui_draw_rect(&rect_area, mask, ext->ddlist.sel_style, opa_scale);
    }
    /*Post draw when the children are drawn*/
    else if(mode == EDGUI_DESIGN_DRAW_POST) {
        const edgui_style_t * style = edgui_roller_get_style(roller, EDGUI_ROLLER_STYLE_BG);
        edgui_roller_ext_t * ext    = edgui_obj_get_ext_attr(roller);
        const edgui_font_t * font   = style->text.font;
        edgui_coord_t font_h        = edgui_font_get_line_height(font);
        edgui_opa_t opa_scale       = edgui_obj_get_opa_scale(roller);

        /*Redraw the text on the selected area with a different color*/
        edgui_area_t rect_area;
        rect_area.y1 = roller->coords.y1 + edgui_obj_get_height(roller) / 2 - font_h / 2 - style->text.line_space / 2;
        if((font_h & 0x1) && (style->text.line_space & 0x1)) rect_area.y1--; /*Compensate the two rounding error*/
        rect_area.y2 = rect_area.y1 + font_h + style->text.line_space - 1;
        rect_area.x1 = roller->coords.x1;
        rect_area.x2 = roller->coords.x2;
        edgui_area_t mask_sel;
        bool area_ok;
        area_ok = edgui_area_intersect(&mask_sel, mask, &rect_area);
        if(area_ok) {
            const edgui_style_t * sel_style = edgui_roller_get_style(roller, EDGUI_ROLLER_STYLE_SEL);
            edgui_style_t new_style;
            edgui_txt_flag_t txt_align = EDGUI_TXT_FLAG_NONE;

            {
                edgui_label_align_t label_align = edgui_label_get_align(ext->ddlist.label);

                if(EDGUI_LABEL_ALIGN_CENTER == label_align) {
                    txt_align |= EDGUI_TXT_FLAG_CENTER;
                } else if(EDGUI_LABEL_ALIGN_RIGHT == label_align) {
                    txt_align |= EDGUI_TXT_FLAG_RIGHT;
                }
            }

            edgui_style_copy(&new_style, style);
            new_style.text.color = sel_style->text.color;
            new_style.text.opa   = sel_style->text.opa;
            edgui_draw_label(&ext->ddlist.label->coords, &mask_sel, &new_style, opa_scale,
                          edgui_label_get_text(ext->ddlist.label), txt_align, NULL, -1, -1, NULL);
        }
    }

    return true;
}

/**
 * Signal function of the roller
 * @param roller pointer to a roller object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_roller_signal(edgui_obj_t * roller, edgui_signal_t sign, void * param)
{
    edgui_res_t res = EDGUI_RES_OK;

    /*Don't let the drop down list to handle the control signals. It works differently*/
    if(sign != EDGUI_SIGNAL_CONTROL && sign != EDGUI_SIGNAL_FOCUS && sign != EDGUI_SIGNAL_DEFOCUS) {
        /* Include the ancient signal function */
        res = ancestor_signal(roller, sign, param);
        if(res != EDGUI_RES_OK) return res;
    }

    edgui_roller_ext_t * ext = edgui_obj_get_ext_attr(roller);

    if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        refr_height(roller);

        refr_position(roller, false);
    } else if(sign == EDGUI_SIGNAL_CORD_CHG) {

        if(edgui_obj_get_width(roller) != edgui_area_get_width(param) ||
           edgui_obj_get_height(roller) != edgui_area_get_height(param)) {

            refr_height(roller);
#if EDGUI_USE_ANIMATION
            edgui_anim_del(edgui_page_get_scrl(roller), (edgui_anim_exec_xcb_t)edgui_obj_set_y);
#endif
            edgui_ddlist_set_selected(roller, ext->ddlist.sel_opt_id);
            refr_position(roller, false);
        }
    } else if(sign == EDGUI_SIGNAL_FOCUS) {
#if EDGUI_USE_GROUP
        edgui_group_t * g             = edgui_obj_get_group(roller);
        bool editing               = edgui_group_get_editing(g);
        edgui_indev_type_t indev_type = edgui_indev_get_type(edgui_indev_get_act());

        /*Encoders need special handling*/
        if(indev_type == EDGUI_INDEV_TYPE_ENCODER) {
            /*In navigate mode revert the original value*/
            if(!editing) {
                if(ext->ddlist.sel_opt_id != ext->ddlist.sel_opt_id_ori) {
                    ext->ddlist.sel_opt_id = ext->ddlist.sel_opt_id_ori;
                    refr_position(roller, true);
                }
            }
            /*Save the current state when entered to edit mode*/
            else {
                ext->ddlist.sel_opt_id_ori = ext->ddlist.sel_opt_id;
            }
        } else {
            ext->ddlist.sel_opt_id_ori = ext->ddlist.sel_opt_id; /*Save the current value. Used to revert this state if
                                                                    ENER wont't be pressed*/
        }
#endif
    } else if(sign == EDGUI_SIGNAL_DEFOCUS) {
#if EDGUI_USE_GROUP
        /*Revert the original state*/
        if(ext->ddlist.sel_opt_id != ext->ddlist.sel_opt_id_ori) {
            ext->ddlist.sel_opt_id = ext->ddlist.sel_opt_id_ori;
            refr_position(roller, true);
        }
#endif
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == EDGUI_KEY_RIGHT || c == EDGUI_KEY_DOWN) {
            if(ext->ddlist.sel_opt_id + 1 < ext->ddlist.option_cnt) {
                uint16_t ori_id = ext->ddlist.sel_opt_id_ori; /*edgui_roller_set_selceted will overwrite this*/
                edgui_roller_set_selected(roller, ext->ddlist.sel_opt_id + 1, true);
                ext->ddlist.sel_opt_id_ori = ori_id;
            }
        } else if(c == EDGUI_KEY_LEFT || c == EDGUI_KEY_UP) {
            if(ext->ddlist.sel_opt_id > 0) {
                uint16_t ori_id = ext->ddlist.sel_opt_id_ori; /*edgui_roller_set_selceted will overwrite this*/
                edgui_roller_set_selected(roller, ext->ddlist.sel_opt_id - 1, true);
                ext->ddlist.sel_opt_id_ori = ori_id;
            }
        }
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_roller";
    }

    return res;
}

/**
 * Signal function of the scrollable part of the roller.
 * @param roller_scrl ointer to the scrollable part of roller (page)
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_roller_scrl_signal(edgui_obj_t * roller_scrl, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_scrl_signal(roller_scrl, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_indev_t * indev    = edgui_indev_get_act();
    int32_t id            = -1;
    edgui_obj_t * roller     = edgui_obj_get_parent(roller_scrl);
    edgui_roller_ext_t * ext = edgui_obj_get_ext_attr(roller);

    if(ext->ddlist.label == NULL)
        return EDGUI_RES_INV; /*On delete the ddlist signal deletes the label so nothing left to do
                              here*/

    const edgui_style_t * style_label = edgui_obj_get_style(ext->ddlist.label);
    const edgui_font_t * font         = style_label->text.font;
    edgui_coord_t font_h              = edgui_font_get_line_height(font);

    if(sign == EDGUI_SIGNAL_DRAG_END) {
        /*If dragged then align the list to have an element in the middle*/
        edgui_coord_t label_y1   = ext->ddlist.label->coords.y1 - roller->coords.y1;
        edgui_coord_t label_unit = font_h + style_label->text.line_space;
        edgui_coord_t mid        = (roller->coords.y2 - roller->coords.y1) / 2;

        id = (mid - label_y1 + style_label->text.line_space / 2) / label_unit;

        if(id < 0) id = 0;
        if(id >= ext->ddlist.option_cnt) id = ext->ddlist.option_cnt - 1;

        ext->ddlist.sel_opt_id     = id;
        ext->ddlist.sel_opt_id_ori = id;
        res                        = edgui_event_send(roller, EDGUI_EVENT_VALUE_CHANGED, &id);
        if(res != EDGUI_RES_OK) return res;
    }
    /*If picked an option by clicking then set it*/
    else if(sign == EDGUI_SIGNAL_RELEASED) {
        if(!edgui_indev_is_dragging(indev)) {
            id = ext->ddlist.sel_opt_id;
#if EDGUI_USE_GROUP
            /*In edit mode go to navigate mode if an option is selected*/
            edgui_group_t * g = edgui_obj_get_group(roller);
            bool editing   = edgui_group_get_editing(g);
            if(editing) edgui_group_set_editing(g, false);
#endif
        }
    } else if(sign == EDGUI_SIGNAL_PRESSED) {
#if EDGUI_USE_ANIMATION
        edgui_anim_del(roller_scrl, (edgui_anim_exec_xcb_t)edgui_obj_set_y);
#endif
    }

    /*Position the scrollable according to the new selected option*/
    if(id != -1) {
        refr_position(roller, true);
    }

    return res;
}

/**
 * Draw a rectangle which has gradient on its top and bottom
 * @param roller pointer to a roller object
 * @param mask pointer to the current mask (from the design function)
 */
static void draw_bg(edgui_obj_t * roller, const edgui_area_t * mask)
{
    const edgui_style_t * style = edgui_roller_get_style(roller, EDGUI_ROLLER_STYLE_BG);
    edgui_area_t half_mask;
    edgui_area_t half_roller;
    edgui_coord_t h = edgui_obj_get_height(roller);
    bool union_ok;
    edgui_area_copy(&half_roller, &roller->coords);

    half_roller.x1 -= roller->ext_draw_pad; /*Add ext size too (e.g. because of shadow draw) */
    half_roller.x2 += roller->ext_draw_pad;
    half_roller.y1 -= roller->ext_draw_pad;
    half_roller.y2 = roller->coords.y1 + h / 2;

    union_ok = edgui_area_intersect(&half_mask, &half_roller, mask);

    half_roller.x1 += roller->ext_draw_pad; /*Revert ext. size adding*/
    half_roller.x2 -= roller->ext_draw_pad;
    half_roller.y1 += roller->ext_draw_pad;
    half_roller.y2 += style->body.radius;

    if(union_ok) {
        edgui_draw_rect(&half_roller, &half_mask, style, edgui_obj_get_opa_scale(roller));
    }

    half_roller.x1 -= roller->ext_draw_pad; /*Add ext size too (e.g. because of shadow draw) */
    half_roller.x2 += roller->ext_draw_pad;
    half_roller.y2 = roller->coords.y2 + roller->ext_draw_pad;
    half_roller.y1 = roller->coords.y1 + h / 2;
    if((h & 0x1) == 0) half_roller.y1++; /*With even height the pixels in the middle would be drawn twice*/

    union_ok = edgui_area_intersect(&half_mask, &half_roller, mask);

    half_roller.x1 += roller->ext_draw_pad; /*Revert ext. size adding*/
    half_roller.x2 -= roller->ext_draw_pad;
    half_roller.y2 -= roller->ext_draw_pad;
    half_roller.y1 -= style->body.radius;

    if(union_ok) {
        edgui_style_t style_tmp;
        memcpy(&style_tmp, style, sizeof(edgui_style_t));
        style_tmp.body.main_color = style->body.grad_color;
        style_tmp.body.grad_color = style->body.main_color;
        edgui_draw_rect(&half_roller, &half_mask, &style_tmp, edgui_obj_get_opa_scale(roller));
    }
}

/**
 * Refresh the position of the roller. It uses the id stored in: ext->ddlist.selected_option_id
 * @param roller pointer to a roller object
 * @param anim_en EDGUI_ANIM_ON: refresh with animation; EDGUI_ANOM_OFF: without animation
 */
static void refr_position(edgui_obj_t * roller, edgui_anim_enable_t anim_en)
{
#if EDGUI_USE_ANIMATION == 0
    anim_en = EDGUI_ANIM_OFF;
#endif

    edgui_obj_t * roller_scrl         = edgui_page_get_scrl(roller);
    edgui_roller_ext_t * ext          = edgui_obj_get_ext_attr(roller);
    const edgui_style_t * style_label = edgui_obj_get_style(ext->ddlist.label);
    const edgui_font_t * font         = style_label->text.font;
    edgui_coord_t font_h              = edgui_font_get_line_height(font);
    edgui_coord_t h                   = edgui_obj_get_height(roller);
    uint16_t anim_time             = edgui_roller_get_anim_time(roller);

    /* Normally the animtaion's `end_cb` sets correct position of the roller is infinite.
     * But without animations do it manually*/
    if(anim_en == EDGUI_ANIM_OFF || anim_time == 0) {
        inf_normalize(roller_scrl);
    }

    int32_t id = ext->ddlist.sel_opt_id;
    edgui_coord_t line_y1 =
        id * (font_h + style_label->text.line_space) + ext->ddlist.label->coords.y1 - roller_scrl->coords.y1;
    edgui_coord_t new_y = -line_y1 + (h - font_h) / 2;

    if(anim_en == EDGUI_ANIM_OFF || anim_time == 0) {
        edgui_obj_set_y(roller_scrl, new_y);
    } else {
#if EDGUI_USE_ANIMATION
        edgui_anim_t a;
        a.var            = roller_scrl;
        a.start          = edgui_obj_get_y(roller_scrl);
        a.end            = new_y;
        a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
        a.path_cb        = edgui_anim_path_linear;
        a.ready_cb       = scroll_anim_ready_cb;
        a.act_time       = 0;
        a.time           = anim_time;
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        edgui_anim_create(&a);
#endif
    }
}

/**
 * Refresh the height of the roller and the scrolable
 * @param roller pointer to roller
 */
static void refr_height(edgui_obj_t * roller)
{
    edgui_roller_ext_t * ext = edgui_obj_get_ext_attr(roller);
    edgui_align_t obj_align  = EDGUI_ALIGN_IN_LEFT_MID;
    if(ext->ddlist.label) {
        edgui_label_align_t label_align = edgui_label_get_align(ext->ddlist.label);
        if(EDGUI_LABEL_ALIGN_CENTER == label_align)
            obj_align = EDGUI_ALIGN_CENTER;
        else if(EDGUI_LABEL_ALIGN_RIGHT == label_align)
            obj_align = EDGUI_ALIGN_IN_RIGHT_MID;
    }

    edgui_obj_set_height(edgui_page_get_scrl(roller), edgui_obj_get_height(ext->ddlist.label) + edgui_obj_get_height(roller));
    edgui_obj_align(ext->ddlist.label, NULL, obj_align, 0, 0);
#if EDGUI_USE_ANIMATION
    edgui_anim_del(edgui_page_get_scrl(roller), (edgui_anim_exec_xcb_t)edgui_obj_set_y);
#endif
    edgui_ddlist_set_selected(roller, ext->ddlist.sel_opt_id);
}

/**
 * Set the middle page for the roller if inifinte is enabled
 * @param scrl pointer to the roller's scrollable (edgui_obj_t *)
 */
static void inf_normalize(void * scrl)
{
    edgui_obj_t * roller_scrl = (edgui_obj_t *)scrl;
    edgui_obj_t * roller      = edgui_obj_get_parent(roller_scrl);
    edgui_roller_ext_t * ext  = edgui_obj_get_ext_attr(roller);

    if(ext->mode == EDGUI_ROLLER_MODE_INIFINITE) {
        uint16_t real_id_cnt = ext->ddlist.option_cnt / EDGUI_ROLLER_INF_PAGES;

        ext->ddlist.sel_opt_id = ext->ddlist.sel_opt_id % real_id_cnt;

        ext->ddlist.sel_opt_id += (EDGUI_ROLLER_INF_PAGES / 2) * real_id_cnt; /*Select the middle page*/

        /*Move to the new id*/
        const edgui_style_t * style_label = edgui_obj_get_style(ext->ddlist.label);
        const edgui_font_t * font         = style_label->text.font;
        edgui_coord_t font_h              = edgui_font_get_line_height(font);
        edgui_coord_t h                   = edgui_obj_get_height(roller);

        edgui_coord_t line_y1 = ext->ddlist.sel_opt_id * (font_h + style_label->text.line_space) +
                             ext->ddlist.label->coords.y1 - roller_scrl->coords.y1;
        edgui_coord_t new_y = -line_y1 + (h - font_h) / 2;
        edgui_obj_set_y(roller_scrl, new_y);
    }
}

#if EDGUI_USE_ANIMATION
static void scroll_anim_ready_cb(edgui_anim_t * a)
{
    inf_normalize(a->var);
}
#endif

#endif
