// vibe_tiny.cpp : �������̨Ӧ�ó������ڵ㡣
//

//#include "stdafx.h"


#include "vibe.h"

#define VIBE_FORGROUND_COUNT (100)  //ͳ��ǰ�����ִ�������������˴������ǰ�����µ�������


const int gVibeRandom = 127; //vibe�Ӳ�����Ϊ16
const int gRadius = 20; //��ǰ֡�뱳��ģ���������ֵ���ɱ䣩
const int gMin = 3; //��ǰ֡�뱳��ģ���������ֵС��20�ĸ�����������ڵ���3����Ϊ�ǵ�ǰ�����Ǳ���
const int gVibeSimpleSize = 8; //����ģ�ͱ����֡��

static int vibe_width = 0;
static int vibe_height = 0;
static int gVibeSize = 0;
static int gVibeModelLastSize = 0;

static unsigned char *VibeModelData = NULL; //���ڴ洢������
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
//  Function name��InitialModelData()
//  Function ��init VibeModelData by first Frame ,
//  Parameter��pImgData 
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
//  Function name��VibeForground()
//  Function��The current frame and background model are compared, 
//            and the foreground of the current frame is obtained.
//  Parameter��CurImgPyr 
//  Reference:<<A universal background subtraction algorithm for video sequences>>
//  201507--zhangbin
//---------------------------------------------------------------------------

void VibeForground(unsigned char *CurImg, unsigned char* ForImg, unsigned int ImgWidth, unsigned int ImgHeight)
{
	unsigned int nOffset = 0;//ƫ����

	short nDiff = 0; //���浱ǰ֡�뱳��ģ����ĳ֡�Ĳ�ֵ
	unsigned int j = 0;
	unsigned int i = 0;
	unsigned char nCount = 0;
	unsigned char *pSrcImg = 0;
	unsigned char *pDstImg = 0;
	static unsigned char CountNum = 0; //��������16,127Ϊһ��ѭ�������ܹ�ȡ0-7֮ǰ���������ģ��VIBE�Ӳ����������ɵ��������
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
		InitialModelData(pSrcImg); //��ǰ֡��ʼ������
		memcpy(BackGroundData, pSrcImg, gVibeSize);
		g_IsModelInit = 0;
		return;
	}

	nOffset = (CountNum >> 4) * gVibeSize;	//CountNum>>4�൱��16֡����һ����

	for (i = 0; i < gVibeSize; i++)
	{
		nCount = 0;
		pDstImg[i] = 255;

		for (j = 0; j < gVibeSimpleSize; j++) //��������ģ��
		{
			nDiff = VibeModelData[j*gVibeSize + i] - pSrcImg[i];

			if (nDiff <= gRadius)
			{
				if (nDiff >= -gRadius)
				{
					nCount++;
				}
			}

			if (nCount >= gMin) //��ǰ֡����ֵС��gRadius�Ĵ�������gMin��������Ϊ#min = 2����˵��ж�Ϊ����
			{
				pDstImg[i] = 0;
				break;
			}
		}
	}

	for (i = 0; i < gVibeSize; i++)
	{
		if (pDstImg[i]) //����˵�Ϊǰ�����ۼӴ˵�Ϊǰ���Ĵ���
		{
			VibeModelData[gVibeModelLastSize + i]++;

			if (VibeModelData[gVibeModelLastSize + i]>100)
			{
				VibeModelData[nOffset + i] = pSrcImg[i];
			}
		}
		else//����˵�Ϊ��������Ա���ģ�ͽ��и���
		{

			VibeModelData[nOffset + i] = pSrcImg[i];

			BackGroundData[i] = pSrcImg[i];

			VibeModelData[gVibeModelLastSize + i] = 0;
		}
	}
}
