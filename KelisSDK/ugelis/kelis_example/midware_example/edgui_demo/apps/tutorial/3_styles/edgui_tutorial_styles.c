/**
 * @file edgui_tutorial_styles.h
 *
 */

/*
 * --------------------------------------------------
 * Learn how to modify the appearance of the objects
 * --------------------------------------------------
 *
 * The appearance of the graphical objects can be customized by styles.
 * A style is simple 'edgui_style_t' variable which is initialized with colors and other parameters.
 * Objects save the address of this variable so it has to be 'static or 'global'.
 *
 * A style contains various attributes to describe rectangle, text, image or line like
 * objects at same time. To know which attribute is used by an object see:
 * http://www.gl.littlev.hu/object-types
 *
 * To set a new style for an object use: 'edgui_xxx_set_style(obj, edgui_..._STYLE_... ,&style);
 * For example the set the release style of a button:
 *      edgui_btn_set_style(btn1, EDGUI_BTN_STYLE_REL, &rel_style)
 *
 * Or if the object has only one style: edgui_label_set_style(label1, &label_style)
 *
 * If NULL is set as style then the object will inherit the parents style.
 * For example is you create a style for button the label's appearance can be defined there as well.
 *
 * You can use built-in styles: "edgui_style_... '
 * Learn more here: http://www.gl.littlev.hu/objects#style
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_tutorial_styles.h"
#if EDGUI_USE_TUTORIALS

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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create objects and styles
 */
void edgui_tutorial_styles(void)
{
    edgui_obj_t * scr = edgui_disp_get_scr_act(NULL);     /*Get the current screen*/

    /****************************************
     * BASE OBJECT + LABEL WITH DEFAULT STYLE
     ****************************************/
    /*Create a simple objects*/
    edgui_obj_t * obj1;
    obj1 = edgui_obj_create(scr, NULL);
    edgui_obj_set_pos(obj1, 10, 10);

    /*Add a label to the object*/
    edgui_obj_t * label;
    label = edgui_label_create(obj1, NULL);
    edgui_label_set_text(label, "Default");
    edgui_obj_align(label, NULL, EDGUI_ALIGN_CENTER, 0, 0);

    /****************************************
     * BASE OBJECT WITH 'PRETTY COLOR' STYLE
     ****************************************/
    /*Create a simple objects*/
    edgui_obj_t * obj2;
    obj2 = edgui_obj_create(scr, NULL);
    edgui_obj_align(obj2, obj1, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);    /*Align next to the previous object*/
    edgui_obj_set_style(obj2, &edgui_style_pretty);                   /*Set built in style*/

    /* Add a label to the object.
     * Labels by default inherit the parent's style */
    label = edgui_label_create(obj2, NULL);
    edgui_label_set_text(label, "Pretty\nstyle");
    edgui_obj_align(label, NULL, EDGUI_ALIGN_CENTER, 0, 0);

    /*****************************
     * BASE OBJECT WITH NEW STYLE
     *****************************/
    /* Create a new style */
    static edgui_style_t style_new;                         /*Styles can't be local variables*/
    edgui_style_copy(&style_new, &edgui_style_pretty);         /*Copy a built-in style as a starting point*/
    style_new.body.radius = EDGUI_RADIUS_CIRCLE;            /*Fully round corners*/
    style_new.body.main_color = EDGUI_COLOR_WHITE;          /*White main color*/
    style_new.body.grad_color = EDGUI_COLOR_BLUE;           /*Blue gradient color*/
    style_new.body.shadow.color = EDGUI_COLOR_SILVER;       /*Light gray shadow color*/
    style_new.body.shadow.width = 8;                     /*8 px shadow*/
    style_new.body.border.width = 2;                     /*2 px border width*/
    style_new.text.color = EDGUI_COLOR_RED;                 /*Red text color */
    style_new.text.letter_space = 10;                    /*10 px letter space*/

    /*Create a base object and apply the new style*/
    edgui_obj_t * obj3;
    obj3 = edgui_obj_create(scr, NULL);
    edgui_obj_align(obj3, obj2, EDGUI_ALIGN_OUT_RIGHT_MID, 20, 0);
    edgui_obj_set_style(obj3, &style_new);

    /* Add a label to the object.
     * Labels by default inherit the parent's style */
    label = edgui_label_create(obj3, NULL);
    edgui_label_set_text(label, "New\nstyle");
    edgui_obj_align(label, NULL, EDGUI_ALIGN_CENTER, 0, 0);


    /************************
     * CREATE A STYLED BAR
     ***********************/
    /* Create a bar background style */
    static edgui_style_t style_bar_bg;
    edgui_style_copy(&style_bar_bg, &edgui_style_pretty);
    style_bar_bg.body.radius = 3;
    style_bar_bg.body.opa = EDGUI_OPA_TRANSP;                  /*Empty (not filled)*/
    style_bar_bg.body.border.color = EDGUI_COLOR_GRAY;         /*Gray border color*/
    style_bar_bg.body.border.width = 6;                     /*2 px border width*/
    style_bar_bg.body.border.opa = EDGUI_OPA_COVER;

    /* Create a bar indicator style */
    static edgui_style_t style_bar_indic;
    edgui_style_copy(&style_bar_indic, &edgui_style_pretty);
    style_bar_indic.body.radius = 3;
    style_bar_indic.body.main_color = EDGUI_COLOR_GRAY;          /*White main color*/
    style_bar_indic.body.grad_color = EDGUI_COLOR_GRAY;           /*Blue gradient color*/
    style_bar_indic.body.border.width = 0;                     /*2 px border width*/
    style_bar_indic.body.padding.left = 8;
    style_bar_indic.body.padding.right = 8;
    style_bar_indic.body.padding.top = 8;
    style_bar_indic.body.padding.bottom = 8;

    /*Create a bar and apply the styles*/
    edgui_obj_t * bar = edgui_bar_create(scr, NULL);
    edgui_bar_set_style(bar, EDGUI_BAR_STYLE_BG, &style_bar_bg);
    edgui_bar_set_style(bar, EDGUI_BAR_STYLE_INDIC, &style_bar_indic);
    edgui_bar_set_value(bar, 70, false);
    edgui_obj_set_size(bar, 200, 30);
    edgui_obj_align(bar, obj1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_TUTORIALS*/
