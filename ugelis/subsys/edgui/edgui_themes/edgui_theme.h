/**
 *@file edgui_themes.h
 *
 */

#ifndef EDGUI_THEMES_H
#define EDGUI_THEMES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *    INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui_conf.h"
#else
#include "../edgui/edgui_conf.h"
#endif

#include "../edgui_core/edgui_style.h"
#include "../edgui_core/edgui_group.h"

/*********************
 *    DEFINES
 *********************/

/**********************
 *    TYPEDEFS
 **********************/

/**
 * A theme in edgui consists of many styles bound together.
 * 
 * There is a style for each object type, as well as a generic style for
 * backgrounds and panels.
 */
typedef struct
{
    struct
    {
        edgui_style_t * scr;
        edgui_style_t * bg;
        edgui_style_t * panel;

#if EDGUI_USE_CONT != 0
        edgui_style_t * cont;
#endif

#if EDGUI_USE_BTN != 0
        struct
        {
            edgui_style_t * rel;
            edgui_style_t * pr;
            edgui_style_t * tgl_rel;
            edgui_style_t * tgl_pr;
            edgui_style_t * ina;
        } btn;
#endif

#if EDGUI_USE_IMGBTN != 0
        struct
        {
            edgui_style_t * rel;
            edgui_style_t * pr;
            edgui_style_t * tgl_rel;
            edgui_style_t * tgl_pr;
            edgui_style_t * ina;
        } imgbtn;
#endif

#if EDGUI_USE_LABEL != 0
        struct
        {
            edgui_style_t * prim;
            edgui_style_t * sec;
            edgui_style_t * hint;
        } label;
#endif

#if EDGUI_USE_IMG != 0
        struct
        {
            edgui_style_t * light;
            edgui_style_t * dark;
        } img;
#endif

#if EDGUI_USE_LINE != 0
        struct
        {
            edgui_style_t * decor;
        } line;
#endif

#if EDGUI_USE_LED != 0
        edgui_style_t * led;
#endif

#if EDGUI_USE_BAR != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * indic;
        } bar;
#endif

#if EDGUI_USE_SLIDER != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * indic;
            edgui_style_t * knob;
        } slider;
#endif

#if EDGUI_USE_LMETER != 0
        edgui_style_t * lmeter;
#endif

#if EDGUI_USE_GAUGE != 0
        edgui_style_t * gauge;
#endif

#if EDGUI_USE_ARC != 0
        edgui_style_t * arc;
#endif

#if EDGUI_USE_PRELOAD != 0
        edgui_style_t * preload;
#endif

#if EDGUI_USE_SW != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * indic;
            edgui_style_t * knob_off;
            edgui_style_t * knob_on;
        } sw;
#endif

#if EDGUI_USE_CHART != 0
        edgui_style_t * chart;
#endif

#if EDGUI_USE_CALENDAR != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * header;
            edgui_style_t * header_pr;
            edgui_style_t * day_names;
            edgui_style_t * highlighted_days;
            edgui_style_t * inactive_days;
            edgui_style_t * week_box;
            edgui_style_t * today_box;
        } calendar;
#endif

#if EDGUI_USE_CB != 0
        struct
        {
            edgui_style_t * bg;
            struct
            {
                edgui_style_t * rel;
                edgui_style_t * pr;
                edgui_style_t * tgl_rel;
                edgui_style_t * tgl_pr;
                edgui_style_t * ina;
            } box;
        } cb;
#endif

#if EDGUI_USE_BTNM != 0
        struct
        {
            edgui_style_t * bg;
            struct
            {
                edgui_style_t * rel;
                edgui_style_t * pr;
                edgui_style_t * tgl_rel;
                edgui_style_t * tgl_pr;
                edgui_style_t * ina;
            } btn;
        } btnm;
#endif

#if EDGUI_USE_KB != 0
        struct
        {
            edgui_style_t * bg;
            struct
            {
                edgui_style_t * rel;
                edgui_style_t * pr;
                edgui_style_t * tgl_rel;
                edgui_style_t * tgl_pr;
                edgui_style_t * ina;
            } btn;
        } kb;
#endif

#if EDGUI_USE_MBOX != 0
        struct
        {
            edgui_style_t * bg;
            struct
            {
                edgui_style_t * bg;
                edgui_style_t * rel;
                edgui_style_t * pr;
            } btn;
        } mbox;
#endif

#if EDGUI_USE_PAGE != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * scrl;
            edgui_style_t * sb;
        } page;
#endif

#if EDGUI_USE_TA != 0
        struct
        {
            edgui_style_t * area;
            edgui_style_t * oneline;
            edgui_style_t * cursor;
            edgui_style_t * sb;
        } ta;
#endif

#if EDGUI_USE_SPINBOX != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * cursor;
            edgui_style_t * sb;
        } spinbox;
#endif

#if EDGUI_USE_LIST
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * scrl;
            edgui_style_t * sb;
            struct
            {
                edgui_style_t * rel;
                edgui_style_t * pr;
                edgui_style_t * tgl_rel;
                edgui_style_t * tgl_pr;
                edgui_style_t * ina;
            } btn;
        } list;
#endif

#if EDGUI_USE_DDLIST != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * sel;
            edgui_style_t * sb;
        } ddlist;
#endif

#if EDGUI_USE_ROLLER != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * sel;
        } roller;
#endif

#if EDGUI_USE_TABVIEW != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * indic;
            struct
            {
                edgui_style_t * bg;
                edgui_style_t * rel;
                edgui_style_t * pr;
                edgui_style_t * tgl_rel;
                edgui_style_t * tgl_pr;
            } btn;
        } tabview;
#endif

#if EDGUI_USE_TILEVIEW != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * scrl;
            edgui_style_t * sb;
        } tileview;
#endif

#if EDGUI_USE_TABLE != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * cell;
        } table;
#endif

#if EDGUI_USE_WIN != 0
        struct
        {
            edgui_style_t * bg;
            edgui_style_t * sb;
            edgui_style_t * header;
            edgui_style_t * content;
            struct
            {
                edgui_style_t * rel;
                edgui_style_t * pr;
            } btn;
        } win;
#endif
    } style;

#if EDGUI_USE_GROUP
    struct
    {
        /* The `x` in the names inidicates that inconsistence becasue
         * the group related function are stored in the theme.*/
        edgui_group_style_mod_cb_t style_mod_xcb;
        edgui_group_style_mod_cb_t style_mod_edit_xcb;
    } group;
#endif
} edgui_theme_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * Set a theme for the system.
 * From now, all the created objects will use styles from this theme by default
 * @param th pointer to theme (return value of: 'edgui_theme_init_xxx()')
 */
void edgui_theme_set_current(edgui_theme_t * th);

/**
 * Get the current system theme.
 * @return pointer to the current system theme. NULL if not set.
 */
edgui_theme_t * edgui_theme_get_current(void);

/**********************
 *    MACROS
 **********************/

/* Returns number of styles within the `edgui_theme_t` structure. */
#define EDGUI_THEME_STYLE_COUNT (sizeof(((edgui_theme_t *)0)->style) / sizeof(edgui_style_t *))

/**********************
 *     POST INCLUDE
 *********************/
#include "edgui_theme_templ.h"
#include "edgui_theme_default.h"
#include "edgui_theme_alien.h"
#include "edgui_theme_night.h"
#include "edgui_theme_zen.h"
#include "edgui_theme_mono.h"
#include "edgui_theme_nemo.h"
#include "edgui_theme_material.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_THEMES_H*/
