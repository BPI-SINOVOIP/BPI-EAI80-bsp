/**
 * @file edgui_tutorial_keyboard_simkpad.c
 *
 */

/*
 * -------------------------------------------
 * Learn how to use a keyboard/keypad device
 * -------------------------------------------
 *
 * You need two things to use keypad/keyboard:
 *
 * INPUT DEVICE DRIVER
 * - Similarly to touchpad you need to register an 'edgui_indev_drv_t' driver
 * - For control keys you should use EDGUI_KEY_... from edgui_group.h (e.g. EDGUI_KEY_NEXT)
 * -
 *
 *
 * OBJECT GROUP
 * - You can iterate through objects in a group (like using 'tab' on PC) and adjust/modify them
 * - Firstly you need to create an object group: `edgui_group_t *g = edgui_group_create();`
 * - And add objects to it: `edgui_group_add_obj(g, btn1);`
 * - Then you can send data to the object in focus: edgui_group_send_data(g, 'a');
 *                                                  edgui_group_send_data(g, EDGUI_GROUP_UP);
 * - Or focus on the next/prev. object:  edgui_group_focus_next(g);
 *
 */
/*********************
 *      INCLUDES
 *********************/
#include "edgui_tutorial_keyboard.h"
#if EDGUI_USE_TUTORIALS && EDGUI_USE_GROUP

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void gui_create(void);
static void kaypad_create(void);
static bool emulated_keypad_read(edgui_indev_drv_t * indev_drv, edgui_indev_data_t * data);
static void mbox_event_cb(edgui_obj_t * mbox, edgui_event_t event);
static void keypad_event_cb(edgui_obj_t * btn, edgui_event_t event);
static void message_btn_event_cb(edgui_obj_t * btn, edgui_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_obj_t * btn_enable;           /*An enable button*/
static edgui_style_t style_mbox_bg;        /*Black bg. style with opacity*/
static edgui_group_t * g;                  /*An Object Group*/
static edgui_indev_t * emulated_kp_indev;  /*The input device of the emulated keypad*/
static edgui_indev_state_t last_state = EDGUI_INDEV_STATE_REL;
static uint32_t last_key = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a simple GUI to demonstrate encoder control capability
 * kp_indev optinonally pass a keypad input device to control the object (NULL if unused)
 */
void edgui_tutorial_keyboard(edgui_indev_t * kp_indev)
{
    /*Register the emulated keyboard*/
    edgui_indev_drv_t  kp_drv;
    edgui_indev_drv_init(&kp_drv);
    kp_drv.type = EDGUI_INDEV_TYPE_KEYPAD;
    kp_drv.read_cb = emulated_keypad_read;
    emulated_kp_indev = edgui_indev_drv_register(&kp_drv);

    /*Create an object group*/
    g = edgui_group_create();

    /*Assig the input device(s) to the created group*/
    edgui_indev_set_group(emulated_kp_indev, g);
    if(kp_indev) edgui_indev_set_group(kp_indev, g);

    /*Create a demo GUI*/
    gui_create();

    /*Create virtual encoder*/
    kaypad_create();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Create a demo GUI
 */
static void gui_create(void)
{
    edgui_obj_t * scr = edgui_disp_get_scr_act(NULL);     /*Get the current screen*/

    /*Create a drop down list*/
    edgui_obj_t * ddlist = edgui_ddlist_create(scr, NULL);
    edgui_ddlist_set_options(ddlist, "Low\nMedium\nHigh");
    edgui_obj_set_pos(ddlist, EDGUI_DPI / 4, EDGUI_DPI / 4);
    edgui_group_add_obj(g, ddlist);                    /*Add the object to the group*/

    /*Create a holder and check boxes on it*/
    edgui_obj_t * holder = edgui_cont_create(scr, NULL);   /*Create a transparent holder*/
    edgui_cont_set_fit(holder, EDGUI_FIT_TIGHT);

    edgui_cont_set_layout(holder, EDGUI_LAYOUT_COL_L);
    edgui_obj_set_style(holder, &edgui_style_transp);
    edgui_obj_align(holder, ddlist, EDGUI_ALIGN_OUT_RIGHT_TOP, EDGUI_DPI / 4, 0);

    edgui_obj_t * cb = edgui_cb_create(holder, NULL);     /*First check box*/
    edgui_cb_set_text(cb, "Red");
    edgui_group_add_obj(g, cb);                        /*Add to the group*/

    cb = edgui_cb_create(holder, cb);                  /*Copy the first check box. Automatically added to the same group*/
    edgui_cb_set_text(cb, "Green");

    cb = edgui_cb_create(holder, cb);                  /*Copy the second check box. Automatically added to the same group*/
    edgui_cb_set_text(cb, "Blue");

    /*Create a sliders*/
    edgui_obj_t * slider = edgui_slider_create(scr, NULL);
    edgui_obj_set_size(slider, EDGUI_DPI, EDGUI_DPI / 3);
    edgui_obj_align(slider, holder, EDGUI_ALIGN_OUT_RIGHT_TOP, EDGUI_DPI / 4, 0);
    edgui_bar_set_range(slider, 0, 20);
    edgui_group_add_obj(g, slider);                    /*Add to the group*/

    /*Create a button*/
    btn_enable = edgui_btn_create(scr, NULL);
    edgui_obj_set_event_cb(btn_enable, message_btn_event_cb);
    edgui_btn_set_fit(btn_enable, EDGUI_FIT_TIGHT);
    edgui_group_add_obj(g, btn_enable);                /*Add to the group*/
    edgui_obj_t * l = edgui_label_create(btn_enable, NULL);
    edgui_label_set_text(l, "Message");
    edgui_obj_align(btn_enable, slider, EDGUI_ALIGN_OUT_BOTTOM_MID, 0, EDGUI_DPI / 2);

    /* Create a dark plain style for a message box's background (modal)*/
    edgui_style_copy(&style_mbox_bg, &edgui_style_plain);
    style_mbox_bg.body.main_color = EDGUI_COLOR_BLACK;
    style_mbox_bg.body.grad_color = EDGUI_COLOR_BLACK;
    style_mbox_bg.body.opa = EDGUI_OPA_50;
}

/**
 * Create virtual keypad using 4 buttons:
 * - Next: focus on the next object in the group
 * - Increment: increment the object value
 * - Decrement: decrement the object value
 * - Enter: Select something
 */
static void kaypad_create(void)
{
    edgui_obj_t * scr = edgui_disp_get_scr_act(NULL);     /*Get the current screen*/

    /*Next button*/
    edgui_obj_t * btn_next = edgui_btn_create(scr, NULL);
    edgui_obj_set_event_cb(btn_next, keypad_event_cb);
    edgui_btn_set_fit(btn_next, EDGUI_FIT_TIGHT);
    edgui_obj_t * l = edgui_label_create(btn_next, NULL);
    edgui_label_set_text(l, "Next");
    edgui_obj_align(btn_next, NULL, EDGUI_ALIGN_IN_BOTTOM_LEFT, EDGUI_DPI / 4, - EDGUI_DPI / 4);

    /*Increment button*/
    edgui_obj_t * btn_inc = edgui_btn_create(scr, btn_next);
    l = edgui_label_create(btn_inc, NULL);
    edgui_label_set_text(l, "Dec");
    edgui_obj_align(btn_inc, btn_next, EDGUI_ALIGN_OUT_RIGHT_MID, EDGUI_DPI / 4, 0);

    /*Decrement button*/
    edgui_obj_t * btn_dec = edgui_btn_create(scr, btn_next);
    l = edgui_label_create(btn_dec, NULL);
    edgui_label_set_text(l, "Inc");
    edgui_obj_align(btn_dec, btn_inc, EDGUI_ALIGN_OUT_RIGHT_MID, EDGUI_DPI / 4, 0);

    /*Enter button*/
    edgui_obj_t * btn_enter = edgui_btn_create(scr, btn_next);
    l = edgui_label_create(btn_enter, NULL);
    edgui_label_set_text(l, "Enter");
    edgui_obj_align(btn_enter, btn_dec, EDGUI_ALIGN_OUT_RIGHT_MID, EDGUI_DPI / 4, 0);
}

static bool emulated_keypad_read(edgui_indev_drv_t * indev_drv, edgui_indev_data_t * data)
{
    (void)indev_drv;                /*Unused*/
    data->key = last_key;
    data->state = last_state;
    return false;
}

/**
 * Called when the Enable button is released. Show a message box to really enable or not?
 * @param btn pointer to the Enable button
 * @param indev_proc pointer to the caller display input or NULL if the encoder used
 * @return EDGUI_RES_OK: because the button is not deleted
 */
static void message_btn_event_cb(edgui_obj_t * btn, edgui_event_t event)
{
    if(event != EDGUI_EVENT_RELEASED) return;  /*We only care only with the release event*/

    /*If the butto nsi released the show message box to be sure about the Enable*/
    if(edgui_btn_get_state(btn) == EDGUI_BTN_STATE_REL) {
        /* Create a dark screen sized bg. with opacity to show
         * the other objects are not available now*/
        edgui_obj_set_style(edgui_disp_get_layer_top(NULL), &style_mbox_bg);
        edgui_obj_set_click(edgui_disp_get_layer_top(NULL), false);     /*It should be `true` but it'd block the emulated keyboard too*/

        /*Create a message box*/
        edgui_obj_t * mbox = edgui_mbox_create(edgui_disp_get_layer_top(NULL), NULL);
        edgui_mbox_set_text(mbox, "Turn on something?");
        edgui_obj_set_event_cb(mbox, mbox_event_cb);
        edgui_group_add_obj(g, mbox);          /*Add to he group*/

        /*Add two buttons*/
        static const char * btns[] = {"Yes", "No", ""};
        edgui_mbox_add_btns(mbox, btns);

        edgui_obj_align(mbox, NULL, EDGUI_ALIGN_CENTER, 0, - EDGUI_DPI / 2);

        /*Focus on the new message box, can freeze focus on it*/
        edgui_group_focus_obj(mbox);
        edgui_group_focus_freeze(g, true);
    }
    /*Just disable without message*/
    else {
        edgui_btn_set_state(btn_enable, EDGUI_BTN_STATE_REL);
    }
}

/**
 * Called when a message box button is clicked
 * @param mbox pointer to message box
 * @param event event type
 */
static void mbox_event_cb(edgui_obj_t * mbox, edgui_event_t event)
{
    if(event != EDGUI_EVENT_CLICKED) return;

    const char * btn_txt = edgui_mbox_get_active_btn_text(mbox);
    if(btn_txt) {
        edgui_group_focus_freeze(g, false);        /*Release the freeze*/

        /*Revert the top layer to not block*/
        edgui_obj_set_style(edgui_disp_get_layer_top(NULL), &edgui_style_transp);
        edgui_obj_set_click(edgui_disp_get_layer_top(NULL), false);

        /*Mark the enabled state by toggling the button*/
        if(strcmp(btn_txt, "No") == 0)  edgui_btn_set_state(btn_enable, EDGUI_BTN_STATE_REL);
        else if(strcmp(btn_txt, "Yes") == 0)  edgui_btn_set_state(btn_enable, EDGUI_BTN_STATE_TGL_REL);

        edgui_obj_del(mbox);
    }
}

/**
 * Called the handle the emulated keys' events
 * @param btn pointer to the button
 * @return EDGUI_RES_OK: because the button is not deleted
 */
static void keypad_event_cb(edgui_obj_t * btn, edgui_event_t event)
{
    if(event == EDGUI_EVENT_PRESSED) {

        edgui_obj_t * label = edgui_obj_get_child(btn, NULL);
        const char * txt = edgui_label_get_text(label);

        if(strcmp(txt, "Next") == 0)        last_key = EDGUI_KEY_NEXT;
        else if (strcmp(txt, "Inc") == 0)   last_key = EDGUI_KEY_UP;
        else if (strcmp(txt, "Dec") == 0)   last_key = EDGUI_KEY_DOWN;
        else if (strcmp(txt, "Enter") == 0) last_key = EDGUI_KEY_ENTER;
        else last_key = 0;

        last_state = EDGUI_INDEV_STATE_PR;         /*Save the state*/
    } else if(event == EDGUI_EVENT_RELEASED || event == EDGUI_EVENT_PRESS_LOST) {
        last_state = EDGUI_INDEV_STATE_REL;
    }

}


#endif /*EDGUI_USE_TUTORIALS*/
