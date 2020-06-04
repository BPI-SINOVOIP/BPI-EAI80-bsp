/**
 * @file edgui_img_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_img_cache.h"
#include "edgui_img_decoder.h"
#include "edgui_draw_img.h"
#include "../edgui_hal/edgui_hal_tick.h"
#include "../edgui_misc/edgui_gc.h"

#if defined(EDGUI_GC_INCLUDE)
#include EDGUI_GC_INCLUDE
#endif /* EDGUI_ENABLE_GC */
/*********************
 *      DEFINES
 *********************/
/*Decrement life with this value in every open*/
#define EDGUI_IMG_CACHE_AGING 1

/*Boost life by this factor (multiply time_to_open with this value)*/
#define EDGUI_IMG_CACHE_LIFE_GAIN 1

/*Don't let life to be greater than this limit because it would require a lot of time to
 * "die" from very high values */
#define EDGUI_IMG_CACHE_LIFE_LIMIT 1000

#if EDGUI_IMG_CACHE_DEF_SIZE < 1
#error "EDGUI_IMG_CACHE_DEF_SIZE must be >= 1. See edgui_conf.h"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static uint16_t entry_cnt;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Open an image using the image decoder interface and cache it.
 * The image will be left open meaning if the image decoder open callback allocated memory then it will remain.
 * The image is closed if a new image is opened and the new image takes its place in the cache.
 * @param src source of the image. Path to file or pointer to an `edgui_img_dsc_t` variable
 * @param style style of the image
 * @return pointer to the cache entry or NULL if can open the image
 */
edgui_img_cache_entry_t * edgui_img_cache_open(const void * src, const edgui_style_t * style)
{
    if(entry_cnt == 0) {
        EDGUI_LOG_WARN("edgui_img_cache_open: the cache size is 0");
        return NULL;
    }

    edgui_img_cache_entry_t * cache = EDGUI_GC_ROOT(_edgui_img_cache_array);

    /*Decrement all lifes. Make the entries older*/
    uint16_t i;
    for(i = 0; i < entry_cnt; i++) {
        if(cache[i].life > INT32_MIN + EDGUI_IMG_CACHE_AGING) {
            cache[i].life -= EDGUI_IMG_CACHE_AGING;
        }
    }

    /*Is the image cached?*/
    edgui_img_cache_entry_t * cached_src = NULL;
    for(i = 0; i < entry_cnt; i++) {
        bool match = false;
        edgui_img_src_t src_type = edgui_img_src_get_type(cache[i].dec_dsc.src);
        if(src_type == EDGUI_IMG_SRC_VARIABLE) {
            if(cache[i].dec_dsc.src == src) match = true;
        } else if(src_type == EDGUI_IMG_SRC_FILE) {
            if(strcmp(cache[i].dec_dsc.src, src) == 0) match = true;
        }

        if(match) {
            /* If opened increment its life.
             * Image difficult to open should live longer to keep avoid frequent their recaching.
             * Therefore increase `life` with `time_to_open`*/
            cached_src = &cache[i];
            cached_src->life += cached_src->dec_dsc.time_to_open * EDGUI_IMG_CACHE_LIFE_GAIN;
            if(cached_src->life > EDGUI_IMG_CACHE_LIFE_LIMIT) cached_src->life = EDGUI_IMG_CACHE_LIFE_LIMIT;
            EDGUI_LOG_TRACE("image draw: image found in the cache");
            break;
        }
    }

    /*The image is not cached then cache it now*/
    if(cached_src == NULL) {
        /*Find an entry to reuse. Select the entry with the least life*/
        cached_src = &cache[0];
        for(i = 1; i < entry_cnt; i++) {
            if(cache[i].life < cached_src->life) {
                cached_src = &cache[i];
            }
        }

        /*Close the decoder to reuse if it was opened (has a valid source)*/
        if(cached_src->dec_dsc.src) {
            edgui_img_decoder_close(&cached_src->dec_dsc);
            EDGUI_LOG_INFO("image draw: cache miss, close and reuse an entry");
        } else {
            EDGUI_LOG_INFO("image draw: cache miss, cached to an empty entry");
        }

        /*Open the image and measure the time to open*/
        uint32_t t_start;
        t_start                          = edgui_tick_get();
        cached_src->dec_dsc.time_to_open = 0;
        edgui_res_t open_res                = edgui_img_decoder_open(&cached_src->dec_dsc, src, style);
        if(open_res == EDGUI_RES_INV) {
            EDGUI_LOG_WARN("Image draw cannot open the image resource");
            edgui_img_decoder_close(&cached_src->dec_dsc);
            memset(&cached_src->dec_dsc, 0, sizeof(edgui_img_decoder_dsc_t));
            memset(cached_src, 0, sizeof(edgui_img_cache_entry_t));
            cached_src->life = INT32_MIN; /*Make the empty entry very "weak" to force its use  */
            return NULL;
        }

        cached_src->life = 0;

        /*If `time_to_open` was not set in the open function set it here*/
        if(cached_src->dec_dsc.time_to_open == 0) {
            cached_src->dec_dsc.time_to_open = edgui_tick_elaps(t_start);
        }

        if(cached_src->dec_dsc.time_to_open == 0) cached_src->dec_dsc.time_to_open = 1;
    }

    return cached_src;
}

/**
 * Set the number of images to be cached.
 * More cached images mean more opened image at same time which might mean more memory usage.
 * E.g. if 20 PNG or JPG images are open in the RAM they consume memory while opened in the cache.
 * @param new_entry_cnt number of image to cache
 */
void edgui_img_cache_set_size(uint16_t new_entry_cnt)
{
    if(EDGUI_GC_ROOT(_edgui_img_cache_array) != NULL) {
        /*Clean the cache before free it*/
        edgui_img_cache_invalidate_src(NULL);
        edgui_mem_free(EDGUI_GC_ROOT(_edgui_img_cache_array));
    }

    /*Reallocate the cache*/
    EDGUI_GC_ROOT(_edgui_img_cache_array) = edgui_mem_alloc(sizeof(edgui_img_cache_entry_t) * new_entry_cnt);
    edgui_mem_assert(EDGUI_GC_ROOT(_edgui_img_cache_array));
    if(EDGUI_GC_ROOT(_edgui_img_cache_array) == NULL) {
        entry_cnt = 0;
        return;
    }
    entry_cnt = new_entry_cnt;

    /*Clean the cache*/
    uint16_t i;
    for(i = 0; i < entry_cnt; i++) {
        memset(&EDGUI_GC_ROOT(_edgui_img_cache_array)[i].dec_dsc, 0, sizeof(edgui_img_decoder_dsc_t));
        memset(&EDGUI_GC_ROOT(_edgui_img_cache_array)[i], 0, sizeof(edgui_img_cache_entry_t));
    }
}

/**
 * Invalidate an image source in the cache.
 * Useful if the image source is updated therefore it needs to be cached again.
 * @param src an image source path to a file or pointer to an `edgui_img_dsc_t` variable.
 */
void edgui_img_cache_invalidate_src(const void * src)
{

    edgui_img_cache_entry_t * cache = EDGUI_GC_ROOT(_edgui_img_cache_array);

    uint16_t i;
    for(i = 0; i < entry_cnt; i++) {
        if(cache[i].dec_dsc.src == src || src == NULL) {
            if(cache[i].dec_dsc.src != NULL) {
                edgui_img_decoder_close(&cache[i].dec_dsc);
            }

            memset(&cache[i].dec_dsc, 0, sizeof(edgui_img_decoder_dsc_t));
            memset(&cache[i], 0, sizeof(edgui_img_cache_entry_t));
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
