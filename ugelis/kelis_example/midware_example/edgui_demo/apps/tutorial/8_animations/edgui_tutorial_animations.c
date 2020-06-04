/**
 * @file edgui_tutorial_animation.h
 *
 */

/*
 * YOu can add animations to improve the user experience.
 * Basically you can animate any attribute by writing a
 * 'void func(void *ptr, int32_t value)' function.
 * The animation will call a function like this to update the value of 'ptr' to 'value'.
 * Fortunately near all 'set' functions in LittlevGL have a similar prototype.
 * E.g. edgui_obj_set_width(edgui_obj_t *obj, edgui_coord_t w)
 * You will see below how declare and start an animation.
 *
 * There are built in animation which can be started with the edgui_obj_anim() function.
 *
 * The other type of animations are the style animations where you can fade
 * one style into an other. See the example below.
 *
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_tutorial_animations.h"
#if EDGUI_USE_TUTORIALS && EDGUI_USE_ANIMATION

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
edgui_style_t btn3_style;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Crate some objects an animate them
 */
void edgui_tutorial_animations(void)
{
    edgui_obj_t * scr = edgui_disp_get_scr_act(NULL);     /*Get the current screen*/

    edgui_obj_t * label;

    /*Create a button the demonstrate built-in animations*/
    edgui_obj_t * btn1;
    btn1 = edgui_btn_create(scr, NULL);
    edgui_obj_set_pos(btn1, 10, 10);       /*Set a position. It will be the animation's destination*/
    edgui_obj_set_size(btn1, 80, 50);

    label = edgui_label_create(btn1, NULL);
    edgui_label_set_text(label, "Float");

    /* Float in the button using a built-in function
     * Delay the animation with 2000 ms and float in 300 ms. NULL means no end callback*/
    edgui_anim_t a;
    a.var = btn1;
    a.start = -edgui_obj_get_height(btn1);
    a.end = edgui_obj_get_y(btn1);
    a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
    a.path_cb = edgui_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = -2000; /*Delay the animation*/
    a.time = 300;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;
    //a.user_data = NULL;
    edgui_anim_create(&a);

    /*Create a button to demonstrate user defined animations*/
    edgui_obj_t * btn2;
    btn2 = edgui_btn_create(scr, NULL);
    edgui_obj_set_pos(btn2, 10, 80);       /*Set a position. It will be the animation's destination*/
    edgui_obj_set_size(btn2, 80, 50);

    label = edgui_label_create(btn2, NULL);
    edgui_label_set_text(label, "Move");

    /*Create an animation to move the button continuously left to right*/
    a.var = btn2;
    a.start = edgui_obj_get_x(btn2);
    a.end = a.start + (100);
    a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_x;
    a.path_cb = edgui_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = -1000;                         /*Negative number to set a delay*/
    a.time = 400;                               /*Animate in 400 ms*/
    a.playback = 1;                             /*Make the animation backward too when it's ready*/
    a.playback_pause = 0;                       /*Wait before playback*/
    a.repeat = 1;                               /*Repeat the animation*/
    a.repeat_pause = 500;                       /*Wait before repeat*/
    edgui_anim_create(&a);

    /*Create a button to demonstrate the style animations*/
    edgui_obj_t * btn3;
    btn3 = edgui_btn_create(scr, NULL);
    edgui_obj_set_pos(btn3, 10, 150);       /*Set a position. It will be the animation's destination*/
    edgui_obj_set_size(btn3, 80, 50);

    label = edgui_label_create(btn3, NULL);
    edgui_label_set_text(label, "Style");

    /*Create a unique style for the button*/
    edgui_style_copy(&btn3_style, edgui_btn_get_style(btn3, EDGUI_BTN_STYLE_REL));
    edgui_btn_set_style(btn3, EDGUI_BTN_STATE_REL, &btn3_style);

    /*Animate the new style*/
    edgui_anim_t sa;
    edgui_style_anim_init(&sa);
    edgui_style_anim_set_styles(&sa, &btn3_style, &edgui_style_btn_rel, &edgui_style_pretty);
    edgui_style_anim_set_time(&sa, 500, 500);
    edgui_style_anim_set_playback(&sa, 500);
    edgui_style_anim_set_repeat(&sa, 500);
    edgui_style_anim_create(&sa);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TUTORIALS && EDGUI_USE_ANIMATION*/

