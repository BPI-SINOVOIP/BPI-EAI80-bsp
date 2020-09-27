/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Fast approximate math functions.
 *
 */
#include "fmath.h"

const float __atanf_lut[4] =
{
    -0.0443265554792128f,    //p7
        -0.3258083974640975f,    //p3
        +0.1555786518463281f,    //p5
        +0.9997878412794807f     //p1
    };

#ifndef __USE_KEIL_

float inline fast_sqrtf(float x)
{

    __asm__ volatile(
                    "vsqrt.f32  %[r], %[x]\n"
                    : [r] "=t"(x)
                    : [x] "t"(x));

    return x;
}

int inline fast_floorf(float x)
{
    int i;

    __asm__ volatile(
                    "vcvt.S32.f32  %[r], %[x]\n"
                    : [r] "=t"(i)
                    : [x] "t"(x));

    return i;
}

int inline fast_ceilf(float x)
{
    int i;
    x += 0.9999f;

    __asm__ volatile(
                    "vcvt.S32.f32  %[r], %[x]\n"
                    : [r] "=t"(i)
                    : [x] "t"(x));

    return i;
}

int inline fast_roundf(float x)
{
    int i;

    __asm__ volatile(
                    "vcvtr.s32.f32  %[r], %[x]\n"
                    : [r] "=t"(i)
                    : [x] "t"(x));

    return i;
}
#else
float asm inline fast_sqrtf_in(float x)
{
    VSQRT.F32 S2, S0
    VMOV.F32  S0, S2
    BX LR
}

float  inline fast_sqrtf(float x)
{
    return fast_sqrtf_in(x);
}

int asm inline fast_floorf_in(float x)
{
    VCVT.S32.F32 S2, S0
    VMOV  R0, S2
    BX LR
}

int inline fast_floorf(float x)
{
    return fast_floorf_in(x);
}

int inline fast_ceilf(float x)
{
    return fast_floorf(x + 0.9999f);
}

int asm inline fast_roundf_in(float x)
{
    VCVTR.S32.F32 S2, S0
    VMOV  R0, S2
    BX LR
}

int inline fast_roundf(float x)
{
    return fast_roundf_in(x);
}
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
typedef union
{
    uint32_t l;
    struct
    {
        uint32_t m : 20;
        uint32_t e : 11;
        uint32_t s : 1;
    };
} exp_t;

float fast_expf(float x)
{
    exp_t e;
    e.l = (uint32_t)(1512775 * x + 1072632447);
    // IEEE binary32 format
    e.e = (e.e - 1023 + 127) & 0xFF; // rebase

    uint32_t packed = (e.s << 31) | (e.e << 23) | e.m << 3;
    return *((float *)&packed);
}
#pragma GCC diagnostic pop

/*
 * From Hackers Delight:
 * This is a very approximate but very fast version of acbrt. It is just eight
 * integer instructions (shift rights and adds), plus instructions to load the constant.
 * 1/3 is approximated as 1/4 + 1/16 + 1/64 + 1/256 + ... + 1/65536.
 * The constant 0x2a511cd0 balances the relative error at +-0.0321.
 */
float fast_cbrtf(float x)
{
    union
    {
        int ix;
        float x;
    } v;
    v.x = x;                // x can be viewed as int.
    v.ix = v.ix / 4 + v.ix / 16; // Approximate divide by 3.
    v.ix = v.ix + v.ix / 16;
    v.ix = v.ix + v.ix / 256;
    v.ix = 0x2a511cd0 + v.ix;  // Initial guess.
    return v.x;
}

float inline fast_fabsf(float x)
{
#ifdef __USE_KEIL_
#pragma asm  volatile ("vabs.f32  %[r], %[x]\n" : [r] "=t" (x) : [x] "t"  (x));
#pragma endasm
#else
    __asm__ volatile(
                    "vabs.f32  %[r], %[x]\n"
                    : [r] "=t"(x)
                    : [x] "t"(x));
#endif
    return x;
}

inline float fast_atanf(float xx)
{
    float x, y, z;
    int sign;

    x = xx;

    /* make argument positive and save the sign */
    if (xx < 0.0f)
    {
        sign = -1;
        x = -xx;
    }
    else
    {
        sign = 1;
        x = xx;
    }
    /* range reduction */
    if (x > 2.414213562373095f)   /* tan 3pi/8 */
    {
        y = M_PI_2;
        x = -(1.0f / x);
    }

    else if (x > 0.4142135623730950f)  /* tan pi/8 */
    {
        y = M_PI_4;
        x = (x - 1.0f) / (x + 1.0f);
    }
    else
    {
        y = 0.0f;
    }

    z = x * x;
    y +=
                    (((8.05374449538e-2f  * z
                       - 1.38776856032E-1f) * z
                      + 1.99777106478E-1f) * z
                     - 3.33329491539E-1f) * z * x + x;

    if (sign < 0)
    {
        y = -y;
    }

    return (y);
}

float fast_atan2f(float y, float x)
{
    if (x > 0 && y >= 0)
    {
        return fast_atanf(y / x);
    }

    if (x < 0 && y >= 0)
    {
        return M_PI - fast_atanf(-y / x);
    }

    if (x < 0 && y < 0)
    {
        return M_PI + fast_atanf(y / x);
    }

    if (x > 0 && y < 0)
    {
        return 2 * M_PI - fast_atanf(-y / x);
    }

    return 0;
}

float fast_log2(float x)
{
    union
    {
        float f;
        uint32_t i;
    } vx = { x };
    union
    {
        uint32_t i;
        float f;
    } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
    float y = vx.i;
    y *= 1.1920928955078125e-7f;

    return y - 124.22551499f - 1.498030302f * mx.f
           - 1.72587999f / (0.3520887068f + mx.f);
}

float fast_log(float x)
{
    return 0.69314718f * fast_log2(x);
}

