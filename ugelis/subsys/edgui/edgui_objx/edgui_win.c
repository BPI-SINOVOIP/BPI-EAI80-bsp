/**
 * @file edgui_win.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_win.h"
#if EDGUI_USE_WIN != 0

#include "../edgui_themes/edgui_theme.h"
#include "../edgui_core/edgui_disp.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_win_signal(edgui_obj_t * win, edgui_signal_t sign, void * param);
static void edgui_win_realign(edgui_obj_t * win);

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
 * Create a window objects
 * @param par pointer to an object, it will be the parent of the new window
 * @param copy pointer to a window object, if not NULL then the new object will be copied from it
 * @return pointer to the created window
 */
edgui_obj_t * edgui_win_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("window create started");

    /*Create the ancestor object*/
    edgui_obj_t * new_win = edgui_obj_create(par, copy);
    edgui_mem_assert(new_win);
    if(new_win == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_win);

    /*Allocate the object type specific extended data*/
    edgui_win_ext_t * ext = edgui_obj_allocate_ext_attr(new_win, sizeof(edgui_win_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->page          = NULL;
    ext->header        = NULL;
    ext->title         = NULL;
    ext->style_btn_rel = &edgui_style_btn_rel;
    ext->style_btn_pr  = &edgui_style_btn_pr;
    ext->btn_size      = (EDGUI_DPI) / 2;

    /*Init the new window object*/
    if(copy == NULL) {
        /* Set a size which fits into the parent.
         * Don't use `par` directly because if the window is created on a page it is moved to the
         * scrollable so the parent has changed */
        edgui_obj_set_size(new_win, edgui_obj_get_width_fit(edgui_obj_get_parent(new_win)),
                        edgui_obj_get_height_fit(edgui_obj_get_parent(new_win)));

        edgui_obj_set_pos(new_win, 0, 0);
        edgui_obj_set_style(new_win, &edgui_style_pretty);

        ext->page = edgui_page_create(new_win, NULL);
        edgui_obj_set_protect(ext->page, EDGUI_PROTECT_PARENT);
        edgui_page_set_sb_mode(ext->page, EDGUI_SB_MODE_AUTO);
        edgui_page_set_style(ext->page, EDGUI_PAGE_STYLE_BG, &edgui_style_transp_fit);

        /*Create a holder for the header*/
        ext->header = edgui_obj_create(new_win, NULL);
        /*Move back the header because it is automatically moved to the scrollable */
        edgui_obj_set_protect(ext->header, EDGUI_PROTECT_PARENT);
        edgui_obj_set_parent(ext->header, new_win);

        /*Create a title on the header*/
        ext->title = edgui_label_create(ext->header, NULL);
        edgui_label_set_text(ext->title, "My title");

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_win_set_style(new_win, EDGUI_WIN_STYLE_BG, th->style.win.bg);
            edgui_win_set_style(new_win, EDGUI_WIN_STYLE_SB, th->style.win.sb);
            edgui_win_set_style(new_win, EDGUI_WIN_STYLE_HEADER, th->style.win.header);
            edgui_win_set_style(new_win, EDGUI_WIN_STYLE_CONTENT, th->style.win.content);
            edgui_win_set_style(new_win, EDGUI_WIN_STYLE_BTN_REL, th->style.win.btn.rel);
            edgui_win_set_style(new_win, EDGUI_WIN_STYLE_BTN_PR, th->style.win.btn.pr);
        } else {
            edgui_win_set_style(new_win, EDGUI_WIN_STYLE_BG, &edgui_style_plain);
            edgui_win_set_style(new_win, EDGUI_WIN_STYLE_CONTENT, &edgui_style_transp);
            edgui_win_set_style(new_win, EDGUI_WIN_STYLE_HEADER, &edgui_style_plain_color);
        }

        edgui_obj_set_signal_cb(new_win, edgui_win_signal);
    }
    /*Copy an existing object*/
    else {
        edgui_win_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        /*Create the objects*/
        ext->header   = edgui_obj_create(new_win, copy_ext->header);
        ext->title    = edgui_label_create(ext->header, copy_ext->title);
        ext->page     = edgui_page_create(new_win, copy_ext->page);
        ext->btn_size = copy_ext->btn_size;

        /*Copy the control buttons*/
        edgui_obj_t * child;
        edgui_obj_t * cbtn;
        child = edgui_obj_get_child_back(copy_ext->header, NULL);
        child = edgui_obj_get_child_back(copy_ext->header, child); /*Sip the title*/
        while(child != NULL) {
            cbtn = edgui_btn_create(ext->header, child);
            edgui_img_create(cbtn, edgui_obj_get_child(child, NULL));
            child = edgui_obj_get_child_back(copy_ext->header, child);
        }

        edgui_obj_set_signal_cb(new_win, edgui_win_signal);
    }

    /*Refresh the style with new signal function*/
    edgui_obj_refresh_style(new_win);

    edgui_win_realign(new_win);

    EDGUI_LOG_INFO("window created");

    return new_win;
}

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void edgui_win_clean(edgui_obj_t * obj)
{
    edgui_obj_t * scrl = edgui_page_get_scrl(obj);
    edgui_obj_clean(scrl);
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add control button to the header of the window
 * @param win pointer to a window object
 * @param img_src an image source ('edgui_img_t' variable, path to file or a symbol)
 * @return pointer to the created button object
 */
edgui_obj_t * edgui_win_add_btn(edgui_obj_t * win, const void * img_src)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);

    edgui_obj_t * btn = edgui_btn_create(ext->header, NULL);
    edgui_btn_set_style(btn, EDGUI_BTN_STYLE_REL, ext->style_btn_rel);
    edgui_btn_set_style(btn, EDGUI_BTN_STYLE_PR, ext->style_btn_pr);
    edgui_obj_set_size(btn, ext->btn_size, ext->btn_size);

    edgui_obj_t * img = edgui_img_create(btn, NULL);
    edgui_obj_set_click(img, false);
    edgui_img_set_src(img, img_src);

    edgui_win_realign(win);

    return btn;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Can be assigned to a window control button to close the window
 * @param btn pointer to the control button on teh widows header
 * @param evet the event type
 */
void edgui_win_close_event_cb(edgui_obj_t * btn, edgui_event_t event)
{
    if(event == EDGUI_EVENT_RELEASED) {
        edgui_obj_t * win = edgui_win_get_from_btn(btn);

        edgui_obj_del(win);
    }
}

/**
 * Set the title of a window
 * @param win pointer to a window object
 * @param title string of the new title
 */
void edgui_win_set_title(edgui_obj_t * win, const char * title)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);

    edgui_label_set_text(ext->title, title);
    edgui_win_realign(win);
}

/**
 * Set the control button size of a window
 * @param win pointer to a window object
 * @param size control button size
 */
void edgui_win_set_btn_size(edgui_obj_t * win, edgui_coord_t size)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);
    if(ext->btn_size == size) return;

    ext->btn_size = size;

    edgui_win_realign(win);
}

/**
 * Set the layout of the window
 * @param win pointer to a window object
 * @param layout the layout from 'edgui_layout_t'
 */
void edgui_win_set_layout(edgui_obj_t * win, edgui_layout_t layout)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);
    edgui_page_set_scrl_layout(ext->page, layout);
}

/**
 * Set the scroll bar mode of a window
 * @param win pointer to a window object
 * @param sb_mode the new scroll bar mode from  'edgui_sb_mode_t'
 */
void edgui_win_set_sb_mode(edgui_obj_t * win, edgui_sb_mode_t sb_mode)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);
    edgui_page_set_sb_mode(ext->page, sb_mode);
}
/**
 * Set focus animation duration on `edgui_win_focus()`
 * @param win pointer to a window object
 * @param anim_time duration of animation [ms]
 */
void edgui_win_set_anim_time(edgui_obj_t * win, uint16_t anim_time)
{
    edgui_page_set_anim_time(edgui_win_get_content(win), anim_time);
}

/**
 * Set a style of a window
 * @param win pointer to a window object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_win_set_style(edgui_obj_t * win, edgui_win_style_t type, const edgui_style_t * style)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);

    switch(type) {
        case EDGUI_WIN_STYLE_BG:
            edgui_obj_set_style(win, style);
            edgui_win_realign(win);
            break;
        case EDGUI_WIN_STYLE_CONTENT: edgui_page_set_style(ext->page, EDGUI_PAGE_STYLE_SCRL, style); break;
        case EDGUI_WIN_STYLE_SB: edgui_page_set_style(ext->page, EDGUI_PAGE_STYLE_SB, style); break;
        case EDGUI_WIN_STYLE_HEADER:
            edgui_obj_set_style(ext->header, style);
            edgui_win_realign(win);
            break;
        case EDGUI_WIN_STYLE_BTN_REL: ext->style_btn_rel = style; break;
        case EDGUI_WIN_STYLE_BTN_PR: ext->style_btn_pr = style; break;
    }

    /*Refresh the existing buttons*/
    if(type == EDGUI_WIN_STYLE_BTN_REL || type == EDGUI_WIN_STYLE_BTN_PR) {
        edgui_obj_t * btn;
        btn = edgui_obj_get_child_back(ext->header, NULL);
        btn = edgui_obj_get_child_back(ext->header, btn); /*Skip the title*/
        while(btn != NULL) {
            if(type == EDGUI_WIN_STYLE_BTN_REL)
                edgui_btn_set_style(btn, EDGUI_BTN_STYLE_REL, style);
            else
                edgui_btn_set_style(btn, EDGUI_BTN_STYLE_PR, style);
            btn = edgui_obj_get_child_back(ext->header, btn);
        }
    }
}

/**
 * Set drag status of a window. If set to 'true' window can be dragged like on a PC.
 * @param win pointer to a window object
 * @param en whether dragging is enabled
 */
void edgui_win_set_drag(edgui_obj_t * win, bool en)
{
    edgui_win_ext_t * ext    = edgui_obj_get_ext_attr(win);
    edgui_obj_t * win_header = ext->header;
    edgui_obj_set_drag_parent(win_header, en);
    edgui_obj_set_drag(win, en);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the title of a window
 * @param win pointer to a window object
 * @return title string of the window
 */
const char * edgui_win_get_title(const edgui_obj_t * win)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);
    return edgui_label_get_text(ext->title);
}

/**
 * Get the content holder object of window (`edgui_page`) to allow additional customization
 * @param win pointer to a window object
 * @return the Page object where the window's content is
 */
edgui_obj_t * edgui_win_get_content(const edgui_obj_t * win)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);
    return ext->page;
}

/**
 * Get the control button size of a window
 * @param win pointer to a window object
 * @return control button size
 */
edgui_coord_t edgui_win_get_btn_size(const edgui_obj_t * win)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);
    return ext->btn_size;
}

/**
 * Get the pointer of a widow from one of  its control button.
 * It is useful in the action of the control buttons where only button is known.
 * @param ctrl_btn pointer to a control button of a window
 * @return pointer to the window of 'ctrl_btn'
 */
edgui_obj_t * edgui_win_get_from_btn(const edgui_obj_t * ctrl_btn)
{
    edgui_obj_t * header = edgui_obj_get_parent(ctrl_btn);
    edgui_obj_t * win    = edgui_obj_get_parent(header);

    return win;
}

/**
 * Get the layout of a window
 * @param win pointer to a window object
 * @return the layout of the window (from 'edgui_layout_t')
 */
edgui_layout_t edgui_win_get_layout(edgui_obj_t * win)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);
    return edgui_page_get_scrl_layout(ext->page);
}

/**
 * Get the scroll bar mode of a window
 * @param win pointer to a window object
 * @return the scroll bar mode of the window (from 'edgui_sb_mode_t')
 */
edgui_sb_mode_t edgui_win_get_sb_mode(edgui_obj_t * win)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);
    return edgui_page_get_sb_mode(ext->page);
}

/**
 * Get focus animation duration
 * @param win pointer to a window object
 * @return duration of animation [ms]
 */
uint16_t edgui_win_get_anim_time(const edgui_obj_t * win)
{
    return edgui_page_get_anim_time(edgui_win_get_content(win));
}

/**
 * Get width of the content area (page scrollable) of the window
 * @param win pointer to a window object
 * @return the width of the content_bg area
 */
edgui_coord_t edgui_win_get_width(edgui_obj_t * win)
{
    edgui_win_ext_t * ext            = edgui_obj_get_ext_attr(win);
    edgui_obj_t * scrl               = edgui_page_get_scrl(ext->page);
    const edgui_style_t * style_scrl = edgui_obj_get_style(scrl);

    return edgui_obj_get_width(scrl) - style_scrl->body.padding.left - style_scrl->body.padding.right;
}

/**
 * Get a style of a window
 * @param win pointer to a button object
 * @param type which style window be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_win_get_style(const edgui_obj_t * win, edgui_win_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_win_ext_t * ext       = edgui_obj_get_ext_attr(win);

    switch(type) {
        case EDGUI_WIN_STYLE_BG: style = edgui_obj_get_style(win); break;
        case EDGUI_WIN_STYLE_CONTENT: style = edgui_page_get_style(ext->page, EDGUI_PAGE_STYLE_SCRL); break;
        case EDGUI_WIN_STYLE_SB: style = edgui_page_get_style(ext->page, EDGUI_PAGE_STYLE_SB); break;
        case EDGUI_WIN_STYLE_HEADER: style = edgui_obj_get_style(ext->header); break;
        case EDGUI_WIN_STYLE_BTN_REL: style = ext->style_btn_rel; break;
        case EDGUI_WIN_STYLE_BTN_PR: style = ext->style_btn_pr; break;
        default: style = NULL; break;
    }

    return style;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Focus on an object. It ensures that the object will be visible in the window.
 * @param win pointer to a window object
 * @param obj pointer to an object to focus (must be in the window)
 * @param anim_en EDGUI_ANIM_ON focus with an animation; EDGUI_ANIM_OFF focus without animation
 */
void edgui_win_focus(edgui_obj_t * win, edgui_obj_t * obj, edgui_anim_enable_t anim_en)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);
    edgui_page_focus(ext->page, obj, anim_en);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the window
 * @param win pointer to a window object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_win_signal(edgui_obj_t * win, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(win, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);
    if(sign == EDGUI_SIGNAL_CHILD_CHG) { /*Move children to the page*/
        edgui_obj_t * page = ext->page;
        if(page != NULL) {
            edgui_obj_t * child;
            child = edgui_obj_get_child(win, NULL);
            while(child != NULL) {
                if(edgui_obj_is_protected(child, EDGUI_PROTECT_PARENT) == false) {
                    edgui_obj_t * tmp = child;
                    child          = edgui_obj_get_child(win, child); /*Get the next child before move this*/
                    edgui_obj_set_parent(tmp, page);
                } else {
                    child = edgui_obj_get_child(win, child);
                }
            }
        }
    } else if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        edgui_win_realign(win);
    } else if(sign == EDGUI_SIGNAL_CORD_CHG) {
        /*If the size is changed refresh the window*/
        if(edgui_area_get_width(param) != edgui_obj_get_width(win) || edgui_area_get_height(param) != edgui_obj_get_height(win)) {
            edgui_win_realign(win);
        }
    } else if(sign == EDGUI_SIGNAL_CLEANUP) {
        ext->header = NULL; /*These objects were children so they are already invalid*/
        ext->page   = NULL;
        ext->title  = NULL;
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        /*Forward all the control signals to the page*/
        ext->page->signal_cb(ext->page, sign, param);
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_win";
    }

    return res;
}

/**
 * Realign the building elements of a window
 * @param win pointer to window objectker
 */
static void edgui_win_realign(edgui_obj_t * win)
{
    edgui_win_ext_t * ext = edgui_obj_get_ext_attr(win);

    if(ext->page == NULL || ext->header == NULL || ext->title == NULL) return;

    const edgui_style_t * header_style = edgui_win_get_style(win, EDGUI_WIN_STYLE_HEADER);
    edgui_obj_set_size(ext->header, edgui_obj_get_width(win),
                    ext->btn_size + header_style->body.padding.top + header_style->body.padding.bottom);

    bool first_btn = true;
    edgui_obj_t * btn;
    edgui_obj_t * btn_prev = NULL;
    /*Refresh the size of all control buttons*/
    btn = edgui_obj_get_child_back(ext->header, NULL);
    btn = edgui_obj_get_child_back(ext->header, btn); /*Skip the title*/
    while(btn != NULL) {
        edgui_obj_set_size(btn, ext->btn_size, ext->btn_size);
        if(first_btn) {
            edgui_obj_align(btn, ext->header, EDGUI_ALIGN_IN_RIGHT_MID, -header_style->body.padding.right, 0);
            first_btn = false;
        } else {
            edgui_obj_align(btn, btn_prev, EDGUI_ALIGN_OUT_LEFT_MID, -header_style->body.padding.inner, 0);
        }
        btn_prev = btn;
        btn      = edgui_obj_get_child_back(ext->header, btn);
    }

    const edgui_style_t * style_header = edgui_win_get_style(win, EDGUI_WIN_STYLE_HEADER);
    edgui_obj_align(ext->title, NULL, EDGUI_ALIGN_IN_LEFT_MID, style_header->body.padding.left, 0);

    edgui_obj_set_pos(ext->header, 0, 0);

    edgui_obj_set_size(ext->page, edgui_obj_get_width(win), edgui_obj_get_height(win) - edgui_obj_get_height(ext->header));
    edgui_obj_align(ext->page, ext->header, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
}

#endif
