/**
 * @file edgui_port_indev_templ.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <ts.h>
#include "edgui_port_indev.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static bool touchpad_read(edgui_indev_drv_t *indev_drv, edgui_indev_data_t *data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(edgui_coord_t *x, edgui_coord_t *y);

static void mouse_init(void);
static bool mouse_read(edgui_indev_drv_t *indev_drv, edgui_indev_data_t *data);
static bool mouse_is_pressed(void);
static void mouse_get_xy(edgui_coord_t *x, edgui_coord_t *y);

static void keypad_init(void);
static bool keypad_read(edgui_indev_drv_t *indev_drv, edgui_indev_data_t *data);
static uint32_t keypad_get_key(void);

static void encoder_init(void);
static bool encoder_read(edgui_indev_drv_t *indev_drv, edgui_indev_data_t *data);
static void encoder_handler(void);

static void button_init(void);
static bool button_read(edgui_indev_drv_t *indev_drv, edgui_indev_data_t *data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);
struct device *touch_dev;

Touch_Screen_t touchScreen;
/**********************
 *  STATIC VARIABLES
 **********************/
edgui_indev_t *indev_touchpad;
edgui_indev_t *indev_mouse;
edgui_indev_t *indev_keypad;
edgui_indev_t *indev_encoder;
edgui_indev_t *indev_button;

static int32_t encoder_diff;
static edgui_indev_state_t encoder_state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void edgui_port_indev_init(void)
{
    /* Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */


    edgui_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    edgui_indev_drv_init(&indev_drv);
    indev_drv.type = EDGUI_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = edgui_indev_drv_register(&indev_drv);

    /*------------------
     * Mouse
     * -----------------*/
#if 0
    /*Initialize your touchpad if you have*/
    mouse_init();

    /*Register a mouse input device*/
    edgui_indev_drv_init(&indev_drv);
    indev_drv.type = EDGUI_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read;
    indev_mouse = edgui_indev_drv_register(&indev_drv);

    /*Set cursor. For simplicity set a HOME symbol now.*/
    edgui_obj_t *mouse_cursor = edgui_img_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_img_set_src(mouse_cursor, EDGUI_SYMBOL_HOME);
    edgui_indev_set_cursor(indev_mouse, mouse_cursor);

    /*------------------
     * Keypad
     * -----------------*/

    /*Initialize your keypad or keyboard if you have*/
    keypad_init();

    /*Register a keypad input device*/
    edgui_indev_drv_init(&indev_drv);
    indev_drv.type = EDGUI_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keypad_read;
    indev_keypad = edgui_indev_drv_register(&indev_drv);

    /* Later you should create group(s) with `edgui_group_t * group = edgui_group_create()`,
     * add objects to the group with `edgui_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `edgui_indev_set_group(indev_keypad, group);` */

    /*------------------
     * Encoder
     * -----------------*/

    /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    edgui_indev_drv_init(&indev_drv);
    indev_drv.type = EDGUI_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = encoder_read;
    indev_encoder = edgui_indev_drv_register(&indev_drv);

    /* Later you should create group(s) with `edgui_group_t * group = edgui_group_create()`,
     * add objects to the group with `edgui_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `edgui_indev_set_group(indev_keypad, group);` */

    /*------------------
     * Button
     * -----------------*/

    /*Initialize your button if you have*/
    button_init();

    /*Register a button input device*/
    edgui_indev_drv_init(&indev_drv);
    indev_drv.type = EDGUI_INDEV_TYPE_BUTTON;
    indev_drv.read_cb = button_read;
    indev_button = edgui_indev_drv_register(&indev_drv);

    /*Assign buttons to points on the screen*/
    static const edgui_point_t btn_points[2] =
    {
        {10, 10},   /*Button 0 -> x:10; y:10*/
        {40, 100},  /*Button 1 -> x:40; y:100*/
    };
    edgui_indev_set_button_points(indev_button, btn_points);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/



/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    /*Your code comes here*/
#if defined(CONFIG_GT9147)
    touch_dev = device_get_binding(GT9147_DEV_NAME);
#elif defined(CONFIG_FT5426)
    touch_dev = device_get_binding(FT5426_DEV_NAME);
#endif
    touchScreen.dir = 1;
}

/* Will be called by the library to read the touchpad */
static bool touchpad_read(edgui_indev_drv_t *indev_drv, edgui_indev_data_t *data)
{
    int ret = 0;
    static uint32_t cnt = 0;
    struct ts_input_event event;
    ret = read_ts_event(&event, K_FOREVER);
    if (!ret)
    {
        if (event.type == ABS_XY)
        {
            //printk("\n###########Touch Screen Action:%d################\n", ++cnt);
            if (event.press_status == TS_PRESS)
            {
                /*ts press*/
                data->state = EDGUI_INDEV_STATE_PR;
                data->point.x = event.x_abs;
                data->point.y = event.y_abs;
                //printk("ts prss: x_abs:%d y_abs:%d\n", event.x_abs, event.y_abs);
            }
            else
            {
                /*ts release*/
                data->state = EDGUI_INDEV_STATE_REL;
                //printk("ts release\n");
            }
        }
    }
    return false;
}

/*Return true is the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    /*Your code comes here*/

    return false;
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(edgui_coord_t *x, edgui_coord_t *y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}


/*------------------
 * Mouse
 * -----------------*/

/* Initialize your mouse */
static void mouse_init(void)
{
    /*Your code comes here*/
}

/* Will be called by the library to read the mouse */
static bool mouse_read(edgui_indev_drv_t *indev_drv, edgui_indev_data_t *data)
{
    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the mouse button is pressed or released*/
    if (mouse_is_pressed())
    {
        data->state = EDGUI_INDEV_STATE_PR;
    }
    else
    {
        data->state = EDGUI_INDEV_STATE_REL;
    }

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}

/*Return true is the mouse button is pressed*/
static bool mouse_is_pressed(void)
{
    /*Your code comes here*/

    return false;
}

/*Get the x and y coordinates if the mouse is pressed*/
static void mouse_get_xy(edgui_coord_t *x, edgui_coord_t *y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}

/*------------------
 * Keypad
 * -----------------*/

/* Initialize your keypad */
static void keypad_init(void)
{
    /*Your code comes here*/
}

/* Will be called by the library to read the mouse */
static bool keypad_read(edgui_indev_drv_t *indev_drv, edgui_indev_data_t *data)
{
    static uint32_t last_key = 0;

    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();
    if (act_key != 0)
    {
        data->state = EDGUI_INDEV_STATE_PR;

        /*Translate the keys to edgui control characters according to your key definitions*/
        switch (act_key)
        {
            case 1:
                act_key = EDGUI_KEY_NEXT;
                break;
            case 2:
                act_key = EDGUI_KEY_PREV;
                break;
            case 3:
                act_key = EDGUI_KEY_LEFT;
                break;
            case 4:
                act_key = EDGUI_KEY_RIGHT;
                break;
            case 5:
                act_key = EDGUI_KEY_ENTER;
                break;
        }

        last_key = act_key;
    }
    else
    {
        data->state = EDGUI_INDEV_STATE_REL;
    }

    data->key = last_key;

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    /*Your code comes here*/

    return 0;
}

/*------------------
 * Encoder
 * -----------------*/

/* Initialize your keypad */
static void encoder_init(void)
{
    /*Your code comes here*/
}

/* Will be called by the library to read the encoder */
static bool encoder_read(edgui_indev_drv_t *indev_drv, edgui_indev_data_t *data)
{

    data->enc_diff = encoder_diff;
    data->state = encoder_state;

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}

/*Call this function in an interrupt to process encoder events (turn, press)*/
static void encoder_handler(void)
{
    /*Your code comes here*/

    encoder_diff += 0;
    encoder_state = EDGUI_INDEV_STATE_REL;
}


/*------------------
 * Button
 * -----------------*/

/* Initialize your buttons */
static void button_init(void)
{
    /*Your code comes here*/
}

/* Will be called by the library to read the button */
static bool button_read(edgui_indev_drv_t *indev_drv, edgui_indev_data_t *data)
{

    static uint8_t last_btn = 0;

    /*Get the pressed button's ID*/
    int8_t btn_act = button_get_pressed_id();

    if (btn_act >= 0)
    {
        data->state = EDGUI_INDEV_STATE_PR;
        last_btn = btn_act;
    }
    else
    {
        data->state = EDGUI_INDEV_STATE_REL;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
    uint8_t i;

    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
    for (i = 0; i < 2; i++)
    {
        /*Return the pressed button's ID*/
        if (button_is_pressed(i))
        {
            return i;
        }
    }

    /*No button pressed*/
    return -1;
}

/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id)
{

    /*Your code comes here*/

    return false;
}

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
