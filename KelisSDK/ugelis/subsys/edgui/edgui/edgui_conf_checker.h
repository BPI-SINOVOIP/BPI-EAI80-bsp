/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file edgui_conf_checker.h
 * Make sure all the defines of edgui_conf.h have a default value
**/

#ifndef EDGUI_CONF_CHECKER_H
#define EDGUI_CONF_CHECKER_H
/* clang-format off */

#include <stdint.h>

/*====================
   Graphical settings
 *====================*/

/* Maximal horizontal and vertical resolution to support by the library.*/
#ifndef EDGUI_HOR_RES_MAX
#define EDGUI_HOR_RES_MAX          (480)
#endif
#ifndef EDGUI_VER_RES_MAX
#define EDGUI_VER_RES_MAX          (320)
#endif

/* Color depth:
 * - 1:  1 byte per pixel
 * - 8:  RGB233
 * - 16: RGB565
 * - 32: ARGB8888
 */
#ifndef EDGUI_COLOR_DEPTH
#define EDGUI_COLOR_DEPTH     16
#endif

/* Swap the 2 bytes of RGB565 color.
 * Useful if the display has a 8 bit interface (e.g. SPI)*/
#ifndef EDGUI_COLOR_16_SWAP
#define EDGUI_COLOR_16_SWAP   0
#endif

/* 1: Enable screen transparency.
 * Useful for OSD or other overlapping GUIs.
 * Requires `EDGUI_COLOR_DEPTH = 32` colors and the screen's style should be modified: `style.body.opa = ...`*/
#ifndef EDGUI_COLOR_SCREEN_TRANSP
#define EDGUI_COLOR_SCREEN_TRANSP    0
#endif

/*Images pixels with this color will not be drawn (with chroma keying)*/
#ifndef EDGUI_COLOR_TRANSP
#define EDGUI_COLOR_TRANSP    EDGUI_COLOR_LIME         /*EDGUI_COLOR_LIME: pure green*/
#endif

/* Enable anti-aliasing (lines, and radiuses will be smoothed) */
#ifndef EDGUI_ANTIALIAS
#define EDGUI_ANTIALIAS        1
#endif

/* Default display refresh period.
 * Can be changed in the display driver (`edgui_disp_drv_t`).*/
#ifndef EDGUI_DISP_DEF_REFR_PERIOD
#define EDGUI_DISP_DEF_REFR_PERIOD      30      /*[ms]*/
#endif

/* Dot Per Inch: used to initialize default sizes.
 * E.g. a button with width = EDGUI_DPI / 2 -> half inch wide
 * (Not so important, you can adjust it to modify default sizes and spaces)*/
#ifndef EDGUI_DPI
#define EDGUI_DPI              100     /*[px]*/
#endif

/* Type of coordinates. Should be `int16_t` (or `int32_t` for extreme cases) */

/*=========================
   Memory manager settings
 *=========================*/

/* LittelvGL's internal memory manager's settings.
 * The graphical objects and other related data are stored here. */

/* 1: use custom malloc/free, 0: use the built-in `edgui_mem_alloc` and `edgui_mem_free` */
#ifndef EDGUI_MEM_CUSTOM
#define EDGUI_MEM_CUSTOM      0
#endif
#if EDGUI_MEM_CUSTOM == 0
/* Size of the memory used by `edgui_mem_alloc` in bytes (>= 2kB)*/
#ifndef EDGUI_MEM_SIZE
#  define EDGUI_MEM_SIZE    (32U * 1024U)
#endif

/* Complier prefix for a big array declaration */
#ifndef EDGUI_MEM_ATTR
#  define EDGUI_MEM_ATTR
#endif

/* Set an address for the memory pool instead of allocating it as an array.
 * Can be in external SRAM too. */
#ifndef EDGUI_MEM_ADR
#  define EDGUI_MEM_ADR          0
#endif

/* Automatically defrag. on free. Defrag. means joining the adjacent free cells. */
#ifndef EDGUI_MEM_AUTO_DEFRAG
#  define EDGUI_MEM_AUTO_DEFRAG  1
#endif
#else       /*EDGUI_MEM_CUSTOM*/
#ifndef EDGUI_MEM_CUSTOM_INCLUDE
#  define EDGUI_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
#endif
#ifndef EDGUI_MEM_CUSTOM_ALLOC
#  define EDGUI_MEM_CUSTOM_ALLOC   malloc       /*Wrapper to malloc*/
#endif
#ifndef EDGUI_MEM_CUSTOM_FREE
#  define EDGUI_MEM_CUSTOM_FREE    free         /*Wrapper to free*/
#endif
#endif     /*EDGUI_MEM_CUSTOM*/

/* Garbage Collector settings
 * Used if edgui is binded to higher level language and the memory is managed by that language */
#ifndef EDGUI_ENABLE_GC
#define EDGUI_ENABLE_GC 0
#endif
#if EDGUI_ENABLE_GC != 0
#ifndef EDGUI_GC_INCLUDE
#  define EDGUI_GC_INCLUDE "gc.h"                           /*Include Garbage Collector related things*/
#endif
#ifndef EDGUI_MEM_CUSTOM_REALLOC
#  define EDGUI_MEM_CUSTOM_REALLOC   your_realloc           /*Wrapper to realloc*/
#endif
#ifndef EDGUI_MEM_CUSTOM_GET_SIZE
#  define EDGUI_MEM_CUSTOM_GET_SIZE  your_mem_get_size      /*Wrapper to edgui_mem_get_size*/
#endif
#endif /* EDGUI_ENABLE_GC */

/*=======================
   Input device settings
 *=======================*/

/* Input device default settings.
 * Can be changed in the Input device driver (`edgui_indev_drv_t`)*/

/* Input device read period in milliseconds */
#ifndef EDGUI_INDEV_DEF_READ_PERIOD
#define EDGUI_INDEV_DEF_READ_PERIOD          30
#endif

/* Drag threshold in pixels */
#ifndef EDGUI_INDEV_DEF_DRAG_LIMIT
#define EDGUI_INDEV_DEF_DRAG_LIMIT           10
#endif

/* Drag throw slow-down in [%]. Greater value -> faster slow-down */
#ifndef EDGUI_INDEV_DEF_DRAG_THROW
#define EDGUI_INDEV_DEF_DRAG_THROW           20
#endif

/* Long press time in milliseconds.
 * Time to send `EDGUI_EVENT_LONG_PRESSSED`) */
#ifndef EDGUI_INDEV_DEF_LONG_PRESS_TIME
#define EDGUI_INDEV_DEF_LONG_PRESS_TIME      400
#endif

/* Repeated trigger period in long press [ms]
 * Time between `EDGUI_EVENT_LONG_PRESSED_REPEAT */
#ifndef EDGUI_INDEV_DEF_LONG_PRESS_REP_TIME
#define EDGUI_INDEV_DEF_LONG_PRESS_REP_TIME  100
#endif

/*==================
 * Feature usage
 *==================*/

/*1: Enable the Animations */
#ifndef EDGUI_USE_ANIMATION
#define EDGUI_USE_ANIMATION        1
#endif
#if EDGUI_USE_ANIMATION

/*Declare the type of the user data of animations (can be e.g. `void *`, `int`, `struct`)*/

#endif

/* 1: Enable shadow drawing*/
#ifndef EDGUI_USE_SHADOW
#define EDGUI_USE_SHADOW           1
#endif

/* 1: Enable object groups (for keyboard/encoder navigation) */
#ifndef EDGUI_USE_GROUP
#define EDGUI_USE_GROUP            1
#endif
#if EDGUI_USE_GROUP
#endif  /*EDGUI_USE_GROUP*/

/* 1: Enable GPU interface*/
#ifndef EDGUI_USE_GPU
#define EDGUI_USE_GPU              1
#endif

/* 1: Enable file system (might be required for images */
#ifndef EDGUI_USE_FILESYSTEM
#define EDGUI_USE_FILESYSTEM       1
#endif
#if EDGUI_USE_FILESYSTEM
/*Declare the type of the user data of file system drivers (can be e.g. `void *`, `int`, `struct`)*/
#endif

/*1: Add a `user_data` to drivers and objects*/
#ifndef EDGUI_USE_USER_DATA
#define EDGUI_USE_USER_DATA        0
#endif

/*========================
 * Image decoder and cache
 *========================*/

/* 1: Enable indexed (palette) images */
#ifndef EDGUI_IMG_CF_INDEXED
#define EDGUI_IMG_CF_INDEXED       1
#endif

/* 1: Enable alpha indexed images */
#ifndef EDGUI_IMG_CF_ALPHA
#define EDGUI_IMG_CF_ALPHA         1
#endif

/* Default image cache size. Image caching keeps the images opened.
 * If only the built-in image formats are used there is no real advantage of caching.
 * (I.e. no new image decoder is added)
 * With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
 * However the opened images might consume additional RAM.
 * EDGUI_IMG_CACHE_DEF_SIZE must be >= 1 */
#ifndef EDGUI_IMG_CACHE_DEF_SIZE
#define EDGUI_IMG_CACHE_DEF_SIZE       1
#endif

/*Declare the type of the user data of image decoder (can be e.g. `void *`, `int`, `struct`)*/

/*=====================
 *  Compiler settings
 *====================*/
/* Define a custom attribute to `edgui_tick_inc` function */
#ifndef EDGUI_ATTRIBUTE_TICK_INC
#define EDGUI_ATTRIBUTE_TICK_INC
#endif

/* Define a custom attribute to `edgui_task_handler` function */
#ifndef EDGUI_ATTRIBUTE_TASK_HANDLER
#define EDGUI_ATTRIBUTE_TASK_HANDLER
#endif

/* With size optimization (-Os) the compiler might not align data to
 * 4 or 8 byte boundary. This alignment will be explicitly applied where needed.
 * E.g. __attribute__((aligned(4))) */
#ifndef EDGUI_ATTRIBUTE_MEM_ALIGN
#define EDGUI_ATTRIBUTE_MEM_ALIGN
#endif

/* Attribute to mark large constant arrays for example
 * font's bitmaps */
#ifndef EDGUI_ATTRIBUTE_LARGE_CONST
#define EDGUI_ATTRIBUTE_LARGE_CONST
#endif

/*===================
 *  HAL settings
 *==================*/

/* 1: use a custom tick source.
 * It removes the need to manually update the tick with `edgui_tick_inc`) */
#ifndef EDGUI_TICK_CUSTOM
#define EDGUI_TICK_CUSTOM     0
#endif
#if EDGUI_TICK_CUSTOM == 1
#ifndef EDGUI_TICK_CUSTOM_INCLUDE
#define EDGUI_TICK_CUSTOM_INCLUDE  "something.h"       /*Header for the sys time function*/
#endif
#ifndef EDGUI_TICK_CUSTOM_SYS_TIME_EXPR
#define EDGUI_TICK_CUSTOM_SYS_TIME_EXPR (millis())     /*Expression evaluating to current systime in ms*/
#endif
#endif   /*EDGUI_TICK_CUSTOM*/


/*================
 * Log settings
 *===============*/

/*1: Enable the log module*/
#ifndef EDGUI_USE_LOG
#define EDGUI_USE_LOG      0
#endif
#if EDGUI_USE_LOG
/* How important log should be added:
 * EDGUI_LOG_LEVEL_TRACE       A lot of logs to give detailed information
 * EDGUI_LOG_LEVEL_INFO        Log important events
 * EDGUI_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
 * EDGUI_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
 * EDGUI_LOG_LEVEL_NONE        Do not log anything
 */
#ifndef EDGUI_LOG_LEVEL
#  define EDGUI_LOG_LEVEL    EDGUI_LOG_LEVEL_WARN
#endif

/* 1: Print the log with 'printf';
 * 0: user need to register a callback with `edgui_log_register_print`*/
#ifndef EDGUI_LOG_PRINTF
#  define EDGUI_LOG_PRINTF   0
#endif
#endif  /*EDGUI_USE_LOG*/

/*================
 *  THEME USAGE
 *================*/
#ifndef EDGUI_THEME_LIVE_UPDATE
#define EDGUI_THEME_LIVE_UPDATE    0   /*1: Allow theme switching at run time. Uses 8..10 kB of RAM*/
#endif

#ifndef EDGUI_USE_THEME_TEMPL
#define EDGUI_USE_THEME_TEMPL      0   /*Just for test*/
#endif
#ifndef EDGUI_USE_THEME_DEFAULT
#define EDGUI_USE_THEME_DEFAULT    0   /*Built mainly from the built-in styles. Consumes very few RAM*/
#endif
#ifndef EDGUI_USE_THEME_ALIEN
#define EDGUI_USE_THEME_ALIEN      0   /*Dark futuristic theme*/
#endif
#ifndef EDGUI_USE_THEME_NIGHT
#define EDGUI_USE_THEME_NIGHT      0   /*Dark elegant theme*/
#endif
#ifndef EDGUI_USE_THEME_MONO
#define EDGUI_USE_THEME_MONO       0   /*Mono color theme for monochrome displays*/
#endif
#ifndef EDGUI_USE_THEME_MATERIAL
#define EDGUI_USE_THEME_MATERIAL   0   /*Flat theme with bold colors and light shadows*/
#endif
#ifndef EDGUI_USE_THEME_ZEN
#define EDGUI_USE_THEME_ZEN        0   /*Peaceful, mainly light theme */
#endif
#ifndef EDGUI_USE_THEME_NEMO
#define EDGUI_USE_THEME_NEMO       0   /*Water-like theme based on the movie "Finding Nemo"*/
#endif

/*==================
 *    FONT USAGE
 *===================*/

/* The built-in fonts contains the ASCII range and some Symbols with  4 bit-per-pixel.
 * The symbols are available via `EDGUI_SYMBOL_...` defines
 * More info about fonts: https://docs.edgui.com/#Fonts
 * To create a new font go to: https://edgui.com/ttf-font-to-c-array
 */

/* Robot fonts with bpp = 4
 * https://fonts.google.com/specimen/Roboto  */
#ifndef EDGUI_FONT_ROBOTO_12
#define EDGUI_FONT_ROBOTO_12    0
#endif
#ifndef EDGUI_FONT_ROBOTO_16
#define EDGUI_FONT_ROBOTO_16    1
#endif
#ifndef EDGUI_FONT_ROBOTO_22
#define EDGUI_FONT_ROBOTO_22    0
#endif
#ifndef EDGUI_FONT_ROBOTO_28
#define EDGUI_FONT_ROBOTO_28    0
#endif

/*Pixel perfect monospace font
 * http://pelulamu.net/unscii/ */
#ifndef EDGUI_FONT_UNSCII_8
#define EDGUI_FONT_UNSCII_8     0
#endif

/* Optionally declare your custom fonts here.
 * You can use these fonts as default font too
 * and they will be available globally. E.g.
 * #define EDGUI_FONT_CUSTOM_DECLARE EDGUI_FONT_DECLARE(my_font_1) \
 *                                EDGUI_FONT_DECLARE(my_font_2)
 */
#ifndef EDGUI_FONT_CUSTOM_DECLARE
#define EDGUI_FONT_CUSTOM_DECLARE
#endif

/*Always set a default font from the built-in fonts*/
#ifndef EDGUI_FONT_DEFAULT
#define EDGUI_FONT_DEFAULT        &edgui_font_roboto_16
#endif

/* Enable it if you have fonts with a lot of characters.
 * The limit depends on the font size, font face and bpp
 * but with > 10,000 characters if you see issues probably you need to enable it.*/
#ifndef EDGUI_FONT_FMT_TXT_LARGE
#define EDGUI_FONT_FMT_TXT_LARGE   0
#endif

/*Declare the type of the user data of fonts (can be e.g. `void *`, `int`, `struct`)*/

/*=================
 *  Text settings
 *=================*/

/* Select a character encoding for strings.
 * Your IDE or editor should have the same character encoding
 * - EDGUI_TXT_ENC_UTF8
 * - EDGUI_TXT_ENC_ASCII
 * */
#ifndef EDGUI_TXT_ENC
#define EDGUI_TXT_ENC EDGUI_TXT_ENC_UTF8
#endif

 /*Can break (wrap) texts on these chars*/
#ifndef EDGUI_TXT_BREAK_CHARS
#define EDGUI_TXT_BREAK_CHARS                  " ,.;:-_"
#endif

/*===================
 *  EDGUI_OBJ SETTINGS
 *==================*/

/*Declare the type of the user data of object (can be e.g. `void *`, `int`, `struct`)*/

/*1: enable `edgui_obj_realaign()` based on `edgui_obj_align()` parameters*/
#ifndef EDGUI_USE_OBJ_REALIGN
#define EDGUI_USE_OBJ_REALIGN          1
#endif

/* Enable to make the object clickable on a larger area.
 * EDGUI_EXT_CLICK_AREA_OFF or 0: Disable this feature
 * EDGUI_EXT_CLICK_AREA_TINY: The extra area can be adjusted horizontally and vertically (0..255 px)
 * EDGUI_EXT_CLICK_AREA_FULL: The extra area can be adjusted in all 4 directions (-32k..+32k px)
 */
#ifndef EDGUI_USE_EXT_CLICK_AREA
#define EDGUI_USE_EXT_CLICK_AREA  EDGUI_EXT_CLICK_AREA_OFF
#endif

/*==================
 *  LV OBJ X USAGE
 *================*/
/*
 * Documentation of the object types: https://docs.edgui.com/#Object-types
 */

/*Arc (dependencies: -)*/
#ifndef EDGUI_USE_ARC
#define EDGUI_USE_ARC      1
#endif

/*Bar (dependencies: -)*/
#ifndef EDGUI_USE_BAR
#define EDGUI_USE_BAR      1
#endif

/*Button (dependencies: edgui_cont*/
#ifndef EDGUI_USE_BTN
#define EDGUI_USE_BTN      1
#endif
#if EDGUI_USE_BTN != 0
/*Enable button-state animations - draw a circle on click (dependencies: EDGUI_USE_ANIMATION)*/
#ifndef EDGUI_BTN_INK_EFFECT
#  define EDGUI_BTN_INK_EFFECT   0
#endif
#endif

/*Button matrix (dependencies: -)*/
#ifndef EDGUI_USE_BTNM
#define EDGUI_USE_BTNM     1
#endif

/*Calendar (dependencies: -)*/
#ifndef EDGUI_USE_CALENDAR
#define EDGUI_USE_CALENDAR 1
#endif

/*Canvas (dependencies: edgui_img)*/
#ifndef EDGUI_USE_CANVAS
#define EDGUI_USE_CANVAS   1
#endif

/*Check box (dependencies: edgui_btn, edgui_label)*/
#ifndef EDGUI_USE_CB
#define EDGUI_USE_CB       1
#endif

/*Chart (dependencies: -)*/
#ifndef EDGUI_USE_CHART
#define EDGUI_USE_CHART    1
#endif
#if EDGUI_USE_CHART
#ifndef EDGUI_CHART_AXIS_TICK_LABEL_MAX_LEN
#  define EDGUI_CHART_AXIS_TICK_LABEL_MAX_LEN    20
#endif
#endif

/*Container (dependencies: -*/
#ifndef EDGUI_USE_CONT
#define EDGUI_USE_CONT     1
#endif

/*Drop down list (dependencies: edgui_page, edgui_label, edgui_symbol_def.h)*/
#ifndef EDGUI_USE_DDLIST
#define EDGUI_USE_DDLIST    1
#endif
#if EDGUI_USE_DDLIST != 0
/*Open and close default animation time [ms] (0: no animation)*/
#ifndef EDGUI_DDLIST_DEF_ANIM_TIME
#  define EDGUI_DDLIST_DEF_ANIM_TIME     200
#endif
#endif

/*Gauge (dependencies:edgui_bar, edgui_lmeter)*/
#ifndef EDGUI_USE_GAUGE
#define EDGUI_USE_GAUGE    1
#endif

/*Image (dependencies: edgui_label*/
#ifndef EDGUI_USE_IMG
#define EDGUI_USE_IMG      1
#endif

/*Image Button (dependencies: edgui_btn*/
#ifndef EDGUI_USE_IMGBTN
#define EDGUI_USE_IMGBTN   1
#endif
#if EDGUI_USE_IMGBTN
/*1: The imgbtn requires left, mid and right parts and the width can be set freely*/
#ifndef EDGUI_IMGBTN_TILED
#  define EDGUI_IMGBTN_TILED 0
#endif
#endif

/*Keyboard (dependencies: edgui_btnm)*/
#ifndef EDGUI_USE_KB
#define EDGUI_USE_KB       1
#endif

/*Label (dependencies: -*/
#ifndef EDGUI_USE_LABEL
#define EDGUI_USE_LABEL    1
#endif
#if EDGUI_USE_LABEL != 0
/*Hor, or ver. scroll speed [px/sec] in 'EDGUI_LABEL_LONG_ROLL/ROLL_CIRC' mode*/
#ifndef EDGUI_LABEL_DEF_SCROLL_SPEED
#  define EDGUI_LABEL_DEF_SCROLL_SPEED       25
#endif

/* Waiting period at beginning/end of animation cycle */
#ifndef EDGUI_LABEL_WAIT_CHAR_COUNT
#  define EDGUI_LABEL_WAIT_CHAR_COUNT        3
#endif

/*Enable selecting text of the label */
#ifndef EDGUI_LABEL_TEXT_SEL
#  define EDGUI_LABEL_TEXT_SEL               0
#endif

/*Store extra some info in labels (12 bytes) to speed up drawing of very long texts*/
#ifndef EDGUI_LABEL_LONG_TXT_HINT
#  define EDGUI_LABEL_LONG_TXT_HINT          0
#endif
#endif

/*LED (dependencies: -)*/
#ifndef EDGUI_USE_LED
#define EDGUI_USE_LED      1
#endif

/*Line (dependencies: -*/
#ifndef EDGUI_USE_LINE
#define EDGUI_USE_LINE     1
#endif

/*List (dependencies: edgui_page, edgui_btn, edgui_label, (edgui_img optionally for icons ))*/
#ifndef EDGUI_USE_LIST
#define EDGUI_USE_LIST     1
#endif
#if EDGUI_USE_LIST != 0
/*Default animation time of focusing to a list element [ms] (0: no animation)  */
#ifndef EDGUI_LIST_DEF_ANIM_TIME
#  define EDGUI_LIST_DEF_ANIM_TIME  100
#endif
#endif

/*Line meter (dependencies: *;)*/
#ifndef EDGUI_USE_LMETER
#define EDGUI_USE_LMETER   1
#endif

/*Message box (dependencies: edgui_rect, edgui_btnm, edgui_label)*/
#ifndef EDGUI_USE_MBOX
#define EDGUI_USE_MBOX     1
#endif

/*Page (dependencies: edgui_cont)*/
#ifndef EDGUI_USE_PAGE
#define EDGUI_USE_PAGE     1
#endif
#if EDGUI_USE_PAGE != 0
/*Focus default animation time [ms] (0: no animation)*/
#ifndef EDGUI_PAGE_DEF_ANIM_TIME
#  define EDGUI_PAGE_DEF_ANIM_TIME     400
#endif
#endif

/*Preload (dependencies: edgui_arc, edgui_anim)*/
#ifndef EDGUI_USE_PRELOAD
#define EDGUI_USE_PRELOAD      1
#endif
#if EDGUI_USE_PRELOAD != 0
#ifndef EDGUI_PRELOAD_DEF_ARC_LENGTH
#  define EDGUI_PRELOAD_DEF_ARC_LENGTH   60      /*[deg]*/
#endif
#ifndef EDGUI_PRELOAD_DEF_SPIN_TIME
#  define EDGUI_PRELOAD_DEF_SPIN_TIME    1000    /*[ms]*/
#endif
#ifndef EDGUI_PRELOAD_DEF_ANIM
#  define EDGUI_PRELOAD_DEF_ANIM         EDGUI_PRELOAD_TYPE_SPINNING_ARC
#endif
#endif

/*Roller (dependencies: edgui_ddlist)*/
#ifndef EDGUI_USE_ROLLER
#define EDGUI_USE_ROLLER    1
#endif
#if EDGUI_USE_ROLLER != 0
/*Focus animation time [ms] (0: no animation)*/
#ifndef EDGUI_ROLLER_DEF_ANIM_TIME
#  define EDGUI_ROLLER_DEF_ANIM_TIME     200
#endif

/*Number of extra "pages" when the roller is infinite*/
#ifndef EDGUI_ROLLER_INF_PAGES
#  define EDGUI_ROLLER_INF_PAGES         7
#endif
#endif

/*Slider (dependencies: edgui_bar)*/
#ifndef EDGUI_USE_SLIDER
#define EDGUI_USE_SLIDER    1
#endif

/*Spinbox (dependencies: edgui_ta)*/
#ifndef EDGUI_USE_SPINBOX
#define EDGUI_USE_SPINBOX       1
#endif

/*Switch (dependencies: edgui_slider)*/
#ifndef EDGUI_USE_SW
#define EDGUI_USE_SW       1
#endif

/*Text area (dependencies: edgui_label, edgui_page)*/
#ifndef EDGUI_USE_TA
#define EDGUI_USE_TA       1
#endif
#if EDGUI_USE_TA != 0
#ifndef EDGUI_TA_DEF_CURSOR_BLINK_TIME
#  define EDGUI_TA_DEF_CURSOR_BLINK_TIME 400     /*ms*/
#endif
#ifndef EDGUI_TA_DEF_PWD_SHOW_TIME
#  define EDGUI_TA_DEF_PWD_SHOW_TIME     1500    /*ms*/
#endif
#endif

/*Table (dependencies: edgui_label)*/
#ifndef EDGUI_USE_TABLE
#define EDGUI_USE_TABLE    1
#endif
#if EDGUI_USE_TABLE
#ifndef EDGUI_TABLE_COL_MAX
#  define EDGUI_TABLE_COL_MAX    12
#endif
#endif

/*Tab (dependencies: edgui_page, edgui_btnm)*/
#ifndef EDGUI_USE_TABVIEW
#define EDGUI_USE_TABVIEW      1
#endif
#  if EDGUI_USE_TABVIEW != 0
/*Time of slide animation [ms] (0: no animation)*/
#ifndef EDGUI_TABVIEW_DEF_ANIM_TIME
#  define EDGUI_TABVIEW_DEF_ANIM_TIME    300
#endif
#endif

/*Tileview (dependencies: edgui_page) */
#ifndef EDGUI_USE_TILEVIEW
#define EDGUI_USE_TILEVIEW     1
#endif
#if EDGUI_USE_TILEVIEW
/*Time of slide animation [ms] (0: no animation)*/
#ifndef EDGUI_TILEVIEW_DEF_ANIM_TIME
#  define EDGUI_TILEVIEW_DEF_ANIM_TIME   300
#endif
#endif

/*Window (dependencies: edgui_cont, edgui_btn, edgui_label, edgui_img, edgui_page)*/
#ifndef EDGUI_USE_WIN
#define EDGUI_USE_WIN      1
#endif

/*==================
 * Non-user section
 *==================*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /* Disable warnings for Visual Studio*/
#ifndef _CRT_SECURE_NO_WARNINGS
#  define _CRT_SECURE_NO_WARNINGS
#endif
#endif


#endif  /*EDGUI_CONF_CHECKER_H*/
