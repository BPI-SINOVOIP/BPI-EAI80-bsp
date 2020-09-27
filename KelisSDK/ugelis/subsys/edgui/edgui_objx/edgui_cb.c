/**
 * @file edgui_cb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_cb.h"
#if EDGUI_USE_CB != 0

#include "../edgui_core/edgui_group.h"
#include "../edgui_themes/edgui_theme.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_cb_design(edgui_obj_t * cb, const edgui_area_t * mask, edgui_design_mode_t mode);
static bool edgui_bullet_design(edgui_obj_t * bullet, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_cb_signal(edgui_obj_t * cb, edgui_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_design_cb_t ancestor_bg_design;
static edgui_design_cb_t ancestor_bullet_design;
static edgui_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a check box objects
 * @param par pointer to an object, it will be the parent of the new check box
 * @param copy pointer to a check box object, if not NULL then the new object will be copied from it
 * @return pointer to the created check box
 */
edgui_obj_t * edgui_cb_create(edgui_obj_t * par, const edgui_obj_t * copy)
{

    EDGUI_LOG_TRACE("check box create started");

    /*Create the ancestor basic object*/
    edgui_obj_t * new_cb = edgui_btn_create(par, copy);
    edgui_mem_assert(new_cb);
    if(new_cb == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_cb);
    if(ancestor_bg_design == NULL) ancestor_bg_design = edgui_obj_get_design_cb(new_cb);

    edgui_cb_ext_t * ext = edgui_obj_allocate_ext_attr(new_cb, sizeof(edgui_cb_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->bullet = NULL;
    ext->label  = NULL;

    edgui_obj_set_signal_cb(new_cb, edgui_cb_signal);
    edgui_obj_set_design_cb(new_cb, edgui_cb_design);

    /*Init the new checkbox object*/
    if(copy == NULL) {
        ext->bullet = edgui_btn_create(new_cb, NULL);
        if(ancestor_bullet_design == NULL) ancestor_bullet_design = edgui_obj_get_design_cb(ext->bullet);
        edgui_obj_set_click(ext->bullet, false);

        ext->label = edgui_label_create(new_cb, NULL);

        edgui_cb_set_text(new_cb, "Check box");
        edgui_btn_set_layout(new_cb, EDGUI_LAYOUT_ROW_M);
        edgui_btn_set_fit(new_cb, EDGUI_FIT_TIGHT);
        edgui_btn_set_toggle(new_cb, true);
        edgui_obj_set_protect(new_cb, EDGUI_PROTECT_PRESS_LOST);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_cb_set_style(new_cb, EDGUI_CB_STYLE_BG, th->style.cb.bg);
            edgui_cb_set_style(new_cb, EDGUI_CB_STYLE_BOX_REL, th->style.cb.box.rel);
            edgui_cb_set_style(new_cb, EDGUI_CB_STYLE_BOX_PR, th->style.cb.box.pr);
            edgui_cb_set_style(new_cb, EDGUI_CB_STYLE_BOX_TGL_REL, th->style.cb.box.tgl_rel);
            edgui_cb_set_style(new_cb, EDGUI_CB_STYLE_BOX_TGL_PR, th->style.cb.box.tgl_pr);
            edgui_cb_set_style(new_cb, EDGUI_CB_STYLE_BOX_INA, th->style.cb.box.ina);
        } else {
            edgui_cb_set_style(new_cb, EDGUI_CB_STYLE_BG, &edgui_style_transp);
            edgui_cb_set_style(new_cb, EDGUI_CB_STYLE_BOX_REL, &edgui_style_pretty);
        }
    } else {
        edgui_cb_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->bullet            = edgui_btn_create(new_cb, copy_ext->bullet);
        ext->label             = edgui_label_create(new_cb, copy_ext->label);

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_cb);
    }

    edgui_obj_set_design_cb(ext->bullet, edgui_bullet_design);

    EDGUI_LOG_INFO("check box created");

    return new_cb;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a check box. `txt` will be copied and may be deallocated
 * after this function returns.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void edgui_cb_set_text(edgui_obj_t * cb, const char * txt)
{
    edgui_cb_ext_t * ext = edgui_obj_get_ext_attr(cb);
    edgui_label_set_text(ext->label, txt);
}

/**
 * Set the text of a check box. `txt` must not be deallocated during the life
 * of this checkbox.
 * @param cb pointer to a check box
 * @param txt the text of the check box. NULL to refresh with the current text.
 */
void edgui_cb_set_static_text(edgui_obj_t * cb, const char * txt)
{
    edgui_cb_ext_t * ext = edgui_obj_get_ext_attr(cb);
    edgui_label_set_static_text(ext->label, txt);
}

/**
 * Set a style of a check box
 * @param cb pointer to check box object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void edgui_cb_set_style(edgui_obj_t * cb, edgui_cb_style_t type, const edgui_style_t * style)
{
    edgui_cb_ext_t * ext = edgui_obj_get_ext_attr(cb);

    switch(type) {
        case EDGUI_CB_STYLE_BG:
            edgui_btn_set_style(cb, EDGUI_BTN_STYLE_REL, style);
            edgui_btn_set_style(cb, EDGUI_BTN_STYLE_PR, style);
            edgui_btn_set_style(cb, EDGUI_BTN_STYLE_TGL_REL, style);
            edgui_btn_set_style(cb, EDGUI_BTN_STYLE_TGL_PR, style);
            edgui_btn_set_style(cb, EDGUI_BTN_STYLE_INA, style);
            break;
        case EDGUI_CB_STYLE_BOX_REL: edgui_btn_set_style(ext->bullet, EDGUI_BTN_STYLE_REL, style); break;
        case EDGUI_CB_STYLE_BOX_PR: edgui_btn_set_style(ext->bullet, EDGUI_BTN_STYLE_PR, style); break;
        case EDGUI_CB_STYLE_BOX_TGL_REL: edgui_btn_set_style(ext->bullet, EDGUI_BTN_STYLE_TGL_REL, style); break;
        case EDGUI_CB_STYLE_BOX_TGL_PR: edgui_btn_set_style(ext->bullet, EDGUI_BTN_STYLE_TGL_PR, style); break;
        case EDGUI_CB_STYLE_BOX_INA: edgui_btn_set_style(ext->bullet, EDGUI_BTN_STYLE_INA, style); break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param cb pointer to check box object
 * @return pointer to the text of the check box
 */
const char * edgui_cb_get_text(const edgui_obj_t * cb)
{
    edgui_cb_ext_t * ext = edgui_obj_get_ext_attr(cb);
    return edgui_label_get_text(ext->label);
}

/**
 * Get a style of a button
 * @param cb pointer to check box object
 * @param type which style should be get
 * @return style pointer to the style
 *  */
const edgui_style_t * edgui_cb_get_style(const edgui_obj_t * cb, edgui_cb_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_cb_ext_t * ext        = edgui_obj_get_ext_attr(cb);

    switch(type) {
        case EDGUI_CB_STYLE_BOX_REL: style = edgui_btn_get_style(ext->bullet, EDGUI_BTN_STYLE_REL); break;
        case EDGUI_CB_STYLE_BOX_PR: style = edgui_btn_get_style(ext->bullet, EDGUI_BTN_STYLE_PR); break;
        case EDGUI_CB_STYLE_BOX_TGL_REL: style = edgui_btn_get_style(ext->bullet, EDGUI_BTN_STYLE_TGL_REL); break;
        case EDGUI_CB_STYLE_BOX_TGL_PR: style = edgui_btn_get_style(ext->bullet, EDGUI_BTN_STYLE_TGL_PR); break;
        case EDGUI_CB_STYLE_BOX_INA: style = edgui_btn_get_style(ext->bullet, EDGUI_BTN_STYLE_INA); break;
        default: style = NULL; break;
    }

    return style;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the check boxes
 * @param cb pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_cb_design(edgui_obj_t * cb, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    bool result = true;

    if(mode == EDGUI_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask_p area*/
        result = ancestor_bg_design(cb, mask, mode);
    } else if(mode == EDGUI_DESIGN_DRAW_MAIN || mode == EDGUI_DESIGN_DRAW_POST) {
        edgui_cb_ext_t * cb_ext      = edgui_obj_get_ext_attr(cb);
        edgui_btn_ext_t * bullet_ext = edgui_obj_get_ext_attr(cb_ext->bullet);

        /*Be sure the state of the bullet is the same as the parent button*/
        bullet_ext->state = cb_ext->bg_btn.state;

        result = ancestor_bg_design(cb, mask, mode);

    } else {
        result = ancestor_bg_design(cb, mask, mode);
    }

    return result;
}

/**
 * Handle the drawing related tasks of the check boxes
 * @param bullet pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_bullet_design(edgui_obj_t * bullet, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return ancestor_bullet_design(bullet, mask, mode);
    } else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
#if EDGUI_USE_GROUP
        /* If the check box is the active in a group and
         * the background is not visible (transparent)
         * then activate the style of the bullet*/
        const edgui_style_t * style_ori  = edgui_obj_get_style(bullet);
        edgui_obj_t * bg                 = edgui_obj_get_parent(bullet);
        const edgui_style_t * style_page = edgui_obj_get_style(bg);
        edgui_group_t * g                = edgui_obj_get_group(bg);
        if(style_page->body.opa == EDGUI_OPA_TRANSP) { /*Is the Background visible?*/
            if(edgui_group_get_focused(g) == bg) {
                edgui_style_t * style_mod;
                style_mod       = edgui_group_mod_style(g, style_ori);
                bullet->style_p = style_mod; /*Temporally change the style to the activated */
            }
        }
#endif
        ancestor_bullet_design(bullet, mask, mode);

#if EDGUI_USE_GROUP
        bullet->style_p = style_ori; /*Revert the style*/
#endif
    } else if(mode == EDGUI_DESIGN_DRAW_POST) {
        ancestor_bullet_design(bullet, mask, mode);
    }

    return true;
}

/**
 * Signal function of the check box
 * @param cb pointer to a check box object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_cb_signal(edgui_obj_t * cb, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(cb, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_cb_ext_t * ext = edgui_obj_get_ext_attr(cb);

    if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        const edgui_style_t * label_style = edgui_label_get_style(ext->label, EDGUI_LABEL_STYLE_MAIN);
        edgui_obj_set_size(ext->bullet, edgui_font_get_line_height(label_style->text.font),
                        edgui_font_get_line_height(label_style->text.font));
        edgui_btn_set_state(ext->bullet, edgui_btn_get_state(cb));
    } else if(sign == EDGUI_SIGNAL_PRESSED || sign == EDGUI_SIGNAL_RELEASED || sign == EDGUI_SIGNAL_PRESS_LOST) {
        edgui_btn_set_state(ext->bullet, edgui_btn_get_state(cb));
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == EDGUI_KEY_RIGHT || c == EDGUI_KEY_DOWN || c == EDGUI_KEY_LEFT || c == EDGUI_KEY_UP) {
            /*Follow the backgrounds state with the bullet*/
            edgui_btn_set_state(ext->bullet, edgui_btn_get_state(cb));
        }
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_cb";
    }

    return res;
}

#endif
