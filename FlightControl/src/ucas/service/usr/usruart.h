#ifndef __USR_UART__
#define __USR_UART__

#include "uart.h"

extern UART g_UsrUartPS0;
extern UART g_UsrUartPS1;

#define USR_UART_PL_MAX (8)
extern UART g_UsrUartPL[USR_UART_PL_MAX];

BOOL UsrUartPS0Init(void);
BOOL UsrUartPS1Init(void);
BOOL UsrUartPLInit(void);

#endif
