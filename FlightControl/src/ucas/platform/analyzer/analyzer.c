/****************************************************************
* FileName    : analyzer.c
* Desc        : 
* Author      : owzq
* Email       : owzq@163.com
* HomePage    : https://github.com/owzq
* Version     : 0.0.1
* Created     : 
* LastChange  : 2020-06-16 10:36:04
* History     : 
*****************************************************************/

#include "analyzer.h"

static WORD AnalyzerGetLittleWordFromBytes(const BYTE *pbyData) 
{
	return ((pbyData[1]<<8)|pbyData[0]);
}

static WORD AnalyzerGetBigWordFromBytes(const BYTE *pbyData)
{
	return ((pbyData[0]<<8) | pbyData[1]);
}

static DWORD AnalyzerGetLittleDwordFromBytes(const BYTE *pbyData) 
{
	return ((pbyData[3]<<24) | (pbyData[2]<<16) | (pbyData[1]<<8) | pbyData[0]);
}

static DWORD AnalyzerGetBigDwordFromBytes(const BYTE *pbyData) 
{
	return ((pbyData[0]<<24) | (pbyData[1]<<16) | (pbyData[2]<<8) | pbyData[3]);
}

static BOOL AnalyzerFindWord(const BYTE *pbyData, const DWORD dwSize, const WORD wKey, DWORD *pPos)
{
	DWORD dwI;
	
	if(NULL == pbyData)
		return FALSE;

	if(dwSize < 2)
		return FALSE;

	for(dwI = 0; dwI < dwSize - 1; dwI++)
	{
#ifdef __BIG_ENDIAN__		
		if(AnalyzerGetLittleWordFromBytes(pbyData+dwI) == wKey)
#else
		if(AnalyzerGetBigWordFromBytes(pbyData+dwI) == wKey)
#endif
		{
			*pPos = dwI;

			return TRUE;
		}
	} 
	return FALSE;
}

static BOOL AnalyzerFindDword(const BYTE *pbyData, const DWORD dwSize, const DWORD dwKey, DWORD *pPos)
{
	DWORD dwI;

	if(NULL == pbyData)
		return FALSE;

	if (dwSize < 4)
		return FALSE;

	for(dwI = 0;dwI < dwSize - 1; dwI++)
	{
#ifdef __BIG_ENDIAN__
		if(AnalyzerGetLittleDwordFromBytes(pbyData + dwI) == dwKey)
#else
		if(AnalyzerGetBigDwordFromBytes(pbyData + dwI) == dwKey)
#endif			
		{
			*pPos = dwI;
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL AnalyzerFindKey(const BYTE *pbyData, const DWORD dwSize, const DWORD dwKey, const DWORD dwKeySize, DWORD *pPos)
{
	BOOL bRet= FALSE;
	
	if(NULL == pbyData)
		return FALSE;

	if(dwKeySize == 2)
	{
		bRet = AnalyzerFindWord(pbyData, dwSize, (WORD)(dwKey&0xffff), pPos);
	}
	else if(dwKeySize == 4)
	{
		bRet = AnalyzerFindDword(pbyData, dwSize, dwKey, pPos);
	}

	return bRet;
}

DWORD AnalyzerWrite(PANALYZER pZer, const BYTE *pbyData, DWORD dwSize)
{
	if(NULL == pZer)
		return 0;

	if(TRUE != pZer->bEnabled)
		return 0;

	return FifoWrite(&pZer->Fifo, pbyData, dwSize);
}

DWORD AnalyzerWriteByte(PANALYZER pZer, const BYTE byValue)
{
	if(NULL == pZer)
		return 0;

	if(TRUE != pZer->bEnabled)
		return 0;

	return FifoWriteByte(&pZer->Fifo, byValue);
}

DWORD AnalyzerRead(PANALYZER pZer, BYTE *pbyData, DWORD dwSize)
{
	if(NULL == pZer)
		return 0;

	if(TRUE != pZer->bEnabled)
		return 0;

	return FifoRead(&pZer->Fifo, pbyData, dwSize);
}


DWORD AnalyzerReadByte(PANALYZER pZer, BYTE *pbyValue)
{
	if(NULL == pZer)
		return 0;

	if(TRUE != pZer->bEnabled)
		return 0;

	return FifoReadByte(&pZer->Fifo, pbyValue);
}

DWORD AnalyzerGetFreeSize(const PANALYZER pZer)
{
	if(NULL == pZer)
		return 0;

	if(TRUE != pZer->bEnabled)
		return 0;

	return FifoGetFreeSize(&pZer->Fifo);
}

DWORD AnalyzerGetDataSize(const PANALYZER pZer)
{
	if(NULL == pZer)
		return 0;

	if(TRUE != pZer->bEnabled)
		return 0;

	return FifoGetDataSize(&pZer->Fifo);
}

UINT64 AnalyzerGetOverflow(const PANALYZER pZer)
{
	if(NULL == pZer)
		return 0;

	if(TRUE != pZer->bEnabled)
		return 0;

	return FifoGetOverflow(&pZer->Fifo);
}

BOOL AnalyzerInit(PANALYZER pZer, DWORD dwSize)
{
	BOOL bRet;

	if (NULL == pZer)
	{
		return FALSE;
	}

	if(0 == dwSize)
	{
		return FALSE;
	}

	pZer->bEnabled = FALSE;

	if(NULL == pZer->GetLengthFunc)
	{
		return FALSE;
	}
	if(NULL == pZer->VerifyFunc)
	{
		return FALSE;
	}

	if (pZer->dwHeaderSize > pZer->dwPackSizeMin)
	{
		return FALSE;
	}

	if(pZer->dwPackSizeMin > pZer->dwPackSizeMax)
	{
		return FALSE;
	}

	if(pZer->dwKeySize != 2 && pZer->dwKeySize != 4)
	{
		return FALSE;
	}
	bRet = FifoInit(&pZer->Fifo, dwSize);
	if(FALSE == bRet)
	{
		return FALSE;
	}
	
	pZer->dwErrCnts  = 0;
	pZer->dwErrTimeoutCnts  = 0;
	pZer->dwMs = 0;

	pZer->bEnabled = TRUE;

	return TRUE;
}

void AnalyzerClear(PANALYZER pZer)
{
	if (NULL == pZer)
	{
		return;
	}

	if (FALSE == pZer->bEnabled)
	{
		return;
	}

	FifoClear(&pZer->Fifo);
	
	pZer->dwErrCnts  = 0;
	pZer->dwErrTimeoutCnts  = 0;
	pZer->dwMs = 0;
}


static BOOL AnalyzerGetPacketDirect(PANALYZER pZer, BYTE *pPackData, DWORD *pdwPackSize)   
{   
	BOOL bRet;
	DWORD dwM;
	DWORD dwL, dwLen;   
	DWORD dwLA, dwLB;
	DWORD dwLenA, dwLenB;
	BYTE  *pPA, *pPB;
	DWORD dwKey, dwKeySize;
	DWORD dwPackSize;
	PFIFO pFifo;

	if(NULL == pZer)
		return 0;
	if(TRUE != pZer->bEnabled)
		return 0;
	if (NULL == pPackData)
		return FALSE;
	pFifo = &pZer->Fifo;
	dwKey = pZer->dwKey;
	dwKeySize = pZer->dwKeySize;
	dwLen = pZer->Fifo.dwIn - pZer->Fifo.dwOut;
	if (dwLen < dwKeySize)
		return FALSE;
	if(pZer->dwMs > 0 && pZer->dwTimeout != 0)
	{
		if(GetMs() > (pZer->dwMs + pZer->dwTimeout)) 
		{
			pFifo->dwOut += 1;
			pZer->dwErrTimeoutCnts += 1;
			pZer->dwMs = 0;
			return FALSE;
		}
	}
	SMP_RMB();
	dwL = _MIN(dwLen, pFifo->dwSize - (pFifo->dwOut & (pFifo->dwSize - 1)));  
	dwLenA = dwL;
	dwLenB = dwLen - dwL;
	pPA = pFifo->pbyData + (pFifo->dwOut & (pFifo->dwSize - 1));
	pPB = pFifo->pbyData;
	bRet = AnalyzerFindKey(pPA, dwLenA, dwKey, dwKeySize, &dwM);
	if(FALSE == bRet)
	{
		dwLA = _MIN(dwLenA, (dwKeySize - 1));
		pFifo->dwOut += (dwLenA - dwLA);
		if(dwLenB == 0) 
		{
			return FALSE;
		}
		dwLB = _MIN(dwLenB, (dwKeySize - 1));
		if((dwLA + dwLB) < dwKeySize)
			return FALSE;
		memcpy(pPackData, pPA + dwLenA - dwLA, dwLA);
		memcpy(pPackData + dwLA, pPB, dwLB);
		dwLen = dwLA + dwLB;
		bRet = AnalyzerFindKey(pPackData, dwLen, dwKey, dwKeySize, &dwM);
		if(FALSE == bRet)
		{	
			dwL = _MIN(dwLen, (dwKeySize - 1));
			SMP_MB();
			pFifo->dwOut += (dwLen - dwL);
			return FALSE;
		}
		else
		{
			SMP_MB();			
			pFifo->dwOut += dwM;
			if (pZer->dwMs == 0 && pZer->dwTimeout != 0)
				pZer->dwMs = GetMs(); 
		}
	}
	else
	{
		SMP_MB();
		pFifo->dwOut += dwM;
		if (pZer->dwMs == 0 && pZer->dwTimeout != 0)
			pZer->dwMs = GetMs(); 
	}
	dwLen = pZer->Fifo.dwIn - pZer->Fifo.dwOut;
	if(dwLen < pZer->dwHeaderSize)
		return FALSE;
	dwLen = pZer->dwHeaderSize;
	SMP_RMB();
	dwL = _MIN(dwLen, pFifo->dwSize - (pFifo->dwOut & (pFifo->dwSize - 1)));  
	memcpy(pPackData, pFifo->pbyData + (pFifo->dwOut & (pFifo->dwSize - 1)), dwL);
	memcpy(pPackData + dwL, pFifo->pbyData, dwLen - dwL);
	dwPackSize = pZer->GetLengthFunc(pPackData, dwLen); 
	if(dwPackSize > pZer->dwPackSizeMax)
	{
		SMP_MB();
		pFifo->dwOut += 1;
		pZer->dwErrCnts += 1;
		pZer->dwMs = 0;
		return FALSE;
	}
	dwLen = pZer->Fifo.dwIn - pZer->Fifo.dwOut;
	if(dwLen < dwPackSize)
		return FALSE;
	dwLen = dwPackSize;
	SMP_RMB();
	dwL = _MIN(dwLen, pFifo->dwSize - (pFifo->dwOut & (pFifo->dwSize - 1)));  
	memcpy(pPackData, pFifo->pbyData + (pFifo->dwOut & (pFifo->dwSize - 1)), dwL);
	memcpy(pPackData + dwL, pFifo->pbyData, dwLen - dwL);
	bRet = pZer->VerifyFunc(pPackData, dwLen);
	if(FALSE == bRet)
	{	
		SMP_MB();
		pFifo->dwOut += 1;
		pZer->dwErrCnts += 1;
		pZer->dwMs = 0;
		return FALSE;
	}
	*pdwPackSize = dwPackSize;
	SMP_MB();
	pFifo->dwOut += dwPackSize;
	pZer->dwMs = 0;

	return TRUE;   
}  

BOOL AnalyzerParse(PANALYZER pZer,BYTE *pPackData, DWORD *pdwPackSize)
{
	return AnalyzerGetPacketDirect(pZer, pPackData, pdwPackSize);
}

