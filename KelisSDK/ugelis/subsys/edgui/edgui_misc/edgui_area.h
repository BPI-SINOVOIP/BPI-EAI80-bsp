/**
 * @file edgui_area.h
 *
 */

#ifndef EDGUI_AREA_H
#define EDGUI_AREA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui_conf.h"
#else
#include "../edgui/edgui_conf.h"
#endif

/*********************
 *      DEFINES
 *********************/
/*To avoid overflow don't let the max ranges (reduce with 1000) */
#define EDGUI_COORD_MAX ((edgui_coord_t)((uint32_t)((uint32_t)1 << (8 * sizeof(edgui_coord_t) - 1)) - 1000))
#define EDGUI_COORD_MIN (-EDGUI_COORD_MAX)

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Represents a point on the screen.
 */
typedef struct
{
    edgui_coord_t x;
    edgui_coord_t y;
} edgui_point_t;

/** Represents an area of the screen. */
typedef struct
{
    edgui_coord_t x1;
    edgui_coord_t y1;
    edgui_coord_t x2;
    edgui_coord_t y2;
} edgui_area_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize an area
 * @param area_p pointer to an area
 * @param x1 left coordinate of the area
 * @param y1 top coordinate of the area
 * @param x2 right coordinate of the area
 * @param y2 bottom coordinate of the area
 */
void edgui_area_set(edgui_area_t * area_p, edgui_coord_t x1, edgui_coord_t y1, edgui_coord_t x2, edgui_coord_t y2);

/**
 * Copy an area
 * @param dest pointer to the destination area
 * @param src pointer to the source area
 */
static __inline void edgui_area_copy(edgui_area_t * dest, const edgui_area_t * src)
{
    memcpy(dest, src, sizeof(edgui_area_t));
}

/**
 * Get the width of an area
 * @param area_p pointer to an area
 * @return the width of the area (if x1 == x2 -> width = 1)
 */
static __inline edgui_coord_t edgui_area_get_width(const edgui_area_t * area_p)
{
    return area_p->x2 - area_p->x1 + 1;
}

/**
 * Get the height of an area
 * @param area_p pointer to an area
 * @return the height of the area (if y1 == y2 -> height = 1)
 */
static __inline edgui_coord_t edgui_area_get_height(const edgui_area_t * area_p)
{
    return area_p->y2 - area_p->y1 + 1;
}

/**
 * Set the width of an area
 * @param area_p pointer to an area
 * @param w the new width of the area (w == 1 makes x1 == x2)
 */
void edgui_area_set_width(edgui_area_t * area_p, edgui_coord_t w);

/**
 * Set the height of an area
 * @param area_p pointer to an area
 * @param h the new height of the area (h == 1 makes y1 == y2)
 */
void edgui_area_set_height(edgui_area_t * area_p, edgui_coord_t h);

/**
 * Set the position of an area (width and height will be kept)
 * @param area_p pointer to an area
 * @param x the new x coordinate of the area
 * @param y the new y coordinate of the area
 */
void edgui_area_set_pos(edgui_area_t * area_p, edgui_coord_t x, edgui_coord_t y);

/**
 * Return with area of an area (x * y)
 * @param area_p pointer to an area
 * @return size of area
 */
uint32_t edgui_area_get_size(const edgui_area_t * area_p);

/**
 * Get the common parts of two areas
 * @param res_p pointer to an area, the result will be stored her
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 * @return false: the two area has NO common parts, res_p is invalid
 */
bool edgui_area_intersect(edgui_area_t * res_p, const edgui_area_t * a1_p, const edgui_area_t * a2_p);

/**
 * Join two areas into a third which involves the other two
 * @param res_p pointer to an area, the result will be stored here
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 */
void edgui_area_join(edgui_area_t * a_res_p, const edgui_area_t * a1_p, const edgui_area_t * a2_p);

/**
 * Check if a point is on an area
 * @param a_p pointer to an area
 * @param p_p pointer to a point
 * @return false:the point is out of the area
 */
bool edgui_area_is_point_on(const edgui_area_t * a_p, const edgui_point_t * p_p);

/**
 * Check if two area has common parts
 * @param a1_p pointer to an area.
 * @param a2_p pointer to an other area
 * @return false: a1_p and a2_p has no common parts
 */
bool edgui_area_is_on(const edgui_area_t * a1_p, const edgui_area_t * a2_p);

/**
 * Check if an area is fully on an other
 * @param ain_p pointer to an area which could be on aholder_p
 * @param aholder pointer to an area which could involve ain_p
 * @return
 */
bool edgui_area_is_in(const edgui_area_t * ain_p, const edgui_area_t * aholder_p);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
