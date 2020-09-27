/**
 *
 * Copyright (C) 2020 Gree Edgeless Microelectronics.  All Rights Reserved.
 *
 * @file          math.c
 *
 * @author        Angrad.Yang
 *
 * @version       1.0.0
 *
 * @date          2020/01/06
 *
 * @brief         Math interfaces with high efficiency
 *
 * @note
 *    2020/01/06, Angrad.Yang, V1.0.0
 *        Initial version.
 */

float sqrt_ex(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x; // get bits for floating VALUE
    i = 0x5f375a86 - (i >> 1); // gives initial guess y0
    x = *(float *)&i; // convert bits BACK to float
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy

    return 1 / x;
}

float InvSqrt_ex(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}

double exp_ex(double x)
{
    x = 1.0 + x / 256.0;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    return x;
}

double power_ex(double x, int n);
double pow_ex(double x, int n)
{
    if (n < 0)
    {
        return 1.0 / power_ex(x, -n);
    }
    else
    {
        return power_ex(x, n);
    }
}
double power_ex(double x, int n)
{
    if (n == 0)
    {
        return 1;
    }
    double result = 0;
    double temp = pow_ex(x, n / 2);
    if (n % 2 == 1)
    {
        result = x * temp * temp;
    }
    else
    {
        result = temp * temp;
    }
    return result;
}
