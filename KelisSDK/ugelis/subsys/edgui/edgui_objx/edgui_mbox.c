/**
 * @file edgui_mbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_mbox.h"
#if EDGUI_USE_MBOX != 0

#include "../edgui_core/edgui_group.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_anim.h"
#include "../edgui_misc/edgui_math.h"

/*********************
 *      DEFINES
 *********************/

#if EDGUI_USE_ANIMATION
#ifndef EDGUI_MBOX_CLOSE_ANIM_TIME
#define EDGUI_MBOX_CLOSE_ANIM_TIME 200 /*List close animation time)  */
#endif
#else
#undef EDGUI_MBOX_CLOSE_ANIM_TIME
#define EDGUI_MBOX_CLOSE_ANIM_TIME 0 /*No animations*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_mbox_signal(edgui_obj_t * mbox, edgui_signal_t sign, void * param);
static void mbox_realign(edgui_obj_t * mbox);
#if EDGUI_USE_ANIMATION
static void edgui_mbox_close_ready_cb(edgui_anim_t * a);
#endif
static void edgui_mbox_default_event_cb(edgui_obj_t * mbox, edgui_event_t event);
static void edgui_mbox_btnm_event_cb(edgui_obj_t * btnm, edgui_event_t event);

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
 * Create a message box objects
 * @param par pointer to an object, it will be the parent of the new message box
 * @param copy pointer to a message box object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created message box
 */
edgui_obj_t * edgui_mbox_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("mesasge box create started");

    /*Create the ancestor message box*/
    edgui_obj_t * new_mbox = edgui_cont_create(par, copy);
    edgui_mem_assert(new_mbox);
    if(new_mbox == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_mbox);

    /*Allocate the message box type specific extended data*/
    edgui_mbox_ext_t * ext = edgui_obj_allocate_ext_attr(new_mbox, sizeof(edgui_mbox_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->text = NULL;
    ext->btnm = NULL;
#if EDGUI_USE_ANIMATION
    ext->anim_time = EDGUI_MBOX_CLOSE_ANIM_TIME;
#endif

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_mbox, edgui_mbox_signal);

    /*Init the new message box message box*/
    if(copy == NULL) {
        ext->text = edgui_label_create(new_mbox, NULL);
        edgui_label_set_align(ext->text, EDGUI_LABEL_ALIGN_CENTER);
        edgui_label_set_long_mode(ext->text, EDGUI_LABEL_LONG_BREAK);
        edgui_label_set_text(ext->text, "Message");

        edgui_cont_set_layout(new_mbox, EDGUI_LAYOUT_COL_M);
        edgui_cont_set_fit2(new_mbox, EDGUI_FIT_NONE, EDGUI_FIT_TIGHT);
        edgui_obj_set_width(new_mbox, EDGUI_DPI * 2);
        edgui_obj_align(new_mbox, NULL, EDGUI_ALIGN_CENTER, 0, 0);
        edgui_obj_set_event_cb(new_mbox, edgui_mbox_default_event_cb);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_mbox_set_style(new_mbox, EDGUI_MBOX_STYLE_BG, th->style.mbox.bg);
        } else {
            edgui_mbox_set_style(new_mbox, EDGUI_MBOX_STYLE_BG, &edgui_style_pretty);
        }

    }
    /*Copy an existing message box*/
    else {
        edgui_mbox_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);

        ext->text = edgui_label_create(new_mbox, copy_ext->text);

        /*Copy the buttons and the label on them*/
        if(copy_ext->btnm) ext->btnm = edgui_btnm_create(new_mbox, copy_ext->btnm);

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_mbox);
    }

    EDGUI_LOG_INFO("mesasge box created");

    return new_mbox;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add button to the message box
 * @param mbox pointer to message box object
 * @param btn_map button descriptor (button matrix map).
 *                E.g.  a const char *txt[] = {"ok", "close", ""} (Can not be local variable)
 */
void edgui_mbox_add_btns(edgui_obj_t * mbox, const char ** btn_map)
{
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);

    /*Create a button matrix if not exists yet*/
    if(ext->btnm == NULL) {
        ext->btnm = edgui_btnm_create(mbox, NULL);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_mbox_set_style(mbox, EDGUI_MBOX_STYLE_BTN_BG, th->style.mbox.btn.bg);
            edgui_mbox_set_style(mbox, EDGUI_MBOX_STYLE_BTN_REL, th->style.mbox.btn.rel);
            edgui_mbox_set_style(mbox, EDGUI_MBOX_STYLE_BTN_PR, th->style.mbox.btn.pr);
        } else {
            edgui_btnm_set_style(ext->btnm, EDGUI_BTNM_STYLE_BG, &edgui_style_transp_fit);
        }
    }

    edgui_btnm_set_map(ext->btnm, btn_map);
    edgui_btnm_set_btn_ctrl_all(ext->btnm, EDGUI_BTNM_CTRL_CLICK_TRIG | EDGUI_BTNM_CTRL_NO_REPEAT);
    edgui_obj_set_event_cb(ext->btnm, edgui_mbox_btnm_event_cb);

    mbox_realign(mbox);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the text of the message box
 * @param mbox pointer to a message box
 * @param txt a '\0' terminated character string which will be the message box text
 */
void edgui_mbox_set_text(edgui_obj_t * mbox, const char * txt)
{
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);
    edgui_label_set_text(ext->text, txt);

    mbox_realign(mbox);
}

/**
 * Set animation duration
 * @param mbox pointer to a message box object
 * @param anim_time animation length in  milliseconds (0: no animation)
 */
void edgui_mbox_set_anim_time(edgui_obj_t * mbox, uint16_t anim_time)
{
#if EDGUI_USE_ANIMATION
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);
    anim_time           = 0;
    ext->anim_time      = anim_time;
#else
    (void)mbox;
    (void)anim_time;
#endif
}

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param delay a time (in milliseconds) to wait before delete the message box
 */
void edgui_mbox_start_auto_close(edgui_obj_t * mbox, uint16_t delay)
{
#if EDGUI_USE_ANIMATION
    if(edgui_mbox_get_anim_time(mbox) != 0) {
        /*Add shrinking animations*/
        edgui_anim_t a;
        a.var            = mbox;
        a.start          = edgui_obj_get_height(mbox);
        a.end            = 0;
        a.exec_cb        = (edgui_anim_exec_xcb_t)edgui_obj_set_height;
        a.path_cb        = edgui_anim_path_linear;
        a.ready_cb       = NULL;
        a.act_time       = -delay;
        a.time           = edgui_mbox_get_anim_time(mbox);
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        edgui_anim_create(&a);

        a.start    = edgui_obj_get_width(mbox);
        a.exec_cb  = (edgui_anim_exec_xcb_t)edgui_obj_set_width;
        a.ready_cb = edgui_mbox_close_ready_cb;
        edgui_anim_create(&a);

        /*Disable fit to let shrinking work*/
        edgui_cont_set_fit(mbox, EDGUI_FIT_NONE);
    } else {
        /*Create an animation to delete the mbox `delay` ms later*/
        edgui_anim_t a;
        a.var            = mbox;
        a.start          = 0;
        a.end            = 1;
        a.exec_cb        = (edgui_anim_exec_xcb_t)NULL;
        a.path_cb        = edgui_anim_path_linear;
        a.ready_cb       = edgui_mbox_close_ready_cb;
        a.act_time       = -delay;
        a.time           = 0;
        a.playback       = 0;
        a.playback_pause = 0;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        edgui_anim_create(&a);
    }
#else
    (void)delay; /*Unused*/
    edgui_obj_del(mbox);
#endif
}

/**
 * Stop the auto. closing of message box
 * @param mbox pointer to a message box object
 */
void edgui_mbox_stop_auto_close(edgui_obj_t * mbox)
{
#if EDGUI_USE_ANIMATION
    edgui_anim_del(mbox, NULL);
#else
    (void)mbox; /*Unused*/
#endif
}

/**
 * Set a style of a message box
 * @param mbox pointer to a message box object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_mbox_set_style(edgui_obj_t * mbox, edgui_mbox_style_t type, const edgui_style_t * style)
{
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);

    switch(type) {
        case EDGUI_MBOX_STYLE_BG: edgui_obj_set_style(mbox, style); break;
        case EDGUI_MBOX_STYLE_BTN_BG: edgui_btnm_set_style(ext->btnm, EDGUI_BTNM_STYLE_BG, style); break;
        case EDGUI_MBOX_STYLE_BTN_REL: edgui_btnm_set_style(ext->btnm, EDGUI_BTNM_STYLE_BTN_REL, style); break;
        case EDGUI_MBOX_STYLE_BTN_PR: edgui_btnm_set_style(ext->btnm, EDGUI_BTNM_STYLE_BTN_PR, style); break;
        case EDGUI_MBOX_STYLE_BTN_TGL_REL: edgui_btnm_set_style(ext->btnm, EDGUI_BTNM_STYLE_BTN_TGL_REL, style); break;
        case EDGUI_MBOX_STYLE_BTN_TGL_PR: edgui_btnm_set_style(ext->btnm, EDGUI_BTNM_STYLE_BTN_TGL_PR, style); break;
        case EDGUI_MBOX_STYLE_BTN_INA: edgui_btnm_set_style(ext->btnm, EDGUI_BTNM_STYLE_BTN_INA, style); break;
    }

    mbox_realign(mbox);
}

/**
 * Set whether recoloring is enabled
 * @param btnm pointer to button matrix object
 * @param en whether recoloring is enabled
 */
void edgui_mbox_set_recolor(edgui_obj_t * mbox, bool en)
{
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);

    if(ext->btnm) edgui_btnm_set_recolor(ext->btnm, en);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the text of the message box
 */
const char * edgui_mbox_get_text(const edgui_obj_t * mbox)
{
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);

    return edgui_label_get_text(ext->text);
}

/**
 * Get the index of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`.
 * @param btnm pointer to button matrix object
 * @return  index of the last released button (EDGUI_BTNM_BTN_NONE: if unset)
 */
uint16_t edgui_mbox_get_active_btn(edgui_obj_t * mbox)
{
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);
    if(ext->btnm)
        return edgui_btnm_get_active_btn(ext->btnm);
    else
        return EDGUI_BTNM_BTN_NONE;
}

/**
 * Get the text of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`.
 * @param btnm pointer to button matrix object
 * @return text of the last released button (NULL: if unset)
 */
const char * edgui_mbox_get_active_btn_text(edgui_obj_t * mbox)
{
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);
    if(ext->btnm)
        return edgui_btnm_get_active_btn_text(ext->btnm);
    else
        return NULL;
}

/**
 * Get the animation duration (close animation time)
 * @param mbox pointer to a message box object
 * @return animation length in  milliseconds (0: no animation)
 */
uint16_t edgui_mbox_get_anim_time(const edgui_obj_t * mbox)
{
#if EDGUI_USE_ANIMATION
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);
    return ext->anim_time;
#else
    (void)mbox;
    return 0;
#endif
}

/**
 * Get a style of a message box
 * @param mbox pointer to a message box object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_mbox_get_style(const edgui_obj_t * mbox, edgui_mbox_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_mbox_ext_t * ext      = edgui_obj_get_ext_attr(mbox);

    switch(type) {
        case EDGUI_MBOX_STYLE_BG: style = edgui_obj_get_style(mbox); break;
        case EDGUI_MBOX_STYLE_BTN_BG: style = edgui_btnm_get_style(ext->btnm, EDGUI_BTNM_STYLE_BG); break;
        case EDGUI_MBOX_STYLE_BTN_REL: style = edgui_btnm_get_style(ext->btnm, EDGUI_BTNM_STYLE_BTN_REL); break;
        case EDGUI_MBOX_STYLE_BTN_PR: style = edgui_btnm_get_style(ext->btnm, EDGUI_BTNM_STYLE_BTN_PR); break;
        case EDGUI_MBOX_STYLE_BTN_TGL_REL: style = edgui_btnm_get_style(ext->btnm, EDGUI_BTNM_STYLE_BTN_TGL_REL); break;
        case EDGUI_MBOX_STYLE_BTN_TGL_PR: style = edgui_btnm_get_style(ext->btnm, EDGUI_BTNM_STYLE_BTN_TGL_PR); break;
        case EDGUI_MBOX_STYLE_BTN_INA: style = edgui_btnm_get_style(ext->btnm, EDGUI_BTNM_STYLE_BTN_INA); break;
        default: style = NULL; break;
    }

    return style;
}

/**
 * Get whether recoloring is enabled
 * @param mbox pointer to a message box object
 * @return whether recoloring is enabled
 */
bool edgui_mbox_get_recolor(const edgui_obj_t * mbox)
{
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);

    if(!ext->btnm) return false;

    return edgui_btnm_get_recolor(ext->btnm);
}

/**
 * Get message box button matrix
 * @param mbox pointer to a message box object
 * @return pointer to button matrix object
 * @remarks return value will be NULL unless `edgui_mbox_add_btns` has been already called
 */
edgui_obj_t * edgui_mbox_get_btnm(edgui_obj_t * mbox)
{
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);
    return ext->btnm;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the message box
 * @param mbox pointer to a message box object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_mbox_signal(edgui_obj_t * mbox, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /*Translate EDGUI_KEY_UP/DOWN to EDGUI_KEY_LEFT/RIGHT */
    char c_trans = 0;
    if(sign == EDGUI_SIGNAL_CONTROL) {
        c_trans = *((char *)param);
        if(c_trans == EDGUI_KEY_DOWN) c_trans = EDGUI_KEY_LEFT;
        if(c_trans == EDGUI_KEY_UP) c_trans = EDGUI_KEY_RIGHT;

        param = &c_trans;
    }

    /* Include the ancient signal function */
    res = ancestor_signal(mbox, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);
    if(sign == EDGUI_SIGNAL_CORD_CHG) {
        if(edgui_obj_get_width(mbox) != edgui_area_get_width(param)) {
            mbox_realign(mbox);
        }
    } else if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        mbox_realign(mbox);
    } else if(sign == EDGUI_SIGNAL_RELEASED) {
        if(ext->btnm) {
            uint32_t btn_id = edgui_btnm_get_active_btn(ext->btnm);
            if(btn_id != EDGUI_BTNM_BTN_NONE) edgui_event_send(mbox, EDGUI_EVENT_VALUE_CHANGED, &btn_id);
        }
    } else if(sign == EDGUI_SIGNAL_FOCUS || sign == EDGUI_SIGNAL_DEFOCUS || sign == EDGUI_SIGNAL_CONTROL ||
              sign == EDGUI_SIGNAL_GET_EDITABLE) {
        if(ext->btnm) {
            ext->btnm->signal_cb(ext->btnm, sign, param);
        }

        /* The button matrix with ENCODER input supposes it's in a group but in this case it isn't
         * (Only the message box's container) So so some actions here instead*/
        if(sign == EDGUI_SIGNAL_FOCUS) {
#if EDGUI_USE_GROUP
            edgui_indev_t * indev         = edgui_indev_get_act();
            edgui_indev_type_t indev_type = edgui_indev_get_type(indev);
            if(indev_type == EDGUI_INDEV_TYPE_ENCODER) {
                /*In navigation mode don't select any button but in edit mode select the fist*/
                edgui_btnm_ext_t * btnm_ext = edgui_obj_get_ext_attr(ext->btnm);
                if(edgui_group_get_editing(edgui_obj_get_group(mbox)))
                    btnm_ext->btn_id_pr = 0;
                else
                    btnm_ext->btn_id_pr = EDGUI_BTNM_BTN_NONE;
            }
#endif
        }
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_mbox";
    }

    return res;
}

/**
 * Resize the button holder to fit
 * @param mbox pointer to message box object
 */
static void mbox_realign(edgui_obj_t * mbox)
{
    edgui_mbox_ext_t * ext = edgui_obj_get_ext_attr(mbox);

    const edgui_style_t * style = edgui_mbox_get_style(mbox, EDGUI_MBOX_STYLE_BG);
    edgui_coord_t w             = edgui_obj_get_width(mbox) - style->body.padding.left - style->body.padding.right;

    if(ext->text) {
        edgui_obj_set_width(ext->text, w);
    }

    if(ext->btnm) {
        const edgui_style_t * btn_bg_style  = edgui_mbox_get_style(mbox, EDGUI_MBOX_STYLE_BTN_BG);
        const edgui_style_t * btn_rel_style = edgui_mbox_get_style(mbox, EDGUI_MBOX_STYLE_BTN_REL);
        edgui_coord_t font_h                = edgui_font_get_line_height(btn_rel_style->text.font);
        edgui_obj_set_size(ext->btnm, w,
                        font_h + btn_rel_style->body.padding.top + btn_rel_style->body.padding.bottom +
                            btn_bg_style->body.padding.top + btn_bg_style->body.padding.bottom);
    }
}

#if EDGUI_USE_ANIMATION
static void edgui_mbox_close_ready_cb(edgui_anim_t * a)
{
    edgui_obj_del(a->var);
}
#endif

static void edgui_mbox_default_event_cb(edgui_obj_t * mbox, edgui_event_t event)
{
    if(event != EDGUI_EVENT_VALUE_CHANGED) return;

    uint32_t btn_id = edgui_mbox_get_active_btn(mbox);
    if(btn_id == EDGUI_BTNM_BTN_NONE) return;

    edgui_mbox_start_auto_close(mbox, 0);
}

static void edgui_mbox_btnm_event_cb(edgui_obj_t * btnm, edgui_event_t event)
{
    edgui_obj_t * mbox = edgui_obj_get_parent(btnm);

    /*clang-format off*/
    if(event == EDGUI_EVENT_PRESSED || event == EDGUI_EVENT_PRESSING || event == EDGUI_EVENT_PRESS_LOST ||
       event == EDGUI_EVENT_RELEASED || event == EDGUI_EVENT_SHORT_CLICKED || event == EDGUI_EVENT_CLICKED ||
       event == EDGUI_EVENT_LONG_PRESSED || event == EDGUI_EVENT_LONG_PRESSED_REPEAT ||
       event == EDGUI_EVENT_VALUE_CHANGED) {
        edgui_event_send(mbox, event, edgui_event_get_data());
    }
    /*clang-format on*/
}

#endif
