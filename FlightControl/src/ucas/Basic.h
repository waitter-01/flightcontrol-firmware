/*
 * Basic.h
 *
 *  Created on: 2024年6月17日
 *      Author: wanghaobo
 */

#ifndef SRC_UCAS_BASIC_H_
#define SRC_UCAS_BASIC_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdbool.h>
#include "xil_types.h"

typedef enum
{
	HEAD_H = 0,
	HEAD_L,
	LEN,
	ID,
	DATA,
	CHECK
}uart_State;

// 4bytes、32bit数据大小端转化
#define L2B32(Little) (((Little & 0xff) << 24) | (((Little) & 0xff00) << 8) | (((Little) & 0xff0000) >> 8) | ((Little >> 24) & 0xff))
// 2bytes、16bit数据大小端转化
#define L2B16(Little) (((Little & 0xff) << 8) | ((Little >> 8) & 0xff))

#define SensorsUart   0//1
#define CommulinkUart 1//2
#define ServoUart 	  2//3
//#define RcUart 	      3

extern SemaphoreHandle_t UartPsRxMessagesMutex;


void setInitializationCompleted();
bool getInitializationCompleted();
void LockInitializationStatus();
void UnLockInitializationStatus();

int UartPl_SendBuff(int ch, const unsigned char *TxBuff, unsigned short Length);
int UartPl_RecvBuff(int ch, unsigned char * RxBuff);
int UartPl_Get_Recvlen(int ch);
u8 UartPl_Get_Recvflag(int ch);
int UART_PL_Setting(int ch, unsigned long Baud, char Par);

int UartPS0Init(void);
void uartpl_init(void);
bool Target_is_time(u32 old_time,u32 Target_time);
uint8_t xor_checksum(const u8 *data, u32 length);
#endif /* SRC_UCAS_BASIC_H_ */
