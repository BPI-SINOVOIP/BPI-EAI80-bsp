#ifndef __GT9147_H__
#define __GT9147_H__

#ifdef __cplusplus
extern "C" {
#endif

#define GT9147_ID           0x39313437
#define GT9147_ADDR         0x14//0x14  0x5D

#define CHECK_FRE       4

#define GT_CTRL_REG     0x8040
#define GT_CFGS_REG     0x8047
#define GT_CHECK_REG    0x80FF
#define GT_PID_REG      0x8040

#define GT_GSTID_REG    0x814E
#define GT_TP1_REG      0X8150
#define GT_TP2_REG      0X8158
#define GT_TP3_REG      0X8160
#define GT_TP4_REG      0X8168
#define GT_TP5_REG      0X8170

#define NORMAL_SCAN_PREIOD      10         /*10ms*/
#define WORKQ_STACK_SIZE        1024
#define TS_SCAN_STACK_SIZE      1024
#define TS_EVENT_TIMEOUT        2000        /*2s*/
#define TS_GT9147_SLOT_CNT      5

struct regval_list
{
    uint16_t reg;
    uint8_t  val;
};

struct gt9147_data_t
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
    bool                   ts_dir;;
};

struct gt9147_ts_config_t
{
    uint8_t resever;
};

/* convenience defines */
#define DEV_CFG(dev)                            \
    (( struct gt9147_ts_config_t *const)(dev)->config->config_info)
#define DEV_DATA(dev)                           \
    ((struct gt9147_data_t *)(dev)->driver_data)

static inline void ts_gt9147_claim_init(struct device *dev)
{
    struct gt9147_data_t *data = DEV_DATA(dev);
    k_mutex_init(&data->reg_mutex);
}

static inline void ts_gt9147_claim_reg(struct device *dev)
{
    struct gt9147_data_t *data = DEV_DATA(dev);
    k_mutex_lock(&data->reg_mutex, K_FOREVER);
}

static inline void ts_gt9147_release_reg(struct device *dev)
{
    struct gt9147_data_t *data = DEV_DATA(dev);
    k_mutex_unlock(&data->reg_mutex);
}

#ifdef __cplusplus
}
#endif

#endif