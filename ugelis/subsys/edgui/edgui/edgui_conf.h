/**
 * @file edgui_conf.h
 *
 */

/*
 * COPY THIS FILE AS `edgui_conf.h` NEXT TO the `edgui` FOLDER
 */
#ifndef EDGUI_CONF_H
    #define EDGUI_CONF_H
    /* clang-format off */

    #include <stdint.h>

    /*====================
    Graphical settings
    *====================*/

    /* Maximal horizontal and vertical resolution to support by the library.*/
    /* 7'LCD 800x480*/
    #define EDGUI_HOR_RES_MAX          (800)
    #define EDGUI_VER_RES_MAX          (480)
    /* 7'LCD 1024x600*/
    //  #define EDGUI_HOR_RES_MAX          (1024)
    //  #define EDGUI_VER_RES_MAX          (600)
    /* 4.3'LCD 480x272*/
    //  #define EDGUI_HOR_RES_MAX          (480)
    //  #define EDGUI_VER_RES_MAX          (272)

    /* Color depth:
    * - 1:  1 byte per pixel
    * - 8:  RGB233
    * - 16: RGB565
    * - 32: ARGB8888
    */
    #define EDGUI_COLOR_DEPTH     16

    /* Swap the 2 bytes of RGB565 color.
    * Useful if the display has a 8 bit interface (e.g. SPI)*/
    #define EDGUI_COLOR_16_SWAP   0

    /* 1: Enable screen transparency.
    * Useful for OSD or other overlapping GUIs.
    * Requires `EDGUI_COLOR_DEPTH = 32` colors and the screen's style should be modified: `style.body.opa = ...`*/
    #define EDGUI_COLOR_SCREEN_TRANSP    0

    /*Images pixels with this color will not be drawn (with chroma keying)*/
    #define EDGUI_COLOR_TRANSP    EDGUI_COLOR_LIME         /*EDGUI_COLOR_LIME: pure green*/

    /* Enable anti-aliasing (lines, and radiuses will be smoothed) */
    #define EDGUI_ANTIALIAS        1

    /* Default display refresh period.
    * Can be changed in the display driver (`edgui_disp_drv_t`).*/
    #define EDGUI_DISP_DEF_REFR_PERIOD      30     /*[ms]*/

    /* Dot Per Inch: used to initialize default sizes.
    * E.g. a button with width = EDGUI_DPI / 2 -> half inch wide
    * (Not so important, you can adjust it to modify default sizes and spaces)*/
    #define EDGUI_DPI              100     /*[px]*/

    /* Type of coordinates. Should be `int16_t` (or `int32_t` for extreme cases) */
    typedef int16_t edgui_coord_t;

    /*=========================
    Memory manager settings
    *=========================*/

    /* LittelvGL's internal memory manager's settings.
    * The graphical objects and other related data are stored here. */

    /* 1: use custom malloc/free, 0: use the built-in `edgui_mem_alloc` and `edgui_mem_free` */
    #define EDGUI_MEM_CUSTOM      0
    #if EDGUI_MEM_CUSTOM == 0
        /* Size of the memory used by `edgui_mem_alloc` in bytes (>= 2kB)*/
        #define EDGUI_MEM_SIZE    (256U * 1024U)

        /* Complier prefix for a big array declaration */
        #define EDGUI_MEM_ATTR

        /* Set an address for the memory pool instead of allocating it as an array.
        * Can be in external SRAM too. */
        #define EDGUI_MEM_ADR          0

        /* Automatically defrag. on free. Defrag. means joining the adjacent free cells. */
        #define EDGUI_MEM_AUTO_DEFRAG  1
    #else       /*EDGUI_MEM_CUSTOM*/
        #define EDGUI_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
        #define EDGUI_MEM_CUSTOM_ALLOC   malloc       /*Wrapper to malloc*/
        #define EDGUI_MEM_CUSTOM_FREE    free         /*Wrapper to free*/
    #endif     /*EDGUI_MEM_CUSTOM*/

    /* Garbage Collector settings
    * Used if edgui is binded to higher level language and the memory is managed by that language */
    #define EDGUI_ENABLE_GC 0
    #if EDGUI_ENABLE_GC != 0
        #define EDGUI_GC_INCLUDE "gc.h"                           /*Include Garbage Collector related things*/
        #define EDGUI_MEM_CUSTOM_REALLOC   your_realloc           /*Wrapper to realloc*/
        #define EDGUI_MEM_CUSTOM_GET_SIZE  your_mem_get_size      /*Wrapper to edgui_mem_get_size*/
    #endif /* EDGUI_ENABLE_GC */

    /*=======================
    Input device settings
    *=======================*/

    /* Input device default settings.
    * Can be changed in the Input device driver (`edgui_indev_drv_t`)*/

    /* Input device read period in milliseconds */
    #define EDGUI_INDEV_DEF_READ_PERIOD          30

    /* Drag threshold in pixels */
    #define EDGUI_INDEV_DEF_DRAG_LIMIT           10

    /* Drag throw slow-down in [%]. Greater value -> faster slow-down */
    #define EDGUI_INDEV_DEF_DRAG_THROW           20

    /* Long press time in milliseconds.
    * Time to send `EDGUI_EVENT_LONG_PRESSSED`) */
    #define EDGUI_INDEV_DEF_LONG_PRESS_TIME      400

    /* Repeated trigger period in long press [ms]
    * Time between `EDGUI_EVENT_LONG_PRESSED_REPEAT */
    #define EDGUI_INDEV_DEF_LONG_PRESS_REP_TIME  100

    /*==================
    * Feature usage
    *==================*/

    /*1: Enable the Animations */
    #define EDGUI_USE_ANIMATION        1
    #if EDGUI_USE_ANIMATION

        /*Declare the type of the user data of animations (can be e.g. `void *`, `int`, `struct`)*/
        typedef void *edgui_anim_user_data_t;

    #endif

    /* 1: Enable shadow drawing*/
    #define EDGUI_USE_SHADOW           1

    /* 1: Enable object groups (for keyboard/encoder navigation) */
    #define EDGUI_USE_GROUP            1
    #if EDGUI_USE_GROUP
        typedef void *edgui_group_user_data_t;
    #endif  /*EDGUI_USE_GROUP*/

    /* 1: Enable GPU interface*/
    #define EDGUI_USE_GPU              1

    /* 1: Enable file system (might be required for images */
    #define EDGUI_USE_FILESYSTEM       1
    #if EDGUI_USE_FILESYSTEM
        /*Declare the type of the user data of file system drivers (can be e.g. `void *`, `int`, `struct`)*/
        typedef void *edgui_fs_drv_user_data_t;
    #endif

    /*1: Add a `user_data` to drivers and objects*/
    #define EDGUI_USE_USER_DATA        0

    /*========================
    * Image decoder and cache
    *========================*/

    /* 1: Enable indexed (palette) images */
    #define EDGUI_IMG_CF_INDEXED       1

    /* 1: Enable alpha indexed images */
    #define EDGUI_IMG_CF_ALPHA         1

    /* Default image cache size. Image caching keeps the images opened.
    * If only the built-in image formats are used there is no real advantage of caching.
    * (I.e. no new image decoder is added)
    * With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
    * However the opened images might consume additional RAM.
    * EDGUI_IMG_CACHE_DEF_SIZE must be >= 1 */
    #define EDGUI_IMG_CACHE_DEF_SIZE       1

    /*Declare the type of the user data of image decoder (can be e.g. `void *`, `int`, `struct`)*/
    typedef void *edgui_img_decoder_user_data_t;

    /*=====================
    *  Compiler settings
    *====================*/
    /* Define a custom attribute to `edgui_tick_inc` function */
    #define EDGUI_ATTRIBUTE_TICK_INC

    /* Define a custom attribute to `edgui_task_handler` function */
    #define EDGUI_ATTRIBUTE_TASK_HANDLER

    /* With size optimization (-Os) the compiler might not align data to
    * 4 or 8 byte boundary. This alignment will be explicitly applied where needed.
    * E.g. __attribute__((aligned(4))) */
    #define EDGUI_ATTRIBUTE_MEM_ALIGN

    /* Attribute to mark large constant arrays for example
    * font's bitmaps */
    #define EDGUI_ATTRIBUTE_LARGE_CONST

    /*===================
    *  HAL settings
    *==================*/

    /* 1: use a custom tick source.
    * It removes the need to manually update the tick with `edgui_tick_inc`) */
    #define EDGUI_TICK_CUSTOM     0
    #if EDGUI_TICK_CUSTOM == 1
        #define EDGUI_TICK_CUSTOM_INCLUDE  "something.h"       /*Header for the sys time function*/
        #define EDGUI_TICK_CUSTOM_SYS_TIME_EXPR (millis())     /*Expression evaluating to current systime in ms*/
    #endif   /*EDGUI_TICK_CUSTOM*/

    typedef void *edgui_disp_drv_user_data_t;              /*Type of user data in the display driver*/
    typedef void *edgui_indev_drv_user_data_t;             /*Type of user data in the input device driver*/

    /*================
    * Log settings
    *===============*/

    /*1: Enable the log module*/
    #define EDGUI_USE_LOG      0
    #if EDGUI_USE_LOG
        /* How important log should be added:
        * EDGUI_LOG_LEVEL_TRACE       A lot of logs to give detailed information
        * EDGUI_LOG_LEVEL_INFO        Log important events
        * EDGUI_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
        * EDGUI_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
        * EDGUI_LOG_LEVEL_NONE        Do not log anything
        */
        #define EDGUI_LOG_LEVEL    EDGUI_LOG_LEVEL_WARN

        /* 1: Print the log with 'printf';
        * 0: user need to register a callback with `edgui_log_register_print_cb`*/
        #define EDGUI_LOG_PRINTF   0
    #endif  /*EDGUI_USE_LOG*/

    /*================
    *  THEME USAGE
    *================*/
    #define EDGUI_THEME_LIVE_UPDATE    1   /*1: Allow theme switching at run time. Uses 8..10 kB of RAM*/

    #define EDGUI_USE_THEME_TEMPL      0   /*Just for test*/
    #define EDGUI_USE_THEME_DEFAULT    1   /*Built mainly from the built-in styles. Consumes very few RAM*/
    #define EDGUI_USE_THEME_ALIEN      1   /*Dark futuristic theme*/
    #define EDGUI_USE_THEME_NIGHT      1   /*Dark elegant theme*/
    #define EDGUI_USE_THEME_MONO       1   /*Mono color theme for monochrome displays*/
    #define EDGUI_USE_THEME_MATERIAL   1   /*Flat theme with bold colors and light shadows*/
    #define EDGUI_USE_THEME_ZEN        1   /*Peaceful, mainly light theme */
    #define EDGUI_USE_THEME_NEMO       1   /*Water-like theme based on the movie "Finding Nemo"*/

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
    #define EDGUI_FONT_ROBOTO_12    0
    #define EDGUI_FONT_ROBOTO_16    1
    #define EDGUI_FONT_ROBOTO_22    0
    #define EDGUI_FONT_ROBOTO_28    0

    /*Pixel perfect monospace font
    * http://pelulamu.net/unscii/ */
    #define EDGUI_FONT_UNSCII_8     0

    /* Optionally declare your custom fonts here.
    * You can use these fonts as default font too
    * and they will be available globally. E.g.
    * #define EDGUI_FONT_CUSTOM_DECLARE EDGUI_FONT_DECLARE(my_font_1) \
    *                                EDGUI_FONT_DECLARE(my_font_2)
    */
    #define EDGUI_FONT_CUSTOM_DECLARE

    /*Always set a default font from the built-in fonts*/
    #define EDGUI_FONT_DEFAULT        &edgui_font_roboto_16

    /* Enable it if you have fonts with a lot of characters.
    * The limit depends on the font size, font face and bpp
    * but with > 10,000 characters if you see issues probably you need to enable it.*/
    #define EDGUI_FONT_FMT_TXT_LARGE   0

    /*Declare the type of the user data of fonts (can be e.g. `void *`, `int`, `struct`)*/
    typedef void *edgui_font_user_data_t;

    /*=================
    *  Text settings
    *=================*/

    /* Select a character encoding for strings.
    * Your IDE or editor should have the same character encoding
    * - EDGUI_TXT_ENC_UTF8
    * - EDGUI_TXT_ENC_ASCII
    * */
    #define EDGUI_TXT_ENC EDGUI_TXT_ENC_UTF8

    /*Can break (wrap) texts on these chars*/
    #define EDGUI_TXT_BREAK_CHARS                  " ,.;:-_"

    /*===================
    *  EDGUI_OBJ SETTINGS
    *==================*/

    /*Declare the type of the user data of object (can be e.g. `void *`, `int`, `struct`)*/
    typedef void *edgui_obj_user_data_t;

    /*1: enable `edgui_obj_realaign()` based on `edgui_obj_align()` parameters*/
    #define EDGUI_USE_OBJ_REALIGN          1

    /* Enable to make the object clickable on a larger area.
    * EDGUI_EXT_CLICK_AREA_OFF or 0: Disable this feature
    * EDGUI_EXT_CLICK_AREA_TINY: The extra area can be adjusted horizontally and vertically (0..255 px)
    * EDGUI_EXT_CLICK_AREA_FULL: The extra area can be adjusted in all 4 directions (-32k..+32k px)
    */
    #define EDGUI_USE_EXT_CLICK_AREA  EDGUI_EXT_CLICK_AREA_OFF

    /*==================
    *  LV OBJ X USAGE
    *================*/
    /*
    * Documentation of the object types: https://docs.edgui.com/#Object-types
    */

    /*Arc (dependencies: -)*/
    #define EDGUI_USE_ARC      1

    /*Bar (dependencies: -)*/
    #define EDGUI_USE_BAR      1

    /*Button (dependencies: edgui_cont*/
    #define EDGUI_USE_BTN      1
    #if EDGUI_USE_BTN != 0
        /*Enable button-state animations - draw a circle on click (dependencies: EDGUI_USE_ANIMATION)*/
        #define EDGUI_BTN_INK_EFFECT   0
    #endif

    /*Button matrix (dependencies: -)*/
    #define EDGUI_USE_BTNM     1

    /*Calendar (dependencies: -)*/
    #define EDGUI_USE_CALENDAR 1

    /*Canvas (dependencies: edgui_img)*/
    #define EDGUI_USE_CANVAS   1

    /*Check box (dependencies: edgui_btn, edgui_label)*/
    #define EDGUI_USE_CB       1

    /*Chart (dependencies: -)*/
    #define EDGUI_USE_CHART    1
    #if EDGUI_USE_CHART
        #define EDGUI_CHART_AXIS_TICK_LABEL_MAX_LEN    20
    #endif

    /*Container (dependencies: -*/
    #define EDGUI_USE_CONT     1

    /*Drop down list (dependencies: edgui_page, edgui_label, edgui_symbol_def.h)*/
    #define EDGUI_USE_DDLIST    1
    #if EDGUI_USE_DDLIST != 0
        /*Open and close default animation time [ms] (0: no animation)*/
        #define EDGUI_DDLIST_DEF_ANIM_TIME     200
    #endif

    /*Gauge (dependencies:edgui_bar, edgui_lmeter)*/
    #define EDGUI_USE_GAUGE    1

    /*Image (dependencies: edgui_label*/
    #define EDGUI_USE_IMG      1

    /*Image Button (dependencies: edgui_btn*/
    #define EDGUI_USE_IMGBTN   1
    #if EDGUI_USE_IMGBTN
        /*1: The imgbtn requires left, mid and right parts and the width can be set freely*/
        #define EDGUI_IMGBTN_TILED 0
    #endif

    /*Keyboard (dependencies: edgui_btnm)*/
    #define EDGUI_USE_KB       1

    /*Label (dependencies: -*/
    #define EDGUI_USE_LABEL    1
    #if EDGUI_USE_LABEL != 0
        /*Hor, or ver. scroll speed [px/sec] in 'EDGUI_LABEL_LONG_ROLL/ROLL_CIRC' mode*/
        #define EDGUI_LABEL_DEF_SCROLL_SPEED       25

        /* Waiting period at beginning/end of animation cycle */
        #define EDGUI_LABEL_WAIT_CHAR_COUNT        3

        /*Enable selecting text of the label */
        #define EDGUI_LABEL_TEXT_SEL               0

        /*Store extra some info in labels (12 bytes) to speed up drawing of very long texts*/
        #define EDGUI_LABEL_LONG_TXT_HINT          0
    #endif

    /*LED (dependencies: -)*/
    #define EDGUI_USE_LED      1

    /*Line (dependencies: -*/
    #define EDGUI_USE_LINE     1

    /*List (dependencies: edgui_page, edgui_btn, edgui_label, (edgui_img optionally for icons ))*/
    #define EDGUI_USE_LIST     1
    #if EDGUI_USE_LIST != 0
        /*Default animation time of focusing to a list element [ms] (0: no animation)  */
        #define EDGUI_LIST_DEF_ANIM_TIME  100
    #endif

    /*Line meter (dependencies: *;)*/
    #define EDGUI_USE_LMETER   1

    /*Message box (dependencies: edgui_rect, edgui_btnm, edgui_label)*/
    #define EDGUI_USE_MBOX     1

    /*Page (dependencies: edgui_cont)*/
    #define EDGUI_USE_PAGE     1
    #if EDGUI_USE_PAGE != 0
        /*Focus default animation time [ms] (0: no animation)*/
        #define EDGUI_PAGE_DEF_ANIM_TIME     400
    #endif

    /*Preload (dependencies: edgui_arc, edgui_anim)*/
    #define EDGUI_USE_PRELOAD      1
    #if EDGUI_USE_PRELOAD != 0
        #define EDGUI_PRELOAD_DEF_ARC_LENGTH   60      /*[deg]*/
        #define EDGUI_PRELOAD_DEF_SPIN_TIME    1000    /*[ms]*/
        #define EDGUI_PRELOAD_DEF_ANIM         EDGUI_PRELOAD_TYPE_SPINNING_ARC
    #endif

    /*Roller (dependencies: edgui_ddlist)*/
    #define EDGUI_USE_ROLLER    1
    #if EDGUI_USE_ROLLER != 0
        /*Focus animation time [ms] (0: no animation)*/
        #define EDGUI_ROLLER_DEF_ANIM_TIME     200

        /*Number of extra "pages" when the roller is infinite*/
        #define EDGUI_ROLLER_INF_PAGES         7
    #endif

    /*Slider (dependencies: edgui_bar)*/
    #define EDGUI_USE_SLIDER    1

    /*Spinbox (dependencies: edgui_ta)*/
    #define EDGUI_USE_SPINBOX       1

    /*Switch (dependencies: edgui_slider)*/
    #define EDGUI_USE_SW       1

    /*Text area (dependencies: edgui_label, edgui_page)*/
    #define EDGUI_USE_TA       1
    #if EDGUI_USE_TA != 0
        #define EDGUI_TA_DEF_CURSOR_BLINK_TIME 400     /*ms*/
        #define EDGUI_TA_DEF_PWD_SHOW_TIME     1500    /*ms*/
    #endif

    /*Table (dependencies: edgui_label)*/
    #define EDGUI_USE_TABLE    1
    #if EDGUI_USE_TABLE
        #define EDGUI_TABLE_COL_MAX    12
    #endif

    /*Tab (dependencies: edgui_page, edgui_btnm)*/
    #define EDGUI_USE_TABVIEW      1
    #if EDGUI_USE_TABVIEW != 0
        /*Time of slide animation [ms] (0: no animation)*/
        #define EDGUI_TABVIEW_DEF_ANIM_TIME    50
    #endif

    /*Tileview (dependencies: edgui_page) */
    #define EDGUI_USE_TILEVIEW     1
    #if EDGUI_USE_TILEVIEW
        /*Time of slide animation [ms] (0: no animation)*/
        #define EDGUI_TILEVIEW_DEF_ANIM_TIME   300
    #endif

    /*Window (dependencies: edgui_cont, edgui_btn, edgui_label, edgui_img, edgui_page)*/
    #define EDGUI_USE_WIN      1

    /*==================
    * Non-user section
    *==================*/

    #if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /* Disable warnings for Visual Studio*/
        #define _CRT_SECURE_NO_WARNINGS
    #endif

    /*--END OF EDGUI_CONF_H--*/

    /*Be sure every define has a default value*/
    #include "edgui_conf_checker.h"

#endif /*EDGUI_CONF_H*/
                                                                                                                             