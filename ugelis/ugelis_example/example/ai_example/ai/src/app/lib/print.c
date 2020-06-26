/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          print.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Interface for convenient print
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */
#include <stdio.h>

void print_float(float array[], int maxlen)
{
    int i;

    for (i = 0; i < maxlen; i++)
    {
        printf("%.2f ", array[i]);
    }
    printf("\n");

    return ;
}

void print_int(int array[], int maxlen)
{
    int i;

    for (i = 0; i < maxlen; i++)
    {
        printf("%4d ", array[i]);
    }
    printf("\n");

    return ;
}
