#ifndef _SINK_ELEMENT_H_
#define _SINK_ELEMENT_H_

#include<ust/core/ust_core.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ELEMENT_SRINK_NAME			"SRINK_DEV"



//#define UST_SINK_ELET_DEBUG 1
#define UST_SINK_ELET_ERR 1


#ifdef UST_SINK_ELET_DEBUG
    #define ust_sink_elet_pr_debug       printk
#else
    #define ust_sink_elet_pr_debug(...)  do{}while(0)
#endif

#ifdef UST_SINK_ELET_ERR
    #define ust_sink_elet_pr_err         printk
#else
    #define ust_sink_elet_pr_err(...)    do{}while(0)
#endif



#ifdef __cplusplus
}
#endif

#endif /* _SINK_ELEMENT_H_ */










