/**
 * @file edgui_test_task.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_task.h"
#include <stdio.h>

#if EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * name;
    edgui_task_prio_t prio;
    uint32_t period;
    uint32_t delay;
} edgui_test_task_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void delay_task(edgui_task_t * task);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*
 * TEST ENVIRONMENT (only for edgui_test_task)
 * - Don't initialize edgui (don't call l'v_init()')
 * - Initialize 'edgui_mem' and 'edgui_task':
 *       edgui_mem_init();
 *       edgui_task_init();
 * - Set up the tick interface: e.g. 1 ms Timer with 'edgui_tick_inc(1);'
 * - Call a test: edgui_test_task_1/2/3();
 */


/**
 * Test the scheduling with various periods and priorities.
 */
void edgui_test_task_1(void)
{

    static edgui_test_task_dsc_t dsc[] = {
        {.name = "highest 2", .prio = EDGUI_TASK_PRIO_HIGHEST, .period = 5000, .delay = 400},
        {.name = "highest 1", .prio = EDGUI_TASK_PRIO_HIGHEST, .period = 4000, .delay = 300},
        {.name = "high 1", .prio = EDGUI_TASK_PRIO_HIGH, .period = 3000, .delay = 100},
        {.name = "mid 4", .prio = EDGUI_TASK_PRIO_MID, .period = 500, .delay = 1000},
        {.name = "mid 3", .prio = EDGUI_TASK_PRIO_MID, .period = 500, .delay = 100},
        {.name = "mid 2", .prio = EDGUI_TASK_PRIO_MID, .period = 500, .delay = 3000},
        {.name = "mid 1", .prio = EDGUI_TASK_PRIO_MID, .period = 500, .delay = 100},
        {.name = ""}
    };

    uint8_t i;
    for(i = 0; dsc[i].name[0] != '\0'; i++) {
        edgui_task_create(delay_task, dsc[i].period, dsc[i].prio, &dsc[i]);
    }

}

/**
 * Create a lot of short task and see their order. They should be executed according to their priority
 */
void edgui_test_task_2(void)
{

    static edgui_test_task_dsc_t dsc[] = {

        {.name = "low 1", .prio = EDGUI_TASK_PRIO_LOW, .period = 5000, .delay = 5},
        {.name = "mid 1", .prio = EDGUI_TASK_PRIO_MID, .period = 5000, .delay = 5},
        {.name = "highest 1", .prio = EDGUI_TASK_PRIO_HIGHEST, .period = 5000, .delay = 5},
        {.name = "highest 2", .prio = EDGUI_TASK_PRIO_HIGHEST, .period = 5000, .delay = 5},
        {.name = "mid 2", .prio = EDGUI_TASK_PRIO_MID, .period = 5000, .delay = 5},
        {.name = "high 1", .prio = EDGUI_TASK_PRIO_HIGH, .period = 5000, .delay = 5},
        {.name = "mid 3", .prio = EDGUI_TASK_PRIO_MID, .period = 5000, .delay = 5},
        {.name = "high 2", .prio = EDGUI_TASK_PRIO_HIGH, .period = 5000, .delay = 5},
        {.name = "high 3", .prio = EDGUI_TASK_PRIO_HIGH, .period = 5000, .delay = 5},
        {.name = "mid 4", .prio = EDGUI_TASK_PRIO_MID, .period = 5000, .delay = 5},
        {.name = "high 4", .prio = EDGUI_TASK_PRIO_HIGH, .period = 5000, .delay = 5},
        {.name = "lowest 1", .prio = EDGUI_TASK_PRIO_LOWEST, .period = 5000, .delay = 5},
        {.name = "low 2", .prio = EDGUI_TASK_PRIO_LOW, .period = 5000, .delay = 5},
        {.name = "lowest 2", .prio = EDGUI_TASK_PRIO_LOWEST, .period = 5000, .delay = 5},
        {.name = "low 3", .prio = EDGUI_TASK_PRIO_LOW, .period = 5000, .delay = 5},
        {.name = ""}
    };

    uint8_t i;
    for(i = 0; dsc[i].name[0] != '\0'; i++) {
        edgui_task_create(delay_task, 1000, dsc[i].prio, &dsc[i]);
    }

}


/**
 * Change the priority later
 */
void edgui_test_task_3(void)
{
    static edgui_test_task_dsc_t dsc[] = {
        {.name = "highest 1", .prio = EDGUI_TASK_PRIO_HIGHEST, .period = 5000, .delay = 10},
        {.name = "highest 2", .prio = EDGUI_TASK_PRIO_HIGHEST, .period = 5000, .delay = 10},
        {.name = "high 1", .prio = EDGUI_TASK_PRIO_HIGH, .period = 5000, .delay = 10},
        {.name = "mid 1", .prio = EDGUI_TASK_PRIO_MID, .period = 5000, .delay = 10},
        {.name = "mid 2", .prio = EDGUI_TASK_PRIO_MID, .period = 5000, .delay = 10},
        {.name = "should be high 2", .prio = EDGUI_TASK_PRIO_MID, .period = 5000, .delay = 10},
        {.name = "mid 3", .prio = EDGUI_TASK_PRIO_MID, .period = 5000, .delay = 10},
        {.name = "mid 4", .prio = EDGUI_TASK_PRIO_MID, .period = 5000, .delay = 10},
        {.name = "low 1", .prio = EDGUI_TASK_PRIO_LOW, .period = 5000, .delay = 10},
        {.name = "lowest 1", .prio = EDGUI_TASK_PRIO_LOWEST, .period = 5000, .delay = 10},
        {.name = ""}
    };

    uint8_t i;
    edgui_task_t * mod_prio = NULL;
    for(i = 0; dsc[i].name[0] != '\0'; i++) {
        edgui_task_t * tmp = edgui_task_create(delay_task, dsc[i].period, dsc[i].prio, &dsc[i]);
        if(strcmp(dsc[i].name, "should be high 2") == 0) {
            mod_prio = tmp;
        }
    }

    edgui_task_set_prio(mod_prio, EDGUI_TASK_PRIO_HIGH);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void delay_task(edgui_task_t * task)
{
    edgui_test_task_dsc_t * dsc = task->user_data;
#if EDGUI_EX_PRINTF
    printf("%s: %d\n", dsc->name, dsc->delay);
#endif
    uint32_t act = edgui_tick_get();
    while(edgui_tick_elaps(act) < dsc->delay);

}

#endif
