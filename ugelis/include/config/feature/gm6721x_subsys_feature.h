#ifndef __GM6721_SUBSYS_FEATURE_H__
#define __GM6721_SUBSYS_FEATURE_H__
/********************************************/
/* Debugging Options */
/********************************************/
#define CONFIG_PRINTK 1
#define CONFIG_EARLY_CONSOLE 1
#define CONFIG_SERIAL_HAS_DRIVER 1
#define CONFIG_SERIAL_SUPPORT_INTERRUPT 1


/********************************************/
/* Console */
/********************************************/
#ifdef CONFIG_CONSOLE
    #define CONFIG_CONSOLE_INPUT_MAX_LINE_LEN       128
    #define CONFIG_CONSOLE_HAS_DRIVER               1
    #define CONFIG_UART_CONSOLE                     1
    #ifdef CONFIG_UART_CONSOLE
        #define CONFIG_UART_GM                      1
        #define CONFIG_GM_HAL_UART                  1
        #define CONFIG_UART_INTERRUPT_DRIVEN        1
        #define CONFIG_UART_CONSOLE_INIT_PRIORITY   60
        #define CONFIG_SERIAL                       1
    #endif
    #define CONFIG_STDOUT_CONSOLE 1
#endif

/********************************************/
/* Shell */
/********************************************/
#ifdef CONFIG_SHELL
    #define CONFIG_CONSOLE_SHELL
    #define CONFIG_CONSOLE_SHELL_STACKSIZE          2000
    #define CONFIG_CONSOLE_SHELL_MAX_CMD_QUEUED     3
    #define CONFIG_CONSOLE_GETCHAR_BUFSIZE          64
    #define CONFIG_CONSOLE_PUTCHAR_BUFSIZE          512
    #define CONFIG_CONSOLE_HANDLER
    #define CONFIG_KERNEL_SHELL
#endif

/********************************************/
/* Security Options */
/********************************************/
#define CONFIG_RETPOLINE                        1
#define CONFIG_STACK_POINTER_RANDOM             0

/********************************************/
/* C Library */
/********************************************/
#define CONFIG_MINIMAL_LIBC_MALLOC_ARENA_SIZE   0

/********************************************/
/* USB Stack */
/********************************************/
//#define CONFIG_GM_USB_DEVICE_STACK
//#define  CONFIG_GM_USB_HOST_STACK

#ifdef CONFIG_GM_USB_DEVICE_STACK
    #ifndef  CONFIG_PINMUX
        #define  CONFIG_PINMUX
    #endif /*CONFIG_PINMUX*/

    #define  CONFIG_USB_PINMUX
    #define  CONFIG_GM_HAL_USB

    //    #define  CONFIG_USB_HID_MOUSE
    #ifdef   CONFIG_USB_HID_MOUSE

    #endif /*CONFIG_USB_HID_MOUSE*/


    //#define  CONFIG_USB_AUDIO
    #ifdef   CONFIG_USB_AUDIO

    #endif /*End CONFIG_USB_AUDIO */

    #ifdef   CONFIG_USB_HID_CUSTOM

    #endif /*End CONFIG_USB_HID_CUSTOM*/

    #ifdef   CONFIG_USB_VIDEO

    #endif /*End CONFIG_USB_VIDEO */

    #ifdef   CONFIG_USB_MASS_STORAGE

    #endif /* End CONFIG_USB_MASS_STORAGE */

#endif /*CONFIG_GM_USB_DEVICE_STACK*/

//#define  CONFIG_USB_MASS_STORAGE
//#define  CONFIG_USB_HID_MOUSE

//#ifdef CONFIG_USB_MASS_STORAGE
///********File Systems********/
//  #define CONFIG_FILE_SYSTEM
///******File Systems Disk*****/
//  #define CONFIG_DISK_ACCESS
//  #define CONFIG_DISK_ACCESS_USB
//#endif /*end CONFIG_USB_MASS_STORAGE*/

#ifdef   CONFIG_GM_USB_HOST_STACK
    #define  CONFIG_GM_HAL_USB
    #define  CONFIG_USB_PINMUX
    #ifndef  CONFIG_PINMUX
        #define  CONFIG_PINMUX
    #endif /*CONFIG_PINMUX*/

    #ifdef CONFIG_USB_MASS_STORAGE
        /********File Systems********/
        #define CONFIG_FILE_SYSTEM
        #define CONFIG_APP_LINK_WITH_FS
        #define CONFIG_FAT_FILESYSTEM_ELM
        #define CONFIG_FS_FATFS_NUM_FILES  4
        #define CONFIG_FS_FATFS_NUM_DIRS   8


        /******File Systems Disk*****/
        #define CONFIG_DISK_ACCESS
        #define CONFIG_DISK_ACCESS_MAX_VOLUMES 8
        #define CONFIG_DISK_ACCESS_USB
        #define CONFIG_DISK_USB_VOLUME_NAME "USB0"
    #endif /*end CONFIG_USB_MASS_STORAGE*/


    #ifdef   CONFIG_USB_HID_MOUSE

    #endif  /*end CONFIG_USB_HID_MOUSE*/

#endif /*end CONFIG_GM_USB_HOST_STACK*/


/********************************************/
/* Disk */
/********************************************/
// #define CONFIG_DISK_ACCESS

#ifdef CONFIG_DISK_ACCESS
    #define CONFIG_DISK_ACCESS_MAX_VOLUMES          8
    //#define CONFIG_DISK_ACCESS_MMC                1
    #ifdef CONFIG_DISK_ACCESS_MMC
        #define CONFIG_DISK_MMC_VOLUME_NAME         "MMC0"
    #endif

    //#define CONFIG_DISK_ACCESS_USB
    #ifdef CONFIG_DISK_ACCESS_USB
        #define CONFIG_DISK_USB_VOLUME_NAME         "USB0"
    #endif

    //#define CONFIG_DISK_ACCESS_FLASH
    #ifdef CONFIG_DISK_ACCESS_SFLASH
        #define CONFIG_SFLASH                       1
        #define CONFIG_DISK_FLASH_VOLUME_NAME       "SFLASH0"
        #define CONFIG_DISK_ERASE_BLOCK_SIZE        (0x1000)
        #define CONFIG_DISK_FLASH_MAX_RW_SIZE       (0x100)
        #define CONFIG_DISK_FLASH_ERASE_ALIGNMENT   (0x1000)
        #define CONFIG_DISK_FLASH_DEV_NAME          "SFLASH_GM_DEV"
        #define CONFIG_DISK_FLASH_START             (0x00700000)
        #define CONFIG_DISK_SECTOR_SIZE             (0x1000)
        #define CONFIG_DISK_VOLUME_SIZE             (0x000C0000)
    #endif
#endif

/********************************************/
/* File Systems */
/********************************************/
//#define CONFIG_FILE_SYSTEM
#ifdef CONFIG_FILE_SYSTEM
    #define CONFIG_APP_LINK_WITH_FS                 1
    #define CONFIG_FAT_FILESYSTEM_ELM               1
    #define CONFIG_FS_FATFS_NUM_FILES               4
    #define CONFIG_FS_FATFS_NUM_DIRS                8
#endif
/********************************************/
/* Management */
/********************************************/
#define CONFIG_MCUMGR_BUF_COUNT                 4
#define CONFIG_MCUMGR_BUF_SIZE                  384
#define CONFIG_MCUMGR_BUF_USER_DATA_SIZE        4

/********************************************/
/* Ztest Systems */
/********************************************/
#ifdef CONFIG_ZTEST
    #define CONFIG_ZTEST_STACKSIZE 1024
    #define CONFIG_ZTEST_FAIL_FAST 1
    #define CONFIG_ZTEST_ZTEST_ASSERT_VERBOSE 1
    #define CONFIG_ZTEST_MOCKING 1
    #define CONFIG_ZTEST_PARAMETER_COUNT 1
    #define ZTEST_UNITTEST 1
    #define KERNEL
#endif

/********************************************/
/* Camera options */
/********************************************/


/********************************************/
/* Storage */
/********************************************/

/********************************************/
/* General Kernel Options */
/********************************************/

/********************************************/
/* External Sources */
/********************************************/

/********************************************/
/* Cryptography */
/********************************************/

/********************************************/
/* Encoding */
/********************************************/
#define CONFIG_OPENAMP_SRC_PATH "open-amp"

/********************************************/
/* Additional libraries */
/********************************************/

/********************************************/
/* Testing */
/********************************************/
#define CONFIG_TEST_EXTRA_STACKSIZE 2048

#endif
