
#ifndef __CPU_TEST_H__
#define __CPU_TEST_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void dhrystone();
/*###########################cache test start####################################*/
/* Icache -> disable Dcache -> disable */

#define CACHE_TEST_0    1

/* Icache -> enable Dcache -> disable */
//#define CACHE_TEST_1  1

/* Icache -> disable Dcache -> enable */
//#define CACHE_TEST_2  1

/* Icache -> enable Dcache -> enable */
//#define CACHE_TEST_3  1

/*##########################cache test end########################################*/


#ifdef __cplusplus
}
#endif

#endif  /* __CPU_TEST_H__ */




