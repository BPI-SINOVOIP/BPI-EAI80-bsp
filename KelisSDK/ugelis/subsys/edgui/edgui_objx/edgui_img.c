/**
 * @file edgui_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_img.h"
#if EDGUI_USE_IMG != 0

/*Testing of dependencies*/
#if EDGUI_USE_LABEL == 0
#error "edgui_img: edgui_label is required. Enable it in edgui_conf.h (EDGUI_USE_LABEL  1) "
#endif

#include "../edgui_themes/edgui_theme.h"
#include "../edgui_draw/edgui_img_decoder.h"
#include "../edgui_misc/edgui_fs.h"
#include "../edgui_misc/edgui_txt.h"
#include "../edgui_misc/edgui_log.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool edgui_img_design(edgui_obj_t * img, const edgui_area_t * mask, edgui_design_mode_t mode);
static edgui_res_t edgui_img_signal(edgui_obj_t * img, edgui_signal_t sign, void * param);

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
 * Create an image objects
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a image object, if not NULL then the new object will be copied from it
 * @return pointer to the created image
 */
edgui_obj_t * edgui_img_create(edgui_obj_t * par, const edgui_obj_t * copy)
{
    EDGUI_LOG_TRACE("image create started");

    edgui_obj_t * new_img = NULL;

    /*Create a basic object*/
    new_img = edgui_obj_create(par, copy);
    edgui_mem_assert(new_img);
    if(new_img == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = edgui_obj_get_signal_cb(new_img);

    /*Extend the basic object to image object*/
    edgui_img_ext_t * ext = edgui_obj_allocate_ext_attr(new_img, sizeof(edgui_img_ext_t));
    edgui_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->src       = NULL;
    ext->src_type  = EDGUI_IMG_SRC_UNKNOWN;
    ext->cf        = EDGUI_IMG_CF_UNKNOWN;
    ext->w         = edgui_obj_get_width(new_img);
    ext->h         = edgui_obj_get_height(new_img);
    ext->auto_size = 1;
    ext->offset.x  = 0;
    ext->offset.y  = 0;

    /*Init the new object*/
    edgui_obj_set_signal_cb(new_img, edgui_img_signal);
    edgui_obj_set_design_cb(new_img, edgui_img_design);

    if(copy == NULL) {
        edgui_obj_set_click(new_img, false);
        /* Enable auto size for non screens
         * because image screens are wallpapers
         * and must be screen sized*/
        if(par != NULL) {
            ext->auto_size = 1;
            edgui_obj_set_style(new_img, NULL); /*Inherit the style  by default*/
        } else {
            ext->auto_size = 0;
            edgui_obj_set_style(new_img, &edgui_style_plain); /*Set a style for screens*/
        }
    } else {
        edgui_img_ext_t * copy_ext = edgui_obj_get_ext_attr(copy);
        ext->auto_size          = copy_ext->auto_size;
        edgui_img_set_src(new_img, copy_ext->src);

        /*Refresh the style with new signal function*/
        edgui_obj_refresh_style(new_img);
    }

    EDGUI_LOG_INFO("image created");

    return new_img;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the pixel map to display by the image
 * @param img pointer to an image object
 * @param data the image data
 */
void edgui_img_set_src(edgui_obj_t * img, const void * src_img)
{
    edgui_img_src_t src_type = edgui_img_src_get_type(src_img);
    edgui_img_ext_t * ext    = edgui_obj_get_ext_attr(img);

#if EDGUI_USE_LOG && EDGUI_LOG_LEVEL >= EDGUI_LOG_LEVEL_INFO
    switch(src_type) {
        case EDGUI_IMG_SRC_FILE: EDGUI_LOG_TRACE("edgui_img_set_src: `EDGUI_IMG_SRC_FILE` type found"); break;
        case EDGUI_IMG_SRC_VARIABLE: EDGUI_LOG_TRACE("edgui_img_set_src: `EDGUI_IMG_SRC_VARIABLE` type found"); break;
        case EDGUI_IMG_SRC_SYMBOL: EDGUI_LOG_TRACE("edgui_img_set_src: `EDGUI_IMG_SRC_SYMBOL` type found"); break;
        default: EDGUI_LOG_WARN("edgui_img_set_src: unknown type");
    }
#endif

    /*If the new source type is unknown free the memories of the old source*/
    if(src_type == EDGUI_IMG_SRC_UNKNOWN) {
        EDGUI_LOG_WARN("edgui_img_set_src: unknown image type");
        if(ext->src_type == EDGUI_IMG_SRC_SYMBOL || ext->src_type == EDGUI_IMG_SRC_FILE) {
            edgui_mem_free(ext->src);
        }
        ext->src      = NULL;
        ext->src_type = EDGUI_IMG_SRC_UNKNOWN;
        return;
    }

    edgui_img_header_t header;
    edgui_img_decoder_get_info(src_img, &header);

    /*Save the source*/
    if(src_type == EDGUI_IMG_SRC_VARIABLE) {
        EDGUI_LOG_INFO("edgui_img_set_src:  `EDGUI_IMG_SRC_VARIABLE` type found");

        /*If memory was allocated because of the previous `src_type` then free it*/
        if(ext->src_type == EDGUI_IMG_SRC_FILE || ext->src_type == EDGUI_IMG_SRC_SYMBOL) {
            edgui_mem_free(ext->src);
        }
        ext->src = src_img;
    } else if(src_type == EDGUI_IMG_SRC_FILE || src_type == EDGUI_IMG_SRC_SYMBOL) {
        /* If the new and the old src are the same then it was only a refresh.*/
        if(ext->src != src_img) {
            const void * old_src = NULL;
            /* If memory was allocated because of the previous `src_type` then save its pointer and free after allocation.
             * It's important to allocate first to be sure the new data will be on a new address.
             * Else `img_cache` wouldn't see the change in source.*/
            if(ext->src_type == EDGUI_IMG_SRC_FILE || ext->src_type == EDGUI_IMG_SRC_SYMBOL) {
                old_src = ext->src;
            }
            char * new_str = edgui_mem_alloc(strlen(src_img) + 1);
            edgui_mem_assert(new_str);
            if(new_str == NULL) return;
            strcpy(new_str, src_img);
            ext->src = new_str;

            if(old_src) edgui_mem_free(old_src);
        }
    }

    if(src_type == EDGUI_IMG_SRC_SYMBOL) {
        /*`edgui_img_dsc_get_info` couldn't set the with and height of a font so set it here*/
        const edgui_style_t * style = edgui_img_get_style(img, EDGUI_IMG_STYLE_MAIN);
        edgui_point_t size;
        edgui_txt_get_size(&size, src_img, style->text.font, style->text.letter_space, style->text.line_space,
                        EDGUI_COORD_MAX, EDGUI_TXT_FLAG_NONE);
        header.w = size.x;
        header.h = size.y;
    }

    ext->src_type = src_type;
    ext->w        = header.w;
    ext->h        = header.h;
    ext->cf       = header.cf;

    if(edgui_img_get_auto_size(img) != false) {
        edgui_obj_set_size(img, ext->w, ext->h);
    }

    edgui_obj_invalidate(img);
}

/**
 * Enable the auto size feature.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image
 * @param en true: auto size enable, false: auto size disable
 */
void edgui_img_set_auto_size(edgui_obj_t * img, bool en)
{
    edgui_img_ext_t * ext = edgui_obj_get_ext_attr(img);

    ext->auto_size = (en == false ? 0 : 1);
}

/**
 * Set an offset for the source of an image.
 * so the image will be displayed from the new origin.
 * @param img pointer to an image
 * @param x: the new offset along x axis.
 */
void edgui_img_set_offset_x(edgui_obj_t * img, edgui_coord_t x)
{
    edgui_img_ext_t * ext = edgui_obj_get_ext_attr(img);

    if(x < ext->w - 1) {
        ext->offset.x = x;
        edgui_obj_invalidate(img);
    }
}

/**
 * Set an offset for the source of an image.
 * so the image will be displayed from the new origin.
 * @param img pointer to an image
 * @param y: the new offset along y axis.
 */
void edgui_img_set_offset_y(edgui_obj_t * img, edgui_coord_t y)
{
    edgui_img_ext_t * ext = edgui_obj_get_ext_attr(img);

    if(y < ext->h - 1) {
        ext->offset.y = y;
        edgui_obj_invalidate(img);
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the source of the image
 * @param img pointer to an image object
 * @return the image source (symbol, file name or C array)
 */
const void * edgui_img_get_src(edgui_obj_t * img)
{
    edgui_img_ext_t * ext = edgui_obj_get_ext_attr(img);

    return ext->src;
}

/**
 * Get the name of the file set for an image
 * @param img pointer to an image
 * @return file name
 */
const char * edgui_img_get_file_name(const edgui_obj_t * img)
{
    edgui_img_ext_t * ext = edgui_obj_get_ext_attr(img);

    if(ext->src_type == EDGUI_IMG_SRC_FILE)
        return ext->src;
    else
        return "";
}

/**
 * Get the auto size enable attribute
 * @param img pointer to an image
 * @return true: auto size is enabled, false: auto size is disabled
 */
bool edgui_img_get_auto_size(const edgui_obj_t * img)
{
    edgui_img_ext_t * ext = edgui_obj_get_ext_attr(img);

    return ext->auto_size == 0 ? false : true;
}

/**
 * Get the offset.x attribute of the img object.
 * @param img pointer to an image
 * @return offset.x value.
 */
edgui_coord_t edgui_img_get_offset_x(edgui_obj_t * img)
{
    edgui_img_ext_t * ext = edgui_obj_get_ext_attr(img);

    return ext->offset.x;
}

/**
 * Get the offset.y attribute of the img object.
 * @param img pointer to an image
 * @return offset.y value.
 */
edgui_coord_t edgui_img_get_offset_y(edgui_obj_t * img)
{
    edgui_img_ext_t * ext = edgui_obj_get_ext_attr(img);

    return ext->offset.y;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the images
 * @param img pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode EDGUI_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             EDGUI_DESIGN_DRAW: draw the object (always return 'true')
 *             EDGUI_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool edgui_img_design(edgui_obj_t * img, const edgui_area_t * mask, edgui_design_mode_t mode)
{
    const edgui_style_t * style = edgui_obj_get_style(img);
    edgui_img_ext_t * ext       = edgui_obj_get_ext_attr(img);

    if(mode == EDGUI_DESIGN_COVER_CHK) {
        bool cover = false;
        if(ext->src_type == EDGUI_IMG_SRC_UNKNOWN || ext->src_type == EDGUI_IMG_SRC_SYMBOL) return false;

        if(ext->cf == EDGUI_IMG_CF_TRUE_COLOR || ext->cf == EDGUI_IMG_CF_RAW) cover = edgui_area_is_in(mask, &img->coords);

        return cover;
    } else if(mode == EDGUI_DESIGN_DRAW_MAIN) {
        if(ext->h == 0 || ext->w == 0) return true;
        edgui_area_t coords;
        edgui_opa_t opa_scale = edgui_obj_get_opa_scale(img);

        edgui_obj_get_coords(img, &coords);

        if(ext->src_type == EDGUI_IMG_SRC_FILE || ext->src_type == EDGUI_IMG_SRC_VARIABLE) {
            coords.x1 -= ext->offset.x;
            coords.y1 -= ext->offset.y;

            EDGUI_LOG_TRACE("edgui_img_design: start to draw image");
            edgui_area_t cords_tmp;
            cords_tmp.y1 = coords.y1;
            cords_tmp.y2 = coords.y1 + ext->h - 1;

            for(; cords_tmp.y1 < coords.y2; cords_tmp.y1 += ext->h, cords_tmp.y2 += ext->h) {
                cords_tmp.x1 = coords.x1;
                cords_tmp.x2 = coords.x1 + ext->w - 1;
                for(; cords_tmp.x1 < coords.x2; cords_tmp.x1 += ext->w, cords_tmp.x2 += ext->w) {
                    edgui_draw_img(&cords_tmp, mask, ext->src, style, opa_scale);
                }
            }
        } else if(ext->src_type == EDGUI_IMG_SRC_SYMBOL) {
            EDGUI_LOG_TRACE("edgui_img_design: start to draw symbol");
            edgui_style_t style_mod;
            edgui_style_copy(&style_mod, style);
            style_mod.text.color = style->image.color;
            edgui_draw_label(&coords, mask, &style_mod, opa_scale, ext->src, EDGUI_TXT_FLAG_NONE, NULL, -1, -1, NULL);
        } else {
            /*Trigger the error handler of image drawer*/
            EDGUI_LOG_WARN("edgui_img_design: image source type is unknown");
            edgui_draw_img(&img->coords, mask, NULL, style, opa_scale);
        }
    }

    return true;
}

/**
 * Signal function of the image
 * @param img pointer to an image object
 * @param sign a signal type from edgui_signal_t enum
 * @param param pointer to a signal specific variable
 * @return EDGUI_RES_OK: the object is not deleted in the function; EDGUI_RES_INV: the object is deleted
 */
static edgui_res_t edgui_img_signal(edgui_obj_t * img, edgui_signal_t sign, void * param)
{
    edgui_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(img, sign, param);
    if(res != EDGUI_RES_OK) return res;

    edgui_img_ext_t * ext = edgui_obj_get_ext_attr(img);
    if(sign == EDGUI_SIGNAL_CLEANUP) {
        if(ext->src_type == EDGUI_IMG_SRC_FILE || ext->src_type == EDGUI_IMG_SRC_SYMBOL) {
            edgui_mem_free(ext->src);
            ext->src      = NULL;
            ext->src_type = EDGUI_IMG_SRC_UNKNOWN;
        }
    } else if(sign == EDGUI_SIGNAL_STYLE_CHG) {
        /*Refresh the file name to refresh the symbol text size*/
        if(ext->src_type == EDGUI_IMG_SRC_SYMBOL) {
            edgui_img_set_src(img, ext->src);
        }
    } else if(sign == EDGUI_SIGNAL_GET_TYPE) {
        edgui_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < EDGUI_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "edgui_img";
    }

    return res;
}

#endif
