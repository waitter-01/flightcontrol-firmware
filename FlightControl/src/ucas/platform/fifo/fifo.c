/****************************************************************
* FileName    : fifo.c
* Desc        : 
* Author      : owzq(zhiqiang.wang)
* Email       : owzq@163.com
* HomePage    : http://owzq.blog.chinaunix.net/
* Version     : 0.0.1
* Created     : 
* LastChange  : 2020-05-13 21:29:08
* History     : 
*****************************************************************/

#include "fifo.h"
#include "mempool.h"

BOOL FifoInit(PFIFO pFifo, DWORD dwFifoSize)
{
	BYTE *pbyFifoData;

	if(NULL == pFifo)
	{
		return FALSE;
	}

	if(0 == dwFifoSize)
	{
		return FALSE;
	}

	pFifo->bEnabled = FALSE;

	if (dwFifoSize & (dwFifoSize - 1))
	{
		return FALSE;
	}

	if (sizeof(pFifo->dwIn) != sizeof(pFifo->dwOut))
	{
		return FALSE;
	}

	pbyFifoData = (BYTE*)MemPoolAlloc(dwFifoSize);

	if (NULL == pbyFifoData)
	{
		return FALSE;
	}

	pFifo->pbyData = pbyFifoData;
	pFifo->dwSize = dwFifoSize;
	pFifo->dwIn  = 0;
	pFifo->dwOut = 0;
	pFifo->OverflowU64 = 0;

	pFifo->bEnabled = TRUE;

	return TRUE;
}

void FifoClear(PFIFO pFifo)
{
	if (NULL == pFifo)
		return;

	pFifo->dwIn = 0;
	pFifo->dwOut = 0;
	pFifo->OverflowU64 = 0;
}

void FifoCancel(PFIFO pFifo)
{

}

DWORD FifoWrite(PFIFO pFifo, const BYTE *pbyData, DWORD dwLen)   
{   
	DWORD dwL;   
	DWORD dwLenOld;

	if (NULL == pFifo || NULL == pbyData || 0 == dwLen)
		return FALSE;

	if(TRUE != pFifo->bEnabled)
		return FALSE;

	dwLenOld = dwLen;

	dwLen = _MIN(dwLen, pFifo->dwSize - pFifo->dwIn + pFifo->dwOut);   
	SMP_MB();
	dwL = _MIN(dwLen, pFifo->dwSize - (pFifo->dwIn & (pFifo->dwSize - 1)));   
	memcpy(pFifo->pbyData + (pFifo->dwIn & (pFifo->dwSize - 1)), pbyData, dwL);   
	memcpy(pFifo->pbyData, pbyData + dwL, dwLen - dwL);   
	SMP_WMB();
	pFifo->dwIn += dwLen;   
	pFifo->OverflowU64 += (dwLenOld - dwLen);
	
	return dwLen;   
}  
  
DWORD FifoRead(PFIFO pFifo, BYTE *pbyData, DWORD dwLen)   
{   
	DWORD dwL;   

	if (NULL == pFifo || NULL == pbyData || 0 == dwLen)
		return FALSE;

	if(TRUE != pFifo->bEnabled)
		return FALSE;

	dwLen = _MIN(dwLen, pFifo->dwIn - pFifo->dwOut);     
	SMP_RMB();
	dwL = _MIN(dwLen, pFifo->dwSize - (pFifo->dwOut & (pFifo->dwSize - 1)));   
	memcpy(pbyData, pFifo->pbyData + (pFifo->dwOut & (pFifo->dwSize - 1)), dwL);   
	memcpy(pbyData + dwL, pFifo->pbyData, dwLen - dwL);   
	SMP_MB();
	pFifo->dwOut += dwLen;   

	return dwLen;   
}  
DWORD FifoGetFreeSize(const PFIFO pFifo)
{
	if (NULL == pFifo)
		return 0;

	if(TRUE != pFifo->bEnabled)
		return 0;

	return (pFifo->dwSize - pFifo->dwIn + pFifo->dwOut);
}

DWORD FifoGetDataSize(const PFIFO pFifo)
{
	if (NULL == pFifo)
		return 0;

	if(TRUE != pFifo->bEnabled)
		return 0;

	return (pFifo->dwIn - pFifo->dwOut);
}

UINT64 FifoGetOverflow(const PFIFO pFifo)
{
	if(NULL == pFifo)
		return 0;
	return pFifo->OverflowU64;
}

DWORD FifoWriteByte( PFIFO pFifo, const BYTE byValue)
{	
	DWORD dwIndex;

	if (NULL == pFifo)
		return 0;

	if(TRUE != pFifo->bEnabled)
		return 0;

	if ((pFifo->dwSize - pFifo->dwIn + pFifo->dwOut) == 0 )
	{
		pFifo->OverflowU64 += 1;
		return 0;
	}

	dwIndex = pFifo->dwIn & (pFifo->dwSize - 1);
 	pFifo->pbyData[ dwIndex ] = byValue;

  	pFifo->dwIn++;
    
	return 1;
}

DWORD FifoReadByte( PFIFO pFifo, BYTE *pbyValue)
{	
	DWORD dwIndex;

	if (NULL == pFifo || NULL == pbyValue)
		return 0;

	if(TRUE != pFifo->bEnabled)
		return 0;

 	if ( pFifo->dwIn - pFifo->dwOut == 0 )
    		return 0;

 	dwIndex = pFifo->dwOut & (pFifo->dwSize - 1);

 	*pbyValue = pFifo->pbyData[ dwIndex ];

 	pFifo->dwOut++;

   	return 1;
}


