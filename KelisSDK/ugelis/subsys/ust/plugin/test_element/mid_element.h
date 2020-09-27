
#ifndef _MID_ELEMENT_H_
#define _MID_ELEMENT_H_



#ifdef __cplusplus
extern "C" {
#endif

#define DEV_MIDDLE_ELEMENT_NAME     "element_middle_dev"

//#define UST_MID_ELET_DEBUG 1
#define UST_MID_ELET_ERR 1


#ifdef UST_MID_ELET_DEBUG
    #define ust_mid_elet_pr_debug       printk
#else
    #define ust_mid_elet_pr_debug(...)  do{}while(0)
#endif

#ifdef UST_MID_ELET_ERR
    #define ust_mid_elet_pr_err         printk
#else
    #define ust_mid_elet_pr_err(...)    do{}while(0)
#endif



#ifdef __cplusplus
}
#endif

#endif /* _MID_ELEMENT_H_ */









