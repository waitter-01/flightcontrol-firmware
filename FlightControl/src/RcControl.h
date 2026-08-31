/*
 * RcControl.h
 *
 *  Created on: 2024年7月25日
 *      Author: wanghaobo
 */

#ifndef SRC_RCCONTROL_H_
#define SRC_RCCONTROL_H_

#include "main.h"
#include <stdbool.h>

//接收机定义
struct Receiver
{
	bool connected;	//是否已连接
	bool available;	//是否可用
	uint8_t available_channels;	//可用的通道数目
	uint32_t last_update_time;	//上次更新时间
	uint32_t update_time;	//更新时间间隔

	float raw_data[16];	//原始数据
	float data[8];	//校准后的数据
};

#define RcControl_TASK_PRIO SysPriority_SafeTask //任务优先级
#define RcControl_STK_SIZE 4096 //任务堆栈大小
TaskHandle_t RcControlTask_Handler; //任务句柄
void ReceiverUpdate(void *pvParameters);//任务函数



void RcControl_init(void *PsUart);
void XUartPs0_Send(u8 *data,u32 len);
void Elrs_RC_In(uint8_t rdata);
#endif /* SRC_RCCONTROL_H_ */

