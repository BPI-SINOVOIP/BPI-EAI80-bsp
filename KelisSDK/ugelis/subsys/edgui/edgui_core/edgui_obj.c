/**
 * @file edgui_base_obj.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_obj.h"
#include "edgui_indev.h"
#include "edgui_refr.h"
#include "edgui_group.h"
#include "edgui_disp.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_draw/edgui_draw.h"
#include "../edgui_misc/edgui_anim.h"
#include "../edgui_misc/edgui_task.h"
#include "../edgui_misc/edgui_async.h"
#include "../edgui_misc/edgui_fs.h"
#include "../edgui_hal/edgui_hal.h"
#include <stdint.h>
#include <string.h>
#include "../edgui_misc/edgui_gc.h"
#include "../edgui_misc/edgui_math.h"

#if defined(EDGUI_GC_INCLUDE)
#include EDGUI_GC_INCLUDE
#endif /* EDGUI_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/
#define EDGUI_OBJ_DEF_WIDTH (EDGUI_DPI)
#define EDGUI_OBJ_DEF_HEIGHT (2 * EDGUI_DPI / 3)

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _edgui_event_temp_data
{
    edgui_obj_t * obj;
    bool deleted;
    struct _edgui_event_temp_data * prev;
} edgui_event_temp_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void refresh_children_position(edgui_obj_t * obj, edgui_coord_t x_diff, edgui_coord_t y_diff);
static void report_style_mod_core(void * style_p, edgui_obj_t * obj);
static void refresh_children_style(edgui_obj_t * obj);
static void delete_children(edgui_obj_t * obj);
static void edgui_event_mark_deleted(edgui_obj_t * obj);
static void edgui_obj_del_async_cb(void * obj);
static bool edgui_obj_design(edgui_obj_t * obj, const edgui_area_t * mask_p, edgui_design_mode_t mode);
static edgui_res_t edgui_obj_signal(edgui_obj_t * obj, edgui_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool edgui_initialized = false;
static edgui_event_temp_data_t * event_temp_data_head;
static const void * event_act_data;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init. the 'lv' library.
 */
void edgui_init(void)
{
    /* Do nothing if already initialized */
    if(edgui_initialized) {
        EDGUI_LOG_WARN("edgui_init: already inited");
        return;
    }

    EDGUI_LOG_TRACE("edgui_init started");

    /*Initialize the edgui_misc modules*/
    edgui_mem_init();
    edgui_task_core_init();

#if EDGUI_USE_FILESYSTEM
    edgui_fs_init();
#endif

#if EDGUI_USE_ANIMATION
    edgui_anim_core_init();
#endif

#if EDGUI_USE_GROUP
    edgui_group_init();
#endif

    /*Init. the sstyles*/
    edgui_style_init();

    /*Initialize the screen refresh system*/
    edgui_refr_init();

    edgui_ll_init(&EDGUI_GC_ROOT(_edgui_disp_ll), sizeof(edgui_disp_t));
    edgui_ll_init(&EDGUI_GC_ROOT(_edgui_indev_ll), sizeof(edgui_indev_t));

    /*Init the input device handling*/
    edgui_indev_init();

    edgui_img_decoder_init();
    edgui_img_cache_set_size(EDGUI_IMG_CACHE_DEF_SIZE);

    edgui_initialized = true;
    EDGUI_LOG_INFO("edgui_init ready");
}

/*--------------------
 * Create and delete
 *-------------------*/

/**
 * Create a basic object
 * @param parent pointer to a parent object.
 *                  If NULL then a screen will be created
 * @param copy pointer to a base object, if not NULL then the new object will be copied from it
 * @return pointer to the new object
 */
edgui_obj_t * edgui_obj_create(edgui_obj_t * parent, const edgui_obj_t * copy)
{

    edgui_obj_t * new_obj = NULL;
    /*Create a screen if the parent is NULL*/
    if(parent == NULL) {
        EDGUI_LOG_TRACE("Screen create started");
        edgui_disp_t * disp = edgui_disp_get_default();
        if(!disp) {
            EDGUI_LOG_WARN("edgui_obj_create: not display created to so far. No place to assign the new screen");
            return NULL;
        }

        new_obj = edgui_ll_ins_head(&disp->scr_ll);
        edgui_mem_assert(new_obj);
        if(new_obj == NULL) return NULL;

        new_obj->par = NULL; /*Screens has no a parent*/
        edgui_ll_init(&(new_obj->child_ll), sizeof(edgui_obj_t));

        /*Set coordinates to full screen size*/
        new_obj->coords.x1    = 0;
        new_obj->coords.y1    = 0;
        new_obj->coords.x2    = edgui_disp_get_hor_res(NULL) - 1;
        new_obj->coords.y2    = edgui_disp_get_ver_res(NULL) - 1;
        new_obj->ext_draw_pad = 0;

#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_FULL
        memset(&new_obj->ext_click_pad, 0, sizeof(new_obj->ext_click_pad));
#endif

#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_TINY
        new_obj->ext_click_pad_hor = 0;
        new_obj->ext_click_pad_ver = 0;
#endif

        /*Init realign*/
#if EDGUI_USE_OBJ_REALIGN
        new_obj->realign.align        = EDGUI_ALIGN_CENTER;
        new_obj->realign.xofs         = 0;
        new_obj->realign.yofs         = 0;
        new_obj->realign.base         = NULL;
        new_obj->realign.auto_realign = 0;
#endif

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            new_obj->style_p = th->style.scr;
        } else {
            new_obj->style_p = &edgui_style_scr;
        }
        /*Set the callbacks*/
        edgui_obj_set_signal_cb(new_obj, edgui_obj_signal);
        edgui_obj_set_design_cb(new_obj, edgui_obj_design);
        new_obj->event_cb = NULL;

        /*Init. user date*/
#if EDGUI_USE_USER_DATA
        memset(&new_obj->user_data, 0, sizeof(edgui_obj_user_data_t));
#endif

#if EDGUI_USE_GROUP
        new_obj->group_p = NULL;
#endif
        /*Set attributes*/
        new_obj->click        = 0;
        new_obj->drag         = 0;
        new_obj->drag_throw   = 0;
        new_obj->drag_parent  = 0;
        new_obj->hidden       = 0;
        new_obj->top          = 0;
        new_obj->protect      = EDGUI_PROTECT_NONE;
        new_obj->opa_scale_en = 0;
        new_obj->opa_scale    = EDGUI_OPA_COVER;
        new_obj->parent_event = 0;
        new_obj->reserved     = 0;

        new_obj->ext_attr = NULL;

        EDGUI_LOG_INFO("Screen create ready");
    }
    /*parent != NULL create normal obj. on a parent*/
    else {
        EDGUI_LOG_TRACE("Object create started");

        new_obj = edgui_ll_ins_head(&parent->child_ll);
        edgui_mem_assert(new_obj);
        if(new_obj == NULL) return NULL;

        new_obj->par = parent; /*Set the parent*/
        edgui_ll_init(&(new_obj->child_ll), sizeof(edgui_obj_t));

        /*Set coordinates left top corner of parent*/
        new_obj->coords.x1    = parent->coords.x1;
        new_obj->coords.y1    = parent->coords.y1;
        new_obj->coords.x2    = parent->coords.x1 + EDGUI_OBJ_DEF_WIDTH;
        new_obj->coords.y2    = parent->coords.y1 + EDGUI_OBJ_DEF_HEIGHT;
        new_obj->ext_draw_pad = 0;

#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_FULL
        memset(&new_obj->ext_click_pad, 0, sizeof(new_obj->ext_click_pad));
#endif

#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_TINY
        new_obj->ext_click_pad_hor = 0;
        new_obj->ext_click_pad_ver = 0;
#endif

        /*Init realign*/
#if EDGUI_USE_OBJ_REALIGN
        new_obj->realign.align        = EDGUI_ALIGN_CENTER;
        new_obj->realign.xofs         = 0;
        new_obj->realign.yofs         = 0;
        new_obj->realign.base         = NULL;
        new_obj->realign.auto_realign = 0;
#endif
        /*Set appearance*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            new_obj->style_p = th->style.panel;
        } else {
            new_obj->style_p = &edgui_style_plain_color;
        }

        /*Set the callbacks*/
        edgui_obj_set_signal_cb(new_obj, edgui_obj_signal);
        edgui_obj_set_design_cb(new_obj, edgui_obj_design);
        new_obj->event_cb = NULL;

#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_FULL
        memset(&new_obj->ext_click_pad, 0, sizeof(new_obj->ext_click_pad));
#endif

#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_TINY
        new_obj->ext_click_pad_hor = 0;
        new_obj->ext_click_pad_ver = 0;
#endif

        /*Init. user date*/
#if EDGUI_USE_USER_DATA
        memset(&new_obj->user_data, 0, sizeof(edgui_obj_user_data_t));
#endif

#if EDGUI_USE_GROUP
        new_obj->group_p = NULL;
#endif

        /*Set attributes*/
        new_obj->click        = 1;
        new_obj->drag         = 0;
        new_obj->drag_dir     = EDGUI_DRAG_DIR_ALL;
        new_obj->drag_throw   = 0;
        new_obj->drag_parent  = 0;
        new_obj->hidden       = 0;
        new_obj->top          = 0;
        new_obj->protect      = EDGUI_PROTECT_NONE;
        new_obj->opa_scale    = EDGUI_OPA_COVER;
        new_obj->opa_scale_en = 0;
        new_obj->parent_event = 0;

        new_obj->ext_attr = NULL;
    }

    /*Copy the attributes if required*/
    if(copy != NULL) {
        edgui_area_copy(&new_obj->coords, &copy->coords);
        new_obj->ext_draw_pad = copy->ext_draw_pad;

#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_FULL
        edgui_area_copy(&new_obj->ext_click_pad, &copy->ext_click_pad);
#endif

#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_TINY
        new_obj->ext_click_pad_hor = copy->ext_click_pad_hor;
        new_obj->ext_click_pad_ver = copy->ext_click_pad_ver;
#endif

        /*Set free data*/
#if EDGUI_USE_USER_DATA
        memcpy(&new_obj->user_data, &copy->user_data, sizeof(edgui_obj_user_data_t));
#endif
        /*Copy realign*/
#if EDGUI_USE_OBJ_REALIGN
        new_obj->realign.align        = copy->realign.align;
        new_obj->realign.xofs         = copy->realign.xofs;
        new_obj->realign.yofs         = copy->realign.yofs;
        new_obj->realign.base         = copy->realign.base;
        new_obj->realign.auto_realign = copy->realign.auto_realign;
#endif

        /*Only copy the `event_cb`. `signal_cb` and `design_cb` will be copied the the derived
         * object type (e.g. `edgui_btn`)*/
        new_obj->event_cb = copy->event_cb;

        /*Copy attributes*/
        new_obj->click        = copy->click;
        new_obj->drag         = copy->drag;
        new_obj->drag_dir     = copy->drag_dir;
        new_obj->drag_throw   = copy->drag_throw;
        new_obj->drag_parent  = copy->drag_parent;
        new_obj->hidden       = copy->hidden;
        new_obj->top          = copy->top;
        new_obj->parent_event = copy->parent_event;

        new_obj->opa_scale_en = copy->opa_scale_en;
        new_obj->protect      = copy->protect;
        new_obj->opa_scale    = copy->opa_scale;

        new_obj->style_p = copy->style_p;

#if EDGUI_USE_GROUP
        /*Add to the same group*/
        if(copy->group_p != NULL) {
            edgui_group_add_obj(copy->group_p, new_obj);
        }
#endif

        /*Set the same coordinates for non screen objects*/
        if(edgui_obj_get_parent(copy) != NULL && parent != NULL) {
            edgui_obj_set_pos(new_obj, edgui_obj_get_x(copy), edgui_obj_get_y(copy));
        } else {
            edgui_obj_set_pos(new_obj, 0, 0);
        }

        EDGUI_LOG_INFO("Object create ready");
    }

    /*Send a signal to the parent to notify it about the new child*/
    if(parent != NULL) {
        parent->signal_cb(parent, EDGUI_SIGNAL_CHILD_CHG, new_obj);

        /*Invalidate the area if not screen created*/
        edgui_obj_invalidate(new_obj);
    }

    return new_obj;
}

/**
 * Delete 'obj' and all of its children
 * @param obj pointer to an object to delete
 * @return EDGUI_RES_INV because the object is deleted
 */
edgui_res_t edgui_obj_del(edgui_obj_t * obj)
{
    edgui_obj_invalidate(obj);

    /*Delete from the group*/
#if EDGUI_USE_GROUP
    edgui_group_t * group = edgui_obj_get_group(obj);
    if(group) edgui_group_remove_obj(obj);
#endif

        /*Remove the animations from this object*/
#if EDGUI_USE_ANIMATION
    edgui_anim_del(obj, NULL);
#endif

    /*Recursively delete the children*/
    edgui_obj_t * i;
    edgui_obj_t * i_next;
    i = edgui_ll_get_head(&(obj->child_ll));
    while(i != NULL) {
        /*Get the next object before delete this*/
        i_next = edgui_ll_get_next(&(obj->child_ll), i);

        /*Call the recursive del to the child too*/
        delete_children(i);

        /*Set i to the next node*/
        i = i_next;
    }

    /*Let the user free the resources used in `EDGUI_EVENT_DELETE`*/
    edgui_event_send(obj, EDGUI_EVENT_DELETE, NULL);

    edgui_event_mark_deleted(obj);

    /*Remove the object from parent's children list*/
    edgui_obj_t * par = edgui_obj_get_parent(obj);
    if(par == NULL) { /*It is a screen*/
        edgui_disp_t * d = edgui_obj_get_disp(obj);
        edgui_ll_rem(&d->scr_ll, obj);
    } else {
        edgui_ll_rem(&(par->child_ll), obj);
    }

    /* Reset all input devices if the object to delete is used*/
    edgui_indev_t * indev = edgui_indev_get_next(NULL);
    while(indev) {
        if(indev->proc.types.pointer.act_obj == obj || indev->proc.types.pointer.last_obj == obj) {
            edgui_indev_reset(indev);
        }
        if(indev->proc.types.pointer.last_pressed == obj) {
            indev->proc.types.pointer.last_pressed = NULL;
        }

#if EDGUI_USE_GROUP
        if(indev->group == group && obj == edgui_indev_get_obj_act()) {
            edgui_indev_reset(indev);
        }
#endif
        indev = edgui_indev_get_next(indev);
    }

    /* All children deleted.
     * Now clean up the object specific data*/
    obj->signal_cb(obj, EDGUI_SIGNAL_CLEANUP, NULL);

    /*Delete the base objects*/
    if(obj->ext_attr != NULL) edgui_mem_free(obj->ext_attr);
    edgui_mem_free(obj); /*Free the object itself*/

    /*Send a signal to the parent to notify it about the child delete*/
    if(par != NULL) {
        par->signal_cb(par, EDGUI_SIGNAL_CHILD_CHG, NULL);
    }

    return EDGUI_RES_INV;
}

/**
 * Helper function for asynchronously deleting objects.
 * Useful for cases where you can't delete an object directly in an `EDGUI_EVENT_DELETE` handler (i.e. parent).
 * @param obj object to delete
 * @see edgui_async_call
 */
void edgui_obj_del_async(edgui_obj_t * obj)
{
    edgui_async_call(edgui_obj_del_async_cb, obj);
}

/**
 * Delete all children of an object
 * @param obj pointer to an object
 */
void edgui_obj_clean(edgui_obj_t * obj)
{
    edgui_obj_t * child = edgui_obj_get_child(obj, NULL);
    edgui_obj_t * child_next;
    while(child) {
        /* Read the next child before deleting the current
         * because the next couldn't be read from a deleted (invalid) node*/
        child_next = edgui_obj_get_child(obj, child);
        edgui_obj_del(child);
        child = child_next;
    }
}

/**
 * Mark the object as invalid therefore its current position will be redrawn by 'edgui_refr_task'
 * @param obj pointer to an object
 */
void edgui_obj_invalidate(const edgui_obj_t * obj)
{
    if(edgui_obj_get_hidden(obj)) return;

    /*Invalidate the object only if it belongs to the 'EDGUI_GC_ROOT(_edgui_act_scr)'*/
    edgui_obj_t * obj_scr = edgui_obj_get_screen(obj);
    edgui_disp_t * disp   = edgui_obj_get_disp(obj_scr);
    if(obj_scr == edgui_disp_get_scr_act(disp) || obj_scr == edgui_disp_get_layer_top(disp) ||
       obj_scr == edgui_disp_get_layer_sys(disp)) {
        /*Truncate recursively to the parents*/
        edgui_area_t area_trunc;
        edgui_obj_t * par = edgui_obj_get_parent(obj);
        bool union_ok  = true;
        /*Start with the original coordinates*/
        edgui_coord_t ext_size = obj->ext_draw_pad;
        edgui_area_copy(&area_trunc, &obj->coords);
        area_trunc.x1 -= ext_size;
        area_trunc.y1 -= ext_size;
        area_trunc.x2 += ext_size;
        area_trunc.y2 += ext_size;

        /*Check through all parents*/
        while(par != NULL) {
            union_ok = edgui_area_intersect(&area_trunc, &area_trunc, &par->coords);
            if(union_ok == false) break;       /*If no common parts with parent break;*/
            if(edgui_obj_get_hidden(par)) return; /*If the parent is hidden then the child is hidden and won't be drawn*/

            par = edgui_obj_get_parent(par);
        }

        if(union_ok) edgui_inv_area(disp, &area_trunc);
    }
}

/*=====================
 * Setter functions
 *====================*/

/*--------------------
 * Parent/children set
 *--------------------*/

/**
 * Set a new parent for an object. Its relative position will be the same.
 * @param obj pointer to an object. Can't be a screen.
 * @param parent pointer to the new parent object. (Can't be NULL)
 */
void edgui_obj_set_parent(edgui_obj_t * obj, edgui_obj_t * parent)
{
    if(obj->par == NULL) {
        EDGUI_LOG_WARN("Can't set the parent of a screen");
        return;
    }

    if(parent == NULL) {
        EDGUI_LOG_WARN("Can't set parent == NULL to an object");
        return;
    }

    edgui_obj_invalidate(obj);

    edgui_point_t old_pos;
    old_pos.x = edgui_obj_get_x(obj);
    old_pos.y = edgui_obj_get_y(obj);

    edgui_obj_t * old_par = obj->par;

    edgui_ll_chg_list(&obj->par->child_ll, &parent->child_ll, obj, true);
    obj->par = parent;
    edgui_obj_set_pos(obj, old_pos.x, old_pos.y);

    /*Notify the original parent because one of its children is lost*/
    old_par->signal_cb(old_par, EDGUI_SIGNAL_CHILD_CHG, NULL);

    /*Notify the new parent about the child*/
    parent->signal_cb(parent, EDGUI_SIGNAL_CHILD_CHG, obj);

    edgui_obj_invalidate(obj);
}

/**
 * Move and object to the foreground
 * @param obj pointer to an object
 */
void edgui_obj_move_foreground(edgui_obj_t * obj)
{
    edgui_obj_t * parent = edgui_obj_get_parent(obj);

    /*Do nothing of already in the foreground*/
    if(edgui_ll_get_head(&parent->child_ll) == obj) return;

    edgui_obj_invalidate(parent);

    edgui_ll_chg_list(&parent->child_ll, &parent->child_ll, obj, true);

    /*Notify the new parent about the child*/
    parent->signal_cb(parent, EDGUI_SIGNAL_CHILD_CHG, obj);

    edgui_obj_invalidate(parent);
}

/**
 * Move and object to the background
 * @param obj pointer to an object
 */
void edgui_obj_move_background(edgui_obj_t * obj)
{
    edgui_obj_t * parent = edgui_obj_get_parent(obj);

    /*Do nothing of already in the background*/
    if(edgui_ll_get_tail(&parent->child_ll) == obj) return;

    edgui_obj_invalidate(parent);

    edgui_ll_chg_list(&parent->child_ll, &parent->child_ll, obj, false);

    /*Notify the new parent about the child*/
    parent->signal_cb(parent, EDGUI_SIGNAL_CHILD_CHG, obj);

    edgui_obj_invalidate(parent);
}

/*--------------------
 * Coordinate set
 * ------------------*/

/**
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent
 * @param y new distance from the top of the parent
 */
void edgui_obj_set_pos(edgui_obj_t * obj, edgui_coord_t x, edgui_coord_t y)
{
    /*Convert x and y to absolute coordinates*/
    edgui_obj_t * par = obj->par;

    x = x + par->coords.x1;
    y = y + par->coords.y1;

    /*Calculate and set the movement*/
    edgui_point_t diff;
    diff.x = x - obj->coords.x1;
    diff.y = y - obj->coords.y1;

    /* Do nothing if the position is not changed */
    /* It is very important else recursive positioning can
     * occur without position change*/
    if(diff.x == 0 && diff.y == 0) return;

    /*Invalidate the original area*/
    edgui_obj_invalidate(obj);

    /*Save the original coordinates*/
    edgui_area_t ori;
    edgui_obj_get_coords(obj, &ori);

    obj->coords.x1 += diff.x;
    obj->coords.y1 += diff.y;
    obj->coords.x2 += diff.x;
    obj->coords.y2 += diff.y;

    refresh_children_position(obj, diff.x, diff.y);

    /*Inform the object about its new coordinates*/
    obj->signal_cb(obj, EDGUI_SIGNAL_CORD_CHG, &ori);

    /*Send a signal to the parent too*/
    par->signal_cb(par, EDGUI_SIGNAL_CHILD_CHG, obj);

    /*Invalidate the new area*/
    edgui_obj_invalidate(obj);
}

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent
 */
void edgui_obj_set_x(edgui_obj_t * obj, edgui_coord_t x)
{
    edgui_obj_set_pos(obj, x, edgui_obj_get_y(obj));
}

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent
 */
void edgui_obj_set_y(edgui_obj_t * obj, edgui_coord_t y)
{
    edgui_obj_set_pos(obj, edgui_obj_get_x(obj), y);
}

/**
 * Set the size of an object
 * @param obj pointer to an object
 * @param w new width
 * @param h new height
 */
void edgui_obj_set_size(edgui_obj_t * obj, edgui_coord_t w, edgui_coord_t h)
{

    /* Do nothing if the size is not changed */
    /* It is very important else recursive resizing can
     * occur without size change*/
    if(edgui_obj_get_width(obj) == w && edgui_obj_get_height(obj) == h) {
        return;
    }

    /*Invalidate the original area*/
    edgui_obj_invalidate(obj);

    /*Save the original coordinates*/
    edgui_area_t ori;
    edgui_obj_get_coords(obj, &ori);

    /*Set the length and height*/
    obj->coords.x2 = obj->coords.x1 + w - 1;
    obj->coords.y2 = obj->coords.y1 + h - 1;

    /*Send a signal to the object with its new coordinates*/
    obj->signal_cb(obj, EDGUI_SIGNAL_CORD_CHG, &ori);

    /*Send a signal to the parent too*/
    edgui_obj_t * par = edgui_obj_get_parent(obj);
    if(par != NULL) par->signal_cb(par, EDGUI_SIGNAL_CHILD_CHG, obj);

    /*Tell the children the parent's size has changed*/
    edgui_obj_t * i;
    EDGUI_LL_READ(obj->child_ll, i)
    {
        i->signal_cb(i, EDGUI_SIGNAL_PARENT_SIZE_CHG, NULL);
    }

    /*Invalidate the new area*/
    edgui_obj_invalidate(obj);

    /*Automatically realign the object if required*/
#if EDGUI_USE_OBJ_REALIGN
    if(obj->realign.auto_realign) edgui_obj_realign(obj);
#endif
}

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width
 */
void edgui_obj_set_width(edgui_obj_t * obj, edgui_coord_t w)
{
    edgui_obj_set_size(obj, w, edgui_obj_get_height(obj));
}

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height
 */
void edgui_obj_set_height(edgui_obj_t * obj, edgui_coord_t h)
{
    edgui_obj_set_size(obj, edgui_obj_get_width(obj), h);
}

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'edgui_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void edgui_obj_align(edgui_obj_t * obj, const edgui_obj_t * base, edgui_align_t align, edgui_coord_t x_mod, edgui_coord_t y_mod)
{
    edgui_coord_t new_x = edgui_obj_get_x(obj);
    edgui_coord_t new_y = edgui_obj_get_y(obj);

    if(base == NULL) {
        base = edgui_obj_get_parent(obj);
    }

    switch(align) {
        case EDGUI_ALIGN_CENTER:
            new_x = edgui_obj_get_width(base) / 2 - edgui_obj_get_width(obj) / 2;
            new_y = edgui_obj_get_height(base) / 2 - edgui_obj_get_height(obj) / 2;
            break;

        case EDGUI_ALIGN_IN_TOP_LEFT:
            new_x = 0;
            new_y = 0;
            break;
        case EDGUI_ALIGN_IN_TOP_MID:
            new_x = edgui_obj_get_width(base) / 2 - edgui_obj_get_width(obj) / 2;
            new_y = 0;
            break;

        case EDGUI_ALIGN_IN_TOP_RIGHT:
            new_x = edgui_obj_get_width(base) - edgui_obj_get_width(obj);
            new_y = 0;
            break;

        case EDGUI_ALIGN_IN_BOTTOM_LEFT:
            new_x = 0;
            new_y = edgui_obj_get_height(base) - edgui_obj_get_height(obj);
            break;
        case EDGUI_ALIGN_IN_BOTTOM_MID:
            new_x = edgui_obj_get_width(base) / 2 - edgui_obj_get_width(obj) / 2;
            new_y = edgui_obj_get_height(base) - edgui_obj_get_height(obj);
            break;

        case EDGUI_ALIGN_IN_BOTTOM_RIGHT:
            new_x = edgui_obj_get_width(base) - edgui_obj_get_width(obj);
            new_y = edgui_obj_get_height(base) - edgui_obj_get_height(obj);
            break;

        case EDGUI_ALIGN_IN_LEFT_MID:
            new_x = 0;
            new_y = edgui_obj_get_height(base) / 2 - edgui_obj_get_height(obj) / 2;
            break;

        case EDGUI_ALIGN_IN_RIGHT_MID:
            new_x = edgui_obj_get_width(base) - edgui_obj_get_width(obj);
            new_y = edgui_obj_get_height(base) / 2 - edgui_obj_get_height(obj) / 2;
            break;

        case EDGUI_ALIGN_OUT_TOP_LEFT:
            new_x = 0;
            new_y = -edgui_obj_get_height(obj);
            break;

        case EDGUI_ALIGN_OUT_TOP_MID:
            new_x = edgui_obj_get_width(base) / 2 - edgui_obj_get_width(obj) / 2;
            new_y = -edgui_obj_get_height(obj);
            break;

        case EDGUI_ALIGN_OUT_TOP_RIGHT:
            new_x = edgui_obj_get_width(base) - edgui_obj_get_width(obj);
            new_y = -edgui_obj_get_height(obj);
            break;

        case EDGUI_ALIGN_OUT_BOTTOM_LEFT:
            new_x = 0;
            new_y = edgui_obj_get_height(base);
            break;

        case EDGUI_ALIGN_OUT_BOTTOM_MID:
            new_x = edgui_obj_get_width(base) / 2 - edgui_obj_get_width(obj) / 2;
            new_y = edgui_obj_get_height(base);
            break;

        case EDGUI_ALIGN_OUT_BOTTOM_RIGHT:
            new_x = edgui_obj_get_width(base) - edgui_obj_get_width(obj);
            new_y = edgui_obj_get_height(base);
            break;

        case EDGUI_ALIGN_OUT_LEFT_TOP:
            new_x = -edgui_obj_get_width(obj);
            new_y = 0;
            break;

        case EDGUI_ALIGN_OUT_LEFT_MID:
            new_x = -edgui_obj_get_width(obj);
            new_y = edgui_obj_get_height(base) / 2 - edgui_obj_get_height(obj) / 2;
            break;

        case EDGUI_ALIGN_OUT_LEFT_BOTTOM:
            new_x = -edgui_obj_get_width(obj);
            new_y = edgui_obj_get_height(base) - edgui_obj_get_height(obj);
            break;

        case EDGUI_ALIGN_OUT_RIGHT_TOP:
            new_x = edgui_obj_get_width(base);
            new_y = 0;
            break;

        case EDGUI_ALIGN_OUT_RIGHT_MID:
            new_x = edgui_obj_get_width(base);
            new_y = edgui_obj_get_height(base) / 2 - edgui_obj_get_height(obj) / 2;
            break;

        case EDGUI_ALIGN_OUT_RIGHT_BOTTOM:
            new_x = edgui_obj_get_width(base);
            new_y = edgui_obj_get_height(base) - edgui_obj_get_height(obj);
            break;
    }

    /*Bring together the coordination system of base and obj*/
    edgui_obj_t * par        = edgui_obj_get_parent(obj);
    edgui_coord_t base_abs_x = base->coords.x1;
    edgui_coord_t base_abs_y = base->coords.y1;
    edgui_coord_t par_abs_x  = par->coords.x1;
    edgui_coord_t par_abs_y  = par->coords.y1;
    new_x += x_mod + base_abs_x;
    new_y += y_mod + base_abs_y;
    new_x -= par_abs_x;
    new_y -= par_abs_y;

    edgui_obj_set_pos(obj, new_x, new_y);

#if EDGUI_USE_OBJ_REALIGN
    /*Save the last align parameters to use them in `edgui_obj_realign`*/
    obj->realign.align       = align;
    obj->realign.xofs        = x_mod;
    obj->realign.yofs        = y_mod;
    obj->realign.base        = base;
    obj->realign.origo_align = 0;
#endif
}

/**
 * Align an object's middle point to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'edgui_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void edgui_obj_align_origo(edgui_obj_t * obj, const edgui_obj_t * base, edgui_align_t align, edgui_coord_t x_mod, edgui_coord_t y_mod)
{
    edgui_coord_t new_x = edgui_obj_get_x(obj);
    edgui_coord_t new_y = edgui_obj_get_y(obj);

    edgui_coord_t obj_w_half = edgui_obj_get_width(obj) / 2;
    edgui_coord_t obj_h_half = edgui_obj_get_height(obj) / 2;

    if(base == NULL) {
        base = edgui_obj_get_parent(obj);
    }

    switch(align) {
        case EDGUI_ALIGN_CENTER:
            new_x = edgui_obj_get_width(base) / 2 - obj_w_half;
            new_y = edgui_obj_get_height(base) / 2 - obj_h_half;
            break;

        case EDGUI_ALIGN_IN_TOP_LEFT:
            new_x = -obj_w_half;
            new_y = -obj_h_half;
            break;
        case EDGUI_ALIGN_IN_TOP_MID:
            new_x = edgui_obj_get_width(base) / 2 - obj_w_half;
            new_y = -obj_h_half;
            break;

        case EDGUI_ALIGN_IN_TOP_RIGHT:
            new_x = edgui_obj_get_width(base) - obj_w_half;
            new_y = -obj_h_half;
            break;

        case EDGUI_ALIGN_IN_BOTTOM_LEFT:
            new_x = -obj_w_half;
            new_y = edgui_obj_get_height(base) - obj_h_half;
            break;
        case EDGUI_ALIGN_IN_BOTTOM_MID:
            new_x = edgui_obj_get_width(base) / 2 - obj_w_half;
            new_y = edgui_obj_get_height(base) - obj_h_half;
            break;

        case EDGUI_ALIGN_IN_BOTTOM_RIGHT:
            new_x = edgui_obj_get_width(base) - obj_w_half;
            new_y = edgui_obj_get_height(base) - obj_h_half;
            break;

        case EDGUI_ALIGN_IN_LEFT_MID:
            new_x = -obj_w_half;
            new_y = edgui_obj_get_height(base) / 2 - obj_h_half;
            break;

        case EDGUI_ALIGN_IN_RIGHT_MID:
            new_x = edgui_obj_get_width(base) - obj_w_half;
            new_y = edgui_obj_get_height(base) / 2 - obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_TOP_LEFT:
            new_x = -obj_w_half;
            new_y = -obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_TOP_MID:
            new_x = edgui_obj_get_width(base) / 2 - obj_w_half;
            new_y = -obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_TOP_RIGHT:
            new_x = edgui_obj_get_width(base) - obj_w_half;
            new_y = -obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_BOTTOM_LEFT:
            new_x = -obj_w_half;
            new_y = edgui_obj_get_height(base) - obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_BOTTOM_MID:
            new_x = edgui_obj_get_width(base) / 2 - obj_w_half;
            new_y = edgui_obj_get_height(base) - obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_BOTTOM_RIGHT:
            new_x = edgui_obj_get_width(base) - obj_w_half;
            new_y = edgui_obj_get_height(base) - obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_LEFT_TOP:
            new_x = -obj_w_half;
            new_y = -obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_LEFT_MID:
            new_x = -obj_w_half;
            new_y = edgui_obj_get_height(base) / 2 - obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_LEFT_BOTTOM:
            new_x = -obj_w_half;
            new_y = edgui_obj_get_height(base) - obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_RIGHT_TOP:
            new_x = edgui_obj_get_width(base) - obj_w_half;
            new_y = -obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_RIGHT_MID:
            new_x = edgui_obj_get_width(base) - obj_w_half;
            new_y = edgui_obj_get_height(base) / 2 - obj_h_half;
            break;

        case EDGUI_ALIGN_OUT_RIGHT_BOTTOM:
            new_x = edgui_obj_get_width(base) - obj_w_half;
            new_y = edgui_obj_get_height(base) - obj_h_half;
            break;
    }

    /*Bring together the coordination system of base and obj*/
    edgui_obj_t * par        = edgui_obj_get_parent(obj);
    edgui_coord_t base_abs_x = base->coords.x1;
    edgui_coord_t base_abs_y = base->coords.y1;
    edgui_coord_t par_abs_x  = par->coords.x1;
    edgui_coord_t par_abs_y  = par->coords.y1;
    new_x += x_mod + base_abs_x;
    new_y += y_mod + base_abs_y;
    new_x -= par_abs_x;
    new_y -= par_abs_y;

    edgui_obj_set_pos(obj, new_x, new_y);

#if EDGUI_USE_OBJ_REALIGN
    /*Save the last align parameters to use them in `edgui_obj_realign`*/
    obj->realign.align       = align;
    obj->realign.xofs        = x_mod;
    obj->realign.yofs        = y_mod;
    obj->realign.base        = base;
    obj->realign.origo_align = 1;
#endif
}

/**
 * Realign the object based on the last `edgui_obj_align` parameters.
 * @param obj pointer to an object
 */
void edgui_obj_realign(edgui_obj_t * obj)
{
#if EDGUI_USE_OBJ_REALIGN
    if(obj->realign.origo_align)
        edgui_obj_align_origo(obj, obj->realign.base, obj->realign.align, obj->realign.xofs, obj->realign.yofs);
    else
        edgui_obj_align(obj, obj->realign.base, obj->realign.align, obj->realign.xofs, obj->realign.yofs);
#else
    (void)obj;
    EDGUI_LOG_WARN("edgui_obj_realaign: no effect because EDGUI_USE_OBJ_REALIGN = 0");
#endif
}

/**
 * Enable the automatic realign of the object when its size has changed based on the last
 * `edgui_obj_align` parameters.
 * @param obj pointer to an object
 * @param en true: enable auto realign; false: disable auto realign
 */
void edgui_obj_set_auto_realign(edgui_obj_t * obj, bool en)
{
#if EDGUI_USE_OBJ_REALIGN
    obj->realign.auto_realign = en ? 1 : 0;
#else
    (void)obj;
    (void)en;
    EDGUI_LOG_WARN("edgui_obj_set_auto_realign: no effect because EDGUI_USE_OBJ_REALIGN = 0");
#endif
}

/**
 * Set the size of an extended clickable area
 * If TINY mode is used, only the largest of the horizontal and vertical padding
 * values are considered.
 * @param obj pointer to an object
 * @param left extended clickable are on the left [px]
 * @param right extended clickable are on the right [px]
 * @param top extended clickable are on the top [px]
 * @param bottom extended clickable are on the bottom [px]
 */
void edgui_obj_set_ext_click_area(edgui_obj_t * obj, edgui_coord_t left, edgui_coord_t right, edgui_coord_t top, edgui_coord_t bottom)
{
#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_FULL
    obj->ext_click_pad.x1 = left;
    obj->ext_click_pad.x2 = right;
    obj->ext_click_pad.y1 = top;
    obj->ext_click_pad.y2 = bottom;
#elif EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_TINY
    obj->ext_click_pad_hor = EDGUI_MATH_MAX(left, right);
    obj->ext_click_pad_ver = EDGUI_MATH_MAX(top, bottom);
#else
    (void)obj;    /*Unused*/
    (void)left;   /*Unused*/
    (void)right;  /*Unused*/
    (void)top;    /*Unused*/
    (void)bottom; /*Unused*/
#endif
}

/*---------------------
 * Appearance set
 *--------------------*/

/**
 * Set a new style for an object
 * @param obj pointer to an object
 * @param style_p pointer to the new style
 */
void edgui_obj_set_style(edgui_obj_t * obj, const edgui_style_t * style)
{
    obj->style_p = style;

    /*Send a signal about style change to every children with NULL style*/
    refresh_children_style(obj);

    /*Notify the object about the style change too*/
    edgui_obj_refresh_style(obj);
}

/**
 * Notify an object about its style is modified
 * @param obj pointer to an object
 */
void edgui_obj_refresh_style(edgui_obj_t * obj)
{
    edgui_obj_invalidate(obj);
    obj->signal_cb(obj, EDGUI_SIGNAL_STYLE_CHG, NULL);
    edgui_obj_invalidate(obj);
}

/**
 * Notify all object if a style is modified
 * @param style pointer to a style. Only the objects with this style will be notified
 *               (NULL to notify all objects)
 */
void edgui_obj_report_style_mod(edgui_style_t * style)
{
    edgui_disp_t * d = edgui_disp_get_next(NULL);

    while(d) {
        edgui_obj_t * i;
        EDGUI_LL_READ(d->scr_ll, i)
        {
            if(i->style_p == style || style == NULL) {
                edgui_obj_refresh_style(i);
            }

            report_style_mod_core(style, i);
        }
        d = edgui_disp_get_next(d);
    }
}

/*-----------------
 * Attribute set
 *----------------*/

/**
 * Hide an object. It won't be visible and clickable.
 * @param obj pointer to an object
 * @param en true: hide the object
 */
void edgui_obj_set_hidden(edgui_obj_t * obj, bool en)
{
    if(!obj->hidden) edgui_obj_invalidate(obj); /*Invalidate when not hidden (hidden objects are ignored) */

    obj->hidden = en == false ? 0 : 1;

    if(!obj->hidden) edgui_obj_invalidate(obj); /*Invalidate when not hidden (hidden objects are ignored) */

    edgui_obj_t * par = edgui_obj_get_parent(obj);
    par->signal_cb(par, EDGUI_SIGNAL_CHILD_CHG, obj);
}

/**
 * Enable or disable the clicking of an object
 * @param obj pointer to an object
 * @param en true: make the object clickable
 */
void edgui_obj_set_click(edgui_obj_t * obj, bool en)
{
    obj->click = (en == true ? 1 : 0);
}

/**
 * Enable to bring this object to the foreground if it
 * or any of its children is clicked
 * @param obj pointer to an object
 * @param en true: enable the auto top feature
 */
void edgui_obj_set_top(edgui_obj_t * obj, bool en)
{
    obj->top = (en == true ? 1 : 0);
}

/**
 * Enable the dragging of an object
 * @param obj pointer to an object
 * @param en true: make the object dragable
 */
void edgui_obj_set_drag(edgui_obj_t * obj, bool en)
{
    if(en == true) edgui_obj_set_click(obj, true); /*Drag is useless without enabled clicking*/
    obj->drag = (en == true ? 1 : 0);
}

/**
 * Set the directions an object can be dragged in
 * @param obj pointer to an object
 * @param drag_dir bitwise OR of allowed directions an object can be dragged in
 */
void edgui_obj_set_drag_dir(edgui_obj_t * obj, edgui_drag_dir_t drag_dir)
{
    obj->drag_dir = drag_dir;

    if(obj->drag_dir != 0) edgui_obj_set_drag(obj, true); /*Drag direction requires drag*/
}

/**
 * Enable the throwing of an object after is is dragged
 * @param obj pointer to an object
 * @param en true: enable the drag throw
 */
void edgui_obj_set_drag_throw(edgui_obj_t * obj, bool en)
{
    obj->drag_throw = (en == true ? 1 : 0);
}

/**
 * Enable to use parent for drag related operations.
 * If trying to drag the object the parent will be moved instead
 * @param obj pointer to an object
 * @param en true: enable the 'drag parent' for the object
 */
void edgui_obj_set_drag_parent(edgui_obj_t * obj, bool en)
{
    obj->drag_parent = (en == true ? 1 : 0);
}

/**
 * Propagate the events to the parent too
 * @param obj pointer to an object
 * @param en true: enable the event propagation
 */
void edgui_obj_set_parent_event(edgui_obj_t * obj, bool en)
{
    obj->parent_event = (en == true ? 1 : 0);
}

/**
 * Set the opa scale enable parameter (required to set opa_scale with `edgui_obj_set_opa_scale()`)
 * @param obj pointer to an object
 * @param en true: opa scaling is enabled for this object and all children; false: no opa scaling
 */
void edgui_obj_set_opa_scale_enable(edgui_obj_t * obj, bool en)
{
    obj->opa_scale_en = en ? 1 : 0;
}

/**
 * Set the opa scale of an object.
 * The opacity of this object and all it's children will be scaled down with this factor.
 * `edgui_obj_set_opa_scale_enable(obj, true)` needs to be called to enable it.
 * (not for all children just for the parent where to start the opa scaling)
 * @param obj pointer to an object
 * @param opa_scale a factor to scale down opacity [0..255]
 */
void edgui_obj_set_opa_scale(edgui_obj_t * obj, edgui_opa_t opa_scale)
{
    obj->opa_scale = opa_scale;
    edgui_obj_invalidate(obj);
}

/**
 * Set a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from `edgui_protect_t`
 */
void edgui_obj_set_protect(edgui_obj_t * obj, uint8_t prot)
{
    obj->protect |= prot;
}

/**
 * Clear a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from `edgui_protect_t`
 */
void edgui_obj_clear_protect(edgui_obj_t * obj, uint8_t prot)
{
    prot = (~prot) & 0xFF;
    obj->protect &= prot;
}

/**
 * Set a an event handler function for an object.
 * Used by the user to react on event which happens with the object.
 * @param obj pointer to an object
 * @param event_cb the new event function
 */
void edgui_obj_set_event_cb(edgui_obj_t * obj, edgui_event_cb_t event_cb)
{
    obj->event_cb = event_cb;
}

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `edgui_event_t`
 * @param data arbitrary data depending on the object type and the event. (Usually `NULL`)
 * @return EDGUI_RES_OK: `obj` was not deleted in the event; EDGUI_RES_INV: `obj` was deleted in the event
 */
edgui_res_t edgui_event_send(edgui_obj_t * obj, edgui_event_t event, const void * data)
{
    if(obj == NULL) return EDGUI_RES_OK;

    edgui_res_t res;
    res = edgui_event_send_func(obj->event_cb, obj, event, data);
    return res;
}

/**
 * Call an event function with an object, event, and data.
 * @param event_xcb an event callback function. If `NULL` `EDGUI_RES_OK` will return without any actions.
 *        (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *         the `func_name(object, callback, ...)` convention)
 * @param obj pointer to an object to associate with the event (can be `NULL` to simply call the `event_cb`)
 * @param event an event
 * @param data pointer to a custom data
 * @return EDGUI_RES_OK: `obj` was not deleted in the event; EDGUI_RES_INV: `obj` was deleted in the event
 */
edgui_res_t edgui_event_send_func(edgui_event_cb_t event_xcb, edgui_obj_t * obj, edgui_event_t event, const void * data)
{
    /* Build a simple linked list from the objects used in the events
     * It's important to know if an this object was deleted by a nested event
     * called from this `even_cb`. */
    edgui_event_temp_data_t event_temp_data;
    event_temp_data.obj     = obj;
    event_temp_data.deleted = false;
    event_temp_data.prev    = NULL;

    if(event_temp_data_head) {
        event_temp_data.prev = event_temp_data_head;
    }
    event_temp_data_head = &event_temp_data;

    const void * event_act_data_save = event_act_data;
    event_act_data                   = data;

    /*Call the input device's feedback callback if set*/
    edgui_indev_t * indev_act = edgui_indev_get_act();
    if(indev_act) {
        if(indev_act->driver.feedback_cb) indev_act->driver.feedback_cb(&indev_act->driver, event);
    }

    /*Call the event callback itself*/
    if(event_xcb) event_xcb(obj, event);

    /*Restore the event data*/
    event_act_data = event_act_data_save;

    /*Remove this element from the list*/
    event_temp_data_head = event_temp_data_head->prev;

    if(event_temp_data.deleted) {
        return EDGUI_RES_INV;
    }

    if(obj) {
        if(obj->parent_event && obj->par) {
            edgui_res_t res = edgui_event_send(obj->par, event, data);
            if(res != EDGUI_RES_OK) {
                return EDGUI_RES_INV;
            }
        }
    }

    return EDGUI_RES_OK;
}

/**
 * Get the `data` parameter of the current event
 * @return the `data` parameter
 */
const void * edgui_event_get_data(void)
{
    return event_act_data;
}

/**
 * Set the a signal function of an object. Used internally by the library.
 * Always call the previous signal function in the new.
 * @param obj pointer to an object
 * @param cb the new signal function
 */
void edgui_obj_set_signal_cb(edgui_obj_t * obj, edgui_signal_cb_t signal_cb)
{
    obj->signal_cb = signal_cb;
}

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `edgui_event_t`.
 */
void edgui_signal_send(edgui_obj_t * obj, edgui_signal_t signal, void * param)
{
    if(obj->signal_cb) obj->signal_cb(obj, signal, param);
}

/**
 * Set a new design function for an object
 * @param obj pointer to an object
 * @param design_cb the new design function
 */
void edgui_obj_set_design_cb(edgui_obj_t * obj, edgui_design_cb_t design_cb)
{
    obj->design_cb = design_cb;
}

/*----------------
 * Other set
 *--------------*/

/**
 * Allocate a new ext. data for an object
 * @param obj pointer to an object
 * @param ext_size the size of the new ext. data
 * @return Normal pointer to the allocated ext
 */
void * edgui_obj_allocate_ext_attr(edgui_obj_t * obj, uint16_t ext_size)
{
    obj->ext_attr = edgui_mem_realloc(obj->ext_attr, ext_size);

    return (void *)obj->ext_attr;
}

/**
 * Send a 'EDGUI_SIGNAL_REFR_EXT_SIZE' signal to the object
 * @param obj pointer to an object
 */
void edgui_obj_refresh_ext_draw_pad(edgui_obj_t * obj)
{
    obj->ext_draw_pad = 0;
    obj->signal_cb(obj, EDGUI_SIGNAL_REFR_EXT_DRAW_PAD, NULL);

    edgui_obj_invalidate(obj);
}

/*=======================
 * Getter functions
 *======================*/

/**
 * Return with the screen of an object
 * @param obj pointer to an object
 * @return pointer to a screen
 */
edgui_obj_t * edgui_obj_get_screen(const edgui_obj_t * obj)
{
    const edgui_obj_t * par = obj;
    const edgui_obj_t * act_p;

    do {
        act_p = par;
        par   = edgui_obj_get_parent(act_p);
    } while(par != NULL);

    return (edgui_obj_t *)act_p;
}

/**
 * Get the display of an object
 * @param scr pointer to an object
 * @return pointer the object's display
 */
edgui_disp_t * edgui_obj_get_disp(const edgui_obj_t * obj)
{
    const edgui_obj_t * scr;

    if(obj->par == NULL)
        scr = obj; /*`obj` is a screen*/
    else
        scr = edgui_obj_get_screen(obj); /*get the screen of `obj`*/

    edgui_disp_t * d;
    EDGUI_LL_READ(EDGUI_GC_ROOT(_edgui_disp_ll), d)
    {
        edgui_obj_t * s;
        EDGUI_LL_READ(d->scr_ll, s)
        {
            if(s == scr) return d;
        }
    }

    EDGUI_LOG_WARN("edgui_scr_get_disp: screen not found")
    return NULL;
}

/*---------------------
 * Parent/children get
 *--------------------*/

/**
 * Returns with the parent of an object
 * @param obj pointer to an object
 * @return pointer to the parent of  'obj'
 */
edgui_obj_t * edgui_obj_get_parent(const edgui_obj_t * obj)
{
    return obj->par;
}

/**
 * Iterate through the children of an object (start from the "youngest")
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
edgui_obj_t * edgui_obj_get_child(const edgui_obj_t * obj, const edgui_obj_t * child)
{
    edgui_obj_t * result = NULL;

    if(child == NULL) {
        result = edgui_ll_get_head(&obj->child_ll);
    } else {
        result = edgui_ll_get_next(&obj->child_ll, child);
    }

    return result;
}

/**
 * Iterate through the children of an object (start from the "oldest")
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
edgui_obj_t * edgui_obj_get_child_back(const edgui_obj_t * obj, const edgui_obj_t * child)
{
    edgui_obj_t * result = NULL;

    if(child == NULL) {
        result = edgui_ll_get_tail(&obj->child_ll);
    } else {
        result = edgui_ll_get_prev(&obj->child_ll, child);
    }

    return result;
}

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t edgui_obj_count_children(const edgui_obj_t * obj)
{
    edgui_obj_t * i;
    uint16_t cnt = 0;

    EDGUI_LL_READ(obj->child_ll, i) cnt++;

    return cnt;
}

/** Recursively count the children of an object
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t edgui_obj_count_children_recursive(const edgui_obj_t * obj)
{
    edgui_obj_t * i;
    uint16_t cnt = 0;

    EDGUI_LL_READ(obj->child_ll, i)
    {
        cnt++;                                     /*Count the child*/
        cnt += edgui_obj_count_children_recursive(i); /*recursively count children's children*/
    }

    return cnt;
}

/*---------------------
 * Coordinate get
 *--------------------*/

/**
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param cords_p pointer to an area to store the coordinates
 */
void edgui_obj_get_coords(const edgui_obj_t * obj, edgui_area_t * cords_p)
{
    edgui_area_copy(cords_p, &obj->coords);
}

/**
 * Reduce area retried by `edgui_obj_get_coords()` the get graphically usable area of an object.
 * (Without the size of the border or other extra graphical elements)
 * @param coords_p store the result area here
 */
void edgui_obj_get_inner_coords(const edgui_obj_t * obj, edgui_area_t * coords_p)
{
    const edgui_style_t * style = edgui_obj_get_style(obj);
    if(style->body.border.part & EDGUI_BORDER_LEFT) coords_p->x1 += style->body.border.width;

    if(style->body.border.part & EDGUI_BORDER_RIGHT) coords_p->x2 -= style->body.border.width;

    if(style->body.border.part & EDGUI_BORDER_TOP) coords_p->y1 += style->body.border.width;

    if(style->body.border.part & EDGUI_BORDER_BOTTOM) coords_p->y2 -= style->body.border.width;
}

/**
 * Get the x coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent
 */
edgui_coord_t edgui_obj_get_x(const edgui_obj_t * obj)
{
    edgui_coord_t rel_x;
    edgui_obj_t * parent = edgui_obj_get_parent(obj);
    rel_x             = obj->coords.x1 - parent->coords.x1;

    return rel_x;
}

/**
 * Get the y coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the top of its parent
 */
edgui_coord_t edgui_obj_get_y(const edgui_obj_t * obj)
{
    edgui_coord_t rel_y;
    edgui_obj_t * parent = edgui_obj_get_parent(obj);
    rel_y             = obj->coords.y1 - parent->coords.y1;

    return rel_y;
}

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width
 */
edgui_coord_t edgui_obj_get_width(const edgui_obj_t * obj)
{
    return edgui_area_get_width(&obj->coords);
}

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height
 */
edgui_coord_t edgui_obj_get_height(const edgui_obj_t * obj)
{
    return edgui_area_get_height(&obj->coords);
}

/**
 * Get that width reduced by the left and right padding.
 * @param obj pointer to an object
 * @return the width which still fits into the container
 */
edgui_coord_t edgui_obj_get_width_fit(edgui_obj_t * obj)
{
    const edgui_style_t * style = edgui_obj_get_style(obj);

    return edgui_obj_get_width(obj) - style->body.padding.left - style->body.padding.right;
}

/**
 * Get that height reduced by the top an bottom padding.
 * @param obj pointer to an object
 * @return the height which still fits into the container
 */
edgui_coord_t edgui_obj_get_height_fit(edgui_obj_t * obj)
{
    const edgui_style_t * style = edgui_obj_get_style(obj);

    return edgui_obj_get_height(obj) - style->body.padding.top - style->body.padding.bottom;
}

/**
 * Get the automatic realign property of the object.
 * @param obj pointer to an object
 * @return  true: auto realign is enabled; false: auto realign is disabled
 */
bool edgui_obj_get_auto_realign(edgui_obj_t * obj)
{
#if EDGUI_USE_OBJ_REALIGN
    return obj->realign.auto_realign ? true : false;
#else
    (void)obj;
    return false;
#endif
}

/**
 * Get the left padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended left padding
 */
edgui_coord_t edgui_obj_get_ext_click_pad_left(const edgui_obj_t * obj)
{
#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_TINY
    return obj->ext_click_pad_hor;
#elif EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_FULL
    return obj->ext_click_pad.x1;
#else
    (void)obj;    /*Unused*/
    return 0;
#endif
}

/**
 * Get the right padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended right padding
 */
edgui_coord_t edgui_obj_get_ext_click_pad_right(const edgui_obj_t * obj)
{
#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_TINY
    return obj->ext_click_pad_hor;
#elif EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_FULL
    return obj->ext_click_pad.x2;
#else
    (void)obj; /*Unused*/
    return 0;
#endif
}

/**
 * Get the top padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended top padding
 */
edgui_coord_t edgui_obj_get_ext_click_pad_top(const edgui_obj_t * obj)
{
#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_TINY
    return obj->ext_click_pad_ver;
#elif EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_FULL
    return obj->ext_click_pad.y1;
#else
    (void)obj; /*Unused*/
    return 0;
#endif
}

/**
 * Get the bottom padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended bottom padding
 */
edgui_coord_t edgui_obj_get_ext_click_pad_bottom(const edgui_obj_t * obj)
{
#if EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_TINY
    return obj->ext_click_pad_ver;
#elif EDGUI_USE_EXT_CLICK_AREA == EDGUI_EXT_CLICK_AREA_FULL
    return obj->ext_click_pad.y2;
#else
    (void)obj; /*Unused*/
    return 0;
#endif
}

/**
 * Get the extended size attribute of an object
 * @param obj pointer to an object
 * @return the extended size attribute
 */
edgui_coord_t edgui_obj_get_ext_draw_pad(const edgui_obj_t * obj)
{
    return obj->ext_draw_pad;
}

/*-----------------
 * Appearance get
 *---------------*/

/**
 * Get the style pointer of an object (if NULL get style of the parent)
 * @param obj pointer to an object
 * @return pointer to a style
 */
const edgui_style_t * edgui_obj_get_style(const edgui_obj_t * obj)
{
    const edgui_style_t * style_act = obj->style_p;
    if(style_act == NULL) {
        edgui_obj_t * par = obj->par;

        while(par) {
            if(par->style_p) {
                if(par->style_p->glass == 0) {
#if EDGUI_USE_GROUP == 0
                    style_act = par->style_p;
#else
                    /*If a parent is focused then use then focused style*/
                    edgui_group_t * g = edgui_obj_get_group(par);
                    if(edgui_group_get_focused(g) == par) {
                        style_act = edgui_group_mod_style(g, par->style_p);
                    } else {
                        style_act = par->style_p;
                    }
#endif
                    break;
                }
            }
            par = par->par;
        }
    }
#if EDGUI_USE_GROUP
    if(obj->group_p) {
        if(edgui_group_get_focused(obj->group_p) == obj) {
            style_act = edgui_group_mod_style(obj->group_p, style_act);
        }
    }
#endif

    if(style_act == NULL) style_act = &edgui_style_plain;

    return style_act;
}

/*-----------------
 * Attribute get
 *----------------*/

/**
 * Get the hidden attribute of an object
 * @param obj pointer to an object
 * @return true: the object is hidden
 */
bool edgui_obj_get_hidden(const edgui_obj_t * obj)
{
    return obj->hidden == 0 ? false : true;
}

/**
 * Get the click enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is clickable
 */
bool edgui_obj_get_click(const edgui_obj_t * obj)
{
    return obj->click == 0 ? false : true;
}

/**
 * Get the top enable attribute of an object
 * @param obj pointer to an object
 * @return true: the auto top feture is enabled
 */
bool edgui_obj_get_top(const edgui_obj_t * obj)
{
    return obj->top == 0 ? false : true;
}

/**
 * Get the drag enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is dragable
 */
bool edgui_obj_get_drag(const edgui_obj_t * obj)
{
    return obj->drag == 0 ? false : true;
}

/**
 * Get the directions an object can be dragged
 * @param obj pointer to an object
 * @return bitwise OR of allowed directions an object can be dragged in
 */
edgui_drag_dir_t edgui_obj_get_drag_dir(const edgui_obj_t * obj)
{
    return obj->drag_dir;
}

/**
 * Get the drag throw enable attribute of an object
 * @param obj pointer to an object
 * @return true: drag throw is enabled
 */
bool edgui_obj_get_drag_throw(const edgui_obj_t * obj)
{
    return obj->drag_throw == 0 ? false : true;
}

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool edgui_obj_get_drag_parent(const edgui_obj_t * obj)
{
    return obj->drag_parent == 0 ? false : true;
}

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool edgui_obj_get_parent_event(const edgui_obj_t * obj)
{
    return obj->parent_event == 0 ? false : true;
}

/**
 * Get the opa scale enable parameter
 * @param obj pointer to an object
 * @return true: opa scaling is enabled for this object and all children; false: no opa scaling
 */
edgui_opa_t edgui_obj_get_opa_scale_enable(const edgui_obj_t * obj)
{
    return obj->opa_scale_en == 0 ? false : true;
}

/**
 * Get the opa scale parameter of an object
 * @param obj pointer to an object
 * @return opa scale [0..255]
 */
edgui_opa_t edgui_obj_get_opa_scale(const edgui_obj_t * obj)
{
    const edgui_obj_t * parent = obj;

    while(parent) {
        if(parent->opa_scale_en) return parent->opa_scale;
        parent = edgui_obj_get_parent(parent);
    }

    return EDGUI_OPA_COVER;
}

/**
 * Get the protect field of an object
 * @param obj pointer to an object
 * @return protect field ('OR'ed values of `edgui_protect_t`)
 */
uint8_t edgui_obj_get_protect(const edgui_obj_t * obj)
{
    return obj->protect;
}

/**
 * Check at least one bit of a given protect bitfield is set
 * @param obj pointer to an object
 * @param prot protect bits to test ('OR'ed values of `edgui_protect_t`)
 * @return false: none of the given bits are set, true: at least one bit is set
 */
bool edgui_obj_is_protected(const edgui_obj_t * obj, uint8_t prot)
{
    return (obj->protect & prot) == 0 ? false : true;
}

/**
 * Get the signal function of an object
 * @param obj pointer to an object
 * @return the signal function
 */
edgui_signal_cb_t edgui_obj_get_signal_cb(const edgui_obj_t * obj)
{
    return obj->signal_cb;
}

/**
 * Get the design function of an object
 * @param obj pointer to an object
 * @return the design function
 */
edgui_design_cb_t edgui_obj_get_design_cb(const edgui_obj_t * obj)
{
    return obj->design_cb;
}

/**
 * Get the event function of an object
 * @param obj pointer to an object
 * @return the event function
 */
edgui_event_cb_t edgui_obj_get_event_cb(const edgui_obj_t * obj)
{
    return obj->event_cb;
}

/*------------------
 * Other get
 *-----------------*/

/**
 * Get the ext pointer
 * @param obj pointer to an object
 * @return the ext pointer but not the dynamic version
 *         Use it as ext->data1, and NOT da(ext)->data1
 */
void * edgui_obj_get_ext_attr(const edgui_obj_t * obj)
{
    return obj->ext_attr;
}

/**
 * Get object's and its ancestors type. Put their name in `type_buf` starting with the current type.
 * E.g. buf.type[0]="edgui_btn", buf.type[1]="edgui_cont", buf.type[2]="edgui_obj"
 * @param obj pointer to an object which type should be get
 * @param buf pointer to an `edgui_obj_type_t` buffer to store the types
 */
void edgui_obj_get_type(edgui_obj_t * obj, edgui_obj_type_t * buf)
{
    edgui_obj_type_t tmp;

    memset(buf, 0, sizeof(edgui_obj_type_t));
    memset(&tmp, 0, sizeof(edgui_obj_type_t));

    obj->signal_cb(obj, EDGUI_SIGNAL_GET_TYPE, &tmp);

    uint8_t cnt;
    for(cnt = 0; cnt < EDGUI_MAX_ANCESTOR_NUM; cnt++) {
        if(tmp.type[cnt] == NULL) break;
    }

    /*Swap the order. The real type comes first*/
    uint8_t i;
    for(i = 0; i < cnt; i++) {
        buf->type[i] = tmp.type[cnt - 1 - i];
    }
}

#if EDGUI_USE_USER_DATA

/**
 * Get the object's user data
 * @param obj pointer to an object
 * @return user data
 */
edgui_obj_user_data_t edgui_obj_get_user_data(edgui_obj_t * obj)
{
    return obj->user_data;
}

/**
 * Get a pointer to the object's user data
 * @param obj pointer to an object
 * @return pointer to the user data
 */
edgui_obj_user_data_t * edgui_obj_get_user_data_ptr(edgui_obj_t * obj)
{
    return &obj->user_data;
}

/**
 * Set the object's user data. The data will be copied.
 * @param obj pointer to an object
 * @param data user data
 */
void edgui_obj_set_user_data(edgui_obj_t * obj, edgui_obj_user_data_t data)
{
    memcpy(&obj->user_data, &data, sizeof(edgui_obj_user_data_t));
}
#endif

#if EDGUI_USE_GROUP
/**
 * Get the group of the object
 * @param obj pointer to an object
 * @return the pointer to group of the object
 */
void * edgui_obj_get_group(const edgui_obj_t * obj)
{
    return obj->group_p;
}

/**
 * Tell whether the object is the focused object of a group or not.
 * @param obj pointer to an object
 * @return true: the object is focused, false: the object is not focused or not in a group
 */
bool edgui_obj_is_focused(const edgui_obj_t * obj)
{
    if(obj->group_p) {
        if(edgui_group_get_focused(obj->group_p) == obj) return true;
    }

    return false;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void edgui_obj_del_async_cb(void * obj)
{
    edgui_obj_del(obj);
}

/**
 * Handle the drawing related tasks of the base objects.
 * @param obj pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool edgui_obj_design(edgui_obj_t * obj, const edgui_area_t * mask_p, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {

        /*Most trivial test. Is the mask fully IN the object? If no it surely doesn't cover it*/
        if(edgui_area_is_in(mask_p, &obj->coords) == false) return false;

        /*Can cover the area only if fully solid (no opacity)*/
        const edgui_style_t * style = edgui_obj_get_style(obj);
        if(style->body.opa < EDGUI_OPA_MAX) return false;

        /* Because of the radius it is not sure the area is covered
         * Check the areas where there is no radius*/
        edgui_coord_t r = style->body.radius;

        if(r == EDGUI_RADIUS_CIRCLE) return false;

        edgui_area_t area_tmp;

        /*Check horizontally without radius*/
        edgui_obj_get_coords(obj, &area_tmp);
        area_tmp.x1 += r;
        area_tmp.x2 -= r;
        if(edgui_area_is_in(mask_p, &area_tmp) == false) return false;

        /*Check vertically without radius*/
        edgui_obj_get_coords(obj, &area_tmp);
        area_tmp.y1 += r;
        area_tmp.y2 -= r;
        if(edgui_area_is_in(mask_p, &area_tmp) == false) return false;

    } else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        const edgui_style_t * style = edgui_obj_get_style(obj);
        edgui_draw_rect(&obj->coords, mask_p, style, edgui_obj_get_opa_scale(obj));
    }

    return true;
}

/**
 * Signal function of the basic object
 * @param obj pointer to an object
 * @param sign signal type
 * @param param parameter for the signal (depends on signal type)
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_obj_signal(edgui_obj_t * obj, edgui_signal_t sign, void * param)
{
    (void)param;

    edgui_res_t res = EDGUI_RES_OK;

    const edgui_style_t * style = edgui_obj_get_style(obj);

    if(sign == EDGUI_SIGNAL_CHILD_CHG) {
        /*Return 'invalid' if the child change signal is not enabled*/
        if(edgui_obj_is_protected(obj, EDGUI_PROTECT_CHILD_CHG) != false) res = EDGUI_RES_INV;
    } else if(sign == EDGUI_SIGNAL_REFR_EXT_DRAW_PAD) {
        if(style->body.shadow.width > obj->ext_draw_pad) obj->ext_draw_pad = style->body.shadow.width;
    } else if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        edgui_obj_refresh_ext_draw_pad(obj);
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        buf->type[0]        = "edgui_obj";
    }

    return res;
}

/**
 * Reposition the children of an object. (Called recursively)
 * @param obj pointer to an object which children will be repositioned
 * @param x_diff x coordinate shift
 * @param y_diff y coordinate shift
 */
static void refresh_children_position(edgui_obj_t * obj, edgui_coord_t x_diff, edgui_coord_t y_diff)
{
    edgui_obj_t * i;
    EDGUI_LL_READ(obj->child_ll, i)
    {
        i->coords.x1 += x_diff;
        i->coords.y1 += y_diff;
        i->coords.x2 += x_diff;
        i->coords.y2 += y_diff;

        refresh_children_position(i, x_diff, y_diff);
    }
}

/**
 * Refresh the style of all children of an object. (Called recursively)
 * @param style_p refresh objects only with this style.
 * @param obj pointer to an object
 */
static void report_style_mod_core(void * style_p, edgui_obj_t * obj)
{
    edgui_obj_t * i;
    EDGUI_LL_READ(obj->child_ll, i)
    {
        if(i->style_p == style_p || style_p == NULL) {
            refresh_children_style(i);
            edgui_obj_refresh_style(i);
        }

        report_style_mod_core(style_p, i);
    }
}

/**
 * Recursively refresh the style of the children. Go deeper until a not NULL style is found
 * because the NULL styles are inherited from the parent
 * @param obj pointer to an object
 */
static void refresh_children_style(edgui_obj_t * obj)
{
    edgui_obj_t * child = edgui_obj_get_child(obj, NULL);
    while(child != NULL) {
        if(child->style_p == NULL) {
            refresh_children_style(child); /*Check children too*/
            edgui_obj_refresh_style(child);   /*Notify the child about the style change*/
        } else if(child->style_p->glass) {
            /*Children with 'glass' parent might be effected if their style == NULL*/
            refresh_children_style(child);
        }
        child = edgui_obj_get_child(obj, child);
    }
}

/**
 * Called by 'edgui_obj_del' to delete the children objects
 * @param obj pointer to an object (all of its children will be deleted)
 */
static void delete_children(edgui_obj_t * obj)
{
    edgui_obj_t * i;
    edgui_obj_t * i_next;
    i = edgui_ll_get_head(&(obj->child_ll));

    /*Remove from the group; remove before transversing children so that
     * the object still has access to all children during the
     * EDGUI_SIGNAL_DEFOCUS call*/
#if EDGUI_USE_GROUP
    edgui_group_t * group = edgui_obj_get_group(obj);
    if(group) edgui_group_remove_obj(obj);
#endif

    while(i != NULL) {
        /*Get the next object before delete this*/
        i_next = edgui_ll_get_next(&(obj->child_ll), i);

        /*Call the recursive del to the child too*/
        delete_children(i);

        /*Set i to the next node*/
        i = i_next;
    }

    /*Let the suer free the resources used in `EDGUI_EVENT_DELETE`*/
    edgui_event_send(obj, EDGUI_EVENT_DELETE, NULL);

    edgui_event_mark_deleted(obj);

    /*Remove the animations from this object*/
#if EDGUI_USE_ANIMATION
    edgui_anim_del(obj, NULL);
#endif

    /* Reset the input devices if
     * the object to delete is used*/
    edgui_indev_t * indev = edgui_indev_get_next(NULL);
    while(indev) {
        if(indev->proc.types.pointer.act_obj == obj || indev->proc.types.pointer.last_obj == obj) {
            edgui_indev_reset(indev);
        }

        if(indev->proc.types.pointer.last_pressed == obj) {
            indev->proc.types.pointer.last_pressed = NULL;
        }
#if EDGUI_USE_GROUP
        if(indev->group == group && obj == edgui_indev_get_obj_act()) {
            edgui_indev_reset(indev);
        }
#endif
        indev = edgui_indev_get_next(indev);
    }

    /*Remove the object from parent's children list*/
    edgui_obj_t * par = edgui_obj_get_parent(obj);
    edgui_ll_rem(&(par->child_ll), obj);

    /* Clean up the object specific data*/
    obj->signal_cb(obj, EDGUI_SIGNAL_CLEANUP, NULL);

    /*Delete the base objects*/
    if(obj->ext_attr != NULL) edgui_mem_free(obj->ext_attr);
    edgui_mem_free(obj); /*Free the object itself*/
}

static void edgui_event_mark_deleted(edgui_obj_t * obj)
{
    edgui_event_temp_data_t * t = event_temp_data_head;

    while(t) {
        if(t->obj == obj) t->deleted = true;
        t = t->prev;
    }
}
