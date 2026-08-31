
#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#include "utypes.h"

#ifdef __cplusplus
extern "C"{
#endif

BOOL MemPoolInit( void );
void* MemPoolAlloc( DWORD dwSize );
void MemPoolFree( void* pPtr );
DWORD MemPoolGetFreeSize( void );
 
#ifdef __cplusplus
}
#endif


#endif
