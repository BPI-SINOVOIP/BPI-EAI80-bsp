
#include <ugelis.h>
#include <misc/printk.h>

#include <stdio.h>
#include <gm_hal_gpio.h>
#include <gm_hal_uart.h>
#include <gm_common.h>

#define MOTOR_IN1  GPIOB5
#define MOTOR_IN2  GPIOB7
#define MOTOR_IN3  GPIOF6
#define MOTOR_IN4  GPIOF5

#define MOTOR_IN1_PIN  GPIO_PIN_5
#define MOTOR_IN2_PIN  GPIO_PIN_7
#define MOTOR_IN3_PIN  GPIO_PIN_6
#define MOTOR_IN4_PIN  GPIO_PIN_5

#define MAX_SPEED    10
#define DEBUG_MOTOR   0
static uint16_t gspeed = 9;
static uint8_t  cur_motorop = 0;
static uint8_t  last_motorop;

#define STACKSIZE                4096
K_THREAD_STACK_DEFINE(motor_stack_area, STACKSIZE);
static struct k_thread motor_task_thread;

static void Gpio_FunctionConfig(void)
{
#if DEBUG_MOTOR
    GPIO_PinConfig_T GPIO_InitStruct;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    GPIO_InitStruct.pin = MOTOR_IN1_PIN | MOTOR_IN2_PIN ;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = MOTOR_IN3_PIN | MOTOR_IN4_PIN;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    PIN_CLEAR(MOTOR_IN1);
    PIN_CLEAR(MOTOR_IN2);
    PIN_CLEAR(MOTOR_IN3);
    PIN_CLEAR(MOTOR_IN4);
#endif
}

static void OutPutResult(uint8_t cmd)
{
    uint8_t tmp = 0;

    tmp = cmd & 0x01;
    if (tmp == 0x01)
    {
        PIN_SET(MOTOR_IN1);
    }
    else
    {
        PIN_CLEAR(MOTOR_IN1);
    }

    tmp = (cmd & 0x02) >> 1;
    if (tmp == 0x01)
    {
        PIN_SET(MOTOR_IN2);
    }
    else
    {
        PIN_CLEAR(MOTOR_IN2);
    }

    tmp = (cmd & 0x04) >> 2;
    if (tmp == 0x01)
    {
        PIN_SET(MOTOR_IN3);
    }
    else
    {
        PIN_CLEAR(MOTOR_IN3);
    }

    tmp = (cmd & 0x08) >> 3;
    if (tmp == 0x01)
    {
        PIN_SET(MOTOR_IN4);
    }
    else
    {
        PIN_CLEAR(MOTOR_IN4);
    }
}

void Motor_SetSpeed(uint16_t speed)
{
    if (speed < MAX_SPEED)
    {
        gspeed = speed;
    }
}

//顺时针转动
void Motor_RoundCW(void)
{
#if DEBUG_MOTOR
    uint16_t   delay = MAX_SPEED - gspeed;

    OutPutResult(8);
    k_sleep(delay);//转速调节

    OutPutResult(4);
    k_sleep(delay);

    OutPutResult(2);
    k_sleep(delay);

    OutPutResult(1);
    k_sleep(delay);
#endif
}

//逆时针转动
void Motor_RoundCCW(void)
{
#if DEBUG_MOTOR
    uint16_t   delay = MAX_SPEED - gspeed;

    OutPutResult(1);
    k_sleep(delay);//转速调节

    OutPutResult(2);
    k_sleep(delay);

    OutPutResult(4);
    k_sleep(delay);

    OutPutResult(8);
    k_sleep(delay);
#endif
}

//停止转动
void Motor_RoundStop(void)
{
#if DEBUG_MOTOR
    OutPutResult(0);
#endif
}

static void motor_process_thread()
{
    while (1)
    {
        if (cur_motorop == 0)
        {
            k_sleep(100);
        }
        else if (cur_motorop == 1)
        {
            Motor_RoundCCW();
            k_sleep(25);
        }
        else if (cur_motorop == 2)
        {
            Motor_RoundCW();
            k_sleep(25);
        }
        else if (cur_motorop == 3)
        {
            Motor_RoundStop();
            cur_motorop = 0;
        }
    }
}

void Motor_Operation()
{
    /**2. Create task to parse the received data  */
    k_tid_t motortid = k_thread_create(&motor_task_thread, motor_stack_area,
                                       K_THREAD_STACK_SIZEOF(motor_stack_area), (k_thread_entry_t)motor_process_thread,  NULL, NULL, NULL,
                                       K_HIGHEST_THREAD_PRIO, 0, K_NO_WAIT);
    k_thread_start(motortid);
}

void motor_setop(int op)
{
    last_motorop = cur_motorop;
    cur_motorop = op;
}

void Motor_Init()
{
    Gpio_FunctionConfig();
    Motor_Operation();
}

void Motor_Example(void)
{
    uint32_t i;

    //Motor_Init();


    while (1)
    {
        for (i = 0; i < 200; i++)
        {
            Motor_RoundCCW();  //逆时针转动
            k_sleep(150);
        }

        for (i = 0; i < 200; i++)
        {
            Motor_RoundCW();  //逆时针转动
            k_sleep(150);
        }
    }
}

