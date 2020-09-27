/**
 * @file terminal.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "terminal.h"
#if EDGUI_USE_TERMINAL

/*********************
 *      DEFINES
 *********************/
#define TERMINAL_ANIM_TIME   100 /*[ms]*/
#define TERMINAL_NO_INPUT    0   /*Do not create Text area and Keyboard*/
#define TERMINAL_LOG_LENGTH  512        /*Characters*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void clr_event_cb(edgui_obj_t * btn, edgui_event_t event);
static void win_close_action(edgui_obj_t * btn, edgui_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_obj_t * win;
static char txt_log[TERMINAL_LOG_LENGTH + 1];
static edgui_obj_t * label;
static edgui_obj_t * clr_btn;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Open a terminal
 * @return pointer to the terminal window
 */
edgui_obj_t * terminal_create(void)
{
    static edgui_style_t style_bg;
    edgui_style_copy(&style_bg, &edgui_style_pretty);
    style_bg.body.main_color = edgui_color_make(0x30, 0x30, 0x30);
    style_bg.body.grad_color = edgui_color_make(0x30, 0x30, 0x30);
    style_bg.body.border.color = EDGUI_COLOR_WHITE;
    style_bg.text.color = edgui_color_make(0xE0, 0xE0, 0xE0);



    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    win = edgui_win_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_win_set_style(win, EDGUI_WIN_STYLE_BG, &style_bg);
    edgui_obj_set_size(win, hres, vres);
    edgui_win_set_sb_mode(win, EDGUI_SB_MODE_AUTO);
    edgui_obj_t * win_btn = edgui_win_add_btn(win, EDGUI_SYMBOL_CLOSE);
    edgui_obj_set_event_cb(win_btn, win_close_action);

    /*Make the window's content responsive*/
    edgui_win_set_layout(win, EDGUI_LAYOUT_PRETTY);

    /*Create a label for the text of the terminal*/
    label = edgui_label_create(win, NULL);
    edgui_label_set_long_mode(label, EDGUI_LABEL_LONG_BREAK);
    edgui_obj_set_width(label, edgui_win_get_width(win));
    edgui_label_set_static_text(label, txt_log);               /*Use the text array directly*/

    /*Create a clear button*/
    clr_btn = edgui_btn_create(win, NULL);
    edgui_btn_set_fit(clr_btn, EDGUI_FIT_TIGHT);
    edgui_obj_set_event_cb(clr_btn, clr_event_cb);
    edgui_obj_t * btn_label = edgui_label_create(clr_btn, NULL);
    edgui_label_set_text(btn_label, "Clear");

    return win;
}

/**
 * Add data to the terminal
 * @param txt_in character sting to add to the terminal
 */
void terminal_add(const char * txt_in)
{
    if(win == NULL) return;                 /*Check if the window is exists*/

    uint16_t txt_len = strlen(txt_in);
    uint16_t old_len = strlen(txt_log);

    /*If the data is longer then the terminal ax size show the last part of data*/
    if(txt_len > TERMINAL_LOG_LENGTH) {
        txt_in += (txt_len - TERMINAL_LOG_LENGTH);
        txt_len = TERMINAL_LOG_LENGTH;
        old_len = 0;
    }
    /*If the text become too long 'forget' the oldest lines*/
    else if(old_len + txt_len > TERMINAL_LOG_LENGTH) {
        uint16_t new_start;
        for(new_start = 0; new_start < old_len; new_start++) {
            if(txt_log[new_start] == '\n') {
                /*If there is enough space break*/
                if(new_start >= txt_len) {
                    /*Ignore line breaks*/
                    while(txt_log[new_start] == '\n' || txt_log[new_start] == '\r') new_start++;
                    break;
                }
            }
        }

        /* If it wasn't able to make enough space on line breaks
         * simply forget the oldest characters*/
        if(new_start == old_len) {
            new_start = old_len - (TERMINAL_LOG_LENGTH - txt_len);
        }
        /*Move the remaining text to the beginning*/
        uint16_t j;
        for(j = new_start; j < old_len; j++) {
            txt_log[j - new_start] = txt_log[j];
        }
        old_len = old_len - new_start;
        txt_log[old_len] = '\0';

    }

    memcpy(&txt_log[old_len], txt_in, txt_len);
    txt_log[old_len + txt_len] = '\0';

    edgui_label_set_static_text(label, txt_log);
    edgui_win_focus(win, clr_btn, TERMINAL_ANIM_TIME);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called when the Clear button is click to clear the text of the terminal
 * @param btn pointer to the clear button
 * @param event the current event
 */
static void clr_event_cb(edgui_obj_t * btn, edgui_event_t event)
{
    (void) btn;    /*Unused*/

    if(event != EDGUI_EVENT_CLICKED) return;

    txt_log[0] = '\0';
    edgui_label_set_static_text(label, txt_log);   /*Refresh the text*/
}

/**
 * Called when the window's close button is clicked
 * @param btn pointer to the close button
 * @return EDGUI_ACTION_RES_INV because the button is deleted in the function
 */
static void win_close_action(edgui_obj_t * btn, edgui_event_t event)
{
    (void) btn;    /*Unused*/

    if(event != EDGUI_EVENT_CLICKED) return;

    edgui_obj_del(win);
    win = NULL;
}

#endif /*EDGUI_USE_TERMINAL*/
