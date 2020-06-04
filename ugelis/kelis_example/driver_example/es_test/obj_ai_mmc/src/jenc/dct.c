#include "jinclude.h"

void fdct_8x8(DCTVAL *in);
void dsp_fdct_8x8(short *dct_data, unsigned num_fdcts);

void fdct_8x8(DCTVAL *in)
{
    float c1 = 0.9808;
    float c2 = 0.9239;
    float c3 = 0.8315;
    float c4 = 0.7071;
    float c5 = 0.5556;
    float c6 = 0.3827;
    float c7 = 0.1951;

    int col, row;

    int s07, s16, s25, s34, s0734, s1625;

    int d07, d16, d25, d34, d0734, d1625;

    DCTVAL *data = in, *tdata[DCTSIZE];

    for (row = 0; row < DCTSIZE; row++)
    {
        /* sum of data */
        s07 = data[0] + data[7];
        s16 = data[1] + data[6];
        s25 = data[2] + data[5];
        s34 = data[3] + data[4];
        s0734 = s07 + s34;
        s1625 = s16 + s25;

        /* dif of data */
        d07 = data[0] - data[7];
        d16 = data[1] - data[6];
        d25 = data[2] - data[5];
        d34 = data[3] - data[4];
        d0734 = d07 - d34;
        d1625 = d16 - d25;

        data[0] = (c4 * (s0734 + s1625));
        data[1] = (c1 * d07 + c3 * d16 + c5 * d25 + c7 * d34);
        data[2] = (c2 * d0734 + c6 * d1625);
        data[3] = (c3 * d07 - c7 * d16 - c1 * d25 - c5 * d34);
        data[4] = (c4 * (s0734 - s1625));
        data[5] = (c5 * d07 - c1 * d16 + c7 * d25 + c3 * d34);
        data[6] = (c6 * d0734 - c2 * d1625);
        data[7] = (c7 * d07 - c5 * d16 + c3 * d25 - c1 * d34);

        data[0] >>= 1;
        data[1] >>= 1;
        data[2] >>= 1;
        data[3] >>= 1;
        data[4] >>= 1;
        data[5] >>= 1;
        data[6] >>= 1;
        data[7] >>= 1;

        data += DCTSIZE;
    }


    data = in;
    for (col = 0; col < DCTSIZE; col++)
    {
        int ii;
        DCTVAL *tmp = data;
        for (ii = 0; ii < DCTSIZE; ii++)
        {
            tdata[ii] = tmp;
            tmp += DCTSIZE;
        }

        //* sum of data

        s07 = *tdata[0] + *tdata[7];
        s16 = *tdata[1] + *tdata[6];
        s25 = *tdata[2] + *tdata[5];
        s34 = *tdata[3] + *tdata[4];
        s0734 = s07 + s34;
        s1625 = s16 + s25;

        // dif of data
        d07 = *tdata[0] - *tdata[7];
        d16 = *tdata[1] - *tdata[6];
        d25 = *tdata[2] - *tdata[5];
        d34 = *tdata[3] - *tdata[4];
        d0734 = d07 - d34;
        d1625 = d16 - d25;


        *tdata[0] = (c4 * (s0734 + s1625));
        *tdata[1] = (c1 * d07 + c3 * d16 + c5 * d25 + c7 * d34);
        *tdata[2] = (c2 * d0734 + c6 * d1625);
        *tdata[3] = (c3 * d07 - c7 * d16 - c1 * d25 - c5 * d34);
        *tdata[4] = (c4 * (s0734 - s1625));
        *tdata[5] = (c5 * d07 - c1 * d16 + c7 * d25 + c3 * d34);
        *tdata[6] = (c6 * d0734 - c2 * d1625);
        *tdata[7] = (c7 * d07 - c5 * d16 + c3 * d25 - c1 * d34);

        *tdata[0] >>= 1;
        *tdata[1] >>= 1;
        *tdata[2] >>= 1;
        *tdata[3] >>= 1;
        *tdata[4] >>= 1;
        *tdata[5] >>= 1;
        *tdata[6] >>= 1;
        *tdata[7] >>= 1;

        data++;
    }
}

volatile int temp_dct1[64];

void dsp_fdct_8x8(short *dct_data, unsigned num_fdcts)
{
    const unsigned short c1 = 0x2C62, c3 = 0x25A0;
    const unsigned short c5 = 0x1924, c7 = 0x08D4;
    const unsigned short c0 = 0xB505, c2 = 0x29CF;
    const unsigned short c6 = 0x1151;

    short f0, f1, f2, f3,
          f4, f5, f6, f7;
    int   g0, g1, h0, h1,
          p0, p1;
    short r0, r1;
    int   P0, P1, R0, R1;
    short g2, g3, h2, h3;
    short q0a, s0a, q0, q1,
          s0, s1;
    short Q0, Q1, S0, S1;
    int   F0, F1, F2, F3,
          F4, F5, F6, F7;
    int   F0r, F1r, F2r, F3r,
          F4r, F5r, F6r, F7r;

    unsigned i, j;
    short    *dct_io_ptr;

    dct_io_ptr = dct_data;

    for (i = 0; i < 64; i++)
    {
        temp_dct1[i] = dct_data[i];
    }


    for (i = 0; i < num_fdcts; i++)
    {
        for (j = 0; j < 8; j++)
        {
            f0 = dct_io_ptr[ 0];
            f1 = dct_io_ptr[ 8];
            f2 = dct_io_ptr[16];
            f3 = dct_io_ptr[24];
            f4 = dct_io_ptr[32];
            f5 = dct_io_ptr[40];
            f6 = dct_io_ptr[48];
            f7 = dct_io_ptr[56];

            g0 = f0 + f7;
            h2 = f0 - f7;
            g1 = f1 + f6;
            h3 = f1 - f6;
            h1 = f2 + f5;
            g3 = f2 - f5;
            h0 = f3 + f4;
            g2 = f3 - f4;

            p0 = g0 + h0;
            r0 = g0 - h0;
            p1 = g1 + h1;
            r1 = g1 - h1;
            q1 = g2;
            s1 = h2;

            s0a = h3 + g3;
            q0a = h3 - g3;
            s0 = (s0a * c0 + 0x7FFF) >> 16;
            q0 = (q0a * c0 + 0x7FFF) >> 16;

            P0 = p0 + p1;
            P1 = p0 - p1;
            R1 = c6 * r1 + c2 * r0;
            R0 = c6 * r0 - c2 * r1;

            Q1 = q1 + q0;
            Q0 = q1 - q0;
            S1 = s1 + s0;
            S0 = s1 - s0;

            F0 = P0;
            F4 = P1;
            F2 = R1;
            F6 = R0;

            F1 = c7 * Q1 + c1 * S1;
            F7 = c7 * S1 - c1 * Q1;
            F5 = c3 * Q0 + c5 * S0;
            F3 = c3 * S0 - c5 * Q0;

            dct_io_ptr[ 0] = F0;
            dct_io_ptr[ 8] = F1 >> 13;
            dct_io_ptr[16] = F2 >> 13;
            dct_io_ptr[24] = F3 >> 13;
            dct_io_ptr[32] = F4;
            dct_io_ptr[40] = F5 >> 13;
            dct_io_ptr[48] = F6 >> 13;
            dct_io_ptr[56] = F7 >> 13;

            dct_io_ptr++;
        }
        dct_io_ptr += 56;
    }

    dct_io_ptr = dct_data;
    for (i = 0; i < 8 * num_fdcts; i++)
    {
        f0 = dct_io_ptr[0];
        f1 = dct_io_ptr[1];
        f2 = dct_io_ptr[2];
        f3 = dct_io_ptr[3];
        f4 = dct_io_ptr[4];
        f5 = dct_io_ptr[5];
        f6 = dct_io_ptr[6];
        f7 = dct_io_ptr[7];

        g0 = f0 + f7;
        h2 = f0 - f7;
        g1 = f1 + f6;
        h3 = f1 - f6;
        h1 = f2 + f5;
        g3 = f2 - f5;
        h0 = f3 + f4;
        g2 = f3 - f4;

        p0 = g0 + h0;
        r0 = g0 - h0;
        p1 = g1 + h1;
        r1 = g1 - h1;
        q1 = g2;
        s1 = h2;

        s0a = h3 + g3;
        q0a = h3 - g3;
        q0 = (q0a * c0 + 0x7FFF) >> 16;
        s0 = (s0a * c0 + 0x7FFF) >> 16;

        P0 = p0 + p1;
        P1 = p0 - p1;
        R1 = c6 * r1 + c2 * r0;
        R0 = c6 * r0 - c2 * r1;

        Q1 = q1 + q0;
        Q0 = q1 - q0;
        S1 = s1 + s0;
        S0 = s1 - s0;

        F0 = P0;
        F4 = P1;
        F2 = R1;
        F6 = R0;

        F1 = c7 * Q1 + c1 * S1;
        F7 = c7 * S1 - c1 * Q1;
        F5 = c3 * Q0 + c5 * S0;
        F3 = c3 * S0 - c5 * Q0;

        F0r = (F0 + 0x0006) >>  3;
        F1r = (F1 + 0x7FFF) >> 16;
        F2r = (F2 + 0x7FFF) >> 16;
        F3r = (F3 + 0x7FFF) >> 16;
        F4r = (F4 + 0x0004) >>  3;
        F5r = (F5 + 0x7FFF) >> 16;
        F6r = (F6 + 0x7FFF) >> 16;
        F7r = (F7 + 0x7FFF) >> 16;

        dct_io_ptr[0] = F0r;
        dct_io_ptr[1] = F1r;
        dct_io_ptr[2] = F2r;
        dct_io_ptr[3] = F3r;
        dct_io_ptr[4] = F4r;
        dct_io_ptr[5] = F5r;
        dct_io_ptr[6] = F6r;
        dct_io_ptr[7] = F7r;

        dct_io_ptr += 8;
    }

    for (i = 0; i < 64; i++)
    {
        temp_dct1[i] = dct_data[i];
    }

    return;
}


