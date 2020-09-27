#ifndef _GM_SOC_H_
#define _GM_SOC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<gm_config.h>
#include<gm_irq.h>

#if 1
    #include<soc_gm6721x.h>
#else
    /*add for other gm soc chip*/
    #error "define gm chip not match"
#endif


#ifdef __cplusplus
}
#endif

#endif










































