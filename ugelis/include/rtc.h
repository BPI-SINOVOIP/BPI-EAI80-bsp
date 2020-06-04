/*
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RTC_H_
#define _RTC_H_
#include <ugelis/types.h>
#include <device.h>
#include <misc/util.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Number of RTC ticks in a second */
#define RTC_ALARM_SECOND (32768 / CONFIG_RTC_PRESCALER)

/** Number of RTC ticks in a minute */
#define RTC_ALARM_MINUTE (RTC_ALARM_SECOND * 60)

/** Number of RTC ticks in an hour */
#define RTC_ALARM_HOUR (RTC_ALARM_MINUTE * 60)

/** Number of RTC ticks in a day */
#define RTC_ALARM_DAY (RTC_ALARM_HOUR * 24)

typedef enum
{
    RTC_CLK_SRC_LIRC	            = 0, /**< RTC clock is sourced by LIRC */
    RTC_CLK_SRC_LOSC                ,    /**< RTC clock is sourced by LOSC */
    RTC_CLK_SRC_HOSC                ,    /**< RTC clock is sourced by HOSC/HOSC_DIV_RTC */
} rtc_clk_src;

typedef struct
{
    int tv_sec;      /**< Seconds.      */
    int tv_msec;     /**< MilliSeconds. */
} rtc_timevalue;

typedef struct
{
    rtc_timevalue cycle;  /**< Time to call the timer/alarm handler periodly */
} rtc_timer;

typedef struct
{
    int sec;        /**< Seconds.       [0-59]                  */
    int min;        /**< Minutes.       [0-59]                  */
    int hour;       /**< Hours.         [0-23]                  */
    int day;        /**< Day.           [1-31]                  */
    int mon;        /**< Month.         [1-12]                  */
    int year;       /**< Year.          [1902-2038]             */
    int wday;       /**< Day of week.   [1-7] , 1-Monday ...    */
    int yday;       /**< Days in year.  [0-365]                 */
} rtc_datetime;

typedef enum
{
	RTC_CALLBACK_ALARM = 0,
	RTC_CALLBACK_TIMER = 1,
} rtc_callback_type;

struct rtc_cfg
{
	void (*callback)(rtc_callback_type type);
};

struct rtc_config {
	u32_t init_val;
	/*!< enable/disable alarm  */
	u8_t alarm_enable;
	/*!< initial configuration value for the 32bit RTC alarm value  */
	u32_t alarm_val;
	/*!< Pointer to function to call when alarm value
	 * matches current RTC value */
	void (*cb_fn)(struct device *dev);
};

typedef void (*rtc_api_enable)(struct device *dev);
typedef void (*rtc_api_disable)(struct device *dev);
typedef void (*rtc_api_reinit)(struct device *dev);
typedef int (*rtc_api_set_config)(struct device *dev,
				  struct rtc_cfg config);
typedef int (*rtc_api_set_alarm)(struct device *dev,
				 const rtc_datetime *rdt);
typedef int (*rtc_api_read)(struct device *dev);
typedef u32_t (*rtc_api_get_pending_int)(struct device *dev);


typedef int (*rtc_api_set_datetime)(struct device *dev, rtc_datetime *rdt);
typedef rtc_datetime* (*rtc_api_get_time)(struct device *dev);
typedef void (*rtc_api_stop_timer)(struct device *dev);
typedef void (*rtc_api_cancel_alarm)(struct device *dev);
typedef int (*rtc_api_start_timer)(struct device *dev, const rtc_timer *rt);
typedef int (*rtc_api_next_sec)(struct device *dev, rtc_datetime *rdt, int sec);

struct rtc_driver_api {
	rtc_api_enable enable;
	rtc_api_disable disable;
	rtc_api_reinit reinit;
	rtc_api_read read;
	rtc_api_set_config set_config;
	rtc_api_set_alarm set_alarm;
	rtc_api_get_pending_int get_pending_int;
	rtc_api_set_datetime set_datetime;
	rtc_api_get_time get_time;
	rtc_api_stop_timer stop_timer;
	rtc_api_cancel_alarm cancel_alarm;
	rtc_api_start_timer start_timer;
	rtc_api_next_sec next_sec;
};

__syscall int rtc_read(struct device *dev);

static inline int _impl_rtc_read(struct device *dev)
{
	const struct rtc_driver_api *api = dev->driver_api;

	return api->read(dev);
}

__syscall void rtc_enable(struct device *dev);

static inline void _impl_rtc_enable(struct device *dev)
{
	const struct rtc_driver_api *api = dev->driver_api;

	api->enable(dev);
}

__syscall void rtc_disable(struct device *dev);

static inline void _impl_rtc_disable(struct device *dev)
{
	const struct rtc_driver_api *api = dev->driver_api;

	api->disable(dev);
}

__syscall void rtc_reinit(struct device *dev);

static inline void _impl_rtc_reinit(struct device *dev)
{
	const struct rtc_driver_api *api = dev->driver_api;

	api->reinit(dev);
}

static inline int rtc_set_config(struct device *dev,
				 struct rtc_cfg cfg)
{
	const struct rtc_driver_api *api = dev->driver_api;

	return api->set_config(dev, cfg);
}

__syscall int rtc_set_alarm(struct device *dev, const rtc_datetime *rdt);

static inline int _impl_rtc_set_alarm(struct device *dev,
				      const rtc_datetime *rdt)
{
	const struct rtc_driver_api *api = dev->driver_api;

	return api->set_alarm(dev, rdt);
}

/**
 * @brief Function to get pending interrupts
 *
 * The purpose of this function is to return the interrupt
 * status register for the device.
 * This is especially useful when waking up from
 * low power states to check the wake up source.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 1 if the rtc interrupt is pending.
 * @retval 0 if no rtc interrupt is pending.
 */
__syscall int rtc_get_pending_int(struct device *dev);

static inline int _impl_rtc_get_pending_int(struct device *dev)
{
	struct rtc_driver_api *api;

	api = (struct rtc_driver_api *)dev->driver_api;
	return api->get_pending_int(dev);
}


__syscall int rtc_set_datetime(struct device *dev, rtc_datetime *rdt);

static inline int _impl_rtc_set_datetime(struct device *dev, rtc_datetime *rdt)
{
	struct rtc_driver_api *api;

	api = (struct rtc_driver_api *)dev->driver_api;
	return api->set_datetime(dev, rdt);
}

__syscall rtc_datetime* rtc_get_time(struct device *dev);

static inline rtc_datetime* _impl_rtc_get_time(struct device *dev)
{
	struct rtc_driver_api *api;

	api = (struct rtc_driver_api *)dev->driver_api;
	return api->get_time(dev);
}

__syscall void rtc_stop_timer(struct device *dev);

static inline void _impl_rtc_stop_timer(struct device *dev)
{
	struct rtc_driver_api *api;

	api = (struct rtc_driver_api *)dev->driver_api;
	api->stop_timer(dev);
}

__syscall void rtc_cancel_alarm(struct device *dev);

static inline void _impl_rtc_cancel_alarm(struct device *dev)
{
	struct rtc_driver_api *api;

	api = (struct rtc_driver_api *)dev->driver_api;
	api->cancel_alarm(dev);
}

__syscall int rtc_start_timer(struct device *dev, const rtc_timer *rt);

static inline int _impl_rtc_start_timer(struct device *dev, const rtc_timer *rt)
{
	struct rtc_driver_api *api;

	api = (struct rtc_driver_api *)dev->driver_api;
	return api->start_timer(dev, rt);
}

__syscall int rtc_next_sec(struct device *dev, rtc_datetime *rdt, int sec);

static inline int _impl_rtc_next_sec(struct device *dev, rtc_datetime *rdt, int sec)
{
	struct rtc_driver_api *api;

	api = (struct rtc_driver_api *)dev->driver_api;
	return api->next_sec(dev, rdt, sec);
}


#ifdef __cplusplus
}
#endif

#include <syscalls/rtc.h>

#endif
