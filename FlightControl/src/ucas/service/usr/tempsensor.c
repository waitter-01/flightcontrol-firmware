/*
 * tmp75.c
 *
 *  Created on: 2022年6月27日
 *      Author: liyuan
 */
#include <tempsensor.h>
#include "xiic.h"
#include "xiic_l.h"



//AXI_IIC_1_TEMP 配电板
#define IIC_1_TEMP_BASEADDR			XPAR_IIC_1_BASEADDR
#define IIC_1_DEVID					XPAR_AXI_IIC_AXI_IIC_PD_TEMP_DEVICE_ID
#define IIC_1_TMP75_TOPADDR			0x48
#define IIC_1_TMP75_BOTADDR			0x49

//AXI_IIC_2_TEMP 飞控板
#define IIC_2_TEMP_BASEADDR			XPAR_IIC_2_BASEADDR
#define IIC_2_DEVID					XPAR_AXI_IIC_AXI_IIC_TEMP_DEVICE_ID
#define IIC_2_TMP75_TOPADDR			0x48
#define IIC_2_TMP75_BOTADDR			0x49

//TMP75 Register Address
#define TMP75_REGISTER_TEMP			0x00
#define TMP75_REGISTER_CONF			0x01
#define TMP75_REGISTER_TLOW			0x02
#define TMP75_REGISTER_THIGH		0x03

XIic IIC_1_InstPtr;
XIic IIC_2_InstPtr;


/****************************************************************************/
/**
 * TMP75 Tempsensor Module Initialize, Polled mode
 *
****************************************************************************/
void Tmp75_Init(void)
{
	int Status;

	/* Initialize the AXI_IIC_1_TEMP_InstPtr Driver */
	Status = XIic_Initialize(&IIC_1_InstPtr, IIC_1_DEVID);
	if (Status != XST_SUCCESS) {
		xil_printf("AXI_IIC_0_TEMP Initialization Failed\r\n");
	}

	/* Initialize the AXI_IIC_2_TEMP_InstPtr Driver */
	Status = XIic_Initialize(&IIC_2_InstPtr, IIC_2_DEVID);
	if (Status != XST_SUCCESS) {
		xil_printf("AXI_IIC_0_TEMP Initialization Failed\r\n");
	}

	//continuous conversion mode, 12bit resolutions
	u8 Tmp75_InitReg[2] = {TMP75_REGISTER_CONF, 0x78};

	XIic_Send(IIC_1_TEMP_BASEADDR, IIC_1_TMP75_TOPADDR,
				Tmp75_InitReg, 2, XIIC_STOP);
//	XIic_Send(IIC_1_TEMP_BASEADDR, IIC_1_TMP75_BOTADDR,
//				Tmp75_InitReg, 2, XIIC_STOP);

	XIic_Send(IIC_2_TEMP_BASEADDR, IIC_2_TMP75_TOPADDR,
				Tmp75_InitReg, 2, XIIC_STOP);
	XIic_Send(IIC_2_TEMP_BASEADDR, IIC_2_TMP75_BOTADDR,
				Tmp75_InitReg, 2, XIIC_STOP);
}

/****************************************************************************/
/**
 * Read TMP75 Tempsensor Module, Polled mode
 *
****************************************************************************/
u8 * Tmp75_RdTempsensor(UINTPTR BaseAddress, u8 Address)
{
	int Index;
	static u8 BufferPtr[2];
	u8 Tmp75_PointerRegister = TMP75_REGISTER_TEMP;

	for (Index = 0; Index < 2; Index++) {
		BufferPtr[Index] = 0;
	}

	XIic_Send(BaseAddress, Address, &Tmp75_PointerRegister, 1, XIIC_REPEATED_START);
	XIic_Recv(BaseAddress, Address, BufferPtr, 2, XIIC_STOP);

	return BufferPtr;
}

/****************************************************************************/
/**
 * Read All Temperature Sensors
 *
****************************************************************************/
u8 * RdPDTempsensor(void)
{
	u8 * Tmp75_TempPtr;
	static u8 TempBuffer[4] = {0};

	//Read Temperature 1, that is Top Sensor
	Tmp75_TempPtr = Tmp75_RdTempsensor(IIC_1_TEMP_BASEADDR, IIC_1_TMP75_TOPADDR);
	TempBuffer[0] = (*(Tmp75_TempPtr + 1) >> 4);				//去除低四位无效位
	TempBuffer[1] = *Tmp75_TempPtr;

//	Read Temperature 2, that is Bottom Sensor
	Tmp75_TempPtr = Tmp75_RdTempsensor(IIC_1_TEMP_BASEADDR, IIC_1_TMP75_BOTADDR);
	TempBuffer[2] = (*(Tmp75_TempPtr + 1) >> 4);
	TempBuffer[3] = *Tmp75_TempPtr;

	return TempBuffer;
}


u8 * RdFKTempsensor(void)
{
	u8 * Tmp75_TempPtr;
	static u8 TempBuffer[4] = {0};

	//Read Temperature 1, that is Top Sensor
	Tmp75_TempPtr = Tmp75_RdTempsensor(IIC_2_TEMP_BASEADDR, IIC_2_TMP75_TOPADDR);
	TempBuffer[0] = (*(Tmp75_TempPtr + 1) >> 4);				//去除低四位无效位
	TempBuffer[1] = *Tmp75_TempPtr;

	//Read Temperature 2, that is Bottom Sensor
	Tmp75_TempPtr = Tmp75_RdTempsensor(IIC_2_TEMP_BASEADDR, IIC_2_TMP75_BOTADDR);
	TempBuffer[2] = (*(Tmp75_TempPtr + 1) >> 4);
	TempBuffer[3] = *Tmp75_TempPtr;

	return TempBuffer;
}


//采集温度转为模拟量
float *GetPDTemp()
{
	static float temp[2] = {0};
	int T[2] = {0};
	u8 *value;
	value = RdPDTempsensor();						//读取两个传感器温度数字量

	T[0] = (*(value+1) << 4) + *value;				//U11数字量   value还有4位有效位
	T[1] = (*(value+3) << 4) + *(value+2);			//U12数字量

	//数模转化
	if((T[0]) >= 0x800)
	{
		temp[0] = ((~T[0] + 1) & 0xFFF) * -0.0625;
	}
	else
	{
		temp[0] = T[0] * 0.0625;
	}

	if((T[1]) >= 0x800)
	{
		temp[1] = ((~T[1] + 1) & 0xFFF) * -0.0625;
	}
	else
	{
		temp[1] = T[1] * 0.0625;
	}

	return temp;
}

//采集温度转为模拟量
float *GetFKTemp()
{
	static float temp[2] = {0};
	int T[2] = {0};
	u8 *value;
	value = RdFKTempsensor();						//读取两个传感器温度数字量

	T[0] = (*(value+1) << 4) + *value;				//U11数字量   U11数字量   value还有4位有效位
	T[1] = (*(value+3) << 4) + *(value+2);			//U12数字量

	//数模转化
	if((T[0]) >= 0x800)
	{
		temp[0] = ((~T[0] + 1) & 0xFFF) * -0.0625;
	}
	else
	{
		temp[0] = T[0] * 0.0625;
	}

	if((T[1]) >= 0x800)
	{
		temp[1] = ((~T[1] + 1) & 0xFFF) * -0.0625;
	}
	else
	{
		temp[1] = T[1] * 0.0625;
	}

	return temp;
}

