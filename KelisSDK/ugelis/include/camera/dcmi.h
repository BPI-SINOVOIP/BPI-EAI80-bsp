/** @file
 *  @brief Digital camera interface APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DCMI_H
#define __DCMI_H


#include <misc/__assert.h>
#include <misc/slist.h>

#include <stdint.h>
#include <stddef.h>
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VIDIOC_CONV_COLORSPACE    0x01   /* Color space convert for YUV to RGB */ 
#define VIDIOC_S_CROP             0x02   /* Setting camera crop feature */ 
#define VIDIOC_CREATE_BUFS        0x03   /* Allocate frame buffers */
#define VIDIOC_QUERYBUF           0x04   /* Query frame buffer information */
#define VIDIOC_QBUF               0x05   /* Queue a buffer from userspace */
#define VIDIOC_DQBUF              0x06   /* Dequeue a buffer to the userspace */
#define VIDIOC_SWITCH_PATH        0x07   /* Switch camera interface pinmux */ 

enum dcmi_pixel_format {
    PIX_FMT_RAW,
    PIX_FMT_YUV422,
    PIX_FMT_RGB565,
    PIX_FMT_JPEG,
};

struct dcmi_requestbuffers {
    uint8_t  count;
    uint32_t mem_start;
    uint32_t length;
};

struct dcmi_buf {
    uint8_t     index;
    uint32_t	addr;
    uint32_t	size;
	uint32_t    timestamp;
};

struct dcmi_crop_rect {
	uint16_t left;	        /* Where to place image */
	uint16_t top;
	uint16_t width;	        /* Size of image */
	uint16_t height;	    
};

struct dcmi_format {
	uint16_t width;	      
	uint16_t height;
	enum dcmi_pixel_format pix_fmt;	 /* the pixel format of video must be a perfect match for camera senosr's output format  */  
};

/**
 * @typedef dcmi_frame_irq_callback_t
 * @brief Define the application callback function signature for DCMI.
 *
 * @param port Device struct for the LTDC device.
 */
typedef void (*dcmi_frame_irq_callback_t)(struct device *dev, uint32_t line_num);


/** @brief DISPLAY driver API definition. */
struct dcmi_driver_api {
    int (*start_streaming) (struct device *dev, uint32_t mode);

    int (*stop_streaming) (struct device *dev);

    int (*set_fmt_vid_cap)(struct device *dev, const struct dcmi_format *f);
    
    int (*get_fmt_vid_cap)(struct device *dev, struct dcmi_format *f);
	
    int (*ioctl) (struct device *dev, uint32_t cmd, void *arg);

    /** Set the callback function */
	void (*irq_callback_set)(struct device *dev, dcmi_frame_irq_callback_t cb);
};

/**
 * @brief Start camera capture.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int dcmi_start_streaming(struct device *dev, uint32_t mode);

static inline int _impl_dcmi_start_streaming(struct device *dev, uint32_t mode)
{
    const struct dcmi_driver_api *api = dev->driver_api;

	if (!api->start_streaming) {
		return -ENOTSUP;
	}

	return api->start_streaming(dev, mode);
}

/**
 * @brief Stop camera capture.
 *
 * @param dev Pointer to the device structure for the driver instance.
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int dcmi_stop_streaming(struct device *dev);

static inline int _impl_dcmi_stop_streaming(struct device *dev)
{
    const struct dcmi_driver_api *api = dev->driver_api;

	if (!api->stop_streaming) {
		return -ENOTSUP;
	}

	return api->stop_streaming(dev);
}

/**
 * @brief Setting camera input format.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fmt .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int dcmi_set_fmt_vid_cap(struct device *dev, const struct dcmi_format *f);

static inline int _impl_dcmi_set_fmt_vid_cap(struct device *dev, const struct dcmi_format *f)
{
    const struct dcmi_driver_api *api = dev->driver_api;

	if (!api->set_fmt_vid_cap) {
		return -ENOTSUP;
	}

	return api->set_fmt_vid_cap(dev, f);
}

/**
 * @brief Get camera input format.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param fmt .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int dcmi_get_fmt_vid_cap(struct device *dev, struct dcmi_format *f);

static inline int _impl_dcmi_get_fmt_vid_cap(struct device *dev, struct dcmi_format *f)
{
    const struct dcmi_driver_api *api = dev->driver_api;

	if (!api->get_fmt_vid_cap) {
		return -ENOTSUP;
	}

	return api->get_fmt_vid_cap(dev, f);
}

/**
 * @brief camera capture io command operation.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param cmd  io command.
 * @param arg  .
 *
 * @retval 0 If successful.
 * @retval Negative errno code if failure.
 */
__syscall int dcmi_ioctl(struct device *dev, uint32_t cmd, void *arg);

static inline int _impl_dcmi_ioctl(struct device *dev, uint32_t cmd, void *arg)
{
    const struct dcmi_driver_api *api = dev->driver_api;

	if (!api->ioctl) {
		return -ENOTSUP;
	}

	return api->ioctl(dev, cmd, arg);
}

/**
 * @brief Set the IRQ callback function pointer.
 *
 * This sets up the callback for IRQ. When an IRQ is triggered,
 * the specified function will be called.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param cb Pointer to the callback function.
 *
 * @return N/A
 */
static inline void dcmi_irq_callback_set(struct device *dev,
					 dcmi_frame_irq_callback_t cb)
{
	const struct dcmi_driver_api *api = dev->driver_api;

	if ((api != NULL) && (api->irq_callback_set != NULL)) {
		api->irq_callback_set(dev, cb);
	}
}

#ifdef __cplusplus
}
#endif

#include <syscalls/dcmi.h>

#endif /* __DCMI_H */
