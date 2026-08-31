/*
 * ina237.h
 *
 *  Created on: 2023Äê11ÔÂ21ÈÕ
 *      Author: liyuan
 */

#ifndef SRC_UCAS_SERVICE_USR_VOLTSENSOR_H_
#define SRC_UCAS_SERVICE_USR_VOLTSENSOR_H_

#include "xil_types.h"

//AXI_IIC_2_7CHPWR
#define IIC_0_MAIN_BASEADDR			XPAR_AXI_IIC_AXI_IIC_MAIN_CURRENT_BASEADDR
#define IIC_0_DEVID					XPAR_AXI_IIC_AXI_IIC_MAIN_CURRENT_DEVICE_ID
#define IIC_0_INA237_ADDR_MAIN		0x40
//#define IIC_0_INA237_ADDR_B2		0x42
//#define IIC_0_INA237_ADDR_B3		0x43
//#define IIC_0_INA237_ADDR_B4		0x44
//#define IIC_0_INA237_ADDR_B5		0x45
//#define IIC_0_INA237_ADDR_B6		0x46
//#define IIC_0_INA237_ADDR_B7		0x47


//INA237 Register Address
#define INA237_PTR_CONFR			0x00	//Configuration Register
#define INA237_PTR_ADC_CFGR			0x01	//ADC Configuration Register
#define INA237_PTR_CALIR			0x02	//Shunt Calibration Register
#define INA237_PTR_SHUVR			0x04	//Shunt Voltage Register
#define INA237_PTR_BUSVR			0x05	//Bus Voltage Register
#define INA237_PTR_TEMP				0x06	//Temperature Register
#define INA237_PTR_CURR				0x07	//Current Register
#define INA237_PTR_POWR				0x08	//Power Register
#define INA237_PTR_MNFID			0x3E	//Manufacturer ID Register

void VoltCurrSensorInit(void);
int Ina237_MnfId(UINTPTR BaseAddress, u8 Address);
int Ina237_CfgCalb(UINTPTR BaseAddress, u8 Address, u8 *OpData);
u8 * Ina237_RdShuntVol(UINTPTR BaseAddress, u8 Address);
u8 * Ina237_RdBusVol(UINTPTR BaseAddress, u8 Address);
u8 * Ina237_RdCurrent(UINTPTR BaseAddress, u8 Address);
//u8 * ReadVolCur(void);
u8 DigitalVolCur(u8 *pbyData);
int AnalogVolCur(float *pbyData);

#endif /* SRC_UCAS_SERVICE_USR_VOLTSENSOR_H_ */
