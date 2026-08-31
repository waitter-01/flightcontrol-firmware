/****************************************************************
* FileName    : usranalyzer.h
* Desc        : 
* Author      : owzq
* Email       : owzq@163.com
* HomePage    : 
* Version     : 0.0.1
* Created     : 
* LastChange  : 2020-06-16 10:02:11
* History     : 
*****************************************************************/
#ifndef __USR_ANALYZER_H__
#define __USR_ANALYZER_H__

#include "analyzer.h"

#ifdef __cplusplus
extern "C"{
#endif

extern ANALYZER g_UsrAnalyzerDemo;
extern ANALYZER g_UsrAnalyzerJKB;
extern ANALYZER g_UsrAnalyzerUARTPL[16];
ANALYZER g_UsrAnalyzerIMU;    //IMU·ÖÎöÆ÷
extern ANALYZER g_UsrAnalyzerCAN;    //can·ÖÎöÆ÷

BOOL UsrAnalyzerDemoInit(void);
BOOL UsrAnalyzerJKBInit(void);
BOOL UsrAnalyzerUARTPLInit(void);
BOOL UsrAnalyzerIMUInit(void);
BOOL UsrAnalyzerCANInit(void);

#ifdef __cplusplus
}
#endif

#endif
