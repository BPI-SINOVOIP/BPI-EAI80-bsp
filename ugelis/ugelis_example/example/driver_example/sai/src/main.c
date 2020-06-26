#include <ugelis.h>
#include <device.h>
#include <i2s.h>
#include <board.h>
#include <gm_hal_sai.h>
#include <gm_hal_gpio.h>
#include <pinmux.h>
#define TX_SAI_NAME "SAI_A"
#define RX_SAI_NAME "SAI_B"

//select SAI used interrupt mode or dma mode 
#define CONFIG_SAIA_INT    1
#define CONFIG_SAIB_INT    1
#define CONFIG_SAIA_DMA    0
#define CONFIG_SAIB_DMA    0

extern uint32_t s_mstTransCmplt;
extern uint32_t s_slvTransCmplt;
//#define  HW32_REG(ADDRESS)  (*((volatile unsigned long  *)(ADDRESS)))
#if  Recoder_SAI
    #define  SAI_USERBUFFER_SIZE  16000
#else
    #define  SAI_USERBUFFER_SIZE  511    /**< Specify user buffer size */
#endif
#define  RECORD_TIMES         5
uint32_t wrbuf[SAI_USERBUFFER_SIZE] = {0};
uint32_t r_rdbuf[SAI_USERBUFFER_SIZE] = {0};
void *rdbuf = r_rdbuf;
uint16_t s_rdbuf[SAI_USERBUFFER_SIZE] = {0};
uint16_t  g_copybuf[SAI_USERBUFFER_SIZE * RECORD_TIMES] = {0};
uint32_t read_buffer[SAI_USERBUFFER_SIZE * RECORD_TIMES] = {0};
#define TIMEOUT_NO_WAIT      0x00000000
#define TIMEOUT_WAIT_FOREVER 0xFFFFFFFF
#define TIMEOUT_NO_BLOCKING  0xFFFFFFFE
#define SAI_SLAVE_DEVICE     (SAI1_Block_B)
#define SAI_MASTER_DEVICE     (SAI1_Block_B)
static volatile uint32_t s_slvReceCmplt = 0;
static int32_t pingpong_offset = 0;
void sai_reset_and_clk_release(void)
{
#if defined(CHIP_F6721B)
    //release the sai_a reset
    HW32_REG(0x40000074) |= (1 << 24);

    //release the sai_b reset
    HW32_REG(0x40000074) |= (1 << 25);

    //release the sai_a clock
    HW32_REG(0x40000084) |= (1 << 24);

    //release the sai_b clock
    HW32_REG(0x40000084) |= (1 << 25);

    HW32_REG(0x40000198) |= (1 << 0); //D_PLLSAI_EN

    HW32_REG(0x40000190) |= (1 << 0); //D_PLLI2S_EN
#endif
}
typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;
void saiRecorderCallback(struct __SAI_HandleTypeDef *phsai, uint8_t event, unsigned long param)
{
	uint32_t i =0;
    if (event == SAI_DMAMST_RECEIVE_EVENT)
    {
        s_slvReceCmplt += 1 ;
        // printf("first s_mstReceCmplt = %d\n", s_slvReceCmplt);
	    for(i = 0;i < SAI_USERBUFFER_SIZE / 2;i ++)	
		{
			s_rdbuf[i] = s_rdbuf[i] << 3;
		}
        memcpy(&g_copybuf[pingpong_offset], &s_rdbuf[0], (SAI_USERBUFFER_SIZE / 2)*sizeof(uint16_t));
        pingpong_offset += SAI_USERBUFFER_SIZE / 2;
    }
    if (event == SAI_DMASLV_RECEIVE_EVENT)
    {
        s_slvReceCmplt += 1 ;
	    for(i = SAI_USERBUFFER_SIZE / 2;i < SAI_USERBUFFER_SIZE;i ++)	
		{
			s_rdbuf[i] = s_rdbuf[i] << 3;
		}
        //  printf("double s_mstReceCmplt = %d\n", s_slvReceCmplt);
        memcpy(&g_copybuf[pingpong_offset], &s_rdbuf[SAI_USERBUFFER_SIZE / 2], (SAI_USERBUFFER_SIZE / 2)*sizeof(uint16_t));
        pingpong_offset += SAI_USERBUFFER_SIZE / 2;
    }
}

static int test_sai_irq_and_poll(void)
{
    RET_CODE retval;
    uint32_t i = 0;
    struct i2s_config *i2s_config_a = NULL;
    struct i2s_config *i2s_config_b = NULL;
    sai_reset_and_clk_release();
    i2s_config_a->word_size = SAI_PROTOCOL_DATASIZE_32BIT;  //datasize
    i2s_config_a->timeout = TIMEOUT_WAIT_FOREVER;

    i2s_config_b->word_size = SAI_PROTOCOL_DATASIZE_32BIT;  //datasize
    i2s_config_b->timeout = TIMEOUT_WAIT_FOREVER;

    struct device *i2s_dev = device_get_binding(CONFIG_SAIA_NAME);
    if (i2s_dev == NULL)
    {
        return RET_ERROR;
    }
    i2s_configure(i2s_dev, I2S_DIR_TX, i2s_config_a);

    struct device *i2s_dev2 = device_get_binding(CONFIG_SAIB_NAME);
    if (i2s_dev2 == NULL)
    {
        return RET_ERROR;
    }
    i2s_configure(i2s_dev2, I2S_DIR_RX, i2s_config_b);

    for (i = 0; i < SAI_USERBUFFER_SIZE; i++)
    {
        wrbuf[i] = i + 0x12345678;
    }
    retval = i2s_read(i2s_dev2, &rdbuf, (unsigned int *)SAI_USERBUFFER_SIZE);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    retval = i2s_write(i2s_dev, wrbuf, SAI_USERBUFFER_SIZE);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    /* Wait till data transcation completes */
    while (s_mstTransCmplt == 0) {};
    while (s_slvTransCmplt == 0) {};
    s_slvTransCmplt = s_mstTransCmplt = 0;
    for (i = 0; i < SAI_USERBUFFER_SIZE; i++)
    {
        if (wrbuf[i] == r_rdbuf[i])
        {
            printf("reveive data %d is %d\n", i, r_rdbuf[i]);
        }
        else
        {
            return RET_ERROR;
        }
    }
    printf("test_sai_irq_and_poll ok.\n");
    return RET_OK;
}
static int test_sai_irq_and_irq(void)
{
    RET_CODE retval;
    uint32_t i = 0;
    struct i2s_config *i2s_config_a = NULL;
    struct i2s_config *i2s_config_b = NULL;
    sai_reset_and_clk_release();
    i2s_config_a->word_size = SAI_PROTOCOL_DATASIZE_32BIT;  //datasize
    i2s_config_a->timeout = TIMEOUT_WAIT_FOREVER;

    i2s_config_b->word_size = SAI_PROTOCOL_DATASIZE_32BIT;  //datasize
    i2s_config_b->timeout = TIMEOUT_WAIT_FOREVER;

    struct device *i2s_dev = device_get_binding(CONFIG_SAIA_NAME);
    if (i2s_dev == NULL)
    {
        return RET_ERROR;
    }
    i2s_configure(i2s_dev, I2S_DIR_TX, i2s_config_a);

    struct device *i2s_dev2 = device_get_binding(CONFIG_SAIB_NAME);
    if (i2s_dev2 == NULL)
    {
        return RET_ERROR;
    }
    i2s_configure(i2s_dev2, I2S_DIR_RX, i2s_config_b);

    for (i = 0; i < SAI_USERBUFFER_SIZE; i++)
    {
        wrbuf[i] = i + 0x12345678;
    }
    retval = i2s_read(i2s_dev2, &rdbuf, (unsigned int *)SAI_USERBUFFER_SIZE);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    retval = i2s_write(i2s_dev, wrbuf, SAI_USERBUFFER_SIZE);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    /* Wait till data transcation completes */
    while (s_mstTransCmplt == 0) {};
    while (s_slvTransCmplt == 0) {};
    s_slvTransCmplt = s_mstTransCmplt = 0;
    for (i = 0; i < SAI_USERBUFFER_SIZE; i++)
    {
        if (wrbuf[i] == r_rdbuf[i])
        {
            printf("reveive data %d is %d\n", i, r_rdbuf[i]);
        }
        else
        {
            return RET_ERROR;
        }
    }
    printf("test_sai_irq_and_irq ok.\n");
    return RET_OK;
}

static int test_sai_and_dma(void)
{
    RET_CODE retval;
    uint32_t i = 0;
    struct i2s_config *i2s_config_a = NULL;
    struct i2s_config *i2s_config_b = NULL;
    sai_reset_and_clk_release();
    i2s_config_a->word_size = SAI_PROTOCOL_DATASIZE_32BIT;  //datasize
    i2s_config_a->timeout = TIMEOUT_WAIT_FOREVER;

    i2s_config_b->word_size = SAI_PROTOCOL_DATASIZE_32BIT;  //datasize
    i2s_config_b->timeout = TIMEOUT_WAIT_FOREVER;

    struct device *i2s_dev = device_get_binding(CONFIG_SAIA_NAME);
    if (i2s_dev == NULL)
    {
        return RET_ERROR;
    }
    i2s_configure(i2s_dev, I2S_DIR_TX, i2s_config_a);

    struct device *i2s_dev2 = device_get_binding(CONFIG_SAIB_NAME);
    if (i2s_dev2 == NULL)
    {
        return RET_ERROR;
    }
    i2s_configure(i2s_dev2, I2S_DIR_RX, i2s_config_b);

    for (i = 0; i < SAI_USERBUFFER_SIZE; i++)
    {
        wrbuf[i] = i + 0x12345678;
    }
    retval = i2s_read(i2s_dev2, &rdbuf, (unsigned int *)SAI_USERBUFFER_SIZE);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    retval = i2s_write(i2s_dev, wrbuf, SAI_USERBUFFER_SIZE);
    if (retval != RET_OK)
    {
        return RET_ERROR;
    }

    /* Wait till data transcation completes */
    while (s_mstTransCmplt == 0) {};
    while (s_slvTransCmplt == 0) {};
    s_slvTransCmplt = s_mstTransCmplt = 0;
    for (i = 0; i < SAI_USERBUFFER_SIZE; i++)
    {
        if (wrbuf[i] == r_rdbuf[i])
        {
            printf("reveive data %d is %d\n", i, r_rdbuf[i]);
        }
        else
        {
            return RET_ERROR;
        }
    }
    printf("test_i2c_and_dma ok.\n");
    return RET_OK;
}

static int test_sai_recorder_SlvRX(void)
{
    uint8_t ret = 0;
    SAI_HandleTypeDef SAI_B_HandleTypeDef;
    SAI_InitTypeDef  SAIB_Init;
    SAI_FrameInitTypeDef  SAIB_FrameInit;
    SAI_SlotInitTypeDef   SAIB_SlotInit;
    sai_reset_and_clk_release();
    /**********sai_b_slave_rx_config **********/
    SAIB_Init.AudioMode = SAI_MODESLAVE_RX; //SLV_RX
    //SAIB_Init.AudioMode = SAI_CLOCKSTROBING_RISINGEDGE;   //CKSTR = 1
    SAIB_Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;//FTH=2
    SAIB_Init.Synchro = SAI_ASYNCHRONOUS;//asynchronous mode
    SAIB_Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    SAIB_Init.MCKNODIV = SAI_MCKNODIV_ENABLE; //MCKDIV
    SAIB_Init.MonoStereoMode = SAI_MONOMODE;//MONO mode
    SAIB_Init.CompandingMode = SAI_NOCOMPANDING;//SAI_NOCOMPANDING
    SAIB_Init.TriState = SAI_OUTPUT_NOTRELEASED; //Default value
    SAIB_Init.Protocol = SAI_I2S_STANDARD;
    SAIB_Init.DataSize = SAI_xCR1_DS_2; //16BIT
    SAIB_Init.FirstBit = SAI_FIRSTBIT_MSB;//MSB mode
    SAIB_Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;//FS

    SAIB_SlotInit.SlotNumber = SAI_xFRCR_SlotInit_SlotNumber_2;  //NBSLOT = 2-1

    SAI_B_HandleTypeDef.Init = SAIB_Init ;
    SAI_B_HandleTypeDef.FrameInit = SAIB_FrameInit ;
    SAI_B_HandleTypeDef.SlotInit = SAIB_SlotInit ;
    SAI_B_HandleTypeDef.sai_callback = saiRecorderCallback ;
    ret = HAL_SAI_InitProtocol(SAI_SLAVE_DEVICE, &SAI_B_HandleTypeDef);
    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    ret = HAL_SAI_Receive_DMA(&SAI_B_HandleTypeDef, SAI_SLAVE_MODE, s_rdbuf, SAI_USERBUFFER_SIZE);

    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    while (1)
    {
        // Record nRecordTimes seconds
        if (s_slvReceCmplt == RECORD_TIMES * 2)
        {
            break;
        }
    }
    printf("s_mstReceCmplt = %d\n", s_slvReceCmplt);
    printf("pingpong_offset = %d\n", pingpong_offset);
    s_slvReceCmplt = 0;
    HAL_DMA_Abort(SAI_B_HandleTypeDef.hDMA);
    printf("test_sai_recorder Stop\n");

    return HAL_SAI_OK;
}

static int test_sai_recorder_MstRX(void)
{
    uint8_t ret = 0;
    uint32_t j = 0;
    SAI_HandleTypeDef SAI_B_HandleTypeDef;
    SAI_InitTypeDef  SAIB_Init;
    SAI_FrameInitTypeDef  SAIB_FrameInit;
    SAI_SlotInitTypeDef   SAIB_SlotInit;
    sai_reset_and_clk_release();

    /**********sai_b_slave_rx_config **********/
    SAIB_Init.AudioMode = SAI_MODEMASTER_RX; //MST_RX
    SAIB_Init.FIFOThreshold = SAI_FIFOTHRESHOLD_3QF;//FTH=2
    //SAIB_Init.Synchro = SAI_ASYNCHRONOUS;//asynchronous mode
    SAIB_Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    SAIB_Init.FSOEn = SAI_FS_SIGNAL_OUTPUT_ENABLE;
    SAIB_Init.BCKOEn = SAI_BCK_ENABLE;
    SAIB_Init.MCKOEn = SAI_xCR1_MCKSOEN;//MCLK OUTPUT ENABLE
    SAIB_Init.MCKNODIV = SAI_MCKNODIV_DISABLE; //MCKDIV
    SAIB_Init.MonoStereoMode = SAI_MONOMODE;//MONO mode
    SAIB_Init.CompandingMode = SAI_NOCOMPANDING;//SAI_NOCOMPANDING
    SAIB_Init.TriState = SAI_OUTPUT_NOTRELEASED; //Default value
    SAIB_Init.Protocol = SAI_I2S_STANDARD;
    SAIB_Init.DataSize = SAI_xCR1_DS_2; //24BIT
    SAIB_Init.FirstBit = SAI_FIRSTBIT_MSB;//MSB mode
    SAIB_Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;//FS

    SAIB_SlotInit.SlotNumber = SAI_xFRCR_SlotInit_SlotNumber_2;  //NBSLOT = 2-1

    SAI_B_HandleTypeDef.Init = SAIB_Init ;
    SAI_B_HandleTypeDef.FrameInit = SAIB_FrameInit ;
    SAI_B_HandleTypeDef.SlotInit = SAIB_SlotInit ;
    SAI_B_HandleTypeDef.sai_callback = saiRecorderCallback ;
    ret = HAL_SAI_InitProtocol(SAI_SLAVE_DEVICE, &SAI_B_HandleTypeDef);
    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    ret = HAL_SAI_Receive_DMA(&SAI_B_HandleTypeDef, SAI_MASTER_MODE, s_rdbuf, SAI_USERBUFFER_SIZE);

    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    while (1)
    {
        // Record nRecordTimes seconds
        if (s_slvReceCmplt == RECORD_TIMES * 2)
        {
            break;
        }
    }
    printf("s_mstReceCmplt = %d\n", s_slvReceCmplt);
    printf("pingpong_offset = %d\n", pingpong_offset);
    s_slvReceCmplt = 0;
    HAL_DMA_Abort(SAI_B_HandleTypeDef.hDMA);
    for (j = 0; j < SAI_USERBUFFER_SIZE * RECORD_TIMES; j ++)
    {
        g_copybuf[j] = g_copybuf[j] << 4;
    }
    printf("test_sai_recorder Stop\n");

    return HAL_SAI_OK;
}
static int test_sai_recorder_MstRX_SAIA(void)
{
    uint8_t ret = 0;
    uint32_t j = 0;
    SAI_HandleTypeDef SAI_A_HandleTypeDef;
    SAI_InitTypeDef  SAIA_Init;
    SAI_FrameInitTypeDef  SAIA_FrameInit;
    SAI_SlotInitTypeDef   SAIA_SlotInit;
    sai_reset_and_clk_release();

    /**********sai_b_slave_rx_config **********/
    SAIA_Init.AudioMode = SAI_MODEMASTER_RX; //MST_RX
    SAIA_Init.FIFOThreshold = SAI_FIFOTHRESHOLD_3QF;//FTH=2
    //SAIB_Init.Synchro = SAI_ASYNCHRONOUS;//asynchronous mode
    SAIA_Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    SAIA_Init.FSOEn = SAI_FS_SIGNAL_OUTPUT_ENABLE;
    SAIA_Init.BCKOEn = SAI_BCK_ENABLE;
    //SAIA_Init.MCKOEn = SAI_xCR1_MCKSOEN;//MCLK OUTPUT ENABLE
    SAIA_Init.MCKNODIV = SAI_MCKNODIV_DISABLE; //MCKDIV
    SAIA_Init.MonoStereoMode = SAI_MONOMODE;//MONO mode
    SAIA_Init.CompandingMode = SAI_NOCOMPANDING;//SAI_NOCOMPANDING
    SAIA_Init.TriState = SAI_OUTPUT_NOTRELEASED; //Default value
    SAIA_Init.Protocol = SAI_I2S_STANDARD;
    SAIA_Init.DataSize = SAI_xCR1_DS_2; //24BIT
    SAIA_Init.FirstBit = SAI_FIRSTBIT_MSB;//MSB mode
    SAIA_Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;//FS

    SAIA_SlotInit.SlotNumber = SAI_xFRCR_SlotInit_SlotNumber_2;  //NBSLOT = 2-1

    SAI_A_HandleTypeDef.Init = SAIA_Init ;
    SAI_A_HandleTypeDef.FrameInit = SAIA_FrameInit ;
    SAI_A_HandleTypeDef.SlotInit = SAIA_SlotInit ;
    SAI_A_HandleTypeDef.sai_callback = saiRecorderCallback ;
    ret = HAL_SAI_InitProtocol(SAI_MASTER_DEVICE, &SAI_A_HandleTypeDef);
    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    ret = HAL_SAI_Receive_DMA(&SAI_A_HandleTypeDef, SAI_MASTER_MODE, s_rdbuf, SAI_USERBUFFER_SIZE);

    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    while (1)
    {
        // Record nRecordTimes seconds
        if (s_slvReceCmplt == RECORD_TIMES * 2)
        {
            break;
        }
    }
    printf("s_mstReceCmplt = %d\n", s_slvReceCmplt);
    printf("pingpong_offset = %d\n", pingpong_offset);
    s_slvReceCmplt = 0;
    HAL_DMA_Abort(SAI_A_HandleTypeDef.hDMA);
    for (j = 0; j < SAI_USERBUFFER_SIZE * RECORD_TIMES; j ++)
    {
        g_copybuf[j] = g_copybuf[j] << 3;
    }
    printf("test_sai_recorder Stop\n");

    return HAL_SAI_OK;
}
int saib_pinmux_config(void)
{
    int ret = 0;
    struct device *portj = device_get_binding(CONFIG_PINMUX_GM_PORTJ_NAME);
    if (portj == NULL)
    {
        return -1;
    }

    GPIO_PinConfig_T pin_cfg;
    Device_Pinmux_T s_saiPinMux[4] =
    {
        {PINMUX_GPIO_PIN_5, GPIOJ, GPIOJ5_AF3_SAI_SD_B},
        {PINMUX_GPIO_PIN_6, GPIOJ, GPIOJ6_AF3_SAI_MCLK_B},
        {PINMUX_GPIO_PIN_7, GPIOJ, GPIOJ7_AF3_SAI_BCK_B},
        {PINMUX_GPIO_PIN_8, GPIOJ, GPIOJ8_AF3_SAI_FS_B},
    };

    pin_cfg.pin = BIT(s_saiPinMux[0].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[0].alternate;

    ret = pinmux_pin_set(portj, s_saiPinMux[0].pin, (unsigned int)&pin_cfg);

    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_saiPinMux[1].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[1].alternate;

    ret = pinmux_pin_set(portj, s_saiPinMux[1].pin, (unsigned int)&pin_cfg);

    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_saiPinMux[2].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[2].alternate;

    ret = pinmux_pin_set(portj, s_saiPinMux[2].pin, (unsigned int)&pin_cfg);

    if (ret)
    {
        return ret;
    }

    pin_cfg.pin = BIT(s_saiPinMux[3].pin);
    pin_cfg.mode = GPIO_MODE_AF;
    pin_cfg.pull = GPIO_PULLUP;
    pin_cfg.alternate = s_saiPinMux[3].alternate;

    ret = pinmux_pin_set(portj, s_saiPinMux[3].pin, (unsigned int)&pin_cfg);

    if (ret)
    {
        return ret;
    }

    return ret;
}

static int test_sai_recorder_MstRX_SAIB(void)
{
    uint8_t ret = 0;
    uint32_t j = 0;
    SAI_HandleTypeDef SAI_B_HandleTypeDef;
    SAI_InitTypeDef  SAIB_Init;
    SAI_FrameInitTypeDef  SAIB_FrameInit;
    SAI_SlotInitTypeDef   SAIB_SlotInit;
    sai_reset_and_clk_release();
    saib_pinmux_config();
    /**********sai_b_slave_rx_config **********/
    SAIB_Init.AudioMode = SAI_MODEMASTER_RX; //MST_RX
    SAIB_Init.FIFOThreshold = SAI_FIFOTHRESHOLD_3QF;//FTH=2
    //SAIB_Init.Synchro = SAI_ASYNCHRONOUS;//asynchronous mode
    SAIB_Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    SAIB_Init.FSOEn = SAI_FS_SIGNAL_OUTPUT_ENABLE;
    SAIB_Init.BCKOEn = SAI_BCK_ENABLE;
    //SAIB_Init.MCKOEn = SAI_xCR1_MCKSOEN;//MCLK OUTPUT ENABLE
    SAIB_Init.MCKNODIV = SAI_MCKNODIV_DISABLE; //MCKDIV
    SAIB_Init.MonoStereoMode = SAI_MONOMODE;//MONO mode
    SAIB_Init.CompandingMode = SAI_NOCOMPANDING;//SAI_NOCOMPANDING
    SAIB_Init.TriState = SAI_OUTPUT_NOTRELEASED; //Default value
    SAIB_Init.Protocol = SAI_I2S_STANDARD;
    SAIB_Init.DataSize = SAI_xCR1_DS_2; //24BIT
    SAIB_Init.FirstBit = SAI_FIRSTBIT_MSB;//MSB mode
    SAIB_Init.AudioFrequency = SAI_AUDIO_FREQUENCY_16K;//FS

    SAIB_SlotInit.SlotNumber = SAI_xFRCR_SlotInit_SlotNumber_2;  //NBSLOT = 2-1

    SAI_B_HandleTypeDef.Init = SAIB_Init ;
    SAI_B_HandleTypeDef.FrameInit = SAIB_FrameInit ;
    SAI_B_HandleTypeDef.SlotInit = SAIB_SlotInit ;
    SAI_B_HandleTypeDef.sai_callback = saiRecorderCallback ;
    ret = HAL_SAI_InitProtocol(SAI_MASTER_DEVICE, &SAI_B_HandleTypeDef);
    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    ret = HAL_SAI_Receive_DMA(&SAI_B_HandleTypeDef, SAI_MASTER_MODE, s_rdbuf, SAI_USERBUFFER_SIZE);

    if (ret != HAL_SAI_OK)
    {
        return HAL_SAI_ERROR;
    }
    while (1)
    {
        // Record nRecordTimes seconds
        if (s_slvReceCmplt == RECORD_TIMES * 2)
        {
            break;
        }
    }
    printf("s_mstReceCmplt = %d\n", s_slvReceCmplt);
    printf("pingpong_offset = %d\n", pingpong_offset);
    s_slvReceCmplt = 0;
    HAL_DMA_Abort(SAI_B_HandleTypeDef.hDMA);
    for (j = 0; j < SAI_USERBUFFER_SIZE * RECORD_TIMES; j ++)
    {
        g_copybuf[j] = g_copybuf[j] << 3;
    }
    printf("test_sai_recorder Stop\n");
    return HAL_SAI_OK;
}

void main(void)
{
    printf("test sai printk.\n");
    test_sai_irq_and_poll();
    //test_sai_irq_and_irq();
    //test_sai_and_dma();
    //test_sai_recorder_SlvRX();
    //test_sai_recorder_MstRX_SAIA();
    //test_sai_recorder_MstRX_SAIB();
    while (1);
}

