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

enum pm_frame_mode
{
    SLEEP_MODE           = 0,
    STANDBY_MODE,
    SHUTDOWN_MODE,
};

enum pm_frame_hash_enable_disable
{
    HASH_DISABLE         = 0,
    HASH_ENABLE,
};

enum pm_frame_wkup_pin_enable_disable
{
    WKUP_PIN_DISABLE         = 0,
    WKUP_PIN_ENABLE,
};

enum pm_frame_wkup_pin_valid
{
    PM_RISING_EDGE      = 0,
    PM__FALLING_EDGE,
};

struct pm_frame_wkup_pin_cfg
{
    enum pm_frame_wkup_pin_enable_disable  wkup_pin_enable_disable;
    enum pm_frame_wkup_pin_valid           wkup_pin_valid;
};

enum pm_frame_sdram_size
{
    sdram_8m      = 0,
    sdram_16m,
    sdram_32m,
    sdram_64m,
};


struct pm_frame_cfg
{
    enum pm_frame_mode lp_mode;
    enum pm_frame_hash_enable_disable hash_enable_diable;
    struct pm_frame_wkup_pin_cfg wkup_pin_xpa0;
    struct pm_frame_wkup_pin_cfg wkup_pin_xpb12;
    struct pm_frame_wkup_pin_cfg wkup_pin_xpb13;
    uint8_t pm_shutdown_status_pin_inv;
    enum pm_frame_sdram_size sdram_size;
};

#define PM_CFG  ((struct pm_frame_cfg *)(0xc0000200))

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
