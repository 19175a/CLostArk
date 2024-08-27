#ifdef __cplusplus
#ifndef __OCR_LITE_C_API_H__
#define __OCR_LITE_C_API_H__
extern "C"
{

#pragma once

	/**************************************************/
	/********** imports / exports HPSocket4C **********/

#ifdef OCRLITE_STATIC_LIB
#define OCRLITE_API		EXTERN_C
#else
#ifdef OCRLITE_EXPORTS
#define OCRLITE_API	EXTERN_C __declspec(dllexport)
#else
#define OCRLITE_API	EXTERN_C __declspec(dllimport)
#endif
#endif

#define __OCRLITE_CALL __stdcall

#ifdef WIN32
#ifdef __CLIB__
#define _QM_OCR_API __declspec(dllexport)
#else
	//#define _QM_OCR_API __declspec(dllimport)
#define _QM_OCR_API 
#endif
#else
#define _QM_OCR_API
#endif

	typedef void* OCR_HANDLE;
	typedef char OCR_BOOL;

#ifndef NULL
#define NULL 0
#endif
#define TRUE 1
#define FALSE 0

	typedef struct __ocr_param {
		int padding;
		int maxSideLen;
		float boxScoreThresh;
		float boxThresh;
		float unClipRatio;
		int doAngle; // 1 means do
		int mostAngle; // 1 means true
	} OCR_PARAM;

	/*
	* nThreads should be the number of threads
	* gpuIndex: -1 means use cpu, 0 means use gpu0...
	*/
	_QM_OCR_API OCR_HANDLE
		OcrInit(const char* szDetModel, const char* szClsModel, const char* szRecModel, const char* szKeyPath, int nThreads,
			int gpuIndex);

	_QM_OCR_API OCR_BOOL
		OcrDetect(OCR_HANDLE handle, const char* imgPath, const char* imgName, OCR_PARAM* pParam);

	_QM_OCR_API char* OcrMem(char* imageData, int imageSize, float sim);

	_QM_OCR_API int OcrGetLen(OCR_HANDLE handle);

	_QM_OCR_API OCR_BOOL OcrGetResult(OCR_HANDLE handle, char* szBuf, int nLen);

	_QM_OCR_API void OcrDestroy(OCR_HANDLE handle);

};
#endif //__OCR_LITE_C_API_H__
#endif //__cplusplus
