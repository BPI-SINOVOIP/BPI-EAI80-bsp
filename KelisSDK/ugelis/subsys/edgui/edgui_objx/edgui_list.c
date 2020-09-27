/**
 * @file edgui_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_list.h"
#if EDGUI_USE_LIST != 0

#include "../edgui_core/edgui_group.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_anim.h"
#include "../edgui_misc/edgui_math.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_LIST_LAYOUT_DEF EDGUI_LAYOUT_COL_M

#if EDGUI_USE_ANIMATION == 0
#undef EDGUI_LIST_DEF_ANIM_TIME
#define EDGUI_LIST_DEF_ANIM_TIME 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_list_signal(edgui_obj_t * list, edgui_signal_t sign, void * param);
static edgui_res_t edgui_list_btn_signal(edgui_obj_t * btn, edgui_signal_t sign, void * param);
static void edgui_list_btn_single_select(edgui_obj_t * btn);
static bool edgui_list_is_list_btn(edgui_obj_t * list_btn);
static bool edgui_list_is_list_img(edgui_obj_t * list_btn);
static bool edgui_list_is_list_label(edgui_obj_t * list_btn);

/**********************
 *  STATIC VARIABLES
 **********************/
#if EDGUI_USE_IMG
static edgui_signal_cb_t img_signal;
#endif
static edgui_signal_cb_t label_signal;
static edgui_signal_cb_t ancestor_page_signal;
static edgui_signal_cb_t ancestor_btn_signal;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a list objects
 * @param par pointer to an object, it will be the parent of the new list
 * @param copy pointer to a list object, if not NULL then the new object will be copied from it
 * @return pointer to the created list
 */
edgui_obj_t * edgui_list_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("list create started");

    /*Create the ancestor basic object*/
    edgui_obj_t * new_list = edgui_page_create(par, copy);
    edgui_mem_assert(new_list);
    if(new_list == NULL) return NULL;

    if(ancestor_page_signal == NULL) ancestor_page_signal = edgui_obj_get_signal_cb(new_list);

    edgui_list_ext_t * ext = edgui_obj_allocate_ext_attr(new_list, sizeof(edgui_list_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->style_img                        = NULL;
    ext->styles_btn[EDGUI_BTN_STATE_REL]     = &edgui_style_btn_rel;
    ext->styles_btn[EDGUI_BTN_STATE_PR]      = &edgui_style_btn_pr;
    ext->styles_btn[EDGUI_BTN_STATE_TGL_REL] = &edgui_style_btn_tgl_rel;
    ext->styles_btn[EDGUI_BTN_STATE_TGL_PR]  = &edgui_style_btn_tgl_pr;
    ext->styles_btn[EDGUI_BTN_STATE_INA]     = &edgui_style_btn_ina;
    ext->single_mode                      = false;
    ext->size                             = 0;

#if EDGUI_USE_GROUP
    ext->last_sel     = NULL;
    ext->selected_btn = NULL;
    ext->last_clicked_btn = NULL;
#endif

    edgui_obj_set_signal_cb(new_list, edgui_list_signal);

    /*Init the new list object*/
    if(copy == NULL) {
        edgui_page_set_anim_time(new_list, EDGUI_LIST_DEF_ANIM_TIME);
        edgui_page_set_scrl_fit2(new_list, EDGUI_FIT_FLOOD, EDGUI_FIT_TIGHT);
        edgui_obj_set_size(new_list, 2 * EDGUI_DPI, 3 * EDGUI_DPI);
        edgui_page_set_scrl_layout(new_list, EDGUI_LIST_LAYOUT_DEF);
        edgui_list_set_sb_mode(new_list, EDGUI_SB_MODE_DRAG);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BG, th->style.list.bg);
            edgui_list_set_style(new_list, EDGUI_LIST_STYLE_SCRL, th->style.list.scrl);
            edgui_list_set_style(new_list, EDGUI_LIST_STYLE_SB, th->style.list.sb);
            edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BTN_REL, th->style.list.btn.rel);
            edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BTN_PR, th->style.list.btn.pr);
            edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BTN_TGL_REL, th->style.list.btn.tgl_rel);
            edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BTN_TGL_PR, th->style.list.btn.tgl_pr);
            edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BTN_INA, th->style.list.btn.ina);
        } else {
            edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BG, &edgui_style_transp_fit);
            edgui_list_set_style(new_list, EDGUI_LIST_STYLE_SCRL, &edgui_style_pretty);
        }
    } else {
        edgui_list_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);

        edgui_obj_t * copy_btn = edgui_list_get_next_btn(copy, NULL);
        while(copy_btn) {
            const void * img_src = NULL;
#if EDGUI_USE_IMG
            edgui_obj_t * copy_img = edgui_list_get_btn_img(copy_btn);
            if(copy_img) img_src = edgui_img_get_src(copy_img);
#endif
            edgui_list_add_btn(new_list, img_src, edgui_list_get_btn_text(copy_btn));
            copy_btn = edgui_list_get_next_btn(copy, copy_btn);
        }

        edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BTN_REL, copy_ext->styles_btn[EDGUI_BTN_STATE_REL]);
        edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BTN_PR, copy_ext->styles_btn[EDGUI_BTN_STATE_PR]);
        edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BTN_TGL_REL, copy_ext->styles_btn[EDGUI_BTN_STATE_TGL_REL]);
        edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BTN_TGL_PR, copy_ext->styles_btn[EDGUI_BTN_STATE_TGL_REL]);
        edgui_list_set_style(new_list, EDGUI_LIST_STYLE_BTN_INA, copy_ext->styles_btn[EDGUI_BTN_STATE_INA]);

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_list);
    }

    EDGUI_LOG_INFO("list created");

    return new_list;
}

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void edgui_list_clean(edgui_obj_t * obj)
{
    edgui_obj_t * scrl = edgui_page_get_scrl(obj);
    edgui_obj_clean(scrl);
    edgui_list_ext_t * ext = edgui_obj_get_ext_attr(obj);
    ext->size           = 0;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a list element to the list
 * @param list pointer to list object
 * @param img_fn file name of an image before the text (NULL if unused)
 * @param txt text of the list element (NULL if unused)
 * @return pointer to the new list element which can be customized (a button)
 */
edgui_obj_t * edgui_list_add_btn(edgui_obj_t * list, const void * img_src, const char * txt)
{
    edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);
    ext->size++;
    /*Create a list element with the image an the text*/
    edgui_obj_t * liste;
    liste = edgui_btn_create(list, NULL);

    /*Save the original signal function because it will be required in `edgui_list_btn_signal`*/
    if(ancestor_btn_signal == NULL) ancestor_btn_signal = edgui_obj_get_signal_cb(liste);

    /*Set the default styles*/
    edgui_btn_set_style(liste, EDGUI_BTN_STYLE_REL, ext->styles_btn[EDGUI_BTN_STATE_REL]);
    edgui_btn_set_style(liste, EDGUI_BTN_STYLE_PR, ext->styles_btn[EDGUI_BTN_STATE_PR]);
    edgui_btn_set_style(liste, EDGUI_BTN_STYLE_TGL_REL, ext->styles_btn[EDGUI_BTN_STATE_TGL_REL]);
    edgui_btn_set_style(liste, EDGUI_BTN_STYLE_TGL_PR, ext->styles_btn[EDGUI_BTN_STATE_TGL_PR]);
    edgui_btn_set_style(liste, EDGUI_BTN_STYLE_INA, ext->styles_btn[EDGUI_BTN_STATE_INA]);

    edgui_page_glue_obj(liste, true);
    edgui_btn_set_layout(liste, EDGUI_LAYOUT_ROW_M);
    edgui_btn_set_fit2(liste, EDGUI_FIT_FLOOD, EDGUI_FIT_TIGHT);
    edgui_obj_set_protect(liste, EDGUI_PROTECT_PRESS_LOST);
    edgui_obj_set_signal_cb(liste, edgui_list_btn_signal);

#if EDGUI_USE_IMG != 0
    edgui_obj_t * img = NULL;
    if(img_src) {
        img = edgui_img_create(liste, NULL);
        edgui_img_set_src(img, img_src);
        edgui_obj_set_style(img, ext->style_img);
        edgui_obj_set_click(img, false);
        if(img_signal == NULL) img_signal = edgui_obj_get_signal_cb(img);
    }
#endif
    if(txt != NULL) {
        edgui_coord_t btn_hor_pad = ext->styles_btn[EDGUI_BTN_STYLE_REL]->body.padding.left -
                                 ext->styles_btn[EDGUI_BTN_STYLE_REL]->body.padding.right;
        edgui_obj_t * label = edgui_label_create(liste, NULL);
        edgui_label_set_text(label, txt);
        edgui_obj_set_click(label, false);
        edgui_label_set_long_mode(label, EDGUI_LABEL_LONG_SROLL_CIRC);
        edgui_obj_set_width(label, liste->coords.x2 - label->coords.x1 - btn_hor_pad);
        if(label_signal == NULL) label_signal = edgui_obj_get_signal_cb(label);
    }
#if EDGUI_USE_GROUP
    /* If this is the first item to be added to the list and the list is
     * focused, select it */
    {
        edgui_group_t * g = edgui_obj_get_group(list);
        if(ext->size == 1 && edgui_group_get_focused(g) == list) {
            edgui_list_set_btn_selected(list, liste);
        }
    }
#endif

    return liste;
}

/**
 * Remove the index of the button in the list
 * @param list pointer to a list object
 * @param index pointer to a the button's index in the list, index must be 0 <= index <
 * edgui_list_ext_t.size
 * @return true: successfully deleted
 */
bool edgui_list_remove(const edgui_obj_t * list, uint16_t index)
{
    edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);
    if(index >= ext->size) return false;
    uint16_t count = 0;
    edgui_obj_t * e   = edgui_list_get_next_btn(list, NULL);
    while(e != NULL) {
        if(count == index) {
            edgui_obj_del(e);
            ext->size--;
            return true;
        }
        e = edgui_list_get_next_btn(list, e);
        count++;
    }
    return false;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set single button selected mode, only one button will be selected if enabled.
 * @param list pointer to the currently pressed list object
 * @param mode, enable(true)/disable(false) single selected mode.
 */
void edgui_list_set_single_mode(edgui_obj_t * list, bool mode)
{
    edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);

    ext->single_mode = mode;
}

#if EDGUI_USE_GROUP

/**
 * Make a button selected
 * @param list pointer to a list object
 * @param btn pointer to a button to select
 *            NULL to not select any buttons
 */
void edgui_list_set_btn_selected(edgui_obj_t * list, edgui_obj_t * btn)
{
    edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);

    if(ext->selected_btn) {
        edgui_btn_state_t s = edgui_btn_get_state(ext->selected_btn);
        if(s == EDGUI_BTN_STATE_PR)
            edgui_btn_set_state(ext->selected_btn, EDGUI_BTN_STATE_REL);
        else if(s == EDGUI_BTN_STATE_TGL_PR)
            edgui_btn_set_state(ext->selected_btn, EDGUI_BTN_STATE_TGL_REL);
    }

    ext->selected_btn = btn;

    /*Don't forget which button was selected.
     * It will be restored when the list is focused.*/
    if(btn != NULL) {
        ext->last_sel = btn;
    }

    if(ext->selected_btn) {
        edgui_btn_state_t s = edgui_btn_get_state(ext->selected_btn);
        if(s == EDGUI_BTN_STATE_REL)
            edgui_btn_set_state(ext->selected_btn, EDGUI_BTN_STATE_PR);
        else if(s == EDGUI_BTN_STATE_TGL_REL)
            edgui_btn_set_state(ext->selected_btn, EDGUI_BTN_STATE_TGL_PR);

        edgui_page_focus(list, ext->selected_btn, edgui_list_get_anim_time(list));
    }
}

#endif

/**
 * Set a style of a list
 * @param list pointer to a list object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_list_set_style(edgui_obj_t * list, edgui_list_style_t type, const edgui_style_t * style)
{
    edgui_list_ext_t * ext           = edgui_obj_get_ext_attr(list);
    edgui_btn_style_t btn_style_refr = EDGUI_BTN_STYLE_REL;
    edgui_obj_t * btn;

    switch(type) {
        case EDGUI_LIST_STYLE_BG:
            edgui_page_set_style(list, EDGUI_PAGE_STYLE_BG, style);
            /*style change signal will call 'refr_btn_width' */
            break;
        case EDGUI_LIST_STYLE_SCRL: edgui_page_set_style(list, EDGUI_PAGE_STYLE_SCRL, style); break;
        case EDGUI_LIST_STYLE_SB: edgui_page_set_style(list, EDGUI_PAGE_STYLE_SB, style); break;
        case EDGUI_LIST_STYLE_EDGE_FLASH: edgui_page_set_style(list, EDGUI_PAGE_STYLE_EDGE_FLASH, style); break;
        case EDGUI_LIST_STYLE_BTN_REL:
            ext->styles_btn[EDGUI_BTN_STATE_REL] = style;
            btn_style_refr                    = EDGUI_BTN_STYLE_REL;
            break;
        case EDGUI_LIST_STYLE_BTN_PR:
            ext->styles_btn[EDGUI_BTN_STATE_PR] = style;
            btn_style_refr                   = EDGUI_BTN_STYLE_PR;
            break;
        case EDGUI_LIST_STYLE_BTN_TGL_REL:
            ext->styles_btn[EDGUI_BTN_STATE_TGL_REL] = style;
            btn_style_refr                        = EDGUI_BTN_STYLE_TGL_REL;
            break;
        case EDGUI_LIST_STYLE_BTN_TGL_PR:
            ext->styles_btn[EDGUI_BTN_STATE_TGL_PR] = style;
            btn_style_refr                       = EDGUI_BTN_STYLE_TGL_PR;
            break;
        case EDGUI_LIST_STYLE_BTN_INA:
            ext->styles_btn[EDGUI_BTN_STATE_INA] = style;
            btn_style_refr                    = EDGUI_BTN_STYLE_INA;
            break;
    }

    /*Refresh existing buttons' style*/
    if(type == EDGUI_LIST_STYLE_BTN_PR || type == EDGUI_LIST_STYLE_BTN_REL || type == EDGUI_LIST_STYLE_BTN_TGL_REL ||
       type == EDGUI_LIST_STYLE_BTN_TGL_PR || type == EDGUI_LIST_STYLE_BTN_INA) {
        btn = edgui_list_get_prev_btn(list, NULL);
        while(btn != NULL) {
            edgui_btn_set_style(btn, btn_style_refr, ext->styles_btn[btn_style_refr]);
            btn = edgui_list_get_prev_btn(list, btn);
        }
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get single button selected mode.
 * @param list pointer to the currently pressed list object.
 */
bool edgui_list_get_single_mode(edgui_obj_t * list)
{
    edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);

    return (ext->single_mode);
}

/**
 * Get the text of a list element
 * @param btn pointer to list element
 * @return pointer to the text
 */
const char * edgui_list_get_btn_text(const edgui_obj_t * btn)
{
    edgui_obj_t * label = edgui_list_get_btn_label(btn);
    if(label == NULL) return "";
    return edgui_label_get_text(label);
}

/**
 * Get the label object from a list element
 * @param btn pointer to a list element (button)
 * @return pointer to the label from the list element or NULL if not found
 */
edgui_obj_t * edgui_list_get_btn_label(const edgui_obj_t * btn)
{
    edgui_obj_t * label = edgui_obj_get_child(btn, NULL);
    if(label == NULL) return NULL;

    while(edgui_list_is_list_label(label) == false) {
        label = edgui_obj_get_child(btn, label);
        if(label == NULL) break;
    }

    return label;
}

/**
 * Get the image object from a list element
 * @param btn pointer to a list element (button)
 * @return pointer to the image from the list element or NULL if not found
 */
edgui_obj_t * edgui_list_get_btn_img(const edgui_obj_t * btn)
{
#if EDGUI_USE_IMG != 0
    edgui_obj_t * img = edgui_obj_get_child(btn, NULL);
    if(img == NULL) return NULL;

    while(edgui_list_is_list_img(img) == false) {
        img = edgui_obj_get_child(btn, img);
        if(img == NULL) break;
    }

    return img;
#else
    return NULL;
#endif
}

/**
 * Get the previous button from list. (Starts from the bottom button)
 * @param list pointer to a list object
 * @param prev_btn pointer to button. Search the previous before it.
 * @return pointer to the previous button or NULL when no more buttons
 */
edgui_obj_t * edgui_list_get_prev_btn(const edgui_obj_t * list, edgui_obj_t * prev_btn)
{
    /* Not a good practice but user can add/create objects to the lists manually.
     * When getting the next button try to be sure that it is at least a button */

    edgui_obj_t * btn;
    edgui_obj_t * scrl = edgui_page_get_scrl(list);

    btn = edgui_obj_get_child(scrl, prev_btn);
    if(btn == NULL) return NULL;

    while(edgui_list_is_list_btn(btn) == false) {
        btn = edgui_obj_get_child(scrl, btn);
        if(btn == NULL) break;
    }

    return btn;
}

/**
 * Get the next button from list. (Starts from the top button)
 * @param list pointer to a list object
 * @param prev_btn pointer to button. Search the next after it.
 * @return pointer to the next button or NULL when no more buttons
 */
edgui_obj_t * edgui_list_get_next_btn(const edgui_obj_t * list, edgui_obj_t * prev_btn)
{
    /* Not a good practice but user can add/create objects to the lists manually.
     * When getting the next button try to be sure that it is at least a button */

    edgui_obj_t * btn;
    edgui_obj_t * scrl = edgui_page_get_scrl(list);

    btn = edgui_obj_get_child_back(scrl, prev_btn);
    if(btn == NULL) return NULL;

    while(edgui_list_is_list_btn(btn) == false) {
        btn = edgui_obj_get_child_back(scrl, btn);
        if(btn == NULL) break;
    }

    return btn;
}

/**
 * Get the index of the button in the list
 * @param list pointer to a list object. If NULL, assumes btn is part of a list.
 * @param btn pointer to a list element (button)
 * @return the index of the button in the list, or -1 of the button not in this list
 */
int32_t edgui_list_get_btn_index(const edgui_obj_t * list, const edgui_obj_t * btn)
{
    int index = 0;
    if(list == NULL) {
        /* no list provided, assuming btn is part of a list */
        list = edgui_obj_get_parent(edgui_obj_get_parent(btn));
    }
    edgui_obj_t * e = edgui_list_get_next_btn(list, NULL);
    while(e != NULL) {
        if(e == btn) {
            return index;
        }
        index++;
        e = edgui_list_get_next_btn(list, e);
    }
    return -1;
}

/**
 * Get the number of buttons in the list
 * @param list pointer to a list object
 * @return the number of buttons in the list
 */
uint16_t edgui_list_get_size(const edgui_obj_t * list)
{
    edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);
    return ext->size;
}

#if EDGUI_USE_GROUP
/**
 * Get the currently selected button
 * @param list pointer to a list object
 * @return pointer to the selected button
 */
edgui_obj_t * edgui_list_get_btn_selected(const edgui_obj_t * list)
{
    edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);
    return ext->selected_btn;
}

#endif

/**
 * Get a style of a list
 * @param list pointer to a list object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
const edgui_style_t * edgui_list_get_style(const edgui_obj_t * list, edgui_list_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_list_ext_t * ext      = edgui_obj_get_ext_attr(list);

    switch(type) {
        case EDGUI_LIST_STYLE_BG: style = edgui_page_get_style(list, EDGUI_PAGE_STYLE_BG); break;
        case EDGUI_LIST_STYLE_SCRL: style = edgui_page_get_style(list, EDGUI_PAGE_STYLE_SCRL); break;
        case EDGUI_LIST_STYLE_SB: style = edgui_page_get_style(list, EDGUI_PAGE_STYLE_SB); break;
        case EDGUI_LIST_STYLE_EDGE_FLASH: style = edgui_page_get_style(list, EDGUI_PAGE_STYLE_EDGE_FLASH); break;
        case EDGUI_LIST_STYLE_BTN_REL: style = ext->styles_btn[EDGUI_BTN_STATE_REL]; break;
        case EDGUI_LIST_STYLE_BTN_PR: style = ext->styles_btn[EDGUI_BTN_STATE_PR]; break;
        case EDGUI_LIST_STYLE_BTN_TGL_REL: style = ext->styles_btn[EDGUI_BTN_STATE_TGL_REL]; break;
        case EDGUI_LIST_STYLE_BTN_TGL_PR: style = ext->styles_btn[EDGUI_BTN_STATE_TGL_PR]; break;
        case EDGUI_LIST_STYLE_BTN_INA: style = ext->styles_btn[EDGUI_BTN_STATE_INA]; break;
        default: style = NULL; break;
    }

    return style;
}
/*=====================
 * Other functions
 *====================*/

/**
 * Move the list elements up by one
 * @param list pointer a to list object
 */
void edgui_list_up(const edgui_obj_t * list)
{
    /*Search the first list element which 'y' coordinate is below the parent
     * and position the list to show this element on the bottom*/
    edgui_obj_t * scrl = edgui_page_get_scrl(list);
    edgui_obj_t * e;
    edgui_obj_t * e_prev = NULL;

    e                 = edgui_list_get_prev_btn(list, NULL);
    while(e != NULL) {
        if(e->coords.y2 <= list->coords.y2) {
            if(e_prev != NULL) {
                edgui_coord_t new_y = edgui_obj_get_height(list) - (edgui_obj_get_y(e_prev) + edgui_obj_get_height(e_prev));
                if(edgui_list_get_anim_time(list) == 0) {
                    edgui_obj_set_y(scrl, new_y);
                } else {
#if EDGUI_USE_ANIMATION
                    edgui_anim_t a;
                    a.var            = scrl;
                    a.start          = edgui_obj_get_y(scrl);
                    a.end            = new_y;
                    a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
                    a.path_cb        = edgui_anim_path_linear;
                    a.ready_cb       = NULL;
                    a.act_time       = 0;
                    a.time           = EDGUI_LIST_DEF_ANIM_TIME;
                    a.playback       = 0;
                    a.playback_pause = 0;
                    a.repeat         = 0;
                    a.repeat_pause   = 0;
                    edgui_anim_create(&a);
#endif
                }
            }
            break;
        }
        e_prev = e;
        e      = edgui_list_get_prev_btn(list, e);
    }
}

/**
 * Move the list elements down by one
 * @param list pointer to a list object
 */
void edgui_list_down(const edgui_obj_t * list)
{
    /*Search the first list element which 'y' coordinate is above the parent
     * and position the list to show this element on the top*/
    edgui_obj_t * scrl = edgui_page_get_scrl(list);
    edgui_obj_t * e;
    e = edgui_list_get_prev_btn(list, NULL);
    while(e != NULL) {
        if(e->coords.y1 < list->coords.y1) {
            edgui_coord_t new_y = -edgui_obj_get_y(e);
            if(edgui_list_get_anim_time(list) == 0) {
                edgui_obj_set_y(scrl, new_y);
            } else {
#if EDGUI_USE_ANIMATION
                edgui_anim_t a;
                a.var            = scrl;
                a.start          = edgui_obj_get_y(scrl);
                a.end            = new_y;
                a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
                a.path_cb        = edgui_anim_path_linear;
                a.ready_cb       = NULL;
                a.act_time       = 0;
                a.time           = EDGUI_LIST_DEF_ANIM_TIME;
                a.playback       = 0;
                a.playback_pause = 0;
                a.repeat         = 0;
                a.repeat_pause   = 0;
                edgui_anim_create(&a);
#endif
            }
            break;
        }
        e = edgui_list_get_prev_btn(list, e);
    }
}

/**
 * Focus on a list button. It ensures that the button will be visible on the list.
 * @param btn pointer to a list button to focus
 * @param anim_en EDGUI_ANIM_ON: scroll with animation, EDGUI_ANOM_OFF: without animation
 */
void edgui_list_focus(const edgui_obj_t * btn, edgui_anim_enable_t anim)
{

#if EDGUI_USE_ANIMATION == 0
    anim = false;
#endif

    edgui_obj_t * list = edgui_obj_get_parent(edgui_obj_get_parent(btn));

    edgui_page_focus(list, btn, anim == EDGUI_ANIM_OFF ? 0 : edgui_list_get_anim_time(list));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the list
 * @param list pointer to a list object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_list_signal(edgui_obj_t * list, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_page_signal(list, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_RELEASED || sign == EDGUI_SIGNAL_PRESSED || sign == EDGUI_SIGNAL_PRESSING ||
       sign == EDGUI_SIGNAL_LONG_PRESS || sign == EDGUI_SIGNAL_LONG_PRESS_REP) {
#if EDGUI_USE_GROUP
        /*If pressed/released etc by a KEYPAD or ENCODER delegate signal to the button*/
        edgui_indev_t * indev         = edgui_indev_get_act();
        edgui_indev_type_t indev_type = edgui_indev_get_type(indev);
        if(indev_type == EDGUI_INDEV_TYPE_KEYPAD ||
           (indev_type == EDGUI_INDEV_TYPE_ENCODER && edgui_group_get_editing(edgui_obj_get_group(list)))) {
            /*Get the 'pressed' button*/
            edgui_obj_t * btn = NULL;
            btn            = edgui_list_get_prev_btn(list, btn);
            while(btn != NULL) {
                if(edgui_btn_get_state(btn) == EDGUI_BTN_STATE_PR) break;
                btn = edgui_list_get_prev_btn(list, btn);
            }
            edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);

            /*The page receives the key presses so the events should be propagated to the selected
             * button*/
            if(btn) {
                if(sign == EDGUI_SIGNAL_PRESSED) {
                    res = edgui_event_send(btn, EDGUI_EVENT_PRESSED, NULL);
                } else if(sign == EDGUI_SIGNAL_PRESSING) {
                    res = edgui_event_send(btn, EDGUI_EVENT_PRESSING, NULL);
                } else if(sign == EDGUI_SIGNAL_LONG_PRESS) {
                    res = edgui_event_send(btn, EDGUI_EVENT_LONG_PRESSED, NULL);
                } else if(sign == EDGUI_SIGNAL_LONG_PRESS_REP) {
                    res = edgui_event_send(btn, EDGUI_EVENT_LONG_PRESSED_REPEAT, NULL);
                } else if(sign == EDGUI_SIGNAL_RELEASED) {
#if EDGUI_USE_GROUP
                    ext->last_sel = btn;
#endif
                    if(indev->proc.long_pr_sent == 0) {
                        res = edgui_event_send(btn, EDGUI_EVENT_SHORT_CLICKED, NULL);
                    }
                    if(edgui_indev_is_dragging(indev) == false && res == EDGUI_RES_OK) {
                        res = edgui_event_send(btn, EDGUI_EVENT_CLICKED, NULL);
                    }
                    if(res == EDGUI_RES_OK) {
                        res = edgui_event_send(btn, EDGUI_EVENT_RELEASED, NULL);
                    }
                }
            }
        }
#endif
    } else if(sign == EDGUI_SIGNAL_FOCUS) {

#if EDGUI_USE_GROUP
        edgui_indev_type_t indev_type = edgui_indev_get_type(edgui_indev_get_act());
        /*With ENCODER select the first button only in edit mode*/
        if(indev_type == EDGUI_INDEV_TYPE_ENCODER) {
            edgui_group_t * g = edgui_obj_get_group(list);
            if(edgui_group_get_editing(g)) {
                edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);
                if(ext->last_sel) {
                    /* Select the    last used button */
                    edgui_list_set_btn_selected(list, ext->last_sel);
                } else {
                    /*Get the first button and mark it as selected*/
                    edgui_list_set_btn_selected(list, edgui_list_get_next_btn(list, NULL));
                }
            } else {
                edgui_list_set_btn_selected(list, NULL);
            }
        }
        /*Else select the clicked button*/
        else {
            /*Mark the last clicked button (if any) as selected because it triggered the focus*/
            edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);
            if(ext->last_clicked_btn) {
                edgui_list_set_btn_selected(list, ext->last_clicked_btn);
                ext->last_clicked_btn = NULL;

            } else {
                if(ext->last_sel) {
                    /* Select the last used button */
                    edgui_list_set_btn_selected(list, ext->last_sel);
                } else {
                    /*Get the first button and mark it as selected*/
                    edgui_list_set_btn_selected(list, edgui_list_get_next_btn(list, NULL));
                }
            }
        }
#endif
    } else if(sign == EDGUI_SIGNAL_DEFOCUS) {

#if EDGUI_USE_GROUP
        /*De-select the selected btn*/
        edgui_list_set_btn_selected(list, NULL);
        edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);
        ext->last_clicked_btn    = NULL; /*button click will be set if click happens before focus*/
        ext->selected_btn   = NULL;
#endif
    } else if(sign == EDGUI_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = true;
    } else if(sign == EDGUI_SIGNAL_CONTROL) {

#if EDGUI_USE_GROUP
        char c = *((char *)param);
        if(c == EDGUI_KEY_RIGHT || c == EDGUI_KEY_DOWN) {
            edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);
            /*If there is a valid selected button the make the previous selected*/
            if(ext->selected_btn) {
                edgui_obj_t * btn_prev = edgui_list_get_next_btn(list, ext->selected_btn);
                if(btn_prev) edgui_list_set_btn_selected(list, btn_prev);
            }
            /*If there is no selected button the make the first selected*/
            else {
                edgui_obj_t * btn = edgui_list_get_next_btn(list, NULL);
                if(btn)
                    edgui_list_set_btn_selected(list,
                                             btn); /*If there are no buttons on the list then there is no first button*/
            }
        } else if(c == EDGUI_KEY_LEFT || c == EDGUI_KEY_UP) {
            edgui_list_ext_t * ext = edgui_obj_get_ext_attr(list);
            /*If there is a valid selected button the make the next selected*/
            if(ext->selected_btn != NULL) {
                edgui_obj_t * btn_next = edgui_list_get_prev_btn(list, ext->selected_btn);
                if(btn_next) edgui_list_set_btn_selected(list, btn_next);
            }
            /*If there is no selected button the make the first selected*/
            else {
                edgui_obj_t * btn = edgui_list_get_next_btn(list, NULL);
                if(btn) edgui_list_set_btn_selected(list, btn);
            }
        }
#endif
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_list";
    }
    return res;
}

/**
 * Signal function of the list buttons
 * @param btn pointer to a button on the list
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_list_btn_signal(edgui_obj_t * btn, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_btn_signal(btn, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_RELEASED) {
        edgui_obj_t * list          = edgui_obj_get_parent(edgui_obj_get_parent(btn));
        edgui_list_ext_t * ext      = edgui_obj_get_ext_attr(list);
        ext->page.scroll_prop_ip = 0;

#if EDGUI_USE_GROUP
        edgui_group_t * g = edgui_obj_get_group(list);
        if(edgui_group_get_focused(g) == list && edgui_indev_is_dragging(edgui_indev_get_act()) == false) {
            /* Is the list is focused then be sure only the button being released
             * has a pressed state to indicate the selected state on the list*/
            edgui_obj_t * btn_i = edgui_list_get_prev_btn(list, NULL);
            while(btn_i) {
                edgui_btn_state_t s = edgui_btn_get_state(btn_i);
                if(s == EDGUI_BTN_STATE_PR)
                    edgui_btn_set_state(btn_i, EDGUI_BTN_STATE_REL);
                else if(s == EDGUI_BTN_STATE_TGL_PR)
                    edgui_btn_set_state(btn_i, EDGUI_BTN_STATE_TGL_REL);
                btn_i = edgui_list_get_prev_btn(list, btn_i);
            }

            /*Make the released button "selected"*/
            edgui_list_set_btn_selected(list, btn);
        }

        /* If `click_focus == 1` then EDGUI_SIGNAL_FOCUS need to know which button triggered the focus
         * to mark it as selected (pressed state)*/
        ext->last_clicked_btn = btn;
#endif
        if(edgui_indev_is_dragging(edgui_indev_get_act()) == false && ext->single_mode) {
            edgui_list_btn_single_select(btn);
        }
    } else if(sign == EDGUI_SIGNAL_PRESS_LOST) {
        edgui_obj_t * list          = edgui_obj_get_parent(edgui_obj_get_parent(btn));
        edgui_list_ext_t * ext      = edgui_obj_get_ext_attr(list);
        ext->page.scroll_prop_ip = 0;
    } else if(sign == EDGUI_SIGNAL_CLEANUP) {

#if EDGUI_USE_GROUP
        edgui_obj_t * list = edgui_obj_get_parent(edgui_obj_get_parent(btn));
        edgui_obj_t * sel  = edgui_list_get_btn_selected(list);
        if(sel == btn) edgui_list_set_btn_selected(list, edgui_list_get_next_btn(list, btn));
#endif
    }

    return res;
}

/**
 * Make a single button selected in the list, deselect others.
 * @param btn pointer to the currently pressed list btn object
 */
static void edgui_list_btn_single_select(edgui_obj_t * btn)
{
    edgui_obj_t * list = edgui_obj_get_parent(edgui_obj_get_parent(btn));

    edgui_obj_t * e = edgui_list_get_next_btn(list, NULL);
    do {
        if(e == btn) {
            edgui_btn_set_state(e, EDGUI_BTN_STATE_TGL_REL);
        } else {
            edgui_btn_set_state(e, EDGUI_BTN_STATE_REL);
        }
        e = edgui_list_get_next_btn(list, e);
    } while(e != NULL);
}

/**
 * Check if this is really a list button or another object.
 * @param list_btn List button
 */
static bool edgui_list_is_list_btn(edgui_obj_t * list_btn)
{
    edgui_obj_type_t type;

    edgui_obj_get_type(list_btn, &type);
    uint8_t cnt;
    for(cnt = 0; cnt < EDGUI_MAX_ANCESTOR_NUM; cnt++) {
        if(type.type[cnt] == NULL) break;
        if(!strcmp(type.type[cnt], "edgui_btn")) return true;
    }
    return false;
}

/**
 * Check if this is really a list label or another object.
 * @param list_label List label
 */
static bool edgui_list_is_list_label(edgui_obj_t * list_label)
{
    edgui_obj_type_t type;

    edgui_obj_get_type(list_label, &type);
    uint8_t cnt;
    for(cnt = 0; cnt < EDGUI_MAX_ANCESTOR_NUM; cnt++) {
        if(type.type[cnt] == NULL) break;
        if(!strcmp(type.type[cnt], "edgui_label")) return true;
    }
    return false;
}

/**
 * Check if this is really a list image or another object.
 * @param list_image List image
 */
static bool edgui_list_is_list_img(edgui_obj_t * list_img)
{
    edgui_obj_type_t type;

    edgui_obj_get_type(list_img, &type);
    uint8_t cnt;
    for(cnt = 0; cnt < EDGUI_MAX_ANCESTOR_NUM; cnt++) {
        if(type.type[cnt] == NULL) break;
        if(!strcmp(type.type[cnt], "edgui_img")) return true;
    }
    return false;
}

#endif
