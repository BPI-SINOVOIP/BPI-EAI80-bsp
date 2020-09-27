
#include <errno.h>
#include <kernel.h>
#include <ugelis.h>

#include <arch/cpu.h>
#include <misc/__assert.h>
#include <misc/printk.h>
#include <misc/mempool.h>
#include <board.h>
#include <init.h>
#include <uart.h>
#include <gpio.h>
#include <clock_control.h>
#include <key.h>
#include <string.h>

#include "gpio_key.h"

#include <input.h>

#include <clock_control/stm32_clock_control.h>

#define DEBUG()  __asm("BKPT")

//#define KEY_DEBUG  1
#define KEY_ERR    1
#define KEY_WARING 1

#ifndef KEY_DEBUG
    #define key_debug(...)      do {} while (0)
#else
    #define key_debug           printk
#endif

#ifndef KEY_ERR
    #define key_err(...)      do {} while (0)
#else
    #define key_err           printk
#endif

#ifndef KEY_WARING
    #define key_warning(...)      do {} while (0)
#else
    #define key_warning           printk
#endif


/* convenience defines */
#define DEV_DATA(dev)							\
                   ((struct gpio_key_data_t *)(dev)->driver_data)


/*need kmalloc for n*/
#define DEV_KEY_NAME                    "GPIO_KEY"
#define CONFIG_EVENT_BUFF_LENGTH        20
#define PUT_EVENT_TIMEOUT               3000 // 3s
#define WORK_DELAY                      30
#define WORKQ_STACK_SIZE                1024



static int gpio_key_init(struct device *dev);
static int config_gpio_key(struct device *dev);
static void gpio_key_delaywork_init(struct device *dev);
static void send_event_to_delaywork(struct k_work *work);
static struct gpio_keys_button  *map_button(struct device *gpio_dev, uint32_t pins)
;
static void gpio_key_callback(struct device *gpio_dev, struct gpio_callback *ca,uint32_t pins);

static struct gpio_key_data_t key_data;

extern struct gpio_keys_button_platform_data *get_buttons_platform_data();


static K_THREAD_STACK_DEFINE(wrokq_tack, WORKQ_STACK_SIZE);


K_MSGQ_DEFINE(key_event_msgq, sizeof(struct key_input_event), CONFIG_EVENT_BUFF_LENGTH, 4);

DEVICE_DEFINE(key, DEV_KEY_NAME, gpio_key_init, NULL, \
                            &key_data, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE, NULL) ;


int read_key_event(struct key_input_event *event, int timeout)
{
    int ret;

    ret = k_msgq_get(&key_event_msgq, (struct key_input_event *)event , timeout);

    return ret;

}

static int put_key_event(struct key_input_event *event, int timeout)
{
    int ret = 0;

    ret = k_msgq_put(&key_event_msgq, (struct key_input_event *)event, timeout);
    if(ret){
        key_warning("waring:key buff is full\n");
    }

    return ret;
}


void register_button_platform_data(struct gpio_keys_button_platform_data *platform_data)
{
    struct device *dev = DEVICE_GET(key);
    struct gpio_key_data_t *data = DEV_DATA(dev);

    data->buttons_platform_data = platform_data;

}


static int gpio_key_init(struct device *dev)
{
    int ret = 0;
    /*init delaywork for wave*/
    gpio_key_delaywork_init(dev);

    ret = config_gpio_key(dev);
    if(ret){
        key_err("Err:%d stm32_config_key_callbak\n",ret);
        return ret;
    }
	
    return ret;
}

static int config_gpio_key(struct device *dev)
{
    int ret = 0;
    uint8_t i = 0;
    uint8_t nbuttons = 0;
    uint32_t pull = 0;
    struct gpio_keys_button *key_button;
    struct device *dev_gpio;
    struct gpio_callback *gpio_callback_config = NULL;
    struct gpio_key_data_t *data =  DEV_DATA(dev);

    if(!data->buttons_platform_data){
        key_err("%s:Err buttons_platform_data NULL\n",__FUNCTION__);
        return -EINVAL;
    }
    nbuttons = data->buttons_platform_data->nr;

    gpio_callback_config = k_malloc(nbuttons*sizeof(struct gpio_callback));

    if(!gpio_callback_config){
        key_err("Err:%d %s : k_malloc\n",ret,__FUNCTION__);
        return ret;
    }

    /*config callback*/
    for(i = 0; i < nbuttons; i++){

        key_button = &(data->buttons_platform_data->buttons_table[i]);

        dev_gpio = device_get_binding(key_button->gpio_port);
        if(!dev_gpio){
            return -EINVAL;
        }
        /*config pin for pullup or pulldown*/
        if(key_button->pullup == true){
            pull = GPIO_PUD_PULL_UP;
        }else{
            pull = GPIO_PUD_PULL_DOWN;
        }

        /*both elge will generate gpio irq*/
        gpio_pin_configure(dev_gpio, key_button->pin,
                           GPIO_INT | pull | (GPIO_INT_EDGE|GPIO_INT_DOUBLE_EDGE));

        gpio_init_callback(&gpio_callback_config[i], gpio_key_callback, BIT(key_button->pin));

        gpio_add_callback(dev_gpio, &gpio_callback_config[i]);

        gpio_pin_enable_callback(dev_gpio, key_button->pin);
    }

    return ret;
}



static struct gpio_keys_button  *map_button(struct device *gpio_dev, uint32_t pins)
{
    uint8_t i = 0;
    uint8_t nr = 0;
    struct device *dev = DEVICE_GET(key);
    struct gpio_key_data_t *data = DEV_DATA(dev);
    struct gpio_keys_button *button = data->buttons_platform_data->buttons_table;

    nr = data->buttons_platform_data->nr;

    for(i=0; i< (nr+1); i++){
        if(( device_get_binding(button[i].gpio_port) == gpio_dev) \
             && (pins == BIT(button[i].pin))){
             return &button[i];
        }
    }

    return NULL;
}



static void gpio_key_callback(struct device *gpio_dev, struct gpio_callback *ca,uint32_t pins)
{
    struct device *dev = DEVICE_GET(key);
    struct gpio_key_data_t *data = DEV_DATA(dev);



    /*start delay work queue*/
    data->event_button = map_button(gpio_dev,pins);

    gpio_pin_read(gpio_dev, data->event_button->pin, &data->key_status);

    k_delayed_work_submit_to_queue(&data->workq,&data->delaywork, data->buttons_platform_data->share_delay);

}


static void gpio_key_delaywork_init(struct device *dev)
{
    struct gpio_key_data_t *data = DEV_DATA(dev);

	k_delayed_work_init(&data->delaywork, send_event_to_delaywork);

	k_work_q_start(&data->workq, wrokq_tack, WORKQ_STACK_SIZE,-1);

}

static void send_event_to_delaywork(struct k_work *work)
{
    uint32_t val = 0;
    struct key_input_event event;

    struct gpio_key_data_t *data = CONTAINER_OF(work, struct gpio_key_data_t, delaywork);

    struct gpio_keys_button *button = data->event_button;
    struct device *gpio_dev = device_get_binding(button->gpio_port);

    if(!gpio_dev){
        key_err("ERR:%s:gpio_dev NULL\n",__FUNCTION__);
    }

    gpio_pin_read(gpio_dev, button->pin, &val);

    if(data->key_status != val){
        return;
    }

    if(button->press_level == val){
        event.status= (bool)KEY_PRESS;
    }else{
        event.status= (bool)KEY_RELEASE;
    }

    event.type = EV_KEY;
    event.code = button->code;

    put_key_event(&event, PUT_EVENT_TIMEOUT);

}

