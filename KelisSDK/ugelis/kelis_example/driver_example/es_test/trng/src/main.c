#include <ugelis.h>
#include <misc/util.h>
#include <misc/printk.h>
#include <gm_hal_trng.h>

#define TEST_CNT         100
#define RANDOM_SIZE      6      // 192bit, 6 DWORD

static TRNG_Handle_T hTrng;
static uint8_t data_ready = FALSE;
static uint8_t error_flag = FALSE;
static uint8_t test_finish = FALSE;
static uint32_t random_polling[RANDOM_SIZE * TEST_CNT];
static uint32_t random_interrupt[RANDOM_SIZE * TEST_CNT];


static void dump_hexdata(uint32_t *data, uint32_t len)
{
    uint32_t i = 0;

    for (i = 0; i < len; i++)
    {
        printk("0x%x ", *data++);
        if ((i > 0) && ((i % 6) == 0))
        {
            printk("\n");
        }
    }
    printk("\n");
}


void trng_callback(TRNG_Handle_T *hTrng, TRNG_Event_T event, unsigned long param)
{
    static uint32_t i = 0;
    switch (event)
    {
        case TRNG_EVENT_DATA_VALID:
            HAL_TRNG_ReadLastRandomNumber(hTrng, &random_interrupt[i], RANDOM_SIZE);
            dump_hexdata(&random_interrupt[i], RANDOM_SIZE);
            i += RANDOM_SIZE;
            if (i >= (TEST_CNT * RANDOM_SIZE))
            {
                test_finish = TRUE;
            }
            data_ready = TRUE;
            break;
        case TRNG_EVENT_ERROR:
            if (param == TRNG_ERR_AUTOCORR)
            {
                printk(" Autocorrelation erro\n");
            }
            else if (param == TRNG_ERR_CRNGT)
            {
                printk("CRNGT error\n");
            }
            else if (param == TRNG_ERR_VN)
            {
                printk("Von Neumann error\n");
            }
            else
            {
                printk("unknow error.\n");
            }
            error_flag = TRUE;
            break;
        default:
            printk("not support event: %d.\n", event);
            break;
    }
}

int main(void)
{
    uint32_t i = 0;
    HAL_Status_T ret = HAL_ERROR;

    /* use polling mode to get random number*/
    printk("start to test TRNG with polling mode:\n");
    memset(&hTrng, 0, sizeof(hTrng));
    HAL_TRNG_Init(&hTrng);
    for (i = 0; i < TEST_CNT; i++)
    {
        //memset(random_polling, 0, sizeof(random_polling));
        ret = HAL_TRNG_GenerateRandomNumber(&hTrng, &random_polling[i * RANDOM_SIZE], RANDOM_SIZE);
        if (ret == HAL_OK)
        {
            dump_hexdata(&random_polling[i * RANDOM_SIZE], RANDOM_SIZE);
        }
        else
        {
            printk("Trng generate data with polling mode fail, i = %d.\n", i);
            goto EXIT;
        }
    }
    HAL_TRNG_DeInit(&hTrng);

    /* use interrupt mode to get random number */
    printk("start to test TRNG with interrupt mode:\n");
    memset(&hTrng, 0, sizeof(hTrng));
    hTrng.priority = 0;
    hTrng.callback = trng_callback;
    HAL_TRNG_Init(&hTrng);
    while (1)
    {
        if (test_finish == TRUE)
        {
            break;
        }
        data_ready = FALSE;
        error_flag = FALSE;
        HAL_TRNG_GenerateRandomNumber_IT(&hTrng);
        while (1)
        {
            if (data_ready == TRUE)
            {
                break;
            }
            if (error_flag == TRUE)
            {
                goto EXIT;
            }
        }
    }

    HAL_TRNG_DeInit(&hTrng);

    printk("TRNG test done.\n");
    return 0;
EXIT:
    HAL_TRNG_DeInit(&hTrng);
    printk("TRNG test error.\n");
    return -1;

}
