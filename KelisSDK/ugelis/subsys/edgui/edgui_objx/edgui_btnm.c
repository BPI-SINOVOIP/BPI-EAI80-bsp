/**
 * @file edgui_btnm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_btnm.h"
#if EDGUI_USE_BTNM != 0

#include "../edgui_core/edgui_group.h"
#include "../edgui_draw/edgui_draw.h"
#include "../edgui_core/edgui_refr.h"
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_txt.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_btnm_signal(edgui_obj_t * btnm, edgui_signal_t sign, void * param);
static bool edgui_btnm_design(edgui_obj_t * btnm, const edgui_area_t * mask, edgui_design_mode_t mode);
static uint8_t get_button_width(edgui_btnm_ctrl_t ctrl_bits);
static bool button_is_hidden(edgui_btnm_ctrl_t ctrl_bits);
static bool button_is_repeat_disabled(edgui_btnm_ctrl_t ctrl_bits);
static bool button_is_inactive(edgui_btnm_ctrl_t ctrl_bits);
static bool button_is_click_trig(edgui_btnm_ctrl_t ctrl_bits);
static bool button_is_tgl_enabled(edgui_btnm_ctrl_t ctrl_bits);
static bool button_get_tgl_state(edgui_btnm_ctrl_t ctrl_bits);
static uint16_t get_button_from_point(edgui_obj_t * btnm, edgui_point_t * p);
static void allocate_btn_areas_and_controls(const edgui_obj_t * btnm, const char ** map);
static void invalidate_button_area(const edgui_obj_t * btnm, uint16_t btn_idx);
static bool maps_are_identical(const char ** map1, const char ** map2);
static void make_one_button_toggled(edgui_obj_t * btnm, uint16_t btn_idx);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * edgui_btnm_def_map[] = {"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5", ""};

static edgui_design_cb_t ancestor_design_f;
static edgui_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a button matrix objects
 * @param par pointer to an object, it will be the parent of the new button matrix
 * @param copy pointer to a button matrix object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created button matrix
 */
edgui_obj_t * edgui_btnm_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("button matrix create started");

    /*Create the ancestor object*/
    edgui_obj_t * new_btnm = edgui_obj_create(par, copy);
    edgui_mem_assert(new_btnm);
    if(new_btnm == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_btnm);

    /*Allocate the object type specific extended data*/
    edgui_btnm_ext_t * ext = edgui_obj_allocate_ext_attr(new_btnm, sizeof(edgui_btnm_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->btn_cnt                          = 0;
    ext->btn_id_pr                        = EDGUI_BTNM_BTN_NONE;
    ext->btn_id_act                       = EDGUI_BTNM_BTN_NONE;
    ext->button_areas                     = NULL;
    ext->ctrl_bits                        = NULL;
    ext->map_p                            = NULL;
    ext->recolor                          = 0;
    ext->one_toggle                       = 0;
    ext->styles_btn[EDGUI_BTN_STATE_REL]     = &edgui_style_btn_rel;
    ext->styles_btn[EDGUI_BTN_STATE_PR]      = &edgui_style_btn_pr;
    ext->styles_btn[EDGUI_BTN_STATE_TGL_REL] = &edgui_style_btn_tgl_rel;
    ext->styles_btn[EDGUI_BTN_STATE_TGL_PR]  = &edgui_style_btn_tgl_pr;
    ext->styles_btn[EDGUI_BTN_STATE_INA]     = &edgui_style_btn_ina;

    if(ancestor_design_f == NULL) ancestor_design_f = edgui_obj_get_design_cb(new_btnm);

    edgui_obj_set_signal_cb(new_btnm, edgui_btnm_signal);
    edgui_obj_set_design_cb(new_btnm, edgui_btnm_design);

    /*Init the new button matrix object*/
    if(copy == NULL) {
        edgui_obj_set_size(new_btnm, EDGUI_DPI * 3, EDGUI_DPI * 2);
        edgui_btnm_set_map(new_btnm, edgui_btnm_def_map);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_btnm_set_style(new_btnm, EDGUI_BTNM_STYLE_BG, th->style.btnm.bg);
            edgui_btnm_set_style(new_btnm, EDGUI_BTNM_STYLE_BTN_REL, th->style.btnm.btn.rel);
            edgui_btnm_set_style(new_btnm, EDGUI_BTNM_STYLE_BTN_PR, th->style.btnm.btn.pr);
            edgui_btnm_set_style(new_btnm, EDGUI_BTNM_STYLE_BTN_TGL_REL, th->style.btnm.btn.tgl_rel);
            edgui_btnm_set_style(new_btnm, EDGUI_BTNM_STYLE_BTN_TGL_PR, th->style.btnm.btn.tgl_pr);
            edgui_btnm_set_style(new_btnm, EDGUI_BTNM_STYLE_BTN_INA, th->style.btnm.btn.ina);
        } else {
            edgui_obj_set_style(new_btnm, &edgui_style_pretty);
        }
    }
    /*Copy an existing object*/
    else {
        edgui_btnm_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        memcpy(ext->styles_btn, copy_ext->styles_btn, sizeof(ext->styles_btn));
        edgui_btnm_set_map(new_btnm, edgui_btnm_get_map_array(copy));
    }

    EDGUI_LOG_INFO("button matrix created");

    return new_btnm;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new map. Buttons will be created/deleted according to the map. The
 * button matrix keeps a reference to the map and so the string array must not
 * be deallocated during the life of the matrix.
 * @param btnm pointer to a button matrix object
 * @param map pointer a string array. The last string has to be: "". Use "\n" to make a line break.
 */
void edgui_btnm_set_map(const edgui_obj_t * btnm, const char * map[])
{
    if(map == NULL) return;

    /*
     * edgui_btnm_set_map is called on receipt of signals such as
     * EDGUI_SIGNAL_CORD_CHG regardless of whether the map has changed (e.g.
     * calling edgui_obj_align on the map will trigger this).
     *
     * We check if the map has changed here to avoid overwriting changes made
     * to hidden/longpress/disabled states after the map was originally set.
     *
     * TODO: separate all map set/allocation from layout code below and skip
     * set/allocation when map hasn't changed.
     */
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    if(!maps_are_identical(ext->map_p, map)) {

        /*Analyze the map and create the required number of buttons*/
        allocate_btn_areas_and_controls(btnm, map);
    }
    ext->map_p = map;

    /*Set size and positions of the buttons*/
    const edgui_style_t * style_bg = edgui_btnm_get_style(btnm, EDGUI_BTNM_STYLE_BG);
    edgui_coord_t max_w            = edgui_obj_get_width(btnm) - style_bg->body.padding.left - style_bg->body.padding.right;
    edgui_coord_t max_h            = edgui_obj_get_height(btnm) - style_bg->body.padding.top - style_bg->body.padding.bottom;
    edgui_coord_t act_y            = style_bg->body.padding.top;

    /*Count the lines to calculate button height*/
    uint8_t line_cnt = 1;
    uint8_t li;
    for(li = 0; strlen(map[li]) != 0; li++) {
        if(strcmp(map[li], "\n") == 0) line_cnt++;
    }

    edgui_coord_t btn_h = max_h - ((line_cnt - 1) * style_bg->body.padding.inner);
    btn_h            = btn_h / line_cnt;
    btn_h--; /*-1 because e.g. height = 100 means 101 pixels (0..100)*/

    /* Count the units and the buttons in a line
     * (A button can be 1,2,3... unit wide)*/
    uint16_t unit_cnt;           /*Number of units in a row*/
    uint16_t unit_act_cnt;       /*Number of units currently put in a row*/
    uint16_t btn_cnt;            /*Number of buttons in a row*/
    uint16_t i_tot          = 0; /*Act. index in the str map*/
    uint16_t btn_i          = 0; /*Act. index of button areas*/
    const char ** map_p_tmp = map;

    /*Count the units and the buttons in a line*/
    while(1) {
        unit_cnt = 0;
        btn_cnt  = 0;
        /*Count the buttons in a line*/
        while(strcmp(map_p_tmp[btn_cnt], "\n") != 0 && strlen(map_p_tmp[btn_cnt]) != 0) { /*Check a line*/
            unit_cnt += get_button_width(ext->ctrl_bits[btn_i + btn_cnt]);
            btn_cnt++;
        }

        /*Make sure the last row is at the bottom of 'btnm'*/
        if(map_p_tmp[btn_cnt][0] == '\0') { /*Last row?*/
            btn_h = edgui_obj_get_height(btnm)- act_y - style_bg->body.padding.bottom - 1;
        }

        /*Only deal with the non empty lines*/
        if(btn_cnt != 0) {
            /*Calculate the width of all units*/
            edgui_coord_t all_unit_w = max_w - ((btn_cnt - 1) * style_bg->body.padding.inner);

            /*Set the button size and positions and set the texts*/
            uint16_t i;
            edgui_coord_t act_x = style_bg->body.padding.left;
            edgui_coord_t act_unit_w;
            unit_act_cnt = 0;
            for(i = 0; i < btn_cnt; i++) {
                /* one_unit_w = all_unit_w / unit_cnt
                 * act_unit_w = one_unit_w * button_width
                 * do this two operations but the multiply first to divide a greater number */
                act_unit_w = (all_unit_w * get_button_width(ext->ctrl_bits[btn_i])) / unit_cnt;
                act_unit_w--; /*-1 because e.g. width = 100 means 101 pixels (0..100)*/

                /*Always recalculate act_x because of rounding errors */
                act_x = (unit_act_cnt * all_unit_w) / unit_cnt + i * style_bg->body.padding.inner +
                        style_bg->body.padding.left;

                /* Set the button's area.
                 * If inner padding is zero then use the prev. button x2 as x1 to avoid rounding
                 * errors*/
                if(style_bg->body.padding.inner == 0 && act_x != style_bg->body.padding.left) {
                    edgui_area_set(&ext->button_areas[btn_i], ext->button_areas[btn_i - 1].x2, act_y, act_x + act_unit_w,
                                act_y + btn_h);
                } else {
                    edgui_area_set(&ext->button_areas[btn_i], act_x, act_y, act_x + act_unit_w, act_y + btn_h);
                }

                unit_act_cnt += get_button_width(ext->ctrl_bits[btn_i]);

                i_tot++;
                btn_i++;
            }
        }
        act_y += btn_h + style_bg->body.padding.inner + 1;

        if(strlen(map_p_tmp[btn_cnt]) == 0) break; /*Break on end of map*/
        map_p_tmp = &map_p_tmp[btn_cnt + 1];       /*Set the map to the next line*/
        i_tot++;                                   /*Skip the '\n'*/
    }

    edgui_obj_invalidate(btnm);
}

/**
 * Set the button control map (hidden, disabled etc.) for a button matrix. The
 * control map array will be copied and so may be deallocated after this
 * function returns.
 * @param btnm pointer to a button matrix object
 * @param ctrl_map pointer to an array of `edgui_btn_ctrl_t` control bytes. The
 *                 length of the array and position of the elements must match
 *                 the number and order of the individual buttons (i.e. excludes
 *                 newline entries).
 *                 An element of the map should look like e.g.:
 *                 `ctrl_map[0] = width | EDGUI_BTNM_CTRL_NO_REPEAT |  EDGUI_BTNM_CTRL_TGL_ENABLE`
 */
void edgui_btnm_set_ctrl_map(const edgui_obj_t * btnm, const edgui_btnm_ctrl_t ctrl_map[])
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    memcpy(ext->ctrl_bits, ctrl_map, sizeof(edgui_btnm_ctrl_t) * ext->btn_cnt);

    edgui_btnm_set_map(btnm, ext->map_p);
}

/**
 * Set the pressed button i.e. visually highlight it.
 * Mainly used a when the btnm is in a group to show the selected button
 * @param btnm pointer to button matrix object
 * @param id index of the currently pressed button (`EDGUI_BTNM_BTN_NONE` to unpress)
 */
void edgui_btnm_set_pressed(const edgui_obj_t * btnm, uint16_t id)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);

    if(id >= ext->btn_cnt && id != EDGUI_BTNM_BTN_NONE) return;

    if(id == ext->btn_id_pr) return;

    ext->btn_id_pr = id;
    edgui_obj_invalidate(btnm);
}

/**
 * Set a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_btnm_set_style(edgui_obj_t * btnm, edgui_btnm_style_t type, const edgui_style_t * style)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);

    switch(type) {
        case EDGUI_BTNM_STYLE_BG: edgui_obj_set_style(btnm, style); break;
        case EDGUI_BTNM_STYLE_BTN_REL:
            ext->styles_btn[EDGUI_BTN_STATE_REL] = style;
            edgui_obj_invalidate(btnm);
            break;
        case EDGUI_BTNM_STYLE_BTN_PR:
            ext->styles_btn[EDGUI_BTN_STATE_PR] = style;
            edgui_obj_invalidate(btnm);
            break;
        case EDGUI_BTNM_STYLE_BTN_TGL_REL:
            ext->styles_btn[EDGUI_BTN_STATE_TGL_REL] = style;
            edgui_obj_invalidate(btnm);
            break;
        case EDGUI_BTNM_STYLE_BTN_TGL_PR:
            ext->styles_btn[EDGUI_BTN_STATE_TGL_PR] = style;
            edgui_obj_invalidate(btnm);
            break;
        case EDGUI_BTNM_STYLE_BTN_INA:
            ext->styles_btn[EDGUI_BTN_STATE_INA] = style;
            edgui_obj_invalidate(btnm);
            break;
    }
}

/**
 * Enable recoloring of button's texts
 * @param btnm pointer to button matrix object
 * @param en true: enable recoloring; false: disable
 */
void edgui_btnm_set_recolor(const edgui_obj_t * btnm, bool en)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);

    ext->recolor = en;
    edgui_obj_invalidate(btnm);
}

/**
 * Set the attributes of a button of the button matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify. (Not counting new lines)
 */
void edgui_btnm_set_btn_ctrl(const edgui_obj_t * btnm, uint16_t btn_id, edgui_btnm_ctrl_t ctrl)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);

    if(btn_id >= ext->btn_cnt) return;

    ext->ctrl_bits[btn_id] |= ctrl;
    invalidate_button_area(btnm, btn_id);
}

/**
 * Clear the attributes of a button of the button matrix
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify. (Not counting new lines)
 */
void edgui_btnm_clear_btn_ctrl(const edgui_obj_t * btnm, uint16_t btn_id, edgui_btnm_ctrl_t ctrl)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);

    if(btn_id >= ext->btn_cnt) return;

    ext->ctrl_bits[btn_id] &= (~ctrl);
    invalidate_button_area(btnm, btn_id);
}

/**
 * Set the attributes of all buttons of a button matrix
 * @param btnm pointer to a button matrix object
 * @param ctrl attribute(s) to set from `edgui_btnm_ctrl_t`. Values can be ORed.
 */
void edgui_btnm_set_btn_ctrl_all(edgui_obj_t * btnm, edgui_btnm_ctrl_t ctrl)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    uint16_t i;
    for(i = 0; i < ext->btn_cnt; i++) {
        edgui_btnm_set_btn_ctrl(btnm, i, ctrl);
    }
}

/**
 * Clear the attributes of all buttons of a button matrix
 * @param btnm pointer to a button matrix object
 * @param ctrl attribute(s) to set from `edgui_btnm_ctrl_t`. Values can be ORed.
 * @param en true: set the attributes; false: clear the attributes
 */
void edgui_btnm_clear_btn_ctrl_all(edgui_obj_t * btnm, edgui_btnm_ctrl_t ctrl)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    uint16_t i;
    for(i = 0; i < ext->btn_cnt; i++) {
        edgui_btnm_clear_btn_ctrl(btnm, i, ctrl);
    }
}

/**
 * Set a single buttons relative width.
 * This method will cause the matrix be regenerated and is a relatively
 * expensive operation. It is recommended that initial width be specified using
 * `edgui_btnm_set_ctrl_map` and this method only be used for dynamic changes.
 * @param btnm pointer to button matrix object
 * @param btn_id 0 based index of the button to modify.
 * @param width Relative width compared to the buttons in the same row. [1..7]
 */
void edgui_btnm_set_btn_width(const edgui_obj_t * btnm, uint16_t btn_id, uint8_t width)
{

    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    if(btn_id >= ext->btn_cnt) return;
    ext->ctrl_bits[btn_id] &= (~EDGUI_BTNM_WIDTH_MASK);
    ext->ctrl_bits[btn_id] |= (EDGUI_BTNM_WIDTH_MASK & width);

    edgui_btnm_set_map(btnm, ext->map_p);
}

/**
 * Make the button matrix like a selector widget (only one button may be toggled at a time).
 *
 * Toggling must be enabled on the buttons you want to be selected with `edgui_btnm_set_ctrl` or
 * `edgui_btnm_set_btn_ctrl_all`.
 *
 * @param btnm Button matrix object
 * @param one_toggle Whether "one toggle" mode is enabled
 */
void edgui_btnm_set_one_toggle(edgui_obj_t * btnm, bool one_toggle)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    ext->one_toggle     = one_toggle;

    /*If more than one button is toggled only the first one should be*/
    make_one_button_toggled(btnm, 0);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current map of a button matrix
 * @param btnm pointer to a button matrix object
 * @return the current map
 */
const char ** edgui_btnm_get_map_array(const edgui_obj_t * btnm)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    return ext->map_p;
}

/**
 * Check whether the button's text can use recolor or not
 * @param btnm pointer to button matrix object
 * @return true: text recolor enable; false: disabled
 */
bool edgui_btnm_get_recolor(const edgui_obj_t * btnm)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);

    return ext->recolor;
}

/**
 * Get the index of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb` to get the text of the button, check if hidden etc.
 * @param btnm pointer to button matrix object
 * @return  index of the last released button (EDGUI_BTNM_BTN_NONE: if unset)
 */
uint16_t edgui_btnm_get_active_btn(const edgui_obj_t * btnm)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    return ext->btn_id_act;
}

/**
 * Get the text of the lastly "activated" button by the user (pressed, released etc)
 * Useful in the the `event_cb`
 * @param btnm pointer to button matrix object
 * @return text of the last released button (NULL: if unset)
 */
const char * edgui_btnm_get_active_btn_text(const edgui_obj_t * btnm)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    if(ext->btn_id_act != EDGUI_BTNM_BTN_NONE) {
        return edgui_btnm_get_btn_text(btnm, ext->btn_id_act);
    } else {
        return NULL;
    }
}

/**
 * Get the pressed button's index.
 * The button be really pressed by the user or manually set to pressed with `edgui_btnm_set_pressed`
 * @param btnm pointer to button matrix object
 * @return  index of the pressed button (EDGUI_BTNM_BTN_NONE: if unset)
 */
uint16_t edgui_btnm_get_pressed_btn(const edgui_obj_t * btnm)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    return ext->btn_id_pr;
}

/**
 * Get the button's text
 * @param btnm pointer to button matrix object
 * @param btn_id the index a button not counting new line characters. (The return value of
 * edgui_btnm_get_pressed/released)
 * @return  text of btn_index` button
 */
const char * edgui_btnm_get_btn_text(const edgui_obj_t * btnm, uint16_t btn_id)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    if(btn_id > ext->btn_cnt) return NULL;

    uint16_t txt_i = 0;
    uint16_t btn_i = 0;

    /* Search the text of ext->btn_pr the buttons text in the map
     * Skip "\n"-s*/
    while(btn_i != btn_id) {
        btn_i++;
        txt_i++;
        if(strcmp(ext->map_p[txt_i], "\n") == 0) txt_i++;
    }

    if(btn_i == ext->btn_cnt) return NULL;

    return ext->map_p[txt_i];
}

/**
 * Get the whether a control value is enabled or disabled for button of a button matrix
 * @param btnm pointer to a button matrix object
 * @param btn_id the index a button not counting new line characters. (E.g. the return value of
 * edgui_btnm_get_pressed/released)
 * @param ctrl control values to check (ORed value can be used)
 * @return true: long press repeat is disabled; false: long press repeat enabled
 */
bool edgui_btnm_get_btn_ctrl(edgui_obj_t * btnm, uint16_t btn_id, edgui_btnm_ctrl_t ctrl)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    if(btn_id >= ext->btn_cnt) return false;

    return ext->ctrl_bits[btn_id] & ctrl ? true : false;
}

/**
 * Get a style of a button matrix
 * @param btnm pointer to a button matrix object
 * @param type which style should be get
 * @return style pointer to a style
 */
const edgui_style_t * edgui_btnm_get_style(const edgui_obj_t * btnm, edgui_btnm_style_t type)
{
    const edgui_style_t * style = NULL;
    edgui_btnm_ext_t * ext      = edgui_obj_get_ext_attr(btnm);

    switch(type) {
        case EDGUI_BTNM_STYLE_BG: style = edgui_obj_get_style(btnm); break;
        case EDGUI_BTNM_STYLE_BTN_REL: style = ext->styles_btn[EDGUI_BTN_STATE_REL]; break;
        case EDGUI_BTNM_STYLE_BTN_PR: style = ext->styles_btn[EDGUI_BTN_STATE_PR]; break;
        case EDGUI_BTNM_STYLE_BTN_TGL_REL: style = ext->styles_btn[EDGUI_BTN_STATE_TGL_REL]; break;
        case EDGUI_BTNM_STYLE_BTN_TGL_PR: style = ext->styles_btn[EDGUI_BTN_STATE_TGL_PR]; break;
        case EDGUI_BTNM_STYLE_BTN_INA: style = ext->styles_btn[EDGUI_BTN_STATE_INA]; break;
        default: style = NULL; break;
    }

    return style;
}

/**
 * Find whether "one toggle" mode is enabled.
 * @param btnm Button matrix object
 * @return whether "one toggle" mode is enabled
 */
bool edgui_btnm_get_one_toggle(const edgui_obj_t * btnm)
{
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);

    return ext->one_toggle;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the button matrixs
 * @param btnm pointer to a button matrix object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_btnm_design(edgui_obj_t * btnm, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        return ancestor_design_f(btnm, mask, mode);
        /*Return false if the object is not covers the mask_p area*/
    }
    /*Draw the object*/
    else if(mode == EDGUI_DESIGN_DRAW_MAIN) {

        ancestor_design_f(btnm, mask, mode);

        edgui_btnm_ext_t * ext         = edgui_obj_get_ext_attr(btnm);
        const edgui_style_t * bg_style = edgui_obj_get_style(btnm);
        const edgui_style_t * btn_style;
        edgui_opa_t opa_scale = edgui_obj_get_opa_scale(btnm);

        edgui_area_t area_btnm;
        edgui_obj_get_coords(btnm, &area_btnm);

        edgui_area_t area_tmp;
        edgui_coord_t btn_w;
        edgui_coord_t btn_h;

        uint16_t btn_i = 0;
        uint16_t txt_i = 0;
        edgui_style_t style_tmp;
        edgui_txt_flag_t txt_flag = EDGUI_TXT_FLAG_NONE;

        if(ext->recolor) txt_flag = EDGUI_TXT_FLAG_RECOLOR;

        for(btn_i = 0; btn_i < ext->btn_cnt; btn_i++, txt_i++) {
            /*Search the next valid text in the map*/
            while(strcmp(ext->map_p[txt_i], "\n") == 0) {
                txt_i++;
            }

            /*Skip hidden buttons*/
            if(button_is_hidden(ext->ctrl_bits[btn_i])) continue;

            edgui_area_copy(&area_tmp, &ext->button_areas[btn_i]);
            area_tmp.x1 += area_btnm.x1;
            area_tmp.y1 += area_btnm.y1;
            area_tmp.x2 += area_btnm.x1;
            area_tmp.y2 += area_btnm.y1;

            btn_w = edgui_area_get_width(&area_tmp);
            btn_h = edgui_area_get_height(&area_tmp);

            /*Load the style*/
            bool tgl_state = button_get_tgl_state(ext->ctrl_bits[btn_i]);
            if(button_is_inactive(ext->ctrl_bits[btn_i]))
                btn_style = edgui_btnm_get_style(btnm, EDGUI_BTNM_STYLE_BTN_INA);
            else if(btn_i != ext->btn_id_pr && tgl_state == false)
                btn_style = edgui_btnm_get_style(btnm, EDGUI_BTNM_STYLE_BTN_REL);
            else if(btn_i == ext->btn_id_pr && tgl_state == false)
                btn_style = edgui_btnm_get_style(btnm, EDGUI_BTNM_STYLE_BTN_PR);
            else if(btn_i != ext->btn_id_pr && tgl_state == true)
                btn_style = edgui_btnm_get_style(btnm, EDGUI_BTNM_STYLE_BTN_TGL_REL);
            else if(btn_i == ext->btn_id_pr && tgl_state == true)
                btn_style = edgui_btnm_get_style(btnm, EDGUI_BTNM_STYLE_BTN_TGL_PR);
            else
                btn_style = edgui_btnm_get_style(btnm, EDGUI_BTNM_STYLE_BTN_REL); /*Not possible option, just to be sure*/

            edgui_style_copy(&style_tmp, btn_style);

            /*Remove borders on the edges if `EDGUI_BORDER_INTERNAL`*/
            if(style_tmp.body.border.part & EDGUI_BORDER_INTERNAL) {
                if(area_tmp.y1 == btnm->coords.y1 + bg_style->body.padding.top) {
                    style_tmp.body.border.part &= ~EDGUI_BORDER_TOP;
                }
                if(area_tmp.y2 == btnm->coords.y2 - bg_style->body.padding.bottom) {
                    style_tmp.body.border.part &= ~EDGUI_BORDER_BOTTOM;
                }

                if(txt_i == 0) {
                    style_tmp.body.border.part &= ~EDGUI_BORDER_LEFT;
                } else if(strcmp(ext->map_p[txt_i - 1], "\n") == 0) {
                    style_tmp.body.border.part &= ~EDGUI_BORDER_LEFT;
                }

                if(ext->map_p[txt_i + 1][0] == '\0' || strcmp(ext->map_p[txt_i + 1], "\n") == 0) {
                    style_tmp.body.border.part &= ~EDGUI_BORDER_RIGHT;
                }
            }
            edgui_draw_rect(&area_tmp, mask, &style_tmp, opa_scale);

            /*Calculate the size of the text*/
            if(btn_style->glass) btn_style = bg_style;
            const edgui_font_t * font = btn_style->text.font;
            edgui_point_t txt_size;
            edgui_txt_get_size(&txt_size, ext->map_p[txt_i], font, btn_style->text.letter_space,
                            btn_style->text.line_space, edgui_area_get_width(&area_btnm), txt_flag);

            area_tmp.x1 += (btn_w - txt_size.x) / 2;
            area_tmp.y1 += (btn_h - txt_size.y) / 2;
            area_tmp.x2 = area_tmp.x1 + txt_size.x;
            area_tmp.y2 = area_tmp.y1 + txt_size.y;

            edgui_draw_label(&area_tmp, mask, btn_style, opa_scale, ext->map_p[txt_i], txt_flag, NULL, -1, -1, NULL);
        }
    }
    return true;
}

/**
 * Signal function of the button matrix
 * @param btnm pointer to a button matrix object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_btnm_signal(edgui_obj_t * btnm, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(btnm, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    edgui_point_t p;
    if(sign == EDGUI_SIGNAL_CLEANUP) {
        edgui_mem_free(ext->button_areas);
        edgui_mem_free(ext->ctrl_bits);
    } else if(sign == EDGUI_SIGNAL_STYLE_CHG || sign == EDGUI_SIGNAL_CORD_CHG) {
        edgui_btnm_set_map(btnm, ext->map_p);
    } else if(sign == EDGUI_SIGNAL_PRESSED) {
        edgui_indev_t * indev = edgui_indev_get_act();
        if(edgui_indev_get_type(indev) == EDGUI_INDEV_TYPE_POINTER || edgui_indev_get_type(indev) == EDGUI_INDEV_TYPE_BUTTON) {
            uint16_t btn_pr;
            /*Search the pressed area*/
            edgui_indev_get_point(param, &p);
            btn_pr = get_button_from_point(btnm, &p);

            invalidate_button_area(btnm, ext->btn_id_pr) /*Invalidate the old area*/;
            ext->btn_id_pr  = btn_pr;
            ext->btn_id_act = btn_pr;
            invalidate_button_area(btnm, ext->btn_id_pr); /*Invalidate the new area*/
        }
        if(ext->btn_id_act != EDGUI_BTNM_BTN_NONE) {
            if(button_is_click_trig(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_inactive(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_hidden(ext->ctrl_bits[ext->btn_id_act]) == false) {
                uint32_t b = ext->btn_id_act;
                res        = edgui_event_send(btnm, EDGUI_EVENT_VALUE_CHANGED, &b);
            }
        }
    } else if(sign == EDGUI_SIGNAL_PRESSING) {
        uint16_t btn_pr;
        /*Search the pressed area*/
        edgui_indev_get_point(param, &p);
        btn_pr = get_button_from_point(btnm, &p);
        /*Invalidate to old and the new areas*/;
        if(btn_pr != ext->btn_id_pr) {
            edgui_indev_reset_long_press(param); /*Start the log press time again on the new button*/
            if(ext->btn_id_pr != EDGUI_BTNM_BTN_NONE) {
                invalidate_button_area(btnm, ext->btn_id_pr);
            }
            if(btn_pr != EDGUI_BTNM_BTN_NONE) {
                uint32_t b = ext->btn_id_act;
                res        = edgui_event_send(btnm, EDGUI_EVENT_VALUE_CHANGED, &b);
                if(res == EDGUI_RES_OK) {
                    invalidate_button_area(btnm, btn_pr);
                }
            }
        }

        ext->btn_id_pr  = btn_pr;
        ext->btn_id_act = btn_pr;
    } else if(sign == EDGUI_SIGNAL_RELEASED) {
        if(ext->btn_id_pr != EDGUI_BTNM_BTN_NONE) {
            /*Toggle the button if enabled*/
            if(button_is_tgl_enabled(ext->ctrl_bits[ext->btn_id_pr])) {
                if(button_get_tgl_state(ext->ctrl_bits[ext->btn_id_pr])) {
                    ext->ctrl_bits[ext->btn_id_pr] &= (~EDGUI_BTNM_CTRL_TGL_STATE);
                } else {
                    ext->ctrl_bits[ext->btn_id_pr] |= EDGUI_BTNM_CTRL_TGL_STATE;
                }
                if(ext->one_toggle) make_one_button_toggled(btnm, ext->btn_id_pr);
            }

            /*Invalidate to old pressed area*/;
            invalidate_button_area(btnm, ext->btn_id_pr);

#if EDGUI_USE_GROUP
            /*Leave the clicked button when releases if this not the focused object in a group*/
            edgui_group_t * g = edgui_obj_get_group(btnm);
            if(edgui_group_get_focused(g) != btnm) {
                ext->btn_id_pr = EDGUI_BTNM_BTN_NONE;
            }
#else
            ext->btn_id_pr = EDGUI_BTNM_BTN_NONE;
#endif

            if(button_is_click_trig(ext->ctrl_bits[ext->btn_id_act]) == true &&
               button_is_inactive(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_hidden(ext->ctrl_bits[ext->btn_id_act]) == false) {
                uint32_t b = ext->btn_id_act;
                res        = edgui_event_send(btnm, EDGUI_EVENT_VALUE_CHANGED, &b);
            }
        }
    } else if(sign == EDGUI_SIGNAL_LONG_PRESS_REP) {
        if(ext->btn_id_act != EDGUI_BTNM_BTN_NONE) {
            if(button_is_repeat_disabled(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_inactive(ext->ctrl_bits[ext->btn_id_act]) == false &&
               button_is_hidden(ext->ctrl_bits[ext->btn_id_act]) == false) {
                uint32_t b = ext->btn_id_act;
                res        = edgui_event_send(btnm, EDGUI_EVENT_VALUE_CHANGED, &b);
            }
        }
    } else if(sign == EDGUI_SIGNAL_PRESS_LOST || sign == EDGUI_SIGNAL_DEFOCUS) {
        ext->btn_id_pr  = EDGUI_BTNM_BTN_NONE;
        ext->btn_id_act = EDGUI_BTNM_BTN_NONE;
        edgui_obj_invalidate(btnm);
    } else if(sign == EDGUI_SIGNAL_FOCUS) {
#if EDGUI_USE_GROUP
        edgui_indev_t * indev         = edgui_indev_get_act();
        edgui_indev_type_t indev_type = edgui_indev_get_type(indev);
        if(indev_type == EDGUI_INDEV_TYPE_POINTER) {
            /*Select the clicked button*/
            edgui_point_t p1;
            edgui_indev_get_point(indev, &p1);
            uint16_t btn_i = get_button_from_point(btnm, &p1);
            ext->btn_id_pr = btn_i;

        } else if(indev_type == EDGUI_INDEV_TYPE_ENCODER) {
            /*In navigation mode don't select any button but in edit mode select the fist*/
            if(edgui_group_get_editing(edgui_obj_get_group(btnm)))
                ext->btn_id_pr = 0;
            else
                ext->btn_id_pr = EDGUI_BTNM_BTN_NONE;
        } else {
            ext->btn_id_pr = 0;
        }
#else
        ext->btn_id_pr = 0;
#endif

        ext->btn_id_act = ext->btn_id_pr;
        edgui_obj_invalidate(btnm);
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        char c = *((char *)param);
        if(c == EDGUI_KEY_RIGHT) {
            if(ext->btn_id_pr == EDGUI_BTNM_BTN_NONE)
                ext->btn_id_pr = 0;
            else
                ext->btn_id_pr++;
            if(ext->btn_id_pr >= ext->btn_cnt - 1) ext->btn_id_pr = ext->btn_cnt - 1;
            ext->btn_id_act = ext->btn_id_pr;
            edgui_obj_invalidate(btnm);
        } else if(c == EDGUI_KEY_LEFT) {
            if(ext->btn_id_pr == EDGUI_BTNM_BTN_NONE) ext->btn_id_pr = 0;
            if(ext->btn_id_pr > 0) ext->btn_id_pr--;
            ext->btn_id_act = ext->btn_id_pr;
            edgui_obj_invalidate(btnm);
        } else if(c == EDGUI_KEY_DOWN) {
            const edgui_style_t * style = edgui_btnm_get_style(btnm, EDGUI_BTNM_STYLE_BG);
            /*Find the area below the the current*/
            if(ext->btn_id_pr == EDGUI_BTNM_BTN_NONE) {
                ext->btn_id_pr = 0;
            } else {
                uint16_t area_below;
                edgui_coord_t pr_center =
                    ext->button_areas[ext->btn_id_pr].x1 + (edgui_area_get_width(&ext->button_areas[ext->btn_id_pr]) >> 1);

                for(area_below = ext->btn_id_pr; area_below < ext->btn_cnt; area_below++) {
                    if(ext->button_areas[area_below].y1 > ext->button_areas[ext->btn_id_pr].y1 &&
                       pr_center >= ext->button_areas[area_below].x1 &&
                       pr_center <= ext->button_areas[area_below].x2 + style->body.padding.inner) {
                        break;
                    }
                }

                if(area_below < ext->btn_cnt) ext->btn_id_pr = area_below;
            }
            ext->btn_id_act = ext->btn_id_pr;
            edgui_obj_invalidate(btnm);
        } else if(c == EDGUI_KEY_UP) {
            const edgui_style_t * style = edgui_btnm_get_style(btnm, EDGUI_BTNM_STYLE_BG);
            /*Find the area below the the current*/
            if(ext->btn_id_pr == EDGUI_BTNM_BTN_NONE) {
                ext->btn_id_pr = 0;
            } else {
                int16_t area_above;
                edgui_coord_t pr_center =
                    ext->button_areas[ext->btn_id_pr].x1 + (edgui_area_get_width(&ext->button_areas[ext->btn_id_pr]) >> 1);

                for(area_above = ext->btn_id_pr; area_above >= 0; area_above--) {
                    if(ext->button_areas[area_above].y1 < ext->button_areas[ext->btn_id_pr].y1 &&
                       pr_center >= ext->button_areas[area_above].x1 - style->body.padding.inner &&
                       pr_center <= ext->button_areas[area_above].x2) {
                        break;
                    }
                }
                if(area_above >= 0) ext->btn_id_pr = area_above;
            }
            ext->btn_id_act = ext->btn_id_pr;
            edgui_obj_invalidate(btnm);
        }
    } else if(sign == EDGUI_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = true;
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_btnm";
    }

    return res;
}

/**
 * Create the required number of buttons and control bytes according to a map
 * @param btnm pointer to button matrix object
 * @param map_p pointer to a string array
 */
static void allocate_btn_areas_and_controls(const edgui_obj_t * btnm, const char ** map)
{
    /*Count the buttons in the map*/
    uint16_t btn_cnt = 0;
    uint16_t i       = 0;
    while(strlen(map[i]) != 0) {
        if(strcmp(map[i], "\n") != 0) { /*Do not count line breaks*/
            btn_cnt++;
        }
        i++;
    }

    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);

    if(ext->button_areas != NULL) {
        edgui_mem_free(ext->button_areas);
        ext->button_areas = NULL;
    }
    if(ext->ctrl_bits != NULL) {
        edgui_mem_free(ext->ctrl_bits);
        ext->ctrl_bits = NULL;
    }

    ext->button_areas = edgui_mem_alloc(sizeof(edgui_area_t) * btn_cnt);
    edgui_mem_assert(ext->button_areas);
    ext->ctrl_bits = edgui_mem_alloc(sizeof(edgui_btnm_ctrl_t) * btn_cnt);
    edgui_mem_assert(ext->ctrl_bits);
    if(ext->button_areas == NULL || ext->ctrl_bits == NULL) btn_cnt = 0;

    memset(ext->ctrl_bits, 0, sizeof(edgui_btnm_ctrl_t) * btn_cnt);

    ext->btn_cnt = btn_cnt;
}

/**
 * Get the width of a button in units (default is 1).
 * @param ctrl_bits least significant 3 bits used (1..7 valid values)
 * @return the width of the button in units
 */
static uint8_t get_button_width(edgui_btnm_ctrl_t ctrl_bits)
{
    uint8_t w = ctrl_bits & EDGUI_BTNM_WIDTH_MASK;
    return w != 0 ? w : 1;
}

static bool button_is_hidden(edgui_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & EDGUI_BTNM_CTRL_HIDDEN ? true : false;
}

static bool button_is_repeat_disabled(edgui_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & EDGUI_BTNM_CTRL_NO_REPEAT ? true : false;
}

static bool button_is_inactive(edgui_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & EDGUI_BTNM_CTRL_INACTIVE ? true : false;
}

static bool button_is_click_trig(edgui_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & EDGUI_BTNM_CTRL_CLICK_TRIG ? true : false;
}

static bool button_is_tgl_enabled(edgui_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & EDGUI_BTNM_CTRL_TGL_ENABLE ? true : false;
}

static bool button_get_tgl_state(edgui_btnm_ctrl_t ctrl_bits)
{
    return ctrl_bits & EDGUI_BTNM_CTRL_TGL_STATE ? true : false;
}

/**
 * Gives the button id of a button under a given point
 * @param btnm pointer to a button matrix object
 * @param p a point with absolute coordinates
 * @return the id of the button or EDGUI_BTNM_BTN_NONE.
 */
static uint16_t get_button_from_point(edgui_obj_t * btnm, edgui_point_t * p)
{
    edgui_area_t btnm_cords;
    edgui_area_t btn_area;
    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    uint16_t i;
    edgui_obj_get_coords(btnm, &btnm_cords);

    for(i = 0; i < ext->btn_cnt; i++) {
        edgui_area_copy(&btn_area, &ext->button_areas[i]);
        btn_area.x1 += btnm_cords.x1;
        btn_area.y1 += btnm_cords.y1;
        btn_area.x2 += btnm_cords.x1;
        btn_area.y2 += btnm_cords.y1;
        if(edgui_area_is_point_on(&btn_area, p) != false) {
            break;
        }
    }

    if(i == ext->btn_cnt) i = EDGUI_BTNM_BTN_NONE;

    return i;
}

static void invalidate_button_area(const edgui_obj_t * btnm, uint16_t btn_idx)
{
    if(btn_idx == EDGUI_BTNM_BTN_NONE) return;

    edgui_area_t btn_area;
    edgui_area_t btnm_area;

    edgui_btnm_ext_t * ext = edgui_obj_get_ext_attr(btnm);
    edgui_area_copy(&btn_area, &ext->button_areas[btn_idx]);
    edgui_obj_get_coords(btnm, &btnm_area);

    /* Convert relative coordinates to absolute */
    btn_area.x1 += btnm_area.x1;
    btn_area.y1 += btnm_area.y1;
    btn_area.x2 += btnm_area.x1;
    btn_area.y2 += btnm_area.y1;

    edgui_inv_area(edgui_obj_get_disp(btnm), &btn_area);
}

/**
 * Compares two button matrix maps for equality
 * @param map1 map to compare
 * @param map2 map to compare
 * @return true if maps are identical in length and content
 */
static bool maps_are_identical(const char ** map1, const char ** map2)
{
    if(map1 == map2) return true;
    if(map1 == NULL || map2 == NULL) return map1 == map2;

    uint16_t i = 0;
    while(map1[i][0] != '\0' && map2[i][0] != '\0') {
        if(strcmp(map1[i], map2[i]) != 0) return false;
        i++;
    }
    return map1[i][0] == '\0' && map2[i][0] == '\0';
}

/**
 * Enforces a single button being toggled on the button matrix.
 * It simply clears the toggle flag on other buttons.
 * @param btnm Button matrix object
 * @param btn_idx Button that should remain toggled
 */
static void make_one_button_toggled(edgui_obj_t * btnm, uint16_t btn_idx)
{
    /*Save whether the button was toggled*/
    bool was_toggled = edgui_btnm_get_btn_ctrl(btnm, btn_idx, EDGUI_BTNM_CTRL_TGL_STATE);

    edgui_btnm_clear_btn_ctrl_all(btnm, EDGUI_BTNM_CTRL_TGL_STATE);

    if(was_toggled) edgui_btnm_set_btn_ctrl(btnm, btn_idx, EDGUI_BTNM_CTRL_TGL_STATE);
}

#endif
