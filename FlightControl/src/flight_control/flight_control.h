/*
 * flight_control.h
 *
 *  Created on: 2024年5月8日
 *      Author: wanghaobo
 */

#ifndef SRC_FLIGHT_CONTROL_FLIGHT_CONTROL_H_
#define SRC_FLIGHT_CONTROL_FLIGHT_CONTROL_H_

#include "main.h"

#define flight_control_TASK_PRIO 1 //任务优先级
#define flight_control_STK_SIZE 4096 //任务堆栈大小
TaskHandle_t flight_controlTask_Handler; //任务句柄
void flight_control_task(void *pvParameters);//任务函数

QueueHandle_t Servo_Control_Queue;

void init_ControlSystem(void);

#endif /* SRC_FLIGHT_CONTROL_FLIGHT_CONTROL_H_ */
