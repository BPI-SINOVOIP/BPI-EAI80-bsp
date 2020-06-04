/**
 * @file edgui_chart.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_chart.h"
#if EDGUI_USE_CHART != 0

#include "../edgui_core/edgui_refr.h"
#include "../edgui_draw/edgui_draw.h"
#include "../edgui_themes/edgui_theme.h"

/*********************
 *      DEFINES
 *********************/
#define EDGUI_CHART_YMIN_DEF 0
#define EDGUI_CHART_YMAX_DEF 100
#define EDGUI_CHART_HDIV_DEF 3
#define EDGUI_CHART_VDIV_DEF 5
#define EDGUI_CHART_PNUM_DEF 10
#define EDGUI_CHART_AXIS_TO_LABEL_DISTANCE 4
#define EDGUI_CHART_AXIS_MAJOR_TICK_LEN_COE 1 / 15
#define EDGUI_CHART_AXIS_MINOR_TICK_LEN_COE 2 / 3

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_chart_design(edgui_obj_t * chart, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_chart_signal(edgui_obj_t * chart, edgui_signal_t sign, void * param);
static void edgui_chart_draw_div(edgui_obj_t * chart, const edgui_area_t * mask);
static void edgui_chart_draw_lines(edgui_obj_t * chart, const edgui_area_t * mask);
static void edgui_chart_draw_points(edgui_obj_t * chart, const edgui_area_t * mask);
static void edgui_chart_draw_cols(edgui_obj_t * chart, const edgui_area_t * mask);
static void edgui_chart_draw_vertical_lines(edgui_obj_t * chart, const edgui_area_t * mask);
static void edgui_chart_draw_areas(edgui_obj_t * chart, const edgui_area_t * mask);
static void edgui_chart_draw_axes(edgui_obj_t * chart, const edgui_area_t * mask);
static void edgui_chart_inv_lines(edgui_obj_t * chart, uint16_t i);
static void edgui_chart_inv_points(edgui_obj_t * chart, uint16_t i);
static void edgui_chart_inv_cols(edgui_obj_t * chart, uint16_t i);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_design_cb_t ancestor_design_f;
static edgui_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a chart background objects
 * @param par pointer to an object, it will be the parent of the new chart background
 * @param copy pointer to a chart background object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created chart background
 */
edgui_obj_t * edgui_chart_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("chart create started");

    /*Create the ancestor basic object*/
    edgui_obj_t * new_chart = edgui_obj_create(par, copy);
    edgui_mem_assert(new_chart);
    if(new_chart == NULL) return NULL;

    /*Allocate the object type specific extended data*/
    edgui_chart_ext_t * ext = edgui_obj_allocate_ext_attr(new_chart, sizeof(edgui_chart_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    edgui_ll_init(&ext->series_ll, sizeof(edgui_chart_series_t));

    ext->series.num            = 0;
    ext->ymin                  = EDGUI_CHART_YMIN_DEF;
    ext->ymax                  = EDGUI_CHART_YMAX_DEF;
    ext->hdiv_cnt              = EDGUI_CHART_HDIV_DEF;
    ext->vdiv_cnt              = EDGUI_CHART_VDIV_DEF;
    ext->point_cnt             = EDGUI_CHART_PNUM_DEF;
    ext->type                  = EDGUI_CHART_TYPE_LINE;
    ext->update_mode           = EDGUI_CHART_UPDATE_MODE_SHIFT;
    ext->series.opa            = EDGUI_OPA_COVER;
    ext->series.dark           = EDGUI_OPA_50;
    ext->series.width          = 2;
    ext->margin                = 0;
    memset(&ext->x_axis, 0, sizeof(ext->x_axis));
    memset(&ext->y_axis, 0, sizeof(ext->y_axis));
    ext->x_axis.major_tick_len = EDGUI_CHART_TICK_LENGTH_AUTO;
    ext->x_axis.minor_tick_len = EDGUI_CHART_TICK_LENGTH_AUTO;
    ext->y_axis.major_tick_len = EDGUI_CHART_TICK_LENGTH_AUTO;
    ext->y_axis.minor_tick_len = EDGUI_CHART_TICK_LENGTH_AUTO;

    if(ancestor_design_f == NULL) ancestor_design_f = edgui_obj_get_design_cb(new_chart);
    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_chart);

    edgui_obj_set_signal_cb(new_chart, edgui_chart_signal);
    edgui_obj_set_design_cb(new_chart, edgui_chart_design);

    /*Init the new chart background object*/
    if(copy == NULL) {
        edgui_obj_set_size(new_chart, EDGUI_DPI * 3, EDGUI_DPI * 2);

        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_chart_set_style(new_chart, EDGUI_CHART_STYLE_MAIN, th->style.chart);
        } else {
            edgui_chart_set_style(new_chart, EDGUI_CHART_STYLE_MAIN, &edgui_style_pretty);
        }

    } else {
        edgui_chart_ext_t * ext_copy = edgui_obj_get_ext_attr(copy);

        ext->type       = ext_copy->type;
        ext->ymin       = ext_copy->ymin;
        ext->ymax       = ext_copy->ymax;
        ext->hdiv_cnt   = ext_copy->hdiv_cnt;
        ext->vdiv_cnt   = ext_copy->vdiv_cnt;
        ext->point_cnt  = ext_copy->point_cnt;
        ext->series.opa = ext_copy->series.opa;
        ext->margin     = ext_copy->margin;
        memcpy(&ext->x_axis, &ext_copy->x_axis, sizeof(edgui_chart_axis_cfg_t));
        memcpy(&ext->y_axis, &ext_copy->y_axis, sizeof(edgui_chart_axis_cfg_t));

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_chart);
    }

    EDGUI_LOG_INFO("chart created");

    return new_chart;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Allocate and add a data series to the chart
 * @param chart pointer to a chart object
 * @param color color of the data series
 * @return pointer to the allocated data series
 */
edgui_chart_series_t * edgui_chart_add_series(edgui_obj_t * chart, edgui_color_t color)
{
    edgui_chart_ext_t * ext    = edgui_obj_get_ext_attr(chart);
    edgui_chart_series_t * ser = edgui_ll_ins_head(&ext->series_ll);
    edgui_mem_assert(ser);
    if(ser == NULL) return NULL;

    edgui_coord_t def = EDGUI_CHART_POINT_DEF;

    if(ser == NULL) return NULL;

    ser->color  = color;
    ser->points = edgui_mem_alloc(sizeof(edgui_coord_t) * ext->point_cnt);
    edgui_mem_assert(ser->points);
    if(ser->points == NULL) {
        edgui_ll_rem(&ext->series_ll, ser);
        edgui_mem_free(ser);
        return NULL;
    }

    ser->start_point = 0;

    uint16_t i;
    edgui_coord_t * p_tmp = ser->points;
    for(i = 0; i < ext->point_cnt; i++) {
        *p_tmp = def;
        p_tmp++;
    }

    ext->series.num++;

    return ser;
}

/**
 * Clear the point of a serie
 * @param chart pointer to a chart object
 * @param serie pointer to the chart's serie to clear
 */
void edgui_chart_clear_serie(edgui_obj_t * chart, edgui_chart_series_t * serie)
{
    if(chart == NULL || serie == NULL) return;
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    if(ext == NULL) return;

    uint32_t i;
    for(i = 0; i < ext->point_cnt; i++) {
        serie->points[i] = EDGUI_CHART_POINT_DEF;
    }

    serie->start_point = 0;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of horizontal and vertical division lines
 * @param chart pointer to a graph background object
 * @param hdiv number of horizontal division lines
 * @param vdiv number of vertical division lines
 */
void edgui_chart_set_div_line_count(edgui_obj_t * chart, uint8_t hdiv, uint8_t vdiv)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    if(ext->hdiv_cnt == hdiv && ext->vdiv_cnt == vdiv) return;

    ext->hdiv_cnt = hdiv;
    ext->vdiv_cnt = vdiv;

    edgui_obj_invalidate(chart);
}

/**
 * Set the minimal and maximal y values
 * @param chart pointer to a graph background object
 * @param ymin y minimum value
 * @param ymax y maximum value
 */
void edgui_chart_set_range(edgui_obj_t * chart, edgui_coord_t ymin, edgui_coord_t ymax)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    if(ext->ymin == ymin && ext->ymax == ymax) return;

    ext->ymin = ymin;
    ext->ymax = ymax;

    edgui_chart_refresh(chart);
}

/**
 * Set a new type for a chart
 * @param chart pointer to a chart object
 * @param type new type of the chart (from 'edgui_chart_type_t' enum)
 */
void edgui_chart_set_type(edgui_obj_t * chart, edgui_chart_type_t type)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    if(ext->type == type) return;

    ext->type = type;

    edgui_chart_refresh(chart);
}

/**
 * Set the number of points on a data line on a chart
 * @param chart pointer r to chart object
 * @param point_cnt new number of points on the data lines
 */
void edgui_chart_set_point_count(edgui_obj_t * chart, uint16_t point_cnt)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    if(ext->point_cnt == point_cnt) return;

    edgui_chart_series_t * ser;
    uint16_t point_cnt_old = ext->point_cnt;
    uint16_t i;
    edgui_coord_t def = EDGUI_CHART_POINT_DEF;

    if(point_cnt < 1) point_cnt = 1;

    EDGUI_LL_READ_BACK(ext->series_ll, ser)
    {
        if(ser->start_point != 0) {
            edgui_coord_t * new_points = edgui_mem_alloc(sizeof(edgui_coord_t) * point_cnt);
            edgui_mem_assert(new_points);
            if(new_points == NULL) return;

            if(point_cnt >= point_cnt_old) {
                for(i = 0; i < point_cnt_old; i++) {
                    new_points[i] =
                        ser->points[(i + ser->start_point) % point_cnt_old]; /*Copy old contents to new array*/
                }
                for(i = point_cnt_old; i < point_cnt; i++) {
                    new_points[i] = def; /*Fill up the rest with default value*/
                }
            } else {
                for(i = 0; i < point_cnt; i++) {
                    new_points[i] =
                        ser->points[(i + ser->start_point) % point_cnt_old]; /*Copy old contents to new array*/
                }
            }

            /*Switch over pointer from old to new*/
            edgui_mem_free(ser->points);
            ser->points = new_points;
        } else {
            ser->points = edgui_mem_realloc(ser->points, sizeof(edgui_coord_t) * point_cnt);
            edgui_mem_assert(ser->points);
            if(ser->points == NULL) return;
            /*Initialize the new points*/
            if(point_cnt > point_cnt_old) {
                for(i = point_cnt_old - 1; i < point_cnt; i++) {
                    ser->points[i] = def;
                }
            }
        }

        ser->start_point = 0;
    }

    ext->point_cnt = point_cnt;

    edgui_chart_refresh(chart);
}

/**
 * Set the opacity of the data series
 * @param chart pointer to a chart object
 * @param opa opacity of the data series
 */
void edgui_chart_set_series_opa(edgui_obj_t * chart, edgui_opa_t opa)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    if(ext->series.opa == opa) return;

    ext->series.opa = opa;
    edgui_obj_invalidate(chart);
}

/**
 * Set the line width or point radius of the data series
 * @param chart pointer to a chart object
 * @param width the new width
 */
void edgui_chart_set_series_width(edgui_obj_t * chart, edgui_coord_t width)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    if(ext->series.width == width) return;

    ext->series.width = width;
    edgui_obj_invalidate(chart);
}
/**
 * Set the dark effect on the bottom of the points or columns
 * @param chart pointer to a chart object
 * @param dark_eff dark effect level (EDGUI_OPA_TRANSP to turn off)
 */
void edgui_chart_set_series_darking(edgui_obj_t * chart, edgui_opa_t dark_eff)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    if(ext->series.dark == dark_eff) return;

    ext->series.dark = dark_eff;
    edgui_obj_invalidate(chart);
}

/**
 * Initialize all data points with a value
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y the new value  for all points
 */
void edgui_chart_init_points(edgui_obj_t * chart, edgui_chart_series_t * ser, edgui_coord_t y)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    uint16_t i;
    for(i = 0; i < ext->point_cnt; i++) {
        ser->points[i] = y;
    }
    ser->start_point = 0;
    edgui_chart_refresh(chart);
}

/**
 * Set the value of points from an array
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y_array array of 'edgui_coord_t' points (with 'points count' elements )
 */
void edgui_chart_set_points(edgui_obj_t * chart, edgui_chart_series_t * ser, edgui_coord_t y_array[])
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    memcpy(ser->points, y_array, ext->point_cnt * (sizeof(edgui_coord_t)));
    ser->start_point = 0;
    edgui_chart_refresh(chart);
}

/**
 * Shift all data left and set the rightmost data on a data line
 * @param chart pointer to chart object
 * @param ser pointer to a data series on 'chart'
 * @param y the new value of the rightmost data
 */
void edgui_chart_set_next(edgui_obj_t * chart, edgui_chart_series_t * ser, edgui_coord_t y)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    if(ext->update_mode == EDGUI_CHART_UPDATE_MODE_SHIFT) {
        ser->points[ser->start_point] =
            y; /*This was the place of the former left most value, after shifting it is the rightmost*/
        ser->start_point = (ser->start_point + 1) % ext->point_cnt;
        edgui_chart_refresh(chart);
    } else if(ext->update_mode == EDGUI_CHART_UPDATE_MODE_CIRCULAR) {
        ser->points[ser->start_point] = y;

        if(ext->type & EDGUI_CHART_TYPE_LINE) edgui_chart_inv_lines(chart, ser->start_point);
        if(ext->type & EDGUI_CHART_TYPE_COLUMN) edgui_chart_inv_cols(chart, ser->start_point);
        if(ext->type & EDGUI_CHART_TYPE_POINT) edgui_chart_inv_points(chart, ser->start_point);
        if(ext->type & EDGUI_CHART_TYPE_VERTICAL_LINE) edgui_chart_inv_lines(chart, ser->start_point);
        if(ext->type & EDGUI_CHART_TYPE_AREA) edgui_chart_inv_lines(chart, ser->start_point);

        ser->start_point = (ser->start_point + 1) % ext->point_cnt; /*update the x for next incoming y*/
    }
}

/**
 * Set update mode of the chart object.
 * @param chart pointer to a chart object
 * @param update mode
 */
void edgui_chart_set_update_mode(edgui_obj_t * chart, edgui_chart_update_mode_t update_mode)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    if(ext->update_mode == update_mode) return;

    ext->update_mode = update_mode;
    edgui_obj_invalidate(chart);
}

/**
 * Set the length of the tick marks on the x axis
 * @param chart pointer to the chart
 * @param major_tick_len the length of the major tick or `EDGUI_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where labels are added)
 * @param minor_tick_len the length of the minor tick, `EDGUI_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where no labels are added)
 */
void edgui_chart_set_x_tick_length(edgui_obj_t * chart, uint8_t major_tick_len, uint8_t minor_tick_len)
{
    edgui_chart_ext_t * ext       = edgui_obj_get_ext_attr(chart);
    ext->x_axis.major_tick_len = major_tick_len;
    ext->x_axis.minor_tick_len = minor_tick_len;
}

/**
 * Set the length of the tick marks on the y axis
 * @param chart pointer to the chart
 * @param major_tick_len the length of the major tick or `EDGUI_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where labels are added)
 * @param minor_tick_len the length of the minor tick, `EDGUI_CHART_TICK_LENGTH_AUTO` to set automatically
 *                       (where no labels are added)
 */
void edgui_chart_set_y_tick_length(edgui_obj_t * chart, uint8_t major_tick_len, uint8_t minor_tick_len)
{
    edgui_chart_ext_t * ext       = edgui_obj_get_ext_attr(chart);
    ext->y_axis.major_tick_len = major_tick_len;
    ext->y_axis.minor_tick_len = minor_tick_len;
}

/**
 * Set the x-axis tick count and labels of a chart
 * @param chart 			pointer to a chart object
 * @param list_of_values 	list of string values, terminated with \n, except the last
 * @param num_tick_marks 	if list_of_values is NULL: total number of ticks per axis
 * 							else number of ticks between two value labels
 * @param options			extra options
 */
void edgui_chart_set_x_tick_texts(edgui_obj_t * chart, const char * list_of_values, uint8_t num_tick_marks,
                               edgui_chart_axis_options_t options)
{
    edgui_chart_ext_t * ext       = edgui_obj_get_ext_attr(chart);
    ext->x_axis.num_tick_marks = num_tick_marks;
    ext->x_axis.list_of_values = list_of_values;
    ext->x_axis.options        = options;
}

/**
 * Set the y-axis tick count and labels of a chart
 * @param chart             pointer to a chart object
 * @param list_of_values    list of string values, terminated with \n, except the last
 * @param num_tick_marks    if list_of_values is NULL: total number of ticks per axis
 *                          else number of ticks between two value labels
 * @param options           extra options
 */
void edgui_chart_set_y_tick_texts(edgui_obj_t * chart, const char * list_of_values, uint8_t num_tick_marks,
                               edgui_chart_axis_options_t options)
{
    edgui_chart_ext_t * ext       = edgui_obj_get_ext_attr(chart);
    ext->y_axis.num_tick_marks = num_tick_marks;
    ext->y_axis.list_of_values = list_of_values;
    ext->y_axis.options        = options;
}

/**
 * Set the margin around the chart, used for axes value and ticks
 * @param chart     pointer to an chart object
 * @param margin    value of the margin [px]
 */
void edgui_chart_set_margin(edgui_obj_t * chart, uint16_t margin)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    ext->margin          = margin;
    edgui_obj_refresh_ext_draw_pad(chart);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the type of a chart
 * @param chart pointer to chart object
 * @return type of the chart (from 'edgui_chart_t' enum)
 */
edgui_chart_type_t edgui_chart_get_type(const edgui_obj_t * chart)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    return ext->type;
}

/**
 * Get the data point number per data line on chart
 * @param chart pointer to chart object
 * @return point number on each data line
 */
uint16_t edgui_chart_get_point_cnt(const edgui_obj_t * chart)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    return ext->point_cnt;
}

/**
 * Get the opacity of the data series
 * @param chart pointer to chart object
 * @return the opacity of the data series
 */
edgui_opa_t edgui_chart_get_series_opa(const edgui_obj_t * chart)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    return ext->series.opa;
}

/**
 * Get the data series width
 * @param chart pointer to chart object
 * @return the width the data series (lines or points)
 */
edgui_coord_t edgui_chart_get_series_width(const edgui_obj_t * chart)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    return ext->series.width;
}

/**
 * Get the dark effect level on the bottom of the points or columns
 * @param chart pointer to chart object
 * @return dark effect level (EDGUI_OPA_TRANSP to turn off)
 */
edgui_opa_t edgui_chart_get_series_darking(const edgui_obj_t * chart)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    return ext->series.dark;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Refresh a chart if its data line has changed
 * @param chart pointer to chart object
 */
void edgui_chart_refresh(edgui_obj_t * chart)
{
    edgui_obj_invalidate(chart);
}

/**
 * Get the margin around the chart, used for axes value and labels
 * @param chart pointer to an chart object
 * @param return value of the margin
 */
uint16_t edgui_chart_get_margin(edgui_obj_t * chart)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    return ext->margin;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the chart backgrounds
 * @param chart pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_chart_design(edgui_obj_t * chart, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        /*Return false if the object is not covers the mask_p area*/
        return ancestor_design_f(chart, mask, mode);
    } else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        /*Draw the background*/
        edgui_draw_rect(&chart->coords, mask, edgui_obj_get_style(chart), edgui_obj_get_opa_scale(chart));

        edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

        edgui_chart_draw_div(chart, mask);

        /* Adjust the mask to remove the margin (clips chart contents to be within background) */

        edgui_area_t mask_tmp, adjusted_mask;
        edgui_obj_get_coords(chart, &mask_tmp);

        bool union_ok = edgui_area_intersect(&adjusted_mask, mask, &mask_tmp);

        if(union_ok) {
                if(ext->type & EDGUI_CHART_TYPE_LINE) edgui_chart_draw_lines(chart, &adjusted_mask);
                if(ext->type & EDGUI_CHART_TYPE_COLUMN) edgui_chart_draw_cols(chart, &adjusted_mask);
                if(ext->type & EDGUI_CHART_TYPE_POINT) edgui_chart_draw_points(chart, &adjusted_mask);
                if(ext->type & EDGUI_CHART_TYPE_VERTICAL_LINE) edgui_chart_draw_vertical_lines(chart, &adjusted_mask);
                if(ext->type & EDGUI_CHART_TYPE_AREA) edgui_chart_draw_areas(chart, &adjusted_mask);
        }

        edgui_chart_draw_axes(chart, mask);
    }
    return true;
}

/**
 * Signal function of the chart background
 * @param chart pointer to a chart background object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 */
static edgui_res_t edgui_chart_signal(edgui_obj_t * chart, edgui_signal_t sign, void * param)
{
    edgui_res_t res;
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

    /* Include the ancient signal function */
    res = ancestor_signal(chart, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_CLEANUP) {
        edgui_coord_t ** datal;
        EDGUI_LL_READ(ext->series_ll, datal)
        {
            edgui_mem_free(*datal);
        }
        edgui_ll_clear(&ext->series_ll);
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_chart";
    } else if(sign == EDGUI_SIGNAL_REFR_EXT_DRAW_PAD) {
        /*Provide extra px draw area around the chart*/
        chart->ext_draw_pad = ext->margin;
    }

    return res;
}

/**
 * Draw the division lines on chart background
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void edgui_chart_draw_div(edgui_obj_t * chart, const edgui_area_t * mask)
{
    edgui_chart_ext_t * ext     = edgui_obj_get_ext_attr(chart);
    const edgui_style_t * style = edgui_obj_get_style(chart);
    edgui_opa_t opa_scale       = edgui_obj_get_opa_scale(chart);

    uint8_t div_i;
    uint8_t div_i_end;
    uint8_t div_i_start;
    edgui_point_t p1;
    edgui_point_t p2;
    edgui_coord_t w     = edgui_obj_get_width(chart);
    edgui_coord_t h     = edgui_obj_get_height(chart);
    edgui_coord_t x_ofs = chart->coords.x1;
    edgui_coord_t y_ofs = chart->coords.y1;

    if(ext->hdiv_cnt != 0) {
        /*Draw side lines if no border*/
        if(style->body.border.width != 0) {
            div_i_start = 1;
            div_i_end   = ext->hdiv_cnt;
        } else {
            div_i_start = 0;
            div_i_end   = ext->hdiv_cnt + 1;
        }

        p1.x = 0 + x_ofs;
        p2.x = w + x_ofs;
        for(div_i = div_i_start; div_i <= div_i_end; div_i++) {
            p1.y = (int32_t)((int32_t)(h - style->line.width) * div_i) / (ext->hdiv_cnt + 1);
            p1.y += y_ofs;
            p2.y = p1.y;
            edgui_draw_line(&p1, &p2, mask, style, opa_scale);
        }
    }

    if(ext->vdiv_cnt != 0) {
        /*Draw side lines if no border*/
        if(style->body.border.width != 0) {
            div_i_start = 1;
            div_i_end   = ext->vdiv_cnt;
        } else {
            div_i_start = 0;
            div_i_end   = ext->vdiv_cnt + 1;
        }

        p1.y = 0 + y_ofs;
        p2.y = h + y_ofs;
        for(div_i = div_i_start; div_i <= div_i_end; div_i++) {
            p1.x = (int32_t)((int32_t)(w - style->line.width) * div_i) / (ext->vdiv_cnt + 1);
            p1.x += x_ofs;
            p2.x = p1.x;
            edgui_draw_line(&p1, &p2, mask, style, opa_scale);
        }
    }
}

/**
 * Draw the data lines as lines on a chart
 * @param obj pointer to chart object
 */
static void edgui_chart_draw_lines(edgui_obj_t * chart, const edgui_area_t * mask)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

    uint16_t i;
    edgui_point_t p1;
    edgui_point_t p2;
    edgui_coord_t w     = edgui_obj_get_width(chart);
    edgui_coord_t h     = edgui_obj_get_height(chart);
    edgui_coord_t x_ofs = chart->coords.x1;
    edgui_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    edgui_coord_t p_prev;
    edgui_coord_t p_act;
    edgui_chart_series_t * ser;
    edgui_opa_t opa_scale = edgui_obj_get_opa_scale(chart);
    edgui_style_t style;
    edgui_style_copy(&style, &edgui_style_plain);
    style.line.opa   = ext->series.opa;
    style.line.width = ext->series.width;

    /*Go through all data lines*/
    EDGUI_LL_READ_BACK(ext->series_ll, ser)
    {
        style.line.color = ser->color;

        edgui_coord_t start_point = ext->update_mode == EDGUI_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

        p1.x = 0 + x_ofs;
        p2.x = 0 + x_ofs;

        p_prev = start_point;
        y_tmp  = (int32_t)((int32_t)ser->points[p_prev] - ext->ymin) * h;
        y_tmp  = y_tmp / (ext->ymax - ext->ymin);
        p2.y   = h - y_tmp + y_ofs;

        for(i = 1; i < ext->point_cnt; i++) {
            p1.x = p2.x;
            p1.y = p2.y;

            p2.x = ((w * i) / (ext->point_cnt - 1)) + x_ofs;

            p_act = (start_point + i) % ext->point_cnt;

            y_tmp = (int32_t)((int32_t)ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);
            p2.y  = h - y_tmp + y_ofs;

            if(ser->points[p_prev] != EDGUI_CHART_POINT_DEF && ser->points[p_act] != EDGUI_CHART_POINT_DEF)
                edgui_draw_line(&p1, &p2, mask, &style, opa_scale);

            p_prev = p_act;
        }
    }
}

/**
 * Draw the data lines as points on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void edgui_chart_draw_points(edgui_obj_t * chart, const edgui_area_t * mask)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

    uint16_t i;
    edgui_area_t cir_a;
    edgui_coord_t w     = edgui_obj_get_width(chart);
    edgui_coord_t h     = edgui_obj_get_height(chart);
    edgui_coord_t x_ofs = chart->coords.x1;
    edgui_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    edgui_coord_t p_act;
    edgui_chart_series_t * ser;
    uint8_t series_cnt = 0;
    edgui_style_t style_point;
    edgui_style_copy(&style_point, &edgui_style_plain);

    style_point.body.border.width = 0;
    style_point.body.radius       = EDGUI_RADIUS_CIRCLE;
    style_point.body.opa          = ext->series.opa;
    style_point.body.radius       = ext->series.width;

    /*Go through all data lines*/

    EDGUI_LL_READ_BACK(ext->series_ll, ser)
    {
        edgui_coord_t start_point = ext->update_mode == EDGUI_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

        style_point.body.main_color = ser->color;
        style_point.body.grad_color = edgui_color_mix(EDGUI_COLOR_BLACK, ser->color, ext->series.dark);

        for(i = 0; i < ext->point_cnt; i++) {
            cir_a.x1 = ((w * i) / (ext->point_cnt - 1)) + x_ofs;
            cir_a.x2 = cir_a.x1 + style_point.body.radius;
            cir_a.x1 -= style_point.body.radius;

            p_act = (start_point + i) % ext->point_cnt;
            y_tmp = (int32_t)((int32_t)ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);

            cir_a.y1 = h - y_tmp + y_ofs;
            cir_a.y2 = cir_a.y1 + style_point.body.radius;
            cir_a.y1 -= style_point.body.radius;

            if(ser->points[p_act] != EDGUI_CHART_POINT_DEF)
                edgui_draw_rect(&cir_a, mask, &style_point, edgui_obj_get_opa_scale(chart));
        }
        series_cnt++;
    }
}

/**
 * Draw the data lines as columns on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void edgui_chart_draw_cols(edgui_obj_t * chart, const edgui_area_t * mask)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

    uint16_t i;
    edgui_area_t col_a;
    edgui_area_t col_mask;
    bool mask_ret;
    edgui_coord_t w = edgui_obj_get_width(chart);
    edgui_coord_t h = edgui_obj_get_height(chart);
    int32_t y_tmp;
    edgui_chart_series_t * ser;
    edgui_style_t rects;
    edgui_coord_t col_w = w / ((ext->series.num + 1) * ext->point_cnt); /* Suppose + 1 series as separator*/
    edgui_coord_t x_ofs = col_w / 2;                                    /*Shift with a half col.*/

    edgui_style_copy(&rects, &edgui_style_plain);
    rects.body.border.width = 0;
    rects.body.radius       = 0;
    rects.body.opa          = ext->series.opa;

    col_a.y2 = chart->coords.y2;

    edgui_coord_t x_act;

    /*Go through all points*/
    for(i = 0; i < ext->point_cnt; i++) {
        x_act = (int32_t)((int32_t)w * i) / ext->point_cnt;
        x_act += chart->coords.x1 + x_ofs;

        /*Draw the current point of all data line*/
        EDGUI_LL_READ_BACK(ext->series_ll, ser)
        {
            edgui_coord_t start_point = ext->update_mode == EDGUI_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;

            col_a.x1 = x_act;
            col_a.x2 = col_a.x1 + col_w;
            x_act += col_w;

            if(col_a.x2 < mask->x1) continue;
            if(col_a.x1 > mask->x2) break;

            rects.body.main_color = ser->color;
            rects.body.grad_color = edgui_color_mix(EDGUI_COLOR_BLACK, ser->color, ext->series.dark);

            edgui_coord_t p_act = (start_point + i) % ext->point_cnt;
            y_tmp            = (int32_t)((int32_t)ser->points[p_act] - ext->ymin) * h;
            y_tmp            = y_tmp / (ext->ymax - ext->ymin);
            col_a.y1         = h - y_tmp + chart->coords.y1;

            mask_ret = edgui_area_intersect(&col_mask, mask, &col_a);
            if(mask_ret != false && ser->points[p_act] != EDGUI_CHART_POINT_DEF) {
                edgui_draw_rect(&chart->coords, &col_mask, &rects, edgui_obj_get_opa_scale(chart));
            }
        }
    }
}

/**
 * Draw the data lines as vertical lines on a chart if there is only 1px between point
 * @param obj pointer to chart object
 */
static void edgui_chart_draw_vertical_lines(edgui_obj_t * chart, const edgui_area_t * mask)
{

    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);
    edgui_coord_t w         = edgui_obj_get_width(chart);
    /*Vertical lines works only if the width == point count. Else use the normal line type*/
    if(ext->point_cnt != w) {
        edgui_chart_draw_lines(chart, mask);
        return;
    }

    uint16_t i;
    edgui_point_t p1;
    edgui_point_t p2;
    edgui_coord_t p_act;
    edgui_coord_t h     = edgui_obj_get_height(chart);
    edgui_coord_t x_ofs = chart->coords.x1;
    edgui_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    edgui_chart_series_t * ser;
    edgui_opa_t opa_scale = edgui_obj_get_opa_scale(chart);
    edgui_style_t style;
    edgui_style_copy(&style, &edgui_style_plain);
    style.line.opa   = ext->series.opa;
    style.line.width = ext->series.width;

    /*Go through all data lines*/
    EDGUI_LL_READ_BACK(ext->series_ll, ser)
    {
        edgui_coord_t start_point = ext->update_mode == EDGUI_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;
        style.line.color       = ser->color;

        p1.x  = 0 + x_ofs;
        p2.x  = 0 + x_ofs;
        y_tmp = (int32_t)((int32_t)ser->points[0] - ext->ymin) * h;
        y_tmp = y_tmp / (ext->ymax - ext->ymin);
        p2.y  = h - y_tmp + y_ofs;
        p1.y  = p2.y;

        for(i = 0; i < ext->point_cnt; i++) {
            p_act = (start_point + i) % ext->point_cnt;

            y_tmp = (int32_t)((int32_t)ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);
            p2.y  = h - y_tmp + y_ofs;

            if(p1.y == p2.y) {
                p2.x++;
            }

            if(ser->points[p_act] != EDGUI_CHART_POINT_DEF) {
                edgui_draw_line(&p1, &p2, mask, &style, opa_scale);
            }

            p2.x = ((w * p_act) / (ext->point_cnt - 1)) + x_ofs;
            p1.x = p2.x;
            p1.y = p2.y;
        }
    }
}

/**
 * Draw the data lines as areas on a chart
 * @param obj pointer to chart object
 */
static void edgui_chart_draw_areas(edgui_obj_t * chart, const edgui_area_t * mask)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

    uint16_t i;
    edgui_point_t p1;
    edgui_point_t p2;
    edgui_coord_t w     = edgui_obj_get_width(chart);
    edgui_coord_t h     = edgui_obj_get_height(chart);
    edgui_coord_t x_ofs = chart->coords.x1;
    edgui_coord_t y_ofs = chart->coords.y1;
    int32_t y_tmp;
    edgui_coord_t p_prev;
    edgui_coord_t p_act;
    edgui_chart_series_t * ser;
    edgui_opa_t opa_scale = edgui_obj_get_opa_scale(chart);
    edgui_style_t style;
    edgui_style_copy(&style, &edgui_style_plain);

    /*Go through all data lines*/
    EDGUI_LL_READ_BACK(ext->series_ll, ser)
    {
        edgui_coord_t start_point = ext->update_mode == EDGUI_CHART_UPDATE_MODE_SHIFT ? ser->start_point : 0;
        style.body.main_color  = ser->color;
        style.body.opa         = ext->series.opa;

        p2.x = 0 + x_ofs;

        p_prev = start_point;
        y_tmp  = (int32_t)((int32_t)ser->points[p_prev] - ext->ymin) * h;
        y_tmp  = y_tmp / (ext->ymax - ext->ymin);
        p2.y   = h - y_tmp + y_ofs;

        for(i = 1; i < ext->point_cnt; i++) {
            p1.x = p2.x;
            p1.y = p2.y;

            p_act = (start_point + i) % ext->point_cnt;
            p2.x  = ((w * i) / (ext->point_cnt - 1)) + x_ofs;

            y_tmp = (int32_t)((int32_t)ser->points[p_act] - ext->ymin) * h;
            y_tmp = y_tmp / (ext->ymax - ext->ymin);
            p2.y  = h - y_tmp + y_ofs;

            if(ser->points[p_prev] != EDGUI_CHART_POINT_DEF && ser->points[p_act] != EDGUI_CHART_POINT_DEF) {
                edgui_point_t triangle_points[3];
                triangle_points[0]   = p1;
                triangle_points[1]   = p2;
                triangle_points[2].x = p1.x;
                triangle_points[2].y = chart->coords.y2;
                edgui_draw_triangle(triangle_points, mask, &style, opa_scale);
                triangle_points[2].x = p2.x;
                triangle_points[0].y = chart->coords.y2;
                edgui_draw_triangle(triangle_points, mask, &style, opa_scale);
            }
            p_prev = p_act;
        }
    }
}

static void edgui_chart_draw_y_ticks(edgui_obj_t * chart, const edgui_area_t * mask)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

    if(ext->y_axis.list_of_values != NULL || ext->y_axis.num_tick_marks != 0) {

        const edgui_style_t * style = edgui_obj_get_style(chart);
        edgui_opa_t opa_scale       = edgui_obj_get_opa_scale(chart);

        uint8_t i, j;
        uint8_t list_index;
        uint8_t num_of_labels;
        uint8_t num_scale_ticks;
        uint8_t major_tick_len, minor_tick_len;
        edgui_point_t p1;
        edgui_point_t p2;
        edgui_coord_t x_ofs = chart->coords.x1;
        edgui_coord_t y_ofs = chart->coords.y1;
        edgui_coord_t h     = edgui_obj_get_height(chart);
        edgui_coord_t w     = edgui_obj_get_width(chart);
        char buf[EDGUI_CHART_AXIS_TICK_LABEL_MAX_LEN + 1]; /* up to N symbols per label + null terminator */

        /* calculate the size of tick marks */
        if(ext->y_axis.major_tick_len == EDGUI_CHART_TICK_LENGTH_AUTO)
            major_tick_len = (int32_t)w * EDGUI_CHART_AXIS_MAJOR_TICK_LEN_COE;
        else
            major_tick_len = ext->y_axis.major_tick_len;

        if(ext->y_axis.minor_tick_len == EDGUI_CHART_TICK_LENGTH_AUTO)
            minor_tick_len = major_tick_len * EDGUI_CHART_AXIS_MINOR_TICK_LEN_COE;
        else
            minor_tick_len = ext->y_axis.minor_tick_len;

        /* count the '\n'-s to determine the number of options */
        list_index    = 0;
        num_of_labels = 0;
        if(ext->y_axis.list_of_values != NULL) {
            for(j = 0; ext->y_axis.list_of_values[j] != '\0'; j++) {
                if(ext->y_axis.list_of_values[j] == '\n') num_of_labels++;
            }

            num_of_labels++; /* last option in the at row*/
        }

        /* we can't have string labels without ticks step, set to 1 if not specified */
        if(ext->y_axis.num_tick_marks == 0) ext->y_axis.num_tick_marks = 1;

        /* calculate total number of ticks */
        if(num_of_labels < 2)
            num_scale_ticks = ext->y_axis.num_tick_marks;
        else
            num_scale_ticks = (ext->y_axis.num_tick_marks * (num_of_labels - 1));

        for(i = 0; i < (num_scale_ticks + 1); i++) { /* one extra loop - it may not exist in the list, empty label */
                                                     /* first point of the tick */
            p1.x = x_ofs - 1;

            /* second point of the tick */
            if((num_of_labels != 0) && (i == 0 || i % ext->y_axis.num_tick_marks == 0))
                p2.x = p1.x - major_tick_len; /* major tick */
            else
                p2.x = p1.x - minor_tick_len; /* minor tick */

            /* draw a line at moving y position */
            p2.y = p1.y =
                y_ofs + (int32_t)((int32_t)(h - style->line.width) * i) / num_scale_ticks;

            if(i != num_scale_ticks)
                edgui_draw_line(&p1, &p2, mask, style, opa_scale);
            else if((ext->y_axis.options & EDGUI_CHART_AXIS_DRAW_LAST_TICK) != 0)
                edgui_draw_line(&p1, &p2, mask, style, opa_scale);

            /* draw values if available */
            if(num_of_labels != 0) {
                /* add text only to major tick */
                if(i == 0 || i % ext->y_axis.num_tick_marks == 0) {
                    /* search for tick string */
                    j = 0;
                    while(ext->y_axis.list_of_values[list_index] != '\n' &&
                          ext->y_axis.list_of_values[list_index] != '\0') {
                        /* do not overflow the buffer, but move to the end of the current label */
                        if(j < EDGUI_CHART_AXIS_TICK_LABEL_MAX_LEN)
                            buf[j++] = ext->y_axis.list_of_values[list_index++];
                        else
                            list_index++;
                    }

                    /* this was a string, but not end of the list, so jump to the next string */
                    if(ext->y_axis.list_of_values[list_index] == '\n') list_index++;

                    /* terminate the string */
                    buf[j] = '\0';

                    /* reserve appropriate area */
                    edgui_point_t size;
                    edgui_txt_get_size(&size, buf, style->text.font, style->text.letter_space, style->text.line_space,
                                    EDGUI_COORD_MAX, EDGUI_TXT_FLAG_CENTER);

                    /* set the area at some distance of the major tick len left of the tick */
                    edgui_area_t a = {(p2.x - size.x - EDGUI_CHART_AXIS_TO_LABEL_DISTANCE), (p2.y - size.y / 2),
                                   (p2.x - EDGUI_CHART_AXIS_TO_LABEL_DISTANCE), (p2.y + size.y / 2)};
                    edgui_draw_label(&a, mask, style, opa_scale, buf, EDGUI_TXT_FLAG_CENTER, NULL, -1, -1, NULL);
                }
            }

        }
    }
}

static void edgui_chart_draw_x_ticks(edgui_obj_t * chart, const edgui_area_t * mask)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

    if(ext->x_axis.list_of_values != NULL || ext->x_axis.num_tick_marks != 0) {

        const edgui_style_t * style = edgui_obj_get_style(chart);
        edgui_opa_t opa_scale       = edgui_obj_get_opa_scale(chart);

        uint8_t i, j;
        uint8_t list_index;
        uint8_t num_of_labels;
        uint8_t num_scale_ticks;
        uint8_t major_tick_len, minor_tick_len;
        edgui_point_t p1;
        edgui_point_t p2;
        edgui_coord_t x_ofs = chart->coords.x1;
        edgui_coord_t y_ofs = chart->coords.y1;
        edgui_coord_t h     = edgui_obj_get_height(chart);
        edgui_coord_t w     = edgui_obj_get_width(chart);
        char buf[EDGUI_CHART_AXIS_TICK_LABEL_MAX_LEN + 1]; /* up to N symbols per label + null terminator */

        /* calculate the size of tick marks */
        if(ext->x_axis.major_tick_len == EDGUI_CHART_TICK_LENGTH_AUTO)
            major_tick_len = (int32_t)w * EDGUI_CHART_AXIS_MAJOR_TICK_LEN_COE;
        else
            major_tick_len = ext->x_axis.major_tick_len;

        if(ext->x_axis.minor_tick_len == EDGUI_CHART_TICK_LENGTH_AUTO)
            minor_tick_len = major_tick_len * EDGUI_CHART_AXIS_MINOR_TICK_LEN_COE;
        else
            minor_tick_len = ext->x_axis.minor_tick_len;

        /* count the '\n'-s to determine the number of options */
        list_index    = 0;
        num_of_labels = 0;
        if(ext->x_axis.list_of_values != NULL) {
            for(j = 0; ext->x_axis.list_of_values[j] != '\0'; j++) {
                if(ext->x_axis.list_of_values[j] == '\n') num_of_labels++;
            }

            num_of_labels++; /* last option in the at row*/
        }

        /* we can't have string labels without ticks step, set to 1 if not specified */
        if(ext->x_axis.num_tick_marks == 0) ext->x_axis.num_tick_marks = 1;

        /* calculate total number of marks */
        if(num_of_labels < 2)
            num_scale_ticks = ext->x_axis.num_tick_marks;
        else
            num_scale_ticks = (ext->x_axis.num_tick_marks * (num_of_labels - 1));

        for(i = 0; i < (num_scale_ticks + 1); i++) { /* one extra loop - it may not exist in the list, empty label */
                                                     /* first point of the tick */
            p1.y = h + y_ofs;

            /* second point of the tick */
            if((num_of_labels != 0) && (i == 0 || i % ext->x_axis.num_tick_marks == 0))
                p2.y = p1.y + major_tick_len; /* major tick */
            else
                p2.y = p1.y + minor_tick_len; /* minor tick */

            /* draw a line at moving x position */
            p2.x = p1.x = x_ofs + (int32_t)((int32_t)(w - style->line.width) * i) / num_scale_ticks;

            if(i != num_scale_ticks)
                edgui_draw_line(&p1, &p2, mask, style, opa_scale);
            else if((ext->x_axis.options & EDGUI_CHART_AXIS_DRAW_LAST_TICK) != 0)
                edgui_draw_line(&p1, &p2, mask, style, opa_scale);

            /* draw values if available */
            if(num_of_labels != 0) {
                /* add text only to major tick */
                if(i == 0 || i % ext->x_axis.num_tick_marks == 0) {
                    /* search for tick string */
                    j = 0;
                    while(ext->x_axis.list_of_values[list_index] != '\n' &&
                          ext->x_axis.list_of_values[list_index] != '\0') {
                        /* do not overflow the buffer, but move to the end of the current label */
                        if(j < EDGUI_CHART_AXIS_TICK_LABEL_MAX_LEN)
                            buf[j++] = ext->x_axis.list_of_values[list_index++];
                        else
                            list_index++;
                    }

                    /* this was a string, but not end of the list, so jump to the next string */
                    if(ext->x_axis.list_of_values[list_index] == '\n') list_index++;

                    /* terminate the string */
                    buf[j] = '\0';

                    /* reserve appropriate area */
                    edgui_point_t size;
                    edgui_txt_get_size(&size, buf, style->text.font, style->text.letter_space, style->text.line_space,
                                    EDGUI_COORD_MAX, EDGUI_TXT_FLAG_CENTER);

                    /* set the area at some distance of the major tick len under of the tick */
                    edgui_area_t a = {(p2.x - size.x / 2), (p2.y + EDGUI_CHART_AXIS_TO_LABEL_DISTANCE), (p2.x + size.x / 2),
                                   (p2.y + size.y + EDGUI_CHART_AXIS_TO_LABEL_DISTANCE)};
                    edgui_draw_label(&a, mask, style, opa_scale, buf, EDGUI_TXT_FLAG_CENTER, NULL, -1, -1, NULL);
                }
            }
        }
    }
}

static void edgui_chart_draw_axes(edgui_obj_t * chart, const edgui_area_t * mask)
{
    edgui_chart_draw_y_ticks(chart, mask);
    edgui_chart_draw_x_ticks(chart, mask);
}

/**
 * invalid area of the new line data lines on a chart
 * @param obj pointer to chart object
 */
static void edgui_chart_inv_lines(edgui_obj_t * chart, uint16_t i)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

    edgui_coord_t w     = edgui_obj_get_width(chart);
    edgui_coord_t x_ofs = chart->coords.x1;

    if(i < ext->point_cnt) {
        edgui_area_t coords;
        edgui_obj_get_coords(chart, &coords);
        if(i < ext->point_cnt - 1) {
            coords.x1 = ((w * i) / (ext->point_cnt - 1)) + x_ofs - ext->series.width;
            coords.x2 = ((w * (i + 1)) / (ext->point_cnt - 1)) + x_ofs + ext->series.width;
            edgui_inv_area(edgui_obj_get_disp(chart), &coords);
        }

        if(i > 0) {
            coords.x1 = ((w * (i - 1)) / (ext->point_cnt - 1)) + x_ofs - ext->series.width;
            coords.x2 = ((w * i) / (ext->point_cnt - 1)) + x_ofs + ext->series.width;
            edgui_inv_area(edgui_obj_get_disp(chart), &coords);
        }
    }
}

/**
 * invalid area of the new point data lines on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void edgui_chart_inv_points(edgui_obj_t * chart, uint16_t i)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

    edgui_area_t cir_a;
    edgui_coord_t w     = edgui_obj_get_width(chart);
    edgui_coord_t x_ofs = chart->coords.x1;

    edgui_obj_get_coords(chart, &cir_a);
    cir_a.x1 = ((w * i) / (ext->point_cnt - 1)) + x_ofs;
    cir_a.x2 = cir_a.x1 + ext->series.width;
    cir_a.x1 -= ext->series.width;

    edgui_inv_area(edgui_obj_get_disp(chart), &cir_a);
}

/**
 * invalid area of the new column data lines on a chart
 * @param chart pointer to chart object
 * @param mask mask, inherited from the design function
 */
static void edgui_chart_inv_cols(edgui_obj_t * chart, uint16_t i)
{
    edgui_chart_ext_t * ext = edgui_obj_get_ext_attr(chart);

    edgui_area_t col_a;
    edgui_coord_t w     = edgui_obj_get_width(chart);
    edgui_coord_t col_w = w / ((ext->series.num + 1) * ext->point_cnt); /* Suppose + 1 series as separator*/
    edgui_coord_t x_ofs = col_w / 2;                                    /*Shift with a half col.*/

    edgui_coord_t x_act;

    x_act = (int32_t)((int32_t)w * i) / ext->point_cnt;
    x_act += chart->coords.x1 + x_ofs;

    edgui_obj_get_coords(chart, &col_a);
    col_a.x1 = x_act;
    col_a.x2 = col_a.x1 + col_w;

    edgui_inv_area(edgui_obj_get_disp(chart), &col_a);
}

#endif
