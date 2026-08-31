/*
 * usrpwm.c
 *
 *  Created on: 2023年7月27日
 *      Author: liyuan
 */
#include "usrpwm.h"
#include "xil_io.h"
#include "xgpio.h"
#include <stdio.h>



#define GPIO_PWM_DEVID		 XPAR_PWM_GPIO_PWM_BUFFER_EN_DEVICE_ID

XGpio GpioPWM;

static u32 ChannelFrequency[8] = {0};


/*
	功能：配置通道周期、占空比
	入口：
	ch：通道号：0-7
	frequency：pwm频率
	Duty：pwm占空比
	例如：通道1，1khz频率，50%占空比
		PwmConfig(1, 1000, 0.5)
*/
int PWMConfig(u8 ch, u32 frequency)
{
	int Status;
	u32 period_cfg;
	static int InitStatus = 1;
	ChannelFrequency[ch] = frequency;
	period_cfg = (1000000000 / frequency /10);


	//初始化PWM输出buffer使能信号
	if(InitStatus == 1)
	{
		Status = XGpio_Initialize(&GpioPWM, GPIO_PWM_DEVID);
		if (Status != XST_SUCCESS) {
			printf("Gpio Initialization Failed\r\n");
			return XST_FAILURE;
		}
	//	把通道设为输出模式
		XGpio_SetDataDirection(&GpioPWM, 1, 0);
		InitStatus = 0;
	}

	Xil_Out32(PWM_BASEADDR + (ch * 8) + 0x08, period_cfg);			//配置周期

	return 0;

}


/*
	功能：指定通道输出pwm
	入口：
	ch：通道号：0-7
	state: 状态配置，对应通道1：输出，0：停止
*/
void PWMControl(u8 ch, float Duty, u8 state)
{
	int value = 0;
	u32 duty_cfg;

	duty_cfg =(Duty * (1000000000 / ChannelFrequency[ch]) / 10);

	XGpio_DiscreteWrite(&GpioPWM, 1, 0);							//配置通道状态，使能pwm输出

	Xil_Out32(PWM_BASEADDR + (ch * 8 + 0x4) + 0x08, duty_cfg);		//配置占空比

	if(state == 0)
	{
		value = Xil_In32(PWM_BASEADDR + 0);								//读取当前状态
		Xil_Out32(PWM_BASEADDR + 0, ~(0x1 << ch) & value);					//逻辑生成pwm波形

		value = Xil_In32(PWM_BASEADDR + 0x4);						//读取当前pwm输出状态
		Xil_Out32(PWM_BASEADDR + 0x4, ~(0x1 << ch) & value);		//配置pwm输出
	}
	else if(state == 1)					//先关后开
	{
		value = Xil_In32(PWM_BASEADDR + 0);								//读取当前状态
		Xil_Out32(PWM_BASEADDR + 0, ~(0x1 << ch) & value);					//逻辑生成pwm波形

		value = Xil_In32(PWM_BASEADDR + 0x4);						//读取当前pwm输出状态
		Xil_Out32(PWM_BASEADDR + 0x4, ~(0x1 << ch) & value);		//配置pwm输出



		value = Xil_In32(PWM_BASEADDR + 0);								//读取当前状态
		Xil_Out32(PWM_BASEADDR + 0, (0x1 << ch) | value);					//逻辑生成pwm波形

		value = Xil_In32(PWM_BASEADDR + 0x4);					//读取当前pwm输出状态
		Xil_Out32(PWM_BASEADDR + 0x4, (0x1 << ch) | value);		//配置pwm输出
	}


}


//所有pwm通道停止输出
void PWMAllStop()
{
	//	配置通道状态，停止输出
		XGpio_DiscreteWrite(&GpioPWM, 1, 1);
}






