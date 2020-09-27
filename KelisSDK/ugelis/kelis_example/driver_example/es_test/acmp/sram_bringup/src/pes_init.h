#include <ugelis.h>
#include <misc/printk.h>
#include <board.h>
#include <device.h>
#include <gpio.h>
#include <misc/util.h>
#include <acmp.h>
#include <gm_hal_gpio.h>

#define PES_ASNY_EN        1
#define PES_SYNC_EN        0

#define PES_RESYN_MODE     0
#define PES_POS_MODE       1
#define PES_NEG_MODE       2
#define PES_POS_NEG_MODE   3

#define MASTER_PES_OUT  0
#define MASTER_CPU      1
#define MASTER_DMA      2
#define MASTER_IO       3
#define MASTER_TIM1     4
#define MASTER_ACMP     5
#define MASTER_ADC      6
#define MASTER_RTCC     7
#define MASTER_TIM2     8
#define MASTER_TIM3     9
#define MASTER_TIM4     10
#define MASTER_TIM5     11
#define MASTER_USB      12
#define MASTER_BTIM2    13
#define MASTER_TIM8     14

#define DETECT_EN   1
#define NO_DETECT   0

#define PULSE_EVENT 0
#define LEVEL_EVENT 1

#define  SPI_BASE_ADDR          0x40003000
#define  SPI_IO_SIZE            0x1000
#define  ACMP_BASE_ADDR         0x4000d000
#define  PES_BASE_ADDR          0x40037000
#define  RTC_BASE_ADDR          0x40009000


#define AC0_EVNT_OUT_SET   HW32_REG(ACMP_BASE_ADDR+0x04) |= (1<<13)
#define AC0_EVNT_OUT_DIS   HW32_REG(ACMP_BASE_ADDR+0x20) &= ~(1<<13)
#define AC1_EVNT_OUT_SET   HW32_REG(ACMP_BASE_ADDR+0x30) |= (1<<13)
#define AC1_EVNT_OUT_DIS   HW32_REG(ACMP_BASE_ADDR+0x30) &= ~(1<<13)
#define AC0_EVNT_TRIG_EN   HW32_REG(ACMP_BASE_ADDR+0x20) |= (1<<21)
#define AC1_EVNT_TRIG_EN   HW32_REG(ACMP_BASE_ADDR+0x30) |= (1<<21)
#define AC0_EVNT_TRIG_DIS  HW32_REG(ACMP_BASE_ADDR+0x20) &= ~(1<<21)
#define AC1_EVNT_TRIG_DIS  HW32_REG(ACMP_BASE_ADDR+0x30) &= ~(1<<21)

void pes_recieve_initial(uint32_t pes_channel, uint32_t master_sel, uint32_t source_sel, uint32_t async_or_sync, uint32_t work_mode, uint32_t detect_en);
int pes_recieve_detect(uint32_t pes_channel);
void pes_send_initial(uint32_t pes_channel, uint32_t pes_evt_type);
