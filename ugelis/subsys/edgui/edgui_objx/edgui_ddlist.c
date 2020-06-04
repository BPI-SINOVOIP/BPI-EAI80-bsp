/**
 * @file edgui_ddlist.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_ddlist.h"
#if EDGUI_USE_DDLIST != 0

#include "../edgui_draw/edgui_draw.h"
#include "../edgui_core/edgui_group.h"
#include "../edgui_core/edgui_indev.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_font/edgui_symbol_def.h"
#include "../edgui_misc/edgui_anim.h"
#include "../edgui_misc/edgui_math.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#if EDGUI_USE_ANIMATION == 0
#undef EDGUI_DDLIST_DEF_ANIM_TIME
#define EDGUI_DDLIST_DEF_ANIM_TIME 0 /*No animation*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_ddlist_design(edgui_obj_t * ddlist, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_ddlist_signal(edgui_obj_t * ddlist, edgui_signal_t sign, void * param);
static edgui_res_t edgui_ddlist_scrl_signal(edgui_obj_t * scrl, edgui_signal_t sign, void * param);
static edgui_res_t release_handler(edgui_obj_t * ddlist);
static void edgui_ddlist_refr_size(edgui_obj_t * ddlist, edgui_anim_enable_t anim);
static void edgui_ddlist_pos_current_option(edgui_obj_t * ddlist);
static void edgui_ddlist_refr_width(edgui_obj_t * ddlist);
#if EDGUI_USE_ANIMATION
static void edgui_ddlist_anim_ready_cb(edgui_anim_t * a);
static void edgui_ddlist_anim_finish(edgui_obj_t * ddlist);
static void edgui_ddlist_adjust_height(edgui_obj_t * ddlist, edgui_anim_value_t height);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;
static edgui_signal_cb_t ancestor_scrl_signal;
static edgui_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a drop down list objects
 * @param par pointer to an object, it will be the parent of the new drop down list
 * @param copy pointer to a drop down list object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created drop down list
 */
edgui_obj_t * edgui_ddlist_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("drop down list create started");

    /*Create the ancestor drop down list*/
    edgui_obj_t * new_ddlist = edgui_page_create(par, copy);
    edgui_mem_assert(new_ddlist);
    if(new_ddlist == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_ddlist);
    if(ancestor_scrl_signal == NULL) ancestor_scrl_signal = edgui_obj_get_signal_cb(edgui_page_get_scrl(new_ddlist));
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_ddlist);

    /*Allocate the drop down list type specific extended data*/
    edgui_ddlist_ext_t * ext = edgui_obj_allocate_ext_attr(new_ddlist, sizeof(edgui_ddlist_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */
    ext->label          = NULL;
    ext->opened         = 0;
    ext->fix_height     = 0;
    ext->sel_opt_id     = 0;
    ext->sel_opt_id_ori = 0;
    ext->option_cnt     = 0;
    ext->sel_style      = &edgui_style_plain_color;
    ext->draw_arrow     = 0; /*Do not draw arrow by default*/
    ext->stay_open      = 0;

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_ddlist, edgui_ddlist_signal);
    edgui_obj_set_signal_cb(edgui_page_get_scrl(new_ddlist), edgui_ddlist_scrl_signal);
    edgui_obj_set_design_cb(new_ddlist, edgui_ddlist_design);

    /*Init the new drop down list drop down list*/
    if(copy == NULL) {
        edgui_page_set_anim_time(new_ddlist, EDGUI_DDLIST_DEF_ANIM_TIME);

        edgui_obj_t * scrl = edgui_page_get_scrl(new_ddlist);
        edgui_obj_set_drag(scrl, false);
        edgui_page_set_scrl_fit2(new_ddlist, EDGUI_FIT_FILL, EDGUI_FIT_TIGHT);

        ext->label = edgui_label_create(new_ddlist, NULL);
        edgui_cont_set_fit2(new_ddlist, EDGUI_FIT_TIGHT, EDGUI_FIT_NONE);
        edgui_page_set_sb_mode(new_ddlist, EDGUI_SB_MODE_HIDE);
        edgui_page_set_style(new_ddlist, EDGUI_PAGE_STYLE_SCRL, &edgui_style_transp_tight);

        edgui_ddlist_set_options(new_ddlist, "Option 1\nOption 2\nOption 3");

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_ddlist_set_style(new_ddlist, EDGUI_DDLIST_STYLE_BG, th->style.ddlist.bg);
            edgui_ddlist_set_style(new_ddlist, EDGUI_DDLIST_STYLE_SEL, th->style.ddlist.sel);
            edgui_ddlist_set_style(new_ddlist, EDGUI_DDLIST_STYLE_SB, th->style.ddlist.sb);
        } else {
            edgui_ddlist_set_style(new_ddlist, EDGUI_DDLIST_STYLE_BG, &edgui_style_pretty);
            edgui_ddlist_set_style(new_ddlist, EDGUI_DDLIST_STYLE_SEL, &edgui_style_plain_color);
            edgui_ddlist_set_style(new_ddlist, EDGUI_DDLIST_STYLE_SB, &edgui_style_pretty_color);
        }
    }
    /*Copy an existing drop down list*/
    else {
        edgui_ddlist_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->label                 = edgui_label_create(new_ddlist, copy_ext->label);
        edgui_label_set_text(ext->label, edgui_label_get_text(copy_ext->label));
        ext->sel_opt_id     = copy_ext->sel_opt_id;
        ext->sel_opt_id_ori = copy_ext->sel_opt_id;
        ext->fix_height     = copy_ext->fix_height;
        ext->option_cnt     = copy_ext->option_cnt;
        ext->sel_style      = copy_ext->sel_style;
        ext->draw_arrow     = copy_ext->draw_arrow;
        ext->stay_open      = copy_ext->stay_open;

        edgui_ddlist_set_style(new_ddlist, EDGUI_DDLIST_STYLE_BG, edgui_ddlist_get_style(copy, EDGUI_DDLIST_STYLE_BG));
        edgui_ddlist_set_style(new_ddlist, EDGUI_DDLIST_STYLE_SB, edgui_ddlist_get_style(copy, EDGUI_DDLIST_STYLE_SB));
        edgui_ddlist_set_style(new_ddlist, EDGUI_DDLIST_STYLE_SEL, edgui_ddlist_get_style(copy, EDGUI_DDLIST_STYLE_SEL));

    }

    EDGUI_LOG_INFO("drop down list created");

    return new_ddlist;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void edgui_ddlist_set_options(edgui_obj_t * ddlist, const char * options)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    /*Count the '\n'-s to determine the number of options*/
    ext->option_cnt = 0;
    uint16_t i;
    for(i = 0; options[i] != '\0'; i++) {
        if(options[i] == '\n') ext->option_cnt++;
    }
    ext->option_cnt++; /*Last option has no `\n`*/
    ext->sel_opt_id     = 0;
    ext->sel_opt_id_ori = 0;

    edgui_label_set_text(ext->label, options);

    edgui_ddlist_refr_width(ddlist);

    switch(edgui_label_get_align(ext->label)) {
        case EDGUI_LABEL_ALIGN_LEFT: edgui_obj_align(ext->label, NULL, EDGUI_ALIGN_IN_LEFT_MID, 0, 0); break;
        case EDGUI_LABEL_ALIGN_CENTER: edgui_obj_align(ext->label, NULL, EDGUI_ALIGN_CENTER, 0, 0); break;
        case EDGUI_LABEL_ALIGN_RIGHT: edgui_obj_align(ext->label, NULL, EDGUI_ALIGN_IN_RIGHT_MID, 0, 0); break;
    }

    edgui_ddlist_refr_size(ddlist, false);
}

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void edgui_ddlist_set_selected(edgui_obj_t * ddlist, uint16_t sel_opt)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);
    if(ext->sel_opt_id == sel_opt) return;

    ext->sel_opt_id     = sel_opt < ext->option_cnt ? sel_opt : ext->option_cnt - 1;
    ext->sel_opt_id_ori = ext->sel_opt_id;
    /*Move the list to show the current option*/
    if(ext->opened == 0) {
        edgui_ddlist_pos_current_option(ddlist);
    } else {
        edgui_obj_invalidate(ddlist);
    }
}

/**
 * Set a fix height for the drop down list
 * If 0 then the opened ddlist will be auto. sized else the set height will be applied.
 * @param ddlist pointer to a drop down list
 * @param h the height when the list is opened (0: auto size)
 */
void edgui_ddlist_set_fix_height(edgui_obj_t * ddlist, edgui_coord_t h)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);
    if(ext->fix_height == h) return;

    ext->fix_height = h;

    edgui_ddlist_refr_size(ddlist, false);
}

/**
 * Set a fix width for the drop down list
 * @param ddlist pointer to a drop down list
 * @param w the width when the list is opened (0: auto size)
 */
void edgui_ddlist_set_fix_width(edgui_obj_t * ddlist, edgui_coord_t w)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);
    if(w == 0) {
        edgui_cont_set_fit2(ddlist, EDGUI_FIT_TIGHT, edgui_cont_get_fit_bottom(ddlist));
    } else {
        edgui_cont_set_fit2(ddlist, EDGUI_FIT_NONE, edgui_cont_get_fit_bottom(ddlist));
        edgui_obj_set_width(ddlist, w);
    }

    switch(edgui_label_get_align(ext->label)) {
        case EDGUI_LABEL_ALIGN_LEFT: edgui_obj_align(ext->label, NULL, EDGUI_ALIGN_IN_LEFT_MID, 0, 0); break;
        case EDGUI_LABEL_ALIGN_CENTER: edgui_obj_align(ext->label, NULL, EDGUI_ALIGN_CENTER, 0, 0); break;
        case EDGUI_LABEL_ALIGN_RIGHT: edgui_obj_align(ext->label, NULL, EDGUI_ALIGN_IN_RIGHT_MID, 0, 0); break;
    }

    edgui_ddlist_refr_size(ddlist, false);
}

/**
 * Set arrow draw in a drop down list
 * @param ddlist pointer to drop down list object
 * @param en enable/disable a arrow draw. E.g. "true" for draw.
 */
void edgui_ddlist_set_draw_arrow(edgui_obj_t * ddlist, bool en)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    /*Set the flag*/
    ext->draw_arrow = en ? 1 : 0;
}

/**
 * Leave the list opened when a new value is selected
 * @param ddlist pointer to drop down list object
 * @param en enable/disable "stay open" feature
 */
void edgui_ddlist_set_stay_open(edgui_obj_t * ddlist, bool en)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    /*Set the flag*/
    ext->stay_open = en ? 1 : 0;
}

/**
 * Set a style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_ddlist_set_style(edgui_obj_t * ddlist, edgui_ddlist_style_t type, const edgui_style_t * style)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    switch(type) {
        case EDGUI_DDLIST_STYLE_BG:
            edgui_page_set_style(ddlist, EDGUI_PAGE_STYLE_BG, style);
            edgui_ddlist_refr_width(ddlist);
            break;
        case EDGUI_DDLIST_STYLE_SB: edgui_page_set_style(ddlist, EDGUI_PAGE_STYLE_SB, style); break;
        case EDGUI_DDLIST_STYLE_SEL:
            ext->sel_style  = style;
            edgui_obj_t * scrl = edgui_page_get_scrl(ddlist);
            edgui_obj_refresh_ext_draw_pad(scrl); /*Because of the wider selected rectangle*/
            break;
    }
}

void edgui_ddlist_set_align(edgui_obj_t * ddlist, edgui_label_align_t align)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    edgui_label_set_align(ext->label, align);
    switch(align) {
        case EDGUI_LABEL_ALIGN_LEFT: edgui_obj_align(ext->label, NULL, EDGUI_ALIGN_IN_LEFT_MID, 0, 0); break;
        case EDGUI_LABEL_ALIGN_CENTER: edgui_obj_align(ext->label, NULL, EDGUI_ALIGN_CENTER, 0, 0); break;
        case EDGUI_LABEL_ALIGN_RIGHT: edgui_obj_align(ext->label, NULL, EDGUI_ALIGN_IN_RIGHT_MID, 0, 0); break;
    }
}
/*=====================
 * Getter functions
 *====================*/

/**
 * Get the options of a drop down list
 * @param ddlist pointer to drop down list object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * edgui_ddlist_get_options(const edgui_obj_t * ddlist)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);
    return edgui_label_get_text(ext->label);
}

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t edgui_ddlist_get_selected(const edgui_obj_t * ddlist)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    return ext->sel_opt_id;
}

/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
void edgui_ddlist_get_selected_str(const edgui_obj_t * ddlist, char * buf, uint16_t buf_size)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    uint16_t i;
    uint16_t line        = 0;
    const char * opt_txt = edgui_label_get_text(ext->label);
    uint16_t txt_len     = strlen(opt_txt);

    for(i = 0; i < txt_len && line != ext->sel_opt_id; i++) {
        if(opt_txt[i] == '\n') line++;
    }

    uint16_t c;
    for(c = 0; opt_txt[i] != '\n' && i < txt_len; c++, i++) {
        if(buf_size && c >= buf_size - 1) {
            EDGUI_LOG_WARN("edgui_ddlist_get_selected_str: the buffer was too small")
            break;
        }
        buf[c] = opt_txt[i];
    }

    buf[c] = '\0';
}

/**
 * Get the fix height value.
 * @param ddlist pointer to a drop down list object
 * @return the height if the ddlist is opened (0: auto size)
 */
edgui_coord_t edgui_ddlist_get_fix_height(const edgui_obj_t * ddlist)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);
    return ext->fix_height;
}

/**
 * Get arrow draw in a drop down list
 * @param ddlist pointer to drop down list object
 */
bool edgui_ddlist_get_draw_arrow(edgui_obj_t * ddlist)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    return ext->draw_arrow ? true : false;
}

/**
 * Get whether the drop down list stay open after selecting a  value or not
 * @param ddlist pointer to drop down list object
 */
bool edgui_ddlist_get_stay_open(edgui_obj_t * ddlist)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    return ext->stay_open ? true : false;
}

/**
 * Get a style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_ddlist_get_style(const edgui_obj_t * ddlist, edgui_ddlist_style_t type)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    switch(type) {
        case EDGUI_DDLIST_STYLE_BG: return edgui_page_get_style(ddlist, EDGUI_PAGE_STYLE_BG);
        case EDGUI_DDLIST_STYLE_SB: return edgui_page_get_style(ddlist, EDGUI_PAGE_STYLE_SB);
        case EDGUI_DDLIST_STYLE_SEL: return ext->sel_style;
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

edgui_label_align_t edgui_ddlist_get_align(const edgui_obj_t * ddlist)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    return edgui_label_get_align(ext->label);
}

/*=====================
 * Other functions
 *====================*/

/**
 * Open the drop down list with or without animation
 * @param ddlist pointer to drop down list object
 * @param anim_en EDGUI_ANIM_EN: use animation; EDGUI_ANIM_OFF: not use animations
 */
void edgui_ddlist_open(edgui_obj_t * ddlist, edgui_anim_enable_t anim)
{
#if EDGUI_USE_ANIMATION == 0
    anim = false;
#endif
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);
    ext->opened           = 1;
    edgui_obj_set_drag(edgui_page_get_scrl(ddlist), true);
    edgui_ddlist_refr_size(ddlist, anim);
}

/**
 * Close (Collapse) the drop down list
 * @param ddlist pointer to drop down list object
 * @param anim_en EDGUI_ANIM_ON: use animation; EDGUI_ANIM_OFF: not use animations
 */
void edgui_ddlist_close(edgui_obj_t * ddlist, edgui_anim_enable_t anim)
{
#if EDGUI_USE_ANIMATION == 0
    anim = false;
#endif
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);
    ext->opened           = 0;
    edgui_obj_set_drag(edgui_page_get_scrl(ddlist), false);
    edgui_ddlist_refr_size(ddlist, anim);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get the text alignment flag for a drop down list.
 * @param ddlist drop down list
 * @return text alignment flag
 */
static edgui_txt_flag_t edgui_ddlist_get_txt_flag(const edgui_obj_t * ddlist)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    /*The label might be already deleted so just return with some value*/
    if(!ext->label) return EDGUI_TXT_FLAG_CENTER;

    edgui_label_align_t align = edgui_label_get_align(ext->label);

    switch(align) {
        default:
        case EDGUI_LABEL_ALIGN_LEFT: return EDGUI_TXT_FLAG_NONE;
        case EDGUI_LABEL_ALIGN_CENTER: return EDGUI_TXT_FLAG_CENTER;
        case EDGUI_LABEL_ALIGN_RIGHT: return EDGUI_TXT_FLAG_RIGHT;
    }
}

/**
 * Handle the drawing related tasks of the drop down lists
 * @param ddlist pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_ddlist_design(edgui_obj_t * ddlist, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return ancestor_design(ddlist, mask, mode);
    }
    /*Draw the object*/
    else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        ancestor_design(ddlist, mask, mode);

        edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);
        edgui_opa_t opa_scale    = edgui_obj_get_opa_scale(ddlist);
        /*If the list is opened draw a rectangle under the selected item*/
        if(ext->opened != 0 || ext->force_sel) {
            const edgui_style_t * style = edgui_ddlist_get_style(ddlist, EDGUI_DDLIST_STYLE_BG);
            const edgui_font_t * font   = style->text.font;
            edgui_coord_t font_h        = edgui_font_get_line_height(font);

            /*Draw the selected*/
            edgui_area_t rect_area;
            rect_area.y1 = ext->label->coords.y1;
            rect_area.y1 += ext->sel_opt_id * (font_h + style->text.line_space);
            rect_area.y1 -= style->text.line_space / 2;

            rect_area.y2 = rect_area.y1 + font_h + style->text.line_space - 1;
            rect_area.x1 = ddlist->coords.x1;
            rect_area.x2 = ddlist->coords.x2;

            edgui_draw_rect(&rect_area, mask, ext->sel_style, opa_scale);
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == EDGUI_DESIGN_DRAW_POST) {
        /*Redraw the text on the selected area with a different color*/
        edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);
        edgui_opa_t opa_scale    = edgui_obj_get_opa_scale(ddlist);

        /*Redraw only in opened state*/
        if(ext->opened || ext->force_sel) {
            const edgui_style_t * style = edgui_ddlist_get_style(ddlist, EDGUI_DDLIST_STYLE_BG);
            const edgui_font_t * font   = style->text.font;
            edgui_coord_t font_h        = edgui_font_get_line_height(font);

            edgui_area_t area_sel;
            area_sel.y1 = ext->label->coords.y1;
            area_sel.y1 += ext->sel_opt_id * (font_h + style->text.line_space);
            area_sel.y1 -= style->text.line_space / 2;

            area_sel.y2 = area_sel.y1 + font_h + style->text.line_space - 1;
            area_sel.x1 = ddlist->coords.x1;
            area_sel.x2 = ddlist->coords.x2;
            edgui_area_t mask_sel;
            bool area_ok;
            area_ok = edgui_area_intersect(&mask_sel, mask, &area_sel);
            if(area_ok) {
                const edgui_style_t * sel_style = edgui_ddlist_get_style(ddlist, EDGUI_DDLIST_STYLE_SEL);
                edgui_style_t new_style;
                edgui_style_copy(&new_style, style);
                new_style.text.color = sel_style->text.color;
                new_style.text.opa   = sel_style->text.opa;
                edgui_txt_flag_t flag   = edgui_ddlist_get_txt_flag(ddlist);
                edgui_draw_label(&ext->label->coords, &mask_sel, &new_style, opa_scale, edgui_label_get_text(ext->label),
                              flag, NULL, -1, -1, NULL);
            }
        }

        /*Add a down symbol in ddlist when closed*/
        else {
            /*Draw a arrow in ddlist if enabled*/
            if(ext->draw_arrow) {
                const edgui_style_t * style     = edgui_ddlist_get_style(ddlist, EDGUI_DDLIST_STYLE_BG);
                const edgui_font_t * font       = style->text.font;
                const edgui_style_t * sel_style = edgui_ddlist_get_style(ddlist, EDGUI_DDLIST_STYLE_BG);
                edgui_coord_t font_h            = edgui_font_get_line_height(font);
                edgui_style_t new_style;
                edgui_style_copy(&new_style, style);
                new_style.text.color = sel_style->text.color;
                new_style.text.opa   = sel_style->text.opa;
                edgui_area_t area_arrow;
                area_arrow.x2 = ddlist->coords.x2 - style->body.padding.right;
                area_arrow.x1 = area_arrow.x2 -
                                edgui_txt_get_width(EDGUI_SYMBOL_DOWN, strlen(EDGUI_SYMBOL_DOWN), sel_style->text.font, 0, 0);

                area_arrow.y1 = ddlist->coords.y1 + style->text.line_space;
                area_arrow.y2 = area_arrow.y1 + font_h;

                edgui_area_t mask_arrow;
                bool area_ok;
                area_ok = edgui_area_intersect(&mask_arrow, mask, &area_arrow);
                if(area_ok) {
                    edgui_draw_label(&area_arrow, &mask_arrow, &new_style, opa_scale, EDGUI_SYMBOL_DOWN, EDGUI_TXT_FLAG_NONE,
                                  NULL, -1, -1, NULL); /*Use a down arrow in ddlist, you can replace it with your
                                                    custom symbol*/
                }
            }
        }
        /*Draw the scrollbar in the ancestor page design function*/
        ancestor_design(ddlist, mask, mode);
    }

    return true;
}

/**
 * Signal function of the drop down list
 * @param ddlist pointer to a drop down list object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_ddlist_signal(edgui_obj_t * ddlist, edgui_signal_t sign, void * param)
{
    edgui_res_t res;
    /* Include the ancient signal function */
    res = ancestor_signal(ddlist, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        edgui_ddlist_refr_size(ddlist, 0);
    } else if(sign == EDGUI_SIGNAL_CLEANUP) {
        ext->label = NULL;
    } else if(sign == EDGUI_SIGNAL_FOCUS) {
#if EDGUI_USE_GROUP
        edgui_group_t * g             = edgui_obj_get_group(ddlist);
        bool editing               = edgui_group_get_editing(g);
        edgui_indev_type_t indev_type = edgui_indev_get_type(edgui_indev_get_act());

        /*Encoders need special handling*/
        if(indev_type == EDGUI_INDEV_TYPE_ENCODER) {
            /*Open the list if editing*/
            if(editing) {
                ext->opened         = true;
                ext->sel_opt_id_ori = ext->sel_opt_id;
                edgui_ddlist_refr_size(ddlist, true);
            }
            /*Close the lift if navigating*/
            else {
                ext->opened     = false;
                ext->sel_opt_id = ext->sel_opt_id_ori;
                edgui_ddlist_refr_size(ddlist, true);
            }
        } else {
            /*Open the list if closed*/
            if(!ext->opened) {
                ext->opened         = true;
                ext->sel_opt_id_ori = ext->sel_opt_id; /*Save the current value. Used to revert this
                                                          state if ENER wont't be pressed*/
                edgui_ddlist_refr_size(ddlist, true);
            }
        }
#endif
    } else if(sign == EDGUI_SIGNAL_RELEASED) {
        release_handler(ddlist);
    } else if(sign == EDGUI_SIGNAL_DEFOCUS) {
        if(ext->opened) {
            ext->opened     = false;
            ext->sel_opt_id = ext->sel_opt_id_ori;
            edgui_ddlist_refr_size(ddlist, true);
        }
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == EDGUI_KEY_RIGHT || c == EDGUI_KEY_DOWN) {
            if(!ext->opened) {
                ext->opened = 1;
                edgui_ddlist_refr_size(ddlist, true);
            }

            if(ext->sel_opt_id + 1 < ext->option_cnt) {
                ext->sel_opt_id++;
                edgui_ddlist_pos_current_option(ddlist);
                edgui_obj_invalidate(ddlist);
            }
        } else if(c == EDGUI_KEY_LEFT || c == EDGUI_KEY_UP) {
            if(!ext->opened) {
                ext->opened = 1;
                edgui_ddlist_refr_size(ddlist, true);
            }
            if(ext->sel_opt_id > 0) {
                ext->sel_opt_id--;
                edgui_ddlist_pos_current_option(ddlist);
                edgui_obj_invalidate(ddlist);
            }
        } else if(c == EDGUI_KEY_ESC) {
            if(ext->opened) {
                ext->opened     = 0;
                ext->sel_opt_id = ext->sel_opt_id_ori;
                edgui_ddlist_refr_size(ddlist, true);
            }
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
        buf->type[i] = "edgui_ddlist";
    }

    return res;
}

/**
 * Signal function of the drop down list's scrollable part
 * @param scrl pointer to a drop down list's scrollable part
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_ddlist_scrl_signal(edgui_obj_t * scrl, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_scrl_signal(scrl, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_obj_t * ddlist = edgui_obj_get_parent(scrl);

    if(sign == EDGUI_SIGNAL_REFR_EXT_DRAW_PAD) {
        /*TODO review this*/
        /* Because of the wider selected rectangle ext. size
         * In this way by dragging the scrollable part the wider rectangle area can be redrawn too*/
        const edgui_style_t * style = edgui_ddlist_get_style(ddlist, EDGUI_DDLIST_STYLE_BG);
        edgui_coord_t hpad          = EDGUI_MATH_MAX(style->body.padding.left, style->body.padding.right);
        if(scrl->ext_draw_pad < hpad) scrl->ext_draw_pad = hpad;
    } else if(sign == EDGUI_SIGNAL_RELEASED) {
        if(edgui_indev_is_dragging(edgui_indev_get_act()) == false) {
            release_handler(ddlist);
        }
    } else if(sign == EDGUI_SIGNAL_CLEANUP) {
        edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);
        ext->label            = NULL; /*The label is already deleted*/
    }

    return res;
}

/**
 * Called when a drop down list is released to open it or set new option
 * @param ddlist pointer to a drop down list object
 * @return EDGUI_ACTION_RES_INV if the ddlist it deleted in the user callback else EDGUI_ACTION_RES_OK
 */
static edgui_res_t release_handler(edgui_obj_t * ddlist)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    if(ext->opened == 0) { /*Open the list*/
        ext->opened = 1;
        edgui_obj_set_drag(edgui_page_get_scrl(ddlist), true);
        edgui_ddlist_refr_size(ddlist, true);
    } else {

        edgui_indev_t * indev = edgui_indev_get_act();
#if EDGUI_USE_GROUP
        /*Leave edit mode once a new item is selected*/
        if(edgui_indev_get_type(indev) == EDGUI_INDEV_TYPE_ENCODER) {
            ext->sel_opt_id_ori = ext->sel_opt_id;
            edgui_group_t * g      = edgui_obj_get_group(ddlist);
            if(edgui_group_get_editing(g)) {
                edgui_group_set_editing(g, false);
            }
        }
#endif

        /*Search the clicked option (For KEYPAD and ENCODER the new value should be already set)*/
        if(edgui_indev_get_type(indev) == EDGUI_INDEV_TYPE_POINTER || edgui_indev_get_type(indev) == EDGUI_INDEV_TYPE_BUTTON) {
            edgui_point_t p;
            edgui_indev_get_point(indev, &p);
            p.y -= ext->label->coords.y1;
            p.x -= ext->label->coords.x1;
            uint16_t letter_i;
            letter_i = edgui_label_get_letter_on(ext->label, &p);

            uint16_t new_opt  = 0;
            const char * txt  = edgui_label_get_text(ext->label);
            uint32_t i        = 0;
            uint32_t i_prev   = 0;

            uint32_t letter_cnt = 0;
            uint32_t letter;
            for(letter_cnt = 0; letter_cnt < letter_i; letter_cnt++) {
                letter = edgui_txt_encoded_next(txt, &i);
                /*Count he lines to reach the clicked letter. But ignore the last '\n' because it
                 * still belongs to the clicked line*/
                if(letter == '\n' && i_prev != letter_i) new_opt++;
                i_prev = i;
            }

            ext->sel_opt_id     = new_opt;
            ext->sel_opt_id_ori = ext->sel_opt_id;
        }

        uint32_t id  = ext->sel_opt_id; /*Just to use uint32_t in event data*/
        edgui_res_t res = edgui_event_send(ddlist, EDGUI_EVENT_VALUE_CHANGED, &id);
        if(res != EDGUI_RES_OK) return res;

        if(ext->stay_open == 0) {
            ext->opened = 0;
            edgui_obj_set_drag(edgui_page_get_scrl(ddlist), false);
            edgui_ddlist_refr_size(ddlist, true);
        } else {
            edgui_obj_invalidate(ddlist);
        }
    }

    return EDGUI_RES_OK;
}

/**
 * Refresh the size of drop down list according to its status (open or closed)
 * @param ddlist pointer to a drop down list object
 * @param anim Change the size (open/close) with or without animation (true/false)
 */
static void edgui_ddlist_refr_size(edgui_obj_t * ddlist, edgui_anim_enable_t anim)
{
#if EDGUI_USE_ANIMATION == 0
    anim = false;
#endif
    edgui_ddlist_ext_t * ext    = edgui_obj_get_ext_attr(ddlist);
    const edgui_style_t * style = edgui_obj_get_style(ddlist);
    edgui_coord_t new_height;

    /*Open the list*/
    if(ext->opened) {
        if(ext->fix_height == 0) {
            new_height =
                edgui_obj_get_height(edgui_page_get_scrl(ddlist)) + style->body.padding.top + style->body.padding.bottom;
        } else {
            new_height = ext->fix_height;
        }

    }
    /*Close the list*/
    else {
        const edgui_font_t * font         = style->text.font;
        const edgui_style_t * label_style = edgui_obj_get_style(ext->label);
        edgui_coord_t font_h              = edgui_font_get_line_height(font);
        new_height                     = font_h + 2 * label_style->text.line_space;

        edgui_page_set_sb_mode(ddlist, EDGUI_SB_MODE_HIDE);
    }

    if(anim == EDGUI_ANIM_OFF) {
        edgui_obj_set_height(ddlist, new_height);
        edgui_ddlist_pos_current_option(ddlist);
        if(ext->opened) edgui_page_set_sb_mode(ddlist, EDGUI_SB_MODE_UNHIDE);
#if EDGUI_USE_ANIMATION
        edgui_anim_del(ddlist, (edgui_anim_exec_xcb_t)edgui_ddlist_adjust_height); /*If an animation is in progress then
                                                                 it will overwrite this changes*/

        /*Force animation complete to fix highlight selection*/
        edgui_ddlist_anim_finish(ddlist);
    } else {
        /*Run the animation only if the the size will be different*/
        if(edgui_obj_get_height(ddlist) != new_height) {
            edgui_anim_t a;
            a.var            = ddlist;
            a.start          = edgui_obj_get_height(ddlist);
            a.end            = new_height;
            a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_ddlist_adjust_height;
            a.path_cb        = edgui_anim_path_linear;
            a.ready_cb       = edgui_ddlist_anim_ready_cb;
            a.act_time       = 0;
            a.time           = edgui_ddlist_get_anim_time(ddlist);
            a.playback       = 0;
            a.playback_pause = 0;
            a.repeat         = 0;
            a.repeat_pause   = 0;

            ext->force_sel = 1; /*Keep the list item selected*/
            edgui_anim_create(&a);
        }
#endif
    }
}

#if EDGUI_USE_ANIMATION
/**
 * Position the list and remove the selection highlight if it's closed.
 * Called at end of list animation.
 * @param a pointer to the animation
 */
static void edgui_ddlist_anim_ready_cb(edgui_anim_t * a)
{
    edgui_obj_t * ddlist = a->var;
    edgui_ddlist_anim_finish(ddlist);
}

/**
 * Clean up after the open animation
 * @param ddlist
 */
static void edgui_ddlist_anim_finish(edgui_obj_t * ddlist)
{
    edgui_ddlist_ext_t * ext = edgui_obj_get_ext_attr(ddlist);

    edgui_ddlist_pos_current_option(ddlist);
    ext->force_sel = 0; /*Turn off drawing of selection*/
    if(ext->opened) edgui_page_set_sb_mode(ddlist, EDGUI_SB_MODE_UNHIDE);
}

/**
 * Adjusts the ddlist's height and then positions the option within it's new height.
 * This keeps the option visible during animation.
 * @param ddlist Drop down list object
 * @param height New drop down list height
 */
static void edgui_ddlist_adjust_height(edgui_obj_t * ddlist, edgui_anim_value_t height)
{
    edgui_obj_set_height(ddlist, height);
    edgui_ddlist_pos_current_option(ddlist);
}
#endif

/**
 * Set the position of list when it is closed to show the selected item
 * @param ddlist pointer to a drop down list
 */
static void edgui_ddlist_pos_current_option(edgui_obj_t * ddlist)
{
    edgui_ddlist_ext_t * ext          = edgui_obj_get_ext_attr(ddlist);
    const edgui_style_t * style       = edgui_obj_get_style(ddlist);
    const edgui_font_t * font         = style->text.font;
    edgui_coord_t font_h              = edgui_font_get_line_height(font);
    const edgui_style_t * label_style = edgui_obj_get_style(ext->label);
    edgui_obj_t * scrl                = edgui_page_get_scrl(ddlist);

    edgui_coord_t h = edgui_obj_get_height(ddlist);
    edgui_coord_t line_y1 =
        ext->sel_opt_id * (font_h + label_style->text.line_space) + ext->label->coords.y1 - scrl->coords.y1;

    edgui_obj_set_y(scrl, -line_y1 + (h - font_h) / 2);
    edgui_obj_invalidate(ddlist);
}

/**
 * Be sure the width of the scrollable exactly fits the ddlist
 * @param ddlist pointer to a ddlist
 */
static void edgui_ddlist_refr_width(edgui_obj_t * ddlist)
{
    /*Set the TIGHT fit horizontally the set the width to the content*/
    edgui_page_set_scrl_fit2(ddlist, EDGUI_FIT_TIGHT, edgui_page_get_scrl_fit_bottom(ddlist));

    /*Revert FILL fit to fill the parent with the options area. It allows to RIGHT/CENTER align the text*/
    edgui_page_set_scrl_fit2(ddlist, EDGUI_FIT_FILL, edgui_page_get_scrl_fit_bottom(ddlist));
}

#endif
