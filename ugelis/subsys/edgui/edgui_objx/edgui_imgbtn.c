/**
 * @file edgui_imgbtn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_imgbtn.h"
#if EDGUI_USE_IMGBTN != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_imgbtn_design(edgui_obj_t * imgbtn, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_imgbtn_signal(edgui_obj_t * imgbtn, edgui_signal_t sign, void * param);
static void refr_img(edgui_obj_t * imgbtn);

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
 * Create a image button object
 * @param par pointer to an object, it will be the parent of the new image button
 * @param copy pointer to a image button object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created image button
 */
edgui_obj_t * edgui_imgbtn_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("image button create started");

    /*Create the ancestor of image button*/
    edgui_obj_t * new_imgbtn = edgui_btn_create(par, copy);
    edgui_mem_assert(new_imgbtn);
    if(new_imgbtn == NULL) return NULL;

    /*Allocate the image button type specific extended data*/
    edgui_imgbtn_ext_t * ext = edgui_obj_allocate_ext_attr(new_imgbtn, sizeof(edgui_imgbtn_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_imgbtn);
    if(ancestor_design == NULL) ancestor_design = edgui_obj_get_design_cb(new_imgbtn);

        /*Initialize the allocated 'ext' */
#if EDGUI_IMGBTN_TILED == 0
    memset(ext->img_src, 0, sizeof(ext->img_src));
#else
    memset(ext->img_src_left, 0, sizeof(ext->img_src_left));
    memset(ext->img_src_mid, 0, sizeof(ext->img_src_mid));
    memset(ext->img_src_right, 0, sizeof(ext->img_src_right));
#endif

    ext->act_cf = EDGUI_IMG_CF_UNKNOWN;

    /*The signal and design functions are not copied so set them here*/
    edgui_obj_set_signal_cb(new_imgbtn, edgui_imgbtn_signal);
    edgui_obj_set_design_cb(new_imgbtn, edgui_imgbtn_design);

    /*Init the new image button image button*/
    if(copy == NULL) {

    }
    /*Copy an existing image button*/
    else {
        edgui_imgbtn_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
#if EDGUI_IMGBTN_TILED == 0
        memcpy(ext->img_src, copy_ext->img_src, sizeof(ext->img_src));
#else
        memcpy(ext->img_src_left, copy_ext->img_src_left, sizeof(ext->img_src_left));
        memcpy(ext->img_src_mid, copy_ext->img_src_mid, sizeof(ext->img_src_mid));
        memcpy(ext->img_src_right, copy_ext->img_src_right, sizeof(ext->img_src_right));
#endif
        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_imgbtn);
    }

    EDGUI_LOG_INFO("image button created");

    return new_imgbtn;
}

/*=====================
 * Setter functions
 *====================*/

#if EDGUI_IMGBTN_TILED == 0
/**
 * Set images for a state of the image button
 * @param imgbtn pointer to an image button object
 * @param state for which state set the new image (from `edgui_btn_state_t`) `
 * @param src pointer to an image source (a C array or path to a file)
 */
void edgui_imgbtn_set_src(edgui_obj_t * imgbtn, edgui_btn_state_t state, const void * src)
{
    edgui_imgbtn_ext_t * ext = edgui_obj_get_ext_attr(imgbtn);

    ext->img_src[state] = src;

    refr_img(imgbtn);
}

#else
/**
 * Set images for a state of the image button
 * @param imgbtn pointer to an image button object
 * @param state for which state set the new image (from `edgui_btn_state_t`) `
 * @param src_left pointer to an image source for the left side of the button (a C array or path to
 * a file)
 * @param src_mid pointer to an image source for the middle of the button (ideally 1px wide) (a C
 * array or path to a file)
 * @param src_right pointer to an image source for the right side of the button (a C array or path
 * to a file)
 */
void edgui_imgbtn_set_src(edgui_obj_t * imgbtn, edgui_btn_state_t state, const void * src_left, const void * src_mid,
                       const void * src_right)
{
    edgui_imgbtn_ext_t * ext = edgui_obj_get_ext_attr(imgbtn);

    ext->img_src_left[state] = src_left;
    ext->img_src_mid[state] = src_mid;
    ext->img_src_right[state] = src_right;

    refr_img(imgbtn);
}

#endif

/**
 * Set a style of a image button.
 * @param imgbtn pointer to image button object
 * @param type which style should be set
 * @param style pointer to a style
 */
void edgui_imgbtn_set_style(edgui_obj_t * imgbtn, edgui_imgbtn_style_t type, const edgui_style_t * style)
{
    edgui_btn_set_style(imgbtn, type, style);
}

/*=====================
 * Getter functions
 *====================*/

#if EDGUI_IMGBTN_TILED == 0
/**
 * Get the images in a  given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `edgui_btn_state_t`) `
 * @return pointer to an image source (a C array or path to a file)
 */
const void * edgui_imgbtn_get_src(edgui_obj_t * imgbtn, edgui_btn_state_t state)
{
    edgui_imgbtn_ext_t * ext = edgui_obj_get_ext_attr(imgbtn);

    return ext->img_src[state];
}
#else

/**
 * Get the left image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `edgui_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * edgui_imgbtn_get_src_left(edgui_obj_t * imgbtn, edgui_btn_state_t state)
{
    edgui_imgbtn_ext_t * ext = edgui_obj_get_ext_attr(imgbtn);

    return ext->img_src_left[state];
}

/**
 * Get the middle image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `edgui_btn_state_t`) `
 * @return pointer to the middle image source (a C array or path to a file)
 */
const void * edgui_imgbtn_get_src_middle(edgui_obj_t * imgbtn, edgui_btn_state_t state)
{
    edgui_imgbtn_ext_t * ext = edgui_obj_get_ext_attr(imgbtn);

    return ext->img_src_mid[state];
}

/**
 * Get the right image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `edgui_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * edgui_imgbtn_get_src_right(edgui_obj_t * imgbtn, edgui_btn_state_t state)
{
    edgui_imgbtn_ext_t * ext = edgui_obj_get_ext_attr(imgbtn);

    return ext->img_src_right[state];
}

#endif

/**
 * Get style of a image button.
 * @param imgbtn pointer to image button object
 * @param type which style should be get
 * @return style pointer to the style
 */
const edgui_style_t * edgui_imgbtn_get_style(const edgui_obj_t * imgbtn, edgui_imgbtn_style_t type)
{
    return edgui_btn_get_style(imgbtn, type);
}

/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the image buttons
 * @param imgbtn pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_imgbtn_design(edgui_obj_t * imgbtn, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == EDGUI_DESIGN_COVER_CHK) {
        edgui_imgbtn_ext_t * ext = edgui_obj_get_ext_attr(imgbtn);
        bool cover            = false;
        if(ext->act_cf == EDGUI_IMG_CF_TRUE_COLOR || ext->act_cf == EDGUI_IMG_CF_RAW) {
            cover = edgui_area_is_in(mask, &imgbtn->coords);
        }

        return cover;
    }
    /*Draw the object*/
    else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        /*Just draw an image*/
        edgui_imgbtn_ext_t * ext    = edgui_obj_get_ext_attr(imgbtn);
        edgui_btn_state_t state     = edgui_imgbtn_get_state(imgbtn);
        const edgui_style_t * style = edgui_imgbtn_get_style(imgbtn, state);
        edgui_opa_t opa_scale       = edgui_obj_get_opa_scale(imgbtn);

#if EDGUI_IMGBTN_TILED == 0
        const void * src = ext->img_src[state];
        edgui_draw_img(&imgbtn->coords, mask, src, style, opa_scale);
#else
        const void * src;
        edgui_img_header_t header;
        edgui_area_t coords;
        edgui_coord_t left_w = 0;
        edgui_coord_t right_w = 0;

        src = ext->img_src_left[state];
        if(src) {
            edgui_img_decoder_get_info(src, &header);
            left_w = header.w;
            coords.x1 = imgbtn->coords.x1;
            coords.y1 = imgbtn->coords.y1;
            coords.x2 = coords.x1 + header.w - 1;
            coords.y2 = coords.y1 + header.h - 1;
            edgui_draw_img(&coords, mask, src, style, opa_scale);
        }

        src = ext->img_src_right[state];
        if(src) {
            edgui_img_decoder_get_info(src, &header);
            right_w = header.w;
            coords.x1 = imgbtn->coords.x2 - header.w + 1;
            coords.y1 = imgbtn->coords.y1;
            coords.x2 = imgbtn->coords.x2;
            coords.y2 = imgbtn->coords.y1 + header.h - 1;
            edgui_draw_img(&coords, mask, src, style, opa_scale);
        }

        src = ext->img_src_mid[state];
        if(src) {
            edgui_coord_t obj_w = edgui_obj_get_width(imgbtn);
            edgui_coord_t i;
            edgui_img_decoder_get_info(src, &header);

            coords.x1 = imgbtn->coords.x1 + left_w;
            coords.y1 = imgbtn->coords.y1;
            coords.x2 = coords.x1 + header.w - 1;
            coords.y2 = imgbtn->coords.y1 + header.h - 1;

            for(i = 0; i < obj_w - right_w - left_w; i += header.w) {
                edgui_draw_img(&coords, mask, src, style, opa_scale);
                coords.x1 = coords.x2 + 1;
                coords.x2 += header.w;
            }
        }

#endif

    }
    /*Post draw when the children are drawn*/
    else if(mode == EDGUI_DESIGN_DRAW_POST) {
    }

    return true;
}

/**
 * Signal function of the image button
 * @param imgbtn pointer to a image button object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_imgbtn_signal(edgui_obj_t * imgbtn, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(imgbtn, sign, param);
    if(res != EDGUI_RES_OK) return res;

    if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        /* If the style changed then the button was clicked, released etc. so probably the state was
         * changed as well Set the new image for the new state.*/
        refr_img(imgbtn);
    } else if(sign == EDGUI_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_imgbtn";
    }

    return res;
}

static void refr_img(edgui_obj_t * imgbtn)
{
    edgui_imgbtn_ext_t * ext = edgui_obj_get_ext_attr(imgbtn);
    edgui_btn_state_t state  = edgui_imgbtn_get_state(imgbtn);
    edgui_img_header_t header;

#if EDGUI_IMGBTN_TILED == 0
    const void * src = ext->img_src[state];
#else
    const void * src = ext->img_src_mid[state];
#endif

    edgui_res_t info_res;
    info_res = edgui_img_decoder_get_info(src, &header);
    if(info_res == EDGUI_RES_OK) {
        ext->act_cf = header.cf;
#if EDGUI_IMGBTN_TILED == 0
        edgui_obj_set_size(imgbtn, header.w, header.h);
#else
        edgui_obj_set_height(imgbtn, header.h);
#endif
    } else {
        ext->act_cf = EDGUI_IMG_CF_UNKNOWN;
    }

    edgui_obj_invalidate(imgbtn);
}

#endif
