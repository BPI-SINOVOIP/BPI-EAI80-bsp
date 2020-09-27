/**
 * @file edgui_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_label.h"
#if EDGUI_USE_LABEL != 0

#include "../edgui_core/edgui_obj.h"
#include "../edgui_core/edgui_group.h"
#include "../edgui_misc/edgui_color.h"
#include "../edgui_misc/edgui_math.h"

/*********************
 *      DEFINES
 *********************/
/*Test configurations*/
#ifndef EDGUI_LABEL_DEF_SCROLL_SPEED
#define EDGUI_LABEL_DEF_SCROLL_SPEED (25)
#endif

#define EDGUI_LABEL_DOT_END_INV 0xFFFF
#define EDGUI_LABEL_HINT_HEIGHT_LIMIT                                                                                     \
    1024 /*Enable "hint" to buffer info about labels larger than this. (Speed up their drawing)*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_label_signal(edgui_obj_t * label, edgui_signal_t sign, void * param);
static bool edgui_label_design(edgui_obj_t * label, const edgui_area_t * mask, edgui_design_mode_t mode);
static void edgui_label_refr_text(edgui_obj_t * label);
static void edgui_label_revert_dots(edgui_obj_t * label);

#if EDGUI_USE_ANIMATION
static void edgui_label_set_offset_x(edgui_obj_t * label, edgui_coord_t x);
static void edgui_label_set_offset_y(edgui_obj_t * label, edgui_coord_t y);
#endif

static bool edgui_label_set_dot_tmp(edgui_obj_t * label, char * data, uint16_t len);
static char * edgui_label_get_dot_tmp(edgui_obj_t * label);
static void edgui_label_dot_tmp_free(edgui_obj_t * label);

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
 * Create a label objects
 * @param par pointer to an object, it will be the parent of the new label
 * @param copy pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
edgui_obj_t * edgui_label_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("label create started");

    /*Create a basic object*/
    edgui_obj_t * new_label = edgui_obj_create(par, copy);
    edgui_mem_assert(new_label);
    if(new_label == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_label);

    /*Extend the basic object to a label object*/
    edgui_obj_allocate_ext_attr(new_label, sizeof(edgui_label_ext_t));

    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(new_label);
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->text       = NULL;
    ext->static_txt = 0;
    ext->recolor    = 0;
    ext->body_draw  = 0;
    ext->align      = EDGUI_LABEL_ALIGN_LEFT;
    ext->dot_end    = EDGUI_LABEL_DOT_END_INV;
    ext->long_mode  = EDGUI_LABEL_LONG_EXPAND;
#if EDGUI_USE_ANIMATION
    ext->anim_speed = EDGUI_LABEL_DEF_SCROLL_SPEED;
#endif
    ext->offset.x = 0;
    ext->offset.y = 0;

#if EDGUI_LABEL_LONG_TXT_HINT
    ext->hint.line_start = -1;
    ext->hint.coord_y    = 0;
    ext->hint.y          = 0;
#endif

#if EDGUI_LABEL_TEXT_SEL
    ext->txt_sel_start = EDGUI_LABEL_TEXT_SEL_OFF;
    ext->txt_sel_end   = EDGUI_LABEL_TEXT_SEL_OFF;
#endif
    ext->dot.tmp_ptr   = NULL;
    ext->dot_tmp_alloc = 0;

    edgui_obj_set_design_cb(new_label, edgui_label_design);
    edgui_obj_set_signal_cb(new_label, edgui_label_signal);

    /*Init the new label*/
    if(copy == NULL) {
        edgui_obj_set_click(new_label, false);
        edgui_label_set_long_mode(new_label, EDGUI_LABEL_LONG_EXPAND);
        edgui_label_set_text(new_label, "Text");
        edgui_label_set_style(new_label, EDGUI_LABEL_STYLE_MAIN, NULL); /*Inherit parent's style*/
    }
    /*Copy 'copy' if not NULL*/
    else {
        edgui_label_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        edgui_label_set_long_mode(new_label, edgui_label_get_long_mode(copy));
        edgui_label_set_recolor(new_label, edgui_label_get_recolor(copy));
        edgui_label_set_body_draw(new_label, edgui_label_get_body_draw(copy));
        edgui_label_set_align(new_label, edgui_label_get_align(copy));
        if(copy_ext->static_txt == 0)
            edgui_label_set_text(new_label, edgui_label_get_text(copy));
        else
            edgui_label_set_static_text(new_label, edgui_label_get_text(copy));

        /*In DOT mode save the text byte-to-byte because a '\0' can be in the middle*/
        if(copy_ext->long_mode == EDGUI_LABEL_LONG_DOT) {
            ext->text = edgui_mem_realloc(ext->text, edgui_mem_get_size(copy_ext->text));
            edgui_mem_assert(ext->text);
            if(ext->text == NULL) return NULL;
            memcpy(ext->text, copy_ext->text, edgui_mem_get_size(copy_ext->text));
        }

        if(copy_ext->dot_tmp_alloc && copy_ext->dot.tmp_ptr) {
            int len = strlen(copy_ext->dot.tmp_ptr);
            edgui_label_set_dot_tmp(new_label, ext->dot.tmp_ptr, len);
        } else {
            memcpy(ext->dot.tmp, copy_ext->dot.tmp, sizeof(ext->dot.tmp));
        }
        ext->dot_tmp_alloc = copy_ext->dot_tmp_alloc;
        ext->dot_end       = copy_ext->dot_end;

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_label);
    }

    EDGUI_LOG_INFO("label created");

    return new_label;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param text '\0' terminated character string. NULL to refresh with the current text.
 */
void edgui_label_set_text(edgui_obj_t * label, const char * text)
{
    edgui_obj_invalidate(label);

    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);

    /*If text is NULL then refresh */
    if(text == NULL) {
        edgui_label_refr_text(label);
        return;
    }

    if(ext->text == text) {
        /*If set its own text then reallocate it (maybe its size changed)*/
        ext->text = edgui_mem_realloc(ext->text, strlen(ext->text) + 1);
        edgui_mem_assert(ext->text);
        if(ext->text == NULL) return;
    } else {
        /*Allocate space for the new text*/
        uint32_t len = strlen(text) + 1;
        if(ext->text != NULL && ext->static_txt == 0) {
            edgui_mem_free(ext->text);
            ext->text = NULL;
        }

        ext->text = edgui_mem_alloc(len);
        edgui_mem_assert(ext->text);
        if(ext->text == NULL) return;

        strcpy(ext->text, text);
        ext->static_txt = 0; /*Now the text is dynamically allocated*/
    }

    edgui_label_refr_text(label);
}

/**
 * Set a new text for a label from a character array. The array don't has to be '\0' terminated.
 * Memory will be allocated to store the array by the label.
 * @param label pointer to a label object
 * @param array array of characters or NULL to refresh the label
 * @param size the size of 'array' in bytes
 */
void edgui_label_set_array_text(edgui_obj_t * label, const char * array, uint16_t size)
{
    edgui_obj_invalidate(label);

    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);

    /*If trying to set its own text or the array is NULL then refresh */
    if(array == ext->text || array == NULL) {
        edgui_label_refr_text(label);
        return;
    }

    /*Allocate space for the new text*/
    if(ext->text != NULL && ext->static_txt == 0) {
        edgui_mem_free(ext->text);
        ext->text = NULL;
    }
    ext->text = edgui_mem_alloc(size + 1);
    edgui_mem_assert(ext->text);
    if(ext->text == NULL) return;

    memcpy(ext->text, array, size);
    ext->text[size] = '\0';
    ext->static_txt = 0; /*Now the text is dynamically allocated*/

    edgui_label_refr_text(label);
}

/**
 * Set a static text. It will not be saved by the label so the 'text' variable
 * has to be 'alive' while the label exist.
 * @param label pointer to a label object
 * @param text pointer to a text. NULL to refresh with the current text.
 */
void edgui_label_set_static_text(edgui_obj_t * label, const char * text)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    if(ext->static_txt == 0 && ext->text != NULL) {
        edgui_mem_free(ext->text);
        ext->text = NULL;
    }

    if(text != NULL) {
        ext->static_txt = 1;
        ext->text       = (char *)text;
    }

    edgui_label_refr_text(label);
}

/**
 * Set the behavior of the label with longer text then the object size
 * @param label pointer to a label object
 * @param long_mode the new mode from 'edgui_label_long_mode' enum.
 *                  In EDGUI_LONG_BREAK/LONG/ROLL the size of the label should be set AFTER this
 * function
 */
void edgui_label_set_long_mode(edgui_obj_t * label, edgui_label_long_mode_t long_mode)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);

#if EDGUI_USE_ANIMATION
    /*Delete the old animation (if exists)*/
    edgui_anim_del(label, (edgui_anim_exec_xcb_t)edgui_obj_set_x);
    edgui_anim_del(label, (edgui_anim_exec_xcb_t)edgui_obj_set_y);
    edgui_anim_del(label, (edgui_anim_exec_xcb_t)edgui_label_set_offset_x);
    edgui_anim_del(label, (edgui_anim_exec_xcb_t)edgui_label_set_offset_y);
#endif
    ext->offset.x = 0;
    ext->offset.y = 0;

    if(long_mode == EDGUI_LABEL_LONG_SROLL || long_mode == EDGUI_LABEL_LONG_SROLL_CIRC || long_mode == EDGUI_LABEL_LONG_CROP)
        ext->expand = 1;
    else
        ext->expand = 0;

    /*Restore the character under the dots*/
    if(ext->long_mode == EDGUI_LABEL_LONG_DOT && ext->dot_end != EDGUI_LABEL_DOT_END_INV) {
        edgui_label_revert_dots(label);
    }

    ext->long_mode = long_mode;
    edgui_label_refr_text(label);
}

/**
 * Set the align of the label (left or center)
 * @param label pointer to a label object
 * @param align 'EDGUI_LABEL_ALIGN_LEFT' or 'EDGUI_LABEL_ALIGN_LEFT'
 */
void edgui_label_set_align(edgui_obj_t * label, edgui_label_align_t align)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    if(ext->align == align) return;

    ext->align = align;

    edgui_obj_invalidate(label); /*Enough to invalidate because alignment is only drawing related
                                 (edgui_refr_label_text() not required)*/
}

/**
 * Enable the recoloring by in-line commands
 * @param label pointer to a label object
 * @param en true: enable recoloring, false: disable
 */
void edgui_label_set_recolor(edgui_obj_t * label, bool en)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    if(ext->recolor == en) return;

    ext->recolor = en == false ? 0 : 1;

    edgui_label_refr_text(label); /*Refresh the text because the potential colo codes in text needs to
                                  be hided or revealed*/
}

/**
 * Set the label to draw (or not draw) background specified in its style's body
 * @param label pointer to a label object
 * @param en true: draw body; false: don't draw body
 */
void edgui_label_set_body_draw(edgui_obj_t * label, bool en)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    if(ext->body_draw == en) return;

    ext->body_draw = en == false ? 0 : 1;

    edgui_obj_refresh_ext_draw_pad(label);

    edgui_obj_invalidate(label);
}

/**
 * Set the label's animation speed in EDGUI_LABEL_LONG_SROLL/SCROLL_CIRC modes
 * @param label pointer to a label object
 * @param anim_speed speed of animation in px/sec unit
 */
void edgui_label_set_anim_speed(edgui_obj_t * label, uint16_t anim_speed)
{
#if EDGUI_USE_ANIMATION
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    if(ext->anim_speed == anim_speed) return;

    ext->anim_speed = anim_speed;

    if(ext->long_mode == EDGUI_LABEL_LONG_SROLL || ext->long_mode == EDGUI_LABEL_LONG_SROLL_CIRC) {
        edgui_label_refr_text(label);
    }
#else
    (void)label;      /*Unused*/
    (void)anim_speed; /*Unused*/
#endif
}

void edgui_label_set_text_sel_start(edgui_obj_t * label, uint16_t index)
{
#if EDGUI_LABEL_TEXT_SEL
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    ext->txt_sel_start   = index;
    edgui_obj_invalidate(label);
#else
    (void)label;    /*Unused*/
    (void)index;    /*Unused*/
#endif
}

void edgui_label_set_text_sel_end(edgui_obj_t * label, uint16_t index)
{
#if EDGUI_LABEL_TEXT_SEL
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    ext->txt_sel_end     = index;
    edgui_obj_invalidate(label);
#else
    (void)label;    /*Unused*/
    (void)index;    /*Unused*/
#endif
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a label
 * @param label pointer to a label object
 * @return the text of the label
 */
char * edgui_label_get_text(const edgui_obj_t * label)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);

    return ext->text;
}

/**
 * Get the long mode of a label
 * @param label pointer to a label object
 * @return the long mode
 */
edgui_label_long_mode_t edgui_label_get_long_mode(const edgui_obj_t * label)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    return ext->long_mode;
}

/**
 * Get the align attribute
 * @param label pointer to a label object
 * @return EDGUI_LABEL_ALIGN_LEFT or EDGUI_LABEL_ALIGN_CENTER
 */
edgui_label_align_t edgui_label_get_align(const edgui_obj_t * label)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    return ext->align;
}

/**
 * Get the recoloring attribute
 * @param label pointer to a label object
 * @return true: recoloring is enabled, false: disable
 */
bool edgui_label_get_recolor(const edgui_obj_t * label)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    return ext->recolor == 0 ? false : true;
}

/**
 * Get the body draw attribute
 * @param label pointer to a label object
 * @return true: draw body; false: don't draw body
 */
bool edgui_label_get_body_draw(const edgui_obj_t * label)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    return ext->body_draw == 0 ? false : true;
}

/**
 * Get the label's animation speed in EDGUI_LABEL_LONG_ROLL and SCROLL modes
 * @param label pointer to a label object
 * @return speed of animation in px/sec unit
 */
uint16_t edgui_label_get_anim_speed(const edgui_obj_t * label)
{
#if EDGUI_USE_ANIMATION
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    return ext->anim_speed;
#else
    (void)label;      /*Unused*/
    return 0;
#endif
}

/**
 * Get the relative x and y coordinates of a letter
 * @param label pointer to a label object
 * @param index index of the letter [0 ... text length]. Expressed in character index, not byte
 * index (different in UTF-8)
 * @param pos store the result here (E.g. index = 0 gives 0;0 coordinates)
 */
void edgui_label_get_letter_pos(const edgui_obj_t * label, uint16_t index, edgui_point_t * pos)
{
    const char * txt         = edgui_label_get_text(label);
    edgui_label_ext_t * ext     = edgui_obj_get_ext_attr(label);
    uint32_t line_start      = 0;
    uint32_t new_line_start  = 0;
    edgui_coord_t max_w         = edgui_obj_get_width(label);
    const edgui_style_t * style = edgui_obj_get_style(label);
    const edgui_font_t * font   = style->text.font;
    uint8_t letter_height    = edgui_font_get_line_height(font);
    edgui_coord_t y             = 0;
    edgui_txt_flag_t flag       = EDGUI_TXT_FLAG_NONE;

    if(ext->recolor != 0) flag |= EDGUI_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= EDGUI_TXT_FLAG_EXPAND;
    if(ext->align == EDGUI_LABEL_ALIGN_CENTER) flag |= EDGUI_TXT_FLAG_CENTER;

    /*If the width will be expanded  the set the max length to very big */
    if(ext->long_mode == EDGUI_LABEL_LONG_EXPAND) {
        max_w = EDGUI_COORD_MAX;
    }

    index = edgui_txt_encoded_get_byte_id(txt, index);

    /*Search the line of the index letter */;
    while(txt[new_line_start] != '\0') {
        new_line_start += edgui_txt_get_next_line(&txt[line_start], font, style->text.letter_space, max_w, flag);
        if(index < new_line_start || txt[new_line_start] == '\0')
            break; /*The line of 'index' letter begins at 'line_start'*/

        y += letter_height + style->text.line_space;
        line_start = new_line_start;
    }

    /*If the last character is line break then go to the next line*/
    if(index > 0) {
        if((txt[index - 1] == '\n' || txt[index - 1] == '\r') && txt[index] == '\0') {
            y += letter_height + style->text.line_space;
            line_start = index;
        }
    }

    /*Calculate the x coordinate*/
    edgui_coord_t x = edgui_txt_get_width(&txt[line_start], index - line_start, font, style->text.letter_space, flag);

    if(index != line_start) x += style->text.letter_space;

    if(ext->align == EDGUI_LABEL_ALIGN_CENTER) {
        edgui_coord_t line_w;
        line_w = edgui_txt_get_width(&txt[line_start], new_line_start - line_start, font, style->text.letter_space, flag);
        x += edgui_obj_get_width(label) / 2 - line_w / 2;

    } else if(ext->align == EDGUI_LABEL_ALIGN_RIGHT) {
        edgui_coord_t line_w;
        line_w = edgui_txt_get_width(&txt[line_start], new_line_start - line_start, font, style->text.letter_space, flag);

        x += edgui_obj_get_width(label) - line_w;
    }
    pos->x = x;
    pos->y = y;
}

/**
 * Get the index of letter on a relative point of a label
 * @param label pointer to label object
 * @param pos pointer to point with coordinates on a the label
 * @return the index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter)
 * Expressed in character index and not byte index (different in UTF-8)
 */
uint16_t edgui_label_get_letter_on(const edgui_obj_t * label, edgui_point_t * pos)
{
    const char * txt         = edgui_label_get_text(label);
    edgui_label_ext_t * ext     = edgui_obj_get_ext_attr(label);
    uint32_t line_start      = 0;
    uint32_t new_line_start  = 0;
    edgui_coord_t max_w         = edgui_obj_get_width(label);
    const edgui_style_t * style = edgui_obj_get_style(label);
    const edgui_font_t * font   = style->text.font;
    uint8_t letter_height    = edgui_font_get_line_height(font);
    edgui_coord_t y             = 0;
    edgui_txt_flag_t flag       = EDGUI_TXT_FLAG_NONE;

    if(ext->recolor != 0) flag |= EDGUI_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= EDGUI_TXT_FLAG_EXPAND;
    if(ext->align == EDGUI_LABEL_ALIGN_CENTER) flag |= EDGUI_TXT_FLAG_CENTER;

    /*If the width will be expanded set the max length to very big */
    if(ext->long_mode == EDGUI_LABEL_LONG_EXPAND) {
        max_w = EDGUI_COORD_MAX;
    }

    /*Search the line of the index letter */;
    while(txt[line_start] != '\0') {
        new_line_start += edgui_txt_get_next_line(&txt[line_start], font, style->text.letter_space, max_w, flag);

        if(pos->y <= y + letter_height) break; /*The line is found (stored in 'line_start')*/
        y += letter_height + style->text.line_space;

        line_start = new_line_start;
    }

    /*Calculate the x coordinate*/
    edgui_coord_t x = 0;
    if(ext->align == EDGUI_LABEL_ALIGN_CENTER) {
        edgui_coord_t line_w;
        line_w = edgui_txt_get_width(&txt[line_start], new_line_start - line_start, font, style->text.letter_space, flag);
        x += edgui_obj_get_width(label) / 2 - line_w / 2;
    }

    edgui_txt_cmd_state_t cmd_state = EDGUI_TXT_CMD_STATE_WAIT;

    uint32_t i         = line_start;
    uint32_t i_act = i;
    uint32_t letter;
    uint32_t letter_next;

    if(new_line_start > 0) {
        while(i < new_line_start) {
            /* Get the current letter.*/
            letter = edgui_txt_encoded_next(txt, &i);

            /*Get the next letter too for kerning*/
            letter_next = edgui_txt_encoded_next(&txt[i], NULL);

            /*Handle the recolor command*/
            if((flag & EDGUI_TXT_FLAG_RECOLOR) != 0) {
                if(edgui_txt_is_cmd(&cmd_state, txt[i]) != false) {
                    continue; /*Skip the letter is it is part of a command*/
                }
            }

            x += edgui_font_get_glyph_width(font, letter, letter_next);

            /*Finish if the x position or the last char of the line is reached*/
            if(pos->x < x || i == new_line_start) {
                i = i_act;
                break;
            }
            x += style->text.letter_space;
            i_act = i;
        }
    }

    return edgui_encoded_get_char_id(txt, i);
}

/**
 * @brief Get the selection start index.
 * @param label pointer to a label object.
 * @return selection start index. `EDGUI_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint16_t edgui_label_get_text_sel_start(const edgui_obj_t * label)
{
#if EDGUI_LABEL_TEXT_SEL
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    return ext->txt_sel_start;

#else
    (void)label;    /*Unused*/
    return EDGUI_LABEL_TEXT_SEL_OFF;
#endif
}

/**
 * @brief Get the selection end index.
 * @param label pointer to a label object.
 * @return selection end index. `EDGUI_LABEL_TXT_SEL_OFF` if nothing is selected.
 */
uint16_t edgui_label_get_text_sel_end(const edgui_obj_t * label)
{
#if EDGUI_LABEL_TEXT_SEL
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    return ext->txt_sel_end;
#else
    (void)label; /*Unused*/
    return EDGUI_LABEL_TEXT_SEL_OFF;
#endif
}

/**
 * Check if a character is drawn under a point.
 * @param label Label object
 * @param pos Point to check for characte under
 * @return whether a character is drawn under the point
 */
bool edgui_label_is_char_under_pos(const edgui_obj_t * label, edgui_point_t * pos)
{
    const char * txt         = edgui_label_get_text(label);
    edgui_label_ext_t * ext     = edgui_obj_get_ext_attr(label);
    uint32_t line_start      = 0;
    uint32_t new_line_start  = 0;
    edgui_coord_t max_w         = edgui_obj_get_width(label);
    const edgui_style_t * style = edgui_obj_get_style(label);
    const edgui_font_t * font   = style->text.font;
    uint8_t letter_height    = edgui_font_get_line_height(font);
    edgui_coord_t y             = 0;
    edgui_txt_flag_t flag       = EDGUI_TXT_FLAG_NONE;

    if(ext->recolor != 0) flag |= EDGUI_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= EDGUI_TXT_FLAG_EXPAND;
    if(ext->align == EDGUI_LABEL_ALIGN_CENTER) flag |= EDGUI_TXT_FLAG_CENTER;

    /*If the width will be expanded set the max length to very big */
    if(ext->long_mode == EDGUI_LABEL_LONG_EXPAND) {
        max_w = EDGUI_COORD_MAX;
    }

    /*Search the line of the index letter */;
    while(txt[line_start] != '\0') {
        new_line_start += edgui_txt_get_next_line(&txt[line_start], font, style->text.letter_space, max_w, flag);

        if(pos->y <= y + letter_height) break; /*The line is found (stored in 'line_start')*/
        y += letter_height + style->text.line_space;

        line_start = new_line_start;
    }

    /*Calculate the x coordinate*/
    edgui_coord_t x      = 0;
    edgui_coord_t last_x = 0;
    if(ext->align == EDGUI_LABEL_ALIGN_CENTER) {
        edgui_coord_t line_w;
        line_w = edgui_txt_get_width(&txt[line_start], new_line_start - line_start, font, style->text.letter_space, flag);
        x += edgui_obj_get_width(label) / 2 - line_w / 2;
    }

    edgui_txt_cmd_state_t cmd_state = EDGUI_TXT_CMD_STATE_WAIT;

    uint32_t i           = line_start;
    uint32_t i_current   = i;
    uint32_t letter      = '\0';
    uint32_t letter_next = '\0';

    if(new_line_start > 0) {
        while(i <= new_line_start - 1) {
            /* Get the current letter
             * Be careful 'i' already points to the next character */
            letter = edgui_txt_encoded_next(txt, &i);

            /*Get the next letter for kerning*/
            letter_next = edgui_txt_encoded_next(&txt[i], NULL);

            /*Handle the recolor command*/
            if((flag & EDGUI_TXT_FLAG_RECOLOR) != 0) {
                if(edgui_txt_is_cmd(&cmd_state, txt[i]) != false) {
                    continue; /*Skip the letter is it is part of a command*/
                }
            }
            last_x = x;
            x += edgui_font_get_glyph_width(font, letter, letter_next);
            if(pos->x < x) {
                i = i_current;
                break;
            }
            x += style->text.letter_space;
            i_current = i;
        }
    }

    int32_t max_diff = edgui_font_get_glyph_width(font, letter, letter_next) + style->text.letter_space + 1;
    return (pos->x >= (last_x - style->text.letter_space) && pos->x <= (last_x + max_diff));
}

/*=====================
 * Other functions
 *====================*/

/**
 * Insert a text to the label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different
 * in UTF-8) 0: before first char. EDGUI_LABEL_POS_LAST: after last char.
 * @param txt pointer to the text to insert
 */
void edgui_label_ins_text(edgui_obj_t * label, uint32_t pos, const char * txt)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);

    /*Can not append to static text*/
    if(ext->static_txt != 0) return;

    edgui_obj_invalidate(label);

    /*Allocate space for the new text*/
    uint32_t old_len = strlen(ext->text);
    uint32_t ins_len = strlen(txt);
    uint32_t new_len = ins_len + old_len;
    ext->text        = edgui_mem_realloc(ext->text, new_len + 1);
    edgui_mem_assert(ext->text);
    if(ext->text == NULL) return;

    if(pos == EDGUI_LABEL_POS_LAST) {
        pos = edgui_txt_get_encoded_length(ext->text);
    }

    edgui_txt_ins(ext->text, pos, txt);

    edgui_label_refr_text(label);
}

/**
 * Delete characters from a label. The label text can not be static.
 * @param label pointer to a label object
 * @param pos character index to insert. Expressed in character index and not byte index (Different
 * in UTF-8) 0: before first char.
 * @param cnt number of characters to cut
 */
void edgui_label_cut_text(edgui_obj_t * label, uint32_t pos, uint32_t cnt)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);

    /*Can not append to static text*/
    if(ext->static_txt != 0) return;

    edgui_obj_invalidate(label);

    char * label_txt = edgui_label_get_text(label);
    /*Delete the characters*/
    edgui_txt_cut(label_txt, pos, cnt);

    /*Refresh the label*/
    edgui_label_refr_text(label);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the labels
 * @param label pointer to a label object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_label_design(edgui_obj_t * label, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    /* A label never covers an area */
    if(mode == EDGUI_DESIGN_COVER_CHK)
        return false;
    else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        edgui_area_t coords;
        const edgui_style_t * style = edgui_obj_get_style(label);
        edgui_opa_t opa_scale       = edgui_obj_get_opa_scale(label);
        edgui_obj_get_coords(label, &coords);

#if EDGUI_USE_GROUP
        edgui_group_t * g = edgui_obj_get_group(label);
        if(edgui_group_get_focused(g) == label) {
            edgui_draw_rect(&coords, mask, style, opa_scale);
        }
#endif

        edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);

        if(ext->body_draw) {
            edgui_area_t bg;
            edgui_obj_get_coords(label, &bg);
            bg.x1 -= style->body.padding.left;
            bg.x2 += style->body.padding.right;
            bg.y1 -= style->body.padding.top;
            bg.y2 += style->body.padding.bottom;

            edgui_draw_rect(&bg, mask, style, edgui_obj_get_opa_scale(label));
        }

        /*TEST: draw a background for the label*/
        // edgui_draw_rect(&label->coords, mask, &edgui_style_plain_color, EDGUI_OPA_COVER);

        edgui_txt_flag_t flag = EDGUI_TXT_FLAG_NONE;
        if(ext->recolor != 0) flag |= EDGUI_TXT_FLAG_RECOLOR;
        if(ext->expand != 0) flag |= EDGUI_TXT_FLAG_EXPAND;
        if(ext->align == EDGUI_LABEL_ALIGN_CENTER) flag |= EDGUI_TXT_FLAG_CENTER;
        if(ext->align == EDGUI_LABEL_ALIGN_RIGHT) flag |= EDGUI_TXT_FLAG_RIGHT;

        /* In ROLL mode the CENTER and RIGHT are pointless so remove them.
         * (In addition they will result mis-alignment is this case)*/
        if((ext->long_mode == EDGUI_LABEL_LONG_SROLL || ext->long_mode == EDGUI_LABEL_LONG_SROLL_CIRC) &&
           (ext->align == EDGUI_LABEL_ALIGN_CENTER || ext->align == EDGUI_LABEL_ALIGN_RIGHT)) {
            edgui_point_t size;
            edgui_txt_get_size(&size, ext->text, style->text.font, style->text.letter_space, style->text.line_space,
                            EDGUI_COORD_MAX, flag);
            if(size.x > edgui_obj_get_width(label)) {
                flag &= ~EDGUI_TXT_FLAG_RIGHT;
                flag &= ~EDGUI_TXT_FLAG_CENTER;
            }
        }
#if EDGUI_LABEL_LONG_TXT_HINT
        edgui_draw_label_hint_t * hint = &ext->hint;
        if(ext->long_mode == EDGUI_LABEL_LONG_SROLL_CIRC || edgui_obj_get_height(label) < EDGUI_LABEL_HINT_HEIGHT_LIMIT)
            hint = NULL;

#else
        /*Just for compatibility*/
        edgui_draw_label_hint_t * hint = NULL;
#endif
        edgui_draw_label(&coords, mask, style, opa_scale, ext->text, flag, &ext->offset,
                              edgui_label_get_text_sel_start(label), edgui_label_get_text_sel_end(label), hint);


        if(ext->long_mode == EDGUI_LABEL_LONG_SROLL_CIRC) {
            edgui_point_t size;
            edgui_txt_get_size(&size, ext->text, style->text.font, style->text.letter_space, style->text.line_space,
                            EDGUI_COORD_MAX, flag);

            edgui_point_t ofs;

            /*Draw the text again next to the original to make an circular effect */
            if(size.x > edgui_obj_get_width(label)) {
                ofs.x = ext->offset.x + size.x +
                        edgui_font_get_glyph_width(style->text.font, ' ', ' ') * EDGUI_LABEL_WAIT_CHAR_COUNT;
                ofs.y = ext->offset.y;

                edgui_draw_label(&coords, mask, style, opa_scale, ext->text, flag, &ofs,
                              edgui_label_get_text_sel_start(label), edgui_label_get_text_sel_end(label), NULL);
            }

            /*Draw the text again below the original to make an circular effect */
            if(size.y > edgui_obj_get_height(label)) {
                ofs.x = ext->offset.x;
                ofs.y = ext->offset.y + size.y + edgui_font_get_line_height(style->text.font);
                edgui_draw_label(&coords, mask, style, opa_scale, ext->text, flag, &ofs,
                              edgui_label_get_text_sel_start(label), edgui_label_get_text_sel_end(label), NULL);
            }
        }
    }
    return true;
}

/**
 * Signal function of the label
 * @param label pointer to a label object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_label_signal(edgui_obj_t * label, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(label, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    if(sign == EDGUI_SIGNAL_CLEANUP) {
        if(ext->static_txt == 0) {
            edgui_mem_free(ext->text);
            ext->text = NULL;
        }
        edgui_label_dot_tmp_free(label);
    } else if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        /*Revert dots for proper refresh*/
        edgui_label_revert_dots(label);

        edgui_label_refr_text(label);
    } else if(sign == EDGUI_SIGNAL_CORD_CHG) {
        if(edgui_area_get_width(&label->coords) != edgui_area_get_width(param) ||
           edgui_area_get_height(&label->coords) != edgui_area_get_height(param)) {
            edgui_label_revert_dots(label);
            edgui_label_refr_text(label);
        }
    } else if(sign == EDGUI_SIGNAL_REFR_EXT_DRAW_PAD) {
        if(ext->body_draw) {
            const edgui_style_t * style = edgui_label_get_style(label, EDGUI_LABEL_STYLE_MAIN);

            label->ext_draw_pad = EDGUI_MATH_MAX(label->ext_draw_pad, style->body.padding.left);
            label->ext_draw_pad = EDGUI_MATH_MAX(label->ext_draw_pad, style->body.padding.right);
            label->ext_draw_pad = EDGUI_MATH_MAX(label->ext_draw_pad, style->body.padding.top);
            label->ext_draw_pad = EDGUI_MATH_MAX(label->ext_draw_pad, style->body.padding.bottom);
        }
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_label";
    }

    return res;
}

/**
 * Refresh the label with its text stored in its extended data
 * @param label pointer to a label object
 */
static void edgui_label_refr_text(edgui_obj_t * label)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);

    if(ext->text == NULL) return;
#if EDGUI_LABEL_LONG_TXT_HINT
    ext->hint.line_start = -1; /*The hint is invalid if the text changes*/
#endif

    edgui_coord_t max_w         = edgui_obj_get_width(label);
    const edgui_style_t * style = edgui_obj_get_style(label);
    const edgui_font_t * font   = style->text.font;

    /*If the width will be expanded set the max length to very big */
    if(ext->long_mode == EDGUI_LABEL_LONG_EXPAND) {
        max_w = EDGUI_COORD_MAX;
    }

    /*Calc. the height and longest line*/
    edgui_point_t size;
    edgui_txt_flag_t flag = EDGUI_TXT_FLAG_NONE;
    if(ext->recolor != 0) flag |= EDGUI_TXT_FLAG_RECOLOR;
    if(ext->expand != 0) flag |= EDGUI_TXT_FLAG_EXPAND;
    edgui_txt_get_size(&size, ext->text, font, style->text.letter_space, style->text.line_space, max_w, flag);

    /*Set the full size in expand mode*/
    if(ext->long_mode == EDGUI_LABEL_LONG_EXPAND) {
        edgui_obj_set_size(label, size.x, size.y);
    }
    /*In roll mode keep the size but start offset animations*/
    else if(ext->long_mode == EDGUI_LABEL_LONG_SROLL) {
#if EDGUI_USE_ANIMATION
        edgui_anim_t anim;
        anim.var      = label;
        anim.repeat   = 1;
        anim.playback = 1;
        anim.start    = 0;
        anim.ready_cb = NULL;
        anim.path_cb  = edgui_anim_path_linear;
        anim.playback_pause =
            (((edgui_font_get_glyph_width(style->text.font, ' ', ' ') + style->text.letter_space) * 1000) /
             ext->anim_speed) *
            EDGUI_LABEL_WAIT_CHAR_COUNT;
        anim.repeat_pause = anim.playback_pause;
        anim.act_time     = -anim.playback_pause;

        bool hor_anim = false;
        if(size.x > edgui_obj_get_width(label)) {
            anim.end     = edgui_obj_get_width(label) - size.x;
            anim.exec_cb = (edgui_anim_exec_xcb_t)edgui_label_set_offset_x;
            anim.time    = edgui_anim_speed_to_time(ext->anim_speed, anim.start, anim.end);
            edgui_anim_create(&anim);
            hor_anim = true;
        } else {
            /*Delete the offset animation if not required*/
            edgui_anim_del(label, (edgui_anim_exec_xcb_t)edgui_label_set_offset_x);
            ext->offset.x = 0;
        }

        if(size.y > edgui_obj_get_height(label) && hor_anim == false) {
            anim.end     = edgui_obj_get_height(label) - size.y - (edgui_font_get_line_height(font));
            anim.exec_cb = (edgui_anim_exec_xcb_t)edgui_label_set_offset_y;

            anim.time = edgui_anim_speed_to_time(ext->anim_speed, anim.start, anim.end);
            edgui_anim_create(&anim);
        } else {
            /*Delete the offset animation if not required*/
            edgui_anim_del(label, (edgui_anim_exec_xcb_t)edgui_label_set_offset_y);
            ext->offset.y = 0;
        }
#endif
    }
    /*In roll inf. mode keep the size but start offset animations*/
    else if(ext->long_mode == EDGUI_LABEL_LONG_SROLL_CIRC) {
#if EDGUI_USE_ANIMATION
        edgui_anim_t anim;
        anim.var      = label;
        anim.repeat   = 1;
        anim.playback = 0;
        anim.start    = 0;
        anim.act_time = -(((edgui_font_get_glyph_width(style->text.font, ' ', ' ') + style->text.letter_space) * 1000) /
                          ext->anim_speed) *
                        EDGUI_LABEL_WAIT_CHAR_COUNT;
        anim.ready_cb       = NULL;
        anim.path_cb        = edgui_anim_path_linear;
        anim.playback_pause = 0;
        anim.repeat_pause   = 0;

        bool hor_anim = false;
        if(size.x > edgui_obj_get_width(label)) {
            anim.end     = -size.x - edgui_font_get_glyph_width(font, ' ', ' ') * EDGUI_LABEL_WAIT_CHAR_COUNT;
            anim.exec_cb = (edgui_anim_exec_xcb_t)edgui_label_set_offset_x;
            anim.time    = edgui_anim_speed_to_time(ext->anim_speed, anim.start, anim.end);
            edgui_anim_create(&anim);
            hor_anim = true;
        } else {
            /*Delete the offset animation if not required*/
            edgui_anim_del(label, (edgui_anim_exec_xcb_t)edgui_label_set_offset_x);
            ext->offset.x = 0;
        }

        if(size.y > edgui_obj_get_height(label) && hor_anim == false) {
            anim.end     = -size.y - (edgui_font_get_line_height(font));
            anim.exec_cb = (edgui_anim_exec_xcb_t)edgui_label_set_offset_y;
            anim.time    = edgui_anim_speed_to_time(ext->anim_speed, anim.start, anim.end);
            edgui_anim_create(&anim);
        } else {
            /*Delete the offset animation if not required*/
            edgui_anim_del(label, (edgui_anim_exec_xcb_t)edgui_label_set_offset_y);
            ext->offset.y = 0;
        }
#endif
    } else if(ext->long_mode == EDGUI_LABEL_LONG_DOT) {
        if(size.y <= edgui_obj_get_height(label)) { /*No dots are required, the text is short enough*/
            ext->dot_end = EDGUI_LABEL_DOT_END_INV;
        } else if(edgui_txt_get_encoded_length(ext->text) <= EDGUI_LABEL_DOT_NUM) { /*Don't turn to dots all the characters*/
            ext->dot_end = EDGUI_LABEL_DOT_END_INV;
        } else {
            edgui_point_t p;
            p.x = edgui_obj_get_width(label) -
                  (edgui_font_get_glyph_width(style->text.font, '.', '.') + style->text.letter_space) *
                      EDGUI_LABEL_DOT_NUM; /*Shrink with dots*/
            p.y = edgui_obj_get_height(label);
            p.y -= p.y %
                   (edgui_font_get_line_height(style->text.font) + style->text.line_space); /*Round down to the last line*/
            p.y -= style->text.line_space;                                               /*Trim the last line space*/
            uint32_t letter_id = edgui_label_get_letter_on(label, &p);

            /*Save letters under the dots and replace them with dots*/
            uint32_t i;
            uint32_t byte_id     = edgui_txt_encoded_get_byte_id(ext->text, letter_id);
            uint32_t byte_id_ori = byte_id;
            uint8_t len          = 0;
            for(i = 0; i <= EDGUI_LABEL_DOT_NUM; i++) {
                len += edgui_txt_encoded_size(&ext->text[byte_id]);
                edgui_txt_encoded_next(ext->text, &byte_id);
            }

            if(edgui_label_set_dot_tmp(label, &ext->text[byte_id_ori], len)) {
                for(i = 0; i < EDGUI_LABEL_DOT_NUM; i++) {
                    ext->text[byte_id_ori + i] = '.';
                }
                ext->text[byte_id_ori + EDGUI_LABEL_DOT_NUM] = '\0';
                ext->dot_end                              = letter_id + EDGUI_LABEL_DOT_NUM;
            }
        }
    }
    /*In break mode only the height can change*/
    else if(ext->long_mode == EDGUI_LABEL_LONG_BREAK) {
        edgui_obj_set_height(label, size.y);
    }
    /*Do not set the size in Clip mode*/
    else if(ext->long_mode == EDGUI_LABEL_LONG_CROP) {
        /*Do nothing*/
    }

    edgui_obj_invalidate(label);
}

static void edgui_label_revert_dots(edgui_obj_t * label)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    if(ext->long_mode != EDGUI_LABEL_LONG_DOT) return;
    if(ext->dot_end == EDGUI_LABEL_DOT_END_INV) return;
    uint32_t letter_i = ext->dot_end - EDGUI_LABEL_DOT_NUM;
    uint32_t byte_i   = edgui_txt_encoded_get_byte_id(ext->text, letter_i);

    /*Restore the characters*/
    uint8_t i      = 0;
    char * dot_tmp = edgui_label_get_dot_tmp(label);
    while(ext->text[byte_i + i] != '\0') {
        ext->text[byte_i + i] = dot_tmp[i];
        i++;
    }
    ext->text[byte_i + i] = dot_tmp[i];
    edgui_label_dot_tmp_free(label);

    ext->dot_end = EDGUI_LABEL_DOT_END_INV;
}

#if EDGUI_USE_ANIMATION
static void edgui_label_set_offset_x(edgui_obj_t * label, edgui_coord_t x)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    ext->offset.x        = x;
    edgui_obj_invalidate(label);
}

static void edgui_label_set_offset_y(edgui_obj_t * label, edgui_coord_t y)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    ext->offset.y        = y;
    edgui_obj_invalidate(label);
}
#endif

/**
 * Store `len` characters from `data`. Allocates space if necessary.
 *
 * @param label pointer to label object
 * @param len Number of characters to store.
 * @return true on success.
 */
static bool edgui_label_set_dot_tmp(edgui_obj_t * label, char * data, uint16_t len)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    edgui_label_dot_tmp_free(label); /* Deallocate any existing space */
    if(len > sizeof(char *)) {
        /* Memory needs to be allocated. Allocates an additional byte
         * for a NULL-terminator so it can be copied. */
        ext->dot.tmp_ptr = edgui_mem_alloc(len + 1);
        if(ext->dot.tmp_ptr == NULL) {
            EDGUI_LOG_ERROR("Failed to allocate memory for dot_tmp_ptr");
            return false;
        }
        memcpy(ext->dot.tmp_ptr, data, len);
        ext->dot.tmp_ptr[len] = '\0';
        ext->dot_tmp_alloc    = true;
    } else {
        /* Characters can be directly stored in object */
        ext->dot_tmp_alloc = false;
        memcpy(ext->dot.tmp, data, len);
    }
    return true;
}

/**
 * Get the stored dot_tmp characters
 * @param label pointer to label object
 * @return char pointer to a stored characters. Is *not* necessarily NULL-terminated.
 */
static char * edgui_label_get_dot_tmp(edgui_obj_t * label)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    if(ext->dot_tmp_alloc) {
        return ext->dot.tmp_ptr;
    } else {
        return ext->dot.tmp;
    }
}

/**
 * Free the dot_tmp_ptr field if it was previously allocated.
 * Always clears the field
 * @param label pointer to label object.
 */
static void edgui_label_dot_tmp_free(edgui_obj_t * label)
{
    edgui_label_ext_t * ext = edgui_obj_get_ext_attr(label);
    if(ext->dot_tmp_alloc && ext->dot.tmp_ptr) {
        edgui_mem_free(ext->dot.tmp_ptr);
    }
    ext->dot_tmp_alloc = false;
    ext->dot.tmp_ptr   = NULL;
}

#endif
