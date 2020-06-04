
/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          pm_core.c
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2019/11/25
 *
 * @brief
 *
 * @note
 *    2019/11/25, Mike.Zheng, V1.0.0
 *        Initial version.
 */


#include <kernel.h>
#include <device.h>
#include <init.h>
#include <soc.h>
#include <arch/cpu.h>
#include <cortex_m/exc.h>
#include <atomic.h>
#include <pm.h>
#include <kernel_structs.h>


//#define PM_DEVICE_TIME_CLAC 1
//#define PM_DEVICE_DEBUG 1
#define PM_DEVICE_ERR   1
#define PM_CORE_ERR     1


#ifndef PM_DEVICE_DEBUG
    #define pm_device_debug(...)      do {} while (0)
#else
    #define pm_device_debug           printk
#endif

#ifndef PM_DEVICE_ERR
    #define pm_device_err(...)      do {} while (0)
#else
    #define pm_device_err           printk
#endif

#ifndef PM_CORE_ERR
    #define pm_core_err(...)      do {} while (0)
#else
    #define pm_core_err           printk
#endif



static uint32_t rtc_wakeup_sec = 0;
extern void pm_system_cpu_config_resume(void);

extern void pm_enter_soc_low_power_asm(uint32_t mode);

static struct platform_pm_ops *pm_get_ops_singleton_instance()
{

    static struct platform_pm_ops *pm_ops = NULL;
#ifdef CONFIG_SDRAM_BOOT
    if (!pm_ops)
    {
        pm_ops = malloc_ext(0, sizeof(struct platform_pm_ops));
        if (!pm_ops)
        {
            pm_core_err("Err:cant not alloc struct platform_pm_ops\n");
        }
        else
        {
            memset(pm_ops, 0, sizeof(struct platform_pm_ops));
        }
    }
#endif
    return pm_ops;
}


static u16_t pm_disable_thread_preempt(struct k_thread *thread)
{
    u16_t preempt_old ;
    unsigned int key;

    key = irq_lock();
    preempt_old = thread->base.preempt;
    thread->base.preempt = _NON_PREEMPT_THRESHOLD;
    irq_unlock(key);

    return preempt_old;
}


static void pm_enable_thread_preempt(struct k_thread *thread,  u16_t preempt)
{
    unsigned int key;

    key = irq_lock();
    thread->base.preempt = preempt;
    irq_unlock(key);

    /*do one sched*/
    k_yield();
}



static int pm_suspend_device(uint8_t pm_stat, uint32_t *vaild_pm_device_num)
{
    int ret = 0;
    int8_t idx = 0;
    int device_count = 0;
    uint32_t real_suspend_device_num = 0;
    uint8_t pm_dev_stat = 0;
    struct device *device_list = NULL;

#ifdef PM_DEVICE_TIME_CLAC
    unsigned int t_now, t_last;
#endif

    if (pm_stat == STANDBY_MODE)
    {
        pm_dev_stat = DEVICE_PM_SUSPEND_STATE;
    }
    else if (pm_stat == SHUTDOWN_MODE)
    {
        pm_dev_stat = DEVICE_PM_OFF_STATE;
    }
    else
    {
        return -EINVAL;
    }


    pm_device_debug("\nSUSPEND: ####enter pm_suspend_device#####\n");

    device_list_get(&device_list, &device_count);

    for (idx = device_count - 1; idx >= 0; idx--)
    {

        /*todo device error process*/
        pm_device_debug("\n##SUSPEND:  devece[%d]:%s start suspend ##\n", idx, device_list[idx].config->name);
#ifdef PM_DEVICE_TIME_CLAC
        t_last = k_uptime_get_32();
#endif

        ret = device_set_power_state(&device_list[idx], pm_dev_stat);
        if (ret)
        {
            pm_device_err("\n##SUSPEND:  Err devece:%s suspend ##\n", device_list[idx].config->name);
            goto out;
        }
        real_suspend_device_num++;

#ifdef PM_DEVICE_TIME_CLAC
        t_now = k_uptime_get_32();
#endif
        pm_device_debug("##SUSPEND: devece[%d]:%s finsh suspend ##\n", idx, device_list[idx].config->name);
#ifdef PM_DEVICE_TIME_CLAC
        pm_device_debug("SUSPEND: devece[%d]:%s suspend cost time:%d Ms\n", idx, device_list[idx].config->name, (t_now - t_last));
#endif
    }

    pm_device_debug("\n####SUSPEND:finish pm_suspend_device#####\n");

out:
    *vaild_pm_device_num = real_suspend_device_num;
    return ret;

}

static int pm_resume_device(uint32_t vaild_pm_device_num)
{
    int ret = 0;
    int8_t idx;
    int device_count;
    struct device *device_list;
#ifdef PM_DEVICE_TIME_CLAC
    unsigned int t_now, t_last;
#endif

    pm_device_debug("\n####RESUME: enter pm_resume_device#####\n");

    device_list_get(&device_list, &device_count);
    for (idx = (device_count - vaild_pm_device_num); idx < device_count; idx++)
    {
        /*todo device error process*/
        pm_device_debug("\n##RESUME: devece[%d]:%s start resume ##\n", idx, device_list[idx].config->name);
#ifdef PM_DEVICE_TIME_CLAC
        t_last = k_uptime_get_32();
#endif

        ret = device_set_power_state(&device_list[idx], DEVICE_PM_ACTIVE_STATE);
        if (ret)
        {
            pm_device_err("\n## Err devece[%d]:%s resume ##\n", idx, device_list[idx].config->name);
            goto out;
        }

#ifdef PM_DEVICE_TIME_CLAC
        t_now = k_uptime_get_32();
#endif
        pm_device_debug("RESUME: devece[%d]:%s fish resume ##\n", idx, device_list[idx].config->name);
#ifdef PM_DEVICE_TIME_CLAC
        pm_device_debug("##RESUME: devece[%d]:%s resume cost time:%d Ms ##\n", idx, device_list[idx].config->name, (t_now - t_last));
#endif

    }


    pm_device_debug("\n####RESUME: finish pm_resume_device#####\n");
out:
    return ret;
}


static uint32_t  pm_get_rtc_wakeup_time()
{
    return rtc_wakeup_sec;
}


static void pm_config_wakeup_src(uint8_t mode)
{
    struct platform_pm_ops *ops = pm_get_ops_singleton_instance();

    uint32_t rtc_wakeup_sec = pm_get_rtc_wakeup_time();

    ops->pm_config_wakeup_src(mode, (void *)(&rtc_wakeup_sec));
}



/*this function called by bootload*/
static void pm_sys_resume_entry()
{
    struct platform_pm_ops *ops = pm_get_ops_singleton_instance();

    /*do sys resume*/
    pm_system_cpu_config_resume();

    /*do cpu context resume*/
    ops->pm_resume_cpu_contex();
}



static void pm_save_entry_resume_from_boot()
{
    struct platform_pm_ops *ops = pm_get_ops_singleton_instance();


    void (*pEntry)();
    pEntry = pm_sys_resume_entry;

    ops->pm_save_entry_resume_from_boot((uint32_t)pEntry);
}


static void pm_save_entry_cpu_contex_resume(uint32_t entry)
{
    struct platform_pm_ops *ops = pm_get_ops_singleton_instance();

    ops->pm_save_entry_cpu_contex_resume(entry);
}

void pm_enter_soc_low_power(uint8_t mode, uint32_t entry)
{
    struct platform_pm_ops *ops = NULL;

    pm_save_entry_cpu_contex_resume(entry);

    ops = pm_get_ops_singleton_instance();

    ops->pm_soc_enter_low_power_mode(mode);

}




void __attribute__((weak)) pm_save_current_cpu_contex_asm(void)
{

}

/*fs_sync, for file system sync*/
void pm_fs_sync(void)
{

}

int pm_entry_low_power(uint8_t mode)
{
    int ret = 0;
    unsigned int key;
    u16_t preempt ;
    u32_t vaild_pm_device_num;

    /*disable thread premmit*/
    //    preempt = pm_disable_thread_preempt(_current);
    /*sync file system*/
    pm_fs_sync();
    /*do device suspend*/
    ret = pm_suspend_device(mode, &vaild_pm_device_num);
    if (ret)
    {
        goto __resume_device;
    }

    /*save resume pc entry for bootload jump*/
    pm_save_entry_resume_from_boot();

    /*disable irq*/
    key = irq_lock();
    /*save cpu_context*/
    pm_config_wakeup_src(mode);
    pm_save_current_cpu_contex_asm();

    /**
     * enter soc low power
     * 1:standy mode: resume from bootload and jump to pm_sys_resume_entry
     * and then goto irq_unlock(key) to run continue
     * 2:shutdown mode: resume will reboot system.almost like code bootup
     */
#ifndef   CONFIG_PM_DEBUG
    pm_enter_soc_low_power_asm(mode);
#endif
    printk("finsh cpu contex resume\n");
    /*enable irq*/
    irq_unlock(key);
__resume_device:
    /*do device resume*/
    ret = pm_resume_device(vaild_pm_device_num);
    /*enable thread premmit */
    //    pm_enable_thread_preempt(_current, preempt);

    return ret;
}



void register_pm_core(struct platform_pm_ops *ops)
{
    struct platform_pm_ops *pm_core_ops = pm_get_ops_singleton_instance();

    pm_core_ops->pm_config_wakeup_src                = ops->pm_config_wakeup_src;
    pm_core_ops->pm_resume_cpu_contex                = ops->pm_resume_cpu_contex;
    pm_core_ops->pm_save_entry_resume_from_boot      = ops->pm_save_entry_resume_from_boot;
    pm_core_ops->pm_save_entry_cpu_contex_resume     = ops->pm_save_entry_cpu_contex_resume;
    pm_core_ops->pm_soc_enter_low_power_mode         = ops->pm_soc_enter_low_power_mode;
}
//void (*pm_save_entry_resume_from_boot)(uint32_t entry);
//  void (*pm_save_entry_cpu_contex_resume)(uint32_t entry);

void pm_config_rtc_wakeup_time(uint32_t sec)
{
    unsigned int key;
    key = irq_lock();
    rtc_wakeup_sec = sec;
    irq_unlock(key);
}

void pm_device_can_be_suspend_set(struct device *set_dev)
{
    atomic_set_bit((atomic_t *)&set_dev->pm_device_can_be_suspended_flag, 0);
}

void pm_device_can_be_suspend_clear(struct device *clear_dev)
{
    atomic_clear_bit((atomic_t *)&clear_dev->pm_device_can_be_suspended_flag, 0);
}

int pm_device_can_be_suspend_check(struct device *chk_dev)
{
    return atomic_test_bit((atomic_t *)&chk_dev->pm_device_can_be_suspended_flag, 0);
}

void pm_device_suspend_state_set(struct device *set_dev)
{
    atomic_set_bit((atomic_t *)&set_dev->pm_device_suspend_state, 0);
}

void pm_device_suspend_state_clear(struct device *clear_dev)
{
    atomic_clear_bit((atomic_t *)&clear_dev->pm_device_suspend_state, 0);
}

int pm_device_suspend_state_check(struct device *chk_dev)
{
    return atomic_test_bit((atomic_t *)&chk_dev->pm_device_suspend_state, 0);
}


