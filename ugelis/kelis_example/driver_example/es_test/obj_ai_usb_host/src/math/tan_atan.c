#define __HI(x) *(1 + (int *) &x)
#define __LO(x) *(int *) &x

#define zero    0.00000000000000000000e+00 /* 0x00000000, 0x00000000 */
#define half    5.00000000000000000000e-01 /* 0x3FE00000, 0x00000000 */
#define one     1.00000000000000000000e+00 /* 0x3FF00000, 0x00000000 */
#define two24   1.67772160000000000000e+07 /* 0x41700000, 0x00000000 */
#define twon24  5.96046447753906250000e-08 /* 0x3E700000, 0x00000000 */


#define T0      3.33333333333334091986e-01 /* 3FD55555, 55555563 */
#define T1      1.33333333333201242699e-01 /* 3FC11111, 1110FE7A */
#define T2      5.39682539762260521377e-02 /* 3FABA1BA, 1BB341FE */
#define T3      2.18694882948595424599e-02 /* 3F9664F4, 8406D637 */
#define T4      8.86323982359930005737e-03 /* 3F8226E3, E96E8493 */
#define T5      3.59207910759131235356e-03 /* 3F6D6D22, C9560328 */
#define T6      1.45620945432529025516e-03 /* 3F57DBC8, FEE08315 */
#define T7      5.88041240820264096874e-04 /* 3F4344D8, F2F26501 */
#define T8      2.46463134818469906812e-04 /* 3F3026F7, 1A8D1068 */
#define T9      7.81794442939557092300e-05 /* 3F147E88, A03792A6 */
#define T10     7.14072491382608190305e-05 /* 3F12B80F, 32F0A7E9 */
#define T11    -1.85586374855275456654e-05 /* BEF375CB, DB605373 */
#define T12     2.59073051863633712884e-05 /* 3EFB2A70, 74BF7AD4 */
#define pio4    7.85398163397448278999e-01 /* 3FE921FB, 54442D18 */
#define pio4lo  3.06161699786838301793e-17 /* 3C81A626, 33145C07 */

static const int npio2_hw[] =
{
    0x3FF921FB, 0x400921FB, 0x4012D97C, 0x401921FB, 0x401F6A7A, 0x4022D97C,
    0x4025FDBB, 0x402921FB, 0x402C463A, 0x402F6A7A, 0x4031475C, 0x4032D97C,
    0x40346B9C, 0x4035FDBB, 0x40378FDB, 0x403921FB, 0x403AB41B, 0x403C463A,
    0x403DD85A, 0x403F6A7A, 0x40407E4C, 0x4041475C, 0x4042106C, 0x4042D97C,
    0x4043A28C, 0x40446B9C, 0x404534AC, 0x4045FDBB, 0x4046C6CB, 0x40478FDB,
    0x404858EB, 0x404921FB,
};

double fabs(double x)
{
    __HI(x) &= 0x7fffffff;
    return x;
} /* fabs */

double copysign(double x, double y)
{
    __HI(x) = (__HI(x) & 0x7fffffff) | (__HI(y) & 0x80000000);
    return x;
} /* copysign */


#define two54  1.80143985094819840000e+16 /* 0x43500000, 0x00000000 */
#define twom54 5.55111512312578270212e-17 /* 0x3C900000, 0x00000000 */
#define huge   1.0e+300
#define tiny   1.0e-300

double scalbn(double x, int n)
{
    int k, hx, lx;

    hx = __HI(x);
    lx = __LO(x);
    k = (hx & 0x7ff00000) >> 20; /* extract exponent */
    if (k == 0) /* 0 or subnormal x */
    {
        if ((lx | (hx & 0x7fffffff)) == 0) /* +-0 */
        {
            return x;
        }
        x *= two54;
        hx = __HI(x);
        k = ((hx & 0x7ff00000) >> 20) - 54;
        if (n < -50000) /*underflow */
        {
            return tiny * x;
        }
    }
    if (k == 0x7ff) /* NaN or Inf */
    {
        return x + x;
    }
    k = k + n;
    if (k > 0x7fe) /* overflow  */
    {
        return huge * copysign(huge, x);
    }
    if (k > 0) /* normal result */
    {
        __HI(x) = (hx & 0x800fffff) | (k << 20);
        return x;
    }
    if (k <= -54)
    {
        if (n > 50000) /* in case integer overflow in n + k */
        {
            return huge * copysign(huge, x);  /*overflow */
        }
        else
        {
            return tiny * copysign(tiny, x);  /*underflow */
        }
    }
    k += 54; /* subnormal result */
    __HI(x) = (hx & 0x800fffff) | (k << 20);
    return x * twom54;
} /* scalbn */

#define huge 1.0e300

double floor(double x)
{
    int i0, i1, j0;
    unsigned i, j;

    i0 = __HI(x);
    i1 = __LO(x);
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    if (j0 < 20)
    {
        if (j0 < 0) /* raise inexact if x != 0 */
        {
            if (huge + x > 0.0) /* return 0 * sign(x) if |x| < 1 */
            {
                if (i0 >= 0)
                {
                    i0 = i1 = 0;
                }
                else if (((i0 & 0x7fffffff) | i1) != 0)
                {
                    i0 = 0xbff00000;
                    i1 = 0;
                }
            }
        }
        else
        {
            i = (0x000fffff) >> j0;
            if (((i0 & i) | i1) == 0) /* x is integral */
            {
                return x;
            }
            if (huge + x > 0.0) /* raise inexact flag */
            {
                if (i0 < 0)
                {
                    i0 += (0x00100000) >> j0;
                }
                i0 &= (~i);
                i1 = 0;
            }
        }
    }
    else if (j0 > 51)
    {
        if (j0 == 0x400) /* inf or NaN */
        {
            return x + x;
        }
        else /* x is integral */
        {
            return x;
        }
    }
    else
    {
        i = ((unsigned)(0xffffffff)) >> (j0 - 20);
        if ((i1 & i) == 0) /* x is integral */
        {
            return x;
        }
        if (huge + x > 0.0) /* raise inexact flag */
        {
            if (i0 < 0)
            {
                if (j0 == 20)
                {
                    i0 += 1;
                }
                else
                {
                    j = i1 + (1 << (52 - j0));
                    if (j < i1) /* got a carry */
                    {
                        i0 += 1;
                    }
                    i1 = j;
                }
            }
            i1 &= (~i);
        }
    }
    __HI(x) = i0;
    __LO(x) = i1;
    return x;
} /* floor */



/* initial value for jk */
static const int init_jk[] =
{
    2, 3, 4, 6
};

static const double PIo2[] =
{
    1.57079625129699707031e+00, /* 0x3FF921FB, 0x40000000 */
    7.54978941586159635335e-08, /* 0x3E74442D, 0x00000000 */
    5.39030252995776476554e-15, /* 0x3CF84698, 0x80000000 */
    3.28200341580791294123e-22, /* 0x3B78CC51, 0x60000000 */
    1.27065575308067607349e-29, /* 0x39F01B83, 0x80000000 */
    1.22933308981111328932e-36, /* 0x387A2520, 0x40000000 */
    2.73370053816464559624e-44, /* 0x36E38222, 0x80000000 */
    2.16741683877804819444e-51, /* 0x3569F31D, 0x00000000 */
};

/*
 *  * Table of constants for 2/pi, 396 Hex digits (476 decimal) of 2/pi
 *   */
static const int ipio2[] =
{
    0xA2F983, 0x6E4E44, 0x1529FC, 0x2757D1, 0xF534DD, 0xC0DB62,
    0x95993C, 0x439041, 0xFE5163, 0xABDEBB, 0xC561B7, 0x246E3A,
    0x424DD2, 0xE00649, 0x2EEA09, 0xD1921C, 0xFE1DEB, 0x1CB129,
    0xA73EE8, 0x8235F5, 0x2EBB44, 0x84E99C, 0x7026B4, 0x5F7E41,
    0x3991D6, 0x398353, 0x39F49C, 0x845F8B, 0xBDF928, 0x3B1FF8,
    0x97FFDE, 0x05980F, 0xEF2F11, 0x8B5A0A, 0x6D1F6D, 0x367ECF,
    0x27CB09, 0xB74F46, 0x3F669E, 0x5FEA2D, 0x7527BA, 0xC7EBE5,
    0xF17B3D, 0x0739F7, 0x8A5292, 0xEA6BFB, 0x5FB11F, 0x8D5D08,
    0x560330, 0x46FC7B, 0x6BABF0, 0xCFBC20, 0x9AF436, 0x1DA9E3,
    0x91615E, 0xE61B08, 0x659985, 0x5F14A0, 0x68408D, 0xFFD880,
    0x4D7327, 0x310606, 0x1556CA, 0x73A8C9, 0x60E27B, 0xC08C6B,
};

static int __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec)
{
    int jz, jx, jv, jp, jk, carry, n, iq[20], i, j, k, m, q0, ih;
    double z, fw, f[20], fq[20], q[20];

    /* initialize jk */
    jk = init_jk[prec];
    jp = jk;

    /* determine jx, jv, q0, note that 3 > q0 */
    jx = nx - 1;
    jv = (e0 - 3) / 24;
    if (jv < 0)
    {
        jv = 0;
    }
    q0 = e0 - 24 * (jv + 1);

    /* set up f[0] to f[jx + jk] where f[jx + jk] = ipio2[jv + jk] */
    j = jv - jx;
    m = jx + jk;
    for (i = 0; i <= m; i++, j++)
    {
        f[i] = (j < 0) ? zero : (double) ipio2[j];
    }

    /* compute q[0], q[1], ... q[jk] */
    for (i = 0; i <= jk; i++)
    {
        for (j = 0, fw = 0.0; j <= jx; j++)
        {
            fw += x[j] * f[jx + i - j];
        }
        q[i] = fw;
    }

    jz = jk;
recompute:
    /* distill q[] into iq[] reversingly */
    for (i = 0, j = jz, z = q[jz]; j > 0; i++, j--)
    {
        fw = (double)((int)(twon24 * z));
        iq[i] = (int)(z - two24 * fw);
        z = q[j - 1] + fw;
    }

    /* compute n */
    z = scalbn(z, q0);  /* actual value of z */
    z -= 8.0 * floor(z * 0.125);  /* trim off integer >= 8 */
    n = (int) z;
    z -= (double) n;
    ih = 0;
    if (q0 > 0) /* need iq[jz - 1] to determine n */
    {
        i = (iq[jz - 1] >> (24 - q0));
        n += i;
        iq[jz - 1] -= i << (24 - q0);
        ih = iq[jz - 1] >> (23 - q0);
    }
    else if (q0 == 0)
    {
        ih = iq[jz - 1] >> 23;
    }
    else if (z >= 0.5)
    {
        ih = 2;
    }

    if (ih > 0) /* q > 0.5 */
    {
        n += 1;
        carry = 0;
        for (i = 0; i < jz; i++) /* compute 1 - q */
        {
            j = iq[i];
            if (carry == 0)
            {
                if (j != 0)
                {
                    carry = 1;
                    iq[i] = 0x1000000 - j;
                }
            }
            else
            {
                iq[i] = 0xffffff - j;
            }
        }
        if (q0 > 0) /* rare case: chance is 1 in 12 */
        {
            switch (q0)
            {
                case 1:
                {
                    iq[jz - 1] &= 0x7fffff;
                    break;
                }
                case 2:
                {
                    iq[jz - 1] &= 0x3fffff;
                    break;
                }
            }
        }
        if (ih == 2)
        {
            z = one - z;
            if (carry != 0)
            {
                z -= scalbn(one, q0);
            }
        }
    }

    /* check if recomputation is needed */
    if (z == zero)
    {
        j = 0;
        for (i = jz - 1; i >= jk; i--)
        {
            j |= iq[i];
        }
        if (j == 0) /* need recomputation */
        {
            for (k = 1; iq[jk - k] == 0; k++) /* k = no. of terms needed */
            {
            }

            for (i = jz + 1; i <= jz + k; i++) /* add q[jz + 1] to q[jz + k] */
            {
                f[jx + i] = (double) ipio2[jv + i];
                for (j = 0, fw = 0.0; j <= jx; j++)
                {
                    fw += x[j] * f[jx + i - j];
                }
                q[i] = fw;
            }
            jz += k;
            goto recompute;
        }
    }

    /* chop off zero terms */
    if (z == 0.0)
    {
        jz -= 1;
        q0 -= 24;
        while (iq[jz] == 0)
        {
            jz--;
            q0 -= 24;
        }
    }
    else
    {
        /* break z into 24-bit if necessary */
        z = scalbn(z, -q0);
        if (z >= two24)
        {
            fw = (double)((int)(twon24 * z));
            iq[jz] = (int)(z - two24 * fw);
            jz += 1;
            q0 += 24;
            iq[jz] = (int) fw;
        }
        else
        {
            iq[jz] = (int) z;
        }
    }

    /* convert integer "bit" chunk to floating-point value */
    fw = scalbn(one, q0);
    for (i = jz; i >= 0; i--)
    {
        q[i] = fw * (double) iq[i];
        fw *= twon24;
    }

    /* compute PIo2[0, ..., jp] * q[jz, ..., 0] */
    for (i = jz; i >= 0; i--)
    {
        for (fw = 0.0, k = 0; k <= jp && k <= jz - i; k++)
        {
            fw += PIo2[k] * q[i + k];
        }
        fq[jz - i] = fw;
    }

    /* compress fq[] into y[] */
    switch (prec)
    {
        case 0:
        {
            fw = 0.0;
            for (i = jz; i >= 0; i--)
            {
                fw += fq[i];
            }
            y[0] = (ih == 0) ? fw : -fw;
            break;
        }
        case 1:
        case 2:
        {
            fw = 0.0;
            for (i = jz; i >= 0; i--)
            {
                fw += fq[i];
            }
            y[0] = (ih == 0) ? fw : -fw;
            fw = fq[0] - fw;
            for (i = 1; i <= jz; i++)
            {
                fw += fq[i];
            }
            y[1] = (ih == 0) ? fw : -fw;
            break;
        }
        case 3: /* painful */
        {
            for (i = jz; i > 0; i--)
            {
                fw = fq[i - 1] + fq[i];
                fq[i] += fq[i - 1] - fw;
                fq[i - 1] = fw;
            }
            for (i = jz; i > 1; i--)
            {
                fw = fq[i - 1] + fq[i];
                fq[i] += fq[i - 1] - fw;
                fq[i - 1] = fw;
            }
            for (fw = 0.0, i = jz; i >= 2; i--)
            {
                fw += fq[i];
            }
            if (ih == 0)
            {
                y[0] = fq[0];
                y[1] = fq[1];
                y[2] = fw;
            }
            else
            {
                y[0] = -fq[0];
                y[1] = -fq[1];
                y[2] = -fw;
            }
        }
    }
    return n & 7;
} /* __kernel_rem_pio2 */

#define invpio2 6.36619772367581382433e-01 /* 0x3FE45F30, 0x6DC9C883 */
#define pio2_1  1.57079632673412561417e+00 /* 0x3FF921FB, 0x54400000 */
#define pio2_1t 6.07710050650619224932e-11 /* 0x3DD0B461, 0x1A626331 */
#define pio2_2  6.07710050630396597660e-11 /* 0x3DD0B461, 0x1A600000 */
#define pio2_2t 2.02226624879595063154e-21 /* 0x3BA3198A, 0x2E037073 */
#define pio2_3  2.02226624871116645580e-21 /* 0x3BA3198A, 0x2E000000 */
#define pio2_3t 8.47842766036889956997e-32 /* 0x397B839A, 0x252049C1 */

static int __ieee754_rem_pio2(double x, double *y)
{
    double z, w, t, r, fn;
    double tx[3];
    int e0, i, j, nx, n, ix, hx;

    hx = __HI(x);  /* high word of x */
    ix = hx & 0x7fffffff;
    if (ix <= 0x3fe921fb) /* |x| ~<= pi/4 , no need for reduction */
    {
        y[0] = x;
        y[1] = 0;
        return 0;
    }
    if (ix < 0x4002d97c) /* |x| < 3pi/4, special case with n = +-1 */
    {
        if (hx > 0)
        {
            z = x - pio2_1;
            if (ix != 0x3ff921fb) /* 33 + 53 bit pi is good enough */
            {
                y[0] = z - pio2_1t;
                y[1] = (z - y[0]) - pio2_1t;
            }
            else /* near pi/2, use 33 + 33 + 53 bit pi */
            {
                z -= pio2_2;
                y[0] = z - pio2_2t;
                y[1] = (z - y[0]) - pio2_2t;
            }
            return 1;
        }
        else /* negative x */
        {
            z = x + pio2_1;
            if (ix != 0x3ff921fb) /* 33 + 53 bit pi is good enough */
            {
                y[0] = z + pio2_1t;
                y[1] = (z - y[0]) + pio2_1t;
            }
            else /* near pi/2, use 33 + 33 + 53 bit pi */
            {
                z += pio2_2;
                y[0] = z + pio2_2t;
                y[1] = (z - y[0]) + pio2_2t;
            }
            return -1;
        }
    }
    if (ix <= 0x413921fb) /* |x| ~<= 2^19 * (pi/2), medium size */
    {
        t = fabs(x);
        n = (int)(t * invpio2 + half);
        fn = (double) n;
        r = t - fn * pio2_1;
        w = fn * pio2_1t; /* 1st round good to 85 bit */
        if (n < 32 && ix != npio2_hw[n - 1])
        {
            y[0] = r - w; /* quick check no cancellation */
        }
        else
        {
            j = ix >> 20;
            y[0] = r - w;
            i = j - (((__HI(y[0])) >> 20) & 0x7ff);
            if (i > 16) /* 2nd iteration needed, good to 118 */
            {
                t = r;
                w = fn * pio2_2;
                r = t - w;
                w = fn * pio2_2t - ((t - r) - w);
                y[0] = r - w;
                i = j - (((__HI(y[0])) >> 20) & 0x7ff);
                if (i > 49) /* 3rd iteration need, 151 bits acc, will cover all possible cases */
                {
                    t = r;
                    w = fn * pio2_3;
                    r = t - w;
                    w = fn * pio2_3t - ((t - r) - w);
                    y[0] = r - w;
                }
            }
        }
        y[1] = (r - y[0]) - w;
        if (hx < 0)
        {
            y[0] = -y[0];
            y[1] = -y[1];
            return -n;
        }
        else
        {
            return n;
        }
    }
    /*
    *    * all other (large) arguments
    *       */
    if (ix >= 0x7ff00000) /* x is inf or NaN */
    {
        y[0] = y[1] = x - x;
        return 0;
    }
    /* set z = scalbn(|x|, ilogb(x) - 23) */
    __LO(z) = __LO(x);
    e0 = (ix >> 20) - 1046; /* e0 = ilogb(z) - 23; */
    __HI(z) = ix - (e0 << 20);
    for (i = 0; i < 2; i++)
    {
        tx[i] = (double)((int)(z));
        z = (z - tx[i]) * two24;
    }
    tx[2] = z;
    nx = 3;
    while (tx[nx - 1] == zero) /* skip zero term */
    {
        nx--;
    }
    n = __kernel_rem_pio2(tx, y, e0, nx, 2);
    if (hx < 0)
    {
        y[0] = -y[0];
        y[1] = -y[1];
        return -n;
    }
    return n;
} /* __ieee754_rem_pio2 */


static double __kernel_tan(double x, double y, int iy)
{
    double z, r, v, w, s;
    int ix, hx;

    hx = __HI(x);  /* high word of x */
    ix = hx & 0x7fffffff; /* high word of |x| */
    if (ix < 0x3e300000) /* x < 2**-28 */
    {
        if ((int) x == 0) /* generate inexact */
        {
            if (((ix | __LO(x)) | (iy + 1)) == 0)
            {
                return one / fabs(x);
            }
            else
            {
                if (iy == 1)
                {
                    return x;
                }
                else /* compute -1 / (x + y) carefully */
                {
                    double a, t;

                    z = w = x + y;
                    __LO(z) = 0;
                    v = y - (z - x);
                    t = a = -one / w;
                    __LO(t) = 0;
                    s = one + t * z;
                    return t + a * (s + t * v);
                }
            }
        }
    }
    if (ix >= 0x3FE59428) /* |x| >= 0.6744 */
    {
        if (hx < 0)
        {
            x = -x;
            y = -y;
        }
        z = pio4 - x;
        w = pio4lo - y;
        x = z + w;
        y = 0.0;
    }
    z = x * x;
    w = z * z;
    /*
    *    * Break x^5 * (T[1] + x^2 * T[2] + ...) into
    *       * x^5 (T[1] + x^4 * T[3] + ... + x^20 * T[11]) +
    *          * x^5 (x^2 * (T[2] + x^4 * T[4] + ... + x^22 * [T12]))
    *             */
    r = T1 + w * (T3 + w * (T5 + w * (T7 + w * (T9 + w * T11))));
    v = z * (T2 + w * (T4 + w * (T6 + w * (T8 + w * (T10 + w * T12)))));
    s = z * x;
    r = y + z * (s * (r + v) + y);
    r += T0 * s;
    w = x + r;
    if (ix >= 0x3FE59428)
    {
        v = (double) iy;
        return (double)(1 - ((hx >> 30) & 2)) * (v - 2.0 * (x - (w * w / (w + v) - r)));
    }
    if (iy == 1)
    {
        return w;
    }
    else
    {
        /*
        *      * if allow error up to 2 ulp, simply return
        *           * -1.0 / (x + r) here
        *                */
        /* compute -1.0 / (x + r) accurately */
        double a, t;

        z = w;
        __LO(z) = 0;
        v = r - (z - x); /* z + v = r + x */
        t = a = -1.0 / w; /* a = -1.0 / w */
        __LO(t) = 0;
        s = 1.0 + t * z;
        return t + a * (s + t * v);
    }
} /* __kernel_tan */

double tan(double x)
{
    double y[2], z = 0.0;
    int n, ix;

    /* High word of x. */
    ix = __HI(x);

    /* |x| ~< pi/4 */
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb)
    {
        return __kernel_tan(x, z, 1);
    }

    /* tan(Inf or NaN) is NaN */
    else if (ix >= 0x7ff00000)
    {
        return x - x; /* NaN */
    }

    /* argument reduction needed */
    else
    {
        n = __ieee754_rem_pio2(x, y);
        return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1));  /*   1 -- n even, -1 -- n odd */
    }
} /* tan */

static const double atanhi[] =
{
    4.63647609000806093515e-01, /* atan(0.5)hi 0x3FDDAC67, 0x0561BB4F */
    7.85398163397448278999e-01, /* atan(1.0)hi 0x3FE921FB, 0x54442D18 */
    9.82793723247329054082e-01, /* atan(1.5)hi 0x3FEF730B, 0xD281F69B */
    1.57079632679489655800e+00, /* atan(inf)hi 0x3FF921FB, 0x54442D18 */
};

static const double atanlo[] =
{
    2.26987774529616870924e-17, /* atan(0.5)lo 0x3C7A2B7F, 0x222F65E2 */
    3.06161699786838301793e-17, /* atan(1.0)lo 0x3C81A626, 0x33145C07 */
    1.39033110312309984516e-17, /* atan(1.5)lo 0x3C700788, 0x7AF0CBBD */
    6.12323399573676603587e-17, /* atan(inf)lo 0x3C91A626, 0x33145C07 */
};

#define aT0   3.33333333333329318027e-01 /* 0x3FD55555, 0x5555550D */
#define aT1  -1.99999999998764832476e-01 /* 0xBFC99999, 0x9998EBC4 */
#define aT2   1.42857142725034663711e-01 /* 0x3FC24924, 0x920083FF */
#define aT3  -1.11111104054623557880e-01 /* 0xBFBC71C6, 0xFE231671 */
#define aT4   9.09088713343650656196e-02 /* 0x3FB745CD, 0xC54C206E */
#define aT5  -7.69187620504482999495e-02 /* 0xBFB3B0F2, 0xAF749A6D */
#define aT6   6.66107313738753120669e-02 /* 0x3FB10D66, 0xA0D03D51 */
#define aT7  -5.83357013379057348645e-02 /* 0xBFADDE2D, 0x52DEFD9A */
#define aT8   4.97687799461593236017e-02 /* 0x3FA97B4B, 0x24760DEB */
#define aT9  -3.65315727442169155270e-02 /* 0xBFA2B444, 0x2C6A6C2F */
#define aT10  1.62858201153657823623e-02 /* 0x3F90AD3A, 0xE322DA11 */

#define one  1.0
#define huge 1.0e300

double atan(double x)
{
    double w, s1, s2, z;
    int ix, hx, id;

    hx = __HI(x);
    ix = hx & 0x7fffffff;
    if (ix >= 0x44100000) /* if |x| >= 2^66 */
    {
        if (ix > 0x7ff00000 || (ix == 0x7ff00000 && (__LO(x) != 0)))
        {
            return x + x; /* NaN */
        }
        if (hx > 0)
        {
            return atanhi[3] + atanlo[3];
        }
        else
        {
            return -atanhi[3] - atanlo[3];
        }
    }
    if (ix < 0x3fdc0000) /* |x| < 0.4375 */
    {
        if (ix < 0x3e200000) /* |x| < 2^-29 */
        {
            if (huge + x > one) /* raise inexact */
            {
                return x;
            }
        }
        id = -1;
    }
    else
    {
        x = fabs(x);
        if (ix < 0x3ff30000) /* |x| < 1.1875 */
        {
            if (ix < 0x3fe60000) /* 7/16 <= |x| < 11/16 */
            {
                id = 0;
                x = (2.0 * x - one) / (2.0 + x);
            }
            else /* 11/16 <= |x| < 19/16 */
            {
                id = 1;
                x = (x - one) / (x + one);
            }
        }
        else
        {
            if (ix < 0x40038000) /* |x| < 2.4375 */
            {
                id = 2;
                x = (x - 1.5) / (one + 1.5 * x);
            }
            else /* 2.4375 <= |x| < 2^66 */
            {
                id = 3;
                x = -1.0 / x;
            }
        }
    }
    /* end of argument reduction */
    z = x * x;
    w = z * z;
    /* break sum from i=0 to 10 aT[i] z**(i+1) into odd and even poly */
    s1 = z * (aT0 + w * (aT2 + w * (aT4 + w * (aT6 + w * (aT8 + w * aT10)))));
    s2 = w * (aT1 + w * (aT3 + w * (aT5 + w * (aT7 + w * aT9))));
    if (id < 0)
    {
        return x - x * (s1 + s2);
    }
    else
    {
        z = atanhi[id] - ((x * (s1 + s2) - atanlo[id]) - x);
        return (hx < 0) ? -z : z;
    }
} /* atan */
