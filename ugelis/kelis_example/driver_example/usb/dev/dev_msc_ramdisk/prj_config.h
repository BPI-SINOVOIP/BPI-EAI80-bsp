
#ifndef _PRJ_CONFIG_H_
#define _PRJ_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*Kernel Configurtion*/
#define CONFIG_POLL
       
#define CONFIG_GM_USB_DEVICE_STACK 
#define  CONFIG_USB_MASS_STORAGE
    
/*Subsystem: Console Shell*/
#define CONFIG_CONSOLE

//#define CONFIG_IWDG_GM_DEVICE_NAME "IWDG"
//#define CONFIG_IWDG_GM_PRESCALER      64
//#define CONFIG_IWDG_GM_RELOAD_COUNTER 500
#ifdef __cplusplus
}
#endif

#endif /* _PRJ_CONFIG_H_ */
