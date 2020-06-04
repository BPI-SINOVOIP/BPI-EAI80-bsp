#include <ugelis.h>

#include <device.h>
#include <watchdog.h>
#include "board.h"
#include <misc/printk.h>
#include "gm_hal_wwdg.h"

#define WDOG_DEV                            ((WDOG_Device_T *)WWDG_BASE) /* WDOG Device Pointer */
#define SYS_RESETSTA                        (*(volatile uint32_t *)(0x40000004))
#define SYS_SRS_WDOG                        ((uint32_t)0x00000002)            /* System Reset by Watchdog Module */


#define SYS_DB_CTRL                         (*(volatile uint32_t *)(0x40000058))

#define RESET_TIME     5000
#define WINDOW_TIME    3000
#define WDOG_TEST_WINCLOSE_TIME  1000


#define NORMAL_WIN_EWI_CASE1          1
#define WIN_EWI_CASE2                 0
#define WIN_EWI_CASE3                 0
#define NORMALEWI_WINEWI_CASE4_RESET  0

#define SYS_DGB_WDOG_WRITEDONE                 ((uint32_t)0x00010000)
#define WDOG_WAIT_WRITEDONE()               do{while((*(volatile uint32_t *)(0x40000054)) & SYS_DGB_WDOG_WRITEDONE);}while(0u);         /* Waiting for DBG_CFG_CTRL bit16 wdog write done flag */
//#define WDOG_WAIT_WRITEDONE()               do{ }while(0u);         /* Waiting for DBG_CFG_CTRL bit16 wdog write done flag */
#define WDOG_UNLOCK(pWDOG)                  do{WDOG_WAIT_WRITEDONE();pWDOG->KR = 0x1acce551;}while(0u)          /* Ulock WDOG Reg,Acess WDOG Reg Is Vaild */
#define WDOG_LOCK(pWDOG)                    do{WDOG_WAIT_WRITEDONE();pWDOG->KR = 0;}while(0u)          /* Lock WDOG Reg,Access WDOG Reg Is Not Vaild */

static void wwdg_RegWo(WDOG_Device_T *pDev)
{

    WDOG_Device_T *pWDOG = pDev;
    uint32_t  temp = 0;

    WDOG_UNLOCK(pWDOG);
    printk("\r\n Write wo's and ro's reg,List correct's reg:\r\n");
    temp = pWDOG->VAL;
    pWDOG->VAL = 0x0;
    if (pWDOG->VAL != 0x0)
    {
        printk("\r VALUE-write:0,Read:0x%x.It's ro.\r\n", pWDOG->VAL);
    }
    pWDOG->VAL = temp;

    temp = pWDOG->IC;
    pWDOG->IC = 0x3;
    if (pWDOG->CTRL != 0x3)
    {
        printk("\r INT_CLR-write:0x3,Read:0x%x.It's wo.\r\n", pWDOG->IC);
    }
    pWDOG->IC = temp;

    temp = pWDOG->RIS;
    pWDOG->RIS = 0xff;
    if (pWDOG->RIS != 0xff)
    {
        printk("\r RIS-write:0xFF,Read:0x%x.It's ro.\r\n", pWDOG->RIS);
    }
    pWDOG->RIS = temp;

    temp = pWDOG->MIS;
    pWDOG->MIS = 0x1;
    if (pWDOG->MIS != 0x1)
    {
        printk("\r MIS-write:0x1,Read:0x%xIt's ro.\r\n", pWDOG->MIS);
    }
    pWDOG->MIS = temp;

    temp = pWDOG->TMOR;
    pWDOG->TMOR = 0xff;
    if (pWDOG->TMOR != 0xff)
    {
        printk("\r TMOR-write:0xFF,Read:0x%x.It's wo.\r\n", pWDOG->TMOR);
    }
    pWDOG->TMOR = temp;

    temp = pWDOG->ID;
    pWDOG->ID = 0x3;
    if (pWDOG->ID != 0x3)
    {
        printk("\r ID-write:0x3,Read:0x%x.It's ro.\r\n", pWDOG->ID);
    }
    pWDOG->ID = temp;

    WDOG_LOCK(pWDOG);

}
static void wwdg_RegPrint(WDOG_Device_T *pDev)
{
    WDOG_Device_T *pWDOG = pDev;

    printk("\r load:0x%x\n", pWDOG->LD);
    printk("\r value:0x%x\n", pWDOG->VAL);
    printk("\r wdog ctrl:0x%x\n", pWDOG->CTRL);
    printk("\r INT ctrl:0x%x\n", pWDOG->IC);
    printk("\r RIS:0x%x\n", pWDOG->RIS);
    printk("\r MIS:0x%x\n", pWDOG->MIS);
    printk("\r window load:0x%x\n", pWDOG->WL);
    printk("\r LOCK:0x%x\n", pWDOG->KR);
    printk("\r TMR:0x%x\n", pWDOG->TMR);
    printk("\r TMOR:0x%x\n", pWDOG->TMOR);
    printk("\r ID:0x%x\n", pWDOG->ID);
}

static void SysRstRegStatus(void)
{


    if (SYS_RESETSTA & SYS_SRS_WDOG)
    {
        printk("\rwDog Rst Status:0x%x\n", SYS_RESETSTA);
        SYS_RESETSTA |= SYS_SRS_WDOG;
        printk("\rclear Rst Status Reg:0x%x\n", SYS_RESETSTA);
    }
    else
    {
        printk("\rother Rst:0x%x\n", SYS_RESETSTA);
    }
}
/* Debug Ctrl Reg about wdog stoped and runned when cpu halted and running*/
static void SysDebugCtlReg(uint32_t SetVal)
{
    /*
        In case 1 CPU halt,
        SYS_TICK will not active and WDG should stop counting and continue
        while recover from halt condition;
        ==>DBG_WDOG_STOP == 1'b0, WDT continue running;
        ==>DBG_WDOG_STOP == 1'b1, Debug mode ,WDT stoped, you can debug software;
        In case 2 CPU runing,
        SYS_TICK clock will active and WDG counter should continue running;
    */
    printk("\rDBGCTRL:0X%X\n", SYS_DB_CTRL);
    if (SetVal != 0)
    {
        SYS_DB_CTRL |= SetVal;
        printk("\rSet DBGCTRL:0X%X\n", SYS_DB_CTRL);
    }
    else
    {
        SYS_DB_CTRL &= ~1;
        printk("\rClear DBGCTRL:0X%X\n", SYS_DB_CTRL);
    }
}
RET_CODE WDOG_GetCountVal(WDOG_Device_T *pWDOG)
{
    uint32_t result_val = 0;

    pWDOG->KR = 0x1acce551;
    result_val =  pWDOG->VAL;
    pWDOG->KR = 0;

    return result_val;
}

/* WDT Requires a callback, there is no interrupt enable / disable. */
void wdt_cb(WDOG_Handle_T *hWDOG, WDOG_Event_T event, ulong_t Param)
{
    ARG_UNUSED(hWDOG);

#if NORMAL_WIN_EWI_CASE1

    printk("\rCASE1:EWI,the action of INT_CLEAR will be reload LOAD\n");

#endif /*#if NORMAL_WIN_EWI_CASE1*/

#if WIN_EWI_CASE2
    uint32_t nu = 1;
    WDOG_Device_T *pWWDG = NULL;

    pWWDG = hWDOG->Instance;
    printk("\rCASE2:WIN_EWI,the action of INT_CLEAR will be reload LOAD\n");
    printk("\r		In closed aero(winload < value < load), WDOG reset was triggeed by FeedWdog\n");

    while ((pWWDG->CTRL & 0X04) == 0x04)
    {
        printk("\r value=0x%x > win_load=0x%x \r\n", WDOG_GetCountVal(pWWDG), pWWDG->WL);
        HAL_WDOG_FeedWdog(hWDOG);
    }
#endif /*WIN_EWI_CASE2*/

#if WIN_EWI_CASE3

    uint32_t nu = 1;
    WDOG_Device_T *pWWDG = NULL;

    pWWDG = hWDOG->Instance;
    printk("\rCASE3:WIN_EMI,during wdog int,repeat feedwdog as 3 times\n");

    while ((pWWDG->CTRL & 0X04) == 0x04);
    do
    {
        if ((WDOG_GetCountVal(pWWDG) < pWWDG->WL)
                && (pWWDG->WL < pWWDG->LD))
        {
            printk("\r value0x%x < win_load=0x%x \r\n", WDOG_GetCountVal(pWWDG), pWWDG->WL);
            printk("\r FeedWdog\n");
            printk("\r int cnt:%d \n", nu++);
            HAL_WDOG_FeedWdog(hWDOG);
        }

        if (nu > 3)
        {
            printk("\r After 3 times, return mainloop \n");
        }
    } while (nu <= 3);


#endif /*WIN_EWI_CASE3*/


#if NORMALEWI_WINEWI_CASE4_RESET

    printk("\rCASE4:EWI,the action of INT_CLEAR will be reload LOAD,if you delay 5s ,it will be reset\n");
    while (1);
#endif /*#if NORMALEWI_WINEWI_CASE4_RESET*/


}

#define DIG_DBG 0
#define NORMAL_EWI_DB 0
#define WINDOWS_EWI_DB 0

#if DIG_DBG

/*digital test*/
#define RESET_TIME_DB     50
#define WINDOW_TIME_DB    30
#define WDOG_TEST_WINCLOSE_TIME_DB 10

void dig_dbg(void)
{
    struct wdt_config wr_cfg;
    struct device *wdt_dev;
    WDOG_Device_T *pWWDG = WDOG_DEV;
    static uint8_t  nu = 0;

    printk("DIG_DBG\n");
    wdt_dev = device_get_binding("WWDG");
    if (!wdt_dev)
    {
        printk("Cann't get GM-WWDG Device\n");
    }

    wr_cfg.timeout = RESET_TIME_DB;
    wr_cfg.windowtimeout = WINDOW_TIME_DB;
    wr_cfg.workmode = WWDG_NORMAL_MODE;
    wr_cfg.clkdiv = 0;   /*clk divide 0,1, the system didn't clear wdg int at moment*/
    wr_cfg.winLen = 15; /*set wdg clear interrupt time,(15+1)*0x40*/

    if ((wr_cfg.workmode == WWDG_NORMAL_EWI) || (wr_cfg.workmode == WWDG_WINDOW_EWI))
    {
        wr_cfg.mode = WDT_MODE_INTERRUPT_RESET;
        wr_cfg.interrupt_fn = wdt_cb;
    }
    else
    {
        wr_cfg.mode = WDT_MODE_RESET;
        wr_cfg.interrupt_fn = NULL;
    }

    wdt_set_config(wdt_dev, &wr_cfg);
    wdt_enable(wdt_dev);
    printk("win_len:%d\n", ((pWWDG->CTRL & 0x78) >> 3));
    printk("wr_cfg.workmode %d\n", wr_cfg.workmode);
    printk("clk_div 0x%x\n", *(volatile uint32_t *)(0x40000094));

#if NORMAL_EWI_DB
    wdt_reload(wdt_dev);

    while (1)
    {
        if (wr_cfg.workmode == WWDG_NORMAL_EWI)
        {
            printk("1");
        }

    }
#endif


#if WINDOWS_EWI_DB

    while (1)
    {

        printk("\r Window cnt:%d \n", nu++);
        k_sleep(WDOG_TEST_WINCLOSE_TIME_DB);
        if ((pWWDG->CTRL & 0x04) != 0x04 && nu <= 5)
        {
            printk("\r FeedWdog\n");
            wdt_reload(wdt_dev);
        }

    }

#endif
    while (1);

}


#endif




void wwdg_test(void)
{

    struct wdt_config wr_cfg;
    struct device *wdt_dev;
    WDOG_Device_T *pWWDG = WDOG_DEV;
    unsigned char  wcnt = 5;
    static uint8_t  nu = 0;

#if DIG_DBG
    dig_dbg();
#endif
    wwdg_RegPrint(pWWDG);
    wwdg_RegWo(pWWDG);
    SysRstRegStatus();
    SysDebugCtlReg(1);

    printk("Start wwdg test,sysclk:%d\n", SYS_CLOCK);

    k_sleep(1000);

    wdt_dev = device_get_binding("WWDG");
    if (!wdt_dev)
    {
        printk("Cann't get GM-WWDG Device\n");
    }

    wr_cfg.timeout = RESET_TIME;
    wr_cfg.windowtimeout = WINDOW_TIME;
    wr_cfg.workmode = CONFIG_WWDG_GM_MODE;
    wr_cfg.clkdiv = WWDG_CLKDIV_FACTOR_32;


    if ((wr_cfg.workmode == WWDG_NORMAL_EWI) || (wr_cfg.workmode == WWDG_WINDOW_EWI))
    {
        wr_cfg.mode = WDT_MODE_INTERRUPT_RESET;
        wr_cfg.winLen = 15;             /*normal_ewi or window_ewi mode,it must set winlen*/
        wr_cfg.interrupt_fn = wdt_cb;
    }
    else
    {
        wr_cfg.mode = WDT_MODE_RESET;
        wr_cfg.winLen = 0;              /*normal or window mode,it may set winlen*/
        wr_cfg.interrupt_fn = NULL;
    }

    wdt_set_config(wdt_dev, &wr_cfg);
    wdt_enable(wdt_dev);
    printk("win_len:%d\n", ((pWWDG->CTRL & 0x78) >> 3));
    printk("wr_cfg.workmode %d\n", wr_cfg.workmode);
    printk("clk_div 0x%x\n", *(volatile uint32_t *)(0x40000094));


    if (wr_cfg.workmode == WWDG_WINDOW_MODE || wr_cfg.workmode == WWDG_WINDOW_EWI)
    {

        while (1)
        {

#if NORMALEWI_WINEWI_CASE4_RESET

            printk("\r Window cnt:%d \n", nu++);
            k_sleep(WDOG_TEST_WINCLOSE_TIME);
            /*when LOAD > WIN_LOAD ,the system reset that you feed wdog*/
            wdt_reload(wdt_dev);

#else
            k_sleep(WDOG_TEST_WINCLOSE_TIME);
            printk("\r Window cnt:%d \n", nu++);

            if ((pWWDG->CTRL & 0x04) != 0x04 && nu <= 5)
            {
                printk("\r FeedWdog\n");
                wdt_reload(wdt_dev);
            }



#endif /*#if NORMAL_WIN_EWI_CASE4_RESET*/
        }

    }


    if (wr_cfg.workmode == WWDG_NORMAL_MODE || wr_cfg.workmode == WWDG_NORMAL_EWI)
    {

        while (wcnt--)
        {

            printk("waiting for .....!\n");
            k_sleep(1000);

            wdt_reload(wdt_dev);
        }
        while (1)
        {
            if (wr_cfg.workmode == WWDG_NORMAL_MODE)
            {
#if  NORMALEWI_WINEWI_CASE4_RESET
                printk("\rif you didn't FeedDog,Chip will reset after :%d ms\n", wr_cfg.timeout);
                k_sleep(1000);

#else
                printk("waiting for .....!\n");
                k_sleep(1000);

                wdt_reload(wdt_dev);
#endif
            }

            else if (wr_cfg.workmode == WWDG_NORMAL_EWI)
            {

                k_sleep(1000);  /*if you use systick delay,systick interupt will
                               snatch wwdg clear int in wwdg interrupt,if wwdgclk is same as sysclk*/
                printk("1");
            }


        }

    }

}

