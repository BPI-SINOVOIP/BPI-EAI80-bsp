/**
 * @file edgui_ex_conf.h
 *
 */
/*
 * COPY THIS FILE AS edgui_ex_conf.h
 */

#ifndef EDGUI_PRJ_CONF_H
#define EDGUI_PRJ_CONF_H

/*******************
 * GENERAL SETTING
 *******************/
#define EDGUI_EX_PRINTF       0       /*Enable printf-ing data*/
#define EDGUI_EX_KEYBOARD     0       /*Add PC keyboard support to some examples (`edgui_drivers` repository is required)*/
#define EDGUI_EX_MOUSEWHEEL   0       /*Add 'encoder' (mouse wheel) support to some examples (`edgui_drivers` repository is required)*/

/*******************
 *   TEST USAGE
 *******************/
#define EDGUI_USE_TESTS        1

/*******************
 * TUTORIAL USAGE
 *******************/
#define EDGUI_USE_TUTORIALS   1


/*********************
 * APPLICATION USAGE
 *********************/

/* Test the graphical performance of your MCU
 * with different settings*/
#define EDGUI_USE_BENCHMARK   0

/*A demo application with Keyboard, Text area, List and Chart
 * placed on Tab view */
#define EDGUI_USE_DEMO        1
#if EDGUI_USE_DEMO
    #define EDGUI_DEMO_WALLPAPER  1    /*Create a wallpaper too*/
    #define EDGUI_DEMO_SLIDE_SHOW 1    /*Automatically switch between tabs*/
#endif

/*MCU and memory usage monitoring*/
#define EDGUI_USE_SYSMON      0

/*A terminal to display received characters*/
#define EDGUI_USE_TERMINAL    0

/*Touch pad calibration with 4 points*/
#define EDGUI_USE_TPCAL       0

#define EDGUI_USE_IMGBTN   1

#endif /*EDGUI_PRJ_CONF_H*/

