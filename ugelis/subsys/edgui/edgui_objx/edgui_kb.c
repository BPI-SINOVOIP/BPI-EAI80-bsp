
/**
 * @file edgui_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_kb.h"
#if EDGUI_USE_KB != 0

#include "edgui_ta.h"
#include "../edgui_themes/edgui_theme.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_KB_CTRL_BTN_FLAGS (EDGUI_BTNM_CTRL_NO_REPEAT | EDGUI_BTNM_CTRL_CLICK_TRIG)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_kb_signal(edgui_obj_t * kb, edgui_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;
/* clang-format off */
static const char * kb_map_lc[] = {"1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "Bksp", "\n",
                                   "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", "Enter", "\n",
                                   "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                   EDGUI_SYMBOL_CLOSE, EDGUI_SYMBOL_LEFT, " ", EDGUI_SYMBOL_RIGHT, EDGUI_SYMBOL_OK, ""};

static const edgui_btnm_ctrl_t kb_ctrl_lc_map[] = {
    EDGUI_KB_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    EDGUI_KB_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    EDGUI_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, EDGUI_KB_CTRL_BTN_FLAGS | 2};

static const char * kb_map_uc[] = {"1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "Bksp", "\n",
                                   "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", "Enter", "\n",
                                   "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
                                   EDGUI_SYMBOL_CLOSE, EDGUI_SYMBOL_LEFT, " ", EDGUI_SYMBOL_RIGHT, EDGUI_SYMBOL_OK, ""};

static const edgui_btnm_ctrl_t kb_ctrl_uc_map[] = {
    EDGUI_KB_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    EDGUI_KB_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    EDGUI_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, EDGUI_KB_CTRL_BTN_FLAGS | 2};

static const char * kb_map_spec[] = {"0", "1", "2", "3", "4" ,"5", "6", "7", "8", "9", "Bksp", "\n",
                                     "abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
                                     "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
                                     EDGUI_SYMBOL_CLOSE, EDGUI_SYMBOL_LEFT, " ", EDGUI_SYMBOL_RIGHT, EDGUI_SYMBOL_OK, ""};

static const edgui_btnm_ctrl_t kb_ctrl_spec_map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, EDGUI_KB_CTRL_BTN_FLAGS | 2,
    EDGUI_KB_CTRL_BTN_FLAGS | 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    EDGUI_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, EDGUI_KB_CTRL_BTN_FLAGS | 2};

static const char * kb_map_num[] = {"1", "2", "3", EDGUI_SYMBOL_CLOSE, "\n",
                                    "4", "5", "6", EDGUI_SYMBOL_OK, "\n",
                                    "7", "8", "9", "Bksp", "\n",
                                    "+/-", "0", ".", EDGUI_SYMBOL_LEFT, EDGUI_SYMBOL_RIGHT, ""};

static const edgui_btnm_ctrl_t kb_ctrl_num_map[] = {
        1, 1, 1, EDGUI_KB_CTRL_BTN_FLAGS | 2,
        1, 1, 1, EDGUI_KB_CTRL_BTN_FLAGS | 2,
        1, 1, 1, 2,
        1, 1, 1, 1, 1};
/* clang-format on */

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a keyboard objects
 * @param par pointer to an object, it will be the parent of the new keyboard
 * @param copy pointer to a keyboard object, if not NULL then the new object will be copied from it
 * @return pointer to the created keyboard
 */
edgui_obj_t * edgui_kb_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("keyboard create started");

    /*Create the ancestor of keyboard*/
    edgui_obj_t * new_kb = edgui_btnm_create(par, copy);
    edgui_mem_assert(new_kb);
    if(new_kb == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_kb);

    /*Allocate the keyboard type specific extended data*/
    edgui_kb_ext_t * ext = edgui_obj_allocate_ext_attr(new_kb, sizeof(edgui_kb_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    /*Initialize the allocated 'ext' */

    ext->ta         = NULL;
    ext->mode       = EDGUI_KB_MODE_TEXT;
    ext->cursor_mng = 0;

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_kb, edgui_kb_signal);

    /*Init the new keyboard keyboard*/
    if(copy == NULL) {
        /* Set a size which fits into the parent.
         * Don't use `par` directly because if the window is created on a page it is moved to the
         * scrollable so the parent has changed */
        edgui_obj_set_size(new_kb, edgui_obj_get_width_fit(edgui_obj_get_parent(new_kb)),
                        edgui_obj_get_height_fit(edgui_obj_get_parent(new_kb)) / 2);

        edgui_obj_align(new_kb, NULL, EDGUI_ALIGN_IN_BOTTOM_MID, 0, 0);
        edgui_obj_set_event_cb(new_kb, edgui_kb_def_event_cb);
        edgui_btnm_set_map(new_kb, kb_map_lc);
        edgui_btnm_set_ctrl_map(new_kb, kb_ctrl_lc_map);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_kb_set_style(new_kb, EDGUI_KB_STYLE_BG, th->style.kb.bg);
            edgui_kb_set_style(new_kb, EDGUI_KB_STYLE_BTN_REL, th->style.kb.btn.rel);
            edgui_kb_set_style(new_kb, EDGUI_KB_STYLE_BTN_PR, th->style.kb.btn.pr);
            edgui_kb_set_style(new_kb, EDGUI_KB_STYLE_BTN_TGL_REL, th->style.kb.btn.tgl_rel);
            edgui_kb_set_style(new_kb, EDGUI_KB_STYLE_BTN_TGL_PR, th->style.kb.btn.tgl_pr);
            edgui_kb_set_style(new_kb, EDGUI_KB_STYLE_BTN_INA, th->style.kb.btn.ina);
        } else {
            /*Let the button matrix's styles*/
        }
    }
    /*Copy an existing keyboard*/
    else {
        edgui_kb_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->ta                = NULL;
        ext->ta                = copy_ext->ta;
        ext->mode              = copy_ext->mode;
        ext->cursor_mng        = copy_ext->cursor_mng;

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_kb);
    }

    EDGUI_LOG_INFO("keyboard created");

    return new_kb;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @param ta pointer to a Text Area object to write there
 */
void edgui_kb_set_ta(edgui_obj_t * kb, edgui_obj_t * ta)
{
    edgui_kb_ext_t * ext = edgui_obj_get_ext_attr(kb);
    edgui_cursor_type_t cur_type;

    /*Hide the cursor of the old Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        cur_type = edgui_ta_get_cursor_type(ext->ta);
        edgui_ta_set_cursor_type(ext->ta, cur_type | EDGUI_CURSOR_HIDDEN);
    }

    ext->ta = ta;

    /*Show the cursor of the new Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        cur_type = edgui_ta_get_cursor_type(ext->ta);
        edgui_ta_set_cursor_type(ext->ta, cur_type & (~EDGUI_CURSOR_HIDDEN));
    }
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @param mode the mode from 'edgui_kb_mode_t'
 */
void edgui_kb_set_mode(edgui_obj_t * kb, edgui_kb_mode_t mode)
{
    edgui_kb_ext_t * ext = edgui_obj_get_ext_attr(kb);
    if(ext->mode == mode) return;

    ext->mode = mode;
    if(mode == EDGUI_KB_MODE_TEXT) {
        edgui_btnm_set_map(kb, kb_map_lc);
        edgui_btnm_set_ctrl_map(kb, kb_ctrl_lc_map);
    } else if(mode == EDGUI_KB_MODE_NUM) {
        edgui_btnm_set_map(kb, kb_map_num);
        edgui_btnm_set_ctrl_map(kb, kb_ctrl_num_map);
    }
}

/**
 * Automatically hide or show the cursor of Text Area
 * @param kb pointer to a Keyboard object
 * @param en true: show cursor on the current text area, false: hide cursor
 */
void edgui_kb_set_cursor_manage(edgui_obj_t * kb, bool en)
{
    edgui_kb_ext_t * ext = edgui_obj_get_ext_attr(kb);
    if(ext->cursor_mng == en) return;

    ext->cursor_mng = en == false ? 0 : 1;

    if(ext->ta) {
        edgui_cursor_type_t cur_type;
        cur_type = edgui_ta_get_cursor_type(ext->ta);

        if(ext->cursor_mng) {
            edgui_ta_set_cursor_type(ext->ta, cur_type & (~EDGUI_CURSOR_HIDDEN));
        } else {
            edgui_ta_set_cursor_type(ext->ta, cur_type | EDGUI_CURSOR_HIDDEN);
        }
    }
}

/**
 * Set a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_kb_set_style(edgui_obj_t * kb, edgui_kb_style_t type, const edgui_style_t * style)
{
    switch(type) {
        case EDGUI_KB_STYLE_BG: edgui_btnm_set_style(kb, EDGUI_BTNM_STYLE_BG, style); break;
        case EDGUI_KB_STYLE_BTN_REL: edgui_btnm_set_style(kb, EDGUI_BTNM_STYLE_BTN_REL, style); break;
        case EDGUI_KB_STYLE_BTN_PR: edgui_btnm_set_style(kb, EDGUI_BTNM_STYLE_BTN_PR, style); break;
        case EDGUI_KB_STYLE_BTN_TGL_REL: edgui_btnm_set_style(kb, EDGUI_BTNM_STYLE_BTN_TGL_REL, style); break;
        case EDGUI_KB_STYLE_BTN_TGL_PR: edgui_btnm_set_style(kb, EDGUI_BTNM_STYLE_BTN_TGL_PR, style); break;
        case EDGUI_KB_STYLE_BTN_INA: edgui_btnm_set_style(kb, EDGUI_BTNM_STYLE_BTN_INA, style); break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
edgui_obj_t * edgui_kb_get_ta(const edgui_obj_t * kb)
{
    edgui_kb_ext_t * ext = edgui_obj_get_ext_attr(kb);
    return ext->ta;
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'edgui_kb_mode_t'
 */
edgui_kb_mode_t edgui_kb_get_mode(const edgui_obj_t * kb)
{
    edgui_kb_ext_t * ext = edgui_obj_get_ext_attr(kb);
    return ext->mode;
}

/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool edgui_kb_get_cursor_manage(const edgui_obj_t * kb)
{
    edgui_kb_ext_t * ext = edgui_obj_get_ext_attr(kb);
    return ext->cursor_mng == 0 ? false : true;
}

/**
 * Get a style of a keyboard
 * @param kb pointer to a keyboard object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_kb_get_style(const edgui_obj_t * kb, edgui_kb_style_t type)
{
    const edgui_style_t * style = NULL;

    switch(type) {
        case EDGUI_KB_STYLE_BG: style = edgui_btnm_get_style(kb, EDGUI_BTNM_STYLE_BG); break;
        case EDGUI_KB_STYLE_BTN_REL: style = edgui_btnm_get_style(kb, EDGUI_BTNM_STYLE_BTN_REL); break;
        case EDGUI_KB_STYLE_BTN_PR: style = edgui_btnm_get_style(kb, EDGUI_BTNM_STYLE_BTN_PR); break;
        case EDGUI_KB_STYLE_BTN_TGL_REL: style = edgui_btnm_get_style(kb, EDGUI_BTNM_STYLE_BTN_TGL_REL); break;
        case EDGUI_KB_STYLE_BTN_TGL_PR: style = edgui_btnm_get_style(kb, EDGUI_BTNM_STYLE_BTN_TGL_PR); break;
        case EDGUI_KB_STYLE_BTN_INA: style = edgui_btnm_get_style(kb, EDGUI_BTNM_STYLE_BTN_INA); break;
        default: style = NULL; break;
    }

    return style;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Default keyboard event to add characters to the Text area and change the map.
 * If a custom `event_cb` is added to the keyboard this function be called from it to handle the
 * button clicks
 * @param kb pointer to a  keyboard
 * @param event the triggering event
 */
void edgui_kb_def_event_cb(edgui_obj_t * kb, edgui_event_t event)
{
    if(event != EDGUI_EVENT_VALUE_CHANGED) return;

    edgui_kb_ext_t * ext = edgui_obj_get_ext_attr(kb);
    uint16_t btn_id   = edgui_btnm_get_active_btn(kb);
    if(btn_id == EDGUI_BTNM_BTN_NONE) return;
    if(edgui_btnm_get_btn_ctrl(kb, btn_id, EDGUI_BTNM_CTRL_HIDDEN | EDGUI_BTNM_CTRL_INACTIVE)) return;
    if(edgui_btnm_get_btn_ctrl(kb, btn_id, EDGUI_BTNM_CTRL_NO_REPEAT) && event == EDGUI_EVENT_LONG_PRESSED_REPEAT) return;

    const char * txt = edgui_btnm_get_active_btn_text(kb);
    if(txt == NULL) return;

    /*Do the corresponding action according to the text of the button*/
    if(strcmp(txt, "abc") == 0) {
        edgui_btnm_set_map(kb, kb_map_lc);
        edgui_btnm_set_ctrl_map(kb, kb_ctrl_lc_map);
        return;
    } else if(strcmp(txt, "ABC") == 0) {
        edgui_btnm_set_map(kb, kb_map_uc);
        edgui_btnm_set_ctrl_map(kb, kb_ctrl_uc_map);
        return;
    } else if(strcmp(txt, "1#") == 0) {
        edgui_btnm_set_map(kb, kb_map_spec);
        edgui_btnm_set_ctrl_map(kb, kb_ctrl_spec_map);
        return;
    } else if(strcmp(txt, EDGUI_SYMBOL_CLOSE) == 0) {
        if(kb->event_cb != edgui_kb_def_event_cb) {
            edgui_res_t res = edgui_event_send(kb, EDGUI_EVENT_CANCEL, NULL);
            if(res != EDGUI_RES_OK) return;
        } else {
            edgui_kb_set_ta(kb, NULL); /*De-assign the text area  to hide it cursor if needed*/
            edgui_obj_del(kb);
            return;
        }
        return;
    } else if(strcmp(txt, EDGUI_SYMBOL_OK) == 0) {
        if(kb->event_cb != edgui_kb_def_event_cb) {
            edgui_res_t res = edgui_event_send(kb, EDGUI_EVENT_APPLY, NULL);
            if(res != EDGUI_RES_OK) return;
        } else {
            edgui_kb_set_ta(kb, NULL); /*De-assign the text area to hide it cursor if needed*/
        }
        return;
    }

    /*Add the characters to the text area if set*/
    if(ext->ta == NULL) return;

    if(strcmp(txt, "Enter") == 0)
        edgui_ta_add_char(ext->ta, '\n');
    else if(strcmp(txt, EDGUI_SYMBOL_LEFT) == 0)
        edgui_ta_cursor_left(ext->ta);
    else if(strcmp(txt, EDGUI_SYMBOL_RIGHT) == 0)
        edgui_ta_cursor_right(ext->ta);
    else if(strcmp(txt, "Bksp") == 0)
        edgui_ta_del_char(ext->ta);
    else if(strcmp(txt, "+/-") == 0) {
        uint16_t cur        = edgui_ta_get_cursor_pos(ext->ta);
        const char * ta_txt = edgui_ta_get_text(ext->ta);
        if(ta_txt[0] == '-') {
            edgui_ta_set_cursor_pos(ext->ta, 1);
            edgui_ta_del_char(ext->ta);
            edgui_ta_add_char(ext->ta, '+');
            edgui_ta_set_cursor_pos(ext->ta, cur);
        } else if(ta_txt[0] == '+') {
            edgui_ta_set_cursor_pos(ext->ta, 1);
            edgui_ta_del_char(ext->ta);
            edgui_ta_add_char(ext->ta, '-');
            edgui_ta_set_cursor_pos(ext->ta, cur);
        } else {
            edgui_ta_set_cursor_pos(ext->ta, 0);
            edgui_ta_add_char(ext->ta, '-');
            edgui_ta_set_cursor_pos(ext->ta, cur + 1);
        }
    } else {
        edgui_ta_add_text(ext->ta, txt);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the keyboard
 * @param kb pointer to a keyboard object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_kb_signal(edgui_obj_t * kb, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(kb, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == EDGUI_SIGNAL_FOCUS) {
        edgui_kb_ext_t * ext = edgui_obj_get_ext_attr(kb);
        /*Show the cursor of the new Text area if cursor management is enabled*/
        if(ext->ta && ext->cursor_mng) {
            edgui_cursor_type_t cur_type = edgui_ta_get_cursor_type(ext->ta);
            edgui_ta_set_cursor_type(ext->ta, cur_type & (~EDGUI_CURSOR_HIDDEN));
        }
    } else if(sign == EDGUI_SIGNAL_DEFOCUS) {
        edgui_kb_ext_t * ext = edgui_obj_get_ext_attr(kb);
        /*Show the cursor of the new Text area if cursor management is enabled*/
        if(ext->ta && ext->cursor_mng) {
            edgui_cursor_type_t cur_type = edgui_ta_get_cursor_type(ext->ta);
            edgui_ta_set_cursor_type(ext->ta, cur_type | EDGUI_CURSOR_HIDDEN);
        }
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_kb";
    }

    return res;
}

#endif
