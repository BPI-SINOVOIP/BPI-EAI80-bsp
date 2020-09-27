float sqrt(float x)
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

float InvSqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}

double exp(double x)
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

double power(double x, int n);
double pow(double x, int n)
{
    if (n < 0)
    {
        return 1.0 / power(x, -n);
    }
    else
    {
        return power(x, n);
    }
}
double power(double x, int n)
{
    if (n == 0)
    {
        return 1;
    }
    double result = 0;
    double temp = pow(x, n / 2);
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


