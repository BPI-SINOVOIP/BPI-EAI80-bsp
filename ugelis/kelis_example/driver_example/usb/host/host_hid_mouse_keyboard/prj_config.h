
#ifndef _PRJ_CONFIG_H_
#define _PRJ_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*Kernel Configurtion*/
#define CONFIG_POLL

/*Subsystem: USB Host Stack Configuration*/
#define CONFIG_GM_USB_HOST_STACK
#define CONFIG_USB_HID_MOUSE

/*Subsystem: Console Shell*/
#define CONFIG_CONSOLE

#ifdef __cplusplus
}
#endif

#endif /* _PRJ_CONFIG_H_ */
