#ifndef _OPERATION_H__
#define _OPERATION_H__

#define  FACTOR_ADJUST_TIME     270
#define  SPEED_ADJUST_TIME       100
#define  FREQ_LIMIT    40

#define AIR_IDLE         0x0
#define AIR_DOWN       0x1
#define AIR_UP            0x2
#define AIR_STABLE    0x3
#define AIR_CHANGE   0x4

#define FACTOR_IDLE     0x0
#define FACTOR_UP         0x1
#define FACTOR_DOWN   0x2

typedef struct
{
    uint8_t reset;
    uint8_t duration;
    int    temp_delta;
    int    temperature;
    float freq;
    float indoorfan_speed;
    float target_temp;
    float indoor_temp;
    float outdoor_temp;
    float humidity;
    float coolingcapacity;
    float totalpower;
    float target_indoorfan_speed;
    struct k_sem predict_lock;
} dnn_context_t;

typedef struct
{
    bool        monitor_flag;
    uint8_t    status;
    uint8_t    threshold;
    uint8_t    capacity_status;
    uint8_t    factor_status;
    uint8_t    update_speed_duration;
    uint16_t  duration;
    uint16_t  factor_adjust_time;
    int          start_temp;
    int          end_temp;
    float       alpha;
    float       beta;
    float       delta;
    float       output_freq;
    float       output_indoorfan_speed;
    float       output_coolingcapacity;
} power_mgr_t;


void soft_powermanager_init(power_mgr_t *pm_info);
int soft_powermanager_operation(power_mgr_t *pm_info, dnn_context_t *cxt);

#endif