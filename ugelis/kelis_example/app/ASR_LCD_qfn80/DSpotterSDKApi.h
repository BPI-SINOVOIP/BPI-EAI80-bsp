
#ifndef __DSPOTTERSDK_API_H
#define __DSPOTTERSDK_API_H

#if defined(_WIN32)
	#ifdef DSpotterDll_EXPORTS
		#define DSPDLL_API __declspec(dllexport)
	#endif
#endif

#ifndef DSPDLL_API
#define DSPDLL_API
#endif

#include "base_types.h"
#include "DSpotterSDKApi_Const.h"

#ifdef __cplusplus
extern "C"{
#endif

// lpbyCYBase(IN): The background model, contents of CYBase.mod.
// lppbyModel(IN): The command model.
// nMaxTime(IN): The maximum buffer length in number of frames for keeping the status of commands.
// lpbyMemPool(IN/OUT): Memory buffer for the recognizer.
// nMemSize(IN): Size in bytes of the memory buffer lpbyMemPool.
// lpbyState(IN/OUT): State buffer for recognizer.
// nStateSize(IN): Size in bytes of the state buffer lpbyState.
// pnErr(OUT): DSPOTTER_SUCCESS indicates success, else error code. It can be NULL.
// Return: a recognizer handle or NULL
DSPDLL_API HANDLE DSpotter_Init_Multi(BYTE *lpbyCYBase, BYTE *lppbyModel[], INT nNumModel, INT nMaxTime, BYTE *lpbyMemPool, INT nMemSize, BYTE *lpbyState, INT nStateSize, INT *pnErr);

// Purpose: Destroy a recognizer (free resources)
// hDSpotter(IN): a handle of the recognizer
// Return: Success or error code
DSPDLL_API INT DSpotter_Release(HANDLE hDSpotter);

// Purpose: Reset recognizer
// hDSpotter(IN): a handle of the recognizer
// Return: Success or error code
DSPDLL_API INT DSpotter_Reset(HANDLE hDSpotter);

// Purpose: Transfer voice samples to the recognizer for recognizing.
// hDSpotter(IN): a handle of the recognizer
// lpsSample(IN): the pointer of voice data buffer
// nNumSample(IN): the number of voice data (a unit is a short, we prefer to add 480 samples per call)
// Return: "DSPOTTER_ERR_NeedMoreSample" indicates call this function again, else call DSpotter_GetResult(...)
DSPDLL_API INT DSpotter_AddSample(HANDLE hDSpotter, SHORT *lpsSample, INT nNumSample);

// Purpose: Get recognition results.
// hDSpotter(IN): a handle of the recognizer
// Return: the command ID. It is 0 based
DSPDLL_API INT DSpotter_GetResult(HANDLE hDSpotter);
DSPDLL_API INT DSpotter_GetResultEPD(HANDLE hDSpotter, INT *pnWordDura, INT *pnEndDelay);
DSPDLL_API INT DSpotter_GetResultScore(HANDLE hDSpotter);
DSPDLL_API INT DSpotter_GetResultCmdModel(HANDLE hDSpotter, INT *pnModelID, INT *pnTagID);
DSPDLL_API INT DSpotter_GetResultMapID(HANDLE hDSpotter);

DSPDLL_API INT DSpotter_SetResultMapID_Sep(HANDLE hDSpotter, USHORT *lpnMapID);
DSPDLL_API INT DSpotter_SetResultMapID_Multi(HANDLE hDSpotter, USHORT **lppnMapID, INT nNumMapID);

DSPDLL_API INT DSpotter_GetNumWord(BYTE *lpbyModel);

DSPDLL_API INT DSpotter_GetMemoryUsage_Multi(BYTE *lpbyCYBase, BYTE *lppbyModel[], INT nNumModel, INT nMaxTime);

DSPDLL_API const char *DSpotter_VerInfo(void);

/************************************************************************/
//  Threshold Adjust API                                                                   
/************************************************************************/
// Purpose: Set model rejection level
// hDSpotter(IN): a handle of the recognizer
// nRejectionLevel(IN): rejection level
// Return: Success or error code
DSPDLL_API INT DSpotter_SetRejectionLevel(HANDLE hDSpotter, INT nRejectionLevel);

#ifdef __cplusplus
}
#endif

#endif // __DSPOTTERSDK_API_H
