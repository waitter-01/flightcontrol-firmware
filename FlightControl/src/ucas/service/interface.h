/*
 * interface.h
 *
 *  Created on: 2020年9月24日
 *      Author: liyuan
 */


#include "xscugic.h"
#include "xuartps.h"

#ifndef SRC_UCAS_SERVICE_INTERFACE_H_
#define SRC_UCAS_SERVICE_INTERFACE_H_

#define GPIO_CHANNEL1   1
#define GPIO_CHANNEL2   2


#define AD7606_REG0_OFFSET		0
#define AD7606_REG1_OFFSET		4
#define AD7606_REG2_OFFSET		8
#define AD7606_REG3_OFFSET		12


#define GPIO_DEVICE_ID XPAR_AXI_GPIO_NOSOURCE_28VIN_DEVICE_ID    //4

#define INTC		XScuGic


int UART_Setting(int ch, unsigned long Baud, char Par);
int UART_PutBuff(int ch, unsigned char * TxBuff, unsigned short Length);
int _UART_IsFrameReady(int ch);
int UART_GetFrame(int ch, unsigned char * RxBuff, unsigned short MaxLength);

//GNSS数据采集函数
int UART_GetGnssValue(unsigned char * RxBuff, unsigned short MaxLength);



//AD采集函数
int ADC_GetValue(unsigned short *ValueBuff);
int ADC_GetVoltage(float * ValueBuff);



int GpioChannel_Init();
int Get_IOState(int IO_ch);
int Set_IOState(int IO_ch, int state);



#endif /* SRC_UCAS_SERVICE_INTERFACE_H_ */
