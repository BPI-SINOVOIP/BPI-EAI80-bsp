/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          picture_raw.h
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Including raw picture file.
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

#ifndef __PICTURE_RAW_H__
#define __PICTURE_RAW_H__

#ifdef CONFIG_FT5426
    #include "rawimage_pass.h"
    #include "rawimage_cap.h"
    #include "rawimage_slide.h"
    #include "rawimage_body_on.h"
    #include "rawimage_body_off.h"
    #include "rawimage_ges_on.h"
    #include "rawimage_ges_off.h"
    #include "rawimage_face_on.h"
    #include "rawimage_face_off.h"
    #include "rawimage_speech_off.h"
    #include "rawimage_speech_on.h"
    #include "rawimage_record_off.h"
    #include "rawimage_record_on.h"
    #include "rawimage_delete_off.h"
    #include "rawimage_delete_on.h"
    #include "rawimage_record_hide.h"
#else
    #include "mini\rawimage_pass.h"
    #include "mini\rawimage_cap.h"
    #include "mini\rawimage_slide.h"
    #include "mini\rawimage_body_on.h"
    #include "mini\rawimage_body_off.h"
    #include "mini\rawimage_ges_on.h"
    #include "mini\rawimage_ges_off.h"
    #include "mini\rawimage_face_on.h"
    #include "mini\rawimage_face_off.h"
    #include "mini\rawimage_speech_off.h"
    #include "mini\rawimage_speech_on.h"
    #include "mini\rawimage_record_off.h"
    #include "mini\rawimage_record_on.h"
    #include "mini\rawimage_delete_off.h"
    #include "mini\rawimage_delete_on.h"
    #include "mini/rawimage_hide.h"
#endif

#include "ui/ugui/ugui.h"

ugui_icon_info_t icons[] =
{
#ifdef CONFIG_FT5426
    {gImage_rawimage_slide, 0, 0, 0, 0, 160, 480},
    {gImage_rawimage_face_off, gImage_rawimage_face_on,   0, 0, 80, 160, 80},
    {gImage_rawimage_body_off, gImage_rawimage_body_on,   0, 0, 160, 160, 80},
    {gImage_rawimage_ges_off,  gImage_rawimage_ges_on,    0, 0, 240, 160, 80},
    {gImage_rawimage_speech_off, gImage_rawimage_speech_on, 0, 0, 320, 160, 80},
    {gImage_rawimage_record_off, gImage_rawimage_record_on, gImage_rawimage_record_hide, 15, 415, 50, 50},
    {gImage_rawimage_delete_off, gImage_rawimage_delete_on, gImage_rawimage_record_hide, 90, 415, 50, 50},
#else
    {gImage_rawimage_slide, 0, 0, 0, 0, 118, 272},
    {gImage_rawimage_face_off, gImage_rawimage_face_on,   0, 0, 45, 118, 45},
    {gImage_rawimage_body_off, gImage_rawimage_body_on,   0, 0, 90, 118, 45},
    {gImage_rawimage_ges_off, gImage_rawimage_ges_on,    0, 0, 135, 118, 45},
    {gImage_rawimage_speech_off, gImage_rawimage_speech_on, 0, 0, 180, 118, 45},
    {gImage_rawimage_record_off, gImage_rawimage_record_on, gImage_rawimage_hide, 20, 235, 30, 30},
    {gImage_rawimage_delete_off, gImage_rawimage_delete_on, gImage_rawimage_hide, 70, 235, 30, 30},
#endif
};

#endif //__PICTURE_RAW_H__
