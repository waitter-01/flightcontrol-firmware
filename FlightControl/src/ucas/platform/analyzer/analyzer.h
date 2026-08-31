/****************************************************************
* FileName    : analyzer.h
* Desc        : 
* Author      : owzq
* Email       : owzq@163.com
* HomePage    : https://github.com/owzq
* Version     : 0.0.1
* Created     : 
* LastChange  : 2020-06-16 10:24:48
* History     : 
*****************************************************************/
#ifndef __ANALYZER_H__
#define __ANALYZER_H__

#include "utypes.h"
#include "fifo.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct stANALYZER
{
	DWORD dwKey;
	DWORD dwKeySize;
	DWORD dwHeaderSize;
	DWORD dwPackSizeMin;
	DWORD dwPackSizeMax;
	DWORD dwTimeout;
	DWORD (*GetLengthFunc)( const BYTE *, const DWORD);
	BOOL (*VerifyFunc)( const BYTE *, const DWORD);
	volatile DWORD dwErrCnts;
	volatile DWORD dwErrTimeoutCnts;
	volatile DWORD dwMs;
	volatile BOOL bEnabled;
	FIFO Fifo;
}ANALYZER, *PANALYZER;

BOOL AnalyzerInit(PANALYZER pZer, DWORD dwSize);
void AnalyzerClear(PANALYZER pZer);
DWORD AnalyzerWrite(PANALYZER pZer, const BYTE *pbyData, DWORD dwSize);
DWORD AnalyzerWriteByte(PANALYZER pZer, const BYTE byValue);
DWORD AnalyzerRead(PANALYZER pZer, BYTE *pbyData, DWORD dwSize);
DWORD AnalyzerReadByte(PANALYZER pZer, BYTE *pbyValue);
DWORD AnalyzerGetFreeSize(const PANALYZER pZer);
DWORD AnalyzerGetDataSize(const PANALYZER pZer);
UINT64 AnalyzerGetOverflow(const PANALYZER pZer);
BOOL AnalyzerParse(PANALYZER pZer,BYTE *pPackData, DWORD *pdwPackSize);

#ifdef __cplusplus
}
#endif

#endif
