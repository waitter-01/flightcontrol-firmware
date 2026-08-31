
#include "mempool.h"

#define MEMPOOL_MEMORY_SIZE (32*1024*1024)
#define MEMPOOL_MEMORY_ALIGN (1024)

static BYTE g_MemPoolBuffer[MEMPOOL_MEMORY_SIZE];

struct MEMPOOL_QUEUE
{
	struct MEMPOOL_QUEUE *pPrev, *pNext;
	DWORD dwSize;
};

static struct MEMPOOL_QUEUE g_MemPoolHead;		/* 内存链表第一个结构 */
static BYTE* g_pbyMemPoolBase;    				/* 内存链表首地址指针 */
static DWORD g_dwMemPoolTotalSize;				/* 可用内存空间大小 */
static DWORD g_dwMemPoolAlign;					/* 内存对齐的字节数 */

static BYTE* g_pbyMemBoolBottom;				/* 内存链表首地址 */
static BYTE* g_pbyMemPoolTop;					/* 内存链表尾地址 */
static DWORD g_dwMemPoolFreeSize;				/* 内存可用空间大小 */

/*=============================================================================
* 函数名称 :  HalMemPoolInit
* 函数功能 : 本地内存池初始化
* 入口参数 :
* 出口参数 : 内存地址:ppbyBase, 内存大小:pdwSize，堆存对齐字节：pdwAlign
* 其他说明 :
=============================================================================*/
static void HalMemPoolInit( BYTE** ppbyBase, DWORD* pdwSize, DWORD* pdwAlign )
{
	*ppbyBase = g_MemPoolBuffer;
	*pdwSize = MEMPOOL_MEMORY_SIZE;
	*pdwAlign = MEMPOOL_MEMORY_ALIGN;
}
/*=============================================================================
* 函数名称 :  MemPoolRoundup
* 函数功能 : 保证申请的内存大小是g_dwMemPoolAlign的倍数向上取整
* 入口参数 : 申请的内存大小：ulSize
* 出口参数 : 申请成功的内存大小：ulSize
* 其他说明 :
=============================================================================*/
//Use ULONG, not DWORD
static ULONG MemPoolRoundup( ULONG ulSize )
{
	ulSize += sizeof(struct MEMPOOL_QUEUE);
	ulSize = (ulSize + (ULONG)g_dwMemPoolAlign - 1) & -(ULONG)g_dwMemPoolAlign;
	return ulSize;
}
/*=============================================================================
* 函数名称 :  MemPoolInit
* 函数功能 : 本地内存池初始化
* 入口参数 : 无
* 出口参数 : 内存池初始化状态 成功--失败
* 其他说明 :
=============================================================================*/
BOOL MemPoolInit( void )
{
	struct MEMPOOL_QUEUE* hdq;
	struct MEMPOOL_QUEUE* dq;

	HalMemPoolInit( &g_pbyMemPoolBase, &g_dwMemPoolTotalSize, &g_dwMemPoolAlign );

	if ( g_dwMemPoolAlign < (DWORD)sizeof(struct MEMPOOL_QUEUE) )
		return FALSE;

	EnterCritical();

	g_pbyMemBoolBottom = (BYTE *)MemPoolRoundup( (ULONG)g_pbyMemPoolBase );
	g_pbyMemPoolTop = (BYTE *)(((ULONG)g_pbyMemPoolBase + g_dwMemPoolTotalSize) & -g_dwMemPoolAlign);

	hdq = &g_MemPoolHead;
	dq = (struct MEMPOOL_QUEUE *)g_pbyMemBoolBottom;

	hdq->pPrev = hdq->pNext = dq;
	hdq->dwSize = 0;
	dq->pPrev = dq->pNext = hdq;

	dq->dwSize = (ULONG)g_pbyMemPoolTop - (ULONG)g_pbyMemBoolBottom;
	g_dwMemPoolFreeSize = dq->dwSize;

	LeaveCritical();
	return TRUE;
}
/*=============================================================================
* 函数名称 :  MemPoolAlloc
* 函数功能 : 申请内存
* 入口参数 : 申请内存的大小：dwSize
* 出口参数 : 申请成功后的地址
* 其他说明 :
=============================================================================*/
void* MemPoolAlloc( DWORD dwSize )
{
    struct MEMPOOL_QUEUE *dq = &g_MemPoolHead;
    BYTE *pbyAlloced;

    EnterCritical();
    dwSize = MemPoolRoundup( dwSize );

    do {
			dq = dq->pNext;
			if ( dq->dwSize == 0 )
			{
				LeaveCritical();
					return NULL;
			}
		} while ( dq->dwSize < dwSize );

    if ( dwSize == dq->dwSize )
    {
        dq->pPrev->pNext = dq->pNext;
        dq->pNext->pPrev = dq->pPrev;
        pbyAlloced = (BYTE *)dq;
    }
    else
    {
        // allocate portion of memory from end of block        
        dq->dwSize -= dwSize;

        pbyAlloced = (BYTE *)dq + dq->dwSize;
    }

    dq = (struct MEMPOOL_QUEUE *)pbyAlloced;
    dq->dwSize = dwSize; 
    dq->pPrev = (struct MEMPOOL_QUEUE *)0xd530d53; // magic number
    dq->pNext = dq->pPrev;

    g_dwMemPoolFreeSize -= dwSize;
	
    LeaveCritical();

    return pbyAlloced + sizeof(struct MEMPOOL_QUEUE);
}
/*=============================================================================
* 函数名称 :  MemPoolFree
* 函数功能 : 释放内存
* 入口参数 : 要释放的内存的地址：p
* 出口参数 : 无
* 其他说明 :
=============================================================================*/
void MemPoolFree( void* p )
{
    struct MEMPOOL_QUEUE *hdq;
    struct MEMPOOL_QUEUE *dq;
    struct MEMPOOL_QUEUE *idq;

    if ( !(g_pbyMemBoolBottom <= (BYTE*)p && (BYTE*)p <= g_pbyMemPoolTop) )
        return;
    
    hdq = &g_MemPoolHead; 
    dq = (struct MEMPOOL_QUEUE *)((BYTE*)p - sizeof(struct MEMPOOL_QUEUE));

    // check magic number in block to be freed
    if ( dq->pNext != dq->pPrev || dq->pNext != (struct MEMPOOL_QUEUE *)0xd530d53 )
        return;

    for ( idq = hdq->pNext; idq != hdq; idq = idq->pNext )
    {
        if ( idq->pNext > dq )
            break;
    }

    EnterCritical();

    g_dwMemPoolFreeSize += dq->dwSize;

    if ( idq != hdq )
	{
        dq->pPrev = idq;
        dq->pNext = idq->pNext;
        idq->pNext = dq;
        dq->pNext->pPrev = dq;
    }
	else
	{
        dq->pNext = idq;
        dq->pPrev = idq->pPrev;
        idq->pPrev = dq;
        dq->pPrev->pNext = dq;
    }
    
	// Now do coalescing
    if ( (BYTE *)dq + dq->dwSize == (BYTE *)dq->pNext )
	{
        dq->dwSize += dq->pNext->dwSize;
        dq->pNext = dq->pNext->pNext;
        dq->pNext->pPrev = dq;
    }

    if ( (BYTE *)dq->pPrev + dq->pPrev->dwSize == (BYTE *)dq )
	{
        dq->pPrev->dwSize += dq->dwSize;
        dq->pPrev->pNext = dq->pNext;
        dq->pNext->pPrev = dq->pPrev;
        dq = dq->pPrev;
    }

	LeaveCritical();
}
/*=============================================================================
* 函数名称 :  MemPoolGetMemSize
* 函数功能 : 获取申请的内存块大小
* 入口参数 : 内存块地址：ptr
* 出口参数 : 内存块大小
* 其他说明 :
=============================================================================*/
static DWORD MemPoolGetMemSize( void* ptr )
{
	struct MEMPOOL_QUEUE *dq;

	if ( !(g_pbyMemBoolBottom <= (BYTE*)ptr && (BYTE*)ptr <= g_pbyMemPoolTop) )
        	return 0;
 
	dq = (struct MEMPOOL_QUEUE *)((BYTE*)ptr - sizeof(struct MEMPOOL_QUEUE));

 	// check magic number in block to be freed
  	if ( dq->pNext != dq->pPrev || dq->pNext != (struct MEMPOOL_QUEUE *)0xd530d53 )
 		return 0;

 	return dq->dwSize;    
}
/*=============================================================================
* 函数名称 :  MemPoolGetTotalSize
* 函数功能 : 获取内存池总大小
* 入口参数 : 无
* 出口参数 : 内存池大小
* 其他说明 :
=============================================================================*/
static DWORD MemPoolGetTotalSize( void )
{
	return g_dwMemPoolTotalSize;
}
/*=============================================================================
* 函数名称 :  MemPoolGetFreeSize
* 函数功能 : 获取内存池剩余有效空间
* 入口参数 : 无
* 出口参数 : 内存块有效剩余空间大小
* 其他说明 :
=============================================================================*/
DWORD MemPoolGetFreeSize( void )
{
	return g_dwMemPoolFreeSize;
}

