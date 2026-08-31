#ifndef __DRVSPIIP_H__
#define __DRVSPIIP_H__

#include "utypes.h"

BOOL DrvSpiIPInit(void *arg);
BOOL DrvSpiIPOpen(void *arg);
DWORD DrvSpiIPRead(void *arg, BYTE *pbyData, DWORD dwSize);
DWORD DrvSpiIPWrite(void *arg, BYTE *pbyData,DWORD dwSize);
BOOL DrvSpiIPClose(void *arg);

#endif

