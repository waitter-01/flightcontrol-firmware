/****************************************************************
* FileName    : usranalyzer.c
* Desc        : 
* Author      : owzq
* Email       : owzq@163.com
* HomePage    : 
* Version     : 0.0.1
* Created     : 
* LastChange  : 2020-06-16 06:16:02
* History     : 
*****************************************************************/

#include "usranalyzer.h"
#include "libcrc.h"

ANALYZER g_UsrAnalyzerDemo;		//串口分析器
ANALYZER g_UsrAnalyzerJKB;    //输出板分析器
ANALYZER g_UsrAnalyzerUARTPL[16];    //串口分析器
ANALYZER g_UsrAnalyzerIMU;    //IMU分析器
ANALYZER g_UsrAnalyzerCAN;    //can分析器


static DWORD UsrAnalyzerDemoGetLength(const BYTE *pbyData, const DWORD dwSize)
{
	DWORD dwLen;

	dwLen = pbyData[2];

	dwLen += 8;

	return dwLen;
}
static BOOL UsrAnalyzerDemoVerity(const BYTE *pbyData, const DWORD dwSize)
{
	u16 wCRC = 0;
	
	if (dwSize < sizeof(wCRC))
		return FALSE;

	wCRC = CalCRC16(pbyData + 2, dwSize - 4);
	if(wCRC == ((pbyData[dwSize - 1]<<8)|pbyData[dwSize - 2]))
		return TRUE;

	return FALSE;

}

static DWORD UsrAnalyzerJKBGetLength(const BYTE *pbyData, const DWORD dwSize)
{
	DWORD dwLen;

	dwLen = pbyData[2];

	dwLen += 8;

	return dwLen;
}
static BOOL UsrAnalyzerJKBVerity(const BYTE *pbyData, const DWORD dwSize)
{
	u16 wCRC = 0;

	if (dwSize < sizeof(wCRC))
		return FALSE;

	wCRC = CalCRC16(pbyData + 2, dwSize - 4);
	if(wCRC == ((pbyData[dwSize - 1]<<8)|pbyData[dwSize - 2]))
		return TRUE;

	return FALSE;

}

static DWORD UsrAnalyzerUARTPLGetLength(const BYTE *pbyData, const DWORD dwSize)
{
	DWORD dwLen;

	dwLen = pbyData[2];

	dwLen += 8;

	return 16;
}
static BOOL UsrAnalyzerUARTPLVerity(const BYTE *pbyData, const DWORD dwSize)
{
	u16 wCRC = 0;

	if (dwSize < sizeof(wCRC))
		return FALSE;

	wCRC = CalCRC16(pbyData + 2, dwSize - 4);
	if(wCRC == ((pbyData[dwSize - 1]<<8)|pbyData[dwSize - 2]))
		return TRUE;

//	return FALSE;
	return TRUE;

}

static DWORD UsrAnalyzerIMUGetLength(const BYTE *pbyData, const DWORD dwSize)
{
	DWORD dwLen;

	dwLen = pbyData[2];

	dwLen += 8;

	return 36;
}
static BOOL UsrAnalyzerIMUVerity(const BYTE *pbyData, const DWORD dwSize)
{
//	u16 wCRC = 0;
//
//	if (dwSize < sizeof(wCRC))
//		return FALSE;
//
//	wCRC = CalCRC16(pbyData + 2, dwSize - 4);
//	if(wCRC == (pbyData[dwSize - 2]<<8)|pbyData[dwSize - 1])
//		return TRUE;

	return TRUE;

}


static DWORD UsrAnalyzerCANGetLength(const BYTE *pbyData, const DWORD dwSize)
{
	DWORD dwLen;

	dwLen = pbyData[4] + 3;

	return dwLen;
}
static BOOL UsrAnalyzerCANVerity(const BYTE *pbyData, const DWORD dwSize)
{
	u16 wCRC = 0;

	if (dwSize < sizeof(wCRC))
		return FALSE;

	wCRC = CalCRC16(pbyData, dwSize - 2);
	if(wCRC == ((pbyData[dwSize - 1]<<8)|pbyData[dwSize - 2]))
		return TRUE;

	return TRUE;

}


BOOL UsrAnalyzerDemoInit(void)
{
	PANALYZER pZer = &g_UsrAnalyzerDemo;
	
	pZer->dwKey = 0x55AA;
	pZer->dwKeySize = 2;
	pZer->dwHeaderSize = 6;
	pZer->dwPackSizeMin = 8;
	pZer->dwPackSizeMax = 260;
	pZer->dwTimeout = 5*1000;
	pZer->GetLengthFunc = UsrAnalyzerDemoGetLength;
	pZer->VerifyFunc = UsrAnalyzerDemoVerity;

	return AnalyzerInit(pZer, 1*1024*1024);
}



BOOL UsrAnalyzerJKBInit(void)
{
	PANALYZER pZer = &g_UsrAnalyzerJKB;

	pZer->dwKey = 0x55AA;
	pZer->dwKeySize = 2;
	pZer->dwHeaderSize = 6;
	pZer->dwPackSizeMin = 189;
	pZer->dwPackSizeMax = 189;
	pZer->dwTimeout = 5*1000;
	pZer->GetLengthFunc = UsrAnalyzerJKBGetLength;
	pZer->VerifyFunc = UsrAnalyzerJKBVerity;

	return AnalyzerInit(pZer, 1*1024*1024);
}

BOOL UsrAnalyzerUARTPLInit(void)
{
	PANALYZER pZer;
	for(int i = 0;i < 16; i++)
	{
		pZer = &g_UsrAnalyzerUARTPL[i];

		pZer->dwKey = 0x55AA;
		pZer->dwKeySize = 2;
		pZer->dwHeaderSize = 6;
		pZer->dwPackSizeMin = 8;
		pZer->dwPackSizeMax = 260;
		pZer->dwTimeout = 5*1000;
		pZer->GetLengthFunc = UsrAnalyzerUARTPLGetLength;
		pZer->VerifyFunc = UsrAnalyzerUARTPLVerity;

		AnalyzerInit(pZer, 1*1024*1024);
	}
	return 0;
}

BOOL UsrAnalyzerIMUInit(void)
{
	PANALYZER pZer = &g_UsrAnalyzerIMU;

	pZer->dwKey = 0x4068;
	pZer->dwKeySize = 2;
	pZer->dwHeaderSize = 6;
	pZer->dwPackSizeMin = 36;
	pZer->dwPackSizeMax = 36;
	pZer->dwTimeout = 5*1000;
	pZer->GetLengthFunc = UsrAnalyzerIMUGetLength;
	pZer->VerifyFunc = UsrAnalyzerIMUVerity;

	return AnalyzerInit(pZer, 1*1024*1024);
}

BOOL UsrAnalyzerCANInit(void)
{
	PANALYZER pZer = &g_UsrAnalyzerCAN;

	pZer->dwKey = 0xEB90;
	pZer->dwKeySize = 2;
	pZer->dwHeaderSize = 6;
	pZer->dwPackSizeMin = 64;
	pZer->dwPackSizeMax = 64;
	pZer->dwTimeout = 5*1000;
	pZer->GetLengthFunc = UsrAnalyzerCANGetLength;
	pZer->VerifyFunc = UsrAnalyzerCANVerity;

	return AnalyzerInit(pZer, 1*1024*1024);
}
