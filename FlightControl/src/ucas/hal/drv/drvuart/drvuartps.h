#ifndef __DRV_UARTPS_H__
#define __DRV_UARTPS_H__

#include "utypes.h"

BOOL DrvUartPSInit(void *arg);
BOOL DrvUartPSOpen(void *arg);
BOOL DrvUartPSClose(void *arg);
DWORD DrvUartPSRead(void *arg, BYTE *pbyData, DWORD dwSize);
DWORD DrvUartPSWrite(void *arg, BYTE *pbyData, DWORD dwSize);

#endif
