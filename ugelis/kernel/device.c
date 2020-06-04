/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          device.c
 *
 * @author        William.Wang
 *
 * @version       1.0.0
 *
 * @date          2018/03/24
 *
 * @brief
 *
 * @note
 *    2018/03/24, William.Wang, V1.0.0
 *        Initial version.
 */

#ifdef __USE_KEIL_
    #include <errno_ex.h>
#else
    #include <errno.h>
#endif
#include <string.h>
#include <device.h>
#include <misc/util.h>
#include <atomic.h>

#ifdef __USE_KEIL_
extern unsigned char Load$$DEVICE_PRE_KERNEL1_REGION$$Base;
extern unsigned char Image$$DEVICE_PRE_KERNEL1_REGION$$Base;
extern unsigned char Image$$DEVICE_PRE_KERNEL2_REGION$$Base;
extern unsigned char Image$$DEVICE_POST_KERNEL_REGION$$Base;
extern unsigned char Image$$DEVICE_APPLICATION_REGION$$Base;
extern unsigned char Image$$DEVICE_PRE_KERNEL1_REGION$$Length;
extern unsigned char Image$$DEVICE_PRE_KERNEL2_REGION$$Length;
extern unsigned char Image$$DEVICE_POST_KERNEL_REGION$$Length;
extern unsigned char Image$$DEVICE_APPLICATION_REGION$$Length;


struct device *__device_init_start = (struct device *) &Image$$DEVICE_PRE_KERNEL1_REGION$$Base;
struct device *__device_PRE_KERNEL1_start = (struct device *) &Image$$DEVICE_PRE_KERNEL1_REGION$$Base;
struct device *__device_PRE_KERNEL2_start = (struct device *) &Image$$DEVICE_PRE_KERNEL2_REGION$$Base;
struct device *__device_POST_KERNEL_start = (struct device *) &Image$$DEVICE_POST_KERNEL_REGION$$Base;
struct device *__device_APPLICATION_start = (struct device *) &Image$$DEVICE_APPLICATION_REGION$$Base;
struct device *__device_init_end;
static struct device *config_levels[5];
#else
extern struct device __device_init_start[];
extern struct device __device_PRE_KERNEL_1_start[];
extern struct device __device_PRE_KERNEL_2_start[];
extern struct device __device_POST_KERNEL_start[];
extern struct device __device_APPLICATION_start[];
extern struct device __device_init_end[];

static struct device *config_levels[] =
{
    __device_PRE_KERNEL_1_start,
    __device_PRE_KERNEL_2_start,
    __device_POST_KERNEL_start,
    __device_APPLICATION_start,
    /* End marker */
    __device_init_end,
};
#endif

#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
    #ifdef __USE_KEIL_
        u32_t *__device_busy_start; /*not use for keil,only for complier*/
        u32_t *__device_busy_end;/*not use for keil,only for complier*/
    #else
        extern u32_t __device_busy_start[];
        extern u32_t __device_busy_end[];
    #endif
    #define DEVICE_BUSY_SIZE (__device_busy_end - __device_busy_start)
#endif

/**
 * @brief Execute all the device initialization functions at a given level
 *
 * @details Invokes the initialization routine for each device object
 * created by the DEVICE_INIT() macro using the specified level.
 * The linker script places the device objects in memory in the order
 * they need to be invoked, with symbols indicating where one level leaves
 * off and the next one begins.
 *
 * @param level init level to run.
 */
void _sys_device_do_config_level(int level)
{
    struct device *info;

#ifdef __USE_KEIL_
    __device_init_start = (struct device *)&Image$$DEVICE_PRE_KERNEL1_REGION$$Base;
    __device_PRE_KERNEL1_start = (struct device *)&Image$$DEVICE_PRE_KERNEL1_REGION$$Base;
    __device_PRE_KERNEL2_start = (struct device *)&Image$$DEVICE_PRE_KERNEL2_REGION$$Base;
    __device_POST_KERNEL_start = (struct device *)&Image$$DEVICE_POST_KERNEL_REGION$$Base;
    __device_APPLICATION_start = (struct device *)&Image$$DEVICE_APPLICATION_REGION$$Base;
    __device_init_end  = (struct device *)((unsigned int)&Image$$DEVICE_APPLICATION_REGION$$Base + (unsigned int)&Image$$DEVICE_APPLICATION_REGION$$Length);

    config_levels[0] = __device_PRE_KERNEL1_start;
    config_levels[1] = __device_PRE_KERNEL2_start;
    config_levels[2] = __device_POST_KERNEL_start;
    config_levels[3] = __device_APPLICATION_start;
    config_levels[4] = __device_init_end;
#endif
    for (info = config_levels[level]; info < config_levels[level + 1];
            info++)
    {
        struct device_config *device = info->config;

        device->init(info);
        _k_object_init(info);
    }
}

struct device *device_get_binding(const char *name)
{
    struct device *info;

    /* Split the search into two loops: in the common scenario, where
     * device names are stored in ROM (and are referenced by the user
     * with CONFIG_* macros), only cheap pointer comparisons will be
     * performed.  Reserve string comparisons for a fallback.
     */
    for (info = __device_init_start; info != __device_init_end; info++)
    {
        if (info->driver_api != NULL && info->config->name == name)
        {
            return info;
        }
    }

    for (info = __device_init_start; info != __device_init_end; info++)
    {
        if (!info->driver_api)
        {
            continue;
        }

        if (!strcmp(name, info->config->name))
        {
            return info;
        }
    }

    return NULL;
}

#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
int device_pm_control_nop(struct device *unused_device,
                          u32_t unused_ctrl_command, void *unused_context)
{
    return 0;
}

void device_list_get(struct device **device_list, int *device_count)
{

    *device_list = __device_init_start;
    *device_count = __device_init_end - __device_init_start;
}


int device_any_busy_check(void)
{
    int i = 0;

    for (i = 0; i < DEVICE_BUSY_SIZE; i++)
    {
        if (__device_busy_start[i] != 0)
        {
            return -EBUSY;
        }
    }
    return 0;
}

int device_busy_check(struct device *chk_dev)
{
    if (atomic_test_bit((const atomic_t *)__device_busy_start,
                        (chk_dev - __device_init_start)))
    {
        return -EBUSY;
    }
    return 0;
}

#endif

void device_busy_set(struct device *busy_dev)
{
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
    atomic_set_bit((atomic_t *) __device_busy_start,
                   (busy_dev - __device_init_start));
#else
    ARG_UNUSED(busy_dev);
#endif
}

void device_busy_clear(struct device *busy_dev)
{
#ifdef CONFIG_DEVICE_POWER_MANAGEMENT
    atomic_clear_bit((atomic_t *) __device_busy_start,
                     (busy_dev - __device_init_start));
#else
    ARG_UNUSED(busy_dev);
#endif
}
