/**
 * @file edgui_test_stress.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "edgui_test_stress.h"

#if EDGUI_USE_TESTS && EDGUI_USE_ANIMATION

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void obj_mem_leak_tester(edgui_task_t *);
static void alloc_free_tester(edgui_task_t *);
static void mem_monitor(edgui_task_t *);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_obj_t * all_obj_h;
static edgui_obj_t * alloc_label;
static edgui_obj_t * alloc_ta;
static const char * mbox_btns[] = {"Ok", "Cancel", ""};
EDGUI_IMG_DECLARE(img_flower_icon)

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create and delete a lot of objects and animations.
 */
void edgui_test_stress_1(void)
{
    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    edgui_task_create(obj_mem_leak_tester, 200, EDGUI_TASK_PRIO_MID, NULL);
    edgui_task_create(mem_monitor, 500, EDGUI_TASK_PRIO_MID, NULL);
    edgui_task_create(alloc_free_tester, 100, EDGUI_TASK_PRIO_MID, NULL);

    /* Holder for all object types */
    all_obj_h = edgui_obj_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_set_size(all_obj_h, hres / 2, vres);
    edgui_obj_set_style(all_obj_h, &edgui_style_pretty);

    alloc_ta = edgui_ta_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(alloc_ta, all_obj_h, EDGUI_ALIGN_OUT_RIGHT_TOP, 10, 10);
    edgui_obj_set_height(alloc_ta, vres / 4);

    alloc_label = edgui_label_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_align(alloc_label, alloc_ta, EDGUI_ALIGN_OUT_BOTTOM_LEFT, 0, 20);


    /*Add style animation to the ta*/
    static edgui_style_t ta_style;
    edgui_style_copy(&ta_style, &edgui_style_pretty);
    edgui_ta_set_style(alloc_ta, EDGUI_TA_STYLE_BG, &ta_style);


    edgui_anim_t sa;
    edgui_style_anim_init(&sa);
    edgui_style_anim_set_styles(&sa, &ta_style, &edgui_style_pretty, &edgui_style_pretty_color);
    edgui_style_anim_set_time(&sa, 500, 500);
    edgui_style_anim_set_playback(&sa, 500);
    edgui_style_anim_set_repeat(&sa, 500);
    edgui_style_anim_create(&sa);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void mem_monitor(edgui_task_t * param)
{
    (void) param;    /*Unused*/

#if EDGUI_EX_PRINTF
    edgui_mem_monitor_t mon;
    edgui_mem_monitor(&mon);
    printf("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n", (int)mon.total_size - mon.free_size,
           mon.used_pct,
           mon.frag_pct,
           (int)mon.free_biggest_size);
#endif
}

static void obj_mem_leak_tester(edgui_task_t * param)
{
    (void) param;    /*Unused*/
    static const edgui_color_t needle_colors[1] = {EDGUI_COLOR_RED};  /*For gauge*/

    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    static int16_t state = 0;
    edgui_obj_t * obj;
    static edgui_obj_t * page;

    edgui_anim_t a;
    a.path_cb = edgui_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = 0;
    a.time = 500;
    a.playback = 0;
    a.repeat = 0;
    a.playback_pause = 100;
    a.repeat_pause = 100;

    switch(state) {
        case 0:
            obj = edgui_obj_create(all_obj_h, NULL);
            edgui_obj_set_pos(obj, 10, 5);
            a.playback = 1;
            a.repeat = 1;
            a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_x;
            a.var = obj;
            a.start = 10 ;
            a.end = 100 ;
            edgui_anim_create(&a);
            break;
        case 1:
            obj = edgui_btn_create(all_obj_h, NULL);
            edgui_obj_set_pos(obj, 60, 5);
            a.playback = 0;
            a.repeat = 1;
            a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_x;
            a.var = obj;
            a.start = 150 ;
            a.end = 200 ;
            edgui_anim_create(&a);
            obj = edgui_label_create(obj, NULL);
            edgui_label_set_text(obj, "Button");
            break;
        case 2:     /*Page tests container too*/
            page = edgui_page_create(all_obj_h, NULL);
            edgui_obj_set_pos(page, 10, 60);
            edgui_obj_set_size(page, edgui_obj_get_width(all_obj_h) - (20), 3 * vres / 4);
            edgui_page_set_scrl_layout(page, EDGUI_LAYOUT_PRETTY);
            break;
        case 3:
            obj = edgui_label_create(page, NULL);
            edgui_label_set_text(obj, "Label");
            break;
        case 4:
            obj = edgui_img_create(page, NULL);
            edgui_img_set_src(obj, &img_flower_icon);
            break;
        case 5:
            obj = edgui_cb_create(page, NULL);
            edgui_cb_set_text(obj, "Check box");
            break;
        case 7:             /*Switch tests bar and slider memory leak too*/
            obj = edgui_sw_create(page, NULL);
            edgui_sw_on(obj, EDGUI_ANIM_OFF);
            break;
        case 8:     /*Kb tests butm too*/
            obj = edgui_kb_create(all_obj_h, NULL);
            edgui_obj_set_size(obj, hres / 3, vres / 5);
            edgui_obj_set_pos(obj, 30, 90);
            a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_y;
            a.var = obj;
            a.start = EDGUI_VER_RES ;
            a.end = edgui_obj_get_y(obj);
            a.time = 200;
            edgui_anim_create(&a);
            break;

        case 9: /*Roller test ddlist too*/
            obj = edgui_roller_create(page, NULL);
            edgui_roller_set_options(obj, "One\nTwo\nThree", false);
            edgui_roller_set_anim_time(obj, 300);
            edgui_roller_set_selected(obj, 2, true);
            break;
        case 10: /*Gauge test lmeter too*/
            obj = edgui_gauge_create(page, NULL);
            edgui_gauge_set_needle_count(obj, 1, needle_colors);
            edgui_gauge_set_value(obj, 1, 30);
            break;
        case 15: /*Wait a little to see the previous results*/
            obj = edgui_list_create(all_obj_h, NULL);
            edgui_obj_set_pos(obj, 40, 50);
            edgui_list_add_btn(obj, EDGUI_SYMBOL_OK, "List 1");
            edgui_list_add_btn(obj, EDGUI_SYMBOL_OK, "List 2");
            edgui_list_add_btn(obj, EDGUI_SYMBOL_OK, "List 3");
            edgui_list_add_btn(obj, EDGUI_SYMBOL_OK, "List 4");
            edgui_list_add_btn(obj, EDGUI_SYMBOL_OK, "List 5");
            edgui_list_add_btn(obj, EDGUI_SYMBOL_OK, "List 6");
            a.exec_cb = (edgui_anim_exec_xcb_t)edgui_obj_set_height;
            a.var = obj;
            a.start = 0;
            a.end = edgui_obj_get_height(obj);
            a.time = 5000;
            edgui_anim_create(&a);
            break;
        case 16:
            obj = edgui_win_create(all_obj_h, NULL);
            edgui_win_add_btn(obj, EDGUI_SYMBOL_CLOSE);
            edgui_win_add_btn(obj, EDGUI_SYMBOL_OK);
            edgui_win_set_style(obj, EDGUI_WIN_STYLE_BG, &edgui_style_pretty);
            edgui_obj_set_size(obj, hres / 3, vres / 3);
            edgui_obj_set_pos(obj, 20, 100);
            break;
        case 17:
            obj = edgui_tabview_create(all_obj_h, NULL);
            edgui_tabview_add_tab(obj, "tab1");
            edgui_tabview_add_tab(obj, "tab2");
            edgui_tabview_add_tab(obj, "tab3");
            edgui_tabview_set_style(obj, EDGUI_TABVIEW_STYLE_BG, &edgui_style_pretty);
            edgui_obj_set_size(obj, hres / 3, vres / 3);
            edgui_obj_set_pos(obj, 50, 140);
            break;
        case 18:
            obj = edgui_mbox_create(all_obj_h, NULL);
            edgui_obj_set_width(obj, hres / 4);
            edgui_mbox_set_text(obj, "message");
            edgui_mbox_add_btns(obj, mbox_btns); /*Set 3 times to test btnm add memory leasks*/
            edgui_mbox_add_btns(obj, mbox_btns);
            edgui_mbox_add_btns(obj, mbox_btns);
            edgui_mbox_set_anim_time(obj, 300);
            edgui_mbox_start_auto_close(obj, 500);
            break;

        /*Delete object from the page*/
        case 20:
            obj = edgui_obj_get_child(edgui_page_get_scrl(page), NULL);
            if(obj) edgui_obj_del(obj);
            else  {
                state = 24;
            }
            break;
        case 21:
            obj = edgui_obj_get_child_back(edgui_page_get_scrl(page), NULL);       /*Delete from the end too to be more random*/
            if(obj) {
                edgui_obj_del(obj);
                state -= 2;     /*Go back to delete state*/
            } else {
                state = 24;
            }
            break;
        /*Remove object from 'all_obj_h'*/
        case 25:
            obj = edgui_obj_get_child(all_obj_h, NULL);
            if(obj) edgui_obj_del(obj);
            else state = 29;
            break;
        case 26:
            obj = edgui_obj_get_child_back(all_obj_h, NULL);       /*Delete from the end too to be more random*/
            if(obj) {
                edgui_obj_del(obj);
                state -= 2;     /*Go back to delete state*/
            } else state = 29;
            break;

        case 30:
            state = -1;
            break;
        default:
            break;
    }

    state ++;
}


/**
 * Test alloc and free by settings the text of a label and instering text to a text area
 */
static void alloc_free_tester(edgui_task_t * param)
{
    (void) param;    /*Unused*/

    static int16_t state = 0;

    switch(state) {
        case 0:
            edgui_label_set_text(alloc_label, "a");
            break;

        case 1:
            edgui_ta_set_text(alloc_ta, "Initilal");
            break;

        case 2:
            edgui_label_set_text(alloc_label, "long long long\nlong long long");
            break;

        case 3:
            edgui_label_set_text(alloc_label, "b");
            break;

        case 6:    /*Wait to be random*/
            edgui_ta_set_cursor_pos(alloc_ta, 0);
            edgui_ta_add_text(alloc_ta, "aaaaaaaaaaaaaaaaaaaaaaaaaaa!\n");
            break;

        case 7:
            edgui_label_set_text(alloc_label, "medium");
            break;

        case 8:
            edgui_label_set_text(alloc_label, "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n");
            break;

        case 10:       /*Wait to be random*/
            edgui_label_set_text(alloc_label, "some text");
            break;

        case 11:
            edgui_ta_set_cursor_pos(alloc_ta, 20);
            edgui_ta_del_char(alloc_ta);
            edgui_ta_del_char(alloc_ta);
            edgui_ta_del_char(alloc_ta);
            edgui_ta_del_char(alloc_ta);
            edgui_ta_del_char(alloc_ta);
            edgui_ta_del_char(alloc_ta);
            edgui_ta_del_char(alloc_ta);
            edgui_ta_del_char(alloc_ta);
            edgui_ta_del_char(alloc_ta);
            edgui_ta_del_char(alloc_ta);
            edgui_ta_del_char(alloc_ta);
            break;

        case 12:
            edgui_label_set_text(alloc_label, EDGUI_SYMBOL_DIRECTORY);
            break;

        case 16:     /*Wait to be random*/
            edgui_label_set_text(alloc_label, "A dummy sentence nothing else");
            break;

        case 17:
            edgui_ta_set_cursor_pos(alloc_ta, 5);
            edgui_ta_add_char(alloc_ta, 'A');
            edgui_ta_add_char(alloc_ta, 'A');
            edgui_ta_add_char(alloc_ta, 'A');
            edgui_ta_add_char(alloc_ta, 'A');
            edgui_ta_add_char(alloc_ta, 'A');
            break;

        case 18:
            edgui_label_set_text(alloc_label, "ok");
            break;

        case 20:   /*Wait to be random*/
            edgui_label_set_text(alloc_label, EDGUI_SYMBOL_FILE);
            break;
        case 21:
            edgui_label_set_text(alloc_label, "c");
            break;
        case 22:
            edgui_ta_set_cursor_pos(alloc_ta, 20);
            edgui_ta_add_text(alloc_ta, "Ú");
            edgui_ta_add_text(alloc_ta, "Ú");
            edgui_ta_add_text(alloc_ta, "Ú");
            edgui_ta_add_text(alloc_ta, "Ú");
            edgui_ta_add_text(alloc_ta, "Ú");
            break;

        case 23:
            edgui_label_set_text(alloc_label, "ÁaÁaaÁÁaaaÁÁÁaaaaÁÁÁÁ");
            break;

        case 25:
            edgui_ta_set_text(alloc_ta, "");
            break;

        case 26:
            edgui_label_set_text(alloc_label, "");
            break;

        case 28:
            state = -1 ;
            break;
        default:
            break;
    }

    state ++;
}

#endif /*EDGUI_USE_TESTS && EDGUI_USE_ANIMATION*/

