#include <gm_common.h>
#ifdef RABBIT_NO_OS_SYSTEM
    #include <systick.h>
#endif
/**
 * resver:add some common function later
 */

void HAL_LockInit(lock_t *pLock)
{
    HAL_IrqLock();

    /*
    * pLock : when write access must be atmoic to
    *         stop from pLock competition,read
    *         it will not care competition.
    */
    *pLock = HAL_UNLOCK;

    HAL_IrqUnlock();
}

RET_CODE HAL_TryLock(lock_t *pLock)
{
    RET_CODE ret = RET_OK;

    HAL_IrqLock();

    /*
    * pLock : when write access must be atmoic to
    *         stop from pLock competition,read
    *         it will not care competition.
    */
    if (*pLock == HAL_LOCKED)
    {
        ret =  RET_BUSY;
        goto out;
    }
    else
    {
        *pLock = HAL_LOCKED;
    }

out:
    HAL_IrqUnlock();

    return ret;
}


void HAL_UnLock(lock_t *pLock)
{
    HAL_IrqLock();

    /*
    * pLock : when write access must be atmoic to
    *         stop from pLock competition,read
    *         it will not care competition.
    */
    *pLock = HAL_UNLOCK;

    HAL_IrqUnlock();
}

#ifdef RABBIT_NO_OS_SYSTEM
/*need to impliment according to different soft platform*/
// __weak uint32_t HAL_GetTick(void)
// {
//#ifdef RABBIT_NO_OS_SYSTEM
//     return SysTick_Get();
//#else
//     return 0;
//#endif
// }

// __weak void HAL_DelayMs(uint32_t ms)
// {
//#ifdef RABBIT_NO_OS_SYSTEM
//   SysTick_DelayMS(ms);
//#endif
// }

// __weak void HAL_DelayUs(uint32_t us)
// {
//#ifdef RABBIT_NO_OS_SYSTEM
//   SysTick_DelayUS(us);
//#endif
// }

__weak void HAL_IrqLock(void)
{

}

__weak void HAL_IrqUnlock(void)
{

}

__weak void HAL_DisableIrq(void)
{

}

__weak void HAL_EnableIrq(void)
{

}
#else
/*need to impliment according to different soft platform*/
__WEAK uint32_t HAL_GetTick(void)
{
    return 0;
}

__WEAK void HAL_DelayMs(uint32_t ms)
{

}

__WEAK void HAL_DelayUs(uint32_t us)
{

}

__WEAK void HAL_IrqLock(void)
{

}

__WEAK void HAL_IrqUnlock(void)
{

}

__WEAK void HAL_DisableIrq(void)
{

}

__WEAK void HAL_EnableIrq(void)
{

}
#endif


#ifdef RABBIT_NO_OS_SYSTEM
#ifdef DEBUG
void assert_failed(uint8_t *file, uint32_t line)
{
#if ASSERT_WITH_PRINTF
    printf("ASSERT ERROR: file \"%s\" Line \"%d\" \n", file, line);
    /* Infinite loop */
    while (1)
    {}
#else
    __asm("bkpt #0");
#endif
}
#endif /* DEBUG */

void *memcpy(void *dest, const void *src, unsigned n)
{
    int longs, bytes;
    uint32_t *dpl = (uint32_t *)dest;
    uint32_t *spl = (uint32_t *)src;
    uint8_t  *dpb, *spb;

    if ((dest != NULL) && (src != NULL) && (n > 0))
    {
        bytes = (n & 0x3);
        longs = (n - bytes) >> 2;

        while (longs--)
        {
            *dpl++ = *spl++;
        }

        dpb = (uint8_t *)dpl;
        spb = (uint8_t *)spl;

        while (bytes--)
        {
            *dpb++ = *spb++;
        }
    }
    return dest;
}

#ifndef USE_EXTERNAL_CLIB
void *memset(void *s, int c, unsigned n)
{
    unsigned char *sp = (unsigned char *)s;

    if ((s != NULL) && (n > 0))
    {
        while (n--)
        {
            *sp++ = (unsigned char)c;
        }
    }
    return s;
}
#endif
#endif

