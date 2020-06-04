/** @file
 *  @brief Digital camera sensor interface APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __CAMERA_SENSOR_H
#define __CAMERA_SENSOR_H


#include <misc/__assert.h>
#include <misc/slist.h>

#include <stdint.h>
#include <stddef.h>
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CAMERA_CID_BRIGHTNESS      0x01 /* Setting brighiness */ 
#define CAMERA_CID_CONTRAST        0x02 /* Setting contrast */ 
#define CAMERA_CID_SATURATION      0x03 /* Setting saturation */ 
#define CAMERA_CID_SHARPNESS       0x04 /* Setting sharpness */ 
#define CAMERA_CID_FLASH_MODE      0x05 /* Setting flash mode */ 
#define CAMERA_CID_FOCUS_AUTO      0x06 /* Setting single or continueous auto focus */ 
#define CAMERA_CID_EXPOSURE        0x07 /* Setting exposure */ 
#define CAMERA_CID_WHITE_BALANCE   0x08 /* Setting white balance */ 
#define CAMERA_CID_FRAME_SIZE      0x09 /* Setting frame size */

enum light_mode {
    WB_AUTO,
    WB_SUNNY,
    WB_OFFICE,
    WB_CLOUDY,
    WB_HOME,
};

enum af_mode {
    AF_SINGLE,
    AF_CONTINUEOUS,
};

enum camera_output_format {
    VIDEO_FMT_RAW,
    VIDEO_FMT_YUV422,
    VIDEO_FMT_RGB565,
    VIDEO_FMT_JPEG,
};

enum camera_mode_id {
    MODE_QVGA_320_240 = 0,
	MODE_VGA_640_480,
	MODE_XGA_1024_768,
	MODE_720P_1280_720,
	MODE_QSXGA_2592_1944,
};

enum camera_frame_rate {
	FPS_15 = 0,
	FPS_30,
};

struct video_format {
	uint16_t width;	      
	uint16_t height;
	uint8_t  framerate;	
    enum camera_output_format pixfmt;
};

/** @brief DISPLAY driver API definition. */
struct camera_driver_api {    
    int (*set_power) (struct device *dev, bool on);

    int (*set_fmt) (struct device *dev, enum camera_output_format format, enum camera_mode_id id, enum camera_frame_rate frame_rate);
    
    int (*get_fmt) (struct device *dev, struct video_format *fmt);

    int (*ioctl)(struct device *dev, uint32_t cmd, void *arg);;
	
};

/**
 * @brief Puts subdevice in power saving mode or normal operation mode.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param on .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int camera_set_power(struct device *dev, bool on);

static inline int _impl_camera_set_power(struct device *dev, bool on)
{
    const struct camera_driver_api *api = dev->driver_api;

	if (!api->set_power) {
		return -ENOTSUP;
	}

	return api->set_power(dev, on);
}

/**
 * @brief Setting camera output format.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fmt .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int camera_set_fmt(struct device *dev, enum camera_output_format format, enum camera_mode_id id, enum camera_frame_rate frame_rate);

static inline int _impl_camera_set_fmt(struct device *dev, enum camera_output_format format, enum camera_mode_id id, enum camera_frame_rate frame_rate)
{
    const struct camera_driver_api *api = dev->driver_api;

	if (!api->set_fmt) {
		return -ENOTSUP;
	}

	return api->set_fmt(dev, format, id, frame_rate);
}

/**
 * @brief Get camera output format.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fmt .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int camera_get_fmt(struct device *dev, struct video_format *fmt);

static inline int _impl_camera_get_fmt(struct device *dev, struct video_format *fmt)
{
    const struct camera_driver_api *api = dev->driver_api;

	if (!api->get_fmt) {
		return -ENOTSUP;
	}

	return api->get_fmt(dev, fmt);
}

/**
 * @brief camera io command operation.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param cmd  io command.
 * @param arg  .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int camera_ioctl(struct device *dev, uint32_t cmd, void *arg);


static inline int _impl_camera_ioctl(struct device *dev, uint32_t cmd, void *arg)
{
    const struct camera_driver_api *api = dev->driver_api;

	if (!api->ioctl) {
		return -ENOTSUP;
	}

	return api->ioctl(dev, cmd, arg);
}

#ifdef __cplusplus
}
#endif

#include <syscalls/camera_sensor.h>

#endif /* __CAMERA_SENSOR_H */
