#ifndef __DRV_UARTPL_H__
#define __DRV_UARTPL_H__

#include "utypes.h"

BOOL DrvUartPLInit(void *arg);
BOOL DrvUartPLOpen(void *arg);
BOOL DrvUartPLClose(void *arg);
DWORD DrvUartPLRead(void *arg, BYTE *pbyData, DWORD dwSize);
DWORD DrvUartPLWrite(void *arg, const BYTE *pbyData, DWORD dwSize);
DWORD DrvUartPLIoctl(void *arg);

#endif
