#include <stdio.h>
#include <misc/util.h>
#include "imlib.h"
#include "xalloc.h"

#define align_ptr(p, a)                                                   \  
    (uint8_t *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))  

static kp_t *alloc_keypoint(uint16_t x, uint16_t y, uint16_t score)
{
    // Note must set keypoint descriptor to zeros
    kp_t *kpt = xalloc0(sizeof(kp_t));
    if (kpt == NULL)
    {
        return NULL;
    }
    
    kpt->x = x;
    kpt->y = y;
    kpt->score = score;
    return kpt;
}

static void makeOffsets(int pixel[25], int rowStride, int patternSize)
{
    static const int offsets[][2] =
    {
        {0,  3}, { 1,  3}, { 2,  2}, { 3,  1}, { 3, 0}, { 3, -1}, { 2, -2}, { 1, -3},
        {0, -3}, {-1, -3}, {-2, -2}, {-3, -1}, {-3, 0}, {-3,  1}, {-2,  2}, {-1,  3}
    };

    int k = 0;
    for( ; k < patternSize; k++ )
        pixel[k] = offsets[k][0] + offsets[k][1] * rowStride;
    for( ; k < 25; k++ )
        pixel[k] = pixel[k - patternSize];
}

static int cornerScore(const uint8_t *ptr, const int pixel[], int threshold)
{
    const int K = 8, N = K*3 + 1;
    int k, v = ptr[0];
    short d[N];
    for( k = 0; k < N; k++ )
        d[k] = (short)(v - ptr[pixel[k]]);

    int a0 = threshold;
    for( k = 0; k < 16; k += 2 )
    {
        int a = min((int)d[k+1], (int)d[k+2]);
        a = min(a, (int)d[k+3]);
        if( a <= a0 )
            continue;
        a = min(a, (int)d[k+4]);
        a = min(a, (int)d[k+5]);
        a = min(a, (int)d[k+6]);
        a = min(a, (int)d[k+7]);
        a = min(a, (int)d[k+8]);
        a0 = max(a0, min(a, (int)d[k]));
        a0 = max(a0, min(a, (int)d[k+9]));
    }

    int b0 = -a0;
    for( k = 0; k < 16; k += 2 )
    {
        int b = max((int)d[k+1], (int)d[k+2]);
        b = max(b, (int)d[k+3]);
        b = max(b, (int)d[k+4]);
        b = max(b, (int)d[k+5]);
        if( b >= b0 )
            continue;
        b = max(b, (int)d[k+6]);
        b = max(b, (int)d[k+7]);
        b = max(b, (int)d[k+8]);

        b0 = min(b0, max(b, (int)d[k]));
        b0 = min(b0, max(b, (int)d[k+9]));
    }

    threshold = -b0-1;

    return threshold;
}

void fast_detect(image_t *img, array_t *keypoints, int threshold, int max_keypoints, bool nonmax_suppression)
{
    const int K = 8, N = 16 + K + 1;
    int i, j, k, pixel[25];
    kp_t *kpt = NULL;
    
    makeOffsets(pixel, (int)img->w, 16);

    threshold = min(max(threshold, 0), 255);

    uint8_t threshold_tab[512];
    for( i = -255; i <= 255; i++ )
        threshold_tab[i+255] = (uint8_t)(i < -threshold ? 1 : i > threshold ? 2 : 0);

    uint8_t *_buf = xalloc0((img->w+16)*3*(sizeof(int) + sizeof(uint8_t)) + 128);
	if(_buf == NULL)
	{
	    return;
	}
	
    uint8_t *buf[3];
    buf[0] = _buf; buf[1] = buf[0] + img->w; buf[2] = buf[1] + img->w;
    int* cpbuf[3];
    cpbuf[0] = (int*)align_ptr(buf[2] + img->w, sizeof(int)) + 1;
    cpbuf[1] = cpbuf[0] + img->w + 1;
    cpbuf[2] = cpbuf[1] + img->w + 1;
    memset(buf[0], 0, img->w*3);

    for(i = 3; i < img->h-2; i++)
    {
        const uint8_t *ptr = img->pixels+(i * img->w + 3);
        uint8_t* curr = buf[(i - 3)%3];
        int* cornerpos = cpbuf[(i - 3)%3];
        memset(curr, 0, img->w);
        int ncorners = 0;

        if( i < img->h - 3 )
        {
            j = 3;
            for( ; j < img->w - 3; j++, ptr++ )
            {
                int v = ptr[0];
                const uint8_t* tab = &threshold_tab[0] - v + 255;
                int d = tab[ptr[pixel[0]]] | tab[ptr[pixel[8]]];

                if( d == 0 )
                    continue;

                d &= tab[ptr[pixel[2]]] | tab[ptr[pixel[10]]];
                d &= tab[ptr[pixel[4]]] | tab[ptr[pixel[12]]];
                d &= tab[ptr[pixel[6]]] | tab[ptr[pixel[14]]];

                if( d == 0 )
                    continue;

                d &= tab[ptr[pixel[1]]] | tab[ptr[pixel[9]]];
                d &= tab[ptr[pixel[3]]] | tab[ptr[pixel[11]]];
                d &= tab[ptr[pixel[5]]] | tab[ptr[pixel[13]]];
                d &= tab[ptr[pixel[7]]] | tab[ptr[pixel[15]]];

                if( d & 1 )
                {
                    int vt = v - threshold, count = 0;

                    for( k = 0; k < N; k++ )
                    {
                        int x = ptr[pixel[k]];
                        if(x < vt)
                        {
                            if( ++count > K )
                            {
                                cornerpos[ncorners++] = j;
                                if(nonmax_suppression)
                                    curr[j] = (uint8_t)cornerScore(ptr, pixel, threshold);
                                break;
                            }
                        }
                        else
                            count = 0;
                    }
                }

                if( d & 2 )
                {
                    int vt = v + threshold, count = 0;

                    for( k = 0; k < N; k++ )
                    {
                        int x = ptr[pixel[k]];
                        if(x > vt)
                        {
                            if( ++count > K )
                            {
                                cornerpos[ncorners++] = j;
                                if(nonmax_suppression)
                                    curr[j] = (uint8_t)cornerScore(ptr, pixel, threshold);
                                break;
                            }
                        }
                        else
                            count = 0;
                    }
                }
            }
        }

        cornerpos[-1] = ncorners;

        if( i == 3 )
            continue;

        const uint8_t* prev = buf[(i - 4 + 3)%3];
        const uint8_t* pprev = buf[(i - 5 + 3)%3];
        cornerpos = cpbuf[(i - 4 + 3)%3];
        ncorners = cornerpos[-1];

        for( k = 0; k < ncorners; k++ )
        {
            j = cornerpos[k];
            int score = prev[j];
            if( !nonmax_suppression ||
               (score > prev[j+1] && score > prev[j-1] &&
                score > pprev[j-1] && score > pprev[j] && score > pprev[j+1] &&
                score > curr[j-1] && score > curr[j] && score > curr[j+1]) )
            {
                kpt = alloc_keypoint(j, (i-1), score);
                if (kpt != NULL)
                {
                    array_push_back(keypoints, kpt);
                    if(array_length(keypoints) > max_keypoints)
                        goto out;
                }
            }
        }
    }

out:
    xfree(_buf);
    _buf = NULL;
}

