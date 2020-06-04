#ifndef _GM_COMMON_H_
#define _GM_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<string.h>
#include<gm_io.h>
#include<gm_basic_types.h>
#include<gm_config.h>
#include<cmsis_compiler.h>




/** Min/max macros */
/* @{ */
#if !defined(MIN)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#if !defined(MAX)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
/* @} */

/** Get container pointer from pointer to memeber of this container */
/* @{ */
#if !defined(HAL_CONTAINER_OF)
#define HAL_CONTAINER_OF(ptr, type, member) (type *)((char *)(ptr) - ((uint32_t) &((type *)0)->member))
#endif
/* @} */


#if !defined(UNUSED)
#define UNUSED(x) ((void)(x))
#endif
#ifdef  DEBUG
/**
  * @brief  The ASSERT macro is used for function's assert declaration.
  * @param  expr: If expr is false, it prints the name of the source file and the source
  *         line number of failed code, otherwise it returns no value.
  * @retval None
  */
#define ASSERT(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
void assert_failed(uint8_t* file, uint32_t line);
#else
#define ASSERT(expr)          do{}while(0)
#endif /* DEBUG */
/**
 * @brief Intialize debug console to print out debug information.
 *
 * @param  None.
 *
 * @retval None.
 */
#ifdef RABBIT_NO_OS_SYSTEM
#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */
#endif

/* Macro to get variable aligned on 4-bytes, for __ICCARM__ the directive "#pragma data_alignment=4" must be used instead */
#if defined   (__GNUC__)        /* GNU Compiler */
  #ifndef __ALIGN_END
    #define __ALIGN_END    __attribute__ ((aligned (4)))
  #endif /* __ALIGN_END */
  #ifndef __ALIGN_BEGIN
    #define __ALIGN_BEGIN
  #endif /* __ALIGN_BEGIN */
#else
  #ifndef __ALIGN_END
    #define __ALIGN_END
  #endif /* __ALIGN_END */
  #ifndef __ALIGN_BEGIN
    #if defined   (__CC_ARM)      /* ARM Compiler */
      #define __ALIGN_BEGIN    __align(4)
    #elif defined (__ICCARM__)    /* IAR Compiler */
      #define __ALIGN_BEGIN
    #endif /* __CC_ARM */
  #endif /* __ALIGN_BEGIN */
#endif /* __GNUC__ */


/**
  * @brief  __RAM_FUNC definition
  */
#if defined ( __CC_ARM   )
/* ARM Compiler
   ------------
   RAM functions are defined using the toolchain options.
   Functions that are executed in RAM should reside in a separate source module.
   Using the 'Options for File' dialog you can simply change the 'Code / Const'
   area of a module to a memory space in physical RAM.
   Available memory areas are declared in the 'Target' tab of the 'Options for Target'
   dialog.
*/
#define __RAM_FUNC HAL_StatusTypeDef

#elif defined ( __ICCARM__ )
/* ICCARM Compiler
   ---------------
   RAM functions are defined using a specific toolchain keyword "__ramfunc".
*/
#define __RAM_FUNC __ramfunc HAL_StatusTypeDef

#elif defined   (  __GNUC__  )
/* GNU Compiler
   ------------
  RAM functions are defined using a specific toolchain attribute
   "__attribute__((section(".RamFunc")))".
*/
#define __RAM_FUNC HAL_StatusTypeDef  __attribute__((section(".RamFunc")))

#endif

/**
  * @brief  __NOINLINE definition
  */
#if defined ( __CC_ARM   ) || defined   (  __GNUC__  )
/* ARM & GNUCompiler
   ----------------
*/
#define __NOINLINE __attribute__ ( (noinline) )

#elif defined ( __ICCARM__ )
/* ICCARM Compiler
   ---------------
*/
#define __NOINLINE _Pragma("optimize = no_inline")


#endif

#if !defined(__IO)
#define __IO  volatile
#endif

typedef uint8_t     lock_t;
#define HAL_LOCKED      1
#define HAL_UNLOCK		0


uint32_t HAL_GetTick(void);
void HAL_DelayMs(uint32_t ms);
void HAL_DelayUs(uint32_t us);
void HAL_IrqLock(void);
void HAL_IrqUnlock(void);
void HAL_LockInit(lock_t *pLock);
void HAL_UnLock(lock_t *pLock);
RET_CODE HAL_TryLock(lock_t *pLock);
void HAL_DisableIrq(void);
void HAL_EnableIrq(void);

#ifdef RABBIT_NO_OS_SYSTEM
void DebugConsoleInit(uint32_t baudrate);

void *memcpy (void *dest, const void *src, unsigned n);
void *memset (void *s, int c, unsigned n);
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_COMMON_H_ */

