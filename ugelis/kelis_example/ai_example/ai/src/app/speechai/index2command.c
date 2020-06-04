#include "base_types.h"
#include "keyword.hpp"
#include "index2command.h"

int GetTriggerCount(void)
{
    return sizeof(k_lpszTrigger) / sizeof(k_lpszTrigger[0]);
}

int GetCommandCount(void)
{
    return sizeof(k_lpszCommand) / sizeof(k_lpszCommand[0]);
}

const char *GetTriggerStringByIndex(int nIndex)
{
    if (nIndex == 0 && nIndex < sizeof(k_lpszTrigger) / sizeof(k_lpszTrigger[0]))
    {
        return k_lpszTrigger[nIndex];
    }
    else
    {
        return NULL;
    }
}

const char *GetCommandStringByIndex(int nIndex)
{
    if (nIndex >= 0 && nIndex < sizeof(k_lpszCommand) / sizeof(k_lpszCommand[0]))
    {
        return k_lpszCommand[nIndex];
    }
    else
    {
        return NULL;
    }
}

char *GetTriggerText(int index)
{
    return (char *)k_lpszTrigger[index];
}

char *GetCommandText(int index)
{
    return (char *)k_lpszCommand[index];
}

char *GetNoticeText(int index)
{
    return (char *)k_lpszNotice[index];
}

