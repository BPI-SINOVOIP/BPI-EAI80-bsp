#ifndef EDGUI_SYMBOL_DEF_H
#define EDGUI_SYMBOL_DEF_H
/* clang-format off */

#ifdef __cplusplus
extern "C" {
#endif
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui_conf.h"
#else
#include "../edgui/edgui_conf.h"
#endif


#define EDGUI_SYMBOL_AUDIO           "\xef\x80\x81"
#define EDGUI_SYMBOL_VIDEO           "\xef\x80\x88"
#define EDGUI_SYMBOL_LIST            "\xef\x80\x8b"
#define EDGUI_SYMBOL_OK              "\xef\x80\x8c"
#define EDGUI_SYMBOL_CLOSE           "\xef\x80\x8d"
#define EDGUI_SYMBOL_POWER           "\xef\x80\x91"
#define EDGUI_SYMBOL_SETTINGS        "\xef\x80\x93"
#define EDGUI_SYMBOL_TRASH           "\xef\x80\x94"
#define EDGUI_SYMBOL_HOME            "\xef\x80\x95"
#define EDGUI_SYMBOL_DOWNLOAD        "\xef\x80\x99"
#define EDGUI_SYMBOL_DRIVE           "\xef\x80\x9c"
#define EDGUI_SYMBOL_REFRESH         "\xef\x80\xa1"
#define EDGUI_SYMBOL_MUTE            "\xef\x80\xa6"
#define EDGUI_SYMBOL_VOLUME_MID      "\xef\x80\xa7"
#define EDGUI_SYMBOL_VOLUME_MAX      "\xef\x80\xa8"
#define EDGUI_SYMBOL_IMAGE           "\xef\x80\xbe"
#define EDGUI_SYMBOL_EDIT            "\xef\x81\x80"
#define EDGUI_SYMBOL_PREV            "\xef\x81\x88"
#define EDGUI_SYMBOL_PLAY            "\xef\x81\x8b"
#define EDGUI_SYMBOL_PAUSE           "\xef\x81\x8c"
#define EDGUI_SYMBOL_STOP            "\xef\x81\x8d"
#define EDGUI_SYMBOL_NEXT            "\xef\x81\x91"
#define EDGUI_SYMBOL_EJECT           "\xef\x81\x92"
#define EDGUI_SYMBOL_LEFT            "\xef\x81\x93"
#define EDGUI_SYMBOL_RIGHT           "\xef\x81\x94"
#define EDGUI_SYMBOL_PLUS            "\xef\x81\xa7"
#define EDGUI_SYMBOL_MINUS           "\xef\x81\xa8"
#define EDGUI_SYMBOL_WARNING         "\xef\x81\xb1"
#define EDGUI_SYMBOL_SHUFFLE         "\xef\x81\xb4"
#define EDGUI_SYMBOL_UP              "\xef\x81\xb7"
#define EDGUI_SYMBOL_DOWN            "\xef\x81\xb8"
#define EDGUI_SYMBOL_LOOP            "\xef\x81\xb9"
#define EDGUI_SYMBOL_DIRECTORY       "\xef\x81\xbb"
#define EDGUI_SYMBOL_UPLOAD          "\xef\x82\x93"
#define EDGUI_SYMBOL_CALL            "\xef\x82\x95"
#define EDGUI_SYMBOL_CUT             "\xef\x83\x84"
#define EDGUI_SYMBOL_COPY            "\xef\x83\x85"
#define EDGUI_SYMBOL_SAVE            "\xef\x83\x87"
#define EDGUI_SYMBOL_CHARGE          "\xef\x83\xa7"
#define EDGUI_SYMBOL_BELL            "\xef\x83\xb3"
#define EDGUI_SYMBOL_KEYBOARD        "\xef\x84\x9c"
#define EDGUI_SYMBOL_GPS             "\xef\x84\xa4"
#define EDGUI_SYMBOL_FILE            "\xef\x85\x9b"
#define EDGUI_SYMBOL_WIFI            "\xef\x87\xab"
#define EDGUI_SYMBOL_BATTERY_FULL    "\xef\x89\x80"
#define EDGUI_SYMBOL_BATTERY_3       "\xef\x89\x81"
#define EDGUI_SYMBOL_BATTERY_2       "\xef\x89\x82"
#define EDGUI_SYMBOL_BATTERY_1       "\xef\x89\x83"
#define EDGUI_SYMBOL_BATTERY_EMPTY   "\xef\x89\x84"
#define EDGUI_SYMBOL_BLUETOOTH       "\xef\x8a\x93"

/** Invalid symbol at (U+F8FF). If written before a string then `edgui_img` will show it as a label*/
#define EDGUI_SYMBOL_DUMMY           "\xEF\xA3\xBF"

/*
 * The following list is generated using
 * cat src/edgui_misc/edgui_symbol_def.h | sed -E -n 's/^#define\s+(EDGUI_SYMBOL_\w+).*"$/    _EDGUI_STR_\1,/p'
 */
enum {
    _EDGUI_STR_SYMBOL_AUDIO,
    _EDGUI_STR_SYMBOL_VIDEO,
    _EDGUI_STR_SYMBOL_LIST,
    _EDGUI_STR_SYMBOL_OK,
    _EDGUI_STR_SYMBOL_CLOSE,
    _EDGUI_STR_SYMBOL_POWER,
    _EDGUI_STR_SYMBOL_SETTINGS,
    _EDGUI_STR_SYMBOL_TRASH,
    _EDGUI_STR_SYMBOL_HOME,
    _EDGUI_STR_SYMBOL_DOWNLOAD,
    _EDGUI_STR_SYMBOL_DRIVE,
    _EDGUI_STR_SYMBOL_REFRESH,
    _EDGUI_STR_SYMBOL_MUTE,
    _EDGUI_STR_SYMBOL_VOLUME_MID,
    _EDGUI_STR_SYMBOL_VOLUME_MAX,
    _EDGUI_STR_SYMBOL_IMAGE,
    _EDGUI_STR_SYMBOL_EDIT,
    _EDGUI_STR_SYMBOL_PREV,
    _EDGUI_STR_SYMBOL_PLAY,
    _EDGUI_STR_SYMBOL_PAUSE,
    _EDGUI_STR_SYMBOL_STOP,
    _EDGUI_STR_SYMBOL_NEXT,
    _EDGUI_STR_SYMBOL_EJECT,
    _EDGUI_STR_SYMBOL_LEFT,
    _EDGUI_STR_SYMBOL_RIGHT,
    _EDGUI_STR_SYMBOL_PLUS,
    _EDGUI_STR_SYMBOL_MINUS,
    _EDGUI_STR_SYMBOL_WARNING,
    _EDGUI_STR_SYMBOL_SHUFFLE,
    _EDGUI_STR_SYMBOL_UP,
    _EDGUI_STR_SYMBOL_DOWN,
    _EDGUI_STR_SYMBOL_LOOP,
    _EDGUI_STR_SYMBOL_DIRECTORY,
    _EDGUI_STR_SYMBOL_UPLOAD,
    _EDGUI_STR_SYMBOL_CALL,
    _EDGUI_STR_SYMBOL_CUT,
    _EDGUI_STR_SYMBOL_COPY,
    _EDGUI_STR_SYMBOL_SAVE,
    _EDGUI_STR_SYMBOL_CHARGE,
    _EDGUI_STR_SYMBOL_BELL,
    _EDGUI_STR_SYMBOL_KEYBOARD,
    _EDGUI_STR_SYMBOL_GPS,
    _EDGUI_STR_SYMBOL_FILE,
    _EDGUI_STR_SYMBOL_WIFI,
    _EDGUI_STR_SYMBOL_BATTERY_FULL,
    _EDGUI_STR_SYMBOL_BATTERY_3,
    _EDGUI_STR_SYMBOL_BATTERY_2,
    _EDGUI_STR_SYMBOL_BATTERY_1,
    _EDGUI_STR_SYMBOL_BATTERY_EMPTY,
    _EDGUI_STR_SYMBOL_BLUETOOTH,
    _EDGUI_STR_SYMBOL_DUMMY,
};

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /*EDGUI_SYMBOL_DEF_H*/





