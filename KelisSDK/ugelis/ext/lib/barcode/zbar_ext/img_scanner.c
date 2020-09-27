/*------------------------------------------------------------------------
 *  Copyright 2007-2009 (c) Jeff Brown <spadix@users.sourceforge.net>
 *
 *  This file is part of the ZBar Bar Code Reader.
 *
 *  The ZBar Bar Code Reader is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation; either version 2.1 of
 *  the License, or (at your option) any later version.
 *
 *  The ZBar Bar Code Reader is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser Public License
 *  along with the ZBar Bar Code Reader; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *  Boston, MA  02110-1301  USA
 *
 *  http://sourceforge.net/projects/zbar
 *------------------------------------------------------------------------*/

#include <config_zbar.h>
//#include <unistd.h>
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#include <stdlib.h>     /* malloc, free */
//#include <time.h>       /* clock_gettime */
//#include <sys/time.h>   /* gettimeofday */
#include <string.h>     /* memcmp, memset, memcpy */
#include <assert.h>

#include <zbar.h>
#include "error.h"
#include "image.h"
#ifdef ENABLE_QRCODE
# include "qrcode.h"
#endif
#include "img_scanner.h"
#include "svg.h"

#ifdef CONFIG_QR_DUAL_EN
#include <rpc_core.h>
#include <rpc_plugin_qr.h>
#endif

//#define ARMC4_OPT /* todo: open this macro has crash problem in dual solution */

#define TIME_DEBUG
#ifdef TIME_DEBUG
#include <ugelis.h>


volatile int32_t time_zbar_scan_prepare = 0;
volatile int32_t time_zbar_scan_Y = 0;
volatile int32_t time_zbar_scan_X = 0;
volatile int32_t time_zbar_scan_XY = 0;
volatile int32_t time_zbar_scan_decode = 0;
volatile int32_t time_zbar_scan_remainder = 0;

volatile int32_t time_zbar_scan_Y_prepare = 0;
volatile int32_t time_zbar_scan_Y_scan_y_0 = 0;
volatile int32_t time_zbar_scan_Y_scan_y_1 = 0;

#endif

#ifdef ENABLE_BARCODE_ZOOM
barcode_detected_info_t barcode_detected_info;
#endif


#if 0
# define ASSERT_POS \
    assert(p == data + x + y * (intptr_t)w)
#else
//# define ASSERT_POS
void assert_test(void *p, void *data, int x, int y, int w)
{
    if(p != data + x + y * (intptr_t)w)
    {
        printf("ASSERT_POS!!\n");
    }
}
# define ASSERT_POS assert_test(p, data, x, y, w)
//# define ASSERT_POS
#endif

/* FIXME cache setting configurability */

/* number of times the same result must be detected
 * in "nearby" images before being reported
 */
#define CACHE_CONSISTENCY    3 /* images */

/* time interval for which two images are considered "nearby"
 */
#define CACHE_PROXIMITY   1000 /* ms */

/* time that a result must *not* be detected before
 * it will be reported again
 */
#define CACHE_HYSTERESIS  2000 /* ms */

/* time after which cache entries are invalidated
 */
#define CACHE_TIMEOUT     (CACHE_HYSTERESIS * 2) /* ms */

#define NUM_SCN_CFGS (ZBAR_CFG_Y_DENSITY - ZBAR_CFG_X_DENSITY + 1)

#define CFG(iscn, cfg) ((iscn)->configs[(cfg) - ZBAR_CFG_X_DENSITY])
#define TEST_CFG(iscn, cfg) (((iscn)->config >> ((cfg) - ZBAR_CFG_POSITION)) & 1)

#ifndef NO_STATS
# define STAT(x) iscn->stat_##x++
#else
# define STAT(...)
# define dump_stats(...)
#endif

#define RECYCLE_BUCKETS     5

typedef struct recycle_bucket_s {
    int nsyms;
    zbar_symbol_t *head;
} recycle_bucket_t;

/* image scanner state */
struct zbar_image_scanner_s {
    zbar_scanner_t *scn;        /* associated linear intensity scanner */
    zbar_decoder_t *dcode;      /* associated symbol decoder */
#ifdef ENABLE_QRCODE
    qr_reader *qr;              /* QR Code 2D reader */
#endif

    const void *userdata;       /* application data */
    /* user result callback */
    zbar_image_data_handler_t *handler;

    unsigned long time;         /* scan start time */
    zbar_image_t *img;          /* currently scanning image *root* */
    int dx, dy, du, umin, v;    /* current scan direction */
    zbar_symbol_set_t *syms;    /* previous decode results */
    /* recycled symbols in 4^n size buckets */
    recycle_bucket_t recycle[RECYCLE_BUCKETS];

    int enable_cache;           /* current result cache state */
    zbar_symbol_t *cache;       /* inter-image result cache entries */

    /* configuration settings */
    unsigned config;            /* config flags */
    int configs[NUM_SCN_CFGS];  /* int valued configurations */

#ifndef NO_STATS
    int stat_syms_new;
    int stat_iscn_syms_inuse, stat_iscn_syms_recycle;
    int stat_img_syms_inuse, stat_img_syms_recycle;
    int stat_sym_new;
    int stat_sym_recycle[RECYCLE_BUCKETS];
#endif
};

void _zbar_image_scanner_recycle_syms (zbar_image_scanner_t *iscn,
                                       zbar_symbol_t *sym)
{
    zbar_symbol_t *next = NULL;
    for(; sym; sym = next) {
        next = sym->next;
        if(sym->refcnt && _zbar_refcnt(&sym->refcnt, -1)) {
            /* unlink referenced symbol */
            /* FIXME handle outstanding component refs (currently unsupported)
             */
            assert(sym->data_alloc);
            sym->next = NULL;
        }
        else {
            /* recycle unreferenced symbol */
            if(!sym->data_alloc) {
                sym->data = NULL;
                sym->datalen = 0;
            }
            if(sym->syms) {
                if(_zbar_refcnt(&sym->syms->refcnt, -1))
                    assert(0);
                _zbar_image_scanner_recycle_syms(iscn, sym->syms->head);
                sym->syms->head = NULL;
                _zbar_symbol_set_free(sym->syms);
                sym->syms = NULL;
            }
            int i;
            for(i = 0; i < RECYCLE_BUCKETS; i++)
                if(sym->data_alloc < 1 << (i * 2))
                    break;
            if(i == RECYCLE_BUCKETS) {
                assert(sym->data);
                zbar_free(sym->data);
                sym->data = NULL;
                sym->data_alloc = 0;
                i = 0;
            }
            recycle_bucket_t *bucket = &iscn->recycle[i];
            /* FIXME cap bucket fill */
            bucket->nsyms++;
            sym->next = bucket->head;
            bucket->head = sym;
        }
    }
}

static inline int recycle_syms (zbar_image_scanner_t *iscn,
                                zbar_symbol_set_t *syms)
{
    if(_zbar_refcnt(&syms->refcnt, -1))
        return(1);

    _zbar_image_scanner_recycle_syms(iscn, syms->head);
    syms->head = syms->tail = NULL;
    syms->nsyms = 0;
    return(0);
}

inline void zbar_image_scanner_recycle_image (zbar_image_scanner_t *iscn,
                                              zbar_image_t *img)
{
    zbar_symbol_set_t *syms = iscn->syms;
    if(syms && syms->refcnt) {
        if(recycle_syms(iscn, syms)) {
            STAT(iscn_syms_inuse);
            iscn->syms = NULL;
        }
        else
            STAT(iscn_syms_recycle);
    }

    syms = img->syms;
    img->syms = NULL;
    if(syms && recycle_syms(iscn, syms)) {
        STAT(img_syms_inuse);
        syms = iscn->syms;
    }
    else if(syms) {
        STAT(img_syms_recycle);

        /* select one set to resurrect, destroy the other */
        if(iscn->syms) {
            _zbar_symbol_set_free(syms);
            syms = iscn->syms;
        }
        else
            iscn->syms = syms;
    }
}

inline zbar_symbol_t*
_zbar_image_scanner_alloc_sym (zbar_image_scanner_t *iscn,
                               zbar_symbol_type_t type,
                               int datalen)
{
    /* recycle old or alloc new symbol */
    int i;
    for(i = 0; i < RECYCLE_BUCKETS - 1; i++)
        if(datalen <= 1 << (i * 2))
            break;

    zbar_symbol_t *sym = NULL;
    for(; i > 0; i--)
        if((sym = iscn->recycle[i].head)) {
            STAT(sym_recycle[i]);
            break;
        }

    if(sym) {
        iscn->recycle[i].head = sym->next;
        sym->next = NULL;
        assert(iscn->recycle[i].nsyms);
        iscn->recycle[i].nsyms--;
    }
    else {
        sym = zbar_calloc(1, sizeof(zbar_symbol_t));
        STAT(sym_new);
    }

    /* init new symbol */
    sym->type = type;
    sym->quality = 1;
    sym->npts = 0;
    sym->cache_count = 0;
    sym->time = iscn->time;
    assert(!sym->syms);

    if(datalen > 0) {
        sym->datalen = datalen - 1;
        if(sym->data_alloc < datalen) {
            if(sym->data)
                zbar_free(sym->data);
            sym->data_alloc = datalen;
            sym->data = zbar_malloc(datalen);
        }
    }
    else {
        if(sym->data)
            zbar_free(sym->data);
        sym->data = NULL;
        sym->datalen = sym->data_alloc = 0;
    }
    return(sym);
}

static inline zbar_symbol_t *cache_lookup (zbar_image_scanner_t *iscn,
                                           zbar_symbol_t *sym)
{
    /* search for matching entry in cache */
    zbar_symbol_t **entry = &iscn->cache;
    while(*entry) {
        if((*entry)->type == sym->type &&
           (*entry)->datalen == sym->datalen &&
           !memcmp((*entry)->data, sym->data, sym->datalen))
            break;
        if((sym->time - (*entry)->time) > CACHE_TIMEOUT) {
            /* recycle stale cache entry */
            zbar_symbol_t *next = (*entry)->next;
            (*entry)->next = NULL;
            _zbar_image_scanner_recycle_syms(iscn, *entry);
            *entry = next;
        }
        else
            entry = &(*entry)->next;
    }
    return(*entry);
}

static inline void cache_sym (zbar_image_scanner_t *iscn,
                              zbar_symbol_t *sym)
{
    if(iscn->enable_cache) {
        zbar_symbol_t *entry = cache_lookup(iscn, sym);
        if(!entry) {
            /* FIXME reuse sym */
            entry = _zbar_image_scanner_alloc_sym(iscn, sym->type,
                                                  sym->datalen + 1);
            memcpy(entry->data, sym->data, sym->datalen);
            entry->time = sym->time - CACHE_HYSTERESIS;
            entry->cache_count = -CACHE_CONSISTENCY;
            /* add to cache */
            entry->next = iscn->cache;
            iscn->cache = entry;
        }

        /* consistency check and hysteresis */
        uint32_t age = sym->time - entry->time;
        entry->time = sym->time;
        int near_thresh = (age < CACHE_PROXIMITY);
        int far_thresh = (age >= CACHE_HYSTERESIS);
        int dup = (entry->cache_count >= 0);
        if((!dup && !near_thresh) || far_thresh)
            entry->cache_count = -CACHE_CONSISTENCY;
        else if(dup || near_thresh)
            entry->cache_count++;

        sym->cache_count = entry->cache_count;
    }
    else
        sym->cache_count = 0;
}

void _zbar_image_scanner_add_sym(zbar_image_scanner_t *iscn,
                                 zbar_symbol_t *sym)
{
    cache_sym(iscn, sym);

    zbar_symbol_set_t *syms = iscn->syms;
    if(sym->cache_count || !syms->tail) {
        sym->next = syms->head;
        syms->head = sym;
    }
    else {
        sym->next = syms->tail->next;
        syms->tail->next = sym;
    }

    if(!sym->cache_count)
        syms->nsyms++;
    else if(!syms->tail)
        syms->tail = sym;

    _zbar_symbol_refcnt(sym, 1);
}

#ifdef ENABLE_QRCODE
extern qr_finder_line *_zbar_decoder_get_qr_finder_line(zbar_decoder_t*);

# define QR_FIXED(v, rnd) ((((v) << 1) + (rnd)) << (QR_FINDER_SUBPREC - 1))
# define PRINT_FIXED(val, prec) \
    ((val) >> (prec)),         \
        (1000 * ((val) & ((1 << (prec)) - 1)) / (1 << (prec)))

static inline void qr_handler (zbar_image_scanner_t *iscn)
{
    qr_finder_line *line = _zbar_decoder_get_qr_finder_line(iscn->dcode);
    assert(line);
    unsigned u = zbar_scanner_get_edge(iscn->scn, line->pos[0],
                                       QR_FINDER_SUBPREC);
    line->boffs = u - zbar_scanner_get_edge(iscn->scn, line->boffs,
                                            QR_FINDER_SUBPREC);
    line->len = zbar_scanner_get_edge(iscn->scn, line->len,
                                      QR_FINDER_SUBPREC);
    line->eoffs = zbar_scanner_get_edge(iscn->scn, line->eoffs,
                                        QR_FINDER_SUBPREC) - line->len;
    line->len -= u;

    u = QR_FIXED(iscn->umin, 0) + iscn->du * u;
    if(iscn->du < 0) {
        u -= line->len;
        int tmp = line->boffs;
        line->boffs = line->eoffs;
        line->eoffs = tmp;
    }
    int vert = !iscn->dx;
    line->pos[vert] = u;
    line->pos[!vert] = QR_FIXED(iscn->v, 1);

    _zbar_qr_found_line(iscn->qr, vert, line);
}
#endif

static void symbol_handler (zbar_decoder_t *dcode)
{
    zbar_image_scanner_t *iscn = zbar_decoder_get_userdata(dcode);
    zbar_symbol_type_t type = zbar_decoder_get_type(dcode);
    /* FIXME assert(type == ZBAR_PARTIAL) */
    /* FIXME debug flag to save/display all PARTIALs */
    if(type <= ZBAR_PARTIAL)
        return;

#ifdef ENABLE_QRCODE
    if(type == ZBAR_QRCODE) {
        qr_handler(iscn);
        return;
    }
#else
    assert(type != ZBAR_QRCODE);
#endif

    const char *data = zbar_decoder_get_data(dcode);
    unsigned datalen = zbar_decoder_get_data_length(dcode);

    int x = 0, y = 0;
    if(TEST_CFG(iscn, ZBAR_CFG_POSITION)) {
        /* tmp position fixup */
        int w = zbar_scanner_get_width(iscn->scn);
        int u = iscn->umin + iscn->du * zbar_scanner_get_edge(iscn->scn, w, 0);
        if(iscn->dx) {
            x = u;
            y = iscn->v;
        }
        else {
            x = iscn->v;
            y = u;
        }
    }

    /* FIXME need better symbol matching */
    zbar_symbol_t *sym;
    unsigned char bad_type = 0;
    if(iscn->syms != NULL)/* Evan Fixed: in case iscn->syms is not create for slave(dual core solution) */
	{
	    for(sym = iscn->syms->head; sym; sym = sym->next)
		{
			//printf("sym=0x%x, sym->type=%d, sym->datalen=%d£¬type=%d, datalen=%d\n", sym, sym->type, sym->datalen, type, datalen);
			#if 0//Evan add for bad syms crash problem
            bad_type = 0;
            switch(sym->type & ZBAR_SYMBOL) 
            {
            #ifdef ENABLE_EAN
                case ZBAR_EAN8:break;
                case ZBAR_UPCE:break;
                case ZBAR_ISBN10:break;
                case ZBAR_UPCA:break;
                case ZBAR_EAN13:break;
                case ZBAR_ISBN13:break;
            #endif
            #ifdef ENABLE_I25
                case ZBAR_I25:break;
            #endif
            #ifdef ENABLE_CODE39
                case ZBAR_CODE39:break;
            #endif
            #ifdef ENABLE_CODE128
                case ZBAR_CODE128:break;
            #endif
            #ifdef ENABLE_PDF417
                case ZBAR_PDF417:break;
            #endif
            #ifdef ENABLE_QRCODE
                case ZBAR_QRCODE:break;
            #endif
                default: 
                    bad_type = 1;
                    break;
            }
            if(bad_type)
            {
                _zbar_symbol_set_free(iscn->syms);
                return;
            }
            #endif
    
	        if(sym->type == type && sym->datalen == datalen) 
            {   
                if(!memcmp(sym->data, data, datalen)) 
    			{
    	            sym->quality++;
    	            if(TEST_CFG(iscn, ZBAR_CFG_POSITION))
    	                /* add new point to existing set */
    	                /* FIXME should be polygon */
    	                sym_add_point(sym, x, y);
    	            return;
    	        }
           }
		}
	}

    sym = _zbar_image_scanner_alloc_sym(iscn, type, datalen + 1);
    /* FIXME grab decoder buffer */
    memcpy(sym->data, data, datalen + 1);

    /* initialize first point */
    if(TEST_CFG(iscn, ZBAR_CFG_POSITION))
        sym_add_point(sym, x, y);

    _zbar_image_scanner_add_sym(iscn, sym);
}

zbar_image_scanner_t *zbar_image_scanner_create ()
{
    zbar_image_scanner_t *iscn = zbar_calloc(1, sizeof(zbar_image_scanner_t));
    if(!iscn)
        return(NULL);
    iscn->dcode = zbar_decoder_create();
    iscn->scn = zbar_scanner_create(iscn->dcode);
    if(!iscn->dcode || !iscn->scn) {
        zbar_image_scanner_destroy(iscn);
        return(NULL);
    }
    zbar_decoder_set_userdata(iscn->dcode, iscn);
    zbar_decoder_set_handler(iscn->dcode, symbol_handler);

#ifdef ENABLE_QRCODE
    iscn->qr = _zbar_qr_create();
#endif

    /* apply default configuration */
    CFG(iscn, ZBAR_CFG_X_DENSITY) = 1;
    CFG(iscn, ZBAR_CFG_Y_DENSITY) = 1;
    zbar_image_scanner_set_config(iscn, 0, ZBAR_CFG_POSITION, 1);
    return(iscn);
}

#ifndef NO_STATS
static inline void dump_stats (const zbar_image_scanner_t *iscn)
{
    zprintf(1, "symbol sets allocated   = %-4d\n", iscn->stat_syms_new);
    zprintf(1, "    scanner syms in use = %-4d\trecycled  = %-4d\n",
            iscn->stat_iscn_syms_inuse, iscn->stat_iscn_syms_recycle);
    zprintf(1, "    image syms in use   = %-4d\trecycled  = %-4d\n",
            iscn->stat_img_syms_inuse, iscn->stat_img_syms_recycle);
    zprintf(1, "symbols allocated       = %-4d\n", iscn->stat_sym_new);
    int i;
    for(i = 0; i < RECYCLE_BUCKETS; i++)
        zprintf(1, "     recycled[%d]        = %-4d\n",
                i, iscn->stat_sym_recycle[i]);
}
#endif

void zbar_image_scanner_destroy (zbar_image_scanner_t *iscn)
{
    dump_stats(iscn);
    if(iscn->syms) {
        if(iscn->syms->refcnt)
            zbar_symbol_set_ref(iscn->syms, -1);
        else
            _zbar_symbol_set_free(iscn->syms);
        iscn->syms = NULL;
    }
    if(iscn->scn)
        zbar_scanner_destroy(iscn->scn);
    iscn->scn = NULL;
    if(iscn->dcode)
        zbar_decoder_destroy(iscn->dcode);
    iscn->dcode = NULL;
    int i;
    for(i = 0; i < RECYCLE_BUCKETS; i++) {
        zbar_symbol_t *sym, *next;
        for(sym = iscn->recycle[i].head; sym; sym = next) {
            next = sym->next;
            _zbar_symbol_free(sym);
        }
    }
#ifdef ENABLE_QRCODE
    if(iscn->qr) {
        _zbar_qr_destroy(iscn->qr);
        iscn->qr = NULL;
    }
#endif
    zbar_free(iscn);
}

zbar_image_data_handler_t*
zbar_image_scanner_set_data_handler (zbar_image_scanner_t *iscn,
                                     zbar_image_data_handler_t *handler,
                                     const void *userdata)
{
    zbar_image_data_handler_t *result = iscn->handler;
    iscn->handler = handler;
    iscn->userdata = userdata;
    return(result);
}

int zbar_image_scanner_set_config (zbar_image_scanner_t *iscn,
                                   zbar_symbol_type_t sym,
                                   zbar_config_t cfg,
                                   int val)
{
    if(cfg < ZBAR_CFG_POSITION)
        return(zbar_decoder_set_config(iscn->dcode, sym, cfg, val));

    if(sym > ZBAR_PARTIAL)
        return(1);

    if(cfg >= ZBAR_CFG_X_DENSITY && cfg <= ZBAR_CFG_Y_DENSITY) {

        CFG(iscn, cfg) = val;
        return(0);
    }

    if(cfg > ZBAR_CFG_POSITION)
        return(1);
    cfg -= ZBAR_CFG_POSITION;

    if(!val)
        iscn->config &= ~(1 << cfg);
    else if(val == 1)
        iscn->config |= (1 << cfg);
    else
        return(1);

    return(0);
}

void zbar_image_scanner_enable_cache (zbar_image_scanner_t *iscn,
                                      int enable)
{
    if(iscn->cache) {
        /* recycle all cached syms */
        _zbar_image_scanner_recycle_syms(iscn, iscn->cache);
        iscn->cache = NULL;
    }
    iscn->enable_cache = (enable) ? 1 : 0;
}

const zbar_symbol_set_t *
zbar_image_scanner_get_results (const zbar_image_scanner_t *iscn)
{
    return(iscn->syms);
}

static inline void quiet_border (zbar_image_scanner_t *iscn)
{
    /* flush scanner pipeline */
    zbar_scanner_t *scn = iscn->scn;
    zbar_scanner_flush(scn);
    zbar_scanner_flush(scn);
    zbar_scanner_new_scan(scn);
}

#define movedelta(dx, dy) do {                  \
        x += (dx);                              \
        y += (dy);                              \
        p += (dx) + ((intptr_t)(dy) * w);       \
    } while(0);

#ifdef CONFIG_QR_DUAL_EN
/* scan each point's up/down grad by y density */  
void zbar_scan_image_x (zbar_image_scanner_t *iscn, const zbar_image_t *img)
{
    unsigned w = img->width;
    unsigned h = img->height;
    const uint8_t *data = img->data;
    zbar_scanner_t *scn = iscn->scn;

    iscn->dx = 0;
    int density = CFG(iscn, ZBAR_CFG_X_DENSITY);
    if(density > 0) {
        svg_group_start("scanner", 90, 1, -1, 0, 0);
        const uint8_t *p = data;
        int x = 0, y = 0;

        int border = (((w - 1) % density) + 1) / 2;
        if(border > w / 2)
            border = w / 2;
        movedelta(border, 0);
        iscn->v = x;

        while(x < w) {
            zprintf(128, "img_y+: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", 1. / 32, 0, x + 0.5);
            iscn->dy = iscn->du = 1;
            iscn->umin = 0;
#ifdef ARMC4_OPT
            asm_zbar_scan_y(p, scn, w, h);
            y += h;
            p += w*h;
#else
            while(y < h) {
                uint8_t d = *p;
                movedelta(0, 1);
                zbar_scan_y(scn, d);
            }
#endif
            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(density, -1);
            iscn->v = x;
            if(x >= w)
                break;

            zprintf(128, "img_y-: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", -1. / 32, h, x + 0.5);
            iscn->dy = iscn->du = -1;
            iscn->umin = h;
#ifdef ARMC4_OPT
            asm_zbar_scan_y(p, scn, -w, h);
            y -= h;
            p -= w*h;
#else
            while(y >= 0) {
                uint8_t d = *p;
                movedelta(0, -1);
                zbar_scan_y(scn, d);
            }
#endif
            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(density, 1);
            iscn->v = x;
        }
        svg_group_end();
    }

    iscn->dx = 0;
    iscn->dy = 0;
    iscn->img = NULL;

}

/* scan each point's left/right grad by y density */  
void zbar_scan_image_y (zbar_image_scanner_t *iscn, const zbar_image_t *img)
{
    unsigned w = img->width;
    unsigned h = img->height;
    const uint8_t *data = img->data;
    zbar_scanner_t *scn = iscn->scn;

    //printk("iscn->scn=0x%x\n", iscn->scn);

    int density = CFG(iscn, ZBAR_CFG_Y_DENSITY);
    //printk("density=%d\n", density);
    if(density > 0) {
        svg_group_start("scanner", 0, 1, 1, 0, 0);
        const uint8_t *p = data;
        int x = 0, y = 0;
        iscn->dy = 0;

        int border = (((h - 1) % density) + 1) / 2;
        if(border > h / 2)
            border = h / 2;
        movedelta(0, border);
        iscn->v = y;

        //time_zbar_scan_Y_prepare = k_uptime_get_32();
        //printk("[Y detail]\n-prepare=%d\n", time_zbar_scan_Y_prepare-time_zbar_scan_prepare);
        while(y < h) {
            zprintf(128, "img_x+: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", 1. / 32, 0, y + 0.5);
            iscn->dx = iscn->du = 1;
            iscn->umin = 0;
            //time_zbar_scan_Y_scan_y_0 = k_uptime_get_32();
#ifdef ARMC4_OPT
            asm_zbar_scan_y(p, scn, 1, w);
            x += w;
            p += w;
#else
            while(x < w) {
                uint8_t d = *p;
                movedelta(1, 0);
                zbar_scan_y(scn, d);
            }
#endif
            //time_zbar_scan_Y_scan_y_1 = k_uptime_get_32();
            //printk("-scan_y=%d\n", time_zbar_scan_Y_scan_y_1-time_zbar_scan_Y_scan_y_0);

            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(-1, density);
            iscn->v = y;
            if(y >= h)
                break;

            zprintf(128, "img_x-: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", -1. / 32, w, y + 0.5);
            iscn->dx = iscn->du = -1;
            iscn->umin = w;
#ifdef ARMC4_OPT
            asm_zbar_scan_y(p, scn, -1, w);
            x -= w;
            p -= w;
#else
            while(x >= 0) {
                uint8_t d = *p;
                movedelta(-1, 0);
                zbar_scan_y(scn, d);
            }
#endif
            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(1, density);
            iscn->v = y;
        }
        svg_group_end();
    }

    iscn->dx = 0;
    iscn->dy = 0;
    iscn->img = NULL;
    return;
}

#ifdef CONFIG_GM_HAL_RPC_SLAVE
/* fix slave compile error */
int zbar_scan_image (zbar_image_scanner_t *iscn, zbar_image_scanner_t *iscn_slave,
                     zbar_image_t *img, zbar_image_t *img_slave)
{
    return 0;
}
#else
int zbar_scan_image (zbar_image_scanner_t *iscn, zbar_image_scanner_t *iscn_slave,
                     zbar_image_t *img, zbar_image_t *img_slave)
{
    /* timestamp image
     * FIXME prefer video timestamp
     */
#if _POSIX_TIMERS > 0
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    iscn->time = (abstime.tv_sec * 1000) + ((abstime.tv_nsec / 500000) + 1) / 2;
#else
//    struct timeval abstime;
//    gettimeofday(&abstime, NULL);
//    iscn->time = (abstime.tv_sec * 1000) + ((abstime.tv_usec / 500) + 1) / 2;
#endif

#ifdef ENABLE_QRCODE
    _zbar_qr_reset(iscn->qr);
    _zbar_qr_reset(iscn_slave->qr);
#endif

    /* get grayscale image, convert if necessary */
    if(img->format != fourcc('Y','8','0','0') &&
       img->format != fourcc('G','R','E','Y'))
        return(-1);
    iscn->img = img;
    iscn_slave->img = img_slave;

    /* recycle previous scanner and image results of iscn_slave*/
    #if 1
    zbar_image_scanner_recycle_image(iscn_slave, img_slave);
    zbar_symbol_set_t *syms_slave = iscn_slave->syms;
    if(!syms_slave) {
        syms_slave = iscn_slave->syms = _zbar_symbol_set_create();
        //STAT(syms_new);
        iscn_slave->stat_syms_new++;
        zbar_symbol_set_ref(syms_slave, 1);
    }
    else
        zbar_symbol_set_ref(syms_slave, 2);
    img_slave->syms = syms_slave;
    #endif

    /* recycle previous scanner and image results */
    zbar_image_scanner_recycle_image(iscn, img);
    zbar_symbol_set_t *syms = iscn->syms;
    if(!syms) {
        syms = iscn->syms = _zbar_symbol_set_create();
        STAT(syms_new);
        zbar_symbol_set_ref(syms, 1);
    }
    else
        zbar_symbol_set_ref(syms, 2);
    img->syms = syms;

    unsigned w = img->width;
    unsigned h = img->height;
    const uint8_t *data = img->data;

    zbar_image_write_png(img, "debug.png");
    svg_open("debug.svg", 0, 0, w, h);
    svg_image("debug.png", w, h);

    //time_zbar_scan_prepare = k_uptime_get_32();


#if 1
    RPC_Package_T pack;
    QR_Data_T qrdata;
    RET_CODE ret = RET_OK;

    /* scan X density */
    zbar_scanner_t *scn_slave = iscn_slave->scn;
    zbar_scanner_new_scan(scn_slave);
    int density = CFG(iscn, ZBAR_CFG_X_DENSITY);
    /* prepare private data */
    qrdata.iscn = (uint8_t *)iscn_slave;
    qrdata.img = (uint8_t *)img_slave;
    /* prepare rpc pack */
    pack.cmd = RPC_CMD_QR;
    pack.privData = &qrdata;
    ret = RPC_MasterSendPack(&pack);
    if(ret != RET_OK)
    {
        printk("!!! RPC_MasterSendPack wrong \n");
        //continue;
    }
    //time_zbar_scan_X = k_uptime_get_32();

    /* scan Y density */
    zbar_scanner_t *scn = iscn->scn;
    zbar_scanner_new_scan(scn);
    zbar_scan_image_y(iscn, img);

    //if(RPC_MasterWaitResp(&pack, TIMEOUT_WAIT_FOREVER) != RET_OK)
    if((RPC_MasterWaitResp(&pack, 2000) != RET_OK) || (pack.result != RET_OK))
    {
        printk("!!! slave wrong \n");
        //continue;
    }
#else
    /* scan X */
    zbar_scanner_t *scn_slave = iscn_slave->scn;
    zbar_scanner_new_scan(scn_slave);
    int density = CFG(iscn, ZBAR_CFG_X_DENSITY);
    zbar_scan_image_x(iscn_slave, img);
    //time_zbar_scan_X = k_uptime_get_32();

    /* scan Y */
    zbar_scanner_t *scn = iscn->scn;
    zbar_scanner_new_scan(scn);
    zbar_scan_image_y(iscn, img);
    //time_zbar_scan_Y = k_uptime_get_32();

#endif
    //time_zbar_scan_XY = k_uptime_get_32();

#ifdef ENABLE_QRCODE
    /* merge X/Y */
    _zbar_qr_copy_lines(iscn->qr, iscn_slave->qr, 1);

    _zbar_qr_decode(iscn->qr, iscn, img);
    //time_zbar_scan_decode = k_uptime_get_32();
#endif

#ifdef ENABLE_EAN
    /* FIXME tmp hack to filter bad EAN results */
    if(syms->nsyms && !iscn->enable_cache &&
       (density == 1 || CFG(iscn, ZBAR_CFG_Y_DENSITY) == 1)) {
        zbar_symbol_t **symp = &syms->head, *sym;
        while((sym = *symp)) {
            if(sym->type < ZBAR_I25 && sym->type > ZBAR_PARTIAL &&
               sym->quality < 3) {
                /* recycle */
                *symp = sym->next;
                syms->nsyms--;
                sym->next = NULL;
                _zbar_image_scanner_recycle_syms(iscn, sym);
            }
            else
			{
				if(symp == &sym->next)//Evan: fixed deadloop problem
				{
					break;
				}
				else
				{
					symp = &sym->next;
				}
			}
        }
    }
#endif

    if(syms->nsyms && iscn->handler)
        iscn->handler(img, iscn->userdata);

    svg_close();
    //time_zbar_scan_remainder = k_uptime_get_32();
    return(syms->nsyms);
}
#endif

#else
int zbar_scan_image (zbar_image_scanner_t *iscn,
                     zbar_image_t *img)
{
    /* timestamp image
     * FIXME prefer video timestamp
     */
#if _POSIX_TIMERS > 0
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    iscn->time = (abstime.tv_sec * 1000) + ((abstime.tv_nsec / 500000) + 1) / 2;
#else
	#if 0//Evan: temply remove to fix compile error, need care !!!
    struct timeval abstime;
    gettimeofday(&abstime, NULL);
    iscn->time = (abstime.tv_sec * 1000) + ((abstime.tv_usec / 500) + 1) / 2;
	#endif
#endif

#ifdef ENABLE_QRCODE
    _zbar_qr_reset(iscn->qr);
#endif

    /* get grayscale image, convert if necessary */
    if(img->format != fourcc('Y','8','0','0') &&
       img->format != fourcc('G','R','E','Y'))
        return(-1);
    iscn->img = img;

    /* recycle previous scanner and image results */
    zbar_image_scanner_recycle_image(iscn, img);
    zbar_symbol_set_t *syms = iscn->syms;
    if(!syms) {
        syms = iscn->syms = _zbar_symbol_set_create();
        STAT(syms_new);
        zbar_symbol_set_ref(syms, 1);
    }
    else
        zbar_symbol_set_ref(syms, 2);
    img->syms = syms;

    unsigned w = img->width;
    unsigned h = img->height;
    const uint8_t *data = img->data;

    zbar_image_write_png(img, "debug.png");
    svg_open("debug.svg", 0, 0, w, h);
    svg_image("debug.png", w, h);

    //time_zbar_scan_prepare = k_uptime_get_32();

    zbar_scanner_t *scn = iscn->scn;

    int density = CFG(iscn, ZBAR_CFG_Y_DENSITY);
    if(density > 0) {
        svg_group_start("scanner", 0, 1, 1, 0, 0);
        const uint8_t *p = data;
        int x = 0, y = 0;
        iscn->dy = 0;

        int border = (((h - 1) % density) + 1) / 2;
        if(border > h / 2)
            border = h / 2;
        movedelta(0, border);
        iscn->v = y;

        zbar_scanner_new_scan(scn);
        //time_zbar_scan_Y_prepare = k_uptime_get_32();
        //printk("[Y detail]\n-prepare=%d\n", time_zbar_scan_Y_prepare-time_zbar_scan_prepare);
        while(y < h) {
            zprintf(128, "img_x+: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", 1. / 32, 0, y + 0.5);
            iscn->dx = iscn->du = 1;
            iscn->umin = 0;
            //time_zbar_scan_Y_scan_y_0 = k_uptime_get_32();
#ifdef ARMC4_OPT
            asm_zbar_scan_y(p, scn, 1, w);
            x += w;
            p += w;
            
#else					
            while(x < w) {
                uint8_t d = *p;
                //printf("(%04d,%04d) @%p\n", x, y, p);
                cur_x = x; cur_y = y;
                movedelta(1, 0);
                zbar_scan_y(scn, d);
            }
#endif						
            //time_zbar_scan_Y_scan_y_1 = k_uptime_get_32();
            //printk("-scan_y=%d\n", time_zbar_scan_Y_scan_y_1-time_zbar_scan_Y_scan_y_0);

            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(-1, density);
            iscn->v = y;
            if(y >= h)
                break;

            zprintf(128, "img_x-: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", -1. / 32, w, y + 0.5);
            iscn->dx = iscn->du = -1;
            iscn->umin = w;
#ifdef ARMC4_OPT
            asm_zbar_scan_y(p, scn, -1, w);
            x -= w;
            p -= w;
#else
            while(x >= 0) {
                uint8_t d = *p;
                movedelta(-1, 0);
                zbar_scan_y(scn, d);
            }
#endif
            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(1, density);
            iscn->v = y;
        }
        svg_group_end();
    }
    //time_zbar_scan_Y = k_uptime_get_32();
    iscn->dx = 0;

    density = CFG(iscn, ZBAR_CFG_X_DENSITY);
    if(density > 0) {
        svg_group_start("scanner", 90, 1, -1, 0, 0);
        const uint8_t *p = data;
        int x = 0, y = 0;

        int border = (((w - 1) % density) + 1) / 2;
        if(border > w / 2)
            border = w / 2;
        movedelta(border, 0);
        iscn->v = x;

        while(x < w) {
            zprintf(128, "img_y+: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", 1. / 32, 0, x + 0.5);
            iscn->dy = iscn->du = 1;
            iscn->umin = 0;
#ifdef ARMC4_OPT
            asm_zbar_scan_y(p, scn, w, h);
            y += h;
            p += w*h;
#else
            while(y < h) {
                uint8_t d = *p;
                movedelta(0, 1);
                zbar_scan_y(scn, d);
            }
#endif
            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(density, -1);
            iscn->v = x;
            if(x >= w)
                break;

            zprintf(128, "img_y-: %04d,%04d @%p\n", x, y, p);
            svg_path_start("vedge", -1. / 32, h, x + 0.5);
            iscn->dy = iscn->du = -1;
            iscn->umin = h;
#ifdef ARMC4_OPT
            asm_zbar_scan_y(p, scn, -w, h);
            y -= h;
            p -= w*h;
#else
            while(y >= 0) {
                uint8_t d = *p;
                movedelta(0, -1);
                zbar_scan_y(scn, d);
            }
#endif
            ASSERT_POS;
            quiet_border(iscn);
            svg_path_end();

            movedelta(density, 1);
            iscn->v = x;
        }
        svg_group_end();
    }
    //time_zbar_scan_X = k_uptime_get_32();
    iscn->dy = 0;
    iscn->img = NULL;

#ifdef ENABLE_QRCODE
    _zbar_qr_decode(iscn->qr, iscn, img);
    //time_zbar_scan_decode = k_uptime_get_32();
#endif

    /* FIXME tmp hack to filter bad EAN results */
    if(syms->nsyms && !iscn->enable_cache &&
       (density == 1 || CFG(iscn, ZBAR_CFG_Y_DENSITY) == 1)) {
        zbar_symbol_t **symp = &syms->head, *sym;
        while((sym = *symp)) {
            if(sym->type < ZBAR_I25 && sym->type > ZBAR_PARTIAL &&
               sym->quality < 3) {
                /* recycle */
                *symp = sym->next;
                syms->nsyms--;
                sym->next = NULL;
                _zbar_image_scanner_recycle_syms(iscn, sym);
            }
            else
                symp = &sym->next;
        }
    }

    if(syms->nsyms && iscn->handler)
        iscn->handler(img, iscn->userdata);

    svg_close();
    //time_zbar_scan_remainder = k_uptime_get_32();
    return(syms->nsyms);
}
#endif

#ifdef DEBUG_SVG
/* FIXME lame...*/
# include "svg.c"
#endif
