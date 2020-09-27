/**
 * @file edgui_test_group.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "stdio.h"
#include "edgui_test_group.h"
#if EDGUI_USE_GROUP && EDGUI_USE_TESTS

#if EDGUI_EX_KEYBOARD || EDGUI_EX_MOUSEWHEEL
#include "edgui_drv_conf.h"
#endif

#if EDGUI_EX_KEYBOARD
#include "edgui_drivers/indev/keyboard.h"
#endif

#if EDGUI_EX_MOUSEWHEEL
#include "edgui_drivers/indev/mousewheel.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
/*To emulate some keys on the window header*/
static bool win_btn_read(edgui_indev_drv_t * indev_drv, edgui_indev_data_t * data);
static void win_btn_event_handler(edgui_obj_t * btn, edgui_event_t event);

static void group_focus_cb(edgui_group_t * group);

static void general_event_handler(edgui_obj_t * obj, edgui_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t last_key;
static edgui_indev_state_t last_key_state = EDGUI_INDEV_STATE_REL;
static edgui_group_t * g;
static edgui_obj_t * win;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create base groups to test their functionalities
 */
edgui_group_t *edgui_test_group_1(void)
{
    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    g = edgui_group_create();
    edgui_group_set_focus_cb(g, group_focus_cb);

    /*A keyboard will be simulated*/
    edgui_indev_drv_t sim_kb_drv;
    edgui_indev_drv_init(&sim_kb_drv);
    sim_kb_drv.type = EDGUI_INDEV_TYPE_KEYPAD;
    sim_kb_drv.read_cb = win_btn_read;
    edgui_indev_t * win_kb_indev = edgui_indev_drv_register(&sim_kb_drv);
    edgui_indev_set_group(win_kb_indev, g);

#if EDGUI_EX_KEYBOARD
    edgui_indev_drv_t real_kb_drv;
    edgui_indev_drv_init(&real_kb_drv);
    real_kb_drv.type = EDGUI_INDEV_TYPE_KEYPAD;
    real_kb_drv.read_cb = keyboard_read;
    edgui_indev_t * real_kb_indev = edgui_indev_drv_register(&real_kb_drv);
    edgui_indev_set_group(real_kb_indev, g);
#endif

#if EDGUI_EX_MOUSEWHEEL
    edgui_indev_drv_t enc_drv;
    edgui_indev_drv_init(&enc_drv);
    enc_drv.type = EDGUI_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = mousewheel_read;
    edgui_indev_t * enc_indev = edgui_indev_drv_register(&enc_drv);
    edgui_indev_set_group(enc_indev, g);
#endif

    /*Create a window to hold all the objects*/
    static edgui_style_t win_style;
    edgui_style_copy(&win_style, &edgui_style_transp);
    win_style.body.padding.left= EDGUI_DPI / 6;
    win_style.body.padding.right = EDGUI_DPI / 6;
    win_style.body.padding.top = EDGUI_DPI / 6;
    win_style.body.padding.bottom = EDGUI_DPI / 6;
    win_style.body.padding.inner = EDGUI_DPI / 6;

    win = edgui_win_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_win_set_title(win, "Group test");
    edgui_page_set_scrl_layout(edgui_win_get_content(win), EDGUI_LAYOUT_PRETTY);
    edgui_win_set_style(win, EDGUI_WIN_STYLE_CONTENT, &win_style);
    edgui_group_add_obj(g, edgui_win_get_content(win));
    edgui_obj_set_event_cb(edgui_win_get_content(win), general_event_handler);

    edgui_obj_t * win_btn = edgui_win_add_btn(win, EDGUI_SYMBOL_RIGHT);
    edgui_obj_set_protect(win_btn, EDGUI_PROTECT_CLICK_FOCUS);
    edgui_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = edgui_win_add_btn(win, EDGUI_SYMBOL_NEXT);
    edgui_obj_set_protect(win_btn, EDGUI_PROTECT_CLICK_FOCUS);
    edgui_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = edgui_win_add_btn(win, EDGUI_SYMBOL_OK);
    edgui_obj_set_protect(win_btn, EDGUI_PROTECT_CLICK_FOCUS);
    edgui_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = edgui_win_add_btn(win, EDGUI_SYMBOL_PREV);
    edgui_obj_set_protect(win_btn, EDGUI_PROTECT_CLICK_FOCUS);
    edgui_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = edgui_win_add_btn(win, EDGUI_SYMBOL_LEFT);
    edgui_obj_set_protect(win_btn, EDGUI_PROTECT_CLICK_FOCUS);
    edgui_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = edgui_win_add_btn(win, EDGUI_SYMBOL_DUMMY"a");
    edgui_obj_set_protect(win_btn, EDGUI_PROTECT_CLICK_FOCUS);
    edgui_obj_set_event_cb(win_btn, win_btn_event_handler);

    edgui_obj_t * obj;


    obj = edgui_spinbox_create(win, NULL);
    edgui_obj_set_event_cb(obj, general_event_handler);
    edgui_spinbox_set_digit_format(obj, 5, 2);
    edgui_group_add_obj(g, obj);

    obj = edgui_btn_create(win, NULL);
    edgui_group_add_obj(g, obj);
    edgui_btn_set_toggle(obj, true);
    edgui_obj_set_event_cb(obj, general_event_handler);
    obj = edgui_label_create(obj, NULL);
    edgui_label_set_text(obj, "Button");

    EDGUI_IMG_DECLARE(imgbtn_img_1);
    EDGUI_IMG_DECLARE(imgbtn_img_2);
    obj = edgui_imgbtn_create(win, NULL);
    edgui_imgbtn_set_src(obj, EDGUI_BTN_STATE_REL, &imgbtn_img_1);
    edgui_imgbtn_set_src(obj, EDGUI_BTN_STATE_PR, &imgbtn_img_2);
    edgui_obj_set_event_cb(obj, general_event_handler);
    edgui_group_add_obj(g, obj);

    obj = edgui_cb_create(win, NULL);
    edgui_obj_set_event_cb(obj, general_event_handler);
    edgui_group_add_obj(g, obj);

    obj = edgui_slider_create(win, NULL);
    edgui_slider_set_range(obj, 0, 10);
    edgui_obj_set_event_cb(obj, general_event_handler);
    edgui_group_add_obj(g, obj);

    obj = edgui_sw_create(win, NULL);
    edgui_obj_set_event_cb(obj, general_event_handler);
    edgui_group_add_obj(g, obj);

    obj = edgui_ddlist_create(win, NULL);
    edgui_ddlist_set_options(obj, "Item1\nItem2\nItem3\nItem4\nItem5\nItem6");
    edgui_ddlist_set_fix_height(obj, EDGUI_DPI);
    edgui_obj_set_event_cb(obj, general_event_handler);
    edgui_group_add_obj(g, obj);

    obj = edgui_roller_create(win, NULL);
    edgui_obj_set_event_cb(obj, general_event_handler);
    edgui_group_add_obj(g, obj);

    edgui_obj_t * ta = edgui_ta_create(win, NULL);
    edgui_ta_set_cursor_type(ta, EDGUI_CURSOR_BLOCK);
    edgui_obj_set_event_cb(ta, general_event_handler);
    edgui_group_add_obj(g, ta);

    obj = edgui_kb_create(win, NULL);
    edgui_obj_set_size(obj, hres - EDGUI_DPI, vres / 2);
    edgui_kb_set_ta(obj, ta);
    edgui_kb_set_cursor_manage(obj, true);
    edgui_group_add_obj(g, obj);

    static const char * mbox_btns[] = {"Yes", "No", ""};
    obj = edgui_mbox_create(win, NULL);
    edgui_mbox_add_btns(obj, mbox_btns);
    edgui_obj_set_event_cb(obj, general_event_handler);
    edgui_group_add_obj(g, obj);

    obj = edgui_list_create(win, NULL);
    edgui_obj_set_event_cb(obj, general_event_handler);
    const char * list_txts[] = {"File 1", "File 2", "File 3", "File 4", "File 5", "File 6", ""};

    uint32_t i;
    for(i = 0; list_txts[i][0] != '\0'; i++) {
        edgui_obj_t * b;
        b = edgui_list_add_btn(obj, EDGUI_SYMBOL_FILE, list_txts[i]);
        edgui_obj_set_event_cb(b, general_event_handler);
    }

    edgui_group_add_obj(g, obj);

    obj = edgui_page_create(win, NULL);
    edgui_obj_set_size(obj, 2 * EDGUI_DPI, EDGUI_DPI);
    edgui_group_add_obj(g, obj);

    obj = edgui_label_create(obj, NULL);
    edgui_label_set_text(obj, "I'm a page\nwith a long \ntext.\n\n"
                      "You can try \nto scroll me\nwith UP and DOWN\nbuttons.");
    edgui_label_set_align(obj, EDGUI_LABEL_ALIGN_CENTER);
    edgui_obj_align(obj, NULL, EDGUI_ALIGN_CENTER, 0, 0);

    obj = edgui_tabview_create(win, NULL);
    edgui_obj_set_size(obj, hres / 2, vres / 2);
    edgui_obj_t * t1 = edgui_tabview_add_tab(obj, "Tab 1");
    edgui_obj_t * t2 = edgui_tabview_add_tab(obj, "Tab 2");
    edgui_obj_set_event_cb(obj, general_event_handler);
    edgui_group_add_obj(g, obj);

    obj = edgui_label_create(t1, NULL);
    edgui_label_set_text(obj, "This is the content\nof the first tab");

    obj = edgui_label_create(t2, NULL);
    edgui_label_set_text(obj, "This is the content\nof the second tab");
    return g;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Read function for the input device which emulates keys on the window header
 * @param indev_drv pointer to the related input device driver
 * @param data store the last key and its state here
 * @return false because the reading in not buffered
 */
static bool win_btn_read(edgui_indev_drv_t * indev_drv, edgui_indev_data_t * data)
{
    (void) indev_drv;      /*Unused*/

    data->state = last_key_state;
    data->key = last_key;

    return false;
}

/**
 * Called when a control button on the window header is released to change the key state to RELEASED
 * @param btn pointer t to a button on the window header
 * @return EDGUI_RES_OK  because the button is not deleted
 */
static void win_btn_event_handler(edgui_obj_t * btn, edgui_event_t event)
{
    (void) btn; /*Unused*/

    uint32_t key = 0;

    edgui_obj_t * label = edgui_obj_get_child(btn, NULL);
    const char * txt = edgui_label_get_text(label);

    if(strcmp(txt, EDGUI_SYMBOL_PREV) == 0) key = EDGUI_KEY_PREV;
    else if(strcmp(txt, EDGUI_SYMBOL_NEXT) == 0) key = EDGUI_KEY_NEXT;
    else if(strcmp(txt, EDGUI_SYMBOL_LEFT) == 0) key = EDGUI_KEY_LEFT;
    else if(strcmp(txt, EDGUI_SYMBOL_RIGHT) == 0) key = EDGUI_KEY_RIGHT;
    else if(strcmp(txt, EDGUI_SYMBOL_OK) == 0) key = EDGUI_KEY_ENTER;
    else key = 'a';

    switch(event) {
        case EDGUI_EVENT_PRESSED:
            last_key_state = EDGUI_INDEV_STATE_PR;
            last_key = key;
            break;

        case EDGUI_EVENT_CLICKED:
        case EDGUI_EVENT_PRESS_LOST:
            last_key_state = EDGUI_INDEV_STATE_REL;
            last_key = 0;
            break;
        default:
            break;
    }
}


static void group_focus_cb(edgui_group_t * group)
{
    edgui_obj_t * f = edgui_group_get_focused(group);
    if(f != win) edgui_win_focus(win, f, EDGUI_ANIM_ON);
}

static void general_event_handler(edgui_obj_t * obj, edgui_event_t event)
{
    (void) obj; /*Unused*/

#if EDGUI_EX_PRINTF
    switch(event) {
        case EDGUI_EVENT_PRESSED:
            printf("Pressed\n");
            break;

        case EDGUI_EVENT_SHORT_CLICKED:
            printf("Short clicked\n");
            break;

        case EDGUI_EVENT_CLICKED:
            printf("Clicked\n");
            break;

        case EDGUI_EVENT_LONG_PRESSED:
            printf("Long press\n");
            break;

        case EDGUI_EVENT_LONG_PRESSED_REPEAT:
            printf("Long press repeat\n");
            break;

        case EDGUI_EVENT_VALUE_CHANGED:
            printf("Value changed: %s\n", edgui_event_get_data() ? (const char *)edgui_event_get_data() : "");
            break;

        case EDGUI_EVENT_RELEASED:
            printf("Released\n");
            break;

        case EDGUI_EVENT_DRAG_BEGIN:
            printf("Drag begin\n");
            break;

        case EDGUI_EVENT_DRAG_END:
            printf("Drag end\n");
            break;

        case EDGUI_EVENT_DRAG_THROW_BEGIN:
            printf("Drag throw begin\n");
            break;

        case EDGUI_EVENT_FOCUSED:
            printf("Focused\n");
            break;
        case EDGUI_EVENT_DEFOCUSED:
            printf("Defocused\n");
            break;
        default:
            break;
    }
#endif
}

#endif /* EDGUI_USE_GROUP && EDGUI_USE_TESTS */
