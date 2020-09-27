/**
 * @file edgui_test_canvas.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "edgui_test_canvas.h"

#if EDGUI_USE_CANVAS && EDGUI_USE_TESTS

/*********************
 *      DEFINES
 *********************/
#define CANVAS_WIDTH    100
#define CANVAS_HEIGHT   100
#define TEST_ROTATE     0

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create canvas to test its functionalities
 */
void edgui_test_canvas_1(void)
{
    static edgui_style_t style;
    edgui_style_copy(&style, &edgui_style_plain);
    style.body.main_color = EDGUI_COLOR_RED;
    style.body.grad_color = EDGUI_COLOR_MAROON;
    style.body.radius = 4;
    style.body.border.width = 2;
    style.body.border.color = EDGUI_COLOR_WHITE;
    style.body.shadow.color = EDGUI_COLOR_WHITE;
    style.body.shadow.width = 4;
    style.line.width = 2;
    style.line.color = EDGUI_COLOR_BLACK;
    style.text.color = EDGUI_COLOR_BLUE;

    edgui_obj_t * canvas = edgui_canvas_create(edgui_scr_act(), NULL);
    static edgui_color_t cbuf[EDGUI_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];
    edgui_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, EDGUI_IMG_CF_TRUE_COLOR);
    edgui_obj_set_pos(canvas, 10, 10);
    edgui_canvas_fill_bg(canvas, EDGUI_COLOR_SILVER);

    edgui_canvas_draw_rect(canvas, 40, 10, 50, 30, &style);

    edgui_canvas_draw_text(canvas, 5, 5, 100, &style, "ABC", EDGUI_LABEL_ALIGN_LEFT);

    const edgui_point_t points[] = {{5, 40}, {35, 45}, {30, 80}, {10, 90}, {5, 40}};

    edgui_canvas_draw_polygon(canvas, points, 5, &style);
    edgui_canvas_draw_line(canvas, points, 5, &style);

    edgui_canvas_draw_arc(canvas, 70, 70, 20, 20, 250, &style);

#if TEST_ROTATE
    /*Copy the current image to buffer and rotate it to the canvas */
    edgui_color_t cbuf_tmp[CANVAS_WIDTH * CANVAS_HEIGHT];
    memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
    edgui_img_dsc_t img;
    img.data = (void *)cbuf_tmp;
    img.header.cf = EDGUI_IMG_CF_TRUE_COLOR;
    img.header.w = CANVAS_WIDTH;
    img.header.h = CANVAS_HEIGHT;

    edgui_canvas_fill_bg(canvas, EDGUI_COLOR_SILVER);
    edgui_canvas_rotate(canvas, &img, 30, 0, 0, CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*EDGUI_USE_CANVAS && EDGUI_USE_TESTS*/
