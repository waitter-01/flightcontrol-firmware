/*
 * sxcontrol.c
 *
 *  Created on: 2023年7月27日
 *      Author: liyuan
 */
#include "usrpdsxcontrol.h"
#include "xstatus.h"
#include "xgpio.h"


XGpio GPIO_0_InstPtr;
XGpio GPIO_1_InstPtr;
XGpio GPIO_2_InstPtr;


//时序gpio初始化
void PD_SX_Init(void)
{
	int Status;

	/* Initialize the AXI_GPIO_0_GPI_0_InstPtr Driver */
	Status = XGpio_Initialize(&GPIO_0_InstPtr, PMOS_PD_DEV);
	if (Status != XST_SUCCESS) {
		xil_printf("AXI_GPIO_0 Initialization Failed\r\n");
	}

	/* Initialize the AXI_GPIO_1_GPO_1_InstPtr driver */
	Status = XGpio_Initialize(&GPIO_1_InstPtr, PMOS_DEV);
	if (Status != XST_SUCCESS) {
		xil_printf("AXI_GPIO_1 Initialization Failed\r\n");
	}

	/* Initialize the AXI_GPIO_2_GPO_2_InstPtr driver */
	Status = XGpio_Initialize(&GPIO_2_InstPtr, RELAY_DEV);
	if (Status != XST_SUCCESS) {
		xil_printf("AXI_GPIO_2 Initialization Failed\r\n");
	}


}



//获取配电通道（B1-B7）状态
int GetPD_Status(void)
{
	int state = 0;

	//	把通道设为输入模式
	XGpio_SetDataDirection(&GPIO_0_InstPtr, 2, 1);

	state = XGpio_DiscreteRead(&GPIO_0_InstPtr, 2);

	state |= ((XGpio_DiscreteRead(&GPIO_1_InstPtr, 2) & 0x1) << 6);

//	state &= 0x1f;
	return state;
}

//设置配电通道 0-6使能
void SetPD_Enable(DWORD ch,DWORD value)
{
	int state = 0;

	if(ch == 6)
	{
		state = XGpio_DiscreteRead(&GPIO_1_InstPtr, 1);
		if(value == 1)
		{
			state |= 0x1;						//制作控制参数
		}
		else
		{
			state &= ~(0x1);						//制作控制参数
		}
		XGpio_DiscreteWrite(&GPIO_1_InstPtr, 1, state);

	}
	else
	{
		state = XGpio_DiscreteRead(&GPIO_0_InstPtr, 1);			//读取当前状态

		if(value == 1)
		{
			state |= ((0x1 << (ch*2) & 0xFFF));						//制作控制参数
		}
		else
		{
			state &= ~(0x1 << (ch*2));						//制作控制参数
		}
		XGpio_DiscreteWrite(&GPIO_0_InstPtr, 1, state);
	}

}

//设置配电通道 0-6控制
void SetPD_Status(DWORD ch, DWORD value)
{
	int state = 0;

	if(ch == 6)
	{
		state = XGpio_DiscreteRead(&GPIO_1_InstPtr, 1);
		if(value == 1)
		{
			state |= 0x2;						//制作控制参数
		}
		else
		{
			state &= ~(0x2);						//制作控制参数
		}
		XGpio_DiscreteWrite(&GPIO_1_InstPtr, 1, state);
	}
	else
	{
		state = XGpio_DiscreteRead(&GPIO_0_InstPtr, 1);			//读取当前状态

		if(value == 1)
		{
			state |= ((0x1 << (ch*2+1) & 0xFFF));						//制作控制参数
		}
		else
		{
			state &= ~(0x1 << (ch*2+1));						//制作控制参数
		}
		XGpio_DiscreteWrite(&GPIO_0_InstPtr, 1, state);

	}

}


//获取时序通道状态 0-14
int GetSX_Status(void)
{
	int state = 0;
	state = XGpio_DiscreteRead(&GPIO_1_InstPtr, 2);
	state = state >> 1;

	state |= (XGpio_DiscreteRead(&GPIO_2_InstPtr, 2) << 11);

	return state;
}


//设置时序使能通道：0-14
void SetSX_Enable(DWORD ch, DWORD value)
{
	int state = 0;

	if(ch < 11)
	{
		if(value == 0)
		{
			state = XGpio_DiscreteRead(&GPIO_1_InstPtr, 1);			//读取当前状态
			state &= ~(0x1 << (ch*2+2));				//制作控制参数
			XGpio_DiscreteWrite(&GPIO_1_InstPtr, 1, state);
		}
		else
		{
			state = XGpio_DiscreteRead(&GPIO_1_InstPtr, 1);			//读取当前状态
			state |= ((0x1 << (ch*2+2)) & 0xFFFFFF);				//制作控制参数

			XGpio_DiscreteWrite(&GPIO_1_InstPtr, 1, state);
		}

	}
	else			//如果超过11，更换axigpio IP核
	{
		if(value == 0)
		{
			state = XGpio_DiscreteRead(&GPIO_2_InstPtr, 1);			//读取当前状态
			state &= ~(0x1 << ((ch-11)*2));							//制作控制参数

			XGpio_DiscreteWrite(&GPIO_2_InstPtr, 1, state);
		}
		else
		{
			state = XGpio_DiscreteRead(&GPIO_2_InstPtr, 1);			//读取当前状态
			state |= ((0x1 << ((ch-11)*2)) & 0xFF);					//制作控制参数

			XGpio_DiscreteWrite(&GPIO_2_InstPtr, 1, state);
		}

	}

}



void SetSX_Status(DWORD ch, DWORD value)
{
	int state = 0;

	if(ch < 11)
	{
		if(value == 0)
		{
			state = XGpio_DiscreteRead(&GPIO_1_InstPtr, 1);			//读取当前状态
			state &= ~(0x1 << (ch*2+3));							//制作控制参数
			XGpio_DiscreteWrite(&GPIO_1_InstPtr, 1, state);
		}
		else
		{
			state = XGpio_DiscreteRead(&GPIO_1_InstPtr, 1);			//读取当前状态
			state |= ((0x1 << (ch*2+3)) & 0xFFFFFF);				//制作控制参数

			XGpio_DiscreteWrite(&GPIO_1_InstPtr, 1, state);
		}


//		state = XGpio_DiscreteRead(&GPIO_1_InstPtr, 1);				//读取当前状态
//		state |= ((value << (ch*2+1)) & 0xFFFFFF);					//制作控制参数
//
//		XGpio_DiscreteWrite(&GPIO_1_InstPtr, 1, state);

	}
	else
	{
		if(value == 0)
		{
			state = XGpio_DiscreteRead(&GPIO_2_InstPtr, 1);			//读取当前状态
			state &= ~(0x1 << ((ch-11)*2+1));						//制作控制参数

			XGpio_DiscreteWrite(&GPIO_2_InstPtr, 1, state);
		}
		else
		{
			state = XGpio_DiscreteRead(&GPIO_2_InstPtr, 1);			//读取当前状态
			state |= ((0x1 << ((ch-11)*2+1)) & 0xFF);				//制作控制参数

			XGpio_DiscreteWrite(&GPIO_2_InstPtr, 1, state);
		}

//		state = XGpio_DiscreteRead(&GPIO_2_InstPtr, 1);				//读取当前状态
//		state |= ((value << ((ch-10)*2+1)) & 0xFF);					//制作控制参数
//
//		XGpio_DiscreteWrite(&GPIO_2_InstPtr, 1, state);
	}



}









