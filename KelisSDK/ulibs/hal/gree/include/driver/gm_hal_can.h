#ifndef _GM_HAL_CAN_H_
#define _GM_HAL_CAN_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include <gm_common.h>
#include <can/gm_ll_can.h>


//#define CAN2                 1
#define CAN1                 1

#ifdef CAN1
#define       CAN          CAN0_DEV
#endif


#ifdef CAN2
#define       CAN          CAN1_DEV
#endif



struct CAN_Handle;

/* CAN node commnicate status**
 */
typedef enum
{
    CAN_COMMUNICATE_IDLE                                        =0x0,                       /**< CAN communicate status is idle */
    CAN_COMMUNICATE_ERR                                             ,                       /**< CAN communicate status is error */
    CAN_TX_COMMUNICATE_SUCCESS                                                              /**< CAN TX communicate status is success*/
} CAN_COMMUNICATE_STATUS_T;

/**
 * TX CAN frame mode: Block eead Or noblock
 */
typedef enum
{
    CAN_NOBLOCK                                                 =0x0,                       /**< Noblock to TX CAN frame, function will return immediately */
    CAN_BLOCK                                                                               /**< Block to TX CAN frame, function will not return until TX finish */
} CAN_BLOCKSTATUS_T;




/**
 * Urs revive event type from driver
 */
typedef enum
{
    CAN_EVENT_WAKEUP                    = 0x00, /**< When CAN wakeup from sleep mode will send this event to usr */
    CAN_EVENT_BUS_ERROR                       , /**< When CAN RX/TX generate bus error will send this event to usr */
    CAN_EVENT_ERROR_PASSIVE                   , /**< When CAN enter passive error status will send this event to usr */
    CAN_EVENT_ERROR_WARNING                   , /**< When CAN enter waring error status will send this event to usr */
    CAN_EVENT_ARBIT_LOST                      , /**< When CAN bus generate arbit will send this event to usr */
    CAN_EVENT_DATA_OVER                       , /**< When CAN RX fifo overflow will send this event to usr */
    CAN_EVENT_TRANSMIT_SUCCESS                , /**< When one TX frame success will send this event to usr */
    CAN_EVENT_RECEIVE_SUCCESS                   /**< WhenoOne RX frame success will send this event to usr */
} CAN_Event_T;

/**
 * CAN frame format
 */
typedef enum
{
    CAN_STAND_FRAME                     = 0x0,  /**< CAN frame format is stand frame */
    CAN_EXTEND_FRAME                            /**< CAN frame format is extern frame */
} CAN_FRAME_FORMAT_T;

/**
 * CAN frame type
 */
typedef enum
{
    CAN_DATA_FRAME                      = 0x0,  /**< CAN frame type is data frame */
    CAN_REMOTE_FRAME                         ,  /**< CAN frame type is remote frame */
    CAN_SELF_DATA_FRAME                      ,  /**< CAN frame type is self data frame */
    CAN_SELF_REMOTE_FRAME                       /**< CAN frame type is remote frame */
} CAN_FRAME_TYPE_T;

/**
 * CAN work mode
 */
typedef enum
{
    CAN_INIT_MODE                       = 0x0,  /**< CAN workmode is init mode */
    CAN_STAND_MODE                          ,  /**< CAN workoode is standard mode */
    CAN_LISTEN_MODE                          ,  /**< CAN workmode is listen mode */
    CAN_SELFTEST_MODE                           /**< CAN workMode is self test mode */
} CAN_WORK_MODE_T;

/**
 * CAN tq clock cycle
 */
typedef enum
{
    CAN_SJW_1TQ                         = 0x0,  /**< 1 Tq clock cycle */
    CAN_SJW_2TQ                              ,  /**< 2 Tq clock cycle */
    CAN_SJW_3TQ                              ,  /**< 3 Tq clock cycle */
    CAN_SJW_4TQ                              ,  /**< 4 Tq clock cycle */
} CAN_SJW_T;

/**
 * CAN sample times
 */
typedef enum
{
    CAN_SAM_1                           = 0x0,  /**< sample 1 time for high baudRate */
    CAN_SAM_3                                   /**< sample 3 time for low baudRate */
} CAN_SAM_T;

/**
 * CAN segment point
 */
typedef enum
{
    CAN_TSEG_1TQ                        = 0x0,  /**< 1 Tq clock cycle */
    CAN_TSEG_2TQ                             ,  /**< 2 Tq clock cycle */
    CAN_TSEG_3TQ                             ,  /**< 3 Tq clock cycle */
    CAN_TSEG_4TQ                             ,  /**< 4 Tq clock cycle */
    CAN_TSEG_5TQ                             ,  /**< 5 Tq clock cycle */
    CAN_TSEG_6TQ                             ,  /**< 6 Tq clock cycle */
    CAN_TSEG_7TQ                             ,  /**< 7 Tq clock cycle */
    CAN_TSEG_8TQ                             ,  /**< 8 Tq clock cycle */
    CAN_TSEG_9TQ                             ,  /**< 9 Tq clock cycle */
    CAN_TSEG_10TQ                            ,  /**< 10 Tq clock cycle */
    CAN_TSEG_11TQ                            ,  /**< 11 Tq clock cycle */
    CAN_TSEG_12TQ                            ,  /**< 12 Tq clock cycle */
    CAN_TSEG_13TQ                            ,  /**< 13 Tq clock cycle */
    CAN_TSEG_14TQ                            ,  /**< 14 Tq clock cycle */
    CAN_TSEG_15TQ                            ,  /**< 15 Tq clock cycle */
    CAN_TSEG_16TQ                            ,  /**< 16 Tq clock cycle */
} CAN_TESG_T;

/**
 * CAN frame infor structure
 */
typedef struct
{
    uint32_t                            id;                         /**< ID filed in the frame */
    uint8_t                             data[CAN_MAX_DATA_LENGTH];  /**< Space to storage data filed in the frame */
    uint8_t                             dataLength;                 /**< Data length filed in the frame */
    uint8_t                             r0;                         /**< R0 filed in the frame */
    uint8_t                             r1;                         /**< R1 filed in the frame */
    CAN_FRAME_FORMAT_T                  frameFormat;                /**< CAN frame format,refer to CAN_FRAME_FORMAT_T */
    CAN_FRAME_TYPE_T                    frameType;                  /**< CAN frame type,refer to CAN_FRAME_TYPE_T */
}CAN_FrameInfor_T;

/**
  * CAN handle Structure.
  */
typedef struct CAN_Handle
{
    CAN_Device_T                       *device;                     /**< Pointer To CAN device handle */
    CAN_BLOCKSTATUS_T                   txBlockStatus;              /**< CAN_BLOCK : polling to send frame
                                                                         CAN_NOBLOCK: noblock to send Frame */
    volatile CAN_COMMUNICATE_STATUS_T            txStatus;                   /**< TX send status:Refer to
                                                                         CAN_COMMUNICATE_STATUS_T */
    uint8_t                             rd;                         /**< Read ringbuf frame index */
    uint8_t                             wr;                         /**< Write ringbuf frame index */
    CAN_WORK_MODE_T                     workMode;                   /**< Backup CAN workmode */
    CAN_FrameInfor_T                    frameInforBuf[CAN_RX_BUF_FRAME_CNT]; /**< RX ring buf */
} CAN_Handle_T;



/**
 * CAN filterConfig structure.
 */
typedef struct
{
    uint32_t                            filterCode[16];             /**< The bit filed in the filtercode&mask is 1 means is vaild bit */
    uint32_t                            filterMask[16];             /**< The mask value to filterCode */

} CAN_FilterConfig_T;




/**
 * CAN baudrate setting structure.
 */
typedef struct
{
    uint32_t                            prescaler;                  /**< Prescaler value */
    CAN_SJW_T                           sjw;                        /**< Synchronization jump width */
    CAN_SAM_T                           sample;                     /**< Sampling */
    CAN_TESG_T                          bs1;                        /**< Bittime segment 1 */
    CAN_TESG_T                          bs2;                        /**< Bittime segment 2 */
    uint32_t                            baudRate;                   /**< CAN node communicate baudrate */
}CAN_BaudRateAttribute_T;

typedef void (*CAN_Callback_T) (CAN_Handle_T *hCAN, CAN_Event_T event, unsigned long param);

/**
 * CAN config Structure.
 */
typedef struct
{
    uint32_t                            errWarnThreshold;           /**< Error waring threshold: when CAN node error cnt get more than
                                                                         this value, it will generate interrupt and driver will sent this
                                                                         event to usr if usr enable this interrupt */
    CAN_FilterConfig_T                  filter;                     /**< Config filter to decide which message to receive */
    uint8_t                             tec;                        /**< TX error cnt value,default set to 0 */
    uint8_t                             rec;                        /**< RX error cnt value,default is set to 0 */
    CAN_WORK_MODE_T                     workMode;                   /**< Workmode,refer to CAN_WORK_MODE_T */
    CAN_Callback_T                      callback;                   /**< Drivr send usr event to call this function */
    CAN_BaudRateAttribute_T             baudRateAttribute;          /**< CAN baudrate setting,refer To CAN_BaudRateSetting_T */
}CAN_Config_T;

/**
 * @brief Initializes CAN device and CAN handle with user configuration.
 *
 * This function use to configures CAN device with user-defined settings.
 *
 * @param pCAN                          CAN device point
 * @param pConfig                       Pointer to user-defined configuration.
 * @param hCAN                          Pointer to CAN handle.
 *
 * @retval RET_OK                       CAN init success
 * @retval RET_INVPARAM                 Input paramter is error
 */
RET_CODE HAL_CAN_Init(CAN_Device_T *pCAN, CAN_Config_T *pConfig, CAN_Handle_T *hCAN);

/**
 * @brief configure CAN mode filter
 *
 * This function use to configures CAN mode filter
 *
 * @param hCAN                          Pointer to CAN handle
 * @param pFilter                       Pointer To filter node according the config about filter to decide which
 *                                      message to receive
 *
 * @retval RET_OK                       Config filter success
 * @retval RET_INVPARAM                 Input paramter is error
 */
RET_CODE HAL_CAN_ConfigFilter(CAN_Handle_T *hCAN, CAN_FilterConfig_T *pFilter);

/**
 * @brief CAN Node to tranfer message
 *
 * This Function use to CAN Node to tranfer one frame message
 *
 * @param pFrameInfor                   Pointer to user-defined frameInfor,usr should fill the struct according to
 *                                      usr seting
 *
 * @param timeoutMs                     timeoutMs != TIMEOUT_NO_BLOCKING, this function is block until it return.
 *                                      If return RET_OK,means CAN node success to transfer one frame message,
 *                                      timeoutMs = TIMEOUT_NO_BLOCKING, this function will return immediately.
 *                                      If usr get the CAN_EVENT_TRANSMIT_SUCCESS event means success to transfer
 *                                      one frame message.
 *
 * @retval RET_OK CAN                   Transfer message success
 * @retval RET_INVPARAM                 Input paramter is error
 * @retval RET_TIMEOUT                  Transfer one frame message is not success finish during the timeout value
 * @retval RET_ONGOING                  Transfer one frame message is on going.
 */
RET_CODE HAL_CAN_TransferMessage(CAN_Handle_T *hCAN, CAN_FrameInfor_T *pFrameInfor ,int32_t timeoutMs);

/**
 * @brief CAN Node RX a frame message
 *
 * This Function Use to CAN node to receive one frame message,usr should better to call this function
 * when receive CAN_EVENT_RECEIVE_SUCCESS Event at callbcak function.
 *
 * @param pFrameInfor                   Pointer to user-defined pointer frameInfor, it is output pointer,it is point to
                                        RX ringbuf address.
 * @param timeoutMs                     It will polling to get message coming before timeoutMs get To 0.
 *
 * @retval RET_OK                       CAN transfer message success
 * @retval RET_INVPARAM                 Input paramter is error
 * @retval RET_TIMEOUT                  CAN is not success get one frame message during the Timeout value
 */
RET_CODE HAL_CAN_GetMessage(CAN_Handle_T *hCAN, CAN_FrameInfor_T **pFrameInfor, uint32_t timeoutMs);
/**
 * @brief get the receive counter param
 *
 * This Function Use To get the param of CAN node receive account
 *
 */


uint32_t HAL_CAN_GetRxParam(void);

/**
 * @brief Stop CAN node to rx/tx frame
 *
 * This Function Use To set CAN node enter init mode, so it will not enable to TX/RX CAN bus Message
 *
 * @param  hCAN                         Pointer To CAN Handle.
 *
 * @retval void
 */

void HAL_CAN_Stop(CAN_Handle_T *hCAN);

/**
 * @brief Start CAN node to rx/tx frame
 *
 * This Function use to set CAN node enter workmode, so it is enable to TX/RX CAN bus Message
 *
 * @param  hCAN                         Pointer To CAN Handle.
 *
 * @retval void
 */
void HAL_CAN_Start(CAN_Handle_T *hCAN);


#ifndef USE_HAL_TEST
 void HAL_CAN_IrqHandler(void);
#endif


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GM_HAL_CAN_H_ */