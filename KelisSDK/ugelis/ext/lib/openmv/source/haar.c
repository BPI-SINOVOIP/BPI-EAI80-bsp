/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Viola-Jones object detector implementation.
 * Original Author: Francesco Comaschi (f.comaschi@tue.nl)
 *
 */
#include <stdio.h>
#include <misc/util.h>
#include "xalloc.h"
#include "imlib.h"
// built-in cascades
#include "cascade.h"

static int eval_weak_classifier(cascade_t *cascade, point_t pt, int t_idx, int w_idx, int r_idx)
{
    int32_t sumw=0;
    mw_image_t *sum = cascade->sum;

    /* The node threshold is multiplied by the standard deviation of the sub window */
    int32_t t = cascade->tree_thresh_array[t_idx] * cascade->std;

    for (int i=0; i<cascade->num_rectangles_array[t_idx]; i++) {
        int x = cascade->rectangles_array[r_idx + (i<<2) + 0];
        int y = cascade->rectangles_array[r_idx + (i<<2) + 1];
        int w = cascade->rectangles_array[r_idx + (i<<2) + 2];
        int h = cascade->rectangles_array[r_idx + (i<<2) + 3];
        // Lookup the feature
        sumw += imlib_integral_mw_lookup(sum, pt.x+x, y, w, h) * (cascade->weights_array[w_idx + i]<<12);
    }

    if (sumw >= t) {
        return cascade->alpha2_array[t_idx];
    }

    return cascade->alpha1_array[t_idx];
}

static int run_cascade_classifier(cascade_t* cascade, point_t pt)
{
    int win_w = cascade->window.w;
    int win_h = cascade->window.h;
    uint32_t n = (win_w * win_h);
    uint32_t i_s = imlib_integral_mw_lookup (cascade->sum, pt.x, 0, win_w, win_h);
    uint32_t i_sq = imlib_integral_mw_lookup(cascade->ssq, pt.x, 0, win_w, win_h);
    uint32_t m = i_s/n;
    uint32_t v = i_sq/n-(m*m);

    // Skip homogeneous regions.
    if (v<(50*50)) {
        return 0;
    }

    cascade->std = fast_sqrtf(i_sq*n-(i_s*i_s));
    for (int i=0, w_idx=0, r_idx=0, t_idx=0; i<cascade->n_stages; i++) {
        int stage_sum = 0;
        for (int j=0; j<cascade->stages_array[i]; j++, t_idx++) {
            // Send the shifted window to a haar filter
            stage_sum += eval_weak_classifier(cascade, pt, t_idx, w_idx, r_idx);
            w_idx += cascade->num_rectangles_array[t_idx];
            r_idx += cascade->num_rectangles_array[t_idx] * 4;
        }
        // If the sum is below the stage threshold, no objects were detected
        if (stage_sum < (cascade->threshold * cascade->stages_thresh_array[i])) {
            return 0;
        }
    }
    return 1;
}

array_t *imlib_detect_objects(image_t *image, cascade_t *cascade, rectangle_t *roi)
{
    // Integral images
    mw_image_t sum;
    mw_image_t ssq;

    // Detected objects array
    array_t *objects = NULL;

    // Allocate the objects array
    array_alloc(&objects, xfree);
    if(objects == NULL)
    {
        return NULL;
    }
    
    // Set cascade image pointers
    cascade->img = image;
    cascade->sum = &sum;
    cascade->ssq = &ssq;

    // Set scanning step.
    // Viola and Jones achieved best results using a scaling factor
    // of 1.25 and a scanning factor proportional to the current scale.
    // Start with a step of 5% of the image width and reduce at each scaling step
    cascade->step = (roi->w*50)/1000;

    // Make sure step is less than window height + 1
    if (cascade->step > cascade->window.h) {
        cascade->step = cascade->window.h;
    }

    // Allocate integral images
    imlib_integral_mw_alloc(&sum, roi->w, cascade->window.h+1);
    imlib_integral_mw_alloc(&ssq, roi->w, cascade->window.h+1);

    // Iterate over the image pyramid
    for(float factor=2.0f; ; factor *= cascade->scale_factor) {
        // Set the scaled width and height
        int szw = roi->w/factor;
        int szh = roi->h/factor;

        // Break if scaled image is smaller than feature size
        if (szw < cascade->window.w || szh < cascade->window.h) {
            break;
        }

        // Set the integral images scale
        imlib_integral_mw_scale(roi, &sum, szw, szh);
        imlib_integral_mw_scale(roi, &ssq, szw, szh);

        // Compute new scaled integral images
        imlib_integral_mw_ss(image, &sum, &ssq, roi);

        // Scale the scanning step
        cascade->step = cascade->step/factor;
        cascade->step = (cascade->step == 0) ? 1 : cascade->step;

        // Process image at the current scale
        // When filter window shifts to borders, some margin need to be kept
        int y2 = szh - cascade->window.h;
        int x2 = szw - cascade->window.w;

        // Shift the filter window over the image.
        for (int y=0; y<y2; y+=cascade->step) {
            for (int x=0; x<x2; x+=cascade->step) {
                point_t p = {x, y};
                // If an object is detected, record the coordinates of the filter window
                if (run_cascade_classifier(cascade, p) > 0) {
                    array_push_back(objects,
                        rectangle_alloc(fast_roundf(x*factor) + roi->x, fast_roundf(y*factor) + roi->y,
                        fast_roundf(cascade->window.w*factor), fast_roundf(cascade->window.h*factor)));
                }
            }

            // If not last line, shift integral images
            if ((y+cascade->step) < y2) {
                imlib_integral_mw_shift_ss(image, &sum, &ssq, roi, cascade->step);
            }
        }
    }

    imlib_integral_mw_free(&ssq);
    imlib_integral_mw_free(&sum);

    if (array_length(objects) > 1)   {
        // Merge objects detected at different scales
        objects = rectangle_merge(objects);
    }

    return objects;
}

void imlib_load_cascade(cascade_t *cascade, float threshold, float scale_factor)
{
    int i;
    // built-in cascade
    cascade->window.w            = frontalface_window_w;
    cascade->window.h            = frontalface_window_h;
    cascade->n_stages            = frontalface_n_stages;
    cascade->stages_array        = (uint8_t *)frontalface_stages_array;
    cascade->stages_thresh_array = (int16_t *)frontalface_stages_thresh_array;
    cascade->tree_thresh_array   = (int16_t *)frontalface_tree_thresh_array;
    cascade->alpha1_array        = (int16_t *)frontalface_alpha1_array;
    cascade->alpha2_array        = (int16_t *)frontalface_alpha2_array;
    cascade->num_rectangles_array= (int8_t  *)frontalface_num_rectangles_array;
    cascade->weights_array       = (int8_t  *)frontalface_weights_array;
    cascade->rectangles_array    = (int8_t  *)frontalface_rectangles_array;

    cascade->scale_factor = scale_factor;
    cascade->threshold = threshold;
    
    // sum the number of features in all stages
    for (i=0, cascade->n_features=0; i<cascade->n_stages; i++) {
        cascade->n_features += cascade->stages_array[i];
    }

    // sum the number of recatngles in all features
    for (i=0, cascade->n_rectangles=0; i<cascade->n_features; i++) {
        cascade->n_rectangles += cascade->num_rectangles_array[i];
    }
}
