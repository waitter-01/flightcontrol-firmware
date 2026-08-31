/*
 * usrtemp.c
 *
 *  Created on: 2020年10月28日
 *      Author: liyuan
 */
#include <stdio.h>
#include "usrtemp.h"
#include "xadcps.h"
//#include "ff.h"

static XAdcPs xadc_inst; 			//XADC 驱动实例
u32 temp_rawdata; 					//温度 原始数据
float temp; 						//温度
XAdcPs_Config *ConfigPtr; 			//XADC 配置指针


/**********初始化**********/

int temp_get_init()
{
	//初始化 XADC 驱动
	ConfigPtr = XAdcPs_LookupConfig(XADC_DEVICE_ID);
	XAdcPs_CfgInitialize(&xadc_inst, ConfigPtr, ConfigPtr->BaseAddress);

	//设置 XADC 操作模式为“默认安全模式”
	XAdcPs_SetSequencerMode(&xadc_inst, XADCPS_SEQ_MODE_SAFE);

	return 0;
}


/*
	功能：获取内核温度
	输入：none
	返回：内核温度模拟量
 */
float System_GetCoreTemperature(void)
{
	//temp_get_init();
	//获取原始温度传感器数据
	temp_rawdata = XAdcPs_GetAdcData(&xadc_inst, XADCPS_CH_TEMP);
	//转换成温度信息
	temp = XAdcPs_RawToTemperature(temp_rawdata);
	return temp;
}


/**********提取温度并打印**********/
void CoreTempTest(void)
{
	float temp;
	temp = System_GetCoreTemperature();

	printf(" CoreTemperature = %.4f°C\n",temp);
}










