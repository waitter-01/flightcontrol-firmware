/****************************************************************
* FileName    : utypes.h
* Desc        : 
* Author      : owzq
* Email       : owzq@163.com
* HomePage    : https://github.com/owzq
* Version     : 0.0.1
* Created     : 2020-06-16 16:25:45
* LastChange  : 2020-06-16 16:25:45
* History     : 
*****************************************************************/

#ifndef __UTYPES_H__
#define __UTYPES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned char BOOL;
typedef unsigned long long UINT64;

typedef unsigned short u16;
typedef long unsigned int u32;
typedef unsigned long long u64;
typedef unsigned char u8;

//32bits: ULOND = DWORD, 64bits : ULONG = UINT64
//When getting th pointer, pay attention to using ULONG
typedef unsigned long ULONG; /*vxworks 5.5 系统有定义*/

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FASLE
#define FALSE (0)
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef _MIN
#define _MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef _MAX
#define _MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

DWORD GetMs(void);
void EnterCritical(void);
void LeaveCritical(void);
void SMP_MB(void);
void SMP_WMB(void);
void SMP_RMB(void);

#ifdef __cplusplus
}
#endif


#endif
