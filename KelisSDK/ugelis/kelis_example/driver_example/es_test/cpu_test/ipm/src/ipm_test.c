#include "common.h"
#include "ipm.h"

static int x[N + 1][N + 1] __sram;
static int y[N + 1][N + 1] __sram;
static int z[N + 1][N + 1] __sram;
static int w[N + 1][N + 1] __sram;

int Ne[M][M] __sram = {1, 1, 1, 1, 1, 1, 1, 1, 1};
int Nq[M][M] __sram = {0, 1, 0, 1, 0, 1, 0, 1, 0};

int expected_x[] = {0, -1, -2, -3, -4, -5, 1, 0, -1, -2, -3, -4, 2, 1, 0, -1, -2, -3, 3, 2, 1, 0, -1, -2, 4, 3, 2, 1, 0, -1, 5, 4, 3, 2, 1, 0,};
int expected_y[] = {0, -1, -2, -3, -4, -5, 1, 0, 0, 1, -3, -4, 2, 0, 0, 0, -2, -3, 3, -1, 0, 0, -1, -2, 4, 3, 2, 1, 0, -1, 5, 4, 3, 2, 1, 0,};
int expected_z[] = {0, -1, -2, -3, -4, -5, 1, 1, 0, 0, -3, -4, 2, 2, 1, 0, -2, -3, 3, 3, 2, 1, -1, -2, 4, 3, 2, 1, 0, -1, 5, 4, 3, 2, 1, 0,};
int expected_w[] = {0, -1, -2, -3, -4, -5, 1, 0, -2, -3, -3, -4, 2, 0, -1, -2, -2, -3, 3, 0, 0, -1, -1, -2, 4, 3, 2, 1, 0, -1, 5, 4, 3, 2, 1, 0,};

extern void dilation(int A[N + 1][N + 1], int D[N + 1][N + 1], int B[M][M]);
extern void erosion(int A[N + 1][N + 1], int E[N + 1][N + 1], int B[M][M]);
extern void laplacian_filter(int A[N + 1][N + 1], int R[N + 1][N + 1], int B[M][M]);

void test_setup() {}

void test_clear()
{
    int i, j;
    for (i = 0; i <= N; i++)
        for (j = 0; j <= N; j++)
        {
            x[i][j] = i - j;
            y[i][j] = i - j;
            z[i][j] = i - j;
            w[i][j] = i - j;
        }
}


void test_run()
{
    laplacian_filter(x, y, Nq);
    dilation(x, z, Nq);
    erosion(z, w, Nq);
}

int test_check()
{
    int i, j;
    int index = 0;
    for (i = 0; i <= M; i++)
        for (j = 0; j <= N; j++)
        {
            if (x[i][j] != expected_x[index] ||
                    y[i][j] != expected_y[index] ||
                    z[i][j] != expected_z[index] ||
                    w[i][j] != expected_w[index])
            {
                return 1;
            }

            index++;
        }

    return 0;
}
