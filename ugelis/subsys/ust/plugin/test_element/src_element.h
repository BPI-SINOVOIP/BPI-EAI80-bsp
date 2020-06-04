#ifndef _SRC_ELEMENT_H_
#define _SRC_ELEMENT_H_


#ifdef __cplusplus
extern "C" {
#endif

#define DEV_SRC_ELEMENT_NAME  	          "element_src_dev"
#define UST_SRC_QUEUE_MAX_OBJECT_NUM       5
#define UST_SRC_DATA_SIZE                  17


//#define UST_SRC_ELET_DEBUG 1
#define UST_SRC_ELET_ERR 1


#ifdef UST_SRC_ELET_DEBUG
    #define ust_src_elet_pr_debug       printk
#else
    #define ust_src_elet_pr_debug(...)  do{}while(0)
#endif

#ifdef UST_SRC_ELET_ERR
    #define ust_src_elet_pr_err         printk
#else
    #define ust_src_elet_pr_err(...)    do{}while(0)
#endif


#ifdef __cplusplus
}
#endif

#endif /* _SRC_ELEMENT_H_ */











