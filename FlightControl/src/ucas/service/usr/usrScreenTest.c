/*
 * usrScreenTest.c
 *
 *  Created on: 2023年12月26日
 *      Author: liyuan
 */

#include <stdio.h>
#include "usrtempvoltest.h"
#include "xil_printf.h"
#include "usruarttest.h"
#include "usrglobal.h"
#include "usrcantest.h"
#include "usrled.h"
#include "usradctest.h"
#include "sleep.h"
#include "usrpdsxtest.h"
#include "usrgpiotest.h"
#include "usrpwmtest.h"
#include "usrtempvoltest.h"
#include "usrtemp.h"
#include "usrScreenTest.h"
#include "interface.h"
#include "usrpwm.h"
#include "usrpdsxcontrol.h"



#define   CHANNEL_ON 	1
#define   CHANNEL_OFF 	0

void ScreenTest()
{
	printf("------------- Test Start -------------\n");

	printf("---TEMP Collect. Voltage Current. Serial. ADC. GPIO. PWM. PD SX. CAN Test.---\n");

	TempTest();

	VolCurTest();

	printf("------------- Serial Test -------------\n");
	UartMultiTest();

	ADCScreenTest();

	PWM_ScreenTest();

	GPIOScreenTest();

	PD_SX_ScreenTest();

	CanScreenTest();

}










void ADCScreenTest()
{
	char c;
    int i;
    u16 ADC_Value[16] = {0};           			 //ADC采集原始值
    float Voltage_Value[16] = {0};		         //ADC采集电压值
    float Voltage_Value_array[16][100] = {0};     //每路采集100个数数组
    float VoltageAverage[16] = {0};               //每路电压平均值

    float VoltageMin[16] = {0};                   //100次电压最小值
	float VoltageMax[16] = {0};   				 //100次电压最大值


//	while(1)
	{
	    float VoltageSum[16] = {0};                   //100次电压和
		printf("----------- ADC Test -------------\n");

		c = '2';
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

	}
}





void PWM_ScreenTest()
{
	char c;



	printf("------------- PWM Test -------------\r\n");
	c = '2';
	if(c == '2')
	{
		printf("All Channel Output!\r\n");
		for(int i = 0;i < 8;i++)
		{
			PWMConfig(i, 1);
			PWMControl(i, 0.5, 1);
		}
		c = '3';
	}
	sleep(1);
	if(c == '3')
	{
		printf("All Channel Stop!\n");
		PWMAllStop();
	}

}




int PD_SX_ScreenTest()
{
	char c;
//	DWORD PDState = 0, SXState = 0;
	DWORD PDStatus = 0, SXStatus = 0;
//	DWORD PDChannel = 0, SXChannel = 0;


	printf("------------- Pei Dian Shi Xu  Test -------------\n");
	PD_SX_Init();

	c = '2';
	if(c == '2')
	{
		printf("All Pei Dian Channel ON!\n");
		for(int i = 0;i < 7;i++)
		{
			SetPD_Enable(i, CHANNEL_ON);
			SetPD_Status(i, CHANNEL_ON);
		}
		usleep(10000);
		PDStatus = GetPD_Status();
		printf("PD current status is: %x\r\n", PDStatus);
		c = '5';
	}

	if(c == '5')
	{
		printf("All Shi Xu Channel ON!\n");
		for(int i = 0;i < 15;i++)
		{
			SetSX_Enable(i, CHANNEL_ON);
			SetSX_Status(i, CHANNEL_ON);
		}
		usleep(10000);
		SXStatus = GetSX_Status();
		printf("SX current status is: %x\r\n", SXStatus);
		c = '6';
	}

	if(c == '6')
	{
		printf("All Shi Xu Channel OFF!\n");
		for(int i = 0;i < 15;i++)
		{
			SetSX_Enable(i, CHANNEL_ON);
			SetSX_Status(i, CHANNEL_OFF);
		}
		usleep(10000);
		SXStatus = GetSX_Status();
		printf("SX current status is: %x\r\n", SXStatus);
		c = '3';
	}

	if(c == '3')
	{
		printf("All Pei Dian Channel OFF!\n");
		for(int i = 0;i < 7;i++)
		{
			SetPD_Status(i, CHANNEL_OFF);
		}
		usleep(10000);
		PDStatus = GetPD_Status();
		printf("PD current status is: %x\r\n", PDStatus);
//			c = '5';
	}

	return 0;
}




void GPIOScreenTest()
{
	char c;
	int state = 0;

//	while(1)
	{
		printf("----------- GPIO Test -------------\n");

		c = '1';
		if(c == '1')
		{
			int value = 0;
			GpioChannel_Init();

			state = Get_IOState(GPIO_CHANNEL2);				//获取初始状态
			printf("io_state = %x\n",state);				//打印初始状态

			while(1)
			{
				state = Get_IOState(GPIO_CHANNEL2);

				if((state != value) && (state != 0x3f))					//换按键，自动打印当前状态
				{
					printf("IO State = %x\n",state);
					value = state;
				}
				if(state == 0x3c)					//如果同时按无源前两个(gpi0、gpi1)，退出循环
				{
					break;
				}
//				printf("io_state = %x\n",state);
			}

			c = '2';
		}
		if(c == '2')
		{
//			GpioChannel_Init();
			printf("All leds ON\n");
			Set_IOState(GPIO_CHANNEL1, 0x7);		//灯全亮
			sleep(2);
			printf("All leds OFF\n");
			Set_IOState(GPIO_CHANNEL1, 0);			//灯全灭

		}

	}

}







u32 SendData1[XCANPS_MAX_FRAME_SIZE_IN_WORDS] = {0xEB, 0x90, 0xEB900011, 0x121355AA, 0x03, 0x04, 0x05,0x06};
u32 RecvData[XCANPS_MAX_FRAME_SIZE_IN_WORDS];

//BYTE ConvertData[8] = {0};

//DWORD stCanId[2] = {0x7DA, 0x1ff1f7DB};			//CAN ID 配置

int CanScreenTest()
{
	printf("------------- CAN Test -------------\n");


//		if(c == '2')
	{
		CanBaudConfig(0, 0);		//设备0波特率配置
		CanBaudConfig(1, 0);		//设备1波特率配置

		for(int j = 0;j < 5;j++)
		{
//				SendData1[2] = 0xEB900111;
			CanSendFrame(1, SendData1);
			usleep(1000);
			CanRecvFrame(0, RecvData);

			for(int i = 2;i < 4;i++)
			{
				xil_printf("CAN Dev[0] recv[%d] = %X", i-2, RecvData[i]);
				xil_printf("  ");
			}
			printf("\n");
			SendData1[2] += 0x0100;
		}

		SendData1[2] = 0xEB900011;
		for(int j = 0;j < 5;j++)
		{
			CanSendFrame(0, SendData1);
			usleep(1000);
			CanRecvFrame(1, RecvData);
			for(int i = 2;i < 4;i++)
			{
				xil_printf("CAN Dev[1] recv[%d] = %X", i-2, RecvData[i]);
				xil_printf("  ");
			}
			printf("\n");
			SendData1[2] += 0x0100;
		}
	}

	return 0;
}






