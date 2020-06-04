/**
 * @file edgui_refr.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "edgui_refr.h"
#include "edgui_disp.h"
#include "../edgui_hal/edgui_hal_tick.h"
#include "../edgui_hal/edgui_hal_disp.h"
#include "../edgui_misc/edgui_task.h"
#include "../edgui_misc/edgui_mem.h"
#include "../edgui_misc/edgui_gc.h"
#include "../edgui_draw/edgui_draw.h"

#if defined(EDGUI_GC_INCLUDE)
#include EDGUI_GC_INCLUDE
#endif /* EDGUI_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/
/* Draw translucent random colored areas on the invalidated (redrawn) areas*/
#define MASK_AREA_DEBUG 0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void edgui_refr_join_area(void);
static void edgui_refr_areas(void);
static void edgui_refr_area(const edgui_area_t * area_p);
static void edgui_refr_area_part(const edgui_area_t * area_p);
static edgui_obj_t * edgui_refr_get_top_obj(const edgui_area_t * area_p, edgui_obj_t * obj);
static void edgui_refr_obj_and_children(edgui_obj_t * top_p, const edgui_area_t * mask_p);
static void edgui_refr_obj(edgui_obj_t * obj, const edgui_area_t * mask_ori_p);
static void edgui_refr_vdb_flush(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t px_num;
static edgui_disp_t * disp_refr; /*Display being refreshed*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the screen refresh subsystem
 */
void edgui_refr_init(void)
{
    /*Nothing to do*/
}

/**
 * Redraw the invalidated areas now.
 * Normally the redrawing is periodically executed in `edgui_task_handler` but a long blocking process
 * can prevent the call of `edgui_task_handler`. In this case if the the GUI is updated in the process
 * (e.g. progress bar) this function can be called when the screen should be updated.
 * @param disp pointer to display to refresh. NULL to refresh all displays.
 */
void edgui_refr_now(edgui_disp_t * disp)
{
    if(disp) {
        edgui_disp_refr_task(disp->refr_task);
    } else {
        edgui_disp_t * d;
        d = edgui_disp_get_next(NULL);
        while(d) {
            edgui_disp_refr_task(d->refr_task);
            d = edgui_disp_get_next(d);
        }
    }
}

/**
 * Invalidate an area on display to redraw it
 * @param area_p pointer to area which should be invalidated (NULL: delete the invalidated areas)
 * @param disp pointer to display where the area should be invalidated (NULL can be used if there is
 * only one display)
 */
void edgui_inv_area(edgui_disp_t * disp, const edgui_area_t * area_p)
{
    if(!disp) disp = edgui_disp_get_default();
    if(!disp) return;

    /*Clear the invalidate buffer if the parameter is NULL*/
    if(area_p == NULL) {
        disp->inv_p = 0;
        return;
    }

    edgui_area_t scr_area;
    scr_area.x1 = 0;
    scr_area.y1 = 0;
    scr_area.x2 = edgui_disp_get_hor_res(disp) - 1;
    scr_area.y2 = edgui_disp_get_ver_res(disp) - 1;

    edgui_area_t com_area;
    bool suc;

    suc = edgui_area_intersect(&com_area, area_p, &scr_area);

    /*The area is truncated to the screen*/
    if(suc != false) {
        if(disp->driver.rounder_cb) disp->driver.rounder_cb(&disp->driver, &com_area);

        /*Save only if this area is not in one of the saved areas*/
        uint16_t i;
        for(i = 0; i < disp->inv_p; i++) {
            if(edgui_area_is_in(&com_area, &disp->inv_areas[i]) != false) return;
        }

        /*Save the area*/
        if(disp->inv_p < EDGUI_INV_BUF_SIZE) {
            edgui_area_copy(&disp->inv_areas[disp->inv_p], &com_area);
        } else { /*If no place for the area add the screen*/
            disp->inv_p = 0;
            edgui_area_copy(&disp->inv_areas[disp->inv_p], &scr_area);
        }
        disp->inv_p++;
    }
}

/**
 * Get the display which is being refreshed
 * @return the display being refreshed
 */
edgui_disp_t * edgui_refr_get_disp_refreshing(void)
{
    return disp_refr;
}

/**
 * Set the display which is being refreshed.
 * It shouldn1t be used directly by the user.
 * It can be used to trick the drawing functions about there is an active display.
 * @param the display being refreshed
 */
void edgui_refr_set_disp_refreshing(edgui_disp_t * disp)
{
    disp_refr = disp;
}

/**
 * Called periodically to handle the refreshing
 * @param task pointer to the task itself
 */
void edgui_disp_refr_task(edgui_task_t * task)
{
    EDGUI_LOG_TRACE("edgui_refr_task: started");

    uint32_t start = edgui_tick_get();

    disp_refr = task->user_data;

    edgui_refr_join_area();

    edgui_refr_areas();

    /*If refresh happened ...*/
    if(disp_refr->inv_p != 0) {
        /*In true double buffered mode copy the refreshed areas to the new VDB to keep it up to
         * date*/
        if(edgui_disp_is_true_double_buf(disp_refr)) {
            edgui_disp_buf_t * vdb = edgui_disp_get_buf(disp_refr);

            /*Flush the content of the VDB*/
            edgui_refr_vdb_flush();

            /* With true double buffering the flushing should be only the address change of the
             * current frame buffer. Wait until the address change is ready and copy the changed
             * content to the other frame buffer (new active VDB) to keep the buffers synchronized*/
            while(vdb->flushing)
                ;

            uint8_t * buf_act = (uint8_t *)vdb->buf_act;
            uint8_t * buf_ina = (uint8_t *)vdb->buf_act == vdb->buf1 ? vdb->buf2 : vdb->buf1;

            edgui_coord_t hres = edgui_disp_get_hor_res(disp_refr);
            uint16_t a;
            for(a = 0; a < disp_refr->inv_p; a++) {
                if(disp_refr->inv_area_joined[a] == 0) {
                    edgui_coord_t y;
                    uint32_t start_offs =
                        (hres * disp_refr->inv_areas[a].y1 + disp_refr->inv_areas[a].x1) * sizeof(edgui_color_t);
                    uint32_t line_length = edgui_area_get_width(&disp_refr->inv_areas[a]) * sizeof(edgui_color_t);

                    for(y = disp_refr->inv_areas[a].y1; y <= disp_refr->inv_areas[a].y2; y++) {
                        memcpy(buf_act + start_offs, buf_ina + start_offs, line_length);
                        start_offs += hres * sizeof(edgui_color_t);
                    }
                }
            }
        } /*End of true double buffer handling*/

        /*Clean up*/
        memset(disp_refr->inv_areas, 0, sizeof(disp_refr->inv_areas));
        memset(disp_refr->inv_area_joined, 0, sizeof(disp_refr->inv_area_joined));
        disp_refr->inv_p = 0;

        /*Call monitor cb if present*/
        if(disp_refr->driver.monitor_cb) {
            disp_refr->driver.monitor_cb(&disp_refr->driver, edgui_tick_elaps(start), px_num);
        }
    }

    edgui_draw_free_buf();

    EDGUI_LOG_TRACE("edgui_refr_task: ready");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Join the areas which has got common parts
 */
static void edgui_refr_join_area(void)
{
    uint32_t join_from;
    uint32_t join_in;
    edgui_area_t joined_area;
    for(join_in = 0; join_in < disp_refr->inv_p; join_in++) {
        if(disp_refr->inv_area_joined[join_in] != 0) continue;

        /*Check all areas to join them in 'join_in'*/
        for(join_from = 0; join_from < disp_refr->inv_p; join_from++) {
            /*Handle only unjoined areas and ignore itself*/
            if(disp_refr->inv_area_joined[join_from] != 0 || join_in == join_from) {
                continue;
            }

            /*Check if the areas are on each other*/
            if(edgui_area_is_on(&disp_refr->inv_areas[join_in], &disp_refr->inv_areas[join_from]) == false) {
                continue;
            }

            edgui_area_join(&joined_area, &disp_refr->inv_areas[join_in], &disp_refr->inv_areas[join_from]);

            /*Join two area only if the joined area size is smaller*/
            if(edgui_area_get_size(&joined_area) < (edgui_area_get_size(&disp_refr->inv_areas[join_in]) +
                                                 edgui_area_get_size(&disp_refr->inv_areas[join_from]))) {
                edgui_area_copy(&disp_refr->inv_areas[join_in], &joined_area);

                /*Mark 'join_form' is joined into 'join_in'*/
                disp_refr->inv_area_joined[join_from] = 1;
            }
        }
    }
}

/**
 * Refresh the joined areas
 */
static void edgui_refr_areas(void)
{
    px_num = 0;
    uint32_t i;

    for(i = 0; i < disp_refr->inv_p; i++) {
        /*Refresh the unjoined areas*/
        if(disp_refr->inv_area_joined[i] == 0) {

            edgui_refr_area(&disp_refr->inv_areas[i]);

            if(disp_refr->driver.monitor_cb) px_num += edgui_area_get_size(&disp_refr->inv_areas[i]);
        }
    }
}

/**
 * Refresh an area if there is Virtual Display Buffer
 * @param area_p  pointer to an area to refresh
 */
static void edgui_refr_area(const edgui_area_t * area_p)
{
    /*True double buffering: there are two screen sized buffers. Just redraw directly into a
     * buffer*/
    if(edgui_disp_is_true_double_buf(disp_refr)) {
        edgui_disp_buf_t * vdb = edgui_disp_get_buf(disp_refr);
        vdb->area.x1        = 0;
        vdb->area.x2        = edgui_disp_get_hor_res(disp_refr) - 1;
        vdb->area.y1        = 0;
        vdb->area.y2        = edgui_disp_get_ver_res(disp_refr) - 1;
        edgui_refr_area_part(area_p);
    }
    /*The buffer is smaller: refresh the area in parts*/
    else {
        edgui_disp_buf_t * vdb = edgui_disp_get_buf(disp_refr);
        /*Calculate the max row num*/
        edgui_coord_t w = edgui_area_get_width(area_p);
        edgui_coord_t h = edgui_area_get_height(area_p);
        edgui_coord_t y2 =
            area_p->y2 >= edgui_disp_get_ver_res(disp_refr) ? y2 = edgui_disp_get_ver_res(disp_refr) - 1 : area_p->y2;

        int32_t max_row = (uint32_t)vdb->size / w;

        if(max_row > h) max_row = h;

        /*Round down the lines of VDB if rounding is added*/
        if(disp_refr->driver.rounder_cb) {
            edgui_area_t tmp;
            tmp.x1 = 0;
            tmp.x2 = 0;
            tmp.y1 = 0;

            edgui_coord_t y_tmp = max_row - 1;
            do {
                tmp.y2 = y_tmp;
                disp_refr->driver.rounder_cb(&disp_refr->driver, &tmp);

                /*If this height fits into `max_row` then fine*/
                if(edgui_area_get_height(&tmp) <= max_row) break;

                /*Decrement the height of the area until it fits into `max_row` after rounding*/
                y_tmp--;
            } while(y_tmp != 0);

            if(y_tmp == 0) {
                EDGUI_LOG_WARN("Can't set VDB height using the round function. (Wrong round_cb or to "
                            "small VDB)");
                return;
            } else {
                max_row = tmp.y2 + 1;
            }
        }

        /*Always use the full row*/
        edgui_coord_t row;
        edgui_coord_t row_last = 0;
        for(row = area_p->y1; row + max_row - 1 <= y2; row += max_row) {
            /*Calc. the next y coordinates of VDB*/
            vdb->area.x1 = area_p->x1;
            vdb->area.x2 = area_p->x2;
            vdb->area.y1 = row;
            vdb->area.y2 = row + max_row - 1;
            if(vdb->area.y2 > y2) vdb->area.y2 = y2;
            row_last = vdb->area.y2;
            edgui_refr_area_part(area_p);
        }

        /*If the last y coordinates are not handled yet ...*/
        if(y2 != row_last) {
            /*Calc. the next y coordinates of VDB*/
            vdb->area.x1 = area_p->x1;
            vdb->area.x2 = area_p->x2;
            vdb->area.y1 = row;
            vdb->area.y2 = y2;

            /*Refresh this part too*/
            edgui_refr_area_part(area_p);
        }
    }
}

/**
 * Refresh a part of an area which is on the actual Virtual Display Buffer
 * @param area_p pointer to an area to refresh
 */
static void edgui_refr_area_part(const edgui_area_t * area_p)
{

    edgui_disp_buf_t * vdb = edgui_disp_get_buf(disp_refr);

    /*In non double buffered mode, before rendering the next part wait until the previous image is
     * flushed*/
    if(edgui_disp_is_double_buf(disp_refr) == false) {
        while(vdb->flushing)
            ;
    }

    edgui_obj_t * top_p;

    /*Get the new mask from the original area and the act. VDB
     It will be a part of 'area_p'*/
    edgui_area_t start_mask;
    edgui_area_intersect(&start_mask, area_p, &vdb->area);

    /*Get the most top object which is not covered by others*/
    top_p = edgui_refr_get_top_obj(&start_mask, edgui_disp_get_scr_act(disp_refr));

    /*Do the refreshing from the top object*/
    edgui_refr_obj_and_children(top_p, &start_mask);

    /*Also refresh top and sys layer unconditionally*/
    edgui_refr_obj_and_children(edgui_disp_get_layer_top(disp_refr), &start_mask);
    edgui_refr_obj_and_children(edgui_disp_get_layer_sys(disp_refr), &start_mask);

    /* In true double buffered mode flush only once when all areas were rendered.
     * In normal mode flush after every area */
    if(edgui_disp_is_true_double_buf(disp_refr) == false) {
        edgui_refr_vdb_flush();
    }
}

/**
 * Search the most top object which fully covers an area
 * @param area_p pointer to an area
 * @param obj the first object to start the searching (typically a screen)
 * @return
 */
static edgui_obj_t * edgui_refr_get_top_obj(const edgui_area_t * area_p, edgui_obj_t * obj)
{
    edgui_obj_t * found_p = NULL;

    /*If this object is fully cover the draw area check the children too */
    if(edgui_area_is_in(area_p, &obj->coords) && obj->hidden == 0) {
        edgui_obj_t * i;
        EDGUI_LL_READ(obj->child_ll, i)
        {
            found_p = edgui_refr_get_top_obj(area_p, i);

            /*If a children is ok then break*/
            if(found_p != NULL) {
                break;
            }
        }

        /*If no better children check this object*/
        if(found_p == NULL) {
            const edgui_style_t * style = edgui_obj_get_style(obj);
            if(style->body.opa == EDGUI_OPA_COVER && obj->design_cb(obj, area_p, EDGUI_DESIGN_COVER_CHK) != false &&
               edgui_obj_get_opa_scale(obj) == EDGUI_OPA_COVER) {
                found_p = obj;
            }
        }
    }

    return found_p;
}

/**
 * Make the refreshing from an object. Draw all its children and the youngers too.
 * @param top_p pointer to an objects. Start the drawing from it.
 * @param mask_p pointer to an area, the objects will be drawn only here
 */
static void edgui_refr_obj_and_children(edgui_obj_t * top_p, const edgui_area_t * mask_p)
{
    /* Normally always will be a top_obj (at least the screen)
     * but in special cases (e.g. if the screen has alpha) it won't.
     * In this case use the screen directly */
    if(top_p == NULL) top_p = edgui_disp_get_scr_act(disp_refr);

    /*Refresh the top object and its children*/
    edgui_refr_obj(top_p, mask_p);

    /*Draw the 'younger' sibling objects because they can be on top_obj */
    edgui_obj_t * par;
    edgui_obj_t * border_p = top_p;

    par = edgui_obj_get_parent(top_p);

    /*Do until not reach the screen*/
    while(par != NULL) {
        /*object before border_p has to be redrawn*/
        edgui_obj_t * i = edgui_ll_get_prev(&(par->child_ll), border_p);

        while(i != NULL) {
            /*Refresh the objects*/
            edgui_refr_obj(i, mask_p);
            i = edgui_ll_get_prev(&(par->child_ll), i);
        }

        /*Call the post draw design function of the parents of the to object*/
        par->design_cb(par, mask_p, EDGUI_DESIGN_DRAW_POST);

        /*The new border will be there last parents,
         *so the 'younger' brothers of parent will be refreshed*/
        border_p = par;
        /*Go a level deeper*/
        par = edgui_obj_get_parent(par);
    }
}

/**
 * Refresh an object an all of its children. (Called recursively)
 * @param obj pointer to an object to refresh
 * @param mask_ori_p pointer to an area, the objects will be drawn only here
 */
static void edgui_refr_obj(edgui_obj_t * obj, const edgui_area_t * mask_ori_p)
{
    /*Do not refresh hidden objects*/
    if(obj->hidden != 0) return;

    bool union_ok; /* Store the return value of area_union */
    /* Truncate the original mask to the coordinates of the parent
     * because the parent and its children are visible only here */
    edgui_area_t obj_mask;
    edgui_area_t obj_ext_mask;
    edgui_area_t obj_area;
    edgui_coord_t ext_size = obj->ext_draw_pad;
    edgui_obj_get_coords(obj, &obj_area);
    obj_area.x1 -= ext_size;
    obj_area.y1 -= ext_size;
    obj_area.x2 += ext_size;
    obj_area.y2 += ext_size;
    union_ok = edgui_area_intersect(&obj_ext_mask, mask_ori_p, &obj_area);

    /*Draw the parent and its children only if they ore on 'mask_parent'*/
    if(union_ok != false) {

        /* Redraw the object */
        obj->design_cb(obj, &obj_ext_mask, EDGUI_DESIGN_DRAW_MAIN);

#if MASK_AREA_DEBUG
        static edgui_color_t debug_color = EDGUI_COLOR_RED;
        edgui_draw_fill(&obj_ext_mask, &obj_ext_mask, debug_color, EDGUI_OPA_50);
        debug_color.full *= 17;
        debug_color.full += 0xA1;
#endif
        /*Create a new 'obj_mask' without 'ext_size' because the children can't be visible there*/
        edgui_obj_get_coords(obj, &obj_area);
        union_ok = edgui_area_intersect(&obj_mask, mask_ori_p, &obj_area);
        if(union_ok != false) {
            edgui_area_t mask_child; /*Mask from obj and its child*/
            edgui_obj_t * child_p;
            edgui_area_t child_area;
            EDGUI_LL_READ_BACK(obj->child_ll, child_p)
            {
                edgui_obj_get_coords(child_p, &child_area);
                ext_size = child_p->ext_draw_pad;
                child_area.x1 -= ext_size;
                child_area.y1 -= ext_size;
                child_area.x2 += ext_size;
                child_area.y2 += ext_size;
                /* Get the union (common parts) of original mask (from obj)
                 * and its child */
                union_ok = edgui_area_intersect(&mask_child, &obj_mask, &child_area);

                /*If the parent and the child has common area then refresh the child */
                if(union_ok) {
                    /*Refresh the next children*/
                    edgui_refr_obj(child_p, &mask_child);
                }
            }
        }

        /* If all the children are redrawn make 'post draw' design */
        obj->design_cb(obj, &obj_ext_mask, EDGUI_DESIGN_DRAW_POST);
    }
}

/**
 * Flush the content of the VDB
 */
static void edgui_refr_vdb_flush(void)
{
    edgui_disp_buf_t * vdb = edgui_disp_get_buf(disp_refr);

    /*In double buffered mode wait until the other buffer is flushed before flushing the current
     * one*/
    if(edgui_disp_is_double_buf(disp_refr)) {
        while(vdb->flushing)
            ;
    }

    vdb->flushing = 1;

    /*Flush the rendered content to the display*/
    edgui_disp_t * disp = edgui_refr_get_disp_refreshing();
    if(disp->driver.flush_cb) disp->driver.flush_cb(&disp->driver, &vdb->area, vdb->buf_act);

    if(vdb->buf1 && vdb->buf2) {
        if(vdb->buf_act == vdb->buf1)
            vdb->buf_act = vdb->buf2;
        else
            vdb->buf_act = vdb->buf1;
    }
}
