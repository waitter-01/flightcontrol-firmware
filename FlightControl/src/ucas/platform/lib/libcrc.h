#ifndef __LIB_CRC_H__
#define __LIB_CRC_H__

#include "utypes.h"

DWORD LibCrc32(BYTE* pbyData, DWORD dwSize);

u16 CalCRC16(void *, int);

#endif
