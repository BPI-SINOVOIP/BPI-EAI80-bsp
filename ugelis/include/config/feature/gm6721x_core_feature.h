#ifndef __GM6721_CORE_FEATURE_H__
#define __GM6721_CORE_FEATURE_H__
/********************************************/
/* General Kernel Options */
/********************************************/
#define CONFIG_MULTITHREADING               1
#define CONFIG_NUM_COOP_PRIORITIES          16
#define CONFIG_NUM_PREEMPT_PRIORITIES       15
#define CONFIG_MAIN_THREAD_PRIORITY         0
#define CONFIG_COOP_ENABLED                 1
#define CONFIG_PREEMPT_ENABLED              1
#define CONFIG_PRIORITY_CEILING             0
#define CONFIG_NUM_METAIRQ_PRIORITIES       0
#define CONFIG_MAIN_STACK_SIZE              4096
#define CONFIG_IDLE_STACK_SIZE              2048
#define CONFIG_ISR_STACK_SIZE               1024
#define CONFIG_ERRNO                        1
#define CONFIG_SCHED_MULTIQ                 1
#define CONFIG_WAITQ_DUMB                   1

/********************************************/
/* Kernel Debugging and Metrics */
/********************************************/
#define CONFIG_BOOT_BANNER                  1
#define CONFIG_BOOT_DELAY                   0

/********************************************/
/* Work Queue Options */
/********************************************/
#define CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE  1024
#define CONFIG_SYSTEM_WORKQUEUE_PRIORITY    -1
#define CONFIG_OFFLOAD_WORKQUEUE_STACK_SIZE 1024
#define CONFIG_OFFLOAD_WORKQUEUE_PRIORITY   -1

/********************************************/
/* Atomic Operations */
/********************************************/
//#define CONFIG_ATOMIC_OPERATIONS_BUILTIN  1

/********************************************/
/* Timer API Options */
/********************************************/
#define CONFIG_TIMESLICING                  1
#define CONFIG_TIMESLICE_SIZE               0
#define CONFIG_TIMESLICE_PRIORITY           0

/********************************************/
/* Other Kernel Object Options */
/********************************************/
#define CONFIG_NUM_MBOX_ASYNC_MSGS          10
#define CONFIG_NUM_PIPE_ASYNC_MSGS          10
#define CONFIG_HEAP_MEM_POOL_SIZE           8192
#define CONFIG_ARCH_HAS_CUSTOM_SWAP_TO_MAIN 1
#define CONFIG_SYS_CLOCK_TICKS_PER_SEC      1000
#define CONFIG_SYS_CLOCK_EXISTS             1

/********************************************/
/* Initialization Priorities */
/********************************************/
#define CONFIG_KERNEL_INIT_PRIORITY_OBJECTS 30
#define CONFIG_KERNEL_INIT_PRIORITY_DEFAULT 40
#define CONFIG_KERNEL_INIT_PRIORITY_DEVICE  50
#define CONFIG_APPLICATION_INIT_PRIORITY    90

/********************************************/
/* Security Options */
/********************************************/
#define CONFIG_RETPOLINE                    1
#define CONFIG_STACK_POINTER_RANDOM         0

/********************************************/
/* SMP Options */
/********************************************/
#define CONFIG_MP_NUM_CPUS                  1
#define CONFIG_HAS_DTS                      1

/********************************************/
/* Linker Options */
/********************************************/
#define CONFIG_HAS_FLASH_LOAD_OFFSET 1
#define CONFIG_TEXT_SECTION_OFFSET 0
#define CONFIG_KERNEL_ENTRY "__start"
#define CONFIG_CHECK_LINK_MAP 1

/********************************************/
/* Compiler Options */
/********************************************/
#define CONFIG_CROSS_COMPILE ""
#define CONFIG_DEBUG_OPTIMIZATIONS 1
#define CONFIG_COMPILER_OPT ""

/********************************************/
/* Build Options */
/********************************************/
#define CONFIG_KERNEL_BIN_NAME "ugelis"
#define CONFIG_OUTPUT_STAT 1
#define CONFIG_OUTPUT_DISASSEMBLY 1
#define CONFIG_OUTPUT_PRINT_MEMORY_USAGE 1
#define CONFIG_BUILD_OUTPUT_BIN 1

/********************************************/
/* Boot Options */
/********************************************/
#define CONFIG_SDRAM_BOOT 1
//#define CONFIG_XIP  1
#define CONFIG_IMAGE_OFFSET_SIZE 64

#ifndef CONFIG_KERNEL_MEM_RESVER_SIZE
    #if (defined GM6721_EVB_QFN80PIN) || (defined GM6721_EVB_QFN40PIN) || (defined F6721B_EVB_QFN80PIN) || (defined BANPIBOARD_QFN80PIN)
        #define CONFIG_KERNEL_MEM_RESVER_SIZE 1024
    #else
        #define CONFIG_KERNEL_MEM_RESVER_SIZE 2048
    #endif

#endif
#define CONFIG_BOOTLOADER_SRAM_SIZE 0

/********************************************/
/* Boot Options */
/********************************************/
#define CONFIG_BOOTLOADER_SRAM_SIZE 0

#define CONFIG_NUM_IRQS 80

/********************************************/
/* KObject Options */
/********************************************/
//#define CONFIG_POLL
#endif
