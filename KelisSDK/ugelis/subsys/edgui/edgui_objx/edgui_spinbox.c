/**
 * @file edgui_spinbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_spinbox.h"

#if EDGUI_USE_SPINBOX != 0
#include "../edgui_themes/edgui_theme.h"
#include "../edgui_misc/edgui_math.h"
#include "../edgui_misc/edgui_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static edgui_res_t edgui_spinbox_signal(edgui_obj_t * spinbox, edgui_signal_t sign, void * param);
static void edgui_spinbox_updatevalue(edgui_obj_t * spinbox);

/**********************
 *  STATIC VARIABLES
 **********************/
static edgui_signal_cb_t ancestor_signal;
static edgui_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a spinbox object
 * @param par pointer to an object, it will be the parent of the new spinbox
 * @param copy pointer to a spinbox object, if not NULL then the new object will be copied from it
 * @return pointer to the created spinbox
 */
edgui_obj_t * edgui_spinbox_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("spinbox create started");

    /*Create the ancestor of spinbox*/
    edgui_obj_t * new_spinbox = edgui_ta_create(par, copy);
    edgui_mem_assert(new_spinbox);
    if(new_spinbox == NULL) return NULL;

    /*Allocate the spinbox type specific extended data*/
    edgui_spinbox_ext_t * ext = edgui_obj_allocate_ext_attr(new_spinbox, sizeof(edgui_spinbox_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_spinbox);
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_spinbox);

    /*Initialize the allocated 'ext'*/
    ext->value              = 0;
    ext->dec_point_pos      = 0;
    ext->digit_count        = 5;
    ext->digit_padding_left = 0;
    ext->step               = 1;
    ext->range_max          = 99999;
    ext->range_min          = -99999;

    edgui_ta_set_cursor_type(new_spinbox, EDGUI_CURSOR_BLOCK);
    edgui_ta_set_one_line(new_spinbox, true);
    edgui_ta_set_cursor_click_pos(new_spinbox, false);

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_spinbox, edgui_spinbox_signal);
    edgui_obj_set_design_cb(new_spinbox, ancestor_design); /*Leave the Text area's design function*/

    /*Init the new spinbox spinbox*/
    if(copy == NULL) {
        /*Set the default styles*/
        edgui_theme_t * th = edgui_theme_get_current();
        if(th) {
            edgui_spinbox_set_style(new_spinbox, EDGUI_SPINBOX_STYLE_BG, th->style.spinbox.bg);
            edgui_spinbox_set_style(new_spinbox, EDGUI_SPINBOX_STYLE_CURSOR, th->style.spinbox.cursor);
            edgui_spinbox_set_style(new_spinbox, EDGUI_SPINBOX_STYLE_SB, th->style.spinbox.sb);
        }
    }
    /*Copy an existing spinbox*/
    else {
        edgui_spinbox_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);

        edgui_spinbox_set_value(new_spinbox, copy_ext->value);
        edgui_spinbox_set_digit_format(new_spinbox, copy_ext->digit_count, copy_ext->dec_point_pos);
        edgui_spinbox_set_range(new_spinbox, copy_ext->range_min, copy_ext->range_max);
        edgui_spinbox_set_step(new_spinbox, copy_ext->step);

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_spinbox);
    }

    edgui_spinbox_updatevalue(new_spinbox);

    EDGUI_LOG_INFO("spinbox created");

    return new_spinbox;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set spinbox value
 * @param spinbox pointer to spinbox
 * @param i value to be set
 */
void edgui_spinbox_set_value(edgui_obj_t * spinbox, int32_t i)
{
    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);
    if(ext == NULL) return;

    if(i > ext->range_max) i = ext->range_max;
    if(i < ext->range_min) i = ext->range_min;

    ext->value = i;

    edgui_spinbox_updatevalue(spinbox);
}

/**
 * Set spinbox digit format (digit count and decimal format)
 * @param spinbox pointer to spinbox
 * @param digit_count number of digit excluding the decimal separator and the sign
 * @param separator_position number of digit before the decimal point. If 0, decimal point is not
 * shown
 */
void edgui_spinbox_set_digit_format(edgui_obj_t * spinbox, uint8_t digit_count, uint8_t separator_position)
{
    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);
    if(ext == NULL) return;

    if(digit_count > EDGUI_SPINBOX_MAX_DIGIT_COUNT) digit_count = EDGUI_SPINBOX_MAX_DIGIT_COUNT;

    if(separator_position > EDGUI_SPINBOX_MAX_DIGIT_COUNT) separator_position = EDGUI_SPINBOX_MAX_DIGIT_COUNT;

    ext->digit_count   = digit_count;
    ext->dec_point_pos = separator_position;

    edgui_spinbox_updatevalue(spinbox);
}

/**
 * Set spinbox step
 * @param spinbox pointer to spinbox
 * @param step steps on increment/decrement
 */
void edgui_spinbox_set_step(edgui_obj_t * spinbox, uint32_t step)
{
    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);
    if(ext == NULL) return;

    ext->step = step;
}

/**
 * Set spinbox value range
 * @param spinbox pointer to spinbox
 * @param range_min maximum value, inclusive
 * @param range_max minimum value, inclusive
 */
void edgui_spinbox_set_range(edgui_obj_t * spinbox, int32_t range_min, int32_t range_max)
{
    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);
    if(ext == NULL) return;

    ext->range_max = range_max;
    ext->range_min = range_min;

    if(ext->value > ext->range_max) {
        ext->value = ext->range_max;
        edgui_obj_invalidate(spinbox);
    }
    if(ext->value < ext->range_min) {
        ext->value = ext->range_min;
        edgui_obj_invalidate(spinbox);
    }
}

/**
 * Set spinbox left padding in digits count (added between sign and first digit)
 * @param spinbox pointer to spinbox
 * @param cb Callback function called on value change event
 */
void edgui_spinbox_set_padding_left(edgui_obj_t * spinbox, uint8_t padding)
{
    edgui_spinbox_ext_t * ext  = edgui_obj_get_ext_attr(spinbox);
    ext->digit_padding_left = padding;
    edgui_spinbox_updatevalue(spinbox);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the spinbox numeral value (user has to convert to float according to its digit format)
 * @param spinbox pointer to spinbox
 * @return value integer value of the spinbox
 */
int32_t edgui_spinbox_get_value(edgui_obj_t * spinbox)
{
    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);

    return ext->value;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Select next lower digit for edition
 * @param spinbox pointer to spinbox
 */
void edgui_spinbox_step_next(edgui_obj_t * spinbox)
{
    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);

    int32_t new_step = ext->step / 10;
    if((new_step) > 0)
        ext->step = new_step;
    else
        ext->step = 1;

    edgui_spinbox_updatevalue(spinbox);
}

/**
 * Select next higher digit for edition
 * @param spinbox pointer to spinbox
 */
void edgui_spinbox_step_prev(edgui_obj_t * spinbox)
{
    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);
    int32_t step_limit;
    step_limit       = EDGUI_MATH_MAX(ext->range_max, (ext->range_min < 0 ? (-ext->range_min) : ext->range_min));
    int32_t new_step = ext->step * 10;
    if(new_step <= step_limit) ext->step = new_step;

    edgui_spinbox_updatevalue(spinbox);
}

/**
 * Increment spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void edgui_spinbox_increment(edgui_obj_t * spinbox)
{
    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);

    if(ext->value + ext->step <= ext->range_max) {
        /*Special mode when zero crossing*/
        if((ext->value + ext->step) > 0 && ext->value < 0) ext->value = -ext->value;
        ext->value += ext->step;

    } else {
        ext->value = ext->range_max;
    }

    edgui_spinbox_updatevalue(spinbox);
}

/**
 * Decrement spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void edgui_spinbox_decrement(edgui_obj_t * spinbox)
{
    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);

    if(ext->value - ext->step >= ext->range_min) {
        /*Special mode when zero crossing*/
        if((ext->value - ext->step) < 0 && ext->value > 0) ext->value = -ext->value;
        ext->value -= ext->step;
    } else {
        ext->value = ext->range_min;
    }

    edgui_spinbox_updatevalue(spinbox);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the spinbox
 * @param spinbox pointer to a spinbox object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_spinbox_signal(edgui_obj_t * spinbox, edgui_signal_t sign, void * param)
{

    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);

    edgui_res_t res = EDGUI_RES_OK;

    /* Include the ancient signal function */
    if(sign != EDGUI_SIGNAL_CONTROL) {
        res = ancestor_signal(spinbox, sign, param);
        if(res != EDGUI_RES_OK) return res;
    }

    if(sign == EDGUI_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_spinbox";
    } else if(sign == EDGUI_SIGNAL_RELEASED) {
        /*If released with an ENCODER then move to the next digit*/
#if EDGUI_USE_GROUP
        edgui_indev_t * indev = edgui_indev_get_act();
        if(edgui_indev_get_type(indev) == EDGUI_INDEV_TYPE_ENCODER) {
            if(edgui_group_get_editing(edgui_obj_get_group(spinbox))) {
                if(ext->step > 1) {
                    edgui_spinbox_step_next(spinbox);
                } else {
                    /*Restart from the MSB*/
                    ext->step = 1;
                    uint32_t i;
                    for(i = 0; i < ext->digit_count; i++) {
                        int32_t new_step = ext->step * 10;
                        if(new_step >= ext->range_max) break;
                        ext->step = new_step;
                    }
                    edgui_spinbox_step_prev(spinbox);
                }
            }
        }
#endif
    } else if(sign == EDGUI_SIGNAL_CONTROL) {
        edgui_indev_type_t indev_type = edgui_indev_get_type(edgui_indev_get_act());

        uint32_t c = *((uint32_t *)param); /*uint32_t because can be UTF-8*/
        if(c == EDGUI_KEY_RIGHT) {
            if(indev_type == EDGUI_INDEV_TYPE_ENCODER)
                edgui_spinbox_increment(spinbox);
            else
                edgui_spinbox_step_next(spinbox);
        } else if(c == EDGUI_KEY_LEFT) {
            if(indev_type == EDGUI_INDEV_TYPE_ENCODER)
                edgui_spinbox_decrement(spinbox);
            else
                edgui_spinbox_step_prev(spinbox);
        } else if(c == EDGUI_KEY_UP) {
            edgui_spinbox_increment(spinbox);
        } else if(c == EDGUI_KEY_DOWN) {
            edgui_spinbox_decrement(spinbox);
        } else {
            edgui_ta_add_char(spinbox, c);
        }
    }

    return res;
}

static void edgui_spinbox_updatevalue(edgui_obj_t * spinbox)
{
    edgui_spinbox_ext_t * ext = edgui_obj_get_ext_attr(spinbox);

    char buf[EDGUI_SPINBOX_MAX_DIGIT_COUNT + 8];
    memset(buf, 0, sizeof(buf));
    char * buf_p = buf;

    /*Add the sign*/
    (*buf_p) = ext->value >= 0 ? '+' : '-';
    buf_p++;

    int i;
    /*padding left*/
    for(i = 0; i < ext->digit_padding_left; i++) {
        (*buf_p) = ' ';
        buf_p++;
    }

    char digits[64];
    /*Convert the numbers to string (the sign is already handled so always covert positive number)*/
    edgui_utils_num_to_str(ext->value < 0 ? -ext->value : ext->value, digits);

    /*Add leading zeros*/
    int lz_cnt = ext->digit_count - (int)strlen(digits);
    if(lz_cnt > 0) {
        for(i = strlen(digits); i >= 0; i--) {
            digits[i + lz_cnt] = digits[i];
        }
        for(i = 0; i < lz_cnt; i++) {
            digits[i] = '0';
        }
    }

    int32_t intDigits;
    intDigits = (ext->dec_point_pos == 0) ? ext->digit_count : ext->dec_point_pos;

    /*Add the decimal part*/
    for(i = 0; i < intDigits && digits[i] != '\0'; i++) {
        (*buf_p) = digits[i];
        buf_p++;
    }

    if(ext->dec_point_pos != 0) {
        /*Insert the decimal point*/
        (*buf_p) = '.';
        buf_p++;

        for(/*Leave i*/; i < ext->digit_count && digits[i] != '\0'; i++) {
            (*buf_p) = digits[i];
            buf_p++;
        }
    }

    /*Refresh the text*/
    edgui_ta_set_text(spinbox, (char *)buf);

    /*Set the cursor position*/
    int32_t step    = ext->step;
    uint8_t cur_pos = ext->digit_count;
    while(step >= 10) {
        step /= 10;
        cur_pos--;
    }

    if(cur_pos > intDigits) cur_pos++; /*Skip teh decimal point*/

    cur_pos += ext->digit_padding_left;

    edgui_ta_set_cursor_pos(spinbox, cur_pos);
}

#endif
