#include "spi.h"

BOOL SpiInit(PSPI pSpi)
{
	return pSpi->Init(&pSpi->Param);
}

BOOL SpiOpen(PSPI pSpi)
{
	pSpi->Open(&pSpi->Param);
}

DWORD SpiRead(PSPI pSpi, BYTE *pbyData,DWORD dwSize)
{
	return pSpi->Read(&pSpi->Param, pbyData, dwSize);
}

DWORD SpiWrite(PSPI pSpi,BYTE *pbyData,DWORD dwSize)
{
	return pSpi->Write(&pSpi->Param, pbyData, dwSize);
}

BOOL SpiClose(PSPI pSpi)
{
	return pSpi->Close(&pSpi->Param);
}




