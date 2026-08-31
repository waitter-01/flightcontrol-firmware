/*
 * ina237.c
 *
 *  Created on: 2023年11月21日
 *      Author: liyuan
 */
#include "voltsensor.h"
#include "xiic_l.h"
#include "xiic.h"




XIic IIC_0_InstPtr;



/****************************************************************************/
/**
 * INA237 VoltageCurrentSensor Modules Initialize, Polled mode
 *
****************************************************************************/
void VoltCurrSensorInit(void)
{
	int Status;
	u8 OpData[6];


	/* Initialize the AXI_IIC_0_TEMP_InstPtr Driver */
	Status = XIic_Initialize(&IIC_0_InstPtr, IIC_0_DEVID);
	if (Status != XST_SUCCESS) {
		xil_printf("AXI_IIC_0_TEMP Initialization Failed\r\n");
	}

	//Find INA237_IIC_3_28VMAIN present or not. that is read Manufacturer ID
	Status = Ina237_MnfId(IIC_0_MAIN_BASEADDR, IIC_0_INA237_ADDR_MAIN);
	if (Status == XST_SUCCESS) {
		OpData[0] = 0x00;
		OpData[1] = 0x00;	//ADCRANGE= ±163.84mV, CONVDLY=0s
		OpData[2] = 0xB2;
		OpData[3] = 0x48;	//CTvbus_vshunt=84us, AVG=1
		OpData[4] = 0x0C;
		OpData[5] = 0xB8;	//Cur_LSB=1221uA, Rshunt=3mΩ, CAL=3000=0x0BB8
//		OpData[4] = 0x0E;
//		OpData[5] = 0x42;	//Cur_LSB=1221uA, Rshunt=3.65mΩ, CAL=3650=0x0E42
		Ina237_CfgCalb(IIC_0_MAIN_BASEADDR, IIC_0_INA237_ADDR_MAIN, OpData);
	}
}




/****************************************************************************/
/**
 * INA237 Manufacturer ID READ
 *
****************************************************************************/
int Ina237_MnfId(UINTPTR BaseAddress, u8 Address)
{
	int Index;
	u8 BufferPtr[2];
	u8 CmdPtr = INA237_PTR_MNFID;

	for (Index = 0; Index < 2; Index++) {
		BufferPtr[Index] = 0;
	}

	XIic_Send(BaseAddress, Address, &CmdPtr, 1, XIIC_REPEATED_START);
	XIic_Recv(BaseAddress, Address, BufferPtr, 2, XIIC_STOP);
	if ((BufferPtr[0] != 0x54) || (BufferPtr[1] != 0x49)) {
		return XST_DEVICE_NOT_FOUND;
	}

	return XST_SUCCESS;
}

/****************************************************************************/
/**
 * Configurate INA237 ConfigurationRegister and CalibrationRegister
 *
****************************************************************************/
int Ina237_CfgCalb(UINTPTR BaseAddress, u8 Address, u8 *OpData)
{
	int Index;
	u8 CfgBufferPtr[3];
	u8 AdcCfgBufferPtr[3];
	u8 CalbBufferPtr[3];

	CfgBufferPtr[0] = INA237_PTR_CONFR;
	for (Index = 0; Index < 2; Index++) {
		CfgBufferPtr[Index + 1] = *(OpData + Index);
	}

	AdcCfgBufferPtr[0] = INA237_PTR_ADC_CFGR;
	for (Index = 0; Index < 2; Index++) {
		AdcCfgBufferPtr[Index + 1] = *(OpData + 2 + Index);
	}

	CalbBufferPtr[0] = INA237_PTR_CALIR;
	for (Index = 0; Index < 2; Index++) {
		CalbBufferPtr[Index + 1] = *(OpData + 4 + Index);
	}

	XIic_Send(BaseAddress, Address, CfgBufferPtr, 3, XIIC_STOP);
	XIic_Send(BaseAddress, Address, AdcCfgBufferPtr, 3, XIIC_STOP);
	XIic_Send(BaseAddress, Address, CalbBufferPtr, 3, XIIC_STOP);

	return XST_SUCCESS;
}

/****************************************************************************/
/**
 * Read INA237 Shunt Voltage Register
 *
****************************************************************************/
u8 * Ina237_RdShuntVol(UINTPTR BaseAddress, u8 Address)
{
	int Index;
	u8 BufferPtr[2];
	u8 * RdShuntVolPtr;
	u8 CmdPtr = INA237_PTR_SHUVR;

	for (Index = 0; Index < 2; Index++) {
		BufferPtr[Index] = 0;
	}

	XIic_Send(BaseAddress, Address, &CmdPtr, 1, XIIC_REPEATED_START);
	XIic_Recv(BaseAddress, Address, BufferPtr, 2, XIIC_STOP);

	RdShuntVolPtr = BufferPtr;
	return RdShuntVolPtr;
}

/****************************************************************************/
/**
 * Read INA237 Bus Voltage Register
 *
****************************************************************************/
u8 * Ina237_RdBusVol(UINTPTR BaseAddress, u8 Address)
{
	int Index;
	u8 BufferPtr[2];
	u8 * RdBusVolPtr;
	u8 CmdPtr = INA237_PTR_BUSVR;

	for (Index = 0; Index < 2; Index++) {
		BufferPtr[Index] = 0;
	}

	XIic_Send(BaseAddress, Address, &CmdPtr, 1, XIIC_REPEATED_START);
	XIic_Recv(BaseAddress, Address, BufferPtr, 2, XIIC_STOP);

	RdBusVolPtr = BufferPtr;
	return RdBusVolPtr;
}

/****************************************************************************/
/**
 * Read INA237 Current Register
 *
****************************************************************************/
u8 * Ina237_RdCurrent(UINTPTR BaseAddress, u8 Address)
{
	int Index;
	u8 BufferPtr[2];
	u8 * RdCurrentPtr;
	u8 CmdPtr = INA237_PTR_CURR;

	for (Index = 0; Index < 2; Index++) {
		BufferPtr[Index] = 0;
	}

	XIic_Send(BaseAddress, Address, &CmdPtr, 1, XIIC_REPEATED_START);
	XIic_Recv(BaseAddress, Address, BufferPtr, 2, XIIC_STOP);

	RdCurrentPtr = BufferPtr;
	return RdCurrentPtr;
}


/****************************************************************************/
/**
 *读取1次 相应配电通道 工作电压和电流
 *
****************************************************************************/
u8 DigitalVolCur(u8 *pbyData)
{
	u8 * RdVolCur_Ptr;
//	u8 VolCurData_Buffer[4];

	RdVolCur_Ptr = Ina237_RdBusVol(IIC_0_MAIN_BASEADDR, IIC_0_INA237_ADDR_MAIN);
	pbyData[0] = *(RdVolCur_Ptr + 1);
	pbyData[1] = *RdVolCur_Ptr;

	RdVolCur_Ptr = Ina237_RdCurrent(IIC_0_MAIN_BASEADDR, IIC_0_INA237_ADDR_MAIN);
	pbyData[2] = *(RdVolCur_Ptr + 1);
	pbyData[3] = *RdVolCur_Ptr;

	return 0;
}



int AnalogVolCur(float *pbyData)
{
	u8 data[4] = {0};
//	float AnalogData[2] = {0};
	int vol,cur;

	DigitalVolCur(data);

	vol = (data[1] << 8) | data[0];
	cur = (data[3] << 8) | data[2];

	pbyData[0] = vol * 3.125 * 0.001;
	pbyData[1] = cur * 1221 *0.000001;

	return 0;
}





