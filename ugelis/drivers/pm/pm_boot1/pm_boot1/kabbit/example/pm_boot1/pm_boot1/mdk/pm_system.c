#include "pm_system.h"





void power_mode_enable(uint32_t mode)
{
    uint32_t reg_value;

    reg_value = readl(PWR_MOD_REG);
    reg_value = (reg_value & (~(0x03 << 4))) | (mode << 4);
    writel(reg_value, PWR_MOD_REG);
}

void pmu_enable(void)
{
    uint32_t reg_value;

    reg_value = readl(PWR_MOD_REG);
    reg_value = reg_value | 0x01;
    writel(reg_value, PWR_MOD_REG);
}

/*
    go to  sleep power mode config

*/
void goto_sleep_mode(void)
{
    SCB->SCR &= ~(1UL << 2);

    __asm("wfi\n");

}

/*
    go to  stop1 power mode config

*/
void goto_stop1_mode(void)
{

    //cofig stop1 mode enable
    power_mode_enable(POWER_MODE_STOP1);
    //pmu enable
    pmu_enable();

    //Set SLEEPDEEP = 1
    SCB->SCR |= (1UL << 2);
    //WFI or WFE go to stop1 mode
    __WFI();

}

/*
    go to  stop2 power mode config

*/
void goto_stop2_mode(void)
{

    power_mode_enable(POWER_MODE_STOP2);
    //pmu enable
    pmu_enable();

    //Set SLEEPDEEP = 1
    SCB->SCR |= (1UL << 2);
    //WFI or WFE go to stop2 mode
    __WFI();

    //__WFE();

}

/*
    go to standby power mode config

*/


void goto_standby_mode(void)
{

    //cofig stop1 mode enable
    power_mode_enable(POWER_MODE_STANDBY);
    //pmu enable
    pmu_enable();

    //Set SLEEPDEEP = 1
    SCB->SCR |= (1UL << 2);
    //WFI or WFE go to stop2 mode

    __WFI();

}

/*
    go to shut down power mode config

*/
void goto_shut_down_mode(void)
{

    //cofig stop1 mode enable
    power_mode_enable(POWER_MODE_SHUTDOWM);
    //pmu enable
    pmu_enable();

    //Set SLEEPDEEP = 1
    SCB->SCR |= (1UL << 2);
    //WFI or WFE go to shut down mode
    __WFI();



}













