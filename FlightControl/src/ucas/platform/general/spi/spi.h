#ifndef __SPI_H__
#define __SPI_H__

#include "utypes.h"

typedef struct stSPI_PARAM
{
	DWORD dwBaseAddr;
}SPI_PARAM, *PSPI_PARAM;

typedef struct stSPI{
	BOOL (* Init)(void *);
	BOOL (* Open)(void *);
	BOOL (* Close)(void *);
	DWORD (* Read)(void *,BYTE *,DWORD );
	DWORD (* Write)(void *,BYTE *,DWORD );
	struct stSPI *pHandle;
	SPI_PARAM Param;

}SPI,*PSPI;

BOOL SpiInit(PSPI pSpi);
BOOL SpiOpen(PSPI pSpi);
BOOL SpiClose(PSPI pSpi);
DWORD SpiRead(PSPI pSpi, BYTE *pbyData,DWORD dwSize);
DWORD SpiWrite(PSPI pSpi,BYTE *pbyData,DWORD dwSize);

#endif
