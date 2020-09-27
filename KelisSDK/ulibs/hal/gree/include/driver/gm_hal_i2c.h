/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          i2c_dev.h
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/07/06
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/07/06, wizephen.wang, V1.0.0
 *        Initial version.
 */

#ifndef _I2C_DEV_H_
#define _I2C_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif
#define  DMA_66XX_OK   1
#include <gm_hal_dma.h>
#include<gm_common.h>

//#include <systick.h>

//define NO_OS
#define SYSTICK_MS_TICKS  ((SYS_CLOCK)/1000)
#define SYSTICK_US_TICKS  ((SYS_CLOCK)/1000000)
static volatile uint32_t  s_sysTime = 0;
#define SYSTICK_RELOAD_VALUE SYSTICK_MS_TICKS

/*I2C clock is 8MHz*/
#define I2C_CLOCKFREQUENCY_8M     8000000
#define I2C_CLOCKFREQUENCY_25M     25000000
/*I2C clock is 16MHz*/
#define I2C_CLOCKFREQUENCY_16M   16000000

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define IO_BIT_GET(reg, bit)    ((reg) & (bit))
#define IO_BIT_SET(reg, bit)    (reg)|=(bit)
#define IO_BIT_CLEAR(reg, bit)  (reg)&=~(bit)
#define IO_BIT_TOGGLE(reg, bit) (reg)^=(bit)
#define IO_BITS_GET(reg, bits)  ((reg)&(bits))
#define IO_BITS_SET(reg, bits)  (reg)|=(bits)
#define IO_WRITE_REG(reg, val)  ((reg) = (val))
#define IO_READ_REG(reg)        ((reg))
#define IO_BITMASK_SET(reg, bitmask, setmask)    (reg) = (((reg) & (~(bitmask))) | (setmask))

#define IO_READB(addr)          *((const volatile uint8_t *) (addr))
#define IO_READW(addr)          *((const volatile uint16_t *) (addr))
#define IO_READL(addr)          *((const volatile uint32_t *) (addr))
#define IO_WRITEB(val,addr)     *(volatile uint8_t *) (addr) = val
#define IO_WRITEW(val,addr)     *(volatile uint16_t *) (addr) = val
#define IO_WRITEL(val,addr)     *(volatile uint32_t *) (addr) = val

#define __IO  volatile
#define NULL    (0)
#define ASSERT(expr)          do{}while(0)

#if 1
typedef int     RET_CODE; /* 32 bits */
#define RET_OK       0x00  /**< Function call returns succesfully  */
#define RET_ERROR    0x01  /**< Function fails because of some error condition  */
#define RET_BUSY     0x02  /**< Function fails because device is busy */
#define RET_TIMEOUT  0x03  /**< Function fails because transaction is not over within specified time */
#define RET_INVPARAM 0x04  /**< Function fails because parameter is invalid */
#define RET_ONGOING  0x05  /**< Non-blocking function returns if transaction is on progress and not finished */

/* Declaration of I2C handle structure */
struct I2C_Handle;

/** Prototype of the user callback function */
typedef void (*I2C_Callback_T)(struct I2C_Handle *hI2C, uint8_t event, unsigned long param);
typedef struct
{
    __IO uint32_t CTRL;        /**< I2C Control Register, offset: 0x00 */
    __IO uint32_t TSAR;        /**< Local & Target Address Register, offset: 0x04 */
    __IO uint32_t FLT;         /**< Filter Register, offset: 0x08 */
    __IO uint32_t HLP;         /**< High & Low Period Clock Register, offset: 0x0C */
    __IO uint32_t SH;          /**< Setup & Hold Time Register, offset: 0x10 */
    __IO uint32_t DT;          /**< Data or Command  Register, offset: 0x14 */
    __IO uint32_t TH;          /**< Threshold for Tx/Rx FIFO Register, offset: 0x18 */
    __IO uint32_t LV;          /**< Tx/Rx FIFO Level Register, offset: 0x1C */
    __IO uint32_t TMO;         /**< Timeout Register, offset: 0x20 */
    __IO uint32_t DMA;         /**< DMA Control Register, offset: 0x24 */
    __IO uint32_t ST;          /**< Status Register, offset: 0x28 */
    __IO uint32_t ERR;         /**< Error Source Register, offset: 0x2C */
    __IO uint32_t IE;          /**< Interrupt Enable Register, offset: 0x30 */
    __IO uint32_t IS;          /**< Interrupt Status Register, offset: 0x34 */
    __IO uint32_t RESERVED[49];
    __IO uint32_t VER;         /**< I2C Version Register, offset: 0xFC */
} I2C_Device_T;
typedef struct
{
    uint32_t startTime;       /* Start time by millisecond */
    uint32_t wakeupPeriod;    /* Wakeup interval */
    uint32_t timeout;         /* Expected timeout interval by millisecond */
} i2c_WaitTimer_T;
/**
 * I2C event type
 */
typedef enum
{
    I2C_EVENT_TRANSFER_DONE     = 0x00,  /**< I2C transaction is completed */
    I2C_EVENT_TX_FIFO_OVERFLOW  = 0x01,  /**< I2C TxFIFO overflows */
    I2C_EVENT_RX_FIFO_OVERFLOW  = 0x02,  /**< I2C RxFIFO overflows */
    I2C_EVENT_RX_FIFO_UNDERFLOW = 0x03,  /**< I2C RxFIFO underlows */
    I2C_EVENT_ARB_LOST          = 0x04,  /**< I2C arbitration losts */
    I2C_EVENT_TERMINATED        = 0x08,  /**< I2C transaction terminated unexpectedly*/
    I2C_EVENT_DMAERROR          = 0x10,  /**< I2C DMA error*/
    I2C_EVNET_TRANSFER_ERROR    = 0xFF   /**< I2C transaction error */
} I2C_Event_T;

/**
 * I2C error code type
 */
typedef enum
{
    I2C_ERRCODE_NOERROR               = 0x00,  /**< No error */
    I2C_ERRCODE_ARBITRATION_LOST      = 0x01,  /**< I2C arbitration-lost error */
    I2C_ERRCODE_TERMINATED_UNEXPECTED = 0x02,  /**< I2C terminates unexpectedly */
    I2C_ERRCODE_TIMEOUT               = 0x04,  /**< Timeout */
    I2C_ERRCODE_DMAERROR              = 0x08,  /**< Error caused by DMA */
    I2C_ERRCODE_NOTREADY              = 0x10,  /**< I2C is not ready */
    I2C_ERRCODE_TRANSACERR            = 0x20,  /**< I2C transcation error */
} I2C_ErrorCode_T;
/**
 * I2C address mode
 */
typedef enum
{
    I2C_7BIT_MODE  = 0x00,  /**< 7-Bit address mode */
    I2C_10BIT_MODE = 0x01   /**< 10-Bit address mode */
} I2C_AddrMode_T;

/**
 * I2C speed mode
 */
typedef enum
{
    I2C_STANDARD_MODE = 0x00,  /**< Standard speed mode */
    I2C_FAST_MODE     = 0x01,  /**< Fast speed mode */
} I2C_SpeedMode_T;

/**
 * I2C Tx mode
 */
typedef enum
{
    I2C_TARGET_ADDRESS_MODE = 0x00,  /**< Target address mode */
    I2C_GENERAL_CALL_MODE   = 0x02,  /**< General call mode */
    I2C_START_BYTE_MODE     = 0x03   /**< Start byte mode */
} I2C_MasterTxMode_T;

/**
 * I2C working mode
 */
typedef enum
{
    I2C_MASTER_MODE = 0x00,  /**< Master mode */
    I2C_SLAVE_MODE  = 0x01   /**< Slave mode */
} I2C_WorkMode_T;

/**
 * I2C command type
 */
typedef enum
{
    I2C_DATA_CMD_NULL    = 0x00,  /**< no sense */
    I2C_DATA_CMD_RESTART = 0x01,  /**< Restart command */
    I2C_DATA_CMD_STOP    = 0x02   /**< Stop command */
} I2C_DataCmdType_T;


/**
 * I2C read/write type
 */
typedef enum
{
    I2C_DATACMD_READ     = 0x00,  /**< I2C read */
    I2C_DATACMD_WRITE    = 0x01,  /**< I2C write */
} I2C_RdWrType_T;


/**
 * I2C peripheral state
 */
typedef enum
{
    I2C_STATE_IDLE       = 0,  /**< I2C is in idle state */
    I2C_STATE_READY,           /**< I2C is ready to use */
    I2C_STATE_MASTER_TX,       /**< Master transfers data to slave */
    I2C_STATE_MASTER_RX,       /**< Master receives data to slave */
    I2C_STATE_MASTER_RX_RESTART,    /**< Master receives data to slave, restart mode */
    I2C_STATE_SLAVE_TX,        /**< Slave transfers data to master */
    I2C_STATE_SLAVE_RX,        /**< Slave receives data to master */
    I2C_STATE_SLAVE_TX_AFTER_RDREQ  /**< Slave transfers data to master after receiving a read-request */
} I2C_State_T;

/**
 * DMA configuraion structure
 */
typedef struct
{
    I2C_AddrMode_T     addrMode;      /**< I2C address mode */
    I2C_WorkMode_T     workMode;      /**< I2C working mode */
    I2C_SpeedMode_T    speedMode;     /**< I2C speed mode */
    I2C_MasterTxMode_T masterTxMode;  /**< I2C Tx mode */
    I2C_DataCmdType_T  restartMode;      /**< I2C address mode */
    uint8_t            enableDMA;     /**< I2C DMA usage option */

    uint32_t localAddress;  /**< I2C local address, valid only in slave mode*/
    uint32_t baudrate;      /**< I2C baudrate */

    uint16_t *pdmaBuffer;       /**< DMA buffer, it must be specified when DMA is enabled and DMA_MEMORY_TO_PERIPH
                                is the direction, must be 16Bit aligned by user app */
    uint16_t  dmaBufferLength;  /**< DMA buffer length*/

    I2C_Callback_T callback;    /**< callback function */
} I2C_Config_T;

typedef struct I2C_Handle
{
    uint8_t          i2c_id;        /**< I2C id */
    I2C_State_T      state;        /**< I2C data-transmission state */
    I2C_Device_T    *pDevice;      /**< I2C device handler */
    I2C_ErrorCode_T  errorCode;    /**< I2C error code */
    I2C_DataCmdType_T  restartMode;      /**< I2C address mode */

    DMA_Handle_T    *hDMA      ;   /**< DMA device handler */


    uint32_t        IRQn;               /*point to mmc_host*/
    uint8_t          enableDMA;    /**< I2C DMA usage option */

    uint8_t  txFifoDepth;  /**< I2C TX FIFO depth */
    uint8_t  rxFifoDepth;  /**< I2C RX FIFO depth */
    uint8_t  txThreshld;   /**< I2C TX threshold */
    uint8_t  rxThreshld;   /**< I2C RX threshold */
    uint8_t *pTxBuffer;    /**< Pointer to Tx buffer */
    uint8_t *pRxBuffer;    /**< Pointer to Rx buffer */
    uint8_t            memAddr;
    uint16_t  txLength;    /**< Data length for transfering */
    uint16_t  txRemained;  /**< Data remained for transfering */
    uint16_t  rxLength;    /**< Data length for receiving */
    uint16_t  rxRemained;  /**< Data remained for receiving */
    uint16_t *pdmaBuffer;  /**< DMA buffer, it must be specified when DMA is enabled and DMA_MEMORY_TO_PERIPH
                                is the direction, must be 16Bit aligned by user app */
    uint16_t  dmaBufferLength;  /**< DMA buffer length */
    I2C_Callback_T callback;    /**< callback function */

} I2C_Handle_T;

/**
 * I2C peripherals list
 */
typedef enum I2CDevn
{
    I2CDEV_0 = 0,
    I2CDEV_1 = 1,
    I2CDEV_2 = 2
} I2C_Devn_T;

/** I2C0 Device Pointer */
#define I2C0                         ((I2C_Device_T *)I2C0_BASE)
/** I2C1 Device Pointer */
#define I2C1                         ((I2C_Device_T *)I2C1_BASE)
/** I2C2 Device Pointer */
#define I2C2                         ((I2C_Device_T *)I2C2_BASE)

/** I2C Device Number */
#define  MAX_I2C_DEVICE              (3U)

/*Threshold depth for Tx FIFO */
#define MAX_TX_FIFO_DEPTH             8
/*Threshold depth for Rx FIFO */
#define MAX_RX_FIFO_DEPTH             8


/********************** I2C Control Register Bitfield ************************/
#define I2C_CTRL_EN                        ((uint32_t) 0x00000001)  /**< I2C enable */
#define I2C_CTRL_SLVM                      ((uint32_t) 0x00000002)  /**< Master or slave enable */
#define I2C_CTRL_AM                        ((uint32_t) 0x00000004)  /**< 7Bit or 10Bit option */
#define I2C_CTRL_RSTA                      ((uint32_t) 0x00000008)  /**< Restart enable */
#define I2C_CTRL_SPDM_POS                  ((uint32_t) 4)
#define I2C_CTRL_SPDM_FIELD(x)             ((uint32_t) ((x) << 4))  /**< Standard or fast speed option */
#define I2C_CTRL_GCA                       ((uint32_t) 0x00000100)  /**< ACK option for general call */
#define I2C_CTRL_GSEN                      ((uint32_t) 0x00000200)  /**< General call or start byte enable */
#define I2C_CTRL_GCSTA                     ((uint32_t) 0x00000400)  /**< General call or start byte option */
#define I2C_CTRL_ABT                       ((uint32_t) 0x00010000)  /**< Abort data transaction in maser mode */
#define I2C_CTRL_RSSE                      ((uint32_t) 0x00020000)  /**< Recover SDA stuck */
#define I2C_CTRL_SLVNA                     ((uint32_t) 0x01000000)  /**< Enable no acknowledge in slave mode */

/********************** I2C Local & Target Register Bitfield ************************/
#define I2C_TSAR_SA_POS                    ((uint32_t) 0)
#define I2C_TSAR_SA_FIELED(x)              ((uint32_t) ((x) << 0))  /**< Local address */
#define I2C_TSAR_TA_POS                    ((uint32_t) 16)
#define I2C_TSAR_TA_FIELED(x)              ((uint32_t) ((x) << 16)) /**< Target address */

/********************** High & Low Period Clock Register ************************/
#define I2C_HLP_LP_POS                     ((uint32_t) 0)
#define I2C_HLP_LP_FIELD(x)                ((uint32_t) ((x) << 0))  /**< Low period clock couter */
#define I2C_HLP_HP_POS                     ((uint32_t) 16)
#define I2C_HLP_HP_FIELD(x)                ((uint32_t) ((x) << 16)) /**< High period clock couter */

/********************** Setup & Hold Time Register ************************/
#define I2C_SH_STP_POS                     ((uint32_t) 0)           /**< SDA Setup configuration */
#define I2C_SH_SHD_POS                     ((uint32_t) 16)          /**< SDA Hold configuration */
#define I2C_SH_SHC                         ((uint32_t) (0x80000000))/**< Enable Tx or Rx hold */

/********************** Data or Command  Register ************************/
#define I2C_DT_RD                          ((uint32_t) (0x00000100)) /**< Data transaction direction (read or write) */
#define I2C_DT_ASTP                        ((uint32_t) (0x00000200)) /**< Stop I2C transaction */
#define I2C_DT_ARST                        ((uint32_t) (0x00000400)) /**< Enable restart before transaction */
#define I2C_DT_FD                          ((uint32_t) (0x00000800)) /**< First data-receive inidcation */

/***************** Threshold for Tx/Rx FIFO Register **********************/
#define I2C_TH_RTHR_POS                    ((uint32_t) 0)
#define I2C_TH_RTHR_FIELD(x)               ((uint32_t) ((x) << 0))   /**< Threshold depth for Rx */
#define I2C_TH_TTHR_POS                    ((uint32_t) 16)
#define I2C_TH_TTHR_FIELD(x)               ((uint32_t) ((x) << 16))  /**< Threshold depth for Tx */

/********************* Tx/Rx FIFO Level Register *************************/
#define I2C_LV_RLVL_POS                    ((uint32_t) 0)
#define I2C_LV_RLVL_FIELD(x)               ((uint32_t) ((x) << 0))   /**< Threshold level for Rx */
#define I2C_LV_TLVL_POS                    ((uint32_t) 16)
#define I2C_LV_TLVL_FIELD(x)               ((uint32_t) ((x) << 16))  /**< Threshold level for Tx */

/********************* DMA Control Register *************************/
#define I2C_DMA_RXEN                       ((uint32_t) (0x00000001)) /**< Enable DMA to receive data */
#define I2C_DMA_RXTHR_POS                  ((uint32_t) 8)
#define I2C_DMA_RXTHR_FIELD(x)             ((uint32_t) ((x) << 8))   /**< Threshold for DMA Rx FIFO */
#define I2C_DMA_TXEN                       ((uint32_t) (0x00010000)) /**< Enable DMA to transfer data */
#define I2C_DMA_TXTHR_POS                  ((uint32_t) 24)
#define I2C_DMA_TXTHR_FIELD(x)             ((uint32_t) ((x) << 24))  /**< Threshold for DMA Tx FIFO */

/********************* I2C STATUS register *************************/
#define I2C_ST_EN                          ((uint32_t) (0x00000001)) /**< I2C enabled*/
#define I2C_ST_SLVDB                       ((uint32_t) (0x00000002)) /**< Transaction disbaled in slave mode, caused by I2C_CTRL_EN */
#define I2C_ST_SLVDL                       ((uint32_t) (0x00000004)) /**< Data lost in slave mode, caused by I2C_CTRL_EN */
#define I2C_ST_ACT                         ((uint32_t) (0x00000100)) /**< I2C is active */
#define I2C_ST_MACT                        ((uint32_t) (0x00000200)) /**< Master mode active */
#define I2C_ST_SACT                        ((uint32_t) (0x00000400)) /**< Slave mode active */
#define I2C_ST_RXF                         ((uint32_t) (0x00001000)) /**< Rx FIFO full flag */
#define I2C_ST_RXE                         ((uint32_t) (0x00002000)) /**< Rx FIFO empty flag */
#define I2C_ST_TXF                         ((uint32_t) (0x00004000)) /**< Tx FIFO full flag */
#define I2C_ST_TXE                         ((uint32_t) (0x00008000)) /**< Tx FIFO empty flag */

/********** I2C Interrupt Enable configure register **************/
#define I2C_IE_RFU                         ((uint32_t) (0x00000001)) /**< Enable Rx FIFO underflow interrupt */
#define I2C_IE_RFO                         ((uint32_t) (0x00000002)) /**< Enable Rx FIFO overflow interrupt */
#define I2C_IE_RFAF                        ((uint32_t) (0x00000004)) /**< Enable Rx FIFO  full interrupt */
#define I2C_IE_TFO                         ((uint32_t) (0x00000008)) /**< Enable Tx FIFO overflow interrupt */

#define I2C_IE_TFAE                        ((uint32_t) (0x00000010)) /**< Enable Tx FIFO empty interrupt */
#define I2C_IE_SRREQ                       ((uint32_t) (0x00000020)) /**< Enable read request interrupt,
                                                                          response to the read request from master device */
#define I2C_IE_TXL                         ((uint32_t) (0x00000040)) /**< Enable arbitration interrupt */
#define I2C_IE_SRD                         ((uint32_t) (0x00000080)) /**< Enable receive complete interrupt,
                                                                          master issues a NACK before the STOP signal for SLV_RX_DONE */

#define I2C_IE_ACT                         ((uint32_t) (0x00000100)) /**< Enable i2c activity interrupt */
#define I2C_IE_STD                         ((uint32_t) (0x00000200)) /**< Enable stop detect interrupt */
#define I2C_IE_STAD                        ((uint32_t) (0x00000400)) /**< Enable start detect interrupt */
#define I2C_IE_GC                          ((uint32_t) (0x00000800)) /**< Enable general call interrupt */
#define I2C_IE_SL                          ((uint32_t) (0x00004000)) /**< Enable SCL stuck interrupt */

/*********** I2C Interrupt Status/Clear register ***************/
#define I2C_IS_RFU                         ((uint32_t) (0x00000001)) /**< Rx FIFO underflow interrupt flag */
#define I2C_IS_RFO                         ((uint32_t) (0x00000002)) /**< Rx FIFO overflow interrupt flag */
#define I2C_IS_RFAF                        ((uint32_t) (0x00000004)) /**< Rx FIFO Almost full interrupt flag */
#define I2C_IS_TFO                         ((uint32_t) (0x00000008)) /**< Tx FIFO overflow interrupt flag */
#define I2C_IS_TFAE                        ((uint32_t) (0x00000010)) /**< Tx FIFO empty interrupt flag */
#define I2C_IS_SRREQ                       ((uint32_t) (0x00000020)) /**< Read request interrupt,
                                                                          response to the read request from master device */
#define I2C_IS_TXL                         ((uint32_t) (0x00000040)) /**< Arbitration interrupt flag */
#define I2C_IS_SRD                         ((uint32_t) (0x00000080)) /**< Data-receive complete interrupt flag,
                                                                          master issues a NACK before the STOP signal for SLV_RX_DONE */
#define I2C_IS_ACT                         ((uint32_t) (0x00000100)) /**< I2C working interrupt flag */
#define I2C_IS_STD                         ((uint32_t) (0x00000200)) /**< Stop condition detection interrupt flag */
#define I2C_IS_STAD                        ((uint32_t) (0x00000400)) /**< Start condition detection interrupt flag */
#define I2C_IS_GC                          ((uint32_t) (0x00000800)) /**< General call interrupt flag */
#define I2C_IS_SL                          ((uint32_t) (0x00004000)) /**< Stuck low interrupt flag */
#define I2C_IS_CMI                         ((uint32_t) (0x00010000)) /**< Combined interrupt flag */


/*control register*/
/** @defgroup I2C_Exported_Macros I2C Exported Macros
 * @{
 */

/**
 * @brief Enable I2C peripheral.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_DEV_ENABLE(DEVICE)           (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_EN))

/**
 * @brief Disable I2C peripheral.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_DEV_DISABLE(DEVICE)          (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_EN))

/**
 * @brief Enable I2C slave mode.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_SLV_MODE_ENABLE(DEVICE)      (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_SLVM))

/**
 * @brief Enable I2C master mode.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_MST_MODE_ENABLE(DEVICE)      (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_SLVM))

/**
 * @brief Enable I2C restart mode.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_RESTART_ENABLE(DEVICE)       (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_RSTA))

/**
 * @brief Disable I2C restart mode.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_RESTART_DISABLE(DEVICE)      (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_RSTA))

/**
 * @brief Enable I2C general call or start byte mode.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_GCSTART_ENABLE(DEVICE)       (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_GSEN))

/**
 * @brief Disable I2C general call or start byte mode.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_GCSTART_DISABLE(DEVICE)      (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_GSEN))
#define I2C_GCSTARTSEL_GC(DEVICE)        (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_GCSTA))
#define I2C_GCSTARTSEL_START(DEVICE)     (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_GCSTA))
#define I2C_GCACK_ENABLE(DEVICE)         (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_GCA))
#define I2C_GCACK_DISABLE(DEVICE)        (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_GCA))

/**
 * @brief Enable 7Bit address mode.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_7BITADDR_ENABLE(DEVICE)      (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_AM))

/**
 * @brief Enable 10Bit address mode.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_10BITADDR_ENABLE(DEVICE)     (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_AM))


/*status register*/
/* 1: indicates I2C is enabled, and vice versa */

/**
 * @brief I2C peripheral is enabled.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_DEV_ISENABLED(DEVICE)        (IO_BIT_GET((DEVICE)->ST, I2C_CTRL_EN))

/**
 * @brief I2C peripheral is active.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_DEV_ISACTIVE(DEVICE)         (IO_BIT_GET((DEVICE)->ST,I2C_ST_ACT))

/**
 * @brief I2C peripheral Tx FIFO is full.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_ISTXFIFO_FULL(DEVICE)        (IO_BIT_GET((DEVICE)->ST,I2C_STATUS_TXFIFOFUL))

/**
 * @brief I2C peripheral Tx FIFO is empty.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_ISTXFIFO_EMPTY(DEVICE)       (IO_BIT_GET((DEVICE)->ST,I2C_STATUS_TXFIFOEMP))

/**
 * @brief I2C peripheral Rx FIFO is full.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_ISRXFIFO_FULL(DEVICE)        (IO_BIT_GET((DEVICE)->ST,I2C_STATUS_RXFIFOFUL))

/**
 * @brief I2C peripheral Rx FIFO is empty.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_ISRXFIFO_EMPTY(DEVICE)       (IO_BIT_GET((DEVICE)->ST,I2C_STATUS_RXFIFOEMP))

/**
 * @brief Get RX FIFO level.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_GET_RXFIFO_LEVEL(DEVICE)     ((uint8_t)((DEVICE)->LV))

/**
 * @brief Get TX FIFO level.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_GET_TXFIFO_LEVEL(DEVICE)     ((uint8_t)(((DEVICE)->LV) >> 16 & 0xFF))

/*set HPCC_LPCC register*/
#define I2C_SET_HPLPCC_HIGHCNT(DEVICE, HIGHCNT, LOWCNT) ((DEVICE)->HLP = ((HIGHCNT & 0xFFFF) << 16 | (LOWCNT & 0xFFFF)))

/**
 * @brief Set TX FIFO threshold.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_SET_FIFOTHRD_TX(DEVICE, TXTHRD)   IO_BITMASK_SET((DEVICE)->TH, (0xFF << 16), ((uint8_t)(TXTHRD) << 16))

/**
 * @brief Set RX FIFO threshold.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_SET_FIFOTHRD_RX(DEVICE, RXTHRD)   IO_BITMASK_SET((DEVICE)->TH, 0xFF, ((uint8_t)(RXTHRD)))

/**
 * @brief Get TX FIFO threshold.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_GET_FIFOTHRD_TX(DEVICE)  ((uint8_t)(((DEVICE)->TH) >> 16 & 0xFF))

/**
 * @brief Get RX FIFO threshold.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_GET_FIFOTHRD_RX(DEVICE)  ((uint8_t)(((DEVICE)->TH) & 0xFF))

/*Get Error Sources*/
#define I2C_GET_ERR_SOURCES(DEVICE, SOURCE) (IO_BITS_GET((DEVICE)->ERR,  SOURCE))

/* Enable Int register */
#define MASK_ALL_INTERRUPTS                             0x0000
#define I2C_DISABLE_ALLINTS(DEVICE)                     ((DEVICE)->IE = MASK_ALL_INTERRUPTS)
#define I2C_ENABLE_INTERRUPT(DEVICE, INTERRUPT)         (IO_BIT_SET((DEVICE)->IE, INTERRUPT))
#define I2C_DISABLE_INTERRUPT(DEVICE, INTERRUPT)        (IO_BIT_CLEAR((DEVICE)->IE, INTERRUPT))

/* 1: indicates INTERRUPT is enabled, and vice versa */
#define I2C_INTERRUPT_ISENABLED(DEVICE, INTERRUPT)      (IO_BIT_GET((DEVICE)->IE, INTERRUPT))
#define I2C_GET_ENABLE_INTERRUPT(DEVICE)                ((uint32_t)((DEVICE)->IE))

/*Interrupt status register*/
/* 1: indicats INTERRUPT is active */
#define I2C_INTERRUPT_ISACTIVE(DEVICE, INTERRUPT)       (IO_BIT_GET((DEVICE)->IS, INTERRUPT))

/* Clear individual interrupt */
/**
 * @brief Clear stuck interrupt.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_CLEAR_STUCKLOW(DEVICE)              ((DEVICE)->IS = I2C_IS_SL)
#define I2C_CLEAR_GC(DEVICE)                    ((DEVICE)->IS = I2C_IS_GC)
#define I2C_CLEAR_SATRT_DET(DEVICE)             ((DEVICE)->IS = I2C_IS_STAD)
#define I2C_CLEAR_STOP_DET(DEVICE)              ((DEVICE)->IS = I2C_IS_STD)
#define I2C_CLEAR_ACTIVITY(DEVICE)              ((DEVICE)->IS = I2C_IS_ACT)
#define I2C_CLEAR_RX_DONE(DEVICE)               ((DEVICE)->IS = I2C_IS_SRD)
#define I2C_CLEAR_ABRT_LOST(DEVICE)             ((DEVICE)->IS = I2C_IS_TXL)
#define I2C_CLEAR_SLV_RD_REQ(DEVICE)            ((DEVICE)->IS = I2C_IS_SRREQ)
#define I2C_CLEAR_TXFIFO_ALMOSTEMPTY(DEVICE)    ((DEVICE)->IS = I2C_IS_TFAE)
#define I2C_CLEAR_TXFIFO_OVER(DEVICE)           ((DEVICE)->IS = I2C_IS_TFO)
#define I2C_CLEAR_RXFIFO_ALMOSTFULL(DEVICE)     ((DEVICE)->IS = I2C_IS_RFAF)
#define I2C_CLEAR_RXFIFO_OV(DEVICE)             ((DEVICE)->IS = I2C_IS_RFO)
#define I2C_CLEAR_RXFIFO_UNDER(DEVICE)          ((DEVICE)->IS = I2C_IS_RFU)

/* Clear combined interrupts, write 1 to clear all interrupts */
#define I2C_CLEAR_COMBINEDINT(DEVICE)           ((DEVICE)->IS = I2C_IS_CMI)

/* I2C DMA control register */
/**
 * @brief Enable DMA Tx.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_DMACTRL_TXEN_ENABLE(DEVICE)         (IO_BIT_SET((DEVICE)->DMA, I2C_DMA_TXEN))

/**
 * @brief Enable DMA Rx.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_DMACTRL_RXEN_ENABLE(DEVICE)         (IO_BIT_SET((DEVICE)->DMA, I2C_DMA_RXEN))
/**
 * @}
 */


/**
*   U2C  PINMUX
*/
#define GPIOI0_AF0_SCL    ((uint8_t)0x00)    /**< I2C0 alternate function mapping */
#define GPIOI1_AF0_SDA    ((uint8_t)0x00)    /**< I2C0 alternate function mapping */
#define GPIOA13_AF0_SCL   ((uint8_t)0x00)    /**< I2C1 alternate function mapping */
#define GPIOA12_AF0_SDA   ((uint8_t)0x00)    /**< I2C1 alternate function mapping */
#define GPIOE10_AFD_SCL   ((uint8_t)0x0D)    /**< I2C2 alternate function mapping */
#define GPIOE12_AF0_SDA   ((uint8_t)0x00)    /**< I2C2 alternate function mapping */


/**
 * @brief Initialize I2C peripheral with specified parameters.
 *
 * @param pI2C Pointer to I2C device handler.
 * @param pConfig Pointer to I2C configuration structure.
 * @param hI2C Pointer to I2C transaction handler.
 *
 * @retval RET_OK Api was successfully executed.
 * @retval RET_BUSY DMA peripheral is being used.
 */
RET_CODE I2C_Init(I2C_Device_T *pI2C, I2C_Config_T *pConfig, I2C_Handle_T *hI2C);

/**
 * @brief DeInitializes the I2C peripheral.
 *
 * @param hI2C Pointer to I2C transaction handler.
 *
 * @retval Always return RET_OK.
 */
RET_CODE I2C_DeInit(I2C_Handle_T *hI2C);

/**
 *@brief Master I2C peripheral transmits bytes of data in Polling
 *       or Interrupt Mode depending on timeout parameter.
 *
 *@param hI2C A pointer to I2C_Handle_T structure containing Handle
 *                   Information of driver for a specified I2C peripheral.
 *@param targetAddress Target slave address
 *@param pData A pointer to data buffer to be transferred.
 *@param size The count of data to be transferred.
 *@param timeout In Microsecond unit. It timeouts if I2C master does not transfer the
 *              expected size of data out within the given timeout duration.
 *              Specially, TIMEOUT_NO_BLOCKING indicates that Master
 *              transfers data in Interrupt Mode. And any other than TIMEOUT_NO_BLOCKING
 *              means that data transmission will proceed in Polling mode.
 *
 * @retval RET_OK Api was successfully executed.
 * @retval RET_TIMEOUT Timeout happens.
 * @retval RET_ERROR Failure occurred while api was being executed.
 */
RET_CODE I2C_MasterTransmit(I2C_Handle_T *hI2C, uint16_t targetAddress, uint8_t *pData, uint16_t size, int32_t timeout);
//RET_CODE I2C_MasterTransmit(I2C_Handle_T *hI2C, uint16_t targetAddress, uint32_t *pData, uint16_t size, int32_t timeout);


/**
 *@brief Master I2C peripheral receives bytes of data in Polling
 *       or Interrupt Mode depending on timeout parameter.
 *
 *@param hI2C A pointer to I2C_Handle_T structure containing Handle
 *                   Information of driver for a specified I2C peripheral.
 *@param targetAddress Target slave address
 *@param pData  A pointer to data buffer to be received.
 *@param size The count of data to be received.
 *@param timeout  In Microsecond unit. It timeouts if I2C master does not receive the
 *               expected size of data within the given timeout duration.
 *               Specially, TIMEOUT_NO_BLOCKING indicates that Master
 *               receives data in Interrupt Mode. And any other than TIMEOUT_NO_BLOCKING
 *               means that data-receiving will proceed in Polling mode.
 *
 * @retval RET_OK Api was successfully executed.
 * @retval RET_TIMEOUT Timeout happens.
 * @retval RET_ERROR Failure occurred while api was being executed.
 */
RET_CODE I2C_MasterReceive(I2C_Handle_T *hI2C, uint16_t targetAddress, uint8_t *pData, uint16_t size, int32_t timeout);

/**
 *@brief Slave I2C peripheral transmits bytes of data in Polling
 *       or Interrupt Mode depending on timeout parameter.
 *
 *@param hI2C A pointer to I2C_Handle_T structure containing Handle
 *               Information of driver for a specified I2C peripheral.
 *@param pData A pointer to data buffer to be transferred.
 *@param size The count of data to be transferred.
 *@param timeout In Microsecond unit. It timeouts if I2C slave does not transfer the
 *              expected size of data out within the given timeout duration.
 *              Specially, TIMEOUT_NO_BLOCKING indicates that Slave
 *              transfers data in Interrupt Mode. And any other than TIMEOUT_NO_BLOCKING
 *              means that data transmission will proceed in Polling mode.
 *
 * @retval RET_OK Api was successfully executed.
 * @retval RET_TIMEOUT Timeout happens.
 * @retval RET_ERROR Failure occurred while api was being executed.
 */
RET_CODE I2C_SlaveTransmit(I2C_Handle_T *hI2C, uint8_t *pData, uint16_t size, int32_t timeout);

/**
 *@brief Slave I2C peripheral receives bytes of data in Polling
 *       or Interrupt Mode depending on timeout parameter.
 *
 *@param hI2C A pointer to I2C_Handle_T structure containing Handle
 *                   Information of driver for a specified I2C peripheral.
 *@param pData A pointer to data buffer to be received.
 *@param size The count of data to be received.
 *@param timeout In Microsecond unit. It timeouts if I2C master does not slave the
 *              expected size of data within the given timeout duration.
 *              Specially, TIMEOUT_NO_BLOCKING indicates that Slave
 *              receives data in Interrupt Mode. And any other than TIMEOUT_NO_BLOCKING
 *              means that data-receiving will proceed in Polling mode.
 *
 * @retval RET_OK Api was successfully executed.
 * @retval RET_TIMEOUT Timeout happens.
 * @retval RET_ERROR Failure occurred while api was being executed.
 */
RET_CODE I2C_SlaveReceive(I2C_Handle_T *hI2C, uint8_t *pData, uint16_t size, int32_t timeout);
//RET_CODE I2C_SlaveReceive(I2C_Handle_T *hI2C, uint32_t *pData, uint16_t size, int32_t timeout);


#endif

#ifdef __cplusplus
}
#endif

#endif /*_I2C_H_*/

