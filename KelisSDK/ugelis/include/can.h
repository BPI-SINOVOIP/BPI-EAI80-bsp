/**
 * @file
 *
 * @brief Public APIs for the CAN drivers.
 */

/*
 * Copyright (c) 2018 Alexander Wachter
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __DRIVERS_CAN_H
#define __DRIVERS_CAN_H

/**
 * @brief CAN Interface
 * @defgroup can_interface CAN Interface
 * @ingroup io_interfaces
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <ugelis/types.h>
#include <device.h>
#include <string.h>

#define DEV_CAN0_NAME "CAN0"//should config for dts



#define CAN_EX_ID      (1 << 31)
#define CAN_MAX_STD_ID (0x7FF)
#define CAN_STD_ID_MASK CAN_MAX_STD_ID
#define CAN_EXT_ID_MASK (0x1FFFFFFF)
#define CAN_MAX_DLC    (8)

/* CAN_TX_* are the error flags from tx_callback and send.*/
/** send successfully */
#define CAN_TX_OK       (0)
/** general send error */
#define CAN_TX_ERR      (1)
/** bus arbitration lost during sending */
#define CAN_TX_ARB_LOST (2)
/** controller is in bus off state */
#define CAN_TX_BUS_OFF  (3)
/** unexpected error */
#define CAN_TX_UNKNOWN  (4)

/** attach_* failed because there is no unused filter left*/
#define CAN_NO_FREE_FILTER (-1)

/** operation timed out*/
#define CAN_TIMEOUT (1)

#define CAN_FILTER_NUM  (16)




/**
 * @brief can_rtr enum
 * Define if the message is a data or remote frame
 */
enum can_evt{
    can_event_wakeup                  =0x00   ,/**< When CAN wakeup from sleep mode will send this event to usr */
    can_event_bus_error                       , /**< When CAN RX/TX generate bus error will send this event to usr */
    can_event_error_passive                   , /**< When CAN enter passive error status will send this event to usr */
    can_event_error_warning                   , /**< When CAN enter waring error status will send this event to usr */
    can_event_arbit_lost                      , /**< When CAN bus generate arbit will send this event to usr */
    can_event_data_over                       , /**< When CAN RX fifo overflow will send this event to usr */
    can_event_transmit_success                , /**< When one TX frame success will send this event to usr */
    can_event_receive_success                   /**< WhenoOne RX frame success will send this event to usr */
};


/**
 * @brief can_mode enum
 * Defines the mode of the can controller
 */
enum can_mode {
	/*Normal mode*/
	CAN_NORMAL_MODE,
	/*Controller is not allowed to send dominant bits*/
	CAN_SILENT_MODE,
	/*Controller is in loopback mode (receive own messages)*/
	CAN_LOOPBACK_MODE,
	/*Combination of loopback and silent*/
	CAN_SILENT_LOOPBACK_MODE
};

/**
 * @brief can message structure
 *
 * Used to pass can messages from userspace to the driver and
 * from driver to userspace
 *
 */

typedef enum
{
    std_msg,  
    ext_msg                          
} msg_info;

typedef enum
{
    can_data_frame                           ,  /**< CAN frame type is data frame */
    can_remote_frame                         ,  /**< CAN frame type is remote frame */
    can_self_data_frame                      ,  /**< CAN frame type is self data frame */
    can_self_remote_frame                       /**< CAN frame type is remote frame */
} msg_type;




struct can_msg {
	/** Indicates the identifier type (standard or extended)
	 * use can_ide enum for assignment
	 */
	 msg_info info;
	 msg_type type;

	/** Set the message to a transmission request instead of data frame
	 * use can_rtr enum for assignment
	 */
	u8_t  r0;
	u8_t  r1;
	/** Message identifier*/
	u32_t id;
	/** The length of the message (max. 8) in byte */
	u8_t dlc;
	/** The message data*/
	u8_t data[CAN_MAX_DLC];
} __packed;

/**
 * @brief can filter structure
 *
 * Used to pass can identifier filter information to the driver.
 * rtr_mask and *_id_mask are used to mask bits of the rtr and id fields.
 * If the mask bit is 0, the value of the corresponding bit in the id or rtr
 * field don't care for the filter matching.
 *
 */
struct can_filter {
	uint32_t  filter_code[CAN_FILTER_NUM];
	uint32_t  filter_mask[CAN_FILTER_NUM];
} __packed;



/**
 * @brief Configure operation of a host controller.
 *
 * @param dev Pointer to the device structure for the driver instance.
 * @param mode Operation mode
 * @param bitrate bus-speed in Baud/s
 *
 * @retval 0 If successful.
 * @retval -EIO General input / output error, failed to configure device.
 */

/**
 * @brief Perform data transfer to CAN bus.
 *
 * This routine provides a generic interface to perform data transfer
 * to the can bus. Use can_write() for simple write.
 * *
 * @param dev          Pointer to the device structure for the driver instance.
 * @param msg          Message to transfer.
 * @param timeout      Waiting for empty tx mailbox timeout in ms or K_FOREVER.
 * @param callback_isr Is called when message was sent or a transmission error
 *                     occurred. If null, this function is blocking until
 *                     message is sent.
 *
 * @retval 0 If successful.
 * @retval CAN_TX_* on failure.
 */
typedef int (*can_send_t)(struct device *dev, struct can_msg *msg,
			  s32_t timeout);


/**
 * @brief Attach a message queue to a single or group of identifiers.
 *
 * This routine attaches a message queue to identifiers specified by
 * a filter. Whenever the filter matches, the message is pushed to the queue
 * If a message passes more than one filter the priority of the match
 * is hardware dependent.
 * A message queue can be attached to more than one filter.
 * The message queue must me initialized before.
 * *
 * @param dev    Pointer to the device structure for the driver instance.
 * @param msgq   Pointer to the already initialized message queue.
 * @param filter Pointer to a can_filter structure defining the id filtering.
 *
 * @retval filter id on success.
 * @retval CAN_NO_FREE_FILTER if there is no filter left.
 */




/**
 * @brief Detach an isr or message queue from the identifier filtering.
 *
 * This routine detaches an isr callback  or message queue from the identifier
 * filtering.
 * *
 * @param dev       Pointer to the device structure for the driver instance.
 * @param filter_id filter id returned by can_attach_isr or can_attach_msgq.
 *
 * @retval none
 */
typedef int (*can_get_msg_t)(struct device *dev, struct can_msg *msg,s32_t timeout);



typedef void (*can_rx_tx_cb_t)(enum can_evt event,s32_t param);

typedef int (*can_set_callback_t)(struct device *dev, can_rx_tx_cb_t cb);




struct can_driver_api {
	can_send_t        can_send;
	can_get_msg_t     can_get_msg;
	can_set_callback_t    can_set_callback;
};


__syscall int can_send(struct device *dev, struct can_msg *msg,
		       s32_t timeout);

static inline int _impl_can_send(struct device *dev, struct can_msg *msg,
				 s32_t timeout)
{
	const struct can_driver_api *api = dev->driver_api;

	return api->can_send(dev, msg, timeout);
}

/*
 * Derived can APIs -- all implemented in terms of can_send()
 */


__syscall int can_get_msg(struct device *dev,struct can_msg *msg,s32_t timeout);

static inline int _impl_can_get_msg(struct device *dev,struct can_msg *msg,
					s32_t timeout)
{
	const struct can_driver_api *api = dev->driver_api;

	return api->can_get_msg(dev,msg,timeout);
}




__syscall int can_set_callback(struct device *dev,can_rx_tx_cb_t cb);

static inline int _impl_can_set_callback(struct device *dev,can_rx_tx_cb_t cb)
{
	const struct can_driver_api *api = dev->driver_api;

	return api->can_set_callback(dev,cb);
}


#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#include <syscalls/can.h>

#endif /*__DRIVERS_CAN_H*/
