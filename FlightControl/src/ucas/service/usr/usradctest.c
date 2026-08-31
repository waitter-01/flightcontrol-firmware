/*
 * adctest.c
 *
 *  Created on: 2020年9月24日
 *      Author: liyuan
 */
#include "interface.h"
#include "utypes.h"
#include "usradctest.h"

#define COLLECT_TIMES    100

void UsrADCTest()
{
	char c;
    int i;
    u16 ADC_Value[16] = {0};           			 //ADC采集原始值
    float Voltage_Value[16] = {0};		         //ADC采集电压值
    float Voltage_Value_array[16][100] = {0};     //每路采集100个数数组
    float VoltageAverage[16] = {0};               //每路电压平均值

    float VoltageMin[16] = {0};                   //100次电压最小值
	float VoltageMax[16] = {0};   				 //100次电压最大值


	while(1)
	{
	    float VoltageSum[16] = {0};                   //100次电压和
		printf("----------- ADC Test -------------\n");
		printf("1. get ADC value.\n");
		printf("2. get voltage value.\n");
		printf("q. Quit.\n");
		printf("Please enter:\n");

		scanf(" %c",&c);

		if(c == '1')
		{
	    	ADC_GetValue(ADC_Value);

			for(i = 0;i < 16; i++)
			{
				printf("ADC_Value[%d] = 0x%x\n",i,ADC_Value[i]);
			}
		}

		if(c == '2')
		{
//	    	取得100次测量电压
	    	for(i =0;i < COLLECT_TIMES;i++)
	    	{
	    		ADC_GetVoltage(Voltage_Value);
	    		Voltage_Value_array[ 0][i] = Voltage_Value[0];
	    		Voltage_Value_array[ 1][i] = Voltage_Value[1];
	    		Voltage_Value_array[ 2][i] = Voltage_Value[2];
	    		Voltage_Value_array[ 3][i] = Voltage_Value[3];
	    		Voltage_Value_array[ 4][i] = Voltage_Value[4];
	    		Voltage_Value_array[ 5][i] = Voltage_Value[5];
	    		Voltage_Value_array[ 6][i] = Voltage_Value[6];
	    		Voltage_Value_array[ 7][i] = Voltage_Value[7];
	    		Voltage_Value_array[ 8][i] = Voltage_Value[8];
				Voltage_Value_array[ 9][i] = Voltage_Value[9];
				Voltage_Value_array[10][i] = Voltage_Value[10];
				Voltage_Value_array[11][i] = Voltage_Value[11];
				Voltage_Value_array[12][i] = Voltage_Value[12];
				Voltage_Value_array[13][i] = Voltage_Value[13];
				Voltage_Value_array[14][i] = Voltage_Value[14];
				Voltage_Value_array[15][i] = Voltage_Value[15];

	    	}
//	    	求100次电压和
	    	for(i = 0;i < COLLECT_TIMES;i++)
	    	{
	    		VoltageSum[ 0] += Voltage_Value_array[ 0][i];
	    		VoltageSum[ 1] += Voltage_Value_array[ 1][i];
	    		VoltageSum[ 2] += Voltage_Value_array[ 2][i];
	    		VoltageSum[ 3] += Voltage_Value_array[ 3][i];
	    		VoltageSum[ 4] += Voltage_Value_array[ 4][i];
	    		VoltageSum[ 5] += Voltage_Value_array[ 5][i];
	    		VoltageSum[ 6] += Voltage_Value_array[ 6][i];
	    		VoltageSum[ 7] += Voltage_Value_array[ 7][i];
	    		VoltageSum[ 8] += Voltage_Value_array[ 8][i];
				VoltageSum[ 9] += Voltage_Value_array[ 9][i];
				VoltageSum[10] += Voltage_Value_array[10][i];
				VoltageSum[11] += Voltage_Value_array[11][i];
				VoltageSum[12] += Voltage_Value_array[12][i];
				VoltageSum[13] += Voltage_Value_array[13][i];
				VoltageSum[14] += Voltage_Value_array[14][i];
				VoltageSum[15] += Voltage_Value_array[15][i];
	    	}
//	    	求平均值
	    	for(i = 0;i < 16;i++)
	    	{
	    		VoltageAverage[i] = VoltageSum[i]/COLLECT_TIMES;
	    	}

//	    	求最大值、最小值
	    	for(i = 0;i < 16;i++)
	    	{
	    		VoltageMax[i] = Voltage_Value_array[i][0];
			    VoltageMin[i] = Voltage_Value_array[i][0];

	    		for(int j = 0;j < COLLECT_TIMES;j++)
	    		{
	    			VoltageMax[i] = _MAX(VoltageMax[i], Voltage_Value_array[i][j]);			//取最大值
	    			VoltageMin[i] = _MIN(VoltageMin[i], Voltage_Value_array[i][j]);			//取最小值
	    		}
	    	}

	    	for(i = 0;i < 16;i++)
	    	{
	    		printf("Max[%2d]：  %.4f  Min[%2d]：  %.4f  Average[%2d]：  %.4f \n"\
	    		       ,i,VoltageMax[i],i,VoltageMin[i],i,VoltageAverage[i]);

	    	}

		}

		if(c == 'q')
		{
			break;
		}


	}
}

