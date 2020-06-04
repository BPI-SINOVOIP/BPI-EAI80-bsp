/**
 * @file edgui_test.h
 *
 */

#ifndef EDGUI_TEST_H
#define EDGUI_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui.h"
#include "edgui_conf.h"
#else
#include <edgui/edgui.h>
#include <edgui/edgui_conf.h>
#endif
	
#include "edgui_prj_conf.h"


#if EDGUI_USE_TESTS

#include "../edgui_examples.h"

#include "edgui_test_obj/edgui_test_obj.h"

#include "edgui_test_objx/edgui_test_arc/edgui_test_arc.h"
#include "edgui_test_objx/edgui_test_bar/edgui_test_bar.h"
#include "edgui_test_objx/edgui_test_btn/edgui_test_btn.h"
#include "edgui_test_objx/edgui_test_btnm/edgui_test_btnm.h"
#include "edgui_test_objx/edgui_test_cb/edgui_test_cb.h"
#include "edgui_test_objx/edgui_test_canvas/edgui_test_canvas.h"
#include "edgui_test_objx/edgui_test_chart/edgui_test_chart.h"
#include "edgui_test_objx/edgui_test_cont/edgui_test_cont.h"
#include "edgui_test_objx/edgui_test_ddlist/edgui_test_ddlist.h"
#include "edgui_test_objx/edgui_test_gauge/edgui_test_gauge.h"
#include "edgui_test_objx/edgui_test_img/edgui_test_img.h"
#include "edgui_test_objx/edgui_test_imgbtn/edgui_test_imgbtn.h"
#include "edgui_test_objx/edgui_test_kb/edgui_test_kb.h"
#include "edgui_test_objx/edgui_test_label/edgui_test_label.h"
#include "edgui_test_objx/edgui_test_led/edgui_test_led.h"
#include "edgui_test_objx/edgui_test_line/edgui_test_line.h"
#include "edgui_test_objx/edgui_test_list/edgui_test_list.h"
#include "edgui_test_objx/edgui_test_lmeter/edgui_test_lmeter.h"
#include "edgui_test_objx/edgui_test_mbox/edgui_test_mbox.h"
#include "edgui_test_objx/edgui_test_page/edgui_test_page.h"
#include "edgui_test_objx/edgui_test_preload/edgui_test_preload.h"
#include "edgui_test_objx/edgui_test_roller/edgui_test_roller.h"
#include "edgui_test_objx/edgui_test_slider/edgui_test_slider.h"
#include "edgui_test_objx/edgui_test_sw/edgui_test_sw.h"
#include "edgui_test_objx/edgui_test_ta/edgui_test_ta.h"
#include "edgui_test_objx/edgui_test_table/edgui_test_table.h"
#include "edgui_test_objx/edgui_test_tabview/edgui_test_tabview.h"
#include "edgui_test_objx/edgui_test_tileview/edgui_test_tileview.h"
#include "edgui_test_objx/edgui_test_win/edgui_test_win.h"

#include "edgui_test_theme/edgui_test_theme_1.h"
#include "edgui_test_theme/edgui_test_theme_2.h"

#include "edgui_test_group/edgui_test_group.h"

#include "edgui_test_stress/edgui_test_stress.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* EDGUI_USE_TESTS */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*EDGUI_TEST_H*/
