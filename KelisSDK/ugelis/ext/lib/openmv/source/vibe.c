// vibe_tiny.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"


#include "vibe.h"

#define VIBE_FORGROUND_COUNT (100)  //统计前景出现次数，如果超过此次数则把前景更新到背景中


const int gVibeRandom = 127; //vibe子采样率为16
const int gRadius = 20; //当前帧与背景模型作差的阈值（可变）
const int gMin = 3; //当前帧与背景模型做差其差值小于20的个数，如果大于等于3则认为是当前像素是背景
const int gVibeSimpleSize = 8; //背景模型保存的帧数

static int vibe_width = 0;
static int vibe_height = 0;
static int gVibeSize = 0;
static int gVibeModelLastSize = 0;

static unsigned char *VibeModelData = NULL; //用于存储样本集
static unsigned char *BackGroundData = NULL;

static int InitialParams(unsigned int ImgWidth, unsigned int ImgHeight)
{
	vibe_width = ImgWidth;
    vibe_height = ImgHeight;
    gVibeSize = vibe_width*vibe_height;
    gVibeModelLastSize = vibe_width*vibe_height * 8;

    VibeModelData = (unsigned char*)malloc_ext(0, vibe_width*vibe_height * 9);
    if(VibeModelData == NULL)
    {
        printf("ViBe InitialParams() fail\n");
        return -1;
    }
    else
    {
        memset(VibeModelData, 0, vibe_width*vibe_height * 9);
    }

    BackGroundData = (unsigned char*)malloc_ext(0, vibe_width*vibe_height);
    if(BackGroundData == NULL)
    {
        printf("ViBe InitialParams() fail\n");
        return -1;
    }
    else
    {
        memset(BackGroundData, 0, vibe_width*vibe_height);
    }

    return 0;
}


//---------------------------------------------------------------------------
//  Function name：InitialModelData()
//  Function ：init VibeModelData by first Frame ,
//  Parameter：pImgData 
//---------------------------------------------------------------------------
static void InitialModelData(const unsigned char *pImgData)
{
	int i = 0;

	for (i = 0; i<gVibeSimpleSize; i++)
	{
		memcpy(VibeModelData + gVibeSize * i, pImgData, gVibeSize);
	}
}

//---------------------------------------------------------------------------
//  Function name：VibeForground()
//  Function：The current frame and background model are compared, 
//            and the foreground of the current frame is obtained.
//  Parameter：CurImgPyr 
//  Reference:<<A universal background subtraction algorithm for video sequences>>
//  201507--zhangbin
//---------------------------------------------------------------------------

void VibeForground(unsigned char *CurImg, unsigned char* ForImg, unsigned int ImgWidth, unsigned int ImgHeight)
{
	unsigned int nOffset = 0;//偏移量

	short nDiff = 0; //保存当前帧与背景模型中某帧的差值
	unsigned int j = 0;
	unsigned int i = 0;
	unsigned char nCount = 0;
	unsigned char *pSrcImg = 0;
	unsigned char *pDstImg = 0;
	static unsigned char CountNum = 0; //此数除以16,127为一个循环，共能够取0-7之前的随机数（模拟VIBE子采样概率生成的随机数）
	static int g_IsModelInit = 1;

	pSrcImg = CurImg;
	pDstImg = ForImg;

	CountNum++;
	if (CountNum>gVibeRandom)
	{
		CountNum = 0;
	}

	if (g_IsModelInit)
	{
	    InitialParams(ImgWidth, ImgHeight);
		InitialModelData(pSrcImg); //当前帧初始化背景
		memcpy(BackGroundData, pSrcImg, gVibeSize);
		g_IsModelInit = 0;
		return;
	}

	nOffset = (CountNum >> 4) * gVibeSize;	//CountNum>>4相当于16帧生成一个数

	for (i = 0; i < gVibeSize; i++)
	{
		nCount = 0;
		pDstImg[i] = 255;

		for (j = 0; j < gVibeSimpleSize; j++) //遍历背景模型
		{
			nDiff = VibeModelData[j*gVibeSize + i] - pSrcImg[i];

			if (nDiff <= gRadius)
			{
				if (nDiff >= -gRadius)
				{
					nCount++;
				}
			}

			if (nCount >= gMin) //当前帧像素值小于gRadius的次数大于gMin（论文中为#min = 2）则此点判断为背景
			{
				pDstImg[i] = 0;
				break;
			}
		}
	}

	for (i = 0; i < gVibeSize; i++)
	{
		if (pDstImg[i]) //如果此点为前景则累加此点为前景的次数
		{
			VibeModelData[gVibeModelLastSize + i]++;

			if (VibeModelData[gVibeModelLastSize + i]>100)
			{
				VibeModelData[nOffset + i] = pSrcImg[i];
			}
		}
		else//如果此点为背景，则对背景模型进行更新
		{

			VibeModelData[nOffset + i] = pSrcImg[i];

			BackGroundData[i] = pSrcImg[i];

			VibeModelData[gVibeModelLastSize + i] = 0;
		}
	}
}
