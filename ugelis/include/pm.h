/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          pm.h
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

#ifndef __DRIVERS_PM_H
#define __DRIVERS_PM_H



#ifdef __cplusplus
extern "C" {
#endif

#include <ugelis/types.h>
#include <device.h>
#include <string.h>


#define  STANDBY_MODE   1
#define  SHUTDOWN_MODE  2



struct platform_pm_ops
{
    int (*pm_config_wakeup_src)(uint32_t low_power_mode, void *arg);
    int (*pm_soc_enter_low_power_mode)(uint32_t low_power_mode);
    void (*pm_save_entry_resume_from_boot)(uint32_t entry);
    void (*pm_save_entry_cpu_contex_resume)(uint32_t entry);
    int (*pm_resume_cpu_contex)();
};

/*gm power*/
void pm_save_current_cpu_contex_asm(void);
void register_pm_core(struct platform_pm_ops *ops);

/*usr*/
int pm_entry_low_power(uint8_t mode);
void pm_config_rtc_wakeup_time(uint32_t ms);
void pm_enter_soc_low_power(uint8_t mode, uint32_t entry);

/*device flag and state*/
void pm_device_can_be_suspend_set(struct device *set_dev);
void pm_device_can_be_suspend_clear(struct device *clear_dev);
int pm_device_can_be_suspend_check(struct device *chk_dev);

void pm_device_suspend_state_set(struct device *set_dev);
void pm_device_suspend_state_clear(struct device *clear_dev);
int pm_device_suspend_state_check(struct device *chk_dev);

#ifdef __cplusplus
}
#endif


#endif /*__DRIVERS_PM_H*/
