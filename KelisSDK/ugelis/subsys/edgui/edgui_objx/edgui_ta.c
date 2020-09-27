/**
 * @file edgui_ta.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_ta.h"
#if EDGUI_USE_TA != 0
#include <string.h>
#include "../edgui_core/edgui_group.h"
#include "../edgui_core/edgui_refr.h"
#include "../edgui_draw/edgui_draw.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_anim.h"
#include "../edgui_misc/edgui_txt.h"
#include "../edgui_misc/edgui_math.h"

/*********************
 *      DEFINES
 *********************/
/*Test configuration*/

#ifndef EDGUI_TA_DEF_CURSOR_BLINK_TIME
#define EDGUI_TA_DEF_CURSOR_BLINK_TIME 400 /*ms*/
#endif

#ifndef EDGUI_TA_DEF_PWD_SHOW_TIME
#define EDGUI_TA_DEF_PWD_SHOW_TIME 1500 /*ms*/
#endif

#define EDGUI_TA_DEF_WIDTH (2 * EDGUI_DPI)
#define EDGUI_TA_DEF_HEIGHT (1 * EDGUI_DPI)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_ta_design(edgui_obj_t * ta, const edgui_area_t * mask, edgui_design_mode_t mode);
static bool edgui_ta_scrollable_design(edgui_obj_t * scrl, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_ta_signal(edgui_obj_t * ta, edgui_signal_t sign, void * param);
static edgui_res_t edgui_ta_scrollable_signal(edgui_obj_t * scrl, edgui_signal_t sign, void * param);
#if EDGUI_USE_ANIMATION
static void cursor_blink_anim(edgui_obj_t * ta, edgui_anim_value_t show);
static void pwd_char_hider_anim(edgui_obj_t * ta, edgui_anim_value_t x);
static void pwd_char_hider_anim_ready(edgui_anim_t * a);
#endif
static void pwd_char_hider(edgui_obj_t * ta);
static bool char_is_accepted(edgui_obj_t * ta, uint32_t c);
static void get_cursor_style(edgui_obj_t * ta, edgui_style_t * style_res);
static void refr_cursor_area(edgui_obj_t * ta);
static void placeholder_update(edgui_obj_t * ta);
static void update_cursor_position_on_click(edgui_obj_t * ta, edgui_signal_t sign, edgui_indev_t * click_source);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_design_cb_t ancestor_design;
static edgui_design_cb_t scrl_design;
static edgui_signal_cb_t ancestor_signal;
static edgui_signal_cb_t scrl_signal;
static const char * ta_insert_replace;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a text area objects
 * @param par pointer to an object, it will be the parent of the new text area
 * @param copy pointer to a text area object, if not NULL then the new object will be copied from it
 * @return pointer to the created text area
 */
edgui_obj_t * edgui_ta_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("text area create started");

    /*Create the ancestor object*/
    edgui_obj_t * new_ta = edgui_page_create(par, copy);
    edgui_mem_assert(new_ta);
    if(new_ta == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_ta);
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_ta);
    if(scrl_signal == NULL) scrl_signal = edgui_obj_get_signal_cb(edgui_page_get_scrl(new_ta));
    if(scrl_design == NULL) scrl_design = edgui_obj_get_design_cb(edgui_page_get_scrl(new_ta));

    /*Allocate the object type specific extended data*/
    edgui_ta_ext_t * ext = edgui_obj_allocate_ext_attr(new_ta, sizeof(edgui_ta_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->cursor.state      = 1;
    ext->pwd_mode          = 0;
    ext->pwd_tmp           = NULL;
    ext->pwd_show_time     = EDGUI_TA_DEF_PWD_SHOW_TIME;
    ext->accapted_chars    = NULL;
    ext->max_length        = 0;
    ext->cursor.style      = NULL;
    ext->cursor.blink_time = EDGUI_TA_DEF_CURSOR_BLINK_TIME;
    ext->cursor.pos        = 0;
    ext->cursor.click_pos  = 1;
    ext->cursor.type       = EDGUI_CURSOR_LINE;
    ext->cursor.valid_x    = 0;
    ext->one_line          = 0;
#if EDGUI_LABEL_TEXT_SEL
    ext->text_sel_en = 0;
#endif
    ext->label       = NULL;
    ext->placeholder = NULL;

#if EDGUI_USE_ANIMATION == 0
    ext->pwd_show_time     = 0;
    ext->cursor.blink_time = 0;
#endif

    edgui_obj_set_signal_cb(new_ta, edgui_ta_signal);
    edgui_obj_set_signal_cb(edgui_page_get_scrl(new_ta), edgui_ta_scrollable_signal);
    edgui_obj_set_design_cb(new_ta, edgui_ta_design);

    /*Init the new text area object*/
    if(copy == NULL) {
        edgui_page_set_scrl_fit2(new_ta, EDGUI_FIT_FLOOD, EDGUI_FIT_TIGHT);

        ext->label = edgui_label_create(new_ta, NULL);

        edgui_obj_set_design_cb(ext->page.scrl, edgui_ta_scrollable_design);

        edgui_label_set_long_mode(ext->label, EDGUI_LABEL_LONG_BREAK);
        edgui_label_set_text(ext->label, "Text area");
        edgui_obj_set_click(ext->label, false);
        edgui_obj_set_size(new_ta, EDGUI_TA_DEF_WIDTH, EDGUI_TA_DEF_HEIGHT);
        edgui_ta_set_sb_mode(new_ta, EDGUI_SB_MODE_DRAG);
        edgui_page_set_style(new_ta, EDGUI_PAGE_STYLE_SCRL, &edgui_style_transp_tight);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_ta_set_style(new_ta, EDGUI_TA_STYLE_BG, th->style.ta.area);
            edgui_ta_set_style(new_ta, EDGUI_TA_STYLE_SB, th->style.ta.sb);
        } else {
            edgui_ta_set_style(new_ta, EDGUI_TA_STYLE_BG, &edgui_style_pretty);
        }
    }
    /*Copy an existing object*/
    else {
        edgui_obj_set_design_cb(ext->page.scrl, edgui_ta_scrollable_design);
        edgui_ta_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->label             = edgui_label_create(new_ta, copy_ext->label);
        ext->pwd_mode          = copy_ext->pwd_mode;
        ext->accapted_chars    = copy_ext->accapted_chars;
        ext->max_length        = copy_ext->max_length;
        ext->cursor.style      = copy_ext->cursor.style;
        ext->cursor.pos        = copy_ext->cursor.pos;
        ext->cursor.valid_x    = copy_ext->cursor.valid_x;
        ext->cursor.type       = copy_ext->cursor.type;

        if(ext->pwd_mode != 0) pwd_char_hider( new_ta);

        if(copy_ext->placeholder != NULL)
            ext->placeholder = edgui_label_create(new_ta, copy_ext->placeholder);
        else
            ext->placeholder = NULL;

        if(copy_ext->pwd_tmp) {
            uint16_t len = edgui_mem_get_size(copy_ext->pwd_tmp);
            ext->pwd_tmp = edgui_mem_alloc(len);
            edgui_mem_assert(ext->pwd_tmp);
            if(ext->pwd_tmp == NULL) return NULL;

            memcpy(ext->pwd_tmp, copy_ext->pwd_tmp, len);
        }

        if(copy_ext->one_line) edgui_ta_set_one_line(new_ta, true);

        edgui_ta_set_style(new_ta, EDGUI_TA_STYLE_CURSOR, edgui_ta_get_style(copy, EDGUI_TA_STYLE_CURSOR));

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_ta);
    }

#if EDGUI_USE_ANIMATION
    if(ext->cursor.blink_time) {
        /*Create a cursor blinker animation*/
        edgui_anim_t a;
        a.var            = new_ta;
        a.exec_cb        = (edgui_anim_exec_xcb_t)cursor_blink_anim;
        a.time           = ext->cursor.blink_time;
        a.act_time       = 0;
        a.ready_cb       = NULL;
        a.start          = 1;
        a.end            = 0;
        a.repeat         = 1;
        a.repeat_pause   = 0;
        a.playback       = 1;
        a.playback_pause = 0;
        a.path_cb        = edgui_anim_path_step;
        edgui_anim_create(&a);
    }
#endif

    EDGUI_LOG_INFO("text area created");

    return new_ta;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Insert a character to the current cursor position.
 * To add a wide char, e.g. 'Á' use `edgui_txt_encoded_conv_wc('Á')`
 * @param ta pointer to a text area object
 * @param c a character (e.g. 'a')
 */
void edgui_ta_add_char(edgui_obj_t * ta, uint32_t c)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    uint32_t letter_buf[2];
    letter_buf[0] = c;
    letter_buf[1] = '\0';

    ta_insert_replace = NULL;
    edgui_event_send(ta, EDGUI_EVENT_INSERT, letter_buf);
    if(ta_insert_replace) {
        if(ta_insert_replace[0] == '\0') return; /*Drop this text*/

        /*Add the replaced text directly it's different from the original*/
        if(strcmp(ta_insert_replace, (char *)letter_buf)) {
            edgui_ta_add_text(ta, ta_insert_replace);
            return;
        }
    }

    if(ext->one_line && (c == '\n' || c == '\r')) {
        EDGUI_LOG_INFO("Text area: line break ignored in one-line mode");
        return;
    }

    uint32_t c_uni = edgui_txt_encoded_next((const char *)&c, NULL);

    if(char_is_accepted(ta, c_uni) == false) {
        EDGUI_LOG_INFO("Character is no accepted by the text area (too long text or not in the "
                    "accepted list)");
        return;
    }

    /*If a new line was added it shouldn't show edge flash effect*/
    bool edge_flash_en = edgui_ta_get_edge_flash(ta);
    edgui_ta_set_edge_flash(ta, false);

    if(ext->pwd_mode != 0) pwd_char_hider(ta); /*Make sure all the current text contains only '*'*/

    edgui_label_ins_text(ext->label, ext->cursor.pos, (const char *)letter_buf); /*Insert the character*/
    edgui_ta_clear_selection(ta);                                                /*Clear selection*/

    if(ext->pwd_mode != 0) {

        ext->pwd_tmp = edgui_mem_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + 2); /*+2: the new char + \0 */
        edgui_mem_assert(ext->pwd_tmp);
        if(ext->pwd_tmp == NULL) return;

        edgui_txt_ins(ext->pwd_tmp, ext->cursor.pos, (const char *)letter_buf);

#if EDGUI_USE_ANIMATION
        /*Auto hide characters*/
        edgui_anim_t a;
        a.var            = ta;
        a.exec_cb        = (edgui_anim_exec_xcb_t)pwd_char_hider_anim;
        a.time           = ext->pwd_show_time;
        a.act_time       = 0;
        a.ready_cb       = pwd_char_hider_anim_ready;
        a.start          = 0;
        a.end            = 1;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        a.playback       = 0;
        a.playback_pause = 0;
        a.path_cb        = edgui_anim_path_step;
        edgui_anim_create(&a);

#else
        pwd_char_hider(ta);
#endif
    }

    /*Move the cursor after the new character*/
    edgui_ta_set_cursor_pos(ta, edgui_ta_get_cursor_pos(ta) + 1);

    /*Revert the original edge flash state*/
    edgui_ta_set_edge_flash(ta, edge_flash_en);

    placeholder_update(ta);

    edgui_event_send(ta, EDGUI_EVENT_VALUE_CHANGED, NULL);
}

/**
 * Insert a text to the current cursor position
 * @param ta pointer to a text area object
 * @param txt a '\0' terminated string to insert
 */
void edgui_ta_add_text(edgui_obj_t * ta, const char * txt)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    ta_insert_replace = NULL;
    edgui_event_send(ta, EDGUI_EVENT_INSERT, txt);
    if(ta_insert_replace) {
        if(ta_insert_replace[0] == '\0') return; /*Drop this text*/

        /*Add the replaced text directly it's different from the original*/
        if(strcmp(ta_insert_replace, txt)) {
            edgui_ta_add_text(ta, ta_insert_replace);
            return;
        }
    }

    if(ext->pwd_mode != 0) pwd_char_hider(ta); /*Make sure all the current text contains only '*'*/

    /*Add the character one-by-one if not all characters are accepted or there is character limit.*/
    if(edgui_ta_get_accepted_chars(ta) || edgui_ta_get_max_length(ta)) {
        uint32_t i = 0;
        while(txt[i] != '\0') {
            uint32_t c = edgui_txt_encoded_next(txt, &i);
            edgui_ta_add_char(ta, edgui_txt_unicode_to_encoded(c));
        }
        return;
    }

    /*If a new line was added it shouldn't show edge flash effect*/
    bool edge_flash_en = edgui_ta_get_edge_flash(ta);
    edgui_ta_set_edge_flash(ta, false);

    /*Insert the text*/
    edgui_label_ins_text(ext->label, ext->cursor.pos, txt);
    edgui_ta_clear_selection(ta);

    if(ext->pwd_mode != 0) {
        ext->pwd_tmp = edgui_mem_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + strlen(txt) + 1);
        edgui_mem_assert(ext->pwd_tmp);
        if(ext->pwd_tmp == NULL) return;

        edgui_txt_ins(ext->pwd_tmp, ext->cursor.pos, txt);

#if EDGUI_USE_ANIMATION
        /*Auto hide characters*/
        edgui_anim_t a;
        a.var            = ta;
        a.exec_cb        = (edgui_anim_exec_xcb_t)pwd_char_hider_anim;
        a.time           = ext->pwd_show_time;
        a.act_time       = 0;
        a.ready_cb       = pwd_char_hider_anim_ready;
        a.start          = 0;
        a.end            = 1;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        a.playback       = 0;
        a.playback_pause = 0;
        a.path_cb        = edgui_anim_path_step;
        edgui_anim_create(&a);
#else
        pwd_char_hider(ta);
#endif
    }

    /*Move the cursor after the new text*/
    edgui_ta_set_cursor_pos(ta, edgui_ta_get_cursor_pos(ta) + edgui_txt_get_encoded_length(txt));

    /*Revert the original edge flash state*/
    edgui_ta_set_edge_flash(ta, edge_flash_en);

    placeholder_update(ta);

    edgui_event_send(ta, EDGUI_EVENT_VALUE_CHANGED, NULL);
}

/**
 * Delete a the left character from the current cursor position
 * @param ta pointer to a text area object
 */
void edgui_ta_del_char(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    uint16_t cur_pos  = ext->cursor.pos;

    if(cur_pos == 0) return;

    ta_insert_replace = NULL;
    char del_buf[2]   = {EDGUI_KEY_DEL, '\0'};
    edgui_event_send(ta, EDGUI_EVENT_INSERT, del_buf);
    if(ta_insert_replace) {
        if(ta_insert_replace[0] == '\0') return; /*Drop this text*/

        /*Add the replaced text directly it's different from the original*/
        if(strcmp(ta_insert_replace, del_buf)) {
            edgui_ta_add_text(ta, ta_insert_replace);
            return;
        }
    }

    char * label_txt = edgui_label_get_text(ext->label);
    /*Delete a character*/
    edgui_txt_cut(label_txt, ext->cursor.pos - 1, 1);
    /*Refresh the label*/
    edgui_label_set_text(ext->label, label_txt);
    edgui_ta_clear_selection(ta);

    /*Don't let 'width == 0' because cursor will not be visible*/
    if(edgui_obj_get_width(ext->label) == 0) {
        const edgui_style_t * style = edgui_obj_get_style(ext->label);
        edgui_obj_set_width(ext->label, style->line.width);
    }

    if(ext->pwd_mode != 0) {
        uint32_t byte_pos = edgui_txt_encoded_get_byte_id(ext->pwd_tmp, ext->cursor.pos - 1);
        edgui_txt_cut(ext->pwd_tmp, ext->cursor.pos - 1, edgui_txt_encoded_size(&label_txt[byte_pos]));

        ext->pwd_tmp = edgui_mem_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + 1);
        edgui_mem_assert(ext->pwd_tmp);
        if(ext->pwd_tmp == NULL) return;
    }

    /*Move the cursor to the place of the deleted character*/
    edgui_ta_set_cursor_pos(ta, ext->cursor.pos - 1);

    placeholder_update(ta);

    edgui_event_send(ta, EDGUI_EVENT_VALUE_CHANGED, NULL);
}

/**
 * Delete the right character from the current cursor position
 * @param ta pointer to a text area object
 */
void edgui_ta_del_char_forward(edgui_obj_t * ta)
{
    uint16_t cp = edgui_ta_get_cursor_pos(ta);
    edgui_ta_set_cursor_pos(ta, cp + 1);
    if(cp != edgui_ta_get_cursor_pos(ta)) edgui_ta_del_char(ta);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void edgui_ta_set_text(edgui_obj_t * ta, const char * txt)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    /*Clear the existing selection*/
    edgui_ta_clear_selection(ta);

    /*Add the character one-by-one if not all characters are accepted or there is character limit.*/
    if(edgui_ta_get_accepted_chars(ta) || edgui_ta_get_max_length(ta)) {
        edgui_label_set_text(ext->label, "");
        edgui_ta_set_cursor_pos(ta, EDGUI_TA_CURSOR_LAST);

        uint32_t i = 0;
        while(txt[i] != '\0') {
            uint32_t c = edgui_txt_encoded_next(txt, &i);
            edgui_ta_add_char(ta, edgui_txt_unicode_to_encoded(c));
        }
    } else {
        edgui_label_set_text(ext->label, txt);
        edgui_ta_set_cursor_pos(ta, EDGUI_TA_CURSOR_LAST);
    }

    /*Don't let 'width == 0' because the cursor will not be visible*/
    if(edgui_obj_get_width(ext->label) == 0) {
        const edgui_style_t * style = edgui_obj_get_style(ext->label);
        edgui_obj_set_width(ext->label, edgui_font_get_glyph_width(style->text.font, ' ', '\0'));
    }

    if(ext->pwd_mode != 0) {
        ext->pwd_tmp = edgui_mem_realloc(ext->pwd_tmp, strlen(txt) + 1);
        edgui_mem_assert(ext->pwd_tmp);
        if(ext->pwd_tmp == NULL) return;
        strcpy(ext->pwd_tmp, txt);

#if EDGUI_USE_ANIMATION
        /*Auto hide characters*/
        edgui_anim_t a;
        a.var            = ta;
        a.exec_cb        = (edgui_anim_exec_xcb_t)pwd_char_hider_anim;
        a.time           = ext->pwd_show_time;
        a.act_time       = 0;
        a.ready_cb       = pwd_char_hider_anim_ready;
        a.start          = 0;
        a.end            = 1;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        a.playback       = 0;
        a.playback_pause = 0;
        a.path_cb        = edgui_anim_path_step;
        edgui_anim_create(&a);
#else
        pwd_char_hider(ta);
#endif
    }

    placeholder_update(ta);

    edgui_event_send(ta, EDGUI_EVENT_VALUE_CHANGED, NULL);
}

/**
 * Set the placeholder text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void edgui_ta_set_placeholder_text(edgui_obj_t * ta, const char * txt)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    /*Create the placeholder label only when it is needed*/
    if(ext->placeholder == NULL) {
        ext->placeholder = edgui_label_create(ta, NULL);

        if(ext->one_line) {
            edgui_label_set_long_mode(ext->placeholder, EDGUI_LABEL_LONG_EXPAND);
        } else {
            edgui_label_set_long_mode(ext->placeholder, EDGUI_LABEL_LONG_BREAK);
        }
    }

    edgui_label_set_text(ext->placeholder, txt);

    /*Refresh the placeholder's align*/
    edgui_ta_set_text_align(ta, edgui_label_get_align(ext->label));

    placeholder_update(ta);
}

/**
 * Set the cursor position
 * @param obj pointer to a text area object
 * @param pos the new cursor position in character index
 *             < 0 : index from the end of the text
 *             EDGUI_TA_CURSOR_LAST: go after the last character
 */
void edgui_ta_set_cursor_pos(edgui_obj_t * ta, int16_t pos)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    if(ext->cursor.pos == pos) return;

    uint16_t len = edgui_txt_get_encoded_length(edgui_label_get_text(ext->label));

    if(pos < 0) pos = len + pos;

    if(pos > len || pos == EDGUI_TA_CURSOR_LAST) pos = len;

    ext->cursor.pos = pos;

    /*Position the label to make the cursor visible*/
    edgui_obj_t * label_par = edgui_obj_get_parent(ext->label);
    edgui_point_t cur_pos;
    const edgui_style_t * style = edgui_obj_get_style(ta);
    const edgui_font_t * font_p = style->text.font;
    edgui_area_t label_cords;
    edgui_area_t ta_cords;
    edgui_label_get_letter_pos(ext->label, pos, &cur_pos);
    edgui_obj_get_coords(ta, &ta_cords);
    edgui_obj_get_coords(ext->label, &label_cords);

    /*Check the top*/
    edgui_coord_t font_h = edgui_font_get_line_height(font_p);
    if(edgui_obj_get_y(label_par) + cur_pos.y < 0) {
        edgui_obj_set_y(label_par, -cur_pos.y + style->body.padding.top);
    }

    /*Check the bottom*/
    if(label_cords.y1 + cur_pos.y + font_h + style->body.padding.bottom > ta_cords.y2) {
        edgui_obj_set_y(label_par, -(cur_pos.y - edgui_obj_get_height(ta) + font_h + style->body.padding.top +
                                  style->body.padding.bottom));
    }
    /*Check the left (use the font_h as general unit)*/
    if(edgui_obj_get_x(label_par) + cur_pos.x < font_h) {
        edgui_obj_set_x(label_par, -cur_pos.x + font_h);
    }

    /*Check the right (use the font_h as general unit)*/
    if(label_cords.x1 + cur_pos.x + font_h + style->body.padding.right > ta_cords.x2) {
        edgui_obj_set_x(label_par, -(cur_pos.x - edgui_obj_get_width(ta) + font_h + style->body.padding.left +
                                  style->body.padding.right));
    }

    ext->cursor.valid_x = cur_pos.x;

#if EDGUI_USE_ANIMATION
    if(ext->cursor.blink_time) {
        /*Reset cursor blink animation*/
        edgui_anim_t a;
        a.var            = ta;
        a.exec_cb        = (edgui_anim_exec_xcb_t)cursor_blink_anim;
        a.time           = ext->cursor.blink_time;
        a.act_time       = 0;
        a.ready_cb       = NULL;
        a.start          = 1;
        a.end            = 0;
        a.repeat         = 1;
        a.repeat_pause   = 0;
        a.playback       = 1;
        a.playback_pause = 0;
        a.path_cb        = edgui_anim_path_step;
        edgui_anim_create(&a);
    }
#endif

    refr_cursor_area(ta);
}

/**
 * Set the cursor type.
 * @param ta pointer to a text area object
 * @param cur_type: element of 'edgui_ta_cursor_type_t'
 */
void edgui_ta_set_cursor_type(edgui_obj_t * ta, edgui_cursor_type_t cur_type)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    if(ext->cursor.type == cur_type) return;

    ext->cursor.type = cur_type;

    refr_cursor_area(ta);
}

/**
 * Enable/Disable the positioning of the the cursor by clicking the text on the text area.
 * @param ta pointer to a text area object
 * @param en true: enable click positions; false: disable
 */
void edgui_ta_set_cursor_click_pos(edgui_obj_t * ta, bool en)
{
    edgui_ta_ext_t * ext     = edgui_obj_get_ext_attr(ta);
    ext->cursor.click_pos = en ? 1 : 0;
}

/**
 * Enable/Disable password mode
 * @param ta pointer to a text area object
 * @param en true: enable, false: disable
 */
void edgui_ta_set_pwd_mode(edgui_obj_t * ta, bool en)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    if(ext->pwd_mode == en) return;

    /*Pwd mode is now enabled*/
    if(ext->pwd_mode == 0 && en != false) {
        char * txt   = edgui_label_get_text(ext->label);
        uint16_t len = strlen(txt);
        ext->pwd_tmp = edgui_mem_alloc(len + 1);
        edgui_mem_assert(ext->pwd_tmp);
        if(ext->pwd_tmp == NULL) return;

        strcpy(ext->pwd_tmp, txt);

        uint16_t i;
        uint16_t encoded_len = edgui_txt_get_encoded_length(txt); 
        for(i = 0; i < encoded_len; i++) {
            txt[i] = '*'; /*All char to '*'*/
        }
        txt[i] = '\0';

        edgui_ta_clear_selection(ta);

        edgui_label_set_text(ext->label, NULL);
    }
    /*Pwd mode is now disabled*/
    else if(ext->pwd_mode == 1 && en == false) {
        edgui_ta_clear_selection(ta);
        edgui_label_set_text(ext->label, ext->pwd_tmp);
        edgui_mem_free(ext->pwd_tmp);
        ext->pwd_tmp = NULL;
    }

    ext->pwd_mode = en == false ? 0 : 1;

    refr_cursor_area(ta);
}

/**
 * Configure the text area to one line or back to normal
 * @param ta pointer to a Text area object
 * @param en true: one line, false: normal
 */
void edgui_ta_set_one_line(edgui_obj_t * ta, bool en)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    if(ext->one_line == en) return;

    if(en) {
        const edgui_style_t * style_ta    = edgui_obj_get_style(ta);
        const edgui_style_t * style_scrl  = edgui_obj_get_style(edgui_page_get_scrl(ta));
        const edgui_style_t * style_label = edgui_obj_get_style(ext->label);
        edgui_coord_t font_h              = edgui_font_get_line_height(style_label->text.font);

        ext->one_line = 1;
        edgui_page_set_scrl_fit2(ta, EDGUI_FIT_TIGHT, EDGUI_FIT_FLOOD);
        edgui_obj_set_height(ta, font_h + style_ta->body.padding.top + style_ta->body.padding.bottom +
                                  style_scrl->body.padding.top + style_scrl->body.padding.bottom);
        edgui_label_set_long_mode(ext->label, EDGUI_LABEL_LONG_EXPAND);
        if(ext->placeholder) edgui_label_set_long_mode(ext->placeholder, EDGUI_LABEL_LONG_EXPAND);
        edgui_obj_set_pos(edgui_page_get_scrl(ta), style_ta->body.padding.left, style_ta->body.padding.top);
    } else {
        const edgui_style_t * style_ta = edgui_obj_get_style(ta);

        ext->one_line = 0;
        edgui_page_set_scrl_fit2(ta, EDGUI_FIT_FLOOD, EDGUI_FIT_TIGHT);
        edgui_label_set_long_mode(ext->label, EDGUI_LABEL_LONG_BREAK);
        if(ext->placeholder) edgui_label_set_long_mode(ext->placeholder, EDGUI_LABEL_LONG_BREAK);

        edgui_obj_set_height(ta, EDGUI_TA_DEF_HEIGHT);
        edgui_obj_set_pos(edgui_page_get_scrl(ta), style_ta->body.padding.left, style_ta->body.padding.top);
    }

    placeholder_update(ta);
    refr_cursor_area(ta);
}

/**
 * Set the alignment of the text area.
 * In one line mode the text can be scrolled only with `EDGUI_LABEL_ALIGN_LEFT`.
 * This function should be called if the size of text area changes.
 * @param ta pointer to a text are object
 * @param align the desired alignment from `edgui_label_align_t`. (EDGUI_LABEL_ALIGN_LEFT/CENTER/RIGHT)
 */
void edgui_ta_set_text_align(edgui_obj_t * ta, edgui_label_align_t align)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    edgui_obj_t * label  = edgui_ta_get_label(ta);
    if(!ext->one_line) {
        edgui_label_set_align(label, align);
        if(ext->placeholder) edgui_label_set_align(ext->placeholder, align);
    } else {
        /*Normal left align. Just let the text expand*/
        if(align == EDGUI_LABEL_ALIGN_LEFT) {
            edgui_label_set_long_mode(label, EDGUI_LABEL_LONG_EXPAND);
            edgui_page_set_scrl_fit2(ta, EDGUI_FIT_TIGHT, EDGUI_FIT_FLOOD);
            edgui_label_set_align(label, align);
            if(ext->placeholder) edgui_label_set_align(ext->placeholder, align);

        }
        /*Else use fix label width equal to the Text area width*/
        else {
            edgui_label_set_long_mode(label, EDGUI_LABEL_LONG_CROP);
            edgui_page_set_scrl_fit2(ta, EDGUI_FIT_FLOOD, EDGUI_FIT_FLOOD);
            edgui_label_set_align(label, align);
            if(ext->placeholder) edgui_label_set_align(ext->placeholder, align);

            edgui_obj_set_width(label, edgui_page_get_fit_width(ta));
        }
    }

    refr_cursor_area(ta);
}

/**
 * Set a list of characters. Only these characters will be accepted by the text area
 * @param ta pointer to  Text Area
 * @param list list of characters. Only the pointer is saved. E.g. "+-.,0123456789"
 */
void edgui_ta_set_accepted_chars(edgui_obj_t * ta, const char * list)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    ext->accapted_chars = list;
}

/**
 * Set max length of a Text Area.
 * @param ta pointer to  Text Area
 * @param num the maximal number of characters can be added (`edgui_ta_set_text` ignores it)
 */
void edgui_ta_set_max_length(edgui_obj_t * ta, uint16_t num)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    ext->max_length = num;
}

/**
 * In `EDGUI_EVENT_INSERT` the text which planned to be inserted can be replaced by an other text.
 * It can be used to add automatic formatting to the text area.
 * @param ta pointer to a text area.
 * @param txt pointer to a new string to insert. If `""` no text will be added.
 *            The variable must be live after the `event_cb` exists. (Should be `global` or
 * `static`)
 */
void edgui_ta_set_insert_replace(edgui_obj_t * ta, const char * txt)
{
    (void)ta; /*Unused*/
    ta_insert_replace = txt;
}

/**
 * Set a style of a text area
 * @param ta pointer to a text area object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_ta_set_style(edgui_obj_t * ta, edgui_ta_style_t type, const edgui_style_t * style)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    switch(type) {
        case EDGUI_TA_STYLE_BG: edgui_page_set_style(ta, EDGUI_PAGE_STYLE_BG, style); break;
        case EDGUI_TA_STYLE_SB: edgui_page_set_style(ta, EDGUI_PAGE_STYLE_SB, style); break;
        case EDGUI_TA_STYLE_EDGE_FLASH: edgui_page_set_style(ta, EDGUI_PAGE_STYLE_EDGE_FLASH, style); break;
        case EDGUI_TA_STYLE_CURSOR:
            ext->cursor.style = style;
            edgui_obj_refresh_ext_draw_pad(edgui_page_get_scrl(ta)); /*Refresh ext. size because of cursor drawing*/
            refr_cursor_area(ta);
            break;
        case EDGUI_TA_STYLE_PLACEHOLDER:
            if(ext->placeholder) edgui_label_set_style(ext->placeholder, EDGUI_LABEL_STYLE_MAIN, style);
            break;
    }
}

/**
 * Enable/disable selection mode.
 * @param ta pointer to a text area object
 * @param en true or false to enable/disable selection mode
 */
void edgui_ta_set_text_sel(edgui_obj_t * ta, bool en)
{
#if EDGUI_LABEL_TEXT_SEL
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    ext->text_sel_en = en;

    if(!en) edgui_ta_clear_selection(ta);
#else
    (void)ta; /*Unused*/
    (void)en; /*Unused*/
#endif
}

/**
 * Set how long show the password before changing it to '*'
 * @param ta pointer to Text area
 * @param time show time in milliseconds. 0: hide immediately.
 */
void edgui_ta_set_pwd_show_time(edgui_obj_t * ta, uint16_t time)
{
#if EDGUI_USE_ANIMATION == 0
    time = 0;
#endif

    edgui_ta_ext_t * ext  = edgui_obj_get_ext_attr(ta);
    ext->pwd_show_time = time;
}

/**
 * Set cursor blink animation time
 * @param ta pointer to Text area
 * @param time blink period. 0: disable blinking
 */
void edgui_ta_set_cursor_blink_time(edgui_obj_t * ta, uint16_t time)
{
#if EDGUI_USE_ANIMATION == 0
    time = 0;
#endif

    edgui_ta_ext_t * ext      = edgui_obj_get_ext_attr(ta);
    ext->cursor.blink_time = time;

#if EDGUI_USE_ANIMATION
    if(ext->cursor.blink_time) {
        /*Reset cursor blink animation*/
        edgui_anim_t a;
        a.var            = ta;
        a.exec_cb        = (edgui_anim_exec_xcb_t)cursor_blink_anim;
        a.time           = ext->cursor.blink_time;
        a.act_time       = 0;
        a.ready_cb       = NULL;
        a.start          = 1;
        a.end            = 0;
        a.repeat         = 1;
        a.repeat_pause   = 0;
        a.playback       = 1;
        a.playback_pause = 0;
        a.path_cb        = edgui_anim_path_step;
        edgui_anim_create(&a);
    } else {
        ext->cursor.state = 1;
    }
#else
    ext->cursor.state = 1;
#endif
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a text area. In password mode it gives the real text (not '*'s).
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * edgui_ta_get_text(const edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    const char * txt;
    if(ext->pwd_mode == 0) {
        txt = edgui_label_get_text(ext->label);
    } else {
        txt = ext->pwd_tmp;
    }

    return txt;
}

/**
 * Get the placeholder text of a text area
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * edgui_ta_get_placeholder_text(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    const char * txt = NULL;

    if(ext->placeholder) txt = edgui_label_get_text(ext->placeholder);

    return txt;
}

/**
 * Get the label of a text area
 * @param ta pointer to a text area object
 * @return pointer to the label object
 */
edgui_obj_t * edgui_ta_get_label(const edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    return ext->label;
}

/**
 * Get the current cursor position in character index
 * @param ta pointer to a text area object
 * @return the cursor position
 */
uint16_t edgui_ta_get_cursor_pos(const edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    return ext->cursor.pos;
}

/**
 * Get the current cursor type.
 * @param ta pointer to a text area object
 * @return element of 'edgui_ta_cursor_type_t'
 */
edgui_cursor_type_t edgui_ta_get_cursor_type(const edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    return ext->cursor.type;
}

/**
 * Get whether the cursor click positioning is enabled or not.
 * @param ta pointer to a text area object
 * @return true: enable click positions; false: disable
 */
bool edgui_ta_get_cursor_click_pos(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    return ext->cursor.click_pos ? true : false;
}

/**
 * Get the password mode attribute
 * @param ta pointer to a text area object
 * @return true: password mode is enabled, false: disabled
 */
bool edgui_ta_get_pwd_mode(const edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    return ext->pwd_mode == 0 ? false : true;
}

/**
 * Get the one line configuration attribute
 * @param ta pointer to a text area object
 * @return true: one line configuration is enabled, false: disabled
 */
bool edgui_ta_get_one_line(const edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    return ext->one_line == 0 ? false : true;
}

/**
 * Get a list of accepted characters.
 * @param ta pointer to  Text Area
 * @return list of accented characters.
 */
const char * edgui_ta_get_accepted_chars(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    return ext->accapted_chars;
}

/**
 * Set max length of a Text Area.
 * @param ta pointer to  Text Area
 * @return the maximal number of characters to be add
 */
uint16_t edgui_ta_get_max_length(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    return ext->max_length;
}

/**
 * Get a style of a text area
 * @param ta pointer to a text area object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_ta_get_style(const edgui_obj_t * ta, edgui_ta_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_ta_ext_t * ext        = edgui_obj_get_ext_attr(ta);

    switch(type) {
        case EDGUI_TA_STYLE_BG: style = edgui_page_get_style(ta, EDGUI_PAGE_STYLE_BG); break;
        case EDGUI_TA_STYLE_SB: style = edgui_page_get_style(ta, EDGUI_PAGE_STYLE_SB); break;
        case EDGUI_TA_STYLE_EDGE_FLASH: style = edgui_page_get_style(ta, EDGUI_PAGE_STYLE_EDGE_FLASH); break;
        case EDGUI_TA_STYLE_CURSOR: style = ext->cursor.style; break;
        case EDGUI_TA_STYLE_PLACEHOLDER:
            if(ext->placeholder) style = edgui_label_get_style(ext->placeholder, EDGUI_LABEL_STYLE_MAIN);
            break;
        default: style = NULL; break;
    }

    return style;
}

/**
 * Find whether text is selected or not.
 * @param ta Text area object
 * @return whether text is selected or not
 */
bool edgui_ta_text_is_selected(const edgui_obj_t * ta)
{
#if EDGUI_LABEL_TEXT_SEL
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    if((edgui_label_get_text_sel_start(ext->label) == EDGUI_LABEL_TEXT_SEL_OFF ||
        edgui_label_get_text_sel_end(ext->label) == EDGUI_LABEL_TEXT_SEL_OFF)) {
        return true;
    } else {
        return false;
    }
#else
    (void)ta; /*Unused*/
    return false;
#endif
}

/**
 * Find whether selection mode is enabled.
 * @param ta pointer to a text area object
 * @return true: selection mode is enabled, false: disabled
 */
bool edgui_ta_get_text_sel_en(edgui_obj_t * ta)
{
#if EDGUI_LABEL_TEXT_SEL
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    return ext->text_sel_en;
#else
    (void)ta; /*Unused*/
    return false;
#endif
}

/**
 * Set how long show the password before changing it to '*'
 * @param ta pointer to Text area
 * @return show time in milliseconds. 0: hide immediately.
 */
uint16_t edgui_ta_get_pwd_show_time(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    return ext->pwd_show_time;
}

/**
 * Set cursor blink animation time
 * @param ta pointer to Text area
 * @return time blink period. 0: disable blinking
 */
uint16_t edgui_ta_get_cursor_blink_time(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    return ext->cursor.blink_time;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Clear the selection on the text area.
 * @param ta Text area object
 */
void edgui_ta_clear_selection(edgui_obj_t * ta)
{
#if EDGUI_LABEL_TEXT_SEL
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    if(edgui_label_get_text_sel_start(ext->label) != EDGUI_LABEL_TEXT_SEL_OFF ||
       edgui_label_get_text_sel_end(ext->label) != EDGUI_LABEL_TEXT_SEL_OFF) {
        edgui_label_set_text_sel_start(ext->label, EDGUI_LABEL_TEXT_SEL_OFF);
        edgui_label_set_text_sel_end(ext->label, EDGUI_LABEL_TEXT_SEL_OFF);
    }
#else
    (void)ta; /*Unused*/
#endif
}

/**
 * Move the cursor one character right
 * @param ta pointer to a text area object
 */
void edgui_ta_cursor_right(edgui_obj_t * ta)
{
    uint16_t cp = edgui_ta_get_cursor_pos(ta);
    cp++;
    edgui_ta_set_cursor_pos(ta, cp);
}

/**
 * Move the cursor one character left
 * @param ta pointer to a text area object
 */
void edgui_ta_cursor_left(edgui_obj_t * ta)
{
    uint16_t cp = edgui_ta_get_cursor_pos(ta);
    if(cp > 0) {
        cp--;
        edgui_ta_set_cursor_pos(ta, cp);
    }
}

/**
 * Move the cursor one line down
 * @param ta pointer to a text area object
 */
void edgui_ta_cursor_down(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    edgui_point_t pos;

    /*Get the position of the current letter*/
    edgui_label_get_letter_pos(ext->label, edgui_ta_get_cursor_pos(ta), &pos);

    /*Increment the y with one line and keep the valid x*/
    const edgui_style_t * label_style = edgui_obj_get_style(ext->label);
    const edgui_font_t * font_p       = label_style->text.font;
    edgui_coord_t font_h              = edgui_font_get_line_height(font_p);
    pos.y += font_h + label_style->text.line_space + 1;
    pos.x = ext->cursor.valid_x;

    /*Do not go below the last line*/
    if(pos.y < edgui_obj_get_height(ext->label)) {
        /*Get the letter index on the new cursor position and set it*/
        uint16_t new_cur_pos = edgui_label_get_letter_on(ext->label, &pos);

        edgui_coord_t cur_valid_x_tmp = ext->cursor.valid_x; /*Cursor position set overwrites the valid positon */
        edgui_ta_set_cursor_pos(ta, new_cur_pos);
        ext->cursor.valid_x = cur_valid_x_tmp;
    }
}

/**
 * Move the cursor one line up
 * @param ta pointer to a text area object
 */
void edgui_ta_cursor_up(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    edgui_point_t pos;

    /*Get the position of the current letter*/
    edgui_label_get_letter_pos(ext->label, edgui_ta_get_cursor_pos(ta), &pos);

    /*Decrement the y with one line and keep the valid x*/
    const edgui_style_t * label_style = edgui_obj_get_style(ext->label);
    const edgui_font_t * font         = label_style->text.font;
    edgui_coord_t font_h              = edgui_font_get_line_height(font);
    pos.y -= font_h + label_style->text.line_space - 1;
    pos.x = ext->cursor.valid_x;

    /*Get the letter index on the new cursor position and set it*/
    uint16_t new_cur_pos       = edgui_label_get_letter_on(ext->label, &pos);
    edgui_coord_t cur_valid_x_tmp = ext->cursor.valid_x; /*Cursor position set overwrites the valid positon */
    edgui_ta_set_cursor_pos(ta, new_cur_pos);
    ext->cursor.valid_x = cur_valid_x_tmp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the text areas
 * @param ta pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_ta_design(edgui_obj_t * ta, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask_p area*/
        return ancestor_design(ta, mask, mode);
    } else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        /*Draw the object*/
        ancestor_design(ta, mask, mode);

    } else if(mode == EDGUI_DESIGN_DRAW_POST) {
        ancestor_design(ta, mask, mode);
    }
    return true;
}

/**
 * An extended scrollable design of the page. Calls the normal design function and draws a cursor.
 * @param scrl pointer to the scrollable part of the Text area
 * @param mask  the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @return return true/false, depends on 'mode'
 */
static bool edgui_ta_scrollable_design(edgui_obj_t * scrl, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask_p area*/
        return scrl_design(scrl, mask, mode);
    } else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        /*Draw the object*/
        scrl_design(scrl, mask, mode);
    } else if(mode == EDGUI_DESIGN_DRAW_POST) {
        scrl_design(scrl, mask, mode);

        /*Draw the cursor*/
        edgui_obj_t * ta     = edgui_obj_get_parent(scrl);
        edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

        if(ext->cursor.type == EDGUI_CURSOR_NONE || (ext->cursor.type & EDGUI_CURSOR_HIDDEN) || ext->cursor.state == 0) {
            return true; /*The cursor is not visible now*/
        }

        edgui_style_t cur_style;
        get_cursor_style(ta, &cur_style);

        const char * txt = edgui_label_get_text(ext->label);

        /*Draw he cursor according to the type*/
        edgui_area_t cur_area;
        edgui_area_copy(&cur_area, &ext->cursor.area);

        cur_area.x1 += ext->label->coords.x1;
        cur_area.y1 += ext->label->coords.y1;
        cur_area.x2 += ext->label->coords.x1;
        cur_area.y2 += ext->label->coords.y1;

        edgui_opa_t opa_scale = edgui_obj_get_opa_scale(ta);

        if(ext->cursor.type == EDGUI_CURSOR_LINE) {
            edgui_draw_rect(&cur_area, mask, &cur_style, opa_scale);
        } else if(ext->cursor.type == EDGUI_CURSOR_BLOCK) {
            edgui_draw_rect(&cur_area, mask, &cur_style, opa_scale);

            char letter_buf[8] = {0};
            memcpy(letter_buf, &txt[ext->cursor.txt_byte_pos], edgui_txt_encoded_size(&txt[ext->cursor.txt_byte_pos]));

            cur_area.x1 += cur_style.body.padding.left;
            cur_area.y1 += cur_style.body.padding.top;
            edgui_draw_label(&cur_area, mask, &cur_style, opa_scale, letter_buf, EDGUI_TXT_FLAG_NONE, 0,
                          EDGUI_LABEL_TEXT_SEL_OFF, EDGUI_LABEL_TEXT_SEL_OFF, NULL);

        } else if(ext->cursor.type == EDGUI_CURSOR_OUTLINE) {
            cur_style.body.opa = EDGUI_OPA_TRANSP;
            if(cur_style.body.border.width == 0) cur_style.body.border.width = 1; /*Be sure the border will be drawn*/
            edgui_draw_rect(&cur_area, mask, &cur_style, opa_scale);
        } else if(ext->cursor.type == EDGUI_CURSOR_UNDERLINE) {
            edgui_draw_rect(&cur_area, mask, &cur_style, opa_scale);
        }
    }

    return true;
}

/**
 * Signal function of the text area
 * @param ta pointer to a text area object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_ta_signal(edgui_obj_t * ta, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(ta, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    if(sign == EDGUI_SIGNAL_CLEANUP) {
        if(ext->pwd_tmp != NULL) edgui_mem_free(ext->pwd_tmp);

        /* (The created label will be deleted automatically) */
    } else if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        if(ext->label) {
            edgui_obj_t * scrl               = edgui_page_get_scrl(ta);
            const edgui_style_t * style_ta   = edgui_obj_get_style(ta);
            const edgui_style_t * style_scrl = edgui_obj_get_style(scrl);
            if(ext->one_line) {
                /*In one line mode refresh the Text Area height because 'vpad' can modify it*/
                const edgui_style_t * style_label = edgui_obj_get_style(ext->label);
                edgui_coord_t font_h              = edgui_font_get_line_height(style_label->text.font);
                edgui_obj_set_height(ta, font_h + style_ta->body.padding.top + style_ta->body.padding.bottom +
                                          style_scrl->body.padding.top + style_scrl->body.padding.bottom);
            } else {
                /*In not one line mode refresh the Label width because 'hpad' can modify it*/
                edgui_obj_set_width(ext->label, edgui_page_get_fit_width(ta));
                edgui_obj_set_pos(ext->label, style_scrl->body.padding.left,
                               style_scrl->body.padding.right); /*Be sure the Label is in the correct position*/

                if(ext->placeholder) {
                    edgui_obj_set_width(ext->placeholder, edgui_page_get_fit_width(ta));
                    edgui_obj_set_pos(ext->placeholder, style_scrl->body.padding.left,
                                   style_scrl->body.padding.top); /*Be sure the placeholder is in the correct position*/
                }
            }
            edgui_label_set_text(ext->label, NULL);
        }
    } else if(sign == EDGUI_SIGNAL_CORD_CHG) {
        /*Set the label width according to the text area width*/
        if(ext->label) {
            if(edgui_obj_get_width(ta) != edgui_area_get_width(param) || edgui_obj_get_height(ta) != edgui_area_get_height(param)) {
                edgui_obj_t * scrl               = edgui_page_get_scrl(ta);
                const edgui_style_t * style_scrl = edgui_obj_get_style(scrl);
                edgui_obj_set_width(ext->label, edgui_page_get_fit_width(ta));
                edgui_obj_set_pos(ext->label, style_scrl->body.padding.left, style_scrl->body.padding.top);
                edgui_label_set_text(ext->label, NULL); /*Refresh the label*/

                refr_cursor_area(ta);
            }
        }
        /*Set the placeholder width according to the text area width*/
        if(ext->placeholder) {
            if(edgui_obj_get_width(ta) != edgui_area_get_width(param) || edgui_obj_get_height(ta) != edgui_area_get_height(param)) {
                edgui_obj_t * scrl               = edgui_page_get_scrl(ta);
                const edgui_style_t * style_scrl = edgui_obj_get_style(scrl);
                edgui_obj_set_width(ext->placeholder, edgui_page_get_fit_width(ta));
                edgui_obj_set_pos(ext->placeholder, style_scrl->body.padding.left, style_scrl->body.padding.top);
                edgui_label_set_text(ext->placeholder, NULL); /*Refresh the label*/

                refr_cursor_area(ta);
            }
        }
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        uint32_t c = *((uint32_t *)param); /*uint32_t because can be UTF-8*/
        if(c == EDGUI_KEY_RIGHT)
            edgui_ta_cursor_right(ta);
        else if(c == EDGUI_KEY_LEFT)
            edgui_ta_cursor_left(ta);
        else if(c == EDGUI_KEY_UP)
            edgui_ta_cursor_up(ta);
        else if(c == EDGUI_KEY_DOWN)
            edgui_ta_cursor_down(ta);
        else if(c == EDGUI_KEY_BACKSPACE)
            edgui_ta_del_char(ta);
        else if(c == EDGUI_KEY_DEL)
            edgui_ta_del_char_forward(ta);
        else if(c == EDGUI_KEY_HOME)
            edgui_ta_set_cursor_pos(ta, 0);
        else if(c == EDGUI_KEY_END)
            edgui_ta_set_cursor_pos(ta, EDGUI_TA_CURSOR_LAST);
        else {
            edgui_ta_add_char(ta, c);
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
        buf->type[i] = "edgui_ta";
    } else if(sign == EDGUI_SIGNAL_DEFOCUS) {
        edgui_cursor_type_t cur_type;
        cur_type = edgui_ta_get_cursor_type(ta);
        edgui_ta_set_cursor_type(ta, cur_type | EDGUI_CURSOR_HIDDEN);
    } else if(sign == EDGUI_SIGNAL_FOCUS) {
#if EDGUI_USE_GROUP
        edgui_cursor_type_t cur_type;
        cur_type                   = edgui_ta_get_cursor_type(ta);
        edgui_group_t * g             = edgui_obj_get_group(ta);
        bool editing               = edgui_group_get_editing(g);
        edgui_indev_type_t indev_type = edgui_indev_get_type(edgui_indev_get_act());

        /*Encoders need special handling*/
        if(indev_type == EDGUI_INDEV_TYPE_ENCODER) {
            if(editing)
                edgui_ta_set_cursor_type(ta, cur_type & (~EDGUI_CURSOR_HIDDEN));
            else
                edgui_ta_set_cursor_type(ta, cur_type | EDGUI_CURSOR_HIDDEN);
        } else {
            edgui_ta_set_cursor_type(ta, cur_type & (~EDGUI_CURSOR_HIDDEN));
        }
#endif
    } else if(sign == EDGUI_SIGNAL_PRESSED || sign == EDGUI_SIGNAL_PRESSING || sign == EDGUI_SIGNAL_PRESS_LOST ||
              sign == EDGUI_SIGNAL_RELEASED) {
        update_cursor_position_on_click(ta, sign, (edgui_indev_t *)param);
    }
    return res;
}

/**
 * Signal function of the scrollable part of the text area
 * @param scrl pointer to scrollable part of a text area object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_ta_scrollable_signal(edgui_obj_t * scrl, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = scrl_signal(scrl, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_obj_t * ta     = edgui_obj_get_parent(scrl);
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    if(sign == EDGUI_SIGNAL_REFR_EXT_DRAW_PAD) {
        /*Set ext. size because the cursor might be out of this object*/
        const edgui_style_t * style_label = edgui_obj_get_style(ext->label);
        edgui_coord_t font_h              = edgui_font_get_line_height(style_label->text.font);
        scrl->ext_draw_pad             = EDGUI_MATH_MAX(scrl->ext_draw_pad, style_label->text.line_space + font_h);
    } else if(sign == EDGUI_SIGNAL_CORD_CHG) {
        /*Set the label width according to the text area width*/
        if(ext->label) {
            if(edgui_obj_get_width(scrl) != edgui_area_get_width(param) ||
               edgui_obj_get_height(scrl) != edgui_area_get_height(param)) {

                const edgui_style_t * style_scrl = edgui_obj_get_style(scrl);
                edgui_obj_set_width(ext->label, edgui_page_get_fit_width(ta));
                edgui_obj_set_pos(ext->label, style_scrl->body.padding.left, style_scrl->body.padding.top);
                edgui_label_set_text(ext->label, NULL); /*Refresh the label*/

                refr_cursor_area(ta);
            }
        }
    } else if(sign == EDGUI_SIGNAL_PRESSING || sign == EDGUI_SIGNAL_PRESSED || sign == EDGUI_SIGNAL_PRESS_LOST ||
              sign == EDGUI_SIGNAL_RELEASED) {
        update_cursor_position_on_click(ta, sign, (edgui_indev_t *)param);
    }

    return res;
}

#if EDGUI_USE_ANIMATION

/**
 * Called to blink the cursor
 * @param ta pointer to a text area
 * @param hide 1: hide the cursor, 0: show it
 */
static void cursor_blink_anim(edgui_obj_t * ta, edgui_anim_value_t show)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    if(show != ext->cursor.state) {
        ext->cursor.state = show == 0 ? 0 : 1;
        if(ext->cursor.type != EDGUI_CURSOR_NONE && (ext->cursor.type & EDGUI_CURSOR_HIDDEN) == 0) {
            edgui_disp_t * disp = edgui_obj_get_disp(ta);
            edgui_area_t area_tmp;
            edgui_area_copy(&area_tmp, &ext->cursor.area);
            area_tmp.x1 += ext->label->coords.x1;
            area_tmp.y1 += ext->label->coords.y1;
            area_tmp.x2 += ext->label->coords.x1;
            area_tmp.y2 += ext->label->coords.y1;
            edgui_inv_area(disp, &area_tmp);
        }
    }
}

/**
 * Dummy function to animate char hiding in pwd mode.
 * Does nothing, but a function is required in car hiding anim.
 * (pwd_char_hider callback do the real job)
 * @param ta unused
 * @param x unused
 */
static void pwd_char_hider_anim(edgui_obj_t * ta, edgui_anim_value_t x)
{
    (void)ta;
    (void)x;
}

/**
 * Call when an animation is ready to convert all characters to '*'
 * @param a pointer to the animation
 */
static void pwd_char_hider_anim_ready(edgui_anim_t * a)
{
    edgui_obj_t * ta = a->var;
    pwd_char_hider(ta);
}
#endif

/**
 * Hide all characters (convert them to '*')
 * @param ta: pointer to text area object
 */
static void pwd_char_hider(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    if(ext->pwd_mode != 0) {
        char * txt  = edgui_label_get_text(ext->label);
        int16_t len = edgui_txt_get_encoded_length(txt);
        bool refr   = false;
        uint16_t i;
        for(i = 0; i < len; i++) {
            txt[i] = '*';
            refr   = true;
        }

        txt[i] = '\0';

        if(refr != false) edgui_label_set_text(ext->label, txt);
    }
}

/**
 * Test an unicode character if it is accepted or not. Checks max length and accepted char list.
 * @param ta pointer to a test area object
 * @param c an unicode character
 * @return true: accapted; false: rejected
 */
static bool char_is_accepted(edgui_obj_t * ta, uint32_t c)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    /*If no restriction accept it*/
    if(ext->accapted_chars == NULL && ext->max_length == 0) return true;

    /*Too many characters?*/
    if(ext->max_length > 0 && edgui_txt_get_encoded_length(edgui_ta_get_text(ta)) >= ext->max_length) {
        return false;
    }

    /*Accepted character?*/
    if(ext->accapted_chars) {
        uint32_t i = 0;
        uint32_t a;
        while(ext->accapted_chars[i] != '\0') {
            a = edgui_txt_encoded_next(ext->accapted_chars, &i);
            if(a == c) return true; /*Accepted*/
        }

        return false; /*The character wasn't in the list*/
    } else {
        return true; /*If the accepted char list in not specified the accept the character*/
    }
}

static void get_cursor_style(edgui_obj_t * ta, edgui_style_t * style_res)
{
    edgui_ta_ext_t * ext              = edgui_obj_get_ext_attr(ta);
    const edgui_style_t * label_style = edgui_obj_get_style(ext->label);

    if(ext->cursor.style) {
        edgui_style_copy(style_res, ext->cursor.style);
    } else {
        /*If cursor style is not specified then use the modified label style */
        edgui_style_copy(style_res, label_style);
        edgui_color_t cedgui_color_tmp = style_res->text.color; /*Make letter color to cursor color*/
        style_res->text.color =
            style_res->body.main_color; /*In block mode the letter color will be current background color*/
        style_res->body.main_color     = cedgui_color_tmp;
        style_res->body.grad_color     = cedgui_color_tmp;
        style_res->body.border.color   = cedgui_color_tmp;
        style_res->body.border.opa     = EDGUI_OPA_COVER;
        style_res->body.border.width   = 1;
        style_res->body.shadow.width   = 0;
        style_res->body.radius         = 0;
        style_res->body.opa            = EDGUI_OPA_COVER;
        style_res->body.padding.left   = 0;
        style_res->body.padding.right  = 0;
        style_res->body.padding.top    = 0;
        style_res->body.padding.bottom = 0;
        style_res->line.width          = 1;
        style_res->body.opa            = EDGUI_OPA_COVER;
    }
}

static void refr_cursor_area(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);

    const edgui_style_t * label_style = edgui_obj_get_style(ext->label);

    edgui_style_t cur_style;
    get_cursor_style(ta, &cur_style);

    uint16_t cur_pos = edgui_ta_get_cursor_pos(ta);
    const char * txt = edgui_label_get_text(ext->label);

    uint32_t byte_pos;
    byte_pos = edgui_txt_encoded_get_byte_id(txt, cur_pos);

    uint32_t letter = edgui_txt_encoded_next(&txt[byte_pos], NULL);

    edgui_coord_t letter_h = edgui_font_get_line_height(label_style->text.font);

    /*Set letter_w (set not 0 on non printable but valid chars)*/
    edgui_coord_t letter_w;
    if(letter == '\0' || letter == '\n' || letter == '\r') {
        letter_w = edgui_font_get_glyph_width(label_style->text.font, ' ', '\0');
    } else {
        /*`letter_next` parameter is '\0' to ignore kerning*/
        letter_w = edgui_font_get_glyph_width(label_style->text.font, letter, '\0');
    }

    edgui_point_t letter_pos;
    edgui_label_get_letter_pos(ext->label, cur_pos, &letter_pos);

    /*If the cursor is out of the text (most right) draw it to the next line*/
    if(letter_pos.x + ext->label->coords.x1 + letter_w > ext->label->coords.x2 && ext->one_line == 0 &&
       edgui_label_get_align(ext->label) != EDGUI_LABEL_ALIGN_RIGHT) {
        letter_pos.x = 0;
        letter_pos.y += letter_h + label_style->text.line_space;

        if(letter != '\0') {
            byte_pos += edgui_txt_encoded_size(&txt[byte_pos]);
            letter = edgui_txt_encoded_next(&txt[byte_pos], NULL);
        }

        if(letter == '\0' || letter == '\n' || letter == '\r') {
            letter_w = edgui_font_get_glyph_width(label_style->text.font, ' ', '\0');
        } else {
            letter_w = edgui_font_get_glyph_width(label_style->text.font, letter, '\0');
        }
    }

    /*Save the byte position. It is required to draw `EDGUI_CURSOR_BLOCK`*/
    ext->cursor.txt_byte_pos = byte_pos;

    /*Draw he cursor according to the type*/
    edgui_area_t cur_area;

    if(ext->cursor.type == EDGUI_CURSOR_LINE) {
        cur_area.x1 =
            letter_pos.x + cur_style.body.padding.left - (cur_style.line.width >> 1) - (cur_style.line.width & 0x1);
        cur_area.y1 = letter_pos.y + cur_style.body.padding.top;
        cur_area.x2 = letter_pos.x + cur_style.body.padding.right + (cur_style.line.width >> 1);
        cur_area.y2 = letter_pos.y + cur_style.body.padding.bottom + letter_h;
    } else if(ext->cursor.type == EDGUI_CURSOR_BLOCK) {
        cur_area.x1 = letter_pos.x - cur_style.body.padding.left;
        cur_area.y1 = letter_pos.y - cur_style.body.padding.top;
        cur_area.x2 = letter_pos.x + cur_style.body.padding.right + letter_w;
        cur_area.y2 = letter_pos.y + cur_style.body.padding.bottom + letter_h;

    } else if(ext->cursor.type == EDGUI_CURSOR_OUTLINE) {
        cur_area.x1 = letter_pos.x - cur_style.body.padding.left;
        cur_area.y1 = letter_pos.y - cur_style.body.padding.top;
        cur_area.x2 = letter_pos.x + cur_style.body.padding.right + letter_w;
        cur_area.y2 = letter_pos.y + cur_style.body.padding.bottom + letter_h;
    } else if(ext->cursor.type == EDGUI_CURSOR_UNDERLINE) {
        cur_area.x1 = letter_pos.x + cur_style.body.padding.left;
        cur_area.y1 = letter_pos.y + cur_style.body.padding.top + letter_h - (cur_style.line.width >> 1);
        cur_area.x2 = letter_pos.x + cur_style.body.padding.right + letter_w;
        cur_area.y2 = letter_pos.y + cur_style.body.padding.bottom + letter_h + (cur_style.line.width >> 1) +
                      (cur_style.line.width & 0x1);
    }

    /*Save the new area*/
    edgui_disp_t * disp = edgui_obj_get_disp(ta);
    edgui_area_t area_tmp;
    edgui_area_copy(&area_tmp, &ext->cursor.area);
    area_tmp.x1 += ext->label->coords.x1;
    area_tmp.y1 += ext->label->coords.y1;
    area_tmp.x2 += ext->label->coords.x1;
    area_tmp.y2 += ext->label->coords.y1;
    edgui_inv_area(disp, &area_tmp);

    edgui_area_copy(&ext->cursor.area, &cur_area);

    edgui_area_copy(&area_tmp, &ext->cursor.area);
    area_tmp.x1 += ext->label->coords.x1;
    area_tmp.y1 += ext->label->coords.y1;
    area_tmp.x2 += ext->label->coords.x1;
    area_tmp.y2 += ext->label->coords.y1;
    edgui_inv_area(disp, &area_tmp);
}

static void placeholder_update(edgui_obj_t * ta)
{
    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    const char * ta_text;

    if(ext->placeholder == NULL) return;

    ta_text = edgui_ta_get_text(ta);

    if(ta_text[0] == '\0') {
        /*Be sure the main label and the placeholder has the same coordinates*/
        edgui_obj_t * scrl               = edgui_page_get_scrl(ta);
        const edgui_style_t * style_scrl = edgui_obj_get_style(scrl);
        edgui_obj_set_pos(ext->placeholder, style_scrl->body.padding.left, style_scrl->body.padding.top);
        edgui_obj_set_pos(ext->label, style_scrl->body.padding.left, style_scrl->body.padding.top);

        edgui_obj_set_width(ext->placeholder, edgui_page_get_fit_width(ta));
        edgui_obj_set_hidden(ext->placeholder, false);
    } else {
        edgui_obj_set_hidden(ext->placeholder, true);
    }
}

static void update_cursor_position_on_click(edgui_obj_t * ta, edgui_signal_t sign, edgui_indev_t * click_source)
{

    if(click_source == NULL) return;

    edgui_ta_ext_t * ext = edgui_obj_get_ext_attr(ta);
    if(ext->cursor.click_pos == 0) return;
    if(ext->cursor.type == EDGUI_CURSOR_NONE) return;

    if(edgui_indev_get_type(click_source) == EDGUI_INDEV_TYPE_KEYPAD ||
       edgui_indev_get_type(click_source) == EDGUI_INDEV_TYPE_ENCODER) {
        return;
    }

    edgui_area_t label_coords;
    edgui_obj_get_coords(ext->label, &label_coords);

    edgui_point_t point_act, vect_act;
    edgui_indev_get_point(click_source, &point_act);
    edgui_indev_get_vect(click_source, &vect_act);

    if(point_act.x < 0 || point_act.y < 0) return; /*Ignore event from keypad*/
    edgui_point_t relative_position;
    relative_position.x = point_act.x - label_coords.x1;
    relative_position.y = point_act.y - label_coords.y1;

    edgui_coord_t label_width = edgui_obj_get_width(ext->label);

    uint16_t index_of_char_at_position;

#if EDGUI_LABEL_TEXT_SEL
    edgui_label_ext_t * ext_label = edgui_obj_get_ext_attr(ext->label);
    bool click_outside_label;
    /*Check if the click happened on the left side of the area outside the label*/
    if(relative_position.x < 0) {
        index_of_char_at_position = 0;
        click_outside_label       = true;
    }
    /*Check if the click happened on the right side of the area outside the label*/
    else if(relative_position.x >= label_width) {
        index_of_char_at_position = EDGUI_TA_CURSOR_LAST;
        click_outside_label       = true;
    } else {
        index_of_char_at_position = edgui_label_get_letter_on(ext->label, &relative_position);
        click_outside_label       = !edgui_label_is_char_under_pos(ext->label, &relative_position);
    }

    if(ext->text_sel_en) {
        if(!ext->text_sel_in_prog && !click_outside_label && sign == EDGUI_SIGNAL_PRESSED) {
            /*Input device just went down. Store the selection start position*/
            ext->tmp_sel_start    = index_of_char_at_position;
            ext->tmp_sel_end      = EDGUI_LABEL_TEXT_SEL_OFF;
            ext->text_sel_in_prog = 1;
            edgui_obj_set_drag(edgui_page_get_scrl(ta), false);
        } else if(ext->text_sel_in_prog && sign == EDGUI_SIGNAL_PRESSING) {
            /*Input device may be moving. Store the end position */
            ext->tmp_sel_end = index_of_char_at_position;
        } else if(ext->text_sel_in_prog && (sign == EDGUI_SIGNAL_PRESS_LOST || sign == EDGUI_SIGNAL_RELEASED)) {
            /*Input device is released. Check if anything was selected.*/
            edgui_obj_set_drag(edgui_page_get_scrl(ta), true);
        }
    }

    if(ext->text_sel_in_prog || sign == EDGUI_SIGNAL_PRESSED) edgui_ta_set_cursor_pos(ta, index_of_char_at_position);

    if(ext->text_sel_in_prog) {
        /*If the selected area has changed then update the real values and*/
        /*invalidate the text area.*/

        if(ext->tmp_sel_start > ext->tmp_sel_end) {
            if(ext_label->txt_sel_start != ext->tmp_sel_end || ext_label->txt_sel_end != ext->tmp_sel_start) {
                ext_label->txt_sel_start = ext->tmp_sel_end;
                ext_label->txt_sel_end   = ext->tmp_sel_start;
                edgui_obj_invalidate(ta);
            }
        } else if(ext->tmp_sel_start < ext->tmp_sel_end) {
            if(ext_label->txt_sel_start != ext->tmp_sel_start || ext_label->txt_sel_end != ext->tmp_sel_end) {
                ext_label->txt_sel_start = ext->tmp_sel_start;
                ext_label->txt_sel_end   = ext->tmp_sel_end;
                edgui_obj_invalidate(ta);
            }
        } else {
            if(ext_label->txt_sel_start != EDGUI_LABEL_TEXT_SEL_OFF || ext_label->txt_sel_end != EDGUI_LABEL_TEXT_SEL_OFF) {
                ext_label->txt_sel_start = EDGUI_LABEL_TEXT_SEL_OFF;
                ext_label->txt_sel_end   = EDGUI_LABEL_TEXT_SEL_OFF;
                edgui_obj_invalidate(ta);
            }
        }
        /*Finish selection if necessary */
        if(sign == EDGUI_SIGNAL_PRESS_LOST || sign == EDGUI_SIGNAL_RELEASED) {
            ext->text_sel_in_prog = 0;
        }
    }
#else
    /*Check if the click happened on the left side of the area outside the label*/
    if(relative_position.x < 0) {
        index_of_char_at_position = 0;
    }
    /*Check if the click happened on the right side of the area outside the label*/
    else if(relative_position.x >= label_width) {
        index_of_char_at_position = EDGUI_TA_CURSOR_LAST;
    } else {
        index_of_char_at_position = edgui_label_get_letter_on(ext->label, &relative_position);
    }

    if(sign == EDGUI_SIGNAL_PRESSED) edgui_ta_set_cursor_pos(ta, index_of_char_at_position);
#endif
}

#endif
