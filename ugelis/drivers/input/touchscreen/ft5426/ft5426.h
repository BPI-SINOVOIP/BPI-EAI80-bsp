/**
 *
 * Copyright (C) 2019 Gree Edgeless Microelectronics. All Rights Reserved.
 *
 * @file        ft5426.h
 *
 * @author      Angrad.Yang
 *
 * @version     v1.0.0
 *
 * @date        2020/02/17
 *
 * @brief       The header file of driver FT5426
 *
 * @note
 *              2020/02/17, Angrad.Yang, v1.0.0
 *                  Initial version.
 *
 **/


#ifndef __FT5426_H__
#define __FT5426_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FT5426_ADDR              0x38

//FT5206 some definitions
#define FT_DEVIDE_MODE          0x00        //DEVIDE_MODE
#define FT_REG_NUM_FINGER       0x02        //Touch Status, Number of touch points[3:0]

#define FT_TP1_REG              0X03        //TOUCH1 position(03H-06H: TOUCH1_XH/TOUCH1_XL/TOUCH1_YH/TOUCH1_YL)
#define FT_TP2_REG              0X09        //TOUCH2 position
#define FT_TP3_REG              0X0F        //TOUCH3 position
#define FT_TP4_REG              0X15        //TOUCH4 position
#define FT_TP5_REG              0X1B        //TOUCH5 position


#define FT_ID_G_LIB_VERSION     0xA1        //Firmware Library Version
#define FT_ID_G_MODE            0xA4        //the interrupt status to host

#define ID_G_CIPHER             0XA3        //Chip vendor ID
#define ID_G_FIRMID             0XA6        //Firmware ID
#define ID_G_STATE              0xA7        //Running State
#define D_G_FT5201ID            0xA8        //CTPM Vendor ID

#define FT_ID_G_THGROUP         0x80        //Valid touching detect threshold
#define FT_ID_G_PERIODACTIVE    0x88        //Period active


#define NORMAL_SCAN_PREIOD      10         /*10ms*/
#define WORKQ_STACK_SIZE        1024
#define TS_SCAN_STACK_SIZE      1024
#define TS_EVENT_TIMEOUT        2000        /*2s*/
#define TS_FT5426_SLOT_CNT      5


struct ft5426_data_t
{
    /*workqueue:to remove key share*/
    struct k_work_q        workq;
    struct k_delayed_work  delaywork;
    struct k_mutex          reg_mutex;
    struct device *i2c;
    struct device *rst_port;
    struct device *int_port;
    char *i2c_name;
    char *rst_port_name;
    char *int_port_name;
    uint8_t  rst_pin;
    uint8_t  int_pin;
    uint8_t                ts_press;
    bool                   ts_dir;
};

struct ft5426_ts_config_t
{
    uint8_t resever;
};

/* convenience defines */
#define DEV_CFG(dev)                            \
    (( struct ft5426_ts_config_t *const)(dev)->config->config_info)
#define DEV_DATA(dev)                           \
    ((struct ft5426_data_t *)(dev)->driver_data)

static inline void ts_ft5426_claim_init(struct device *dev)
{
    struct ft5426_data_t *data = DEV_DATA(dev);
    k_mutex_init(&data->reg_mutex);
}

static inline void ts_ft5426_claim_reg(struct device *dev)
{
    struct ft5426_data_t *data = DEV_DATA(dev);
    k_mutex_lock(&data->reg_mutex, K_FOREVER);
}

static inline void ts_ft5426_release_reg(struct device *dev)
{
    struct ft5426_data_t *data = DEV_DATA(dev);
    k_mutex_unlock(&data->reg_mutex);
}

#ifdef __cplusplus
}
#endif

#endif /*__FT5426_H__ */
















