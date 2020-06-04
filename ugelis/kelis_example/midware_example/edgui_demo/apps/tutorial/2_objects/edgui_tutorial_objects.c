/**
 * @file edgui_tutorial_objects.c
 *
 */

/*
 * ------------------------------------------------
 * Learn how to create GUI elements on the screen
 * ------------------------------------------------
 *
 * The basic building blocks (components or widgets) in edgui are the graphical objects.
 * For example:
 *  - Buttons
 *  - Labels
 *  - Charts
 *  - Sliders etc
 *
 * In this part you can learn the basics of the objects like creating, positioning, sizing etc.
 * You will also meet some different object types and their attributes.
 *
 * Regardless to the object's type the 'edgui_obj_t' variable type is used
 * and you can refer to an object with an edgui_obj_t pointer (edgui_obj_t *)
 *
 * PARENT-CHILD STRUCTURE
 * -------------------------
 * A parent can be considered as the container of its children.
 * Every object has exactly one parent object (except screens).
 * A parent can have unlimited number of children.
 * There is no limitation for the type of the parent.
 *
 * The children are visible only on their parent. The parts outside will be cropped (not displayed)
 *
 * If the parent is moved the children will be moved with it.
 *
 * The earlier created object (and its children) will drawn earlier.
 * Using this layers can be built.
 *
 * INHERITANCE
 * -------------
 * Similarly to object oriented languages some kind of inheritance is used
 * among the object types. Every object is derived from the 'Basic object'. (edgui_obj)
 * The types are backward compatible therefore to set the basic parameters (size, position etc.)
 * you can use 'edgui_obj_set/get_...()' function.

 * LEARN MORE
 * -------------
 * - General overview: http://www.gl.littlev.hu/objects
 * - Detailed description of types: http://www.gl.littlev.hu/object-types
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_tutorial_objects.h"
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
static void btn_event_cb(edgui_obj_t * btn, edgui_event_t event);
static void ddlist_event_cb(edgui_obj_t * ddlist, edgui_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_obj_t * slider;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create some objects
 */
void edgui_tutorial_objects(void)
{

    /********************
     * CREATE A SCREEN
     *******************/
    /* Create a new screen and load it
     * Screen can be created from any type object type
     * Now a Page is used which is an objects with scrollable content*/
    edgui_obj_t * scr = edgui_page_create(NULL, NULL);
    edgui_disp_load_scr(scr);

    /****************
     * ADD A TITLE
     ****************/
    edgui_obj_t * label = edgui_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    edgui_label_set_text(label, "Object usage demo");  /*Set the text*/
    edgui_obj_set_x(label, 50);                        /*Set the x coordinate*/

    /***********************
     * CREATE TWO BUTTONS
     ***********************/
    /*Create a button*/
    edgui_obj_t * btn1 = edgui_btn_create(edgui_disp_get_scr_act(NULL), NULL);         /*Create a button on the currently loaded screen*/
    edgui_obj_set_event_cb(btn1, btn_event_cb);                                  /*Set function to be called when the button is released*/
    edgui_obj_align(btn1, label, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);               /*Align below the label*/

    /*Create a label on the button (the 'label' variable can be reused)*/
    label = edgui_label_create(btn1, NULL);
    edgui_label_set_text(label, "Button 1");

    /*Copy the previous button*/
    edgui_obj_t * btn2 = edgui_btn_create(scr, btn1);                 /*Second parameter is an object to copy*/
    edgui_obj_align(btn2, btn1, EDGUI_ALIGN_OUT_RIGHT_MID, 50, 0);    /*Align next to the prev. button.*/

    /*Create a label on the button*/
    label = edgui_label_create(btn2, NULL);
    edgui_label_set_text(label, "Button 2");

    /****************
     * ADD A SLIDER
     ****************/
    slider = edgui_slider_create(scr, NULL);                            /*Create a slider*/
    edgui_obj_set_size(slider, edgui_obj_get_width(scr)  / 3, EDGUI_DPI / 3);            /*Set the size*/
    edgui_obj_align(slider, btn1, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);                /*Align below the first button*/
    edgui_slider_set_value(slider, 30, false);                                            /*Set the current value*/

    /***********************
     * ADD A DROP DOWN LIST
     ************************/
    edgui_obj_t * ddlist = edgui_ddlist_create(scr, NULL);                     /*Create a drop down list*/
    edgui_obj_align(ddlist, slider, EDGUI_ALIGN_OUT_RIGHT_TOP, 50, 0);         /*Align next to the slider*/
    edgui_obj_set_top(ddlist, true);                                        /*Enable to be on the top when clicked*/
    edgui_ddlist_set_options(ddlist, "None\nLittle\nHalf\nA lot\nAll");     /*Set the options*/
    edgui_obj_set_event_cb(ddlist, ddlist_event_cb);                        /*Set function to call on new option is chosen*/

    /****************
     * CREATE A CHART
     ****************/
    edgui_obj_t * chart = edgui_chart_create(scr, NULL);                         /*Create the chart*/
    edgui_obj_set_size(chart, edgui_obj_get_width(scr) / 2, edgui_obj_get_width(scr) / 4);   /*Set the size*/
    edgui_obj_align(chart, slider, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/
    edgui_chart_set_series_width(chart, 3);                                            /*Set the line width*/

    /*Add a RED data series and set some points*/
    edgui_chart_series_t * dl1 = edgui_chart_add_series(chart, EDGUI_COLOR_RED);
    edgui_chart_set_next(chart, dl1, 10);
    edgui_chart_set_next(chart, dl1, 25);
    edgui_chart_set_next(chart, dl1, 45);
    edgui_chart_set_next(chart, dl1, 80);

    /*Add a BLUE data series and set some points*/
    edgui_chart_series_t * dl2 = edgui_chart_add_series(chart, edgui_color_make(0x40, 0x70, 0xC0));
    edgui_chart_set_next(chart, dl2, 10);
    edgui_chart_set_next(chart, dl2, 25);
    edgui_chart_set_next(chart, dl2, 45);
    edgui_chart_set_next(chart, dl2, 80);
    edgui_chart_set_next(chart, dl2, 75);
    edgui_chart_set_next(chart, dl2, 505);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called when a button is released
 * @param btn pointer to the released button
 * @param event the triggering event
 * @return EDGUI_RES_OK because the object is not deleted in this function
 */
static void btn_event_cb(edgui_obj_t * btn, edgui_event_t event)
{
    if(event == EDGUI_EVENT_RELEASED) {
        /*Increase the button width*/
        edgui_coord_t width = edgui_obj_get_width(btn);
        edgui_obj_set_width(btn, width + 20);
    }
}

/**
 * Called when a new option is chosen in the drop down list
 * @param ddlist pointer to the drop down list
 * @param event the triggering event
 * @return EDGUI_RES_OK because the object is not deleted in this function
 */
static  void ddlist_event_cb(edgui_obj_t * ddlist, edgui_event_t event)
{
    if(event == EDGUI_EVENT_VALUE_CHANGED) {
        uint16_t opt = edgui_ddlist_get_selected(ddlist);            /*Get the id of selected option*/

        edgui_slider_set_value(slider, (opt * 100) / 4, true);       /*Modify the slider value according to the selection*/
    }

}

#endif /*EDGUI_USE_TUTORIALS*/
