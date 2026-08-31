#include "usrglobal.h"

BOOL UsrGlobalInit(void)
{
	//内存池初始化
	if (FALSE == MemPoolInit())
		return FALSE;

//    spi_init();
    UsrCanInit();

	//uart初始化
	//UsrUartPS0Init();
	//UsrUartPS1Init();
	UsrAnalyzerDemoInit();
	UsrAnalyzerJKBInit();
	UsrAnalyzerUARTPLInit();
//	UsrAnalyzerIMUInit();
	UsrAnalyzerCANInit();
	UsrUartPLInit();


    VoltCurrSensorInit();	//ina237初始化
    LedInit();
    Tmp75_Init();			//主控板、输出板tmp75初始化

//    WatchDogInit();			//看门狗初始化

	return TRUE;
}
