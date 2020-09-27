/**
 * @file edgui_cont.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "edgui_cont.h"
#if EDGUI_USE_CONT != 0

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../edgui_draw/edgui_draw.h"
#include "../edgui_draw/edgui_draw_basic.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_area.h"
#include "../edgui_misc/edgui_color.h"
#include "../edgui_misc/edgui_math.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_cont_signal(edgui_obj_t * cont, edgui_signal_t sign, void * param);
static void edgui_cont_refr_layout(edgui_obj_t * cont);
static void edgui_cont_layout_col(edgui_obj_t * cont);
static void edgui_cont_layout_row(edgui_obj_t * cont);
static void edgui_cont_layout_center(edgui_obj_t * cont);
static void edgui_cont_layout_pretty(edgui_obj_t * cont);
static void edgui_cont_layout_grid(edgui_obj_t * cont);
static void edgui_cont_refr_autofit(edgui_obj_t * cont);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a container objects
 * @param par pointer to an object, it will be the parent of the new container
 * @param copy pointer to a container object, if not NULL then the new object will be copied from it
 * @return pointer to the created container
 */
edgui_obj_t * edgui_cont_create(edgui_obj_t * par, const edgui_obj_t * copy)
{

    EDGUI_LOG_TRACE("container create started");

    /*Create a basic object*/
    edgui_obj_t * new_cont = edgui_obj_create(par, copy);
    edgui_mem_assert(new_cont);
    if(new_cont == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_cont);

    edgui_obj_allocate_ext_attr(new_cont, sizeof(edgui_cont_ext_t));
    edgui_cont_ext_t * ext = edgui_obj_get_ext_attr(new_cont);
    if(ext == NULL) return NULL;

    edgui_mem_assert(ext);
    ext->fit_left   = EDGUI_FIT_NONE;
    ext->fit_right  = EDGUI_FIT_NONE;
    ext->fit_top    = EDGUI_FIT_NONE;
    ext->fit_bottom = EDGUI_FIT_NONE;
    ext->layout     = EDGUI_LAYOUT_OFF;

    edgui_obj_set_signal_cb(new_cont, edgui_cont_signal);

    /*Init the new container*/
    if(copy == NULL) {
        /*Set the default styles if it's not screen*/
        if(par != NULL) {
            edgui_theme_t * th = edgui_theme_get_current();
            if(th) {
                edgui_cont_set_style(new_cont, EDGUI_CONT_STYLE_MAIN, th->style.cont);
            } else {
                edgui_cont_set_style(new_cont, EDGUI_CONT_STYLE_MAIN, &edgui_style_pretty);
            }
        }
    }
    /*Copy an existing object*/
    else {
        edgui_cont_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->fit_left            = copy_ext->fit_left;
        ext->fit_right           = copy_ext->fit_right;
        ext->fit_top             = copy_ext->fit_top;
        ext->fit_bottom          = copy_ext->fit_bottom;
        ext->layout              = copy_ext->layout;

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_cont);
    }

    EDGUI_LOG_INFO("container created");

    return new_cont;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a layout on a container
 * @param cont pointer to a container object
 * @param layout a layout from 'edgui_cont_layout_t'
 */
void edgui_cont_set_layout(edgui_obj_t * cont, edgui_layout_t layout)
{
    edgui_cont_ext_t * ext = edgui_obj_get_ext_attr(cont);
    if(ext->layout == layout) return;

    ext->layout = layout;

    /*Send a signal to refresh the layout*/
    cont->signal_cb(cont, EDGUI_SIGNAL_CHILD_CHG, NULL);
}

/**
 * Set the fit policy in all 4 directions separately.
 * It tell how to change the container's size automatically.
 * @param cont pointer to a container object
 * @param left left fit policy from `edgui_fit_t`
 * @param right right fit policy from `edgui_fit_t`
 * @param top bottom fit policy from `edgui_fit_t`
 * @param bottom bottom fit policy from `edgui_fit_t`
 */
void edgui_cont_set_fit4(edgui_obj_t * cont, edgui_fit_t left, edgui_fit_t right, edgui_fit_t top, edgui_fit_t bottom)
{
    edgui_obj_invalidate(cont);
    edgui_cont_ext_t * ext = edgui_obj_get_ext_attr(cont);
    if(ext->fit_left == left && ext->fit_right == right && ext->fit_top == top && ext->fit_bottom == bottom) {
        return;
    }

    ext->fit_left   = left;
    ext->fit_right  = right;
    ext->fit_top    = top;
    ext->fit_bottom = bottom;

    /*Send a signal to refresh the layout*/
    cont->signal_cb(cont, EDGUI_SIGNAL_CHILD_CHG, NULL);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the layout of a container
 * @param cont pointer to container object
 * @return the layout from 'edgui_cont_layout_t'
 */
edgui_layout_t edgui_cont_get_layout(const edgui_obj_t * cont)
{
    edgui_cont_ext_t * ext = edgui_obj_get_ext_attr(cont);
    return ext->layout;
}

/**
 * Get left fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `edgui_fit_t`
 */
edgui_fit_t edgui_cont_get_fit_left(const edgui_obj_t * cont)
{
    edgui_cont_ext_t * ext = edgui_obj_get_ext_attr(cont);
    return ext->fit_left;
}

/**
 * Get right fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `edgui_fit_t`
 */
edgui_fit_t edgui_cont_get_fit_right(const edgui_obj_t * cont)
{
    edgui_cont_ext_t * ext = edgui_obj_get_ext_attr(cont);
    return ext->fit_right;
}

/**
 * Get top fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `edgui_fit_t`
 */
edgui_fit_t edgui_cont_get_fit_top(const edgui_obj_t * cont)
{
    edgui_cont_ext_t * ext = edgui_obj_get_ext_attr(cont);
    return ext->fit_top;
}

/**
 * Get bottom fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `edgui_fit_t`
 */
edgui_fit_t edgui_cont_get_fit_bottom(const edgui_obj_t * cont)
{
    edgui_cont_ext_t * ext = edgui_obj_get_ext_attr(cont);
    return ext->fit_bottom;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the container
 * @param cont pointer to a container object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_cont_signal(edgui_obj_t * cont, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(cont, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_STYLE_CHG) { /*Recalculate the padding if the style changed*/
        edgui_cont_refr_layout(cont);
        edgui_cont_refr_autofit(cont);
    } else if(sign == EDGUI_SIGNAL_CHILD_CHG) {
        edgui_cont_refr_layout(cont);
        edgui_cont_refr_autofit(cont);
    } else if(sign == EDGUI_SIGNAL_CORD_CHG) {
        if(edgui_obj_get_width(cont) != edgui_area_get_width(param) || edgui_obj_get_height(cont) != edgui_area_get_height(param)) {
            edgui_cont_refr_layout(cont);
            edgui_cont_refr_autofit(cont);
        }
    } else if(sign == EDGUI_SIGNAL_PARENT_SIZE_CHG) {
        /*FLOOD and FILL fit needs to be refreshed if the parent size has changed*/
        edgui_cont_refr_autofit(cont);

    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_cont";
    }

    return res;
}

/**
 * Refresh the layout of a container
 * @param cont pointer to an object which layout should be refreshed
 */
static void edgui_cont_refr_layout(edgui_obj_t * cont)
{
    edgui_layout_t type = edgui_cont_get_layout(cont);

    /*'cont' has to be at least 1 child*/
    if(edgui_obj_get_child(cont, NULL) == NULL) return;

    if(type == EDGUI_LAYOUT_OFF) return;

    if(type == EDGUI_LAYOUT_CENTER) {
        edgui_cont_layout_center(cont);
    } else if(type == EDGUI_LAYOUT_COL_L || type == EDGUI_LAYOUT_COL_M || type == EDGUI_LAYOUT_COL_R) {
        edgui_cont_layout_col(cont);
    } else if(type == EDGUI_LAYOUT_ROW_T || type == EDGUI_LAYOUT_ROW_M || type == EDGUI_LAYOUT_ROW_B) {
        edgui_cont_layout_row(cont);
    } else if(type == EDGUI_LAYOUT_PRETTY) {
        edgui_cont_layout_pretty(cont);
    } else if(type == EDGUI_LAYOUT_GRID) {
        edgui_cont_layout_grid(cont);
    }
}

/**
 * Handle column type layouts
 * @param cont pointer to an object which layout should be handled
 */
static void edgui_cont_layout_col(edgui_obj_t * cont)
{
    edgui_layout_t type = edgui_cont_get_layout(cont);
    edgui_obj_t * child;

    /*Adjust margin and get the alignment type*/
    edgui_align_t align;
    const edgui_style_t * style = edgui_obj_get_style(cont);
    edgui_coord_t hpad_corr;

    switch(type) {
        case EDGUI_LAYOUT_COL_L:
            hpad_corr = style->body.padding.left;
            align     = EDGUI_ALIGN_IN_TOP_LEFT;
            break;
        case EDGUI_LAYOUT_COL_M:
            hpad_corr = 0;
            align     = EDGUI_ALIGN_IN_TOP_MID;
            break;
        case EDGUI_LAYOUT_COL_R:
            hpad_corr = -style->body.padding.right;
            align     = EDGUI_ALIGN_IN_TOP_RIGHT;
            break;
        default:
            hpad_corr = 0;
            align     = EDGUI_ALIGN_IN_TOP_LEFT;
            break;
    }

    /* Disable child change action because the children will be moved a lot
     * an unnecessary child change signals could be sent*/
    edgui_obj_set_protect(cont, EDGUI_PROTECT_CHILD_CHG);
    /* Align the children */
    edgui_coord_t last_cord = style->body.padding.top;
    EDGUI_LL_READ_BACK(cont->child_ll, child)
    {
        if(edgui_obj_get_hidden(child) != false || edgui_obj_is_protected(child, EDGUI_PROTECT_POS) != false) continue;

        edgui_obj_align(child, cont, align, hpad_corr, last_cord);
        last_cord += edgui_obj_get_height(child) + style->body.padding.inner;
    }

    edgui_obj_clear_protect(cont, EDGUI_PROTECT_CHILD_CHG);
}

/**
 * Handle row type layouts
 * @param cont pointer to an object which layout should be handled
 */
static void edgui_cont_layout_row(edgui_obj_t * cont)
{
    edgui_layout_t type = edgui_cont_get_layout(cont);
    edgui_obj_t * child;

    /*Adjust margin and get the alignment type*/
    edgui_align_t align;
    const edgui_style_t * style = edgui_obj_get_style(cont);
    edgui_coord_t vpad_corr;

    switch(type) {
        case EDGUI_LAYOUT_ROW_T:
            vpad_corr = style->body.padding.top;
            align     = EDGUI_ALIGN_IN_TOP_LEFT;
            break;
        case EDGUI_LAYOUT_ROW_M:
            vpad_corr = 0;
            align     = EDGUI_ALIGN_IN_LEFT_MID;
            break;
        case EDGUI_LAYOUT_ROW_B:
            vpad_corr = -style->body.padding.bottom;
            align     = EDGUI_ALIGN_IN_BOTTOM_LEFT;
            break;
        default:
            vpad_corr = 0;
            align     = EDGUI_ALIGN_IN_TOP_LEFT;
            break;
    }

    /* Disable child change action because the children will be moved a lot
     * an unnecessary child change signals could be sent*/
    edgui_obj_set_protect(cont, EDGUI_PROTECT_CHILD_CHG);

    /* Align the children */
    edgui_coord_t last_cord = style->body.padding.left;
    EDGUI_LL_READ_BACK(cont->child_ll, child)
    {
        if(edgui_obj_get_hidden(child) != false || edgui_obj_is_protected(child, EDGUI_PROTECT_POS) != false) continue;

        edgui_obj_align(child, cont, align, last_cord, vpad_corr);
        last_cord += edgui_obj_get_width(child) + style->body.padding.inner;
    }

    edgui_obj_clear_protect(cont, EDGUI_PROTECT_CHILD_CHG);
}

/**
 * Handle the center layout
 * @param cont pointer to an object which layout should be handled
 */
static void edgui_cont_layout_center(edgui_obj_t * cont)
{
    edgui_obj_t * child;
    const edgui_style_t * style = edgui_obj_get_style(cont);
    uint32_t obj_num         = 0;
    edgui_coord_t h_tot         = 0;

    EDGUI_LL_READ(cont->child_ll, child)
    {
        if(edgui_obj_get_hidden(child) != false || edgui_obj_is_protected(child, EDGUI_PROTECT_POS) != false) continue;
        h_tot += edgui_obj_get_height(child) + style->body.padding.inner;
        obj_num++;
    }

    if(obj_num == 0) return;

    h_tot -= style->body.padding.inner;

    /* Disable child change action because the children will be moved a lot
     * an unnecessary child change signals could be sent*/
    edgui_obj_set_protect(cont, EDGUI_PROTECT_CHILD_CHG);

    /* Align the children */
    edgui_coord_t last_cord = -(h_tot / 2);
    EDGUI_LL_READ_BACK(cont->child_ll, child)
    {
        if(edgui_obj_get_hidden(child) != false || edgui_obj_is_protected(child, EDGUI_PROTECT_POS) != false) continue;

        edgui_obj_align(child, cont, EDGUI_ALIGN_CENTER, 0, last_cord + edgui_obj_get_height(child) / 2);
        last_cord += edgui_obj_get_height(child) + style->body.padding.inner;
    }

    edgui_obj_clear_protect(cont, EDGUI_PROTECT_CHILD_CHG);
}

/**
 * Handle the pretty layout. Put as many object as possible in row
 * then begin a new row
 * @param cont pointer to an object which layout should be handled
 */
static void edgui_cont_layout_pretty(edgui_obj_t * cont)
{
    edgui_obj_t * child_rs;  /* Row starter child */
    edgui_obj_t * child_rc;  /* Row closer child */
    edgui_obj_t * child_tmp; /* Temporary child */
    const edgui_style_t * style = edgui_obj_get_style(cont);
    edgui_coord_t w_obj         = edgui_obj_get_width(cont);
    edgui_coord_t act_y         = style->body.padding.top;
    /* Disable child change action because the children will be moved a lot
     * an unnecessary child change signals could be sent*/

    child_rs = edgui_ll_get_tail(&cont->child_ll); /*Set the row starter child*/
    if(child_rs == NULL) return;                /*Return if no child*/

    edgui_obj_set_protect(cont, EDGUI_PROTECT_CHILD_CHG);

    child_rc = child_rs; /*Initially the the row starter and closer is the same*/
    while(child_rs != NULL) {
        edgui_coord_t h_row = 0;
        edgui_coord_t w_row =
            style->body.padding.left + style->body.padding.right; /*The width is at least the left+right hpad*/
        uint32_t obj_num = 0;

        /*Find the row closer object and collect some data*/
        do {
            if(edgui_obj_get_hidden(child_rc) == false && edgui_obj_is_protected(child_rc, EDGUI_PROTECT_POS) == false) {
                /*If this object is already not fit then break*/
                if(w_row + edgui_obj_get_width(child_rc) > w_obj) {
                    /*Step back one child because the last already not fit, so the previous is the
                     * closer*/
                    if(child_rc != NULL && obj_num != 0) {
                        child_rc = edgui_ll_get_next(&cont->child_ll, child_rc);
                    }
                    break;
                }
                w_row += edgui_obj_get_width(child_rc) + style->body.padding.inner; /*Add the object width + opad*/
                h_row = EDGUI_MATH_MAX(h_row, edgui_obj_get_height(child_rc));         /*Search the highest object*/
                obj_num++;
                if(edgui_obj_is_protected(child_rc, EDGUI_PROTECT_FOLLOW))
                    break; /*If can not be followed by an other object then break here*/
            }
            child_rc = edgui_ll_get_prev(&cont->child_ll, child_rc); /*Load the next object*/
            if(obj_num == 0)
                child_rs = child_rc; /*If the first object was hidden (or too long) then set the
                                        next as first */
        } while(child_rc != NULL);

        /*If the object is too long  then align it to the middle*/
        if(obj_num == 0) {
            if(child_rc != NULL) {
                edgui_obj_align(child_rc, cont, EDGUI_ALIGN_IN_TOP_MID, 0, act_y);
                h_row = edgui_obj_get_height(child_rc); /*Not set previously because of the early break*/
            }
        }
        /*If there is only one object in the row then align it to the middle*/
        else if(obj_num == 1) {
            edgui_obj_align(child_rs, cont, EDGUI_ALIGN_IN_TOP_MID, 0, act_y);
        }
        /*If there are two object in the row then align them proportionally*/
        else if(obj_num == 2) {
            edgui_obj_t * obj1 = child_rs;
            edgui_obj_t * obj2 = edgui_ll_get_prev(&cont->child_ll, child_rs);
            w_row           = edgui_obj_get_width(obj1) + edgui_obj_get_width(obj2);
            edgui_coord_t pad  = (w_obj - w_row) / 3;
            edgui_obj_align(obj1, cont, EDGUI_ALIGN_IN_TOP_LEFT, pad, act_y + (h_row - edgui_obj_get_height(obj1)) / 2);
            edgui_obj_align(obj2, cont, EDGUI_ALIGN_IN_TOP_RIGHT, -pad, act_y + (h_row - edgui_obj_get_height(obj2)) / 2);
        }
        /* Align the children (from child_rs to child_rc)*/
        else {
            w_row -= style->body.padding.inner * obj_num;
            edgui_coord_t new_opad = (w_obj - w_row) / (obj_num - 1);
            edgui_coord_t act_x    = style->body.padding.left; /*x init*/
            child_tmp           = child_rs;
            while(child_tmp != NULL) {
                if(edgui_obj_get_hidden(child_tmp) == false && edgui_obj_is_protected(child_tmp, EDGUI_PROTECT_POS) == false) {
                    edgui_obj_align(child_tmp, cont, EDGUI_ALIGN_IN_TOP_LEFT, act_x,
                                 act_y + (h_row - edgui_obj_get_height(child_tmp)) / 2);
                    act_x += edgui_obj_get_width(child_tmp) + new_opad;
                }
                if(child_tmp == child_rc) break;
                child_tmp = edgui_ll_get_prev(&cont->child_ll, child_tmp);
            }
        }

        if(child_rc == NULL) break;
        act_y += style->body.padding.inner + h_row;           /*y increment*/
        child_rs = edgui_ll_get_prev(&cont->child_ll, child_rc); /*Go to the next object*/
        child_rc = child_rs;
    }
    edgui_obj_clear_protect(cont, EDGUI_PROTECT_CHILD_CHG);
}

/**
 * Handle the grid layout. Align same-sized objects in a grid
 * @param cont pointer to an object which layout should be handled
 */
static void edgui_cont_layout_grid(edgui_obj_t * cont)
{
    edgui_obj_t * child;
    const edgui_style_t * style = edgui_obj_get_style(cont);
    edgui_coord_t w_tot         = edgui_obj_get_width(cont);
    edgui_coord_t w_obj         = edgui_obj_get_width(edgui_obj_get_child(cont, NULL));
    edgui_coord_t w_fit         =  edgui_obj_get_width_fit(cont);
    edgui_coord_t h_obj         = edgui_obj_get_height(edgui_obj_get_child(cont, NULL));
    uint16_t obj_row         = (w_fit) / (w_obj + style->body.padding.inner); /*Obj. num. in a row*/
    edgui_coord_t x_ofs;
    if(obj_row > 1) {
        x_ofs = w_obj + (w_fit - (obj_row * w_obj)) / (obj_row - 1);
    } else {
        x_ofs = w_tot / 2 - w_obj / 2;
    }
    edgui_coord_t y_ofs = h_obj + style->body.padding.inner;

    /* Disable child change action because the children will be moved a lot
     * an unnecessary child change signals could be sent*/
    edgui_obj_set_protect(cont, EDGUI_PROTECT_CHILD_CHG);

    /* Align the children */
    edgui_coord_t act_x = style->body.padding.left;
    edgui_coord_t act_y = style->body.padding.top;
    uint16_t obj_cnt = 0;
    EDGUI_LL_READ_BACK(cont->child_ll, child)
    {
        if(edgui_obj_get_hidden(child) != false || edgui_obj_is_protected(child, EDGUI_PROTECT_POS) != false) continue;

        if(obj_row > 1) {
            edgui_obj_set_pos(child, act_x, act_y);
            act_x += x_ofs;
        } else {
            edgui_obj_set_pos(child, x_ofs, act_y);
        }
        obj_cnt++;

        if(obj_cnt >= obj_row) {
            obj_cnt = 0;
            act_x   = style->body.padding.left;
            act_y += y_ofs;
        }
    }

    edgui_obj_clear_protect(cont, EDGUI_PROTECT_CHILD_CHG);
}

/**
 * Handle auto fit. Set the size of the object to involve all children.
 * @param cont pointer to an object which size will be modified
 */
static void edgui_cont_refr_autofit(edgui_obj_t * cont)
{
    edgui_cont_ext_t * ext = edgui_obj_get_ext_attr(cont);

    if(ext->fit_left == EDGUI_FIT_NONE && ext->fit_right == EDGUI_FIT_NONE && ext->fit_top == EDGUI_FIT_NONE &&
       ext->fit_bottom == EDGUI_FIT_NONE) {
        return;
    }

    edgui_area_t tight_area;
    edgui_area_t ori;
    const edgui_style_t * style = edgui_obj_get_style(cont);
    edgui_obj_t * child_i;

    edgui_obj_t * par               = edgui_obj_get_parent(cont);
    const edgui_style_t * par_style = edgui_obj_get_style(par);
    edgui_area_t flood_area;
    edgui_area_copy(&flood_area, &par->coords);
    flood_area.x1 += par_style->body.padding.left;
    flood_area.x2 -= par_style->body.padding.right;
    flood_area.y1 += par_style->body.padding.top;
    flood_area.y2 -= par_style->body.padding.bottom;

    /*Search the side coordinates of the children*/
    edgui_obj_get_coords(cont, &ori);
    edgui_obj_get_coords(cont, &tight_area);

    bool has_children = edgui_ll_is_empty(&cont->child_ll) ? false : true;

    if(has_children) {
        tight_area.x1 = EDGUI_COORD_MAX;
        tight_area.y1 = EDGUI_COORD_MAX;
        tight_area.x2 = EDGUI_COORD_MIN;
        tight_area.y2 = EDGUI_COORD_MIN;

        EDGUI_LL_READ(cont->child_ll, child_i)
        {
            if(edgui_obj_get_hidden(child_i) != false) continue;
            tight_area.x1 = EDGUI_MATH_MIN(tight_area.x1, child_i->coords.x1);
            tight_area.y1 = EDGUI_MATH_MIN(tight_area.y1, child_i->coords.y1);
            tight_area.x2 = EDGUI_MATH_MAX(tight_area.x2, child_i->coords.x2);
            tight_area.y2 = EDGUI_MATH_MAX(tight_area.y2, child_i->coords.y2);
        }

        tight_area.x1 -= style->body.padding.left;
        tight_area.x2 += style->body.padding.right;
        tight_area.y1 -= style->body.padding.top;
        tight_area.y2 += style->body.padding.bottom;
    }

    edgui_area_t new_area;
    edgui_area_copy(&new_area, &ori);

    switch(ext->fit_left) {
        case EDGUI_FIT_TIGHT: new_area.x1 = tight_area.x1; break;
        case EDGUI_FIT_FLOOD: new_area.x1 = flood_area.x1; break;
        case EDGUI_FIT_FILL: new_area.x1 = has_children ? EDGUI_MATH_MIN(tight_area.x1, flood_area.x1) : flood_area.x1; break;
        default: break;
    }

    switch(ext->fit_right) {
        case EDGUI_FIT_TIGHT: new_area.x2 = tight_area.x2; break;
        case EDGUI_FIT_FLOOD: new_area.x2 = flood_area.x2; break;
        case EDGUI_FIT_FILL: new_area.x2 = has_children ? EDGUI_MATH_MAX(tight_area.x2, flood_area.x2) : flood_area.x2; break;
        default: break;
    }

    switch(ext->fit_top) {
        case EDGUI_FIT_TIGHT: new_area.y1 = tight_area.y1; break;
        case EDGUI_FIT_FLOOD: new_area.y1 = flood_area.y1; break;
        case EDGUI_FIT_FILL: new_area.y1 = has_children ? EDGUI_MATH_MIN(tight_area.y1, flood_area.y1) : flood_area.y1; break;
        default: break;
    }

    switch(ext->fit_bottom) {
        case EDGUI_FIT_TIGHT: new_area.y2 = tight_area.y2; break;
        case EDGUI_FIT_FLOOD: new_area.y2 = flood_area.y2; break;
        case EDGUI_FIT_FILL: new_area.y2 = has_children ? EDGUI_MATH_MAX(tight_area.y2, flood_area.y2) : flood_area.y2; break;
        default: break;
    }

    /*Do nothing if the coordinates are not changed*/
    if(cont->coords.x1 != new_area.x1 || cont->coords.y1 != new_area.y1 || cont->coords.x2 != new_area.x2 ||
       cont->coords.y2 != new_area.y2) {

        edgui_obj_invalidate(cont);
        edgui_area_copy(&cont->coords, &new_area);
        edgui_obj_invalidate(cont);

        /*Notify the object about its new coordinates*/
        cont->signal_cb(cont, EDGUI_SIGNAL_CORD_CHG, &ori);

        /*Inform the parent about the new coordinates*/
        par->signal_cb(par, EDGUI_SIGNAL_CHILD_CHG, cont);

        if(edgui_obj_get_auto_realign(cont)) {
            edgui_obj_realign(cont);
        }

        /*Tell the children the parent's size has changed*/
        EDGUI_LL_READ(cont->child_ll, child_i)
        {
            child_i->signal_cb(child_i, EDGUI_SIGNAL_PARENT_SIZE_CHG, NULL);
        }
    }
}

#endif
