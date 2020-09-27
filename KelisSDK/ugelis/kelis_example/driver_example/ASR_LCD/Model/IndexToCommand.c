#include "../base_types.h"

const char* const k_lpszTrigger[] =
{
	"格力金贝",
};

const char* const k_lpszCommand[] =
{
	"打开空调 ",
	"关闭空调 ",
	"十六度 ",
	"十七度 ",
	"十八度 ",
	"十九度 ",
	"二十度 ",
	"二十一度 ",
	"二十二度 ",
	"二十三度 ",
	"二十四度 ",
	"二十五度 ",
	"二十六度 ",
	"二十七度 ",
	"二十八度 ",
	"二十九度 ",
	"三十度 ",
	"升高温度 ",
	"降低温度 ",
	"自动模式 ",
	"制冷模式 ",
	"除湿模式 ",
	"送风模式 ",
	"加热模式 ",
	"最大风 ",
	"最小风 ",
	"增大风速 ",
	"减小风速 ",
	"自动风 ",
	"增大音量 ",
	"减小音量 ",
	"打开左右扫风 ",
	"打开上下扫风 ",
	"关闭左右扫风 ",
	"关闭上下扫风 ",
	"语音关闭 ",
	"关闭语音 ",
};

int GetTriggerCount()
{
	return sizeof(k_lpszTrigger) / sizeof(k_lpszTrigger[0]);
}

int GetCommandCount()
{
	return sizeof(k_lpszCommand) / sizeof(k_lpszCommand[0]);
}

const char* GetTriggerStringByIndex(int nIndex)
{
	if (nIndex == 0 && nIndex < sizeof(k_lpszTrigger) / sizeof(k_lpszTrigger[0]))
		return k_lpszTrigger[nIndex];
	else
		return NULL;
}

const char* GetCommandStringByIndex(int nIndex)
{
	if (nIndex >= 0 && nIndex < sizeof(k_lpszCommand) / sizeof(k_lpszCommand[0]))
		return k_lpszCommand[nIndex];
	else
		return NULL;
}
