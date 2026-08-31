#include "drvspiip.h"
#include "spi.h"

BOOL DrvSpiIPInit(void *arg)
{
	PSPI_PARAM pParam = (PSPI_PARAM)arg;
	return TRUE;
}

BOOL DrvSpiIPOpen(void *arg)
{	
	PSPI_PARAM pParam = (PSPI_PARAM)arg;
	return TRUE;
}

DWORD DrvSpiIPRead(void *arg, BYTE *pbyData, DWORD dwSize)
{
	DWORD dwCount = 0;
	DWORD dwRet = 0;

	PSPI_PARAM pParam = (PSPI_PARAM)arg;

//	for(dwCount = 0; dwCount < dwSize;dwCount++)
//	{
//		if(0 == xxx((DWORD)pParam->dwBaseAddr, pbyData + dwCount))
//		{
//			break;
//		}
//
//	}
	return dwCount;
	
}

DWORD DrvSpiIPWrite(void *arg, BYTE *pbyData,DWORD dwSize)
{
	DWORD i = 0;
	DWORD dwCount = 0;
	
	PSPI_PARAM pParam = (PSPI_PARAM)arg;

//	for(dwCount = 0;dwCount < dwSize;dwCount++)
//	{
//		if(0 == xxx((DWORD)pParam->dwBaseAddr, pbyData + dwCount))
//		{
//			break;
//		}
//	}
	return dwCount;
	
}

BOOL DrvSpiIPClose(void *arg)
{
	PSPI_PARAM pParam = (PSPI_PARAM)arg;

	return TRUE;
}



