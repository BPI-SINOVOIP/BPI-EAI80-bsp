/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ugelis.h>
#include <misc/printk.h>

float testAdd(float a, float b)
{
    return a + b;
}

float testSub(float a, float b)
{
    return a - b;
}


float testMux(float a, float b)
{
    return a * b;
}


float testDiv(float a, float b)
{
    return a / b;
}


int main(void)
{

    float result = 0.33;
    float a = 8.85335;
    float b = 2.33344;

    result = testAdd(a, b);
    printf("a = %6.5f, b = %6.5f a+b=%f\n", a, b, result);
    result = testSub(a, b);
    printf("a = %f, b = %f a-b=%f\n", a, b, result);
    result = testMux(a, b);
    printf("a = %f, b = %f a*b=%f\n", a, b, result);
    result = testDiv(a, b);
    printf("a = %f, b = %f a/b=%f\n", a, b, result);




    return 0;
}
