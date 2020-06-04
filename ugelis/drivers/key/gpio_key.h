
#ifndef _GPIO_KEY_H_
#define _GPIO_KEY_H_
#ifdef __cplusplus
extern "C" {
#endif


#include <key.h>

struct gpio_keys_button{
    char        *gpio_port;
    uint32_t    pin;
    bool        pullup;
    uint32_t    code;
    bool        press_level;
    char        *desc;
};

struct gpio_keys_button_platform_data{
    uint8_t share_delay;  //ms
    uint32_t  nr;
    struct gpio_keys_button *buttons_table;

};


struct gpio_key_data_t {
     /*workqueue:to remove key share*/
     struct k_work_q workq;
     struct k_delayed_work delaywork;
     uint32_t   key_status;

     struct gpio_keys_button *event_button;
     struct gpio_keys_button_platform_data *buttons_platform_data;
};

/*board file to call this funtion to tell gpio_key driver platform_data*/
void register_button_platform_data(struct gpio_keys_button_platform_data *platform_data);

int read_key_event(struct key_input_event *event, int timeout);

int test();


#ifdef __cplusplus
}
#endif

#endif /* _GPIO_KEY_H_ */

