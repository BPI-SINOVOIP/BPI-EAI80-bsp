
//#include <ugelis.h>

#include <stdlib.h>
#include <math.h>
#include <sobel.h>

//#define SOBEL_DEBUG 1
#ifdef SOBEL_DEBUG
    #define sobel_prdebug       printk
#else
    #define sobel_prdebug(...)  do{}while(0);
#endif


//#define var uchar
typedef unsigned char var;

//extern double sqrt (double x);


//static int abs(int i) { return (i >= 0) ? i : -i; }

static void conv2(var *img, int w[3][3], int rows, int cols, var *result) 
{
	
	int bm = 3; int bn = 3;
	int n1 = 0;//rows + bm - 1;
	int n2 = 0;// cols + bn - 1;
	int sum_ = 0;
	int i = 0, j = 0, m = 0, n = 0;
	int rm = 0, rn = 0;

	n1 = rows;//rows + bm - 1;
	n2 = cols;// cols + bn - 1;

	for (i = 0; i < n1; i++) 
	{
		for (j = 0; j < n2; j++)
		{     
			sum_ = 0;
			for (m = 0; m < bm; m++)
			{
				for (n = 0; n < bn; n++)
				{
					rm = i - m;
					rn = j - n;
					/*补0处理*/
					if (rm >= 0 && rm < rows&&rn >= 0 && rn < cols)
						sum_ += img[rm*cols + rn] * w[m][n];
				}
			}
			/*取绝对值，超过255则视为255*/
			sum_ = abs(sum_);
			result[i*cols + j] = sum_;
			if (sum_ > 255)
			{
				result[i*cols + j] = 255;
			}
		}
	}
	return;
}

static void addxy(var *src1, var *src2, var *dst, int rows, int cols)
{
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			int sum = sqrt(src1[i*cols+j] * src1[i*cols + j] + src2[i*cols + j] * src2[i*cols + j]);
			if (sum > 255) dst[i*cols + j] = 255;
			else dst[i*cols + j] = sum;
		}
	}
	return;
}



void imlib_sobel(unsigned char *img_data, int rows, int cols, unsigned char *result)
{
	/*Sobel算子*/
	int sx[3][3] = {
		{ -1, 0, 1 },
		{ -2, 0, 2 },
		{ -1, 0, 1 },
	};
	int sy[3][3] = {
		{ 1, 2, 1 },
		{ 0, 0, 0 },
		{ -1, -2, -1 },
	};

	var *array = img_data;

    var *gradx = (var *)malloc(rows*cols);
    if(gradx == NULL)
    {
        sobel_prdebug("sobel malloc error!\n");
        return;
    }
    memset(gradx, 0, rows*cols);
	var *grady = (var *)malloc(rows*cols);
    if(grady == NULL)
    {
        sobel_prdebug("sobel malloc error!\n");
        free(gradx);
        return;
    }
    memset(grady, 0, rows*cols);
    
	/*利用sobel算子计算梯度*/
	conv2(array, sx, rows, cols, gradx);
	conv2(array, sy, rows, cols, grady);

    /*图像叠加*/
#if 1
    addxy(gradx, grady, result, rows, cols);
#else
    //addxy(grady, grady, result, rows, cols);
    //memcpy(result, grady, rows*cols);
#endif

    free(grady);
    free(gradx);
    return;
}


