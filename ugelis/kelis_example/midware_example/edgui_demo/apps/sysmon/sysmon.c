/**
 * @file edgui_sysmon.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "sysmon.h"
#if EDGUI_USE_SYSMON

#include <stdio.h>


/*********************
 *      DEFINES
 *********************/
#define CPU_LABEL_COLOR     "FF0000"
#define MEM_LABEL_COLOR     "0000FF"
#define CHART_POINT_NUM     100
#define REFR_TIME    500

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void sysmon_task(edgui_task_t * param);
static void win_close_action(edgui_obj_t * btn, edgui_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_obj_t * win;
static edgui_obj_t * chart;
static edgui_chart_series_t * cpu_ser;
static edgui_chart_series_t * mem_ser;
static edgui_obj_t * info_label;
static edgui_task_t * refr_task;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the system monitor
 */
void sysmon_create(void)
{
    refr_task = edgui_task_create(sysmon_task, REFR_TIME, EDGUI_TASK_PRIO_LOW, NULL);


    edgui_coord_t hres = edgui_disp_get_hor_res(NULL);
    edgui_coord_t vres = edgui_disp_get_ver_res(NULL);

    win = edgui_win_create(edgui_disp_get_scr_act(NULL), NULL);
    edgui_obj_t * win_btn = edgui_win_add_btn(win, EDGUI_SYMBOL_CLOSE);
    edgui_obj_set_event_cb(win_btn, win_close_action);

    /*Make the window content responsive*/
    edgui_win_set_layout(win, EDGUI_LAYOUT_PRETTY);

    /*Create a chart with two data lines*/
    chart = edgui_chart_create(win, NULL);
    edgui_obj_set_size(chart, hres / 2, vres / 2);
    edgui_obj_set_pos(chart, EDGUI_DPI / 10, EDGUI_DPI / 10);
    edgui_chart_set_point_count(chart, CHART_POINT_NUM);
    edgui_chart_set_range(chart, 0, 100);
    edgui_chart_set_type(chart, EDGUI_CHART_TYPE_LINE);
    edgui_chart_set_series_width(chart, 4);
    cpu_ser =  edgui_chart_add_series(chart, EDGUI_COLOR_RED);
    mem_ser =  edgui_chart_add_series(chart, EDGUI_COLOR_BLUE);

    /*Set the data series to zero*/
    uint16_t i;
    for(i = 0; i < CHART_POINT_NUM; i++) {
        edgui_chart_set_next(chart, cpu_ser, 0);
        edgui_chart_set_next(chart, mem_ser, 0);
    }

    /*Create a label for the details of Memory and CPU usage*/
    info_label = edgui_label_create(win, NULL);
    edgui_label_set_recolor(info_label, true);
    edgui_obj_align(info_label, chart, EDGUI_ALIGN_OUT_RIGHT_TOP, EDGUI_DPI / 4, 0);

    /*Refresh the chart and label manually at first*/
    sysmon_task(NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#define SYSMON_STRING_BUFFER_SIZE 256
/**
 * Called periodically to monitor the CPU and memory usage.
 * @param param unused
 */
static void sysmon_task(edgui_task_t * param)
{

    (void) param;    /*Unused*/

    EDGUI_LOG_TRACE("sys_mon task started");

    /*Get CPU and memory information */
    uint8_t cpu_busy;
    cpu_busy = 100 - edgui_task_get_idle();

    uint8_t mem_used_pct = 0;
#if  EDGUI_MEM_CUSTOM == 0
    edgui_mem_monitor_t mem_mon;
    edgui_mem_monitor(&mem_mon);
    mem_used_pct = mem_mon.used_pct;
#endif

    /*Add the CPU and memory data to the chart*/
    edgui_chart_set_next(chart, cpu_ser, cpu_busy);
    edgui_chart_set_next(chart, mem_ser, mem_used_pct);

    /*Refresh the and windows*/
    char buf_long[SYSMON_STRING_BUFFER_SIZE];
    int len = 0;
    len += snprintf(buf_long+len, SYSMON_STRING_BUFFER_SIZE-len, "%s%s CPU: %d %%%s\n\n",
            EDGUI_TXT_COLOR_CMD,
            CPU_LABEL_COLOR,
            cpu_busy,
            EDGUI_TXT_COLOR_CMD);

#if EDGUI_MEM_CUSTOM == 0
    len += snprintf(buf_long+len, SYSMON_STRING_BUFFER_SIZE-len, EDGUI_TXT_COLOR_CMD"%s MEMORY: %d %%"EDGUI_TXT_COLOR_CMD"\n"
            "Total: %d bytes\n"
            "Used: %d bytes\n"
            "Free: %d bytes\n"
            "Frag: %d %%",
            buf_long,
            MEM_LABEL_COLOR,
            mem_used_pct,
            (int)mem_mon.total_size,
            (int)mem_mon.total_size - mem_mon.free_size, mem_mon.free_size, mem_mon.frag_pct);

#else
    len += snprintf(buf_long+len, SYSMON_STRING_BUFFER_SIZE-len, EDGUI_TXT_COLOR_CMD"%s MEMORY: N/A"EDGUI_TXT_COLOR_CMD,
            MEM_LABEL_COLOR);
#endif
    edgui_label_set_text(info_label, buf_long);


    EDGUI_LOG_TRACE("sys_mon task finished");
}

/**
 * Called when the window's close button is clicked
 * @param btn pointer to the close button
 * @param event the current event
 */
static void win_close_action(edgui_obj_t * btn, edgui_event_t event)
{
    (void) btn;    /*Unused*/

    if(event != EDGUI_EVENT_CLICKED) return;

    edgui_obj_del(win);
    win = NULL;

    edgui_task_del(refr_task);
    refr_task = NULL;
}

#endif /*EDGUI_USE_SYMON*/
