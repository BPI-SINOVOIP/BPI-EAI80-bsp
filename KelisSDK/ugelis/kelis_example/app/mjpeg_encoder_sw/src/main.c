
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <board.h>
#include <gpio.h>
#include <stdlib.h>
#include <math.h>

#include <image.h>

extern int demo(void);
int main(void)
{
    RET_CODE ret = RET_OK;

    printf("jpeg demo begin\n");
    demo();
    printf("jpeg demo exit\n");

    return ret;
}
